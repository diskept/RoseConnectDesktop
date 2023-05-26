#include "roseHome/rosehomeTrackListAll.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/bugs_struct.h"
#include "bugs/ProcBugsAPI.h"

#include "qobuz/ProcCommon_forQobuz.h"

#include "tidal/ProcCommon.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"
#include "widget/toastmsg.h"
#include "widget/VerticalScrollArea.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QDebug>
#include <QLabel>
#include <QScrollBar>
#include <QPushButton>
#include <QPixmapCache>


namespace roseHome {

    const int SECTION_FOR_MORE_POPUP___tracks = 0;

    const QString ICON_PATH___playAll = ":/images/icon_list_allplay.png";
    const QString ICON_PATH___playShuffle = ":/images/icon_list_shuffle.png";

    /**
     * @brief "ROSEHOME > RECENT TRACK" 화면을 위한 생성자.
     * @see PAGECODE_RH_TRACK_LIST_ALL_VIEW
     * @param parent
     */
    RoseHomeTrackListAll::RoseHomeTrackListAll(QWidget *parent) : AbstractRoseHomeSubWidget(VerticalScroll_rosefilter, parent) {

        // data
        this->list_track = new QList<roseHome::TrackItemData>();

        // UI
        this->setUIControl_tracks();
    }


    /**
    * @brief 소멸자.
    */
    RoseHomeTrackListAll::~RoseHomeTrackListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void RoseHomeTrackListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        PageInfo_TrackAllView data_pageInfo = ConvertData::convertData_pageInfo_trackAllView(jsonObj);
        this->flagNeedReload = false;

