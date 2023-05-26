#include "apple/AppleAlbumListAll_Rose.h"

#include "apple/ProcRosePlay_withApple.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QResizeEvent>


namespace apple {

    const int SECTION_FOR_MORE_POPUP___albums = 1;

    const int GET_ITEM_SIZE___ONCE = 20;


    /**
     * @brief AppleAlbumListAll_Rose::AppleAlbumListAll_Rose
     * @param parent
     */
    AppleAlbumListAll_Rose::AppleAlbumListAll_Rose(QWidget *parent) : AbstractAppleSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // Data
        this->list_album = new QList<roseHome::AlbumItemData>();
        this->list_filterOpt_sort = AbstractAppleSubWidget::get_list_filterOptData_forMyCollection();

        // 기본 UI 세팅
        this->setUIControl_albums();
    }


    /**
     * @brief 소멸자.
     */
    AppleAlbumListAll_Rose::~AppleAlbumListAll_Rose(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void AppleAlbumListAll_Rose::setJsonObject_forData(const QJsonObject &jsonObj){

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
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void AppleAlbumListAll_Rose::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractAppleSubWidget::setActivePage();

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
    void AppleAlbumListAll_Rose::setUIControl_albums(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Albums");

        // layout for items
        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &AppleAlbumListAll_Rose::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyAlbumAll::setUIControl_filter
     */
    void AppleAlbumListAll_Rose::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &AppleAlbumListAll_Rose::slot_filterClicked);

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
    void AppleAlbumListAll_Rose::initAll(){

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
    void AppleAlbumListAll_Rose::proc_wheelEvent_to_getMoreData(){

        if((this->album_total_cnt > this->album_draw_cnt) && (this->list_album->size() > this->album_draw_cnt) && (this->flag_album_draw == false)){

            this->flag_album_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void AppleAlbumListAll_Rose::resizeEvent(QResizeEvent *event){

        AbstractAppleSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }
    }



    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void AppleAlbumListAll_Rose::slot_filterClicked(){

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
    void AppleAlbumListAll_Rose::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

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
    void AppleAlbumListAll_Rose::request_more_albumData(){

        if(!this->flagReqMore_album && !this->flag_lastPage_album){
            this->flagReqMore_album = true;

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
            connect(proc, &roseHome::ProcCommon::completeReq_list_albums, this, &AppleAlbumListAll_Rose::slot_applyResult_myAlbums_rose);
            proc->request_rose_getList_recentlyAlbums("member/album/recent", "APPLE_MUSIC", next_offset, GET_ITEM_SIZE___ONCE);

            if(this->next_offset == 0){
                this->flag_album_draw = true;
            }
        }
    }


    void AppleAlbumListAll_Rose::request_more_albumDraw(){

        int start_index = this->album_draw_cnt;
        int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > GET_ITEM_SIZE___ONCE ) ? GET_ITEM_SIZE___ONCE : (this->album_total_cnt - this->album_draw_cnt);
        this->album_draw_cnt += max_cnt;

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
            connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &AppleAlbumListAll_Rose::slot_clickedItemAlbum);
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
    void AppleAlbumListAll_Rose::slot_applyResult_myAlbums_rose(const QList<roseHome::AlbumItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_album->size() == 0){
                this->album_total_cnt = list_data.at(0).totalCount;
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
                    this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                    connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &AppleAlbumListAll_Rose::slot_clickedItemAlbum);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i]->setData(this->list_album->at(i));
                    this->flowLayout_albums->addWidget(this->listAlbum_all[i]);

                    QCoreApplication::processEvents();
                }

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }

            ContentLoadingwaitingMsgHide();
            this->request_more_albumData();
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->flowLayout_albums->addWidget(noData_widget);
        }
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void AppleAlbumListAll_Rose::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___albums){
            roseHome::AlbumItemData data = this->list_album->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Album 바로 재생
                apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
                procRosePlay->requestPlayRose_byAlbumID(data.clientKey.toInt(), OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Album Detail 페이지 진입
                QJsonObject jsonObj_move = QJsonObject();
                jsonObj_move.insert("id", data.clientKey.toInt());
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_APPLE_ALBUM_DETAIL);

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
    void AppleAlbumListAll_Rose::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        Q_UNUSED(clickMode);
        Q_UNUSED(index);

        if(section == SECTION_FOR_MORE_POPUP___albums){
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
                    )
            {
                // Rose Play 요청
                apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
                procRosePlay->requestPlayRose_byAlbumID(this->list_album->at(index).clientKey.toInt(), OptMorePopup::ClickMode::Play_RightNow);
            }
        }
    }
}
