#include "music/MusicPlaylistAll.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"

#include <QScrollBar>

namespace music {

    const int SECTION_FOR_MORE_POPUP___PLAYLIST = 0;


    /**
     * @brief PlaylistAll::PlaylistAll
     * @param parent
     */
    PlaylistAll::PlaylistAll(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        this->linker = Linker::getInstance();

        this->list_playlist = new QList<roseHome::PlaylistItemData>();

        // 기본 UI 세팅
        this->setUIControl_playlist();
    }

    /**
     * @brief 소멸자.
     */
    PlaylistAll::~PlaylistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void PlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::PageInfo_PlaylistAllView data_pageInfo = roseHome::ConvertData::convertData_pageInfo_playlistAllView(jsonObj);
        this->flagNeedReload = false;

        if(this->curr_api_subPath != data_pageInfo.api_subPath || this->curr_api_mainTitle != data_pageInfo.pathTitle){
            this->flagNeedReload = true;

            this->curr_api_subPath = data_pageInfo.api_subPath;
            this->curr_api_mainTitle = data_pageInfo.pathTitle;

            this->label_mainTitle->setText(this->curr_api_mainTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

            // init
            this->playlist_next_offset = 0;
            this->playlist_totalCount = 0;
            this->playlist_drawCount = 0;

            this->list_playlist->clear();

            // request HTTP API
            this->flagReqMore_playlist = false;
            this->flag_lastPage_playlist = false;

            this->flag_playlist_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_Data();
        }
        else{
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_playlists, this, &PlaylistAll::slot_applyResult_playlistCheck);
            proc->request_rose_getList_recentlyPlaylists(this->curr_api_subPath, "MUSIC", 0, 10);
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void PlaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            roseHome::AbstractRoseHomeSubWidget::setActivePage();

            GSCommon::clearLayout(this->flowLayout_playlists);

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
    void PlaylistAll::setUIControl_playlist(){

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

        connect(this->btn_mainTitle, &QPushButton::clicked, this, &PlaylistAll::slot_btnClicked_Delete);

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

        roseHome::ItemPlaylist_rosehome *listAlbum = new roseHome::ItemPlaylist_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, 2, true);

        this->playlist_widget_width = listAlbum->get_fixedWidth();
        this->playlist_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeAlbumListAll::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;

        // layout for items
        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void PlaylistAll::initAll(){

        // init
        this->playlist_next_offset = 0;
        this->playlist_totalCount = 0;
        this->playlist_drawCount = 0;

        this->list_playlist->clear();

        // request HTTP API
        this->flagReqMore_playlist = false;
        this->flag_lastPage_playlist = false;

        this->flag_playlist_draw = false;

        GSCommon::clearLayout(this->flowLayout_playlists);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void PlaylistAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_playlist && !this->flag_lastPage_playlist) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_playlist = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_Data();
        }
        else if((this->playlist_totalCount > this->playlist_drawCount) && this->flag_playlist_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_Draw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    void PlaylistAll::request_more_Data(){

        // next_offset
        if(this->list_playlist->size() == 0){
            this->playlist_next_offset = 0;
        }
        else{
            this->playlist_next_offset++;
        }

        roseHome::ProcCommon *proc_playlist = new roseHome::ProcCommon(this);
        connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_playlists, this, &PlaylistAll::slot_applyResult_playlists);
        proc_playlist->setProperty("idx", this->playlist_next_offset);
        proc_playlist->request_rose_getList_recentlyPlaylists(this->curr_api_subPath, "MUSIC", this->playlist_next_offset, 200);

        if(this->playlist_next_offset == 0){
            this->flag_playlist_draw = true;
        }
    }


    void PlaylistAll::request_more_Draw(){

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
            this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, 2, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->widget_playlist[i]->setData(this->list_playlist->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
            connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &PlaylistAll::slot_clickedItemPlaylist);
        }

        ContentLoadingwaitingMsgHide();

