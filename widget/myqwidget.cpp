
#include "common/global.h"
#include "myqwidget.h"
#include <QDrag>
#include <QMimeData>
//#include <QDropEvent>
//#include <QDropEvent>
//#include <QDragEnterEvent>

MyQWidget::MyQWidget(QWidget* parent)

   : QWidget(parent)
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
    //this->setDragEnabled(true); // ***
    this->setAcceptDrops(true); // ***
    //this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    //this->setDefaultDropAction(Qt::MoveAction); //
    //this->setDropIndicatorShown(true); // ***
    //this->setDragDropMode(QAbstractItemView::InternalMove);
    //------------------------------------------------------

    //make_CustomLineEdit();

    //this->setStyleSheet("QListWidget::item:hover {background-color:yellow;}");
    linker = Linker::getInstance();

}

void MyQWidget::make_CustomLineEdit(){//c220728
    this->le_search_back = new CustomLineEdit();//c220727
    this->le_search_back->setObjectName("le_search");
    //this->le_search_back->setStyleSheet("#le_search { background-color:white; border-radius:20px; font-size:15px; color:#333333; padding-left:35px;}");
    this->le_search_back->setStyleSheet("#le_search { background-color:#333333;  font-size:23px; color:#333333; padding-left:35px;}");
    this->le_search_back->setMaxLength(100);
    //this->le_search_back->setText("Supports drag and drop function. Drop it here.");
    this->le_search_back->setFixedSize(this->width(),global.height_mainwindow);//c220930
    this->le_search_back->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
    this->le_search_back->hide();
}

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QPixmap>
void MyQWidget::dropEvent(QDropEvent *event)
{
    QWidget::dropEvent(event);
    event->accept();
    //print_debug();
    //if(global.isMoveMode == true){

    //qDebug() << "dropEvent"<<event;


    //}
    //emit linker->signal_dropEvent_hide_show(true);//c220727
    //emit signal_dropEvent_hide_show(true);//c220727

}

void MyQWidget::startDrag(Qt::DropActions supportedActions){


}

void MyQWidget::dragEnterEvent(QDragEnterEvent* event) {

    QWidget::dragEnterEvent(event);
    event->accept();
    print_debug();
    qDebug() << "QDragEnterEvent"<<event;
    emit linker->signal_dragEnterEvent_hide_show(true);//c220727
    //this->hide();
    //this->le_search_back->show();
    //emit signal_dragEnterEvent_hide_show(true);//c220727
        //qDebug() << "QDragEnterEvent"<<event;

        //for(QModelIndex index: selectedIndexes()){
         //   print_debug();
         //   QListWidgetItem* item2 = item(index.row());
          //  QWidget *ww = itemWidget(item2);
          //  ww->setStyleSheet(ww->styleSheet().replace("#333333","#9c9c9c"));//e6e1a5
         //   ww->setStyleSheet(ww->styleSheet().replace("#e6e1a5","#9c9c9c"));//e6e1a5

        //}


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
void MyQWidget::dragMoveEvent(QDragMoveEvent *event)
{

    //print_debug();
    //qDebug() << "QDragMoveEvent"<<event;
        QWidget::dragMoveEvent(event);

        event->acceptProposedAction();

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

QRectF MyQWidget::boundingRectFor( const QRectF& sourceRect ) const {
    return sourceRect.adjusted(-2,-2,2,2);

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
