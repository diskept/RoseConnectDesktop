#include "BugsTrackListAll_ofArtist.h"
#include "ProcBugsAPI.h"
#include "common/ProcJsonEasy.h"
#include "ConvertData_forBugs.h"
#include "common/gscommon.h"

namespace bugs {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___track = 2;


    const int GET_ITEM_SIZE___ONCE = 30;


    /**
     * @brief
     * @param parent
     */
    BugsTrackListAll_ofArtist::BugsTrackListAll_ofArtist(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll, parent) {
        // data
        this->list_track = new QList<bugs::TrackItemData>();

        this->label_mainTitle = this->get_addUIControl_mainTitle("noname title");
    }

    /**
     * @brief 소멸자.
     */
    BugsTrackListAll_ofArtist::~BugsTrackListAll_ofArtist(){
    }


    /**
     * @brief TidalArtistDetail::setJsonObject_forData
     * @param jsonObj
     */
    void BugsTrackListAll_ofArtist::setJsonObject_forData(const QJsonObject &jsonObj){
        bugs::PageInfo_AlbumAllView_ofArtist prev_data_page = this->data_pageInfo;
        this->data_pageInfo = ConvertData_forBugs::convertData_pageInfo_albumAllView_ofArtist(jsonObj);

//        this->flagNeedReload = false;
//        if(prev_data_page.artist_id!=this->data_pageInfo.artist_id || prev_data_page.filterOpt!=this->data_pageInfo.filterOpt){
//            this->flagNeedReload = true;
//        }

        // "좋아요 하트"가 변경의 소지가 있어서, 항상 새롭게 가져오는 구조로 해야함
        this->flagNeedReload = true;
        this->label_mainTitle->setText(data_pageInfo.mainTitle);
    }



    /**
     * @brief BugsHome::applyLoginSuccessState
     */
    void BugsTrackListAll_ofArtist::applyLoginSuccessState(){

        if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                this->setUIControl_logined();
            }


            // 데이터 갱신이 필요한 경우임
            if(this->flagNeedReload){

                this->label_mainTitle->setText(data_pageInfo.mainTitle);

                this->list_track->clear();
                this->listWidget_track->clear();

                // request HTTP API
                this->flagReqMore_album = false;
                this->request_more_trackData();
            }
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
     * @brief 화면 UI 세팅
     */
    void BugsTrackListAll_ofArtist::setUIControl_logined(){

        // Artist의  Top Track  -----------------------------------------------------------------
        this->delegate = new BugsTrackDelegate(SECTION_FOR_MORE_POPUP___track, BugsTrackDelegate::Bugs_Track_General, this);
        connect(delegate, &BugsTrackDelegate::clicked, this, &BugsTrackListAll_ofArtist::slot_clickedItemTrack_inList);
        this->listWidget_track = this->get_addUIControl_ListWidget(this->delegate);

    }






    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief BugsMusicRecently::proc_wheelEvent_to_getMoreData
     */
    void BugsTrackListAll_ofArtist::proc_wheelEvent_to_getMoreData(){
        this->request_more_trackData();
    }


    /**
     * @brief BugsMusicRecently::request_more_albumData
     */
    void BugsTrackListAll_ofArtist::request_more_trackData(){
        if(!this->flagReqMore_album && !this->flag_lastPage_album){

            this->flagReqMore_album = true;
            // page number
            int page_reqNext = (this->list_track->size() / GET_ITEM_SIZE___ONCE) + 1;


            ProcBugsAPI::BugsArtist_TrackFilter tmp_filter = ProcBugsAPI::BugsArtist_TrackFilter::Release;          // 기본값
            if(data_pageInfo.filterOpt == QVariant::fromValue(ProcBugsAPI::BugsArtist_TrackFilter::All).toString()){
                tmp_filter = ProcBugsAPI::BugsArtist_TrackFilter::All;
            }
            else if(data_pageInfo.filterOpt == QVariant::fromValue(ProcBugsAPI::BugsArtist_TrackFilter::Join).toString()){
                tmp_filter = ProcBugsAPI::BugsArtist_TrackFilter::Join;
            }

            ProcBugsAPI::BugsAritst_TrackSort tmp_sort = ProcBugsAPI::BugsAritst_TrackSort::Popular;          // 기본값
            if(data_pageInfo.filterOpt == QVariant::fromValue(ProcBugsAPI::BugsAritst_TrackSort::Recent).toString()){
                tmp_sort = ProcBugsAPI::BugsAritst_TrackSort::Recent;
            }
            else if(data_pageInfo.filterOpt == QVariant::fromValue(ProcBugsAPI::BugsAritst_TrackSort::Word).toString()){
                tmp_sort = ProcBugsAPI::BugsAritst_TrackSort::Word;
            }

            // getting album list - request HTTP API
            ProcBugsAPI *proc = new ProcBugsAPI(this);
            connect(proc, &ProcBugsAPI::completeReq_list_tracks, this, &BugsTrackListAll_ofArtist::slot_applyResult_tracks);
            proc->request_bugs_getList_tracks_of_artist(this->data_pageInfo.artist_id, GET_ITEM_SIZE___ONCE, page_reqNext, tmp_filter, tmp_sort);

        }
    }


    /**
     * @brief BugsTrackListAll_ofArtist::slot_applyResult_tracks
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void BugsTrackListAll_ofArtist::slot_applyResult_tracks(const QList<bugs::TrackItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        this->flag_lastPage_album = flag_lastPage;

        this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;
        int start_index = 0;
        this->list_track->append(list_data);
        this->createList_itemTrackDelegate_applyingWithData_showImage(list_data, this->listWidget_track, start_index, SECTION_FOR_MORE_POPUP___track, true);
    }


    /**
     * @brief [슬롯] 썸네일 다운로드 완료
     */
    void BugsTrackListAll_ofArtist::slot_thumbnailDownloaded_fotListWidget_delegate(){
        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());
        this->applyImageData_itemTrackDelegate_withFileDownload(fileDownloader_sender, this->listWidget_track, this->list_track);
    }



    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void BugsTrackListAll_ofArtist::slot_clickedItemTrack_inList(const int idx, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode){
        int section = ((tidal::AbstractTidalTrackDelegate*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }



    /**
     * @brief BugsArtistDetail::proc_preUpdate_favorite_track
     * @param data_itemPos
     */
    void BugsTrackListAll_ofArtist::proc_preUpdate_favorite_track(const ItemPositionData& data_itemPos){
        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___track){
            // Update Data
            bugs::TrackItemData data_track = this->list_track->at(data_itemPos.index);
            data_track.likes_yn = data_itemPos.likes_yn;
            this->list_track->replace(data_itemPos.index, data_track);

            // Update ListWidget
            this->listWidget_track->item(data_itemPos.index)->setData(Qt::UserRole, ConvertData_forBugs::getObjectJson_trackData(data_track));
        }
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
    void BugsTrackListAll_ofArtist::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }

}
