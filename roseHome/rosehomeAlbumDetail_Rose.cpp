#include "roseHome/rosehomeAlbumDetail_Rose.h"

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


namespace roseHome {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___albums = 1;
    const int SECTION_FOR_MORE_POPUP___tracks = 2;

    const int GET_MAX_ITEM_SIZE___ONCE = 20;

    /**
     * @brief
     * @param parent
     */
    RoseHomeAlbumDetail_Rose::RoseHomeAlbumDetail_Rose(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_filter, parent) {

        // data
        this->list_track = new QList<roseHome::TrackItemData>();
    }


    /**
     * @brief 소멸자.
     */
    RoseHomeAlbumDetail_Rose::~RoseHomeAlbumDetail_Rose(){

        this->deleteLater();
    }


    /**
     * @brief RoseHomeRoseHomeAlbumDetail_Rose::setJsonObject_forData
     * @param jsonObj
     */
    void RoseHomeAlbumDetail_Rose::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::AlbumItemData tmp_data_album = roseHome::ConvertData::convertData_albumData(jsonObj);
        this->flagNeedReload = false;

        if(tmp_data_album.id != this->data_album.id){
            this->flagNeedReload = true;

            this->data_album = tmp_data_album;

            this->list_track->clear();
            this->jsonArr_tracks_toPlay = QJsonArray();

            this->track_drawCount = 0;
            this->track_totalCount = 0;
            this->track_beforeOffset = 0;
            this->track_currentOffset = 0;
            this->track_favoriteOffset = 0;

            this->track_id_fav = 0;
            this->track_idx_fav = 0;

            this->album_star_fav = 0;
            this->track_star_fav = 0;

            this->flag_album_fav = false;
            this->flag_track_fav = false;
            this->flag_send_album = false;
            this->flag_check_album = false;
            this->flag_send_track = false;
            this->flag_check_track = false;

            this->flagReqMore_track = false;
            this->flag_lastPage_track = false;

            this->flag_draw = false;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            roseHome::ProcCommon *proc_album = new roseHome::ProcCommon(this);
            connect(proc_album, &roseHome::ProcCommon::completeReq_albumlist, this, &RoseHomeAlbumDetail_Rose::slot_applyResult_album);
            connect(proc_album, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseHomeAlbumDetail_Rose::slot_applyResult_album_tracks);
            proc_album->request_rose_get_album(tmp_data_album.id, this->track_currentOffset, GET_MAX_ITEM_SIZE___ONCE);
        }
    }


    /**
     * @brief RoseHomePlaylistDetail_Rose 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void RoseHomeAlbumDetail_Rose::setActivePage(){

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
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // Album 상세정보
            this->album_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::rose_album);
            connect(this->album_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &RoseHomeAlbumDetail_Rose::slot_detailInfo_btnClicked);

            this->album_detail_info->initView();

            this->box_main_contents->addWidget(this->album_detail_info);
            this->box_main_contents->addSpacing(50);
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void RoseHomeAlbumDetail_Rose::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)){

            this->flag_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    void RoseHomeAlbumDetail_Rose::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > GET_MAX_ITEM_SIZE___ONCE ) ? GET_MAX_ITEM_SIZE___ONCE : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->album_track_info[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->album_track_info[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &RoseHomeAlbumDetail_Rose::slot_clickedItemTrack_inList);
            this->album_track_info[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->album_track_info[i]);
        }

        for(int i = start_index; i < this->track_drawCount; i++){
            this->album_track_info[i]->setDataTrackInfo_Rose(this->list_track->at(i));

            QCoreApplication::processEvents();
        }

        if(this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_draw = false;
    }


    void RoseHomeAlbumDetail_Rose::request_more_trackData(){

        if((this->track_totalCount > this->list_track->count()) && (this->track_beforeOffset == this->track_currentOffset)){
            this->track_currentOffset++;

            roseHome::ProcCommon *proc_tracks = new roseHome::ProcCommon(this);
            connect(proc_tracks, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseHomeAlbumDetail_Rose::slot_applyResult_album_tracks);
            proc_tracks->request_rose_get_album(this->data_album.id, this->track_currentOffset, GET_MAX_ITEM_SIZE___ONCE);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : [Slot]
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RoseHomeAlbumDetail_Rose::slot_applyResult_album(const roseHome::AlbumItemData &dataAlbum){

        roseHome::AlbumItemData tmpAlbumData = this->data_album;

        this->track_totalCount = dataAlbum.trackCount;
        this->data_album = dataAlbum;

        if(this->data_album.thumbnail.isEmpty()){
            this->data_album.thumbnail = tmpAlbumData.thumbnail;
        }

        QJsonObject albumObj;
        albumObj.insert("image", this->data_album.thumbnail);
        albumObj.insert("title", this->data_album.title);
        albumObj.insert("owner", "");
        albumObj.insert("track_count", this->data_album.totalCount);
        albumObj.insert("type", this->data_album.type);
        albumObj.insert("id", this->data_album.id);

        this->album_detail_info->setData_fromRoseData(albumObj);

        this->album_detail_info->setFavorite(this->data_album.favorite, this->data_album.star);
    }


    /**
     * @brief playlist 내의 트랙 아이템들 반환 데이터를 처리함
     * @param list_track
     * @param jsonArr_dataToPlay
     */
    void RoseHomeAlbumDetail_Rose::slot_applyResult_album_tracks(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                this->data_album.duration = makeDuration;

                // Update UI
                QJsonObject albumObj;
                albumObj.insert("track_count", this->track_totalCount);
                albumObj.insert("duration", this->data_album.duration);
                albumObj.insert("owner", this->list_track->at(0).list_artist_name.join(","));

                this->album_detail_info->setData_Resolution(albumObj);
            }

            if(start_index == 0){
                int max_cnt = this->list_track->size();
                this->track_drawCount = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->album_track_info[i] = new PlaylistTrackDetailInfo_RHV;
                    connect(this->album_track_info[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &RoseHomeAlbumDetail_Rose::slot_clickedItemTrack_inList);
                    this->album_track_info[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->album_track_info[i]);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->album_track_info[i]->setDataTrackInfo_Rose(this->list_track->at(i));
                    this->album_track_info[i]->setFavoritesIds(this->list_track->at(i).favorite, this->list_track->at(i).star);

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
            //if(abs_ani_dialog_wait->isHidden() != true){
                ContentLoadingwaitingMsgHide();
            //}

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void RoseHomeAlbumDetail_Rose::slot_applyResult_getRating_album(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject tmp_star = contents.at(0).toObject();

            if(tmp_star.contains("star")){
                int star = ProcJsonEasy::getInt(tmp_star, "star");
                bool flag = star > 0 ? true : false;

                this->album_detail_info->setFavorite(flag, star);
            }

            if(this->flag_check_album == true){
                this->flag_check_album = false;
            }
        }
    }


    void RoseHomeAlbumDetail_Rose::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->album_track_info[this->track_favoriteOffset + i]->setFavoritesIds(flag, star);
                }
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void RoseHomeAlbumDetail_Rose::slot_applyResult_getRating_thumbup(const QJsonObject &jsonObj){

        if(jsonObj.size() > 0){
            if(jsonObj.contains("result")){
                bool fav_flag = false;

                if(jsonObj["result"].toBool() == true){
                    fav_flag = true;
                }

                this->album_detail_info->setFavorite(fav_flag, this->data_album.star);
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
    void RoseHomeAlbumDetail_Rose::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

        if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx2 ||
                clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx3 || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){

            if(this->flag_check_album == false){
                int star_cnt = this->album_detail_info->getFavoritesStars();
                this->flag_album_fav = false;

                if(star_cnt == 3){
                    star_cnt = 0;
                    this->flag_album_fav = false;
                }
                else if(star_cnt >= 0 && star_cnt < 3){
                    star_cnt++;
                    this->flag_album_fav = true;
                }

                QJsonObject data;
                data.insert("macAddress", this->data_album.macAddress);
                data.insert("trackId", this->data_album.trackId);

                QJsonArray tracks;
                for(int i = 0; i < this->jsonArr_tracks_toPlay.size(); i++){
                    tracks.append(this->jsonArr_tracks_toPlay.at(i).toObject());
                }

                QJsonObject album;
                album.insert("clientKey", "");
                album.insert("data", data);
                album.insert("favCnt", 0);
                album.insert("favorite", true);
                album.insert("id", this->data_album.id);
                album.insert("star", star_cnt);
                album.insert("thumbnail", this->data_album.thumbnail);
                album.insert("title", this->data_album.title);
                album.insert("trackCount", this->data_album.totalCount);
                album.insert("tracks", tracks);
                album.insert("type", this->data_album.type);

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_album_fav);
                ratingInfo.insert("star", star_cnt);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", this->data_album.type);

                QJsonObject json;
                json.insert("album", album);
                json.insert("ratingInfo", ratingInfo);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
                connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_album, this, &RoseHomeAlbumDetail_Rose::slot_applyResult_getRating_album);
                proc_fav_album->request_rose_setRating_Album(json, this->flag_album_fav, star_cnt);
                this->flag_check_album = true;

                this->album_detail_info->setFavorite(this->flag_album_fav, star_cnt);
            }
        }
        else{
            if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){
                print_debug();
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, roseHome::ConvertData::getConvertOptHeaderData(data_album), 0, SECTION_FOR_MORE_POPUP___albums, true);
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayAll){
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byAlbum(data_album, this->jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, roseHome::ProcRosePlay_withRosehome::PlayShuffleMode::JustPlay);
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayShuffle){
                // 셔플재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byAlbum(data_album, this->jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, roseHome::ProcRosePlay_withRosehome::PlayShuffleMode::ShufflePlay);
            }
        }
    }


    /**
     * @brief [Slot] AlbumImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void RoseHomeAlbumDetail_Rose::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___tracks;

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                int star_cnt = this->album_track_info[idx]->getFavoritesStars();
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
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseHomeAlbumDetail_Rose::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, star_cnt);
                this->flag_check_track = true;

                this->album_track_info[idx]->setFavoritesIds(this->flag_track_fav, star_cnt);
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
    void RoseHomeAlbumDetail_Rose::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___albums){
            //this->proc_clicked_optMorePopup_fromPlaylist(this->data_playlist, index, section, clickMode);

            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                // Rose Play 요청
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byAlbum(this->data_album, this->jsonArr_tracks_toPlay, 0, clickMode, roseHome::ProcRosePlay_withRosehome::PlayShuffleMode::JustPlay);
            }
            else if(clickMode == OptMorePopup::ClickMode::Share){//c220903_2
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);

            /*if(clickMode == OptMorePopup::ClickMode::Play_RightNow
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
            }*/
        }
    }

}
