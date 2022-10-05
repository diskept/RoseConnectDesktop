#include "musiclist_artistTrack.h"
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
#include "common/rosesettings.h"
#include <QSettings>

const int ARTISTIMG_SIZE = 300;
const int HTTP_PLAY_ALL = 99;
const int HTTP_PLAY_SHUFFLE = 98;


MusicList_ArtistTrack::MusicList_ArtistTrack(QWidget *parent)
    : QWidget(parent)
{

    linker = Linker::getInstance();
    setInit();
    setUIControl();
}


MusicList_ArtistTrack::~MusicList_ArtistTrack()
{

    this->deleteLater();
}

void MusicList_ArtistTrack::setInit(){

    proc_add = new Proc_AddTackInPlayList(this);
    connect(proc_add, SIGNAL(signal_playlistClicked(int)), this, SLOT(slot_playlistClicked(int)));
    connect(proc_add, SIGNAL(signal_newPlaylistNameEdited(QString)), this, SLOT(slot_newPlaylistNameEdited(QString)));

    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, SIGNAL(downloaded()), this, SLOT(slot_loadImage()));
}


void MusicList_ArtistTrack::setUIControl(){

    QPixmap pixmap_play(":/images/icon_list_allplay.png");
    QPushButton *btn_play = new QPushButton;
    btn_play->setIcon(pixmap_play);
    btn_play->setIconSize(pixmap_play.rect().size());
    btn_play->setText(tr("Play All"));
    //btn_play->setText(tr("전체재생"));
    btn_play->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    btn_play->setCursor(Qt::PointingHandCursor);


    QPixmap pixmap_shuffle(":/images/icon_list_shuffle.png");
    QPushButton *btn_shuffle = new QPushButton;
    btn_shuffle->setIcon(pixmap_shuffle);
    btn_shuffle->setIconSize(pixmap_play.rect().size());
    btn_shuffle->setText(tr("Shuffle Play"));
    //btn_shuffle->setText(tr("셔플재생"));
    btn_shuffle->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    btn_shuffle->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *box_option = new QHBoxLayout;
    box_option->setAlignment(Qt::AlignLeft);
    box_option->setSpacing(30);
    box_option->setContentsMargins(0,10,0,10);
    box_option->addWidget(btn_play);
    box_option->addWidget(btn_shuffle);


    this->vl_tracks = new QVBoxLayout();
    this->vl_tracks->setContentsMargins(0,0,0,0);
    this->vl_tracks->setSpacing(0);
    this->vl_tracks->setAlignment(Qt::AlignTop);
    GSCommon::clearLayout(this->vl_tracks);

    QWidget *widget_scrollArea = new QWidget();
    widget_scrollArea->setLayout(vl_tracks);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(widget_scrollArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("#scrollArea { border:0px; }");
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);


    QVBoxLayout *boxContents = new QVBoxLayout();
    boxContents->setSpacing(0);
    boxContents->setContentsMargins(33,0,28,0);
    boxContents->addLayout(box_option);
    //boxContents->addWidget(listWidget);
    boxContents->addWidget(scrollArea);


    QVBoxLayout *box = new QVBoxLayout;
    box->setSpacing(0);
    box->setContentsMargins(0,0,0,0);
    box->setAlignment(Qt::AlignTop);
    box->addLayout(boxContents);

    setLayout(box);

    // 커넥트
    connect(btn_play, SIGNAL(clicked()), this, SLOT(slot_clickedPlayAll()));
    connect(btn_shuffle, SIGNAL(clicked()), this, SLOT(slot_clickedPlayShuffle()));
}


void MusicList_ArtistTrack::requestInitData(QJsonObject jsonObj){

    //listPopup->clear();
    this->list_audioInfo.clear();
    totalCount = 0;

    this->artist_name = ProcJsonEasy::getString(jsonObj, "artist");

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = " SELECT count(A.album) AS track_count ";
        strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
        if(this->artist_name.front() == "'" || this->artist_name.back() == "'" || this->artist_name.contains("'")){
            strQuery += " WHERE A.artist_id=(SELECT artist_id from artists where artist=\"%1\") ORDER BY A.track ";
        }
        else{
            strQuery += " WHERE A.artist_id=(SELECT artist_id from artists where artist='%1') ORDER BY A.track ";
        }

        QVariantList data_cnt;
        sqlite->exec(strQuery.arg(this->artist_name), data_cnt);

        if(data_cnt.size() > 0){
            QJsonObject tmp_data = data_cnt.at(0).toJsonObject();
            this->totalCount = tmp_data["track_count"].toInt();
        }
    }

    sqlite->close();
    delete sqlite;

    requestTracks();
}

