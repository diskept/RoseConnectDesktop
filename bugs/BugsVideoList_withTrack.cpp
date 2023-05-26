#include "BugsVideoList_withTrack.h"
#include "ConvertData_forBugs.h"
#include "DataCommon_Bugs.h"
#include "ProcBugsAPI.h"
#include "ProcRoseAPI_withBugs.h"
#include <QDebug>
#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"
#include "tidal/AbstractItem.h"
#include "ConvertGenreCode_BugsAPI.h"


namespace bugs {

    const int SECTION_FOR_MORE_POPUP___video = 1;


    /**
     * @brief Bugs의 Track에 대하여, 관련 영상 목록을 보여주는 화면
     * @param parent
     */
    BugsVideoList_withTrack::BugsVideoList_withTrack(QWidget *parent) : AbstractBugsSubWidget(NoneScroll, parent) {

        // Data
        this->list_video = new QList<bugs::VideoItemData>();

        // margin 변경
        this->box_contents->setContentsMargins(33,0,0,0);
    }


    /**
     * @brief BugsVideoList_withTrack::setJsonObject_forData
     * @param jsonObj
     */
    void BugsVideoList_withTrack::setJsonObject_forData(const QJsonObject &jsonObj){

        bugs::TrackItemData tmp_data_track = ConvertData_forBugs::convertData_trackData(jsonObj);

        this->flagNeedReload = false;
        if(tmp_data_track.track_id != this->data_track.track_id){
            this->flagNeedReload = true;
            this->data_track = tmp_data_track;
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief BugsVideoList_withTrack::applyLoginSuccessState
     */
    void BugsVideoList_withTrack::applyLoginSuccessState(){

        if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                this->setUIControl_logined();
            }

            // clear & init
            this->list_video->clear();
            GSCommon::clearLayout(this->flowLayout_video);

            this->jsonArr_videos_toPlay = QJsonArray();

            this->label_mainTitle->setText(this->data_track.track_title);

            // 연관 동영상의 mv_id 개수만큼 - 개별요청함
            for(int i=0 ; i<this->data_track.list_mv_id.count() ; i++){
                ProcBugsAPI *proc = new ProcBugsAPI(this);
                connect(proc, &ProcBugsAPI::completeReq_video, this, &BugsVideoList_withTrack::slot_applyResult_videoInfo);
                proc->request_bugs_get_video(this->data_track.list_mv_id.at(i));
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
     * @brief BugsVideoList_withTrack::setUIControl_logined
     */
    void BugsVideoList_withTrack::setUIControl_logined(){

        this->label_mainTitle = this->get_addUIControl_mainTitle(" ");
        this->flowLayout_video = this->get_addUIControl_flowLayout(0, 20);

    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief 개별 Video 에 대해서, 여러개 처리하는 로직에 그대로 적용
     * @param data_video
     */
    void BugsVideoList_withTrack::slot_applyResult_videoInfo(bugs::VideoItemData data_video, const QJsonObject& jsonObj_dataToPlay){

        QList<bugs::VideoItemData> tmp_list;
        tmp_list.append(data_video);

        int start_index = this->list_video->length();
        this->list_video->append(tmp_list);
        this->jsonArr_videos_toPlay.append(jsonObj_dataToPlay);

        this->createList_itemVideo_applyingWithData(tmp_list, tidal::AbstractItem::ImageSizeMode::Ractangle_251x139, this->flowLayout_video, start_index, SECTION_FOR_MORE_POPUP___video);
    }


    /**
     * @brief BugsVideoList_withTrack::slot_clickedItemVideo
     * @param clickMode
     */
    void BugsVideoList_withTrack::slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemVideo(this->list_video, this->jsonArr_videos_toPlay, clickMode, index, section);
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
    void BugsVideoList_withTrack::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
//        AbstractCommonSubWidget::slot_optMorePopup_menuClicked(clickMode, index, section);        // to debug

        if(section == SECTION_FOR_MORE_POPUP___video){
            this->proc_clicked_optMorePopup_fromVideo(this->list_video, this->jsonArr_videos_toPlay, index, clickMode);
        }
    }



}
