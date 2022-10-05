#include "bugs/BugsSearchVideoAll.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/ProcBugsAPI.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QScrollBar>


namespace bugs {

    const int SECTION_FOR_MORE_POPUP___video = 1;

    /**
     * @brief BugsSearchPD_videoAll::BugsSearchPD_videoAll
     * @param parent
     */
    BugsSearchVideoAll::BugsSearchVideoAll(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // Data
        this->list_video = new QList<bugs::VideoItemData>();

        if(global.user_forBugs.isLogined() == true){

            // 기본 UI 세팅
            this->setUIControl_videos();
        }
    }


    /**
     * @brief 소멸자.
     */
    BugsSearchVideoAll::~BugsSearchVideoAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsSearchVideoAll::setJsonObject_forData(const QJsonObject &jsonObj){

        common::SearchData tmp_searchData = common::ConvertData_common::convertData_searchData(jsonObj);
        this->flagNeedReload = false;

        if(this->data_search.search_word != tmp_searchData.search_word){
            this->flagNeedReload = true;

            this->data_search = tmp_searchData;
            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + "검색 결과 - 영상");

            // init
            this->next_offset = 0;
            this->video_total_cnt = 0;
            this->video_draw_cnt = 0;

            this->list_video->clear();

            // request HTTP API
            this->flagReqMore_video = false;
            this->flag_lastPage_video = false;

            this->flag_video_draw = false;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_videoData();
        }
    }


    /**
     * @brief BugsSearchVideoAll::applyLoginSuccessState
     */
    /*void BugsSearchVideoAll::applyLoginSuccessState(){

        if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                this->setUIControl_logined();
                this->flagNeedReload = true;        // 강제 세팅
            }

            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + "검색 결과 - 영상");
            this->list_video->clear();
            this->jsonArr_videos_toPlay = QJsonArray();
            GSCommon::clearLayout(this->flowLayout_video);

            // request HTTP API
            this->flagReqMore_video = false;
            this->flag_lastPage_video = false;
            this->request_more_videoData();
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
    void BugsSearchVideoAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            GSCommon::clearLayout(this->flowLayout_video);

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
    void BugsSearchVideoAll::setUIControl_videos(){

        this->label_mainTitle = this->get_addUIControl_mainTitle(" ");

        GSCommon::clearLayout(this->box_contents);      // Layout 초기화
        this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // layout for items
        this->flowLayout_video = this->get_addUIControl_flowLayout(0, 20);
        GSCommon::clearLayout(this->flowLayout_video);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief BugsSearchVideoAll::proc_wheelEvent_to_getMoreData
     */
    void BugsSearchVideoAll::proc_wheelEvent_to_getMoreData(){

        if((this->video_total_cnt > this->video_draw_cnt) && (this->list_video->size() > this->video_draw_cnt) && (this->flag_video_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_video_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_videoDraw();
        }
    }


    /**
     * @brief BugsSearchVideoAll::request_more_videoData
     */
    void BugsSearchVideoAll::request_more_videoData(){

        if(!this->flagReqMore_video && !this->flag_lastPage_video){
            this->flagReqMore_video = true;

            // j220913 list count check
            int width_cnt = global.LmtCnt / 220;
            int mod = this->video_widget_cnt % width_cnt;

            if(mod == 0){
                this->video_widget_cnt = width_cnt * 10;
            }
            else{
                this->video_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
            // j220913 list count check

            // next_offset
            if(this->list_video->size() == 0){
                this->next_offset = 1;
            }
            else{
                this->next_offset++;
            }

            // request HTTP API
            ProcBugsAPI *proc = new ProcBugsAPI(this);
            connect(proc, &ProcBugsAPI::completeReq_list_videos, this, &BugsSearchVideoAll::slot_applyResult_videos);
            proc->request_bugs_search_video(this->data_search.search_word, this->next_offset, this->video_widget_cnt, ProcBugsAPI::BugsSearch_Sort::exact);

            if(this->next_offset == 0){
                this->flag_video_draw = true;
            }
        }
    }


    void BugsSearchVideoAll::request_more_videoDraw(){

        // j220913 list count check
        int width_cnt = global.LmtCnt / 220;
        int mod = this->video_widget_cnt % width_cnt;

        if(mod == 0){
            this->video_widget_cnt = width_cnt * 10;
        }
        else{
            this->video_widget_cnt = width_cnt * 10 + (width_cnt - mod);
        }
        // j220913 list count check

        int start_index = this->video_draw_cnt;
        int max_cnt = ((this->video_total_cnt - this->video_draw_cnt) >  this->video_widget_cnt ) ?  this->video_widget_cnt : (this->video_total_cnt - this->video_draw_cnt);
        this->video_draw_cnt += max_cnt;

        for(int i = start_index; i < this->video_draw_cnt; i++){
            this->listVideo_all[i] = new bugs::ItemVideo_bugs(i, SECTION_FOR_MORE_POPUP___video, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157);
            connect(this->listVideo_all[i], &bugs::ItemVideo_bugs::signal_clicked, this, &BugsSearchVideoAll::slot_clickedItemVideo);
        }

        for(int i = start_index; i < this->video_draw_cnt; i++){
            this->listVideo_all[i]->setData(this->list_video->at(i));
            this->flowLayout_video->addWidget(this->listVideo_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_video_draw = false;
        //this->flagReqMore_video = false;//c220716
    }


    /**
     * @brief BugsSearchVideoAll::slot_applyResult_videos
     * @param list_data
     * @param flag_lastPage
     */
    void BugsSearchVideoAll::slot_applyResult_videos(const QList<bugs::VideoItemData>& list_data, const QJsonArray& jsonArr_dataToPlay, const bool flag_lastPage){

        /*this->flagReqMore_video = false;            // init (ready to get more data)
        this->flag_lastPage_video = flag_lastPage;

        ProcJsonEasy::mergeJsonArray(this->jsonArr_videos_toPlay, jsonArr_dataToPlay);

        int start_index = this->list_video->length();
        this->list_video->append(list_data);
        this->createList_itemVideo_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Ractangle_251x139, this->flowLayout_video, start_index, SECTION_FOR_MORE_POPUP___video);*/

        if(list_data.size() > 0){
            this->flag_lastPage_video = flag_lastPage;
            this->flagReqMore_video = false;//c220716

            if(this->list_video->size() == 0){
                this->jsonArr_videos_toPlay = QJsonArray();
                this->video_total_cnt = list_data.at(0).total_count;
            }

            int start_index = this->list_video->size();
            this->list_video->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_videos_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                int max_cnt = this->list_video->size();
                this->video_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->listVideo_all[i] = new bugs::ItemVideo_bugs(i, SECTION_FOR_MORE_POPUP___video, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157);
                    connect(this->listVideo_all[i], &bugs::ItemVideo_bugs::signal_clicked, this, &BugsSearchVideoAll::slot_clickedItemVideo);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listVideo_all[i]->setData(this->list_video->at(i));
                    this->flowLayout_video->addWidget(this->listVideo_all[i]);

                    QCoreApplication::processEvents();
                }

                this->flag_flow_draw = true;
                this->flag_video_draw = false;
            }

            ContentLoadingwaitingMsgHide();      //cheon Tidal
            this->request_more_videoData();
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            if(this->list_video->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Video_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->flowLayout_video->addWidget(noData_widget);
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - Itemvideo 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void BugsSearchVideoAll::slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemVideo(this->list_video, this->jsonArr_videos_toPlay, clickMode, index, section);
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
    void BugsSearchVideoAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___video){
            this->proc_clicked_optMorePopup_fromVideo(this->list_video, this->jsonArr_videos_toPlay, index, clickMode);
        }
    }
}
