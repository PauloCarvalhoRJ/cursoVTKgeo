#ifndef V3DMOUSEINTERACTORSTYLE_H
#define V3DMOUSEINTERACTORSTYLE_H
#include <vtkObjectFactory.h>
#include <vtkInteractorStyleTrackballCamera.h>

class v3dMouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static v3dMouseInteractorStyle* New();
    vtkTypeMacro(v3dMouseInteractorStyle, vtkInteractorStyleTrackballCamera)

    virtual void OnLeftButtonDown();

    virtual void OnLeftButtonUp();

    virtual void OnMouseMove();

protected:

    bool m_isDragging;
    bool m_isLBdown;
};

#endif
