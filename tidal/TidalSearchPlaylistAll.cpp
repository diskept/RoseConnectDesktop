#include "tidal/TidalSearchPlaylistAll.h"
#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QScrollBar>


namespace tidal {

    const int SECTION_FOR_MORE_POPUP___playlists = 1;


    /**
     * @brief TidalSearchPlaylistAll::TidalSearchPlaylistAll
     * @param parent
     */
    TidalSearchPlaylistAll::TidalSearchPlaylistAll(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // Data
        this->list_playlist = new QList<tidal::PlaylistItemData>();

        // 기본 UI 세팅
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ");
        this->flowLayout_playlists = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    TidalSearchPlaylistAll::~TidalSearchPlaylistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalSearchPlaylistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        common::SearchData tmp_data = common::ConvertData_common::convertData_searchData(jsonObj);
        this->flagNeedReload = false;

        if(tmp_data.search_word != this->data_search.search_word){
            this->flagNeedReload = true;
            this->data_search = common::ConvertData_common::convertData_searchData(jsonObj);
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalSearchPlaylistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            // init data
            this->list_playlist->clear();

            // init UI
            GSCommon::clearLayout(this->flowLayout_playlists);

            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + tr("Search Result") + " - Playlists");
            //this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + tr("검색결과") + " - Playlists");

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
    }


    // MARK : TidalPlaylistAll HTTP API ---------------------------------------------------------------------------------

    /**
     * @brief 스크롤 Bottom에 대하여 Get more data 처리
     */
    void TidalSearchPlaylistAll::proc_wheelEvent_to_getMoreData(){

        if((this->playlist_total_cnt > this->playlist_draw_cnt) && (this->list_playlist->size() > this->playlist_draw_cnt) && (this->flag_playlist_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_playlist_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_playlistDraw();
        }
    }


    /**
     * @brief Tidal API 요청
     */
    void TidalSearchPlaylistAll::request_more_playlistData(){

        if(!this->flagReqMore_playlist && !this->flag_lastPage_playlist){
            this->flagReqMore_playlist = true;

            // j220913 list count check
            int width_cnt = global.LmtCnt / 220;
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
            this->next_offset = this->list_playlist->size();//cheon210704-list

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_playlists, this, &TidalSearchPlaylistAll::slot_applyResult_playlists);
            proc->request_tidal_searchAllItems(this->data_search.search_word, this->playlist_widget_cnt, this->next_offset, ProcCommon::TidalSearch_ContentType::Playlists);

        }
    }


    void TidalSearchPlaylistAll::request_more_playlistDraw(){

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

        int start_index = this->playlist_draw_cnt;
        int max_cnt = ((this->playlist_total_cnt - this->playlist_draw_cnt) > this->playlist_widget_cnt ) ? this->playlist_widget_cnt : (this->playlist_total_cnt - this->playlist_draw_cnt);
        this->playlist_draw_cnt += max_cnt;

        for(int i = start_index; i < this->playlist_draw_cnt; i++){
            this->listPlaylist_all[i] = new tidal::ItemPlaylist(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->listPlaylist_all[i], &tidal::ItemPlaylist::signal_clicked, this, &TidalSearchPlaylistAll::slot_clickedItemPlaylist);
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
     * @brief [slot] Tidal API playlist 반환값에 대하여 UI 적용
     * @param list_data
     */
    void TidalSearchPlaylistAll::slot_applyResult_playlists(const QList<tidal::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        /*this->flagReqMore_playlist = false;
        this->flag_lastPage_playlist = flag_lastPage;

        int start_index = this->list_playlist->count();
        this->list_playlist->append(list_data);
        //this->createList_itemPlaylsit_applyingWithData(list_data, AbstractItem::ImageSizeMode::Square_186x186, this->flowLayout_playlists, start_index, SECTION_FOR_MORE_POPUP___playlists);
        this->createList_itemPlaylsit_applyingWithData(list_data, AbstractItem::ImageSizeMode::Square_200x200, this->flowLayout_playlists, start_index, SECTION_FOR_MORE_POPUP___playlists);*/

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
                    this->listPlaylist_all[i] = new tidal::ItemPlaylist(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->listPlaylist_all[i], &tidal::ItemPlaylist::signal_clicked, this, &TidalSearchPlaylistAll::slot_clickedItemPlaylist);
                }

                for(int i = start_index; i < this->playlist_draw_cnt; i++){
                    this->listPlaylist_all[i]->setData(this->list_playlist->at(i));
                    this->flowLayout_playlists->addWidget(this->listPlaylist_all[i]);

                    QCoreApplication::processEvents();
                }

                //c230306_1-start
                int w = flowLayout_playlists->sizeHint().width();
                int l = 80, r = 60, scrollbarW = 10, mod = 0;

                int mod_nn = (global.LmtCnt-l-r-scrollbarW)%(w + mod);
                int nn = (global.LmtCnt-l-r-scrollbarW)/(w + mod);
                //qDebug() << "global.LmtCnt=" << global.LmtCnt;
                //qDebug() << "this->width()=" << this->width();
                //qDebug() << "nn=" << nn;
                //qDebug() << "mod_nn=" << mod_nn;

                int i = 0;
                while(1){

                    mod_nn = (global.LmtCnt-l-r-scrollbarW)%(w + mod + i);
                    if(mod_nn > 20){

                        mod_nn = (global.LmtCnt-l-r-scrollbarW)%(w + ( mod +(i++)));
                    }else{
                        break;
                    }
                }
                //print_debug();
                //qDebug() << "w=" << w;
                //qDebug() << "i=" << i;
                //qDebug() << "this->width()=" << this->width();
                //this->resize(this->width()+1, this->height());
                flowLayout_playlists->setSpacingHV(mod+i,20);
                //c230306_1-end
                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_playlist_draw = false;
            }

            if(this->flag_lastPage_playlist == false){
                this->request_more_playlistData();
            }
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


    void TidalSearchPlaylistAll::resizeEvent(QResizeEvent *event){//c230223

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
    }

    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalSearchPlaylistAll::slot_clickedItemPlaylist(const ItemPlaylist::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

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
    void TidalSearchPlaylistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, section, clickMode);
    }
}
