#include "bugs/BugsSearchArtistAll.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/ProcBugsAPI.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"

#include <QScrollBar>


namespace bugs {

    const int SECTION_FOR_MORE_POPUP___artist = 1;


    /**
     * @brief BugsSearchPD_artistAll::BugsSearchPD_artistAll
     * @param parent
     */
    BugsSearchArtistAll::BugsSearchArtistAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // Data
        this->list_artist = new QList<bugs::ArtistItemData>();

        if(global.user_forBugs.isLogined() == true){

            // 기본 UI 세팅
            this->setUIControl_artists();
        }
    }


    BugsSearchArtistAll::~BugsSearchArtistAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsSearchArtistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        common::SearchData tmp_searchData = common::ConvertData_common::convertData_searchData(jsonObj);
        this->flagNeedReload = false;

        if(this->data_search.search_word != tmp_searchData.search_word){
            this->flagNeedReload = true;

            this->data_search = tmp_searchData;
            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + "검색 결과 - 아티스트");

            // init
            this->next_offset = 0;
            this->artist_total_cnt = 0;
            this->artist_draw_cnt = 0;

            this->list_artist->clear();

            // request HTTP API
            this->flagReqMore_artist = false;
            this->flag_lastPage_artist = false;

            this->flag_artist_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_artistData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief BugsSearchArtistAll::applyLoginSuccessState
     */
    /*void BugsSearchArtistAll::applyLoginSuccessState(){

        if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                this->setUIControl_logined();
                this->flagNeedReload = true;        // 강제 세팅
            }

            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + tr("검색 결과 - 아티스트"));

            this->list_artist->clear();
            GSCommon::clearLayout(this->flowLayout_artist);

            // request HTTP API
            this->flag_lastPage_artist = false;
            this->flagReqMore_artist = false;
            this->request_more_artistData();
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


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void BugsSearchArtistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            GSCommon::clearLayout(this->flowLayout_artist);

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
    void BugsSearchArtistAll::setUIControl_artists(){

        this->label_mainTitle = this->get_addUIControl_mainTitle(" ");

        GSCommon::clearLayout(this->box_contents);      // Layout 초기화
        this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // layout for items
        this->flowLayout_artist = this->get_addUIControl_flowLayout(0, 20);
        GSCommon::clearLayout(this->flowLayout_artist);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief BugsSearchArtistAll::proc_wheelEvent_to_getMoreData
     */
    void BugsSearchArtistAll::proc_wheelEvent_to_getMoreData(){

        if((this->artist_total_cnt > this->artist_draw_cnt) && (this->list_artist->size() > this->artist_draw_cnt) && (this->flag_artist_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_artist_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_artistDraw();
        }
    }


    /**
     * @brief BugsSearchArtistAll::request_more_artistData
     */
    void BugsSearchArtistAll::request_more_artistData(){

        if(!this->flagReqMore_artist && !this->flag_lastPage_artist){
            this->flagReqMore_artist = true;

            // j220913 list count check
            int width_cnt = global.LmtCnt / 220;
            int mod = this->artist_draw_cnt % width_cnt;

            if(mod == 0){
                this->artist_widget_cnt = width_cnt * 10;
            }
            else{
                this->artist_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
            // j220913 list count check

            // next_offset
            if(this->list_artist->size() == 0){
                this->next_offset = 1;
            }
            else{
                this->next_offset++;
            }

            // request HTTP API
            ProcBugsAPI *proc = new ProcBugsAPI(this);
            connect(proc, &ProcBugsAPI::completeReq_list_artists, this, &BugsSearchArtistAll::slot_applyResult_artists);
            proc->request_bugs_search_artist(this->data_search.search_word, this->next_offset, this->artist_widget_cnt, ProcBugsAPI::BugsSearch_Sort::exact);

            if(this->next_offset == 0){
                this->flag_artist_draw = true;
            }
        }
    }


    void BugsSearchArtistAll::request_more_artistDraw(){

        // j220913 list count check
        int width_cnt = global.LmtCnt / 220;
        int mod = this->artist_draw_cnt % width_cnt;

        if(mod == 0){
            this->artist_widget_cnt = width_cnt * 10;
        }
        else{
            this->artist_widget_cnt = width_cnt * 10 + (width_cnt - mod);
        }
        // j220913 list count check

        int start_index = this->artist_draw_cnt;
        int max_cnt = ((this->artist_total_cnt - this->artist_draw_cnt) > this->artist_widget_cnt ) ? this->artist_widget_cnt : (this->artist_total_cnt - this->artist_draw_cnt);
        this->artist_draw_cnt += max_cnt;

        for(int i = start_index; i < this->artist_draw_cnt; i++){
            this->list_artist_all[i] = new bugs::ItemArtist_bugs(i, SECTION_FOR_MORE_POPUP___artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->list_artist_all[i], &bugs::ItemArtist_bugs::signal_clicked, this, &BugsSearchArtistAll::slot_clickedItemArtist);
        }

        for(int i = start_index; i < this->artist_draw_cnt; i++){
            this->list_artist_all[i]->setData(this->list_artist->at(i));
            this->flowLayout_artist->addWidget(this->list_artist_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_artist_draw = false;
    }


    /**
     * @brief BugsSearchArtistAll::slot_applyResult_artists
     * @param list_data
     * @param flag_lastPage
     */
    void BugsSearchArtistAll::slot_applyResult_artists(const QList<bugs::ArtistItemData>& list_data, const bool flag_lastPage){

        /*this->flagReqMore_artist = false;            // init (ready to get more data)
        this->flag_lastPage_artist = flag_lastPage;

        int start_index = this->list_artist->length();
        this->list_artist->append(list_data);
        this->createList_itemArtist_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_180x180, this->flowLayout_artist, start_index, SECTION_FOR_MORE_POPUP___artist);*/

        if(list_data.size() > 0){
            this->flag_lastPage_artist = flag_lastPage;
            this->flagReqMore_artist = false;

            if(this->list_artist->size() == 0){
                this->artist_total_cnt = list_data.at(0).total_count;
            }

            int start_index = this->list_artist->size();
            this->list_artist->append(list_data);

            if(start_index == 0){
                int max_cnt = this->list_artist->size();
                this->artist_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->list_artist_all[i] = new bugs::ItemArtist_bugs(i, SECTION_FOR_MORE_POPUP___artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->list_artist_all[i], &bugs::ItemArtist_bugs::signal_clicked, this, &BugsSearchArtistAll::slot_clickedItemArtist);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->list_artist_all[i]->setData(this->list_artist->at(i));
                    this->flowLayout_artist->addWidget(this->list_artist_all[i]);

                    QCoreApplication::processEvents();
                }
                ContentLoadingwaitingMsgHide();      //cheon Tidal

                this->flag_flow_draw = true;
                this->flag_artist_draw = false;
            }

            this->request_more_artistData();
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            if(this->list_artist->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Artist_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->flowLayout_artist->addWidget(noData_widget);
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - Itemartist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void BugsSearchArtistAll::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemArtist(this->list_artist, clickMode, index, section);
    }


    /**
     * @brief BugsSearchArtistAll::proc_preUpdate_favorite_artist
     * @param data_itemPos
     */
    void BugsSearchArtistAll::proc_preUpdate_favorite_artist(const ItemPositionData& data_itemPos){

        // Update Data
        bugs::ArtistItemData data_artist = this->list_artist->at(data_itemPos.index);
        data_artist.likes_yn = data_itemPos.likes_yn;
        this->list_artist->replace(data_itemPos.index, data_artist);
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
    void BugsSearchArtistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___artist){
            this->proc_clicked_optMorePopup_fromArtist(this->list_artist, index, clickMode);
        }
    }
}
