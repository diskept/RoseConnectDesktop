#include "music/MusicSearchMain.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/networkhttp.h"
#include "common/global.h"
#include "common/gscommon.h"
#include "common/rosesettings.h"
#include "common/sqlitehelper.h"

#include "widget/NoData_Widget.h"
#include "widget/optionpopup.h"

#include <QScroller>


namespace music {

    const QString tmp_btnStyle      = "padding:10px;border:1px solid #707070;color:#CCCCCC;font-size:18px;";//cheon211008
    //const QString tmp_btnStyleHover = "background-color:#B18658;color:#FFFFFF;";//cheon211115-01
    const QString tmp_btnStyleHover = "background-color:#CCCCCC;color:#FFFFFF;";//cheon211115-01
    const QString tmp_btnStyleHover_selected = "background-color:#B18658;color:#FFFFFF;";//cheon211115-01


    const QString SORT_STEP_MUSIC = "music";//cheon211008
    const QString SORT_STEP_RoseTube = "rosetube";//cheon211008
    const QString SORT_STEP_RoseRadio = "roseradio";//cheon211008
    const QString SORT_STEP_TIDAL = "tidal";//cheon211008
    const QString SORT_STEP_BUGS = "bugs";//cheon211008
    const QString SORT_STEP_QOBUZ = "qobuz";//cheon211008

    // MUSIC subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX_SUBTITLE_ARTIST = 0;
    const int BTN_IDX_SUBTITLE_ALBUM = 1;
    const int BTN_IDX_SUBTITLE_TRACK = 2;

    // MUSIC section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___ARTIST = 0;
    const int SECTION_FOR_MORE_POPUP___ALBUM = 1;
    const int SECTION_FOR_MORE_POPUP___TRACK = 2;

    const int HTTP_NETWORK_PLAY = 99;


    MusicSearch::MusicSearch(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_filter, parent) {

        // 기본 UI 세팅
        this->setUIControl_basic();

        this->list_search_album = new QList<roseHome::AlbumItemData>();
        this->list_search_artist = new QList<roseHome::ArtistItemData>();
        this->list_search_track = new QList<roseHome::TrackItemData>();
    }


    MusicSearch::~MusicSearch(){

        this->deleteLater();
    }


