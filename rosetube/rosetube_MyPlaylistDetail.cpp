#include "rosetube/rosetube_MyPlaylistDetail.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"

#include <QDebug>
#include <QPainter>
#include <QScrollArea>
#include <QPixmapCache>


namespace rosetube {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___playlists = 1;
    const int SECTION_FOR_MORE_POPUP___tracks = 2;

    const int GET_MAX_ITEM_SIZE___ONCE = 20;

    /**
     * @brief
     * @param parent
     */
    RosetubeMyPlaylistDetail::RosetubeMyPlaylistDetail(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_filter, parent) {

        // data
        this->list_track = new QList<roseHome::TrackItemData>();
    }

    /**
     * @brief 소멸자.
     */
    RosetubeMyPlaylistDetail::~RosetubeMyPlaylistDetail(){

        this->deleteLater();
    }


    /**
     * @brief RosetubeMyPlaylistDetail::setJsonObject_forData
     * @param jsonObj
     */
    void RosetubeMyPlaylistDetail::setJsonObject_forData(const QJsonObject &jsonObj){

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
            connect(proc_playlist, &roseHome::ProcCommon::completeReq_playlist, this, &RosetubeMyPlaylistDetail::slot_applyResult_playlist);
            connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_tracks, this, &RosetubeMyPlaylistDetail::slot_applyResult_playlist_tracks);
            proc_playlist->request_rose_get_playlist(tmp_data_playlist.id, this->track_currentOffset, GET_MAX_ITEM_SIZE___ONCE);

            if(this->ownerId_chk != global.user.getSub().toInt()){
                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_thumb_playlist = new roseHome::ProcCommon(this);
                connect(proc_thumb_playlist, &roseHome::ProcCommon::completeReq_rating_thumbup, this, &RosetubeMyPlaylistDetail::slot_applyResult_getRating_thumbup);
                proc_thumb_playlist->request_rose_getRating_Thumbup("PLAY_LIST", QString("%1").arg(tmp_data_playlist.id));
            }
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief RosetubeMyPlaylistDetail 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void RosetubeMyPlaylistDetail::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            if(this->flag_track_ok == true){
                this->widget_main_contents->hide();

                GSCommon::clearLayout(this->box_main_contents);
                this->box_contents->removeWidget(this->widget_main_contents);

                this->flag_track_ok = false;
            }


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
            connect(this->playlist_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &RosetubeMyPlaylistDetail::slot_detailInfo_btnClicked);

            this->playlist_detail_info->initView();

            this->box_main_contents->addWidget(this->playlist_detail_info);
            this->box_main_contents->addSpacing(50);

            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void RosetubeMyPlaylistDetail::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)){

            this->flag_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    void RosetubeMyPlaylistDetail::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > GET_MAX_ITEM_SIZE___ONCE ) ? GET_MAX_ITEM_SIZE___ONCE : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->playlist_track_info[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->playlist_track_info[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &RosetubeMyPlaylistDetail::slot_clickedItemTrack_inList);
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


    void RosetubeMyPlaylistDetail::request_more_trackData(){

        if((this->track_totalCount > this->list_track->count()) && (this->track_beforeOffset == this->track_currentOffset)){
            this->track_currentOffset++;

            roseHome::ProcCommon *proc_playlist = new roseHome::ProcCommon(this);
            connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_tracks, this, &RosetubeMyPlaylistDetail::slot_applyResult_playlist_tracks);
            proc_playlist->request_rose_get_playlist(this->data_playlist.id, this->track_currentOffset, GET_MAX_ITEM_SIZE___ONCE);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : [Slot]
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RosetubeMyPlaylistDetail::slot_applyResult_playlist(const roseHome::PlaylistItemData &playlist_data){

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

        print_debug();


    }

    //c220818 share link
    void RosetubeMyPlaylistDetail::slot_applyResult_getShareLink(const QString &link){

        this->shareLink = link;
    }

    /**
     * @brief playlist 내의 트랙 아이템들 반환 데이터를 처리함
     * @param list_track
     * @param jsonArr_dataToPlay
     */
    void RosetubeMyPlaylistDetail::slot_applyResult_playlist_tracks(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
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
                    connect(this->playlist_track_info[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &RosetubeMyPlaylistDetail::slot_clickedItemTrack_inList);
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
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
            noData_widget->setFixedSize(1500, 500);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void RosetubeMyPlaylistDetail::slot_applyResult_getRating_playlist(const QJsonArray &contents){

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


    void RosetubeMyPlaylistDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

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


    void RosetubeMyPlaylistDetail::slot_applyResult_getRating_thumbup(const QJsonObject &jsonObj){

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
     * @brief [Slot] PlaylistImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void RosetubeMyPlaylistDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

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
                connect(proc_fav_playlist, &roseHome::ProcCommon::completeReq_rating_playlist, this, &RosetubeMyPlaylistDetail::slot_applyResult_getRating_playlist);
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
                connect(proc_thumb_playlist, &roseHome::ProcCommon::completeReq_rating_thumbup, this, &RosetubeMyPlaylistDetail::slot_applyResult_getRating_thumbup);
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
                print_debug();
                // OptMorePopup 띄우기
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
     * @brief [Slot] AlbumImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void RosetubeMyPlaylistDetail::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___tracks;

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                int star_cnt = this->playlist_track_info[idx]->getFavoritesStars();
                this->flag_track_fav = false;

                if(star_cnt == 3){
                    star_cnt = 0;
                    this->flag_track_fav = false;

                }
                else if(star_cnt >= 0 && star_cnt < 3){
                    star_cnt++;
                    this->flag_track_fav = true;

                }

                QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QString track_type = ProcJsonEasy::getString(track, "type");

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_track_fav);
                ratingInfo.insert("star", star_cnt);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", track_type);

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RosetubeMyPlaylistDetail::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, star_cnt);
                this->flag_check_track = true;

                this->playlist_track_info[idx]->setFavoritesIds(this->flag_track_fav, star_cnt);
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
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                // 플레이리스트 담기 - Track
                //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){
                print_debug();
                // OptMorePopup 띄우기 필요
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
    void RosetubeMyPlaylistDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

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

                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RT_ADDPLAYLIST);

                emit linker->signal_clickedMovePage(jsonObj_move);
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
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(this->list_track->at(index), this->jsonArr_tracks_toPlay, index, clickMode);
            }
        }
    }
}
