#include "music/MusicPlaylistAll.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"


namespace music {

    const int SECTION_FOR_MORE_POPUP___PLAYLIST = 0;


    /**
     * @brief PlaylistAll::PlaylistAll
     * @param parent
     */
    PlaylistAll::PlaylistAll(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        this->linker = Linker::getInstance();

        // 기본 UI 세팅
        this->label_mainTitle = this->get_addUIControl_mainTitle("noname title");
        this->setUIControl_playlist();

        this->list_playlist = new QList<roseHome::PlaylistItemData>();
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

            // init
            this->list_playlist->clear();
            GSCommon::clearLayout(this->flowLayout_playlists);

            this->flagReqMore_playlist = false;
            this->flag_lastPage_playlist = false;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

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
    void PlaylistAll::setUIControl_playlist(){

        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void PlaylistAll::proc_wheelEvent_to_getMoreData(){

        if((this->playlist_totalCount > this->playlist_drawCount) && (this->list_playlist->size() > this->playlist_drawCount) && (this->flag_playlist_draw == false)){

            this->flag_playlist_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_Draw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    void PlaylistAll::request_more_Data(){

        if(!this->flagReqMore_playlist && !this->flag_lastPage_playlist){
            this->flagReqMore_playlist = true;

            // next_offset
            if(this->list_playlist->size() == 0){
                this->playlist_next_offset = 0;
            }
            else{
                this->playlist_next_offset++;
            }

            roseHome::ProcCommon *proc_playlist = new roseHome::ProcCommon(this);
            connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_playlists, this, &PlaylistAll::slot_applyResult_playlists);

            proc_playlist->request_rose_getList_recentlyPlaylists(this->curr_api_subPath, "MUSIC", this->playlist_next_offset, 20);
        }
    }


    void PlaylistAll::request_more_Draw(){

        int start_index = this->playlist_drawCount;
        int max_cnt = ((this->playlist_totalCount - this->playlist_drawCount) > 20 ) ? 20 : (this->playlist_totalCount - this->playlist_drawCount);
        this->playlist_drawCount += max_cnt;

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, 2, true);
            connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &PlaylistAll::slot_clickedItemPlaylist);
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

    void PlaylistAll::slot_applyResult_playlists(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        ContentLoadingwaitingMsgHide();//c220616
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
                    this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, 2, true);
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &PlaylistAll::slot_clickedItemPlaylist);
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
     * @brief PlaylistAll::PlaylistAll
     * @param parent
     */
    MyPlaylistAll::MyPlaylistAll(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

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

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

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

        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void MyPlaylistAll::proc_wheelEvent_to_getMoreData(){

        if((this->playlist_totalCount > this->playlist_drawCount) && (this->list_playlist->size() > this->playlist_drawCount) && (this->flag_playlist_draw == false)){

            this->flag_playlist_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_Draw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    void MyPlaylistAll::request_more_Data(){

        if(!this->flagReqMore_playlist && !this->flag_lastPage_playlist){
            this->flagReqMore_playlist = true;

            // next_offset
            if(this->list_playlist->size() == 0){
                this->playlist_next_offset = 0;
            }
            else{
                this->playlist_next_offset++;
            }

            roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
            connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &MyPlaylistAll::slot_applyResult_playlists);
            proc_myPlaylist->request_rose_getList_myPlaylists(this->curr_api_subPath , "MUSIC", this->playlist_next_offset, 15);
        }
    }


    void MyPlaylistAll::request_more_Draw(){

        int start_index = this->playlist_drawCount;
        int max_cnt = ((this->playlist_totalCount - this->playlist_drawCount) > 20 ) ? 20 : (this->playlist_totalCount - this->playlist_drawCount);
        this->playlist_drawCount += max_cnt;

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
            connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &MyPlaylistAll::slot_clickedItemPlaylist);
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

    void MyPlaylistAll::slot_applyResult_playlists(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &MyPlaylistAll::slot_clickedItemPlaylist);
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
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, SECTION_FOR_MORE_POPUP___PLAYLIST, clickMode);
        }
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

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

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

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

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
}
