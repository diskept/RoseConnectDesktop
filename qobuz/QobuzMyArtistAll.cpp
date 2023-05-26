#include "qobuz/QobuzMyArtistAll.h"

#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ProcCommon_forQobuz.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QResizeEvent>
#include <QScrollBar>


namespace qobuz {

    const int SECTION_FOR_MORE_POPUP___artist = 0;


    /**
     * @brief QobuzMoodsMain::QobuzMoodsMain
     * @param parent
     */
    QobuzMyArtistAll::QobuzMyArtistAll(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll, parent) {

        // Data
        this->list_artist = new QList<qobuz::ArtistItemData>();
        this->list_filterOpt_sort = AbstractQobuzSubWidget::get_list_filterOptData_forMyCollection();

        // 기본 UI 세팅
        this->setUIControl_artists();
    }


    /**
     * @brief 소멸자.
     */
    QobuzMyArtistAll::~QobuzMyArtistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzMyArtistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(this->page != pageCode){
            this->flagNeedReload = true;

            this->page = pageCode;

            // init
            this->next_offset = 0;
            this->artist_total_cnt = 0;
            this->artist_draw_cnt = 0;

            this->list_artist->clear();;

            // request HTTP API
            this->flagReqMore_artist = false;
            this->flag_lastPage_artist = false;

            this->flag_artist_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_artistData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void QobuzMyArtistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

            GSCommon::clearLayout(this->flowLayout_artists);

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


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void QobuzMyArtistAll::setUIControl_artists(){

        this->addUIControl_mainTitle("My Artists");

        // layout for items
        this->flowLayout_artists = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &QobuzMyArtistAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyArtistAll::setUIControl_filter
     */
    void QobuzMyArtistAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &QobuzMyArtistAll::slot_filterClicked);


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
    void QobuzMyArtistAll::initAll(){

        // init
        this->next_offset = 0;
        this->artist_total_cnt = 0;
        this->artist_draw_cnt = 0;

        this->list_artist->clear();;

        // request HTTP API
        this->flagReqMore_artist = false;
        this->flag_lastPage_artist = false;

        this->flag_artist_draw = false;

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

        GSCommon::clearLayout(this->flowLayout_artists);
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void QobuzMyArtistAll::proc_wheelEvent_to_getMoreData(){

        if((this->artist_total_cnt > this->artist_draw_cnt) && (this->list_artist->size() > this->artist_draw_cnt) && (this->flag_artist_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_artist_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_artistDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void QobuzMyArtistAll::resizeEvent(QResizeEvent *event){

        AbstractQobuzSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }
    }


    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void QobuzMyArtistAll::slot_filterClicked(){

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
    void QobuzMyArtistAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

        // 코드가 다른 경우에 대해서만 처리한다.
        if(this->selected_filterCode != selected_filterCode){

            // update
            this->selected_filterCode = selected_filterCode;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(selected_filterCode.toString(), selected_filterName);

            // clear - 옵션코드가 바뀌었으므로 모두 초기화한다.
            this->initAll();

            this->scrollTop();
            this->request_more_artistData();

        }

    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Artist 데이터를 더 요청한다.
     * @param page
     */
    void QobuzMyArtistAll::request_more_artistData(){

        if(!this->flagReqMore_artist && !this->flag_lastPage_artist){
            this->flagReqMore_artist = true;

            // j220913 list count check
            int width_cnt = global.LmtCnt / 220;
            int mod = this->artist_draw_cnt % width_cnt;

            if(mod == 0){
                this->artist_widget_cnt = width_cnt * 10;
            }
            else{
                this->artist_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
            // j220913 list count check

            // next_offset
            this->next_offset = this->list_artist->size();

            // filtering
            QString selected_sortOpt = this->selected_filterCode.toString();
            ProcCommon::DataOrderOption sortOpt = ProcCommon::DataOrderOption::NAME;                // 기본값
            ProcCommon::DataOrderDirection orderDirection = ProcCommon::DataOrderDirection::ASC;    // 기본값

            if(selected_sortOpt == QVariant::fromValue(ProcCommon::DataOrderOption::DATE).toString()){
                sortOpt = ProcCommon::DataOrderOption::DATE;
                orderDirection = ProcCommon::DataOrderDirection::DESC;
            }

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_myFavoriteArtists, this, &QobuzMyArtistAll::slot_applyResult_myFavoriteArtists);
            proc->request_qobuz_getList_myFavoriteArtists(sortOpt, orderDirection, this->artist_widget_cnt, this->next_offset);

            if(this->next_offset == 0){
                this->flag_artist_draw = true;
            }
        }
    }


    void QobuzMyArtistAll::request_more_artistDraw(){

        // j220913 list count check
        int width_cnt = global.LmtCnt / 220;
        int mod = this->artist_draw_cnt % width_cnt;

        if(mod == 0){
            this->artist_widget_cnt = width_cnt * 10;
        }
        else{
            this->artist_widget_cnt = width_cnt * 10 + (width_cnt - mod);
        }
        // j220913 list count check

        int start_index = this->artist_draw_cnt;
        int max_cnt = ((this->artist_total_cnt - this->artist_draw_cnt) > this->artist_widget_cnt ) ? this->artist_widget_cnt : (this->artist_total_cnt - this->artist_draw_cnt);
        this->artist_draw_cnt += max_cnt;

        for(int i = start_index; i < this->artist_draw_cnt; i++){
            this->listArtist_all[i] = new qobuz::ItemArtist_qobuz(i, SECTION_FOR_MORE_POPUP___artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->listArtist_all[i], &qobuz::ItemArtist_qobuz::signal_clicked, this, &QobuzMyArtistAll::slot_clickedItemArtist);
        }

        for(int i = start_index; i < this->artist_draw_cnt; i++){
            this->listArtist_all[i]->setData(this->list_artist->at(i));
            this->flowLayout_artists->addWidget(this->listArtist_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_artist_draw = false;
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
    void QobuzMyArtistAll::slot_applyResult_myFavoriteArtists(const QList<qobuz::ArtistItemData> &list_data, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_artist = flag_lastPage;
            this->flagReqMore_artist = false;

            if(this->list_artist->size() == 0){
                this->artist_total_cnt = list_data.at(0).artist_total_cnt;
            }

            int start_index = this->list_artist->size();
            this->list_artist->append(list_data);

            if(start_index == 0){
                int max_cnt = this->list_artist->size();
                this->artist_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->listArtist_all[i] = new qobuz::ItemArtist_qobuz(i, SECTION_FOR_MORE_POPUP___artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->listArtist_all[i], &qobuz::ItemArtist_qobuz::signal_clicked, this, &QobuzMyArtistAll::slot_clickedItemArtist);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listArtist_all[i]->setData(this->list_artist->at(i));
                    this->flowLayout_artists->addWidget(this->listArtist_all[i]);

                    QCoreApplication::processEvents();
                }
                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_artist_draw = false;
            }

            this->request_more_artistData();
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            if(this->list_artist->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Artist_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->flowLayout_artists->addWidget(noData_widget);
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - ItemArtist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzMyArtistAll::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemArtist(this->list_artist, clickMode, index, section);
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
    void QobuzMyArtistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___artist){
            this->proc_clicked_optMorePopup_fromArtist(this->list_artist, index, clickMode);
        }
    }





    /**
     * @brief QobuzRecenltyArtistAll::QobuzRecenltyArtistAll
     * @param parent
     */
    QobuzRecenltyArtistAll::QobuzRecenltyArtistAll(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        this->linker = Linker::getInstance();

        // Data
        this->list_artist = new QList<roseHome::ArtistItemData>();

        // 기본 UI 세팅
        this->setUIControl_artists();
    }

    /**
     * @brief 소멸자.
     */
    QobuzRecenltyArtistAll::~QobuzRecenltyArtistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzRecenltyArtistAll::setJsonObject_forData(const QJsonObject &jsonObj){

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
            this->artist_total_cnt = 0;
            this->artist_draw_cnt = 0;

            this->list_artist->clear();

            // request HTTP API
            this->flagReqMore_artist = false;
            this->flag_lastPage_artist = false;

            this->flag_artist_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_artistData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_artists, this, &QobuzRecenltyArtistAll::slot_applyResult_artistlistCheck);
            proc->request_rose_getList_recentlyArtists("member/artist/recent", "QOBUZ", 0, 10);
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void QobuzRecenltyArtistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

            // init
            GSCommon::clearLayout(this->flowLayout_artists);

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
    void QobuzRecenltyArtistAll::setUIControl_artists(){

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

        connect(this->btn_mainTitle, &QPushButton::clicked, this, &QobuzRecenltyArtistAll::slot_btnClicked_Delete);

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

        // get widget width, right margin - by diskept j230317 start
        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // list count check - by diskept j230317 start
        roseHome::ItemArtist_rosehome *listArtist = new roseHome::ItemArtist_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200);

        this->artist_widget_width = listArtist->get_fixedWidth();
        this->artist_widget_margin = listArtist->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeartistListAll::setUIControl_artists " << listartist->get_fixedWidth() << listartist->get_rightMargin();

        delete listArtist;

        // layout for items
        this->flowLayout_artists = this->get_addUIControl_flowLayout(0, 20);
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void QobuzRecenltyArtistAll::initAll(){

        // init
        this->next_offset = 0;
        this->artist_total_cnt = 0;
        this->artist_draw_cnt = 0;

        this->list_artist->clear();

        // request HTTP API
        this->flagReqMore_artist = false;
        this->flag_lastPage_artist = false;

        this->flag_artist_draw = false;

        GSCommon::clearLayout(this->flowLayout_artists);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void QobuzRecenltyArtistAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_artist && !this->flag_lastPage_artist) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flagReqMore_artist = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_artistData();
        }
        else if((this->artist_total_cnt > this->artist_draw_cnt) && this->flag_artist_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_artistDraw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Artist 데이터를 더 요청한다.
     * @param page
     */
    void QobuzRecenltyArtistAll::request_more_artistData(){

        // next_offset
        if(this->list_artist->size() == 0){
            this->next_offset = 0;
        }
        else{
            this->next_offset++;
        }


        // request HTTP API
        roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
        connect(proc, &roseHome::ProcCommon::completeReq_list_artists, this, &QobuzRecenltyArtistAll::slot_applyResult_recentlyArtists);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_recentlyArtists("member/artist/recent", "QOBUZ", next_offset, 200);

        if(this->next_offset == 0){
            this->flag_artist_draw = true;
        }
    }


    void QobuzRecenltyArtistAll::request_more_artistDraw(){

        this->flag_artist_draw = true;

        int f_width = this->flowLayout_artists->geometry().width();

        if(this->flowLayout_artists->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->artist_widget_width + this->artist_widget_margin);
        int f_mod = this->artist_draw_cnt % f_wg_cnt;

        if(f_mod == 0){
            this->artist_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->artist_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        //qDebug() << "[Debug] RoseHomeArtistListAll::request_more_artistDraw " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

        int start_index = this->artist_draw_cnt;
        int max_cnt = ((this->artist_total_cnt - this->artist_draw_cnt) > this->artist_widget_cnt ) ? this->artist_widget_cnt : (this->artist_total_cnt - this->artist_draw_cnt);
        this->artist_draw_cnt += max_cnt;

        for(int i = start_index; i < this->artist_draw_cnt; i++){
            this->listArtist_all[i] = new roseHome::ItemArtist_rosehome(i, SECTION_FOR_MORE_POPUP___artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->artist_draw_cnt; i++){
            this->listArtist_all[i]->setData(this->list_artist->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->artist_draw_cnt; i++){
            connect(this->listArtist_all[i], &roseHome::ItemArtist_rosehome::signal_clicked, this, &QobuzRecenltyArtistAll::slot_clickedItemArtist);
            this->flowLayout_artists->addWidget(this->listArtist_all[i]);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_artist_draw = false;
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
    void QobuzRecenltyArtistAll::slot_applyResult_recentlyArtists(const QList<roseHome::ArtistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        int idx = sender()->property("idx").toInt();

        if(idx != this->next_offset){
            return;
        }

        if(list_data.size() > 0){
            this->flag_lastPage_artist = flag_lastPage;
            this->flagReqMore_artist = false;

            if(this->list_artist->size() == 0){
                this->artist_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_artist->size();
            this->list_artist->append(list_data);

            if(start_index == 0){
                int f_width = this->flowLayout_artists->geometry().width();

                if(this->flowLayout_artists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                int f_wg_cnt = f_width / (this->artist_widget_width + this->artist_widget_margin);
                int f_mod = this->artist_draw_cnt % f_wg_cnt;

                if(f_mod == 0){
                    this->artist_widget_cnt = f_wg_cnt * 10;
                }
                else{
                    this->artist_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                }

                //qDebug() << "[Debug] RoseHomeartistListAll::slot_applyResult_artists " << f_width << f_wg_cnt << f_mod << this->artist_widget_cnt;

                int start_index = this->artist_draw_cnt;
                int max_cnt = ((this->artist_total_cnt - this->artist_draw_cnt) > this->artist_widget_cnt ) ? this->artist_widget_cnt : (this->artist_total_cnt - this->artist_draw_cnt);
                this->artist_draw_cnt += max_cnt;

                for(int i = start_index; i < this->artist_draw_cnt; i++){
                    this->listArtist_all[i] = new roseHome::ItemArtist_rosehome(i, SECTION_FOR_MORE_POPUP___artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->artist_draw_cnt; i++){
                    this->listArtist_all[i]->setData(this->list_artist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->artist_draw_cnt; i++){
                    connect(this->listArtist_all[i], &roseHome::ItemArtist_rosehome::signal_clicked, this, &QobuzRecenltyArtistAll::slot_clickedItemArtist);
                    this->flowLayout_artists->addWidget(this->listArtist_all[i]);
                }

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_artist_draw = false;
            }
            else{
                this->request_more_artistDraw();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->list_artist->size() <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_artists->geometry().width();

                if(this->flowLayout_artists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Artist_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_artists->addWidget(noData_widget);
            }
        }
    }


    void QobuzRecenltyArtistAll::slot_applyResult_artistlistCheck(const QList<roseHome::ArtistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_artist->count() == 0){
                QString tmpTitle = tr("Recently Played Artists") + QString(" (%1)").arg(list_data.at(0).totalCount);
                this->label_mainTitle->setText(tmpTitle);
                this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                this->initAll();

                this->request_more_artistData();

                this->flagNeedReload = true;
                this->setActivePage();
            }
            else{
                for(int i = 0; i < list_data.count(); i++){
                    if(list_data.at(i).name != this->list_artist->at(i).name){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_artist->at(0).totalCount) || (change_flag > 0)){

                    QString tmpTitle = tr("Recently Played Artists") + QString(" (%1)").arg(list_data.at(0).totalCount);
                    this->label_mainTitle->setText(tmpTitle);
                    this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                    this->initAll();

                    this->request_more_artistData();

                    this->flagNeedReload = true;
                    this->setActivePage();
                }
                else{
                    ContentLoadingwaitingMsgHide();
                }
            }
        }
        else if(list_data.size() == 0){

            QString tmpTitle = tr("Recently Played Artists (0)");
            this->label_mainTitle->setText(tmpTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

            this->initAll();

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_artistData();

            this->flagNeedReload = true;
            this->setActivePage();
        }
        else{
            ContentLoadingwaitingMsgHide();
        }
    }


    void QobuzRecenltyArtistAll::slot_btnClicked_Delete(){

        QStringList tmpTitle = this->title.split("(");

        QJsonObject tmpObj;
        tmpObj.insert("contents_type", tmpTitle.at(0));
        tmpObj.insert("list_type", "ARTIST");
        tmpObj.insert("api_subPath", "member/artist/recent");
        tmpObj.insert("filter_type", "QOBUZ");

        QJsonObject tmpObj_move;
        tmpObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_MY_RECENTLY_LIST_DELETE);
        tmpObj_move.insert(KEY_DATA, tmpObj);

        emit this->linker->signal_clickedMovePage(tmpObj_move);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - ItemArtist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzRecenltyArtistAll::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리 - Artist 파트가 여러개임
        if(section == SECTION_FOR_MORE_POPUP___artist){
            roseHome::ArtistItemData data = this->list_artist->at(index);

            // ClickMode 별로 처리
            if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Artist Detail 페이지 진입
                QJsonObject jsonObj_move = QJsonObject();
                jsonObj_move.insert("id", data.clientKey.toInt());
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ARTIST_DETAIL);

                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){

                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Artist, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
            }
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void QobuzRecenltyArtistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        Q_UNUSED(index);

        if(section == SECTION_FOR_MORE_POPUP___artist){
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                setUIShare();
            }
        }
    }


    /**
     * @brief RoseHomeArtistListAll::resizeEvent
     * @param event
     */
    void QobuzRecenltyArtistAll::resizeEvent(QResizeEvent *event){
        Q_UNUSED(event);

        this->setFlowLayoutResize(this, this->flowLayout_artists, this->artist_widget_width, this->artist_widget_margin);

        this->widget_mainTitle->resize(global.LmtCnt, 69);
        this->btn_mainTitle->setGeometry(global.LmtCnt - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);
    }





    /**
     * @brief QobuzArtistHistoryAll::RoseHomeartistListAll
     * @param parent
     */
    QobuzArtistHistoryAll::QobuzArtistHistoryAll(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // Data
        this->list_artist = new QList<roseHome::ArtistItemData>();

        // 기본 UI 세팅
        this->setUIControl_artists();
    }


    /**
     * @brief 소멸자.
     */
    QobuzArtistHistoryAll::~QobuzArtistHistoryAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzArtistHistoryAll::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::PageInfo_ArtistAllView tmp_pageInfo = roseHome::ConvertData::convertData_pageInfo_artistAllView(jsonObj);
        this->flagNeedReload = false;

        if(tmp_pageInfo.api_subPath != this->data_pageInfo.api_subPath || tmp_pageInfo.pathTitle != this->data_pageInfo.pathTitle || tmp_pageInfo.filter_type != this->data_pageInfo.filter_type){
            this->flagNeedReload = true;

            this->data_pageInfo = tmp_pageInfo;

            this->label_mainTitle->setText(this->data_pageInfo.pathTitle);

            // init
            this->next_offset = 0;
            this->artist_total_cnt = 0;
            this->artist_draw_cnt = 0;

            this->list_artist->clear();

            // request HTTP API
            this->flagReqMore_artist = false;
            this->flag_lastPage_artist = false;

            this->flag_artist_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_artistData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void QobuzArtistHistoryAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

            // init
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // layout for items
            this->flowLayout_artists = this->get_addUIControl_flowLayout(0, 20);
            GSCommon::clearLayout(this->flowLayout_artists);

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
    void QobuzArtistHistoryAll::setUIControl_artists(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("");

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        roseHome::ItemArtist_rosehome *listArtist = new roseHome::ItemArtist_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200);

        this->artist_widget_width = listArtist->get_fixedWidth();
        this->artist_widget_margin = listArtist->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeartistListAll::setUIControl_artists " << listartist->get_fixedWidth() << listartist->get_rightMargin();

        delete listArtist;

        // layout for items
        this->flowLayout_artists = this->get_addUIControl_flowLayout(0, 20);
    }

    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void QobuzArtistHistoryAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_artist && !this->flag_lastPage_artist) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flagReqMore_artist = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_artistData();
        }
        else if((this->artist_total_cnt > this->artist_draw_cnt) && this->flag_artist_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_artistDraw();
        }
    }

    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief artist 데이터를 더 요청한다.
     * @param page
     */
    void QobuzArtistHistoryAll::request_more_artistData(){

        // next_offset
        if(this->list_artist->size() == 0){
            this->next_offset = 0;
        }
        else{
            this->next_offset++;
        }

        // request HTTP API
        roseHome::ProcCommon *proc_artist = new roseHome::ProcCommon(this);
        connect(proc_artist, &roseHome::ProcCommon::completeReq_list_artists, this, &QobuzArtistHistoryAll::slot_applyResult_historyArtist);
        proc_artist->setProperty("idx", this->next_offset);
        proc_artist->request_rose_getList_historyArtists(this->data_pageInfo.api_subPath, this->data_pageInfo.filter_type, this->next_offset, 200);

        if(this->next_offset == 0){
            this->flag_artist_draw = true;
        }
    }

    void QobuzArtistHistoryAll::request_more_artistDraw(){

        this->flag_artist_draw = true;

        int f_width = this->flowLayout_artists->geometry().width();

        if(this->flowLayout_artists->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->artist_widget_width + this->artist_widget_margin);
        int f_mod = this->artist_draw_cnt % f_wg_cnt;

        if(f_mod == 0){
            this->artist_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->artist_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        //qDebug() << "[Debug] RoseHomeArtistListAll::request_more_artistDraw " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

        int start_index = this->artist_draw_cnt;
        int max_cnt = ((this->artist_total_cnt - this->artist_draw_cnt) > this->artist_widget_cnt ) ? this->artist_widget_cnt : (this->artist_total_cnt - this->artist_draw_cnt);
        this->artist_draw_cnt += max_cnt;

        for(int i = start_index; i < this->artist_draw_cnt; i++){
            this->listArtist_all[i] = new roseHome::ItemArtist_rosehome(i, SECTION_FOR_MORE_POPUP___artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->artist_draw_cnt; i++){
            this->listArtist_all[i]->setData(this->list_artist->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->artist_draw_cnt; i++){
            connect(this->listArtist_all[i], &roseHome::ItemArtist_rosehome::signal_clicked, this, &QobuzArtistHistoryAll::slot_clickedItemArtist);
            this->flowLayout_artists->addWidget(this->listArtist_all[i]);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_artist_draw = false;
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
    void QobuzArtistHistoryAll::slot_applyResult_historyArtist(const QList<roseHome::ArtistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        int idx = sender()->property("idx").toInt();

        if(idx != this->next_offset){
            return;
        }
        if(list_data.size() > 0){
            this->flag_lastPage_artist = flag_lastPage;
            this->flagReqMore_artist = false;

            if(this->list_artist->size() == 0){
                this->artist_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_artist->size();
            this->list_artist->append(list_data);

            if(start_index == 0){
                int f_width = this->flowLayout_artists->geometry().width();

                if(this->flowLayout_artists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                int f_wg_cnt = f_width / (this->artist_widget_width + this->artist_widget_margin);
                int f_mod = this->artist_draw_cnt % f_wg_cnt;

                if(f_mod == 0){
                    this->artist_widget_cnt = f_wg_cnt * 10;
                }
                else{
                    this->artist_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                }

                //qDebug() << "[Debug] RoseHomeartistListAll::slot_applyResult_artists " << f_width << f_wg_cnt << f_mod << this->artist_widget_cnt;

                int start_index = this->artist_draw_cnt;
                int max_cnt = ((this->artist_total_cnt - this->artist_draw_cnt) > this->artist_widget_cnt ) ? this->artist_widget_cnt : (this->artist_total_cnt - this->artist_draw_cnt);
                this->artist_draw_cnt += max_cnt;

                for(int i = start_index; i < this->artist_draw_cnt; i++){
                    this->listArtist_all[i] = new roseHome::ItemArtist_rosehome(i, SECTION_FOR_MORE_POPUP___artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->artist_draw_cnt; i++){
                    this->listArtist_all[i]->setData(this->list_artist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->artist_draw_cnt; i++){
                    connect(this->listArtist_all[i], &roseHome::ItemArtist_rosehome::signal_clicked, this, &QobuzArtistHistoryAll::slot_clickedItemArtist);
                    this->flowLayout_artists->addWidget(this->listArtist_all[i]);
                }

                this->setFlowLayoutResize(this, this->flowLayout_artists, this->artist_widget_width, this->artist_widget_margin);

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_artist_draw = false;
            }
            else{
                this->request_more_artistDraw();
            }

//            if(flag_lastPage == false){
//                this->request_more_artistData();
//            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->list_artist->size() <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_artists->geometry().width();

                if(this->flowLayout_artists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Artist_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_artists->addWidget(noData_widget);
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - Itemartist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzArtistHistoryAll::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___artist){
            //this->proc_clicked_itemArtist(this->list_artist, clickMode, index, section);
            if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Artist Detail 페이지 진입
                QJsonObject jsonObj_move;
                jsonObj_move.insert("image_large", "");
                jsonObj_move.insert("image_medium", this->list_artist->at(index).thumbnail);
                jsonObj_move.insert("image_small", "");
                jsonObj_move.insert("biography", "");
                jsonObj_move.insert("name", this->list_artist->at(index).name);
                jsonObj_move.insert("id", this->list_artist->at(index).clientKey.toInt());
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ARTIST_DETAIL);

                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){

                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Artist, roseHome::ConvertData::getConvertOptHeaderData(this->list_artist->at(index)), index, section);
            }
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void QobuzArtistHistoryAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        Q_UNUSED(index);

        if(section == SECTION_FOR_MORE_POPUP___artist){
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                setUIShare();
            }
        }
    }

    /**
     * @brief BugsRecenltyArtistAll::resizeEvent
     * @param event
     */
    void QobuzArtistHistoryAll::resizeEvent(QResizeEvent *event){

        AbstractQobuzSubWidget::resizeEvent(event);

        this->setFlowLayoutResize(this, this->flowLayout_artists, this->artist_widget_width, this->artist_widget_margin);

    }
}
