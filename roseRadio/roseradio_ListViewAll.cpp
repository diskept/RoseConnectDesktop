#include "roseRadio/roseradio_ListViewAll.h"

#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "widget/NoData_Widget.h"

#include <QScrollBar>


namespace roseRadio {

    const int HTTP_SET_QUEUE = 0;

    const int SECTION_FOR_MORE_POPUP___CHANNEL = 0;

    const int GET_ITEM_SIZE___ONCE = 30;


    RoseRadioListViewAll::RoseRadioListViewAll(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseradio, parent) {

        this->setUIControl_RoseRadio();
    }


    RoseRadioListViewAll::~RoseRadioListViewAll(){

        this->deleteLater();
    }


    void RoseRadioListViewAll::setJsonObject_forData(const QJsonObject& jsonObj){

        QString mainTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        QString api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");
        QString type = ProcJsonEasy::getString(jsonObj, "type");
        int id = ProcJsonEasy::getInt(jsonObj, "genre_id");
        this->flagNeedReload = false;

        if(this->regionId != id || this->title != mainTitle){
            this->flagNeedReload = true;

            this->regionId = id;
            this->listType = type;
            this->title = mainTitle;
            this->apiPath = api_subPath;

            this->label_mainTitle->setText(this->title);

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;

                if(this->roseRadio_draw_cnt > 0){
                    for(int i = 0; i < this->roseRadio_draw_cnt; i++){
                        this->viewAll_roseRadio[i]->hide();
                        this->viewAll_roseRadio[i]->disconnect();
                        this->flowLayout_roseRadio->removeWidget(this->viewAll_roseRadio[i]);
                    }
                }
                GSCommon::clearLayout(this->flowLayout_roseRadio);

                this->box_contents->removeWidget(this->widget_roseRadio);
                GSCommon::clearLayout(this->box_contents);
            }

            // init
            this->next_offset = 0;
            this->roseRadio_total_cnt = 0;
            this->roseRadio_draw_cnt = 0;

            this->jsonArr_roseRadio = QJsonArray();

            // request HTTP API
            this->flagReqMore_roseRadio = false;
            this->flag_lastPage_roseRadio = false;

            this->flag_flow_draw = false;
            this->flag_roseRadio_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_roseData();
        }
        else{
            ContentLoadingwaitingMsgHide();
        }
    }


    void RoseRadioListViewAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            // layout for items
            this->flowLayout_roseRadio = new FlowLayout(0, 0, 20);
            this->flowLayout_roseRadio->setSizeConstraint(QLayout::SetMinimumSize);

            GSCommon::clearLayout(this->flowLayout_roseRadio);

            this->widget_roseRadio = new QWidget();
            this->widget_roseRadio->setLayout(this->flowLayout_roseRadio);
            this->widget_roseRadio->setContentsMargins(0, 20, 0, 0);
            this->widget_roseRadio->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            this->widget_roseRadio->setStyleSheet("background-color:transparent;");

            this->box_contents->addWidget(this->widget_roseRadio);
        }
    }


    void RoseRadioListViewAll::setUIControl_RoseRadio(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("List view all");

        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    void RoseRadioListViewAll::proc_wheelEvent_to_getMoreData(){

        if(this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum()){

            this->request_more_roseData();
        }
    }


    void RoseRadioListViewAll::request_more_roseData(){

        if(!this->flagReqMore_roseRadio && !this->flag_lastPage_roseRadio
                && ((this->jsonArr_roseRadio.size() == this->roseRadio_draw_cnt) && (this->flag_roseRadio_draw == false))){
            this->flagReqMore_roseRadio = true;

            // next_offset
            if(this->roseRadio_total_cnt == 0){
                this->next_offset = 0;
                this->jsonArr_roseRadio = QJsonArray();
            }
            else{
                this->next_offset++;
            }

            // request HTTP API
            roseHome::ProcCommon *proc_channel = new roseHome::ProcCommon(this);
            connect(proc_channel, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &RoseRadioListViewAll::slot_applyResult_Channel);

            if(this->listType == "FeaturedChannel"){
                proc_channel->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_FeaturedChannel, this->apiPath, 0, "", this->next_offset, GET_ITEM_SIZE___ONCE);
            }
            else if(this->listType == "RecentlyChannel"){
                proc_channel->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Recently_Radio, this->apiPath, 0, "", this->next_offset, GET_ITEM_SIZE___ONCE);
            }
            else if(this->listType == "FavoriteChannnel"){
                proc_channel->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Favorite_Radio, this->apiPath, 0, "", this->next_offset, GET_ITEM_SIZE___ONCE);
            }
            else if(this->listType == "LocalPouplarChannel"){
                proc_channel->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Item_LocalPopular, this->apiPath, 0, "", this->next_offset, GET_ITEM_SIZE___ONCE);
            }
            else if(this->listType == "LocalChannel"){
                proc_channel->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Item_Local, this->apiPath, 0, "", this->next_offset, GET_ITEM_SIZE___ONCE);
            }
            else if(this->listType == "CountryPopular"){
                //proc_channel->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Item_Local, this->apiPath, this->regionId, "POPULAR", this->next_offset, GET_ITEM_SIZE___ONCE);
                proc_channel->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Item_Local, this->apiPath, this->regionId, "NAME_ASC", this->next_offset, GET_ITEM_SIZE___ONCE);
            }
            else if(this->listType == "RegionChannel"){
                //proc_channel->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Item_Local, this->apiPath, this->regionId, "", this->next_offset, GET_ITEM_SIZE___ONCE);
                proc_channel->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Item_Local, this->apiPath, this->regionId, "NAME_ASC", this->next_offset, GET_ITEM_SIZE___ONCE);
            }
            else if(this->listType == "LanguageChannel"){
                //proc_channel->request_rose_getList_languageChannel(this->apiPath, this->regionId, this->next_offset, GET_ITEM_SIZE___ONCE, "POPULAR");
                proc_channel->request_rose_getList_languageChannel(this->apiPath, this->regionId, this->next_offset, GET_ITEM_SIZE___ONCE, "NAME_ASC");
            }

            if(this->next_offset == 0){
                this->flag_roseRadio_draw = true;
            }
        }
    }


    void RoseRadioListViewAll::request_more_roseDraw(){

        int start_index = this->roseRadio_draw_cnt;
        int max_cnt = ((this->roseRadio_total_cnt - this->roseRadio_draw_cnt) > GET_ITEM_SIZE___ONCE ) ? GET_ITEM_SIZE___ONCE : (this->roseRadio_total_cnt - this->roseRadio_draw_cnt);
        this->roseRadio_draw_cnt += max_cnt;

        for(int i = start_index; i < this->roseRadio_draw_cnt; i++){
            this->viewAll_roseRadio[i] = new roseRadio::ItemRectangle(i, SECTION_FOR_MORE_POPUP___CHANNEL, tidal::AbstractItem::ImageSizeMode::Ractangle_480x120);
            connect(this->viewAll_roseRadio[i], &roseRadio::ItemRectangle::rectangle_clicked, this, &RoseRadioListViewAll::slot_clickedItemRoseRadio);
        }

        ContentLoadingwaitingMsgHide();

        for(int i = start_index; i < this->roseRadio_draw_cnt; i++){
            this->viewAll_roseRadio[i]->setData(this->jsonArr_roseRadio.at(i).toObject());

            this->flowLayout_roseRadio->addWidget(this->viewAll_roseRadio[i]);

            QCoreApplication::processEvents();
        }

        if(this->roseRadio_total_cnt == this->roseRadio_draw_cnt){
            this->box_contents->addSpacing(60);
        }

        this->flag_roseRadio_draw = false;
    }


    void RoseRadioListViewAll::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << "p_jsonObj = " << strJson;*/

        if(p_id == HTTP_SET_QUEUE){

           if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
           }
        }

        sender()->deleteLater();
    }


    void RoseRadioListViewAll::slot_applyResult_Channel(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage){

        if(jsonArr.size() > 0){
            this->flagReqMore_roseRadio = false;
            this->flag_lastPage_roseRadio = flag_lastPage;

            if(this->roseRadio_total_cnt == 0){
                this->roseRadio_total_cnt = totalCount;
                this->jsonArr_roseRadio = QJsonArray();
            }

            int start_index = this->jsonArr_roseRadio.size();
            ProcJsonEasy::mergeJsonArray(this->jsonArr_roseRadio, jsonArr);

            if(start_index == 0){
                int max_cnt = this->jsonArr_roseRadio.size();
                this->roseRadio_draw_cnt = max_cnt;

                for(int i = start_index; i < this->roseRadio_draw_cnt; i++){
                    this->viewAll_roseRadio[i] = new roseRadio::ItemRectangle(i, SECTION_FOR_MORE_POPUP___CHANNEL, tidal::AbstractItem::ImageSizeMode::Ractangle_480x120);
                    connect(this->viewAll_roseRadio[i], &roseRadio::ItemRectangle::rectangle_clicked, this, &RoseRadioListViewAll::slot_clickedItemRoseRadio);
                }

                ContentLoadingwaitingMsgHide();

                for(int i = start_index; i < this->roseRadio_draw_cnt; i++){
                    this->viewAll_roseRadio[i]->setData(this->jsonArr_roseRadio.at(i).toObject());

                    this->flowLayout_roseRadio->addWidget(this->viewAll_roseRadio[i]);

                    QCoreApplication::processEvents();
                }

                this->flag_flow_draw = true;
                this->flag_roseRadio_draw = false;

                if(this->roseRadio_total_cnt == this->roseRadio_draw_cnt){
                    this->box_contents->addSpacing(60);
                }

                //this->request_more_roseData();
            }
            else{
                if((this->roseRadio_total_cnt > this->roseRadio_draw_cnt) && (this->jsonArr_roseRadio.size() > this->roseRadio_draw_cnt) && (this->flag_roseRadio_draw == false)){

                    this->flag_roseRadio_draw = true;

                    this->request_more_roseDraw();
                }
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 290);

            this->flowLayout_roseRadio->addWidget(noData_widget);
        }
    }


    void RoseRadioListViewAll::slot_applyResult_Favorite_state(const QJsonObject &jsonObj){

        if(jsonObj.contains("code") && jsonObj.value("code").toString() == "200"){

            this->viewAll_roseRadio[this->fav_index]->setFavorite(this->fav_flag);
        }
    }


    void RoseRadioListViewAll::slot_clickedItemRoseRadio(const int section, const int index, const roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio clickMode){

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___CHANNEL){

            if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::AllBox || clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::PlayBtn){

                QJsonObject channel = this->jsonArr_roseRadio.at(index).toObject();

                QJsonArray data;
                data.append(channel);

                // Rose Play 요청
                QJsonObject json = QJsonObject();
                json.insert("data", data);
                json.insert("currentPosition", 0);
                json.insert("roseToken", global.device.getDeviceRoseToken());

                NetworkHttp *network = new NetworkHttp();
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/rose_radio_play";
                network->request(HTTP_SET_QUEUE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
            else if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Add){

                QJsonObject channel = this->jsonArr_roseRadio.at(index).toObject();

                this->fav_section = section;
                this->fav_index = index;
                this->fav_id = ProcJsonEasy::getInt(channel, "id");
                this->fav_flag = true;

                QJsonObject radioChannel;
                radioChannel.insert("favorite", this->fav_flag);
                radioChannel.insert("id", this->fav_id);

                QJsonObject json;
                json.insert("radioChannel", radioChannel);

                roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
                connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &RoseRadioListViewAll::slot_applyResult_Favorite_state);
                proc_fav->request_rose_setFavorite_Radio(json);

            }
            else if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Delete){

                QJsonObject channel = this->jsonArr_roseRadio.at(index).toObject();

                this->fav_section = section;
                this->fav_index = index;
                this->fav_id = ProcJsonEasy::getInt(channel, "id");
                this->fav_flag = false;

                QJsonObject radioChannel;
                radioChannel.insert("favorite", this->fav_flag);
                radioChannel.insert("id", this->fav_id);

                QJsonObject json;
                json.insert("radioChannel", radioChannel);

                roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
                connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &RoseRadioListViewAll::slot_applyResult_Favorite_state);
                proc_fav->request_rose_setFavorite_Radio(json);

            }
        }
    }
}
