#include "tidal/TidalPlaylistDetail.h"

#include "tidal/ProcCommon.h"
#include "tidal/ConvertData.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"
#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"

#include <QDebug>
#include <QPainter>
#include <QScrollBar>
#include <QScroller>
#include <QScrollArea>
#include <QPixmapCache>
#include <QJsonDocument>


namespace tidal {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___playlists = 1;
    const int SECTION_FOR_MORE_POPUP___tracks = 2;
    const int SECTION_FOR_MORE_POPUP___similarlists = 3;

    const int GET_MAX_ITEM_SIZE___ONCE = 100;


    /**
     * @brief
     * @param parent
     */
    TidalPlaylistDetail::TidalPlaylistDetail(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_filter, parent) {

        linker = Linker::getInstance();

        // data
        this->list_track = new QList<tidal::TrackItemData>();
        this->list_similarPlaylist = new QList<tidal::PlaylistItemData>();
    }


    /**
     * @brief 소멸자.
     */
    TidalPlaylistDetail::~TidalPlaylistDetail(){

        this->deleteLater();
    }


    /**
     * @brief TidalPlaylistDetail::setJsonObject_forData
     * @param jsonObj
     */
    void TidalPlaylistDetail::setJsonObject_forData(const QJsonObject &jsonObj){
print_debug();
qDebug() << "jsonObj=" << jsonObj;
        tidal::PlaylistItemData tmp_data_playlist = ConvertData::convertData_playlistData(jsonObj);

        this->flagNeedReload = false;

        if(global.user_forTidal.isLogined() == false){

            this->login_wait_Object = QJsonObject();
            this->login_wait_Object = jsonObj;

            this->tidalSettings = new TidalSettings();
            this->tidalSettings->setActivePage();

            this->box_login_contents = new QVBoxLayout();
            this->box_login_contents->setSpacing(0);
            this->box_login_contents->setContentsMargins(0, 0, 0, 0);
            this->box_login_contents->addWidget(this->tidalSettings);

            this->widget_login_contents = new QWidget();
            this->widget_login_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_login_contents->setLayout(this->box_login_contents);

            this->box_contents->addWidget(this->widget_login_contents);

            connect(this->tidalSettings, &TidalSettings::signal_logState_change, this, &TidalPlaylistDetail::slot_tidal_logChange);
        }
        else{

            if(tmp_data_playlist.uuid != this->data_playlist.uuid){

                this->flagNeedReload = true;

                if(this->flag_track_ok == true){
                    this->widget_main_contents->hide();
                    this->flag_track_ok = false;
                }

                this->data_playlist = tmp_data_playlist;

                this->list_track->clear();
                this->list_similarPlaylist->clear();
                this->jsonArr_tracks_toPlay = QJsonArray();

                this->track_drawCount = 0;
                this->track_totalCount = 0;
                this->track_beforeOffset = 0;
                this->track_currentOffset = 0;
                this->track_favoriteOffset = 0;

                this->track_id_fav = 0;
                this->track_idx_fav = 0;

                this->playlist_star_fav = 0;
                this->track_star_fav = 0;

                this->flag_playlist_fav = false;
                this->flag_track_fav = false;
                this->flag_send_playlist = false;
                this->flag_check_playlist = false;
                this->flag_send_track = false;
                this->flag_check_track = false;

                this->flagReqMore_track = false;
                this->flag_lastPage_track = false;

                this->flag_similarPlaylist = false;

                this->flag_draw = false;
print_debug();
                if(this->data_playlist.title.isEmpty()){
                    // Playlsit 기본 정보를 가져와야 하는 경우임. (id만 전달받은 상태)
                    ProcCommon *proc_playlist = new ProcCommon(this);
                    connect(proc_playlist, &ProcCommon::completeReq_playlist, this, &TidalPlaylistDetail::slot_applyResult_playlist);
                    proc_playlist->request_tidal_get_playlist(this->data_playlist.uuid);
                    print_debug();
                }
                else{
                    // request HTTP API - get favorite for Rose Server
                    roseHome::ProcCommon *proc_fav_playlist = new roseHome::ProcCommon(this);
                    connect(proc_fav_playlist, &roseHome::ProcCommon::completeReq_rating_playlist, this, &TidalPlaylistDetail::slot_applyResult_getRating_playlist);
                    proc_fav_playlist->request_rose_getRating_Playlist("TIDAL", this->data_playlist.uuid);

                    // Playlsit track 정보를 가져와야 하는 경우임.
                    ProcCommon *proc_tracks = new ProcCommon(this);
                    connect(proc_tracks, &ProcCommon::completeReq_list_items_of_playlist, this, &TidalPlaylistDetail::slot_applyResult_tracks);
                    proc_tracks->request_tidal_getList_items_of_playlist(this->data_playlist.uuid, GET_MAX_ITEM_SIZE___ONCE, 0);
                    print_debug();
                }

                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }
            else{
                print_debug();
                // 리로드 하지 않는 경우에는, favorite 정보만 다시 요청한다. (playlist_id 가 변경되지 않고, 페이지가 다시 요청된 경우임)
                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_playlist = new roseHome::ProcCommon(this);
                connect(proc_fav_playlist, &roseHome::ProcCommon::completeReq_rating_playlist, this, &TidalPlaylistDetail::slot_applyResult_getRating_playlist);
                proc_fav_playlist->request_rose_getRating_Playlist("TIDAL", this->data_playlist.uuid);
                print_debug();
            }
        }
    }


    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalPlaylistDetail::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            if(this->flag_track_ok == true){
                GSCommon::clearLayout(this->box_main_contents);
                this->box_contents->removeWidget(this->widget_main_contents);
            }

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);

            this->setUIControl_basic();
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief TidalPlaylistDetail::setUIControl_playlist_info
     */
    void TidalPlaylistDetail::setUIControl_basic(){

        // Playlist 상세정보
        this->playlist_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Tidal_playlist);
        connect(this->playlist_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &TidalPlaylistDetail::slot_detailInfo_btnClicked);

        this->playlist_detail_info->initView();

        this->box_main_contents->addWidget(this->playlist_detail_info);

        if(!this->data_playlist.title.isEmpty()){
            // Update Data
            this->track_totalCount = this->data_playlist.numberOfTracks;

            // Update UI
            QJsonObject playlistObj;
            playlistObj.insert("image", this->data_playlist.image);
            playlistObj.insert("title", this->data_playlist.title);
            playlistObj.insert("owner", this->data_playlist.creatorName);
            playlistObj.insert("track_count", this->data_playlist.numberOfTracks);
            playlistObj.insert("duration", this->data_playlist.duration);
            playlistObj.insert("lastUpdated", this->data_playlist.lastUpdated);
            playlistObj.insert("description", this->data_playlist.description);
            playlistObj.insert("uuid", this->data_playlist.uuid);

            this->playlist_detail_info->setData_fromTidalData(playlistObj);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_playlist = new roseHome::ProcCommon(this);
            connect(proc_fav_playlist, &roseHome::ProcCommon::completeReq_rating_playlist, this, &TidalPlaylistDetail::slot_applyResult_getRating_playlist);
            proc_fav_playlist->request_rose_getRating_Playlist("TIDAL", this->data_playlist.uuid);

            print_debug();
        }
    }


    void TidalPlaylistDetail::setUIControl_similarPlaylist(){

        this->setUIControl_subTitle_withSideBtn("Playlist suggestions ", "View All");
        this->box_main_contents->addSpacing(10);

        this->hBox_similarPlaylists = this->setUIControl_hBoxLayout_forPlaylists();

        int max_cnt = this->list_similarPlaylist->count();
        if(max_cnt > 15){
            max_cnt = 15;
        }

        for(int i = 0; i < max_cnt; i++){
            this->similarPlaylist[i]->setData(this->list_similarPlaylist->at(i));
            this->hBox_similarPlaylists->addWidget(this->similarPlaylist[i]);
        }
    }


    void TidalPlaylistDetail::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText){

        // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.
        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        tmp_hBox->setSpacing(0);
        tmp_hBox->setContentsMargins(0,0,0,0);
        tmp_hBox->setAlignment(Qt::AlignTop);

        QWidget *widget_box_subTitle = new QWidget();
        widget_box_subTitle->setContentsMargins(10,5,5,0);
        widget_box_subTitle->setLayout(tmp_hBox);

        QLabel *lb_subTitle = new QLabel();
        lb_subTitle->setText(subTitle);
        lb_subTitle->setStyleSheet("font-size:24px;font-weight:bold;color:#FFFFFF;");
        tmp_hBox->addWidget(lb_subTitle, 0, Qt::AlignVCenter);

        // side button - it works when the button text is not empty
        if(btnText.isEmpty() == false){
            QPushButton *btnView_all = new QPushButton();
            btnView_all->setText(btnText);
            btnView_all->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
            btnView_all->setCursor(Qt::PointingHandCursor);
            connect(btnView_all, SIGNAL(clicked()), this, SLOT(slot_clickBtn_subTitle_viewAll()));

            tmp_hBox->addWidget(btnView_all, 1, Qt::AlignVCenter | Qt::AlignRight);
        }

        this->box_main_contents->addWidget(widget_box_subTitle, 0, Qt::AlignTop);
    }


    QHBoxLayout* TidalPlaylistDetail::setUIControl_hBoxLayout_forPlaylists(){

        //----------------------------------------------------------------------------------------------------
        QHBoxLayout *hBox_playlist = new QHBoxLayout();
        hBox_playlist->setSpacing(0);
        hBox_playlist->setContentsMargins(0,0,0,0);
        hBox_playlist->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_contentSub = new QWidget();
        widget_contentSub->setLayout(hBox_playlist);
        widget_contentSub->setContentsMargins(0,0,0,0);


        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_contentSub);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(320);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------

        QHBoxLayout *hBox_background = new QHBoxLayout();
        hBox_background->addWidget(tmp_scrollArea);

        QWidget *widget_background = new QWidget();
        widget_background->setLayout(hBox_background);
        widget_background->setContentsMargins(0,0,0,0);
        widget_background->setStyleSheet("background-color:#212121; border:0px;");

        // Apply Main Layout with spacing
        this->box_main_contents->addWidget(widget_background, 0, Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // return
        return hBox_playlist;
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void TidalPlaylistDetail::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void TidalPlaylistDetail::request_more_trackDraw(){


        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > GET_MAX_ITEM_SIZE___ONCE ) ? GET_MAX_ITEM_SIZE___ONCE : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->playlist_tracks[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->playlist_tracks[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TidalPlaylistDetail::slot_clickedItemTrack_inList);
            this->playlist_tracks[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->playlist_tracks[i]);
        }

        QList<QString> tmp_clientkey;
        for(int i = start_index; i < this->track_drawCount; i++){
            this->playlist_tracks[i]->setDataTrackInfo_Tidal(this->list_track->at(i));
            tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));

            QCoreApplication::processEvents();
        }

        if(this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);
        }

        roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
        connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalPlaylistDetail::slot_applyResult_getRating_track);
        proc_fav_track->request_rose_getRating_Track("TIDAL", tmp_clientkey);

        if(this->flag_track_ok == true && this->track_totalCount == this->track_drawCount){
            //this->box_main_contents->addSpacing(70);
            //this->setUIControl_similarPlaylist();
            this->box_main_contents->addSpacing(30);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_draw = false;
    }


    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void TidalPlaylistDetail::request_more_trackData(){

        if((this->track_totalCount > this->list_track->count()) && (this->track_beforeOffset == this->track_currentOffset)){
            this->track_currentOffset += GET_MAX_ITEM_SIZE___ONCE;

            // request HTTP API
            ProcCommon *proc_playlist = new ProcCommon(this);
            connect(proc_playlist, &ProcCommon::completeReq_list_items_of_playlist, this, &TidalPlaylistDetail::slot_applyResult_tracks);
            proc_playlist->request_tidal_getList_items_of_playlist(this->data_playlist.uuid, GET_MAX_ITEM_SIZE___ONCE, this->track_currentOffset);     // 트랙 수 만큼 전체 요청함 like App Proc
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : [Slot]
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief playlist 내의 트랙 아이템들 반환 데이터를 처리함
     * @param list_track
     * @param jsonArr_dataToPlay
     */
    void TidalPlaylistDetail::slot_applyResult_tracks(const QList<tidal::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        this->flag_lastPage_track = flag_lastPage;
        this->flagReqMore_track = false;

        if(this->list_track->count() == 0){
            this->jsonArr_tracks_toPlay = QJsonArray();
        }

        int start_index = this->list_track->count();

        this->list_track->append(list_data);
        ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

        if(start_index == 0){
            int max_cnt = this->list_track->size();
            this->track_drawCount = max_cnt;

            for(int i = start_index; i < max_cnt; i++){
                this->playlist_tracks[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->playlist_tracks[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TidalPlaylistDetail::slot_clickedItemTrack_inList);
                this->playlist_tracks[i]->setProperty("index", i);

                this->box_main_contents->addWidget(this->playlist_tracks[i]);
            }

            QList<QString> tmp_clientkey;
            for(int i = start_index; i < max_cnt; i++){
                this->playlist_tracks[i]->setDataTrackInfo_Tidal(this->list_track->at(i));
                tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));
               // print_debug();


                QCoreApplication::processEvents();
            }

            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalPlaylistDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_getRating_Track("TIDAL", tmp_clientkey);
        }

        this->track_beforeOffset = this->track_currentOffset;

        if((this->track_totalCount == this->track_drawCount) && (this->track_totalCount < 101)){
            this->box_main_contents->addSpacing(30);
        }

        this->request_more_trackData();

        ContentLoadingwaitingMsgHide();
        this->flag_track_ok = true;
    }


    void TidalPlaylistDetail::slot_applyResult_playlist(const tidal::PlaylistItemData &list_data){

        // Update Data
        this->data_playlist = list_data;
        this->track_totalCount = list_data.numberOfTracks;

        // Update UI
        QJsonObject playlistObj;
        playlistObj.insert("image", this->data_playlist.image);
        playlistObj.insert("title", this->data_playlist.title);
        playlistObj.insert("owner", this->data_playlist.creatorName);
        playlistObj.insert("track_count", this->data_playlist.numberOfTracks);
        playlistObj.insert("duration", this->data_playlist.duration);
        playlistObj.insert("lastUpdated", this->data_playlist.lastUpdated);
        playlistObj.insert("description", this->data_playlist.description);
        playlistObj.insert("uuid", this->data_playlist.uuid);

        this->playlist_detail_info->setData_fromTidalData(playlistObj);

        // request HTTP API - get favorite for Rose Server
        roseHome::ProcCommon *proc_fav_playlist = new roseHome::ProcCommon(this);
        connect(proc_fav_playlist, &roseHome::ProcCommon::completeReq_rating_playlist, this, &TidalPlaylistDetail::slot_applyResult_getRating_playlist);
        proc_fav_playlist->request_rose_getRating_Playlist("TIDAL", this->data_playlist.uuid);

        // Playlsit 기본 정보를 가져와야 하는 경우임. (id만 전달받은 상태)
        ProcCommon *proc_tracks = new ProcCommon(this);
        connect(proc_tracks, &ProcCommon::completeReq_list_items_of_playlist, this, &TidalPlaylistDetail::slot_applyResult_tracks);
        proc_tracks->request_tidal_getList_items_of_playlist(this->data_playlist.uuid, GET_MAX_ITEM_SIZE___ONCE, 0);


    }

    //c220818 share link
    void TidalPlaylistDetail::slot_applyResult_getShareLink(const QString &link){

        this->shareLink = link;
    }


    /**
     * @brief playlist 내의 similarPlaylist 아이템들 반환 데이터를 처리함
     * @param list_track
     * @param jsonArr_dataToPlay
     */
    void TidalPlaylistDetail::slot_applyResult_similarPlaylist(const QList<tidal::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);
        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.count() > 0){
            this->list_similarPlaylist->append(list_data);

            int max_cnt = this->list_similarPlaylist->count();

            if(max_cnt > 15){
                max_cnt = 15;
            }

            for(int i = 0; i < max_cnt; i++){
                this->similarPlaylist[i] = new tidal::ItemPlaylist(i, SECTION_FOR_MORE_POPUP___similarlists, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->similarPlaylist[i], &tidal::ItemPlaylist::signal_clicked, this, &TidalPlaylistDetail::slot_clickedItemPlaylist);
            }

            if(this->flag_track_ok == true && this->track_totalCount == this->track_drawCount){
                this->box_main_contents->addSpacing(70);
                this->setUIControl_similarPlaylist();
                this->box_main_contents->addSpacing(30);
            }
        }
        else{
            this->box_main_contents->addSpacing(30);
        }
        print_debug();
    }


    /**
     * @brief TidalPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds
     * @param p_jsonObj
     */
    void TidalPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            // Tidal favorite check
            if(this->flag_send_playlist == true){
                QVariantList arr_myFavoriteIds = ProcJsonEasy::getJsonArray(p_jsonObj, "PLAYLIST").toVariantList();
                bool status = arr_myFavoriteIds.contains(data_playlist.uuid);

                if(status == true && this->flag_playlist_fav == false){
                    // Tidal playlistbFavorite del
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_del_favorite("playlists", data_playlist.uuid);
                }
                else if(status == false && this->flag_playlist_fav == true){
                        // Tidal playlistb Favorite add
                        ProcCommon *proc = new ProcCommon(this);
                        connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                        proc->request_tidal_set_favorite("playlists", data_playlist.uuid);
                }
                this->flag_send_playlist = false;
            }
            else if(this->flag_send_track == true){
                QVariantList arr_myFavoriteIds = ProcJsonEasy::getJsonArray(p_jsonObj, "TRACK").toVariantList();
                bool status = arr_myFavoriteIds.contains(this->track_id_fav);

                if(status == true && this->flag_track_fav == false){
                    // Tidal track Favorite del
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                else if(status == false && this->flag_track_fav == true){
                    // Tidal track Favorite add
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                this->flag_send_track = false;
            }
            else{

            }
        }
    }


    void TidalPlaylistDetail::slot_applyResult_checkRating_palylist(const QJsonObject &jsonObj){

        int id = ProcJsonEasy::getInt(jsonObj, "id");

        if(id <= 0){

            QJsonObject playlist;
            playlist.insert("type", "TIDAL");
            playlist.insert("title", this->data_playlist.title);
            playlist.insert("star", 1);
            playlist.insert("favorite", true);
            playlist.insert("favoriteTrack", false);
            //playlist.insert("thumbup", false);
            //playlist.insert("comment", this->data_playlist.description);
            playlist.insert("clientKey", this->data_playlist.uuid);
            playlist.insert("isRose", false);
            playlist.insert("ownerId", 0);
            playlist.insert("thumbnail", this->data_playlist.image);
            playlist.insert("trackCount", this->data_playlist.numberOfTracks);
            playlist.insert("duration", this->data_playlist.duration);

            QJsonObject json;
            json.insert("playlist", playlist);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_playlist = new roseHome::ProcCommon(this);
            connect(proc_fav_playlist, &roseHome::ProcCommon::completeCheck_rating_playlist, this, &TidalPlaylistDetail::slot_applyResult_addRating_playlist);
            proc_fav_playlist->request_rose_addRating_Playlist(json);
        }
        else{

            roseHome::PlaylistItemData tmp_playlist;
            tmp_playlist.favorite = false;
            tmp_playlist.favoriteTrack = false;
            tmp_playlist.id = id;
            tmp_playlist.isRose = true;
            tmp_playlist.ownerId = 0;
            tmp_playlist.star = 0;
            tmp_playlist.thumbup = false;
            tmp_playlist.thumbupCount = 0;
            tmp_playlist.trackCount = 0;
            tmp_playlist.type = "TIDAL";

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_playlist = new roseHome::ProcCommon(this);
            connect(proc_fav_playlist, &roseHome::ProcCommon::completeReq_rating_playlist, this, &TidalPlaylistDetail::slot_applyResult_getRating_playlist);
            proc_fav_playlist->request_rose_setRating_Playlist(tmp_playlist, this->flag_playlist_fav, this->playlist_star_fav);
        }
        print_debug();
    }


    void TidalPlaylistDetail::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        int id = ProcJsonEasy::getInt(jsonObj, "id");

        QJsonObject jsonObj_track = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();
        if(id <= 0){

            QJsonObject tmp_album = ProcJsonEasy::getJsonObject(jsonObj_track, "album");
            QJsonObject album;
            album.insert("cover", ProcJsonEasy::getString(tmp_album, "cover"));
            album.insert("id", ProcJsonEasy::getInt(tmp_album, "id"));
            album.insert("releaseDate", ProcJsonEasy::getString(tmp_album, "releaseDate"));
            album.insert("title", ProcJsonEasy::getString(tmp_album, "title"));

            QJsonObject tmp_artist = ProcJsonEasy::getJsonObject(jsonObj_track, "artist");
            QJsonObject artist;
            artist.insert("id", ProcJsonEasy::getInt(tmp_artist, "id"));
            artist.insert("name", ProcJsonEasy::getString(tmp_artist, "name"));
            artist.insert("picture", ProcJsonEasy::getInt(tmp_artist, "picture"));

            QJsonArray tmp_artists = ProcJsonEasy::getJsonArray(jsonObj_track, "artists");
            QJsonArray artists;
            for(int i = 0; i < tmp_artists.size(); i++){
                QJsonObject tmpArtist = tmp_artists.at(i).toObject();

                QJsonObject artist;
                artist.insert("id", ProcJsonEasy::getInt(tmpArtist, "id"));
                artist.insert("name", ProcJsonEasy::getString(tmpArtist, "name"));
                artist.insert("picture", ProcJsonEasy::getInt(tmpArtist, "picture"));

                artists.append(artist);
            }

            QJsonObject data;
            data.insert("album", album);
            data.insert("artist", artist);
            data.insert("artists", artists);
            data.insert("audioQuality", ProcJsonEasy::getString(jsonObj_track, "audioQuality"));
            data.insert("copyright", ProcJsonEasy::getString(jsonObj_track, "copyright"));
            data.insert("description", ProcJsonEasy::getString(jsonObj_track, "description"));

            QString thumbnail = ProcJsonEasy::getString(tmp_album, "cover");
            QString thumbnailUrl = tidal::ConvertData::convertImagePath(thumbnail, tidal::ConvertData::ImageSize_Square::Size_750x750);

            QJsonObject track;
            track.insert("clientKey", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            track.insert("data", data);
            track.insert("duration", ProcJsonEasy::getInt(jsonObj_track, "duration"));
            track.insert("favorite", this->flag_track_fav);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 1);
            track.insert("thumbnailUrl", thumbnailUrl);
            track.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            track.insert("type", "TIDAL");

            QJsonArray tracks;
            tracks.append(track);

            QJsonObject json;
            json.insert("tracks", tracks);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalPlaylistDetail::slot_applyResult_addRating_track);
            proc_fav_track->request_rose_addRating_Track(json);
        }
        else{
            //{"ratingInfo":{"favorite":true,"star":1,"thumbup":false,"type":"TIDAL"},
            //"track":{"duration":0,"favorite":false,"id":2546294,"ownerId":0,"sort":0,"star":0,"type":"TIDAL"}}
            QJsonObject ratingInfo;
            ratingInfo.insert("favorite", this->flag_track_fav);
            ratingInfo.insert("star", this->track_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "TIDAL");

            QJsonObject track;
            track.insert("duration", 0);
            track.insert("favorite", false);
            track.insert("id", id);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 0);
            track.insert("type", "TIDAL");

            QJsonObject json;
            json.insert("ratingInfo", ratingInfo);
            json.insert("track", track);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalPlaylistDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

        }
    }


    void TidalPlaylistDetail::slot_applyResult_addRating_playlist(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_playlist == true){
                this->flag_check_playlist = false;
            }
        }
        else{
            this->flag_check_playlist = false;
        }
    }


    void TidalPlaylistDetail::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void TidalPlaylistDetail::slot_applyResult_getRating_playlist(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject tmp_star = contents.at(0).toObject();

            if(tmp_star.contains("star")){
                int star = ProcJsonEasy::getInt(tmp_star, "star");
                bool flag = star > 0 ? true : false;

                this->playlist_detail_info->setFavorite(flag, star);
            }

            if(this->flag_check_playlist == true){
                this->flag_check_playlist = false;
            }
        }
    }


    void TidalPlaylistDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->playlist_tracks[this->track_favoriteOffset + i]->setFavoritesIds(flag, star);
                }

                this->track_favoriteOffset += GET_MAX_ITEM_SIZE___ONCE;
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    /**
     * @brief [Slot] AlbumImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void TidalPlaylistDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

        if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx2 ||
                clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx3 || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){

            if(this->flag_check_playlist == false){
                this->playlist_star_fav = this->playlist_detail_info->getFavoritesStars();
                this->flag_playlist_fav = false;

                if(this->playlist_star_fav == 3){
                    this->playlist_star_fav = 0;
                    this->flag_playlist_fav = false;
                }
                else if(this->playlist_star_fav >= 0 && this->playlist_star_fav < 3){
                    this->playlist_star_fav++;
                    this->flag_playlist_fav = true;
                }

                if(this->playlist_star_fav == 1){
                    // Tidal Playlist Favorite add
                    ProcCommon *proc = new ProcCommon(this);
                    proc->request_tidal_set_favorite("playlists", data_playlist.uuid);
                }
                else if(this->playlist_star_fav == 0){
                    // Tidal Playlist Favorite del
                    ProcCommon *proc = new ProcCommon(this);
                    proc->request_tidal_del_favorite("playlists", data_playlist.uuid);
                }

                // favorite 정보 가져오기
                ProcCommon *proc_fav = new ProcCommon(this);
                connect(proc_fav, &ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                proc_fav->request_tidal_getAll_favorites();
                this->flag_send_playlist = true;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_playlist = new roseHome::ProcCommon(this);
                connect(proc_favCheck_playlist, &roseHome::ProcCommon::completeCheck_rating_playlist, this, &TidalPlaylistDetail::slot_applyResult_checkRating_palylist);
                proc_favCheck_playlist->request_rose_checkRating_Playlist("TIDAL", this->data_playlist.uuid);

                print_debug();

                this->flag_check_playlist = true;

                this->playlist_detail_info->setFavorite(this->flag_playlist_fav, this->playlist_star_fav);
            }
        }
        else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Share){//c220824

            this->shareLink = this->playlist_detail_info->detailInfo_shareLink;     //220905 share code ... --

            //220816 share code ...
            setUIShare();//c220818
        }
        else{
            this->proc_clicked_imageDetailInfo_fromPlaylist(this->data_playlist, this->jsonArr_tracks_toPlay, SECTION_FOR_MORE_POPUP___playlists, clickMode);
        }
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void TidalPlaylistDetail::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___tracks;

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->playlist_tracks[idx]->getFavoritesStars();
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

                if(this->track_star_fav == 1){
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
                connect(proc_fav, &ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                proc_fav->request_tidal_getAll_favorites();
                this->flag_send_track = true;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalPlaylistDetail::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("TIDAL", QString("%1").arg(this->track_id_fav));
                this->flag_check_track = true;

                this->playlist_tracks[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }
        }
        else{
            print_debug();

            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);

        }
    }


    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalPlaylistDetail::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        this->proc_clicked_itemPlaylist(this->list_similarPlaylist, clickMode, index, section);
        print_debug();
    }


    void TidalPlaylistDetail::slot_clickBtn_subTitle_viewAll(){

        /*if(global.user_forTidal.flag_rosehome == true){
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = QString("%1 - Playlist suggetions").arg(this->data_playlist.title);
            data_page.api_subPath = this->list_similarPlaylist->at(0).title;
            data_page.list_similarPlaylist.append(*this->list_similarPlaylist);

            global.user_forTidal.rosehome_obj = QJsonObject();
            global.user_forTidal.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_T_SIMILAR_PLAYLIST_VIEW);
            QJsonObject tmp_data = ConvertData::ConvertData::getObjectJson_pageInfo_playlistAllView(data_page);
            global.user_forTidal.rosehome_obj.insert(KEY_DATA, tmp_data);

            emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::TIDAL));
        }
        else{
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = QString("%1 - Playlist suggetions").arg(this->data_playlist.title);
            data_page.api_subPath = this->list_similarPlaylist->at(0).title;
            data_page.list_similarPlaylist.append(*this->list_similarPlaylist);

            this->movePage_similarPlaylist_allView(data_page);        // 페이지 이동 signal
        }*/
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void TidalPlaylistDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        print_debug();
        if(section == SECTION_FOR_MORE_POPUP___playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->data_playlist, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___similarlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_similarPlaylist, index, section, clickMode);
        }
        print_debug();
    }

    void TidalPlaylistDetail::slot_tidal_logChange(){

        this->widget_login_contents->hide();

        this->setJsonObject_forData(this->login_wait_Object);
        this->setActivePage();
    }

}
