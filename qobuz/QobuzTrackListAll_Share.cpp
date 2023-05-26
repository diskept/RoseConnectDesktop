#include "qobuz/QobuzTrackListAll_Share.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"
#include "tidal/ProcRosePlay_withTidal.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"

#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ProcCommon_forQobuz.h"
#include "qobuz/ProcRosePlay_withQobuz.h"

#include "apple/ConvertData_forApple.h"
#include "apple/ProcCommon_forApple.h"
#include "apple/ProcRosePlay_withApple.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"
#include "widget/toastmsg.h"
#include "widget/VerticalScrollArea.h"

#include <QDebug>
#include <QLabel>
#include <QScroller>
#include <QPushButton>
#include <QPixmapCache>


namespace qobuz {

    const int SECTION_FOR_MORE_POPUP___tracks = 0;

    const QString ICON_PATH___playAll = ":/images/icon_list_allplay.png";
    const QString ICON_PATH___playShuffle = ":/images/icon_list_shuffle.png";

    /**
     * @brief "ROSEHOME > REcENT TRACK" 화면을 위한 생성자. @see PAGECODE_RH_TRACK_LIST_ALL_VIEW
     * @param parent
     */
    QobuzTrackListAll_Share::QobuzTrackListAll_Share(QWidget *parent) : AbstractQobuzSubWidget(VerticalScroll_rosefilter, parent) {

        // data
        this->list_track = new QList<qobuz::TrackItemData>();

        // UI
        this->setUIControl_tracks();
    }