    void MusicSearch::setJsonObject_forData(const QJsonObject& jsonObj){

        common::SearchData tmp_data = common::ConvertData_common::convertData_searchData(jsonObj);
        this->flagNeedReload = false;

        if(tmp_data.search_word != this->data_search.search_word){
            this->flagNeedReload = true;

            this->data_search = common::ConvertData_common::convertData_searchData(jsonObj);

            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + tr("Search result"));

            this->list_search_album->clear();
            this->list_search_artist->clear();
            this->list_search_track->clear();

            this->search_track_count = 0;

            this->jsonArr_artists = QJsonArray();
            this->jsonArr_albums = QJsonArray();
            this->jsonArr_tracks = QJsonArray();
            this->jsonArr_tracks_toPlay = QJsonArray();
        }
    }


    void MusicSearch::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            if(this->flag_artist == true || this->flag_album == true || this->flag_track == true){
                GSCommon::clearLayout(this->vBox_search_artist);
                GSCommon::clearLayout(this->vBox_search_album);
                GSCommon::clearLayout(this->vBox_search_track);
                GSCommon::clearLayout(this->box_search_contents);
                this->box_contents->removeWidget(this->widget_search_contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);
            }

            this->flag_artist = false;
            this->flag_album = false;
            this->flag_track = false;

            this->box_search_contents = new QVBoxLayout();
            this->box_search_contents->setSpacing(0);
            this->box_search_contents->setContentsMargins(0, 0, 0, 0);

            GSCommon::clearLayout(this->box_search_contents);

            this->widget_search_contents = new QWidget();
            this->widget_search_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_search_contents->setLayout(this->box_search_contents);

            this->box_contents->addWidget(this->widget_search_contents, 0, Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // sub Title UI
            for(int i = 0; i < 5; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            // init UI
            this->vBox_search_artist = new QVBoxLayout();
            this->vBox_search_album = new QVBoxLayout();
            this->vBox_search_track = new QVBoxLayout();

            GSCommon::clearLayout(this->vBox_search_artist);
            GSCommon::clearLayout(this->vBox_search_album);
            GSCommon::clearLayout(this->vBox_search_track);

            this->hBox_search_artist = new QHBoxLayout();
            this->hBox_search_album = new QHBoxLayout();

            GSCommon::clearLayout(this->hBox_search_artist);
            GSCommon::clearLayout(this->hBox_search_album);

            for(int i = 0; i < 15; i++){
                this->search_artist[i] = new roseHome::ItemArtist_roseHome(i, BTN_IDX_SUBTITLE_ARTIST, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->search_artist[i], &roseHome::ItemArtist_roseHome::signal_clicked, this, &MusicSearch::slot_clickedItemArtist);
            }

            for(int i = 0; i < 15; i++){
                this->search_album[i] = new roseHome::ItemAlbum_rosehome(i, BTN_IDX_SUBTITLE_ALBUM, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                connect(this->search_album[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &MusicSearch::slot_clickedItemAlbum);
            }

            for (int i = 0; i < 5; i++) {
                this->search_track[i] = new PlaylistTrackDetailInfo_RHV();
                connect(this->search_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &MusicSearch::slot_clickedItemTrack_inList);

                this->search_track[i]->setProperty("index", i);
                this->search_track[i]->setObjectName("search_tracks");
            }

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->setUIControl_requestArtist();
            this->setUIControl_requestAlbum();
            this->setUIControl_requestTrack();
        }
    }


    /**
     * @brief 기본화면 구성
     */
    void MusicSearch::setUIControl_basic(){//cheon211008-search-----------------------------

        //start 상단 step 버튼//cheon211008-search-----------------------------
        global.btn_Music= new QPushButton(tr("Music"));
        global.btn_Music->setObjectName("btn_Music");
        global.btn_Music->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_MUSIC);

        global.btn_RoseTube = new QPushButton(tr("RoseTube"));
        global.btn_RoseTube->setObjectName("btn_RoseTube");
        global.btn_RoseTube->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_RoseTube);

        global.btn_RoseRadio= new QPushButton(tr("RoseRadio"));
        global.btn_RoseRadio->setObjectName("btn_RoseRadio");
        global.btn_RoseRadio->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_RoseRadio);

        global.btn_Tidal = new QPushButton(tr("Tidal"));
        global.btn_Tidal->setObjectName("btn_Tidal");
        global.btn_Tidal->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_TIDAL);

        global.btn_Bugs= new QPushButton(tr("Bugs"));
        global.btn_Bugs->setObjectName("btn_Bugs");
        global.btn_Bugs->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_BUGS);

        global.btn_Qobuz = new QPushButton(tr("Qobuz"));
        global.btn_Qobuz->setObjectName("btn_Qobuz");
        global.btn_Qobuz->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_QOBUZ);

        global.btn_Music->setFixedSize(125,40);
        global.btn_RoseTube->setFixedSize(125,40);
        global.btn_RoseRadio->setFixedSize(125,40);
        global.btn_Tidal->setFixedSize(125,40);
        global.btn_Bugs->setFixedSize(125,40);
        global.btn_Qobuz->setFixedSize(125,40);

        global.btn_Music->setCursor(Qt::PointingHandCursor);
        global.btn_RoseTube->setCursor(Qt::PointingHandCursor);
        global.btn_RoseRadio->setCursor(Qt::PointingHandCursor);
        global.btn_Tidal->setCursor(Qt::PointingHandCursor);
        global.btn_Bugs->setCursor(Qt::PointingHandCursor);
        global.btn_Qobuz->setCursor(Qt::PointingHandCursor);

        //end  상단 step 버튼 //--------------------------------------------------------------
        global.btn_Music->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Music").arg(tmp_btnStyle + tmp_btnStyleHover_selected).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover_selected));
        global.btn_RoseRadio->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_RoseRadio").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_RoseTube->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_RoseTube").arg(tmp_btnStyle).arg(tmp_btnStyleHover));//cheon211115-01
        global.btn_Tidal->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_Tidal").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_Bugs->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_Bugs").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_Qobuz->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Qobuz").arg(tmp_btnStyle).arg("border-top-right-radius:20px;border-bottom-right-radius:20px;").arg(tmp_btnStyleHover));

        // Main Title
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ", global.btn_Music, global.btn_RoseRadio, global.btn_RoseTube, global.btn_Tidal,  global.btn_Bugs, global.btn_Qobuz );//cheon211008-search
    }


    void MusicSearch::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

        if(p_id == HTTP_NETWORK_PLAY){
            QJsonDocument doc(p_jsonObject);
            QString strJson(doc.toJson(QJsonDocument::Compact));
            //qDebug() << "jsonObj = " << strJson;
        }
    }


    void MusicSearch::setUIControl_requestArtist(){

        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){

            QString strQuery = "";
            strQuery = " SELECT AI._id AS id, AI.artist, AI.number_of_albums, AI.number_of_tracks, ";
            strQuery += " (SELECT _data FROM album_art WHERE album_id=M.album_id) AS album_art";
            strQuery += " FROM artist_info AS AI INNER JOIN artists_albums_map AS M ON AI._id=M.artist_id ";
            strQuery += " WHERE AI.artist LIKE '%" + this->data_search.search_word + "%'";
            strQuery += " GROUP BY AI._id ";

            QVariantList data;
            sqlite->exec(strQuery, data);

            if(data.size() > 0){
                for(int i = 0 ; i < data.size(); i++){
                    this->jsonArr_artists.append(data.at(i).toJsonObject());

                    QMap<QString, QVariant> map = data.at(i).toMap();

                    QString tmpImg = map["album_art"].toString();
                    if(!tmpImg.isEmpty()){
                        tmpImg = "http://" + global.device.getDeviceIP() + ":" + global.port_img + map["album_art"].toString();
                    }

                    roseHome::ArtistItemData data_output;
                    data_output.id = map["id"].toInt();
                    data_output.imageUrl = tmpImg;
                    data_output.name = map["artist"].toString();
                    data_output.albums_count = map["number_of_albums"].toInt();
                    data_output.tracks_count = map["number_of_tracks"].toInt();
                    data_output.artist_total_cnt = data.size();

                    this->list_search_artist->append(data_output);
                }
            }
        }

        sqlite->close();
        delete sqlite;

        this->flag_artist = true;
        this->setUIControl_appendWidget();
    }


    void MusicSearch::setUIControl_requestAlbum(){

        // DB 쿼리
        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art  ";
            strQuery += " FROM album_info AS AI ";
            strQuery += " LEFT OUTER JOIN album_art AS AA ";
            strQuery += " ON AI._id=AA.album_id ";
            strQuery += " WHERE AI.album LIKE '%" + this->data_search.search_word + "%' OR AI.artist LIKE '%" + this->data_search.search_word + "%'";
            strQuery += " ORDER BY AI.album";

            QVariantList data;
            sqlite->exec(strQuery, data);

            if(data.size() > 0){
                for(int i = 0 ; i < data.size(); i++){
                    this->jsonArr_albums.append(data.at(i).toJsonObject());

                    QMap<QString, QVariant> map = data.at(i).toMap();

                    QString tmpMime = map["mime_type"].toString();
                    int tmpbitDepth = map["bitdepth"].toInt();
                    int tmpsampleRate = map["samplerate"].toInt();

                    if(!tmpMime.isEmpty()){
                        QString tmp_data = tmpMime;
                        QStringList splitToken = tmp_data.split("/");

                        if(splitToken.size() > 1){
                            QString tmp_type = splitToken.at(1);
                            if(tmp_type == "x-wav" || tmp_type == "wav"){
                                tmpMime = "WAV";
                            }
                            else if (tmp_type == "wv") {
                                tmpMime = "WV";
                            }
                            else if(tmp_type == "flac"){
                                tmpMime = "FLAC";
                            }
                            else if(tmp_type == "dff" || tmp_type == "dsf"){
                                tmpMime = "DSD";
                            }
                            else if(tmp_type == "mp4"){
                                tmpMime = "ALAC";
                            }
                            else if(tmp_type == "mpeg"){
                                tmpMime = "MP3";
                            }
                            else if(tmp_type == "amr"){
                                tmpMime = "AMR";
                            }
                            else if(tmp_type == "amr-wb"){
                                tmpMime = "AWB";
                            }
                            else if(tmp_type == "x-ms-wma"){
                                tmpMime = "WMA";
                            }
                            else if(tmp_type == "ape"){
                                tmpMime = "APE";
                            }
                            else if(tmp_type == "ogg"){
                                tmpMime = "OGG";
                            }
                            else if(tmp_type == "aac" || tmp_type == "aac-adts"){
                                tmpMime = "AAC";
                            }
                            else if(tmp_type == "x-matroska"){
                                tmpMime = "MKA";
                            }
                            else if(tmp_type == "midi"){
                                tmpMime = "MIDI";
                            }
                            else if(tmp_type == "sp-midi"){
                                tmpMime = "SMF";
                            }
                            else if(tmp_type == "imelody"){
                                tmpMime = "IMY";
                            }
                            else if(tmp_type == "x-mpegurl"){
                                tmpMime = "M3U8";
                            }
                            else if(tmp_type == "x-scpls"){
                                tmpMime = "PLS";
                            }
                            else if(tmp_type == "aiff"){
                                tmpMime = "AIFF";
                            }
                        }
                    }

                    QString mimeType = "";
                    if(tmpMime == "MP3" || tmpMime == "AMR" || tmpMime == "AWB" || tmpMime == "WMA" || tmpMime == "APE" || tmpMime == "OGG" || tmpMime == "AAC" || tmpMime == "MKA"
                            || tmpMime == "MIDI" || tmpMime == "SMF" || tmpMime == "IMY" || tmpMime == "M3U8" || tmpMime == "PLS" || tmpMime == "AIFF" || tmpMime == "WV"){

                        mimeType = tmpMime;
                    }
                    else if(tmpMime == "DSD"){
                        if(tmpsampleRate == 88200 || tmpsampleRate == 2822400){
                            mimeType = tmpMime + " 64";
                        }
                        else if(tmpsampleRate == 176400 || tmpsampleRate == 5644800){
                            mimeType = tmpMime + " 128";
                        }
                        else if(tmpsampleRate == 352800 || tmpsampleRate == 11289600){
                            mimeType = tmpMime + " 256";
                        }
                        else if(tmpsampleRate == 705600 || tmpsampleRate == 22579200){
                            mimeType = tmpMime + " 512";
                        }
                        else if(tmpsampleRate == 1411200 || tmpsampleRate == 45158400){
                            mimeType = tmpMime + " 1024";
                        }
                        else{
                            mimeType = tmpMime;
                        }
                    }
                    else if(tmpMime == "WAV" || tmpMime == "FLAC" || tmpMime == "ALAC"){
                        if(tmpbitDepth == 0 || tmpbitDepth == 16){
                            mimeType = tmpMime;
                        }
                        else if(tmpbitDepth > 16){
                            mimeType = tmpMime + QString(" %1").arg(tmpbitDepth);
                        }
                    }

                    QString tmpImg = map["album_art"].toString();
                    if(!tmpImg.isEmpty()){
                        tmpImg = "http://" + global.device.getDeviceIP() + ":" + global.port_img + map["album_art"].toString();
                    }

                    roseHome::AlbumItemData data_output;
                    data_output.id = map["album_id"].toInt();
                    data_output.trackCount = map["num_of_song"].toInt();
                    data_output.totalCount = data.size();

                    data_output.type = "MUSIC";
                    data_output.title = map["album"].toString();
                    data_output.artist = map["artist"].toString();
                    data_output.playTime = map["album_art"].toString();
                    data_output.thumbnail = tmpImg;
                    data_output.mime = mimeType;

                    this->list_search_album->append(data_output);
                }
            }
        }

        sqlite->close();
        delete sqlite;

        this->flag_album = true;
        this->setUIControl_appendWidget();
    }


    void MusicSearch::setUIControl_requestTrack(){

        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            strQuery += " SELECT A.album, A.album_key, A.album_id, A.artist, A.artist_id, A.artist_key, AI._data AS album_art, A._id AS id, ";
            strQuery += " A.bitdepth, A.composer, A._data AS data, A.duration, A.mime_type, A.samplerate, A.title, A.track ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS AI ON A.album_id=AI.album_id ";
            strQuery += " WHERE artist LIKE '%" + this->data_search.search_word + "%' OR title LIKE '%" + this->data_search.search_word + "%'";
            strQuery += " ORDER BY A.title ";

            QVariantList data;
            sqlite->exec(strQuery, data);

            this->search_track_count = data.size();
            if(data.size() > 0){
                for(int i = 0 ; i < data.size(); i++){

                    QMap<QString, QVariant> map = data.at(i).toMap();

                    QString tmpImg = map["album_art"].toString();
                    if(!tmpImg.isEmpty()){
                        tmpImg = "http://" + global.device.getDeviceIP() + ":" + global.port_img + map["album_art"].toString();
                    }

                    QJsonObject tmpTrack = QJsonObject();
                    tmpTrack.insert("album", map["album"].toString());
                    tmpTrack.insert("album_art", tmpImg);
                    tmpTrack.insert("album_id", map["album_id"].toInt());
                    tmpTrack.insert("album_key", map["album_key"].toString());
                    tmpTrack.insert("artist", map["artist"].toString());
                    tmpTrack.insert("artist_id", map["artist_id"].toString());
                    tmpTrack.insert("artist_key", map["artist_key"].toString());
                    tmpTrack.insert("bitdepth", map["bitdepth"].toInt());
                    tmpTrack.insert("composer", map["composer"].toString());
                    tmpTrack.insert("composercnt", 0);
                    tmpTrack.insert("data", map["data"].toString());
                    tmpTrack.insert("duration", map["duration"].toInt());
                    tmpTrack.insert("favCnt", 0);
                    tmpTrack.insert("gener_song_cnt", 0);
                    tmpTrack.insert("id", map["id"].toInt());
                    tmpTrack.insert("isSelected", false);
                    tmpTrack.insert("mime_type", map["mime_type"].toString());
                    tmpTrack.insert("num_of_song", 0);
                    tmpTrack.insert("num_of_tracks", 0);
                    tmpTrack.insert("samplerate", map["samplerate"].toInt());
                    tmpTrack.insert("star", 0);
                    tmpTrack.insert("title", map["title"].toString());
                    tmpTrack.insert("track_id", map["track"].toInt());

                    this->jsonArr_tracks.append(tmpTrack);

                    if(i < 5){
                        this->jsonArr_tracks_toPlay.append(tmpTrack);
                    }
                }
            }
        }

        sqlite->close();
        delete sqlite;

        this->flag_track = true;
        this->setUIControl_appendWidget();
    }


    void MusicSearch::setUIControl_appendWidget(){

        if(this->flag_artist == true && this->flag_album == true && this->flag_track == true){

            QString artistTitle = tr("Artists");
            QString albumTitle = tr("Albums");
            QString trackTitle = tr("Tracks");

            if(this->list_search_artist->size() > 0){
                this->widget_search_artist = new QWidget();
                this->widget_search_artist = this->setUIControl_subTitle_withSideBtn(artistTitle + QString(" (%1)").arg(this->list_search_artist->at(0).artist_total_cnt), "View All", BTN_IDX_SUBTITLE_ARTIST, this->vBox_search_artist);

                this->vBox_search_artist->addSpacing(10);

                this->hBox_search_artist = this->setUIControl_hBoxLayout_forArtist(this->vBox_search_artist);

                int maxCount = 0;
                if(this->list_search_artist->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_search_artist->size();
                }

                for(int i = 0; i < maxCount; i++){
                    this->search_artist[i]->setData(this->list_search_artist->at(i));
                    this->hBox_search_artist->addWidget(this->search_artist[i]);
                }

                this->box_search_contents->addLayout(this->vBox_search_artist);
                this->box_search_contents->addSpacing(30);
            }

            if(this->list_search_album->size() > 0){
                this->widget_search_album = new QWidget();
                this->widget_search_album = this->setUIControl_subTitle_withSideBtn(albumTitle + QString(" (%1)").arg(this->list_search_album->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_ALBUM, this->vBox_search_album);

                this->vBox_search_album->addSpacing(10);

                this->hBox_search_album = this->setUIControl_hBoxLayout_forAlbum(this->vBox_search_album);

                int maxCount = 0;
                if(this->list_search_album->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_search_album->size();
                }

                for(int i = 0; i < maxCount; i++){
                    this->search_album[i]->setData(this->list_search_album->at(i));
                    this->hBox_search_album->addWidget(this->search_album[i]);
                }

                this->box_search_contents->addLayout(this->vBox_search_album);
                this->box_search_contents->addSpacing(30);
            }

            if(this->jsonArr_tracks_toPlay.size() > 0){
                this->widget_search_track = new QWidget();
                this->widget_search_track = this->setUIControl_subTitle_withSideBtn(trackTitle + QString(" (%1)").arg(this->search_track_count), "View All", BTN_IDX_SUBTITLE_TRACK, this->vBox_search_track);

                this->vBox_search_track->addSpacing(10);

                int maxCount = 0;
                if(this->jsonArr_tracks_toPlay.size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->jsonArr_tracks_toPlay.size();
                }

                for(int i = 0; i < maxCount; i++){
                    QJsonObject tmpData = this->jsonArr_tracks_toPlay.at(i).toObject();
                    tmpData.insert("favorite", false);

                    this->search_track[i]->setDataTrackInfo_Music(tmpData);
                    this->search_track[i]->resize(1550, 70);

                    this->vBox_search_track->addWidget(this->search_track[i]);
                }

                this->box_search_contents->addLayout(this->vBox_search_track);
                this->box_search_contents->addSpacing(30);
            }

            /*if(this->jsonArr_tracks_toPlay.size() > 0){
                this->track_svr_idx = 0;

                QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(this->track_svr_idx).toObject();

                QJsonObject data = QJsonObject();
                data.insert("macAddress", global.device.getDeviceID());
                data.insert("trackId", 0);

                QJsonObject track = QJsonObject();
                track.insert("data", data);
                track.insert("playUrl", ProcJsonEasy::getString(trackInfo, "data"));
                track.insert("type", "MUSIC");

                QJsonArray tracks = QJsonArray();
                tracks.append(track);

                // request HTTP API - track check for Rose Server
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &MusicSearch::slot_applyResult_checkTrackId);
                proc->request_rose_checkRating_Track("MUSIC", tracks);
            }*/

            if(this->list_search_artist->size() <= 0 && this->list_search_album->size() <= 0 && this->jsonArr_tracks_toPlay.size() <= 0){

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::SearchResult_NoData);
                noData_widget->setFixedSize(1500, 700);

                GSCommon::clearLayout(this->box_search_contents);
                this->box_search_contents->addWidget(noData_widget, 0, Qt::AlignTop);
            }

            ContentLoadingwaitingMsgHide();
        }
    }


    QWidget* MusicSearch::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

        // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.
        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        tmp_hBox->setSpacing(0);
        tmp_hBox->setContentsMargins(0, 0, 0, 0);
        tmp_hBox->setAlignment(Qt::AlignTop);

        QWidget *widget_box_subTitle = new QWidget();
        widget_box_subTitle->setContentsMargins(0, 0, 0, 0);
        widget_box_subTitle->setLayout(tmp_hBox);

        this->lb_subTitle[btnId]->setText(subTitle);
        this->lb_subTitle[btnId]->setStyleSheet("font-size:24px;font-weight:bold;color:#FFFFFF;");
        tmp_hBox->addWidget(this->lb_subTitle[btnId], 0, Qt::AlignVCenter);

        // side button - it works when the button text is not empty
        if(btnText.isEmpty() == false){
            this->btnView_all[btnId]->setText(btnText);
            this->btnView_all[btnId]->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
            this->btnView_all[btnId]->setProperty("idx", btnId);
            this->btnView_all[btnId]->setCursor(Qt::PointingHandCursor);
            connect(this->btnView_all[btnId], SIGNAL(clicked()), this, SLOT(slot_clickBtn_subTitle_viewAll()));

            tmp_hBox->addWidget(this->btnView_all[btnId], 1, Qt::AlignVCenter | Qt::AlignRight);
        }

        // Apply Main Layout with spacing
        p_layout->addWidget(widget_box_subTitle);

        return widget_box_subTitle;
    }


    QHBoxLayout* MusicSearch::setUIControl_hBoxLayout_forArtist(QLayout *p_layout){
        //----------------------------------------------------------------------------------------------------  BODY : START
        QHBoxLayout *hBox_album = new QHBoxLayout();
        hBox_album->setSpacing(0);
        hBox_album->setContentsMargins(0,0,0,0);
        hBox_album->setAlignment(Qt::AlignTop);
        hBox_album->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_content = new QWidget();
        widget_content->setLayout(hBox_album);
        widget_content->setContentsMargins(0,0,0,0);

        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_content);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(285);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        QHBoxLayout *hBox_total = new QHBoxLayout();
        hBox_total->setSpacing(0);
        hBox_total->setContentsMargins(0,0,0,0);
        hBox_total->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        hBox_total->setSizeConstraint(QLayout::SetFixedSize);
        hBox_total->addWidget(tmp_scrollArea);

        // return
        return hBox_album;
    }


    QHBoxLayout* MusicSearch::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QHBoxLayout *hBox_album = new QHBoxLayout();
        hBox_album->setSpacing(0);
        hBox_album->setContentsMargins(0, 0, 0, 0);
        hBox_album->setAlignment(Qt::AlignTop);
        hBox_album->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_content = new QWidget;
        widget_content->setLayout(hBox_album);
        widget_content->setContentsMargins(0, 0, 0, 0);

        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_content);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0, 0, 0, 0);
        tmp_scrollArea->setFixedHeight(275);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_album;
    }


    void MusicSearch::slot_applyResult_checkTrackId(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk") && jsonObj.value("flagOk").toBool()){

            if(this->track_svr_idx < 5){

                int id = ProcJsonEasy::getInt(jsonObj, "id");

                QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(this->track_svr_idx).toObject();

                if(id > 0){
                    QString track_id = QString("%1").arg(ProcJsonEasy::getInt(trackInfo, "id"));
                    QString playUrl = ProcJsonEasy::getString(trackInfo, "data");

                    QJsonObject data;
                    data.insert("macAddress", global.device.getDeviceID());
                    data.insert("playUrl", playUrl);
                    data.insert("clientKey", track_id);

                    QJsonObject tmpjson;
                    tmpjson.insert("id", id);
                    tmpjson.insert("favorite", false);
                    tmpjson.insert("playUrl", playUrl);
                    tmpjson.insert("clientKey", track_id);
                    tmpjson.insert("data", data);

                    QJsonObject track;
                    track.insert("track", tmpjson);

                    roseHome::ProcCommon *proc_rating = new roseHome::ProcCommon(this);
                    connect(proc_rating, &roseHome::ProcCommon::completeReq_rating_track, this, &MusicSearch::slot_applyResult_getTrckRating);
                    proc_rating->request_rose_getRating_Track("MUSIC", track);
                }
                else{
                    this->track_svr_idx++;

                    trackInfo = this->jsonArr_tracks_toPlay.at(this->track_svr_idx).toObject();

                    QJsonObject data = QJsonObject();
                    data.insert("macAddress", global.device.getDeviceID());
                    data.insert("trackId", 0);

                    QJsonObject track = QJsonObject();
                    track.insert("data", data);
                    track.insert("playUrl", ProcJsonEasy::getString(trackInfo, "data"));
                    track.insert("type", "MUSIC");

                    QJsonArray tracks = QJsonArray();
                    tracks.append(track);

                    // request HTTP API - track check for Rose Server
                    roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                    connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &MusicSearch::slot_applyResult_checkTrackId);
                    proc->request_rose_checkRating_Track("MUSIC", tracks);
                }
            }
        }
    }


    void MusicSearch::slot_applyResult_getTrckRating(const QJsonArray &jsonObj){

        QJsonObject rcvJson = jsonObj.at(0).toObject();

        bool flag_fav = ProcJsonEasy::getBool(rcvJson, "favorite");
        int star_fav = ProcJsonEasy::getInt(rcvJson, "star");

        this->search_track[this->track_svr_idx]->setFavoritesIds(flag_fav, star_fav);

        this->track_svr_idx++;

        QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(this->track_svr_idx).toObject();

        QJsonObject data = QJsonObject();
        data.insert("macAddress", global.device.getDeviceID());
        data.insert("trackId", 0);

        QJsonObject track = QJsonObject();
        track.insert("data", data);
        track.insert("playUrl", ProcJsonEasy::getString(trackInfo, "data"));
        track.insert("type", "MUSIC");

        QJsonArray tracks = QJsonArray();
        tracks.append(track);

        // request HTTP API - track check for Rose Server
        roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
        connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &MusicSearch::slot_applyResult_checkTrackId);
        proc->request_rose_checkRating_Track("MUSIC", tracks);
    }


    void MusicSearch::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        //QJsonDocument doc(jsonObj);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "jsonObj = " << strJson;

        int id = ProcJsonEasy::getInt(jsonObj, "id");
        QJsonObject trackData = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();

        if(id > 0){
            QJsonObject ratingInfo = QJsonObject();
            ratingInfo.insert("favorite", this->flag_track_fav);
            ratingInfo.insert("star", this->track_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "MUSIC");

            QJsonObject track = QJsonObject();
            track.insert("duration", 0);
            track.insert("favorite", false);
            track.insert("id", id);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 0);
            track.insert("type", "MUSIC");

            QJsonObject jsonData = QJsonObject();
            jsonData.insert("ratingInfo", ratingInfo);
            jsonData.insert("track", track);

            // request HTTP API - track check for Rose Server
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_rating_track, this, &MusicSearch::slot_applyResult_getRating_track);
            proc->request_rose_setRating_Track(jsonData, this->flag_track_fav, this->track_star_fav);
        }
        else{
            QJsonObject data = QJsonObject();
            data.insert("macAddress", global.device.getDeviceID());
            data.insert("trackId", 0);

            QJsonArray albums = QJsonArray();
            albums.append(ProcJsonEasy::getString(trackData, "album"));

            QJsonArray artists = QJsonArray();
            artists.append(ProcJsonEasy::getString(trackData, "artist"));

            QJsonObject track = QJsonObject();
            track.insert("albums", albums);
            track.insert("artists", artists);
            track.insert("clientKey", QString("%1").arg(ProcJsonEasy::getInt(trackData, "id")));
            track.insert("data", data);
            track.insert("duration", ProcJsonEasy::getInt(trackData, "duration") / 1000);
            track.insert("favorite", this->flag_track_fav);
            track.insert("ownerId", 0);
            track.insert("playUrl", ProcJsonEasy::getString(trackData, "data"));
            track.insert("sort", 0);
            track.insert("star", this->track_star_fav);
            track.insert("thumbnailUrl", ProcJsonEasy::getString(trackData, "album_art"));
            track.insert("title", ProcJsonEasy::getString(trackData, "title"));
            track.insert("type", "MUSIC");

            QJsonArray tracks = QJsonArray();
            tracks.append(track);

            QJsonObject json = QJsonObject();
            json.insert("tracks", tracks);

            // request HTTP API - track check for Rose Server
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &MusicSearch::slot_applyResult_addRating_track);
            proc->request_rose_addRating_Track(json);
        }
    }


    void MusicSearch::slot_applyResult_getRating_track(const QJsonArray &jsonObj){

        //QJsonDocument doc(jsonObj);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "jsonObj = " << strJson;

        QJsonObject rcvJson = jsonObj.at(0).toObject();

        if(rcvJson.contains("flagOk") && rcvJson.value("flagOk").toBool()){
            this->search_track[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
        }

        if(this->flag_check_track == true){
            this->flag_check_track = false;
        }
    }


    void MusicSearch::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk") && jsonObj.value("flagOk").toBool()){
            this->search_track[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
        }

        if(this->flag_check_track == true){
            this->flag_check_track = false;
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Functions
    //
    //-----------------------------------------------------------------------------------------------------------------------

    // 서브 타이틀별 우측 View All ---------------------------------------------------------------------------------------------------

    /**
     * @brief 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void MusicSearch::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_ARTIST){
            QJsonObject jsonObj;
            jsonObj.insert("title", QString("'%1' ").arg(this->data_search.search_word) + tr("Search result") + " - " + tr("Artists"));
            jsonObj.insert("data", this->jsonArr_artists);
            jsonObj.insert(KEY_PAGE_CODE, PAGECODE_VA_RECOMMANDARTIST);

            emit linker->signal_clickedMovePage(jsonObj);
        }
        else if(btnId == BTN_IDX_SUBTITLE_ALBUM){
            QJsonObject jsonObj;
            jsonObj.insert("title", QString("'%1' ").arg(this->data_search.search_word) + tr("Search result") + " - " + tr("Albums"));
            jsonObj.insert("data", this->jsonArr_albums);
            jsonObj.insert(KEY_PAGE_CODE, PAGECODE_VA_SEARCH_ALBUM);

            emit linker->signal_clickedMovePage(jsonObj);
        }
        else if(btnId == BTN_IDX_SUBTITLE_TRACK){
            QJsonObject jsonObj;
            jsonObj.insert("title", QString("'%1' ").arg(this->data_search.search_word) + tr("Search result") + " - " + tr("Tracks"));
            jsonObj.insert("data", this->jsonArr_tracks);
            jsonObj.insert(KEY_PAGE_CODE, PAGECODE_VA_SEARCH_TRACK);

            emit linker->signal_clickedMovePage(jsonObj);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void MusicSearch::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리 - Album 파트가 여러개임

        if(section == SECTION_FOR_MORE_POPUP___ARTIST){

            roseHome::ArtistItemData data_artist = this->list_search_artist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                SqliteHelper *sqlite = new SqliteHelper(this);
                QSqlError err = sqlite->addConnectionRose();

                QList<DataPopup*> *dataPopupList = new QList<DataPopup*>();
                if(err.type() == QSqlError::NoError){
                    QString strQuery = "";
                    strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
                    strQuery += " FROM audio A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
                    strQuery += " WHERE A.artist_id="+QString::number(data_artist.id);

                    QVariantList data;
                    sqlite->exec(strQuery, data);

                    for(int i = 0; i < data.size(); i++){
                        DataPopup *tmp_dataPopup = new DataPopup(this);
                        tmp_dataPopup->setJsonData(data.at(i).toJsonObject());
                        dataPopupList->append(tmp_dataPopup);
                    }
                }

                sqlite->close();
                delete sqlite;

                QJsonArray jsonArrAlbum;
                for(int i = 0; i < dataPopupList->size(); i++){
                    jsonArrAlbum.append(dataPopupList->at(i)->getJsonData());
                }

                // Artist Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert(KEY_OP_TITLE_MAIN, data_artist.name);
                jsonData.insert(KEY_OP_TITLE_SUB, "");
                jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Home_Artist);
                jsonData.insert(KEY_OP_albumImg, data_artist.imageUrl);
                jsonData.insert(KEY_OP_cntStar, 0);
                jsonData.insert(KEY_OP_DATA, jsonArrAlbum);
                jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);

                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ARTISTINFO);

                emit linker->signal_clickedMovePage(jsonData);
            }
        }
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void MusicSearch::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리 - Album 파트가 여러개임

        if(section == SECTION_FOR_MORE_POPUP___ALBUM){

            roseHome::AlbumItemData data_album = this->list_search_album->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Music_Album, roseHome::ConvertData::getConvertOptHeaderData(data_album), index, SECTION_FOR_MORE_POPUP___ALBUM, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){

                QJsonArray jsonArr_tracks = this->getDataForPlayMusic(data_album.id);

                // Album 바로 재생
                QJsonObject tmp_json;
                tmp_json.insert("musicPlayType", 15);
                tmp_json.insert("music", jsonArr_tracks);
                tmp_json.insert("currentPosition", 0);
                tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

                if(global.Queue_track_count != 0) {
                    emit linker->signal_checkQueue(27, "");

                    return;
                }
                emit linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(HTTP_NETWORK_PLAY
                                 , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song")
                                 , tmp_json
                                 , true
                                 , true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                // Album Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert(KEY_OP_TITLE_MAIN, data_album.title);
                jsonData.insert(KEY_OP_TITLE_SUB, data_album.artist);
                jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Album);
                jsonData.insert(KEY_OP_albumImg, data_album.thumbnail);
                jsonData.insert(KEY_OP_MIME, data_album.mime);
                jsonData.insert(KEY_OP_ARTIST, data_album.artist);
                jsonData.insert(KEY_OP_cntStar, 0);
                jsonData.insert(KEY_OP_DATA, this->getDataForPlayMusic(data_album.id));
                jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);
                jsonData.insert("isAlbum", true);

                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ALBUMINFO);

                emit linker->signal_clickedMovePage(jsonData);
            }
        }
    }


    QJsonArray MusicSearch::getDataForPlayMusic(const int album_id){

        QJsonArray jsonArray;

        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();
        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, A.mime_type, A.samplerate, A.bitdepth, ART._data AS album_art ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            strQuery += " WHERE A.album_id=%1 ORDER BY A.track ";

            QVariantList dataDB;
            sqlite->exec(strQuery.arg(album_id), dataDB);
            if(dataDB.size() > 0){
                for(int i = 0 ; i < dataDB.size(); i++){
                    DataPopup *tmp_dataPopup = new DataPopup(this);
                    tmp_dataPopup->setJsonData(dataDB.at(i).toJsonObject());

                    jsonArray.append(tmp_dataPopup->getJsonData());
                }
            }
        }

        sqlite->close();
        delete sqlite;

        return jsonArray;
    }


    void MusicSearch::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___TRACK;

        // ClickMode 별로 처리
        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->search_track[idx]->getFavoritesStars();
                this->flag_track_fav = false;

                if(this->track_star_fav == 3){
                    this->track_star_fav = 0;
                    this->flag_track_fav = false;
                }
                else if(this->track_star_fav >= 0 && this->track_star_fav < 3){
                    this->track_star_fav++;
                    this->flag_track_fav = true;
                }

                this->track_idx_fav = idx;

                QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QJsonObject data = QJsonObject();
                data.insert("macAddress", global.device.getDeviceID());
                data.insert("trackId", 0);

                QJsonObject track = QJsonObject();
                track.insert("data", data);
                track.insert("playUrl", ProcJsonEasy::getString(trackInfo, "data"));
                track.insert("type", "MUSIC");

                QJsonArray tracks = QJsonArray();
                tracks.append(track);

                // request HTTP API - track check for Rose Server
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &MusicSearch::slot_applyResult_checkRating_track);
                proc->request_rose_checkRating_Track("MUSIC", tracks);

                this->flag_check_track = true;
            }
        }
        else{
            // ClickMode 별로 처리
            if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AllBox){
                // Track 바로 재생 - clicked 항목만 재생
                OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::Play_RightNow_withReorder;
                int curr_playType = global.device.getMusicPlayType();

                if(global.device.getMusicPlayType() == 15){
                    playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                }
                else if(global.device.getMusicPlayType() == 13){
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                }
                else if(global.device.getMusicPlayType() == 12){
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                }
                else if(global.device.getMusicPlayType() == 16){
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                }
                else if(global.device.getMusicPlayType() == 17){
                    playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                }
                else if(global.device.getMusicPlayType() == 18){
                    playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                }
                else if(global.device.getMusicPlayType() == 19){
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                }

                QJsonArray jsonArr_tracks = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_tracks_toPlay, idx, playType);

                QJsonObject tmp_json;
                tmp_json.insert("musicPlayType", curr_playType);
                tmp_json.insert("music", jsonArr_tracks);
                tmp_json.insert("currentPosition", 0);
                tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

                if(global.Queue_track_count != 0) {
                    print_debug();emit linker->signal_checkQueue(27, "");

                    return;
                }
                print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(HTTP_NETWORK_PLAY
                                 , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song")
                                 , tmp_json
                                 , true
                                 , true);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                // 플레이리스트 담기 - Track
                //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){
                QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

                OptMorePopup::HeaderData data_header;
                data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                data_header.sub_title = ProcJsonEasy::getString(tmpObj, "artist");
                data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "album_art");
                data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(tmpObj, "id"));
                data_header.type = "MUSIC";      //j220906 share link
                data_header.flagProcStar = false;
                data_header.isShare = false;      //j220906 share link

                // OptMorePopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Track, data_header, idx, section);
            }
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void MusicSearch::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___ALBUM){

            roseHome::AlbumItemData data_album = this->list_search_album->at(index);


            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                int playType = 0;
/*
                if(clickMode == OptMorePopup::ClickMode::Share){//c220826_2
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }*/
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow){
                    playType = 15;
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last){
                    playType = 13;
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty){
                    playType = 12;
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext){
                    playType = 16;
                }
                else{
                    playType = 15;
                }

                // Album 바로 재생
                QJsonArray jsonArr_tracks = this->getDataForPlayMusic(data_album.id);

                // Album 바로 재생
                QJsonObject tmp_json;
                tmp_json.insert("musicPlayType", playType);
                tmp_json.insert("music", jsonArr_tracks);
                tmp_json.insert("currentPosition", 0);
                tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

                if(global.Queue_track_count != 0) {
                    print_debug();emit linker->signal_checkQueue(27, "");

                    return;
                }
                print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(HTTP_NETWORK_PLAY
                                 , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song")
                                 , tmp_json
                                 , true
                                 , true);
            }
            else if(clickMode == OptMorePopup::ClickMode::Go_Artist){
                // artist Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Etc_ArtistDetailTrack);
                jsonData.insert(KEY_OP_albumImg, data_album.thumbnail);
                jsonData.insert(KEY_OP_DATA, this->getDataForPlayMusic(data_album.id));

                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ARTISTINFO);

                emit this->linker->signal_clickedMovePage(jsonData);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___TRACK){

            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last

                    )
            {
                int playType = this->get_rose_playType(clickMode);

                QJsonArray jsonArr_tracks = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_tracks_toPlay, index, clickMode);

                QJsonObject jsonData;
                jsonData.insert("musicPlayType", playType);
                jsonData.insert("music", jsonArr_tracks);
                jsonData.insert("currentPosition", 0);
                jsonData.insert("roseToken", global.device.getDeviceRoseToken());

                if(global.queueTimerFlag && !global.queuelist_mouse_trigger_menu_flag && global.Queue_track_count != 0) {
                    emit this->linker->signal_checkQueue(27, "");

                    return;
                }
                emit this->linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3")
                                 .arg(global.device.getDeviceIP())
                                 .arg(global.port).arg("music_song")
                                 , jsonData
                                 , true
                                 , true);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___ARTIST){

        }
    }


    int MusicSearch::get_rose_playType(OptMorePopup::ClickMode clickMode){


            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::Play_RightNow_withReorder
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
                ){
            // 바로 재생을 요청하는 타입
            return 15;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
                ){
            // 현재 Queue 끝에 추가
            return 13;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
                ){
            // 큐를 비우고 추가(재생)
            return 12;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
                ){
            return 16;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere){
            // 여기서부터 재생
            return 17;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty){
            // 큐 비우고 여기서부터 재생
            return 18;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last){
            // 여기서부터 큐 끝에 추가
            return 19;
        }
        else{
            return 0;           // unknown or not supported
        }
    }


    QJsonArray MusicSearch::get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode){


            if(clickMode == OptMorePopup::ClickMode::Play_RightNow_withReorder
                || clickMode == OptMorePopup::ClickMode::Play_RightNow
        ){
            if(curr_index > 0){
                // 전체이지만 re-ordering
                QJsonArray jsonArr_output = this->reorderJsonArray(jsonArr_toPlayAll, curr_index);
                return jsonArr_output;
            }
            else{
                return jsonArr_toPlayAll;
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
        ){
            // 1곡만
            QJsonArray jsonArr_output;
            jsonArr_output.append(jsonArr_toPlayAll.at(curr_index).toObject());
            return jsonArr_output;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last
        ){
            // index ~ 마지막까지 구성
            if(curr_index > 0){
                QJsonArray jsonArr_output = this->subRange_JsonArray(jsonArr_toPlayAll, curr_index);
                return jsonArr_output;
            }
            else{
                return jsonArr_toPlayAll;
            }
        }
        else{
            // 그 이외의 경우
            return jsonArr_toPlayAll;
        }

    }


    /**
     * @brief AbstractProcRosePlay::subRange_JsonArray
     * @param p_jsonArr
     * @param startIndex
     * @return
     */
    QJsonArray MusicSearch::subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex){

        const int len_data = p_jsonArr.count();

        if(startIndex > 0 && startIndex < len_data){
            QJsonArray jsonArr_subRange;
            for(int i = startIndex; i < len_data; i++){
                jsonArr_subRange.append(p_jsonArr.at(i));
            }
            return jsonArr_subRange;
        }
        else{
            // 유효하지 않는 경우, 그대로 반환
            return p_jsonArr;
        }
    }


    /**
     * @brief AbstractProcRosePlay::reorderJsonArray
     * @param p_jsonArr
     * @param startIndex
     * @return
     */
    QJsonArray MusicSearch::reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex){

        const int len_data = p_jsonArr.count();

        if(startIndex > 0 && startIndex < len_data){
            QJsonArray jsonArr_reorder;
            for(int i = startIndex; i < len_data; i++){
                jsonArr_reorder.append(p_jsonArr.at(i));
            }
            for(int i = 0; i < startIndex; i++){
                jsonArr_reorder.append(p_jsonArr.at(i));
            }
            return jsonArr_reorder;
        }
        else{
            // 유효하지 않는 경우, 그대로 반환
            return p_jsonArr;
        }
    }
}
