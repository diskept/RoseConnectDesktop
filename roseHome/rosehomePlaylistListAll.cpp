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

    const int SECTION_FOR_MORE_POPUP___PLAYLIST = 0;


    /**
     * @brief RoseHomePlaylistListAll::RoseHomePlaylistListAll
     * @param parent
     */
    RoseHomePlaylistListAll::RoseHomePlaylistListAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // 기본 UI 세팅
        this->setUIControl_playlist();

        this->list_playlist = new QList<roseHome::PlaylistItemData>();
        this->list_Recommends = new QList<roseHome::RecommendsItemData>();
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

        if(this->curr_api_subPath != data_pageInfo.api_subPath || this->curr_api_mainTitle != data_pageInfo.pathTitle){
            this->flagNeedReload = true;

            this->api_subPath = data_pageInfo.api_subPath;
            this->api_subType = data_pageInfo.filter_type;
            this->type = data_pageInfo.type;
            this->genre_id = data_pageInfo.genre_id;
            this->genre_ids = data_pageInfo.genre_ids;
            this->type_id = data_pageInfo.type_id;

            this->curr_api_mainTitle = data_pageInfo.pathTitle;
            this->label_mainTitle->setText(this->curr_api_mainTitle);
            this->curr_api_subPath = this->api_subPath;
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
            this->list_playlist->clear();
            this->list_Recommends->clear();
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
    void RoseHomePlaylistListAll::setUIControl_playlist(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("noname title");
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void RoseHomePlaylistListAll::proc_wheelEvent_to_getMoreData(){

        if((this->playlist_totalCount > this->playlist_drawCount) && (this->list_playlist->size() > this->playlist_drawCount) && (this->flag_playlist_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_playlist_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_Draw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    void RoseHomePlaylistListAll::request_more_Data(){

        if(!this->flagReqMore_playlist && !this->flag_lastPage_playlist){
            this->flagReqMore_playlist = true;

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

            // next_offset
            if(this->list_playlist->size() == 0){
                this->playlist_next_offset = 0;
            }
            else{
                this->playlist_next_offset++;
            }

            if(this->type_id == HTTP_RECOMMENDS){

                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeReq_list_recommeds, this, &RoseHomePlaylistListAll::slot_applyResult_Recommends);
                proc->request_rose_getList_recommends(this->api_subPath, this->playlist_next_offset, this->playlist_widget_cnt);
            }
            else{

                roseHome::ProcCommon *proc_playlist = new roseHome::ProcCommon(this);
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
                        break;
                }
            }
        }
    }


    void RoseHomePlaylistListAll::request_more_Draw(){

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
    }


    // MARK : User Event Handler (slots) ---------------------------------------------------------------------------------

    void RoseHomePlaylistListAll::slot_applyResult_Recommends(const QList<roseHome::RecommendsItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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

            this->slot_applyResult_playlists(tmp_playlist, jsonArr_dataToPlay, flag_lastPage);
        }
    }


    void RoseHomePlaylistListAll::slot_applyResult_playlists(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    this->widget_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->type_id, true);
                    connect(this->widget_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHomePlaylistListAll::slot_clickedItemPlaylist);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->widget_playlist[i]->setData(this->list_playlist->at(i));
                    this->flowLayout_playlists->addWidget(this->widget_playlist[i]);

                    QCoreApplication::processEvents();
                }

                this->flag_flow_draw = true;
                this->flag_playlist_draw = false;
            }

            if(flag_lastPage == false){
                this->request_more_Data();
            }
        }
        else{
            if(this->list_playlist->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedSize(1500, 500);

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
}