void MusicList_ArtistTrack::requestTracks(){

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        //strQuery = " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
        strQuery = " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art , A.mime_type, A.samplerate, A.bitdepth ";
        strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";

        if(this->artist_name.front() == "'" || this->artist_name.back() == "'" || this->artist_name.contains("'")){
            strQuery += " WHERE A.artist_id=(SELECT artist_id from artists where artist= \"%1\") ORDER BY A.track ";
        }
        else{
            strQuery += " WHERE A.artist_id=(SELECT artist_id from artists where artist='%1') ORDER BY A.track ";
        }
        strQuery += QString("LIMIT 100 OFFSET %1 ").arg(this->offset);

        QVariantList data;
        sqlite->exec(strQuery.arg(this->artist_name), data);

        this->list_audioInfo.clear();
        GSCommon::clearLayout(this->vl_tracks);
        if(data.size() > 0){
            for(int i = 0 ; i < data.size(); i++){
                QJsonObject tmp_data = data.at(i).toJsonObject();

                QString tmp_split = "";
                QStringList splitToken;
                QString tmp_type = "";

                QString tmp_mime = "";
                int tmp_sample = 0;
                int tmp_bit = 0;
                QString snd_mime = "";

                tmp_sample = tmp_data["samplerate"].toInt();
                tmp_bit = tmp_data["bitdepth"].toInt();

                tmp_split = tmp_data["mime_type"].toString();
                splitToken = tmp_split.split("/");

                if(splitToken.size() > 1){
                    tmp_type = splitToken.at(1);

                    if(tmp_type == "x-wav" || tmp_type == "wav"){
                        tmp_mime = "WAV";
                    }
                    else if (tmp_type == "wv") {
                        tmp_mime = "WV";
                    }
                    else if(tmp_type == "flac"){
                        tmp_mime = "FLAC";
                    }
                    else if(tmp_type == "dff" || tmp_type == "dsf"){
                        tmp_mime = "DSD";
                    }
                    else if(tmp_type == "mp4"){
                        tmp_mime = "ALAC";
                    }
                    else if(tmp_type == "mpeg"){
                        tmp_mime = "MP3";
                    }
                    else if(tmp_type == "amr"){
                        tmp_mime = "AMR";
                    }
                    else if(tmp_type == "amr-wb"){
                        tmp_mime = "AWB";
                    }
                    else if(tmp_type == "x-ms-wma"){
                        tmp_mime = "WMA";
                    }
                    else if(tmp_type == "ape"){
                        tmp_mime = "APE";
                    }
                    else if(tmp_type == "ogg"){
                        tmp_mime = "OGG";
                    }
                    else if(tmp_type == "aac" || tmp_type == "aac-adts"){
                        tmp_mime = "AAC";
                    }
                    else if(tmp_type == "x-matroska"){
                        tmp_mime = "MKA";
                    }
                    else if(tmp_type == "midi"){
                        tmp_mime = "MIDI";
                    }
                    else if(tmp_type == "sp-midi"){
                        tmp_mime = "SMF";
                    }
                    else if(tmp_type == "imelody"){
                        tmp_mime = "IMY";
                    }
                    else if(tmp_type == "x-mpegurl"){
                        tmp_mime = "M3U8";
                    }
                    else if(tmp_type == "x-scpls"){
                        tmp_mime = "PLS";
                    }
                    else if(tmp_type == "aiff"){
                        tmp_mime = "AIFF";
                    }
                }

                if(tmp_mime == "MP3" || tmp_mime == "AMR" || tmp_mime == "AWB" || tmp_mime == "WMA" || tmp_mime == "APE" || tmp_mime == "OGG" || tmp_mime == "AAC" || tmp_mime == "MKA"
                        || tmp_mime == "MIDI" || tmp_mime == "SMF" || tmp_mime == "IMY" || tmp_mime == "M3U8" || tmp_mime == "PLS" || tmp_mime == "AIFF" || tmp_mime == "WV"){

                    snd_mime = tmp_mime;
                }
                else if(tmp_mime == "DSD"){
                    if(tmp_sample == 88200 || tmp_sample == 2822400){
                        snd_mime = tmp_mime + " 64";
                    }
                    else if(tmp_sample == 176400 || tmp_sample == 5644800){
                        snd_mime = tmp_mime + " 128";
                    }
                    else if(tmp_sample == 352800 || tmp_sample == 11289600){
                        snd_mime = tmp_mime + " 256";
                    }
                    else if(tmp_sample == 705600 || tmp_sample == 22579200){
                        snd_mime = tmp_mime + " 512";
                    }
                    else if(tmp_sample == 1411200 || tmp_sample == 45158400){
                        snd_mime = tmp_mime + " 1024";
                    }
                    else{
                        snd_mime = tmp_mime;
                    }
                }
                else if(tmp_mime == "WAV" || tmp_mime == "FLAC" || tmp_mime == "ALAC"){
                    if(tmp_bit == 0 || tmp_bit == 16){
                        snd_mime = tmp_mime;
                    }
                    else if(tmp_bit > 16){
                        snd_mime = tmp_mime + QString(" %1").arg(tmp_bit);
                    }
                }
                tmp_data.insert("album_mime", snd_mime);

                DataPopup *tmp_dataPopup = new DataPopup();
                tmp_dataPopup->setJsonData(tmp_data);
                this->list_audioInfo.append(tmp_dataPopup);

                QString tmp_albumImg = "";
                if(tmp_data.contains("album_art")){
                    tmp_albumImg = tmp_data["album_art"].toString();
                }
                TrackInfo *tmp_trackInfo = new TrackInfo(OptionPopup::TypeMenu::Music_Etc_ArtistDetailTrack);
                tmp_trackInfo->setProperty("index", i);
                tmp_trackInfo->setDataTrackInfo(this->list_audioInfo.at(i), tmp_albumImg);

                connect(tmp_trackInfo, SIGNAL(signal_clickedAddPlaylist()), this, SLOT(slot_playlistClickedTrack()));

                this->vl_tracks->addWidget(tmp_trackInfo);
            }
        }
    }

    sqlite->close();
    delete sqlite;
}

