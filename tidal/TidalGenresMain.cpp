#include "TidalGenresMain.h"
#include <QDebug>
#include <QScroller>
#include "widget/VerticalScrollArea.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/global.h"
#include "common/ProcJsonEasy.h"

#include "tidal/ProcCommon.h"
#include "tidal/ItemPlaylist.h"
#include "tidal/ItemAlbum.h"

#include "ConvertData.h"

namespace tidal {

    const int BTN_IDX_SUBTITLE_playlists = 1;
    const int BTN_IDX_SUBTITLE_tracks = 2;
    const int BTN_IDX_SUBTITLE_albums = 3;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___playlists = 1;
    const int SECTION_FOR_MORE_POPUP___tracks = 2;
    const int SECTION_FOR_MORE_POPUP___albums = 3;

    const int GET_ITEM_SIZE___ONCE_FOR_MAIN = 10;


    /**
     * @brief 생성자. 기본UI만 세팅함
     * @param parent
     */
    TidalGenresMain::TidalGenresMain(QWidget *parent) : AbstractTidalSubWidget(VerticalScroll, parent){

        // 기본 UI 세팅
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ");
        this->setUIControl_playlist();
        this->setUIControl_track();
        this->setUIControl_album();
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // 데이터
        this->list_playlist = new QList<tidal::PlaylistItemData>();
        this->list_album = new QList<tidal::AlbumItemData>();
        this->list_track = new QList<tidal::TrackItemData>();
    }

    /**
     * @brief 소멸자
     */
    TidalGenresMain::~TidalGenresMain(){

        this->deleteLater();
    }



    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalGenresMain::setJsonObject_forData(const QJsonObject &jsonObj){
        // set the main title and genre's path
        this->flagNeedReload = false;

        QString tmp_genre_path =ProcJsonEasy::getString(jsonObj, "path", "");
        if(this->curr_genre_path != tmp_genre_path){
            this->flagNeedReload = true;
            this->label_mainTitle->setText(ProcJsonEasy::getString(jsonObj, "name", "Noname genre"));

            this->curr_genre_path = tmp_genre_path;
            this->genre_api_playlist = QString("genres/%1/playlists").arg(this->curr_genre_path);
            this->genre_api_track = QString("genres/%1/tracks").arg(this->curr_genre_path);
            this->genre_api_album = QString("genres/%1/albums").arg(this->curr_genre_path);
        }
    }


    /**
     * @brief 타이달 > 탐색 > "장르" 1개 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalGenresMain::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractTidalSubWidget::setActivePage();

        if(this->flagNeedReload){

            // init view
            GSCommon::clearLayout(this->hBox_playlist);
            GSCommon::clearLayout(this->hBox_album);
            this->listWidget_track->clear();

            // init data
            jsonArr_tracks_toPlay = QJsonArray();
            this->list_playlist->clear();
            this->list_album->clear();
            this->list_track->clear();


            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_playlists, this, &TidalGenresMain::slot_applyResult_playlists);
            connect(proc, &ProcCommon::completeReq_list_tracks, this, &TidalGenresMain::slot_applyResult_tracks);
            connect(proc, &ProcCommon::completeReq_list_albums, this, &TidalGenresMain::slot_applyResult_albums);

            proc->request_tidal_getList_playlists(this->genre_api_playlist, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);
            proc->request_tidal_getList_tracks(this->genre_api_track, 5, 0);
            proc->request_tidal_getList_albums(this->genre_api_album, 10, 0);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief 장르선택 메인화면의 초기 UI 설정 > 상단의 PlayList 파트
     */
    void TidalGenresMain::setUIControl_playlist(){
        this->addUIControl_subTitle_withSideBtn_common(tr("Playlists"), tr("View All"), BTN_IDX_SUBTITLE_playlists);
        this->hBox_playlist = this->get_addUIControl_hBoxLayout_forPlaylist();
    }


    /**
     * @brief 장르선택 메인화면의 초기 UI 설정 > 트랙 파트
     */
    void TidalGenresMain::setUIControl_track(){
        this->addUIControl_subTitle_withSideBtn_common(tr("New Tracks"), tr("View All"), BTN_IDX_SUBTITLE_tracks);
        this->delegate = new TidalTrackDelegate(SECTION_FOR_MORE_POPUP___tracks, TidalTrackDelegate::Tidal_Track_General, this);
        connect(delegate, &TidalTrackDelegate::clicked, this, &TidalGenresMain::slot_clickedItemTrack_inList);
        this->listWidget_track = this->get_addUIControl_ListWidget(this->delegate);
        this->listWidget_track->setFixedHeight(70*5);
    }


    /**
     * @brief 장르선택 메인화면의 초기 UI 설정 > 앨범 파트
     */
    void TidalGenresMain::setUIControl_album(){
        this->addUIControl_subTitle_withSideBtn_common(tr("New Albums"), tr("View All"), BTN_IDX_SUBTITLE_albums);
        this->hBox_album = this->get_addUIControl_hBoxLayout_forAlbum();
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief [slot] playlist 데이터를 받아서 처리함
     * @param list_data
     */
    void TidalGenresMain::slot_applyResult_playlists(const QList<tidal::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);
        this->list_playlist->append(list_data);
        this->createList_itemPlaylsit_applyingWithData(list_data, AbstractItem::ImageSizeMode::Square_200x200, this->hBox_playlist, 0, SECTION_FOR_MORE_POPUP___playlists);
    }


    /**
     * @brief [slot] Track 데이터를 받아서 처리함
     * @param list_data
     */
    void TidalGenresMain::slot_applyResult_tracks(const QList<tidal::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(flag_lastPage);
        this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;
        this->list_track->append(list_data);
        this->createList_itemTrackDelegate_applyingWithData_showImage(list_data, this->listWidget_track, 0, SECTION_FOR_MORE_POPUP___tracks, true);
    }


    /**
     * @brief [Slot] ListWidgetItem 관련 - 썸네일 다운로드 완료
     */
    void TidalGenresMain::slot_thumbnailDownloaded_fotListWidget_track(){
        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());
        this->applyImageData_itemTrackDelegate_withFileDownload(fileDownloader_sender, this->listWidget_track, this->list_track);
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void TidalGenresMain::slot_applyResult_albums(const QList<tidal::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);
        this->list_album->append(list_data);
        this->createList_itemAlbum_applyingWithData(list_data, AbstractItem::ImageSizeMode::Square_200x200, this->hBox_album, 0, SECTION_FOR_MORE_POPUP___albums);
    }


    /**
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void TidalGenresMain::slot_clickBtn_subTitle_side(const int btnId){

        if(btnId == BTN_IDX_SUBTITLE_playlists){
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = this->label_mainTitle->text();
            data_page.api_subPath = this->genre_api_playlist;
            this->movePage_playlist_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_tracks){
            PageInfo_TrackAllView data_page;
            data_page.pathTitle = this->label_mainTitle->text();
            data_page.api_subPath = this->genre_api_track;
            this->movePage_track_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_albums){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = this->label_mainTitle->text();
            data_page.api_subPath = this->genre_api_album;
            this->movePage_album_allView(data_page);
        }

    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalGenresMain::slot_clickedItemAlbum(const ItemAlbum::ClickMode clickMode){
        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
    }

    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalGenresMain::slot_clickedItemPlaylist(const ItemPlaylist::ClickMode clickMode){
        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void TidalGenresMain::slot_clickedItemTrack_inList(const int idx, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode){
        int section = ((AbstractTidalTrackDelegate*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void TidalGenresMain::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, section, clickMode);
        }
    }

}

