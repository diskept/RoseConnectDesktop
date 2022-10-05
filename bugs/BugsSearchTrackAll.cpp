#include "BugsSearchTrackAll.h"
#include "ProcBugsAPI.h"
#include "common/ProcJsonEasy.h"
#include "ConvertData_forBugs.h"
#include "common/gscommon.h"


namespace bugs {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___track = 1;

    const int GET_ITEM_SIZE___ONCE = 30;


    /**
     * @brief
     * @param parent
     */
    BugsSearchTrackAll::BugsSearchTrackAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll, parent) {
        // data
        this->list_track = new QList<bugs::TrackItemData>();

        // margin 변경
        this->box_contents->setContentsMargins(0,0,0,0);
    }



    /**
     * @brief
     * @param jsonObj
     */
    void BugsSearchTrackAll::setJsonObject_forData(const QJsonObject &jsonObj){
        this->data_search = common::ConvertData_common::convertData_searchData(jsonObj);
        this->flagNeedReload = true;
    }



    /**
     * @brief BugsTracksListAll::applyLoginSuccessState
     */
    void BugsSearchTrackAll::applyLoginSuccessState(){

        if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                this->setUIControl_logined();
            }

            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + "검색 결과 - 곡");
            // clear & init
            this->list_track->clear();
            this->jsonArr_tracks_toPlay = QJsonArray();
            this->listWidget_track->clear();

            // request
            this->flag_lastPage_track = false;
            this->flagReqMore_track = false;
            this->request_more_trackData();

        }
        else{
            if(this->m_flagCurrLogined){
                // 로그인 ---> 로그아웃
                this->m_flagCurrLogined = false;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->setUIControl_notLogin();
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief BugsMusicRecently::setUIControl_logined
     */
    void BugsSearchTrackAll::setUIControl_logined(){

        // UI
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ");

        // 재생버튼 관련 - track용으로
        QWidget *widget_playBtn = this->get_UIControl_btnPlays(true, false);
        this->box_contents->addWidget(widget_playBtn, 0, Qt::AlignLeft);

        this->delegate = new BugsTrackDelegate(SECTION_FOR_MORE_POPUP___track, BugsTrackDelegate::Bugs_Track_General, this);
        connect(delegate, &BugsTrackDelegate::clicked, this, &BugsSearchTrackAll::slot_clickedItemTrack_inList);
        this->listWidget_track = this->get_addUIControl_ListWidget(this->delegate);

    }





    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Request HTTP API
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief BugsTracksListAll::request_trackData
     * @param page
     */
    void BugsSearchTrackAll::request_more_trackData(){
        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // page number
            int page_reqNext = this->list_track->size() / GET_ITEM_SIZE___ONCE + 1;

            // request HTTP API
            ProcBugsAPI *proc = new ProcBugsAPI(this);
            proc->setProperty("section", SECTION_FOR_MORE_POPUP___track);
            connect(proc, &ProcBugsAPI::completeReq_list_tracks, this, &BugsSearchTrackAll::slot_applyResult_tracks);
            proc->request_bugs_search_track(this->data_search.search_word, page_reqNext, GET_ITEM_SIZE___ONCE, ProcBugsAPI::BugsSearch_Sort::exact);
        }
    }

    /**
     * @brief BugsTracksListAll::proc_wheelEvent_to_getMoreData
     */
    void BugsSearchTrackAll::proc_wheelEvent_to_getMoreData(){
        this->request_more_trackData();
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief "전체재생"  버튼 클릭
     */
    void BugsSearchTrackAll::slot_btnClicked_playAll(){
        this->proc_play_allTrack(this->jsonArr_tracks_toPlay, false);
    }


    /**
     * @brief [slot] Track 데이터를 받아서 처리함
     * @param list_data
     */
    void BugsSearchTrackAll::slot_applyResult_tracks(const QList<bugs::TrackItemData>& list_data, const QJsonArray& jsonArr_dataToPlay, const bool flag_lastPage){
        this->flag_lastPage_track = flag_lastPage;

        this->flagReqMore_track = false;            // init
        int section = sender()->property("section").toInt();
        if(section == SECTION_FOR_MORE_POPUP___track){
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            int start_index = this->list_track->size();
            this->list_track->append(list_data);
            this->createList_itemTrackDelegate_applyingWithData_showImage(list_data, this->listWidget_track, start_index, section, true);
        }
    }


    /**
     * @brief [슬롯] 썸네일 다운로드 완료
     */
    void BugsSearchTrackAll::slot_thumbnailDownloaded_fotListWidget_delegate(){
        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());
        int section = fileDownloader_sender->property("section").toInt();
        if(section == SECTION_FOR_MORE_POPUP___track){
            this->applyImageData_itemTrackDelegate_withFileDownload(fileDownloader_sender, this->listWidget_track, this->list_track);
        }
    }


    /**
     * @brief TidalTracksListAll::slot_clickedItemTrack_inList
     * @param index
     * @param clickMode
     */
    void BugsSearchTrackAll::slot_clickedItemTrack_inList(const int idx, const bugs::BugsTrackDelegate::ClickMode clickMode){
        int section = ((tidal::AbstractTidalTrackDelegate*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }


    /**
     * @brief BugsTracksListAll::proc_preUpdate_favorite_track
     * @param data_itemPos
     */
    void BugsSearchTrackAll::proc_preUpdate_favorite_track(const ItemPositionData& data_itemPos){
        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___track){
            // Update Data
            bugs::TrackItemData data_track = this->list_track->at(data_itemPos.index);
            data_track.likes_yn = data_itemPos.likes_yn;
            this->list_track->replace(data_itemPos.index, data_track);

            // Update ListWidget
            this->listWidget_track->item(data_itemPos.index)->setData(Qt::UserRole, ConvertData_forBugs::getObjectJson_trackData(data_track));
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void BugsSearchTrackAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }

}
