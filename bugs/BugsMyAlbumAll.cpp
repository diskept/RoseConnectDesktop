#include "bugs/BugsMyAlbumAll.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/ProcBugsAPI.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QResizeEvent>
#include <QScrollBar>


namespace bugs {

    const int SECTION_FOR_MORE_POPUP___albums = 1;

    /**
     * @brief BugsMyalbumAll::BugsMyalbumAll
     * @param parent
     */
    BugsMyAlbumlistAll::BugsMyAlbumlistAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        this->linker = Linker::getInstance();

        // Data
        this->list_Myalbum = new QList<bugs::MyAlbumItemData>();

        if(global.user_forBugs.isLogined() == true){

            // 기본 UI 세팅
            this->setUIControl_album();
        }
    }


    /**
     * @brief 소멸자.
     */
    BugsMyAlbumlistAll::~BugsMyAlbumlistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsMyAlbumlistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        QString mainTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(pageCode != this->page || mainTitle != this->title){
            this->flagNeedReload = true;

            this->page = pageCode;
            this->title = mainTitle;

            this->label_mainTitle->setText(this->title);

            // init
            this->next_offset = 0;
            this->album_total_cnt = 0;
            this->album_draw_cnt = 0;

            // init data
            this->list_Myalbum->clear();

            // request HTTP API
            this->flagReqMore_album = false;
            this->flag_lastPage_album = false;

            this->flag_album_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void BugsMyAlbumlistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            GSCommon::clearLayout(this->box_contents);      // Layout 초기화

            this->box_contents->setAlignment(Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // layout for items
            this->flowLayout_albums = this->get_addUIControl_flowLayout(0 , 20);
            GSCommon::clearLayout(this->flowLayout_albums);

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
        }
    }


    /**
     * @brief BugsMyalbumAll::setUIControll_basic
     */
    void BugsMyAlbumlistAll::setUIControl_album(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("내 앨범");

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        bugs::ItemMyAlbum_bugs *listAlbum = new bugs::ItemMyAlbum_bugs(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200);

        this->album_widget_width = listAlbum->get_fixedWidth();
        this->album_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;

        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
    }


    /**
     * @brief 스크롤 Bottom에 대하여 Get more data 처리
     */
    void BugsMyAlbumlistAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_album && !this->flag_lastPage_album) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_album = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumData();
        }
        else if((this->album_total_cnt > this->album_draw_cnt) && this->flag_album_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
    }


    // MARK : QobuzalbumAll HTTP API ---------------------------------------------------------------------------------

    /**
     * @brief Qobuz API 요청 - 사용자가 생성한 album
     */
    void BugsMyAlbumlistAll::request_more_albumData(){

        // next_offset
        if(this->list_Myalbum->size() == 0){
            this->next_offset = 1;
        }
        else{
            this->next_offset++;
        }

        // request HTTP API
        ProcBugsAPI *proc_my = new ProcBugsAPI(this);
        connect(proc_my, &ProcBugsAPI::completeReq_list_my_albums, this, &BugsMyAlbumlistAll::slot_applyResult_my_albums);
        proc_my->request_bugs_getList_my_albums(200, this->next_offset);

        if(this->next_offset == 0){
            this->flag_album_draw = true;
        }
    }


    void BugsMyAlbumlistAll::request_more_albumDraw(){

        this->flag_album_draw = true;

        int f_width = this->flowLayout_albums->geometry().width();

        if(this->flowLayout_albums->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->album_widget_width + this->album_widget_margin);
        int f_mod = this->album_draw_cnt % f_wg_cnt;

        if(f_mod == 0){
            this->album_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->album_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        //qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

        int start_index = this->album_draw_cnt;
        int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
        this->album_draw_cnt += max_cnt;


        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->myCollection_album_myAlbum[i] = new bugs::ItemMyAlbum_bugs(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->myCollection_album_myAlbum[i]->setData(this->list_Myalbum->at(i));
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->flowLayout_albums->addWidget(this->myCollection_album_myAlbum[i]);
            connect(this->myCollection_album_myAlbum[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &BugsMyAlbumlistAll::slot_clickedItemAlbum);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_album_draw = false;
    }


    void BugsMyAlbumlistAll::slot_applyResult_my_albums(const QList<bugs::MyAlbumItemData> &list_data, const bool flag_lastPage){

        if(list_data.length() > 0)
        {
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_Myalbum->size() == 0){
                this->album_total_cnt = list_data.at(0).total_count;
            }

            int start_index = this->list_Myalbum->size();
            this->list_Myalbum->append(list_data);

            if(start_index == 0){
                int f_width = this->flowLayout_albums->geometry().width();

                if(this->flowLayout_albums->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                int f_wg_cnt = f_width / (this->album_widget_width + this->album_widget_margin);
                int f_mod = this->album_draw_cnt % f_wg_cnt;

                if(f_mod == 0){
                    this->album_widget_cnt = f_wg_cnt * 10;
                }
                else{
                    this->album_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                }

                //qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

                int start_index = this->album_draw_cnt;
                int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
                this->album_draw_cnt += max_cnt;

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->myCollection_album_myAlbum[i] = new bugs::ItemMyAlbum_bugs(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->myCollection_album_myAlbum[i]->setData(this->list_Myalbum->at(i));
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->flowLayout_albums->addWidget(this->myCollection_album_myAlbum[i]);
                    connect(this->myCollection_album_myAlbum[i], &bugs::ItemMyAlbum_bugs::signal_clicked, this, &BugsMyAlbumlistAll::slot_clickedItemAlbum);
                }

                this->setFlowLayoutResize(this,this->flowLayout_albums,this->album_widget_width,this->album_widget_margin);

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }
            else{
                this->request_more_albumDraw();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            if(this->list_Myalbum->size() <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_albums->geometry().width();

                if(this->flowLayout_albums->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_albums->addWidget(noData_widget);
            }
        }
    }


    /**
     * @brief BugsMyCollection::slot_clickedItemMyAlbum
     * @param clickMode
     */
    void BugsMyAlbumlistAll::slot_clickedItemMyAlbum(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemMyAlbum(this->list_Myalbum, clickMode, index, section);
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
    void BugsMyAlbumlistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromMyAlbum(this->list_Myalbum, index, clickMode);
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void BugsMyAlbumlistAll::resizeEvent(QResizeEvent *event){

        Q_UNUSED(event);

        this->setFlowLayoutResize(this, this->flowLayout_albums, this->album_widget_width, this->album_widget_margin);
    }





    /**
     * @brief QobuzMoodsMain::QobuzMoodsMain
     * @param parent
     */
    BugsRecentlyAlbumAll::BugsRecentlyAlbumAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        this->linker = Linker::getInstance();

        // Data
        this->list_album = new QList<roseHome::AlbumItemData>();
        this->list_filterOpt_sort = AbstractBugsSubWidget::get_list_filterOptData_forMyCollection();

        if(global.user_forBugs.isLogined() == true){
            // 기본 UI 세팅
            this->setUIControl_albums();
        }
    }


    /**
     * @brief 소멸자.
     */
    BugsRecentlyAlbumAll::~BugsRecentlyAlbumAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsRecentlyAlbumAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        QString mainTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(this->page != pageCode || this->title != mainTitle){
            this->flagNeedReload = true;

            this->page = pageCode;
            this->title = mainTitle;

            this->label_mainTitle->setText(this->title);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

            // init
            this->next_offset = 0;
            this->album_total_cnt = 0;
            this->album_draw_cnt = 0;

            this->list_album->clear();

            // request HTTP API
            this->flagReqMore_album = false;
            this->flag_lastPage_album = false;

            this->flag_album_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_albumData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_albums, this, &BugsRecentlyAlbumAll::slot_applyResult_albumlistCheck);
            proc->request_rose_getList_recentlyAlbums("member/album/recent", "BUGS", 0, 10);
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void BugsRecentlyAlbumAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            GSCommon::clearLayout(this->box_contents);      // Layout 초기화

            this->box_contents->setAlignment(Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // layout for items
            this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
            GSCommon::clearLayout(this->flowLayout_albums);

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
    void BugsRecentlyAlbumAll::setUIControl_albums(){

        GSCommon::clearLayout(this->box_mainTitle);

        this->widget_mainTitle = new QWidget();
        this->widget_mainTitle->setStyleSheet("background-color:#171717;");
        this->widget_mainTitle->setMinimumSize(800, 69);
        this->widget_mainTitle->resize(global.LmtCnt, 69);

        this->label_mainTitle = new QLabel(this->widget_mainTitle);
        this->label_mainTitle->setStyleSheet("background-color:transparent; font-size:26px; color:#FFFFFF;");
        this->label_mainTitle->setText("");
        this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

        this->btn_mainTitle = new QPushButton(this->widget_mainTitle);
        this->btn_mainTitle->setObjectName("btn_mainTitle");
        this->btn_mainTitle->setCursor(Qt::PointingHandCursor);
        this->btn_mainTitle->setStyleSheet("background-color:transparent;");

        connect(this->btn_mainTitle, &QPushButton::clicked, this, &BugsRecentlyAlbumAll::slot_btnClicked_Delete);

        this->label_delete_icon = GSCommon::getUILabelImg(":/images/q_del_ico.png", this->btn_mainTitle);
        this->label_delete_icon->setGeometry(0, 10, 50, 50);

        this->label_delete = new QLabel(this->btn_mainTitle);
        this->label_delete->setText(tr("Delete"));
        this->label_delete->setStyleSheet("background-color:transparent;font-size:20px;color:#FFFFFF;");
        this->label_delete->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        this->label_delete->setGeometry(60, 23, this->label_delete->sizeHint().width(), this->label_delete->sizeHint().height());

        this->btn_mainTitle->setGeometry(global.LmtCnt - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);

        this->box_mainTitle->addWidget(this->widget_mainTitle);

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        roseHome::ItemAlbum_rosehome *listAlbum = new roseHome::ItemAlbum_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);

        this->album_widget_width = listAlbum->get_fixedWidth();
        this->album_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeAlbumListAll::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;

        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);

        // filtering
//        this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &BugsRecentlyAlbumAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyAlbumAll::setUIControl_filter
     */
    void BugsRecentlyAlbumAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &BugsRecentlyAlbumAll::slot_filterClicked);

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
    void BugsRecentlyAlbumAll::initAll(){

        // init
        this->next_offset = 0;
        this->album_total_cnt = 0;
        this->album_draw_cnt = 0;

        this->list_album->clear();

        // request HTTP API
        this->flagReqMore_album = false;
        this->flag_lastPage_album = false;

        this->flag_album_draw = false;

        GSCommon::clearLayout(this->flowLayout_albums);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void BugsRecentlyAlbumAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_album && !this->flag_lastPage_album) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_album = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumData();
        }
        else if((this->album_total_cnt > this->album_draw_cnt) && this->flag_album_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void BugsRecentlyAlbumAll::resizeEvent(QResizeEvent *event){

        AbstractBugsSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }

        this->setFlowLayoutResize(this, this->flowLayout_albums, this->album_widget_width, this->album_widget_margin);

        this->widget_mainTitle->resize(global.LmtCnt, 69);
        this->btn_mainTitle->setGeometry(global.LmtCnt - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);
    }



    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void BugsRecentlyAlbumAll::slot_filterClicked(){

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
    void BugsRecentlyAlbumAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

        // 코드가 다른 경우에 대해서만 처리한다.
        if(this->selected_filterCode != selected_filterCode){

            // update
            this->selected_filterCode = selected_filterCode;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(selected_filterCode.toString(), selected_filterName);

            // clear - 옵션코드가 바뀌었으므로 모두 초기화한다.
            this->initAll();

            this->scrollTop();
            this->request_more_albumData();

        }

    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void BugsRecentlyAlbumAll::request_more_albumData(){

        // next_offset
        if(this->list_album->size() == 0){
            this->next_offset = 0;
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
        roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
        connect(proc, &roseHome::ProcCommon::completeReq_list_albums, this, &BugsRecentlyAlbumAll::slot_applyResult_myAlbums_rose);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_recentlyAlbums("member/album/recent", "BUGS", this->next_offset, 200);

        if(this->next_offset == 0){
            this->flag_album_draw = true;
        }

    }


    void BugsRecentlyAlbumAll::request_more_albumDraw(){

        this->flag_album_draw = true;

        int f_width = this->flowLayout_albums->geometry().width();

        if(this->flowLayout_albums->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->album_widget_width + this->album_widget_margin);
        int f_mod = this->album_draw_cnt % f_wg_cnt;

        if(f_mod == 0){
            this->album_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->album_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        //qDebug() << "[Debug] RoseHomeAlbumListAll::request_more_albumDraw " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

        int start_index = this->album_draw_cnt;
        int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
        this->album_draw_cnt += max_cnt;


        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i]->setData(this->list_album->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->flowLayout_albums->addWidget(this->listAlbum_all[i]);
            connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &BugsRecentlyAlbumAll::slot_clickedItemAlbum);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_album_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief Album 목록 데이터를 화면에 보인다.
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void BugsRecentlyAlbumAll::slot_applyResult_myAlbums_rose(const QList<roseHome::AlbumItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        int idx = sender()->property("idx").toInt();

        if(idx != this->next_offset){
            return;
        }

        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_album->size() == 0){
                this->album_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_album->size();
            this->list_album->append(list_data);

            if(start_index == 0){
                int f_width = this->flowLayout_albums->geometry().width();

                if(this->flowLayout_albums->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                int f_wg_cnt = f_width / (this->album_widget_width + this->album_widget_margin);
                int f_mod = this->album_draw_cnt % f_wg_cnt;

                if(f_mod == 0){
                    this->album_widget_cnt = f_wg_cnt * 10;
                }
                else{
                    this->album_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                }

                //qDebug() << "[Debug] RoseHomeAlbumListAll::slot_applyResult_albums " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

                int start_index = this->album_draw_cnt;
                int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
                this->album_draw_cnt += max_cnt;

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->listAlbum_all[i]->setData(this->list_album->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->flowLayout_albums->addWidget(this->listAlbum_all[i]);
                    connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &BugsRecentlyAlbumAll::slot_clickedItemAlbum);
                }

                this->setFlowLayoutResize(this,this->flowLayout_albums,this->album_widget_width,this->album_widget_margin);

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }
            else{
                this->request_more_albumDraw();
            }

            /*if(this->flag_lastPage_album == false){
                this->request_more_albumData();
            }*/
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            if(this->list_album->size() <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_albums->geometry().width();

                if(this->flowLayout_albums->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_albums->addWidget(noData_widget);
            }
        }
    }


    void BugsRecentlyAlbumAll::slot_applyResult_albumlistCheck(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_album->count() == 0){
                QString tmpTitle = "최근 재생 앨범" + QString(" (%1)").arg(list_data.at(0).totalCount);
                this->label_mainTitle->setText(tmpTitle);
                this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                this->initAll();

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                this->request_more_albumData();

                this->flagNeedReload = true;
                this->setActivePage();
            }
            else{
                for(int i = 0; i < list_data.count(); i++){
                    if(list_data.at(i).title != this->list_album->at(i).title){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_album->at(0).totalCount) || (change_flag > 0)){

                    QString tmpTitle = "최근 재생 앨범" + QString(" (%1)").arg(list_data.at(0).totalCount);
                    this->label_mainTitle->setText(tmpTitle);
                    this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                    this->initAll();

                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                    this->request_more_albumData();

                    this->flagNeedReload = true;
                    this->setActivePage();
                }
                else{
                    ContentLoadingwaitingMsgHide();
                }
            }
        }
        else if(list_data.size() == 0){

            QString tmpTitle = "최근 재생 앨범 (0)";
            this->label_mainTitle->setText(tmpTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

            this->initAll();

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_albumData();

            this->flagNeedReload = true;
            this->setActivePage();
        }
        else{
            ContentLoadingwaitingMsgHide();
        }
    }


    void BugsRecentlyAlbumAll::slot_btnClicked_Delete(){

        QStringList tmpTitle = this->title.split("(");

        QJsonObject tmpObj;
        tmpObj.insert("contents_type", tmpTitle.at(0));
        tmpObj.insert("list_type", "ALBUM");
        tmpObj.insert("api_subPath", "member/album/recent");
        tmpObj.insert("filter_type", "BUGS");

        QJsonObject tmpObj_move;
        tmpObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_RECENTLY_LIST_DELETE);
        tmpObj_move.insert(KEY_DATA, tmpObj);

        emit this->linker->signal_clickedMovePage(tmpObj_move);
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void BugsRecentlyAlbumAll::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___albums){
            roseHome::AlbumItemData data = this->list_album->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Album 바로 재생
                bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byAlbumID(data.clientKey.toInt(), OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Album Detail 페이지 진입
                QJsonObject jsonObj_move = QJsonObject();
                jsonObj_move.insert("album_id", data.clientKey.toInt());
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ALBUM_DETAIL);

                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptionPopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
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
    void BugsRecentlyAlbumAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        Q_UNUSED(clickMode);
        Q_UNUSED(index);

        if(section == SECTION_FOR_MORE_POPUP___albums){
            //this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
            roseHome::AlbumItemData data = this->list_album->at(index);

            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
                    )
            {
                // Rose Play 요청
                bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byAlbumID(data.clientKey.toInt(), clickMode);
            }
            else if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;
            }
        }
    }





    /**
     * @brief BugsAlbumHistoryAll::RoseHomeAlbumListAll
     * @param parent
     */
    BugsAlbumHistoryAll::BugsAlbumHistoryAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // Data
        this->list_album = new QList<roseHome::AlbumItemData>();

        // 기본 UI 세팅
        this->setUIControl_albums();
    }

    /**
     * @brief 소멸자.
     */
    BugsAlbumHistoryAll::~BugsAlbumHistoryAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsAlbumHistoryAll::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::PageInfo_AlbumAllView tmp_pageInfo = roseHome::ConvertData::convertData_pageInfo_albumAllView(jsonObj);
        this->flagNeedReload = false;

        if(tmp_pageInfo.api_subPath != this->data_pageInfo.api_subPath || tmp_pageInfo.pathTitle != this->data_pageInfo.pathTitle || tmp_pageInfo.filter_type != this->data_pageInfo.filter_type){
            this->flagNeedReload = true;

            this->data_pageInfo = tmp_pageInfo;

            this->label_mainTitle->setText(this->data_pageInfo.pathTitle);

            // init
            this->next_offset = 0;
            this->album_total_cnt = 0;
            this->album_draw_cnt = 0;

            this->list_album->clear();

            // request HTTP API
            this->flagReqMore_album = false;
            this->flag_lastPage_album = false;

            this->flag_album_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void BugsAlbumHistoryAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            // init
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // layout for items
            this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
            GSCommon::clearLayout(this->flowLayout_albums);

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void BugsAlbumHistoryAll::setUIControl_albums(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("");

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        roseHome::ItemAlbum_rosehome *listAlbum = new roseHome::ItemAlbum_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);

        this->album_widget_width = listAlbum->get_fixedWidth();
        this->album_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeAlbumListAll::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;


        // layout for items
        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
    }

    /**
     * @brief 스크롤 Bottom에 대하여 Get more data 처리
     */
    void BugsAlbumHistoryAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_album && !this->flag_lastPage_album) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_album = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumData();
        }
        else if((this->album_total_cnt > this->album_draw_cnt) && this->flag_album_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
    }

    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void BugsAlbumHistoryAll::resizeEvent(QResizeEvent *event){

        AbstractBugsSubWidget::resizeEvent(event);

        this->setFlowLayoutResize(this, this->flowLayout_albums, this->album_widget_width, this->album_widget_margin);
    }

    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Album 데이터를 더 요청한다.
     * @param page
     */
    void BugsAlbumHistoryAll::request_more_albumData(){

        // next_offset
        if(this->list_album->size() == 0){
            this->next_offset = 0;
        }
        else{
            this->next_offset++;
        }


        // request HTTP API
        roseHome::ProcCommon *proc_album = new roseHome::ProcCommon(this);
        connect(proc_album, &roseHome::ProcCommon::completeReq_list_albums, this, &BugsAlbumHistoryAll::slot_applyResult_historyAlbum);
        proc_album->setProperty("idx", this->next_offset);
        proc_album->request_rose_getList_historyAlbums(this->data_pageInfo.api_subPath, this->data_pageInfo.filter_type, this->next_offset, 200);

        if(this->next_offset == 0){
            this->flag_album_draw = true;
        }
    }

    void BugsAlbumHistoryAll::request_more_albumDraw(){

        this->flag_album_draw = true;

        int f_width = this->flowLayout_albums->geometry().width();

        if(this->flowLayout_albums->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->album_widget_width + this->album_widget_margin);
        int f_mod = this->album_draw_cnt % f_wg_cnt;

        if(f_mod == 0){
            this->album_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->album_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        //qDebug() << "[Debug] RoseHomeAlbumListAll::request_more_albumDraw " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

        int start_index = this->album_draw_cnt;
        int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
        this->album_draw_cnt += max_cnt;


        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i]->setData(this->list_album->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->flowLayout_albums->addWidget(this->listAlbum_all[i]);
            connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &BugsAlbumHistoryAll::slot_clickedItemAlbum);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_album_draw = false;
    }

    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] [Override]
     * @param list_data
     */
    void BugsAlbumHistoryAll::slot_applyResult_historyAlbum(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        int idx = sender()->property("idx").toInt();

        if(idx != this->next_offset){
            return;
        }

        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_album->size() == 0){
                this->album_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_album->size();
            this->list_album->append(list_data);

            if(start_index == 0){
                int f_width = this->flowLayout_albums->geometry().width();

                if(this->flowLayout_albums->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                int f_wg_cnt = f_width / (this->album_widget_width + this->album_widget_margin);
                int f_mod = this->album_draw_cnt % f_wg_cnt;

                if(f_mod == 0){
                    this->album_widget_cnt = f_wg_cnt * 10;
                }
                else{
                    this->album_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                }

                int start_index = this->album_draw_cnt;
                int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
                this->album_draw_cnt += max_cnt;

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->listAlbum_all[i]->setData(this->list_album->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->flowLayout_albums->addWidget(this->listAlbum_all[i]);
                    connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &BugsAlbumHistoryAll::slot_clickedItemAlbum);
                }

                this->setFlowLayoutResize(this,this->flowLayout_albums,this->album_widget_width,this->album_widget_margin);

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }
            else{
                this->request_more_albumDraw();
            }


            /*if(flag_lastPage == false){
                this->request_more_albumData();
            }*/
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->list_album->size() <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_albums->geometry().width();

                if(this->flowLayout_albums->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_albums->addWidget(noData_widget);
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void BugsAlbumHistoryAll::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        //this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
        if(section == SECTION_FOR_MORE_POPUP___albums){
            roseHome::AlbumItemData data = this->list_album->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Album 바로 재생
                ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byAlbumID(data.clientKey.toUInt(), OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Album Detail 페이지 진입
                QJsonObject jsonObj_move = QJsonObject();
                jsonObj_move.insert("album_id", data.clientKey.toInt());
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ALBUM_DETAIL);

                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptionPopup 띄우기 필요
                print_debug();

                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
            }
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void BugsAlbumHistoryAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        Q_UNUSED(clickMode);
        Q_UNUSED(index);

        if(section == SECTION_FOR_MORE_POPUP___albums){
            //this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
            if(clickMode == OptMorePopup::ClickMode::Share){
                setUIShare();
            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                // Rose Play 요청
                bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byAlbumID(this->list_album->at(index).clientKey.toInt(), clickMode);
            }
        }
    }





    /**
     * @brief QobuzMoodsMain::QobuzMoodsMain
     * @param parent
     */
    BugsAlbumRecentlyAll::BugsAlbumRecentlyAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // Data
        this->list_album = new QList<bugs::AlbumItemData>();
        this->list_filterOpt_sort = AbstractBugsSubWidget::get_list_filterOptData_forMyCollection();

        if(global.user_forBugs.isLogined() == true){

            // 기본 UI 세팅
            this->setUIControl_albums();
        }
    }


    /**
     * @brief 소멸자.
     */
    BugsAlbumRecentlyAll::~BugsAlbumRecentlyAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsAlbumRecentlyAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        QString mainTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(this->page != pageCode){
            this->flagNeedReload = true;

            this->page = pageCode;

            this->label_mainTitle->setText(mainTitle);

            // init
            this->next_offset = 0;
            this->album_total_cnt = 0;
            this->album_draw_cnt = 0;

            this->list_album->clear();

            // request HTTP API
            this->flagReqMore_album = false;
            this->flag_lastPage_album = false;

            this->flag_album_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_albumData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void BugsAlbumRecentlyAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            GSCommon::clearLayout(this->flowLayout_albums);

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
    void BugsAlbumRecentlyAll::setUIControl_albums(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Albums");

        GSCommon::clearLayout(this->box_contents);      // Layout 초기화
        this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        // layout for items
        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
        GSCommon::clearLayout(this->flowLayout_albums);

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &BugsAlbumRecentlyAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyAlbumAll::setUIControl_filter
     */
    void BugsAlbumRecentlyAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &BugsAlbumRecentlyAll::slot_filterClicked);

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
    void BugsAlbumRecentlyAll::initAll(){

        // init
        this->next_offset = 0;
        this->album_total_cnt = 0;
        this->album_draw_cnt = 0;

        this->list_album->clear();

        // request HTTP API
        this->flagReqMore_album = false;
        this->flag_lastPage_album = false;

        this->flag_album_draw = false;

        GSCommon::clearLayout(this->flowLayout_albums);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void BugsAlbumRecentlyAll::proc_wheelEvent_to_getMoreData(){

        if((this->album_total_cnt > this->album_draw_cnt) && (this->list_album->size() > this->album_draw_cnt) && (this->flag_album_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_album_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void BugsAlbumRecentlyAll::resizeEvent(QResizeEvent *event){//c230223

        AbstractBugsSubWidget::resizeEvent(event);
        int w = flowLayout_albums->sizeHint().width();
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

        flowLayout_albums->setSpacingHV(0+i,20);
        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }
    }



    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void BugsAlbumRecentlyAll::slot_filterClicked(){

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
    void BugsAlbumRecentlyAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

        // 코드가 다른 경우에 대해서만 처리한다.
        if(this->selected_filterCode != selected_filterCode){

            // update
            this->selected_filterCode = selected_filterCode;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(selected_filterCode.toString(), selected_filterName);

            // clear - 옵션코드가 바뀌었으므로 모두 초기화한다.
            this->initAll();

            this->scrollTop();
            this->request_more_albumData();

        }

    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void BugsAlbumRecentlyAll::request_more_albumData(){

        if(!this->flagReqMore_album && !this->flag_lastPage_album){
            this->flagReqMore_album = true;

            // j220913 list count check
            int width_cnt;//c230223
            if(flowLayout_albums->sizeHint().width() < 0) {//c230223
                width_cnt = global.LmtCnt / 217;
            }else{
                width_cnt = global.LmtCnt / flowLayout_albums->sizeHint().width();//
            }
            int mod = this->album_draw_cnt % width_cnt;

            if(mod == 0){
                this->album_widget_cnt = width_cnt * 10;
            }
            else{
                this->album_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
            // j220913 list count check

            // next_offset
            if(this->list_album->size() == 0){
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
            ProcBugsAPI *proc_album_recently = new ProcBugsAPI(this);
            connect(proc_album_recently, &ProcBugsAPI::completeReq_list_albums, this, &BugsAlbumRecentlyAll::slot_applyResult_albumRecently);
            proc_album_recently->request_bugs_getList_albums("genre/album/total/all", this->album_widget_cnt, this->next_offset);

            if(this->next_offset == 0){
                this->flag_album_draw = true;
            }
        }
    }


    void BugsAlbumRecentlyAll::request_more_albumDraw(){

        // j220913 list count check
        int width_cnt = global.LmtCnt / 220;
        int mod = this->album_draw_cnt % width_cnt;

        if(mod == 0){
            this->album_widget_cnt = width_cnt * 10;
        }
        else{
            this->album_widget_cnt = width_cnt * 10 + (width_cnt - mod);
        }
        // j220913 list count check

        int start_index = this->album_draw_cnt;
        int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
        this->album_draw_cnt += max_cnt;

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i] = new bugs::ItemAlbum_bugs(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->listAlbum_all[i], &bugs::ItemAlbum_bugs::signal_clicked, this, &BugsAlbumRecentlyAll::slot_clickedItemAlbum);
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i]->setData(this->list_album->at(i));
            this->flowLayout_albums->addWidget(this->listAlbum_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_album_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief Album 목록 데이터를 화면에 보인다.
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void BugsAlbumRecentlyAll::slot_applyResult_albumRecently(const QList<bugs::AlbumItemData>& list_data, const bool flag_lastPage){


        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_album->size() == 0){
                this->album_total_cnt = list_data.at(0).total_count;
                if(this->album_total_cnt > 50){
                    this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
                }
            }

            int start_index = this->list_album->size();
            this->list_album->append(list_data);

            if(start_index == 0){
                int max_cnt = this->list_album->size();
                this->album_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i] = new bugs::ItemAlbum_bugs(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->listAlbum_all[i], &bugs::ItemAlbum_bugs::signal_clicked, this, &BugsAlbumRecentlyAll::slot_clickedItemAlbum);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i]->setData(this->list_album->at(i));
                    this->flowLayout_albums->addWidget(this->listAlbum_all[i]);

                    QCoreApplication::processEvents();
                }

                ContentLoadingwaitingMsgHide();      //cheon Tidal

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }


            this->request_more_albumData();
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            if(this->list_album->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->flowLayout_albums->addWidget(noData_widget);
            }
        }
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void BugsAlbumRecentlyAll::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___albums){
            // ClickMode 별로 처리
            this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
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
    void BugsAlbumRecentlyAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
    }





    /**
     * @brief QobuzMoodsMain::QobuzMoodsMain
     * @param parent
     */
    BugsAlbumChartAll::BugsAlbumChartAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // Data
        this->list_album = new QList<bugs::AlbumItemData>();
        this->list_filterOpt_sort = AbstractBugsSubWidget::get_list_filterOptData_forMyCollection();

        if(global.user_forBugs.isLogined() == true){

            // 기본 UI 세팅
            this->setUIControl_albums();
        }
    }


    /**
     * @brief 소멸자.
     */
    BugsAlbumChartAll::~BugsAlbumChartAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsAlbumChartAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        QString mainTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(this->page != pageCode){
            this->flagNeedReload = true;

            this->page = pageCode;

            this->label_mainTitle->setText(mainTitle);

            // init
            this->next_offset = 0;
            this->album_total_cnt = 0;
            this->album_draw_cnt = 0;

            this->list_album->clear();

            // request HTTP API
            this->flagReqMore_album = false;
            this->flag_lastPage_album = false;

            this->flag_album_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_albumData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void BugsAlbumChartAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            GSCommon::clearLayout(this->flowLayout_albums);

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
    void BugsAlbumChartAll::setUIControl_albums(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Albums");

        GSCommon::clearLayout(this->box_contents);      // Layout 초기화
        this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        // layout for items
        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
        GSCommon::clearLayout(this->flowLayout_albums);

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &BugsAlbumChartAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyAlbumAll::setUIControl_filter
     */
    void BugsAlbumChartAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &BugsAlbumChartAll::slot_filterClicked);

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
    void BugsAlbumChartAll::initAll(){

        // init
        this->next_offset = 0;
        this->album_total_cnt = 0;
        this->album_draw_cnt = 0;

        this->list_album->clear();

        // request HTTP API
        this->flagReqMore_album = false;
        this->flag_lastPage_album = false;

        this->flag_album_draw = false;

        GSCommon::clearLayout(this->flowLayout_albums);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void BugsAlbumChartAll::proc_wheelEvent_to_getMoreData(){

        if((this->album_total_cnt > this->album_draw_cnt) && (this->list_album->size() > this->album_draw_cnt) && (this->flag_album_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_album_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void BugsAlbumChartAll::resizeEvent(QResizeEvent *event){

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
    void BugsAlbumChartAll::slot_filterClicked(){

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
    void BugsAlbumChartAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

        // 코드가 다른 경우에 대해서만 처리한다.
        if(this->selected_filterCode != selected_filterCode){

            // update
            this->selected_filterCode = selected_filterCode;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(selected_filterCode.toString(), selected_filterName);

            // clear - 옵션코드가 바뀌었으므로 모두 초기화한다.
            this->initAll();

            this->scrollTop();
            this->request_more_albumData();

        }

    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void BugsAlbumChartAll::request_more_albumData(){

        if(!this->flagReqMore_album && !this->flag_lastPage_album){
            this->flagReqMore_album = true;

            // j220913 list count check
            int width_cnt = global.LmtCnt / 220;
            int mod = this->album_draw_cnt % width_cnt;

            if(mod == 0){
                this->album_widget_cnt = width_cnt * 10;
            }
            else{
                this->album_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
            // j220913 list count check

            // next_offset
            if(this->list_album->size() == 0){
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
            ProcBugsAPI *proc_album_recently = new ProcBugsAPI(this);
            connect(proc_album_recently, &ProcBugsAPI::completeReq_list_albums, this, &BugsAlbumChartAll::slot_applyResult_albumChart);
            proc_album_recently->request_bugs_getList_albums("chart/album/day/20151", this->album_widget_cnt, this->next_offset);

            if(this->next_offset == 0){
                this->flag_album_draw = true;
            }
        }
    }


    void BugsAlbumChartAll::request_more_albumDraw(){

        // j220913 list count check
        int width_cnt = global.LmtCnt / 220;
        int mod = this->album_draw_cnt % width_cnt;

        if(mod == 0){
            this->album_widget_cnt = width_cnt * 10;
        }
        else{
            this->album_widget_cnt = width_cnt * 10 + (width_cnt - mod);
        }
        // j220913 list count check

        int start_index = this->album_draw_cnt;
        int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
        this->album_draw_cnt += max_cnt;

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i] = new bugs::ItemAlbum_bugs(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->listAlbum_all[i], &bugs::ItemAlbum_bugs::signal_clicked, this, &BugsAlbumChartAll::slot_clickedItemAlbum);
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i]->setData(this->list_album->at(i));
            this->flowLayout_albums->addWidget(this->listAlbum_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();      //cheon Tidal
        this->flag_album_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief Album 목록 데이터를 화면에 보인다.
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void BugsAlbumChartAll::slot_applyResult_albumChart(const QList<bugs::AlbumItemData>& list_data, const bool flag_lastPage){


        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_album->size() == 0){
                this->album_total_cnt = list_data.at(0).total_count;
                if(this->album_total_cnt > 50){
                    this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
                }
            }

            int start_index = this->list_album->size();
            this->list_album->append(list_data);

            if(start_index == 0){
                int max_cnt = this->list_album->size();
                this->album_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i] = new bugs::ItemAlbum_bugs(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->listAlbum_all[i], &bugs::ItemAlbum_bugs::signal_clicked, this, &BugsAlbumChartAll::slot_clickedItemAlbum);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i]->setData(this->list_album->at(i));
                    this->flowLayout_albums->addWidget(this->listAlbum_all[i]);

                    QCoreApplication::processEvents();
                }
                ContentLoadingwaitingMsgHide();      //cheon Tidal

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }

            this->request_more_albumData();
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            if(this->list_album->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->flowLayout_albums->addWidget(noData_widget);
            }
        }
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void BugsAlbumChartAll::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___albums){
            // ClickMode 별로 처리
            this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
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
    void BugsAlbumChartAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
    }
}
