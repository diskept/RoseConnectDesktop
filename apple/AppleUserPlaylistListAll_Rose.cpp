#include "apple/AppleUserPlaylistListAll_Rose.h"

#include "apple/ProcRosePlay_withApple.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/global.h"
#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QResizeEvent>


namespace apple {

    const int SECTION_FOR_MORE_POPUP___playlists = 1;

    const int GET_ITEM_SIZE___ONCE = 70;


    /**
     * @brief QobuzMoodsMain::QobuzMoodsMain
     * @param parent
     */
    AppleUserPlaylistListAll_Rose::AppleUserPlaylistListAll_Rose(QWidget *parent) : AbstractAppleSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // 기본 UI 세팅
        this->setUIControl_playlist();

        // Data
        this->list_playlist = new QList<roseHome::PlaylistItemData>();
        this->list_filterOpt_sort = AbstractAppleSubWidget::get_list_filterOptData_forMyCollection();
    }


    /**
     * @brief 소멸자.
     */
    AppleUserPlaylistListAll_Rose::~AppleUserPlaylistListAll_Rose(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void AppleUserPlaylistListAll_Rose::setJsonObject_forData(const QJsonObject &jsonObj){

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

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistData();
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void AppleUserPlaylistListAll_Rose::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractAppleSubWidget::setActivePage();

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
    void AppleUserPlaylistListAll_Rose::setUIControl_playlist(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Playlists");

        // filtering
        //this->setUiControl_filter();

        // layout for items
        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &AppleUserPlaylistListAll_Rose::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyPlaylistAll::setUiControl_filter
     */
    void AppleUserPlaylistListAll_Rose::setUiControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &AppleUserPlaylistListAll_Rose::slot_filterClicked);


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
    void AppleUserPlaylistListAll_Rose::proc_wheelEvent_to_getMoreData(){

        if((this->playlist_total_cnt > this->playlist_draw_cnt) && (this->list_playlist->size() > this->playlist_draw_cnt) && (this->flag_playlist_draw == false)){

            this->flag_playlist_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void AppleUserPlaylistListAll_Rose::resizeEvent(QResizeEvent *event){

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
    void AppleUserPlaylistListAll_Rose::slot_filterClicked(){

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
    void AppleUserPlaylistListAll_Rose::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

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
    void AppleUserPlaylistListAll_Rose::request_more_playlistData(){

        if(!this->flagReqMore_playlist && !this->flag_lastPage_playlist){
            this->flagReqMore_playlist = true;

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
            connect(proc, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &AppleUserPlaylistListAll_Rose::slot_applyResult_userPlaylist_rose);
            proc->request_rose_getList_usersPlaylists("member/playlist/all", "PLAYLIST_RECENT", "APPLE_MUSIC", this->next_offset, GET_ITEM_SIZE___ONCE);

            if(this->next_offset == 0){
                this->flag_playlist_draw = true;
            }
        }
    }


    void AppleUserPlaylistListAll_Rose::request_more_playlistDraw(){

        int start_index = this->playlist_draw_cnt;
        int max_cnt = ((this->playlist_total_cnt - this->playlist_draw_cnt) > GET_ITEM_SIZE___ONCE ) ? GET_ITEM_SIZE___ONCE : (this->playlist_total_cnt - this->playlist_draw_cnt);
        this->playlist_draw_cnt += max_cnt;

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);
            connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &AppleUserPlaylistListAll_Rose::slot_clickedItemPlaylist);
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
        this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);

        QCoreApplication::processEvents();
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
void AppleUserPlaylistListAll_Rose::slot_applyResult_userPlaylist_rose(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

    Q_UNUSED(jsonArr_dataToPlay);

    if(list_data.size() > 0){
        this->flagReqMore_playlist = false;
        this->flag_lastPage_playlist = flag_lastPage;

        if(this->list_playlist->size() == 0){
            this->playlist_total_cnt = list_data.at(0).totalCount;
        }

        int start_index = this->list_playlist->size();
        this->list_playlist->append(list_data);

        if(start_index == 0){
            int max_cnt = this->list_playlist->size();
            this->playlist_draw_cnt = max_cnt;

            for(int i = start_index; i < this->playlist_draw_cnt; i++){
                this->listPlaylist_all[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);
                connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &AppleUserPlaylistListAll_Rose::slot_clickedItemPlaylist);
            }

            for(int i = start_index; i < this->playlist_draw_cnt; i++){
                this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
                this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);

                QCoreApplication::processEvents();
            }

            this->flag_flow_draw = true;
            this->flag_playlist_draw = false;
        }

        ContentLoadingwaitingMsgHide();
        this->request_more_playlistData();
    }
    else{
        ContentLoadingwaitingMsgHide();

        NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
        noData_widget->setFixedSize(1500, 300);

        this->flowLayout_playlists->addWidget(noData_widget);
    }
}


/**
 * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
 * @param clickMode
 */
void AppleUserPlaylistListAll_Rose::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){


    int index = ((tidal::AbstractItem*)sender())->index();
    int section = ((tidal::AbstractItem*)sender())->section();

    // ClickMode 별로 처리
    if(section == SECTION_FOR_MORE_POPUP___playlists){
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
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_APPLE_PLAYLIST_ROSE_DETAIL);

            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
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
void AppleUserPlaylistListAll_Rose::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

    if(section == SECTION_FOR_MORE_POPUP___playlists){
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
