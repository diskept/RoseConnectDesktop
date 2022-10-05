#include "QobuzMaster.h"
#include "qobuz/ConvertData_forQobuz.h"
#include "ProcCommon_forQobuz.h"
#include <QLabel>
#include <QVBoxLayout>
#include "common/gscommon.h"

namespace qobuz {


    const int BTN_IDX_SUBTITLE_playlists= 1;
    const int BTN_IDX_SUBTITLE_albums = 3;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___playlists = 1;
    const int SECTION_FOR_MORE_POPUP___albums = 3;

    const int GET_ITEM_SIZE___ONCE_FOR_MAIN = 10;

    const QString TIDAL_API_PATH_ALBUM = "master/recommended/albums";
    const QString TIDAL_API_PATH_PLAYLIST = "master/recommended/playlists";

    const QString TIDAL_MAIN_TITLE = "Master";
    const QString TIDAL_SUB_TITLE___album = "New Albums";
    const QString TIDAL_SUB_TITLE___playlist = "New Playlist";


    /**
     * @brief "TIDAL > 탐색 > NEW 메인" 화면의 생성자
     * @param parent
     */
    QobuzMaster::QobuzMaster(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll, parent){

        this->flagInitDraw = true;

        this->setUIControl_album();
        this->setUIControl_playlist();

        this->list_album = new QList<qobuz::AlbumItemData>();
        this->list_playlist = new QList<qobuz::PlaylistItemData>();
    }


    /**
     * @brief 소멸자.
     */
    QobuzMaster::~QobuzMaster(){
    }

    /**
     * @brief 타이달 > 탐색 > "장르" 1개 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void QobuzMaster::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractQobuzSubWidget::setActivePage();

        if(this->flagInitDraw){
            this->flagInitDraw = false;

            // init
            this->list_album->clear();
            this->list_playlist->clear();

            GSCommon::clearLayout(this->hBox_playlist);
            GSCommon::clearLayout(this->hBox_album);


            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_albums, this, &QobuzMaster::slot_applyResult_albums);
            connect(proc, &ProcCommon::completeReq_list_playlists, this, &QobuzMaster::slot_applyResult_playlists);
            proc->request_qobuz_getList_albums(TIDAL_API_PATH_ALBUM, "", 0, 10, 0);
            proc->request_qobuz_getList_playlists(TIDAL_API_PATH_PLAYLIST, "", 0, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 메인화면의 초기 UI 설정 > Playlist 파트
     */
    void QobuzMaster::setUIControl_playlist(){
        this->addUIControl_subTitle_withSideBtn_common(TIDAL_SUB_TITLE___playlist, tr("View All"), BTN_IDX_SUBTITLE_playlists);
        this->hBox_playlist = this->get_addUIControl_hBoxLayout_forPlaylist();
    }


    /**
     * @brief 메인화면의 초기 UI 설정 > 앨범 파트
     */
    void QobuzMaster::setUIControl_album(){
        this->addUIControl_subTitle_withSideBtn_common(TIDAL_SUB_TITLE___album, tr("View All"), BTN_IDX_SUBTITLE_albums);
        this->hBox_album = this->get_addUIControl_hBoxLayout_forAlbum();
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] 앨범 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzMaster::slot_applyResult_albums(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);
        this->list_album->append(list_data);
        this->createList_itemAlbum_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_album, 0, SECTION_FOR_MORE_POPUP___albums);
    }


    /**
     * @brief [slot] playlist 데이터를 받아서 처리함
     * @param list_data
     * @param jsonArr_dataToPlay
     * @param flag_lastPage
     */
    void QobuzMaster::slot_applyResult_playlists(const QList<qobuz::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(flag_lastPage);
        Q_UNUSED(jsonArr_dataToPlay);
        this->list_playlist->append(list_data);
        this->createList_itemPlaylsit_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_playlist, 0, SECTION_FOR_MORE_POPUP___playlists);
    }


    /**
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void QobuzMaster::slot_clickBtn_subTitle_side(const int btnId){

        if(btnId == BTN_IDX_SUBTITLE_playlists){
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = TIDAL_SUB_TITLE___playlist;
            data_page.api_subPath = TIDAL_API_PATH_PLAYLIST;
            this->movePage_playlist_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_albums){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = TIDAL_SUB_TITLE___album;
            data_page.api_subPath = TIDAL_API_PATH_ALBUM;
            this->movePage_album_allView(data_page);
        }

    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzMaster::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
    }



    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzMaster::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);
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
    void QobuzMaster::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
    }

}
