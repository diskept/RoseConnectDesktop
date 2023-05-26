#include "rosetube/rosetube_ListViewAll.h"

#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "widget/NoData_Widget.h"
#include "roseHome/ProcCommon_forRosehome.h"//c220818

#include <QScrollBar>


namespace rosetube {

    const int HTTP_ROSETUBE = 0;
    const int HTTP_CACHE = 10;

    const int SECTION_FOR_MORE_POPUP___TRACK = 0;
    const int SECTION_FOR_MORE_POPUP___PLAYLIST = 1;


    RosetubeViewAll::RosetubeViewAll(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        this->setUIControl_RoseTube();
    }


    RosetubeViewAll::~RosetubeViewAll(){

        this->deleteLater();
    }


    void RosetubeViewAll::setJsonObject_forData(const QJsonObject& jsonObj){

        QString mainTitle = ProcJsonEasy::getString(jsonObj, "title");
        QString type = ProcJsonEasy::getString(jsonObj, "type");
        int id = ProcJsonEasy::getInt(jsonObj, "subCategoryId");
        this->flagNeedReload = false;

        if(this->subCategoryId != id || this->title != mainTitle){
            this->flagNeedReload = true;

            this->subCategoryId = id;
            this->listtype = type;
            this->title = mainTitle;

            this->label_mainTitle->setText(this->title);

            // init
            this->next_offset = 0;
            this->rosetube_total_cnt = 0;
            this->rosetube_draw_cnt = 0;


            // request HTTP API
            this->flagReqMore_rosetube = false;
            this->flag_lastPage_rosetube = false;

            this->flag_rosetube_draw = false;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_rosetubeData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    void RosetubeViewAll::setActivePage(){

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


    void RosetubeViewAll::setUIControl_RoseTube(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("View All");

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        rosetube::ItemTrack_rosetube *listAlbum = new rosetube::ItemTrack_rosetube(0, SECTION_FOR_MORE_POPUP___TRACK, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);

        this->rosetube_widget_width = listAlbum->get_fixedWidth();
        this->rosetube_widget_margin = listAlbum->get_rightMargin();

        delete listAlbum;

        // layout for items
        this->flowLayout_rosetube = this->get_addUIControl_flowLayout(0, 20);

    }


    void RosetubeViewAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_rosetube && !this->flag_lastPage_rosetube) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_rosetube = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_rosetubeData();
        }
        else if((this->rosetube_total_cnt > this->rosetube_draw_cnt) && this->flag_rosetube_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_rosetubeDraw();
        }
    }


    void RosetubeViewAll::request_more_rosetubeData(){

        // next_offset
        if(this->rosetube_total_cnt == 0){
            this->next_offset = 0;
            this->jsonArr_rosetubeTrack = QJsonArray();
            this->jsonArr_rosetubePlaylist = QJsonArray();
        }
        else{
            this->next_offset++;
        }


        // request HTTP API
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + "/member/youtube/viewmore?" + QString("subCategoryId=%1").arg(this->subCategoryId) + QString("&page=%1&size=200").arg(this->next_offset);

        qDebug() << url;
        QJsonObject json;

        network->request(HTTP_ROSETUBE
                         , url
                         , json
                         , false
                         , true);

        if(this->next_offset == 0){
            this->flag_rosetube_draw = true;
        }

    }

