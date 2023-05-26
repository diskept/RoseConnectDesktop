#include "BugsMyAlbumDetail.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "common/rosesettings.h"

#include "widget/NoData_Widget.h"

#include <QSettings>
#include <QScrollBar>
#include <QResizeEvent>
#include <QPixmapCache>


namespace bugs {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___track = 1;

    const int GET_ITEM_SIZE___ONCE = 30;


    /**
     * @brief
     * @param parent
     */
    BugsMyAlbumDetail::BugsMyAlbumDetail(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_rosefilter, parent) {
        // data
        this->list_track = new QList<bugs::TrackItemData>();
        this->list_filterOpt_sort = AbstractBugsSubWidget::get_list_filterOptData_forMyCollection();

        if(global.user_forBugs.isLogined() == true){
            // UI
            this->setUIControl_tracks();
        }
    }


    /**
     * @brief 소멸자.
     */
    BugsMyAlbumDetail::~BugsMyAlbumDetail(){

        this->deleteLater();
    }


    /**
     * @brief BugsMyAlbumDetail::setJsonObject_forData
     * @param jsonObj
     */
    void BugsMyAlbumDetail::setJsonObject_forData(const QJsonObject &jsonObj){


        bugs::MyAlbumItemData data_param = ConvertData_forBugs::convertData_my_albumData(jsonObj);
        int tmp_playlist_id = this->playlist_id;

        this->flagNeedReload = false;

        if(tmp_playlist_id != data_param.playlist_id){
            this->flagNeedReload = true;

            if(this->flag_widget_visible == true){
                this->widget_main_contents->hide();
                this->flag_widget_visible = false;
            }

            this->label_mainTitle->setText(data_param.title);

            this->playlist_id = data_param.playlist_id;

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

    /**
     * @brief
     * HTTP API 호출한다.
    */
    void BugsMyAlbumDetail::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            /*// init
            this->chooseFilterOpt->hideFilterBox();

            // init Data
            this->selected_filterCode = this->list_filterOpt_sort.first().opt_code;         // default

            QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
            QString def_name = this->list_filterOpt_sort.first().opt_name;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(def_code, def_name);*/

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
            connect(btn_playAll, &QPushButton::clicked, this, &BugsMyAlbumDetail::slot_btnClicked_playAll);

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
            connect(btn_shuffleAll, &QPushButton::clicked, this, &BugsMyAlbumDetail::slot_btnClicked_playShuffle);

            this->box_filter->addWidget(this->widget_filter_contents);

            this->box_contents->removeWidget(this->widget_main_contents);

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


    // MARK : Create UI and Set --------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void BugsMyAlbumDetail::setUIControl_tracks(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Tracks");

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // filtering
        //this->setUiControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &BugsMyAlbumDetail::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyAlbumAll::setUiControl_filter
     */
    void BugsMyAlbumDetail::setUiControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &BugsMyAlbumDetail::slot_filterClicked);


        QHBoxLayout *hBox_filter = new QHBoxLayout();
        hBox_filter->setSpacing(0);
        hBox_filter->setContentsMargins(0,0,0,0);
        hBox_filter->addWidget(this->filterWidget, 0, Qt::AlignVCenter | Qt::AlignLeft);
        hBox_filter->addStretch(1);
        hBox_filter->addSpacing(77);

        QWidget *tmp_widget_forFilter = new QWidget();
        tmp_widget_forFilter->setContentsMargins(0,0,0,0);
        tmp_widget_forFilter->setObjectName("widget_forFilter");
        tmp_widget_forFilter->setStyleSheet("#widget_forFilter { background-color:#171717; }");
        tmp_widget_forFilter->setLayout(hBox_filter);
        tmp_widget_forFilter->setFixedHeight(70);

        this->box_mainTitle->addWidget(tmp_widget_forFilter);
        this->addUIControl_dividedLine_onMain();
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void BugsMyAlbumDetail::initAll(){

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

        this->widget_main_contents->hide();
        this->box_contents->removeWidget(this->widget_main_contents);
        GSCommon::clearLayout(this->box_contents);

        this->box_main_contents = new QVBoxLayout();
        this->box_main_contents->setSpacing(0);
        this->box_main_contents->setContentsMargins(0, 0, 0, 0);

        this->widget_main_contents = new QWidget();
        this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
        this->widget_main_contents->setLayout(this->box_main_contents);

        this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void BugsMyAlbumDetail::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void BugsMyAlbumDetail::resizeEvent(QResizeEvent *event){

        AbstractBugsSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }
    }


    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void BugsMyAlbumDetail::slot_filterClicked(){

        if(this->chooseFilterOpt->isVisible()){
            this->chooseFilterOpt->hideFilterBox();
        }
        else{
            // Filter Box Show
            this->chooseFilterOpt->clearFilter();
            this->chooseFilterOpt->addFilterGroup("정렬", this->list_filterOpt_sort, this->selected_filterCode);
            this->chooseFilterOpt->showFilterBox(QPoint(30, 130), this->size());
        }
    }

    /**
     * @brief 필터링 옵션박스 Widget에서 필터링을 하나 클릭한 경우이다.
     * @param selectedCode
     */
    void BugsMyAlbumDetail::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

        // 코드가 다른 경우에 대해서만 처리한다.
        if(this->selected_filterCode != selected_filterCode){

            // update
            this->selected_filterCode = selected_filterCode;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(selected_filterCode.toString(), selected_filterName);

            // clear - 옵션코드가 바뀌었으므로 모두 초기화한다.
            this->initAll();

            this->scrollTop();
            this->request_more_trackData();

        }

    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Request HTTP API
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief Track 정보를 요청 (Bugs API)
     */
    void BugsMyAlbumDetail::request_more_trackData(){

        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // page number
            int page_reqNext = this->list_track->size() / GET_ITEM_SIZE___ONCE + 1;

            // request HTTP API -
            ProcBugsAPI *proc = new ProcBugsAPI(this);
            proc->setProperty("section", SECTION_FOR_MORE_POPUP___track);
            connect(proc, &ProcBugsAPI::completeReq_list_items_of_my_album, this, &BugsMyAlbumDetail::slot_applyResult_tracks);
            proc->request_bugs_getList_items_of_my_album(this->playlist_id, page_reqNext, GET_ITEM_SIZE___ONCE);
        }
    }

