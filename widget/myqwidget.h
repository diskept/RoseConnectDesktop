#ifndef MYQWIDGET_H
#define MYQWIDGET_H
#include <QListWidget>
#include "common/linker.h"

#include <QDialog>
#include <QLabel>
#include <QWidget>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include "widget/customlineedit.h"//c220729

class MyQWidget : public QWidget {//c220727

   Q_OBJECT
public:

   MyQWidget(QWidget *parent = nullptr);
   virtual QRectF boundingRectFor( const QRectF& sourceRect ) const;
   int moveCnt = 0;
protected:
    void make_CustomLineEdit();//c220728

    void dragMoveEvent(QDragMoveEvent* e) override;
    void dropEvent(QDropEvent* event) override;
    virtual void startDrag(Qt::DropActions supportedActions) ;
    void dragEnterEvent(QDragEnterEvent* event) override;
    //void dragLeaveEvent(QDragLeaveEvent* event) override;
    //Qt::DropAction supportedDropActions() ;
signals:

    void signal_dropEvent_hide_show(bool);//c220729
    void signal_dragEnterEvent_hide_show(bool);
   void itemDroped();

private:
    Linker *linker;
    CustomLineEdit *le_search_back;

};



/*
#include <QtWidgets>
class ListWidget : public QListWidget{
    Q_OBJECT
public:
    ListWidget(QWidget *parent = 0);
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
signals:
public slots:
};
*/
#endif // LISTBOX_H
