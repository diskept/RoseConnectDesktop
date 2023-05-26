#include "tidal/TidalSearchVidelAll.h"

#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"
#include "tidal/ProcRosePlay_withTidal.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"

#include <QScrollBar>


namespace tidal {

    const int SECTION_FOR_MORE_POPUP___video = 1;


    /**
     * @brief Video All List
     * @param parent
     */
    TidalSearchVidelAll::TidalSearchVidelAll(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // Data
        this->list_video = new QList<tidal::VideoItemData>();

        // UI
        this->setUIControl_video();
    }


    TidalSearchVidelAll::~TidalSearchVidelAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalSearchVidelAll::setJsonObject_forData(const QJsonObject &jsonObj){

        common::SearchData tmp_data = common::ConvertData_common::convertData_searchData(jsonObj);
        this->flagNeedReload = false;

        if(tmp_data.search_word != this->data_search.search_word){
            this->flagNeedReload = true;

            this->data_search = common::ConvertData_common::convertData_searchData(jsonObj);

            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + tr("Search result") + " - Videos");

            // init
            this->next_offset = 0;
            this->video_total_cnt = 0;
            this->video_draw_cnt = 0;

            this->list_video->clear();

            // request HTTP API
            this->flagReqMore_video = false;
            this->flag_lastPage_video = false;

            this->flag_video_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_videoData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 화면 활성화
     */
    void TidalSearchVidelAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            // init UI
            GSCommon::clearLayout(this->flowLayout_video);

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
        }
    }


    // MARK : Create UI and Set ------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void TidalSearchVidelAll::setUIControl_video(){

        // UI
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ");
        this->flowLayout_video = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void TidalSearchVidelAll::proc_wheelEvent_to_getMoreData(){

        if((this->video_total_cnt > this->video_draw_cnt) && (this->flag_video_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_video_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_videoDraw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Artist 데이터를 더 요청한다.
     * @param page
     */
    void TidalSearchVidelAll::request_more_videoData(){

        if(!this->flagReqMore_video && !this->flag_lastPage_video){
            this->flagReqMore_video = true;

            // j220913 list count check
            int width_cnt = global.LmtCnt / 304;
            int mod = this->video_widget_cnt % width_cnt;
            int height_cnt = 0;

            if(mod == 0){
                if(width_cnt > 5){
                    height_cnt = 50 / width_cnt;
                    this->video_widget_cnt = width_cnt * height_cnt;
                }
                else{
                    this->video_widget_cnt = width_cnt * 10;
                }
            }
            else{
                if(width_cnt > 5){
                    height_cnt = 50 / width_cnt;
                    this->video_widget_cnt = width_cnt * height_cnt + (width_cnt - mod);
                }
                else{
                    this->video_widget_cnt = width_cnt * 10 + (width_cnt - mod);
                }
            }
            // j220913 list count check

            // next_offset
            this->next_offset = this->list_video->size();

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_videos, this, &TidalSearchVidelAll::slot_applyResult_videos);
            proc->request_tidal_searchAllItems(this->data_search.search_word, this->video_widget_cnt, this->next_offset, ProcCommon::TidalSearch_ContentType::Videos);

            if(this->next_offset == 0){
                this->flag_video_draw = true;
            }
        }
    }


    void TidalSearchVidelAll::request_more_videoDraw(){

        // j220913 list count check
        int width_cnt = global.LmtCnt / 304;
        int mod = this->video_widget_cnt % width_cnt;
        int height_cnt = 0;

        if(mod == 0){
            if(width_cnt > 5){
                height_cnt = 50 / width_cnt;
                this->video_widget_cnt = width_cnt * height_cnt;
            }
            else{
                this->video_widget_cnt = width_cnt * 10;
            }
        }
        else{
            if(width_cnt > 5){
                height_cnt = 50 / width_cnt;
                this->video_widget_cnt = width_cnt * height_cnt + (width_cnt - mod);
            }
            else{
                this->video_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
        }
        // j220913 list count check

        int start_index = this->video_draw_cnt;
        int max_cnt = ((this->video_total_cnt - this->video_draw_cnt) > this->video_widget_cnt ) ? this->video_widget_cnt : (this->video_total_cnt - this->video_draw_cnt);
        this->video_draw_cnt += max_cnt;

        for(int i = start_index; i < this->video_draw_cnt; i++){
            this->listVideo_all[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___video, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157);
            connect(this->listVideo_all[i], &tidal::ItemVideo::signal_clicked, this, &TidalSearchVidelAll::slot_clickedItemVideo);
        }

        for(int i = start_index; i < this->video_draw_cnt; i++){
            this->listVideo_all[i]->setData(this->list_video->at(i));
            this->flowLayout_video->addWidget(this->listVideo_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_video_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : [Slot] functions
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief about New Video
     * @param list_data
     */
    void TidalSearchVidelAll::slot_applyResult_videos(const QList<tidal::VideoItemData> &list_data, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_video = flag_lastPage;
            this->flagReqMore_video = false;

            if(this->list_video->size() == 0){
                this->video_total_cnt = list_data.at(0).totalCount;
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            }

            int start_index = this->list_video->size();
            this->list_video->append(list_data);

            if(start_index == 0){
                int max_cnt = this->list_video->size();
                this->video_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->listVideo_all[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___video, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157);
                    connect(this->listVideo_all[i], &tidal::ItemVideo::signal_clicked, this, &TidalSearchVidelAll::slot_clickedItemVideo);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listVideo_all[i]->setData(this->list_video->at(i));
                    this->flowLayout_video->addWidget(this->listVideo_all[i]);

                    QCoreApplication::processEvents();
                }
                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_video_draw = false;
            }


            this->request_more_videoData();
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->list_video->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Video_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->flowLayout_video->addWidget(noData_widget);
            }
        }
    }


    /**
     * @brief [slot] override - ItemVideo 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalSearchVidelAll::slot_clickedItemVideo(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___video){

            if(clickMode == AbstractItem::ClickMode::PlayBtn || clickMode == AbstractItem::ClickMode::AllBox){

                int curr_clickMode = global.device.getMusicPlayType();

                OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                switch(curr_clickMode)
                {
                    case 13:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                        break;
                    case 12:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                        break;
                    case 15:
                        playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                        break;
                    case 16:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                        break;
                    case 17:
                        playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                        break;
                    case 18:
                        playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                        break;
                    case 19:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                        break;
                }

                QJsonArray tmpJsonArr = QJsonArray();
                for(int i = index; i < this->list_video->size(); i++){
                    QJsonObject tmpJsonObj = this->list_video->at(i).jsonObj_toPlay;
                    tmpJsonArr.append(tmpJsonObj);
                }

                // Rose Play 요청
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_videolist(tmpJsonArr, playType);

                /*if(playType == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                        || playType == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                        || playType == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last)
                {
                    QJsonArray tmpJsonArr = QJsonArray();
                    for(int i = index; i < this->list_video->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }

                    // Rose Play 요청
                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_videolist(tmpJsonArr, playType);
                }
                else{
                    QJsonObject tmpJsonObj = this->list_video->at(index).jsonObj_toPlay;

                    // Rose Play 요청
                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_video(tmpJsonObj, playType);
                }*/
            }
            else{
                this->proc_clicked_itemVideo(this->list_video, clickMode, index, section);
            }
        }
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
    void TidalSearchVidelAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___video){
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last

                    )
            {
                if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                        || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                        || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last)
                {
                    QJsonArray tmpJsonArr = QJsonArray();
                    for(int i = index; i < this->list_video->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }

                    // Rose Play 요청
                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_videolist(tmpJsonArr, clickMode);
                }
                else{
                    QJsonObject tmpJsonObj = this->list_video->at(index).jsonObj_toPlay;

                    // Rose Play 요청
                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_video(tmpJsonObj, clickMode);
                }
            }
            else{
                this->proc_clicked_optMorePopup_fromVideo(this->list_video, index, clickMode);
            }
        }
    }

}
