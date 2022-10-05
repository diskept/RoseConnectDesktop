#ifndef MUSICLIST_ARTISTALBUM_H
#define MUSICLIST_ARTISTALBUM_H

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include <data/datapopup.h>

#include <common/filedownloader.h>
#include <common/linker.h>

#include <widget/flowlayout.h>
#include "widget/optionpopup.h"
#include "proc_addtackinplaylist.h"

class MusicList_ArtistAlbum : public QWidget{
    Q_OBJECT

public:
    explicit MusicList_ArtistAlbum(QWidget *parent = nullptr);
    ~MusicList_ArtistAlbum();

    void requestInitData(QJsonObject jsonObj);

private:
    void setInit();
    void setUIControl();

    void requestAlbums();

    void wheelEvent(QWheelEvent *event) override;

private:
    FlowLayout *fl_albums;

    QString artist_name;
    bool flagReqMore = false;
    int totalCount = 0;
    int offset = 0;
};

#endif // MUSICLIST_ARTISTALBUM_H
