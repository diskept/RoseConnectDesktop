#include "qobuz/QobuzPlaylistAll.h"

#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ProcCommon_forQobuz.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QScrollBar>


namespace qobuz {

    const int SECTION_FOR_MORE_POPUP___playlists = 1;


    /**
     * @brief QobuzMoodsMain::QobuzMoodsMain
     * @param parent
     */
    QobuzPlaylistAll::QobuzPlaylistAll(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // 기본 UI 세팅
        this->label_mainTitle = this->get_addUIControl_mainTitle("noname title");
        this->setUIControl_playlist();

        this->list_playlist = new QList<qobuz::PlaylistItemData>();
    }


    /**
     * @brief 소멸자.
     */
    QobuzPlaylistAll::~QobuzPlaylistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzPlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        qobuz::PageInfo_PlaylistAllView data_pageInfo = ConvertData::convertData_pageInfo_playlistAllView(jsonObj);        
        this->flagNeedReload = false;

        if((this->api_subPath != data_pageInfo.api_subPath) || (this->api_mainTitle != data_pageInfo.pathTitle)){
            this->flagNeedReload = true;

            this->api_mainTitle = data_pageInfo.pathTitle;
            this->api_subPath = data_pageInfo.api_subPath;
            this->type = data_pageInfo.type;
            this->genre_id = data_pageInfo.genre_id;
            this->genre_ids = data_pageInfo.genre_ids;

            this->label_mainTitle->setText(this->api_mainTitle);

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
    void QobuzPlaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

            // init UI
            GSCommon::clearLayout(this->flowLayout_playlists);

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
        }
    }


    // MARK : Create UI and Set ---------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void QobuzPlaylistAll::setUIControl_playlist(){

        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    // MARK : QobuzPlaylistAll HTTP API ---------------------------------------------------------------------------------

    /**
     * @brief 스크롤 Bottom에 대하여 Get more data 처리
     */
    void QobuzPlaylistAll::proc_wheelEvent_to_getMoreData(){

        if((this->playlist_total_cnt > this->playlist_draw_cnt) && (this->list_playlist->size() > this->playlist_draw_cnt) && (this->flag_playlist_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_playlist_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistDraw();
        }
    }


    /**
     * @brief Qobuz API 요청
     */
    void QobuzPlaylistAll::request_more_playlistData(){

        if(!this->flagReqMore_playlist && !this->flag_lastPage_playlist){
            this->flagReqMore_playlist = true;

            // j220913 list count check
            int width_cnt = global.LmtCnt / 220;
            int mod = this->playlist_draw_cnt % width_cnt;

            if(mod == 0){
                this->playlist_widget_cnt = width_cnt * 10;
            }
            else{
                this->playlist_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
            // j220913 list count check

            // next_offset
            this->next_offset = this->list_playlist->size();

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_playlists, this, &QobuzPlaylistAll::slot_applyResult_playlists);
            proc->request_qobuz_getList_playlists(this->api_subPath, this->type, this->genre_ids, this->playlist_widget_cnt, next_offset);

            if(this->next_offset == 0){
                this->flag_playlist_draw = true;
            }
        }
    }


    void QobuzPlaylistAll::request_more_playlistDraw(){

        // j220913 list count check
        int width_cnt = global.LmtCnt / 220;
        int mod = this->playlist_draw_cnt % width_cnt;

        if(mod == 0){
            this->playlist_widget_cnt = width_cnt * 10;
        }
        else{
            this->playlist_widget_cnt = width_cnt * 10 + (width_cnt - mod);
        }
        // j220913 list count check

        int start_index = this->playlist_draw_cnt;
        int max_cnt = ((this->playlist_total_cnt - this->playlist_draw_cnt) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_total_cnt - this->playlist_draw_cnt);
        this->playlist_draw_cnt += max_cnt;

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i] = new qobuz::ItemPlaylist_qobuz(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Ractangle_360x176);
            connect(this->listPlaylist_all[i], &qobuz::ItemPlaylist_qobuz::signal_clicked, this, &QobuzPlaylistAll::slot_clickedItemPlaylist);
        }

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
            this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_playlist_draw = false;
    }


    // MARK : User Event Handler (slots) ---------------------------------------------------------------------------------

    /**
     * @brief [slot] Qobuz API playlist 반환값에 대하여 UI 적용
     * @param list_data
     */
    void QobuzPlaylistAll::slot_applyResult_playlists(const QList<qobuz::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.size() > 0){
            this->flagReqMore_playlist = false;
            this->flag_lastPage_playlist = flag_lastPage;

            if(this->list_playlist->size() == 0){
                this->playlist_total_cnt = list_data.at(0).album_total_cnt;
            }

            int start_index = this->list_playlist->size();
            this->list_playlist->append(list_data);

            if(start_index == 0){
                int max_cnt = this->list_playlist->size();
                this->playlist_draw_cnt = max_cnt;

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->listPlaylist_all[i] = new qobuz::ItemPlaylist_qobuz(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Ractangle_360x176);
                    connect(this->listPlaylist_all[i], &qobuz::ItemPlaylist_qobuz::signal_clicked, this, &QobuzPlaylistAll::slot_clickedItemPlaylist);
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

            if(this->list_playlist->size() <= 0){
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
    void QobuzPlaylistAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);
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
    void QobuzPlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, section, clickMode);
        }
    }





    /**
     * @brief QobuzMoodsMain::QobuzMoodsMain
     * @param parent
     */
    QobuzSimilarplaylistAll::QobuzSimilarplaylistAll(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll, parent) {

        // 기본 UI 세팅
        this->label_mainTitle = this->get_addUIControl_mainTitle("noname Title");
        this->setUIControl_playlist();

        this->list_playlist = new QList<qobuz::PlaylistItemData>();
    }


    /**
     * @brief 소멸자.
     */
    QobuzSimilarplaylistAll::~QobuzSimilarplaylistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzSimilarplaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        qobuz::PageInfo_PlaylistAllView data_pageInfo = ConvertData::convertData_pageInfo_playlistAllView(jsonObj);
        this->flagNeedReload = false;

        if((this->api_subPath != data_pageInfo.api_subPath) || (this->api_mainTitle != data_pageInfo.pathTitle)){
            this->flagNeedReload = true;

            this->api_mainTitle = data_pageInfo.pathTitle;
            this->api_subPath = data_pageInfo.api_subPath;
            this->type = data_pageInfo.type;
            this->genre_id = data_pageInfo.genre_id;
            this->genre_ids = data_pageInfo.genre_ids;

            // init data
            this->list_playlist->clear();
            this->list_playlist->append(data_pageInfo.list_similarPlaylist);

            this->label_mainTitle->setText(this->api_mainTitle);
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void QobuzSimilarplaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

            GSCommon::clearLayout(this->flowLayout_playlists);

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            QJsonArray json_arr;
            this->slot_applyResult_playlists(*this->list_playlist, json_arr, false);
        }
    }


    // MARK : Create UI and Set ---------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void QobuzSimilarplaylistAll::setUIControl_playlist(){

        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    // MARK : User Event Handler (slots) ---------------------------------------------------------------------------------

    /**
     * @brief [slot] Qobuz API playlist 반환값에 대하여 UI 적용
     * @param list_data
     */
    void QobuzSimilarplaylistAll::slot_applyResult_playlists(const QList<qobuz::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        this->list_playlist->append(list_data);
        int max_cnt = this->list_playlist->size();

        for(int i = 0; i < max_cnt; i++){
            this->listPlaylist_all[i] = new qobuz::ItemPlaylist_qobuz(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Ractangle_360x176);
            connect(this->listPlaylist_all[i], &qobuz::ItemPlaylist_qobuz::signal_clicked, this, &QobuzSimilarplaylistAll::slot_clickedItemPlaylist);
        }

        for(int i = 0; i < max_cnt; i++){
            this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
            this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
    }


    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzSimilarplaylistAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);
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
    void QobuzSimilarplaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, section, clickMode);
        }
    }
}