    /**
    * @brief 소멸자.
    */
    QobuzTrackListAll_Share::~QobuzTrackListAll_Share(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzTrackListAll_Share::setJsonObject_forData(const QJsonObject &jsonObj){

        QString tmpType = ProcJsonEasy::getString(jsonObj, "type");
        int tmpIntID = ProcJsonEasy::getInt(jsonObj, "id");

        this->flagNeedReload = false;

        if(this->type != tmpType || this->int_id != tmpIntID){
            this->flagNeedReload = true;

            this->type = tmpType;
            this->int_id = tmpIntID;

            //this->label_mainTitle->setText(this->title);

            // init
            this->list_track->clear();
            this->jsonArr_tracks_toPlay = QJsonArray();

            this->track_id_fav = 0;
            this->track_idx_fav = 0;
            this->track_star_fav = 0;

            this->flag_check_track = false;
            this->flag_track_fav = false;
            this->flag_send_track = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_trackData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief QobuzTrackListAll_Share::setActivePage
     */
    void QobuzTrackListAll_Share::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

            this->box_filter->removeWidget(this->widget_filter_contents);
            GSCommon::clearLayout(this->box_filter);

            this->widget_filter_contents = new QWidget();
            this->widget_filter_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_filter_contents->setFixedSize(1500, 50);

            int width = 0;
            int left = 0;
            // All
            QPixmap pixmap_play(":/images/icon_list_allplay.png");
            QPushButton *btn_playAll = new QPushButton(this->widget_filter_contents);
            btn_playAll->setIcon(pixmap_play);
            btn_playAll->setIconSize(pixmap_play.rect().size());
            btn_playAll->setText("Play All");
            btn_playAll->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            btn_playAll->setCursor(Qt::PointingHandCursor);

            width = btn_playAll->sizeHint().width();
            btn_playAll->setGeometry(left, 0, width, 50);
            connect(btn_playAll, &QPushButton::clicked, this, &QobuzTrackListAll_Share::slot_btnClicked_playAll);

            // shuffle
            QPixmap pixmap_shuffle(":/images/icon_list_shuffle.png");
            QPushButton *btn_shuffleAll = new QPushButton(this->widget_filter_contents);
            btn_shuffleAll->setIcon(pixmap_shuffle);
            btn_shuffleAll->setIconSize(pixmap_shuffle.rect().size());
            btn_shuffleAll->setText("Shuffle Play");
            btn_shuffleAll->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            btn_shuffleAll->setCursor(Qt::PointingHandCursor);

            left = width + 30;
            width = btn_shuffleAll->sizeHint().width();
            btn_shuffleAll->setGeometry(left, 0, width, 50);
            connect(btn_shuffleAll, &QPushButton::clicked, this, &QobuzTrackListAll_Share::slot_btnClicked_playShuffle);

            this->box_filter->addWidget(this->widget_filter_contents);

            this->box_contents->removeWidget(this->widget_main_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            this->vBox_tracks = new QVBoxLayout();
            this->vBox_tracks->setSpacing(0);
            this->vBox_tracks->setContentsMargins(0, 0, 0, 0);
            GSCommon::clearLayout(this->vBox_tracks);

            this->box_main_contents->addLayout(this->vBox_tracks);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void QobuzTrackListAll_Share::setUIControl_tracks(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Qobuz");
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void QobuzTrackListAll_Share::request_more_trackData(){

        if(this->type == "ROSE_TOTAL"){
            // request HTTP API
            /*roseHome::ProcCommon *proc_track = new roseHome::ProcCommon(this);
            connect(proc_track, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseHomeTrackListAll_Share::slot_applyResult_tracks);
            proc_track->request_rose_get_track(this->int_id, 0, 20);*/
        }
        else if(this->type == "TIDAL"){
            // request HTTP API
            /*QString url = "tracks/" + QString("%1").arg(this->int_id);

            tidal::ProcCommon *proc_track = new tidal::ProcCommon(this);
            connect(proc_track, &tidal::ProcCommon::completeReq_list_tracks, this, &TidalTrackListAll_Share::slot_applyResult_tracks);
            proc_track->request_tidal_getList_tracks(url, 0, 0);*/
        }
        else if(this->type == "BUGS"){
            // request HTTP API
            /*QString url = "track/" + QString("%1").arg(this->int_id);

            bugs::ProcBugsAPI *proc_track = new bugs::ProcBugsAPI(this);
            connect(proc_track, &bugs::ProcBugsAPI::completeReq_list_tracks, this, &BugsTrackListAll_Share::slot_applyResult_tracks);
            proc_track->request_bugs_getList_tracks(url, 0, 0);*/
        }
        else if(this->type == "QOBUZ"){
            // request HTTP API
            QString url = "track/get";

            qobuz::ProcCommon *proc_track = new qobuz::ProcCommon(this);
            connect(proc_track, &qobuz::ProcCommon::completeReq_list_tracks, this, &QobuzTrackListAll_Share::slot_applyResult_tracks);
            proc_track->request_qobuz_getList_track(url, this->int_id, 0, 0);
        }
        else if(this->type == "APPLE_MUSIC"){

        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] Track 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzTrackListAll_Share::slot_applyResult_tracks(const QList<qobuz::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){

            if(list_data.size() == 0){
                ContentLoadingwaitingMsgHide();
                return;
            }

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
                this->track_totalCount = list_data.at(0).appearson_count;
            }

            int start_index = this->list_track->count();

            this->list_track->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                int max_cnt = this->list_track->size();

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzTrackListAll_Share::slot_clickedItemTrack_inList);
                    this->track_listAll[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->track_listAll[i]);
                }

                QList<QString> tmp_clientkey;
                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i]->setDataTrackInfo_Qobuz(this->list_track->at(i));
                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));

                    QCoreApplication::processEvents();
                }

                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzTrackListAll_Share::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("QOBUZ", tmp_clientkey);

                ContentLoadingwaitingMsgHide();
                this->flag_widget_visible = true;

