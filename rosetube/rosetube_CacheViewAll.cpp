#include "rosetube/rosetube_CacheViewAll.h"

#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "widget/NoData_Widget.h"
#include "widget/toastmsg.h"

#include <QDebug>
#include <QScroller>
#include <QScrollBar>


namespace rosetube {

    const int HTTP_CACHING_LOCK = 0;
    const int HTTP_CACHING_CONTENTS = 1;
    const int HTTP_CACHING_LOCK_ITEM = 2;
    const int HTTP_CACHING_LOCK_ROSE = 3;
    const int HTTP_CACHING_DELETE_ITEM = 4;

    const int HTTP_SET_QUEUE = 99;

    const int SECTION_FOR_MORE_POPUP___CACHE = 0;


    RoseTubeCacheViewAll::RoseTubeCacheViewAll(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent){

        this->setUIControl_RoseTube();
    }


    RoseTubeCacheViewAll::~RoseTubeCacheViewAll(){

        this->deleteLater();
    }


    void RoseTubeCacheViewAll::setJsonObject_forData(const QJsonObject& jsonObj){

        QString tmpPage = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = true;

        this->page = tmpPage;

        if(this->flag_flow_draw == true){
            this->flag_flow_draw = false;

            this->widget_rosetube->hide();

            if(this->rosetube_draw_cnt > 0){
                for(int i = 0; i < this->rosetube_draw_cnt; i++){
                    this->viewAll_rosetube_track[i]->disconnect();
                    this->flowLayout_rosetube->removeWidget(this->viewAll_rosetube_track[i]);
                }
            }
            GSCommon::clearLayout(this->flowLayout_rosetube);

            this->box_rt_contents->removeWidget(this->widget_rosetube);
            GSCommon::clearLayout(this->box_rt_contents);
        }

        // init
        this->next_offset = 0;
        this->rosetube_total_cnt = 0;
        this->rosetube_draw_cnt = 0;

        this->jsonArr_rosetubeTrack = QJsonArray();
        this->jsonArr_cacheLock = QJsonArray();
        this->jsonObj_CacheLock = QJsonObject();

        // request HTTP API
        this->flagReqMore_rosetube = false;
        this->flag_lastPage_rosetube = false;

        this->flag_rosetube_draw = false;

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

        QJsonObject json;
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_lock";
        network->request(HTTP_CACHING_LOCK,
                         Url,
                         json,
                         true,
                         true);
    }


    void RoseTubeCacheViewAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // layout for items
            this->box_rt_contents = new QVBoxLayout();
            this->box_rt_contents->setSpacing(0);
            this->box_rt_contents->setContentsMargins(0, 0, 0, 0);

            GSCommon::clearLayout(this->box_rt_contents);

            this->widget_rt_contents = new QWidget();
            this->widget_rt_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_rt_contents->setLayout(this->box_rt_contents);

            this->flowLayout_rosetube = new FlowLayout(0, 0, 20);
            this->flowLayout_rosetube->setSizeConstraint(QLayout::SetMinimumSize);

            GSCommon::clearLayout(this->flowLayout_rosetube);

            this->widget_rosetube = new QWidget();
            this->widget_rosetube->setLayout(this->flowLayout_rosetube);
            this->widget_rosetube->setContentsMargins(0, 20, 0, 0);
            this->widget_rosetube->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            this->widget_rosetube->setStyleSheet("background-color:transparent;");


