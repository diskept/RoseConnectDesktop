#include "listwidget_rosehome_playlist.h"

#include "common/global.h"

#include <QDrag>
#include <QMimeData>

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QPixmap>
//#include <QDropEvent>
//#include <QDropEvent>
//#include <QDragEnterEvent>

ListWidget_Rosehome_Playlist::ListWidget_Rosehome_Playlist(QWidget* parent)

   : QListWidget(parent)
{
/*
   //this->setViewMode(QListView::IconMode);
   //this->setIconSize(QSize(55, 55));
   this->setSelectionMode(QAbstractItemView::SingleSelection);
   this->setDragEnabled(true);
   this->setDefaultDropAction(Qt::MoveAction);
   this->setAcceptDrops(true);
   this->setDropIndicatorShown(true);

*/

    /*
    //--------------------------------------------------
    this->setDragEnabled(true); // ***
    this->setAcceptDrops(true); // ***
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    //this->setDefaultDropAction(Qt::MoveAction); //
    this->setDefaultDropAction(Qt::CopyAction);
    //this->setDropIndicatorShown(true); // ***
    //this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setViewMode(QListView::ListMode);

    //------------------------------------------------------
*/


    //--------------------------------------------------
    this->setDragEnabled(true); // ***
    this->setAcceptDrops(true); // ***
    //this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setDefaultDropAction(Qt::MoveAction); //
    //this->setDefaultDropAction(Qt::CopyAction);
    this->setDropIndicatorShown(true); // ***
    this->setDragDropMode(QAbstractItemView::InternalMove);
    //this->setDragDropMode(QAbstractItemView::NoDragDrop);
    //this->setViewMode(QListView::ListMode);

    //------------------------------------------------------

    linker = Linker::getInstance();

}


void ListWidget_Rosehome_Playlist::dropEvent(QDropEvent *event)
{
    print_debug();
    if(global.isMoveModePlaylist == true){

        qDebug() << "dropEvent"<<event;
        QListWidgetItem* item = currentItem();
        int so_tmp_index = row(item);
        qDebug() << "so_tmp_index = " << so_tmp_index;
        //delete takeItem(row(item));
        qDebug() << event->pos();
        QListWidgetItem* item2 = itemAt(event->pos());
        int tg_tmp_index = row(item2);
        qDebug() << "tg_tmp_index = " << tg_tmp_index;

        const QMimeData *mimeData = event->mimeData();

        if (mimeData->hasFormat("application/x-qabstractitemmodeldatalist")) {
            print_debug();

            if( so_tmp_index != tg_tmp_index){
                emit linker->signal_dragDrop_playlist();
            }


        }
    }
    QListWidget::dropEvent(event);

}

void ListWidget_Rosehome_Playlist::startDrag(Qt::DropActions supportedActions){

    if(global.isMoveModePlaylist == true){
        QDrag *drag = new QDrag(this);
        drag->setMimeData(model()->mimeData(selectedIndexes()));
        QPixmap pixmap(viewport()->visibleRegion().boundingRect().size());
        pixmap.fill(Qt::transparent);//rgb(227,184,138)
        //pixmap.fill(QColor(227,184,138));//rgb(227,184,138)
        QPainter painter(&pixmap);
        QPen pen = painter.pen();
        painter.setPen("#B18658");
        pen.setWidth(300);
        moveCnt = selectedIndexes().count();
        for(QModelIndex index: selectedIndexes()){
            //QListWidgetItem* item2 = item(index.row());
            //QWidget *ww = itemWidget(item2);
            //ww->setStyleSheet(ww->styleSheet().replace("#333333","#9c9c9c"));//e6e1a5
            //ww->setStyleSheet(ww->styleSheet().replace("#e6e1a5","#9c9c9c"));//e6e1a5

            painter.drawPixmap(visualRect(index), viewport()->grab(visualRect(index)));
            painter.drawRect(visualRect(index));
            painter.setCompositionMode(QPainter::CompositionMode_ColorDodge);
            //painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            painter.fillRect(visualRect(index), "#0f0070");
        }
        drag->setPixmap(pixmap);
        drag->setHotSpot(viewport()->mapFromGlobal(QCursor::pos()));
        drag->exec(supportedActions, Qt::MoveAction);
    }
}

