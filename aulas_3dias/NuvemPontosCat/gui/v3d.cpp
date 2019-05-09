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


#include <QVTKOpenGLWidget.h>

#include <QFileDialog>

V3D::V3D()
{
    //monta a GUI
    this->setupUi(this);

    //usuario abre o arquivo de dados
    QString path_arq_dados = QFileDialog::getOpenFileName(this, "Abrir aquivo de dados" );

    //se o usuario nao cancelou, monta um ator com os dados do arquivo
    vtkSmartPointer<vtkActor> pointCloudActor = vtkSmartPointer<vtkActor>::New();
    if( ! path_arq_dados.isEmpty() ){
        int indiceCoordX = 1;
        int indiceCoordY = 2;
        int indiceValor = 5;

        // Carrega a tabela de valores do arquivo
        std::vector< std::vector< double > > dados = Util::loadGEOEAS( path_arq_dados );

        // A geometria de um ponto (coordenadas)
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

        // A topologia de um ponto (um vertice)
        vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();

        // Cria um VTK array para os valores amostrais
        vtkSmartPointer<vtkFloatArray> values = vtkSmartPointer<vtkFloatArray>::New();
        values->SetName("values");
        values->Allocate( dados.size() );

        // Monta pontos com os X, Y lidos do arquivo
        // aproveita tambem para carregar os valores amostrais
        vtkSmartPointer<vtkIdList> pids = vtkSmartPointer<vtkIdList>::New();
        pids->Allocate( dados.size() );
        for( std::vector<double>& registro : dados ){
            //ponto
            double x = registro[indiceCoordX];
            double y = registro[indiceCoordY];
            double z = 0.0;
            pids->InsertNextId( points->InsertNextPoint( x, y, z ) );
            //valor amostral
            values->InsertNextValue( registro[indiceValor] );
            //atualiza min e max
        }
        vertices->InsertNextCell( pids );

        // Cria um objeto polydata que receberah os pontos
        vtkSmartPointer<vtkPolyData> pointCloud = vtkSmartPointer<vtkPolyData>::New();

        // Passa os pontos, a topologia e os valores amostrais para o objeto polydata
        pointCloud->SetPoints(points);
        pointCloud->SetVerts(vertices);
        pointCloud->GetPointData()->SetScalars( values );
        pointCloud->GetPointData()->SetActiveScalars("values");

        // Cria uma tabela de cores para todos os codigos possiveis da variavel categorica.
        std::vector< std::pair< uint, QColor > > tabelaCategoriaCores = { { 1, QColor( Qt::yellow ).darker() },
                                                                          { 2, QColor( Qt::cyan )            } };
        vtkSmartPointer<vtkLookupTable> lut = Util::getCategoricalColorTable( tabelaCategoriaCores );

        // Cria um mapper que mapeia vtkPolyData para primitivas geometricas
        vtkSmartPointer<vtkPolyDataMapper> pointCloudMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        pointCloudMapper->SetInputData(pointCloud);
        pointCloudMapper->SetLookupTable(lut);
        pointCloudMapper->SetScalarModeToUsePointFieldData();
        pointCloudMapper->SetColorModeToMapScalars();
        pointCloudMapper->SelectColorArray("values");

        // Cria um ator (propriedades visuais, como a cor)
        pointCloudActor->SetMapper(pointCloudMapper);
        pointCloudActor->GetProperty()->SetPointSize(3);
    }

    // O renderer para as gerar as chamadas ao backend (OpenGL ou Mesa)
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor( pointCloudActor);

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
