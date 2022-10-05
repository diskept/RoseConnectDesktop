#include "music/MusicAlbumDetail.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"
#include "common/rosesettings.h"
#include "common/sqlitehelper.h"

#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"
#include "widget/optionpopup.h"

#include <QDebug>
#include <QPainter>
#include <QScrollArea>
#include <QSettings>
#include <QPixmapCache>


namespace music {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___albums = 1;
    const int SECTION_FOR_MORE_POPUP___tracks = 2;

    const int GET_MAX_ITEM_SIZE___ONCE = 100;

    QString ALBTAB_STEP_TRACK = "track";
    QString ALBTAB_STEP_CREDIT = "credit";

    /**
     * @brief
     * @param parent
     */
    AlbumDetail::AlbumDetail(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_filter, parent) {

        this->linker = Linker::getInstance();
    }


    /**
     * @brief 소멸자.
     */
    AlbumDetail::~AlbumDetail(){

        this->deleteLater();
    }


    /**
     * @brief RoseHomeAlbumDetail::setJsonObject_forData
     * @param jsonObj
     */
    void AlbumDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        /*QJsonDocument doc(jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << "jsonObj = " << strJson;*/

        QJsonArray tmpArr = ProcJsonEasy::getJsonArray(jsonObj, "op_data");
        int tmpAlbumId = ProcJsonEasy::getInt(tmpArr.at(0).toObject(), "album_id");

        bool isAlbum = ProcJsonEasy::getBool(jsonObj, "isAlbum");
        QString album = ProcJsonEasy::getString(jsonObj, "op_mainTitle");
        QString artist = ProcJsonEasy::getString(jsonObj, "op_artist");

        QString img = ProcJsonEasy::getString(jsonObj, "op_albumImg");
        QString tmp_mime = ProcJsonEasy::getString(jsonObj, "op_mime");
        int tmp_sample = ProcJsonEasy::getInt(jsonObj, "op_sampleRate");
        int tmp_bit = ProcJsonEasy::getInt(jsonObj, "op_bitDepth");

        this->flagNeedReload = false;

        if(isAlbum == true && (album != this->albumTitle || artist != this->artistName)){

            this->flagNeedReload = true;

            QString mime = "";
            if(tmp_mime == "MP3" || tmp_mime == "AMR" || tmp_mime == "AWB" || tmp_mime == "WMA" || tmp_mime == "APE" || tmp_mime == "OGG" || tmp_mime == "AAC" || tmp_mime == "MKA"
                    || tmp_mime == "MIDI" || tmp_mime == "SMF" || tmp_mime == "IMY" || tmp_mime == "M3U8" || tmp_mime == "PLS" || tmp_mime == "AIFF" || tmp_mime == "WV"){

                mime = tmp_mime;
            }
            else if(tmp_mime == "DSD"){
                if(tmp_sample == 88200 || tmp_sample == 2822400){
                    mime = tmp_mime + " 64";
                }
                else if(tmp_sample == 176400 || tmp_sample == 5644800){
                    mime = tmp_mime + " 128";
                }
                else if(tmp_sample == 352800 || tmp_sample == 11289600){
                    mime = tmp_mime + " 256";
                }
                else if(tmp_sample == 705600 || tmp_sample == 22579200){
                    mime = tmp_mime + " 512";
                }
                else if(tmp_sample == 1411200 || tmp_sample == 45158400){
                    mime = tmp_mime + " 1024";
                }
                else{
                    mime = tmp_mime;
                }
            }
            else if(tmp_mime == "WAV" || tmp_mime == "FLAC" || tmp_mime == "ALAC"){
                if(tmp_bit == 0 || tmp_bit == 16){
                    mime = tmp_mime;
                }
                else if(tmp_bit > 16){
                    mime = tmp_mime + QString(" %1").arg(tmp_bit);
                }
            }
            else{
                mime = tmp_mime;
            }

            QString img_path = "";
            if(img.startsWith("http")){
                img_path = img;
            }
            else{
                img_path = "http://" + global.device.getDeviceIP() + ":" + global.port_img + img;
            }
            this->albumImg = img_path;
            this->albumTitle = album;
            this->albumMime = mime;
            this->artistName = artist;

            this->list_track = new QList<roseHome::TrackItemData>();

            this->jsonArr_tracks_toPlay = QJsonArray();
            this->jsonArr_tracks_toPlay = ProcJsonEasy::getJsonArray(jsonObj, "op_data");

            this->track_drawCount = 0;
            this->track_totalCount = 0;
            this->track_favoriteOffset = 0;

            this->album_id_roseSVR = 0;

            this->track_id_fav = 0;
            this->track_idx_fav = 0;

            this->album_star_fav = 0;
            this->track_star_fav = 0;

            this->flag_album_fav = false;
            this->flag_track_fav = false;
            this->flag_check_album = false;
            this->flag_check_track = false;

            this->flag_draw = false;

            if(this->flag_track_ok == true){
                this->widget_main_contents->hide();

                GSCommon::clearLayout(this->box_main_contents);
                this->box_contents->removeWidget(this->widget_main_contents);

                this->flag_track_ok = false;
            }

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);

            QJsonArray *p_jsonArray_titlSub = new QJsonArray();
            QJsonObject sub1 { {"name", tr("Track")}, {"code", ALBTAB_STEP_TRACK} };
            //QJsonObject sub2 { {"name", tr("Credit")}, {"code", ALBTAB_STEP_CREDIT} };