                if(this->track_totalCount > this->list_track->count()){
                    this->request_more_trackData();
                }
                else{
                    this->box_main_contents->addSpacing(30);
                }
            }
            else{

                if(this->track_totalCount > this->list_track->count()){
                    this->request_more_trackData();
                }
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void QobuzTrackListAll_Share::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        int id = ProcJsonEasy::getInt(jsonObj, "id");
        QJsonObject jsonObj_track = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();

        if(id <= 0){
            /*QJsonObject artist;
            artist.insert("albums_count", this->data_album.artist_albums_count);
            artist.insert("id", this->data_album.artist_id);
            artist.insert("name", this->data_album.artist_name);

            QJsonObject image;
            image.insert("large", this->data_album.image_large);
            image.insert("small", this->data_album.image_small);
            image.insert("thumbnail", this->data_album.image_thumbnail);

            QJsonObject label;
            label.insert("albums_count", this->data_album.label_albums_count);
            label.insert("id", this->data_album.label_id);
            label.insert("name", this->data_album.label_name);

            QJsonObject album;
            album.insert("artist", artist);
            album.insert("duration", this->data_album.duration);
            album.insert("favCnt", 0);
            album.insert("hires", this->data_album.hires);
            album.insert("hires_streamable", this->data_album.hires_streamable);
            album.insert("id", this->data_album.id);
            album.insert("image", image);
            album.insert("label", label);
            album.insert("maximum_bit_depth",  this->data_album.maximum_bit_depth);
            album.insert("maximum_channel_count",  this->data_album.maximum_channel_count);
            album.insert("maximum_sampling_rate", this->data_album.maximum_sampling_rate);
            album.insert("release_date_original",  this->data_album.release_date_original);
            album.insert("release_date_stream",  this->data_album.release_date_stream);
            album.insert("streamable", this->data_album.streamable);
            album.insert("title", this->data_album.title);
            album.insert("tracks_count", this->data_album.tracks_count);

            QJsonObject data;
            data.insert("album", album);
            data.insert("composer", ProcJsonEasy::getJsonObject(jsonObj_track, "composer"));
            data.insert("copyright", ProcJsonEasy::getString(jsonObj_track, "copyright"));
            data.insert("duration", ProcJsonEasy::getInt(jsonObj_track, "duration"));
            data.insert("favCnt", ProcJsonEasy::getInt(jsonObj_track, "favCnt"));
            data.insert("hires", ProcJsonEasy::getBool(jsonObj_track, "hires"));
            data.insert("hires_streamable", ProcJsonEasy::getBool(jsonObj_track, "hires_streamable"));
            data.insert("id", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            data.insert("maximum_bit_depth",  ProcJsonEasy::getInt(jsonObj_track, "maximum_bit_depth"));
            data.insert("maximum_sampling_rate",  ProcJsonEasy::getInt(jsonObj_track, "maximum_sampling_rate"));
            data.insert("media_number", ProcJsonEasy::getInt(jsonObj_track, "media_number"));
            data.insert("performer", ProcJsonEasy::getJsonObject(jsonObj_track, "performer"));
            data.insert("performers", ProcJsonEasy::getString(jsonObj_track, "performers"));
            data.insert("previewable", ProcJsonEasy::getBool(jsonObj_track, "previewable"));
            data.insert("purchasable", ProcJsonEasy::getBool(jsonObj_track, "purchasable"));
            data.insert("qobuz_id", ProcJsonEasy::getInt(jsonObj_track, "qobuz_id"));
            data.insert("sampleable", ProcJsonEasy::getBool(jsonObj_track, "sampleable"));
            data.insert("streamable", ProcJsonEasy::getBool(jsonObj_track, "streamable"));
            data.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            data.insert("track_number", ProcJsonEasy::getInt(jsonObj_track, "track_number"));

            QJsonObject track;
            track.insert("clientKey", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            track.insert("data", data);
            track.insert("duration", 0);
            track.insert("favorite", this->flag_track_fav);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 1);
            track.insert("thumbnailUrl", this->data_album.image_small);
            track.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            track.insert("type", "QOBUZ");*/

            QJsonObject tmp_album = ProcJsonEasy::getJsonObject(jsonObj_track, "album");

            QJsonArray tmp_artists = ProcJsonEasy::getJsonArray(tmp_album, "artists");
            QJsonArray artists;
            for(int i = 0; i < tmp_artists.size(); i++){
                QJsonObject tmpArtist = tmp_artists.at(i).toObject();

                QJsonObject artist;
                artist.insert("name", ProcJsonEasy::getString(tmpArtist, "name"));

                artists.append(artist);
            }

            QJsonObject thumbnail = ProcJsonEasy::getJsonObject(tmp_album, "image");
            QString thumbnailUrl = ProcJsonEasy::getString(thumbnail, "large");

            QJsonObject track;
            track.insert("artists", artists);
            track.insert("clientKey", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            track.insert("comment", "");

            track.insert("data", jsonObj_track);

            track.insert("duration", ProcJsonEasy::getInt(jsonObj_track, "duration"));
            track.insert("favorite", false);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 0);
            track.insert("thumbnailUrl", thumbnailUrl);
            track.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            track.insert("type", "QOBUZ");

            QJsonArray tracks;
            tracks.append(track);

            QJsonObject json;
            json.insert("tracks", tracks);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzTrackListAll_Share::slot_applyResult_addRating_track);
            proc_fav_track->request_rose_addRating_Track(json);
        }
        else{
            QJsonObject ratingInfo;
            ratingInfo.insert("favorite", this->flag_track_fav);
            ratingInfo.insert("star", this->track_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "QOBUZ");

            QJsonObject track;
            track.insert("duration", 0);
            track.insert("favorite", false);
            track.insert("id", id);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 0);
            track.insert("type", "QOBUZ");

            QJsonObject json;
            json.insert("ratingInfo", ratingInfo);
            json.insert("track", track);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzTrackListAll_Share::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
        }
    }


    void QobuzTrackListAll_Share::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){

            for(int i = 0; i < contents.size(); i++){

                QJsonObject track = contents.at(i).toObject();

                if(track.contains("star")){
                    QString clientKey = ProcJsonEasy::getString(track, "clientKey");
                    int star = ProcJsonEasy::getInt(track, "star");

                    QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(i).toObject();
                    int trackID = ProcJsonEasy::getInt(trackInfo, "id");

                    if(clientKey.toInt() == trackID){

                        bool flag = (star > 0) ? true : false;
                        this->track_listAll[i]->setFavoritesIds(flag, star);
                    }
                }
                else{

                    this->track_listAll[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
                }

            }
        }

        if(this->flag_check_track == true){
            this->flag_check_track = false;
        }
    }


    void QobuzTrackListAll_Share::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
            connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzTrackListAll_Share::slot_applyResult_checkRating_track);
            proc_favCheck_track->request_rose_checkRating_Track("TIDAL", ProcJsonEasy::getString(this->jsonArr_tracks_toPlay[this->track_idx_fav].toObject(), "id"));

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    /**
     * @brief TidalTrackListAll_Share::slot_tidal_completeReq_listAll_myFavoritesIds
     * @param p_jsonObj
     */
    void QobuzTrackListAll_Share::slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject &p_jsonObj){

        if(this->flag_send_track == true){
            if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
                bool status  = ProcJsonEasy::getBool(p_jsonObj, "status");

                if(this->flag_send_track == true){
                    if((status == true && this->flag_track_fav == false) || (status == false && this->flag_track_fav == true)){
                        // Qobuz Favorite toggle
                        ProcCommon *proc = new ProcCommon(this);
                        connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzTrackListAll_Share::slot_qobuz_completeReq_listAll_myFavoritesIds);
                        proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                    }
                    this->flag_send_track = false;
                }
            }

            this->flag_send_track = false;
        }
        else{

        }
    }


    void QobuzTrackListAll_Share::slot_btnClicked_playAll(){

        // 전체재생
        ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
        procRosePlay->requestPlayRose_byTracks(jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::JustPlay);
    }

    void QobuzTrackListAll_Share::slot_btnClicked_playShuffle(){

        // 셔플재생
        ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
        procRosePlay->requestPlayRose_byTracks(jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::ShufflePlay);
    }


    /**
     * @brief [Slot] PlaylistTrackDetailInfo_RHV signal에 대하여 처리
     * @param clickMode
     */
    void QobuzTrackListAll_Share::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->track_listAll[idx]->getFavoritesStars();
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
                this->track_id_fav = this->list_track->at(idx).id;

                if(this->track_star_fav == 0 || this->track_star_fav == 1){
                    // Qobuz Favorite toggle
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzTrackListAll_Share::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("album", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                }

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzTrackListAll_Share::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("QOBUZ", QString("%1").arg(this->track_id_fav));
                this->flag_check_track = true;
            }
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, SECTION_FOR_MORE_POPUP___tracks);
        }
    }

    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void QobuzTrackListAll_Share::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }
}
