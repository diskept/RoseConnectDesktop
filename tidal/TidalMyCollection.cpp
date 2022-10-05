#include "tidal/TidalMyCollection.h"

#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"

#include "common/global.h"
#include "common/gscommon.h"

#include <QLabel>
#include <QVBoxLayout>


namespace tidal {


    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int BTN_IDX_SUBTITLE_playlists_tidal = 1;
    const int BTN_IDX_SUBTITLE_playlists = 2;
    const int BTN_IDX_SUBTITLE_tracks = 3;
    const int BTN_IDX_SUBTITLE_albums = 4;
    const int BTN_IDX_SUBTITLE_artists = 5;
    const int BTN_IDX_SUBTITLE_videos = 6;


    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___playlists_tidal = 1;
    const int SECTION_FOR_MORE_POPUP___playlists_created = 2;
    const int SECTION_FOR_MORE_POPUP___tracks = 3;
    const int SECTION_FOR_MORE_POPUP___albums = 4;
    const int SECTION_FOR_MORE_POPUP___artists = 5;
    const int SECTION_FOR_MORE_POPUP___videos = 6;

    const int GET_ITEM_SIZE___ONCE_FOR_MAIN = 10;

    const QString TIDAL_API_PATH = "users";          // 뒤에 userId 추가로 붙여야 함
    const QString TIDAL_MAIN_TITLE = "My Playlist";


    /**
     * @brief "TIDAL > My Collection" 관련 화면
     * @param parent
     */
    TidalMycollection::TidalMycollection(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_filter, parent) {

        // Data
        this->list_playlist_created = new QList<tidal::PlaylistItemData>();
        this->list_playlist_tidal = new QList<tidal::PlaylistItemData>();

        this->list_album = new QList<tidal::AlbumItemData>();
        this->list_track = new QList<tidal::TrackItemData>();
        this->list_artist = new QList<tidal::ArtistItemData>();
        this->list_video = new QList<tidal::VideoItemData>();

        // 타이달 로그인 여부에 따라서
        if(this->is_tidal_logined()){
            this->setUIControl_logined();            
        }
        else{
            this->setUIControl_notLogin();
        }
    }


    /**
     * @brief 소멸자.
     */
    TidalMycollection::~TidalMycollection(){

        this->deleteLater();
    }


    /**
     * @brief 화면 활성화
     */
    void TidalMycollection::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractTidalSubWidget::setActivePage();

        // 항상 새로 가져오도록 한다.
        GSCommon::clearLayout(this->box_mainTitle);
        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // 타이달 로그인 여부에 따라서
        if(this->is_tidal_logined()){
            // 로그인 되어 있음
            this->setUIControl_logined();

            // init - data
            this->list_playlist_created->clear();
            this->list_playlist_tidal->clear();

            this->list_album->clear();
            this->list_track->clear();
            this->list_artist->clear();
            this->list_video->clear();

            // init - UI
            GSCommon::clearLayout(this->hBox_playlist);
            GSCommon::clearLayout(this->hBox_playlist_tidal);
            GSCommon::clearLayout(this->hBox_album);
            GSCommon::clearLayout(this->hBox_artist);
            GSCommon::clearLayout(this->hBox_video);
            this->listWidget_track->clear();

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);

