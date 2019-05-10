#ifndef V3D_H
#define V3D_H

#include <QMainWindow>
#include "ui_v3d.h"

#include <vtkSmartPointer.h>

class vtkOrientationMarkerWidget;
class vtkExtractGrid;

class V3D : public QMainWindow, private Ui::v3d
{

    Q_OBJECT

public:

    V3D();

    ~V3D() {}

protected:
    int m_nX;
    int m_nY;
    int m_nZ;
    int m_delta;
    vtkSmartPointer<vtkOrientationMarkerWidget> m_vtkAxesWidget;
    vtkSmartPointer<vtkExtractGrid> m_reamostradorGrid;
    void atualizaReamostrador();

private Q_SLOTS:
    void onMenos();
    void onMais();
};

#endif