            p_jsonArray_titlSub->push_back(sub1);
            //p_jsonArray_titlSub->push_back(sub2);

            this->menubar = new TopMenuBar(TopMenuBar::ShowMenuStyle::AlbumDetail);
            this->menubar->setDataTopMenuBar(QJsonObject(), p_jsonArray_titlSub);

            connect(this->menubar, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(slot_changedSubTabUI_Album(QJsonObject)));

            this->contentStep = "track";
            this->menubar->setSelectedSubMenuNoSignal(this->contentStep);
            this->menubar->hide();

            // Album 상세정보
            this->album_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Music_album);
            connect(this->album_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &AlbumDetail::slot_detailInfo_btnClicked);

            this->album_detail_info->initView();

            this->box_main_contents->addWidget(this->album_detail_info);
            this->box_main_contents->addSpacing(50);

            // 앨범의 트랙정보 UI
            this->vl_tracks = new QVBoxLayout();
            this->vl_tracks->setContentsMargins(0,0,0,0);
            this->vl_tracks->setSpacing(0);
            this->vl_tracks->setAlignment(Qt::AlignTop);

            this->vl_credit = new QVBoxLayout();
            this->vl_credit->setContentsMargins(0,0,0,0);
            this->vl_credit->setSpacing(0);
            this->vl_credit->setAlignment(Qt::AlignTop);

            this->widget_tab_tracks = new QWidget();
            this->widget_tab_tracks->hide();

            this->widget_tab_credit = new QWidget();
            this->widget_tab_credit->hide();

            this->widget_tab_tracks->setLayout(this->vl_tracks);
            this->widget_tab_credit->setLayout(this->vl_credit);

            this->stackedwidget = new QStackedWidget();
            this->stackedwidget->addWidget(this->widget_tab_tracks);
            this->stackedwidget->addWidget(this->widget_tab_credit);
            this->stackedwidget->hide();

            this->vbox_trackList = new QVBoxLayout();
            this->vbox_trackList->setSpacing(0);
            this->vbox_trackList->setContentsMargins(0, 0, 0, 0);
            this->vbox_trackList->setAlignment(Qt::AlignTop);
            this->vbox_trackList->addWidget(this->menubar);
            this->vbox_trackList->addWidget(this->stackedwidget);

            this->box_main_contents->addLayout(this->vbox_trackList);

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->slot_applyResult_album(this->jsonArr_tracks_toPlay);
        }
        else if(isAlbum == false && (tmpAlbumId > 0) && (tmpAlbumId != this->albumId)){
            this->flagNeedReload = true;

            SqliteHelper *sqlite = new SqliteHelper(this);
            QSqlError err = sqlite->addConnectionRose();
            if(err.type() == QSqlError::NoError){
                QString strQuery = "";

                strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art ";
                strQuery += " FROM album_info AS AI ";
                strQuery += " LEFT OUTER JOIN album_art AS AA ";
                strQuery += " ON AI._id=AA.album_id ";
                strQuery += QString(" WHERE AI._id = %1").arg(tmpAlbumId);

                QVariantList data;
                sqlite->exec(strQuery, data);

                if(data.size() > 0){

                    for(int i = 0; i < data.size(); i++){
                        QJsonObject jsonData = data.at(i).toJsonObject();

                        album = ProcJsonEasy::getString(jsonData, "album");
                        artist = ProcJsonEasy::getString(jsonData, "artist");
                        img = ProcJsonEasy::getString(jsonData, "album_art");
                        tmp_mime = ProcJsonEasy::getString(jsonData, "mime_type");
                        tmp_mime.replace("audio/", "");
                        tmp_sample = ProcJsonEasy::getInt(jsonData, "samplerate");
                        tmp_bit = ProcJsonEasy::getInt(jsonData, "bitdepth");
                    }

                }

                sqlite->close();
                delete sqlite;
            }

            QString mime = "";
            if(tmp_mime == "MP3" || tmp_mime == "AMR" || tmp_mime == "AWB" || tmp_mime == "WMA" || tmp_mime == "APE" || tmp_mime == "OGG" || tmp_mime == "AAC" || tmp_mime == "MKA"
                    || tmp_mime == "MIDI" || tmp_mime == "SMF" || tmp_mime == "IMY" || tmp_mime == "M3U8" || tmp_mime == "PLS" || tmp_mime == "AIFF" || tmp_mime == "WV"){

                mime = tmp_mime;
            }
            else if(tmp_mime == "DSD"){
                if(tmp_sample == 88200 || tmp_sample == 2822400){
                    mime = tmp_mime + " 64";
                }
                else if(tmp_sample == 176400 || tmp_sample == 5644800){
                    mime = tmp_mime + " 128";
                }
                else if(tmp_sample == 352800 || tmp_sample == 11289600){
                    mime = tmp_mime + " 256";
                }
                else if(tmp_sample == 705600 || tmp_sample == 22579200){
                    mime = tmp_mime + " 512";
                }
                else if(tmp_sample == 1411200 || tmp_sample == 45158400){
                    mime = tmp_mime + " 1024";
                }
                else{
                    mime = tmp_mime;
                }
            }
            else if(tmp_mime == "WAV" || tmp_mime == "FLAC" || tmp_mime == "ALAC"){
                if(tmp_bit == 0 || tmp_bit == 16){
                    mime = tmp_mime;
                }
                else if(tmp_bit > 16){
                    mime = tmp_mime + QString(" %1").arg(tmp_bit);
                }
            }
            else{
                mime = tmp_mime;
            }

            QString img_path = "";
            if(img.startsWith("http")){
                img_path = img;
            }
            else{
                img_path = "http://" + global.device.getDeviceIP() + ":" + global.port_img + img;
            }
            this->albumImg = img_path;
            this->albumTitle = album;
            this->albumMime = mime;
            this->artistName = artist;

            this->list_track = new QList<roseHome::TrackItemData>();

            this->jsonArr_tracks_toPlay = QJsonArray();
            this->jsonArr_tracks_toPlay = this->getDataForPlayMusic(tmpAlbumId);;

            this->track_drawCount = 0;
            this->track_totalCount = 0;
            this->track_favoriteOffset = 0;

            this->album_id_roseSVR = 0;

            this->track_id_fav = 0;
            this->track_idx_fav = 0;

            this->album_star_fav = 0;
            this->track_star_fav = 0;

            this->flag_album_fav = false;
            this->flag_track_fav = false;
            this->flag_check_album = false;
            this->flag_check_track = false;

            this->flag_draw = false;

            if(this->flag_track_ok == true){
                this->widget_main_contents->hide();

                GSCommon::clearLayout(this->box_main_contents);
                this->box_contents->removeWidget(this->widget_main_contents);

                this->flag_track_ok = false;
            }

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);

            QJsonArray *p_jsonArray_titlSub = new QJsonArray();
            QJsonObject sub1 { {"name", tr("Track")}, {"code", ALBTAB_STEP_TRACK} };
            //QJsonObject sub2 { {"name", tr("Credit")}, {"code", ALBTAB_STEP_CREDIT} };

            p_jsonArray_titlSub->push_back(sub1);
            //p_jsonArray_titlSub->push_back(sub2);

            this->menubar = new TopMenuBar(TopMenuBar::ShowMenuStyle::AlbumDetail);
            this->menubar->setDataTopMenuBar(QJsonObject(), p_jsonArray_titlSub);

            connect(this->menubar, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(slot_changedSubTabUI_Album(QJsonObject)));

            this->contentStep = "track";
            this->menubar->setSelectedSubMenuNoSignal(this->contentStep);
            this->menubar->hide();

            // Album 상세정보
            this->album_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Music_album);
            connect(this->album_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &AlbumDetail::slot_detailInfo_btnClicked);

            this->album_detail_info->initView();

            this->box_main_contents->addWidget(this->album_detail_info);
            this->box_main_contents->addSpacing(50);

            // 앨범의 트랙정보 UI
            this->vl_tracks = new QVBoxLayout();
            this->vl_tracks->setContentsMargins(0,0,0,0);
            this->vl_tracks->setSpacing(0);
            this->vl_tracks->setAlignment(Qt::AlignTop);

            this->vl_credit = new QVBoxLayout();
            this->vl_credit->setContentsMargins(0,0,0,0);
            this->vl_credit->setSpacing(0);
            this->vl_credit->setAlignment(Qt::AlignTop);

            this->widget_tab_tracks = new QWidget();
            this->widget_tab_tracks->hide();

            this->widget_tab_credit = new QWidget();
            this->widget_tab_credit->hide();

            this->widget_tab_tracks->setLayout(this->vl_tracks);
            this->widget_tab_credit->setLayout(this->vl_credit);

            this->stackedwidget = new QStackedWidget();
            this->stackedwidget->addWidget(this->widget_tab_tracks);
            this->stackedwidget->addWidget(this->widget_tab_credit);
            this->stackedwidget->hide();

            this->vbox_trackList = new QVBoxLayout();
            this->vbox_trackList->setSpacing(0);
            this->vbox_trackList->setContentsMargins(0, 0, 0, 0);
            this->vbox_trackList->setAlignment(Qt::AlignTop);
            this->vbox_trackList->addWidget(this->menubar);
            this->vbox_trackList->addWidget(this->stackedwidget);

            this->box_main_contents->addLayout(this->vbox_trackList);

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->slot_applyResult_album(this->jsonArr_tracks_toPlay);
        }
        else if(isAlbum == false && (tmpAlbumId <= 0)){
            this->flagNeedReload = true;

            this->albumImg = "";
            this->albumTitle = tr("");
            this->albumMime = "";
            this->artistName = "";

            this->list_track = new QList<roseHome::TrackItemData>();

            this->jsonArr_tracks_toPlay = QJsonArray();
            this->jsonArr_tracks_toPlay = ProcJsonEasy::getJsonArray(jsonObj, "op_data");

            this->track_drawCount = 0;
            this->track_totalCount = 0;
            this->track_favoriteOffset = 0;

            this->album_id_roseSVR = 0;

            this->track_id_fav = 0;
            this->track_idx_fav = 0;

            this->album_star_fav = 0;
            this->track_star_fav = 0;

            this->flag_album_fav = false;
            this->flag_track_fav = false;
            this->flag_check_album = false;
            this->flag_check_track = false;

            this->flag_draw = false;

            if(this->flag_track_ok == true){
                this->widget_main_contents->hide();

                GSCommon::clearLayout(this->box_main_contents);
                this->box_contents->removeWidget(this->widget_main_contents);

                this->flag_track_ok = false;
            }

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            QJsonArray *p_jsonArray_titlSub = new QJsonArray();
            QJsonObject sub1 { {"name", tr("Track")}, {"code", ALBTAB_STEP_TRACK} };
            //QJsonObject sub2 { {"name", tr("Credit")}, {"code", ALBTAB_STEP_CREDIT} };

            p_jsonArray_titlSub->push_back(sub1);
            //p_jsonArray_titlSub->push_back(sub2);

            this->menubar = new TopMenuBar(TopMenuBar::ShowMenuStyle::AlbumDetail);
            this->menubar->setDataTopMenuBar(QJsonObject(), p_jsonArray_titlSub);

            connect(this->menubar, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(slot_changedSubTabUI_Album(QJsonObject)));

            this->contentStep = "track";
            this->menubar->setSelectedSubMenuNoSignal(this->contentStep);
            this->menubar->hide();

            // Album 상세정보
            this->album_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Music_album);
            connect(this->album_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &AlbumDetail::slot_detailInfo_btnClicked);

            this->album_detail_info->initView();

            this->box_main_contents->addWidget(this->album_detail_info);
            this->box_main_contents->addSpacing(50);

            // 앨범의 트랙정보 UI
            this->vl_tracks = new QVBoxLayout();
            this->vl_tracks->setContentsMargins(0,0,0,0);
            this->vl_tracks->setSpacing(0);
            this->vl_tracks->setAlignment(Qt::AlignTop);

            this->vl_credit = new QVBoxLayout();
            this->vl_credit->setContentsMargins(0,0,0,0);
            this->vl_credit->setSpacing(0);
            this->vl_credit->setAlignment(Qt::AlignTop);

            this->widget_tab_tracks = new QWidget();
            this->widget_tab_tracks->hide();

            this->widget_tab_credit = new QWidget();
            this->widget_tab_credit->hide();

            this->widget_tab_tracks->setLayout(this->vl_tracks);
            this->widget_tab_credit->setLayout(this->vl_credit);

            this->stackedwidget = new QStackedWidget();
            this->stackedwidget->addWidget(this->widget_tab_tracks);
            this->stackedwidget->addWidget(this->widget_tab_credit);
            this->stackedwidget->hide();

            this->vbox_trackList = new QVBoxLayout();
            this->vbox_trackList->setSpacing(0);
            this->vbox_trackList->setContentsMargins(0, 0, 0, 0);
            this->vbox_trackList->setAlignment(Qt::AlignTop);
            this->vbox_trackList->addWidget(this->menubar);
            this->vbox_trackList->addWidget(this->stackedwidget);

            this->box_main_contents->addLayout(this->vbox_trackList);

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->slot_applyResult_album(this->jsonArr_tracks_toPlay);
        }
    }


    /**
     * @brief RoseHomePlaylistDetail_Rose 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void AlbumDetail::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            this->slot_applyResult_album_tracks(this->jsonArr_tracks_toPlay);
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void AlbumDetail::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)){

            this->flag_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    void AlbumDetail::request_more_trackDraw(){

        ContentLoadingwaitingMsgHide();//c220616

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > GET_MAX_ITEM_SIZE___ONCE ) ? GET_MAX_ITEM_SIZE___ONCE : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->album_track_info[i] = new AlbumTrackDetailInfo_RHV;
            connect(this->album_track_info[i], &AlbumTrackDetailInfo_RHV::clicked, this, &AlbumDetail::slot_clickedItemTrack_inList);
            this->album_track_info[i]->setProperty("index", i);

            this->vl_tracks->addWidget(this->album_track_info[i]);
        }

        for(int i = start_index; i < this->track_drawCount; i++){
            this->album_track_info[i]->setDataTrackInfo_Music(this->jsonArr_tracks_toPlay.at(i).toObject());

            QCoreApplication::processEvents();
        }

        if(this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_draw = false;

        this->menubar->setSelectedSubMenuNoSignal(this->contentStep);

        QJsonObject p_data;
        p_data.insert("pageCode", "track");
        slot_changedSubTabUI_Album(p_data);

        this->menubar->show();
        this->stackedwidget->show();
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : [Slot]
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void AlbumDetail::slot_applyResult_album(const QJsonArray &jsonArr){

        int maxCnt = jsonArr.size();
        this->track_totalCount = maxCnt;

        this->albumDuration = 0;
        if(maxCnt > 0){
            for(int i = 0; i < maxCnt; i++){
                QJsonObject tmpObj = jsonArr.at(i).toObject();
                int tmp_duration = ProcJsonEasy::getInt(tmpObj, "duration") / 1000;

                this->albumDuration += tmp_duration;

                if(i == 0){
                    this->albumId = ProcJsonEasy::getInt(tmpObj, "album_id");
                    this->albumKey = ProcJsonEasy::getString(tmpObj, "album_key");
                }
            }
        }

        QJsonObject albumObj;
        albumObj.insert("image", this->albumImg);
        albumObj.insert("title", this->albumTitle);
        albumObj.insert("mime", this->albumMime);
        albumObj.insert("artist", this->artistName);
        albumObj.insert("track_count", this->track_totalCount);
        albumObj.insert("duration", this->albumDuration);
        albumObj.insert("type", "MUSIC");

        this->album_detail_info->setData_fromMusicData(albumObj);

        // album favorite check
        /*roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
        connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_album, this, &AlbumDetail::slot_applyResult_getRating_album);
        proc_fav_album->request_rose_getRating_Album("MUSIC", this->albumKey);
        this->flag_check_album = true;*/

        roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
        connect(proc_fav_album, &roseHome::ProcCommon::completeCheck_rating_album, this, &AlbumDetail::slot_applyResult_checkRating_album);
        proc_fav_album->request_rose_checkRating_Album("MUSIC", this->albumKey);
    }


    /**
     * @brief playlist 내의 트랙 아이템들 반환 데이터를 처리함
     * @param list_track
     * @param jsonArr_dataToPlay
     */
    void AlbumDetail::slot_applyResult_album_tracks(const QJsonArray &jsonArr_dataToPlay){

        if(jsonArr_dataToPlay.size() > 0){

            int max_cnt = 0;
            if(this->track_totalCount > 100){
                max_cnt = 100;
            }
            else{
                max_cnt = this->track_totalCount;
            }
            this->track_drawCount = max_cnt;

            for(int i = 0; i < max_cnt; i++){
                this->album_track_info[i] = new AlbumTrackDetailInfo_RHV;
                connect(this->album_track_info[i], &AlbumTrackDetailInfo_RHV::clicked, this, &AlbumDetail::slot_clickedItemTrack_inList);
                this->album_track_info[i]->setProperty("index", i);

                this->vl_tracks->addWidget(this->album_track_info[i]);
            }

            for(int i = 0; i < max_cnt; i++){
                this->album_track_info[i]->setDataTrackInfo_Music(this->jsonArr_tracks_toPlay.at(i).toObject());

                QCoreApplication::processEvents();
            }

            ContentLoadingwaitingMsgHide();
            this->flag_track_ok = true;

            if(this->flag_track_ok == true && (this->track_totalCount == this->track_drawCount)){
                this->box_main_contents->addSpacing(30);
            }
        }
        else{
            this->flag_track_ok = true;
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->vl_tracks->addWidget(noData_widget);
        }

        this->menubar->setSelectedSubMenuNoSignal(this->contentStep);

        QJsonObject p_data;
        p_data.insert("pageCode", "track");
        slot_changedSubTabUI_Album(p_data);

        this->menubar->show();
        this->stackedwidget->show();
    }


    void AlbumDetail::slot_applyResult_rose_tracks(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){

            this->list_track->append(list_data);

            if(this->track_totalCount == this->list_track->size()){
                for(int i = 0; i < this->track_drawCount; i++)
                this->album_track_info[i]->setFavoritesIds(this->list_track->at(i).favorite, this->list_track->at(i).star);
            }
            else{
                this->request_more_trackData();
            }
        }
    }


    void AlbumDetail::request_more_trackData(){

        if((this->track_totalCount > this->list_track->count())){
            this->track_page++;

            roseHome::ProcCommon *proc_tracks = new roseHome::ProcCommon(this);
            connect(proc_tracks, &roseHome::ProcCommon::completeReq_list_tracks, this, &AlbumDetail::slot_applyResult_rose_tracks);
            proc_tracks->request_rose_get_album(this->album_id_roseSVR, this->track_page, 20);
        }
    }


    void AlbumDetail::slot_applyResult_checkRating_album(const QJsonObject &jsonObj){

        int rcv_id = ProcJsonEasy::getInt(jsonObj, "id");

        if(rcv_id > 0){
            QJsonObject album;
            album.insert("favorite", false);
            album.insert("id", rcv_id);
            album.insert("star", 0);
            album.insert("trackCount", 0);
            album.insert("type", "MUSIC");

            QJsonObject ratingInfo;
            ratingInfo.insert("favorite", this->flag_album_fav);
            ratingInfo.insert("star", this->album_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "MUSIC");

            QJsonObject json;
            json.insert("album", album);
            json.insert("ratingInfo", ratingInfo);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
            connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_album, this, &AlbumDetail::slot_applyResult_getRating_album);
            proc_fav_album->request_rose_setRating_Album(json, this->flag_album_fav, this->album_star_fav);

            if(this->album_id_roseSVR == 0){
                this->album_id_roseSVR = rcv_id;

                this->track_page = 0;
                roseHome::ProcCommon *proc_album = new roseHome::ProcCommon(this);
                connect(proc_album, &roseHome::ProcCommon::completeReq_list_tracks, this, &AlbumDetail::slot_applyResult_rose_tracks);
                proc_album->request_rose_get_album(this->album_id_roseSVR, this->track_page, 20);
            }

            this->flag_check_album = true;
        }
        else if(rcv_id <= 0){
            QJsonObject data;
            data.insert("macAddress", global.device.getDeviceID());
            data.insert("trackId", 0);

            QJsonObject album;
            album.insert("clientKey", this->albumKey);
            album.insert("data", data);
            album.insert("favorite", true);
            album.insert("star", 1);
            album.insert("title", this->albumTitle);
            album.insert("trackCount", 0);
            album.insert("tracks", this->jsonArr_tracks_toPlay);
            album.insert("type", "MUSIC");

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
            connect(proc_fav_album, &roseHome::ProcCommon::completeCheck_rating_album, this, &AlbumDetail::slot_applyResult_addRating_album);
            proc_fav_album->request_rose_addRating_Album(album);

            this->flag_check_album = true;
        }
    }


    void AlbumDetail::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        int rcv_id = ProcJsonEasy::getInt(jsonObj, "id");

        if(rcv_id > 0){
            QJsonObject track;
            track.insert("duration", 0);
            track.insert("favorite", false);
            track.insert("id", rcv_id);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 0);
            track.insert("type", "MUSIC");

            QJsonObject ratingInfo;
            ratingInfo.insert("favorite", this->flag_track_fav);
            ratingInfo.insert("star", this->track_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "MUSIC");

            QJsonObject json;
            json.insert("track", track);
            json.insert("ratingInfo", ratingInfo);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &AlbumDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
        }
        else if(rcv_id == 0){
            QJsonObject tmpTrack = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();

            QJsonObject data;
            data.insert("macAddress", global.device.getDeviceID());
            data.insert("trackId", 0);

            QJsonObject track;
            track.insert("duration", ProcJsonEasy::getInt(tmpTrack, "duration"));
            track.insert("data", data);
            track.insert("favorite", true);
            track.insert("id", ProcJsonEasy::getInt(tmpTrack, "id"));
            track.insert("star", 1);
            track.insert("title", ProcJsonEasy::getInt(tmpTrack, "title"));
            track.insert("type", "MUSIC");

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
            connect(proc_fav_album, &roseHome::ProcCommon::completeCheck_rating_track, this, &AlbumDetail::slot_applyResult_addRating_track);
            proc_fav_album->request_rose_addRating_Track(track);

            this->flag_check_track = true;
        }
    }


    void AlbumDetail::slot_applyResult_addRating_album(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_album == true){
                this->flag_check_album = false;
            }
        }
        else{
            this->flag_check_album = false;
        }
    }


    void AlbumDetail::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void AlbumDetail::slot_applyResult_getRating_album(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject tmp_star = contents.at(0).toObject();

            if(tmp_star.contains("star")){
                int star = ProcJsonEasy::getInt(tmp_star, "star");
                bool flag = star > 0 ? true : false;

                this->album_detail_info->setFavorite(flag, star);
            }

            if(this->flag_check_album == true){
                this->flag_check_album = false;
            }
        }
    }


    void AlbumDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->album_track_info[this->track_favoriteOffset + i]->setFavoritesIds(flag, star);
                }
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void AlbumDetail::changedOnlyTabUI_notSendSignal(QString p_step){

        this->contentStep_Album = p_step;

        // Tab 타이틀 변경
        this->menubar->setSelectedSubMenuNoSignal(contentStep_Album);

        // DB 가져와서 데이터 세팅
        if(contentStep_Album == ALBTAB_STEP_TRACK){
            this->stackedwidget->setCurrentIndex(0);
            //this->stackedwidget->setFixedHeight(this->vl_tracks->sizeHint().height());
        }
        else if(contentStep_Album == ALBTAB_STEP_CREDIT){
            //this->stackedwidget->setCurrentIndex(1);
            //this->stackedwidget->setFixedHeight(this->vl_credit->sizeHint().height());
        }
    }



    void AlbumDetail::slot_changedSubTabUI_Album(const QJsonObject &p_data){

        QString tmp_step = p_data[KEY_PAGE_CODE].toString();

        // 페이지 변경 및 데이터 세팅
        this->changedOnlyTabUI_notSendSignal(tmp_step);
    }


    /**
     * @brief [Slot] PlaylistImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void AlbumDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

        if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx2 ||
                clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx3 || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){

            if(this->flag_check_album == false){
                int star_cnt = this->album_detail_info->getFavoritesStars();
                this->flag_album_fav = false;

                if(star_cnt == 3){
                    star_cnt = 0;
                    this->flag_album_fav = false;
                    this->album_star_fav = star_cnt;
                }
                else if(star_cnt >= 0 && star_cnt < 3){
                    star_cnt++;
                    this->flag_album_fav = true;
                    this->album_star_fav = star_cnt;
                }

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
                connect(proc_fav_album, &roseHome::ProcCommon::completeCheck_rating_album, this, &AlbumDetail::slot_applyResult_checkRating_album);
                proc_fav_album->request_rose_checkRating_Album("MUSIC", this->albumKey);

                this->album_detail_info->setFavorite(this->flag_album_fav, this->album_star_fav);
            }
        }
        else{

            if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){

                OptMorePopup::HeaderData data_header;
                data_header.main_title = this->albumTitle;
                data_header.sub_title = this->artistName;
                data_header.imageUrl = this->albumImg;
                data_header.data_pk = this->albumKey;
                data_header.type = "MUSIC";      //j220906 share link
                data_header.flagProcStar = false;
                data_header.isShare = false;      //j220906 share link

                this->makeObj_optMorePopup(OptMorePopup::Music_Album, data_header, 0, SECTION_FOR_MORE_POPUP___albums, true);

            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Info){

                QJsonObject tmpData;
                tmpData.insert("image", this->albumImg);
                tmpData.insert("title", this->albumTitle);
                tmpData.insert("mime", this->albumMime);
                tmpData.insert("artist", this->artistName);
                tmpData.insert("track", this->track_totalCount);
                tmpData.insert("duration", this->albumDuration);
                tmpData.insert("description", this->albumDiscription);

                this->dlgAlbumInfo = new DialogNotice(this, DialogNotice::DlgNotice::Music_album_info);
                this->dlgAlbumInfo->setData_formJsonObj(tmpData);
                this->dlgAlbumInfo->exec();
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayAll){

                if(global.Queue_track_count != 0) {
                    print_debug();emit linker->signal_checkQueue(27, "");

                    return;
                }
                print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += this->jsonArr_tracks_toPlay.count();     // 220419 queue count

                QJsonObject tmp_json;
                tmp_json.insert("music", this->jsonArr_tracks_toPlay);
                tmp_json.insert("musicPlayType", 15);
                tmp_json.insert("currentPosition", 0);
                tmp_json.insert("shuffle", 0);
                tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

                NetworkHttp *network = new NetworkHttp;
                network->request(0, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayShuffle){

                if(global.Queue_track_count != 0) {
                    print_debug();emit linker->signal_checkQueue(27, "");

                    return;
                }
                print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += this->jsonArr_tracks_toPlay.count();     // 220419 queue count

                QJsonObject tmp_json;
                tmp_json.insert("music", this->jsonArr_tracks_toPlay);
                tmp_json.insert("musicPlayType", 15);
                tmp_json.insert("currentPosition", 0);
                tmp_json.insert("shuffle", 1);
                tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

                NetworkHttp *network = new NetworkHttp;
                network->request(0, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
            }
        }
    }


    /**
     * @brief [Slot] AlbumImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void AlbumDetail::slot_clickedItemTrack_inList(const int idx, const AlbumTrackDetailInfo_RHV::ClickMode clickMode){

        if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                int star_cnt = this->album_track_info[idx]->getFavoritesStars();
                this->flag_track_fav = false;


                if(star_cnt == 3){
                    star_cnt = 0;
                    this->flag_track_fav = false;
                    this->track_star_fav = star_cnt;

                }
                else if(star_cnt >= 0 && star_cnt < 3){
                    star_cnt++;
                    this->flag_track_fav = true;
                    this->track_star_fav = star_cnt;
                }

                this->track_idx_fav = idx;
                QJsonObject trackObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QJsonObject data = QJsonObject();
                data.insert("macAddress", global.device.getDeviceID());
                data.insert("trackId", 0);

                QJsonObject track = QJsonObject();
                track.insert("data", data);
                track.insert("playUrl", ProcJsonEasy::getString(trackObj, "data"));
                track.insert("type", "MUSIC");

                QJsonArray tracks = QJsonArray();
                tracks.append(track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &AlbumDetail::slot_applyResult_checkRating_track);
                proc_fav_track->request_rose_checkRating_Track("MUSIC", tracks);

                this->album_track_info[idx]->setFavoritesIds(this->flag_track_fav, star_cnt);
            }
        }
        else{

            // ClickMode 별로 처리
            if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::AllBox){
                // Track 바로 재생 - clicked 항목만 재생
                QJsonArray trackData = QJsonArray();
                QJsonObject tmp_json;

                if(global.device.getMusicPlayType() == 15){
                    trackData.append(this->jsonArr_tracks_toPlay.at(idx).toObject());

                    tmp_json.insert("music", trackData);
                    tmp_json.insert("musicPlayType", 15);
                }
                else if(global.device.getMusicPlayType() == 13){
                    trackData.append(this->jsonArr_tracks_toPlay.at(idx).toObject());

                    tmp_json.insert("music", trackData);
                    tmp_json.insert("musicPlayType", 13);
                }
                else if(global.device.getMusicPlayType() == 12){
                    trackData.append(this->jsonArr_tracks_toPlay.at(idx).toObject());

                    tmp_json.insert("music", trackData);
                    tmp_json.insert("musicPlayType", 12);
                }
                else if(global.device.getMusicPlayType() == 16){
                    trackData.append(this->jsonArr_tracks_toPlay.at(idx).toObject());

                    tmp_json.insert("music", trackData);
                    tmp_json.insert("musicPlayType", 16);
                }
                else if(global.device.getMusicPlayType() == 17){
                    //trackData = this->reorderJsonArray(this->jsonArr_tracks_toPlay, index);
                    for(int i = idx; i < this->jsonArr_tracks_toPlay.size(); i++){
                        trackData.append(this->jsonArr_tracks_toPlay.at(i).toObject());
                    }

                    tmp_json.insert("music", trackData);
                    tmp_json.insert("musicPlayType", 17);
                }
                else if(global.device.getMusicPlayType() == 18){
                    //trackData = this->reorderJsonArray(this->jsonArr_tracks_toPlay, index);
                    for(int i = idx; i < this->jsonArr_tracks_toPlay.size(); i++){
                        trackData.append(this->jsonArr_tracks_toPlay.at(i).toObject());
                    }

                    tmp_json.insert("music", trackData);
                    tmp_json.insert("musicPlayType", 18);
                }
                else if(global.device.getMusicPlayType() == 19){
                    //trackData = this->reorderJsonArray(this->jsonArr_tracks_toPlay, index);
                    for(int i = idx; i < this->jsonArr_tracks_toPlay.size(); i++){
                        trackData.append(this->jsonArr_tracks_toPlay.at(i).toObject());
                    }

                    tmp_json.insert("music", trackData);
                    tmp_json.insert("musicPlayType", 19);
                }

                if(global.Queue_track_count != 0) {
                    print_debug();emit linker->signal_checkQueue(27, "");

                    return;
                }
                print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += trackData.count();     // 220419 queue count

                tmp_json.insert("currentPosition", 0);
                tmp_json.insert("roseToken",global.device.getDeviceRoseToken());

                NetworkHttp *network = new NetworkHttp;
                network->request(0, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);

                /*QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
                int curr_clickMode = settings->value(rosesettings.SETTINGS_TRACK_OPTION).toInt();

                int playType = 0;
                switch(curr_clickMode)
                {
                    case 0:
                        playType = 13;
                        break;
                    case 1:
                        playType = 12;
                        break;
                    case 2:
                        playType = 15;
                        break;
                    case 3:
                        playType = 16;
                        break;
                    case 4:
                        playType = 17;
                        break;
                    case 5:
                        playType = 18;
                        break;
                    case 6:
                        playType = 19;
                        break;
                }*/
            }
            else if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                // 플레이리스트 담기 - Track
                //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
            }
            else if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::MoreBtn){

                QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

                // OptMorePopup 띄우기 필요
                OptMorePopup::HeaderData data_header;
                data_header.main_title = ProcJsonEasy::getString(track, "title");
                data_header.sub_title = ProcJsonEasy::getString(track, "artist");
                data_header.imageUrl = this->albumImg;
                data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(track, "id"));
                data_header.type = "MUSIC";      //j220906 share link
                data_header.flagProcStar = false;
                data_header.isShare = false;      //j220906 share link

                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Music_Track, data_header, idx, SECTION_FOR_MORE_POPUP___tracks, true);
            }
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void AlbumDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___albums){
/*
            if(clickMode == OptMorePopup::ClickMode::Share){//c220826_2
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }*/
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext){

                QJsonObject tmp_json;
                tmp_json.insert("music", this->jsonArr_tracks_toPlay);

                if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow){
                    tmp_json.insert("musicPlayType", 15);
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last){
                    tmp_json.insert("musicPlayType", 13);
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty){
                    tmp_json.insert("musicPlayType", 12);
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext){
                    tmp_json.insert("musicPlayType", 16);
                }

                tmp_json.insert("currentPosition", 0);
                tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

                if(global.Queue_track_count != 0) {
                    print_debug();emit linker->signal_checkQueue(27, "");

                    return;
                }
                print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += this->jsonArr_tracks_toPlay.count();     // 220419 queue count

                NetworkHttp *network = new NetworkHttp;
                network->request(0, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
            }
            else if(clickMode == OptMorePopup::ClickMode::Go_Artist){
                // artist Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Etc_ArtistDetailTrack);
                jsonData.insert(KEY_OP_albumImg, this->albumImg);
                jsonData.insert(KEY_OP_DATA, this->jsonArr_tracks_toPlay);

                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ARTISTINFO);

                emit this->linker->signal_clickedMovePage(jsonData);
            }

        }
        else if(section == SECTION_FOR_MORE_POPUP___tracks){

            QJsonArray trackData = QJsonArray();
            QJsonObject tmp_json;

            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow){
                trackData.append(this->jsonArr_tracks_toPlay.at(index).toObject());

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 15);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne){
                trackData.append(this->jsonArr_tracks_toPlay.at(index).toObject());

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 13);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne){
                trackData.append(this->jsonArr_tracks_toPlay.at(index).toObject());

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 12);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne){
                trackData.append(this->jsonArr_tracks_toPlay.at(index).toObject());

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 15);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne){
                trackData.append(this->jsonArr_tracks_toPlay.at(index).toObject());

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 16);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere){
                //trackData = this->reorderJsonArray(this->jsonArr_tracks_toPlay, index);
                for(int i = index; i < this->jsonArr_tracks_toPlay.size(); i++){
                    trackData.append(this->jsonArr_tracks_toPlay.at(i).toObject());
                }

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 17);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty){
                //trackData = this->reorderJsonArray(this->jsonArr_tracks_toPlay, index);
                for(int i = index; i < this->jsonArr_tracks_toPlay.size(); i++){
                    trackData.append(this->jsonArr_tracks_toPlay.at(i).toObject());
                }

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 18);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last){
                //trackData = this->reorderJsonArray(this->jsonArr_tracks_toPlay, index);
                for(int i = index; i < this->jsonArr_tracks_toPlay.size(); i++){
                    trackData.append(this->jsonArr_tracks_toPlay.at(i).toObject());
                }

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 19);
            }

            tmp_json.insert("currentPosition", 0);
            tmp_json.insert("roseToken",global.device.getDeviceRoseToken());

            if(global.Queue_track_count != 0) {
                print_debug();emit linker->signal_checkQueue(27, "");
                return;
            }
            print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
            global.Queue_track_count += trackData.count();     // 220419 queue count

            NetworkHttp *network = new NetworkHttp;
            network->request(0, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
        }
    }


    QJsonArray AlbumDetail::getDataForPlayMusic(const int album_id){

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


    QJsonArray AlbumDetail::reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex){

        const int len_data = p_jsonArr.count();
        if(startIndex > 0 && startIndex<len_data){
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
