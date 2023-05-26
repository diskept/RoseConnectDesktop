#include "tidal/TidalArtistListAll.h"
#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QScrollBar>


namespace tidal {

    const int SECTION_FOR_MORE_POPUP___artist = 0;


    /**
     * @brief TidalMoodsMain::TidalMoodsMain
     * @param parent
     */
    TidalArtistListAll::TidalArtistListAll(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll, parent) {

        // 기본 UI 세팅
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ");
        this->setUIControl_artists();

        this->list_artist = new QList<tidal::ArtistItemData>();
    }


    /**
     * @brief 소멸자.
     */
    TidalArtistListAll::~TidalArtistListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalArtistListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        tidal::PageInfo_ArtistAllView data_pageInfo = ConvertData::convertData_pageInfo_artistAllView(jsonObj);
        QString curr_api_subPath = this->api_subPath;
        this->flagNeedReload = false;

        if(curr_api_subPath != data_pageInfo.api_subPath){
            this->flagNeedReload = true;

            this->label_mainTitle->setText(data_pageInfo.pathTitle);
            this->api_subPath = data_pageInfo.api_subPath;

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->setUIControl_artists();

            // init
            this->next_offset = 0;
            this->artist_total_cnt = 0;
            this->artist_draw_cnt = 0;

            this->list_artist->clear();;

            // request HTTP API
            this->flagReqMore_artist = false;
            this->flag_lastPage_artist = false;

            this->flag_artist_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_artistData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalArtistListAll::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            GSCommon::clearLayout(this->flowLayout_artists);

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
    void TidalArtistListAll::setUIControl_artists(){

        this->flowLayout_artists = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void TidalArtistListAll::proc_wheelEvent_to_getMoreData(){

        if((this->artist_total_cnt > this->artist_draw_cnt) && (this->list_artist->size() > this->artist_draw_cnt) && (this->flag_artist_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_artist_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_artistDraw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Artist 데이터를 더 요청한다.
     * @param page
     */
    void TidalArtistListAll::request_more_artistData(){

        if(!this->flagReqMore_artist && !this->flag_lastPage_artist){
            this->flagReqMore_artist = true;

            // j220913 list count check
            int width_cnt = global.LmtCnt / 220;
            int mod = this->artist_draw_cnt % width_cnt;
            int height_cnt = 0;

            if(mod == 0){
                if(width_cnt > 5){
                    height_cnt = 30 / width_cnt;
                    this->artist_draw_cnt = width_cnt * height_cnt;
                }
                else{
                    this->artist_draw_cnt = width_cnt * 10;
                }
            }
            else{
                if(width_cnt > 5){
                    height_cnt = 30 / width_cnt;
                    this->artist_draw_cnt = width_cnt * height_cnt + (width_cnt - mod);
                }
                else{
                    this->artist_draw_cnt = width_cnt * 10 + (width_cnt - mod);
                }
            }
            // j220913 list count check

            // next_offset
            this->next_offset = this->list_artist->size();

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_artists, this, &TidalArtistListAll::slot_applyResult_artists);
            proc->request_tidal_getList_artists(this->api_subPath, this->artist_draw_cnt, this->next_offset);

            if(this->next_offset == 0){
                this->flag_artist_draw = true;
            }
        }
    }


    void TidalArtistListAll::request_more_artistDraw(){

        // j220913 list count check
        int width_cnt = global.LmtCnt / 220;
        int mod = this->artist_draw_cnt % width_cnt;
        int height_cnt = 0;

        if(mod == 0){
            if(width_cnt > 5){
                height_cnt = 30 / width_cnt;
                this->artist_draw_cnt = width_cnt * height_cnt;
            }
            else{
                this->artist_draw_cnt = width_cnt * 10;
            }
        }
        else{
            if(width_cnt > 5){
                height_cnt = 30 / width_cnt;
                this->artist_draw_cnt = width_cnt * height_cnt + (width_cnt - mod);
            }
            else{
                this->artist_draw_cnt = width_cnt * 10 + (width_cnt - mod);
            }
        }
        // j220913 list count check

        int start_index = this->artist_draw_cnt;
        int max_cnt = ((this->artist_total_cnt - this->artist_draw_cnt) > this->artist_draw_cnt ) ? this->artist_draw_cnt : (this->artist_total_cnt - this->artist_draw_cnt);
        this->artist_draw_cnt += max_cnt;

        for(int i = start_index; i < this->artist_draw_cnt; i++){
            this->listArtist_all[i] = new tidal::ItemArtist(i, SECTION_FOR_MORE_POPUP___artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->listArtist_all[i], &tidal::ItemArtist::signal_clicked, this, &TidalArtistListAll::slot_clickedItemArtist);
        }

        for(int i = start_index; i < this->artist_draw_cnt; i++){
            this->listArtist_all[i]->setData(this->list_artist->at(i));
            this->flowLayout_artists->addWidget(this->listArtist_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_artist_draw = false;
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] [Override]
     * @param list_data
     */
    void TidalArtistListAll::slot_applyResult_artists(const QList<tidal::ArtistItemData> &list_data, const bool flag_lastPage){

        /*this->flag_lastPage_artist = flag_lastPage;
        this->flagReqMore_artist = false;

        int start_index = this->list_artist->size();
        this->list_artist->append(list_data);
        this->createList_itemArtist_applyingWithData(list_data, AbstractItem::ImageSizeMode::Square_180x180, this->flowLayout_artists, start_index, SECTION_FOR_MORE_POPUP___artist);*/

        if(list_data.size() > 0){

            this->flag_lastPage_artist = flag_lastPage;
            this->flagReqMore_artist = false;

            if(this->list_artist->size() == 0){
                this->artist_total_cnt = list_data.at(0).artist_total_cnt;
            }

            int start_index = this->list_artist->size();
            this->list_artist->append(list_data);

            if(start_index == 0){
                int max_cnt = this->list_artist->size();
                this->artist_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->listArtist_all[i] = new tidal::ItemArtist(i, SECTION_FOR_MORE_POPUP___artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->listArtist_all[i], &tidal::ItemArtist::signal_clicked, this, &TidalArtistListAll::slot_clickedItemArtist);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listArtist_all[i]->setData(this->list_artist->at(i));
                    this->flowLayout_artists->addWidget(this->listArtist_all[i]);

                    QCoreApplication::processEvents();
                }

                ContentLoadingwaitingMsgHide();

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

                this->flowLayout_artists->addWidget(noData_widget);
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - ItemArtist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalArtistListAll::slot_clickedItemArtist(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemArtist(this->list_artist, clickMode, index, section);
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
    void TidalArtistListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___artist){
            this->proc_clicked_optMorePopup_fromArtist(this->list_artist, index, clickMode);
        }
    }

}