            this->box_rt_contents->addWidget(this->widget_rosetube);
            this->box_contents->addWidget(this->widget_rt_contents);
        }
    }


    void RoseTubeCacheViewAll::setUIControl_RoseTube(){

        this->linker = Linker::getInstance();
        connect(this->linker, SIGNAL(signal_cacheState_change()), SLOT(slot_change_cache_state()));
        connect(this->linker, SIGNAL(signal_cacheList_change()), SLOT(slot_change_cache_list()));

        rosetube::ItemTrack_rosetube *listAlbum = new rosetube::ItemTrack_rosetube(0, 0, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, false);

        this->rosetube_widget_width = listAlbum->get_fixedWidth();
        this->rosetube_widget_margin = listAlbum->get_rightMargin();

        delete listAlbum;

        // layout for items
        this->flowLayout_rosetube = this->get_addUIControl_flowLayout(0, 20);
    }


    void RoseTubeCacheViewAll::setUIControl_appendWidget_cache(){

        if(this->flag_cache_delete_send == true){
            this->flag_cache_delete_send = false;

            int maxCnt = this->flowLayout_rosetube->count();
            int i = 0;
            for(i = 0; i < maxCnt; i++){
                this->viewAll_rosetube_track[i]->hide();
                this->viewAll_rosetube_track[i]->disconnect();
                this->flowLayout_rosetube->removeWidget(this->viewAll_rosetube_track[i]);
            }
            GSCommon::clearLayout(this->flowLayout_rosetube);

            this->box_rt_contents->removeWidget(this->widget_rosetube);
            GSCommon::clearLayout(this->box_rt_contents);

            this->box_contents->removeWidget(this->widget_rt_contents);
            GSCommon::clearLayout(this->box_contents);

            this->box_rt_contents = new QVBoxLayout();
            this->box_rt_contents->setSpacing(0);
            this->box_rt_contents->setContentsMargins(0, 0, 0, 0);

            GSCommon::clearLayout(this->box_rt_contents);

            this->widget_rt_contents = new QWidget();
            this->widget_rt_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_rt_contents->setLayout(this->box_rt_contents);

            this->flowLayout_rosetube = new FlowLayout(0, 0, 20);
            this->flowLayout_rosetube->setSizeConstraint(QLayout::SetMinimumSize);

            GSCommon::clearLayout(this->flowLayout_rosetube);

            this->widget_rosetube = new QWidget();
            this->widget_rosetube->setLayout(this->flowLayout_rosetube);
            this->widget_rosetube->setContentsMargins(0, 20, 0, 0);
            this->widget_rosetube->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            this->widget_rosetube->setStyleSheet("background-color:transparent;");

            this->box_rt_contents->addWidget(this->widget_rosetube);
            this->box_contents->addWidget(this->widget_rt_contents);

            this->rosetube_draw_cnt = 0;
        }

        if(this->jsonArr_rosetubeTrack.size() > 0){

            this->rosetube_total_cnt = this->jsonArr_rosetubeTrack.size();
            //qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << this->rosetube_total_cnt;

            int f_width = this->flowLayout_rosetube->geometry().width();

            if(this->flowLayout_rosetube->geometry().width() <= 0){
                f_width = this->width() - (80 + 63) - 10;
            }

            int f_wg_cnt = f_width / (this->rosetube_widget_width + this->rosetube_widget_margin);
            int f_mod = this->rosetube_draw_cnt % f_wg_cnt;

            if(f_mod == 0){
                this->rosetube_widget_cnt = f_wg_cnt * 10;
            }
            else{
                this->rosetube_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
            }

            int start_index = this->rosetube_draw_cnt;
            int max_cnt = ((this->rosetube_total_cnt - this->rosetube_draw_cnt) > this->rosetube_widget_cnt ) ? this->rosetube_widget_cnt : (this->rosetube_total_cnt - this->rosetube_draw_cnt);
            this->rosetube_draw_cnt += max_cnt;

            for(int i = start_index; i < this->rosetube_draw_cnt; i++){
                this->viewAll_rosetube_track[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___CACHE, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
            }

            for(int i = start_index; i < this->rosetube_draw_cnt; i++){
                QJsonObject tmpCaching = this->jsonArr_rosetubeTrack.at(i).toObject();
                tmpCaching.insert("favorite_view", false);

                QString trackId = ProcJsonEasy::getString(tmpCaching, "id");

                if(this->jsonObj_CacheLock.contains(trackId)){
                    tmpCaching.insert("flag_lock", true);

                    this->jsonArr_cacheLock.append(true);
                }
                else{
                    tmpCaching.insert("flag_lock", false);

                    this->jsonArr_cacheLock.append(false);
                }

                this->viewAll_rosetube_track[i]->setData(tmpCaching);
                QCoreApplication::processEvents();
            }

            for(int i = start_index; i < this->rosetube_draw_cnt; i++){
                connect(this->viewAll_rosetube_track[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTubeCacheViewAll::slot_clickedItemPlaylist);
                this->flowLayout_rosetube->addWidget(this->viewAll_rosetube_track[i]);
            }

            ContentLoadingwaitingMsgHide();

            this->flagReqMore_rosetube = false;
        }
        else{
            if(this->rosetube_draw_cnt <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_rosetube->geometry().width();

                if(this->flowLayout_rosetube->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_rosetube->addWidget(noData_widget);
            }
        }

        this->flag_rosetube_draw = false;
        this->flag_flow_draw = true;
        this->slot_hide_msg();
    }


    void RoseTubeCacheViewAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_rosetube && !this->flag_lastPage_rosetube)
               && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_rosetubeData();
        }

    }


    void RoseTubeCacheViewAll::request_more_rosetubeData(){

        if((!this->flagReqMore_rosetube && !this->flag_lastPage_rosetube) && (this->flag_rosetube_draw == false)){
            this->flagReqMore_rosetube = true;
            this->flag_rosetube_draw = true;

            // next_offset
            if(this->rosetube_draw_cnt == 0){
                this->next_offset = 0;
                this->rosetube_total_cnt = 0;
                this->jsonArr_rosetubeTrack = QJsonArray();
            }
            else{
                this->next_offset++;
            }


            // request HTTP API
            QJsonObject json;
            json.insert("page", this->next_offset);

            NetworkHttp *network = new NetworkHttp();
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/tube_cache_contents_get";
            network->request(HTTP_CACHING_CONTENTS,
                             Url,
                             json,
                             true,
                             true);

            if(this->next_offset == 0){
                this->flag_rosetube_draw = true;
            }
        }
    }


    void RoseTubeCacheViewAll::slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj){

        if(p_id == HTTP_CACHING_LOCK){

            if(p_jsonObj.contains("lock")){
                this->jsonObj_CacheLock = ProcJsonEasy::getJsonObject(p_jsonObj, "lock");
            }

            if(this->flag_cache_lock_send == true){
                this->flag_cache_lock_send = false;
                this->viewAll_rosetube_track[this->cache_lock_index]->setCacheImgShow(this->cache_lock_state);

                this->jsonArr_cacheLock.replace(this->cache_lock_index, this->cache_lock_state);
            }
            else{
                if(this->flag_cache_delete_send == true){
                    this->flagReqMore_rosetube = false;
                    this->flag_lastPage_rosetube = false;

                    this->rosetube_draw_cnt = 0;
                    this->next_offset = 0;

                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                }

                // request HTTP API
                this->request_more_rosetubeData();
            }
        }
        else if(p_id == HTTP_CACHING_CONTENTS){

            if(p_jsonObj.contains("data")){
                QJsonObject dataObj = ProcJsonEasy::getJsonObject(p_jsonObj, "data");
                QJsonArray jsonArr = ProcJsonEasy::getJsonArray(dataObj, "arr");

                this->flag_lastPage_rosetube = ProcJsonEasy::getBool(dataObj, "isLast");

                if(this->flag_cache_delete_send == true){
                    this->jsonArr_rosetubeTrack = QJsonArray();
                    this->jsonArr_rosetubeTrack = jsonArr;
                }
                else{
                    ProcJsonEasy::mergeJsonArray(this->jsonArr_rosetubeTrack, jsonArr);
                }

                this->setUIControl_appendWidget_cache();
            }
        }
        else if(p_id == HTTP_CACHING_LOCK_ITEM){

            if(p_jsonObj.contains("code") && (p_jsonObj.value("code").toString() == "G0000")){

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_lock";
                QJsonObject json;
                network->request(HTTP_CACHING_LOCK,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
        }
        else if(p_id == HTTP_CACHING_LOCK_ROSE){

            if(p_jsonObj.contains("lock")){
                QJsonObject tmpLock = ProcJsonEasy::getJsonObject(p_jsonObj, "lock");

                for(int i = 0; i < this->jsonArr_rosetubeTrack.count(); i++){
                    QJsonObject tmpId = this->jsonArr_rosetubeTrack.at(i).toObject();
                    QString cacheId = ProcJsonEasy::getString(tmpId, "id");

                    if(tmpLock.contains(cacheId) == true){
                        this->jsonArr_cacheLock.replace(i, true);
                        this->viewAll_rosetube_track[i]->setCacheImgShow(true);
                    }
                    else{
                        this->jsonArr_cacheLock.replace(i, false);
                        this->viewAll_rosetube_track[i]->setCacheImgShow(false);
                    }
                }
            }
        }
        else if(p_id == HTTP_CACHING_DELETE_ITEM){

            if(p_jsonObj.contains("code") && (p_jsonObj.value("code").toString() == "G0000")){

                ToastMsg::delay(this,"", tr("delay"), 5500);
                QThread::msleep(5500);

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_lock";
                QJsonObject json;
                network->request(HTTP_CACHING_LOCK,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
        }

        sender()->deleteLater();
    }


    void RoseTubeCacheViewAll::slot_hide_msg(){

        ContentLoadingwaitingMsgHide();
    }


    void RoseTubeCacheViewAll::slot_change_cache_state(){

        QJsonObject json;
        json.insert("page", 0);

        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/tube_cache_lock";
        network->request(HTTP_CACHING_LOCK_ROSE,
                         Url,
                         json,
                         true,
                         true);
    }


    void RoseTubeCacheViewAll::slot_change_cache_list(){

        this->flag_cache_delete_send = true;

        QJsonObject json;
        json.insert("page", 0);

        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/tube_cache_contents_get";
        network->request(HTTP_CACHING_CONTENTS,
                         Url,
                         json,
                         true,
                         true);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RoseTubeCacheViewAll::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___CACHE){

            if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add){

            }
            else{

                if(clickMode == tidal::AbstractItem::ClickMode::AllBox || clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                    // Track 바로 재생 - clicked 항목만 재생
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

                    // needed to get JsonArray rearranged to play on the Rose
                    QJsonArray youtube = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_rosetubeTrack, index, playType);

                    // Rose Play 요청
                    QJsonObject json = QJsonObject();
                    json.insert("youtube", youtube);
                    json.insert("youtubePlayType", curr_clickMode);
                    json.insert("roseToken", global.device.getDeviceRoseToken());

                    NetworkHttp *network = new NetworkHttp();
                    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                    QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/youtubePlay.playlist.add";
                    network->request(HTTP_SET_QUEUE,
                                     Url,
                                     json,
                                     true,
                                     true);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    QJsonObject tmpObj = this->jsonArr_rosetubeTrack.at(index).toObject();

                    OptMorePopup::HeaderData data_header;
                    data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                    data_header.sub_title = ProcJsonEasy::getString(tmpObj, "channelName");
                    data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                    data_header.data_pk = "https://youtu.be/" + ProcJsonEasy::getString(tmpObj, "id");  //j220906 share link
                    data_header.type = "YOUTUBE";      //j220906 share link
                    data_header.cache_lock = this->jsonArr_cacheLock.at(index).toBool();
                    data_header.flagProcStar = false;
                    data_header.isShare = true;      //j220906 share link

                    // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Rosetube_Caching, data_header, index, section);
                }
            }
        }
    }


    void RoseTubeCacheViewAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___CACHE){

            QJsonObject tmpObj = this->jsonArr_rosetubeTrack.at(index).toObject();

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
                // needed to get JsonArray rearranged to play on the Rose
                int playType = this->get_rose_playType(clickMode);
                QJsonArray youtube = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_rosetubeTrack, index, clickMode);

                // Rose Play 요청
                QJsonObject json = QJsonObject();
                json.insert("youtube", youtube);
                json.insert("youtubePlayType", playType);
                json.insert("roseToken", global.device.getDeviceRoseToken());

                NetworkHttp *network = new NetworkHttp();
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/youtubePlay.playlist.add";
                network->request(HTTP_SET_QUEUE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubscribInfo){

                QString channelName = ProcJsonEasy::getString(tmpObj, "channelName");
                QString channelId = ProcJsonEasy::getString(tmpObj, "channelId");

                QJsonObject tmp_data;
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert("title", channelName);
                tmp_data.insert("channel_id", channelId);

                emit this->signal_clickedViewAll(tmp_data);
            }
            else if(clickMode == OptMorePopup::ClickMode::Cache_Lock){

                if(this->flag_cache_lock_send == false){
                    QJsonObject json;
                    json.insert("isLock", true);
                    json.insert("data", tmpObj);

                    NetworkHttp *network = new NetworkHttp();
                    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                    QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/tube_cache_lock_item";
                    network->request(HTTP_CACHING_LOCK_ITEM,
                                     Url,
                                     json,
                                     true,
                                     true);

                    this->cache_lock_index = index;
                    this->cache_lock_state = true;
                    this->flag_cache_lock_send = true;
                }
            }
            else if(clickMode == OptMorePopup::ClickMode::Cache_Unlock){

                if(this->flag_cache_lock_send == false){
                    QJsonObject json;
                    json.insert("isLock", false);
                    json.insert("data", tmpObj);

                    NetworkHttp *network = new NetworkHttp();
                    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                    QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/tube_cache_lock_item";
                    network->request(HTTP_CACHING_LOCK_ITEM,
                                     Url,
                                     json,
                                     true,
                                     true);

                    this->cache_lock_index = index;
                    this->cache_lock_state = false;
                    this->flag_cache_lock_send = true;
                }
            }
            else if(clickMode == OptMorePopup::ClickMode::Delete){

                if(this->flag_cache_delete_send == false){
                    QJsonObject json;
                    json.insert("data", tmpObj);
                    json.insert("isDelete", true);

                    NetworkHttp *network = new NetworkHttp();
                    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                    QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/tube_cache_download_item";
                    network->request(HTTP_CACHING_DELETE_ITEM,
                                     Url,
                                     json,
                                     true,
                                     true);

                    this->flag_cache_delete_send = true;
                }
            }
        }
    }


    /**
     * @brief OptMorePopup::ClickMode 에 따라 Rose로 요청할 playType 정보를 int 로 반환한다.
     * @details Tidal의 음원, 비디오 모두 똑같다. playType 옵션값이.   음악, 비디오 등의 메뉴에서도 똑같은지는 확인해봐야.
     * @param clickMode
     * @return
     */
    int RoseTubeCacheViewAll::get_rose_playType(OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::Play_RightNow_withReorder
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
        ){
            // 바로 재생을 요청하는 타입
            return 15;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
        ){
            // 현재 큐 끝에 추가
            return 13;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
        ){
            // 큐를 비우고 추가(재생)
            return 12;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
        ){
            return 16;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere){
            // 여기부터 재생
            return 17;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty){
            // 큐 비우고 여기부터 재생
            return 18;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last){
            // 여기부터 큐 끝에 추가
            return 19;
        }
        else{
            return 0;           // unknown or not supported
        }
    }


    /**
     * @brief OptMorePopup::ClickMode에 따라서 JsonArray(Track정보)를 재가공해서 반환한다.
     * @param jsonArr_toPlayAll
     * @param curr_index
     * @param clickMode
     * @return
     */
    QJsonArray RoseTubeCacheViewAll::get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow_withReorder
                || clickMode == OptMorePopup::ClickMode::Play_RightNow
        ){
            if(curr_index > 0){
                // 전체이지만 re-ordering
                QJsonArray jsonArr_output = this->reorderJsonArray(jsonArr_toPlayAll, curr_index);
                return jsonArr_output;
            }
            else{
                return jsonArr_toPlayAll;
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
        ){
            // 1곡만
            QJsonArray jsonArr_output;
            jsonArr_output.append(jsonArr_toPlayAll.at(curr_index).toObject());
            return jsonArr_output;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last
        ){
            // index ~ 마지막까지 구성
            if(curr_index > 0){
                QJsonArray jsonArr_output = this->subRange_JsonArray(jsonArr_toPlayAll, curr_index);
                return jsonArr_output;
            }
            else{
                return jsonArr_toPlayAll;
            }
        }
        else{
            // 그 이외의 경우
            return jsonArr_toPlayAll;
        }
    }


    /**
     * @brief AbstractProcRosePlay::subRange_JsonArray
     * @param p_jsonArr
     * @param startIndex
     * @return
     */
    QJsonArray RoseTubeCacheViewAll::subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex){

        const int len_data = p_jsonArr.count();

        if(startIndex>0 && startIndex<len_data){
            QJsonArray jsonArr_subRange;
            for(int i=startIndex ; i<len_data ; i++){
                jsonArr_subRange.append(p_jsonArr.at(i));
            }
            return jsonArr_subRange;
        }
        else{
            // 유효하지 않는 경우, 그대로 반환
            return p_jsonArr;
        }
    }


    /**
     * @brief AbstractProcRosePlay::reorderJsonArray
     * @param p_jsonArr
     * @param startIndex
     * @return
     */
    QJsonArray RoseTubeCacheViewAll::reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex){

        const int len_data = p_jsonArr.count();

        if(startIndex>0 && startIndex<len_data){
            QJsonArray jsonArr_reorder;
            for(int i=startIndex ; i<len_data ; i++){
                jsonArr_reorder.append(p_jsonArr.at(i));
            }
            for(int i=0 ; i<startIndex ; i++){
                jsonArr_reorder.append(p_jsonArr.at(i));
            }
            return jsonArr_reorder;
        }
        else{
            // 유효하지 않는 경우, 그대로 반환
            return p_jsonArr;
        }
    }

    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void RoseTubeCacheViewAll::resizeEvent(QResizeEvent *event){

        Q_UNUSED(event);

        this->setFlowLayoutResize(this, this->flowLayout_rosetube, this->rosetube_widget_width, this->rosetube_widget_margin);
    }
}
