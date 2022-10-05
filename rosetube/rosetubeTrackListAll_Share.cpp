#include "rosetubeTrackListAll_Share.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "tidal/ProcCommon.h"
#include "bugs/ProcBugsAPI.h"
#include "qobuz/ProcCommon_forQobuz.h"
#include "apple/ProcCommon_forApple.h"

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


namespace rosetube {

    const int SECTION_FOR_MORE_POPUP___tracks = 0;
    const int SECTION_FOR_MORE_POPUP___rosetube = 1;

    const int HTTP_NETWORK_PLAY = 99;

    const QString ICON_PATH___playAll = ":/images/icon_list_allplay.png";
    const QString ICON_PATH___playShuffle = ":/images/icon_list_shuffle.png";

    /**
     * @brief "ROSEHOME > REcENT TRACK" 화면을 위한 생성자. @see PAGECODE_RH_TRACK_LIST_ALL_VIEW
     * @param parent
     */
    RoseTubeTrackListAll_Share::RoseTubeTrackListAll_Share(QWidget *parent) : AbstractRoseHomeSubWidget(VerticalScroll_rosefilter, parent) {

        // data
        this->list_track = new QList<roseHome::TrackItemData>();

        // UI
        this->setUIControl_tracks();
    }


    /**
    * @brief 소멸자.
    */
    RoseTubeTrackListAll_Share::~RoseTubeTrackListAll_Share(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void RoseTubeTrackListAll_Share::setJsonObject_forData(const QJsonObject &jsonObj){

        QString tmpType = ProcJsonEasy::getString(jsonObj, "type");
        int tmpIntID = ProcJsonEasy::getInt(jsonObj, "id");
        QString tmpStrID = ProcJsonEasy::getString(jsonObj, "id");

        this->flagNeedReload = false;

        if(this->type != tmpType || this->int_id != tmpIntID || this->str_id != tmpStrID){
            this->flagNeedReload = true;

            this->type = tmpType;
            this->int_id = tmpIntID;
            this->str_id = tmpStrID;

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

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_trackData();
        }
    }


    /**
     * @brief RoseTubeTrackListAll_Share::setActivePage
     */
    void RoseTubeTrackListAll_Share::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            this->box_filter->removeWidget(this->widget_filter_contents);
            GSCommon::clearLayout(this->box_filter);

            if(this->type != "ROSE_TUBE"){
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
                connect(btn_playAll, &QPushButton::clicked, this, &RoseTubeTrackListAll_Share::slot_btnClicked_playAll);

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
                connect(btn_shuffleAll, &QPushButton::clicked, this, &RoseTubeTrackListAll_Share::slot_btnClicked_playShuffle);

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

            if(this->type != "ROSE_TUBE"){
                this->vBox_tracks = new QVBoxLayout();
                this->vBox_tracks->setSpacing(0);
                this->vBox_tracks->setContentsMargins(0, 0, 0, 0);
                GSCommon::clearLayout(this->vBox_tracks);

                this->box_main_contents->addLayout(this->vBox_tracks);
            }
            else if(this->type == "ROSE_TUBE"){
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


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void RoseTubeTrackListAll_Share::setUIControl_tracks(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("");
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void RoseTubeTrackListAll_Share::request_more_trackData(){

        if(this->type == "ROSE_TOTAL"){
            // request HTTP API
            /*roseHome::ProcCommon *proc_track = new roseHome::ProcCommon(this);
            connect(proc_track, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseHomeTrackListAll_Share::slot_applyResult_tracks);
            proc_track->request_rose_get_track(this->int_id, 0, 0);*/
        }
        else if(this->type == "ROSE_TUBE"){
            // request HTTP API
            roseHome::ProcCommon *proc_track = new roseHome::ProcCommon(this);
            connect(proc_track, &roseHome::ProcCommon::completeReq_list_rosetubes, this, &RoseTubeTrackListAll_Share::slot_applyResult_rosetube);
            proc_track->request_rose_get_rosetube(this->str_id, 0, 0);
        }
        else if(this->type == "TIDAL"){

        }
        else if(this->type == "BUGS"){

        }
        else if(this->type == "QOBUZ"){

        }
        else if(this->type == "APPLE_MUSIC"){

        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RoseTubeTrackListAll_Share::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << "p_jsonObj = " << strJson;*/

        if(p_id == HTTP_NETWORK_PLAY){
            if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
            }
        }

        sender()->deleteLater();
    }


    /**
     * @brief [slot] Track 데이터를 받아서 처리함
     * @param list_data
     */
    void RoseTubeTrackListAll_Share::slot_applyResult_tracks(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){

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

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &RoseTubeTrackListAll_Share::slot_clickedItemTrack_inList);
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

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void RoseTubeTrackListAll_Share::slot_applyResult_rosetube(const QJsonArray &jsonArr_dataToPlay, const int &track_count, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(track_count > 0){
            ContentLoadingwaitingMsgHide();

            for(int i = 0; i < jsonArr_dataToPlay.size(); i++){
                this->share_track[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___rosetube, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
                connect(this->share_track[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTubeTrackListAll_Share::slot_clickedItemPlaylist);
            }

            for(int i = 0; i < jsonArr_dataToPlay.size(); i++){
                QJsonObject tmpTrack = jsonArr_dataToPlay.at(i).toObject();

                QString tmpchannelId = ProcJsonEasy::getString(tmpTrack, "uploaderUrl");
                QString channelId = "";
                QStringList strListchannelId = tmpchannelId.split("channel/");
                if(strListchannelId.size() == 2){
                    channelId = strListchannelId.at(1);
                }

                QString tmpId = ProcJsonEasy::getString(tmpTrack, "url");
                QString id = "";
                QStringList strListId = tmpId.split("?v=");
                if(strListId.size() == 2){
                    id = strListId.at(1);
                }

                QString channelName = ProcJsonEasy::getString(tmpTrack, "uploaderName");
                QString title = ProcJsonEasy::getString(tmpTrack, "name");
                QString thumbnailUrl = ProcJsonEasy::getString(tmpTrack, "thumbnailUrl");

                int duration = ProcJsonEasy::getInt(tmpTrack, "duration");
                int viewCount = ProcJsonEasy::getInt(tmpTrack, "viewCount");

                QJsonObject jsonPlayTrack;
                jsonPlayTrack.insert("channelId", channelId);
                jsonPlayTrack.insert("channelName", channelName);
                jsonPlayTrack.insert("duration", duration);
                jsonPlayTrack.insert("id", id);
                jsonPlayTrack.insert("isFavorite", false);
                jsonPlayTrack.insert("isLiveStream", false);
                jsonPlayTrack.insert("thumbnailUrl", thumbnailUrl);
                jsonPlayTrack.insert("thumbsUpPercentage", 0);
                jsonPlayTrack.insert("title", title);
                jsonPlayTrack.insert("viewCount", viewCount);

                this->jsonArr_tracks_toPlay.append(jsonPlayTrack);

                QJsonObject jsonTrack;
                jsonTrack.insert("duration", duration);
                jsonTrack.insert("title", title);
                jsonTrack.insert("channelTitle", channelName);
                jsonTrack.insert("thumbnailUrl", thumbnailUrl);
                jsonTrack.insert("viewCount", ProcJsonEasy::getInt(tmpTrack, "viewCount"));

                this->share_track[i]->setData(jsonTrack);
                this->flowLayout_track->addWidget(this->share_track[i]);

                this->label_mainTitle->setText(channelName);
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void RoseTubeTrackListAll_Share::slot_applyResult_getShareLink(const QString &link){//c220818 share link

        this->shareLink = link;
    }


    void RoseTubeTrackListAll_Share::slot_applyResult_getRating_track(const QJsonArray &contents){

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
    }


    void RoseTubeTrackListAll_Share::slot_btnClicked_playAll(){

        roseHome::TrackItemData data_track;

        // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
        procRosePlay->requestPlayRose_byTracks(data_track, jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow_withReorder, AbstractProcRosePlay::PlayShuffleMode::JustPlay);
    }

    void RoseTubeTrackListAll_Share::slot_btnClicked_playShuffle(){

        roseHome::TrackItemData data_track;

        // Track shuffle 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
        procRosePlay->requestPlayRose_byTracks(data_track, jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow_withReorder, AbstractProcRosePlay::PlayShuffleMode::ShufflePlay);
    }


    /**
     * @brief [Slot] PlaylistTrackDetailInfo_RHV signal에 대하여 처리
     * @param clickMode
     */
    void RoseTubeTrackListAll_Share::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

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
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseTubeTrackListAll_Share::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

                this->flag_check_track = true;
            }
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, SECTION_FOR_MORE_POPUP___tracks);
        }
    }


    void RoseTubeTrackListAll_Share::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___rosetube){

            if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add){

            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox || clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
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

                QJsonArray jsonArr_toPlayReal = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_tracks_toPlay, index, playType);

                QJsonObject jsonData;
                jsonData.insert("youtube", jsonArr_toPlayReal);
                jsonData.insert("youtubePlayType", curr_clickMode);
                jsonData.insert("roseToken", global.device.getDeviceRoseToken());

                if(global.queueTimerFlag && !global.queuelist_mouse_trigger_menu_flag && global.Queue_track_count != 0) {
                    emit this->linker->signal_checkQueue(27, "");

                    return;
                }
                emit this->linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += this->jsonArr_tracks_toPlay.count();     // 220419 queue count

                QString playlistType = "";
                if(curr_clickMode == 12 || curr_clickMode == 15 || curr_clickMode == 17 || curr_clickMode == 18){
                    playlistType = "youtubePlay";
                }
                else{
                    playlistType = "youtubePlay.playlist.add";
                }

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3")
                                 .arg(global.device.getDeviceIP())
                                 .arg(global.port).arg(playlistType), jsonData, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AddCollectionBtn){
                // 플레이리스트 담기 - Track
                //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(index).toObject();

                OptMorePopup::HeaderData data_header;
                data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                data_header.sub_title = ProcJsonEasy::getString(tmpObj, "channelName");//c220818
                data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                data_header.data_pk = ProcJsonEasy::getString(tmpObj, "id");//c220818--스트링이어야 하지 않는지? int  이어서 항상 0
                data_header.type = ProcJsonEasy::getString(tmpObj, "type");      //j220906 share link
                data_header.flagProcStar = false;
                data_header.isShare = true;      //j220906 share link

                // OptMorePopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosetube_Video, data_header, index, section);
            }
        }

    }

    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void RoseTubeTrackListAll_Share::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___rosetube){
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
                int curr_clickMode = this->get_rose_playType(clickMode);

                QJsonArray jsonArr_toPlayReal = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_tracks_toPlay, index, clickMode);

