#include "roseRadio/roseradio_RegionViewAll.h"

#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "data/datauser_forRoseRadio.h"

#include "widget/NoData_Widget.h"

#include <QScrollBar>


namespace roseRadio {

    const int GET_ITEM_SIZE___ONCE = 30;


    RoseRadioRegionViewAll::RoseRadioRegionViewAll(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_rosetube, parent) {

        this->list_region = new QList<roseRadio::RoseRadioRegionInfo>();

        this->setUIControl_Region();
    }


    RoseRadioRegionViewAll::~RoseRadioRegionViewAll(){

        this->deleteLater();
    }


    void RoseRadioRegionViewAll::setJsonObject_forData(const QJsonObject& jsonObj){

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

            // init
            this->next_offset = 0;
            this->region_total_cnt = 0;
            this->region_draw_cnt = 0;

            this->jsonArr_region = QJsonArray();

            // request HTTP API
            this->flagReqMore_region = false;
            this->flag_lastPage_region = false;

            this->flag_flow_draw = false;
            this->flag_region_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_RegionData();
        }
        else{
            ContentLoadingwaitingMsgHide();
        }
    }


    void RoseRadioRegionViewAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            // layout for items
            this->flowLayout_region = new FlowLayout(0, 85, 0);
            this->flowLayout_region->setSizeConstraint(QLayout::SetMinimumSize);

            GSCommon::clearLayout(this->flowLayout_region);

            this->widget_region = new QWidget();
            this->widget_region->setLayout(this->flowLayout_region);
            this->widget_region->setContentsMargins(40, 6, 0, 0);
            this->widget_region->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            this->widget_region->setStyleSheet("background-color:transparent;");

            this->box_contents->addWidget(this->widget_region);
        }
    }


    void RoseRadioRegionViewAll::setUIControl_Region(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Country view all");

        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    void RoseRadioRegionViewAll::proc_wheelEvent_to_getMoreData(){

        if(this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum()){

            this->request_more_RegionData();
        }
    }


    void RoseRadioRegionViewAll::request_more_RegionData(){

        if((!this->flagReqMore_region && !this->flag_lastPage_region)
                && ((this->jsonArr_region.size() == this->region_draw_cnt) && (this->flag_region_draw == false))){

            this->flagReqMore_region = true;

            // next_offset
            if(this->region_total_cnt == 0){
                this->next_offset = 0;
                this->jsonArr_region = QJsonArray();
            }
            else{
                this->next_offset++;

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }

            // request HTTP API
            roseHome::ProcCommon *proc_region = new roseHome::ProcCommon(this);
            connect(proc_region, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &RoseRadioRegionViewAll::slot_applyResult_Region);
            proc_region->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Radio_Region, "/region?", this->regionId, "", this->next_offset, GET_ITEM_SIZE___ONCE);

            if(this->next_offset == 0){
                this->flag_region_draw = true;
            }
        }
    }


    void RoseRadioRegionViewAll::request_more_RegionDraw(){

        int start_index = this->region_draw_cnt;
        int max_cnt = ((this->region_total_cnt - this->region_draw_cnt) > GET_ITEM_SIZE___ONCE ) ? GET_ITEM_SIZE___ONCE : (this->region_total_cnt - this->region_draw_cnt);
        this->region_draw_cnt += max_cnt;

        for(int i = start_index; i < this->region_draw_cnt; i++){

            QJsonObject tmpObj = this->jsonArr_region.at(i).toObject();

            QString regionName = ProcJsonEasy::getString(tmpObj, "name");
            QWidget *region = this->setRegion_pushButton(i, regionName);

            this->flowLayout_region->addWidget(region);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();

        if(this->region_total_cnt == this->region_draw_cnt){
            this->box_contents->addSpacing(60);
        }

        this->flag_region_draw = false;
    }


    QWidget* RoseRadioRegionViewAll::setRegion_pushButton(const int index, const QString name){

        QPushButton *btn_country = new QPushButton();
        btn_country->setFixedSize(735, 69);
        btn_country->setProperty("idx", index);
        btn_country->setCursor(Qt::PointingHandCursor);
        btn_country->setStyleSheet(".QPushButton{ background-color:transparent; } .QPushButton:hover { background-color:#4d4d4d; } ");

        connect(btn_country, SIGNAL(clicked()), this, SLOT(slot_clickedItemRegion()));

        QLabel *countryName = new QLabel(btn_country);
        countryName->setStyleSheet("background-color:transparent; color:#FFFFFF; font-size:18px; line-height:0.89; font-weight:normal;");
        countryName->setGeometry(34, 25, 500, 24);
        countryName->setText(name);

        QWidget *widget_line = new QWidget();
        widget_line->setFixedSize(735, 1);
        widget_line->setStyleSheet("background-color:#4d4d4d;");

        QVBoxLayout *vbox_country = new QVBoxLayout();
        vbox_country->setContentsMargins(0, 0, 0, 0);
        vbox_country->addWidget(btn_country);
        vbox_country->addWidget(widget_line);

        QWidget *widget_region = new QWidget();
        widget_region->setFixedSize(735, 70);
        widget_region->setLayout(vbox_country);

        return widget_region;
    }


    void RoseRadioRegionViewAll::slot_applyResult_Region(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage){

        if(jsonArr.size() > 0){
            this->flagReqMore_region = false;
            this->flag_lastPage_region = flag_lastPage;

            if(this->region_total_cnt == 0){
                this->region_total_cnt = totalCount;
                this->jsonArr_region = QJsonArray();
            }

            int start_index = this->jsonArr_region.size();
            ProcJsonEasy::mergeJsonArray(this->jsonArr_region, jsonArr);

            if(start_index == 0){
                int max_cnt = this->jsonArr_region.size();
                this->region_draw_cnt = max_cnt;

                for(int i = start_index; i < this->region_draw_cnt; i++){

                    QJsonObject tmpObj = this->jsonArr_region.at(i).toObject();

                    QString regionName = ProcJsonEasy::getString(tmpObj, "name");
                    QWidget *region = this->setRegion_pushButton(i, regionName);

                    this->flowLayout_region->addWidget(region);

                    QCoreApplication::processEvents();
                }

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_region_draw = false;

                if(this->region_total_cnt == this->region_draw_cnt){
                    this->box_contents->addSpacing(60);
                }

                //this->request_more_roseData();
            }
            else{
                if((this->region_total_cnt > this->region_draw_cnt) && (this->jsonArr_region.size() > this->region_draw_cnt) && (this->flag_region_draw == false)){

                    this->flag_region_draw = true;

                    this->request_more_RegionDraw();
                }
            }

            ContentLoadingwaitingMsgHide();
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 290);

            this->flowLayout_region->addWidget(noData_widget);
        }
    }


    void RoseRadioRegionViewAll::slot_clickedItemRegion(){

        int idx = sender()->property("idx").toInt();

        QJsonObject tmpCountry = this->jsonArr_region.at(idx).toObject();

        roseHome::PageInfo_RadioAllView data_page;
        data_page.pathTitle = ProcJsonEasy::getString(tmpCountry, "name");
        data_page.genre_id = ProcJsonEasy::getInt(tmpCountry, "id");
        data_page.api_subPath = "/channel?";
        data_page.type = "RegionChannel";

        this->movePage_radio_allView(data_page);
    }
}
