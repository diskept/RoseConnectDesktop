#include "qobuz/QobuzAlbumListAll.h"

#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ProcCommon_forQobuz.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"

#include <QScrollBar>


namespace qobuz {

    const int SECTION_FOR_MORE_POPUP___albums = 1;


    /**
     * @brief QobuzMoodsMain::QobuzMoodsMain
     * @param parent
     */
    QobuzAlbumListAll::QobuzAlbumListAll(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // 기본 UI 세팅
        this->label_mainTitle = this->get_addUIControl_mainTitle("noname title");
        this->setUIControl_albums();

        this->list_album = new QList<qobuz::AlbumItemData>();
    }


    /**
     * @brief 소멸자.
     */
    QobuzAlbumListAll::~QobuzAlbumListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzAlbumListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        qobuz::PageInfo_AlbumAllView data_pageInfo = ConvertData::convertData_pageInfo_albumAllView(jsonObj);        
        this->flagNeedReload = false;

        if((this->api_subPath != data_pageInfo.api_subPath) || (this->api_mainTitle != data_pageInfo.pathTitle)){
            this->flagNeedReload = true;

            this->api_mainTitle = data_pageInfo.pathTitle;
            this->api_subPath = data_pageInfo.api_subPath;
            this->type = data_pageInfo.type;
            this->genre_id = data_pageInfo.genre_id;
            this->genre_ids = data_pageInfo.genre_ids;
            this->artist_id = data_pageInfo.artist_id;
            this->page = data_pageInfo.page;

            this->label_mainTitle->setText(this->api_mainTitle);

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
    void QobuzAlbumListAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

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
    void QobuzAlbumListAll::setUIControl_albums(){

        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }



    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void QobuzAlbumListAll::proc_wheelEvent_to_getMoreData(){

        if((this->album_total_cnt > this->album_draw_cnt) && (this->list_album->size() > this->album_draw_cnt) && (this->flag_album_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_album_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
    }


    void QobuzAlbumListAll::resizeEvent(QResizeEvent *event){//c230223

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
    void QobuzAlbumListAll::request_more_albumData(){

        if(!this->flagReqMore_album && !this->flag_lastPage_album){
            this->flagReqMore_album = true;

            // j220913 list count check
            int width_cnt;//c230223
            if(flowLayout_albums->sizeHint().width() < 0) {//c230223
                width_cnt = global.LmtCnt / 217;
            }else{
                width_cnt = global.LmtCnt / flowLayout_albums->sizeHint().width();//
            }
            int mod = this->album_draw_cnt % width_cnt;

            if(mod == 0){
                this->album_widget_cnt = width_cnt * 10;
            }
            else{
                this->album_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
            // j220913 list count check

            // next_offset
            this->next_offset = this->list_album->size();

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);

            if(this->page.isEmpty())
            {
                connect(proc, &ProcCommon::completeReq_list_albums, this, &QobuzAlbumListAll::slot_applyResult_albums);
                proc->request_qobuz_getList_albums(this->api_subPath, this->type, this->genre_ids, this->album_widget_cnt, this->next_offset);
            }
            else
            {
                connect(proc, &ProcCommon::completeReq_list_albums, this, &QobuzAlbumListAll::slot_applyResult_albums);
                proc->request_qobuz_getList_lastrelease(this->api_subPath, "albums_with_last_release", this->artist_id, this->album_widget_cnt, this->next_offset);
            }

            if(this->next_offset == 0){
                this->flag_album_draw = true;
            }
        }
    }


    void QobuzAlbumListAll::request_more_albumDraw(){

        // j220913 list count check
        int width_cnt;//c230223
        if(flowLayout_albums->sizeHint().width() < 0) {//c230223
            width_cnt = global.LmtCnt / 217;
        }else{
            width_cnt = global.LmtCnt / flowLayout_albums->sizeHint().width();//
        }
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
            this->listAlbum_all[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->listAlbum_all[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzAlbumListAll::slot_clickedItemAlbum);
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
    void QobuzAlbumListAll::slot_applyResult_albums(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_album->size() == 0){
                this->album_total_cnt = list_data.at(0).album_total_cnt;
            }

            int start_index = this->list_album->size();
            this->list_album->append(list_data);

            if(start_index == 0){
                int max_cnt = this->list_album->size();
                this->album_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->listAlbum_all[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzAlbumListAll::slot_clickedItemAlbum);
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
    void QobuzAlbumListAll::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
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
    void QobuzAlbumListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
    }





    /**
     * @brief QobuzMoodsMain::QobuzMoodsMain
     * @param parent
     */
    QobuzLastReleaseList::QobuzLastReleaseList(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // 기본 UI 세팅
        this->label_mainTitle = this->get_addUIControl_mainTitle("noname title");
        this->setUIControl_albums();

        this->list_album = new QList<qobuz::AlbumItemData>();
    }


    /**
     * @brief 소멸자.
     */
    QobuzLastReleaseList::~QobuzLastReleaseList(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzLastReleaseList::setJsonObject_forData(const QJsonObject &jsonObj){

        qobuz::PageInfo_AlbumAllView data_pageInfo = ConvertData::convertData_pageInfo_albumAllView(jsonObj);

        this->flagNeedReload = false;

        if((this->api_subPath != data_pageInfo.api_subPath) || (this->api_mainTitle != data_pageInfo.pathTitle)){
            this->flagNeedReload = true;

            this->api_mainTitle = data_pageInfo.pathTitle;
            this->api_subPath = data_pageInfo.api_subPath;
            this->type = data_pageInfo.type;
            this->artist_id = data_pageInfo.artist_id;

            this->label_mainTitle->setText(this->api_mainTitle);

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
    void QobuzLastReleaseList::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

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
    void QobuzLastReleaseList::setUIControl_albums(){

        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void QobuzLastReleaseList::proc_wheelEvent_to_getMoreData(){

        if((this->album_total_cnt > this->album_draw_cnt) && (this->list_album->size() > this->album_draw_cnt) && (this->flag_album_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_album_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
    }

    void QobuzLastReleaseList::resizeEvent(QResizeEvent *event){//c230223

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
    void QobuzLastReleaseList::request_more_albumData(){

        if(!this->flagReqMore_album && !this->flag_lastPage_album){
            this->flagReqMore_album = true;

            // j220913 list count check
            int width_cnt;//c230223
            if(flowLayout_albums->sizeHint().width() < 0) {//c230223
                width_cnt = global.LmtCnt / 217;
            }else{
                width_cnt = global.LmtCnt / flowLayout_albums->sizeHint().width();//
            }
            int mod = this->album_draw_cnt % width_cnt;

            if(mod == 0){
                this->album_widget_cnt = width_cnt * 10;
            }
            else{
                this->album_widget_cnt = width_cnt * 10 + (width_cnt - mod);
            }
            // j220913 list count check

            // next_offset
            this->next_offset = this->list_album->size();

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_lastrelease, this, &QobuzLastReleaseList::slot_applyResult_albums);
            proc->request_qobuz_getList_lastrelease(this->api_subPath, this->type, this->artist_id, this->album_widget_cnt, this->next_offset);

        }
    }


    void QobuzLastReleaseList::request_more_albumDraw(){

        // j220913 list count check
        int width_cnt;//c230223
        if(flowLayout_albums->sizeHint().width() < 0) {//c230223
            width_cnt = global.LmtCnt / 217;
        }else{
            width_cnt = global.LmtCnt / flowLayout_albums->sizeHint().width();//
        }
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
            this->listAlbum_all[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->listAlbum_all[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzLastReleaseList::slot_clickedItemAlbum);
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
    void QobuzLastReleaseList::slot_applyResult_albums(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_album->size() == 0){
                this->album_total_cnt = list_data.at(0).album_total_cnt;
            }

            int start_index = this->list_album->size();
            this->list_album->append(list_data);

            if(start_index == 0){
                int max_cnt = this->list_album->size();
                this->album_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->listAlbum_all[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzLastReleaseList::slot_clickedItemAlbum);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i]->setData(this->list_album->at(i));
                    this->flowLayout_albums->addWidget(this->listAlbum_all[i]);

                    QCoreApplication::processEvents();
                }
                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }

            this->request_more_albumData();
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
    void QobuzLastReleaseList::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
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
    void QobuzLastReleaseList::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }

    }





    /**
     * @brief QobuzMoodsMain::QobuzMoodsMain
     * @param parent
     */
    QobuzSameArtistAll::QobuzSameArtistAll(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        // 기본 UI 세팅
        this->label_mainTitle = this->get_addUIControl_mainTitle("noname title");
        this->setUIControl_albums();

        this->list_album = new QList<qobuz::AlbumItemData>();
    }


    /**
     * @brief 소멸자.
     */
    QobuzSameArtistAll::~QobuzSameArtistAll(){

        this->deleteLater();
    }



    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzSameArtistAll::setJsonObject_forData(const QJsonObject &jsonObj){

        qobuz::PageInfo_AlbumAllView data_pageInfo = ConvertData::convertData_pageInfo_albumAllView(jsonObj);

        this->flagNeedReload = false;

        if((this->api_subPath != data_pageInfo.api_subPath) || (this->api_mainTitle != data_pageInfo.pathTitle)){
            this->flagNeedReload = true;

            this->api_mainTitle = data_pageInfo.pathTitle;
            this->api_subPath = data_pageInfo.api_subPath;
            this->type = data_pageInfo.type;
            this->artist_id = data_pageInfo.artist_id;

            this->list_album->clear();
            this->list_album->append(data_pageInfo.list_sameAritst);

            this->label_mainTitle->setText(this->api_mainTitle);
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void QobuzSameArtistAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

            GSCommon::clearLayout(this->flowLayout_albums);

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            QJsonArray json_arr;
            this->slot_applyResult_albums(*this->list_album, json_arr, false);
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
    void QobuzSameArtistAll::setUIControl_albums(){

        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
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
    void QobuzSameArtistAll::slot_applyResult_albums(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.at(0).album_total_cnt > 50){
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }

        //this->list_album->append(list_data);
        int max_cnt = this->list_album->size();

        for(int i = 0; i < max_cnt; i++){
            this->sameArits_all[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->sameArits_all[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzSameArtistAll::slot_clickedItemAlbum);
        }

        for(int i = 0; i < max_cnt; i++){
            this->sameArits_all[i]->setData(this->list_album->at(i));
            this->flowLayout_albums->addWidget(this->sameArits_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
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
    void QobuzSameArtistAll::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
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
    void QobuzSameArtistAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
    }


}
