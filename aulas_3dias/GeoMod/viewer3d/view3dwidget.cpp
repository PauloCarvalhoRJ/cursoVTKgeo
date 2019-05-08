//----------Since we're not building with CMake, we need to init the VTK
// modules------------------
//--------------linking with the VTK libraries is often not
// enough--------------------------------
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2) // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)
//------------------------------------------------------------------------------------------------

#include "ui_view3dwidget.h"
#include "view3dwidget.h"
#include <QVTKOpenGLWidget.h>

#include <QSettings>
#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkFXAAOptions.h>

#include "view3dverticalexaggerationwidget.h"

View3DWidget::View3DWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::View3DWidget),
      _verticalExaggWiget(nullptr)
{
    ui->setupUi(this);

    // restore main splitter position
    {
        QSettings qs;
        QByteArray state;
        if (qs.contains("viewer3dsplitter"))
            state = qs.value("viewer3dsplitter").toByteArray();
        ui->splitter->restoreState(state);
    }
    // restore left splitter position
    {
        QSettings qs;
        QByteArray state;
        if (qs.contains("viewer3dsplitter2"))
            state = qs.value("viewer3dsplitter2").toByteArray();
        ui->splitter_2->restoreState(state);
    }

    _vtkwidget = new QVTKOpenGLWidget();

    //===========VTK TEST CODE==========================================
    //    vtkSmartPointer<vtkSphereSource> sphereSource =
    //        vtkSmartPointer<vtkSphereSource>::New();
    //    vtkSmartPointer<vtkPolyDataMapper> sphereMapper =
    //        vtkSmartPointer<vtkPolyDataMapper>::New();
    //    sphereMapper->SetInputConnection( sphereSource->GetOutputPort() );
    //    vtkSmartPointer<vtkActor> sphereActor =
    //        vtkSmartPointer<vtkActor>::New();
    //    sphereActor->SetMapper( sphereMapper );
    //==================================================================

    _renderer = vtkSmartPointer<vtkRenderer>::New();

    // add a nice sky-like background
    _renderer->GradientBackgroundOn();
    _renderer->SetBackground(0.9, 0.9, 1);
    _renderer->SetBackground2(0.5, 0.5, 1);


    // enable antialiasing (fast approximate method)
    _renderer->UseFXAAOn();

    // configure the FXAA antialiasing
    vtkSmartPointer<vtkFXAAOptions> fxaaOptions = _renderer->GetFXAAOptions();
    fxaaOptions->SetSubpixelBlendLimit( 1/2.0 );
    //fxaaOptions->SetSubpixelContrastThreshold(1/2.0);
    //fxaaOptions->SetRelativeContrastThreshold(0.125);
    //fxaaOptions->SetHardContrastThreshold(0.045);
    //fxaaOptions->SetSubpixelBlendLimit(0.75);
    //fxaaOptions->SetSubpixelContrastThreshold(0.25);
    //fxaaOptions->SetUseHighQualityEndpoints(true);
    //fxaaOptions->SetEndpointSearchIterations(12);

    //    renderer->AddActor( sphereActor );  // VTK TEST CODE
    //    vtkRenderWindow* renwin = vtkRenderWindow::New();
    //	vtkGenericOpenGLRenderWindow* glrw =
    // vtkGenericOpenGLRenderWindow::SafeDownCast(renwin);
    //	_vtkwidget->SetRenderWindow( glrw );


    _vtkwidget->SetRenderWindow( vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New() );
    _vtkwidget->GetRenderWindow()->AddRenderer(_renderer);
    _vtkwidget->setFocusPolicy(Qt::StrongFocus);

    //----------------------adding the orientation axes-------------------------
    vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
    _vtkAxesWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    _vtkAxesWidget->SetOutlineColor(0.9300, 0.5700, 0.1300);
    _vtkAxesWidget->SetOrientationMarker(axes);
    _vtkAxesWidget->SetInteractor(_vtkwidget->GetRenderWindow()->GetInteractor());
    _vtkAxesWidget->SetViewport(0.0, 0.0, 0.2, 0.2);
    _vtkAxesWidget->SetEnabled(1);
    _vtkAxesWidget->InteractiveOn();
    //--------------------------------------------------------------------------

    // adjusts view so everything fits in the screen
    _renderer->ResetCamera();

    // add the VTK widget the layout
    ui->frmViewer->layout()->addWidget(_vtkwidget);

    // enable and configure the objects list's drag-and-drop feature.
    // ui->listWidget->setDragEnabled(true);
    // ui->listWidget->setDrafgDropMode(QAbstractItemView::DragDrop);
    // ui->listWidget->viewport()->setAcceptDrops(true);
    // ui->listWidget->setDropIndicatorShown(true);
    ui->listWidget->setAcceptDrops(true);

    _verticalExaggWiget = new View3DVerticalExaggerationWidget(this);
    _verticalExaggWiget->hide();
    //_verticalExaggWiget->setWindowFlags( Qt::CustomizeWindowHint );
    //_verticalExaggWiget->setWindowFlags( Qt::FramelessWindowHint );
    connect(_verticalExaggWiget, SIGNAL(valueChanged(double)), this,
            SLOT(onVerticalExaggerationChanged(double)));
}

