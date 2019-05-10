#include "gui/v3dMouseInteractorStyle.h"
#include <vtkRenderWindowInteractor.h>
#include <vtkPropPicker.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>

// Implementa a funcao New() desta classe
vtkStandardNewMacro(v3dMouseInteractorStyle);

void v3dMouseInteractorStyle::OnLeftButtonDown()
{
    m_isLBdown = true;

    // Repassar os eventos adiante (chamada da superclasse), uma boa pratica.
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void v3dMouseInteractorStyle::OnLeftButtonUp()
{
    // Para nao pickar durante a manipulacao do modelo
    if( ! m_isDragging ){
        // Obter a posicao do pick em coordenadas de tela.
        int* clickPos = this->GetInteractor()->GetEventPosition();

        // Pickar um objeto na posicao de tela onde o mouse foi clickado.
        vtkSmartPointer<vtkPropPicker>  picker = vtkSmartPointer<vtkPropPicker>::New();
        picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

        // Obter a posicao do pick em coordenadas de mundo.
        double* pos = picker->GetPickPosition();
        std::cout << "Posicao do pick (coordenadas de mundo): X="
                  << pos[0] << " Y=" << pos[1]
                  << " Z=" << pos[2] << std::endl;

        // Obter qual ator foi pickado.
        std::cout << "Ponteiro do ator: " << picker->GetActor() << std::endl;

        // Criar uma pequena esfera para assinalar o local onde o pick ocorreu.
        vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetCenter(pos[0], pos[1], pos[2]);
        sphereSource->SetRadius(100.0);

        // Cria um mapper para a pequena esfera
        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(sphereSource->GetOutputPort());

        // Cria um ator para a pequena esfera (atribui cor vermelha)
        vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
        actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
        actor->SetMapper(mapper);

        // Adiciona a pequena esfera aa cena.
        //this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetDefaultRenderer()->AddActor(actor);
        this->GetDefaultRenderer()->AddActor(actor);
    }

    m_isLBdown = false;
    m_isDragging = false;

    // Repassar os eventos adiante (chamada da superclasse), uma boa pratica.
    vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
}

void v3dMouseInteractorStyle::OnMouseMove()
{
    if( m_isLBdown )
        m_isDragging = true;

    // Repassar os eventos adiante (chamada da superclasse), uma boa pratica.
    vtkInteractorStyleTrackballCamera::OnMouseMove();
}

