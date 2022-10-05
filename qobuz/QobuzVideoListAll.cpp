#include "QobuzVideoListAll.h"
#include "ProcCommon_forQobuz.h"
#include "ConvertData_forQobuz.h"
#include "common/gscommon.h"


namespace qobuz {

    const int SECTION_FOR_MORE_POPUP___video = 1;

    const int GET_ITEM_SIZE___ONCE = 30;

    /**
     * @brief Video All List
     * @param parent
     */
    QobuzVideoListAll::QobuzVideoListAll(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll, parent) {

        // Data
        this->list_video = new QList<qobuz::VideoItemData>();

        // UI
        this->setUIControl_video();
    }

    /**
     * @brief 소멸자.
     */
    QobuzVideoListAll::~QobuzVideoListAll(){
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzVideoListAll::setJsonObject_forData(const QJsonObject &jsonObj){
        qobuz::PageInfo_VideoAllView data_pageInfo = ConvertData::convertData_pageInfo_videoAllView(jsonObj);
        QString curr_api_subPath = this->api_subPath;
        this->flagNeedReload = false;

        if(curr_api_subPath != data_pageInfo.api_subPath){
            this->flagNeedReload = true;
            this->label_mainTitle->setText(data_pageInfo.pathTitle);
            this->api_subPath = data_pageInfo.api_subPath;
        }
    }


    /**
     * @brief 화면 활성화
     */
    void QobuzVideoListAll::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractQobuzSubWidget::setActivePage();

        if(this->flagNeedReload){
            // init data
            this->list_video->clear();

            // init UI
            GSCommon::clearLayout(this->flowLayout_video);

            // request HTTP API
            this->flagReqMore_video = false;
            this->flag_lastPage_video = false;
            this->request_more_videoData();

        }
    }






    // MARK : Create UI and Set ------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void QobuzVideoListAll::setUIControl_video(){
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ");
        this->flowLayout_video = this->get_addUIControl_flowLayout(0, 20);
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void QobuzVideoListAll::proc_wheelEvent_to_getMoreData(){
        this->request_more_videoData();
    }




    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Artist 데이터를 더 요청한다.
     * @param page
     */
    void QobuzVideoListAll::request_more_videoData(){
        if(!this->flagReqMore_video && !this->flag_lastPage_video){
            this->flagReqMore_video = true;

            // next_offset
            int next_offset = this->list_video->size();

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_videos, this, &QobuzVideoListAll::slot_applyResult_videos);
            proc->request_qobuz_getList_videos(this->api_subPath, GET_ITEM_SIZE___ONCE, next_offset);             // 우선 최대 30까지만.
        }
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : [Slot] functions
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief about New Video
     * @param list_data
     */
    void QobuzVideoListAll::slot_applyResult_videos(const QList<qobuz::VideoItemData> &list_data, const bool flag_lastPage){
        // api가 limit 적용이 안됨
        QList<qobuz::VideoItemData> list_30;
        for(int i=0 ; i<30 ; i++){
            if(i >= list_data.size()){
                break;
            }
            list_30.append(list_data.at(i));
        }


        this->flag_lastPage_video = flag_lastPage;
        this->flagReqMore_video = false;

        int start_index = this->list_video->size();
        this->list_video->append(list_30);
        this->createList_itemVideo_applyingWithData(list_30, tidal::AbstractItem::ImageSizeMode::Ractangle_251x139, this->flowLayout_video, start_index, SECTION_FOR_MORE_POPUP___video);

    }



    /**
     * @brief [slot] override - ItemVideo 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzVideoListAll::slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리 - Video 파트가 여러개임
        this->proc_clicked_itemVideo(this->list_video, clickMode, index, section);
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
    void QobuzVideoListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___video){
            this->proc_clicked_optMorePopup_fromVideo(this->list_video, index, clickMode);
        }
    }

}
