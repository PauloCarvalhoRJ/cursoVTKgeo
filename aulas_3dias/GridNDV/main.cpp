#include <QApplication>
#include "gui/v3d.h"

int main(int argc, char* argv[])
{
  QApplication app( argc, argv );

  V3D v3d;
  v3d.show();

  return app.exec();
}
