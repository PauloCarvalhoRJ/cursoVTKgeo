#include <QApplication>

#include "gui/mainwindow.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Hello VTK!\n");
    printf("Iniciando...\n");
    QApplication application(argc, argv);
    QApplication::setOrganizationName(APP_NAME);
    QApplication::setOrganizationDomain("geomod.unisinos.br");
    QApplication::setApplicationName(APP_NAME_VER);
    MainWindow mw;
    mw.show();

    return application.exec();
    printf("Finalizou normalmente.\n");
    return 0;
}
