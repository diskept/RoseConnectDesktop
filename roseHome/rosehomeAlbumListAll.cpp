#include "roseHome/rosehomeAlbumListAll.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QResizeEvent>
#include <QScrollBar>


namespace roseHome {

    const int SECTION_FOR_MORE_POPUP___albums = 0;

    /**
     * @brief RoseHomeAlbumListAll::RoseHomeAlbumListAll
     * @param parent
     */
    RoseHomeAlbumListAll::RoseHomeAlbumListAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // Data
        this->list_album = new QList<roseHome::AlbumItemData>();

        // 기본 UI 세팅
        this->setUIControl_albums();
    }

    /**
     * @brief 소멸자.
     */
    RoseHomeAlbumListAll::~RoseHomeAlbumListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void RoseHomeAlbumListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        PageInfo_AlbumAllView data_pageInfo = ConvertData::convertData_pageInfo_albumAllView(jsonObj);
        this->flagNeedReload = false;

        if(this->current_pageInfo.filter_type != data_pageInfo.filter_type || this->current_pageInfo.api_subPath != data_pageInfo.api_subPath){
            this->flagNeedReload = true;

            this->current_pageInfo = data_pageInfo;

            this->label_mainTitle->setText(this->current_pageInfo.pathTitle);
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
            connect(proc, &roseHome::ProcCommon::completeReq_list_albums, this, &RoseHomeAlbumListAll::slot_applyResult_albumlistCheck);
            proc->request_rose_getList_recentlyAlbums(this->current_pageInfo.api_subPath, this->current_pageInfo.filter_type, 0, 10);
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void RoseHomeAlbumListAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // init
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
    void RoseHomeAlbumListAll::setUIControl_albums(){

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

        connect(this->btn_mainTitle, &QPushButton::clicked, this, &RoseHomeAlbumListAll::slot_btnClicked_Delete);

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

        roseHome::ItemAlbum_rosehome *listAlbum = new roseHome::ItemAlbum_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);

        this->album_widget_width = listAlbum->get_fixedWidth();
        this->album_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeAlbumListAll::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;
        //  get widget width, right margin - by diskept j230317 finish

        // layout for items
        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void RoseHomeAlbumListAll::initAll(){

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
    void RoseHomeAlbumListAll::proc_wheelEvent_to_getMoreData(){

        // wheel evnet change - by diskept j230317 start
        if((!this->flagReqMore_album && !this->flag_lastPage_album) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_album = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumData();
        }
        else if((this->album_total_cnt > this->album_draw_cnt) && this->flag_album_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
        // wheel evnet change - by diskept j230317 start
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Album 데이터를 더 요청한다.
     * @param page
     */
    void RoseHomeAlbumListAll::request_more_albumData(){

        // request api change - by diskept j230317 start
        // next_offset
        if(this->list_album->size() == 0){
            this->next_offset = 0;
        }
        else{
            this->next_offset++;
        }

        // request HTTP API
        roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
        connect(proc, &roseHome::ProcCommon::completeReq_list_albums, this, &RoseHomeAlbumListAll::slot_applyResult_albums);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_recentlyAlbums(this->current_pageInfo.api_subPath, this->current_pageInfo.filter_type, next_offset, 200);
        // request api change - by diskept j230317 finish

        if(this->next_offset == 0){
            this->flag_album_draw = true;
        }
    }


    void RoseHomeAlbumListAll::request_more_albumDraw(){

        // widget draw change - by diskept j230317 start
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

        //qDebug() << "[Debug] RoseHomeAlbumListAll::request_more_albumDraw " << start_index << max_cnt << this->album_draw_cnt;

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
            connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &RoseHomeAlbumListAll::slot_clickedItemAlbum);
        }

        ContentLoadingwaitingMsgHide();

        this->flag_album_draw = false;
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
    void RoseHomeAlbumListAll::slot_applyResult_albums(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        int idx = sender()->property("idx").toInt();

        if(idx != this->next_offset){
            return;
        }

        //qDebug() << "[Debug] RoseHomeAlbumListAll::slot_applyResult_albums " << list_data.count();

        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_album->size() == 0){
                this->album_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_album->size();
            this->list_album->append(list_data);

            if(start_index == 0){
                // widget draw change - by diskept j230317 start
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

                if(max_cnt > this->list_album->count()){
                    max_cnt = this->list_album->count();
                }
                this->album_draw_cnt += max_cnt;

                //qDebug() << "[Debug] RoseHomeAlbumListAll::slot_applyResult_albums " << start_index << max_cnt;

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->listAlbum_all[i]->setData(this->list_album->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &RoseHomeAlbumListAll::slot_clickedItemAlbum);
                    this->flowLayout_albums->addWidget(this->listAlbum_all[i]);
                }
                // widget draw change - by diskept j230317 finish

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }
            else{
                this->request_more_albumDraw();
            }

            // request call change - by diskept j230317 start
            /*if((this->list_album->count() < this->album_total_cnt) && (this->list_album->count() > this->album_draw_cnt)){
                this->request_more_albumData();
            }*/
            // request call change - by diskept j230317 finish
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


    void RoseHomeAlbumListAll::slot_applyResult_albumlistCheck(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_album->count() == 0){
                QString tmpTitle = tr("Recently Played Album") + QString(" (%1)").arg(list_data.at(0).totalCount);
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

                    QString tmpTitle = tr("Recently Played Album") + QString(" (%1)").arg(list_data.at(0).totalCount);
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
            QString tmpTitle = tr("Recently Played Album (0)");
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


    void RoseHomeAlbumListAll::slot_btnClicked_Delete(){

        QStringList tmpTitle = this->current_pageInfo.pathTitle.split("(");

        QJsonObject tmpObj;
        tmpObj.insert("contents_type", tmpTitle.at(0));
        tmpObj.insert("list_type", "ALBUM");
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
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void RoseHomeAlbumListAll::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void RoseHomeAlbumListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        Q_UNUSED(clickMode);
        Q_UNUSED(index);

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
    }


    /**
     * @brief RoseHomeAlbumListAll::resizeEvent
     * @param event
     */
    void RoseHomeAlbumListAll::resizeEvent(QResizeEvent *event){//c230223

        Q_UNUSED(event);

        // flowlayout spacing change - by diskept j230317 start
        this->setFlowLayoutResize(this, this->flowLayout_albums, this->album_widget_width, this->album_widget_margin);
        // flowlayout spacing change - by diskept j230317 finish

        this->widget_mainTitle->resize(this->width(), 69);
        this->btn_mainTitle->setGeometry(this->width() - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);
    }





    /**
     * @brief RoseHomeAlbumHistoryAll::RoseHomeAlbumListAll
     * @param parent
     */
    RoseHomeAlbumHistoryAll::RoseHomeAlbumHistoryAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // Data
        this->list_album = new QList<roseHome::AlbumItemData>();

        // 기본 UI 세팅
        this->setUIControl_albums();
    }

    /**
     * @brief 소멸자.
     */
    RoseHomeAlbumHistoryAll::~RoseHomeAlbumHistoryAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void RoseHomeAlbumHistoryAll::setJsonObject_forData(const QJsonObject &jsonObj){

        PageInfo_AlbumAllView tmp_pageInfo = ConvertData::convertData_pageInfo_albumAllView(jsonObj);
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
    void RoseHomeAlbumHistoryAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // init
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
    void RoseHomeAlbumHistoryAll::setUIControl_albums(){

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
    void RoseHomeAlbumHistoryAll::proc_wheelEvent_to_getMoreData(){

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
    void RoseHomeAlbumHistoryAll::resizeEvent(QResizeEvent *event){

        AbstractRoseHomeSubWidget::resizeEvent(event);

        this->setFlowLayoutResize(this, this->flowLayout_albums, this->album_widget_width, this->album_widget_margin);
    }

    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Album 데이터를 더 요청한다.
     * @param page
     */
    void RoseHomeAlbumHistoryAll::request_more_albumData(){

        // next_offset
        if(this->list_album->size() == 0){
            this->next_offset = 0;
        }
        else{
            this->next_offset++;
        }


        // request HTTP API
        roseHome::ProcCommon *proc_album = new roseHome::ProcCommon(this);
        connect(proc_album, &roseHome::ProcCommon::completeReq_list_albums, this, &RoseHomeAlbumHistoryAll::slot_applyResult_historyAlbum);
        proc_album->setProperty("idx", this->next_offset);
        proc_album->request_rose_getList_historyAlbums(this->data_pageInfo.api_subPath, this->data_pageInfo.filter_type, this->next_offset, 200);

        if(this->next_offset == 0){
            this->flag_album_draw = true;
        }

    }

    void RoseHomeAlbumHistoryAll::request_more_albumDraw(){

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
            connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &RoseHomeAlbumHistoryAll::slot_clickedItemAlbum);
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
    void RoseHomeAlbumHistoryAll::slot_applyResult_historyAlbum(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &RoseHomeAlbumHistoryAll::slot_clickedItemAlbum);
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
    void RoseHomeAlbumHistoryAll::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void RoseHomeAlbumHistoryAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        Q_UNUSED(clickMode);
        Q_UNUSED(index);

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
    }
}
