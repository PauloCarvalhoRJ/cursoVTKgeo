#ifndef V3D_H
#define V3D_H

#include <QMainWindow>
#include "ui_v3d.h"

class V3D : public QMainWindow, private Ui::v3d
{

    Q_OBJECT

public:

  V3D();

  ~V3D() {}

};

#endif
