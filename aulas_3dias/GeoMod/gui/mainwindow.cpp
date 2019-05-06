#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Arruma os widgets.
    ui->setupUi(this);

    //Coloca o nome e a versao do aplicativo na barra de titulos da janela.
    this->setWindowTitle(APP_NAME_VER);

    //maximiza a janela
    this->showMaximized();

    //configura as posicoes dos splitter
    ui->splitter->setSizes( QList<int>() << 70 << 200 ); //proporcao 70/200 (nao eh pixels)
    ui->splitter_2->setSizes( QList<int>() << 700 << 100 );

    //Mostra o widget 3D.
    //ui->frmContent->layout()->addWidget( new View3DWidget( this ) );
}

MainWindow::~MainWindow()
{
    delete ui;
}
