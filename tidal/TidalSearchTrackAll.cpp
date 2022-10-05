#include "TidalSearchTrackAll.h"
#include "ProcCommon.h"
#include <QPixmapCache>
#include "common/ProcJsonEasy.h"
#include "ConvertData.h"

namespace tidal {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___search_tracks = 1;

    const int GET_ITEM_SIZE___ONCE = 50;


    /**
     * @brief
     * @param parent
     */
    TidalSearchTrackAll::TidalSearchTrackAll(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll, parent) {

        // data
        this->list_track = new QList<tidal::TrackItemData>();

        // UI
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ");

        this->delegate = new TidalTrackDelegate(SECTION_FOR_MORE_POPUP___search_tracks, TidalTrackDelegate::Tidal_Track_General, this);
        connect(delegate, &TidalTrackDelegate::clicked, this, &TidalSearchTrackAll::slot_clickedItemTrack_inList);
        this->listWidget_track = this->get_addUIControl_ListWidget(this->delegate);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    }


    /**
     * @brief
     * @param jsonObj
     */
    void TidalSearchTrackAll::setJsonObject_forData(const QJsonObject &jsonObj){
        common::SearchData tmp_data = common::ConvertData_common::convertData_searchData(jsonObj);
        this->flagNeedReload = false;
        if(tmp_data.search_word != this->data_search.search_word){
            this->flagNeedReload = true;
            this->data_search = common::ConvertData_common::convertData_searchData(jsonObj);
        }
    }


    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalSearchTrackAll::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractTidalSubWidget::setActivePage();

        if(this->flagNeedReload){
            // INIT
            this->list_track->clear();
            this->listWidget_track->clear();
            this->listWidget_track->setFixedHeight(70*5);

            this->jsonArr_tracks_toPlay = QJsonArray();

            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + tr("Search Result") + " - Tracks");
            //this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + tr("검색결과") + " - Tracks");

            // request HTTP API
            this->flagReqMore_track = false;
            this->flag_lastPage_track = false;
            this->request_more_trackData();
        }

    }



    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void TidalSearchTrackAll::proc_wheelEvent_to_getMoreData(){
        this->request_more_trackData();
    }




    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void TidalSearchTrackAll::request_more_trackData(){
        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // next_offset
            int next_offset = this->list_track->size();

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_tracks, this, &TidalSearchTrackAll::slot_applyResult_tracks);
            proc->request_tidal_searchAllItems(this->data_search.search_word, GET_ITEM_SIZE___ONCE, next_offset, ProcCommon::TidalSearch_ContentType::Tracks);

        }
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief [slot] Track 데이터를 받아서 처리함
     * @param list_data
     */
    void TidalSearchTrackAll::slot_applyResult_tracks(const QList<tidal::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        this->flag_lastPage_track = flag_lastPage;
        this->flagReqMore_track = false;
        ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

        int start_index = this->list_track->size();
        this->list_track->append(list_data);
        this->createList_itemTrackDelegate_applyingWithData_showImage(list_data, this->listWidget_track, start_index, SECTION_FOR_MORE_POPUP___search_tracks, true);
    }


    /**
     * @brief [슬롯] 썸네일 다운로드 완료
     */
    void TidalSearchTrackAll::slot_thumbnailDownloaded_fotListWidget_track(){
        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());
        this->applyImageData_itemTrackDelegate_withFileDownload(fileDownloader_sender, this->listWidget_track, this->list_track);
    }


    /**
     * @brief TidalTracksListAll::slot_clickedItemTrack_inList
     * @param index
     * @param clickMode
     */
    void TidalSearchTrackAll::slot_clickedItemTrack_inList(const int idx, const tidal::TidalTrackDelegate::ClickMode clickMode){
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
    void TidalSearchTrackAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        Q_UNUSED(section);
        this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
    }

}