            // playlist 는 created 다음으로 favorite 가 보여야 하므로, slot(created)에서 요청하도록 한다.
            connect(proc, &ProcCommon::completeReq_list_myFavoritePlaylists, this, &TidalMycollection::slot_applyResult_myFavoritePlaylists);
            proc->request_tidal_getList_myFavoritePlaylists(ProcCommon::DataOrderOption::DATE, ProcCommon::DataOrderDirection::DESC, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            connect(proc, &ProcCommon::completeReq_list_myCreatedPlaylists, this, &TidalMycollection::slot_applyResult_myCreatedPlaylists);
            proc->request_tidal_getList_myCreatedPlaylists(ProcCommon::DataOrderOption::DATE, ProcCommon::DataOrderDirection::DESC, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            connect(proc, &ProcCommon::completeReq_list_myFavoriteAlbums, this, &TidalMycollection::slot_applyResult_myFavoriteAlbums);
            proc->request_tidal_getList_myFavoriteAlbums(ProcCommon::DataOrderOption::DATE, ProcCommon::DataOrderDirection::DESC, 10, 0);

            connect(proc, &ProcCommon::completeReq_list_myFavoriteTracks, this, &TidalMycollection::slot_applyResult_myFavoriteTracks);
            proc->request_tidal_getList_myFavoriteTracks(ProcCommon::DataOrderOption::DATE, ProcCommon::DataOrderDirection::DESC, 5, 0);

            connect(proc, &ProcCommon::completeReq_list_myFavoriteArtists, this, &TidalMycollection::slot_applyResult_myFavoriteArtists);
            proc->request_tidal_getList_myFavoriteArtists(ProcCommon::DataOrderOption::DATE, ProcCommon::DataOrderDirection::DESC, 10, 0);

            connect(proc, &ProcCommon::completeReq_list_myFavoriteVideos, this, &TidalMycollection::slot_applyResult_myFavoriteVideos);
            proc->request_tidal_getList_myFavoriteVideos(ProcCommon::DataOrderOption::DATE, ProcCommon::DataOrderDirection::DESC, 10, 0);
        }
        else{
            // 로그인 안되어 있음
            this->setUIControl_notLogin();
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief  사용자가 타이달 로그인했을 경우의 화면을 위한 Widget
     */
    void TidalMycollection::setUIControl_logined(){

        // 기본 UI 세팅
        this->addUIControl_subTitle_withSideBtn_common(tr("Playlist By TIDAL"), tr("View All"), BTN_IDX_SUBTITLE_playlists_tidal);
        this->hBox_playlist_tidal = this->get_addUIControl_hBoxLayout_forPlaylist();

        this->addUIControl_subTitle_withSideBtn_common(tr("My Playlists"), tr("View All"), BTN_IDX_SUBTITLE_playlists);
        this->hBox_playlist = this->get_addUIControl_hBoxLayout_forPlaylist();

        this->addUIControl_subTitle_withSideBtn_common(tr("My Albums"), tr("View All"), BTN_IDX_SUBTITLE_albums);
        this->hBox_album = this->get_addUIControl_hBoxLayout_forAlbum();

        this->addUIControl_subTitle_withSideBtn_common(tr("My Tracks"), tr("View All"), BTN_IDX_SUBTITLE_tracks);
        this->delegate = new TidalTrackDelegate(SECTION_FOR_MORE_POPUP___tracks, TidalTrackDelegate::Tidal_Track_General, this);
        connect(delegate, &TidalTrackDelegate::clicked, this, &TidalMycollection::slot_clickedItemTrack_inList);
        this->listWidget_track = this->get_addUIControl_ListWidget(this->delegate);
        this->listWidget_track->setFixedHeight(70*5);

        this->addUIControl_subTitle_withSideBtn_common(tr("My Artists"), tr("View All"), BTN_IDX_SUBTITLE_artists);
        this->hBox_artist = this->get_addUIControl_hBoxLayout_forArtist();

        this->addUIControl_subTitle_withSideBtn_common(tr("My Videos"), tr("View All"), BTN_IDX_SUBTITLE_videos);
        this->hBox_video = this->get_addUIControl_hBoxLayout_forVideo();
    }


    /**
     * @brief TidalMycollection::setUIControl_notLogin
     */
    void TidalMycollection::setUIControl_notLogin(){

        this->widget_notLogin = this->get_createUIControl_notLoginView();
        this->box_contents->addWidget(this->widget_notLogin, 1, Qt::AlignCenter | Qt::AlignVCenter);
    }


    /**
     * @brief My created playlist 삭제 처리에 대해서 UI갱신 처리
     * @param data_itemPos
     */
    void TidalMycollection::proc_preUpdate_delete_myCreatePlaylist(const ItemPositionData& data_itemPos){

        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___playlists_created){
            // index : 실제 데이터 QList 인덱스가 아님. 생성 시의 index임
            this->hide_itemWidget_fromLayout(this->hBox_playlist, data_itemPos.index, data_itemPos.section);

            // 참고로 list_playlist 데이터에서는 삭제하지 않음. :: 화면 refreshing 하기 전까지, index 데이터를 유지하기 위함
        }
    }


    // MARK : TidalPlaylistAll HTTP API ---------------------------------------------------------------------------------

    /**
     * @brief Tidal API 요청 - 사용자가 생성한 playlist
     */
    void TidalMycollection::request_playlistData_tidal(){

        // request HTTP API
        ProcCommon *proc = new ProcCommon(this);
        connect(proc, &ProcCommon::completeReq_list_myFavoritePlaylists, this, &TidalMycollection::slot_applyResult_myFavoritePlaylists);
        proc->request_tidal_getList_myFavoritePlaylists(ProcCommon::DataOrderOption::NAME, ProcCommon::DataOrderDirection::ASC, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief 타이달 다이얼로그에서 로그인 성공 이벤트 발생에 대한 처리
     * @details 토스트 메시지를 띄우고, 로그인한 화면으로 전환
     */
    void TidalMycollection::slot_acceptedDialogLogin(){
        // 부모 함수 호출
        AbstractTidalSubWidget::slot_acceptedDialogLogin();

        // 로그인한 UI로 변경함
        this->setActivePage();
    }


    /**
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void TidalMycollection::slot_clickBtn_subTitle_side(const int btnId){

        if(btnId == BTN_IDX_SUBTITLE_playlists_tidal){
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_COLLECTION_TIDALPALYLIST_ALL_LIST);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_playlists){
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_COLLECTION_PLAYLIST_ALL_LIST);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_tracks){
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_COLLECTION_TRACK_ALL_LIST);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_albums){
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_COLLECTION_ALBUM_ALL_LIST);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_artists){
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_COLLECTION_ARTIST_ALL_LIST);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_videos){
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_COLLECTION_VIDEO_ALL_LIST);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }

    }


    /**
     * @brief Playlist 목록 데이터를 화면에 보인다. Created 와 Favoriate 같은 Layout에 보이도록 한다.
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void TidalMycollection::slot_applyResult_myFavoritePlaylists(const QList<tidal::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);
        int start_index = this->list_playlist_tidal->count();
        this->list_playlist_tidal->append(list_data);
        //this->createList_itemPlaylsit_applyingWithData(*this->list_playlist_tidal, AbstractItem::ImageSizeMode::Square_200x200, this->hBox_playlist, start_index, SECTION_FOR_MORE_POPUP___playlists_tidal);
        this->createList_itemPlaylsit_applyingWithData(*this->list_playlist_tidal, AbstractItem::ImageSizeMode::Square_200x200, this->hBox_playlist_tidal, start_index, SECTION_FOR_MORE_POPUP___playlists_tidal);
    }

    /**
     * @brief Playlist 목록 데이터를 화면에 보인다. - user created playlist
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void TidalMycollection::slot_applyResult_myCreatedPlaylists(const QList<tidal::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){


        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);
        int start_index = this->list_playlist_created->count();
        this->list_playlist_created->append(list_data);
        this->createList_itemPlaylsit_applyingWithData(list_data, AbstractItem::ImageSizeMode::Square_200x200, this->hBox_playlist, start_index, SECTION_FOR_MORE_POPUP___playlists_created);

        // request HTTP API
        //this->request_playlistData_tidal();
    }    


    /**
     * @brief Album 목록 데이터를 화면에 보인다.
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void TidalMycollection::slot_applyResult_myFavoriteAlbums(const QList<tidal::AlbumItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);
        this->list_album->append(list_data);
        this->createList_itemAlbum_applyingWithData(list_data, AbstractItem::ImageSizeMode::Square_200x200, this->hBox_album, 0, SECTION_FOR_MORE_POPUP___albums);
    }


    /**
     * @brief [slot] Track 데이터를 받아서 처리함
     * @param list_data
     */
    void TidalMycollection::slot_applyResult_myFavoriteTracks(const QList<tidal::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);
        this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;
        this->list_track->append(list_data);
        this->createList_itemTrackDelegate_applyingWithData_showImage(list_data, this->listWidget_track, 0, SECTION_FOR_MORE_POPUP___tracks, true);
    }

    /**
     * @brief [Slot] ListWidgetItem 관련 - 썸네일 다운로드 완료
     */
    void TidalMycollection::slot_thumbnailDownloaded_fotListWidget_track(){

        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());
        this->applyImageData_itemTrackDelegate_withFileDownload(fileDownloader_sender, this->listWidget_track, this->list_track);
    }


    /**
     * @brief TidalMycollection::slot_applyResult_myFavoriteArtists
     * @param list_data
     */
    void TidalMycollection::slot_applyResult_myFavoriteArtists(const QList<tidal::ArtistItemData> &list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);
        this->list_artist->append(list_data);
        this->createList_itemArtist_applyingWithData(list_data, AbstractItem::ImageSizeMode::Square_180x180, this->hBox_artist, 0, SECTION_FOR_MORE_POPUP___artists);
    }


