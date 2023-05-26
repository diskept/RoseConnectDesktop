#include "tidal/TidalMyPlaylistDetail.h"

#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

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

//    const int GET_ITEM_SIZE___ONCE = 30;
    const int GET_MAX_ITEM_SIZE___ONCE = 99;


    /**
     * @brief
     * @param parent
     */
    TidalMyPlaylistDetail::TidalMyPlaylistDetail(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_filter, parent) {

        // data
        this->list_track = new QList<tidal::TrackItemData>();
    }

    /**
     * @brief 소멸자.
     */
    TidalMyPlaylistDetail::~TidalMyPlaylistDetail(){

        this->deleteLater();
    }


    /**
     * @brief TidalPlaylistDetail::setJsonObject_forData
     * @param jsonObj
     */
    void TidalMyPlaylistDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        tidal::PlaylistItemData tmp_data_playlist = ConvertData::convertData_playlistData(jsonObj);

        // My Playlist 인 경우
        this->flagNeedReload = false;

        if(tmp_data_playlist.uuid != this->data_playlist.uuid){
            this->flagNeedReload = true;

            this->data_playlist = tmp_data_playlist;

            this->list_track->clear();
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

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_items_of_playlist, this, &TidalMyPlaylistDetail::slot_applyResult_items_of_playlist);
            proc->request_tidal_getList_items_of_playlist(this->data_playlist.uuid, GET_MAX_ITEM_SIZE___ONCE, 0);
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalMyPlaylistDetail::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            if(this->flag_track_ok == true){
                this->widget_main_contents->hide();

                GSCommon::clearLayout(this->box_main_contents);
                this->box_contents->removeWidget(this->widget_main_contents);

                this->flag_track_ok = false;
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
     * @brief TidalPlaylistDetail::setUIControl_playlist_info
     */
    void TidalMyPlaylistDetail::setUIControl_basic(){

        // Playlist 상세정보
        this->playlist_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Tidal_playlist);
        connect(this->playlist_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &TidalMyPlaylistDetail::slot_detailInfo_btnClicked);

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
            connect(proc_fav_playlist, &roseHome::ProcCommon::completeReq_rating_playlist, this, &TidalMyPlaylistDetail::slot_applyResult_getRating_playlist);
            proc_fav_playlist->request_rose_getRating_Playlist("TIDAL", this->data_playlist.uuid);
        }
    }



    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void TidalMyPlaylistDetail::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    /**
     * @brief My created playlist 삭제 처리에 대해서 ToastMsg 알려줌
     * @param data_itemPos
     */
    void TidalMyPlaylistDetail::proc_preUpdate_delete_myCreatePlaylist(const ItemPositionData& data_itemPos){

        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___playlists){
            // App과 똑같이 적용함. 화면을 뒤로 넘기거가 하지 않음.
            this->showToastMsg(tr("A track is removed from a Playlist."));
            //this->showToastMsg(tr("플레이리스트에서 제거되었습니다."));
//            emit this->signal_shouldGoBackward(QJsonObject());    // 이전 페이지로 이동함 (아직 테스트 안해봄)
        }
    }





    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void TidalMyPlaylistDetail::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > GET_MAX_ITEM_SIZE___ONCE ) ? GET_MAX_ITEM_SIZE___ONCE : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->playlist_tracks[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->playlist_tracks[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TidalMyPlaylistDetail::slot_clickedItemTrack_inList);
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

        this->track_favoriteOffset = start_index;
        roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
        connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalMyPlaylistDetail::slot_applyResult_getRating_track);
        proc_fav_track->request_rose_getRating_Track("TIDAL", tmp_clientkey);

        if(this->flag_track_ok == true && this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_draw = false;
    }


    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void TidalMyPlaylistDetail::request_more_trackData(){

        if((this->track_totalCount > this->list_track->count()) && (this->track_beforeOffset == this->track_currentOffset)){
            this->track_currentOffset += GET_MAX_ITEM_SIZE___ONCE;

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_items_of_playlist, this, &TidalMyPlaylistDetail::slot_applyResult_items_of_playlist);
            proc->request_tidal_getList_items_of_playlist(this->data_playlist.uuid, GET_MAX_ITEM_SIZE___ONCE, this->track_currentOffset);
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
    void TidalMyPlaylistDetail::slot_applyResult_items_of_playlist(const QList<tidal::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        this->flag_lastPage_track = flag_lastPage;
        this->flagReqMore_track = false;

        if(this->list_track->count() == 0){
            this->jsonArr_tracks_toPlay = QJsonArray();
        }

        int start_index = this->list_track->count();

        this->list_track->append(list_data);
        ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

        if(this->list_track->count() != 0){
            if(start_index == 0){
                int max_cnt = this->list_track->size();
                this->track_drawCount = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->playlist_tracks[i] = new PlaylistTrackDetailInfo_RHV;
                    connect(this->playlist_tracks[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TidalMyPlaylistDetail::slot_clickedItemTrack_inList);
                    this->playlist_tracks[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->playlist_tracks[i]);
                }

                QList<QString> tmp_clientkey;
                for(int i = start_index; i < max_cnt; i++){
                    this->playlist_tracks[i]->setDataTrackInfo_Tidal(this->list_track->at(i));
                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));

                    QCoreApplication::processEvents();
                }

                this->track_favoriteOffset = start_index;
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalMyPlaylistDetail::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("TIDAL", tmp_clientkey);
            }

            this->track_beforeOffset = this->track_currentOffset;

            if(this->track_totalCount == this->track_drawCount){
                this->box_main_contents->addSpacing(30);
            }

            this->request_more_trackData();

            ContentLoadingwaitingMsgHide();
            this->flag_track_ok = true;
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget, 0, Qt::AlignTop);

            this->flag_track_ok = true;
        }
    }


    /**
     * @brief TidalPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds
     * @param p_jsonObj
     */
    void TidalMyPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            // Tidal favorite check
            if(this->flag_send_playlist == true){
                QVariantList arr_myFavoriteIds = ProcJsonEasy::getJsonArray(p_jsonObj, "PLAYLIST").toVariantList();
                bool status = arr_myFavoriteIds.contains(data_playlist.uuid);

                if(status == true && this->flag_playlist_fav == false){
                    // Tidal playlistbFavorite del
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalMyPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_del_favorite("playlists", data_playlist.uuid);
                }
                else if(status == false && this->flag_playlist_fav == true){
                        // Tidal playlistb Favorite add
                        ProcCommon *proc = new ProcCommon(this);
                        connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalMyPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
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
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalMyPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                else if(status == false && this->flag_track_fav == true){
                    // Tidal track Favorite add
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalMyPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                this->flag_send_track = false;
            }
            else{

            }
        }
    }


    //c220818 share link
    void TidalMyPlaylistDetail::slot_applyResult_getShareLink(const QString &link){

        this->shareLink = link;
    }


    void TidalMyPlaylistDetail::slot_applyResult_checkRating_palylist(const QJsonObject &jsonObj){

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
            connect(proc_fav_playlist, &roseHome::ProcCommon::completeCheck_rating_playlist, this, &TidalMyPlaylistDetail::slot_applyResult_addRating_playlist);
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
            connect(proc_fav_playlist, &roseHome::ProcCommon::completeReq_rating_playlist, this, &TidalMyPlaylistDetail::slot_applyResult_getRating_playlist);
            proc_fav_playlist->request_rose_setRating_Playlist(tmp_playlist, this->flag_playlist_fav, this->playlist_star_fav);
        }
        print_debug();
    }


    void TidalMyPlaylistDetail::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

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
            connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalMyPlaylistDetail::slot_applyResult_addRating_track);
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
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalMyPlaylistDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

        }
    }


    void TidalMyPlaylistDetail::slot_applyResult_addRating_playlist(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_playlist == true){
                this->flag_check_playlist = false;
            }
        }
        else{
            this->flag_check_playlist = false;
        }
    }


    void TidalMyPlaylistDetail::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void TidalMyPlaylistDetail::slot_applyResult_getRating_playlist(const QJsonArray &contents){

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


    void TidalMyPlaylistDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

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
    void TidalMyPlaylistDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

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
                connect(proc_fav, &ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalMyPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                proc_fav->request_tidal_getAll_favorites();
                this->flag_send_playlist = true;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_playlist = new roseHome::ProcCommon(this);
                connect(proc_favCheck_playlist, &roseHome::ProcCommon::completeCheck_rating_playlist, this, &TidalMyPlaylistDetail::slot_applyResult_checkRating_palylist);
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
    void TidalMyPlaylistDetail::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

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
                connect(proc_fav, &ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalMyPlaylistDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                proc_fav->request_tidal_getAll_favorites();
                this->flag_send_track = true;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalMyPlaylistDetail::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("TIDAL", QString("%1").arg(this->track_id_fav));
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
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void TidalMyPlaylistDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->data_playlist, index, section, clickMode);
        }
    }





    /**
     * @brief
     * @param parent
     */
    TidalMyPlaylistDetail_ROSE::TidalMyPlaylistDetail_ROSE(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_filter, parent){

        // data
        this->list_track = new QList<roseHome::TrackItemData>();
    }


    /**
     * @brief 소멸자.
     */
    TidalMyPlaylistDetail_ROSE::~TidalMyPlaylistDetail_ROSE(){

        this->deleteLater();
    }


    /**
     * @brief QobuzMyPlaylistDetail_ROSE::setJsonObject_forData
     * @param jsonObj
     */
    void TidalMyPlaylistDetail_ROSE::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::PlaylistItemData tmp_data_playlist = roseHome::ConvertData::convertData_playlistData(jsonObj);
        this->flagNeedReload = false;

        if(tmp_data_playlist.id != this->data_playlist.id){
            this->flagNeedReload = true;

            this->ownerId_chk = tmp_data_playlist.ownerId;

            this->list_track->clear();
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

            roseHome::ProcCommon *proc_playlist = new roseHome::ProcCommon(this);
            connect(proc_playlist, &roseHome::ProcCommon::completeReq_playlist, this, &TidalMyPlaylistDetail_ROSE::slot_applyResult_playlist);
            connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_tracks, this, &TidalMyPlaylistDetail_ROSE::slot_applyResult_playlist_tracks);
            proc_playlist->request_rose_get_playlist(tmp_data_playlist.id, this->track_currentOffset, 20);

            if(this->ownerId_chk != global.user.getSub().toInt()){
                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_thumb_playlist = new roseHome::ProcCommon(this);
                connect(proc_thumb_playlist, &roseHome::ProcCommon::completeReq_rating_thumbup, this, &TidalMyPlaylistDetail_ROSE::slot_applyResult_getRating_thumbup);
                proc_thumb_playlist->request_rose_getRating_Thumbup("PLAY_LIST", QString("%1").arg(tmp_data_playlist.id));
            }
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalMyPlaylistDetail_ROSE::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            if(this->flag_track_ok == true){
                this->widget_main_contents->hide();

                GSCommon::clearLayout(this->box_main_contents);
                this->box_contents->removeWidget(this->widget_main_contents);

                this->flag_track_ok = false;
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

            // Playlist 상세정보
            if(this->ownerId_chk == global.user.getSub().toInt()){
                this->playlist_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::rose_playlist);
            }
            else{
                this->playlist_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::rose_userplaylist);
            }
            connect(this->playlist_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &TidalMyPlaylistDetail_ROSE::slot_detailInfo_btnClicked);

            this->playlist_detail_info->initView();

            this->box_main_contents->addWidget(this->playlist_detail_info);
            this->box_main_contents->addSpacing(50);
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void TidalMyPlaylistDetail_ROSE::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)){

            this->flag_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    void TidalMyPlaylistDetail_ROSE::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > 20 ) ? 20 : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->playlist_track_info[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->playlist_track_info[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TidalMyPlaylistDetail_ROSE::slot_clickedItemTrack_inList);
            this->playlist_track_info[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->playlist_track_info[i]);
        }

        for(int i = start_index; i < this->track_drawCount; i++){
            this->playlist_track_info[i]->setDataTrackInfo_Rose(this->list_track->at(i));

            QCoreApplication::processEvents();
        }

        if(this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_draw = false;
    }


    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void TidalMyPlaylistDetail_ROSE::request_more_trackData(){

        if((this->track_totalCount > this->list_track->count()) && (this->track_beforeOffset == this->track_currentOffset)){
            this->track_currentOffset++;

            roseHome::ProcCommon *proc_playlist = new roseHome::ProcCommon(this);
            connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_tracks, this, &TidalMyPlaylistDetail_ROSE::slot_applyResult_playlist_tracks);
            proc_playlist->request_rose_get_playlist(this->data_playlist.id, this->track_currentOffset, 20);
        }
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : [Slot]
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void TidalMyPlaylistDetail_ROSE::slot_applyResult_playlist(const roseHome::PlaylistItemData &playlist_data){

        this->track_totalCount = playlist_data.totalCount;
        this->data_playlist = playlist_data;

        QJsonObject playlistObj;
        playlistObj.insert("image", this->data_playlist.thumbnail);
        playlistObj.insert("title", this->data_playlist.title);
        if(this->ownerId_chk == global.user.getSub().toInt()){
            playlistObj.insert("owner", this->data_playlist.share);
        }
        else{
            playlistObj.insert("owner", this->data_playlist.ownerName);
        }
        playlistObj.insert("track_count", this->data_playlist.trackCount);
        playlistObj.insert("registDateTime", this->data_playlist.registDateTime);
        playlistObj.insert("type", this->data_playlist.type);
        playlistObj.insert("id", this->data_playlist.id);
        playlistObj.insert("comment", this->data_playlist.comment);

        this->playlist_detail_info->setData_fromRoseData(playlistObj);

        if(this->ownerId_chk == global.user.getSub().toInt()){
            this->playlist_detail_info->setFavorite(this->data_playlist.favorite, this->data_playlist.star);
        }


    }

    //c220818 share link
    void TidalMyPlaylistDetail_ROSE::slot_applyResult_getShareLink(const QString &link){

        this->shareLink = link;
        print_debug();
        qDebug() << "this->shareLink=" << this->shareLink;
    }


    void TidalMyPlaylistDetail_ROSE::slot_applyResult_playlist_tracks(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
                if(this->track_totalCount > 50){
                    this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
                }
            }

            int start_index = this->list_track->count();

            this->list_track->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            if(this->track_totalCount == this->list_track->size()){
                int makeDuration = 0;
                for(int i = 0; i < list_data.size(); i++){
                    makeDuration += list_data.at(i).duration;
                }
                this->data_playlist.duration = makeDuration;

                // Update UI
                QJsonObject playlistObj;
                playlistObj.insert("track_count", this->track_totalCount);
                playlistObj.insert("duration", this->data_playlist.duration);
                playlistObj.insert("registDateTime", this->data_playlist.registDateTime);

                this->playlist_detail_info->setData_Resolution(playlistObj);
            }

            if(start_index == 0){
                int max_cnt = this->list_track->size();
                this->track_drawCount = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->playlist_track_info[i] = new PlaylistTrackDetailInfo_RHV;
                    connect(this->playlist_track_info[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TidalMyPlaylistDetail_ROSE::slot_clickedItemTrack_inList);
                    this->playlist_track_info[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->playlist_track_info[i]);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->playlist_track_info[i]->setDataTrackInfo_Rose(this->list_track->at(i));
                    this->playlist_track_info[i]->setFavoritesIds(this->list_track->at(i).favorite, this->list_track->at(i).star);

                    QCoreApplication::processEvents();
                }
            }

            this->track_beforeOffset = this->track_currentOffset;

            ContentLoadingwaitingMsgHide();
            this->flag_track_ok = true;

            if(this->flag_track_ok == true && (this->track_totalCount == this->track_drawCount)){
                this->box_main_contents->addSpacing(30);
            }
            else{
                this->request_more_trackData();
            }
        }
        else{
            this->flag_track_ok = true;
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void TidalMyPlaylistDetail_ROSE::slot_applyResult_getRating_playlist(const QJsonArray &contents){

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

    void TidalMyPlaylistDetail_ROSE::slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고.
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            // Tidal favorite check
            if(this->flag_send_track == true){
                QVariantList arr_myFavoriteIds = ProcJsonEasy::getJsonArray(p_jsonObj, "TRACK").toVariantList();
                bool status = arr_myFavoriteIds.contains(this->track_id_fav);

                if(status == true && this->flag_track_fav == false){
                    // Tidal track Favorite del
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalMyPlaylistDetail_ROSE::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                else if(status == false && this->flag_track_fav == true){
                    // Tidal track Favorite add
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalMyPlaylistDetail_ROSE::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                this->flag_send_track = false;
            }
            else{

            }
        }
    }


    void TidalMyPlaylistDetail_ROSE::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->playlist_track_info[this->track_favoriteOffset + i]->setFavoritesIds(flag, star);
                }
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void TidalMyPlaylistDetail_ROSE::slot_applyResult_getRating_thumbup(const QJsonObject &jsonObj){

        if(jsonObj.size() > 0){
            if(jsonObj.contains("result")){
                bool fav_flag = false;

                if(jsonObj["result"].toBool() == true){
                    fav_flag = true;
                }

                this->playlist_detail_info->setFavorite(fav_flag, this->data_playlist.star);
            }

            if(this->flag_check_thumb == true){
                this->flag_check_thumb = false;
            }
        }
    }


    /**
     * @brief [Slot] AlbumImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void TidalMyPlaylistDetail_ROSE::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

        if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx2 ||
                clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx3 || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){

            if(this->flag_check_playlist == false){
                int star_cnt = this->playlist_detail_info->getFavoritesStars();
                this->flag_playlist_fav = false;

                if(star_cnt == 3){
                    star_cnt = 0;
                    this->flag_playlist_fav = false;
                }
                else if(star_cnt >= 0 && star_cnt < 3){
                    star_cnt++;
                    this->flag_playlist_fav = true;
                }

                QJsonArray tags;
                for(int i = 0; i < this->data_playlist.tags.size(); i++){
                    tags.append(this->data_playlist.tags.at(i));
                }

                QJsonArray tracks;
                for(int i = 0; i < this->jsonArr_tracks_toPlay.size(); i++){
                    tracks.append(this->jsonArr_tracks_toPlay.at(i).toObject());
                }

                QJsonObject playlist;
                playlist.insert("clientKey", "");
                playlist.insert("comment", "");
                playlist.insert("favCnt", 0);
                playlist.insert("favorite", true);
                playlist.insert("favoriteTrack", false);
                playlist.insert("id", this->data_playlist.id);
                playlist.insert("isRose", true);
                playlist.insert("ownerId", this->data_playlist.ownerId);
                playlist.insert("ownerName", this->data_playlist.ownerName);
                playlist.insert("registDateTime", this->data_playlist.registDateTime);
                playlist.insert("share", this->data_playlist.share);
                playlist.insert("sort", this->data_playlist.sort);
                playlist.insert("star", star_cnt);
                playlist.insert("tags", tags);
                playlist.insert("thumbnail", this->data_playlist.thumbnail);
                playlist.insert("thumbupCount", this->data_playlist.thumbupCount);
                playlist.insert("title", this->data_playlist.title);
                playlist.insert("trackCount", this->data_playlist.trackCount);
                playlist.insert("tracks", tracks);
                playlist.insert("type", this->data_playlist.type);

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_playlist_fav);
                ratingInfo.insert("star", star_cnt);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", this->data_playlist.type);

                QJsonObject json;
                json.insert("playlist", playlist);
                json.insert("ratingInfo", ratingInfo);


                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_playlist = new roseHome::ProcCommon(this);
                connect(proc_fav_playlist, &roseHome::ProcCommon::completeReq_rating_playlist, this, &TidalMyPlaylistDetail_ROSE::slot_applyResult_getRating_playlist);
                proc_fav_playlist->request_rose_setRating_rosePlaylist(json, this->flag_playlist_fav, star_cnt);
                this->flag_check_playlist = true;

                this->playlist_detail_info->setFavorite(this->flag_playlist_fav, star_cnt);
            }            
        }
        else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Thumbup_toAdd || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Thumbup_toDelete){

            if(this->flag_check_thumb == false){
                int star_cnt = this->playlist_detail_info->getFavoritesStars();
                this->flag_playlist_thumb = false;

                if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Thumbup_toAdd){
                    this->flag_playlist_thumb = true;
                }
                else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Thumbup_toDelete){
                    this->flag_playlist_thumb = false;
                }

                QJsonObject json;
                json.insert("targetId", QString("%1").arg(this->data_playlist.id));
                json.insert("type", "PLAY_LIST");
                json.insert("thumbup", this->flag_playlist_thumb);

                roseHome::ProcCommon *proc_thumb_playlist = new roseHome::ProcCommon(this);
                connect(proc_thumb_playlist, &roseHome::ProcCommon::completeReq_rating_thumbup, this, &TidalMyPlaylistDetail_ROSE::slot_applyResult_getRating_thumbup);
                proc_thumb_playlist->request_rose_setRating_Thumbup(json, this->flag_playlist_thumb);
                this->flag_check_thumb = true;

                this->playlist_detail_info->setFavorite(this->flag_playlist_thumb, star_cnt);
            }
        }
        else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Share){

            this->shareLink = this->playlist_detail_info->detailInfo_shareLink;     //220905 share code ... --


            //220816 share code ...
            setUIShare();//c220818
        }
        else{

            if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){
                // OptMorePopup 띄우기
                print_debug();
                        this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data_playlist), 0, SECTION_FOR_MORE_POPUP___playlists, true);
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayAll){
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylist(data_playlist, this->jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, roseHome::ProcRosePlay_withRosehome::PlayShuffleMode::JustPlay);
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayShuffle){
                // 셔플재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylist(data_playlist, this->jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, roseHome::ProcRosePlay_withRosehome::PlayShuffleMode::ShufflePlay);
            }
        }
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
   void TidalMyPlaylistDetail_ROSE::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___tracks;

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->playlist_track_info[idx]->getFavoritesStars();
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
                this->track_id_fav = this->list_track->at(idx).clientKey.toInt();

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
                connect(proc_fav, &ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalMyPlaylistDetail_ROSE::slot_tidal_completeReq_listAll_myFavoritesIds);
                proc_fav->request_tidal_getAll_favorites();
                this->flag_send_track = true;

                QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QString track_type = ProcJsonEasy::getString(track, "type");

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_track_fav);
                ratingInfo.insert("star", this->track_star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", track_type);

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalMyPlaylistDetail_ROSE::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
                this->flag_check_track = true;

                this->playlist_track_info[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }
        }
        else{

            // ClickMode 별로 처리
            if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AllBox){
                // Track 바로 재생 - clicked 항목만 재생
                OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::Play_RightNow_withReorder;

                if(global.device.getMusicPlayType() == 15){
                    playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                }
                else if(global.device.getMusicPlayType() == 13){
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                }
                else if(global.device.getMusicPlayType() == 12){
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                }
                else if(global.device.getMusicPlayType() == 16){
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                }
                else if(global.device.getMusicPlayType() == 17){
                    playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                }
                else if(global.device.getMusicPlayType() == 18){
                    playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                }
                else if(global.device.getMusicPlayType() == 19){
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                }

                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(this->list_track->at(idx), this->jsonArr_tracks_toPlay, idx, playType);

                /*QJsonArray send_track_json = QJsonArray();
                send_track_json.append(this->jsonArr_tracks_toPlay.at(idx).toObject());

                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(this->list_track->at(idx), send_track_json, idx, OptMorePopup::ClickMode::Play_RightNow_withReorder);*/
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                // 플레이리스트 담기 - Track
                //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){
                // OptMorePopup 띄우기 필요
                print_debug();


                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Track, roseHome::ConvertData::getConvertOptHeaderData(this->list_track->at(idx)), idx, section);
            }
        }
    }


   /**
    * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
    * @param clickMode
    * @param index
    * @param section
    */
   void TidalMyPlaylistDetail_ROSE::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

       if(section == SECTION_FOR_MORE_POPUP___playlists){
           //this->proc_clicked_optMorePopup_fromPlaylist(this->data_playlist, index, section, clickMode);
           if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            else if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                   || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                   || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                   || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
           )
           {
               // Rose Play 요청
               roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
               procRosePlay->requestPlayRose_byPlaylist(this->data_playlist, this->jsonArr_tracks_toPlay, 0, clickMode, roseHome::ProcRosePlay_withRosehome::PlayShuffleMode::JustPlay);
           }
           else if(clickMode == OptMorePopup::ClickMode::Edit){
               QString view_type = "edit";

               QJsonObject data;
               data.insert("view_type", view_type);
               data.insert("playlist_id", this->data_playlist.id);
               data.insert("type", "ROSE");

               QJsonObject jsonObj_move;
               jsonObj_move.insert("data", data);

               jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_ADD_PLAYLIST);

               emit this->signal_clickedMovePage(jsonObj_move);
           }
       }
       else if(section == SECTION_FOR_MORE_POPUP___tracks){
           //this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
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
               //roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
               //procRosePlay->requestPlayRose_byTracks(this->list_track->at(index), this->jsonArr_tracks_toPlay, index, clickMode);

               if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                       || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                       || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last)
               {
                   QJsonArray tmpJsonArr = QJsonArray();
                   for(int i = index; i < this->jsonArr_tracks_toPlay.size(); i++){
                       QJsonObject tmpJsonObj = this->jsonArr_tracks_toPlay.at(i).toObject();
                       tmpJsonArr.append(tmpJsonObj);
                   }

                   // Rose Play 요청
                   roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                   procRosePlay->requestPlayRose_byTracks(this->list_track->at(index), tmpJsonArr, 0, clickMode);
               }
               else{
                   QJsonObject tmpJsonObj = this->jsonArr_tracks_toPlay.at(index).toObject();
                   QJsonArray tmpJsonArr = QJsonArray();
                   tmpJsonArr.append(tmpJsonObj);

                   // Rose Play 요청
                   roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                   procRosePlay->requestPlayRose_byTracks(this->list_track->at(index), tmpJsonArr, 0, clickMode);
               }
           }
       }
   }

}
