#include "bugs/BugsMyVideoAll.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/ProcBugsAPI.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QResizeEvent>
#include <QScrollBar>


namespace bugs {

    const int SECTION_FOR_MORE_POPUP___videos = 1;


    /**
     * @brief QobuzMoodsMain::QobuzMoodsMain
     * @param parent
     */
    BugsVideoRecentlyAll::BugsVideoRecentlyAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // Data
        this->list_video = new QList<bugs::VideoItemData>();
        this->list_filterOpt_sort = AbstractBugsSubWidget::get_list_filterOptData_forMyCollection();

        if(global.user_forBugs.isLogined() == true){

            // 기본 UI 세팅
            this->setUIControl_videos();
        }
    }


    /**
     * @brief 소멸자.
     */
    BugsVideoRecentlyAll::~BugsVideoRecentlyAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsVideoRecentlyAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        QString mainTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(this->page != pageCode){
            this->flagNeedReload = true;

            this->page = pageCode;

            this->label_mainTitle->setText(mainTitle);

            // init
            this->next_offset = 0;
            this->video_total_cnt = 0;
            this->video_draw_cnt = 0;

            this->list_video->clear();

            // request HTTP API
            this->flagReqMore_video = false;
            this->flag_lastPage_video = false;

            this->flag_video_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_videoData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void BugsVideoRecentlyAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            GSCommon::clearLayout(this->flowLayout_videos);

           /*// init
            this->chooseFilterOpt->hideFilterBox();

            // init Data
            this->selected_filterCode = this->list_filterOpt_sort.first().opt_code;         // default

            QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
            QString def_name = this->list_filterOpt_sort.first().opt_name;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(def_code, def_name);*/

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
        }
    }


    // MARK : Create UI and Set --------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void BugsVideoRecentlyAll::setUIControl_videos(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("videos");

        GSCommon::clearLayout(this->box_contents);      // Layout 초기화
        this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // layout for items
        this->flowLayout_videos = this->get_addUIControl_flowLayout(0, 20);
        GSCommon::clearLayout(this->flowLayout_videos);

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &BugsVideoRecentlyAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyvideoAll::setUIControl_filter
     */
    void BugsVideoRecentlyAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &BugsVideoRecentlyAll::slot_filterClicked);

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
    void BugsVideoRecentlyAll::initAll(){

        // init
        this->next_offset = 0;
        this->video_total_cnt = 0;
        this->video_draw_cnt = 0;

        this->list_video->clear();

        // request HTTP API
        this->flagReqMore_video = false;
        this->flag_lastPage_video = false;

        this->flag_video_draw = false;

        GSCommon::clearLayout(this->flowLayout_videos);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void BugsVideoRecentlyAll::proc_wheelEvent_to_getMoreData(){

        print_debug();

        if((this->video_total_cnt > this->video_draw_cnt) && (this->list_video->size() > this->video_draw_cnt) && (this->flag_video_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_video_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_videoDraw();
           // this->request_more_videoData();//c220716
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void BugsVideoRecentlyAll::resizeEvent(QResizeEvent *event){//c230223

        AbstractBugsSubWidget::resizeEvent(event);
        int w = flowLayout_videos->sizeHint().width();
        int  l = 80, r = 60, scrollbarW = 0;

        int mod_nn = (global.LmtCnt-l-r-scrollbarW)%(w+0);
        //qDebug() << "mod_nn=" << mod_nn;
        int i = 0;
        while(1){

            mod_nn = (global.LmtCnt-l-r-scrollbarW)%(w+0+i);
            if(mod_nn > 20){

                mod_nn = (global.LmtCnt-l-r-scrollbarW)%(w+(0+(i++)));
            }else{
                break;
            }
        }

        flowLayout_videos->setSpacingHV(0+i,20);
        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }
    }



    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void BugsVideoRecentlyAll::slot_filterClicked(){

        if(this->chooseFilterOpt->isVisible()){
            this->chooseFilterOpt->hideFilterBox();
        }
        else{
            // Filter Box Show
            this->chooseFilterOpt->clearFilter();
            this->chooseFilterOpt->addFilterGroup("정렬", this->list_filterOpt_sort, this->selected_filterCode);
            this->chooseFilterOpt->showFilterBox(QPoint(30, 130), this->size());
        }
    }

    /**
     * @brief 필터링 옵션박스 Widget에서 필터링을 하나 클릭한 경우이다.
     * @param selectedCode
     */
    void BugsVideoRecentlyAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

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
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void BugsVideoRecentlyAll::request_more_videoData(){

        if(!this->flagReqMore_video && !this->flag_lastPage_video){
            this->flagReqMore_video = true;

            // j220913 list count check
            int width_cnt;//c230223
            if(flowLayout_videos->sizeHint().width() < 0) {//c230223
                width_cnt = global.LmtCnt / 217;
            }else{
                width_cnt = global.LmtCnt / flowLayout_videos->sizeHint().width();//
            }
            int mod = this->video_widget_cnt % width_cnt;

            if(mod == 0){
                this->video_widget_cnt = width_cnt * 10;
            }
            else{
                this->video_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
            // j220913 list count check

            // next_offset
            if(this->list_video->size() == 0){
                this->next_offset = 1;
            }
            else{
                this->next_offset++;
            }


            // filtering
            /*QString selected_sortOpt = this->selected_filterCode.toString();
            ProcCommon::DataOrderOption sortOpt = ProcCommon::DataOrderOption::NAME;                // 기본값
            ProcCommon::DataOrderDirection orderDirection = ProcCommon::DataOrderDirection::ASC;    // 기본값

            if(selected_sortOpt == QVariant::fromValue(ProcCommon::DataOrderOption::DATE).toString()){
                sortOpt = ProcCommon::DataOrderOption::DATE;
                orderDirection = ProcCommon::DataOrderDirection::DESC;
            }*/
            // request HTTP API
            ProcBugsAPI *proc_video_recently = new ProcBugsAPI(this);
            connect(proc_video_recently, &ProcBugsAPI::completeReq_list_videos, this, &BugsVideoRecentlyAll::slot_applyResult_videoRecently);
            proc_video_recently->request_bugs_getList_videos("genre/mv/total/all", this->video_widget_cnt, this->next_offset);

            if(this->next_offset == 0){
                this->flag_video_draw = true;
            }
        }
    }


    void BugsVideoRecentlyAll::request_more_videoDraw(){

        // j220913 list count check
        int width_cnt;//c230223
        if(flowLayout_videos->sizeHint().width() < 0) {//c230223
            width_cnt = global.LmtCnt / 217;
        }else{
            width_cnt = global.LmtCnt / flowLayout_videos->sizeHint().width();//
        }
        int mod = this->video_widget_cnt % width_cnt;

        if(mod == 0){
            this->video_widget_cnt = width_cnt * 10;
        }
        else{
            this->video_widget_cnt = width_cnt * 10 + (width_cnt - mod);
        }
        // j220913 list count check

        int start_index = this->video_draw_cnt;
        int max_cnt = ((this->video_total_cnt - this->video_draw_cnt) > this->video_widget_cnt) ? this->video_widget_cnt : (this->video_total_cnt - this->video_draw_cnt);
        this->video_draw_cnt += max_cnt;

        for(int i = start_index; i < this->video_draw_cnt; i++){
            this->listVideo_all[i] = new bugs::ItemVideo_bugs(i, SECTION_FOR_MORE_POPUP___videos, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157);
            connect(this->listVideo_all[i], &bugs::ItemVideo_bugs::signal_clicked, this, &BugsVideoRecentlyAll::slot_clickedItemVideo);
        }

        for(int i = start_index; i < this->video_draw_cnt; i++){
            this->listVideo_all[i]->setData(this->list_video->at(i));
            this->flowLayout_videos->addWidget(this->listVideo_all[i]);

            QCoreApplication::processEvents();
        }
        ContentLoadingwaitingMsgHide();      //cheon Tidal
        this->flag_video_draw = false;


    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief video 목록 데이터를 화면에 보인다.
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void BugsVideoRecentlyAll::slot_applyResult_videoRecently(const QList<bugs::VideoItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){


        if(list_data.size() > 0){
            this->flag_lastPage_video = flag_lastPage;
            this->flagReqMore_video = false;//c220716

            if(this->list_video->size() == 0){
                this->jsonArr_videos_toPlay = QJsonArray();
                this->video_total_cnt = list_data.at(0).total_count;
            }

            int start_index = this->list_video->size();
            this->list_video->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_videos_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                int max_cnt = this->list_video->size();
                this->video_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->listVideo_all[i] = new bugs::ItemVideo_bugs(i, SECTION_FOR_MORE_POPUP___videos, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157);
                    connect(this->listVideo_all[i], &bugs::ItemVideo_bugs::signal_clicked, this, &BugsVideoRecentlyAll::slot_clickedItemVideo);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listVideo_all[i]->setData(this->list_video->at(i));
                    this->flowLayout_videos->addWidget(this->listVideo_all[i]);

                    QCoreApplication::processEvents();
                }

                //c230306_1-start
                int w = flowLayout_videos->sizeHint().width();
                int l = 80, r = 60, scrollbarW = 10, mod = 0;

                int mod_nn = (global.LmtCnt-l-r-scrollbarW)%(w + mod);
                int nn = (global.LmtCnt-l-r-scrollbarW)/(w + mod);
                //qDebug() << "global.LmtCnt=" << global.LmtCnt;
                //qDebug() << "this->width()=" << this->width();
                //qDebug() << "nn=" << nn;
                //qDebug() << "mod_nn=" << mod_nn;


                int i = 0;
                while(1){

                    mod_nn = (global.LmtCnt-l-r-scrollbarW)%(w + mod + i);
                    if(mod_nn > 20){

                        mod_nn = (global.LmtCnt-l-r-scrollbarW)%(w + ( mod +(i++)));
                    }else{
                        break;
                    }
                }
                //print_debug();
                //qDebug() << "w=" << w;
                //qDebug() << "i=" << i;
                //qDebug() << "this->width()=" << this->width();
                //this->resize(this->width()+1, this->height());
                flowLayout_videos->setSpacingHV(mod+i,20);
                //c230306_1-end

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_video_draw = false;
            }

            if(this->flag_lastPage_video == false){
                this->request_more_videoData();//c220716
            }
        }
        else{

            ContentLoadingwaitingMsgHide();      //cheon Tidal

            if(this->list_video->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Video_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->flowLayout_videos->addWidget(noData_widget);
            }
        }
    }


    /**
     * @brief [slot] override - Itemvideo 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void BugsVideoRecentlyAll::slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___videos){
            // ClickMode 별로 처리
            this->proc_clicked_itemVideo(this->list_video, this->jsonArr_videos_toPlay, clickMode, index, section);
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
    void BugsVideoRecentlyAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___videos){
            this->proc_clicked_optMorePopup_fromVideo(this->list_video, this->jsonArr_videos_toPlay, index, clickMode);
        }
    }





    /**
     * @brief QobuzMoodsMain::QobuzMoodsMain
     * @param parent
     */
    BugsVideoChartAll::BugsVideoChartAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // Data
        this->list_video = new QList<bugs::VideoItemData>();
        this->list_filterOpt_sort = AbstractBugsSubWidget::get_list_filterOptData_forMyCollection();

        if(global.user_forBugs.isLogined() == true){

            // 기본 UI 세팅
            this->setUIControl_videos();
        }
    }


    /**
     * @brief 소멸자.
     */
    BugsVideoChartAll::~BugsVideoChartAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsVideoChartAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        QString mainTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(this->page != pageCode){
            this->flagNeedReload = true;

            this->page = pageCode;

            this->label_mainTitle->setText(mainTitle);

            // init
            this->next_offset = 0;
            this->video_total_cnt = 0;
            this->video_draw_cnt = 0;

            this->list_video->clear();

            // request HTTP API
            this->flagReqMore_video = false;
            this->flag_lastPage_video = false;

            this->flag_video_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_videoData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void BugsVideoChartAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            GSCommon::clearLayout(this->flowLayout_videos);

           /*// init
            this->chooseFilterOpt->hideFilterBox();

            // init Data
            this->selected_filterCode = this->list_filterOpt_sort.first().opt_code;         // default

            QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
            QString def_name = this->list_filterOpt_sort.first().opt_name;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(def_code, def_name);*/

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
        }
    }


    // MARK : Create UI and Set --------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void BugsVideoChartAll::setUIControl_videos(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("videos");

        GSCommon::clearLayout(this->box_contents);      // Layout 초기화
        this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // layout for items
        this->flowLayout_videos = this->get_addUIControl_flowLayout(0, 20);
        GSCommon::clearLayout(this->flowLayout_videos);

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &BugsVideoChartAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyvideoAll::setUIControl_filter
     */
    void BugsVideoChartAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &BugsVideoChartAll::slot_filterClicked);

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
    void BugsVideoChartAll::initAll(){

        // init
        this->next_offset = 0;
        this->video_total_cnt = 0;
        this->video_draw_cnt = 0;

        this->list_video->clear();

        // request HTTP API
        this->flagReqMore_video = false;
        this->flag_lastPage_video = false;

        this->flag_video_draw = false;

        GSCommon::clearLayout(this->flowLayout_videos);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void BugsVideoChartAll::proc_wheelEvent_to_getMoreData(){

        if((this->video_total_cnt > this->video_draw_cnt) && (this->list_video->size() > this->video_draw_cnt) && (this->flag_video_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_video_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_videoDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void BugsVideoChartAll::resizeEvent(QResizeEvent *event){

        AbstractBugsSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }
    }



    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void BugsVideoChartAll::slot_filterClicked(){

        if(this->chooseFilterOpt->isVisible()){
            this->chooseFilterOpt->hideFilterBox();
        }
        else{
            // Filter Box Show
            this->chooseFilterOpt->clearFilter();
            this->chooseFilterOpt->addFilterGroup("정렬", this->list_filterOpt_sort, this->selected_filterCode);
            this->chooseFilterOpt->showFilterBox(QPoint(30, 130), this->size());
        }
    }

    /**
     * @brief 필터링 옵션박스 Widget에서 필터링을 하나 클릭한 경우이다.
     * @param selectedCode
     */
    void BugsVideoChartAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

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
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void BugsVideoChartAll::request_more_videoData(){

        if(!this->flagReqMore_video && !this->flag_lastPage_video){
            this->flagReqMore_video = true;

            // j220913 list count check
            int width_cnt = global.LmtCnt / 220;
            int mod = this->video_widget_cnt % width_cnt;

            if(mod == 0){
                this->video_widget_cnt = width_cnt * 10;
            }
            else{
                this->video_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
            // j220913 list count check

            // next_offset
            if(this->list_video->size() == 0){
                this->next_offset = 1;
            }
            else{
                this->next_offset++;
            }


            // filtering
            /*QString selected_sortOpt = this->selected_filterCode.toString();
            ProcCommon::DataOrderOption sortOpt = ProcCommon::DataOrderOption::NAME;                // 기본값
            ProcCommon::DataOrderDirection orderDirection = ProcCommon::DataOrderDirection::ASC;    // 기본값

            if(selected_sortOpt == QVariant::fromValue(ProcCommon::DataOrderOption::DATE).toString()){
                sortOpt = ProcCommon::DataOrderOption::DATE;
                orderDirection = ProcCommon::DataOrderDirection::DESC;
            }*/

            // request HTTP API
            ProcBugsAPI *proc_video_recently = new ProcBugsAPI(this);
            connect(proc_video_recently, &ProcBugsAPI::completeReq_list_videos, this, &BugsVideoChartAll::slot_applyResult_videoChart);
            proc_video_recently->request_bugs_getList_videos("chart/mv/day/20151",  this->video_widget_cnt, this->next_offset);

            if(this->next_offset == 0){
                this->flag_video_draw = true;
            }
        }
    }


    void BugsVideoChartAll::request_more_videoDraw(){

        // j220913 list count check
        int width_cnt = global.LmtCnt / 220;
        int mod = this->video_widget_cnt % width_cnt;

        if(mod == 0){
            this->video_widget_cnt = width_cnt * 10;
        }
        else{
            this->video_widget_cnt = width_cnt * 10 + (width_cnt - mod);
        }
        // j220913 list count check

        int start_index = this->video_draw_cnt;
        int max_cnt = ((this->video_total_cnt - this->video_draw_cnt) >  this->video_widget_cnt ) ?  this->video_widget_cnt : (this->video_total_cnt - this->video_draw_cnt);
        this->video_draw_cnt += max_cnt;

        for(int i = start_index; i < this->video_draw_cnt; i++){
            this->listVideo_all[i] = new bugs::ItemVideo_bugs(i, SECTION_FOR_MORE_POPUP___videos, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157);
            connect(this->listVideo_all[i], &bugs::ItemVideo_bugs::signal_clicked, this, &BugsVideoChartAll::slot_clickedItemVideo);
        }

        for(int i = start_index; i < this->video_draw_cnt; i++){
            this->listVideo_all[i]->setData(this->list_video->at(i));
            this->flowLayout_videos->addWidget(this->listVideo_all[i]);

            QCoreApplication::processEvents();
        }
        ContentLoadingwaitingMsgHide();
        this->flag_video_draw = false;
        //this->flagReqMore_video = false;//c220716
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief video 목록 데이터를 화면에 보인다.
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void BugsVideoChartAll::slot_applyResult_videoChart(const QList<bugs::VideoItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){


        if(list_data.size() > 0){
            this->flag_lastPage_video = flag_lastPage;
            this->flagReqMore_video = false;//c220716

            if(this->list_video->size() == 0){
                this->jsonArr_videos_toPlay = QJsonArray();
                this->video_total_cnt = list_data.at(0).total_count;
            }

            int start_index = this->list_video->size();
            this->list_video->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_videos_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                int max_cnt = this->list_video->size();
                this->video_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->listVideo_all[i] = new bugs::ItemVideo_bugs(i, SECTION_FOR_MORE_POPUP___videos, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157);
                    connect(this->listVideo_all[i], &bugs::ItemVideo_bugs::signal_clicked, this, &BugsVideoChartAll::slot_clickedItemVideo);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listVideo_all[i]->setData(this->list_video->at(i));
                    this->flowLayout_videos->addWidget(this->listVideo_all[i]);

                    QCoreApplication::processEvents();
                }
                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_video_draw = false;
            }


            this->request_more_videoData();
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            if(this->list_video->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Video_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->flowLayout_videos->addWidget(noData_widget);
            }
        }
    }


    /**
     * @brief [slot] override - Itemvideo 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void BugsVideoChartAll::slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___videos){
            // ClickMode 별로 처리
            this->proc_clicked_itemVideo(this->list_video, this->jsonArr_videos_toPlay, clickMode, index, section);
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
    void BugsVideoChartAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___videos){
            this->proc_clicked_optMorePopup_fromVideo(this->list_video, this->jsonArr_videos_toPlay, index, clickMode);
        }
    }
}
