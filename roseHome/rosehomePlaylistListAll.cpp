#include "rosehomePlaylistListAll.h"

#include "ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"

#include <QScrollBar>


namespace roseHome {

    const int HTTP_RECOMMENDS = 0;
    const int HTTP_RECENTLY_PLAYLIST = 2;
    const int HTTP_MY_PLAYLIST = 5;
    const int HTTP_NEW_PLAYLIST = 6;
    const int HTTP_POP_PLAYLIST = 7;
    const int HTTP_DUDUO_PLAYLIST = 9;

    const int SECTION_FOR_MORE_POPUP___PLAYLIST = 0;


    /**
     * @brief RoseHomePlaylistListAll::RoseHomePlaylistListAll
     * @param parent
     */
    RoseHomePlaylistListAll::RoseHomePlaylistListAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        this->list_playlist = new QList<roseHome::PlaylistItemData>();
        this->list_Recommends = new QList<roseHome::RecommendsItemData>();

        // 기본 UI 세팅
        this->setUIControl_playlist();
    }

    /**
     * @brief 소멸자.
     */
    RoseHomePlaylistListAll::~RoseHomePlaylistListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void RoseHomePlaylistListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        PageInfo_PlaylistAllView data_pageInfo = ConvertData::convertData_pageInfo_playlistAllView(jsonObj);
        this->flagNeedReload = false;

        if(this->current_pageInfo.api_subPath != data_pageInfo.api_subPath || this->current_pageInfo.type != data_pageInfo.type){
            this->flagNeedReload = true;

            this->current_pageInfo = data_pageInfo;

            this->label_mainTitle->setText(this->current_pageInfo.pathTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());


            if(this->current_pageInfo.type_id == HTTP_RECENTLY_PLAYLIST){
                this->btn_mainTitle->show();
            }
        }
        else{
            if(this->current_pageInfo.type_id == HTTP_RECENTLY_PLAYLIST){
                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

                // request HTTP API
                roseHome::ProcCommon *proc_recently = new roseHome::ProcCommon(this);
                connect(proc_recently, &roseHome::ProcCommon::completeReq_list_playlists, this, &RoseHomePlaylistListAll::slot_applyResult_playlistsCheck);
                proc_recently->request_rose_getList_recentlyPlaylists(this->current_pageInfo.api_subPath, this->current_pageInfo.filter_type, 0, 10);
            }
            else{
                print_debug();ContentLoadingwaitingMsgHide();   //j230328
            }
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void RoseHomePlaylistListAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // init
            this->playlist_next_offset = 0;
            this->playlist_totalCount = 0;
            this->playlist_drawCount = 0;

            this->list_playlist->clear();
            this->list_Recommends->clear();

            this->flag_playlist_draw = false;

            GSCommon::clearLayout(this->flowLayout_playlists);

            //qDebug() << "[Debug] RoseHomePlaylistListAll::setActivePage " << this->list_playlist->count() << this->list_playlist->size();

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
    void RoseHomePlaylistListAll::setUIControl_playlist(){

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
        this->btn_mainTitle->hide();

        connect(this->btn_mainTitle, &QPushButton::clicked, this, &RoseHomePlaylistListAll::slot_btnClicked_Delete);

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
        roseHome::ItemPlaylist_rosehome *listPlaylist = new roseHome::ItemPlaylist_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->current_pageInfo.type_id, true);

        this->playlist_widget_width = listPlaylist->get_fixedWidth();
        this->playlist_widget_margin = listPlaylist->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeplaylistListAll::setUIControl_playlists " << listPlaylist->get_fixedWidth() << listPlaylist->get_rightMargin();

        delete listPlaylist;
        //  get widget width, right margin - by diskept j230317 finish


        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void RoseHomePlaylistListAll::initAll(){

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
    void RoseHomePlaylistListAll::proc_wheelEvent_to_getMoreData(){

        // wheel evnet change - by diskept j230317 start
        if((!this->flagReqMore_playlist && !this->flag_lastPage_playlist) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_playlist = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_Data();
        }
        else if((this->playlist_totalCount> this->playlist_drawCount) && this->flag_playlist_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_Draw();
        }
        // wheel evnet change - by diskept j230317 start
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    void RoseHomePlaylistListAll::request_more_Data(){

        // request api change - by diskept j230317 start
        // next_offset
        if(this->list_playlist->size() == 0){
            this->playlist_next_offset = 0;
        }
        else{
            this->playlist_next_offset++;
        }
print_debug();
        if(this->current_pageInfo.type_id == HTTP_RECOMMENDS){
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_recommeds, this, &RoseHomePlaylistListAll::slot_applyResult_Recommends);
            proc->request_rose_getList_recommends(this->current_pageInfo.api_subPath, this->playlist_next_offset, 200);
        }
        else if(this->current_pageInfo.type_id == HTTP_RECENTLY_PLAYLIST){
            roseHome::ProcCommon *proc_recently = new roseHome::ProcCommon(this);
            connect(proc_recently, &roseHome::ProcCommon::completeReq_list_playlists, this, &RoseHomePlaylistListAll::slot_applyResult_playlists);
            proc_recently->setProperty("idx", this->playlist_next_offset);
            proc_recently->request_rose_getList_recentlyPlaylists(this->current_pageInfo.api_subPath, this->current_pageInfo.filter_type, this->playlist_next_offset, 200);
        }
        else if(this->current_pageInfo.type_id == HTTP_MY_PLAYLIST){
            roseHome::ProcCommon *proc_myplaylist = new roseHome::ProcCommon(this);
            connect(proc_myplaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &RoseHomePlaylistListAll::slot_applyResult_MYplaylists);
            proc_myplaylist->request_rose_getList_myPlaylists(this->current_pageInfo.api_subPath, this->current_pageInfo.filter_type, this->playlist_next_offset, 200);
        }
        else if(this->current_pageInfo.type_id == HTTP_NEW_PLAYLIST){
            roseHome::ProcCommon *proc_newplaylist = new roseHome::ProcCommon(this);
            connect(proc_newplaylist, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &RoseHomePlaylistListAll::slot_applyResult_NEWplaylists);
            proc_newplaylist->request_rose_getList_usersPlaylists(this->current_pageInfo.api_subPath, "PLAYLIST_RECENT", this->current_pageInfo.filter_type, this->playlist_next_offset, 200);
        }
        else if(this->current_pageInfo.type_id == HTTP_POP_PLAYLIST){
            roseHome::ProcCommon *proc_popplaylist = new roseHome::ProcCommon(this);
            connect(proc_popplaylist, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &RoseHomePlaylistListAll::slot_applyResult_POPplaylists);
            proc_popplaylist->request_rose_getList_usersPlaylists(this->current_pageInfo.api_subPath, "PLAYLIST_THUMBUP", this->current_pageInfo.filter_type, this->playlist_next_offset, 200);
        }
        else if(this->current_pageInfo.type_id == HTTP_DUDUO_PLAYLIST){
            roseHome::ProcCommon *proc_duduoPlaylist = new roseHome::ProcCommon(this);
            connect(proc_duduoPlaylist, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &RoseHomePlaylistListAll::slot_applyResult_DUDUOplaylists);
            proc_duduoPlaylist->request_rose_getList_duduoPlaylist(this->current_pageInfo.api_subPath, "", this->playlist_next_offset, 200);
        }
        else{
            /*roseHome::ProcCommon *proc_playlist = new roseHome::ProcCommon(this);
            connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_playlists, this, &RoseHomePlaylistListAll::slot_applyResult_playlists);
            connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &RoseHomePlaylistListAll::slot_applyResult_playlists);
            connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &RoseHomePlaylistListAll::slot_applyResult_playlists);

            switch (this->type_id) {
            case HTTP_RECOMMENDS:
                break;
            case HTTP_RECENTLY_PLAYLIST:
                proc_playlist->request_rose_getList_recentlyPlaylists(this->api_subPath, this->api_subType, this->playlist_next_offset, this->playlist_widget_cnt);
                break;
            case HTTP_MY_PLAYLIST:
                proc_playlist->request_rose_getList_myPlaylists(this->api_subPath, this->api_subType, this->playlist_next_offset, this->playlist_widget_cnt);
                break;
            case HTTP_NEW_PLAYLIST:
                proc_playlist->request_rose_getList_usersPlaylists(this->api_subPath, "PLAYLIST_RECENT", this->api_subType, this->playlist_next_offset, this->playlist_widget_cnt);
                break;
            case HTTP_POP_PLAYLIST:
                proc_playlist->request_rose_getList_usersPlaylists(this->api_subPath, "PLAYLIST_THUMBUP", this->api_subType, this->playlist_next_offset, this->playlist_widget_cnt);
                break;*/
        }
        // request api change - by diskept j230317 finish
print_debug();
        if(this->playlist_next_offset == 0){
            this->flag_playlist_draw = true;
        }
    }


    void RoseHomePlaylistListAll::request_more_Draw(){

        // widget draw change - by diskept j230317 start
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

        //qDebug() << "[Debug] RoseHomePlaylistListAll::request_more_Draw " << f_width << f_wg_cnt << f_mod << this->playlist_widget_cnt;

        int start_index = this->playlist_drawCount;
        int max_cnt = ((this->playlist_totalCount - this->playlist_drawCount) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_totalCount - this->playlist_drawCount);
        this->playlist_drawCount += max_cnt;

        //qDebug() << "[Debug] RoseHomePlaylistListAll::request_more_Draw " << start_index << max_cnt << this->playlist_drawCount;

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->current_pageInfo.type_id, true);
            //QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->widget_playlist[i]->setData(this->list_playlist->at(i));
            //QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHomePlaylistListAll::slot_clickedItemPlaylist);
            this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
        }

        ContentLoadingwaitingMsgHide();

        this->flag_playlist_draw = false;
        // widget draw change - by diskept j230317 finish
    }


    // MARK : User Event Handler (slots) ---------------------------------------------------------------------------------

    void RoseHomePlaylistListAll::slot_applyResult_Recommends(const QList<roseHome::RecommendsItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        if(this->current_pageInfo.type_id != HTTP_RECOMMENDS){
            return;
        }

        if(list_data.length() > 0){

            this->list_Recommends->append(list_data);

            QList<roseHome::PlaylistItemData> tmp_playlist;
            for(int i = 0; i < list_data.count(); i++){
                PlaylistItemData tmp_data;

                tmp_data.id = list_data.at(i).id;
                tmp_data.star = list_data.at(i).star;
                tmp_data.ownerId = list_data.at(i).ownerId;
                tmp_data.thumbupCount = list_data.at(i).thumbupCount;
                tmp_data.trackCount = list_data.at(i).trackCount;
                tmp_data.sort = list_data.at(i).sort;

                tmp_data.type = list_data.at(i).type;
                tmp_data.title = list_data.at(i).title;
                tmp_data.share = list_data.at(i).share;
                tmp_data.ownerName = list_data.at(i).ownerName;
                tmp_data.thumbnail = list_data.at(i).thumbnail;
                tmp_data.registDateTime = list_data.at(i).registDateTime;
                tmp_data.clientKey = list_data.at(i).clientKey;

                tmp_data.favoriteTrack = list_data.at(i).favoriteTrack;
                tmp_data.favorite = list_data.at(i).favorite;
                tmp_data.thumbup = list_data.at(i).thumbup;
                tmp_data.isRose = list_data.at(i).isRose;

                for(int j = 0; j < list_data.at(i).tags.count(); j++){
                    tmp_data.tags.append(list_data.at(i).tags.at(j));
                }

                tmp_playlist.append(tmp_data);
            }

            //this->slot_applyResult_playlists(tmp_playlist, jsonArr_dataToPlay, flag_lastPage);

            if(list_data.size() > 0){
                this->flag_lastPage_playlist = flag_lastPage;
                this->flagReqMore_playlist = false;

                if(this->list_playlist->count() == 0){
                    this->playlist_totalCount = list_data.at(0).totalCount;
                }

                int start_index = this->list_playlist->size();
                this->list_playlist->append(tmp_playlist);

                if(start_index == 0){
                    // widget draw change - by diskept j230317 start
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

                    //qDebug() << "[Debug] RoseHomePlaylistListAll::slot_applyResult_Recommends " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

                    int start_index = this->playlist_drawCount;
                    int max_cnt = ((this->playlist_totalCount - this->playlist_drawCount) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_totalCount - this->playlist_drawCount);
                    this->playlist_drawCount += max_cnt;

                    //qDebug() << "[Debug] RoseHomePlaylistListAll::slot_applyResult_Recommends " << start_index << max_cnt;

                    for(int i = start_index; i < this->playlist_drawCount; i++){
                        this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->current_pageInfo.type_id, true);
                        //QCoreApplication::processEvents();
                    }

                    for(int i = start_index; i < this->playlist_drawCount; i++){
                        this->widget_playlist[i]->setData(this->list_playlist->at(i));
                        //QCoreApplication::processEvents();
                    }

                    for(int i = start_index; i < this->playlist_drawCount; i++){
                        connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHomePlaylistListAll::slot_clickedItemPlaylist);
                        this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
                    }
                    // widget draw change - by diskept j230317 finish

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

                if(this->list_playlist->size() <= 0){
                    // noData widget change - by diskept j230317 start
                    int f_width = this->flowLayout_playlists->geometry().width();

                    if(this->flowLayout_playlists->geometry().width() <= 0){
                        f_width = this->width() - (80 + 63) - 10;
                    }

                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(f_width, 500);
                    // noData widget change - by diskept j230317 finish

                    this->flowLayout_playlists->addWidget(noData_widget);
                }
            }
        }
    }


    void RoseHomePlaylistListAll::slot_applyResult_playlists(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        int idx = sender()->property("idx").toInt();

        if(idx != this->playlist_next_offset){
            return;
        }

        if(this->current_pageInfo.type_id != HTTP_RECENTLY_PLAYLIST){
            return;
        }

        //qDebug() << "[Debug] RoseHomePlaylistListAll::slot_applyResult_playlists " << list_data.count();

        if(list_data.size() > 0){
            this->flag_lastPage_playlist = flag_lastPage;
            this->flagReqMore_playlist = false;

            if(this->list_playlist->count() == 0){
                this->playlist_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_playlist->size();
            this->list_playlist->append(list_data);

            if(start_index == 0){
                // widget draw change - by diskept j230317 start
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

                //qDebug() << "[Debug] RoseHomeplaylistListAll::slot_applyResult_playlists " << f_width << f_wg_cnt << f_mod << this->playlist_widget_cnt;

                int start_index = this->playlist_drawCount;
                int max_cnt = ((this->playlist_totalCount - this->playlist_drawCount) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_totalCount - this->playlist_drawCount);
                this->playlist_drawCount += max_cnt;

                //qDebug() << "[Debug] RoseHomeplaylistListAll::slot_applyResult_playlists " << start_index << max_cnt;

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->current_pageInfo.type_id, true);
                    //QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i]->setData(this->list_playlist->at(i));
                    //QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHomePlaylistListAll::slot_clickedItemPlaylist);
                    this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
                }
                // widget draw change - by diskept j230317 finish

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

            if(this->list_playlist->size() <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_playlists->geometry().width();

                if(this->flowLayout_playlists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_playlists->addWidget(noData_widget);
            }
        }
    }


    void RoseHomePlaylistListAll::slot_applyResult_MYplaylists(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        if(this->current_pageInfo.type_id != HTTP_MY_PLAYLIST){
            return;
        }

        //qDebug() << "[Debug] RoseHomePlaylistListAll::slot_applyResult_MYplaylists " << list_data.count();

        if(list_data.size() > 0){
            this->flag_lastPage_playlist = flag_lastPage;
            this->flagReqMore_playlist = false;

            if(this->list_playlist->count() == 0){
                this->playlist_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_playlist->size();
            this->list_playlist->append(list_data);

            if(start_index == 0){
                // widget draw change - by diskept j230317 start
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

                //qDebug() << "[Debug] RoseHomeplaylistListAll::slot_applyResult_MYplaylists " << f_width << f_wg_cnt << f_mod << this->playlist_widget_cnt;

                int start_index = this->playlist_drawCount;
                int max_cnt = ((this->playlist_totalCount - this->playlist_drawCount) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_totalCount - this->playlist_drawCount);
                this->playlist_drawCount += max_cnt;

                //qDebug() << "[Debug] RoseHomeplaylistListAll::slot_applyResult_MYplaylists " << start_index << max_cnt;

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->current_pageInfo.type_id, true);
                    //QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i]->setData(this->list_playlist->at(i));
                    //QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHomePlaylistListAll::slot_clickedItemPlaylist);
                    this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
                }
                // widget draw change - by diskept j230317 finish

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

            if(this->list_playlist->size() <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_playlists->geometry().width();

                if(this->flowLayout_playlists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_playlists->addWidget(noData_widget);
            }
        }
    }


    void RoseHomePlaylistListAll::slot_applyResult_NEWplaylists(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        if(this->current_pageInfo.type_id != HTTP_NEW_PLAYLIST){
            print_debug();
            return;
        }

        //qDebug() << "[Debug] RoseHomePlaylistListAll::slot_applyResult_NEWplaylists " << list_data.count();
print_debug();
        if(list_data.size() > 0){
            this->flag_lastPage_playlist = flag_lastPage;
            this->flagReqMore_playlist = false;

            if(this->list_playlist->count() == 0){
                this->playlist_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_playlist->size();
            this->list_playlist->append(list_data);

            if(start_index == 0){
                // widget draw change - by diskept j230317 start
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

                //qDebug() << "[Debug] RoseHomePlaylistListAll::slot_applyResult_NEWplaylists " << f_width << f_wg_cnt << f_mod << this->playlist_widget_cnt;

                int start_index = this->playlist_drawCount;
                int max_cnt = ((this->playlist_totalCount - this->playlist_drawCount) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_totalCount - this->playlist_drawCount);
                this->playlist_drawCount += max_cnt;

                //qDebug() << "[Debug] RoseHomePlaylistListAll::slot_applyResult_NEWplaylists " << start_index << max_cnt;

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->current_pageInfo.type_id, true);
                    //QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i]->setData(this->list_playlist->at(i));
                    //QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHomePlaylistListAll::slot_clickedItemPlaylist);
                    this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
                }
                // widget draw change - by diskept j230317 finish

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

            if(this->list_playlist->size() <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_playlists->geometry().width();

                if(this->flowLayout_playlists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_playlists->addWidget(noData_widget);
            }
        }
    }


    void RoseHomePlaylistListAll::slot_applyResult_POPplaylists(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        if(this->current_pageInfo.type_id != HTTP_POP_PLAYLIST){
            return;
        }
print_debug();
        //qDebug() << "[Debug] RoseHomePlaylistListAll::slot_applyResult_POPplaylists " << list_data.count();

        if(list_data.size() > 0){
            this->flag_lastPage_playlist = flag_lastPage;
            this->flagReqMore_playlist = false;

            if(this->list_playlist->count() == 0){
                this->playlist_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_playlist->size();
            this->list_playlist->append(list_data);

            if(start_index == 0){
                // widget draw change - by diskept j230317 start
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

                //qDebug() << "[Debug] RoseHomePlaylistListAll::slot_applyResult_POPplaylists " << f_width << f_wg_cnt << f_mod << this->playlist_widget_cnt;

                int start_index = this->playlist_drawCount;
                int max_cnt = ((this->playlist_totalCount - this->playlist_drawCount) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_totalCount - this->playlist_drawCount);
                this->playlist_drawCount += max_cnt;

                //qDebug() << "[Debug] RoseHomePlaylistListAll::slot_applyResult_POPplaylists " << start_index << max_cnt;

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->current_pageInfo.type_id, true);
                    //QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i]->setData(this->list_playlist->at(i));
                    //QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHomePlaylistListAll::slot_clickedItemPlaylist);
                    this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
                }
                // widget draw change - by diskept j230317 finish

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

            if(this->list_playlist->size() <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_playlists->geometry().width();

                if(this->flowLayout_playlists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_playlists->addWidget(noData_widget);
            }
        }
    }


    void RoseHomePlaylistListAll::slot_applyResult_DUDUOplaylists(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        if(this->current_pageInfo.type_id != HTTP_DUDUO_PLAYLIST){
            return;
        }

        //qDebug() << "[Debug] RoseHomePlaylistListAll::slot_applyResult_DUDUOplaylists " << list_data.count();

        if(list_data.size() > 0){
            this->flag_lastPage_playlist = flag_lastPage;
            this->flagReqMore_playlist = false;

            if(this->list_playlist->count() == 0){
                this->playlist_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_playlist->size();
            this->list_playlist->append(list_data);

            if(start_index == 0){
                // widget draw change - by diskept j230317 start
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

                //qDebug() << "[Debug] RoseHomePlaylistListAll::slot_applyResult_DUDUOplaylists " << f_width << f_wg_cnt << f_mod << this->playlist_widget_cnt;

                int start_index = this->playlist_drawCount;
                int max_cnt = ((this->playlist_totalCount - this->playlist_drawCount) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_totalCount - this->playlist_drawCount);
                this->playlist_drawCount += max_cnt;

                //qDebug() << "[Debug] RoseHomePlaylistListAll::slot_applyResult_DUDUOplaylists " << start_index << max_cnt;

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->current_pageInfo.type_id, true);
                    //QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i]->setData(this->list_playlist->at(i));
                    //QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHomePlaylistListAll::slot_clickedItemPlaylist);
                    this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
                }
                // widget draw change - by diskept j230317 finish

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

            if(this->list_playlist->size() <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_playlists->geometry().width();

                if(this->flowLayout_playlists->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_playlists->addWidget(noData_widget);
            }
        }
    }


    void RoseHomePlaylistListAll::slot_applyResult_playlistsCheck(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_playlist->count() == 0){
                QString tmpTitle = tr("Recently Played Playlists") + QString(" (%1)").arg(list_data.at(0).totalCount);
                this->label_mainTitle->setText(tmpTitle);
                this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                this->initAll();

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

            this->flagNeedReload = true;
            this->setActivePage();
        }
        else{
            ContentLoadingwaitingMsgHide();
        }
    }


    void RoseHomePlaylistListAll::slot_btnClicked_Delete(){

        QStringList tmpTitle = this->current_pageInfo.pathTitle.split("(");

        QJsonObject tmpObj;
        tmpObj.insert("contents_type", tmpTitle.at(0));
        tmpObj.insert("list_type", "PLAYLIST");
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
     * @brief [slot] override - Itemplaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void RoseHomePlaylistListAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___PLAYLIST){
            this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);
        }
    }


    void RoseHomePlaylistListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___PLAYLIST){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, SECTION_FOR_MORE_POPUP___PLAYLIST, clickMode);
        }
    }


    /**
     * @brief RoseHomePlaylistListAll::resizeEvent
     * @param event
     */
    void RoseHomePlaylistListAll::resizeEvent(QResizeEvent *event){//c230223

        Q_UNUSED(event);

        // flowlayout spacing change - by diskept j230317 start
        this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);
        // flowlayout spacing change - by diskept j230317 finish

        this->widget_mainTitle->resize(this->width(), 69);
        this->btn_mainTitle->setGeometry(this->width() - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);
    }





    /**
     * @brief RoseHomePlaylistListAll::RoseHomePlaylistListAll
     * @param parent
     */
    RoseHomePlaylistHistoryAll::RoseHomePlaylistHistoryAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // 기본 UI 세팅
        this->setUIControl_playlist();

        this->list_playlist = new QList<roseHome::PlaylistItemData>();
    }

    /**
     * @brief 소멸자.
     */
    RoseHomePlaylistHistoryAll::~RoseHomePlaylistHistoryAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void RoseHomePlaylistHistoryAll::setJsonObject_forData(const QJsonObject &jsonObj){

        PageInfo_PlaylistAllView tmp_pageInfo = ConvertData::convertData_pageInfo_playlistAllView(jsonObj);
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
    void RoseHomePlaylistHistoryAll::setActivePage(){

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
    void RoseHomePlaylistHistoryAll::setUIControl_playlist(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("");

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        roseHome::ItemPlaylist_rosehome *listAlbum = new roseHome::ItemPlaylist_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, HTTP_MY_PLAYLIST, true);

        this->playlist_widget_width = listAlbum->get_fixedWidth();
        this->playlist_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeAlbumListAll::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;

        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
    }

    /**
     * @brief 스크롤 Bottom에 대하여 Get more data 처리
     */
    void RoseHomePlaylistHistoryAll::proc_wheelEvent_to_getMoreData(){

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
    void RoseHomePlaylistHistoryAll::resizeEvent(QResizeEvent *event){//c230223

        AbstractRoseHomeSubWidget::resizeEvent(event);

        this->setFlowLayoutResize(this, this->flowLayout_playlists, this->playlist_widget_width, this->playlist_widget_margin);

    }

    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    void RoseHomePlaylistHistoryAll::request_more_Data(){

        // next_offset
        if(this->list_playlist->size() == 0){
            this->playlist_next_offset = 0;
        }
        else{
            this->playlist_next_offset++;
        }

        roseHome::ProcCommon *proc_playlist = new roseHome::ProcCommon(this);
        connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_playlists, this, &RoseHomePlaylistHistoryAll::slot_applyResult_historyPlaylist);
        proc_playlist->setProperty("idx", this->playlist_next_offset);
        proc_playlist->request_rose_getList_historyPlaylists(this->data_pageInfo.api_subPath, this->data_pageInfo.filter_type, this->playlist_next_offset, 200);

        if(this->playlist_next_offset == 0){
            this->flag_playlist_draw = true;
        }
    }


    void RoseHomePlaylistHistoryAll::request_more_Draw(){

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
            this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, HTTP_MY_PLAYLIST, true);
            //QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->widget_playlist[i]->setData(this->list_playlist->at(i));
            //QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->playlist_drawCount; i++){
            this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
            connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHomePlaylistHistoryAll::slot_clickedItemPlaylist);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_playlist_draw = false;
    }

    // MARK : User Event Handler (slots) ---------------------------------------------------------------------------------

    void RoseHomePlaylistHistoryAll::slot_applyResult_historyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, HTTP_MY_PLAYLIST, true);
                    //QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->widget_playlist[i]->setData(this->list_playlist->at(i));
                    //QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->playlist_drawCount; i++){
                    this->flowLayout_playlists->addWidget(this->widget_playlist[i]);
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHomePlaylistHistoryAll::slot_clickedItemPlaylist);
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
     * @brief [slot] override - Itemplaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void RoseHomePlaylistHistoryAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___PLAYLIST){
            this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);
        }
    }


    void RoseHomePlaylistHistoryAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___PLAYLIST){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, SECTION_FOR_MORE_POPUP___PLAYLIST, clickMode);
        }
    }
}
