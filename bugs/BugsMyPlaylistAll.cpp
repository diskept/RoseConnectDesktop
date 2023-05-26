#include "bugs/BugsMyPlaylistAll.h"

#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/global.h"
#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QResizeEvent>
#include <QScrollBar>


namespace bugs {

    const int SECTION_FOR_MORE_POPUP___playlists_created = 1;

    /**
     * @brief BugsRecentlyPlaylistAll::BugsRecentlyPlaylistAll
     * @param parent
     */
    BugsRecentlyPlaylistAll::BugsRecentlyPlaylistAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        this->linker = Linker::getInstance();

        // Data
        this->list_playlist = new QList<roseHome::PlaylistItemData>();
        this->list_filterOpt_sort = AbstractBugsSubWidget::get_list_filterOptData_forMyCollection();

        if(global.user_forBugs.isLogined() == true){

            // 기본 UI 세팅
            this->setUIControl_playlist();
        }
    }


    /**
     * @brief 소멸자.
     */
    BugsRecentlyPlaylistAll::~BugsRecentlyPlaylistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsRecentlyPlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        QString mainTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(pageCode != this->page || mainTitle != this->title){
            this->flagNeedReload = true;

            this->page = pageCode;
            this->title = mainTitle;

            this->label_mainTitle->setText(this->title);
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
            connect(proc_recently, &roseHome::ProcCommon::completeReq_list_playlists, this, &BugsRecentlyPlaylistAll::slot_applyResult_playlistCheck);
            proc_recently->request_rose_getList_recentlyPlaylists("member/playlist/recent", "BUGS", 0, 10);
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void BugsRecentlyPlaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            GSCommon::clearLayout(this->box_contents);      // Layout 초기화

            this->box_contents->setAlignment(Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // layout for items
            this->flowLayout_playlists = this->get_addUIControl_flowLayout(0 , 20);
            GSCommon::clearLayout(this->flowLayout_playlists);

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
        }
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void BugsRecentlyPlaylistAll::initAll(){

        // init
        this->next_offset = 0;
        this->playlist_total_cnt = 0;
        this->playlist_draw_cnt = 0;

        this->list_playlist->clear();

        // request HTTP API
        this->flagReqMore_playlist = false;
        this->flag_lastPage_playlist = false;

        this->flag_flow_draw = false;

        GSCommon::clearLayout(this->flowLayout_playlists);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }



    /**
     * @brief BugsRecentlyPlaylistAll::setUIControll_basic
     */
    void BugsRecentlyPlaylistAll::setUIControl_playlist(){

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

        connect(this->btn_mainTitle, &QPushButton::clicked, this, &BugsRecentlyPlaylistAll::slot_btnClicked_Delete);

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
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &BugsRecentlyPlaylistAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief BugsRecentlyPlaylistAll::setUIControl_filter
     */
    void BugsRecentlyPlaylistAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &BugsRecentlyPlaylistAll::slot_filterClicked);


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
    void BugsRecentlyPlaylistAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_playlist && !this->flag_lastPage_playlist) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_playlist = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistData();
        }
        else if((this->playlist_total_cnt > this->playlist_draw_cnt) && this->flag_playlist_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void BugsRecentlyPlaylistAll::resizeEvent(QResizeEvent *event){//c230223

        AbstractBugsSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }

        this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

        this->widget_mainTitle->resize(this->width(), 69);
        this->btn_mainTitle->setGeometry(this->width() - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);
    }


    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void BugsRecentlyPlaylistAll::slot_filterClicked(){

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
    void BugsRecentlyPlaylistAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

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
    void BugsRecentlyPlaylistAll::request_more_playlistData(){

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
        connect(proc, &roseHome::ProcCommon::completeReq_list_playlists, this, &BugsRecentlyPlaylistAll::slot_applyResult_myPlaylist_rose);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_recentlyPlaylists("member/playlist/recent", "BUGS", this->next_offset, 200);

        if(this->next_offset == 0){
            this->flag_playlist_draw = true;
        }

    }


    void BugsRecentlyPlaylistAll::request_more_playlistDraw(){

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
            connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &BugsRecentlyPlaylistAll::slot_clickedItemPlaylist);
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
    void BugsRecentlyPlaylistAll::slot_applyResult_myPlaylist_rose(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &BugsRecentlyPlaylistAll::slot_clickedItemPlaylist);
                }

                this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_playlist_draw = false;
            }
            else{
                this->request_more_playlistDraw();
            }


            /*if(this->flag_lastPage_playlist == false){
                this->request_more_playlistData();
            }*/
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


    void BugsRecentlyPlaylistAll::slot_applyResult_playlistCheck(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){

            if(this->list_playlist->count() == 0){
                QString tmpTitle = "최근 재생 플레이리스트" + QString(" (%1)").arg(list_data.at(0).totalCount);
                this->label_mainTitle->setText(tmpTitle);
                this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                this->initAll();

                this->request_more_playlistData();

                this->flagNeedReload = true;
                this->setActivePage();
            }
            else{
                for(int i = 0; i < list_data.count(); i++){
                    if(list_data.at(i).title != this->list_playlist->at(i).title){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_playlist->at(0).totalCount) || (change_flag > 0)){

                    QString tmpTitle = "최근 재생 플레이리스트" + QString(" (%1)").arg(list_data.at(0).totalCount);
                    this->label_mainTitle->setText(tmpTitle);
                    this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                    this->initAll();

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

            QString tmpTitle = "최근 재생 플레이리스트 (0)";
            this->label_mainTitle->setText(tmpTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

            this->initAll();

            this->request_more_playlistData();

            this->flagNeedReload = true;
            this->setActivePage();
        }
        else{
            ContentLoadingwaitingMsgHide();
        }
    }


    void BugsRecentlyPlaylistAll::slot_btnClicked_Delete(){

        QStringList tmpTitle = this->title.split("(");

        QJsonObject tmpObj;
        tmpObj.insert("contents_type", tmpTitle.at(0));
        tmpObj.insert("list_type", "PLAYLIST");
        tmpObj.insert("api_subPath", "member/playlist/recent");
        tmpObj.insert("filter_type", "BUGS");

        QJsonObject tmpObj_move;
        tmpObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_RECENTLY_LIST_DELETE);
        tmpObj_move.insert(KEY_DATA, tmpObj);

        emit this->linker->signal_clickedMovePage(tmpObj_move);
    }


    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void BugsRecentlyPlaylistAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){


        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___playlists_created){
            roseHome::PlaylistItemData data = this->list_playlist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                if(data.type == "BUGS" && data.isRose == false){
                    // PD Album 바로 재생
                    bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
                    procRosePlay->requestPlayRose_byPD_AlbumID(data.clientKey.toInt(), OptMorePopup::ClickMode::Play_RightNow);
                }
                else{
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byPlaylistID(data.id, OptMorePopup::ClickMode::Play_RightNow);
                }
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입
                if(data.isRose == false){
                    if(data.clientKey.isEmpty()){
                        QJsonObject jsonObj_move = QJsonObject();
                        jsonObj_move.insert("id", data.id);
                        jsonObj_move.insert("ownerId", data.ownerId);
                        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_ROSE_PLAYLIST_DETAIL);

                        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                    }
                    else{
                        QJsonObject jsonObj_move = QJsonObject();
                        jsonObj_move.insert("es_album_id", data.clientKey.toInt());
                        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___PD_ALBUM_DETAIL);

                        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                    }
                }
                else if(data.isRose == true){
                    QJsonObject jsonObj_move = QJsonObject();
                    jsonObj_move.insert("id", data.id);
                    jsonObj_move.insert("ownerId", data.ownerId);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_ROSE_PLAYLIST_DETAIL);

                    emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                }
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
    void BugsRecentlyPlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

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
                if(this->list_playlist->at(index).type == "BUGS" && this->list_playlist->at(index).isRose == false){
                    bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
                    procRosePlay->requestPlayRose_byPD_AlbumID(this->list_playlist->at(index).clientKey.toInt(), clickMode);
                }
                else{
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byPlaylistID(this->list_playlist->at(index).id, clickMode);
                }
            }
        }
    }





    /**
     * @brief RoseHomePlaylistListAll::RoseHomePlaylistListAll
     * @param parent
     */
    BugsPlaylistHistoryAll::BugsPlaylistHistoryAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // 기본 UI 세팅
        this->setUIControl_playlist();

        this->list_playlist = new QList<roseHome::PlaylistItemData>();
    }

    /**
     * @brief 소멸자.
     */
    BugsPlaylistHistoryAll::~BugsPlaylistHistoryAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsPlaylistHistoryAll::setJsonObject_forData(const QJsonObject &jsonObj){

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
    void BugsPlaylistHistoryAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

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
    void BugsPlaylistHistoryAll::setUIControl_playlist(){

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
    void BugsPlaylistHistoryAll::proc_wheelEvent_to_getMoreData(){

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
    void BugsPlaylistHistoryAll::resizeEvent(QResizeEvent *event){//c230223

        AbstractBugsSubWidget::resizeEvent(event);

        this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

    }

    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    void BugsPlaylistHistoryAll::request_more_Data(){

        // next_offset
        if(this->list_playlist->size() == 0){
            this->playlist_next_offset = 0;
        }
        else{
            this->playlist_next_offset++;
        }

        roseHome::ProcCommon *proc_playlist = new roseHome::ProcCommon(this);
        connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_playlists, this, &BugsPlaylistHistoryAll::slot_applyResult_historyPlaylist);
        proc_playlist->setProperty("idx", this->playlist_next_offset);
        proc_playlist->request_rose_getList_historyPlaylists(this->data_pageInfo.api_subPath, this->data_pageInfo.filter_type, this->playlist_next_offset, 200);

        if(this->playlist_next_offset == 0){
            this->flag_playlist_draw = true;
        }
    }


    /*void BugsPlaylistHistoryAll::request_more_Draw(){

        // j220913 list count check
        int width_cnt = global.LmtCnt / 220;
        int mod = this->playlist_drawCount % width_cnt;

        if(mod == 0){
            this->playlist_widget_cnt = width_cnt * 10;
        }
        else{
            this->playlist_widget_cnt = width_cnt * 10 + (width_cnt - mod);
        }
        // j220913 list count check

        int start_index = this->playlist_drawCount;
        int max_cnt = ((this->playlist_totalCount - this->playlist_drawCount) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_totalCount - this->playlist_drawCount);
        this->playlist_drawCount += max_cnt;

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->type_id, true);
            connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHomePlaylistListAll::slot_clickedItemPlaylist);
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->widget_playlist[i]->setData(this->list_playlist->at(i));
            this->flowLayout_playlists->addWidget(this->widget_playlist[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_playlist_draw = false;
    }*/

    void BugsPlaylistHistoryAll::request_more_Draw(){

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
            connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &BugsPlaylistHistoryAll::slot_clickedItemPlaylist);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_playlist_draw = false;
    }

    // MARK : User Event Handler (slots) ---------------------------------------------------------------------------------

    void BugsPlaylistHistoryAll::slot_applyResult_historyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

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
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &BugsPlaylistHistoryAll::slot_clickedItemPlaylist);
                }

                this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_playlist_draw = false;
            }
            else{
                this->request_more_Draw();
            }

            //this->request_more_Data();
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
    void BugsPlaylistHistoryAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___playlists_created){
            //this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);
            roseHome::PlaylistItemData data = this->list_playlist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(this->list_playlist->at(index).id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입
                if(data.isRose == false){
                    if(data.clientKey.isEmpty()){
                        QJsonObject jsonObj_move = QJsonObject();
                        jsonObj_move.insert("id", data.id);
                        jsonObj_move.insert("ownerId", data.ownerId);
                        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_ROSE_PLAYLIST_DETAIL);

                        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                    }
                    else{
                        QJsonObject jsonObj_move = QJsonObject();
                        jsonObj_move.insert("es_album_id", data.clientKey.toInt());
                        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___PD_ALBUM_DETAIL);

                        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                    }
                }
                else if(data.isRose == true){
                    QJsonObject jsonObj_move = QJsonObject();
                    jsonObj_move.insert("id", data.id);
                    jsonObj_move.insert("ownerId", data.ownerId);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_ROSE_PLAYLIST_DETAIL);

                    emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                }
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptionPopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
            }
        }
    }


    void BugsPlaylistHistoryAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

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

                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_PLAYLIST_ADD);

                emit this->signal_clickedMovePage(jsonObj_move);
            }
        }
    }





    /**
     * @brief QobuzMoodsMain::QobuzMoodsMain
     * @param parent
     */
    BugsMyRosePlaylistAll::BugsMyRosePlaylistAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // Data
        this->list_playlist = new QList<roseHome::PlaylistItemData>();
        this->list_filterOpt_sort = AbstractBugsSubWidget::get_list_filterOptData_forMyCollection();

        if(global.user_forBugs.isLogined() == true){

            // 기본 UI 세팅
            this->setUIControl_playlist();
        }
    }


    /**
     * @brief 소멸자.
     */
    BugsMyRosePlaylistAll::~BugsMyRosePlaylistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsMyRosePlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

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
    void BugsMyRosePlaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

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
    void BugsMyRosePlaylistAll::setUIControl_playlist(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Playlists");
        GSCommon::clearLayout(this->flowLayout_playlists);

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

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &BugsMyRosePlaylistAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyPlaylistAll::setUIControl_filter
     */
    void BugsMyRosePlaylistAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &BugsMyRosePlaylistAll::slot_filterClicked);


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
    void BugsMyRosePlaylistAll::proc_wheelEvent_to_getMoreData(){

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
    void BugsMyRosePlaylistAll::resizeEvent(QResizeEvent *event){

        AbstractBugsSubWidget::resizeEvent(event);

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
    void BugsMyRosePlaylistAll::slot_filterClicked(){

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
    void BugsMyRosePlaylistAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

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
    void BugsMyRosePlaylistAll::request_more_playlistData(){

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
        connect(proc, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &BugsMyRosePlaylistAll::slot_applyResult_myPlaylist_rose);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_myPlaylists("member/playlist", "BUGS", this->next_offset, 200);

        if(this->next_offset == 0){
            this->flag_playlist_draw = true;
        }

    }


    void BugsMyRosePlaylistAll::request_more_playlistDraw(){

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
            connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &BugsMyRosePlaylistAll::slot_clickedItemPlaylist);
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
    void BugsMyRosePlaylistAll::slot_applyResult_myPlaylist_rose(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    this->listPlaylist_all[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists_created, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);
                    connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &BugsMyRosePlaylistAll::slot_clickedItemPlaylist);
                }

                this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

                ContentLoadingwaitingMsgHide();

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


    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void BugsMyRosePlaylistAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){


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
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_ROSE_PLAYLIST_DETAIL);

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
    void BugsMyRosePlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

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

                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_PLAYLIST_ADD);

                emit this->signal_clickedMovePage(jsonObj_move);
            }
        }
    }





    /**
     * @brief BugsUserRosePlaylistAll::BugsUserRosePlaylistAll
     * @param parent
     */
    BugsUserRosePlaylistAll::BugsUserRosePlaylistAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // Data
        this->list_playlist = new QList<roseHome::PlaylistItemData>();
        this->list_filterOpt_sort = AbstractBugsSubWidget::get_list_filterOptData_forMyCollection();

        if(global.user_forBugs.isLogined() == true){

            // 기본 UI 세팅
            this->setUIControl_playlist();
        }
    }


    /**
     * @brief 소멸자.
     */
    BugsUserRosePlaylistAll::~BugsUserRosePlaylistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsUserRosePlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

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
    void BugsUserRosePlaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            // init UI
            GSCommon::clearLayout(this->flowLayout_playlists);

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
        }
    }



    /**
     * @brief BugsUserRosePlaylistAll::setUIControll_basic
     */
    void BugsUserRosePlaylistAll::setUIControl_playlist(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Playlists");

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        roseHome::ItemPlaylist_rosehome *listAlbum = new roseHome::ItemPlaylist_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);

        this->playlist_widget_width = listAlbum->get_fixedWidth();
        this->playlist_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeAlbumListAll::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;


        // layout for items
        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &BugsUserRosePlaylistAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief BugsUserRosePlaylistAll::setUIControl_filter
     */
    void BugsUserRosePlaylistAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &BugsUserRosePlaylistAll::slot_filterClicked);


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
    void BugsUserRosePlaylistAll::proc_wheelEvent_to_getMoreData(){

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
    void BugsUserRosePlaylistAll::resizeEvent(QResizeEvent *event){

        AbstractBugsSubWidget::resizeEvent(event);

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
    void BugsUserRosePlaylistAll::slot_filterClicked(){

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
    void BugsUserRosePlaylistAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

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
    void BugsUserRosePlaylistAll::request_more_playlistData(){

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
        connect(proc, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &BugsUserRosePlaylistAll::slot_applyResult_userPlaylist_rose);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_usersPlaylists("member/playlist/all", "PLAYLIST_RECENT", "BUGS", this->next_offset, 200);

        if(this->next_offset == 0){
            this->flag_playlist_draw = true;
        }
    }


    void BugsUserRosePlaylistAll::request_more_playlistDraw(){

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
            connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &BugsUserRosePlaylistAll::slot_clickedItemPlaylist);
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
    void BugsUserRosePlaylistAll::slot_applyResult_userPlaylist_rose(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &BugsUserRosePlaylistAll::slot_clickedItemPlaylist);
                }

                this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

                ContentLoadingwaitingMsgHide();

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


    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void BugsUserRosePlaylistAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){


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
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_ROSE_PLAYLIST_DETAIL);

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
    void BugsUserRosePlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

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
