#include "tidal/TidalMyVideoAll.h"

#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"
#include "tidal/ProcRosePlay_withTidal.h"

#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QResizeEvent>
#include <QScrollBar>


namespace tidal {

    const int SECTION_FOR_MORE_POPUP___video = 1;


    /**
     * @brief Video All List
     * @param parent
     */
    TidalMyVideoAll::TidalMyVideoAll(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_roseradio, parent) {

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


    void TidalMyVideoAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString curr_api_subPath = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(curr_api_subPath != this->api_subPath){
            this->flagNeedReload = true;

            this->api_subPath = curr_api_subPath;
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 화면 활성화
     */
    void TidalMyVideoAll::setActivePage(){

        if(this->flagNeedReload == true){

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

            // init
            this->next_offset = 0;
            this->video_total_cnt = 0;
            this->video_draw_cnt = 0;

            this->list_video->clear();

            // request HTTP API
            this->flag_lastPage_video = false;
            this->flagReqMore_video = false;

            this->flag_video_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_videoData();
        }
    }


    // MARK : Create UI and Set ------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void TidalMyVideoAll::setUIControl_video(){

        this->addUIControl_mainTitle("My Videos");

        // filtering
        this->setUIControl_filter();

        this->box_filter->removeWidget(this->widget_filter_contents);

        this->widget_filter_contents = new QWidget();
        this->widget_filter_contents->setStyleSheet("background:#212121; border:0px;");
        this->widget_filter_contents->setFixedSize(1500, 50);

        int width = 0;
        int left = 0;
        // All
        QPixmap pixmap_play(":/images/icon_list_allplay.png");
        QPushButton *btn_playAll = new QPushButton(this->widget_filter_contents);
        btn_playAll->setIcon(pixmap_play);
        btn_playAll->setIconSize(pixmap_play.rect().size());
        btn_playAll->setText("Play All");
        btn_playAll->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
        btn_playAll->setCursor(Qt::PointingHandCursor);

        width = btn_playAll->sizeHint().width();
        btn_playAll->setGeometry(left, 0, width, 50);
        connect(btn_playAll, &QPushButton::clicked, this, &TidalMyVideoAll::slot_btnClicked_playAll);

        // shuffle
        QPixmap pixmap_shuffle(":/images/icon_list_shuffle.png");
        QPushButton *btn_shuffleAll = new QPushButton(this->widget_filter_contents);
        btn_shuffleAll->setIcon(pixmap_shuffle);
        btn_shuffleAll->setIconSize(pixmap_shuffle.rect().size());
        btn_shuffleAll->setText("Shuffle Play");
        btn_shuffleAll->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
        btn_shuffleAll->setCursor(Qt::PointingHandCursor);

        left = width + 30;
        width = btn_shuffleAll->sizeHint().width();
        btn_shuffleAll->setGeometry(left, 0, width, 50);
        connect(btn_shuffleAll, &QPushButton::clicked, this, &TidalMyVideoAll::slot_btnClicked_playShuffle);

        this->box_filter->addWidget(this->widget_filter_contents);

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // layout for items
        this->flowLayout_video = this->get_addUIControl_flowLayout(0, 20);

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &TidalMyVideoAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief TidalMyVideoAll::setUIControl_filter
     */
    void TidalMyVideoAll::setUIControl_filter(){

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

        // init
        this->next_offset = 0;
        this->video_total_cnt = 0;
        this->video_draw_cnt = 0;

        this->list_video->clear();

        GSCommon::clearLayout(this->flowLayout_video);
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void TidalMyVideoAll::proc_wheelEvent_to_getMoreData(){

        //this->request_more_videoData();
        if((this->video_total_cnt > this->video_draw_cnt) && (this->flag_video_draw == false)
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

            // request HTTP API
            this->flag_lastPage_video = false;
            this->flagReqMore_video = false;

            this->flag_video_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
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

            // j220913 list count check
            int width_cnt = global.LmtCnt / 304;
            int mod = this->video_widget_cnt % width_cnt;
            int height_cnt = 0;

            if(mod == 0){
                if(width_cnt > 5){
                    height_cnt = 50 / width_cnt;
                    this->video_widget_cnt = width_cnt * height_cnt;
                }
                else{
                    this->video_widget_cnt = width_cnt * 10;
                }
            }
            else{
                if(width_cnt > 5){
                    height_cnt = 50 / width_cnt;
                    this->video_widget_cnt = width_cnt * height_cnt + (width_cnt - mod);
                }
                else{
                    this->video_widget_cnt = width_cnt * 10 + (width_cnt - mod);
                }
            }
            // j220913 list count check

            // next_offset
            this->next_offset = this->list_video->size();//cheon210704-list

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
            proc->request_tidal_getList_myFavoriteVideos(sortOpt, orderDirection, this->video_widget_cnt, next_offset);       // 최대 99까지

            if(this->next_offset == 0){
                this->flag_video_draw = true;
            }
        }
    }


    void TidalMyVideoAll::request_more_videoDraw(){

        // j220913 list count check
        int width_cnt = global.LmtCnt / 304;
        int mod = this->video_widget_cnt % width_cnt;
        int height_cnt = 0;

        if(mod == 0){
            if(width_cnt > 5){
                height_cnt = 50 / width_cnt;
                this->video_widget_cnt = width_cnt * height_cnt;
            }
            else{
                this->video_widget_cnt = width_cnt * 10;
            }
        }
        else{
            if(width_cnt > 5){
                height_cnt = 50 / width_cnt;
                this->video_widget_cnt = width_cnt * height_cnt + (width_cnt - mod);
            }
            else{
                this->video_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
        }
        // j220913 list count check

        int start_index = this->video_draw_cnt;
        int max_cnt = ((this->video_total_cnt - this->video_draw_cnt) > this->video_widget_cnt ) ? this->video_widget_cnt : (this->video_total_cnt - this->video_draw_cnt);
        this->video_draw_cnt += max_cnt;

        for(int i = start_index; i < this->video_draw_cnt; i++){
            this->listVideo_all[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___video, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157);
            connect(this->listVideo_all[i], &tidal::ItemVideo::signal_clicked, this, &TidalMyVideoAll::slot_clickedItemVideo);
        }

        for(int i = start_index; i < this->video_draw_cnt; i++){
            this->listVideo_all[i]->setData(this->list_video->at(i));
            this->flowLayout_video->addWidget(this->listVideo_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_video_draw = false;
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

        /*this->flag_lastPage_video = flag_lastPage;
        this->flagReqMore_video = false;

        int start_index = this->list_video->size();
        this->list_video->append(list_data);
        //this->createList_itemVideo_applyingWithData(list_data, AbstractItem::ImageSizeMode::Ractangle_251x139, this->flowLayout_video, start_index, SECTION_FOR_MORE_POPUP___video);
        this->createList_itemVideo_applyingWithData(list_data, AbstractItem::ImageSizeMode::Ractangle_284x157, this->flowLayout_video, start_index, SECTION_FOR_MORE_POPUP___video);*/

        if(list_data.size() > 0){
            this->flag_lastPage_video = flag_lastPage;
            this->flagReqMore_video = false;

            if(this->list_video->size() == 0){
                this->video_total_cnt = list_data.at(0).totalCount;
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            }

            int start_index = this->list_video->size();
            this->list_video->append(list_data);

            if(start_index == 0){
                int max_cnt = this->list_video->size();
                this->video_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->listVideo_all[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___video, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157);
                    connect(this->listVideo_all[i], &tidal::ItemVideo::signal_clicked, this, &TidalMyVideoAll::slot_clickedItemVideo);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listVideo_all[i]->setData(this->list_video->at(i));
                    this->flowLayout_video->addWidget(this->listVideo_all[i]);

                    QCoreApplication::processEvents();
                }

                this->flag_flow_draw = true;
                this->flag_video_draw = false;
            }

            ContentLoadingwaitingMsgHide();
            this->request_more_videoData();
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->list_video->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Video_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->flowLayout_video->addWidget(noData_widget);
            }
        }
    }


    void TidalMyVideoAll::slot_btnClicked_playAll(){

        QJsonArray tmpJsonArr = QJsonArray();
        for(int i = 0; i < this->list_video->size(); i++){
            QJsonObject tmpJsonObj = this->list_video->at(i).jsonObj_toPlay;
            tmpJsonArr.append(tmpJsonObj);
        }

        // 전체재생
        ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
        procRosePlay->requestPlayRose_byTracks(tmpJsonArr, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::JustPlay);
    }


    void TidalMyVideoAll::slot_btnClicked_playShuffle(){

        QJsonArray tmpJsonArr = QJsonArray();
        for(int i = 0; i < this->list_video->size(); i++){
            QJsonObject tmpJsonObj = this->list_video->at(i).jsonObj_toPlay;
            tmpJsonArr.append(tmpJsonObj);
        }

        // 셔플재생
        ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
        procRosePlay->requestPlayRose_byTracks(tmpJsonArr, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::ShufflePlay);
    }


    /**
     * @brief [slot] override - ItemVideo 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalMyVideoAll::slot_clickedItemVideo(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리 - Video 파트가 여러개임
        //this->proc_clicked_itemVideo(this->list_video, clickMode, index, section);
        if(section == SECTION_FOR_MORE_POPUP___video){

            if(clickMode == AbstractItem::ClickMode::PlayBtn || clickMode == AbstractItem::ClickMode::AllBox){

                int curr_clickMode = global.device.getMusicPlayType();

                OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                switch(curr_clickMode)
                {
                    case 13:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                        break;
                    case 12:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                        break;
                    case 15:
                        playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                        break;
                    case 16:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                        break;
                    case 17:
                        playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                        break;
                    case 18:
                        playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                        break;
                    case 19:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                        break;
                }

                QJsonArray tmpJsonArr = QJsonArray();
                for(int i = index; i < this->list_video->size(); i++){
                    QJsonObject tmpJsonObj = this->list_video->at(i).jsonObj_toPlay;
                    tmpJsonArr.append(tmpJsonObj);
                }

                // Rose Play 요청
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_videolist(tmpJsonArr, playType);
            }
            else{
                this->proc_clicked_itemVideo(this->list_video, clickMode, index, section);
            }
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
    void TidalMyVideoAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        /*if(section == SECTION_FOR_MORE_POPUP___video){
            this->proc_clicked_optMorePopup_fromVideo(this->list_video, index, clickMode);
        }*/
        if(section == SECTION_FOR_MORE_POPUP___video){
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last

                    )
            {
                if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                        || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                        || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last)
                {
                    QJsonArray tmpJsonArr = QJsonArray();
                    for(int i = index; i < this->list_video->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }

                    // Rose Play 요청
                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_videolist(tmpJsonArr, clickMode);
                }
                else{
                    QJsonObject tmpJsonObj = this->list_video->at(index).jsonObj_toPlay;

                    // Rose Play 요청
                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_video(tmpJsonObj, clickMode);
                }
            }
            else{
                this->proc_clicked_optMorePopup_fromVideo(this->list_video, index, clickMode);
            }
        }
    }
}