        if(this->current_pageInfo.type != data_pageInfo.type || this->current_pageInfo.api_subPath != data_pageInfo.api_subPath){
            this->flagNeedReload = true;

            this->current_pageInfo = data_pageInfo;

            this->label_mainTitle->setText(this->current_pageInfo.pathTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

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
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseHomeTrackListAll::slot_applyResult_tracklistCheck);
            proc->request_rose_getList_recentlyTracks(this->current_pageInfo.api_subPath, this->current_pageInfo.filter_type, 0, 10);
        }
    }


    /**
     * @brief RoseHomeTrackListAll::setActivePage
     */
    void RoseHomeTrackListAll::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

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
            connect(btn_playAll, &QPushButton::clicked, this, &RoseHomeTrackListAll::slot_btnClicked_playAll);

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
            connect(btn_shuffleAll, &QPushButton::clicked, this, &RoseHomeTrackListAll::slot_btnClicked_playShuffle);

            this->box_filter->addWidget(this->widget_filter_contents);

            if(this->flag_widget_visible == true){
                this->widget_main_contents->hide();
                this->box_contents->removeWidget(this->widget_main_contents);

                this->flag_widget_visible = false;
            }

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);

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
    void RoseHomeTrackListAll::setUIControl_tracks(){

        GSCommon::clearLayout(this->box_mainTitle);

        this->widget_mainTitle = new QWidget();
        this->widget_mainTitle->setStyleSheet("background-color:#171717;");
        this->widget_mainTitle->setMinimumSize(800, 69);
        this->widget_mainTitle->resize(global.LmtCnt, 69);

        this->label_mainTitle = new QLabel(this->widget_mainTitle);
        this->label_mainTitle->setStyleSheet("background-color:transparent; font-size:26px; color:#FFFFFF;");
        this->label_mainTitle->setText("");
        this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

        this->btn_mainTitle = new QPushButton(this->widget_mainTitle);
        this->btn_mainTitle->setObjectName("btn_mainTitle");
        this->btn_mainTitle->setCursor(Qt::PointingHandCursor);
        this->btn_mainTitle->setStyleSheet("background-color:transparent;");

        connect(this->btn_mainTitle, &QPushButton::clicked, this, &RoseHomeTrackListAll::slot_btnClicked_Delete);

        this->label_delete_icon = GSCommon::getUILabelImg(":/images/q_del_ico.png", this->btn_mainTitle);
        this->label_delete_icon->setGeometry(0, 10, 50, 50);

        this->label_delete = new QLabel(this->btn_mainTitle);
        this->label_delete->setText(tr("Delete"));
        this->label_delete->setStyleSheet("background-color:transparent;font-size:20px;color:#FFFFFF;");
        this->label_delete->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        this->label_delete->setGeometry(60, 23, this->label_delete->sizeHint().width(), this->label_delete->sizeHint().height());

        this->btn_mainTitle->setGeometry(global.LmtCnt - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);

        this->box_mainTitle->addWidget(this->widget_mainTitle);

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void RoseHomeTrackListAll::initAll(){

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

        if(this->flag_widget_visible == true){
            this->widget_main_contents->hide();
            this->box_contents->removeWidget(this->widget_main_contents);

            this->flag_widget_visible = false;
        }

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        this->box_main_contents = new QVBoxLayout();
        this->box_main_contents->setSpacing(0);
        this->box_main_contents->setContentsMargins(0, 0, 0, 0);

        this->widget_main_contents = new QWidget();
        this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
        this->widget_main_contents->setLayout(this->box_main_contents);

        this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);

        this->vBox_tracks = new QVBoxLayout();
        this->vBox_tracks->setSpacing(0);
        this->vBox_tracks->setContentsMargins(0, 0, 0, 0);
        GSCommon::clearLayout(this->vBox_tracks);

        this->box_main_contents->addLayout(this->vBox_tracks);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void RoseHomeTrackListAll::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void RoseHomeTrackListAll::request_more_trackData(){

        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // next_offset
            if(this->list_track->size() == 0){
                this->next_offset = 0;
            }
            else{
                this->next_offset++;
            }

            // request HTTP API
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseHomeTrackListAll::slot_applyResult_tracks);
            proc->setProperty("idx", this->next_offset);
            proc->request_rose_getList_recentlyTracks(this->current_pageInfo.api_subPath, this->current_pageInfo.filter_type, this->next_offset, 100);
        }
    }


    void RoseHomeTrackListAll::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > 100 ) ? 100 : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &RoseHomeTrackListAll::slot_clickedItemTrack_inList);
            this->track_listAll[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->track_listAll[i]);
        }

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i]->setDataTrackInfo_Rose(this->list_track->at(i));
            this->track_listAll[i]->setFavoritesIds(this->list_track->at(i).favorite, this->list_track->at(i).star);

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
    void RoseHomeTrackListAll::slot_applyResult_tracks(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        int idx = sender()->property("idx").toInt();

        if(idx != this->next_offset){
            return;
        }

        if(list_data.size() > 0 || (list_data.size() == 0 && this->list_track->count() != 0)){
            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
                this->track_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_track->count();

            this->list_track->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                int max_cnt = this->list_track->size();
                this->track_drawCount = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &RoseHomeTrackListAll::slot_clickedItemTrack_inList);
                    this->track_listAll[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->track_listAll[i]);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i]->setDataTrackInfo_Rose(this->list_track->at(i));
                    this->track_listAll[i]->setFavoritesIds(this->list_track->at(i).favorite, this->list_track->at(i).star);

                    QCoreApplication::processEvents();
                }

                ContentLoadingwaitingMsgHide();
                this->flag_widget_visible = true;

                if(flag_lastPage == false){
                    this->request_more_trackData();
                }
                else{
                    this->box_main_contents->addSpacing(30);
                }
            }
            else{
                if(flag_lastPage == false){
                    this->request_more_trackData();
                }
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->list_track->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
                noData_widget->setFixedSize(1500, 500);

                this->box_main_contents->addWidget(noData_widget);
            }
        }
    }


    void RoseHomeTrackListAll::slot_applyResult_tracklistCheck(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;
        //int change_star = 0;

        if(list_data.size() > 0){
            if(this->list_track->count() == 0){
                QString tmpTitle = tr("Recently played tracks") + QString(" (%1)").arg(list_data.at(0).totalCount);
                this->label_mainTitle->setText(tmpTitle);
                this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                this->initAll();

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                this->request_more_trackData();

                this->flagNeedReload = true;
                this->setActivePage();
            }
            else{
                for(int i = 0; i < list_data.count(); i++){
                    if(list_data.at(i).title != this->list_track->at(i).title){
                        change_flag++;
                    }

//                    if(list_data.at(i).star != this->list_track->at(i).star){
//                        change_star++;
//                    }
                }

                if((list_data.at(0).totalCount != this->list_track->at(0).totalCount) || (change_flag > 0)){

                    QString tmpTitle = tr("Recently played tracks") + QString(" (%1)").arg(list_data.at(0).totalCount);
                    this->label_mainTitle->setText(tmpTitle);
                    this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                    this->initAll();

                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                    this->request_more_trackData();

                    this->flagNeedReload = true;
                    this->setActivePage();
                }
//                else if((list_data.at(0).totalCount == this->list_track->at(0).totalCount) || (change_star > 0)){

//                }
                else{
                    ContentLoadingwaitingMsgHide();
                }
            }
        }
        else if(list_data.size() == 0){
            QString tmpTitle = tr("Recently played tracks (0)");
            this->label_mainTitle->setText(tmpTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

            this->initAll();

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackData();

            this->flagNeedReload = true;
            this->setActivePage();
        }
        else{
            ContentLoadingwaitingMsgHide();
        }
    }


    void RoseHomeTrackListAll::slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고. => OptMorePopup 에서 하라고, 가려줌
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
            bool status  = ProcJsonEasy::getBool(p_jsonObj, "status");

            // Qobuz favorite toggle check
            if(this->flag_send_track == true){
                if((status == true && this->flag_track_fav == false) || (status == false && this->flag_track_fav == true)){
                    // Tidal Favorite toggle
                    //ProcCommon *proc = new ProcCommon(this);
                    //connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &BugsRecentlyTrackAll::slot_bugs_completeReq_listAll_myFavoritesIds);
                    //proc->request_tidal_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                }
                //this->flag_send_track = false;
            }
        }
    }

    void RoseHomeTrackListAll::slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고.
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            // Tidal favorite check
            if(this->flag_send_track == true){
                QVariantList arr_myFavoriteIds = ProcJsonEasy::getJsonArray(p_jsonObj, "TRACK").toVariantList();
                bool status = arr_myFavoriteIds.contains(this->track_id_fav);

                if(status == true && this->flag_track_fav == false){
                    // Tidal track Favorite del
                    tidal::ProcCommon *proc = new tidal::ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &RoseHomeTrackListAll::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                else if(status == false && this->flag_track_fav == true){
                    // Tidal track Favorite add
                    tidal::ProcCommon *proc = new tidal::ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &RoseHomeTrackListAll::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                this->flag_send_track = false;
            }
            else{

            }
        }
    }

    void RoseHomeTrackListAll::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->track_listAll[this->track_favoriteOffset + i]->setFavoritesIds(flag, star);
                }
            }
            else if(track_info.contains("message") && (track_info["message"].toString() == "정상")){
                this->track_listAll[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void RoseHomeTrackListAll::slot_btnClicked_playAll(){

        roseHome::TrackItemData data_track;

        // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
        procRosePlay->requestPlayRose_byTracks(data_track, jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow_withReorder, AbstractProcRosePlay::PlayShuffleMode::JustPlay);
    }


    void RoseHomeTrackListAll::slot_btnClicked_playShuffle(){

        roseHome::TrackItemData data_track;

        // Track shuffle 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
        procRosePlay->requestPlayRose_byTracks(data_track, jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow_withReorder, AbstractProcRosePlay::PlayShuffleMode::ShufflePlay);
    }


    void RoseHomeTrackListAll::slot_btnClicked_Delete(){

        QStringList tmpTitle = this->current_pageInfo.pathTitle.split("(");

        QJsonObject tmpObj;
        tmpObj.insert("contents_type", tmpTitle.at(0));
        tmpObj.insert("list_type", "TRACK");
        tmpObj.insert("api_subPath", this->current_pageInfo.api_subPath);
        tmpObj.insert("filter_type", this->current_pageInfo.filter_type);

        QJsonObject tmpObj_move;
        tmpObj_move.insert(KEY_PAGE_CODE, PAGECODE_RH_RECENTLY_LIST_DELETE);
        tmpObj_move.insert(KEY_DATA, tmpObj);

        emit this->linker->signal_clickedMovePage(tmpObj_move);
    }


    /**
     * @brief [Slot] PlaylistTrackDetailInfo_RHV signal에 대하여 처리
     * @param clickMode
     */
    void RoseHomeTrackListAll::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->track_listAll[idx]->getFavoritesStars();
                this->flag_track_fav = false;

                QString type = this->list_track->at(idx).type;

                if(this->track_star_fav == 3){
                    this->track_star_fav = 0;
                    this->flag_track_fav = false;
                }
                else if(this->track_star_fav >= 0 && this->track_star_fav < 3){
                    this->track_star_fav++;
                    this->flag_track_fav = true;
                }
                if(this->track_star_fav == 0 || this->track_star_fav == 1){
                    if(type == "BUGS"){
                        // Bugs Favorite toggle
                        bugs::ItemPositionData itemPosData;
                        itemPosData.section = SECTION_FOR_MORE_POPUP___tracks;
                        itemPosData.index = idx;
                        itemPosData.data_id = QString("%1").arg(this->list_track->at(idx).id);

                        bugs::ProcBugsAPI *proc = new bugs::ProcBugsAPI(this);
                        connect(proc, &bugs::ProcBugsAPI::completeReq_favarite_track, this, &RoseHomeTrackListAll::slot_bugs_completeReq_listAll_myFavoritesIds);

                        if(this->track_star_fav == 0){
                            itemPosData.likes_yn = false;

                            proc->request_bugs_deleteFavorite_track(this->list_track->at(idx).clientKey.toInt(), bugs::ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                        }
                        else if(this->track_star_fav == 1){
                            itemPosData.likes_yn = true;

                            proc->request_bugs_addFavorite_track(this->list_track->at(idx).clientKey.toInt(), bugs::ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                        }

                        this->flag_send_track = true;
                    }
                    else if(type == "QOBUZ"){
                        this->track_id_fav = this->list_track->at(idx).clientKey.toInt();

                        qobuz::ProcCommon *proc = new qobuz::ProcCommon(this);
                        connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &RoseHomeTrackListAll::slot_bugs_completeReq_listAll_myFavoritesIds);
                        proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                        this->flag_send_track = true;
                    }
                    else if(type == "TIDAL"){
                        this->track_id_fav = this->list_track->at(idx).clientKey.toInt();

                        if(this->track_star_fav == 1){
                            // Tidal Track Favorite add
                            tidal::ProcCommon *proc = new tidal::ProcCommon(this);
                            proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                        }
                        else if(this->track_star_fav == 0){
                            // Tidal Track Favorite del
                            tidal::ProcCommon *proc = new tidal::ProcCommon(this);
                            proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                        }

                        // favorite 정보 가져오기
                        tidal::ProcCommon *proc_fav = new tidal::ProcCommon(this);
                        connect(proc_fav, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &RoseHomeTrackListAll::slot_tidal_completeReq_listAll_myFavoritesIds);
                        proc_fav->request_tidal_getAll_favorites();
                        this->flag_send_track = true;
                    }
                }

                this->track_idx_fav = idx;

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_track_fav);
                ratingInfo.insert("star", this->track_star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", this->list_track->at(idx).type);

                QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseHomeTrackListAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

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
    void RoseHomeTrackListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }


    /**
     * @brief RoseHomeTrackListAll::resizeEvent
     * @param event
     */
    void RoseHomeTrackListAll::resizeEvent(QResizeEvent *event){
        Q_UNUSED(event);

        this->widget_mainTitle->resize(global.LmtCnt, 69);
        this->btn_mainTitle->setGeometry(global.LmtCnt - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);
    }





    /**
     * @brief "ROSEHOME > REcENT TRACK" 화면을 위한 생성자. @see PAGECODE_RH_TRACK_LIST_ALL_VIEW
     * @param parent
     */
    RoseHomeTrackHistoryAll::RoseHomeTrackHistoryAll(QWidget *parent) : AbstractRoseHomeSubWidget(VerticalScroll_rosefilter, parent) {

        // data
        this->list_track = new QList<roseHome::TrackItemData>();

        // UI
        this->setUIControl_tracks();
    }


    /**
    * @brief 소멸자.
    */
    RoseHomeTrackHistoryAll::~RoseHomeTrackHistoryAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void RoseHomeTrackHistoryAll::setJsonObject_forData(const QJsonObject &jsonObj){

        PageInfo_TrackAllView tmp_pageInfo = ConvertData::convertData_pageInfo_trackAllView(jsonObj);
        this->flagNeedReload = false;

        if(tmp_pageInfo.api_subPath != this->data_pageInfo.api_subPath || tmp_pageInfo.pathTitle != this->data_pageInfo.pathTitle || tmp_pageInfo.filter_type != this->data_pageInfo.filter_type){
            this->flagNeedReload = true;

            this->data_pageInfo = tmp_pageInfo;

            this->label_mainTitle->setText(this->data_pageInfo.pathTitle);

            if(this->flag_widget_visible == true){
                this->widget_main_contents->hide();
                this->box_contents->removeWidget(this->widget_main_contents);
                GSCommon::clearLayout(this->box_contents);

                this->flag_widget_visible = false;
            }

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


    /**
     * @brief RoseHomeTrackListAll::setActivePage
     */
    void RoseHomeTrackHistoryAll::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

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
            connect(btn_playAll, &QPushButton::clicked, this, &RoseHomeTrackHistoryAll::slot_btnClicked_playAll);

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
            connect(btn_shuffleAll, &QPushButton::clicked, this, &RoseHomeTrackHistoryAll::slot_btnClicked_playShuffle);

            this->box_filter->addWidget(this->widget_filter_contents);


            this->box_contents->removeWidget(this->widget_main_contents);
            GSCommon::clearLayout(this->box_contents);

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
    void RoseHomeTrackHistoryAll::setUIControl_tracks(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("");
        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();
    }

    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void RoseHomeTrackHistoryAll::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }

    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void RoseHomeTrackHistoryAll::request_more_trackData(){

        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // next_offset
            if(this->list_track->size() == 0){
                this->next_offset = 0;
            }
            else{
                this->next_offset++;
            }

            // request HTTP API
            roseHome::ProcCommon *proc_track = new roseHome::ProcCommon(this);
            connect(proc_track, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseHomeTrackHistoryAll::slot_applyResult_historyTrack);
            proc_track->setProperty("idx", this->next_offset);
            proc_track->request_rose_getList_historyTracks(this->data_pageInfo.api_subPath, this->data_pageInfo.filter_type, this->next_offset, 20);
        }
    }

    void RoseHomeTrackHistoryAll::request_more_trackDraw(){

        this->flag_draw = true;

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > 20 ) ? 20 : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &RoseHomeTrackHistoryAll::slot_clickedItemTrack_inList);
            this->track_listAll[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->track_listAll[i]);
        }

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i]->setDataTrackInfo_Rose(this->list_track->at(i));
            this->track_listAll[i]->setFavoritesIds(this->list_track->at(i).favorite, this->list_track->at(i).star);

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
    void RoseHomeTrackHistoryAll::slot_applyResult_historyTrack(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            if(list_data.size() == 0){
                ContentLoadingwaitingMsgHide();
                return;
            }

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
                this->track_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_track->count();

            this->list_track->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                int max_cnt = this->list_track->size();
                this->track_drawCount = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &RoseHomeTrackHistoryAll::slot_clickedItemTrack_inList);
                    this->track_listAll[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->track_listAll[i]);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i]->setDataTrackInfo_Rose(this->list_track->at(i));
                    this->track_listAll[i]->setFavoritesIds(this->list_track->at(i).favorite, this->list_track->at(i).star);

                    QCoreApplication::processEvents();
                }

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

            if(this->list_track->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->box_main_contents->addWidget(noData_widget);
            }
        }
    }

    void RoseHomeTrackHistoryAll::slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고. => OptMorePopup 에서 하라고, 가려줌
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
            bool status  = ProcJsonEasy::getBool(p_jsonObj, "status");

            // Qobuz favorite toggle check
            if(this->flag_send_track == true){
                if((status == true && this->flag_track_fav == false) || (status == false && this->flag_track_fav == true)){
                    // Tidal Favorite toggle
                    //ProcCommon *proc = new ProcCommon(this);
                    //connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &BugsRecentlyTrackAll::slot_bugs_completeReq_listAll_myFavoritesIds);
                    //proc->request_tidal_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                }
                //this->flag_send_track = false;
            }
        }
    }

    void RoseHomeTrackHistoryAll::slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고.
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            // Tidal favorite check
            if(this->flag_send_track == true){
                QVariantList arr_myFavoriteIds = ProcJsonEasy::getJsonArray(p_jsonObj, "TRACK").toVariantList();
                bool status = arr_myFavoriteIds.contains(this->track_id_fav);

                if(status == true && this->flag_track_fav == false){
                    // Tidal track Favorite del
                    tidal::ProcCommon *proc = new tidal::ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &RoseHomeTrackHistoryAll::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                else if(status == false && this->flag_track_fav == true){
                    // Tidal track Favorite add
                    tidal::ProcCommon *proc = new tidal::ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &RoseHomeTrackHistoryAll::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                this->flag_send_track = false;
            }
            else{

            }
        }
    }

    void RoseHomeTrackHistoryAll::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->track_listAll[this->track_favoriteOffset + i]->setFavoritesIds(flag, star);
                }
            }
            else if(track_info.contains("message") && (track_info["message"].toString() == "정상")){
                this->track_listAll[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void RoseHomeTrackHistoryAll::slot_btnClicked_playAll(){

        roseHome::TrackItemData data_track;

        // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
        procRosePlay->requestPlayRose_byTracks(data_track, jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow_withReorder, AbstractProcRosePlay::PlayShuffleMode::JustPlay);
    }

    void RoseHomeTrackHistoryAll::slot_btnClicked_playShuffle(){

        roseHome::TrackItemData data_track;

        // Track shuffle 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
        procRosePlay->requestPlayRose_byTracks(data_track, jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow_withReorder, AbstractProcRosePlay::PlayShuffleMode::ShufflePlay);
    }


    /**
     * @brief [Slot] PlaylistTrackDetailInfo_RHV signal에 대하여 처리
     * @param clickMode
     */
    void RoseHomeTrackHistoryAll::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->track_listAll[idx]->getFavoritesStars();
                this->flag_track_fav = false;

                QString type = this->list_track->at(idx).type;

                if(this->track_star_fav == 3){
                    this->track_star_fav = 0;
                    this->flag_track_fav = false;
                }
                else if(this->track_star_fav >= 0 && this->track_star_fav < 3){
                    this->track_star_fav++;
                    this->flag_track_fav = true;
                }
                if(this->track_star_fav == 0 || this->track_star_fav == 1){
                    if(type == "BUGS"){
                        // Bugs Favorite toggle
                        bugs::ItemPositionData itemPosData;
                        itemPosData.section = SECTION_FOR_MORE_POPUP___tracks;
                        itemPosData.index = idx;
                        itemPosData.data_id = QString("%1").arg(this->list_track->at(idx).id);

                        bugs::ProcBugsAPI *proc = new bugs::ProcBugsAPI(this);
                        connect(proc, &bugs::ProcBugsAPI::completeReq_favarite_track, this, &RoseHomeTrackHistoryAll::slot_bugs_completeReq_listAll_myFavoritesIds);

                        if(this->track_star_fav == 0){
                            itemPosData.likes_yn = false;

                            proc->request_bugs_deleteFavorite_track(this->list_track->at(idx).clientKey.toInt(), bugs::ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                        }
                        else if(this->track_star_fav == 1){
                            itemPosData.likes_yn = true;

                            proc->request_bugs_addFavorite_track(this->list_track->at(idx).clientKey.toInt(), bugs::ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                        }

                        this->flag_send_track = true;
                    }
                    else if(type == "QOBUZ"){
                        this->track_id_fav = this->list_track->at(idx).clientKey.toInt();

                        qobuz::ProcCommon *proc = new qobuz::ProcCommon(this);
                        connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &RoseHomeTrackHistoryAll::slot_bugs_completeReq_listAll_myFavoritesIds);
                        proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                        this->flag_send_track = true;
                    }
                    else if(type == "TIDAL"){
                        this->track_id_fav = this->list_track->at(idx).clientKey.toInt();

                        if(this->track_star_fav == 1){
                            // Tidal Track Favorite add
                            tidal::ProcCommon *proc = new tidal::ProcCommon(this);
                            proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                        }
                        else if(this->track_star_fav == 0){
                            // Tidal Track Favorite del
                            tidal::ProcCommon *proc = new tidal::ProcCommon(this);
                            proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                        }

                        // favorite 정보 가져오기
                        tidal::ProcCommon *proc_fav = new tidal::ProcCommon(this);
                        connect(proc_fav, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &RoseHomeTrackHistoryAll::slot_tidal_completeReq_listAll_myFavoritesIds);
                        proc_fav->request_tidal_getAll_favorites();
                        this->flag_send_track = true;
                    }
                }

                this->track_idx_fav = idx;

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_track_fav);
                ratingInfo.insert("star", this->track_star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", this->list_track->at(idx).type);

                QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseHomeTrackHistoryAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

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
    void RoseHomeTrackHistoryAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }
}
