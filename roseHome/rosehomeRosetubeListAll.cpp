#include "roseHome/rosehomeRosetubeListAll.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"

#include <QResizeEvent>
#include <QScrollBar>


namespace roseHome {

    const int SECTION_FOR_MORE_POPUP___RecentlyRosetube = 3;

    const int HTTP_ROSETUBE_CACHE = 0;



    /**
     * @brief RoseHomePlaylistListAll::RoseHomePlaylistListAll
     * @param parent
     */
    RoseHomeRosetubeListAll::RoseHomeRosetubeListAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // 기본 UI 세팅
        this->setUIControl_rosetube();
    }

    /**
     * @brief 소멸자.
     */
    RoseHomeRosetubeListAll::~RoseHomeRosetubeListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void RoseHomeRosetubeListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        PageInfo_PlaylistAllView data_pageInfo = ConvertData::convertData_pageInfo_playlistAllView(jsonObj);
        this->flagNeedReload = false;

        if(this->current_pageInfo.api_subPath != data_pageInfo.api_subPath || this->current_pageInfo.type != data_pageInfo.type){
            this->flagNeedReload = true;

            this->current_pageInfo = data_pageInfo;
            this->label_mainTitle->setText(this->current_pageInfo.pathTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

            // init
            this->next_offset = 0;
            this->playlist_total_cnt = 0;
            this->playlist_draw_cnt = 0;

            // init data
            this->playlist_Arr = QJsonArray();

            this->flagReqMore_playlist = false;
            this->flag_lastPage_playlist = false;

            this->flag_playlist_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_rosetubeData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_rosetubes, this, &RoseHomeRosetubeListAll::slot_applyResult_rosetubelistCheck);
            proc->request_rose_getList_recentlyRosetube("member/track/recent", "YOUTUBE", 0, 10);
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void RoseHomeRosetubeListAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // init
            GSCommon::clearLayout(this->flowLayout_rosetube);

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
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
    void RoseHomeRosetubeListAll::setUIControl_rosetube(){

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

        connect(this->btn_mainTitle, &QPushButton::clicked, this, &RoseHomeRosetubeListAll::slot_btnClicked_Delete);

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

        // get widget width, right margin - by diskept j230317 start
        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        rosetube::ItemTrack_rosetube *listRosetube = new rosetube::ItemTrack_rosetube(0, 0, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);

        this->playlist_widget_width = listRosetube->get_fixedWidth();
        this->playlist_widget_margin = listRosetube->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeRosetubeListAll::setUIControl_playlists " << listRosetube->get_fixedWidth() << listRosetube->get_rightMargin();

        delete listRosetube;
        //  get widget width, right margin - by diskept j230317 finish

        // layout for items
        this->flowLayout_rosetube = this->get_addUIControl_flowLayout(0, 20);
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void RoseHomeRosetubeListAll::initAll(){

        // init
        this->next_offset = 0;
        this->playlist_total_cnt = 0;
        this->playlist_draw_cnt = 0;

        this->playlist_Arr = QJsonArray();

        // request HTTP API
        this->flagReqMore_playlist = false;
        this->flag_lastPage_playlist = false;

        this->flag_playlist_draw = false;

        GSCommon::clearLayout(this->flowLayout_rosetube);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void RoseHomeRosetubeListAll::proc_wheelEvent_to_getMoreData(){

        // wheel evnet change - by diskept j230317 start
        if((!this->flagReqMore_playlist && !this->flag_lastPage_playlist) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_playlist = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_rosetubeData();
        }
        else if((this->playlist_total_cnt > this->playlist_draw_cnt) && this->flag_playlist_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_rosetubeDraw();
        }
        // wheel evnet change - by diskept j230317 start
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief playlist 데이터를 더 요청한다.
     * @param page
     */
    void RoseHomeRosetubeListAll::request_more_rosetubeData(){

        // request api change - by diskept j230317 start
        // next_offset
        if(this->playlist_Arr.size() == 0){
            this->next_offset = 0;
        }
        else{
            this->next_offset++;
        }

        // request HTTP API
        roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
        connect(proc, &roseHome::ProcCommon::completeReq_list_rosetubes, this, &RoseHomeRosetubeListAll::slot_applyResult_rosetube);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_recentlyRosetube("member/track/recent", "YOUTUBE", this->next_offset, 200);
        // request api change - by diskept j230317 finish


        /*if(!this->flagReqMore_playlist && !this->flag_lastPage_playlist){
            this->flagReqMore_playlist = true;

            // j220913 list count check
            int width_cnt;//c230223
            if(flowLayout_rosetube->sizeHint().width() < 0) {//c230223
                width_cnt = global.LmtCnt / 304;
            }else{
                width_cnt = global.LmtCnt / flowLayout_rosetube->sizeHint().width();//
            }
            int mod = this->playlist_draw_cnt % width_cnt;

            if(mod == 0){
                this->playlist_widget_cnt = width_cnt * 10;
            }
            else{
                this->playlist_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
            // j220913 list count check

            // next_offset
            if(this->playlist_Arr.size() == 0){
                this->next_offset = 0;
            }
            else{
                this->next_offset++;
            }

            // request HTTP API
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_rosetubes, this, &RoseHomeRosetubeListAll::slot_applyResult_rosetube);
            proc->setProperty("idx", this->next_offset);
            proc->request_rose_getList_recentlyRosetube("member/track/recent", "YOUTUBE", this->next_offset, this->playlist_widget_cnt);

            if(this->next_offset == 0){
                this->flag_playlist_draw = true;
            }
        }*/
    }


    void RoseHomeRosetubeListAll::request_more_rosetubeDraw(){

        // widget draw change - by diskept j230317 start
        this->flag_playlist_draw = true;

        int f_width = this->flowLayout_rosetube->geometry().width();

        if(this->flowLayout_rosetube->geometry().width() <= 0){
            f_width = this->width() - (65 + 65) - 10;
        }

        int f_wg_cnt = f_width / (this->playlist_widget_width + this->playlist_widget_margin);
        int f_mod = this->playlist_draw_cnt % f_wg_cnt;

        if(f_mod == 0){
            this->playlist_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->playlist_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        //qDebug() << "[Debug] RoseHomeAlbumListAll::slot_applyResult_albums " << f_width << f_wg_cnt << f_mod << this->playlist_widget_cnt;

        int start_index = this->playlist_draw_cnt;
        int max_cnt = ((this->playlist_total_cnt - this->playlist_draw_cnt) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_total_cnt - this->playlist_draw_cnt);
        this->playlist_draw_cnt += max_cnt;

        //qDebug() << "[Debug] RoseHomeAlbumListAll::slot_applyResult_albums " << start_index << max_cnt;

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->recently_rosetube[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___RecentlyRosetube, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->recently_rosetube[i]->setData(this->playlist_Arr.at(i).toObject());
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->flowLayout_rosetube->addWidget(this->recently_rosetube[i]);
            connect(this->recently_rosetube[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseHomeRosetubeListAll::slot_clickedItemPlaylist);
        }

        ContentLoadingwaitingMsgHide();

        this->flag_playlist_draw = false;
        // widget draw change - by diskept j230317 finish

        /*// j220913 list count check
        int width_cnt;//c230223
        if(flowLayout_rosetube->sizeHint().width() < 0) {//c230223
            width_cnt = global.LmtCnt / 304;
        }else{
            width_cnt = global.LmtCnt / flowLayout_rosetube->sizeHint().width();//
        }
        int mod = this->playlist_draw_cnt % width_cnt;

        if(mod == 0){
            this->playlist_widget_cnt = width_cnt * 10;
        }
        else{
            this->playlist_widget_cnt = width_cnt * 10 + (width_cnt - mod);
        }
        // j220913 list count check

        int start_index = this->playlist_draw_cnt;
        int max_cnt = ((this->playlist_total_cnt - this->playlist_draw_cnt) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_total_cnt - this->playlist_draw_cnt);
        this->playlist_draw_cnt += max_cnt;

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->recently_rosetube[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___RecentlyRosetube, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
            connect(this->recently_rosetube[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseHomeRosetubeListAll::slot_clickedItemPlaylist);
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->recently_rosetube[i]->setData(this->playlist_Arr.at(i).toObject());

            this->flowLayout_rosetube->addWidget(this->recently_rosetube[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_playlist_draw = false;*/
    }


    void RoseHomeRosetubeListAll::slot_btnClicked_Delete(){

        QStringList tmpTitle = this->current_pageInfo.pathTitle.split("(");

        QJsonObject tmpObj;
        tmpObj.insert("contents_type", tmpTitle.at(0));
        tmpObj.insert("list_type", "YOUTUBE");
        tmpObj.insert("api_subPath", this->current_pageInfo.api_subPath);
        tmpObj.insert("filter_type", this->current_pageInfo.filter_type);

        QJsonObject tmpObj_move;
        tmpObj_move.insert(KEY_PAGE_CODE, PAGECODE_RH_RECENTLY_LIST_DELETE);
        tmpObj_move.insert(KEY_DATA, tmpObj);

        emit this->linker->signal_clickedMovePage(tmpObj_move);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RoseHomeRosetubeListAll::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){

            QJsonObject tmpObj = contents.at(0).toObject();

            if(tmpObj.value("flagOk").toBool() == true && tmpObj.value("message").toString() == "정상"){
                if(this->flag_fav_type == SECTION_FOR_MORE_POPUP___RecentlyRosetube){
                    this->recently_rosetube[this->flag_fav_idx]->setFavorite_btnHeart(this->flag_fav_star == 0 ? false : true, this->flag_fav_star);
                }
            }
        }

        ContentLoadingwaitingMsgHide();

        if(contents.size() > 0 && this->flag_fav_star == 0){
            // request HTTP API
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_rosetubes, this, &RoseHomeRosetubeListAll::slot_applyResult_rosetubelistCheck);
            proc->request_rose_getList_recentlyRosetube("member/track/recent", "YOUTUBE", 0, 10);

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
        }
    }


    void RoseHomeRosetubeListAll::slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj){

        if(p_id == HTTP_ROSETUBE_CACHE){
            if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
            }
        }
    }


    void RoseHomeRosetubeListAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___RecentlyRosetube){

            if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx2
                    || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx3 || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){

                bool flag_fav = false;
                int star_fav = 0;

                if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add){
                    flag_fav = true;
                    star_fav = 1;
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx2){
                    flag_fav = true;
                    star_fav = 2;
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx3){
                    flag_fav = true;
                    star_fav = 3;
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){
                    flag_fav = true;
                    star_fav = 0;
                }

                this->flag_fav_idx = index;
                this->flag_fav_star = star_fav;
                this->flag_fav_type = SECTION_FOR_MORE_POPUP___RecentlyRosetube;

                QJsonObject track = this->playlist_Arr.at(index).toObject();

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", flag_fav);
                ratingInfo.insert("star", star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", "YOUTUBE");

                QJsonObject json;
                json.insert("track", track);
                json.insert("ratingInfo", ratingInfo);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseHomeRosetubeListAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, flag_fav, star_fav);

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }
            else{

                if(clickMode == tidal::AbstractItem::ClickMode::AllBox || clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                    // Track 바로 재생 - clicked 항목만 재생
                    int curr_clickMode = global.device.getMusicPlayType();

                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    switch(curr_clickMode)
                    {
                        case 13:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                            break;
                        case 12:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                            break;
                        case 15:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                            break;
                        case 16:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                            break;
                        case 17:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                            break;
                        case 18:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                            break;
                        case 19:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                            break;
                    }

                    roseHome::TrackItemData data_playlist;
                    ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byTracks(data_playlist, this->playlist_Arr, index, playType);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    QJsonObject tmpObj = this->playlist_Arr.at(index).toObject();
                    print_debug();
                    qDebug() << "tmpObj=" << tmpObj;
                    QJsonObject data = ProcJsonEasy::getJsonObject(tmpObj, "data");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(data, "snippet");

                    OptMorePopup::HeaderData data_header;
                    data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                    data_header.sub_title = ProcJsonEasy::getString(snippet, "channelTitle");
                    data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                    data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(tmpObj, "id"));
                    data_header.type = ProcJsonEasy::getString(tmpObj, "type");//c220903_2
                    data_header.flagProcStar = false;
                    data_header.isShare = true;      //j220906 share link

                    // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Rosetube_Track, data_header, index, section);
                }
            }
        }
    }


    /**
     * @brief
     * @param list_data
     */
    void RoseHomeRosetubeListAll::slot_applyResult_rosetube(const QJsonArray &jsonArr_dataToPlay, const int &totalCnt, const bool flag_lastPage){

        int idx = sender()->property("idx").toInt();

        if(idx != this->next_offset){
            return;
        }

        //qDebug() << "[Debug] RoseHomeAlbumListAll::slot_applyResult_albums " << jsonArr_dataToPlay.count();

        if(jsonArr_dataToPlay.size() > 0){
            this->flagReqMore_playlist = false;
            this->flag_lastPage_playlist = flag_lastPage;

            if(this->playlist_Arr.size() == 0){
                this->playlist_total_cnt = totalCnt;
                this->playlist_Arr = QJsonArray();
            }

            int start_index = this->playlist_Arr.size();
            ProcJsonEasy::mergeJsonArray(this->playlist_Arr, jsonArr_dataToPlay);

            if(start_index == 0){
                // widget draw change - by diskept j230317 start
                int f_width = this->flowLayout_rosetube->geometry().width();

                if(this->flowLayout_rosetube->geometry().width() <= 0){
                    f_width = this->width() - (65 + 65) - 10;
                }

                int f_wg_cnt = f_width / (this->playlist_widget_width + this->playlist_widget_margin);
                int f_mod = this->playlist_draw_cnt % f_wg_cnt;

                if(f_mod == 0){
                    this->playlist_widget_cnt = f_wg_cnt * 10;
                }
                else{
                    this->playlist_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                }

                //qDebug() << "[Debug] RoseHomeAlbumListAll::slot_applyResult_albums " << f_width << f_wg_cnt << f_mod << this->playlist_widget_cnt;

                int start_index = this->playlist_draw_cnt;
                int max_cnt = ((this->playlist_total_cnt - this->playlist_draw_cnt) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_total_cnt - this->playlist_draw_cnt);
                this->playlist_draw_cnt += max_cnt;

                //qDebug() << "[Debug] RoseHomeAlbumListAll::slot_applyResult_albums " << start_index << max_cnt;

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->recently_rosetube[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___RecentlyRosetube, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->recently_rosetube[i]->setData(this->playlist_Arr.at(i).toObject());
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->flowLayout_rosetube->addWidget(this->recently_rosetube[i]);
                    connect(this->recently_rosetube[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseHomeRosetubeListAll::slot_clickedItemPlaylist);
                }

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_playlist_draw = false;
            }
            else{
                this->request_more_rosetubeDraw();
            }

            // request call change - by diskept j230317 start
            /*if(this->flag_lastPage_playlist == false){
                this->request_more_rosetubeData();
            }*/
            // request call change - by diskept j230317 finish
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->playlist_draw_cnt <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_rosetube->geometry().width();

                if(this->flowLayout_rosetube->geometry().width() <= 0){
                    f_width = this->width() - (65 + 65) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_rosetube->addWidget(noData_widget);
            }
        }
    }


    void RoseHomeRosetubeListAll::slot_applyResult_rosetubelistCheck(const QJsonArray &jsonArr_dataToPlay, const int &totalCnt, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(jsonArr_dataToPlay.size() > 0){
            if(this->playlist_total_cnt == 0){
                QString tmpTitle = tr("Recently Played Rosetube") + QString(" (%1)").arg(totalCnt);
                this->label_mainTitle->setText(tmpTitle);
                this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                this->initAll();

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                this->request_more_rosetubeData();

                this->flagNeedReload = true;
                this->setActivePage();
            }
            else{
                for(int i = 0; i < jsonArr_dataToPlay.count(); i++){
                    QJsonObject tmpData = jsonArr_dataToPlay.at(i).toObject();
                    QJsonObject tmpObj = this->playlist_Arr.at(i).toObject();

                    if(tmpData != tmpObj){
                        change_flag++;
                    }
                }

                if((totalCnt != this->playlist_total_cnt) || (change_flag > 0)){

                    QString tmpTitle = tr("Recently Played Rosetube") + QString(" (%1)").arg(totalCnt);
                    this->label_mainTitle->setText(tmpTitle);
                    this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                    this->initAll();

                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                    this->request_more_rosetubeData();

                    this->flagNeedReload = true;
                    this->setActivePage();
                }
                else{
                    ContentLoadingwaitingMsgHide();
                }
            }
        }
        else if(totalCnt == 0){
            QString tmpTitle = tr("Recently Played Rosetube (0)");
            this->label_mainTitle->setText(tmpTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

            this->initAll();

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_rosetubeData();

            this->flagNeedReload = true;
            this->setActivePage();
        }
        else{
            ContentLoadingwaitingMsgHide();
        }
    }


    void RoseHomeRosetubeListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___RecentlyRosetube){
            roseHome::TrackItemData data_track;
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
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
                // Rose Play 요청
                ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data_track, this->playlist_Arr, index, clickMode);
            }
            else if(clickMode == OptMorePopup::ClickMode::Caching_ahead){
                QJsonObject trackInfo = this->playlist_Arr.at(index).toObject();

                QString channelId = "";
                QString channelName = "";
                QString duration = "";
                QString id = "";
                QString thumbnailUrl = "";
                QString title = "";

                if(trackInfo.contains("data")){
                    QJsonObject trackData = ProcJsonEasy::getJsonObject(trackInfo, "data");
                    QJsonObject detailContents = ProcJsonEasy::getJsonObject(trackData, "detailContents");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(trackData, "snippet");
                    QJsonObject trackStatistic = ProcJsonEasy::getJsonObject(trackData, "detailContents");

                    QString tmpDuration = ProcJsonEasy::getString(detailContents, "duration");

                    QString time = tmpDuration;
                    QString hour;
                    QString minute;
                    QString second;

                    if(tmpDuration.indexOf("PT") >= 0){
                        time = tmpDuration.right(tmpDuration.size() - 2);
                    }
                    if(time.indexOf("H") >= 0){
                        hour = time.section("H", 0, -2);
                        time = time.mid(time.indexOf("H") + 1);
                    }
                    if(time.indexOf("M") >= 0){
                        minute = time.section("M", 0, -2);
                        time = time.mid(time.indexOf("M") + 1);
                    }
                    if(time.indexOf("S") >= 0){
                        second = time.section("S", 0, -2);
                    }

                    if(hour.isEmpty()){
                        if(minute.isEmpty()){
                            duration = "0:" + second.rightJustified(2, '0');
                        }
                        else{
                            duration = minute + ":" + second.rightJustified(2, '0');
                        }
                    }
                    else{
                        duration = hour + ":" + minute.rightJustified(2, '0') + ":" + second.rightJustified(2, '0');
                    }

                    channelId = ProcJsonEasy::getString(snippet, "channelId");
                    channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                    id = ProcJsonEasy::getString(trackInfo, "clientKey");
                    thumbnailUrl = ProcJsonEasy::getString(trackInfo, "thumbnailUrl");
                    title = ProcJsonEasy::getString(trackInfo, "title");
                }
                else{
                    if(trackInfo.contains("channelTitle")){
                        channelName = ProcJsonEasy::getString(trackInfo, "channelTitle");
                    }
                    else if(trackInfo.contains("channelName")){
                        channelName = ProcJsonEasy::getString(trackInfo, "channelName");
                    }

                    int time = ProcJsonEasy::getInt(trackInfo, "duration");
                    if(time == 0){
                        QString tmpDur = ProcJsonEasy::getString(trackInfo, "duration");

                        if(tmpDur.isEmpty()){
                            duration = "";
                        }
                        else{
                            duration = tmpDur;
                        }
                    }
                    else if(time >= 3600){
                         duration = QDateTime::fromTime_t(time).toUTC().toString("hh:mm:ss");
                    }
                    else{
                        duration = QDateTime::fromTime_t(time).toUTC().toString("mm:ss");
                    }

                    channelId = ProcJsonEasy::getString(trackInfo, "channelId");
                    id = ProcJsonEasy::getString(trackInfo, "id");
                    thumbnailUrl = ProcJsonEasy::getString(trackInfo, "thumbnailUrl");
                    title = ProcJsonEasy::getString(trackInfo, "title");
                }

                QJsonObject data;
                data.insert("channelId", channelId);
                data.insert("channelName", channelName);
                data.insert("duration", duration);
                data.insert("id", id);
                data.insert("isFavorite", false);
                data.insert("isLiveStream", false);
                data.insert("thumbnailUrl", thumbnailUrl);
                data.insert("thumbsUpPercentage", 0);
                data.insert("title", title);
                data.insert("viewsCount", "-1");

                QJsonObject json;
                json.insert("data", data);
                json.insert("isDelete", false);

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_download_item";
                network->request(HTTP_ROSETUBE_CACHE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
        }
    }


    /**
     * @brief RoseHomeRosetubeListAll::resizeEvent
     * @param event
     */
    void RoseHomeRosetubeListAll::resizeEvent(QResizeEvent *event){//c230223

        AbstractRoseHomeSubWidget::resizeEvent(event);
        Q_UNUSED(event);

        // flowlayout spacing change - by diskept j230317 start
        this->setFlowLayoutResize(this, this->flowLayout_rosetube, this->playlist_widget_width, this->playlist_widget_margin);
        // flowlayout spacing change - by diskept j230317 finish

        this->widget_mainTitle->resize(this->width(), 69);
        this->btn_mainTitle->setGeometry(this->width() - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);
    }
}
