#include "BugsMyAlbumDetail.h"
#include <QPixmapCache>

#include "common/gscommon.h"
#include "ProcBugsAPI.h"
#include "common/ProcJsonEasy.h"
#include "bugs/ConvertData_forBugs.h"


namespace bugs {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___track = 1;

    const int GET_ITEM_SIZE___ONCE = 30;                   // 여기만 100개임. 보통 다른곳은 30개


    /**
     * @brief
     * @param parent
     */
    BugsMyAlbumDetail::BugsMyAlbumDetail(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll, parent) {
        // data
        this->list_track = new QList<bugs::TrackItemData>();
    }


    /**
     * @brief 소멸자.
     */
    BugsMyAlbumDetail::~BugsMyAlbumDetail(){

        this->deleteLater();
    }


    /**
     * @brief BugsMyAlbumDetail::setJsonObject_forData
     * @param jsonObj
     */
    void BugsMyAlbumDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        // flagNeedReload 의미가 없을 듯. 좋아요 때문에.  (그래도 혹시 모르니, 아래 로직은 남겨둠)
        this->data_my_album = ConvertData_forBugs::convertData_my_albumData(jsonObj);
        this->flagNeedReload = true;

//        bugs::MyAlbumItemData tmp_data_my_album = ConvertData_forBugs::convertData_my_albumData(jsonObj);
//        this->flagNeedReload = false;
//        if(tmp_data_my_album.playlist_id != this->data_my_album.playlist_id){
//            this->flagNeedReload = true;
//            this->data_my_album = tmp_data_my_album;
//        }

    }


    /**
     * @brief BugsPDAlbumDetail::applyLoginSuccessState
     */
    void BugsMyAlbumDetail::applyLoginSuccessState(){

        if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                this->setUIControl_logined();
            }


            if(this->flagNeedReload){

                // Init UI
                this->label_mainTitle->setText(this->data_my_album.title);
                this->label_mainTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
                this->list_track->clear();
                this->listWidget_track->clear();
                this->listWidget_track->setFixedHeight(70*5);

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
    void BugsMyAlbumDetail::setUIControl_logined(){

        // 메인 타이틀
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ");
        this->label_mainTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy

        // 재생버튼 관련 - track용으로
        QWidget *widget_playBtn = this->get_UIControl_btnPlays(true, false);
        this->box_contents->addWidget(widget_playBtn, 0, Qt::AlignLeft);

        // Track list of my album
        this->delegate_track = new BugsTrackDelegate(SECTION_FOR_MORE_POPUP___track, BugsTrackDelegate::Bugs_Track_General, this);
        connect(delegate_track, &BugsTrackDelegate::clicked, this, &BugsMyAlbumDetail::slot_clickedItemTrack_inList);
        this->listWidget_track = this->get_addUIControl_ListWidget(this->delegate_track);


    }

    /**
     * @brief BugsMyAlbumDetail::proc_wheelEvent_to_getMoreData
     */
    void BugsMyAlbumDetail::proc_wheelEvent_to_getMoreData(){

        this->request_more_trackData();
    }

    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Request HTTP API
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief Track 정보를 요청 (Bugs API)
     */
    void BugsMyAlbumDetail::request_more_trackData(){

        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // page number
            int page_reqNext = this->list_track->size() / GET_ITEM_SIZE___ONCE + 1;

            // request HTTP API -
            ProcBugsAPI *proc = new ProcBugsAPI(this);
            proc->setProperty("section", SECTION_FOR_MORE_POPUP___track);
            connect(proc, &ProcBugsAPI::completeReq_list_items_of_my_album, this, &BugsMyAlbumDetail::slot_applyResult_tracks);
            proc->request_bugs_getList_items_of_my_album(this->data_my_album.playlist_id, page_reqNext, GET_ITEM_SIZE___ONCE);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief "전체재생"  버튼 클릭
     */
    void BugsMyAlbumDetail::slot_btnClicked_playAll(){

        this->proc_play_allTrack(this->jsonArr_tracks_toPlay, false);
    }


    /**
     * @brief "셔플재생" 버튼 클릭
     */
    void BugsMyAlbumDetail::slot_btnClicked_playShuffle(){

        this->proc_play_allTrack(this->jsonArr_tracks_toPlay, true);
    }




    /**
     * @brief BugsMyAlbumDetail::slot_applyResult_tracks
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void BugsMyAlbumDetail::slot_applyResult_tracks(const QList<bugs::TrackItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        this->flag_lastPage_track = flag_lastPage;
        this->flagReqMore_track = false;            // init
        int section = sender()->property("section").toInt();
        if(section == SECTION_FOR_MORE_POPUP___track){
            // Track UI Update
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            int start_index = this->list_track->size();
            this->list_track->append(list_data);
            this->createList_itemTrackDelegate_applyingWithData_showImage(list_data, this->listWidget_track, start_index, SECTION_FOR_MORE_POPUP___track, true);
        }
        print_debug();
    }


    /**
     * @brief [슬롯] 썸네일 다운로드 완료
     */
    void BugsMyAlbumDetail::slot_thumbnailDownloaded_fotListWidget_delegate(){

        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());
        this->applyImageData_itemTrackDelegate_withFileDownload(fileDownloader_sender, this->listWidget_track, this->list_track);
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void BugsMyAlbumDetail::slot_clickedItemTrack_inList(const int idx, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode){

        int section = ((tidal::AbstractTidalTrackDelegate*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }


    /**
     * @brief BugsPDAlbumDetail::proc_preUpdate_favorite_track
     * @param data_itemPos
     */
    void BugsMyAlbumDetail::proc_preUpdate_favorite_track(const ItemPositionData& data_itemPos){

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
    void BugsMyAlbumDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }
}
