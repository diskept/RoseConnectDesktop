#include "tidal/TidalMyPlaylistAll.h"

#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"
#include "tidal/ProcRosePlay_withTidal.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/global.h"
#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QResizeEvent>
#include <QScrollBar>


namespace tidal {

    // 여기는 겉에서는 하나의 섹션이지만, 내부 데이터가 section 이 2가지로 나뉨
    const int SECTION_FOR_MORE_POPUP___playlists_created = 1;


    /**
     * @brief TidalMoodsMain::TidalMoodsMain
     * @param parent
     */
    TidalMyPlaylistAll::TidalMyPlaylistAll(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // Data
        this->list_playlist_created = new QList<tidal::PlaylistItemData>();        
        this->list_filterOpt_sort = AbstractTidalSubWidget::get_list_filterOptData_forMyCollection();

        // 기본 UI 구성
        this->setUIControl_basic();
    }


    /**
     * @brief 소멸자.
     */
    TidalMyPlaylistAll::~TidalMyPlaylistAll(){

        this->deleteLater();
    }


    void TidalMyPlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

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
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalMyPlaylistAll::setActivePage(){

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

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistData();
        }
    }


    /**
     * @brief TidalMyPlaylistAll::setUIControll_basic
     */
    void TidalMyPlaylistAll::setUIControl_basic(){

        this->addUIControl_mainTitle(tr("My Playlists"));

        // filtering
        this->setUIControl_filter();

        // layout for items
        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &TidalMyPlaylistAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief TidalMyPlaylistAll::setUIControl_filter
     */
    void TidalMyPlaylistAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &TidalMyPlaylistAll::slot_filterClicked);


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
    void TidalMyPlaylistAll::initAll(){

        // init
        this->next_offset = 0;
        this->playlist_total_cnt = 0;
        this->playlist_draw_cnt = 0;

        // init data
        this->list_playlist_created->clear();

        // request HTTP API
        this->flagReqMore_playlist = false;
        this->flag_lastPage_playlist = false;

        this->flag_playlist_draw = false;

        GSCommon::clearLayout(this->flowLayout_playlists);
    }


    /**
     * @brief 스크롤 Bottom에 대하여 Get more data 처리
     */
    void TidalMyPlaylistAll::proc_wheelEvent_to_getMoreData(){

        qDebug() << this->playlist_total_cnt << this->playlist_draw_cnt << this->list_playlist_created->size() << this->flag_playlist_draw;

        if((this->playlist_total_cnt > this->playlist_draw_cnt) && (this->flag_playlist_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_playlist_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistDraw();
        }
    }

    /**
     * @brief My created playlist 삭제 처리에 대해서 UI갱신 처리
     * @param data_itemPos
     */
    void TidalMyPlaylistAll::proc_preUpdate_delete_myCreatePlaylist(const ItemPositionData& data_itemPos){

        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___playlists_created){
            // index : 실제 데이터 QList 인덱스가 아님. 생성 시의 index임
            this->hide_itemWidget_fromLayout(this->flowLayout_playlists, data_itemPos.index, data_itemPos.section);

            // 참고로 list_playlist 데이터에서는 삭제하지 않음. :: 화면 refreshing 하기 전까지, index 데이터를 유지하기 위함
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void TidalMyPlaylistAll::resizeEvent(QResizeEvent *event){//c230223

        AbstractTidalSubWidget::resizeEvent(event);
        print_debug();
        //qDebug() << flowLayout_playlists->sizeHint();
        //qDebug() << this->box_contents->sizeHint();

        //int w = flowLayout_playlists->sizeHint().width();
        int w = 217, l = 80, r = 60, scrollbarW = 0;

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

        flowLayout_playlists->setSpacingHV(0+i,20);
        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }
    }


    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void TidalMyPlaylistAll::slot_filterClicked(){

        if(this->chooseFilterOpt->isVisible()){
            this->chooseFilterOpt->hideFilterBox();
        }
        else{
            // Filter Box Show
            this->chooseFilterOpt->clearFilter();
            this->chooseFilterOpt->addFilterGroup(tr("Sort"), this->list_filterOpt_sort, this->selected_filterCode);
            //this->chooseFilterOpt->addFilterGroup(tr("정렬"), this->list_filterOpt_sort, this->selected_filterCode);
            this->chooseFilterOpt->showFilterBox(QPoint(30, 130), this->size());
        }
    }


    /**
     * @brief 필터링 옵션박스 Widget에서 필터링을 하나 클릭한 경우이다.
     * @param selectedCode
     */
    void TidalMyPlaylistAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

        // 코드가 다른 경우에 대해서만 처리한다.
        if(this->selected_filterCode != selected_filterCode){

            // update
            this->selected_filterCode = selected_filterCode;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(selected_filterCode.toString(), selected_filterName);

            // clear - 옵션코드가 바뀌었으므로 모두 초기화한다.
            this->initAll();

            this->scrollTop();
            this->request_more_playlistData();
        }
    }



    // MARK : TidalPlaylistAll HTTP API ---------------------------------------------------------------------------------

    /**
     * @brief Tidal API 요청 - Tidal playlist 인데, 사용자가 favorite 체크한 playlist 목록을 요청함
     */
    void TidalMyPlaylistAll::request_more_playlistData(){

        if(!this->flagReqMore_playlist && !this->flag_lastPage_playlist){
            this->flagReqMore_playlist = true;

            // j220913 list count check
            int width_cnt;//c230223
            if(flowLayout_playlists->sizeHint().width() < 0) {//c230223
                width_cnt = global.LmtCnt / 217;
            }else{
                width_cnt = global.LmtCnt / flowLayout_playlists->sizeHint().width();//
            }
            int mod = this->playlist_draw_cnt % width_cnt;
            int height_cnt = 0;

            if(mod == 0){
                if(width_cnt > 5){
                    height_cnt = 50 / width_cnt;
                    this->playlist_widget_cnt = width_cnt * height_cnt;
                }
                else{
                    this->playlist_widget_cnt = width_cnt * 10;
                }
            }
            else{
                if(width_cnt > 5){
                    height_cnt = 50 / width_cnt;
                    this->playlist_widget_cnt = width_cnt * height_cnt + (width_cnt - mod);
                }
                else{
                    this->playlist_widget_cnt = width_cnt * 10 + (width_cnt - mod);
                }
            }
            // j220913 list count check

            // next_offset
            this->next_offset = this->list_playlist_created->size();

            // filtering
            QString selected_sortOpt = this->selected_filterCode.toString();
            ProcCommon::DataOrderOption sortOpt = ProcCommon::DataOrderOption::DATE;      // 기본값
            ProcCommon::DataOrderDirection orderDirection = ProcCommon::DataOrderDirection::DESC;      // 기본값
            if(selected_sortOpt == QVariant::fromValue(ProcCommon::DataOrderOption::NAME).toString()){
                sortOpt = ProcCommon::DataOrderOption::NAME;
                orderDirection = ProcCommon::DataOrderDirection::ASC;
            }

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_myCreatedPlaylists, this, &TidalMyPlaylistAll::slot_applyResult_myCreatedPlaylists);
            proc->request_tidal_getList_myCreatedPlaylists(sortOpt, orderDirection, this->playlist_widget_cnt, this->next_offset);
        }
    }


    void TidalMyPlaylistAll::request_more_playlistDraw(){

        // j220913 list count check
        int width_cnt = global.LmtCnt / flowLayout_playlists->sizeHint().width();
        int mod = this->playlist_draw_cnt % width_cnt;
        int height_cnt = 0;

        if(mod == 0){
            if(width_cnt > 5){
                height_cnt = 50 / width_cnt;
                this->playlist_widget_cnt = width_cnt * height_cnt;
            }
            else{
                this->playlist_widget_cnt = width_cnt * 10;
            }
        }
        else{
            if(width_cnt > 5){
                height_cnt = 50 / width_cnt;
                this->playlist_widget_cnt = width_cnt * height_cnt + (width_cnt - mod);
            }
            else{
                this->playlist_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
        }
        // j220913 list count check

        int start_index = this->playlist_draw_cnt;
        int max_cnt = ((this->playlist_total_cnt - this->playlist_draw_cnt) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_total_cnt - this->playlist_draw_cnt);
        this->playlist_draw_cnt += max_cnt;

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i] = new tidal::ItemPlaylist(i, SECTION_FOR_MORE_POPUP___playlists_created, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->listPlaylist_all[i], &tidal::ItemPlaylist::signal_clicked, this, &TidalMyPlaylistAll::slot_clickedItemPlaylist);
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i]->setData(this->list_playlist_created->at(i));
            this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_playlist_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief
     * @param list_data
     */
    void TidalMyPlaylistAll::slot_applyResult_myCreatedPlaylists(const QList<tidal::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        /*Q_UNUSED(flag_lastPage);
        int start_index = this->list_playlist_created->count();

        this->list_playlist_created->append(list_data);
        //this->createList_itemPlaylsit_applyingWithData(list_data, AbstractItem::ImageSizeMode::Square_186x186, this->flowLayout_playlists, start_index, SECTION_FOR_MORE_POPUP___playlists_created);
        this->createList_itemPlaylsit_applyingWithData(list_data, AbstractItem::ImageSizeMode::Square_200x200, this->flowLayout_playlists, start_index, SECTION_FOR_MORE_POPUP___playlists_created);

        // request HTTP API - tidal playlist
        //this->request_more_playlistData_tidal();*/


        ContentLoadingwaitingMsgHide();

        if(list_data.size() > 0){
            this->flagReqMore_playlist = false;
            this->flag_lastPage_playlist = flag_lastPage;

            if(this->list_playlist_created->size() == 0){
                this->playlist_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_playlist_created->size();
            this->list_playlist_created->append(list_data);

            if(start_index == 0){
                int max_cnt = this->list_playlist_created->size();
                this->playlist_draw_cnt = max_cnt;

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->listPlaylist_all[i] = new tidal::ItemPlaylist(i, SECTION_FOR_MORE_POPUP___playlists_created, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->listPlaylist_all[i], &tidal::ItemPlaylist::signal_clicked, this, &TidalMyPlaylistAll::slot_clickedItemPlaylist);
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->listPlaylist_all[i]->setData(this->list_playlist_created->at(i));
                    this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);

                    QCoreApplication::processEvents();
                }

                ContentLoadingwaitingMsgHide();      // 20230314

                this->flag_flow_draw = true;
                this->flag_playlist_draw = false;
            }

            this->request_more_playlistData();
        }
        else{
            if(this->list_playlist_created->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->flowLayout_playlists->addWidget(noData_widget);
            }
        }
    }


    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalMyPlaylistAll::slot_clickedItemPlaylist(const ItemPlaylist::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___playlists_created){
            this->proc_clicked_itemPlaylist(this->list_playlist_created, clickMode, index, section);
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
    void TidalMyPlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___playlists_created){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist_created, index, section, clickMode);
        }
    }





    /**
     * @brief TidalRecentlyPlaylistAll::TidalRecentlyPlaylistAll
     * @param parent
     */
    TidalRecentlyPlaylistAll::TidalRecentlyPlaylistAll(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        this->linker = Linker::getInstance();

        // 기본 UI 세팅
        this->setUIControl_playlist();

        // Data
        this->list_playlist = new QList<roseHome::PlaylistItemData>();
        this->list_filterOpt_sort = AbstractTidalSubWidget::get_list_filterOptData_forMyCollection();
    }


    /**
     * @brief 소멸자.
     */
    TidalRecentlyPlaylistAll::~TidalRecentlyPlaylistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalRecentlyPlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        QString mainTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(pageCode != this->page || this->title != mainTitle){
            this->flagNeedReload = true;

            this->page = pageCode;
            this->title = mainTitle;

            this->label_mainTitle->setText(mainTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

            // init
            this->next_offset = 0;
            this->playlist_total_cnt = 0;
            this->playlist_draw_cnt = 0;

            // init data
            this->list_playlist->clear();

            // request HTTP API
            this->flagReqMore_playlist = false;
            this->flag_lastPage_playlist = false;

            this->flag_playlist_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            roseHome::ProcCommon *proc_recently = new roseHome::ProcCommon(this);
            connect(proc_recently, &roseHome::ProcCommon::completeReq_list_playlists, this, &TidalRecentlyPlaylistAll::slot_applyResult_playlistsCheck);
            proc_recently->request_rose_getList_recentlyPlaylists("member/playlist/recent", "TIDAL", 0, 10);
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalRecentlyPlaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            // init UI
            GSCommon::clearLayout(this->flowLayout_playlists);

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
        }
    }


    /**
     * @brief TidalRecentlyPlaylistAll::setUIControll_basic
     */
    void TidalRecentlyPlaylistAll::setUIControl_playlist(){

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

        connect(this->btn_mainTitle, &QPushButton::clicked, this, &TidalRecentlyPlaylistAll::slot_btnClicked_Delete);

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

        roseHome::ItemPlaylist_rosehome *listAlbum = new roseHome::ItemPlaylist_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);

        this->playlist_widget_width = listAlbum->get_fixedWidth();
        this->playlist_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeAlbumListAll::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;

        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &TidalRecentlyPlaylistAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief TidalRecentlyPlaylistAll::setUIControl_filter
     */
    void TidalRecentlyPlaylistAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &TidalRecentlyPlaylistAll::slot_filterClicked);


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
    void TidalRecentlyPlaylistAll::initAll(){

        // init
        this->next_offset = 0;
        this->playlist_total_cnt = 0;
        this->playlist_draw_cnt = 0;

        this->list_playlist->clear();

        // request HTTP API
        this->flagReqMore_playlist = false;
        this->flag_lastPage_playlist = false;

        this->flag_playlist_draw = false;

        GSCommon::clearLayout(this->flowLayout_playlists);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤 Bottom에 대하여 Get more data 처리
     */
    void TidalRecentlyPlaylistAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_playlist && !this->flag_lastPage_playlist) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_playlist = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistData();
        }
        else if((this->playlist_total_cnt > this->playlist_draw_cnt) && this->flag_playlist_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void TidalRecentlyPlaylistAll::resizeEvent(QResizeEvent *event){

        AbstractTidalSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }

        this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

    }


    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void TidalRecentlyPlaylistAll::slot_filterClicked(){

        if(this->chooseFilterOpt->isVisible()){
            this->chooseFilterOpt->hideFilterBox();
        }
        else{
            // Filter Box Show
            this->chooseFilterOpt->clearFilter();
            this->chooseFilterOpt->addFilterGroup(tr("Sort"), this->list_filterOpt_sort, this->selected_filterCode);
            //this->chooseFilterOpt->addFilterGroup(tr("정렬"), this->list_filterOpt_sort, this->selected_filterCode);
            this->chooseFilterOpt->showFilterBox(QPoint(30, 130), this->size());
        }
    }

    /**
     * @brief 필터링 옵션박스 Widget에서 필터링을 하나 클릭한 경우이다.
     * @param selectedCode
     */
    void TidalRecentlyPlaylistAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

        // 코드가 다른 경우에 대해서만 처리한다.
        if(this->selected_filterCode != selected_filterCode){

            // update
            this->selected_filterCode = selected_filterCode;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(selected_filterCode.toString(), selected_filterName);

            // clear - 옵션코드가 바뀌었으므로 모두 초기화한다.
            //this->initAll();

            this->scrollTop();
            //this->request_playlistData_created();
        }

    }




    // MARK : QobuzPlaylistAll HTTP API ---------------------------------------------------------------------------------

    /**
     * @brief Qobuz API 요청 - 사용자가 생성한 playlist
     */
    void TidalRecentlyPlaylistAll::request_more_playlistData(){

        // next_offset
        if(this->list_playlist->size() == 0){
            this->next_offset = 0;
        }
        else{
            this->next_offset++;
        }


        // filtering
        /*QString selected_sortOpt = this->selected_filterCode.toString();
        ProcCommon::DataOrderOption sortOpt = ProcCommon::DataOrderOption::NAME;      // 기본값
        ProcCommon::DataOrderDirection orderDirection = ProcCommon::DataOrderDirection::ASC;      // 기본값

        if(selected_sortOpt == QVariant::fromValue(ProcCommon::DataOrderOption::DATE).toString()){
            sortOpt = ProcCommon::DataOrderOption::DATE;
            orderDirection = ProcCommon::DataOrderDirection::DESC;
        }*/

        // request HTTP API
        roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
        connect(proc, &roseHome::ProcCommon::completeReq_list_playlists, this, &TidalRecentlyPlaylistAll::slot_applyResult_myPlaylist_rose);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_recentlyPlaylists("member/playlist/recent", "TIDAL", this->next_offset, 200);

        if(this->next_offset == 0){
            this->flag_playlist_draw = true;
        }

    }


    void TidalRecentlyPlaylistAll::request_more_playlistDraw(){

        this->flag_playlist_draw = true;

        int f_width = this->flowLayout_playlists->geometry().width();

        if(this->flowLayout_playlists->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->playlist_widget_width + this->playlist_widget_margin);
        int f_mod = this->playlist_draw_cnt % f_wg_cnt;

        if(f_mod == 0){
            this->playlist_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->playlist_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        //qDebug() << "[Debug] RoseHomeAlbumListAll::request_more_albumDraw " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

        int start_index = this->playlist_draw_cnt;
        int max_cnt = ((this->playlist_total_cnt - this->playlist_draw_cnt) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_total_cnt - this->playlist_draw_cnt);
        this->playlist_draw_cnt += max_cnt;

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists_created, tidal::AbstractItem::ImageSizeMode::Square_200x200, 2, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);
            connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &TidalRecentlyPlaylistAll::slot_clickedItemPlaylist);
        }

        ContentLoadingwaitingMsgHide();

        this->flag_playlist_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief
     * @param list_data
     */
    void TidalRecentlyPlaylistAll::slot_applyResult_myPlaylist_rose(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        int idx = sender()->property("idx").toInt();

        if(idx != this->next_offset){
            return;
        }

        if(list_data.size() > 0){
            this->flagReqMore_playlist = false;
            this->flag_lastPage_playlist = flag_lastPage;

            if(this->list_playlist->size() == 0){
                this->playlist_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_playlist->size();
            this->list_playlist->append(list_data);

            if(start_index == 0){
                int f_width = this->flowLayout_playlists->geometry().width();

                if(this->flowLayout_playlists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                int f_wg_cnt = f_width / (this->playlist_widget_width + this->playlist_widget_margin);
                int f_mod = this->playlist_draw_cnt % f_wg_cnt;

                if(f_mod == 0){
                    this->playlist_widget_cnt = f_wg_cnt * 10;
                }
                else{
                    this->playlist_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                }

                //qDebug() << "[Debug] RoseHomeAlbumListAll::slot_applyResult_albums " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

                int start_index = this->playlist_draw_cnt;
                int max_cnt = ((this->playlist_total_cnt - this->playlist_draw_cnt) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_total_cnt - this->playlist_draw_cnt);
                this->playlist_draw_cnt += max_cnt;

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->listPlaylist_all[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists_created, tidal::AbstractItem::ImageSizeMode::Square_200x200, 2, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);
                    connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &TidalRecentlyPlaylistAll::slot_clickedItemPlaylist);
                }

                ContentLoadingwaitingMsgHide();      //cheon Tidal

                this->flag_flow_draw = true;
                this->flag_playlist_draw = false;
            }
            else{
                this->request_more_playlistDraw();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            if(this->playlist_draw_cnt <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_playlists->geometry().width();

                if(this->flowLayout_playlists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_playlists->addWidget(noData_widget);
            }
        }
    }


    void TidalRecentlyPlaylistAll::slot_applyResult_playlistsCheck(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_playlist->count() == 0){

            }
            else{
                for(int i = 0; i < list_data.count(); i++){
                    if(list_data.at(i).title != this->list_playlist->at(i).title){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_playlist->at(0).totalCount) || (change_flag > 0)){

                    QString tmpTitle = tr("Recently Played Playlists") + QString(" (%1)").arg(list_data.at(0).totalCount);
                    this->label_mainTitle->setText(tmpTitle);
                    this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                    this->initAll();

                    ContentLoadingwaitingMsgHide();
                    this->request_more_playlistData();

                    this->flagNeedReload = true;
                    this->setActivePage();
                }
                else{
                    ContentLoadingwaitingMsgHide();
                }
            }
        }
        else if(list_data.size() == 0){

            QString tmpTitle = tr("Recently Played Playlists (0)");
            this->label_mainTitle->setText(tmpTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

            this->initAll();

            ContentLoadingwaitingMsgHide();
            this->request_more_playlistData();

            this->flagNeedReload = true;
            this->setActivePage();
        }
        else{
            ContentLoadingwaitingMsgHide();
        }
    }


    void TidalRecentlyPlaylistAll::slot_btnClicked_Delete(){

        QStringList tmpTitle = this->title.split("(");

        QJsonObject tmpObj;
        tmpObj.insert("contents_type", tmpTitle.at(0));
        tmpObj.insert("list_type", "PLAYLIST");
        tmpObj.insert("api_subPath", "member/playlist/recent");
        tmpObj.insert("filter_type", "TIDAL");

        QJsonObject tmpObj_move;
        tmpObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_RECENTLY_LIST_DELETE);
        tmpObj_move.insert(KEY_DATA, tmpObj);

        emit this->linker->signal_clickedMovePage(tmpObj_move);
    }


    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalRecentlyPlaylistAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){


        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___playlists_created){
            roseHome::PlaylistItemData data = this->list_playlist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                if(data.type == "TIDAL" && data.isRose == false){
                    tidal::PlaylistItemData tmpPlaylist;
                    tmpPlaylist.uuid = data.clientKey;

                    if(data.title.contains("Mix")){
                        tmpPlaylist.description = "mixes";
                    }

                    tidal::ProcRosePlay_withTidal *procRosePlay = new tidal::ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_byPlaylistUUID(tmpPlaylist, OptMorePopup::ClickMode::Play_RightNow);
                }
                else{
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byPlaylistID(data.id, OptMorePopup::ClickMode::Play_RightNow);
                }
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입
                if(data.isRose == false){
                    if(data.ownerName == "tidal" && data.title.contains("Mix")){
                        QJsonObject jsonObj_move = QJsonObject();
                        jsonObj_move.insert("uuid", data.clientKey);
                        jsonObj_move.insert("title", data.title);
                        jsonObj_move.insert("image", data.thumbnail);
                        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MIXES_DETAIL);

                        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                    }
                    else{
                        if(data.clientKey.isEmpty()){
                            QJsonObject jsonObj_move = QJsonObject();
                            jsonObj_move.insert("id", data.id);
                            jsonObj_move.insert("ownerId", data.ownerId);
                            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_ROSE_PLAYLIST_DETAIL);

                            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                        }
                        else{
                            QJsonObject jsonObj_move = QJsonObject();
                            jsonObj_move.insert("uuid", data.clientKey);
                            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_PLAYLIST_DETAIL);

                            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                        }
                    }
                }
                else{
                    QJsonObject jsonObj_move = QJsonObject();
                    jsonObj_move.insert("id", data.id);
                    jsonObj_move.insert("ownerId", data.ownerId);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_ROSE_PLAYLIST_DETAIL);

                    emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                }
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){

                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
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
    void TidalRecentlyPlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___playlists_created){

            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                if(this->list_playlist->at(index).type == "TIDAL" && this->list_playlist->at(index).isRose == false){
                    tidal::PlaylistItemData tmpPlaylist;
                    tmpPlaylist.uuid = this->list_playlist->at(index).clientKey;

                    if(this->list_playlist->at(index).title.contains("Mix")){
                        tmpPlaylist.description = "mixes";
                    }

                    tidal::ProcRosePlay_withTidal *procRosePlay = new tidal::ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_byPlaylistUUID(tmpPlaylist, OptMorePopup::ClickMode::Play_RightNow);
                }
                else{
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byPlaylistID(this->list_playlist->at(index).id, clickMode);
                }
            }
            else if(clickMode == OptMorePopup::ClickMode::Share){//c220823

                setUIShare();
            }
        }
    }





    /**
     * @brief RoseHomePlaylistListAll::RoseHomePlaylistListAll
     * @param parent
     */
    TidalPlaylistHistoryAll::TidalPlaylistHistoryAll(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // 기본 UI 세팅
        this->setUIControl_playlist();

        this->list_playlist = new QList<roseHome::PlaylistItemData>();
    }

    /**
         * @brief 소멸자.
         */
    TidalPlaylistHistoryAll::~TidalPlaylistHistoryAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalPlaylistHistoryAll::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::PageInfo_PlaylistAllView tmp_pageInfo = roseHome::ConvertData::convertData_pageInfo_playlistAllView(jsonObj);
        this->flagNeedReload = false;

        if(tmp_pageInfo.api_subPath != this->data_pageInfo.api_subPath || tmp_pageInfo.pathTitle != this->data_pageInfo.pathTitle || tmp_pageInfo.filter_type != this->data_pageInfo.filter_type){
            this->flagNeedReload = true;

            this->data_pageInfo = tmp_pageInfo;

            this->label_mainTitle->setText(this->data_pageInfo.pathTitle);
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalPlaylistHistoryAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            // init
            this->list_playlist->clear();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
            GSCommon::clearLayout(this->flowLayout_playlists);

            this->flagReqMore_playlist = false;
            this->flag_lastPage_playlist = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_Data();
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
    void TidalPlaylistHistoryAll::setUIControl_playlist(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("");

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        roseHome::ItemPlaylist_rosehome *listAlbum = new roseHome::ItemPlaylist_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);

        this->playlist_widget_width = listAlbum->get_fixedWidth();
        this->playlist_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeAlbumListAll::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;

        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
    }

    /**
     * @brief 스크롤 Bottom에 대하여 Get more data 처리
     */
    void TidalPlaylistHistoryAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_playlist && !this->flag_lastPage_playlist) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_playlist = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_Data();
        }
        else if((this->playlist_totalCount > this->playlist_drawCount) && this->flag_playlist_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_Draw();
        }
    }

    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void TidalPlaylistHistoryAll::resizeEvent(QResizeEvent *event){//c230223

        AbstractTidalSubWidget::resizeEvent(event);

        this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

    }

    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    void TidalPlaylistHistoryAll::request_more_Data(){

        // next_offset
        if(this->list_playlist->size() == 0){
            this->playlist_next_offset = 0;
        }
        else{
            this->playlist_next_offset++;
        }

        roseHome::ProcCommon *proc_playlist = new roseHome::ProcCommon(this);
        connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_playlists, this, &TidalPlaylistHistoryAll::slot_applyResult_historyPlaylist);
        proc_playlist->setProperty("idx", this->playlist_next_offset);
        proc_playlist->request_rose_getList_historyPlaylists(this->data_pageInfo.api_subPath, this->data_pageInfo.filter_type, this->playlist_next_offset, 200);

        if(this->playlist_next_offset == 0){
            this->flag_playlist_draw = true;
        }
    }


    void TidalPlaylistHistoryAll::request_more_Draw(){

        this->flag_playlist_draw = true;

        int f_width = this->flowLayout_playlists->geometry().width();

        if(this->flowLayout_playlists->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->playlist_widget_width + this->playlist_widget_margin);
        int f_mod = this->playlist_drawCount % f_wg_cnt;

        if(f_mod == 0){
            this->playlist_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->playlist_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        //qDebug() << "[Debug] RoseHomeAlbumListAll::request_more_albumDraw " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

        int start_index = this->playlist_drawCount;
        int max_cnt = ((this->playlist_totalCount - this->playlist_drawCount) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_totalCount - this->playlist_drawCount);
        this->playlist_drawCount += max_cnt;

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists_created, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->widget_playlist[i]->setData(this->list_playlist->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
            connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &TidalPlaylistHistoryAll::slot_clickedItemPlaylist);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_playlist_draw = false;
    }


    // MARK : User Event Handler (slots) ---------------------------------------------------------------------------------

    void TidalPlaylistHistoryAll::slot_applyResult_historyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        int idx = sender()->property("idx").toInt();

        if(idx != this->playlist_next_offset){
            return;
        }

        if(list_data.size() > 0){
            this->flag_lastPage_playlist = flag_lastPage;
            this->flagReqMore_playlist = false;

            if(this->list_playlist->count() == 0){
                this->playlist_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_playlist->count();
            this->list_playlist->append(list_data);

            if(start_index == 0){
                int f_width = this->flowLayout_playlists->geometry().width();

                if(this->flowLayout_playlists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                int f_wg_cnt = f_width / (this->playlist_widget_width + this->playlist_widget_margin);
                int f_mod = this->playlist_drawCount % f_wg_cnt;

                if(f_mod == 0){
                    this->playlist_widget_cnt = f_wg_cnt * 10;
                }
                else{
                    this->playlist_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                }

                //qDebug() << "[Debug] RoseHomeAlbumListAll::slot_applyResult_albums " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

                int start_index = this->playlist_drawCount;
                int max_cnt = ((this->playlist_totalCount - this->playlist_drawCount) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_totalCount - this->playlist_drawCount);
                this->playlist_drawCount += max_cnt;

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists_created, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i]->setData(this->list_playlist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &TidalPlaylistHistoryAll::slot_clickedItemPlaylist);
                }

                this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_playlist_draw = false;
            }
            else{
                this->request_more_Draw();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            if(this->playlist_drawCount <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_playlists->geometry().width();

                if(this->flowLayout_playlists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_playlists->addWidget(noData_widget);
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
    void TidalPlaylistHistoryAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___playlists_created){
            //this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);
            roseHome::PlaylistItemData data = this->list_playlist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                if(data.type == "TIDAL" && data.isRose == false){
                    tidal::PlaylistItemData tmpPlaylist;
                    tmpPlaylist.uuid = data.clientKey;

                    if(data.title.contains("Mix")){
                        tmpPlaylist.description = "mixes";
                    }

                    tidal::ProcRosePlay_withTidal *procRosePlay = new tidal::ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_byPlaylistUUID(tmpPlaylist, OptMorePopup::ClickMode::Play_RightNow);
                }
                else{
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byPlaylistID(this->list_playlist->at(index).id, OptMorePopup::ClickMode::Play_RightNow);
                }
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입
                if(data.isRose == false){
                    if(data.ownerName == "tidal" && data.title.contains("Mix")){
                        QJsonObject jsonObj_move = QJsonObject();
                        jsonObj_move.insert("uuid", data.clientKey);
                        jsonObj_move.insert("title", data.title);
                        jsonObj_move.insert("image", data.thumbnail);
                        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MIXES_DETAIL);

                        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                    }
                    else{
                        if(data.clientKey.isEmpty()){
                            QJsonObject jsonObj_move = QJsonObject();
                            jsonObj_move.insert("id", data.id);
                            jsonObj_move.insert("ownerId", data.ownerId);
                            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_ROSE_PLAYLIST_DETAIL);

                            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                        }
                        else{
                            QJsonObject jsonObj_move = QJsonObject();
                            jsonObj_move.insert("uuid", data.clientKey);
                            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_PLAYLIST_DETAIL);

                            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                        }
                    }
                }
                else{
                    QJsonObject jsonObj_move = QJsonObject();
                    jsonObj_move.insert("id", data.id);
                    jsonObj_move.insert("ownerId", data.ownerId);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_ROSE_PLAYLIST_DETAIL);

                    emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                }
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){//c220826_2
                // OptionPopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
            }
        }
    }


    void TidalPlaylistHistoryAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___playlists_created){
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            else if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
                    )
            {
                if(this->list_playlist->at(index).type == "TIDAL" && this->list_playlist->at(index).isRose == false){
                    tidal::PlaylistItemData tmpPlaylist;
                    tmpPlaylist.uuid = this->list_playlist->at(index).clientKey;

                    if(this->list_playlist->at(index).title.contains("Mix")){
                        tmpPlaylist.description = "mixes";
                    }

                    tidal::ProcRosePlay_withTidal *procRosePlay = new tidal::ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_byPlaylistUUID(tmpPlaylist, clickMode);
                }
                else{
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byPlaylistID(this->list_playlist->at(index).id, clickMode);
                }
            }
        }
    }





    /**
     * @brief QobuzMoodsMain::QobuzMoodsMain
     * @param parent
     */
    TidalMyRosePlaylistAll::TidalMyRosePlaylistAll(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // 기본 UI 세팅
        this->setUIControl_playlist();

        // Data
        this->list_playlist = new QList<roseHome::PlaylistItemData>();
        this->list_filterOpt_sort = AbstractTidalSubWidget::get_list_filterOptData_forMyCollection();
    }


    /**
     * @brief 소멸자.
     */
    TidalMyRosePlaylistAll::~TidalMyRosePlaylistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalMyRosePlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        QString mainTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(pageCode != this->page_reload_check){
            this->flagNeedReload = true;

            this->page_reload_check = pageCode;

            this->label_mainTitle->setText(mainTitle);

            // init
            this->next_offset = 0;
            this->playlist_total_cnt = 0;
            this->playlist_draw_cnt = 0;

            // init data
            this->list_playlist->clear();

            // request HTTP API
            this->flagReqMore_playlist = false;
            this->flag_lastPage_playlist = false;

            this->flag_playlist_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalMyRosePlaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            // init UI
            GSCommon::clearLayout(this->flowLayout_playlists);

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
        }
    }



    /**
     * @brief QobuzMyPlaylistAll::setUIControll_basic
     */
    void TidalMyRosePlaylistAll::setUIControl_playlist(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Playlists");

        // filtering
        //this->setUIControl_filter();
        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        roseHome::ItemPlaylist_rosehome *listAlbum = new roseHome::ItemPlaylist_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);

        this->playlist_widget_width = listAlbum->get_fixedWidth();
        this->playlist_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeAlbumListAll::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;

        // layout for items
        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &TidalMyRosePlaylistAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyPlaylistAll::setUIControl_filter
     */
    void TidalMyRosePlaylistAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &TidalMyRosePlaylistAll::slot_filterClicked);


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

    }


    /**
     * @brief 스크롤 Bottom에 대하여 Get more data 처리
     */
    void TidalMyRosePlaylistAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_playlist && !this->flag_lastPage_playlist) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_playlist = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistData();
        }
        else if((this->playlist_total_cnt > this->playlist_draw_cnt) && this->flag_playlist_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void TidalMyRosePlaylistAll::resizeEvent(QResizeEvent *event){

        AbstractTidalSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }
        this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

    }


    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void TidalMyRosePlaylistAll::slot_filterClicked(){

        if(this->chooseFilterOpt->isVisible()){
            this->chooseFilterOpt->hideFilterBox();
        }
        else{
            // Filter Box Show
            this->chooseFilterOpt->clearFilter();
            this->chooseFilterOpt->addFilterGroup(tr("Sort"), this->list_filterOpt_sort, this->selected_filterCode);
            //this->chooseFilterOpt->addFilterGroup(tr("정렬"), this->list_filterOpt_sort, this->selected_filterCode);
            this->chooseFilterOpt->showFilterBox(QPoint(30, 130), this->size());
        }
    }

    /**
     * @brief 필터링 옵션박스 Widget에서 필터링을 하나 클릭한 경우이다.
     * @param selectedCode
     */
    void TidalMyRosePlaylistAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

        // 코드가 다른 경우에 대해서만 처리한다.
        if(this->selected_filterCode != selected_filterCode){

            // update
            this->selected_filterCode = selected_filterCode;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(selected_filterCode.toString(), selected_filterName);

            // clear - 옵션코드가 바뀌었으므로 모두 초기화한다.
            //this->initAll();

            this->scrollTop();
            //this->request_playlistData_created();
        }

    }




    // MARK : QobuzPlaylistAll HTTP API ---------------------------------------------------------------------------------

    /**
     * @brief Qobuz API 요청 - 사용자가 생성한 playlist
     */
    void TidalMyRosePlaylistAll::request_more_playlistData(){

        // next_offset
        if(this->list_playlist->size() == 0){
            this->next_offset = 0;
        }
        else{
            this->next_offset++;
        }


        // filtering
        /*QString selected_sortOpt = this->selected_filterCode.toString();
        ProcCommon::DataOrderOption sortOpt = ProcCommon::DataOrderOption::NAME;      // 기본값
        ProcCommon::DataOrderDirection orderDirection = ProcCommon::DataOrderDirection::ASC;      // 기본값

        if(selected_sortOpt == QVariant::fromValue(ProcCommon::DataOrderOption::DATE).toString()){
            sortOpt = ProcCommon::DataOrderOption::DATE;
            orderDirection = ProcCommon::DataOrderDirection::DESC;
        }*/

        // request HTTP API
        roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
        connect(proc, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &TidalMyRosePlaylistAll::slot_applyResult_myPlaylist_rose);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_myPlaylists("member/playlist", "TIDAL", this->next_offset, 200);

        if(this->next_offset == 0){
            this->flag_playlist_draw = true;
        }

    }


    void TidalMyRosePlaylistAll::request_more_playlistDraw(){

        this->flag_playlist_draw = true;

        int f_width = this->flowLayout_playlists->geometry().width();

        if(this->flowLayout_playlists->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->playlist_widget_width + this->playlist_widget_margin);
        int f_mod = this->playlist_draw_cnt % f_wg_cnt;

        if(f_mod == 0){
            this->playlist_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->playlist_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        //qDebug() << "[Debug] RoseHomeAlbumListAll::request_more_albumDraw " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

        int start_index = this->playlist_draw_cnt;
        int max_cnt = ((this->playlist_total_cnt - this->playlist_draw_cnt) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_total_cnt - this->playlist_draw_cnt);
        this->playlist_draw_cnt += max_cnt;


        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists_created, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);
            connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &TidalMyRosePlaylistAll::slot_clickedItemPlaylist);
        }

        ContentLoadingwaitingMsgHide();      //cheon Tidal

        this->flag_playlist_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief
     * @param list_data
     */
    void TidalMyRosePlaylistAll::slot_applyResult_myPlaylist_rose(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        int idx = sender()->property("idx").toInt();

        if(idx != this->next_offset){
            return;
        }

        if(list_data.size() > 0){
            this->flagReqMore_playlist = false;
            this->flag_lastPage_playlist = flag_lastPage;

            if(this->list_playlist->size() == 0){
                this->playlist_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_playlist->size();
            this->list_playlist->append(list_data);

            if(start_index == 0){
                int f_width = this->flowLayout_playlists->geometry().width();

                if(this->flowLayout_playlists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                int f_wg_cnt = f_width / (this->playlist_widget_width + this->playlist_widget_margin);
                int f_mod = this->playlist_draw_cnt % f_wg_cnt;

                if(f_mod == 0){
                    this->playlist_widget_cnt = f_wg_cnt * 10;
                }
                else{
                    this->playlist_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                }

                int start_index = this->playlist_draw_cnt;
                int max_cnt = ((this->playlist_total_cnt - this->playlist_draw_cnt) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_total_cnt - this->playlist_draw_cnt);
                this->playlist_draw_cnt += max_cnt;

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->listPlaylist_all[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists_created, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);
                    connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &TidalMyRosePlaylistAll::slot_clickedItemPlaylist);
                }

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_playlist_draw = false;
            }
            else{
                this->request_more_playlistDraw();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->playlist_draw_cnt <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_playlists->geometry().width();

                if(this->flowLayout_playlists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_playlists->addWidget(noData_widget);
            }
        }
    }


    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalMyRosePlaylistAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){


        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___playlists_created){
            roseHome::PlaylistItemData data = this->list_playlist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입
                QJsonObject jsonObj_move = QJsonObject();
                jsonObj_move.insert("id", data.id);
                jsonObj_move.insert("ownerId", data.ownerId);
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_ROSE_PLAYLIST_DETAIL);

                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                print_debug();
                // OptionPopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
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
    void TidalMyRosePlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___playlists_created){
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            else if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(this->list_playlist->at(index).id, clickMode);
            }
            else if(clickMode == OptMorePopup::ClickMode::Edit){
                QString view_type = "edit";

                QJsonObject data;
                data.insert("view_type", view_type);
                data.insert("playlist_id", this->list_playlist->at(index).id);
                data.insert("type", "ROSE");

                QJsonObject jsonObj_move;
                jsonObj_move.insert("data", data);

                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_ADD_PLAYLIST);

                emit this->signal_clickedMovePage(jsonObj_move);
            }
        }
    }





    /**
     * @brief TidalUserRosePlaylistAll::TidalUserRosePlaylistAll
     * @param parent
     */
    TidalUserRosePlaylistAll::TidalUserRosePlaylistAll(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // 기본 UI 세팅
        this->setUIControl_playlist();

        // Data
        this->list_playlist = new QList<roseHome::PlaylistItemData>();
        this->list_filterOpt_sort = AbstractTidalSubWidget::get_list_filterOptData_forMyCollection();
    }


    /**
     * @brief 소멸자.
     */
    TidalUserRosePlaylistAll::~TidalUserRosePlaylistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalUserRosePlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        QString mainTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(pageCode != this->page_reload_check){
            this->flagNeedReload = true;

            this->page_reload_check = pageCode;

            this->label_mainTitle->setText(mainTitle);

            // init
            this->next_offset = 0;
            this->playlist_total_cnt = 0;
            this->playlist_draw_cnt = 0;

            // init data
            this->list_playlist->clear();

            // request HTTP API
            this->flagReqMore_playlist = false;
            this->flag_lastPage_playlist = false;

            this->flag_playlist_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalUserRosePlaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            // init UI
            GSCommon::clearLayout(this->flowLayout_playlists);

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
        }
    }



    /**
     * @brief TidalUserRosePlaylistAll::setUIControll_basic
     */
    void TidalUserRosePlaylistAll::setUIControl_playlist(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Playlists");

        // filtering
        //this->setUIControl_filter();

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        roseHome::ItemPlaylist_rosehome *listAlbum = new roseHome::ItemPlaylist_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);

        this->playlist_widget_width = listAlbum->get_fixedWidth();
        this->playlist_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeAlbumListAll::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;
        // layout for items
        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &TidalUserRosePlaylistAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyPlaylistAll::setUIControl_filter
     */
    void TidalUserRosePlaylistAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &TidalUserRosePlaylistAll::slot_filterClicked);


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
     * @brief 스크롤 Bottom에 대하여 Get more data 처리
     */
    void TidalUserRosePlaylistAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_playlist && !this->flag_lastPage_playlist) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_playlist = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistData();
        }
        else if((this->playlist_total_cnt > this->playlist_draw_cnt) && this->flag_playlist_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void TidalUserRosePlaylistAll::resizeEvent(QResizeEvent *event){

        AbstractTidalSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }

        this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);
    }


    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void TidalUserRosePlaylistAll::slot_filterClicked(){

        if(this->chooseFilterOpt->isVisible()){
            this->chooseFilterOpt->hideFilterBox();
        }
        else{
            // Filter Box Show
            this->chooseFilterOpt->clearFilter();
            this->chooseFilterOpt->addFilterGroup(tr("Sort"), this->list_filterOpt_sort, this->selected_filterCode);
            //this->chooseFilterOpt->addFilterGroup(tr("정렬"), this->list_filterOpt_sort, this->selected_filterCode);
            this->chooseFilterOpt->showFilterBox(QPoint(30, 130), this->size());
        }

    }

    /**
     * @brief 필터링 옵션박스 Widget에서 필터링을 하나 클릭한 경우이다.
     * @param selectedCode
     */
    void TidalUserRosePlaylistAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

        // 코드가 다른 경우에 대해서만 처리한다.
        if(this->selected_filterCode != selected_filterCode){

            // update
            this->selected_filterCode = selected_filterCode;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(selected_filterCode.toString(), selected_filterName);

            // clear - 옵션코드가 바뀌었으므로 모두 초기화한다.
            //this->initAll();

            this->scrollTop();
            //this->request_playlistData_created();
        }

    }




    // MARK : QobuzPlaylistAll HTTP API ---------------------------------------------------------------------------------

    /**
     * @brief Qobuz API 요청 - 사용자가 생성한 playlist
     */
    void TidalUserRosePlaylistAll::request_more_playlistData(){

        // next_offset
        if(this->list_playlist->size() == 0){
            this->next_offset = 0;
        }
        else{
            this->next_offset++;
        }


        // filtering
        /*QString selected_sortOpt = this->selected_filterCode.toString();
        ProcCommon::DataOrderOption sortOpt = ProcCommon::DataOrderOption::NAME;      // 기본값
        ProcCommon::DataOrderDirection orderDirection = ProcCommon::DataOrderDirection::ASC;      // 기본값

        if(selected_sortOpt == QVariant::fromValue(ProcCommon::DataOrderOption::DATE).toString()){
            sortOpt = ProcCommon::DataOrderOption::DATE;
            orderDirection = ProcCommon::DataOrderDirection::DESC;
        }*/

        // request HTTP API
        roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
        connect(proc, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &TidalUserRosePlaylistAll::slot_applyResult_userPlaylist_rose);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_usersPlaylists("member/playlist/all", "PLAYLIST_RECENT", "TIDAL", this->next_offset, 200);

        if(this->next_offset == 0){
            this->flag_playlist_draw = true;
        }

    }


    void TidalUserRosePlaylistAll::request_more_playlistDraw(){

        this->flag_playlist_draw = true;

        int f_width = this->flowLayout_playlists->geometry().width();

        if(this->flowLayout_playlists->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->playlist_widget_width + this->playlist_widget_margin);
        int f_mod = this->playlist_draw_cnt % f_wg_cnt;

        if(f_mod == 0){
            this->playlist_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->playlist_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        //qDebug() << "[Debug] RoseHomeAlbumListAll::request_more_albumDraw " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

        int start_index = this->playlist_draw_cnt;
        int max_cnt = ((this->playlist_total_cnt - this->playlist_draw_cnt) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_total_cnt - this->playlist_draw_cnt);
        this->playlist_draw_cnt += max_cnt;


        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists_created, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);
            connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &TidalUserRosePlaylistAll::slot_clickedItemPlaylist);
        }

        ContentLoadingwaitingMsgHide();      //cheon Tidal

        this->flag_playlist_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief
     * @param list_data
     */
    void TidalUserRosePlaylistAll::slot_applyResult_userPlaylist_rose(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        int idx = sender()->property("idx").toInt();

        if(idx != this->next_offset){
            return;
        }

        if(list_data.size() > 0){
            this->flagReqMore_playlist = false;
            this->flag_lastPage_playlist = flag_lastPage;

            if(this->list_playlist->size() == 0){
                this->playlist_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_playlist->size();
            this->list_playlist->append(list_data);

            if(start_index == 0){
                int f_width = this->flowLayout_playlists->geometry().width();

                if(this->flowLayout_playlists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                int f_wg_cnt = f_width / (this->playlist_widget_width + this->playlist_widget_margin);
                int f_mod = this->playlist_draw_cnt % f_wg_cnt;

                if(f_mod == 0){
                    this->playlist_widget_cnt = f_wg_cnt * 10;
                }
                else{
                    this->playlist_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                }

                //qDebug() << "[Debug] RoseHomeAlbumListAll::slot_applyResult_albums " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

                int start_index = this->playlist_draw_cnt;
                int max_cnt = ((this->playlist_total_cnt - this->playlist_draw_cnt) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_total_cnt - this->playlist_draw_cnt);
                this->playlist_draw_cnt += max_cnt;

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->listPlaylist_all[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists_created, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);
                    connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &TidalUserRosePlaylistAll::slot_clickedItemPlaylist);
                }

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_playlist_draw = false;
            }
            else{
                this->request_more_playlistDraw();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();


            if(this->playlist_draw_cnt <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_playlists->geometry().width();

                if(this->flowLayout_playlists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_playlists->addWidget(noData_widget);
            }
        }
    }


    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalUserRosePlaylistAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){


        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___playlists_created){
            roseHome::PlaylistItemData data = this->list_playlist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입
                QJsonObject jsonObj_move = QJsonObject();
                jsonObj_move.insert("id", data.id);
                jsonObj_move.insert("ownerId", data.ownerId);
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_ROSE_PLAYLIST_DETAIL);

                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                print_debug();
                // OptionPopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
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
    void TidalUserRosePlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___playlists_created){
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
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(this->list_playlist->at(index).id, clickMode);
            }
        }
    }
}
