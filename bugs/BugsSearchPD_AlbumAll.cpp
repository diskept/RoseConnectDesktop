#include "bugs/BugsSearchPD_AlbumAll.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/ProcBugsAPI.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"

#include <QScrollBar>


namespace bugs {

    const int SECTION_FOR_MORE_POPUP___pd_album = 1;


    /**
     * @brief BugsSearchPD_AlbumAll::BugsSearchPD_AlbumAll
     * @param parent
     */
    BugsSearchPD_AlbumAll::BugsSearchPD_AlbumAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // Data
        this->list_pd_album = new QList<bugs::PD_AlbumItemData>();

        if(global.user_forBugs.isLogined() == true){

            // 기본 UI 세팅
            this->setUIControl_PDalbums();
        }
    }


    BugsSearchPD_AlbumAll::~BugsSearchPD_AlbumAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsSearchPD_AlbumAll::setJsonObject_forData(const QJsonObject &jsonObj){

//        bugs::BugsSearchPD_AlbumAll prev_data_page = this->data_pageInfo;
        common::SearchData tmp_searchData = common::ConvertData_common::convertData_searchData(jsonObj);
        this->flagNeedReload = false;

        if(this->data_search.search_word != tmp_searchData.search_word){
            this->flagNeedReload = true;

            this->data_search = tmp_searchData;
            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + "검색 결과 - 뮤직PD앨범");

            // init
            this->next_offset = 0;
            this->album_total_cnt = 0;
            this->album_draw_cnt = 0;

            this->list_pd_album->clear();

            // request HTTP API
            this->flagReqMore_pd_album = false;
            this->flag_lastPage_pd_album = false;

            this->flag_album_draw = false;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_pd_albumData();
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void BugsSearchPD_AlbumAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            GSCommon::clearLayout(this->flowLayout_pd_albums);

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
        }
    }

    /**
     * @brief BugsSearchPD_AlbumAll::applyLoginSuccessState
     */
    /*void BugsSearchPD_AlbumAll::applyLoginSuccessState(){

        if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                this->setUIControl_logined();
                this->flagNeedReload = true;        // 강제 세팅
            }

            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + "검색 결과 - 뮤직PD앨범");

            this->list_pd_album->clear();
            GSCommon::clearLayout(this->flowLayout_pd_albums);

            // request HTTP API
            this->flag_lastPage_pd_album = false;
            this->flagReqMore_pd_album = false;

            this->request_more_pd_albumData();//cheon210704-list

        }
        else{
            if(this->m_flagCurrLogined){
                // 로그인 ---> 로그아웃
                this->m_flagCurrLogined = false;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->setUIControl_notLogin();
            }
        }
    }*/



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void BugsSearchPD_AlbumAll::setUIControl_PDalbums(){

        this->label_mainTitle = this->get_addUIControl_mainTitle(" ");

        GSCommon::clearLayout(this->box_contents);      // Layout 초기화
        this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // layout for items
        this->flowLayout_pd_albums = this->get_addUIControl_flowLayout(0, 20);
        GSCommon::clearLayout(this->flowLayout_pd_albums);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief BugsSearchPD_AlbumAll::proc_wheelEvent_to_getMoreData
     */
    void BugsSearchPD_AlbumAll::proc_wheelEvent_to_getMoreData(){

        if((this->album_total_cnt > this->album_draw_cnt) && (this->list_pd_album->size() > this->album_draw_cnt) && (this->flag_album_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_album_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_pd_albumDraw();
        }
    }


    /**
     * @brief BugsSearchPD_AlbumAll::request_more_albumData
     */
    void BugsSearchPD_AlbumAll::request_more_pd_albumData(){

        if(!this->flagReqMore_pd_album && !this->flag_lastPage_pd_album){
            this->flagReqMore_pd_album = true;

            // j220913 list count check
            int width_cnt = global.LmtCnt / 220;
            int mod = this->album_draw_cnt % width_cnt;

            if(mod == 0){
                this->album_widget_cnt = width_cnt * 10;
            }
            else{
                this->album_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
            // j220913 list count check

            // next_offset
            if(this->list_pd_album->size() == 0){
                this->next_offset = 1;
            }
            else{
                this->next_offset++;
            }

            // getting PD's Album list - request HTTP API
            ProcBugsAPI *proc = new ProcBugsAPI(this);
            connect(proc, &ProcBugsAPI::completeReq_list_pd_albums, this, &BugsSearchPD_AlbumAll::slot_applyResult_pd_albums);
            proc->request_bugs_search_pd_album(this->data_search.search_word, next_offset, this->album_widget_cnt, ProcBugsAPI::BugsSearch_Sort::exact);

            if(this->next_offset == 0){
                this->flag_album_draw = true;
            }
        }
    }


    void BugsSearchPD_AlbumAll::request_more_pd_albumDraw(){

        // j220913 list count check
        int width_cnt = global.LmtCnt / 220;
        int mod = this->album_draw_cnt % width_cnt;

        if(mod == 0){
            this->album_widget_cnt = width_cnt * 10;
        }
        else{
            this->album_widget_cnt = width_cnt * 10 + (width_cnt - mod);
        }
        // j220913 list count check

        int start_index = this->album_draw_cnt;
        int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
        this->album_draw_cnt += max_cnt;

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->list_pd_album_all[i] = new bugs::ItemPD_Album_bugs(i, SECTION_FOR_MORE_POPUP___pd_album, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->list_pd_album_all[i], &bugs::ItemPD_Album_bugs::signal_clicked, this, &BugsSearchPD_AlbumAll::slot_clickedItemPD_Album);
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->list_pd_album_all[i]->setData(this->list_pd_album->at(i));
            this->flowLayout_pd_albums->addWidget(this->list_pd_album_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_album_draw = false;
    }


    /**
     * @brief BugsSearchPD_AlbumAll::slot_applyResult_pd_albums
     * @param list_data
     * @param flag_lastPage
     */
    void BugsSearchPD_AlbumAll::slot_applyResult_pd_albums(const QList<bugs::PD_AlbumItemData>& list_data, const bool flag_lastPage){

        /*this->flagReqMore_pd_album = false;            // init (ready to get more data)
        this->flag_lastPage_pd_album = flag_lastPage;

        int start_index = this->list_pd_album->length();
        this->list_pd_album->append(list_data);
        this->createList_itemPD_Album_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_186x186, this->flowLayout_pd_albums, start_index, SECTION_FOR_MORE_POPUP___pd_album);
        if(this->list_pd_album->size() % GET_ITEM_SIZE___ONCE == 0 && this->list_pd_album->size() % global.request_total_count !=0)//cheon210704-list
            request_more_pd_albumData();//cheon210704-list*/

        if(list_data.size() > 0){
            this->flag_lastPage_pd_album = flag_lastPage;
            this->flagReqMore_pd_album = false;

            if(this->list_pd_album->size() == 0){
                this->album_total_cnt = list_data.at(0).total_count;
            }

            int start_index = this->list_pd_album->size();
            this->list_pd_album->append(list_data);

            if(start_index == 0){
                int max_cnt = this->list_pd_album->size();
                this->album_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->list_pd_album_all[i] = new bugs::ItemPD_Album_bugs(i, SECTION_FOR_MORE_POPUP___pd_album, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->list_pd_album_all[i], &bugs::ItemPD_Album_bugs::signal_clicked, this, &BugsSearchPD_AlbumAll::slot_clickedItemPD_Album);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->list_pd_album_all[i]->setData(this->list_pd_album->at(i));
                    this->flowLayout_pd_albums->addWidget(this->list_pd_album_all[i]);

                    QCoreApplication::processEvents();
                }

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }

            ContentLoadingwaitingMsgHide();
            this->request_more_pd_albumData();
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            if(this->list_pd_album->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::PD_Album_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->flowLayout_pd_albums->addWidget(noData_widget);
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
    void BugsSearchPD_AlbumAll::slot_clickedItemPD_Album(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemPD_Album(this->list_pd_album, clickMode, index, section);
    }




    /**
     * @brief BugsSearchPD_AlbumAll::proc_preUpdate_favorite_pd_album
     * @param data_itemPos
     */
    void BugsSearchPD_AlbumAll::proc_preUpdate_favorite_pd_album(const ItemPositionData& data_itemPos){

        // Update Data
        bugs::PD_AlbumItemData data_pd_album = this->list_pd_album->at(data_itemPos.index);
        data_pd_album.likes_yn = data_itemPos.likes_yn;
        this->list_pd_album->replace(data_itemPos.index, data_pd_album);
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
    void BugsSearchPD_AlbumAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___pd_album){
            this->proc_clicked_optMorePopup_fromPD_Album(this->list_pd_album, index, clickMode);
        }
    }
}
