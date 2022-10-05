#include "musiclist_genreAlbum.h"
#include "common/global.h"
#include "proc_uploadthumbnail.h"

#include <QJsonObject>
#include <QVBoxLayout>
#include <QPainter>
#include <QJsonArray>
#include <QScrollArea>
#include <QScrollBar>
#include <common/gscommon.h>
#include <common/networkhttp.h>
#include <common/sqlitehelper.h>
#include <widget/trackinfo.h>
#include <widget/framemusicalbum.h>

const int ARTISTIMG_SIZE = 300;


MusicList_GenreAlbum::MusicList_GenreAlbum(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();
}

MusicList_GenreAlbum::~MusicList_GenreAlbum(){

    this->deleteLater();
}

void MusicList_GenreAlbum::setInit(){

}

void MusicList_GenreAlbum::setUIControl(){
print_debug();
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

    this->scrollArea = new QScrollArea();
    this->scrollArea->setObjectName("scrollArea");
    this->scrollArea->setWidget(widget_scrollArea);
    this->scrollArea->setWidgetResizable(true);
    this->scrollArea->setStyleSheet("#scrollArea { border:0px; }");
    this->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QVBoxLayout *vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);

    this->setLayout(vl_scroll);

}

void MusicList_GenreAlbum::requestInitData(QJsonObject jsonObj){

    // 초기화
    this->totalCount = 0;
    this->drawCount = 0;
    QJsonDocument doc(jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " MusicList_GenreAlbum::requestInitData---jsonData_obj = " << strJson << "\n";

    this->genre_name = ProcJsonEasy::getString(jsonObj, "genre");

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = " SELECT count(AI.album) AS album_count ";
        strQuery += " FROM album_info AS AI ";
        strQuery += " LEFT OUTER JOIN album_art AS AA ";
        strQuery += " ON AI._id=AA.album_id ";
        if(this->genre_name.front() == "'" || this->genre_name.back() == "'" || this->genre_name.contains("'")){
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE genre_name=\"%1\")";
        }
        else{
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE genre_name='%1') ";
        }

        QVariantList data_cnt;
        sqlite->exec(strQuery.arg(this->genre_name), data_cnt);
print_debug();
qDebug() << "data_cnt.size()= " << data_cnt.size();
        if(data_cnt.size() > 0){
            QJsonObject tmp_data = data_cnt.at(0).toJsonObject();
            this->totalCount = tmp_data["album_count"].toInt();
        }
    }

    sqlite->close();
    delete sqlite;

    requestAlbums();
}

void MusicList_GenreAlbum::requestAlbums(){
    // 초기화
    GSCommon::clearLayout(this->fl_albums);

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery =  " SELECT AI.album, AI._id AS album_id, AI.genre_name, AI.genre_id, AI.numsongs AS num_of_song, AA._data AS album_art ";
        strQuery += " FROM album_info AS AI ";
        strQuery += " LEFT OUTER JOIN album_art AS AA ";
        strQuery += " ON AI._id=AA.album_id ";
        //strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist='%1')";
        if(this->genre_name.front() == "'" || this->genre_name.back() == "'" || this->genre_name.contains("'")){
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE genre_name=\"%1\") ";
        }
        else{
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE genre_name='%1') ";
        }
        strQuery += QString(" ORDER BY AI.album LIMIT 100 OFFSET %1 ").arg(this->offset);

        QVariantList data;
        sqlite->exec(strQuery.arg(this->genre_name), data);
        print_debug();
        qDebug() << "data.size()= " << data.size();
        if(data.size() > 0){
            this->drawCount += data.size();
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

    this->flagReqMore = false;
}

void MusicList_GenreAlbum::wheelEvent(QWheelEvent *event){

    // 더 가져오기 실행
    if(event->delta() < 0){
        if(!flagReqMore && this->drawCount < this->totalCount && this->scrollArea->verticalScrollBar()->value() == this->scrollArea->verticalScrollBar()->maximum()){
            /*if(fl_albums->count() + 100 < totalCount){
                flagReqMore = false;
            }
            else if(fl_albums->count() + 100 > totalCount){
                flagReqMore = true;
            }*/

            this->offset += 100;
            this->flagReqMore = true;

            requestAlbums();
        }
    }
}
