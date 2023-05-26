#include "rosetube/rosetube_MyPlaylistAll.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"

#include <QResizeEvent>
#include <QScrollBar>


namespace rosetube {

    const int HTTP_CACHE = 0;

    const int SECTION_FOR_MORE_POPUP___playlists = 1;

    const int GET_ITEM_SIZE___ONCE = 200;


    /**
         * @brief QobuzMoodsMain::QobuzMoodsMain
         * @param parent
         */
    RoseTubePlaylistRecentlyAll::RoseTubePlaylistRecentlyAll(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        this->linker = Linker::getInstance();

        // Data
        this->list_playlist = new QList<roseHome::PlaylistItemData>();
        this->list_filterOpt_sort = roseHome::AbstractRoseHomeSubWidget::get_list_filterOptData_forMyCollection();

        if(global.user.isValid() == true){

            // 기본 UI 세팅
            this->setUIControl_playlist();
        }
    }


    /**
         * @brief 소멸자.
         */
    RoseTubePlaylistRecentlyAll::~RoseTubePlaylistRecentlyAll(){

        this->deleteLater();
    }


    /**
         * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
         * @param jsonObj
         */
    void RoseTubePlaylistRecentlyAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "viewAllMode");
        QString mainTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(this->page_reload_check != pageCode || this->page_mainTitle != mainTitle){
            this->flagNeedReload = true;

            this->page_reload_check = pageCode;
            this->page_mainTitle = mainTitle;

            this->label_mainTitle->setText(this->page_mainTitle);
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
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_playlists, this, &RoseTubePlaylistRecentlyAll::slot_applyResult_playlistCheck);
            proc->request_rose_getList_recentlyPlaylists("member/playlist/recent", "YOUTUBE", 0, 10);
        }
    }


    /**
         * @brief 페이지 활성화를 처리하는 함수임.
         * HTTP API 호출한다.
         */
    void RoseTubePlaylistRecentlyAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            roseHome::AbstractRoseHomeSubWidget::setActivePage();

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
    void RoseTubePlaylistRecentlyAll::setUIControl_playlist(){

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

        connect(this->btn_mainTitle, &QPushButton::clicked, this, &RoseTubePlaylistRecentlyAll::slot_btnClicked_Delete);

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

        // layout for items
        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &RoseTubePlaylistRecentlyAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
         * @brief QobuzMyPlaylistAll::setUIControl_filter
         */
    void RoseTubePlaylistRecentlyAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &RoseTubePlaylistRecentlyAll::slot_filterClicked);


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
    void RoseTubePlaylistRecentlyAll::initAll(){

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
    void RoseTubePlaylistRecentlyAll::proc_wheelEvent_to_getMoreData(){

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
    void RoseTubePlaylistRecentlyAll::resizeEvent(QResizeEvent *event){

        roseHome::AbstractRoseHomeSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }

        this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

        this->widget_mainTitle->resize(global.LmtCnt, 69);
        this->btn_mainTitle->setGeometry(global.LmtCnt - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);
    }


    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
         * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
         */
    void RoseTubePlaylistRecentlyAll::slot_filterClicked(){

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
    void RoseTubePlaylistRecentlyAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

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
    void RoseTubePlaylistRecentlyAll::request_more_playlistData(){

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
        connect(proc, &roseHome::ProcCommon::completeReq_list_playlists, this, &RoseTubePlaylistRecentlyAll::slot_applyResult_recentlyPlaylist_rose);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_recentlyPlaylists("member/playlist/recent", "YOUTUBE", this->next_offset, GET_ITEM_SIZE___ONCE);

        if(this->next_offset == 0){
            this->flag_playlist_draw = true;
        }

    }


    void RoseTubePlaylistRecentlyAll::request_more_playlistDraw(){

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
            this->listPlaylist_all[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);
            connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseTubePlaylistRecentlyAll::slot_clickedItemPlaylist);
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
    void RoseTubePlaylistRecentlyAll::slot_applyResult_recentlyPlaylist_rose(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    this->listPlaylist_all[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);
                    connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseTubePlaylistRecentlyAll::slot_clickedItemPlaylist);
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


    void RoseTubePlaylistRecentlyAll::slot_applyResult_playlistCheck(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_playlist->count() == 0){
                QString tmpTitle = tr("Recently Played Playlists") + QString(" (%1)").arg(list_data.at(0).totalCount);
                this->label_mainTitle->setText(tmpTitle);
                this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                this->initAll();

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
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

                    QString tmpTitle = tr("Recently Played Playlists") + QString(" (%1)").arg(list_data.at(0).totalCount);
                    this->label_mainTitle->setText(tmpTitle);
                    this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                    this->initAll();

                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
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

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_playlistData();

            this->flagNeedReload = true;
            this->setActivePage();
        }
        else{
            ContentLoadingwaitingMsgHide();
        }
    }


    void RoseTubePlaylistRecentlyAll::slot_btnClicked_Delete(){

        QStringList tmpTitle = this->page_mainTitle.split("(");

        QJsonObject tmpObj;
        tmpObj.insert("contents_type", tmpTitle.at(0));
        tmpObj.insert("list_type", "PLAYLIST");
        tmpObj.insert("api_subPath", "member/playlist/recent");
        tmpObj.insert("filter_type", "YOUTUBE");

        QJsonObject tmpObj_move;
        tmpObj_move.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_RECENTLY_DELETE);
        tmpObj_move.insert(KEY_DATA, tmpObj);

        emit this->linker->signal_clickedMovePage(tmpObj_move);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RoseTubePlaylistRecentlyAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___playlists){
            roseHome::PlaylistItemData data_playlist = this->list_playlist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                print_debug();

                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data_playlist), index, SECTION_FOR_MORE_POPUP___playlists, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                // Playlist Detail 페이지 진입
                QJsonObject tmp_data;
                tmp_data.insert("id", data_playlist.id);
                tmp_data.insert("ownerId", data_playlist.ownerId);
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYDETAIL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_MYDETAIL);
                // 부모에게 페이지 변경하라고 시그널 보냄
                emit this->signal_clickedViewAll(tmp_data);
            }
        }
    }


    void RoseTubePlaylistRecentlyAll::slot_applyResult_getShareLink(const QString &link){//c220818 share link

        this->shareLink = link;
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
    void RoseTubePlaylistRecentlyAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, SECTION_FOR_MORE_POPUP___playlists, clickMode);
        }
    }





    /**
         * @brief RoseTubeRecentlyPlaylistAll::RoseTubeRecentlyPlaylistAll
         * @param parent
         */
    RoseTubeRecentlyPlaylistAll::RoseTubeRecentlyPlaylistAll(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        this->linker = Linker::getInstance();

        // Data
        this->list_filterOpt_sort = roseHome::AbstractRoseHomeSubWidget::get_list_filterOptData_forMyCollection();

        // 기본 UI 세팅
        this->setUIControl_playlist();
    }


    /**
         * @brief 소멸자.
         */
    RoseTubeRecentlyPlaylistAll::~RoseTubeRecentlyPlaylistAll(){

        this->deleteLater();
    }


    /**
         * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
         * @param jsonObj
         */
    void RoseTubeRecentlyPlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "viewAllMode");
        QString mainTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(this->page_reload_check != pageCode && this->page_mainTitle != mainTitle){
            this->flagNeedReload = true;

            this->page_reload_check = pageCode;
            this->page_mainTitle = mainTitle;

            this->label_mainTitle->setText(this->page_mainTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

            // init
            this->next_offset = 0;
            this->playlist_total_cnt = 0;
            this->playlist_draw_cnt = 0;

            // init data
            this->playlist_Arr = QJsonArray();

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
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_rosetubes, this, &RoseTubeRecentlyPlaylistAll::slot_applyResult_rosetubelistCheck);
            proc->request_rose_getList_recentlyRosetube("member/track/recent", "YOUTUBE", 0, 10);
        }
    }


    /**
         * @brief 페이지 활성화를 처리하는 함수임.
         * HTTP API 호출한다.
         */
    void RoseTubeRecentlyPlaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            roseHome::AbstractRoseHomeSubWidget::setActivePage();

            // init UI
            GSCommon::clearLayout(this->flowLayout_playlists);

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
        }
    }



    /**
         * @brief RoseTubeRecentlyPlaylistAll::setUIControll_basic
         */
    void RoseTubeRecentlyPlaylistAll::setUIControl_playlist(){

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

        connect(this->btn_mainTitle, &QPushButton::clicked, this, &RoseTubeRecentlyPlaylistAll::slot_btnClicked_Delete);

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

        rosetube::ItemTrack_rosetube *listAlbum = new rosetube::ItemTrack_rosetube(0, 0, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);

        this->playlist_widget_width = listAlbum->get_fixedWidth();
        this->playlist_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeAlbumListAll::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;
        // layout for items
        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0 , 20);

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &RoseTubeRecentlyPlaylistAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void RoseTubeRecentlyPlaylistAll::initAll(){

        // init
        this->next_offset = 0;
        this->playlist_total_cnt = 0;
        this->playlist_draw_cnt = 0;

        this->playlist_Arr = QJsonArray();

        // request HTTP API
        this->flagReqMore_playlist = false;
        this->flag_lastPage_playlist = false;

        this->flag_playlist_draw = false;

        GSCommon::clearLayout(this->flowLayout_playlists);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
         * @brief RoseTubeRecentlyPlaylistAll::setUIControl_filter
         */
    void RoseTubeRecentlyPlaylistAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &RoseTubeRecentlyPlaylistAll::slot_filterClicked);


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
    void RoseTubeRecentlyPlaylistAll::proc_wheelEvent_to_getMoreData(){

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
    void RoseTubeRecentlyPlaylistAll::resizeEvent(QResizeEvent *event){

        roseHome::AbstractRoseHomeSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }

        this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

        this->widget_mainTitle->resize(global.LmtCnt, 69);
        this->btn_mainTitle->setGeometry(global.LmtCnt - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);
    }


    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
         * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
         */
    void RoseTubeRecentlyPlaylistAll::slot_filterClicked(){

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
    void RoseTubeRecentlyPlaylistAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

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
    void RoseTubeRecentlyPlaylistAll::request_more_playlistData(){

        // next_offset
        if(this->playlist_Arr.size() == 0){
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
        connect(proc, &roseHome::ProcCommon::completeReq_list_rosetubes, this, &RoseTubeRecentlyPlaylistAll::slot_applyResult_myPlaylist_rose);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_recentlyRosetube("member/track/recent", "YOUTUBE", this->next_offset, GET_ITEM_SIZE___ONCE);

        if(this->next_offset == 0){
            this->flag_playlist_draw = true;
        }

    }


    void RoseTubeRecentlyPlaylistAll::request_more_playlistDraw(){

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
            this->recently_roseTube[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
           this->recently_roseTube[i]->setData(this->playlist_Arr.at(i).toObject());
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->flowLayout_playlists->addWidget(this->recently_roseTube[i]);
            connect(this->recently_roseTube[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTubeRecentlyPlaylistAll::slot_clickedItemPlaylist);
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
    void RoseTubeRecentlyPlaylistAll::slot_applyResult_myPlaylist_rose(const QJsonArray &jsonArr_dataToPlay, const int &totalCnt, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        int idx = sender()->property("idx").toInt();

        if(idx != this->next_offset){
            return;
        }

        if(jsonArr_dataToPlay.size() > 0){
            this->flagReqMore_playlist = false;
            this->flag_lastPage_playlist = flag_lastPage;

            if(this->playlist_Arr.size() == 0){
                this->playlist_total_cnt = totalCnt;
                this->playlist_Arr = QJsonArray();
            }

            int start_index = this->playlist_Arr.size();
            ProcJsonEasy::mergeJsonArray(this->playlist_Arr, jsonArr_dataToPlay);

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
                    this->recently_roseTube[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                   this->recently_roseTube[i]->setData(this->playlist_Arr.at(i).toObject());
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->flowLayout_playlists->addWidget(this->recently_roseTube[i]);
                    connect(this->recently_roseTube[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTubeRecentlyPlaylistAll::slot_clickedItemPlaylist);
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


    void RoseTubeRecentlyPlaylistAll::slot_applyResult_rosetubelistCheck(const QJsonArray &jsonArr_dataToPlay, const int &totalCnt, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(jsonArr_dataToPlay.size() > 0){
            if(this->playlist_total_cnt == 0){
                QString tmpTitle = tr("Recently Played Rosetube") + QString(" (%1)").arg(totalCnt);
                this->label_mainTitle->setText(tmpTitle);
                this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                this->initAll();

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                this->request_more_playlistData();

                this->flagNeedReload = true;
                this->setActivePage();
            }
            else{
                for(int i = 0; i < jsonArr_dataToPlay.count(); i++){
                    QJsonObject tmpData = jsonArr_dataToPlay.at(i).toObject();
                    QJsonObject tmpObj = this->playlist_Arr.at(i).toObject();

                    if(tmpData != tmpObj){
                        change_flag++;
                    }
                }

                if((totalCnt != this->playlist_total_cnt) || (change_flag > 0)){

                    QString tmpTitle = tr("Recently Played Rosetube") + QString(" (%1)").arg(totalCnt);
                    this->label_mainTitle->setText(tmpTitle);
                    this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                    this->initAll();

                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                    this->request_more_playlistData();

                    this->flagNeedReload = true;
                    this->setActivePage();
                }
                else{
                    ContentLoadingwaitingMsgHide();
                }
            }
        }
        else if(totalCnt == 0){
            QString tmpTitle = tr("Recently Played Rosetube (0)");
            this->label_mainTitle->setText(tmpTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

            this->initAll();

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_playlistData();

            this->flagNeedReload = true;
            this->setActivePage();
        }
        else{
            ContentLoadingwaitingMsgHide();
        }
    }


    void RoseTubeRecentlyPlaylistAll::slot_btnClicked_Delete(){

        QStringList tmpTitle = this->page_mainTitle.split("(");

        QJsonObject tmpObj;
        tmpObj.insert("contents_type", tmpTitle.at(0));
        tmpObj.insert("list_type", "YOUTUBE");
        tmpObj.insert("api_subPath", "member/playlist/recent");
        tmpObj.insert("filter_type", "YOUTUBE");

        QJsonObject tmpObj_move;
        tmpObj_move.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_RECENTLY_DELETE);
        tmpObj_move.insert(KEY_DATA, tmpObj);

        emit this->linker->signal_clickedMovePage(tmpObj_move);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RoseTubeRecentlyPlaylistAll::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){

            QJsonObject tmpObj = contents.at(0).toObject();

            if(tmpObj.value("flagOk").toBool() == true && tmpObj.value("message").toString() == "정상"){
                if(this->flag_fav_type == SECTION_FOR_MORE_POPUP___playlists){
                    this->recently_roseTube[this->flag_fav_idx]->setFavorite_btnHeart(this->flag_fav_star == 0 ? false : true, this->flag_fav_star);
                }
            }
        }

        ContentLoadingwaitingMsgHide();

        if(contents.size() > 0 && this->flag_fav_star == 0){
            // request HTTP API
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_rosetubes, this, &RoseTubeRecentlyPlaylistAll::slot_applyResult_rosetubelistCheck);
            proc->request_rose_getList_recentlyRosetube("member/track/recent", "YOUTUBE", 0, 10);

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
        }
    }


    void RoseTubeRecentlyPlaylistAll::slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj){

        if(p_id == HTTP_CACHE){
            if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
            }
        }
    }


    void RoseTubeRecentlyPlaylistAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___playlists){

            if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx2
                    || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx3 || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){

                bool flag_fav = false;
                int star_fav = 0;

                if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add){
                    flag_fav = true;
                    star_fav = 1;
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx2){
                    flag_fav = true;
                    star_fav = 2;
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx3){
                    flag_fav = true;
                    star_fav = 3;
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){
                    flag_fav = true;
                    star_fav = 0;
                }

                this->flag_fav_idx = index;
                this->flag_fav_star = star_fav;
                this->flag_fav_type = SECTION_FOR_MORE_POPUP___playlists;

                QJsonObject track = this->playlist_Arr.at(index).toObject();

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", flag_fav);
                ratingInfo.insert("star", star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", "YOUTUBE");

                QJsonObject json;
                json.insert("track", track);
                json.insert("ratingInfo", ratingInfo);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseTubeRecentlyPlaylistAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, flag_fav, star_fav);

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }
            else{

                if(clickMode == tidal::AbstractItem::ClickMode::AllBox || clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                    // Track 바로 재생 - clicked 항목만 재생
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

                    roseHome::TrackItemData data_playlist;
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byTracks(data_playlist, this->playlist_Arr, index, playType);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    QJsonObject tmpObj = this->playlist_Arr.at(index).toObject();

                    QJsonObject data = ProcJsonEasy::getJsonObject(tmpObj, "data");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(data, "snippet");

                    OptMorePopup::HeaderData data_header;
                    data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                    data_header.sub_title = ProcJsonEasy::getString(snippet, "channelTitle");
                    data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                    data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(tmpObj, "id"));
                    data_header.type = "YOUTUBE";      //j220906 share link
                    data_header.flagProcStar = false;
                    data_header.isShare = true;      //j220906 share link

                    // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Rosetube_Video, data_header, index, section);
                }
            }

        }
    }

    void RoseTubeRecentlyPlaylistAll::slot_applyResult_getShareLink(const QString &link){//c220818 share link

        this->shareLink = link;
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
    void RoseTubeRecentlyPlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___playlists){
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
                // Rose Play 요청
                roseHome::TrackItemData data_track;
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data_track, this->playlist_Arr, index, clickMode);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubscribInfo){
                QJsonObject tmpObj = this->playlist_Arr.at(index).toObject();
                QJsonObject dataObj = ProcJsonEasy::getJsonObject(tmpObj, "data");
                QJsonObject snippet = ProcJsonEasy::getJsonObject(dataObj, "snippet");
                QString channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                QString channelId = ProcJsonEasy::getString(snippet, "channelId");

                QJsonObject tmp_data;
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert("title", channelName);
                tmp_data.insert("channel_id", channelId);

                emit this->signal_clickedViewAll(tmp_data);
            }
            else if(clickMode == OptMorePopup::ClickMode::Share){//c220818
                print_debug();//c220818-3dot
                setUIShare();//c220818
                print_debug();
                qDebug() << "this->shareLink="<<this->shareLink;//c220818
            }
            else if(clickMode == OptMorePopup::ClickMode::Caching_ahead){
                QJsonObject trackInfo = this->playlist_Arr.at(index).toObject();

                QString channelId = "";
                QString channelName = "";
                QString duration = "";
                QString id = "";
                QString thumbnailUrl = "";
                QString title = "";

                if(trackInfo.contains("data")){
                    QJsonObject trackData = ProcJsonEasy::getJsonObject(trackInfo, "data");
                    QJsonObject detailContents = ProcJsonEasy::getJsonObject(trackData, "detailContents");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(trackData, "snippet");
                    QJsonObject trackStatistic = ProcJsonEasy::getJsonObject(trackData, "detailContents");

                    QString tmpDuration = ProcJsonEasy::getString(detailContents, "duration");

                    QString time = tmpDuration;
                    QString hour;
                    QString minute;
                    QString second;

                    if(tmpDuration.indexOf("PT") >= 0){
                        time = tmpDuration.right(tmpDuration.size() - 2);
                    }
                    if(time.indexOf("H") >= 0){
                        hour = time.section("H", 0, -2);
                        time = time.mid(time.indexOf("H") + 1);
                    }
                    if(time.indexOf("M") >= 0){
                        minute = time.section("M", 0, -2);
                        time = time.mid(time.indexOf("M") + 1);
                    }
                    if(time.indexOf("S") >= 0){
                        second = time.section("S", 0, -2);
                    }

                    if(hour.isEmpty()){
                        if(minute.isEmpty()){
                            duration = "0:" + second.rightJustified(2, '0');
                        }
                        else{
                            duration = minute + ":" + second.rightJustified(2, '0');
                        }
                    }
                    else{
                        duration = hour + ":" + minute.rightJustified(2, '0') + ":" + second.rightJustified(2, '0');
                    }

                    channelId = ProcJsonEasy::getString(snippet, "channelId");
                    channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                    id = ProcJsonEasy::getString(trackInfo, "clientKey");
                    thumbnailUrl = ProcJsonEasy::getString(trackInfo, "thumbnailUrl");
                    title = ProcJsonEasy::getString(trackInfo, "title");
                }
                else{
                    if(trackInfo.contains("channelTitle")){
                        channelName = ProcJsonEasy::getString(trackInfo, "channelTitle");
                    }
                    else if(trackInfo.contains("channelName")){
                        channelName = ProcJsonEasy::getString(trackInfo, "channelName");
                    }

                    int time = ProcJsonEasy::getInt(trackInfo, "duration");
                    if(time == 0){
                        QString tmpDur = ProcJsonEasy::getString(trackInfo, "duration");

                        if(tmpDur.isEmpty()){
                            duration = "";
                        }
                        else{
                            duration = tmpDur;
                        }
                    }
                    else if(time >= 3600){
                         duration = QDateTime::fromTime_t(time).toUTC().toString("hh:mm:ss");
                    }
                    else{
                        duration = QDateTime::fromTime_t(time).toUTC().toString("mm:ss");
                    }

                    channelId = ProcJsonEasy::getString(trackInfo, "channelId");
                    id = ProcJsonEasy::getString(trackInfo, "id");
                    thumbnailUrl = ProcJsonEasy::getString(trackInfo, "thumbnailUrl");
                    title = ProcJsonEasy::getString(trackInfo, "title");
                }

                QJsonObject data;
                data.insert("channelId", channelId);
                data.insert("channelName", channelName);
                data.insert("duration", duration);
                data.insert("id", id);
                data.insert("isFavorite", false);
                data.insert("isLiveStream", false);
                data.insert("thumbnailUrl", thumbnailUrl);
                data.insert("thumbsUpPercentage", 0);
                data.insert("title", title);
                data.insert("viewsCount", "-1");

                QJsonObject json;
                json.insert("data", data);
                json.insert("isDelete", false);

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_download_item";
                network->request(HTTP_CACHE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }            
        }
    }





    /**
     * @brief RoseHomePlaylistListAll::RoseHomePlaylistListAll
     * @param parent
     */
    RoseTubeHistoryPlaylistAll::RoseTubeHistoryPlaylistAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // 기본 UI 세팅
        this->setUIControl_playlist();

        this->list_playlist = new QList<roseHome::PlaylistItemData>();
    }

    /**
     * @brief 소멸자.
     */
    RoseTubeHistoryPlaylistAll::~RoseTubeHistoryPlaylistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void RoseTubeHistoryPlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

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
    void RoseTubeHistoryPlaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // init
            this->list_playlist->clear();
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
    void RoseTubeHistoryPlaylistAll::setUIControl_playlist(){

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
    void RoseTubeHistoryPlaylistAll::proc_wheelEvent_to_getMoreData(){

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
    void RoseTubeHistoryPlaylistAll::resizeEvent(QResizeEvent *event){//c230223

        AbstractRoseHomeSubWidget::resizeEvent(event);

        this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

    }

    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    void RoseTubeHistoryPlaylistAll::request_more_Data(){

        // next_offset
        if(this->list_playlist->size() == 0){
            this->playlist_next_offset = 0;
        }
        else{
            this->playlist_next_offset++;
        }

        roseHome::ProcCommon *proc_playlist = new roseHome::ProcCommon(this);
        connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_playlists, this, &RoseTubeHistoryPlaylistAll::slot_applyResult_historyPlaylist);
        proc_playlist->setProperty("idx", this->playlist_next_offset);
        proc_playlist->request_rose_getList_historyPlaylists(this->data_pageInfo.api_subPath, this->data_pageInfo.filter_type, this->playlist_next_offset, 200);

        if(this->playlist_next_offset == 0){
            this->flag_playlist_draw = true;
        }
    }

    void RoseTubeHistoryPlaylistAll::request_more_Draw(){

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
            this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->widget_playlist[i]->setData(this->list_playlist->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
            connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseTubeHistoryPlaylistAll::slot_clickedItemPlaylist);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_playlist_draw = false;
    }

    // MARK : User Event Handler (slots) ---------------------------------------------------------------------------------

    void RoseTubeHistoryPlaylistAll::slot_applyResult_historyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i]->setData(this->list_playlist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseTubeHistoryPlaylistAll::slot_clickedItemPlaylist);
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
    void RoseTubeHistoryPlaylistAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___playlists){
            //this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);

            roseHome::PlaylistItemData data_playlist = this->list_playlist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data_playlist), index, section, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                // Playlist Detail 페이지 진입
                QJsonObject tmp_data;
                tmp_data.insert("id", data_playlist.id);
                tmp_data.insert("ownerId", data_playlist.ownerId);
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYDETAIL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_MYDETAIL);

                // 부모에게 페이지 변경하라고 시그널 보냄
                emit this->signal_clickedViewAll(tmp_data);
            }
        }
    }


    void RoseTubeHistoryPlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, section, clickMode);
        }
    }





    /**
     * @brief "ROSEHOME > REcENT TRACK" 화면을 위한 생성자. @see PAGECODE_RH_TRACK_LIST_ALL_VIEW
     * @param parent
     */
    RoseTubeHistoryTrackAll::RoseTubeHistoryTrackAll(QWidget *parent) : AbstractRoseHomeSubWidget(VerticalScroll_roseviewAll, parent) {

        // data
        this->list_track = new QList<roseHome::TrackItemData>();

        // UI
        this->setUIControl_tracks();
    }


    /**
    * @brief 소멸자.
    */
    RoseTubeHistoryTrackAll::~RoseTubeHistoryTrackAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void RoseTubeHistoryTrackAll::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::PageInfo_TrackAllView tmp_pageInfo = roseHome::ConvertData::convertData_pageInfo_trackAllView(jsonObj);
        this->flagNeedReload = false;

        if(tmp_pageInfo.api_subPath != this->data_pageInfo.api_subPath || tmp_pageInfo.pathTitle != this->data_pageInfo.pathTitle || tmp_pageInfo.filter_type != this->data_pageInfo.filter_type){
            this->flagNeedReload = true;

            this->data_pageInfo = tmp_pageInfo;

            this->label_mainTitle->setText(this->data_pageInfo.pathTitle);

            // init
            this->list_track->clear();
            this->jsonArr_tracks_toPlay = QJsonArray();

            this->track_id_fav = 0;
            this->track_idx_fav = 0;
            this->track_star_fav = 0;

            this->track_totalCount = 0;
            this->track_drawCount = 0;
            this->track_favoriteOffset = 0;

            this->flag_draw = false;
            this->flag_check_track = false;
            this->flag_track_fav = false;
            this->flag_send_track = false;

            // request HTTP API
            this->flagReqMore_track = false;
            this->flag_lastPage_track = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief RoseHomeTrackListAll::setActivePage
     */
    void RoseTubeHistoryTrackAll::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

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
            connect(btn_playAll, &QPushButton::clicked, this, &RoseTubeHistoryTrackAll::slot_btnClicked_playAll);

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
            connect(btn_shuffleAll, &QPushButton::clicked, this, &RoseTubeHistoryTrackAll::slot_btnClicked_playShuffle);

            this->box_filter->addWidget(this->widget_filter_contents);

            // init
            GSCommon::clearLayout(this->flowLayout_tracks);

            this->flagReqMore_track = false;
            this->flag_lastPage_track = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackData();
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
    void RoseTubeHistoryTrackAll::setUIControl_tracks(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("");
        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        rosetube::ItemTrack_rosetube *listAlbum = new rosetube::ItemTrack_rosetube(0, 0, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);

        this->track_widget_width = listAlbum->get_fixedWidth();
        this->track_widget_margin = listAlbum->get_rightMargin();

        delete listAlbum;

        this->flowLayout_tracks = this->get_addUIControl_flowLayout(0, 20);
    }

    void RoseTubeHistoryTrackAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_track && !this->flag_lastPage_track) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_track = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_trackData();
        }
        else if((this->track_totalCount > this->track_drawCount) && this->flag_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_trackDraw();
        }
    }

    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void RoseTubeHistoryTrackAll::resizeEvent(QResizeEvent *event){

        AbstractRoseHomeSubWidget::resizeEvent(event);

        this->setFlowLayoutResize(this, this->flowLayout_tracks, this->track_widget_width, this->track_widget_margin);
    }

    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void RoseTubeHistoryTrackAll::request_more_trackData(){

        // next_offset
        if(this->list_track->size() == 0){
            this->track_next_offset = 0;
        }
        else{
            this->track_next_offset++;
        }

        // request HTTP API
        roseHome::ProcCommon *proc_track = new roseHome::ProcCommon(this);
        connect(proc_track, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseTubeHistoryTrackAll::slot_applyResult_historyTrack);
        proc_track->setProperty("idx", this->track_next_offset);
        proc_track->request_rose_getList_historyTracks(this->data_pageInfo.api_subPath, this->data_pageInfo.filter_type, this->track_next_offset, 200);

        if(this->track_next_offset == 0){
            this->flag_draw = true;
        }
    }
    void RoseTubeHistoryTrackAll::request_more_trackDraw(){

        this->flag_draw = true;

        int f_width = this->flowLayout_tracks->geometry().width();

        if(this->flowLayout_tracks->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->track_widget_width + this->track_widget_margin);
        int f_mod = this->track_drawCount % f_wg_cnt;

        if(f_mod == 0){
            this->track_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->track_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > this->track_widget_cnt ) ? this->track_widget_cnt : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->historyTrack[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->track_drawCount; i++){
            this->historyTrack[i]->setData(this->jsonArr_tracks_toPlay.at(i).toObject());
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->track_drawCount; i++){
            connect(this->historyTrack[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTubeHistoryTrackAll::slot_clickedItemPlaylist);
            this->flowLayout_tracks->addWidget(this->historyTrack[i]);
        }

        ContentLoadingwaitingMsgHide();

        this->flag_draw = false;
    }

    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] Track 데이터를 받아서 처리함
     * @param list_data
     */
    void RoseTubeHistoryTrackAll::slot_applyResult_historyTrack(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        int idx = sender()->property("idx").toInt();

        if(idx != this->track_next_offset){
            return;
        }

        if(list_data.size() > 0){
            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            if(list_data.size() == 0){
                ContentLoadingwaitingMsgHide();
                return;
            }

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
                this->track_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_track->count();

            this->list_track->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                this->flag_draw = true;

                int f_width = this->flowLayout_tracks->geometry().width();

                if(this->flowLayout_tracks->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                int f_wg_cnt = f_width / (this->track_widget_width + this->track_widget_margin);
                int f_mod = this->track_drawCount % f_wg_cnt;

                if(f_mod == 0){
                    this->track_widget_cnt = f_wg_cnt * 10;
                }
                else{
                    this->track_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                }

                int start_index = this->track_drawCount;
                int max_cnt = ((this->track_totalCount - this->track_drawCount) > this->track_widget_cnt ) ? this->track_widget_cnt : (this->track_totalCount - this->track_drawCount);
                this->track_drawCount += max_cnt;

                for(int i = start_index; i < this->track_drawCount; i++){
                    this->historyTrack[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->track_drawCount; i++){
                    this->historyTrack[i]->setData(this->jsonArr_tracks_toPlay.at(i).toObject());
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->track_drawCount; i++){
                    connect(this->historyTrack[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTubeHistoryTrackAll::slot_clickedItemPlaylist);
                    this->flowLayout_tracks->addWidget(this->historyTrack[i]);
                }

                ContentLoadingwaitingMsgHide();

                this->flag_draw = false;
            }
            else{
                this->request_more_trackDraw();
            }
            /*else{
                if(flag_lastPage == false){
                    this->request_more_trackData();
                }
            }*/

        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->list_track->size() <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_tracks->geometry().width();

                if(this->flowLayout_tracks->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_tracks->addWidget(noData_widget);
            }
        }
    }


    void RoseTubeHistoryTrackAll::slot_applyResult_getRating_track(const QJsonArray &contents){

        /*if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->track_listAll[this->track_favoriteOffset + i]->setFavoritesIds(flag, star);
                }
            }
            else if(track_info.contains("message") && (track_info["message"].toString() == "정상")){
                this->track_listAll[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }*/
    }


    void RoseTubeHistoryTrackAll::slot_btnClicked_playAll(){

        roseHome::TrackItemData data_track;

        // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
        procRosePlay->requestPlayRose_byTracks(data_track, this->jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow_withReorder, AbstractProcRosePlay::PlayShuffleMode::JustPlay);
    }

    void RoseTubeHistoryTrackAll::slot_btnClicked_playShuffle(){

        roseHome::TrackItemData data_track;

        // Track shuffle 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
        procRosePlay->requestPlayRose_byTracks(data_track, this->jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow_withReorder, AbstractProcRosePlay::PlayShuffleMode::ShufflePlay);
    }


    void RoseTubeHistoryTrackAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___playlists){
            if(clickMode == tidal::AbstractItem::ClickMode::AllBox || clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Track 바로 재생 - clicked 항목만 재생
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

                roseHome::TrackItemData data_playlist;
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data_playlist, this->jsonArr_tracks_toPlay, index, playType);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AddCollectionBtn){
                // 플레이리스트 담기 - Track
                //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(index).toObject();

                QJsonObject data = ProcJsonEasy::getJsonObject(tmpObj, "data");
                QJsonObject snippet = ProcJsonEasy::getJsonObject(data, "snippet");

                OptMorePopup::HeaderData data_header;
                data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                data_header.sub_title = ProcJsonEasy::getString(snippet, "channelTitle");
                data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(tmpObj, "id"));
                data_header.type = "YOUTUBE";      //j220906 share link
                data_header.flagProcStar = false;
                data_header.isShare = true;      //j220906 share link

                // OptMorePopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosetube_Video, data_header, index, section);
            }
        }
    }


    /**
     * @brief [Slot] PlaylistTrackDetailInfo_RHV signal에 대하여 처리
     * @param clickMode
     */
    void RoseTubeHistoryTrackAll::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        /*if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->track_listAll[idx]->getFavoritesStars();
                this->flag_track_fav = false;

                if(this->track_star_fav == 3){
                    this->track_star_fav = 0;
                    this->flag_track_fav = false;
                }
                else if(this->track_star_fav >= 0 && this->track_star_fav < 3){
                    this->track_star_fav++;
                    this->flag_track_fav = true;
                }

                this->track_idx_fav = idx;

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_track_fav);
                ratingInfo.insert("star", this->track_star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", this->list_track->at(idx).type);

                QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseTubeHistoryTrackAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

                this->flag_check_track = true;
            }
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, SECTION_FOR_MORE_POPUP___tracks);
        }*/
    }

    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void RoseTubeHistoryTrackAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___playlists){
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
                // Rose Play 요청
                roseHome::TrackItemData data_track;
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data_track, this->jsonArr_tracks_toPlay, index, clickMode);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubscribInfo){
                QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(index).toObject();
                QJsonObject dataObj = ProcJsonEasy::getJsonObject(tmpObj, "data");
                QJsonObject snippet = ProcJsonEasy::getJsonObject(dataObj, "snippet");
                QString channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                QString channelId = ProcJsonEasy::getString(snippet, "channelId");

                QJsonObject tmp_data;
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert("title", channelName);
                tmp_data.insert("channel_id", channelId);

                emit this->signal_clickedViewAll(tmp_data);
            }
            else if(clickMode == OptMorePopup::ClickMode::Share){//c220818
                print_debug();//c220818-3dot
                setUIShare();//c220818
                print_debug();
                qDebug() << "this->shareLink="<<this->shareLink;//c220818
            }
            else if(clickMode == OptMorePopup::ClickMode::Caching_ahead){
                QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(index).toObject();

                QString channelId = "";
                QString channelName = "";
                QString duration = "";
                QString id = "";
                QString thumbnailUrl = "";
                QString title = "";

                if(trackInfo.contains("data")){
                    QJsonObject trackData = ProcJsonEasy::getJsonObject(trackInfo, "data");
                    QJsonObject detailContents = ProcJsonEasy::getJsonObject(trackData, "detailContents");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(trackData, "snippet");
                    QJsonObject trackStatistic = ProcJsonEasy::getJsonObject(trackData, "detailContents");

                    QString tmpDuration = ProcJsonEasy::getString(detailContents, "duration");

                    QString time = tmpDuration;
                    QString hour;
                    QString minute;
                    QString second;

                    if(tmpDuration.indexOf("PT") >= 0){
                        time = tmpDuration.right(tmpDuration.size() - 2);
                    }
                    if(time.indexOf("H") >= 0){
                        hour = time.section("H", 0, -2);
                        time = time.mid(time.indexOf("H") + 1);
                    }
                    if(time.indexOf("M") >= 0){
                        minute = time.section("M", 0, -2);
                        time = time.mid(time.indexOf("M") + 1);
                    }
                    if(time.indexOf("S") >= 0){
                        second = time.section("S", 0, -2);
                    }

                    if(hour.isEmpty()){
                        if(minute.isEmpty()){
                            duration = "0:" + second.rightJustified(2, '0');
                        }
                        else{
                            duration = minute + ":" + second.rightJustified(2, '0');
                        }
                    }
                    else{
                        duration = hour + ":" + minute.rightJustified(2, '0') + ":" + second.rightJustified(2, '0');
                    }

                    channelId = ProcJsonEasy::getString(snippet, "channelId");
                    channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                    id = ProcJsonEasy::getString(trackInfo, "clientKey");
                    thumbnailUrl = ProcJsonEasy::getString(trackInfo, "thumbnailUrl");
                    title = ProcJsonEasy::getString(trackInfo, "title");
                }
                else{
                    if(trackInfo.contains("channelTitle")){
                        channelName = ProcJsonEasy::getString(trackInfo, "channelTitle");
                    }
                    else if(trackInfo.contains("channelName")){
                        channelName = ProcJsonEasy::getString(trackInfo, "channelName");
                    }

                    int time = ProcJsonEasy::getInt(trackInfo, "duration");
                    if(time == 0){
                        QString tmpDur = ProcJsonEasy::getString(trackInfo, "duration");

                        if(tmpDur.isEmpty()){
                            duration = "";
                        }
                        else{
                            duration = tmpDur;
                        }
                    }
                    else if(time >= 3600){
                        duration = QDateTime::fromTime_t(time).toUTC().toString("hh:mm:ss");
                    }
                    else{
                        duration = QDateTime::fromTime_t(time).toUTC().toString("mm:ss");
                    }

                    channelId = ProcJsonEasy::getString(trackInfo, "channelId");
                    id = ProcJsonEasy::getString(trackInfo, "id");
                    thumbnailUrl = ProcJsonEasy::getString(trackInfo, "thumbnailUrl");
                    title = ProcJsonEasy::getString(trackInfo, "title");
                }

                QJsonObject data;
                data.insert("channelId", channelId);
                data.insert("channelName", channelName);
                data.insert("duration", duration);
                data.insert("id", id);
                data.insert("isFavorite", false);
                data.insert("isLiveStream", false);
                data.insert("thumbnailUrl", thumbnailUrl);
                data.insert("thumbsUpPercentage", 0);
                data.insert("title", title);
                data.insert("viewsCount", "-1");

                QJsonObject json;
                json.insert("data", data);
                json.insert("isDelete", false);

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_download_item";
                network->request(HTTP_CACHE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
        }
    }





    /**
         * @brief QobuzMoodsMain::QobuzMoodsMain
         * @param parent
         */
    RosetubeMyRosePlaylistAll::RosetubeMyRosePlaylistAll(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // Data
        this->list_playlist = new QList<roseHome::PlaylistItemData>();
        this->list_filterOpt_sort = roseHome::AbstractRoseHomeSubWidget::get_list_filterOptData_forMyCollection();

        if(global.user.isValid() == true){

            // 기본 UI 세팅
            this->setUIControl_playlist();
        }
    }


    /**
         * @brief 소멸자.
         */
    RosetubeMyRosePlaylistAll::~RosetubeMyRosePlaylistAll(){

        this->deleteLater();
    }


    /**
         * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
         * @param jsonObj
         */
    void RosetubeMyRosePlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "viewAllMode");
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
    void RosetubeMyRosePlaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            roseHome::AbstractRoseHomeSubWidget::setActivePage();

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
    void RosetubeMyRosePlaylistAll::setUIControl_playlist(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Playlists");

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        roseHome::ItemPlaylist_rosehome *listAlbum =  new roseHome::ItemPlaylist_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);

        this->playlist_widget_width = listAlbum->get_fixedWidth();
        this->playlist_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeAlbumListAll::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;

        // layout for items
        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
        GSCommon::clearLayout(this->flowLayout_playlists);

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &RosetubeMyRosePlaylistAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
         * @brief QobuzMyPlaylistAll::setUIControl_filter
         */
    void RosetubeMyRosePlaylistAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &RosetubeMyRosePlaylistAll::slot_filterClicked);


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
    void RosetubeMyRosePlaylistAll::proc_wheelEvent_to_getMoreData(){

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
    void RosetubeMyRosePlaylistAll::resizeEvent(QResizeEvent *event){

        roseHome::AbstractRoseHomeSubWidget::resizeEvent(event);

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
    void RosetubeMyRosePlaylistAll::slot_filterClicked(){

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
    void RosetubeMyRosePlaylistAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

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
    void RosetubeMyRosePlaylistAll::request_more_playlistData(){

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
        connect(proc, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &RosetubeMyRosePlaylistAll::slot_applyResult_myPlaylist_rose);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_myPlaylists("member/playlist", "YOUTUBE", this->next_offset, GET_ITEM_SIZE___ONCE);

        if(this->next_offset == 0){
            this->flag_playlist_draw = true;
        }

    }




    void RosetubeMyRosePlaylistAll::request_more_playlistDraw(){

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
            this->listPlaylist_all[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);
            connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RosetubeMyRosePlaylistAll::slot_clickedItemPlaylist);
        }

        this->flag_playlist_draw = false;

        ContentLoadingwaitingMsgHide();      //cheon Tidal
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
    void RosetubeMyRosePlaylistAll::slot_applyResult_myPlaylist_rose(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    this->listPlaylist_all[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);
                    connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RosetubeMyRosePlaylistAll::slot_clickedItemPlaylist);
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


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RosetubeMyRosePlaylistAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___playlists){
            roseHome::PlaylistItemData data_playlist = this->list_playlist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                print_debug();

                print_debug();
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data_playlist), index, SECTION_FOR_MORE_POPUP___playlists, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                // Playlist Detail 페이지 진입
                QJsonObject tmp_data;
                tmp_data.insert("id", data_playlist.id);
                tmp_data.insert("ownerId", data_playlist.ownerId);
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYDETAIL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_MYDETAIL);
                // 부모에게 페이지 변경하라고 시그널 보냄
                emit this->signal_clickedViewAll(tmp_data);
            }
        }

        print_debug();
    }


    void RosetubeMyRosePlaylistAll::slot_applyResult_getShareLink(const QString &link){//c220818 share link

        this->shareLink = link;
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
    void RosetubeMyRosePlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___playlists){
            if(clickMode == OptMorePopup::ClickMode::Edit){
                QString view_type = "edit";

                QJsonObject data;
                data.insert("view_type", view_type);
                data.insert("playlist_id", this->list_playlist->at(index).id);
                data.insert("type", "ROSE");

                QJsonObject jsonObj_move;
                jsonObj_move.insert("data", data);

                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RT_ADDPLAYLIST);

                emit linker->signal_clickedMovePage(jsonObj_move);
            }
            else{
                this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, SECTION_FOR_MORE_POPUP___playlists, clickMode);
            }
        }
    }





    /**
         * @brief QobuzMoodsMain::QobuzMoodsMain
         * @param parent
         */
    RosetubeUserRosePlaylistAll::RosetubeUserRosePlaylistAll(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // Data
        this->list_playlist = new QList<roseHome::PlaylistItemData>();
        this->list_filterOpt_sort = roseHome::AbstractRoseHomeSubWidget::get_list_filterOptData_forMyCollection();

        if(global.user.isValid() == true){

            // 기본 UI 세팅
            this->setUIControl_playlist();
        }
    }


    /**
         * @brief 소멸자.
         */
    RosetubeUserRosePlaylistAll::~RosetubeUserRosePlaylistAll(){

        this->deleteLater();
    }


    /**
         * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
         * @param jsonObj
         */
    void RosetubeUserRosePlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "viewAllMode");
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
    void RosetubeUserRosePlaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            roseHome::AbstractRoseHomeSubWidget::setActivePage();

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
    void RosetubeUserRosePlaylistAll::setUIControl_playlist(){

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
        GSCommon::clearLayout(this->flowLayout_playlists);

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &RosetubeUserRosePlaylistAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
         * @brief QobuzMyPlaylistAll::setUIControl_filter
         */
    void RosetubeUserRosePlaylistAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &RosetubeUserRosePlaylistAll::slot_filterClicked);


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
    void RosetubeUserRosePlaylistAll::proc_wheelEvent_to_getMoreData(){

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
    void RosetubeUserRosePlaylistAll::resizeEvent(QResizeEvent *event){

        roseHome::AbstractRoseHomeSubWidget::resizeEvent(event);

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
    void RosetubeUserRosePlaylistAll::slot_filterClicked(){

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
    void RosetubeUserRosePlaylistAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

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
    void RosetubeUserRosePlaylistAll::request_more_playlistData(){

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
        connect(proc, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &RosetubeUserRosePlaylistAll::slot_applyResult_userPlaylist_rose);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_usersPlaylists("member/playlist/all", "PLAYLIST_RECENT", "YOUTUBE", this->next_offset, GET_ITEM_SIZE___ONCE);

        if(this->next_offset == 0){
            this->flag_playlist_draw = true;
        }

    }


    void RosetubeUserRosePlaylistAll::request_more_playlistDraw(){

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
            this->listPlaylist_all[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);
            connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RosetubeUserRosePlaylistAll::slot_clickedItemPlaylist);
        }

        this->flag_playlist_draw = false;

        ContentLoadingwaitingMsgHide();      //cheon Tidal
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
    void RosetubeUserRosePlaylistAll::slot_applyResult_userPlaylist_rose(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    this->listPlaylist_all[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);
                    connect(this->listPlaylist_all[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RosetubeUserRosePlaylistAll::slot_clickedItemPlaylist);
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


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RosetubeUserRosePlaylistAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___playlists){
            roseHome::PlaylistItemData data_playlist = this->list_playlist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                print_debug();

                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data_playlist), index, SECTION_FOR_MORE_POPUP___playlists, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                // Playlist Detail 페이지 진입
                QJsonObject tmp_data;
                tmp_data.insert("id", data_playlist.id);
                tmp_data.insert("ownerId", data_playlist.ownerId);
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYDETAIL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_MYDETAIL);
                // 부모에게 페이지 변경하라고 시그널 보냄
                emit this->signal_clickedViewAll(tmp_data);
            }
        }
    }


    void RosetubeUserRosePlaylistAll::slot_applyResult_getShareLink(const QString &link){//c220818 share link

        this->shareLink = link;
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
    void RosetubeUserRosePlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, SECTION_FOR_MORE_POPUP___playlists, clickMode);
        }
    }
}
