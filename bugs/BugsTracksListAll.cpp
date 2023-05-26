#include "bugs/BugsTracksListAll.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/ProcBugsAPI.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"

#include <QScrollBar>


namespace bugs {

// section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
const int SECTION_FOR_MORE_POPUP___track = 1;

const int GET_ITEM_SIZE___ONCE = 100;


    /**
         * @brief
         * @param parent
         */
    BugsTracksListAll::BugsTracksListAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_rosefilter, parent) {

        // data
        this->list_track = new QList<bugs::TrackItemData>();

        // margin 변경
        this->box_contents->setContentsMargins(0,0,0,0);
    }


    /**
         * @brief 소멸자.
         */
    BugsTracksListAll::~BugsTracksListAll(){

        this->deleteLater();
    }


    /**
         * @brief
         * @param jsonObj
         */
    void BugsTracksListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        bugs::PageInfo_TrackAllView tmp_pagaInfo = ConvertData_forBugs::convertData_pageInfo_trackAllView(jsonObj);
        this->flagNeedReload = false;

        if(this->data_pagaInfo.api_subPath != tmp_pagaInfo.api_subPath){
            this->flagNeedReload = true;

            this->data_pagaInfo = ConvertData_forBugs::convertData_pageInfo_trackAllView(jsonObj);

            GSCommon::clearLayout(this->box_mainTitle);
            this->label_mainTitle = this->get_addUIControl_mainTitle("Tracks");
            this->label_mainTitle->setText(this->data_pagaInfo.pathTitle);

            // init
            this->list_track->clear();
            this->jsonArr_tracks_toPlay = QJsonArray();

            this->track_id_fav = 0;
            this->track_idx_fav = 0;
            this->track_star_fav = 0;

            this->track_totalCount = 0;
            this->track_drawCount = 0;
            this->track_favoriteOffset = 0;

            this->flag_draw = false;
            this->flag_check_track = false;
            this->flag_track_fav = false;
            this->flag_send_track = false;

            // request HTTP API
            this->flagReqMore_track = false;
            this->flag_lastPage_track = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    void BugsTracksListAll::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            this->box_filter->removeWidget(this->widget_filter_contents);

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
            connect(btn_playAll, &QPushButton::clicked, this, &BugsTracksListAll::slot_btnClicked_playAll);

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
            connect(btn_shuffleAll, &QPushButton::clicked, this, &BugsTracksListAll::slot_btnClicked_playShuffle);

            this->box_filter->addWidget(this->widget_filter_contents);

            if(this->flag_widget_visible == true){
                this->widget_main_contents->hide();
                this->box_contents->removeWidget(this->widget_main_contents);
            }

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setFixedWidth(1500);
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Request HTTP API
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
         * @brief BugsTracksListAll::request_trackData
         * @param page
         */
    void BugsTracksListAll::request_more_trackData(){

        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // page number
            int page_reqNext = this->list_track->size() / GET_ITEM_SIZE___ONCE + 1;

            // getting track list - request HTTP API
            ProcBugsAPI *proc_track = new ProcBugsAPI(this);
            proc_track->setProperty("section", SECTION_FOR_MORE_POPUP___track);
            connect(proc_track, &ProcBugsAPI::completeReq_list_tracks, this, &BugsTracksListAll::slot_applyResult_recentlyTracks);
            proc_track->request_bugs_getList_tracks(this->data_pagaInfo.api_subPath, GET_ITEM_SIZE___ONCE, page_reqNext);
        }
    }


    void BugsTracksListAll::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > GET_ITEM_SIZE___ONCE ) ? GET_ITEM_SIZE___ONCE : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &BugsTracksListAll::slot_clickedItemTrack_inList);
            this->track_listAll[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->track_listAll[i]);
        }

        QList<QString> tmp_clientkey;
        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i]->setDataTrackInfo_Bugs(this->list_track->at(i), "");
            tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).track_id));

            QCoreApplication::processEvents();
        }

        this->track_favoriteOffset = start_index;
        roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
        connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsTracksListAll::slot_applyResult_getRating_track);
        proc_fav_track->request_rose_getRating_Track("BUGS", tmp_clientkey);

        if(this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_draw = false;
    }


    /**
         * @brief BugsTracksListAll::proc_wheelEvent_to_getMoreData
         */
    void BugsTracksListAll::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
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
    void BugsTracksListAll::slot_applyResult_recentlyTracks(const QList<bugs::TrackItemData>& list_data, const QJsonArray& jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            if(list_data.size() == 0){
                ContentLoadingwaitingMsgHide();      //cheon Tidal

                return;
            }

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
                this->track_totalCount = list_data.at(0).total_count;

                if(this->track_totalCount > 50){
                    this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
                }
            }

            int start_index = this->list_track->count();

            this->list_track->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                int max_cnt = this->list_track->size();
                this->track_drawCount = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &BugsTracksListAll::slot_clickedItemTrack_inList);
                    this->track_listAll[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->track_listAll[i]);\
                }

                QList<QString> tmp_clientkey;
                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i]->setDataTrackInfo_Bugs(this->list_track->at(i), "");
                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).track_id));

                    QCoreApplication::processEvents();
                }

                this->track_favoriteOffset = start_index;
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsTracksListAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("BUGS", tmp_clientkey);

                ContentLoadingwaitingMsgHide();      //cheon Tidal
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
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void BugsTracksListAll::slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌.
        int idx = ProcJsonEasy::getInt(p_jsonObj, "index");
        int track_id = ProcJsonEasy::getString(p_jsonObj, "data_id").toInt();

        if(track_id == this->list_track->at(idx).track_id){
            qDebug() << "likes_yn : " << ProcJsonEasy::getBool(p_jsonObj, "likes_yn");
        }

        this->flag_send_track = false;
    }

    void BugsTracksListAll::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("id")){

            int id = ProcJsonEasy::getInt(jsonObj, "id");

            if(id > 0){
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
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsTracksListAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
            }
            else{
                QJsonObject data = this->jsonArr_tracks_toPlay[this->track_idx_fav].toObject();

                QJsonArray tmpArtists = ProcJsonEasy::getJsonArray(data, "artists");

                QJsonArray artists;
                for(int i = 0; i < tmpArtists.size(); i++){
                    QJsonObject artistName = tmpArtists.at(i).toObject();
                    artists.append(ProcJsonEasy::getString(artistName, "artist_nm"));
                }

                QJsonObject track;
                track.insert("title", ProcJsonEasy::getString(data, "track_title"));
                track.insert("type", "BUGS");
                track.insert("favorite", false);
                track.insert("artists", artists);
                track.insert("albums", ProcJsonEasy::getJsonObject(data, "album"));
                track.insert("star", 0);
                track.insert("thumbnailUrl", this->list_track->at(this->track_idx_fav).album_image);
                track.insert("clientKey", ProcJsonEasy::getString(data, "id"));
                track.insert("duration", ProcJsonEasy::getString(data, "len"));
                track.insert("data", data);

                QJsonArray tracks;
                tracks.append(track);

                QJsonObject json;
                json.insert("tracks", tracks);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsTracksListAll::slot_applyResult_addRating_track);
                proc_fav_track->request_rose_addRating_Track(json);
            }
        }
        else{

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void BugsTracksListAll::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void BugsTracksListAll::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){

            for(int i = 0; i < contents.size(); i++){

                QJsonObject track = contents.at(i).toObject();

                if(track.contains("star")){
                    QString clientKey = ProcJsonEasy::getString(track, "clientKey");
                    int star = ProcJsonEasy::getInt(track, "star");

                    QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(i).toObject();
                    QString trackID = QString("%1").arg(ProcJsonEasy::getInt(trackInfo, "track_id"));

                    if(clientKey == trackID){

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


    /**
         * @brief TidalTracksListAll::slot_clickedItemTrack_inList
         * @param index
         * @param clickMode
         */
    void BugsTracksListAll::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___track;

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->track_listAll[idx]->getFavoritesStars();
                this->flag_track_fav = false;

                if(this->track_star_fav == 3){
                    this->track_star_fav = 0;

                    this->track_idx_fav = idx;
                    this->flag_track_fav = false;
                }
                else if(this->track_star_fav >= 0 && this->track_star_fav < 3){
                    this->track_star_fav++;

                    this->track_idx_fav = idx;
                    this->flag_track_fav = true;
                }

                if(this->track_star_fav == 0 || this->track_star_fav == 1){
                    // Bugs Favorite
                    ItemPositionData itemPosData;
                    itemPosData.section = SECTION_FOR_MORE_POPUP___track;
                    itemPosData.index = idx;
                    itemPosData.data_id = QString("%1").arg(this->list_track->at(idx).track_id);

                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    connect(proc, &bugs::ProcBugsAPI::completeReq_favarite_track, this, &BugsTracksListAll::slot_bugs_completeReq_listAll_myFavoritesIds);

                    if(this->track_star_fav == 0){
                        itemPosData.likes_yn = false;

                        proc->request_bugs_deleteFavorite_track(this->list_track->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }
                    else if(this->track_star_fav == 1){
                        itemPosData.likes_yn = true;

                        proc->request_bugs_addFavorite_track(this->list_track->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }

                    this->flag_send_track = true;
                }

                this->track_idx_fav = idx;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsTracksListAll::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("BUGS", QString("%1").arg(this->list_track->at(idx).track_id));

                this->flag_check_track = true;
            }
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }


    /**
         * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
         * @param clickMode
         * @param index
         * @param section
         */
    void BugsTracksListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }


    void BugsTracksListAll::slot_btnClicked_playAll(){
        this->proc_play_allTrack(this->jsonArr_tracks_toPlay, false);
    }


    void BugsTracksListAll::slot_btnClicked_playShuffle(){
        this->proc_play_allTrack(this->jsonArr_tracks_toPlay, true);
    }





    /**
         * @brief
         * @param parent
         */
    BugsMostTracksListAll::BugsMostTracksListAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_rosefilter, parent) {

        // data
        this->list_track = new QList<bugs::TrackItemData>();

        // margin 변경
        this->box_contents->setContentsMargins(0,0,0,0);
    }


    /**
         * @brief 소멸자.
         */
    BugsMostTracksListAll::~BugsMostTracksListAll(){

        this->deleteLater();
    }


    /**
         * @brief
         * @param jsonObj
         */
    void BugsMostTracksListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        bugs::PageInfo_TrackAllView tmp_pagaInfo = ConvertData_forBugs::convertData_pageInfo_trackAllView(jsonObj);
        this->flagNeedReload = false;

        if(this->data_pagaInfo.api_subPath != tmp_pagaInfo.api_subPath){
            this->flagNeedReload = true;

            this->data_pagaInfo = ConvertData_forBugs::convertData_pageInfo_trackAllView(jsonObj);

            GSCommon::clearLayout(this->box_mainTitle);
            this->label_mainTitle = this->get_addUIControl_mainTitle("Tracks");
            this->label_mainTitle->setText(this->data_pagaInfo.pathTitle);

            // init
            this->list_track->clear();
            this->jsonArr_tracks_toPlay = QJsonArray();

            this->track_id_fav = 0;
            this->track_idx_fav = 0;
            this->track_star_fav = 0;

            this->track_totalCount = 0;
            this->track_drawCount = 0;
            this->track_favoriteOffset = 0;

            this->flag_draw = false;
            this->flag_check_track = false;
            this->flag_track_fav = false;
            this->flag_send_track = false;

            // request HTTP API
            this->flagReqMore_track = false;
            this->flag_lastPage_track = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    void BugsMostTracksListAll::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            this->box_filter->removeWidget(this->widget_filter_contents);

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
            connect(btn_playAll, &QPushButton::clicked, this, &BugsMostTracksListAll::slot_btnClicked_playAll);

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
            connect(btn_shuffleAll, &QPushButton::clicked, this, &BugsMostTracksListAll::slot_btnClicked_playShuffle);

            this->box_filter->addWidget(this->widget_filter_contents);

            if(this->flag_widget_visible == true){
                this->widget_main_contents->hide();
                this->box_contents->removeWidget(this->widget_main_contents);
            }

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setFixedWidth(1500);
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Request HTTP API
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
         * @brief BugsMostTracksListAll::request_trackData
         * @param page
         */
    void BugsMostTracksListAll::request_more_trackData(){

        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // page number
            int page_reqNext = this->list_track->size() / GET_ITEM_SIZE___ONCE + 1;

            // getting track list - request HTTP API
            ProcBugsAPI *proc_track = new ProcBugsAPI(this);
            proc_track->setProperty("section", SECTION_FOR_MORE_POPUP___track);
            connect(proc_track, &ProcBugsAPI::completeReq_list_tracks, this, &BugsMostTracksListAll::slot_applyResult_mostTracks);
            proc_track->request_bugs_getList_tracks(this->data_pagaInfo.api_subPath, GET_ITEM_SIZE___ONCE, page_reqNext);
        }
    }


    void BugsMostTracksListAll::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > GET_ITEM_SIZE___ONCE ) ? GET_ITEM_SIZE___ONCE : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &BugsMostTracksListAll::slot_clickedItemTrack_inList);
            this->track_listAll[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->track_listAll[i]);
        }

        QList<QString> tmp_clientkey;
        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i]->setDataTrackInfo_Bugs(this->list_track->at(i), "");
            tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).track_id));

            QCoreApplication::processEvents();
        }

        this->track_favoriteOffset = start_index;
        roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
        connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsMostTracksListAll::slot_applyResult_getRating_track);
        proc_fav_track->request_rose_getRating_Track("BUGS", tmp_clientkey);

        if(this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_draw = false;
    }


    /**
         * @brief BugsTracksListAll::proc_wheelEvent_to_getMoreData
         */
    void BugsMostTracksListAll::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
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
    void BugsMostTracksListAll::slot_applyResult_mostTracks(const QList<bugs::TrackItemData>& list_data, const QJsonArray& jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            if(list_data.size() == 0){
                ContentLoadingwaitingMsgHide();      //cheon Tidal

                return;
            }

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
                this->track_totalCount = list_data.at(0).total_count;

                if(this->track_totalCount > 50){
                    this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
                }
            }

            int start_index = this->list_track->count();

            this->list_track->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                int max_cnt = this->list_track->size();
                this->track_drawCount = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &BugsMostTracksListAll::slot_clickedItemTrack_inList);
                    this->track_listAll[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->track_listAll[i]);\
                }

                QList<QString> tmp_clientkey;
                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i]->setDataTrackInfo_Bugs(this->list_track->at(i), "");
                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).track_id));

                    QCoreApplication::processEvents();
                }

                this->track_favoriteOffset = start_index;
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsMostTracksListAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("BUGS", tmp_clientkey);

                ContentLoadingwaitingMsgHide();      //cheon Tidal
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
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }

    void BugsMostTracksListAll::slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌.
        int idx = ProcJsonEasy::getInt(p_jsonObj, "index");
        int track_id = ProcJsonEasy::getString(p_jsonObj, "data_id").toInt();

        if(track_id == this->list_track->at(idx).track_id){
            qDebug() << "likes_yn : " << ProcJsonEasy::getBool(p_jsonObj, "likes_yn");
        }

        this->flag_send_track = false;
    }

    void BugsMostTracksListAll::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("id")){

            int id = ProcJsonEasy::getInt(jsonObj, "id");

            if(id > 0){
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
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsMostTracksListAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
            }
            else{
                QJsonObject data = this->jsonArr_tracks_toPlay[this->track_idx_fav].toObject();

                QJsonArray tmpArtists = ProcJsonEasy::getJsonArray(data, "artists");

                QJsonArray artists;
                for(int i = 0; i < tmpArtists.size(); i++){
                    QJsonObject artistName = tmpArtists.at(i).toObject();
                    artists.append(ProcJsonEasy::getString(artistName, "artist_nm"));
                }

                QJsonObject track;
                track.insert("title", ProcJsonEasy::getString(data, "track_title"));
                track.insert("type", "BUGS");
                track.insert("favorite", false);
                track.insert("artists", artists);
                track.insert("albums", ProcJsonEasy::getJsonObject(data, "album"));
                track.insert("star", 0);
                track.insert("thumbnailUrl", this->list_track->at(this->track_idx_fav).album_image);
                track.insert("clientKey", ProcJsonEasy::getString(data, "id"));
                track.insert("duration", ProcJsonEasy::getString(data, "len"));
                track.insert("data", data);

                QJsonArray tracks;
                tracks.append(track);

                QJsonObject json;
                json.insert("tracks", tracks);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsMostTracksListAll::slot_applyResult_addRating_track);
                proc_fav_track->request_rose_addRating_Track(json);
            }
        }
        else{

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void BugsMostTracksListAll::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void BugsMostTracksListAll::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){

            for(int i = 0; i < contents.size(); i++){

                QJsonObject track = contents.at(i).toObject();

                if(track.contains("star")){
                    QString clientKey = ProcJsonEasy::getString(track, "clientKey");
                    int star = ProcJsonEasy::getInt(track, "star");

                    QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(i).toObject();
                    QString trackID = QString("%1").arg(ProcJsonEasy::getInt(trackInfo, "track_id"));

                    if(clientKey == trackID){

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


    /**
         * @brief TidalTracksListAll::slot_clickedItemTrack_inList
         * @param index
         * @param clickMode
         */
    void BugsMostTracksListAll::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___track;

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->track_listAll[idx]->getFavoritesStars();
                this->flag_track_fav = false;

                if(this->track_star_fav == 3){
                    this->track_star_fav = 0;

                    this->track_idx_fav = idx;
                    this->flag_track_fav = false;
                }
                else if(this->track_star_fav >= 0 && this->track_star_fav < 3){
                    this->track_star_fav++;

                    this->track_idx_fav = idx;
                    this->flag_track_fav = true;
                }

                if(this->track_star_fav == 0 || this->track_star_fav == 1){
                    // Bugs Favorite
                    ItemPositionData itemPosData;
                    itemPosData.section = SECTION_FOR_MORE_POPUP___track;
                    itemPosData.index = idx;
                    itemPosData.data_id = QString("%1").arg(this->list_track->at(idx).track_id);

                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    connect(proc, &bugs::ProcBugsAPI::completeReq_favarite_track, this, &BugsMostTracksListAll::slot_bugs_completeReq_listAll_myFavoritesIds);

                    if(this->track_star_fav == 0){
                        itemPosData.likes_yn = false;

                        proc->request_bugs_deleteFavorite_track(this->list_track->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }
                    else if(this->track_star_fav == 1){
                        itemPosData.likes_yn = true;

                        proc->request_bugs_addFavorite_track(this->list_track->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }

                    this->flag_send_track = true;
                }

                this->track_idx_fav = idx;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsMostTracksListAll::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("BUGS", QString("%1").arg(this->list_track->at(idx).track_id));

                this->flag_check_track = true;
            }
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }


    /**
         * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
         * @param clickMode
         * @param index
         * @param section
         */
    void BugsMostTracksListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }


    void BugsMostTracksListAll::slot_btnClicked_playAll(){
        this->proc_play_allTrack(this->jsonArr_tracks_toPlay, false);
    }


    void BugsMostTracksListAll::slot_btnClicked_playShuffle(){
        this->proc_play_allTrack(this->jsonArr_tracks_toPlay, true);
    }
}
