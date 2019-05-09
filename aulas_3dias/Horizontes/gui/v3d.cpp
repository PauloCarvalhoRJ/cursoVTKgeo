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
#include <vtkUnstructuredGrid.h>
#include <vtkQuad.h>

#include <QVTKOpenGLWidget.h>

#include <QFileDialog>

V3D::V3D()
{
    // Monta a GUI
    this->setupUi(this);

    // Usuario abre o arquivo de dados
    QString path_arq_dados = QFileDialog::getOpenFileName(this, "Abrir aquivo de dados" );

    //se o usuario nao cancelou, monta um ator com os dados do arquivo
    vtkSmartPointer<vtkActor> superficieActor = vtkSmartPointer<vtkActor>::New();
    if( ! path_arq_dados.isEmpty() ){

        // Indice da variavel a ser a profundidade ( 0 == primeira )
        uint indiceVariavel = 2;

        // Obter os parametros do grid (normalmente ha uma classe Grid com esses metadados)
        int nX = 108;
        int nY = 261;
        double X0 = 675000.0;
        double Y0 = 7138000.0;
        double dX = 100.0;
        double dY = 100.0;
        int numVertices = nX * nY;

        // Carrega a tabela de valores do arquivo
        std::vector< std::vector< double > > dados = Util::loadGEOEAS( path_arq_dados );

        // Array VTK para os valores de profundidade
        vtkSmartPointer<vtkFloatArray> values = vtkSmartPointer<vtkFloatArray>::New();
        values->SetName("values");

        // Carrega os valores de profundidade
        values->Allocate( numVertices );
        for( int j = 0; j < nY; ++j){
            for( int i = 0; i < nX; ++i){
                int indiceLinear = j * nX + i;
                // valor de profundidade
                double value = dados[indiceLinear][indiceVariavel];
                values->InsertNextValue( value );
            }
        }

        // Container VTK container para os vertices da malha
        vtkSmartPointer< vtkPoints > quadVertexes = vtkSmartPointer< vtkPoints >::New();
        quadVertexes->SetNumberOfPoints( numVertices );
        for( int i = 0; i < quadVertexes->GetNumberOfPoints(); ++i ){
            double x, y, z;
            uint ii, jj, kk;
            //converte o indice linear em enderecos de grid
            Util::indexToIJK( i, nX, nY, ii, jj, kk );
            //obter coordenada de mundo do centro da celula
            Util::IJKtoXYZ( ii, jj, 0,
                            X0, Y0, 0,
                            dX, dY, 0,
                             x,  y, z );
            //ler valor da profundidade
            int indiceLinear = jj * nX + ii;
            double sampleValue = dados[indiceLinear][indiceVariavel];
            //monta vertice usando o valor da profundidade como z
            quadVertexes->InsertPoint(i, x, y, sampleValue * 10.0);
        }

        // Cria um objeto do tipo malha nao estruturada (geometria livre)
        vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
        uint numCelulas = ( nX - 1 ) * ( nY - 1 ); //reparar numCelulas != numVertices
        unstructuredGrid->Allocate( numCelulas );
        for( uint i = 0; i < numCelulas; ++i ) {
            uint cellJ = i / ( nX - 1 );
            // cria um vtkCell do tipo vtkQuad (quadrilatero)
            vtkSmartPointer< vtkQuad > quad = vtkSmartPointer< vtkQuad >::New();
            quad->GetPointIds()->SetId(0, i + cellJ );
            quad->GetPointIds()->SetId(1, i + cellJ + 1 );
            quad->GetPointIds()->SetId(2, i + cellJ + nX + 1 );
            quad->GetPointIds()->SetId(3, i + cellJ + nX );
            // registra a definicao do quad no grid nao estruturado
            unstructuredGrid->InsertNextCell( quad->GetCellType(), quad->GetPointIds() );
        }
        // Passa as coordenadas dos vertices para o grid
        unstructuredGrid->SetPoints(quadVertexes);

        // Cria um mapper apropriado para um vtkUnstructuredGrid
        vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
        mapper->SetInputData( unstructuredGrid );
        mapper->Update();

        // Cria um actor contendo o modelo inteiro.
        superficieActor = vtkSmartPointer<vtkActor>::New();
        superficieActor->SetMapper(mapper);
    }

    // O renderer para as gerar as chamadas ao backend (OpenGL ou Mesa)
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor( superficieActor);

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

    //ajusta a camera de forma que toda a cena caiba na janela.
    renderer->ResetCamera();
}
