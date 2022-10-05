#include "TidalTop20Main.h"
#include "ItemAlbum.h"
#include "ItemPlaylist.h"
#include "ProcCommon.h"

#include <QScrollArea>
#include <QScroller>

#include <common/gscommon.h>
#include "ConvertData.h"

namespace tidal {

    const int BTN_IDX_SUBTITLE_tracks = 2;
    const int BTN_IDX_SUBTITLE_albums = 3;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___tracks = 1;
    const int SECTION_FOR_MORE_POPUP___albums = 2;

    const QString TIDAL_API_PATH_ALBUM = "featured/top/albums";
    const QString TIDAL_API_PATH_TRACK = "featured/top/tracks";

    const QString TIDAL_MAIN_TITLE = "TOP";
    const QString TIDAL_SUB_TITLE___album = "New Albums";
    const QString TIDAL_SUB_TITLE___track = "New Tracks";


    /**
     * @brief "TIDAL > 탐색 > Top 메인" 화면의 생성자
     * @param parent
     */
    TidalTop20Main::TidalTop20Main(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll, parent){


        this->addUIControl_mainTitle(TIDAL_MAIN_TITLE);
        this->setUIControl_track();
        this->setUIControl_album();
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        this->flagInitDraw = true;
        this->list_album = new QList<tidal::AlbumItemData>();
        this->list_track = new QList<tidal::TrackItemData>();
    }


    /**
     * @brief 소멸자.
     */
    TidalTop20Main::~TidalTop20Main(){

        this->deleteLater();
    }


    /**
     * @brief 타이달 > 탐색 > "장르" 1개 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalTop20Main::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractTidalSubWidget::setActivePage();

        if(this->flagInitDraw){
            this->flagInitDraw = false;

            // init
            this->list_album->clear();
            this->list_track->clear();

            this->listWidget_track->clear();
            GSCommon::clearLayout(this->hBox_album);

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_albums, this, &TidalTop20Main::slot_applyResult_albums);
            connect(proc, &ProcCommon::completeReq_list_tracks, this, &TidalTop20Main::slot_applyResult_tracks);
            proc->request_tidal_getList_albums(TIDAL_API_PATH_ALBUM, 10, 0);
            proc->request_tidal_getList_tracks(TIDAL_API_PATH_TRACK, 5, 0);
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
    void TidalTop20Main::setUIControl_track(){
        this->addUIControl_subTitle_withSideBtn_common(TIDAL_SUB_TITLE___track, tr("View All"), BTN_IDX_SUBTITLE_tracks);
        this->delegate = new TidalTrackDelegate(SECTION_FOR_MORE_POPUP___tracks, TidalTrackDelegate::Tidal_Track_General, this);
        connect(delegate, &TidalTrackDelegate::clicked, this, &TidalTop20Main::slot_clickedItemTrack_inList);
        this->listWidget_track = this->get_addUIControl_ListWidget(this->delegate);
        this->listWidget_track->setFixedHeight(70*5);
    }


    /**
     * @brief 장르선택 메인화면의 초기 UI 설정 > 앨범 파트
     */
    void TidalTop20Main::setUIControl_album(){
        this->addUIControl_subTitle_withSideBtn_common(TIDAL_SUB_TITLE___album, tr("View All"), BTN_IDX_SUBTITLE_albums);
        this->hBox_album = this->get_addUIControl_hBoxLayout_forAlbum();
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] 앨범 데이터를 받아서 처리함
     * @param list_data
     */
    void TidalTop20Main::slot_applyResult_albums(const QList<tidal::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);
        this->list_album->append(list_data);
        this->createList_itemAlbum_applyingWithData(list_data, AbstractItem::ImageSizeMode::Square_200x200, this->hBox_album, 0, SECTION_FOR_MORE_POPUP___albums);
    }



    /**
     * @brief
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void TidalTop20Main::slot_applyResult_tracks(const QList<tidal::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(flag_lastPage);
        this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;
        this->list_track->append(list_data);
        this->createList_itemTrackDelegate_applyingWithData_showImage(list_data, this->listWidget_track, 0, SECTION_FOR_MORE_POPUP___tracks, true);
    }


    /**
     * @brief [Slot] ListWidgetItem 관련 - 썸네일 다운로드 완료
     */
    void TidalTop20Main::slot_thumbnailDownloaded_fotListWidget_track(){
        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());
        this->applyImageData_itemTrackDelegate_withFileDownload(fileDownloader_sender, this->listWidget_track, this->list_track);
    }


    /**
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void TidalTop20Main::slot_clickBtn_subTitle_side(const int btnId){

        if(btnId == BTN_IDX_SUBTITLE_tracks){
            PageInfo_TrackAllView data_page;
            data_page.pathTitle = QString("%1 - %2").arg(TIDAL_MAIN_TITLE).arg(TIDAL_SUB_TITLE___track);
            data_page.api_subPath = TIDAL_API_PATH_TRACK;
            this->movePage_track_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_albums){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = QString("%1 - %2").arg(TIDAL_MAIN_TITLE).arg(TIDAL_SUB_TITLE___album);
            data_page.api_subPath = TIDAL_API_PATH_ALBUM;
            this->movePage_album_allView(data_page);
        }

    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalTop20Main::slot_clickedItemAlbum(const AbstractItem::ClickMode clickMode){
        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();
        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void TidalTop20Main::slot_clickedItemTrack_inList(const int idx, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode){
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
    void TidalTop20Main::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }

}
