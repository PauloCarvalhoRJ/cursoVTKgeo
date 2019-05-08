#include "viewer3dlistwidget.h"

#include <QDragEnterEvent>
#include <QMenu>
#include <QMimeData>

Viewer3DListWidget::Viewer3DListWidget( QWidget *parent ) : QListWidget( parent )
{
    //configure list item context menu
    _contextMenu = new QMenu( this );
    this->setContextMenuPolicy( Qt::CustomContextMenu );
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onContextMenu(const QPoint &)));
}

uint Viewer3DListWidget::getNextInstance( const QString object_locator )
{
    uint greatest_instance_found = 0;
    return ++greatest_instance_found;
}

void Viewer3DListWidget::onContextMenu(const QPoint &mouse_location)
{
    QModelIndex index = this->indexAt(mouse_location); //get index to project component under mouse
    //Project* project = Application::instance()->getProject(); //get pointer to open project
    _contextMenu->clear(); //remove all context menu actions

    //get all selected items, this may include other items different from the one under the mouse pointer.
    QModelIndexList selected_indexes = this->selectionModel()->selectedIndexes();

    //if there is just one selected item.
    if( selected_indexes.size() == 1 ){
        //build context menu for any item
        if ( index.isValid() /*&& index.internalPointer() == project->getDataFilesGroup() */) {
            _contextMenu->addAction("Remove from view", this, SLOT(onRemoveFromView()));
        }
    }

    //show the context menu under the mouse cursor.
    if( _contextMenu->actions().size() > 0 )
        _contextMenu->exec(this->mapToGlobal(mouse_location));

}

void Viewer3DListWidget::onRemoveFromView()
{
    QModelIndex index = this->currentIndex();
    QListWidgetItem* item = this->takeItem( index.row() );
}
