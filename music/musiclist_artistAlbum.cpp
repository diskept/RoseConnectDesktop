#include "musiclist_artistAlbum.h"
#include "common/global.h"
#include "proc_uploadthumbnail.h"

#include <QJsonObject>
#include <QVBoxLayout>
#include <QPainter>
#include <QJsonArray>
#include <QScrollArea>
#include <common/gscommon.h>
#include <common/networkhttp.h>
#include <common/sqlitehelper.h>
#include <widget/trackinfo.h>
#include <widget/framemusicalbum.h>

const int ARTISTIMG_SIZE = 300;


MusicList_ArtistAlbum::MusicList_ArtistAlbum(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();
}

MusicList_ArtistAlbum::~MusicList_ArtistAlbum(){

}

void MusicList_ArtistAlbum::setInit(){

}

void MusicList_ArtistAlbum::setUIControl(){

    this->fl_albums = new FlowLayout();
    this->fl_albums->setContentsMargins(0,0,0,0);
    this->fl_albums->setSpacing(0);
    this->fl_albums->setSpacingHV(18, 20);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(38,10,28,40);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addLayout(fl_albums, 9);

    // 스크롤
    QWidget *widget_scrollArea = new QWidget();
    widget_scrollArea->setLayout(vl_total);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(widget_scrollArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("#scrollArea { border:0px; }");
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout *vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);

    this->setLayout(vl_scroll);

}

void MusicList_ArtistAlbum::requestInitData(QJsonObject jsonObj){

    // 초기화
    totalCount = 0;

    this->artist_name = ProcJsonEasy::getString(jsonObj, "artist");

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = " SELECT count(AI.album) AS album_count ";
        strQuery += " FROM album_info AS AI ";
        strQuery += " LEFT OUTER JOIN album_art AS AA ";
        strQuery += " ON AI._id=AA.album_id ";
        if(this->artist_name.front() == "'" || this->artist_name.back() == "'" || this->artist_name.contains("'")){
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist=\"%1\")";
        }
        else{
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist='%1') ";
        }

        QVariantList data_cnt;
        sqlite->exec(strQuery.arg(this->artist_name), data_cnt);

        if(data_cnt.size() > 0){
            QJsonObject tmp_data = data_cnt.at(0).toJsonObject();
            this->totalCount = tmp_data["album_count"].toInt();
        }
    }

    sqlite->close();
    delete sqlite;

    requestAlbums();
}

void MusicList_ArtistAlbum::requestAlbums(){
    // 초기화
    GSCommon::clearLayout(this->fl_albums);

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery =  " SELECT AI.album, AI._id AS album_id, AI.artist, AI.artist_id, AI.numsongs AS num_of_song, AA._data AS album_art ";
        strQuery += " FROM album_info AS AI ";
        strQuery += " LEFT OUTER JOIN album_art AS AA ";
        strQuery += " ON AI._id=AA.album_id ";
        //strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist='%1')";
        if(this->artist_name.front() == "'" || this->artist_name.back() == "'" || this->artist_name.contains("'")){
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist=\"%1\") ";
        }
        else{
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist='%1') ";
        }
        strQuery += QString(" ORDER BY AI.album LIMIT 100 OFFSET %1 ").arg(this->offset);

        QVariantList data;
        sqlite->exec(strQuery.arg(this->artist_name), data);

        if(data.size() > 0){
            for(int i = 0 ; i < data.size(); i++){
                QJsonObject tmp_data = data.at(i).toJsonObject();

                FrameMusicAlbum *tmp_album = new FrameMusicAlbum(this);
                tmp_album->setHover();
                tmp_album->setData(tmp_data);

                this->fl_albums->addWidget(tmp_album);
            }
        }
    }
    sqlite->close();
    delete sqlite;
}

void MusicList_ArtistAlbum::wheelEvent(QWheelEvent *event){

    // 더 가져오기 실행
    if(event->delta() < 0){
        if(!flagReqMore && fl_albums->count() < totalCount){
            if(fl_albums->count() + 100 < totalCount){
                flagReqMore = false;
            }
            else if(fl_albums->count() + 100 > totalCount){
                flagReqMore = true;
            }

            this->offset += 100;
            requestAlbums();
        }
    }
}