void ListWidget_Rosehome_Playlist::dragEnterEvent(QDragEnterEvent* event) {

    print_debug();
    if(global.isMoveModePlaylist == true){
        //qDebug() << "QDragEnterEvent"<<event;

        //for(QModelIndex index: selectedIndexes()){
         //   print_debug();
         //   QListWidgetItem* item2 = item(index.row());
          //  QWidget *ww = itemWidget(item2);
          //  ww->setStyleSheet(ww->styleSheet().replace("#333333","#9c9c9c"));//e6e1a5
         //   ww->setStyleSheet(ww->styleSheet().replace("#e6e1a5","#9c9c9c"));//e6e1a5

        //}

        QListWidget::dragEnterEvent(event);
        event->accept();
    }
    /*
   if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")){
       print_debug();
       QListWidgetItem* item = currentItem();
       //item->setTextColor(QColor::fromRgb(22,22,22));
       event->accept();
       //emit linker->signal_dragEnter();
   }else{
       event->ignore();
   }
*/
//
}
void ListWidget_Rosehome_Playlist::dragMoveEvent(QDragMoveEvent *event)
{

    if(global.isMoveModePlaylist == true){
        QListWidget::dragMoveEvent(event);

        event->acceptProposedAction();
    }
    //qDebug() << "QDragMoveEvent"<<event;
    //QListWidgetItem* item = currentItem();
    //int so_tmp_index = row(item);
    //qDebug() << "so_tmp_index = " << so_tmp_index;
    //delete takeItem(row(item));
    //const QMimeData *mimeData=event->mimeData();
    //qDebug() << "mimeData = " << mimeData;
    //if(mimeData->hasImage()){
    /*
        print_debug();
        QList<QListWidgetItem *> m_items = selectedItems();
        if(m_items.isEmpty()){
            print_debug();
            return;
        }
        QMimeData *data = mimeData(m_items);
        QDrag *drag = new QDrag(this);
        QPixmap pixmap = QPixmap::fromImage(qvariant_cast<QImage>(data->imageData()));
        drag->setPixmap(pixmap);
        drag->setMimeData(data);
        drag->setHotSpot(pixmap.rect().center());
        drag->exec(Qt::MoveAction);
        //event->acceptProposedAction();
    //}
*/
/*
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")){
        print_debug();
        //emit linker->signal_dragEnter();
        //QListWidgetItem* item = currentItem();
        //item->setTextColor(QColor::fromRgb(22,22,22));

        event->acceptProposedAction();

    }
*/
}


QRectF ListWidget_Rosehome_Playlist::boundingRectFor( const QRectF& sourceRect ) const {

    return sourceRect.adjusted(-2, -2, 2, 2);
}

/*
if(supportedActions & Qt::MoveAction){
    QList<QListWidgetItem *> m_items = selectedItems();
    if(m_items.isEmpty()){
        print_debug();
        return;
    }
    QMimeData *data = mimeData(m_items);
    QDrag *drag = new QDrag(this);
    QPixmap pixmap = QPixmap::fromImage(qvariant_cast<QImage>(data->imageData()));
    drag->setPixmap(pixmap);
    drag->setMimeData(data);
    drag->setHotSpot(pixmap.rect().center());
    drag->exec(Qt::MoveAction);
}
else
    QListWidget::startDrag(supportedActions);
    *
    */
/*
void ListWidget::startDrag(Qt::DropActions supportedActions){
print_debug();
    QListWidgetItem* item = currentItem();
       QMimeData* mimeData = new QMimeData;
       QByteArray ba;
       ba = item->text().toLatin1().data();
       mimeData->setData("application/x-qabstractitemmodeldatalist", ba);
       QDrag* drag = new QDrag(this);
       drag->setMimeData(mimeData);
       if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
           print_debug();
           item->setTextColor(QColor::fromRgb(22,22,22));
           //delete takeItem(row(item));
           //emit itemDroped();
           emit linker->signal_dragEnter();
       }
}


Qt::DropAction ListWidget::supportedDropActions() {
print_debug();
   return Qt::MoveAction;
}

*/





/*
void ListWidget::dragLeaveEvent(QDragMoveEvent *event)
{
    qDebug() << "QDragLeaveEvent"<<event;
    event->acceptProposedAction();
}
*/
/*
void ListWidget::dragMoveEvent(QDragMoveEvent* e) {
    print_debug();
    qDebug() << e->pos();
    QListWidgetItem* item = currentItem();
    int tmp_index = row(item);
    qDebug() << "tmp_index" << tmp_index;

   if (e->mimeData()->hasFormat("application/x-item") && e->source() != this) {
       e->setDropAction(Qt::MoveAction);
       e->accept();
   } else
       e->ignore();


}

void  ListWidget::dropEvent(QDropEvent *event) {
    QListWidget::dropEvent(event);
    //event->setDropAction(Qt::MoveAction);
    //event->accept();
print_debug();
qDebug() << event->pos();
QListWidgetItem* item = currentItem();
int tmp_index = row(item);
qDebug() << "tmp_index" << tmp_index;

   if (event->mimeData()->hasFormat("application/x-item")) {
       event->accept();
       event->setDropAction(Qt::MoveAction);
       QListWidgetItem *item = new QListWidgetItem;
       QString name = event->mimeData()->data("application/x-item");
       item->setText(name);
       item->setIcon(QIcon(":/images/iString")); //set path to image
       addItem(item);
   } else
       event->ignore();

}

void ListWidget::startDrag(Qt::DropActions supportedActions) {
print_debug();
   QListWidgetItem* item = currentItem();
   int tmp_index = row(item);
   qDebug() << "tmp_index" << tmp_index;

   QMimeData* mimeData = new QMimeData;
   QByteArray ba;
   ba = item->text().toLatin1().data();
   mimeData->setData("application/x-item", ba);
   QDrag* drag = new QDrag(this);
   drag->setMimeData(mimeData);
   if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
       delete takeItem(row(item));
       emit itemDroped();
   }

}

void ListWidget::dragEnterEvent(QDragEnterEvent* event) {
print_debug();

   if (event->mimeData()->hasFormat("application/x-item"))
       event->accept();
   else
       event->ignore();

}

Qt::DropAction ListWidget::supportedDropActions() {
print_debug();
   return Qt::MoveAction;
}
*/
