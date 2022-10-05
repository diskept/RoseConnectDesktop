#include "TidalMyVideoAll.h"
#include "ProcCommon.h"
#include "ConvertData.h"
#include "common/gscommon.h"
#include <QResizeEvent>

namespace tidal {

    const int SECTION_FOR_MORE_POPUP___video = 1;

    const int GET_ITEM_SIZE___ONCE = 30;


    /**
     * @brief Video All List
     * @param parent
     */
    TidalMyVideoAll::TidalMyVideoAll(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // Data
        this->list_video = new QList<tidal::VideoItemData>();
        this->list_filterOpt_sort = AbstractTidalSubWidget::get_list_filterOptData_forMyCollection();

        // UI
        this->setUIControl_video();

    }

    /**
     * @brief 소멸자.
     */
    TidalMyVideoAll::~TidalMyVideoAll(){

        this->deleteLater();
    }


    /**
     * @brief 화면 활성화
     */
    void TidalMyVideoAll::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractTidalSubWidget::setActivePage();

        // init
        this->initAll();
        this->chooseFilterOpt->hideFilterBox();

        // init Data
        this->selected_filterCode = this->list_filterOpt_sort.first().opt_code;         // default

        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->clear_filter();
        this->filterWidget->add_filter(def_code, def_name);

        this->request_more_videoData();
    }






    // MARK : Create UI and Set ------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void TidalMyVideoAll::setUIControl_video(){
        this->addUIControl_mainTitle("My Videos");

        // filtering
        this->setUiControl_filter();

        // layout for items
        this->flowLayout_video = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &TidalMyVideoAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief TidalMyVideoAll::setUiControl_filter
     */
    void TidalMyVideoAll::setUiControl_filter(){
        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &TidalMyVideoAll::slot_filterClicked);


        QHBoxLayout *hBox_filter = new QHBoxLayout();
        hBox_filter->setSpacing(0);
        hBox_filter->setContentsMargins(0,0,0,0);
        hBox_filter->addWidget(this->filterWidget, 0, Qt::AlignVCenter | Qt::AlignLeft);
        hBox_filter->addStretch(1);
        hBox_filter->addSpacing(77);

        QWidget *tmp_widget_forFilter = new QWidget();
        tmp_widget_forFilter->setContentsMargins(0,0,0,0);
        tmp_widget_forFilter->setObjectName("widget_forFilter");
        tmp_widget_forFilter->setStyleSheet("#widget_forFilter { background-color:#171717; }");
        tmp_widget_forFilter->setLayout(hBox_filter);
        tmp_widget_forFilter->setFixedHeight(70);

        this->box_mainTitle->addWidget(tmp_widget_forFilter);
        this->addUIControl_dividedLine_onMain();

    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void TidalMyVideoAll::initAll(){

        this->list_video->clear();

        this->flag_lastPage_video = false;

        this->flagReqMore_video = false;

        GSCommon::clearLayout(this->flowLayout_video);

    }

    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void TidalMyVideoAll::proc_wheelEvent_to_getMoreData(){

        this->request_more_videoData();
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void TidalMyVideoAll::resizeEvent(QResizeEvent *event){

        AbstractTidalSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }
    }




    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void TidalMyVideoAll::slot_filterClicked(){

        if(this->chooseFilterOpt->isVisible()){
            this->chooseFilterOpt->hideFilterBox();
        }
        else{
            // Filter Box Show
            this->chooseFilterOpt->clearFilter();
            this->chooseFilterOpt->addFilterGroup(tr("sort"), this->list_filterOpt_sort, this->selected_filterCode);
            //this->chooseFilterOpt->addFilterGroup(tr("정렬"), this->list_filterOpt_sort, this->selected_filterCode);
            this->chooseFilterOpt->showFilterBox(QPoint(30, 130), this->size());
        }
    }

    /**
     * @brief 필터링 옵션박스 Widget에서 필터링을 하나 클릭한 경우이다.
     * @param selectedCode
     */
    void TidalMyVideoAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

        // 코드가 다른 경우에 대해서만 처리한다.
        if(this->selected_filterCode != selected_filterCode){

            // update
            this->selected_filterCode = selected_filterCode;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(selected_filterCode.toString(), selected_filterName);

            // clear - 옵션코드가 바뀌었으므로 모두 초기화한다.
            this->initAll();

            this->scrollTop();
            this->request_more_videoData();

        }

    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Artist 데이터를 더 요청한다.
     * @param page
     */
    void TidalMyVideoAll::request_more_videoData(){

        if(!this->flagReqMore_video && !this->flag_lastPage_video){
            this->flagReqMore_video = true;

            // next_offset
            int next_offset = this->list_video->size();

            // filtering
            QString selected_sortOpt = this->selected_filterCode.toString();
            ProcCommon::DataOrderOption sortOpt = ProcCommon::DataOrderOption::NAME;      // 기본값
            ProcCommon::DataOrderDirection orderDirection = ProcCommon::DataOrderDirection::ASC;      // 기본값
            if(selected_sortOpt == QVariant::fromValue(ProcCommon::DataOrderOption::DATE).toString()){
                sortOpt = ProcCommon::DataOrderOption::DATE;
                orderDirection = ProcCommon::DataOrderDirection::DESC;
            }

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_myFavoriteVideos, this, &TidalMyVideoAll::slot_applyResult_myFavoriteVideos);
            proc->request_tidal_getList_myFavoriteVideos(sortOpt, orderDirection, GET_ITEM_SIZE___ONCE, next_offset);       // 최대 99까지
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : [Slot] functions
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief TidalMycollection::slot_applyResult_myFavoriteVideos
     * @param list_data
     */
    void TidalMyVideoAll::slot_applyResult_myFavoriteVideos(const QList<tidal::VideoItemData> &list_data, const bool flag_lastPage){

        this->flag_lastPage_video = flag_lastPage;
        this->flagReqMore_video = false;

        int start_index = this->list_video->size();
        this->list_video->append(list_data);
        //this->createList_itemVideo_applyingWithData(list_data, AbstractItem::ImageSizeMode::Ractangle_251x139, this->flowLayout_video, start_index, SECTION_FOR_MORE_POPUP___video);
        this->createList_itemVideo_applyingWithData(list_data, AbstractItem::ImageSizeMode::Ractangle_284x157, this->flowLayout_video, start_index, SECTION_FOR_MORE_POPUP___video);
    }


    /**
     * @brief [slot] override - ItemVideo 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalMyVideoAll::slot_clickedItemVideo(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

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
    void TidalMyVideoAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___video){
            this->proc_clicked_optMorePopup_fromVideo(this->list_video, index, clickMode);
        }
    }

}
