#include "tidal/TidalAlbumListAll.h"
#include "tidal/ProcRosePlay_withTidal.h"
#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"

#include <QScrollBar>


namespace tidal {

    const int SECTION_FOR_MORE_POPUP___albums = 1;


    /**
     * @brief TidalMoodsMain::TidalMoodsMain
     * @param parent
     */
    TidalAlbumListAll::TidalAlbumListAll(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // 기본 UI 세팅
        this->label_mainTitle = this->get_addUIControl_mainTitle("noname title");
        this->setUIControl_albums();

        this->list_album = new QList<tidal::AlbumItemData>();
    }

    /**
     * @brief 소멸자.
     */
    TidalAlbumListAll::~TidalAlbumListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalAlbumListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        tidal::PageInfo_AlbumAllView data_pageInfo = ConvertData::convertData_pageInfo_albumAllView(jsonObj);
        QString curr_api_subPath = this->api_subPath;
        this->flagNeedReload = false;

        if(curr_api_subPath != data_pageInfo.api_subPath){
            this->flagNeedReload = true;

            this->visualAlbum_first = false;
            this->label_mainTitle->setText(data_pageInfo.pathTitle);
            this->api_subPath = data_pageInfo.api_subPath;

            // init
            this->next_offset = 0;
            this->album_total_cnt = 0;
            this->album_draw_cnt = 0;

            this->list_album->clear();

            // request HTTP API
            this->flagReqMore_album = false;
            this->flag_lastPage_album = false;

            this->flag_album_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalAlbumListAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            GSCommon::clearLayout(this->flowLayout_albums);

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
    void TidalAlbumListAll::setUIControl_albums(){

        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }



    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void TidalAlbumListAll::proc_wheelEvent_to_getMoreData(){

        if((this->album_total_cnt > this->album_draw_cnt) && (this->list_album->size() > this->album_draw_cnt) && (this->flag_album_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_album_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
    }

    void TidalAlbumListAll::resizeEvent(QResizeEvent *event){//c230223
        AbstractTidalSubWidget::resizeEvent(event);
        int w = flowLayout_albums->sizeHint().width();
        int l = 80, r = 60, scrollbarW = 0;

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

        flowLayout_albums->setSpacingHV(0+i,20);
        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함

    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Album 데이터를 더 요청한다.
     * @param page
     */
    void TidalAlbumListAll::request_more_albumData(){

        if(!this->flagReqMore_album && !this->flag_lastPage_album){//cheon210704-list
            this->flagReqMore_album = true;//cheon210704-list

            // j220913 list count check
            int width_cnt;//c230223
            if(flowLayout_albums->sizeHint().width() < 0) {//c230223
                width_cnt = global.LmtCnt / 217;
            }else{
                width_cnt = global.LmtCnt / flowLayout_albums->sizeHint().width();//
            }
            int mod = this->album_draw_cnt % width_cnt;
            int height_cnt = 0;

            if(mod == 0){
                if(width_cnt > 5){
                    height_cnt = 50 / width_cnt;
                    this->album_widget_cnt = width_cnt * height_cnt;
                }
                else{
                    this->album_widget_cnt = width_cnt * 10;
                }
            }
            else{
                if(width_cnt > 5){
                    height_cnt = 50 / width_cnt;
                    this->album_widget_cnt = width_cnt * height_cnt + (width_cnt - mod);
                }
                else{
                    this->album_widget_cnt = width_cnt * 10 + (width_cnt - mod);
                }
            }
            // j220913 list count check

            // next_offset
            if((this->list_album->size() == 0) && (this->api_subPath == "visual album")){
                this->list_album->append(global.user_forTidal.list_visualData);
                this->next_offset = this->list_album->size();
                this->visualAlbum_first = false;
            }
            else{
                this->next_offset = this->list_album->size();
            }

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);

            if(this->api_subPath == "visual album"){
                connect(proc, &ProcCommon::completeReq_list_visuals, this, &TidalAlbumListAll::slot_applyResult_albums);
                proc->request_tidal_getList_visual(this->api_subPath, 50, this->next_offset);
            }
            else{
                connect(proc, &ProcCommon::completeReq_list_albums, this, &TidalAlbumListAll::slot_applyResult_albums);
                proc->request_tidal_getList_albums(this->api_subPath, this->album_widget_cnt, this->next_offset);
            }

        }//cheon210704-list

    }


    void TidalAlbumListAll::request_more_albumDraw(){

        // j220913 list count check
        int width_cnt;//c230223
        if(flowLayout_albums->sizeHint().width() < 0) {//c230223
            width_cnt = global.LmtCnt / 217;
        }else{
            width_cnt = global.LmtCnt / flowLayout_albums->sizeHint().width();//
        }
        int mod = this->album_draw_cnt % width_cnt;
        int height_cnt = 0;

        if(mod == 0){
            if(width_cnt > 5){
                height_cnt = 50 / width_cnt;
                this->album_widget_cnt = width_cnt * height_cnt;
            }
            else{
                this->album_widget_cnt = width_cnt * 10;
            }
        }
        else{
            if(width_cnt > 5){
                height_cnt = 50 / width_cnt;
                this->album_widget_cnt = width_cnt * height_cnt + (width_cnt - mod);
            }
            else{
                this->album_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
        }
        // j220913 list count check

        int start_index = this->album_draw_cnt;
        int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
        this->album_draw_cnt += max_cnt;

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i] = new tidal::ItemAlbum(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->listAlbum_all[i], &tidal::ItemAlbum::signal_clicked, this, &TidalAlbumListAll::slot_clickedItemAlbum);
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i]->setData(this->list_album->at(i));
            this->flowLayout_albums->addWidget(this->listAlbum_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_album_draw = false;
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
    void TidalAlbumListAll::slot_applyResult_albums(const QList<tidal::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        /*this->flag_lastPage_album = flag_lastPage;
        this->flagReqMore_album = false;

        int start_index;
        if(this->api_subPath == "visual album" && (this->visualAlbum_first == false)){
            this->visualAlbum_first = true;
            start_index = 0;
        }
        else{
            start_index = this->list_album->size();
        }

        this->list_album->append(list_data);
        //this->createList_itemAlbum_applyingWithData(list_data, AbstractItem::ImageSizeMode::Square_186x186, this->flowLayout_albums, start_index, SECTION_FOR_MORE_POPUP___albums);
        this->createList_itemAlbum_applyingWithData(*this->list_album, AbstractItem::ImageSizeMode::Square_200x200, this->flowLayout_albums, start_index, SECTION_FOR_MORE_POPUP___albums);

        if(this->list_album->size() % this->album_widget_cnt == 0 && this->list_album->size() % global.request_total_count != 0){
            request_more_albumData();//cheon210704-list
        }*/

        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_album->size() == 0){
                this->album_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_album->size();
            this->list_album->append(list_data);

            if(start_index == 0){
                int max_cnt = this->list_album->size();
                this->album_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i] = new tidal::ItemAlbum(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->listAlbum_all[i], &tidal::ItemAlbum::signal_clicked, this, &TidalAlbumListAll::slot_clickedItemAlbum);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i]->setData(this->list_album->at(i));
                    this->flowLayout_albums->addWidget(this->listAlbum_all[i]);

                    QCoreApplication::processEvents();
                }

                //c230306_1-start
                int w = flowLayout_albums->sizeHint().width();
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
                flowLayout_albums->setSpacingHV(mod+i,20);
                //c230306_1-end

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }
            else if(this->api_subPath == "visual album" && (this->visualAlbum_first == false)){
                this->visualAlbum_first = true;
                start_index = 0;

                int max_cnt = this->list_album->size();
                this->album_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i] = new tidal::ItemAlbum(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->listAlbum_all[i], &tidal::ItemAlbum::signal_clicked, this, &TidalAlbumListAll::slot_clickedItemAlbum);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i]->setData(this->list_album->at(i));
                    this->flowLayout_albums->addWidget(this->listAlbum_all[i]);

                    QCoreApplication::processEvents();
                }

                //c230306_1-start
                int w = flowLayout_albums->sizeHint().width();
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
                flowLayout_albums->setSpacingHV(mod+i,20);
                //c230306_1-end

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }

            if(this->flag_lastPage_album == false){
                this->request_more_albumData();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            if(this->list_album->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->flowLayout_albums->addWidget(noData_widget);
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
    void TidalAlbumListAll::slot_clickedItemAlbum(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(clickMode == AbstractItem::ClickMode::PlayBtn && this->list_album->at(0).visualAlbum == true && this->list_album->at(0).isVideo == true){
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_items_of_visual, this, &TidalAlbumListAll::slot_applyResult_tracks);
            proc->request_tidal_getList_items_of_visual(this->list_album->at(index).id, this->list_album->at(index).numberOfTracks);
        }
        else{
            this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
        }
    }


    void TidalAlbumListAll::slot_applyResult_tracks(const QList<tidal::TrackItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);
        Q_UNUSED(list_data);

        ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
        procRosePlay->requestPlayRose_videolist(jsonArr_dataToPlay, OptMorePopup::ClickMode::Play_RightNow);
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
    void TidalAlbumListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
    }
}