    void BugsMyAlbumDetail::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > 100 ) ? 100 : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &BugsMyAlbumDetail::slot_clickedItemTrack_inList);
            this->track_listAll[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->track_listAll[i]);
        }

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i]->setDataTrackInfo_Bugs(this->list_track->at(i), "Recently");

            QCoreApplication::processEvents();
        }

        if(this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_draw = false;
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
    void BugsMyAlbumDetail::slot_applyResult_tracks(const QList<bugs::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
            }

            int start_index = this->list_track->count();

            this->list_track->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                int max_cnt = this->list_track->size();
                this->track_drawCount = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &BugsMyAlbumDetail::slot_clickedItemTrack_inList);
                    this->track_listAll[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->track_listAll[i]);\
                }

                QList<QString> tmp_clientkey;
                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i]->setDataTrackInfo_Bugs(this->list_track->at(i), "Recently");

                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).track_id));
                    QCoreApplication::processEvents();
                }
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsMyAlbumDetail::slot_applyResult_getRating_track);
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


    void BugsMyAlbumDetail::slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌.
        int idx = ProcJsonEasy::getInt(p_jsonObj, "index");
        int track_id = ProcJsonEasy::getString(p_jsonObj, "data_id").toInt();

        if(track_id == this->list_track->at(idx).track_id){
            qDebug() << "likes_yn : " << ProcJsonEasy::getBool(p_jsonObj, "likes_yn");
        }

        this->flag_send_track = false;
    }


    void BugsMyAlbumDetail::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

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
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsMyAlbumDetail::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
            }
            else{

                QJsonObject data = this->jsonArr_tracks_toPlay[this->track_idx_fav].toObject();


                QJsonObject track;
                track.insert("clientKey", ProcJsonEasy::getInt(data, "track_id"));
                track.insert("duration", 0);
                track.insert("favorite", true);
                track.insert("ownerId", 0);
                track.insert("sort", 0);
                track.insert("star", 1);
                track.insert("thumbnailUrl", this->list_track->at(this->track_idx_fav).album_image);
                track.insert("title", ProcJsonEasy::getString(data, "track_title"));
                track.insert("type", "BUGS");
                track.insert("data", data);

                QJsonArray tracks;
                tracks.append(track);

                QJsonObject json;
                json.insert("tracks", tracks);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsMyAlbumDetail::slot_applyResult_addRating_track);
                proc_fav_track->request_rose_addRating_Track(json);
                proc_fav_track->setProperty("whatTrack",sender()->property("whatTrack").toString());
            }
        }
        else{

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void BugsMyAlbumDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->track_listAll[i]->setFavoritesIds(flag, star);
                }

            }
            else if(track_info.contains("message") && (track_info["message"].toString() == "정상")){
                this->track_listAll[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }

    void BugsMyAlbumDetail::slot_applyResult_addRating_track(const QJsonObject &jsonObj){


        if(jsonObj.contains("flagOk")){

            if(jsonObj.contains("message") && (jsonObj["message"].toString() == "정상")){
                this->track_listAll[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);

            }
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void BugsMyAlbumDetail::slot_btnClicked_playAll(){

        // 전체재생
        this->proc_play_allTrack(this->jsonArr_tracks_toPlay, false);
    }


    void BugsMyAlbumDetail::slot_btnClicked_playShuffle(){

        // 셔플재생
        this->proc_play_allTrack(this->jsonArr_tracks_toPlay, true);
    }


    /**
     * @brief QobuzTracksListAll::slot_clickedItemTrack_inList
     * @param index
     * @param clickMode
     */
    void BugsMyAlbumDetail::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

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
                    connect(proc, &bugs::ProcBugsAPI::completeReq_favarite_track, this, &BugsMyAlbumDetail::slot_bugs_completeReq_listAll_myFavoritesIds);

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
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsMyAlbumDetail::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("BUGS", QString("%1").arg(this->list_track->at(idx).track_id));

                this->flag_check_track = true;
            }
        }
        else{
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, SECTION_FOR_MORE_POPUP___track);
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void BugsMyAlbumDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }

}
