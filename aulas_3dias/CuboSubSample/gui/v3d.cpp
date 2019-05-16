#include "v3d.h"

#include "misc/util.h"

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkAxesActor.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkProperty.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkTransform.h>
#include <vtkStructuredGrid.h>
#include <vtkCellData.h>
#include <vtkTransformFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractGrid.h>

#include <QVTKOpenGLWidget.h>

#include <QFileDialog>

V3D::V3D() :
    m_txAmostragem( 1 )
{
    // Monta a GUI
    this->setupUi(this);

    // Usuario abre o arquivo de dados
    QString path_arq_dados = QFileDialog::getOpenFileName(this, "Abrir aquivo de dados" );

    //se o usuario nao cancelou, monta um ator com os dados do arquivo
    vtkSmartPointer<vtkActor> gridActor = vtkSmartPointer<vtkActor>::New();
    if( ! path_arq_dados.isEmpty() ){

        // Indice da variavel a ser mostrada ( 0 == primeira )
        uint indiceVariavel = 0;

        // Obter os parametros do grid (normalmente ha uma classe Grid com esses metadados)
        int nX = 134;
        int nY = 95;
        int nZ = 94;
        double X0 = 0.0;
        double Y0 = 0.0;
        double Z0 = 0.0;
        double dX = 1.0;
        double dY = 1.0;
        double dZ = 1.0;
        double X0frame = X0 - dX/2.0;
        double Y0frame = Y0 - dY/2.0;
        double Z0frame = Z0 - dZ/2.0;
        int numCelulas = nX * nY * nZ;

        //max e min dos valores a serem exibidos
        double max = -std::numeric_limits<double>::max();
        double min = -max;

        // Carrega a tabela de valores do arquivo
        std::vector< std::vector< double > > dados = Util::loadGEOEAS( path_arq_dados );

        // Cria um array VTK para armazenar os valores amostrais
        vtkSmartPointer<vtkFloatArray> values = vtkSmartPointer<vtkFloatArray>::New();
        values->SetName("values");
        values->Allocate( numCelulas );

        // Ler os valores amostrais para dentro do array do VTK
        // Scan order de acordo com a convencao GEO-EAS:
        //      primeiro coluna-a-coluna (direcao X, de oeste para leste)
        //      depois linha-a-linha (direcao Y, de sul para norte)
        //      depois slice-a-slice (direcao Z, de baixo para cima)
        // aproveita o loop para atualizar min e max dos valores para a tabela de cores
        for( int k = 0; k < nZ; ++k){
            for( int j = 0; j < nY; ++j){
                for( int i = 0; i < nX; ++i) {
                    int indiceLinear = k * nY * nX + j * nX + i;
                    double value = dados[indiceLinear][indiceVariavel];
                    values->InsertNextValue( value );
                    //atualiza max e min
                    max = std::max( max, value );
                    min = std::min( min, value );
                }
            }
        }

        // Criar um grid VTK (corner-point, explicit geometry)
        // Mas como a GSLib grids sao cell-centered, entao eh necessario adicionar
        // um ponto extra em todas as direcoes
        vtkSmartPointer<vtkStructuredGrid> structuredGrid = vtkSmartPointer<vtkStructuredGrid>::New();
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        for( int k = 0; k <= nZ; ++k)
            for(int j = 0; j <= nY; ++j)
                for(int i = 0; i <= nX; ++i)
                    points->InsertNextPoint( X0frame + i * dX,
                                             Y0frame + j * dY,
                                             Z0frame + k * dZ );
        structuredGrid->SetDimensions( nX+1, nY+1, nZ+1 );
        structuredGrid->SetPoints(points);

        // Atribuir os valores aas celulas do grid
        structuredGrid->GetCellData()->SetScalars( values );

        // Cria um reamostrador de grids para efetuar reamostragem e clipping (secoes)
        // inicialmente mostra todo o cubo
        m_reamostradorGrid = vtkSmartPointer<vtkExtractGrid>::New();
        m_reamostradorGrid->SetInputData( structuredGrid );
        m_reamostradorGrid->SetVOI( 0, nX, 0, nY, 0, nZ );
        m_reamostradorGrid->SetSampleRate( 1, 1, 1);
        m_reamostradorGrid->Update();

        // Tabela de cor
        vtkSmartPointer<vtkLookupTable> lut = Util::getColorTable( ColorTable::RAINBOW, min, max );

        // Cria um mapper adequado para um vtkStructuredGrid
        vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
        mapper->SetInputConnection( m_reamostradorGrid->GetOutputPort() );
        mapper->SetLookupTable(lut);
        mapper->SetScalarRange(min, max);
        mapper->Update();

        // Finalmente, cria um ator VTK para tudo
        gridActor = vtkSmartPointer<vtkActor>::New();
        gridActor->SetMapper( mapper );
    }

    // O renderer para as gerar as chamadas ao backend (OpenGL ou Mesa)
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor( gridActor);

    // Cria uma render window: a saída do backend (OpenGL neste caso) em uma janela do sistema gráfico (Qt e afins).
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    this->qvtkOpenGLWidget->SetRenderWindow( renderWindow );

    // Liga o renderer aa interface grafica (Qt)
    this->qvtkOpenGLWidget->GetRenderWindow()->AddRenderer(renderer);

    // Adiciona eixos de orientacao
    // O smart pointer do widget VTK (HUD) eh uma variavel membro para que ele
    // nao destrua o objeto ao sair deste escopo, senao daria segfault.
    {
        // Este ator eh uma comodidate do VTK (cria eixos prontos).
        // A bussola poderia ser feita com qualquer ator.
        vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
        m_vtkAxesWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
        m_vtkAxesWidget->SetOutlineColor(0.9300, 0.5700, 0.1300);
        m_vtkAxesWidget->SetOrientationMarker(axes);
        m_vtkAxesWidget->SetInteractor( this->qvtkOpenGLWidget->GetRenderWindow()->GetInteractor() );
        m_vtkAxesWidget->SetViewport(0.0, 0.0, 0.2, 0.2);
        m_vtkAxesWidget->SetEnabled(1);
        m_vtkAxesWidget->InteractiveOn();
    }

    // adjusts view so everything fits in the screen
    renderer->ResetCamera();
}

void V3D::atualizaReamostrador()
{
    m_reamostradorGrid->SetSampleRate( m_txAmostragem, m_txAmostragem, m_txAmostragem );
    m_reamostradorGrid->Update();

    // Redesenha a cena
    this->qvtkOpenGLWidget->GetRenderWindow()->Render();
}

void V3D::onMenos()
{
    ++m_txAmostragem;
    atualizaReamostrador();
}

void V3D::onMais()
{
    --m_txAmostragem;
    if( m_txAmostragem < 1 )
        m_txAmostragem = 1;
    atualizaReamostrador();
}
