#include "bugs/BugsTrackListAll_Share.h"

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


namespace bugs {

    const int SECTION_FOR_MORE_POPUP___tracks = 0;
    const int SECTION_FOR_MORE_POPUP___videos = 1;

    const QString ICON_PATH___playAll = ":/images/icon_list_allplay.png";
    const QString ICON_PATH___playShuffle = ":/images/icon_list_shuffle.png";

    /**
     * @brief "bugs > REcENT TRACK" 화면을 위한 생성자. @see PAGECODE_RH_TRACK_LIST_ALL_VIEW
     * @param parent
     */
    BugsTrackListAll_Share::BugsTrackListAll_Share(QWidget *parent) : AbstractBugsSubWidget(VerticalScroll_rosefilter, parent) {

        // data
        this->list_track = new QList<bugs::TrackItemData>();
        this->list_video = new  QList<bugs::VideoItemData>();

        this->setUIControl_tracks();
    }


    /**
    * @brief 소멸자.
    */
    BugsTrackListAll_Share::~BugsTrackListAll_Share(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsTrackListAll_Share::setJsonObject_forData(const QJsonObject &jsonObj){

        QString tmpType = ProcJsonEasy::getString(jsonObj, "type");
        QString tmpsubTtype = ProcJsonEasy::getString(jsonObj, "subType");
        int tmpIntID = ProcJsonEasy::getInt(jsonObj, "id");

        this->flagNeedReload = false;

        if(this->type != tmpType || this->int_id != tmpIntID){
            this->flagNeedReload = true;

            this->type = tmpType;
            this->subType = tmpsubTtype;
            this->int_id = tmpIntID;

            //this->label_mainTitle->setText(this->title);

            // init
            this->list_track->clear();
            this->list_video->clear();
            this->jsonArr_tracks_toPlay = QJsonArray();

            this->track_id_fav = 0;
            this->track_idx_fav = 0;
            this->track_star_fav = 0;

            this->flag_check_track = false;
            this->flag_track_fav = false;
            this->flag_send_track = false;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_trackData();
        }
    }


    /**
     * @brief BugsTrackListAll_Share::setActivePage
     */
    void BugsTrackListAll_Share::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            this->box_filter->removeWidget(this->widget_filter_contents);
            GSCommon::clearLayout(this->box_filter);

            if(this->subType == "TRACK"){
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
                connect(btn_playAll, &QPushButton::clicked, this, &BugsTrackListAll_Share::slot_btnClicked_playAll);

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
                connect(btn_shuffleAll, &QPushButton::clicked, this, &BugsTrackListAll_Share::slot_btnClicked_playShuffle);

                this->box_filter->addWidget(this->widget_filter_contents);
            }

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

            if(this->subType == "TRACK"){
                this->vBox_tracks = new QVBoxLayout();
                this->vBox_tracks->setSpacing(0);
                this->vBox_tracks->setContentsMargins(0, 0, 0, 0);
                GSCommon::clearLayout(this->vBox_tracks);

                this->box_main_contents->addLayout(this->vBox_tracks);
            }
            else if(this->subType == "mv"){
                this->flowLayout_track = new FlowLayout(0, 0, 20);
                this->flowLayout_track->setSizeConstraint(QLayout::SetMinimumSize);
                this->flowLayout_track->setContentsMargins(0, 20, 0, 0);
                GSCommon::clearLayout(this->flowLayout_track);

                this->widget_share_content = new QWidget();
                this->widget_share_content->setLayout(this->flowLayout_track);
                this->widget_share_content->setContentsMargins(0, 0, 0, 0);
                this->widget_share_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                this->widget_share_content->setStyleSheet("background-color:transparent;");

                this->box_main_contents->addWidget(this->widget_share_content);
            }
        }
    }


    /**
     * @brief BugsTrackListAll_Share::setActivePage
     */
    void BugsTrackListAll_Share::applyLoginSuccessState(){

        /*if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;

                this->setUIControl_logined();
            }
        }
        else{
            if(this->m_flagCurrLogined){
                // 로그인 ---> 로그아웃
                this->m_flagCurrLogined = false;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->setUIControl_notLogin();
            }
        }*/
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void BugsTrackListAll_Share::setUIControl_tracks(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Bugs");
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void BugsTrackListAll_Share::request_more_trackData(){

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
            if(this->subType == "TRACK"){
                QString url = "track/" + QString("%1").arg(this->int_id);

                ProcBugsAPI *proc_track = new ProcBugsAPI(this);
                connect(proc_track, &ProcBugsAPI::completeReq_list_tracks, this, &BugsTrackListAll_Share::slot_applyResult_tracks);
                proc_track->request_bugs_getList_tracks(url, 0, 0);
            }
            else if(this->subType == "mv"){
                QString url = "mv/" + QString("%1").arg(this->int_id);

                ProcBugsAPI *proc_track = new ProcBugsAPI(this);
                connect(proc_track, &ProcBugsAPI::completeReq_list_videos, this, &BugsTrackListAll_Share::slot_applyResult_videoRecently);
                proc_track->request_bugs_getList_videos(url, 0, 0);
            }
        }
        else if(this->type == "QOBUZ"){
            // request HTTP API
            /*QString url = "track/get";

            qobuz::ProcCommon *proc_track = new qobuz::ProcCommon(this);
            connect(proc_track, &qobuz::ProcCommon::completeReq_list_tracks, this, &QobuzTrackListAll_Share::slot_applyResult_tracks);*/
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
    void BugsTrackListAll_Share::slot_applyResult_tracks(const QList<bugs::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){

            if(list_data.size() == 0){
                ContentLoadingwaitingMsgHide();
                return;
            }

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
                this->track_totalCount = list_data.at(0).total_count;
            }

            int start_index = this->list_track->count();

            this->list_track->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                int max_cnt = this->list_track->size();

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &BugsTrackListAll_Share::slot_clickedItemTrack_inList);
                    this->track_listAll[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->track_listAll[i]);
                }

                QList<QString> tmp_clientkey;
                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i]->setDataTrackInfo_Bugs(this->list_track->at(i), "Share");
                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).track_id));

                    QCoreApplication::processEvents();
                }

                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsTrackListAll_Share::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("BUGS", tmp_clientkey);

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


    void BugsTrackListAll_Share::slot_applyResult_videoRecently(const QList<bugs::VideoItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){

            if(list_data.size() == 0){
                ContentLoadingwaitingMsgHide();
                return;
            }

            if(this->list_video->size() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
                this->track_totalCount = list_data.at(0).total_count;
            }

            int start_index = this->list_video->size();
            this->list_video->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                int max_cnt = this->list_video->size();

                for(int i = start_index; i < max_cnt; i++){
                    this->listVideo_all[i] = new bugs::ItemVideo_bugs(i, SECTION_FOR_MORE_POPUP___videos, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157);
                    connect(this->listVideo_all[i], &bugs::ItemVideo_bugs::signal_clicked, this, &BugsTrackListAll_Share::slot_clickedItemVideo);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listVideo_all[i]->setData(this->list_video->at(i));
                    this->flowLayout_track->addWidget(this->listVideo_all[i]);

                    QCoreApplication::processEvents();
                }
            }

            ContentLoadingwaitingMsgHide();
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Video_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void BugsTrackListAll_Share::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        int id = ProcJsonEasy::getInt(jsonObj, "id");
        QJsonObject jsonObj_track = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();

        if(id <= 0){
            QJsonArray tmp_artists = ProcJsonEasy::getJsonArray(jsonObj_track, "artists");
            QJsonArray artists;
            for(int i = 0; i < tmp_artists.size(); i++){
                QJsonObject tmpArtist = tmp_artists.at(i).toObject();

                QJsonObject artist;
                artist.insert("name", ProcJsonEasy::getString(tmpArtist, "name"));

                artists.append(artist);
            }

            QJsonObject tmp_album = ProcJsonEasy::getJsonObject(jsonObj_track, "album");
            QJsonObject thumbnail = ProcJsonEasy::getJsonObject(tmp_album, "image");
            QString thumbnailUrl = ProcJsonEasy::getString(thumbnail, "url") + "350" + ProcJsonEasy::getString(thumbnail, "path");

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
            track.insert("type", "BUGS");

            QJsonArray tracks;
            tracks.append(track);

            QJsonObject json;
            json.insert("tracks", tracks);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsTrackListAll_Share::slot_applyResult_addRating_track);
            proc_fav_track->request_rose_addRating_Track(json);
        }
        else{
            QJsonObject ratingInfo;
            ratingInfo.insert("favorite", this->flag_track_fav);
            ratingInfo.insert("star", this->track_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "BUGS");

            QJsonObject track;
            track.insert("duration", 0);
            track.insert("favorite", false);
            track.insert("id", id);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 0);
            track.insert("type", "BUGS");

            QJsonObject json;
            json.insert("ratingInfo", ratingInfo);
            json.insert("track", track);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsTrackListAll_Share::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
        }
    }


    void BugsTrackListAll_Share::slot_applyResult_getRating_track(const QJsonArray &contents){

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


    void BugsTrackListAll_Share::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
            connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsTrackListAll_Share::slot_applyResult_checkRating_track);
            proc_favCheck_track->request_rose_checkRating_Track("BUGS", ProcJsonEasy::getString(this->jsonArr_tracks_toPlay[this->track_idx_fav].toObject(), "id"));

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
    /*void BugsTrackListAll_Share::slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){

        if(this->flag_send_track == true){
            QVariantList arr_myFavoriteIds = ProcJsonEasy::getJsonArray(p_jsonObj, "TRACK").toVariantList();
            bool status = arr_myFavoriteIds.contains(this->track_id_fav);

            if(status == true && this->flag_track_fav == false){
                // bugs track Favorite del
                ProcCommon *proc = new ProcCommon(this);
                connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalTrackListAll_Share::slot_tidal_completeReq_listAll_myFavoritesIds);
                proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
            }
            else if(status == false && this->flag_track_fav == true){
                // bugs track Favorite add
                ProcCommon *proc = new ProcCommon(this);
                connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalTrackListAll_Share::slot_tidal_completeReq_listAll_myFavoritesIds);
                proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
            }
            this->flag_send_track = false;
        }
        else{

        }
    }*/


    void BugsTrackListAll_Share::slot_btnClicked_playAll(){

        this->proc_play_allTrack(this->jsonArr_tracks_toPlay, false);
    }

    void BugsTrackListAll_Share::slot_btnClicked_playShuffle(){

        this->proc_play_allTrack(this->jsonArr_tracks_toPlay, true);
    }


    /**
     * @brief [Slot] PlaylistTrackDetailInfo_RHV signal에 대하여 처리
     * @param clickMode
     */
    void BugsTrackListAll_Share::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

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
                this->track_id_fav = this->list_track->at(idx).track_id;

                /*if(this->track_star_fav == 1){
                    // Tidal Track Favorite add
                    ProcCommon *proc = new ProcCommon(this);
                    proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                else if(this->track_star_fav == 0){
                    // Tidal Track Favorite del
                    ProcCommon *proc = new ProcCommon(this);
                    proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }

                // favorite 정보 가져오기
                ProcCommon *proc_fav = new ProcCommon(this);
                connect(proc_fav, &ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalTrackListAll_Share::slot_tidal_completeReq_listAll_myFavoritesIds);
                proc_fav->request_tidal_getAll_favorites();
                this->flag_send_track = true;*/

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsTrackListAll_Share::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("BUGS", QString("%1").arg(this->track_id_fav));

                this->flag_check_track = true;
            }
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, SECTION_FOR_MORE_POPUP___tracks);
        }
    }


    /**
     * @brief [slot] override - Itemvideo 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void BugsTrackListAll_Share::slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___videos){
            // ClickMode 별로 처리
            this->proc_clicked_itemVideo(this->list_video, this->jsonArr_tracks_toPlay, clickMode, index, section);
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void BugsTrackListAll_Share::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___videos){
            this->proc_clicked_optMorePopup_fromVideo(this->list_video, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }
}
