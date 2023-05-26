#include "roseRadio/roseradio_CountryViewAll.h"

#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "data/datauser_forRoseRadio.h"

#include "widget/NoData_Widget.h"


namespace roseRadio {

    const int GET_ITEM_SIZE___ONCE = 30;


    RoseRadioCountryViewAll::RoseRadioCountryViewAll(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_rosetube, parent) {

        this->list_countryInfo = new QList<roseRadio::RoseRadioRegionInfo>();

        this->setUIControl_Country();
    }


    RoseRadioCountryViewAll::~RoseRadioCountryViewAll(){

        this->deleteLater();
    }


    void RoseRadioCountryViewAll::setJsonObject_forData(const QJsonObject& jsonObj){

        QString mainTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        QString api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");
        QString type = ProcJsonEasy::getString(jsonObj, "type");
        int id = ProcJsonEasy::getInt(jsonObj, "genre_id");
        this->flagNeedReload = false;

        if(this->genreId != id || this->title != mainTitle){
            this->flagNeedReload = true;

            this->genreId = id;
            this->listType = type;
            this->title = mainTitle;
            this->apiPath = api_subPath;

            this->label_mainTitle->setText(this->title);

            // init
            this->next_offset = 0;
            this->country_total_cnt = 0;
            this->country_draw_cnt = 0;

            this->list_countryInfo = new QList<roseRadio::RoseRadioRegionInfo>();
            this->list_countryInfo = global.user_forRoseRadio.getRegionInfo();
            this->jsonArr_country = QJsonArray();

            // request HTTP API
            this->flagReqMore_country = false;
            this->flag_lastPage_country = false;

            this->flag_flow_draw = false;
            this->flag_country_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_countryData();
        }
        else{
            ContentLoadingwaitingMsgHide();
            print_debug();
        }
    }


    void RoseRadioCountryViewAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            // layout for items
            this->flowLayout_country = new FlowLayout(0, 85, 0);
            this->flowLayout_country->setSizeConstraint(QLayout::SetMinimumSize);

            GSCommon::clearLayout(this->flowLayout_country);

            this->widget_country = new QWidget();
            this->widget_country->setLayout(this->flowLayout_country);
            this->widget_country->setContentsMargins(40, 6, 0, 0);
            this->widget_country->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            this->widget_country->setStyleSheet("background-color:transparent;");

