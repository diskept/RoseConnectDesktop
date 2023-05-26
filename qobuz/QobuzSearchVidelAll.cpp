#include "QobuzSearchVidelAll.h"
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
    QobuzSearchVidelAll::QobuzSearchVidelAll(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll, parent) {

        // Data
        this->list_video = new QList<qobuz::VideoItemData>();

        // UI
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ");
        this->flowLayout_video = this->get_addUIControl_flowLayout(0, 20);
    }

    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzSearchVidelAll::setJsonObject_forData(const QJsonObject &jsonObj){
        common::SearchData tmp_data = common::ConvertData_common::convertData_searchData(jsonObj);
        this->flagNeedReload = false;
        if((tmp_data.search_word != this->data_search.search_word) || (tmp_data.search_keyword != this->data_search.search_keyword)){
            this->flagNeedReload = true;
            this->data_search = common::ConvertData_common::convertData_searchData(jsonObj);
        }
    }


    /**
     * @brief 화면 활성화
     */
    void QobuzSearchVidelAll::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractQobuzSubWidget::setActivePage();

        if(this->flagNeedReload){
            // init data
            this->list_video->clear();

            // init UI
            GSCommon::clearLayout(this->flowLayout_video);

            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + tr("Search result") + " - Videos");
            //this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + tr("검색결과") + " - Videos");

            // request HTTP API
            this->flagReqMore_video = false;
            this->flag_lastPage_video = false;
            this->request_more_videoData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }






    // MARK : Create UI and Set ------------------------------------------------------------------------------------------

    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void QobuzSearchVidelAll::proc_wheelEvent_to_getMoreData(){
        this->request_more_videoData();
    }




    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Artist 데이터를 더 요청한다.
     * @param page
     */
    void QobuzSearchVidelAll::request_more_videoData(){
        if(!this->flagReqMore_video && !this->flag_lastPage_video){
            this->flagReqMore_video = true;

            // next_offset
            int next_offset = this->list_video->size();

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_videos, this, &QobuzSearchVidelAll::slot_applyResult_videos);
            proc->request_qobuz_searchAllItems(this->data_search.search_keyword, GET_ITEM_SIZE___ONCE, next_offset, ProcCommon::QobuzSearch_ContentType::Videos);

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
    void QobuzSearchVidelAll::slot_applyResult_videos(const QList<qobuz::VideoItemData> &list_data, const bool flag_lastPage){
        // api가 limit 적용이 안됨
        this->flag_lastPage_video = flag_lastPage;
        this->flagReqMore_video = false;

        int start_index = this->list_video->size();
        this->list_video->append(list_data);
        this->createList_itemVideo_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Ractangle_251x139, this->flowLayout_video, start_index, SECTION_FOR_MORE_POPUP___video);

    }



    /**
     * @brief [slot] override - ItemVideo 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzSearchVidelAll::slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode){
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
    void QobuzSearchVidelAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        Q_UNUSED(section);
        this->proc_clicked_optMorePopup_fromVideo(this->list_video, index, clickMode);
    }

}
