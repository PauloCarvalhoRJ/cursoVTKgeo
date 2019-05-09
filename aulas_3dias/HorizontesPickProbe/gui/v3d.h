#ifndef V3D_H
#define V3D_H

#include <QMainWindow>
#include "ui_v3d.h"

#include <vtkSmartPointer.h>

class vtkOrientationMarkerWidget;
class vtkActor;

class V3D : public QMainWindow, private Ui::v3d
{

    Q_OBJECT

public:

    V3D();

    ~V3D() {}

protected:
    vtkSmartPointer<vtkOrientationMarkerWidget> m_vtkAxesWidget;

    vtkSmartPointer<vtkActor> criaHorizonte(int indiceVariavel, const std::vector<std::vector<double> > &dados);
};

#endif
