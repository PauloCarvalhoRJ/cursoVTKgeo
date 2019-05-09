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
#include <vtkLine.h>
#include <vtkCellData.h>
#include <vtkTubeFilter.h>
#include <vtkCamera.h>

#include <QVTKOpenGLWidget.h>

#include <QFileDialog>

V3D::V3D()
{
    //monta a GUI
    this->setupUi(this);

    //usuario abre o arquivo de dados
    QString path_arq_dados = QFileDialog::getOpenFileName(this, "Abrir aquivo de dados" );

    //se o usuario nao cancelou, monta um ator com os dados do arquivo
    vtkSmartPointer<vtkActor> pocoActor = vtkSmartPointer<vtkActor>::New();
    if( ! path_arq_dados.isEmpty() ){
        int indiceCoordXi = 0;
        int indiceCoordXf = 2;
        int indiceCoordYi = 1;
        int indiceCoordYf = 3;
        int indiceCoordZi = 4;
        int indiceCoordZf = 5;
        int indiceValor   = 7;

        //array VTK para conter os valores amostrais
        vtkSmartPointer<vtkFloatArray> values = vtkSmartPointer<vtkFloatArray>::New();
        values->SetName("values");

        // Carrega a tabela de valores do arquivo
        std::vector< std::vector< double > > dados = Util::loadGEOEAS( path_arq_dados );

        // Constroi a trajetoria como uma serie de segmentos
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkCellArray> segments = vtkSmartPointer<vtkCellArray>::New();
        for( std::vector< double >& registro : dados ){
            // ler as coordenadas X,Y,Z das extremidades do segmento
            double x0 = registro[ indiceCoordXi ];
            double y0 = registro[ indiceCoordYi ];
            double z0 = registro[ indiceCoordZi ];
            double x1 = registro[ indiceCoordXf ];
            double y1 = registro[ indiceCoordYf ];
            double z1 = registro[ indiceCoordZf ];
            // cadastra as coordenadas na lista de vertices
            vtkIdType id0 = points->InsertNextPoint( x0, y0, z0 );
            vtkIdType id1 = points->InsertNextPoint( x1, y1, z1 );
            // cria um objeto segmento
            vtkSmartPointer<vtkLine> segment = vtkSmartPointer<vtkLine>::New();
            // diz que as extremidades do segmento sao dois vertices identificados pelos seus IDs
            segment->GetPointIds()->SetId( 0, id0 );
            segment->GetPointIds()->SetId( 1, id1 );
            // guarda a definicao do segmento no cell array
            segments->InsertNextCell( segment );
            // aproveita o loop para ler o valor
            double value = registro[ indiceValor ];
            values->InsertNextValue( value );
        }

        // Cria uma linha poligonal a partir das definicoes de segmentos e coordenadas
        // astribuindo valores para os SEGMENTOS e nao para os vertices (reparar que estamos usando GetCellData())
        vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
        poly->SetPoints( points );
        poly->SetLines( segments );
        poly->GetCellData()->SetScalars( values );
        poly->GetCellData()->SetActiveScalars("values");

        // Transforma a linha em um tubo
        vtkSmartPointer<vtkTubeFilter> tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
        tubeFilter->SetInputData( poly );
        tubeFilter->SetRadius(10); //default eh .5
        tubeFilter->SetNumberOfSides(50);
        tubeFilter->Update();

        // Cria uma tabela de cores para todos os codigos possiveis da variavel categorica.
        std::vector< std::pair< uint, QColor > > tabelaCategoriaCores = { {  1, QColor( Qt::yellow ) },
                                                                          {  2, QColor( Qt::blue ) },
                                                                          {  3, QColor( Qt::red ) },
                                                                          {  4, QColor( Qt::green ) },
                                                                          {  5, QColor( Qt::gray ) },
                                                                          {  6, QColor( Qt::white ) },
                                                                          {  7, QColor( Qt::darkBlue ) },
                                                                          {  8, QColor( Qt::magenta ) },
                                                                          {  9, QColor( Qt::darkBlue ) },
                                                                          { 10, QColor( Qt::darkYellow ) },
                                                                          { 11, QColor( Qt::lightGray ) },
                                                                          { 12, QColor( Qt::darkGray ) },
                                                                          { 13, QColor( Qt::darkGreen ) },
                                                                          { 14, QColor( Qt::darkRed ) },
                                                                          { 15, QColor( Qt::darkCyan ) } };
        vtkSmartPointer<vtkLookupTable> lut = Util::getCategoricalColorTable( tabelaCategoriaCores );

        // Cria um maper para objetos do tipo poly data (poligonais)
        vtkSmartPointer<vtkPolyDataMapper> tubeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        tubeMapper->SetInputConnection(tubeFilter->GetOutputPort());
        tubeMapper->SetLookupTable(lut);
        tubeMapper->SetScalarModeToUseCellData();
        tubeMapper->SetColorModeToMapScalars();
        tubeMapper->SelectColorArray("values");

        // Cria o actor para o objeto
        pocoActor = vtkSmartPointer<vtkActor>::New();
        pocoActor->GetProperty()->SetOpacity(0.7);
        pocoActor->SetMapper(tubeMapper);
    }

    // O renderer para as gerar as chamadas ao backend (OpenGL ou Mesa)
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor( pocoActor );

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

    //reorienta a camera para olhar ortogonalmente ao plano XZ (ver o poco de lado e nao de cima)
    {
        double *fp = renderer->GetActiveCamera()->GetFocalPoint();
        double *p = renderer->GetActiveCamera()->GetPosition();
        double dist = std::sqrt((p[0] - fp[0]) * (p[0] - fp[0])
                              + (p[1] - fp[1]) * (p[1] - fp[1])
                              + (p[2] - fp[2]) * (p[2] - fp[2]));
        renderer->GetActiveCamera()->SetPosition(fp[0], fp[1] - dist, fp[2]);
        renderer->GetActiveCamera()->SetViewUp(0.0, 0.0, 1.0);
        // redesenha a cena
        this->qvtkOpenGLWidget->GetRenderWindow()->Render();
    }

}