        this->flag_playlist_draw = false;
    }


    // MARK : User Event Handler (slots) ---------------------------------------------------------------------------------

    void PlaylistAll::slot_applyResult_playlists(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, 2, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i]->setData(this->list_playlist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &PlaylistAll::slot_clickedItemPlaylist);
                }

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_playlist_draw = false;
            }
            else{
                this->request_more_Draw();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

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


    void PlaylistAll::slot_applyResult_playlistCheck(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                this->request_more_Data();

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
                    this->request_more_Data();

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
            this->request_more_Data();

            this->flagNeedReload = true;
            this->setActivePage();
        }
        else{
            ContentLoadingwaitingMsgHide();
        }
    }


    void PlaylistAll::slot_btnClicked_Delete(){

        QStringList tmpTitle = this->curr_api_mainTitle.split("(");

        QJsonObject tmpObj;
        tmpObj.insert("contents_type", tmpTitle.at(0));
        tmpObj.insert("list_type", "PLAYLIST");
        tmpObj.insert("api_subPath", this->curr_api_subPath);
        tmpObj.insert("filter_type", "MUSIC");

        QJsonObject tmpObj_move;
        tmpObj_move.insert(KEY_PAGE_CODE, PAGECODE_VA_RECENTLY_LIST_DELETE);
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
    void PlaylistAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___PLAYLIST){

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
                QJsonObject jsonData;
                jsonData.insert("id", data_playlist.id);
                jsonData.insert("ownerId", data_playlist.ownerId);
                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_RECENTPLAYLISTINFO);

                emit linker->signal_clickedMovePage(jsonData);
            }
        }
    }


    void PlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___PLAYLIST){

            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, SECTION_FOR_MORE_POPUP___PLAYLIST, clickMode);
        }
    }


    /**
     * @brief PlaylistAll::resizeEvent
     * @param event
     */
    void PlaylistAll::resizeEvent(QResizeEvent *event){
        Q_UNUSED(event);

        this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

        this->widget_mainTitle->resize(global.LmtCnt, 69);
        this->btn_mainTitle->setGeometry(global.LmtCnt - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);
    }




    /**
     * @brief PlaylistAll::PlaylistAll
     * @param parent
     */
    MyPlaylistAll::MyPlaylistAll(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        this->linker = Linker::getInstance();

        // 기본 UI 세팅
        this->label_mainTitle = this->get_addUIControl_mainTitle("noname title");
        this->setUIControl_playlist();

        this->list_playlist = new QList<roseHome::PlaylistItemData>();
    }

    /**
     * @brief 소멸자.
     */
    MyPlaylistAll::~MyPlaylistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void MyPlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::PageInfo_PlaylistAllView data_pageInfo = roseHome::ConvertData::convertData_pageInfo_playlistAllView(jsonObj);
        this->flagNeedReload = false;

        if(this->curr_api_subPath != data_pageInfo.api_subPath || this->curr_api_mainTitle != data_pageInfo.pathTitle){
            this->flagNeedReload = true;

            this->curr_api_subPath = data_pageInfo.api_subPath;
            this->curr_api_mainTitle = data_pageInfo.pathTitle;

            this->label_mainTitle->setText(this->curr_api_mainTitle);
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void MyPlaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            roseHome::AbstractRoseHomeSubWidget::setActivePage();

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
    void MyPlaylistAll::setUIControl_playlist(){

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
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void MyPlaylistAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_playlist && !this->flag_lastPage_playlist) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_playlist = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_Data();
        }
        else if((this->playlist_totalCount > this->playlist_drawCount) && this->flag_playlist_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_Draw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    void MyPlaylistAll::request_more_Data(){

        // next_offset
        if(this->list_playlist->size() == 0){
            this->playlist_next_offset = 0;
        }
        else{
            this->playlist_next_offset++;
        }

        roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
        connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &MyPlaylistAll::slot_applyResult_playlists);
        proc_myPlaylist->setProperty("idx", this->playlist_next_offset);
        proc_myPlaylist->request_rose_getList_myPlaylists(this->curr_api_subPath , "MUSIC", this->playlist_next_offset, 200);

        if(this->playlist_next_offset == 0){
            this->flag_playlist_draw = true;
        }
    }


    void MyPlaylistAll::request_more_Draw(){

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
            this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->widget_playlist[i]->setData(this->list_playlist->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
             connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &MyPlaylistAll::slot_clickedItemPlaylist);
        }

        ContentLoadingwaitingMsgHide();

        this->flag_playlist_draw = false;
    }


    // MARK : User Event Handler (slots) ---------------------------------------------------------------------------------

    void MyPlaylistAll::slot_applyResult_playlists(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i]->setData(this->list_playlist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &MyPlaylistAll::slot_clickedItemPlaylist);
                }

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_playlist_draw = false;
            }
            else{
                this->request_more_Draw();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

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
    void MyPlaylistAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___PLAYLIST){

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
                QJsonObject jsonData;
                jsonData.insert("id", data_playlist.id);
                jsonData.insert("ownerId", data_playlist.ownerId);
                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_RECENTPLAYLISTINFO);

                emit linker->signal_clickedMovePage(jsonData);
            }
        }
    }


    void MyPlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___PLAYLIST){
            if(clickMode == OptMorePopup::ClickMode::Edit){
                QString view_type = "edit";

                QJsonObject data;
                data.insert("view_type", view_type);
                data.insert("playlist_id", this->list_playlist->at(index).id);
                data.insert("type", "ROSE");

                QJsonObject jsonObj_move;
                jsonObj_move.insert("data", data);

                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_M_ADDPLAYLIST);

                emit linker->signal_clickedMovePage(jsonObj_move);
            }
            else{
                this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, SECTION_FOR_MORE_POPUP___PLAYLIST, clickMode);
            }
        }
    }

    /**
     * @brief PlaylistAll::resizeEvent
     * @param event
     */
    void MyPlaylistAll::resizeEvent(QResizeEvent *event){
        Q_UNUSED(event);

        this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

    }



    /**
     * @brief PlaylistAll::PlaylistAll
     * @param parent
     */
    UserPlaylistAll::UserPlaylistAll(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        this->linker = Linker::getInstance();

        // 기본 UI 세팅
        this->label_mainTitle = this->get_addUIControl_mainTitle("noname title");
        this->setUIControl_playlist();

        this->list_playlist = new QList<roseHome::PlaylistItemData>();
    }

    /**
     * @brief 소멸자.
     */
    UserPlaylistAll::~UserPlaylistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void UserPlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::PageInfo_PlaylistAllView data_pageInfo = roseHome::ConvertData::convertData_pageInfo_playlistAllView(jsonObj);
        this->flagNeedReload = false;

        if(this->curr_api_subPath != data_pageInfo.api_subPath || this->curr_api_mainTitle != data_pageInfo.pathTitle){
            this->flagNeedReload = true;

            this->curr_api_subPath = data_pageInfo.api_subPath;
            this->curr_api_mainTitle = data_pageInfo.pathTitle;

            this->label_mainTitle->setText(this->curr_api_mainTitle);
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void UserPlaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            roseHome::AbstractRoseHomeSubWidget::setActivePage();

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
    void UserPlaylistAll::setUIControl_playlist(){

        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void UserPlaylistAll::proc_wheelEvent_to_getMoreData(){

        if((this->playlist_totalCount > this->playlist_drawCount) && (this->list_playlist->size() > this->playlist_drawCount) && (this->flag_playlist_draw == false)){

            this->flag_playlist_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_Draw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    void UserPlaylistAll::request_more_Data(){

        if(!this->flagReqMore_playlist && !this->flag_lastPage_playlist){
            this->flagReqMore_playlist = true;

            // next_offset
            if(this->list_playlist->size() == 0){
                this->playlist_next_offset = 0;
            }
            else{
                this->playlist_next_offset++;
            }

            roseHome::ProcCommon *proc_Playlist = new roseHome::ProcCommon(this);
            connect(proc_Playlist, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &UserPlaylistAll::slot_applyResult_playlists);
            proc_Playlist->request_rose_getList_usersPlaylists(this->curr_api_subPath , "PLAYLIST_RECENT", "MUSIC", this->playlist_next_offset, 15);
        }
    }


    void UserPlaylistAll::request_more_Draw(){

        int start_index = this->playlist_drawCount;
        int max_cnt = ((this->playlist_totalCount - this->playlist_drawCount) > 20 ) ? 20 : (this->playlist_totalCount - this->playlist_drawCount);
        this->playlist_drawCount += max_cnt;

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
            connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &UserPlaylistAll::slot_clickedItemPlaylist);
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->widget_playlist[i]->setData(this->list_playlist->at(i));
            this->flowLayout_playlists->addWidget(this->widget_playlist[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_playlist_draw = false;
    }


    // MARK : User Event Handler (slots) ---------------------------------------------------------------------------------

    void UserPlaylistAll::slot_applyResult_playlists(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                int max_cnt = this->list_playlist->size();
                this->playlist_drawCount = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &UserPlaylistAll::slot_clickedItemPlaylist);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->widget_playlist[i]->setData(this->list_playlist->at(i));
                    this->flowLayout_playlists->addWidget(this->widget_playlist[i]);

                    QCoreApplication::processEvents();
                }

                ContentLoadingwaitingMsgHide();
                this->flag_flow_draw = true;
                this->flag_playlist_draw = false;
            }

            this->request_more_Data();
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
            noData_widget->setFixedSize(1500, 500);

            this->flowLayout_playlists->addWidget(noData_widget);
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
    void UserPlaylistAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___PLAYLIST){

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
                QJsonObject jsonData;
                jsonData.insert("id", data_playlist.id);
                jsonData.insert("ownerId", data_playlist.ownerId);
                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_RECENTPLAYLISTINFO);

                emit linker->signal_clickedMovePage(jsonData);
            }
        }
    }


    void UserPlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___PLAYLIST){

            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, SECTION_FOR_MORE_POPUP___PLAYLIST, clickMode);
        }
    }





    /**
     * @brief RoseHomePlaylistListAll::RoseHomePlaylistListAll
     * @param parent
     */
    PlaylistHistoryAll::PlaylistHistoryAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // 기본 UI 세팅
        this->setUIControl_playlist();

        this->list_playlist = new QList<roseHome::PlaylistItemData>();
    }

    /**
     * @brief 소멸자.
     */
    PlaylistHistoryAll::~PlaylistHistoryAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void PlaylistHistoryAll::setJsonObject_forData(const QJsonObject &jsonObj){

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
    void PlaylistHistoryAll::setActivePage(){

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
    void PlaylistHistoryAll::setUIControl_playlist(){

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
    void PlaylistHistoryAll::proc_wheelEvent_to_getMoreData(){

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
    void PlaylistHistoryAll::resizeEvent(QResizeEvent *event){//c230223

        AbstractRoseHomeSubWidget::resizeEvent(event);

        this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

    }

    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------


    void PlaylistHistoryAll::request_more_Data(){

        // next_offset
        if(this->list_playlist->size() == 0){
            this->playlist_next_offset = 0;
        }
        else{
            this->playlist_next_offset++;
        }

        roseHome::ProcCommon *proc_playlist = new roseHome::ProcCommon(this);
        connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_playlists, this, &PlaylistHistoryAll::slot_applyResult_historyPlaylist);
        proc_playlist->setProperty("idx", this->playlist_next_offset);
        proc_playlist->request_rose_getList_historyPlaylists(this->data_pageInfo.api_subPath, this->data_pageInfo.filter_type, this->playlist_next_offset, 200);

        if(this->playlist_next_offset == 0){
            this->flag_playlist_draw = true;
        }
    }

    void PlaylistHistoryAll::request_more_Draw(){

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
            this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->widget_playlist[i]->setData(this->list_playlist->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
            connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &PlaylistHistoryAll::slot_clickedItemPlaylist);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_playlist_draw = false;
    }


    // MARK : User Event Handler (slots) ---------------------------------------------------------------------------------

    void PlaylistHistoryAll::slot_applyResult_historyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i]->setData(this->list_playlist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &PlaylistHistoryAll::slot_clickedItemPlaylist);
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
    void PlaylistHistoryAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___PLAYLIST){
            //this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);

            roseHome::PlaylistItemData data_playlist = this->list_playlist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data_playlist), index, SECTION_FOR_MORE_POPUP___PLAYLIST, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                // Playlist Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert("id", data_playlist.id);
                jsonData.insert("ownerId", data_playlist.ownerId);
                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_RECENTPLAYLISTINFO);

                emit linker->signal_clickedMovePage(jsonData);
            }
        }
    }


    void PlaylistHistoryAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___PLAYLIST){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, SECTION_FOR_MORE_POPUP___PLAYLIST, clickMode);
        }
    }
}
