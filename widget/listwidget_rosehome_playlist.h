#ifndef LISTWIDGET_ROSEHOME_PLAYLIST_H
#define LISTWIDGET_ROSEHOME_PLAYLIST_H
#include <QListWidget>
#include "common/linker.h"

#include <QDialog>
#include <QLabel>
#include <QWidget>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>

class ListWidget_Rosehome_Playlist : public QListWidget {

   Q_OBJECT

public:
   ListWidget_Rosehome_Playlist(QWidget *parent = nullptr);
   virtual QRectF boundingRectFor( const QRectF& sourceRect ) const;

   int moveCnt = 0;

protected:
    void dragMoveEvent(QDragMoveEvent* e) override;
    void dropEvent(QDropEvent* event) override;
    void startDrag(Qt::DropActions supportedActions) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    //void dragLeaveEvent(QDragLeaveEvent* event) override;
    //Qt::DropAction supportedDropActions() ;

signals:
   void itemDroped();

private:
    Linker *linker;

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
#endif // LISTWIDGET_ROSEHOME_PLAYLIST_H
