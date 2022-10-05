#ifndef MUSICLIST_GENREALBUM_H
#define MUSICLIST_GENREALBUM_H

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QScrollArea>

#include <data/datapopup.h>

#include <common/filedownloader.h>
#include <common/linker.h>

#include <widget/flowlayout.h>
#include "widget/optionpopup.h"
#include "proc_addtackinplaylist.h"

class MusicList_GenreAlbum : public QWidget{
    Q_OBJECT

public:
    explicit MusicList_GenreAlbum(QWidget *parent = nullptr);
    ~MusicList_GenreAlbum();

    void requestInitData(QJsonObject jsonObj);

private:
    void setInit();
    void setUIControl();

    void requestAlbums();

    void wheelEvent(QWheelEvent *event) override;

private:
    QScrollArea *scrollArea;
    FlowLayout *fl_albums;

    QString genre_name;
    bool flagReqMore = false;
    int totalCount = 0;
    int offset = 0;

    int drawCount = 0;
};

#endif // MUSICLIST_ARTISTALBUM_H
