#include "apple/ApplePlaylistDetail.h"

#include "apple/ConvertData_forApple.h"
#include "apple/ProcCommon_forApple.h"
#include "apple/ProcRosePlay_withApple.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"

#include <QDebug>
#include <QPainter>
#include <QScrollArea>
#include <QPixmapCache>


namespace apple {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___playlist = 1;
    const int SECTION_FOR_MORE_POPUP___track = 2;


    /**
     * @brief
     * @param parent
     */
    ApplePlaylistDetail::ApplePlaylistDetail(QWidget *parent) : AbstractAppleSubWidget(MainUIType::VerticalScroll_filter, parent) {

    }


    /**
     * @brief 소멸자.
     */
    ApplePlaylistDetail::~ApplePlaylistDetail(){

        this->deleteLater();
    }


    /**
     * @brief ApplePlaylistDetail::setJsonObject_forData
     * @param jsonObj
     */
    void ApplePlaylistDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        apple::PlaylistItemData tmp_data_playlist = apple::ConvertData::convertData_playlistData(jsonObj);
        this->flagNeedReload = false;

        if(global.user_forApple.isLogined() == false){

            this->login_wait_Object = QJsonObject();
            this->login_wait_Object = jsonObj;

            this->appleSettings = new apple::AppleSettings();
            this->appleSettings->setActivePage();

            this->box_login_contents = new QVBoxLayout();
            this->box_login_contents->setSpacing(0);
            this->box_login_contents->setContentsMargins(0, 0, 0, 0);
            this->box_login_contents->addWidget(this->appleSettings);

            this->widget_login_contents = new QWidget();
            this->widget_login_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_login_contents->setLayout(this->box_login_contents);

            this->box_contents->addWidget(this->widget_login_contents);

            //connect(this->qobuzSettings, &QobuzSettings::signal_logState_change, this, &ApplePlaylistDetail::slot_qobuz_logChange);
        }
        else{

            if(tmp_data_playlist.id != this->data_playlist.id){

                this->flagNeedReload = true;

                if(this->flag_track_ok == true){
                    this->widget_main_contents->hide();

                    GSCommon::clearLayout(this->box_main_contents);
                    this->box_contents->removeWidget(this->widget_main_contents);

                    this->flag_track_ok = false;
                }

                this->data_playlist = tmp_data_playlist;

                this->jsonObj_playlistInfo = QJsonObject();
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

                this->flag_draw = false;

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

                apple::ProcCommon *proc_playlist = new apple::ProcCommon(this);
                connect(proc_playlist, &apple::ProcCommon::completeReq_playlist, this, &ApplePlaylistDetail::slot_applyResult_playlist);
                connect(proc_playlist, &apple::ProcCommon::completeReq_tracks, this, &ApplePlaylistDetail::slot_applyResult_playlist_tracks);
                proc_playlist->request_apple_get_playlist(this->data_playlist.id);
            }
            else{

            }
        }
    }


    /**
     * @brief ApplePlaylistDetail 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void ApplePlaylistDetail::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractAppleSubWidget::setActivePage();

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

            this->setUIControl_basic();
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 화면 UI 세팅
     */
    void ApplePlaylistDetail::setUIControl_basic(){

        this->playlist_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Apple_playlist);
        connect(this->playlist_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &ApplePlaylistDetail::slot_detailInfo_btnClicked);

        this->playlist_detail_info->initView();

        this->box_main_contents->addWidget(this->playlist_detail_info);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : [Slot]
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void ApplePlaylistDetail::slot_applyResult_playlist(const QJsonObject &jsonObj){

        this->jsonObj_playlistInfo = jsonObj;
        this->playlist_detail_info->setData_fromAppleData(jsonObj);

        // request HTTP API - favorite for Rose Server
        roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
        connect(proc_fav, &roseHome::ProcCommon::completeCheck_rating_playlist, this, &ApplePlaylistDetail::slot_applyResult_checkRating_playlist);
        proc_fav->request_rose_checkRating_Playlist("APPLE_MUSIC", this->data_playlist.id);
    }


    void ApplePlaylistDetail::slot_applyResult_playlist_tracks(const QJsonArray &jsonArr){

        if(jsonArr.size() > 0){

            this->jsonArr_tracks_toPlay = QJsonArray();
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr);

            int max_cnt = this->jsonArr_tracks_toPlay.size();

            int makeDuration = 0;
            for(int i = 0; i < max_cnt; i++){
                QJsonObject tmpTrack = this->jsonArr_tracks_toPlay.at(i).toObject();
                QJsonObject tmpAttributes = ProcJsonEasy::getJsonObject(tmpTrack, "attributes");
                makeDuration += ProcJsonEasy::getInt(tmpAttributes, "durationInMillis");
            }
            makeDuration /= 1000;

            // Update UI
            QJsonObject playlistObj;
            playlistObj.insert("track_count", max_cnt);
            playlistObj.insert("duration", makeDuration);
            playlistObj.insert("registDateTime", ProcJsonEasy::getString(this->jsonObj_playlistInfo, "lastModifiedDate"));

            this->playlist_detail_info->setData_Resolution(playlistObj);

            for(int i = 0; i < max_cnt; i++){
                this->playlist_tracks[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->playlist_tracks[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &ApplePlaylistDetail::slot_clickedItemTrack_inList);
                this->playlist_tracks[i]->setProperty("index", i);

                this->box_main_contents->addWidget(this->playlist_tracks[i]);
            }

            ContentLoadingwaitingMsgHide();

            QList<QString> tmp_clientkey;
            for(int i = 0; i < max_cnt; i++){
                QJsonObject track = this->jsonArr_tracks_toPlay.at(i).toObject();
                this->playlist_tracks[i]->setDataTrackInfo_Apple(track);

                QString id = ProcJsonEasy::getString(track, "id");
                tmp_clientkey.append(id);

                QCoreApplication::processEvents();
            }

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &ApplePlaylistDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_getRating_Track("APPLE_MUSIC", tmp_clientkey);

            this->box_main_contents->addSpacing(30);

            this->flag_track_ok = true;
        }
        else{
            this->flag_track_ok = true;

            ContentLoadingwaitingMsgHide();


            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void ApplePlaylistDetail::slot_applyResult_checkRating_playlist(const QJsonObject &jsonObj){

        this->roseServer_playlistId = ProcJsonEasy::getInt(jsonObj, "id");

        if(this->roseServer_playlistId > 0){

            if(this->flag_check_playlist == true){

                QJsonObject playlist;
                playlist.insert("favorite", false);
                playlist.insert("id", this->roseServer_playlistId);
                playlist.insert("star", 0);
                playlist.insert("trackCount", 0);
                playlist.insert("type", "APPLE_MUSIC");

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_playlist_fav);
                ratingInfo.insert("star", this->playlist_star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", "APPLE_MUSIC");

                QJsonObject json;
                json.insert("playlist", playlist);
                json.insert("ratingInfo", ratingInfo);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_playlist = new roseHome::ProcCommon(this);
                connect(proc_fav_playlist, &roseHome::ProcCommon::completeReq_rating_playlist, this, &ApplePlaylistDetail::slot_applyResult_getRating_playlist);
                proc_fav_playlist->request_rose_setRating_rosePlaylist(json, this->flag_playlist_fav, this->playlist_star_fav);
            }
            else{

                roseHome::ProcCommon *proc_fav_playlist = new roseHome::ProcCommon(this);
                connect(proc_fav_playlist, &roseHome::ProcCommon::completeReq_rating_playlist, this, &ApplePlaylistDetail::slot_applyResult_getRating_playlist);
                proc_fav_playlist->request_rose_getRating_Playlist("APPLE_MUSIC", ProcJsonEasy::getString(this->jsonObj_playlistInfo, "id"));
            }
        }
        else{

            QJsonObject artwork = ProcJsonEasy::getJsonObject(this->jsonObj_playlistInfo, "artwork");
            QString url = ProcJsonEasy::getString(artwork, "url");
            url.replace("{w}x{h}", "600x600");

            QJsonObject playlist;
            playlist.insert("clientKey", ProcJsonEasy::getString(this->jsonObj_playlistInfo, "id"));
            playlist.insert("favorite", false);
            playlist.insert("star", 0);
            playlist.insert("thumbnail", url);
            playlist.insert("title", ProcJsonEasy::getString(this->jsonObj_playlistInfo, "name"));
            playlist.insert("trackCount", 0);
            playlist.insert("type", "APPLE_MUSIC");

            QJsonObject json;
            json.insert("playlist", playlist);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_playlist = new roseHome::ProcCommon(this);
            connect(proc_fav_playlist, &roseHome::ProcCommon::completeCheck_rating_playlist, this, &ApplePlaylistDetail::slot_applyResult_addRating_playlist);
            proc_fav_playlist->request_rose_addRating_Playlist(json);
        }
    }


    void ApplePlaylistDetail::slot_applyResult_getRating_playlist(const QJsonArray &contents){

        if(contents.size() > 0){

            QJsonObject tmp_star = contents.at(0).toObject();

            if(tmp_star.contains("star")){

                int star = ProcJsonEasy::getInt(tmp_star, "star");
                bool flag = star > 0 ? true : false;

                this->playlist_detail_info->setFavorite(flag, star);
            }
            else{

                this->playlist_detail_info->setFavorite(this->flag_playlist_fav, this->playlist_star_fav);
            }

            if(this->flag_check_playlist == true){
                this->flag_check_playlist = false;
            }
        }
    }


    void ApplePlaylistDetail::slot_applyResult_addRating_playlist(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){

            // request HTTP API - favorite for Rose Server
            roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
            connect(proc_fav, &roseHome::ProcCommon::completeCheck_rating_playlist, this, &ApplePlaylistDetail::slot_applyResult_checkRating_playlist);
            proc_fav->request_rose_checkRating_Playlist("APPLE_MUSIC", this->data_playlist.id);

            if(this->flag_check_playlist == true){
                this->flag_check_playlist = false;
            }
        }
        else{
            this->flag_check_playlist = false;
        }
    }


    void ApplePlaylistDetail::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("id")){

            int id = ProcJsonEasy::getInt(jsonObj, "id");

            if(id > 0){
                this->roseServer_trackId[this->track_idx_fav] = id;

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_track_fav);
                ratingInfo.insert("star", this->track_star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", "APPLE_MUSIC");

                QJsonObject track;
                track.insert("duration", 0);
                track.insert("favorite", false);
                track.insert("id", id);
                track.insert("ownerId", 0);
                track.insert("sort", 0);
                track.insert("star", 0);
                track.insert("type", "APPLE_MUSIC");

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &ApplePlaylistDetail::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
            }
            else{

                QJsonObject data = this->jsonArr_tracks_toPlay[this->track_idx_fav].toObject();

                QJsonObject attributes = ProcJsonEasy::getJsonObject(data, "attributes");
                QJsonObject artwork = ProcJsonEasy::getJsonObject(attributes, "artwork");

                QString url = ProcJsonEasy::getString(artwork, "url");
                url.replace("{w}x{h}", "600x600");

                QJsonArray albums = QJsonArray();
                albums.append(ProcJsonEasy::getString(attributes, "albumName"));

                QJsonArray artists = QJsonArray();
                artists.append(ProcJsonEasy::getString(attributes, "artistName"));

                QJsonObject track;
                track.insert("title", ProcJsonEasy::getString(attributes, "name"));
                track.insert("type", "APPLE_MUSIC");
                track.insert("favorite", false);
                track.insert("artists", artists);
                track.insert("albums", albums);
                track.insert("star", 0);
                track.insert("thumbnailUrl", url);
                track.insert("clientKey", ProcJsonEasy::getString(data, "id"));
                track.insert("duration", ProcJsonEasy::getInt(attributes, "durationInMillis"));
                track.insert("data", data);

                QJsonArray tracks;
                tracks.append(track);

                QJsonObject json;
                json.insert("tracks", tracks);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &ApplePlaylistDetail::slot_applyResult_addRating_track);
                proc_fav_track->request_rose_addRating_Track(json);
            }
        }
        else{

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void ApplePlaylistDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){

            for(int i = 0; i < contents.size(); i++){

                QJsonObject track = contents.at(i).toObject();

                if(track.contains("star")){
                    QString clientKey = ProcJsonEasy::getString(track, "clientKey");
                    int star = ProcJsonEasy::getInt(track, "star");

                    QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(i).toObject();
                    QString trackID = ProcJsonEasy::getString(trackInfo, "id");

                    if(clientKey == trackID){

                        bool flag = (star > 0) ? true : false;
                        this->playlist_tracks[i]->setFavoritesIds(flag, star);
                    }
                }
                else{

                    this->playlist_tracks[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
                }

            }
        }

        if(this->flag_check_track == true){
            this->flag_check_track = false;
        }
    }


    void ApplePlaylistDetail::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
            connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &ApplePlaylistDetail::slot_applyResult_checkRating_track);
            proc_favCheck_track->request_rose_checkRating_Track("APPLE_MUSIC", ProcJsonEasy::getString(this->jsonArr_tracks_toPlay[this->track_idx_fav].toObject(), "id"));

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void ApplePlaylistDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

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

                /*if(this->playlist_star_fav == 0 || this->playlist_star_fav == 1){
                    // Apple Favorite toggle
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzplaylistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("playlist", QString("%1").arg(data_playlist.id), this->flag_playlist_fav);
                    this->flag_send_playlist = true;
                }*/

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_playlist = new roseHome::ProcCommon(this);
                connect(proc_favCheck_playlist, &roseHome::ProcCommon::completeCheck_rating_playlist, this, &ApplePlaylistDetail::slot_applyResult_checkRating_playlist);
                proc_favCheck_playlist->request_rose_checkRating_Playlist("APPLE_MUSIC", this->data_playlist.id);

                this->flag_check_playlist = true;
            }
        }
        else{
            if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::AddCollection){

            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){

                // OptMorePopup 띄우기 - 즐겨찾기 관련 메뉴를 가림. DetailView에 하트가 있음
                this->makeObj_optMorePopup(OptMorePopup::Apple_Playlist, apple::ConvertData::getConvertOptHeaderData_playlist(this->jsonObj_playlistInfo), 0, SECTION_FOR_MORE_POPUP___playlist, true);
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayAll){

                // 전체재생
                apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
                procRosePlay->requestPlayRose_byTracks(this->jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, apple::ProcRosePlay_withApple::PlayShuffleMode::JustPlay);
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayShuffle){

                // 셔플재생
                apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
                procRosePlay->requestPlayRose_byTracks(this->jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, apple::ProcRosePlay_withApple::PlayShuffleMode::ShufflePlay);
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Share){

                this->shareLink = this->playlist_detail_info->detailInfo_shareLink; //220905 share code ... --

                setUIShare();//c220818
            }
        }
    }


    void ApplePlaylistDetail::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                int star_cnt = this->playlist_tracks[idx]->getFavoritesStars();
                this->flag_track_fav = false;

                this->track_star_fav = star_cnt;

                if(this->track_star_fav == 3){
                    this->track_star_fav = 0;
                    this->flag_track_fav = false;
                }
                else if(this->track_star_fav >= 0 && this->track_star_fav < 3){
                    this->track_star_fav++;
                    this->flag_track_fav = true;
                }

                /*if(this->track_star_fav == 0 || this->track_star_fav == 1){
                    // Apple Favorite toggle
                    this->track_id_fav = this->list_track->at(idx).id;

                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzAlbumDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                    this->flag_send_track = true;
                }*/

                this->track_idx_fav = idx;
                QJsonObject jsonObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &ApplePlaylistDetail::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("APPLE_MUSIC", ProcJsonEasy::getString(jsonObj, "id"));

                this->flag_check_track = true;
            }
        }
        else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AllBox){

            int playType = global.device.getMusicPlayType();

            OptMorePopup::ClickMode curr_clickMode = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
            switch(playType)
            {
                case 13:
                    curr_clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                    break;
                case 12:
                    curr_clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                    break;
                case 15:
                    curr_clickMode = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    break;
                case 16:
                    curr_clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                    break;
                case 17:
                    curr_clickMode = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                    break;
                case 18:
                    curr_clickMode = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                    break;
                case 19:
                    curr_clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                    break;
            }

            apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
            procRosePlay->requestPlayRose_byTracks(this->jsonArr_tracks_toPlay, idx, curr_clickMode);
        }
        else{
            // OptMorePopup 띄우기 - 즐겨찾기 관련 메뉴를 가림. DetailView에 하트가 있음
            this->makeObj_optMorePopup(OptMorePopup::Apple_Track, apple::ConvertData::getConvertOptHeaderData_track(this->jsonArr_tracks_toPlay.at(idx).toObject()), idx, SECTION_FOR_MORE_POPUP___track, true);
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void ApplePlaylistDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___playlist){

            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                // Rose Play 요청
                apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
                procRosePlay->requestPlayRose_byTracks(this->jsonArr_tracks_toPlay, 0, clickMode);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___track){

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
                // Rose Play 요청// Rose Play 요청
                apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
                procRosePlay->requestPlayRose_byTracks(this->jsonArr_tracks_toPlay, index, clickMode);
            }
        }
    }
}