void MusicList_ArtistTrack::slot_loadImage()
{
    bool flagLoad = this->pixmap_albumImg->loadFromData(this->fileDownLoader->downloadedData());

    if(flagLoad){
        QPixmap tmp_pixmap = this->pixmap_albumImg->scaled(ARTISTIMG_SIZE, ARTISTIMG_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // 빈 Pixmap
        QPixmap pixmap_painter = QPixmap(QSize(ARTISTIMG_SIZE,ARTISTIMG_SIZE));
        pixmap_painter.fill(Qt::transparent);

        QPainter painter (&pixmap_painter);
        painter.setRenderHint(QPainter::Antialiasing, true);
        QBrush brush = QBrush(tmp_pixmap);
        painter.setBrush(brush);
        painter.drawRoundedRect(0, 0, ARTISTIMG_SIZE, ARTISTIMG_SIZE, 150, 150);    // border-radius:4px
        this->lb_albumImg->setPixmap(pixmap_painter);
    }

    this->fileDownLoader->deleteLater();
}

void MusicList_ArtistTrack::slot_clickedPlayAll(){
    QJsonArray array_music;
    for(int i=0; i<this->list_audioInfo.count(); i++){
        array_music.append(this->list_audioInfo.at(i)->getJsonData());
    }
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject tmp_json;
    tmp_json.insert("music", array_music);
    tmp_json.insert("musicPlayType", 15);
    tmp_json.insert("currentPosition", 0);
    tmp_json.insert("shuffle", 0);
    tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

    if(global.Queue_track_count != 0) {
        print_debug();emit linker->signal_checkQueue(27, "");

        return;
    }
    print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
    global.Queue_track_count += array_music.count();     // 220419 queue count

    network->request(HTTP_PLAY_ALL, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);

}

void MusicList_ArtistTrack::slot_clickedPlayShuffle(){

    QJsonArray array_music;
    for(int i=0; i<this->list_audioInfo.count(); i++){
        array_music.append(this->list_audioInfo.at(i)->getJsonData());
    }
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject tmp_json;
    tmp_json.insert("music", array_music);
    tmp_json.insert("musicPlayType", 15);
    tmp_json.insert("currentPosition", 0);
    tmp_json.insert("shuffle", 1);
    tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

    if(global.Queue_track_count != 0) {
        print_debug();emit linker->signal_checkQueue(27, "");

        return;
    }
    print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
    global.Queue_track_count += array_music.count();     // 220419 queue count

    network->request(HTTP_PLAY_SHUFFLE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
}

QJsonArray MusicList_ArtistTrack::getJsonArr_TrackData(const int &p_index){
    QJsonArray jsonArray;

    if(p_index<0){
        for(int i= 0 ; i < list_audioInfo.size(); i++){
            jsonArray.append(getJsonObject_TrackData(i));
        }
    }else{
        jsonArray.append(getJsonObject_TrackData(p_index));
    }



    return jsonArray;
}

void MusicList_ArtistTrack::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    Q_UNUSED(p_id);
    Q_UNUSED(p_jsonObject);

    sender()->deleteLater();
}

QJsonObject MusicList_ArtistTrack::getJsonObject_TrackData(const int &p_index){

    QJsonObject jsonTrack;

    if(p_index >=0 && p_index < list_audioInfo.size()){
        DataPopup *dataPopup = list_audioInfo.at(p_index);

        // 앨범
        jsonTrack.insert("album", dataPopup->getAlbum());

        // 아티스트
        QJsonObject jsonArtist;
        jsonArtist.insert("name", dataPopup->getArtist());
        QJsonArray jsonArrArtists;
        jsonArrArtists.append(jsonArtist);
        jsonTrack.insert("artists", jsonArrArtists);

        // 클라이언트 아이디 = 오디오 아이디
        jsonTrack.insert("client_id", dataPopup->getId());

        // 썸네일
        QJsonObject jsonThumb;
        jsonThumb.insert("url", albumImg);

        QJsonArray jsonArrThumbs;
        jsonArrThumbs.append(jsonThumb);
        jsonTrack.insert("thumbnails", jsonArrThumbs);

        // 타이틀
        jsonTrack.insert("title", dataPopup->getTitle());

        // 재생 URL
        jsonTrack.insert("url", dataPopup->getData());

        jsonTrack.insert("isFavorite", false);
        jsonTrack.insert("isSelected", false);
        jsonTrack.insert("mac_address", global.device.getDeviceID());
        jsonTrack.insert("type", "music");
    }


    return jsonTrack;
}

void MusicList_ArtistTrack::slot_playlistClicked(const int &p_playlistno){

    int tmp_index = sender()->property("index").toInt();

    if(p_playlistno >0){
        proc_add->requestAddPlayListTrack(p_playlistno, getJsonArr_TrackData(tmp_index));

        // ------------------------------------------------
        //  썸네일 업로드
        // ------------------------------------------------
        if(tmp_index >=0 && tmp_index < list_audioInfo.size()){
            QString imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(list_audioInfo.at(tmp_index)->getAlbum_art());
            Proc_UploadThumbnail *proc_uploadThumb = new Proc_UploadThumbnail(this);
            proc_uploadThumb->uploadThumnail(imgPath, list_audioInfo.at(tmp_index)->getData());
            connect(proc_uploadThumb, SIGNAL(signal_uploadedThumbnail()), proc_uploadThumb, SLOT(deleteLater()));
        }

    }else{
        proc_add->showDlgOfNewPlayList();
    }


}

void MusicList_ArtistTrack::slot_newPlaylistNameEdited(QString p_name){

    int tmp_index = sender()->property("index").toInt();

    QString tmp_playUrl;
    if(tmp_index <=0){
        tmp_playUrl = list_audioInfo.at(0)->getData();
    }else{
        tmp_playUrl = list_audioInfo.at(tmp_index)->getData();
    }

    proc_add->requestNewPlaylistTrack(*pixmap_albumImg, tmp_playUrl, p_name, getJsonArr_TrackData(tmp_index));
}

void MusicList_ArtistTrack::slot_playlistClickedTrack(){

    int tmp_index = sender()->property("index").toInt();
    proc_add->setProperty("index",tmp_index);
    proc_add->showDlgOfPlayList();
}

void MusicList_ArtistTrack::wheelEvent(QWheelEvent *event){

    // 더 가져오기 실행
    if(event->delta() < 0){
        if(!flagReqMore && vl_tracks->count() < totalCount){
            if(vl_tracks->count() + 100 < totalCount){
                flagReqMore = false;
            }
            else if(vl_tracks->count() + 100 > totalCount){
                flagReqMore = true;
            }

            this->offset += 100;
            requestTracks();
        }
    }
}

