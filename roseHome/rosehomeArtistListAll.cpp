#include "roseHome/rosehomeArtistListAll.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"

#include "widget/dialogconfirm.h"
#include "widget/NoData_Widget.h"

#include <QResizeEvent>
#include <QScrollBar>


namespace roseHome {

    const int SECTION_FOR_MORE_POPUP___artist = 0;

    /**
     * @brief RoseHomeArtistListAll::RoseHomeArtistListAll
     * @param parent
     */
    RoseHomeArtistListAll::RoseHomeArtistListAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // Data
        this->list_artist = new QList<roseHome::ArtistItemData>();

        // 기본 UI 세팅
        this->setUIControl_artists();
    }

    /**
     * @brief 소멸자.
     */
    RoseHomeArtistListAll::~RoseHomeArtistListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void RoseHomeArtistListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        PageInfo_ArtistAllView data_pageInfo = ConvertData::convertData_pageInfo_artistAllView(jsonObj);
        this->flagNeedReload = false;

        if(this->current_pageInfo.type != data_pageInfo.type || this->current_pageInfo.api_subPath != data_pageInfo.api_subPath){
            this->flagNeedReload = true;

            this->current_pageInfo = data_pageInfo;

            this->label_mainTitle->setText(this->current_pageInfo.pathTitle);
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
            connect(proc, &roseHome::ProcCommon::completeReq_list_artists, this, &RoseHomeArtistListAll::slot_applyResult_artistlistCheck);
            proc->request_rose_getList_recentlyArtists(this->current_pageInfo.api_subPath, this->current_pageInfo.filter_type, 0, 10);
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void RoseHomeArtistListAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

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
    void RoseHomeArtistListAll::setUIControl_artists(){

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

        connect(this->btn_mainTitle, &QPushButton::clicked, this, &RoseHomeArtistListAll::slot_btnClicked_Delete);

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
        roseHome::ItemArtist_rosehome *listArtist = new roseHome::ItemArtist_rosehome(0, SECTION_FOR_MORE_POPUP___artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);

        this->artist_widget_width = listArtist->get_fixedWidth();
        this->artist_widget_margin = listArtist->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeartistListAll::setUIControl_artists " << listartist->get_fixedWidth() << listartist->get_rightMargin();

        delete listArtist;
        //  get widget width, right margin - by diskept j230317 finish

        // layout for items
        this->flowLayout_artists = this->get_addUIControl_flowLayout(0, 20);
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void RoseHomeArtistListAll::initAll(){

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
    void RoseHomeArtistListAll::proc_wheelEvent_to_getMoreData(){

        // wheel evnet change - by diskept j230317 start
        if((!this->flagReqMore_artist && !this->flag_lastPage_artist) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flagReqMore_artist = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_artistData();
        }
        else if((this->artist_total_cnt > this->artist_draw_cnt) && this->flag_artist_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_artistDraw();
        }
        // wheel evnet change - by diskept j230317 start
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Artist 데이터를 더 요청한다.
     * @param page
     */
    void RoseHomeArtistListAll::request_more_artistData(){

        // request api change - by diskept j230317 start
        // next_offset
        if(this->list_artist->size() == 0){
            this->next_offset = 0;
        }
        else{
            this->next_offset++;
        }

        // request HTTP API
        roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
        connect(proc, &roseHome::ProcCommon::completeReq_list_artists, this, &RoseHomeArtistListAll::slot_applyResult_artists);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_recentlyArtists(this->current_pageInfo.api_subPath, this->current_pageInfo.filter_type, next_offset, 200);
        // request api change - by diskept j230317 finish

        if(this->next_offset == 0){
            this->flag_artist_draw = true;
        }

    }


    void RoseHomeArtistListAll::request_more_artistDraw(){

        // widget draw change - by diskept j230317 start
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

        //qDebug() << "[Debug] RoseHomeArtistListAll::request_more_artistDraw " << start_index << max_cnt << this->album_draw_cnt;

        for(int i = start_index; i < this->artist_draw_cnt; i++){
            this->listArtist_all[i] = new roseHome::ItemArtist_rosehome(i, SECTION_FOR_MORE_POPUP___artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->artist_draw_cnt; i++){
            this->listArtist_all[i]->setData(this->list_artist->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->artist_draw_cnt; i++){
            connect(this->listArtist_all[i], &roseHome::ItemArtist_rosehome::signal_clicked, this, &RoseHomeArtistListAll::slot_clickedItemArtist);
            this->flowLayout_artists->addWidget(this->listArtist_all[i]);
        }

        ContentLoadingwaitingMsgHide();

        this->flag_artist_draw = false;
        // widget draw change - by diskept j230317 finish
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
    void RoseHomeArtistListAll::slot_applyResult_artists(const QList<roseHome::ArtistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        int idx = sender()->property("idx").toInt();

        if(idx != this->next_offset){
            return;
        }

        //qDebug() << "[Debug] RoseHomeartistListAll::slot_applyResult_artists " << list_data.count();

        if(list_data.size() > 0){
            this->flag_lastPage_artist = flag_lastPage;
            this->flagReqMore_artist = false;

            if(this->list_artist->size() == 0){
                this->artist_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_artist->size();
            this->list_artist->append(list_data);

            if(start_index == 0){
                // widget draw change - by diskept j230317 start
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

                //qDebug() << "[Debug] RoseHomeartistListAll::slot_applyResult_artists " << start_index << max_cnt;

                for(int i = start_index; i < this->artist_draw_cnt; i++){
                    this->listArtist_all[i]->setData(this->list_artist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->artist_draw_cnt; i++){
                    connect(this->listArtist_all[i], &roseHome::ItemArtist_rosehome::signal_clicked, this, &RoseHomeArtistListAll::slot_clickedItemArtist);
                    this->flowLayout_artists->addWidget(this->listArtist_all[i]);
                }

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_artist_draw = false;
            }
            else{
                this->request_more_artistDraw();
            }

            // request call change - by diskept j230317 start
            /*if(flag_lastPage == false){
                this->request_more_artistData();
            }*/
            // request call change - by diskept j230317 finish
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


    void RoseHomeArtistListAll::slot_applyResult_artistlistCheck(const QList<roseHome::ArtistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void RoseHomeArtistListAll::slot_btnClicked_Delete(){

        QStringList tmpTitle = this->current_pageInfo.pathTitle.split("(");

        QJsonObject tmpObj;
        tmpObj.insert("contents_type", tmpTitle.at(0));
        tmpObj.insert("list_type", "ARTIST");
        tmpObj.insert("api_subPath", this->current_pageInfo.api_subPath);
        tmpObj.insert("filter_type", this->current_pageInfo.filter_type);

        QJsonObject tmpObj_move;
        tmpObj_move.insert(KEY_PAGE_CODE, PAGECODE_RH_RECENTLY_LIST_DELETE);
        tmpObj_move.insert(KEY_DATA, tmpObj);

        emit this->linker->signal_clickedMovePage(tmpObj_move);
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
    void RoseHomeArtistListAll::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        if(this->list_artist->at(index).type == "APPLE_MUSIC"){
            DialogConfirm *dlgConfirmMSG = new DialogConfirm(this);
            dlgConfirmMSG->setTitle(tr("Notice"));
            dlgConfirmMSG->setFixedSize(350, 400);
            dlgConfirmMSG->setAlignment(Qt::AlignCenter);
            dlgConfirmMSG->setTextHeight(150);

            dlgConfirmMSG->setText(tr("Service is being prepared."));

            int result = dlgConfirmMSG->exec();

            delete dlgConfirmMSG;
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemArtist(this->list_artist, clickMode, index, section);
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void RoseHomeArtistListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        Q_UNUSED(clickMode);
        Q_UNUSED(index);

        if(section == SECTION_FOR_MORE_POPUP___artist){
            this->proc_clicked_optMorePopup_fromArtist(this->list_artist, index, clickMode);
        }
    }


    /**
     * @brief RoseHomeArtistListAll::resizeEvent
     * @param event
     */
    void RoseHomeArtistListAll::resizeEvent(QResizeEvent *event){
        Q_UNUSED(event);

        // flowlayout spacing change - by diskept j230317 start
        this->setFlowLayoutResize(this, this->flowLayout_artists, this->artist_widget_width, this->artist_widget_margin);
        // flowlayout spacing change - by diskept j230317 finish

        this->widget_mainTitle->resize(this->width(), 69);
        this->btn_mainTitle->setGeometry(this->width() - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);
    }





    /**
     * @brief RoseHomeArtistHistoryAll::RoseHomeartistListAll
     * @param parent
     */
    RoseHomeArtistHistoryAll::RoseHomeArtistHistoryAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // Data
        this->list_artist = new QList<roseHome::ArtistItemData>();

        // 기본 UI 세팅
        this->setUIControl_artists();
    }


    /**
     * @brief 소멸자.
     */
    RoseHomeArtistHistoryAll::~RoseHomeArtistHistoryAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void RoseHomeArtistHistoryAll::setJsonObject_forData(const QJsonObject &jsonObj){

        PageInfo_ArtistAllView tmp_pageInfo = ConvertData::convertData_pageInfo_artistAllView(jsonObj);
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
    void RoseHomeArtistHistoryAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

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
    void RoseHomeArtistHistoryAll::setUIControl_artists(){

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
    void RoseHomeArtistHistoryAll::proc_wheelEvent_to_getMoreData(){

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
    void RoseHomeArtistHistoryAll::request_more_artistData(){

        // next_offset
        if(this->list_artist->size() == 0){
            this->next_offset = 0;
        }
        else{
            this->next_offset++;
        }

        // request HTTP API
        roseHome::ProcCommon *proc_artist = new roseHome::ProcCommon(this);
        connect(proc_artist, &roseHome::ProcCommon::completeReq_list_artists, this, &RoseHomeArtistHistoryAll::slot_applyResult_historyArtist);
        proc_artist->setProperty("idx", this->next_offset);
        proc_artist->request_rose_getList_historyArtists(this->data_pageInfo.api_subPath, this->data_pageInfo.filter_type, this->next_offset, 200);

        if(this->next_offset == 0){
            this->flag_artist_draw = true;
        }
    }

    void RoseHomeArtistHistoryAll::request_more_artistDraw(){

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
            connect(this->listArtist_all[i], &roseHome::ItemArtist_rosehome::signal_clicked, this, &RoseHomeArtistHistoryAll::slot_clickedItemArtist);
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
    void RoseHomeArtistHistoryAll::slot_applyResult_historyArtist(const QList<roseHome::ArtistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    connect(this->listArtist_all[i], &roseHome::ItemArtist_rosehome::signal_clicked, this, &RoseHomeArtistHistoryAll::slot_clickedItemArtist);
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
    void RoseHomeArtistHistoryAll::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(this->list_artist->at(index).type == "APPLE_MUSIC"){
            DialogConfirm *dlgConfirmMSG = new DialogConfirm(this);
            dlgConfirmMSG->setTitle(tr("Notice"));
            dlgConfirmMSG->setFixedSize(350, 400);
            dlgConfirmMSG->setAlignment(Qt::AlignCenter);
            dlgConfirmMSG->setTextHeight(150);

            dlgConfirmMSG->setText(tr("Service is being prepared."));

            int result = dlgConfirmMSG->exec();

            delete dlgConfirmMSG;
        }
        else{
            this->proc_clicked_itemArtist(this->list_artist, clickMode, index, section);
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void RoseHomeArtistHistoryAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        Q_UNUSED(clickMode);
        Q_UNUSED(index);

        if(section == SECTION_FOR_MORE_POPUP___artist){
            this->proc_clicked_optMorePopup_fromArtist(this->list_artist, index, clickMode);
        }
    }

    /**
     * @brief BugsRecenltyArtistAll::resizeEvent
     * @param event
     */
    void RoseHomeArtistHistoryAll::resizeEvent(QResizeEvent *event){

        AbstractRoseHomeSubWidget::resizeEvent(event);

        this->setFlowLayoutResize(this, this->flowLayout_artists, this->artist_widget_width, this->artist_widget_margin);

    }
}
