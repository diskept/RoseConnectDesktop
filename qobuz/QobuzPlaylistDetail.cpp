#include "qobuz/QobuzPlaylistDetail.h"

#include "qobuz/ProcCommon_forQobuz.h"
#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ItemPlaylist_forQobuz.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"
#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"

#include <QDebug>
#include <QPainter>
#include <QScroller>
#include <QScrollArea>
#include <QPixmapCache>
#include <QJsonDocument>


namespace qobuz {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___playlists = 1;
    const int SECTION_FOR_MORE_POPUP___tracks = 2;
    const int SECTION_FOR_MORE_POPUP___similarlists = 3;

    const int GET_MAX_ITEM_SIZE___ONCE = 100;


    /**
     * @brief
     * @param parent
     */
    QobuzPlaylistDetail::QobuzPlaylistDetail(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll_filter, parent) {

        linker = Linker::getInstance();

        // data
        this->list_track = new QList<qobuz::TrackItemData>();
        this->list_similarPlaylist = new QList<qobuz::PlaylistItemData>();
    }

    /**
     * @brief 소멸자.
     */
    QobuzPlaylistDetail::~QobuzPlaylistDetail(){

        this->deleteLater();
    }


    /**
     * @brief QobuzPlaylistDetail::setJsonObject_forData
     * @param jsonObj
     */
    void QobuzPlaylistDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        qobuz::PlaylistItemData tmp_data_playlist = ConvertData::convertData_playlistData(jsonObj);

        this->flagNeedReload = false;