            this->box_contents->addWidget(this->widget_country);
        }
    }


    void RoseRadioCountryViewAll::setUIControl_Country(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Country view all");

        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    void RoseRadioCountryViewAll::proc_wheelEvent_to_getMoreData(){

        this->request_more_countryData();
    }


    void RoseRadioCountryViewAll::request_more_countryData(){

        if((!this->flagReqMore_country && !this->flag_lastPage_country)
                && ((this->jsonArr_country.size() == this->country_draw_cnt) && (this->flag_country_draw == false))){

            this->flagReqMore_country = true;

            // next_offset
            if(this->country_total_cnt == 0){
                this->next_offset = 0;
                this->jsonArr_country = QJsonArray();
            }
            else{
                this->next_offset++;

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }

            // request HTTP API
            roseHome::ProcCommon *proc_country = new roseHome::ProcCommon(this);
            connect(proc_country, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &RoseRadioCountryViewAll::slot_applyResult_Country);
            proc_country->request_rose_getList_radioCountry(this->next_offset, GET_ITEM_SIZE___ONCE);

            if(this->next_offset == 0){
                this->flag_country_draw = true;
            }
        }
    }


    void RoseRadioCountryViewAll::request_more_countryDraw(){

        int start_index = this->country_draw_cnt;
        int max_cnt = ((this->country_total_cnt - this->country_draw_cnt) > GET_ITEM_SIZE___ONCE ) ? GET_ITEM_SIZE___ONCE : (this->country_total_cnt - this->country_draw_cnt);
        this->country_draw_cnt += max_cnt;

        for(int i = start_index; i < this->country_draw_cnt; i++){

            QJsonObject tmpObj = this->jsonArr_country.at(i).toObject();

            QString countryName = ProcJsonEasy::getString(tmpObj, "name");
            QWidget *country = this->setCountry_pushButton(i, countryName);

            this->flowLayout_country->addWidget(country);

            //QCoreApplication::processEvents();
        }

        if(this->country_total_cnt == this->country_draw_cnt){
            ContentLoadingwaitingMsgHide();
            this->box_contents->addSpacing(60);
        }

        this->flag_country_draw = false;
    }


    QWidget* RoseRadioCountryViewAll::setCountry_pushButton(const int index, const QString name){

        QPushButton *btn_country = new QPushButton();
        btn_country->setFixedSize(735, 69);
        btn_country->setProperty("idx", index);
        btn_country->setCursor(Qt::PointingHandCursor);
        btn_country->setStyleSheet(".QPushButton{ background-color:transparent; } .QPushButton:hover { background-color:#4d4d4d; } ");

        connect(btn_country, SIGNAL(clicked()), this, SLOT(slot_clickedItemCountry()));

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

        QWidget *widget_country = new QWidget();
        widget_country->setFixedSize(735, 70);
        widget_country->setLayout(vbox_country);

        return widget_country;
    }


    void RoseRadioCountryViewAll::slot_applyResult_Country(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage){

        if(jsonArr.size() > 0){
            this->flagReqMore_country = false;
            this->flag_lastPage_country = flag_lastPage;

            if(this->country_total_cnt == 0){
                this->country_total_cnt = totalCount;
                this->jsonArr_country = QJsonArray();
            }

            int start_index = this->jsonArr_country.size();
            if(this->list_countryInfo->size() == this->country_total_cnt){
                this->flag_lastPage_country = true;

                for(int i = 0; i < this->list_countryInfo->size(); i++){
                    QJsonObject tmpObj;
                    tmpObj.insert("name", this->list_countryInfo->at(i).name);
                    tmpObj.insert("id", this->list_countryInfo->at(i).id);
                    tmpObj.insert("sort", this->list_countryInfo->at(i).sort);

                    this->jsonArr_country.append(tmpObj);
                }
            }
            else{
                if(start_index == 0){
                    this->jsonArr_country = jsonArr;
                }
                else{
                    ProcJsonEasy::mergeJsonArray(this->jsonArr_country, jsonArr);
                }
            }

            if(start_index == 0){
                int max_cnt = this->jsonArr_country.size();
                this->country_draw_cnt = max_cnt;

                for(int i = start_index; i < this->country_draw_cnt; i++){

                    QJsonObject tmpObj = this->jsonArr_country.at(i).toObject();

                    QString countryName = ProcJsonEasy::getString(tmpObj, "name");
                    QWidget *country = this->setCountry_pushButton(i, countryName);

                    this->flowLayout_country->addWidget(country);

                    //QCoreApplication::processEvents();
                }

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_country_draw = false;

                if(this->country_total_cnt == this->country_draw_cnt){

                    this->box_contents->addSpacing(60);
                }

                //this->request_more_roseData();
            }
            else{
                if((this->country_total_cnt > this->country_draw_cnt) && (this->jsonArr_country.size() > this->country_draw_cnt) && (this->flag_country_draw == false)){

                    this->flag_country_draw = true;

                    this->request_more_countryDraw();
                }
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 290);

            this->flowLayout_country->addWidget(noData_widget);
        }
    }


    void RoseRadioCountryViewAll::slot_clickedItemCountry(){

        int idx = sender()->property("idx").toInt();

        QJsonObject tmpCountry = this->jsonArr_country.at(idx).toObject();

        roseHome::PageInfo_RadioAllView data_page;
        data_page.pathTitle = ProcJsonEasy::getString(tmpCountry, "name");
        data_page.genre_id = ProcJsonEasy::getInt(tmpCountry, "id");
        data_page.type = "CountryDetail";

        this->movePage_radio_allView(data_page);
    }
}