    void RosetubeViewAll::request_more_rosetubeDraw(){

        this->flag_rosetube_draw = true;

        int f_width = this->flowLayout_rosetube->geometry().width();

        if(this->flowLayout_rosetube->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->rosetube_widget_width + this->rosetube_widget_margin);
        int f_mod = this->rosetube_draw_cnt % f_wg_cnt;

        if(f_mod == 0){
            this->rosetube_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->rosetube_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        int start_index = this->rosetube_draw_cnt;
        int max_cnt = ((this->rosetube_total_cnt - this->rosetube_draw_cnt) > this->rosetube_widget_cnt ) ? this->rosetube_widget_cnt : (this->rosetube_total_cnt - this->rosetube_draw_cnt);
        this->rosetube_draw_cnt += max_cnt;

        if(this->list_type =="TRACK"){

            for(int i = start_index; i < this->rosetube_draw_cnt; i++){
                this->viewAll_rosetube_track[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___TRACK, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
                qDebug() << "i = :" << i;
                QCoreApplication::processEvents();
            }

            for(int i = start_index; i < this->rosetube_draw_cnt; i++){
                this->viewAll_rosetube_track[i]->setData(this->jsonArr_rosetubeTrack.at(i).toObject());
                QCoreApplication::processEvents();
            }

            for(int i = start_index; i < this->rosetube_draw_cnt; i++){
                connect(this->viewAll_rosetube_track[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RosetubeViewAll::slot_clickedItemPlaylist);
                this->flowLayout_rosetube->addWidget(this->viewAll_rosetube_track[i]);
            }
        }
        else if(this->list_type =="PLAYLIST"){

            for(int i = start_index; i < this->rosetube_draw_cnt; i++){
                this->viewAll_rosetube_track[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
                QCoreApplication::processEvents();
            }

            for(int i = start_index; i < this->rosetube_draw_cnt; i++){
                this->viewAll_rosetube_track[i]->setData(this->jsonArr_rosetubePlaylist.at(i).toObject());
                QCoreApplication::processEvents();
            }

            for(int i = start_index; i < this->rosetube_draw_cnt; i++){
                connect(this->viewAll_rosetube_track[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RosetubeViewAll::slot_clickedItemPlaylist);
                this->flowLayout_rosetube->addWidget(this->viewAll_rosetube_track[i]);
            }
        }

        ContentLoadingwaitingMsgHide();

        this->flag_rosetube_draw = false;
    }

    void RosetubeViewAll::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){

            QJsonObject tmpObj = contents.at(0).toObject();

            if(contents.size() > 0){

                QJsonObject tmpObj = contents.at(0).toObject();

                if(tmpObj.value("flagOk").toBool() == true && tmpObj.value("message").toString() == "정상"){
                    if(this->flag_fav_type == SECTION_FOR_MORE_POPUP___TRACK){
                        this->viewAll_rosetube_track[this->flag_fav_idx]->setFavorite_btnHeart(this->flag_fav_star == 0 ? false : true, this->flag_fav_star);
                    }
                }
            }
        }

        ContentLoadingwaitingMsgHide();



    }

    void RosetubeViewAll::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << "p_jsonObj = " << strJson;*/

        if(p_id==HTTP_ROSETUBE){
            if(p_jsonObj.contains("roseTubeSubCategory")){

                this->slot_applyResult_subCategories(p_jsonObj);
            }
        }
        else if(p_id == HTTP_CACHE){
            if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
            }
        }

        sender()->deleteLater();
    }


    void RosetubeViewAll::slot_applyResult_subCategories(const QJsonObject &jsonObj){

        int size = 0;

        QJsonObject tmpCategory = ProcJsonEasy::getJsonObject(jsonObj, "roseTubeSubCategory");

        int start_index;

        QJsonArray trackArr;
        QJsonArray palylistArr;
        if(tmpCategory["listType"].toString() == "TRACK"){
            start_index = this->jsonArr_rosetubeTrack.size();

            this->list_type = "TRACK";

            QJsonObject trackObj = ProcJsonEasy::getJsonObject(tmpCategory, "playlist");
            trackArr = ProcJsonEasy::getJsonArray(trackObj, "tracks");
            ProcJsonEasy::mergeJsonArray(this->jsonArr_rosetubeTrack, trackArr);

            size = trackArr.size();
        }
        else if(tmpCategory["listType"].toString() == "PLAYLIST"){
            start_index = this->jsonArr_rosetubePlaylist.size();

            this->list_type = "PLAYLIST";

            palylistArr = ProcJsonEasy::getJsonArray(tmpCategory, "playlists");
            ProcJsonEasy::mergeJsonArray(this->jsonArr_rosetubePlaylist, palylistArr);

            size = palylistArr.size();
        }

        if(size > 0){
            this->flagReqMore_rosetube = false;
            this->flag_rosetube_draw = false;

            if(this->rosetube_total_cnt == 0){
                this->rosetube_total_cnt = ProcJsonEasy::getInt(jsonObj, "totalCount");
            }


            if(tmpCategory["listType"].toString() == "TRACK"){

                if(start_index == 0){
                    int f_width = this->flowLayout_rosetube->geometry().width();

                    if(this->flowLayout_rosetube->geometry().width() <= 0){
                        f_width = this->width() - (80 + 63) - 10;
                    }

                    int f_wg_cnt = f_width / (this->rosetube_widget_width + this->rosetube_widget_margin);
                    int f_mod = this->rosetube_draw_cnt % f_wg_cnt;

                    if(f_mod == 0){
                        this->rosetube_widget_cnt = f_wg_cnt * 10;
                    }
                    else{
                        this->rosetube_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                    }

                    int start_index = this->rosetube_draw_cnt;
                    int max_cnt = ((this->rosetube_total_cnt - this->rosetube_draw_cnt) > this->rosetube_widget_cnt ) ? this->rosetube_widget_cnt : (this->rosetube_total_cnt - this->rosetube_draw_cnt);
                    this->rosetube_draw_cnt += max_cnt;


                    for(int i = start_index; i < max_cnt; i++){
                        this->viewAll_rosetube_track[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___TRACK, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
                        QCoreApplication::processEvents();
                    }

                    for(int i = start_index; i < max_cnt; i++){
                        this->viewAll_rosetube_track[i]->setData(this->jsonArr_rosetubeTrack.at(i).toObject());
                        QCoreApplication::processEvents();
                    }

                    for(int i = start_index; i < max_cnt; i++){
                        connect(this->viewAll_rosetube_track[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RosetubeViewAll::slot_clickedItemPlaylist);
                        this->flowLayout_rosetube->addWidget(this->viewAll_rosetube_track[i]);
                    }

                    ContentLoadingwaitingMsgHide();

                    if(this->rosetube_total_cnt > this->rosetube_draw_cnt){
                        this->flag_lastPage_rosetube = false;
                        this->flag_rosetube_draw = false;
                    }
                    else{
                        this->flag_lastPage_rosetube = true;
                        this->flag_rosetube_draw = true;
                    }
                }
                else{
                    this->request_more_rosetubeDraw();
                }
            }
            else if(tmpCategory["listType"].toString() == "PLAYLIST"){

                if(start_index == 0){
                    int f_width = this->flowLayout_rosetube->geometry().width();

                    if(this->flowLayout_rosetube->geometry().width() <= 0){
                        f_width = this->width() - (80 + 63) - 10;
                    }

                    int f_wg_cnt = f_width / (this->rosetube_widget_width + this->rosetube_widget_margin);
                    int f_mod = this->rosetube_draw_cnt % f_wg_cnt;

                    if(f_mod == 0){
                        this->rosetube_widget_cnt = f_wg_cnt * 10;
                    }
                    else{
                        this->rosetube_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                    }

                    int start_index = this->rosetube_draw_cnt;
                    int max_cnt = ((this->rosetube_total_cnt - this->rosetube_draw_cnt) > this->rosetube_widget_cnt ) ? this->rosetube_widget_cnt : (this->rosetube_total_cnt - this->rosetube_draw_cnt);
                    this->rosetube_draw_cnt += max_cnt;

                    for(int i = start_index; i < max_cnt; i++){
                        this->viewAll_rosetube_track[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
                        QCoreApplication::processEvents();
                    }

                    for(int i = start_index; i < max_cnt; i++){
                        this->viewAll_rosetube_track[i]->setData(this->jsonArr_rosetubePlaylist.at(i).toObject());
                        QCoreApplication::processEvents();
                    }

                    for(int i = start_index; i < max_cnt; i++){
                        connect(this->viewAll_rosetube_track[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RosetubeViewAll::slot_clickedItemPlaylist);
                        this->flowLayout_rosetube->addWidget(this->viewAll_rosetube_track[i]);
                    }

                    ContentLoadingwaitingMsgHide();

                    if(this->rosetube_total_cnt > this->rosetube_draw_cnt){
                        this->flag_lastPage_rosetube = false;
                        this->flag_rosetube_draw = false;
                    }
                    else{
                        this->flag_lastPage_rosetube = true;
                        this->flag_rosetube_draw = true;
                    }
                }
                else{
                    this->request_more_rosetubeDraw();
                }
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->rosetube_draw_cnt <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_rosetube->geometry().width();

                if(this->flowLayout_rosetube->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_rosetube->addWidget(noData_widget);
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RosetubeViewAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___TRACK){
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
                this->flag_fav_type = SECTION_FOR_MORE_POPUP___TRACK;

                QJsonObject track = this->jsonArr_rosetubeTrack.at(index).toObject();

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
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RosetubeViewAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, flag_fav, star_fav);

                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
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
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byTracks(data_playlist, this->jsonArr_rosetubeTrack, index, playType);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    QJsonObject tmpObj = this->jsonArr_rosetubeTrack.at(index).toObject();

                    QJsonObject data = ProcJsonEasy::getJsonObject(tmpObj, "data");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(data, "snippet");

                    OptMorePopup::HeaderData data_header;
                    data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                    data_header.sub_title = ProcJsonEasy::getString(snippet, "channelTitle");
                    data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                    data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(tmpObj, "id"));
                    data_header.type = "YOUTUBE";      //j220906 share link
                    data_header.flagProcStar = false;
                    data_header.isShare = true;      //j220906 share link

                    // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Rosetube_Video, data_header, index, section);
                }
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___PLAYLIST){

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Track 바로 재생 - clicked 항목만 재생
                int curr_clickMode = global.device.getMusicPlayType();

                OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                switch(curr_clickMode)
                {
                case 13:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last;
                    break;
                case 12:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty;
                    break;
                case 15:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_RightNow;
                    break;
                case 16:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext;
                    break;
                default:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_RightNow;
                    break;
                }

                QJsonObject tmpObj = this->jsonArr_rosetubePlaylist.at(index).toObject();
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(ProcJsonEasy::getInt(tmpObj, "id"), playType);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                QJsonObject tmpObj = this->jsonArr_rosetubePlaylist.at(index).toObject();
                // Playlist Detail 페이지 진입
                QJsonObject tmp_data;
                tmp_data.insert("id", ProcJsonEasy::getInt(tmpObj, "id"));
                tmp_data.insert("ownerId", ProcJsonEasy::getInt(tmpObj, "ownerId"));
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYDETAIL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_MYDETAIL);
                // 부모에게 페이지 변경하라고 시그널 보냄
                emit this->signal_clickedViewAll(tmp_data);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                QJsonObject tmpObj = this->jsonArr_rosetubePlaylist.at(index).toObject();

                QJsonObject data = ProcJsonEasy::getJsonObject(tmpObj, "data");
                QJsonObject snippet = ProcJsonEasy::getJsonObject(data, "snippet");

                OptMorePopup::HeaderData data_header;
                data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                data_header.sub_title = ProcJsonEasy::getString(tmpObj, "ownerName").toUpper();
                data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnail");
                data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(tmpObj, "id"));
                data_header.type = "YOUTUBE";      //j220906 share link
                data_header.flagProcStar = false;
                data_header.isShare = true;      //j220906 share link

                // OptMorePopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosetube_Playlist, data_header, index, section);

            }

        }
    }

    void RosetubeViewAll::slot_applyResult_getShareLink(const QString &link){//c220818 share link

        this->shareLink = link;
    }

    void RosetubeViewAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___TRACK){
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
                roseHome::TrackItemData data_track;
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data_track, this->jsonArr_rosetubeTrack, index, clickMode);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubscribInfo){
                QJsonObject tmpObj = this->jsonArr_rosetubeTrack.at(index).toObject();
                QJsonObject dataObj = ProcJsonEasy::getJsonObject(tmpObj, "data");
                QJsonObject snippet = ProcJsonEasy::getJsonObject(dataObj, "snippet");
                QString channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                QString channelId = ProcJsonEasy::getString(snippet, "channelId");

                QJsonObject tmp_data;
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert("title", channelName);
                tmp_data.insert("channel_id", channelId);

                emit this->signal_clickedViewAll(tmp_data);
            }
            else if(clickMode == OptMorePopup::ClickMode::Share){//c220818
                print_debug();//c220818-3dot
                setUIShare();//c220818
                print_debug();
                qDebug() << "this->shareLink="<<this->shareLink;//c220818


            }
            else if(clickMode == OptMorePopup::ClickMode::Caching_ahead){
                QJsonObject trackInfo = this->jsonArr_rosetubeTrack.at(index).toObject();

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
                network->request(HTTP_CACHE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___PLAYLIST){
            // Track 바로 재생 - clicked 항목만 재생
            int curr_clickMode = global.device.getMusicPlayType();

            OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
            switch(curr_clickMode)
            {
                case 13:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last;
                    break;
                case 12:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty;
                    break;
                case 15:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_RightNow;
                    break;
                case 16:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext;
                    break;
                default:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_RightNow;
                    break;
            }

            QJsonObject tmpObj = this->jsonArr_rosetubePlaylist.at(index).toObject();
            roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
            procRosePlay->requestPlayRose_byPlaylistID(ProcJsonEasy::getInt(tmpObj, "id"), playType);
        }
    }
    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void RosetubeViewAll::resizeEvent(QResizeEvent *event){

        Q_UNUSED(event);

        this->setFlowLayoutResize(this, this->flowLayout_rosetube, this->rosetube_widget_width, this->rosetube_widget_margin);
    }
}