        if(global.user_forQobuz.isLogined() == false){

            this->login_wait_Object = QJsonObject();
            this->login_wait_Object = jsonObj;

            this->qobuzSettings = new QobuzSettings();
            this->qobuzSettings->setActivePage();

            this->box_login_contents = new QVBoxLayout();
            this->box_login_contents->setSpacing(0);
            this->box_login_contents->setContentsMargins(0, 0, 0, 0);
            this->box_login_contents->addWidget(this->qobuzSettings);

            this->widget_login_contents = new QWidget();
            this->widget_login_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_login_contents->setLayout(this->box_login_contents);

            this->box_contents->addWidget(this->widget_login_contents);

            connect(this->qobuzSettings, &QobuzSettings::signal_logState_change, this, &QobuzPlaylistDetail::slot_qobuz_logChange);
        }
        else{

            if(tmp_data_playlist.id != this->data_playlist.id){

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

                // Playlsit 기본 정보를 가져와야 하는 경우임. (id만 전달받은 상태)
                ProcCommon *proc_playlist = new ProcCommon(this);
                connect(proc_playlist, &ProcCommon::completeReq_playlist, this, &QobuzPlaylistDetail::slot_applyResult_playlistInfo);
                connect(proc_playlist, &ProcCommon::completeReq_list_items_of_playlist, this, &QobuzPlaylistDetail::slot_applyResult_tracks);
                connect(proc_playlist, &ProcCommon::completeReq_list_playlists, this, &QobuzPlaylistDetail::slot_applyResult_similarPlaylist);
                proc_playlist->request_qobuz_getList_items_of_playlist(this->data_playlist.id, GET_MAX_ITEM_SIZE___ONCE, 0);

                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }
            else{
                // 리로드 하지 않는 경우에는, favorite 정보만 다시 요청한다. (playlist_id 가 변경되지 않고, 페이지가 다시 요청된 경우임)
                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_playlist = new roseHome::ProcCommon(this);
                connect(proc_fav_playlist, &roseHome::ProcCommon::completeReq_rating_playlist, this, &QobuzPlaylistDetail::slot_applyResult_getRating_playlist);
                proc_fav_playlist->request_rose_getRating_Playlist("QOBUZ", this->data_playlist.id);
            }
        }
    }


    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void QobuzPlaylistDetail::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractQobuzSubWidget::setActivePage();

        if(this->flagNeedReload){

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
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            this->setUIControl_basic();
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief QobuzPlaylistDetail::setUIControl_playlist_info
     */
    void QobuzPlaylistDetail::setUIControl_basic(){

        // Playlist 상세정보
        this->playlist_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Qobuz_playlist);
        connect(this->playlist_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &QobuzPlaylistDetail::slot_detailInfo_btnClicked);

        this->playlist_detail_info->initView();

        this->box_main_contents->addWidget(this->playlist_detail_info);


    }


    void QobuzPlaylistDetail::setUIControl_similarPlaylist(){

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


    void QobuzPlaylistDetail::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText){

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

        this->box_main_contents->addWidget(widget_box_subTitle);
    }


    QHBoxLayout* QobuzPlaylistDetail::setUIControl_hBoxLayout_forPlaylists(){

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
        tmp_scrollArea->setFixedHeight(293);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------

        QHBoxLayout *hBox_background = new QHBoxLayout();
        hBox_background->addWidget(tmp_scrollArea);

        QWidget *widget_background = new QWidget();
        widget_background->setLayout(hBox_background);
        widget_background->setContentsMargins(0,0,0,0);
        widget_background->setStyleSheet("background-color:#212121; border:0px;");

        // Apply Main Layout with spacing
        this->box_main_contents->addWidget(widget_background);

        // return
        return hBox_playlist;
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void QobuzPlaylistDetail::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)){

            this->flag_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    void QobuzPlaylistDetail::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > 100 ) ? 100 : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->playlist_tracks[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->playlist_tracks[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzPlaylistDetail::slot_clickedItemTrack_inList);
            this->playlist_tracks[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->playlist_tracks[i]);
        }

        QList<QString> tmp_clientkey;
        for(int i = start_index; i < this->track_drawCount; i++){
            this->playlist_tracks[i]->setDataTrackInfo_Qobuz(this->list_track->at(i));
            tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));

            QCoreApplication::processEvents();
        }

        roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
        connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzPlaylistDetail::slot_applyResult_getRating_track);
        proc_fav_track->request_rose_getRating_Track("QOBUZ", tmp_clientkey);

        if(this->flag_track_ok == true && this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);

            if(this->list_similarPlaylist->size() > 0){
                this->setUIControl_similarPlaylist();
            }
        }

        ContentLoadingwaitingMsgHide();
        this->flag_draw = false;
    }

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void QobuzPlaylistDetail::request_more_trackData(){

        if((this->track_totalCount > this->list_track->count()) && (this->track_beforeOffset == this->track_currentOffset)){
            this->track_currentOffset += GET_MAX_ITEM_SIZE___ONCE;

            // request HTTP API
            ProcCommon *proc_playlist = new ProcCommon(this);
            connect(proc_playlist, &ProcCommon::completeReq_list_items_of_playlist, this, &QobuzPlaylistDetail::slot_applyResult_tracks);
            proc_playlist->request_qobuz_getList_items_of_playlist(this->data_playlist.id, GET_MAX_ITEM_SIZE___ONCE, this->track_currentOffset);     // 트랙 수 만큼 전체 요청함 like App Proc
        }
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : [Slot]
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief QobuzPlaylistDetail::slot_applyResult_playlistInfo
     * @param data_playlist
     */
    void QobuzPlaylistDetail::slot_applyResult_playlistInfo(const qobuz::PlaylistItemData data_playlist){
        ContentLoadingwaitingMsgHide();//c220616
        // Update Data
        this->track_totalCount = data_playlist.tracks_count;

        this->data_playlist = data_playlist;

        // Update UI
        QJsonObject playlistObj;
        playlistObj.insert("image", this->data_playlist.image);
        playlistObj.insert("title", this->data_playlist.title);
        playlistObj.insert("hires", this->data_playlist.hires);
        playlistObj.insert("owner", this->data_playlist.ownerName);
        playlistObj.insert("track_count", this->data_playlist.tracks_count);
        playlistObj.insert("duration", this->data_playlist.duration);
        playlistObj.insert("lastUpdated", this->data_playlist.lastUpdated);
        playlistObj.insert("description", this->data_playlist.description);
        playlistObj.insert("id", this->data_playlist.id);

        this->playlist_detail_info->setData_fromQobuzData(playlistObj);

        // request HTTP API - get favorite for Rose Server
        roseHome::ProcCommon *proc_fav_playlist = new roseHome::ProcCommon(this);
        connect(proc_fav_playlist, &roseHome::ProcCommon::completeReq_rating_playlist, this, &QobuzPlaylistDetail::slot_applyResult_getRating_playlist);
        proc_fav_playlist->request_rose_getRating_Playlist("QOBUZ", this->data_playlist.id);
    }


    //c220818 share link
    void QobuzPlaylistDetail::slot_applyResult_getShareLink(const QString &link){

        this->shareLink = link;
    }

    /**
     * @brief playlist 내의 트랙 아이템들 반환 데이터를 처리함
     * @param list_track
     * @param jsonArr_dataToPlay
     */
    void QobuzPlaylistDetail::slot_applyResult_tracks(const QList<qobuz::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                connect(this->playlist_tracks[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzPlaylistDetail::slot_clickedItemTrack_inList);
                this->playlist_tracks[i]->setProperty("index", i);

                this->box_main_contents->addWidget(this->playlist_tracks[i]);
            }

            QList<QString> tmp_clientkey;
            for(int i = start_index; i < max_cnt; i++){
                this->playlist_tracks[i]->setDataTrackInfo_Qobuz(this->list_track->at(i));
                tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));

                QCoreApplication::processEvents();
            }

            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzPlaylistDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_getRating_Track("QOBUZ", tmp_clientkey);
        }

        this->track_beforeOffset = this->track_currentOffset;

        if(this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);
        }
        else{
            this->request_more_trackData();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_track_ok = true;
    }


    /**
     * @brief playlist 내의 similarPlaylist 아이템들 반환 데이터를 처리함
     * @param list_track
     * @param jsonArr_dataToPlay
     */
    void QobuzPlaylistDetail::slot_applyResult_similarPlaylist(const QList<qobuz::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);
        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.count() > 0){
            this->list_similarPlaylist->append(list_data);

            int max_cnt = this->list_similarPlaylist->count();

            if(max_cnt > 15){
                max_cnt = 15;
            }

            for(int i = 0; i < max_cnt; i++){
                this->similarPlaylist[i] = new qobuz::ItemPlaylist_qobuz(i, SECTION_FOR_MORE_POPUP___similarlists, tidal::AbstractItem::ImageSizeMode::Ractangle_360x176);
                connect(this->similarPlaylist[i], &qobuz::ItemPlaylist_qobuz::signal_clicked, this, &QobuzPlaylistDetail::slot_clickedItemPlaylist);
            }

            if(this->flag_track_ok == true && this->track_totalCount == this->track_drawCount){

                this->setUIControl_similarPlaylist();
            }
        }
    }


    /**
     * @brief QobuzPlaylistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds
     * @param p_jsonObj
     */
    void QobuzPlaylistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
            bool status  = ProcJsonEasy::getBool(p_jsonObj, "status");

            // Qobuz favorite toggle check
            if(this->flag_send_playlist == true){
                if((status == true && this->flag_playlist_fav == false) || (status == false && this->flag_playlist_fav == true)){
                    // Qobuz Favorite toggle
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzPlaylistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("playlist", QString("%1").arg(data_playlist.id), this->flag_playlist_fav);
                }
                this->flag_send_playlist = false;
            }
            else if(this->flag_send_track == true){
                if((status == true && this->flag_track_fav == false) || (status == false && this->flag_track_fav == true)){
                    // Qobuz Favorite toggle
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzPlaylistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                }
                this->flag_send_track = false;
            }
            else{

            }
        }
    }


    void QobuzPlaylistDetail::slot_applyResult_checkRating_palylist(const QJsonObject &jsonObj){

        int id = ProcJsonEasy::getInt(jsonObj, "id");

        if(id <= 0){

            QJsonObject playlist;
            playlist.insert("type", "QOBUZ");
            playlist.insert("title", this->data_playlist.title);
            playlist.insert("star", 1);
            playlist.insert("favorite", true);
            playlist.insert("favoriteTrack", false);
            //playlist.insert("thumbup", false);
            //playlist.insert("comment", this->data_playlist.description);
            playlist.insert("clientKey", this->data_playlist.id);
            playlist.insert("isRose", false);
            playlist.insert("ownerId", 0);
            playlist.insert("thumbnail", this->data_playlist.image);
            playlist.insert("trackCount", this->data_playlist.tracks_count);

            QJsonObject json;
            json.insert("playlist", playlist);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_playlist = new roseHome::ProcCommon(this);
            connect(proc_fav_playlist, &roseHome::ProcCommon::completeCheck_rating_playlist, this, &QobuzPlaylistDetail::slot_applyResult_addRating_playlist);
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
            tmp_playlist.type = "QOBUZ";

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_playlist = new roseHome::ProcCommon(this);
            connect(proc_fav_playlist, &roseHome::ProcCommon::completeReq_rating_playlist, this, &QobuzPlaylistDetail::slot_applyResult_getRating_playlist);
            proc_fav_playlist->request_rose_setRating_Playlist(tmp_playlist, this->flag_playlist_fav, this->playlist_star_fav);
        }
    }


    void QobuzPlaylistDetail::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        int id = ProcJsonEasy::getInt(jsonObj, "id");

        QJsonObject jsonObj_track = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();
        if(id <= 0){

            QJsonObject tmp_album = ProcJsonEasy::getJsonObject(jsonObj_track, "album");

            QJsonObject tmp_artist = ProcJsonEasy::getJsonObject(tmp_album, "artist");
            QJsonObject artist;
            artist.insert("albums_count", ProcJsonEasy::getInt(tmp_artist, "albums_count"));
            artist.insert("id", ProcJsonEasy::getInt(tmp_artist, "id"));
            artist.insert("name", ProcJsonEasy::getString(tmp_artist, "name"));

            QJsonObject tmp_label = ProcJsonEasy::getJsonObject(tmp_album, "label");
            QJsonObject label;
            label.insert("albums_count", ProcJsonEasy::getInt(tmp_label, "albums_count"));
            label.insert("id", ProcJsonEasy::getInt(tmp_label, "id"));
            label.insert("name", ProcJsonEasy::getString(tmp_label, "name"));

            QJsonObject album;
            album.insert("artist", artist);
            album.insert("duration", ProcJsonEasy::getInt(tmp_album, "duration"));
            album.insert("favCnt", 0);
            album.insert("hires", false);
            album.insert("hires_streamable", false);
            album.insert("id", ProcJsonEasy::getString(tmp_album, "id"));
            album.insert("image", ProcJsonEasy::getJsonObject(tmp_album, "image"));
            album.insert("label", label);
            album.insert("streamable", false);
            album.insert("title", ProcJsonEasy::getString(tmp_album, "title"));
            album.insert("tracks_count", 0);

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
            data.insert("work", ProcJsonEasy::getString(jsonObj_track, "work"));

            QJsonObject tmp_thumbnail = ProcJsonEasy::getJsonObject(tmp_album, "image");
            QString thumbnail = ProcJsonEasy::getString(tmp_thumbnail, "large");

            QJsonObject track;
            track.insert("clientKey", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            track.insert("data", data);
            track.insert("duration", ProcJsonEasy::getInt(jsonObj_track, "duration"));
            track.insert("favorite", this->flag_track_fav);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 1);
            track.insert("thumbnailUrl", thumbnail);
            track.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            track.insert("type", "QOBUZ");

            QJsonArray tracks;
            tracks.append(track);

            QJsonObject json;
            json.insert("tracks", tracks);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzPlaylistDetail::slot_applyResult_addRating_track);
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
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzPlaylistDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

        }
    }


    void QobuzPlaylistDetail::slot_applyResult_addRating_playlist(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_playlist == true){
                this->flag_check_playlist = false;
            }
        }
        else{
            this->flag_check_playlist = false;
        }
    }


    void QobuzPlaylistDetail::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void QobuzPlaylistDetail::slot_applyResult_getRating_playlist(const QJsonArray &contents){

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


    void QobuzPlaylistDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

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
    void QobuzPlaylistDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

        print_debug();
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

                if(this->playlist_star_fav == 0 || this->playlist_star_fav == 1){
                    // Qobuz Favorite subscribe/unsubscribe
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzPlaylistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("playlist", QString("%1").arg(data_playlist.id), this->flag_playlist_fav);
                    this->flag_send_playlist = true;
                }

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_playlist = new roseHome::ProcCommon(this);
                connect(proc_favCheck_playlist, &roseHome::ProcCommon::completeCheck_rating_playlist, this, &QobuzPlaylistDetail::slot_applyResult_checkRating_palylist);
                proc_favCheck_playlist->request_rose_checkRating_Playlist("QOBUZ", this->data_playlist.id);



                this->flag_check_playlist = true;

                this->playlist_detail_info->setFavorite(this->flag_playlist_fav, this->playlist_star_fav);
            }
        }
        else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Share){

            this->shareLink = this->playlist_detail_info->detailInfo_shareLink;     //220905 share code ... --

            //220816 share code ... --
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
   void QobuzPlaylistDetail::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

       print_debug();
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

                if(this->track_star_fav == 0 || this->track_star_fav == 1){
                    // Qobuz Favorite toggle
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzPlaylistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                    this->flag_send_track = true;
                }

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzPlaylistDetail::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("QOBUZ", QString("%1").arg(this->track_id_fav));
                this->flag_check_track = true;

                this->playlist_tracks[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }
        }
        else{

            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzPlaylistDetail::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        this->proc_clicked_itemPlaylist(this->list_similarPlaylist, clickMode, index, section);
    }


    void QobuzPlaylistDetail::slot_clickBtn_subTitle_viewAll(){

        if(global.user_forQobuz.flag_rosehome == true){
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = QString("%1 - Playlist suggetions").arg(this->data_playlist.title);
            data_page.api_subPath = this->list_similarPlaylist->at(0).title;
            data_page.list_similarPlaylist.append(*this->list_similarPlaylist);

            global.user_forQobuz.rosehome_obj = QJsonObject();
            global.user_forQobuz.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_SIMILAR_PLAYLIST_VIEW);
            QJsonObject tmp_data = ConvertData::ConvertData::getObjectJson_pageInfo_playlistAllView(data_page);
            global.user_forQobuz.rosehome_obj.insert(KEY_DATA, tmp_data);

            emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Qobuz));
        }
        else{
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = QString("%1 - Playlist suggetions").arg(this->data_playlist.title);
            data_page.api_subPath = this->list_similarPlaylist->at(0).title;
            data_page.list_similarPlaylist.append(*this->list_similarPlaylist);

            this->movePage_similarPlaylist_allView(data_page);        // 페이지 이동 signal*/
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void QobuzPlaylistDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->data_playlist, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___similarlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_similarPlaylist, index, section, clickMode);
        }
    }


    void QobuzPlaylistDetail::slot_qobuz_logChange(){

        this->widget_login_contents->hide();

        this->setJsonObject_forData(this->login_wait_Object);
        this->setActivePage();
    }

}