                QJsonObject jsonData;
                jsonData.insert("youtube", jsonArr_toPlayReal);
                jsonData.insert("youtubePlayType", curr_clickMode);
                jsonData.insert("roseToken", global.device.getDeviceRoseToken());

                if(global.queueTimerFlag && !global.queuelist_mouse_trigger_menu_flag && global.Queue_track_count != 0) {
                    emit this->linker->signal_checkQueue(27, "");

                    return;
                }
                emit this->linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArr_toPlayReal.count();     // 220419 queue count

                QString playlistType = "";
                if(curr_clickMode == 12 || curr_clickMode == 15 || curr_clickMode == 17 || curr_clickMode == 18){
                    playlistType = "youtubePlay";
                }
                else{
                    playlistType = "youtubePlay.playlist.add";
                }

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3")
                                 .arg(global.device.getDeviceIP())
                                 .arg(global.port).arg(playlistType), jsonData, true);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubscribInfo){
                QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(index).toObject();

                QString channelName = "";
                QString channelId = "";

                if(tmpObj.contains("data")){
                    QJsonObject dataObj = ProcJsonEasy::getJsonObject(tmpObj, "data");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(dataObj, "snippet");

                    channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                    channelId = ProcJsonEasy::getString(snippet, "channelId");
                }
                else{
                    if(tmpObj.contains("channelTitle")){
                        channelName = ProcJsonEasy::getString(tmpObj, "channelTitle");
                    }
                    else if(tmpObj.contains("channelName")){
                        channelName = ProcJsonEasy::getString(tmpObj, "channelName");
                    }

                    channelId = ProcJsonEasy::getString(tmpObj, "channelId");
                }

                QJsonObject tmp_data;
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert("title", channelName);
                tmp_data.insert("channel_id", channelId);

                emit this->signal_clickedViewAll(tmp_data);
            }
            else if(clickMode == OptMorePopup::ClickMode::Share){//c220818
                setUIShare();
            }
        }
    }


    int RoseTubeTrackListAll_Share::get_rose_playType(OptMorePopup::ClickMode clickMode){

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


    QJsonArray RoseTubeTrackListAll_Share::get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode){

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
    QJsonArray RoseTubeTrackListAll_Share::subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex){

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
    QJsonArray RoseTubeTrackListAll_Share::reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex){

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