View3DWidget::~View3DWidget()
{
    QSettings qs;
    qs.setValue("viewer3dsplitter", ui->splitter->saveState());
    qs.setValue("viewer3dsplitter2", ui->splitter_2->saveState());
    delete ui;
}

void View3DWidget::removeCurrentConfigWidget()
{
}

void View3DWidget::onViewAll()
{
    // adjusts view so everything fits in the screen
    _renderer->ResetCamera();
    // redraw the scene
    _vtkwidget->GetRenderWindow()->Render();
}

void View3DWidget::onLookAtXY()
{
    //_renderer->ResetCamera();
    double *fp = _renderer->GetActiveCamera()->GetFocalPoint();
    double *p = _renderer->GetActiveCamera()->GetPosition();
    double dist
        = std::sqrt((p[0] - fp[0]) * (p[0] - fp[0]) + (p[1] - fp[1]) * (p[1] - fp[1])
                    + (p[2] - fp[2]) * (p[2] - fp[2]));
    _renderer->GetActiveCamera()->SetPosition(fp[0], fp[1], fp[2] + dist);
    _renderer->GetActiveCamera()->SetViewUp(0.0, 1.0, 0.0);
    // redraw the scene
    _vtkwidget->GetRenderWindow()->Render();
}

void View3DWidget::onLookAtXZ()
{
    double *fp = _renderer->GetActiveCamera()->GetFocalPoint();
    double *p = _renderer->GetActiveCamera()->GetPosition();
    double dist
        = std::sqrt((p[0] - fp[0]) * (p[0] - fp[0]) + (p[1] - fp[1]) * (p[1] - fp[1])
                    + (p[2] - fp[2]) * (p[2] - fp[2]));
    _renderer->GetActiveCamera()->SetPosition(fp[0], fp[1] - dist, fp[2]);
    _renderer->GetActiveCamera()->SetViewUp(0.0, 0.0, 1.0);
    // redraw the scene
    _vtkwidget->GetRenderWindow()->Render();
}

void View3DWidget::onLookAtYZ()
{
    double *fp = _renderer->GetActiveCamera()->GetFocalPoint();
    double *p = _renderer->GetActiveCamera()->GetPosition();
    double dist
        = std::sqrt((p[0] - fp[0]) * (p[0] - fp[0]) + (p[1] - fp[1]) * (p[1] - fp[1])
                    + (p[2] - fp[2]) * (p[2] - fp[2]));
    _renderer->GetActiveCamera()->SetPosition(fp[0] + dist, fp[1], fp[2]);
    _renderer->GetActiveCamera()->SetViewUp(0.0, 0.0, 1.0);
    // redraw the scene
    _vtkwidget->GetRenderWindow()->Render();
}

void View3DWidget::onVerticalExaggeration()
{
    _verticalExaggWiget->show();
    QPoint mousePos = mapFromGlobal(QCursor::pos());
    mousePos.setX(mousePos.x() - _verticalExaggWiget->width());
    _verticalExaggWiget->move(mousePos);
    _verticalExaggWiget->setFocus();
}

void View3DWidget::onVerticalExaggerationChanged(double value)
{
    // Get the current model (objects) transform matrix.
    vtkSmartPointer<vtkMatrix4x4> xform
        = _renderer->GetActiveCamera()->GetModelTransformMatrix();

    // Get the camera's focal point (where it is looking at).
    double *fp = _renderer->GetActiveCamera()->GetFocalPoint();

    // Get where the focal point would have to go so the scene stays focused.
    double offset = fp[2] * value;

    // Scale the whole scene along the Z axis.
    xform->SetElement(2, 2, value);

    // Translate back the whole scene so the scene stays in the same place when viewed.
    xform->SetElement(2, 3, fp[2] - offset);

    // Apply transform to the whole scene.
    _renderer->GetActiveCamera()->SetModelTransformMatrix(xform);

    // redraw the scene (none of these works :( )
    //    _renderer->Render();
    //    _vtkwidget->GetRenderWindow()->GetInteractor()->Render();
    //    _vtkwidget->GetRenderWindow()->Render();
    //    _vtkwidget->repaint();

    // Perturb the splitter to force a redraw.
    // TODO: find out a more elegant way to make the VTK widget redraw
    {
        QList<int> oldSizes = ui->splitter->sizes();
        QList<int> tmpSizes = oldSizes;
        tmpSizes[0] = oldSizes[0] + 1;
        tmpSizes[1] = oldSizes[1] - 1;
        ui->splitter->setSizes(tmpSizes);
        qApp->processEvents();
        ui->splitter->setSizes(oldSizes);
    }
}
