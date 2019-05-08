#include "v3d.h"

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

V3D::V3D()
{
    this->setupUi(this);

    //Modelo 1: esfera
    vtkSmartPointer<vtkActor> sphereActor = vtkSmartPointer<vtkActor>::New();
    {
        // Cria uma fonte de dados: esfera parametrica
        vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->Update();

        // Cria um mapper que mapeia um vtkPolyData, que eh o resutado de vtkSphereSource, para primitivas geometricas
        vtkSmartPointer<vtkPolyDataMapper> sphereMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        sphereMapper->SetInputConnection(sphereSource->GetOutputPort());

        // O ator recebe as propriedades visuais do que se deseja desenhar
        // caso queiras dar uma cor, eh no ator que se faz.
        sphereActor->SetMapper(sphereMapper);
    }

    // O renderer para as gerar as chamadas ao backend (OpenGL ou Mesa)
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor( sphereActor);

    // Cria uma render window: a saída do backend (OpenGL neste caso) em uma janela do sistema gráfico (Qt e afins).
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    this->qvtkOpenGLWidget->SetRenderWindow( renderWindow );

    // Liga o renderer aa interface grafica (Qt)
    this->qvtkOpenGLWidget->GetRenderWindow()->AddRenderer(renderer);
}