    /**
     * @brief TidalMycollection::slot_applyResult_myFavoriteVideos
     * @param list_data
     */
    void TidalMycollection::slot_applyResult_myFavoriteVideos(const QList<tidal::VideoItemData> &list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);
        this->list_video->append(list_data);
        this->createList_itemVideo_applyingWithData(list_data, AbstractItem::ImageSizeMode::Ractangle_340x188, this->hBox_video, 0, SECTION_FOR_MORE_POPUP___videos);
    }



    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalMycollection::slot_clickedItemPlaylist(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___playlists_created){
            this->proc_clicked_itemPlaylist(this->list_playlist_created, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___playlists_tidal){
            this->proc_clicked_itemPlaylist(this->list_playlist_tidal, clickMode, index, section);
        }

    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalMycollection::slot_clickedItemAlbum(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
    }




    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalMycollection::slot_clickedItemArtist(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemArtist(this->list_artist, clickMode, index, section);
    }


    /**
     * @brief [slot] override - ItemVideo 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalMycollection::slot_clickedItemVideo(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemVideo(this->list_video, clickMode, index, section);
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void TidalMycollection::slot_clickedItemTrack_inList(const int idx, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode){

        int section = ((AbstractTidalTrackDelegate*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] OptMorePopup (더보기팝업에서 발생한 signal 처리)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void TidalMycollection::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___videos){
            this->proc_clicked_optMorePopup_fromVideo(this->list_video, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___playlists_created){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist_created, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___playlists_tidal){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist_tidal, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___artists){
            this->proc_clicked_optMorePopup_fromArtist(this->list_artist, index, clickMode);
        }
    }

}
