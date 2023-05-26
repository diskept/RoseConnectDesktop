#include "roseRadio/roseradio_LanguageViewAll.h"

#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "widget/NoData_Widget.h"


namespace roseRadio {

    const int GET_ITEM_SIZE___ONCE = 30;


    RoseRadioLanguageViewAll::RoseRadioLanguageViewAll(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_rosetube, parent) {

        this->list_languageInfo = new QList<roseRadio::RoseRadioLanguageInfo>();

        this->setUIControl_Language();
    }


    RoseRadioLanguageViewAll::~RoseRadioLanguageViewAll(){

        this->deleteLater();
    }


    void RoseRadioLanguageViewAll::setJsonObject_forData(const QJsonObject& jsonObj){

        QString mainTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        QString api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");
        QString type = ProcJsonEasy::getString(jsonObj, "type");
        int id = ProcJsonEasy::getInt(jsonObj, "genre_id");
        this->flagNeedReload = false;

        if(this->languageId != id || this->title != mainTitle){
            this->flagNeedReload = true;

            this->languageId = id;
            this->listType = type;
            this->title = mainTitle;
            this->apiPath = api_subPath;

            this->label_mainTitle->setText(this->title);

            // init
            this->next_offset = 0;
            this->language_total_cnt = 0;
            this->language_draw_cnt = 0;

            this->list_languageInfo = new QList<roseRadio::RoseRadioLanguageInfo>();
            this->list_languageInfo = global.user_forRoseRadio.getLanguageInfo();
            this->jsonArr_language = QJsonArray();

            // request HTTP API
            this->flagReqMore_language = false;
            this->flag_lastPage_language = false;

            this->flag_flow_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_LanguageData();
        }
        else{
            ContentLoadingwaitingMsgHide();
            print_debug();
        }
    }


    void RoseRadioLanguageViewAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            // layout for items
            this->flowLayout_language = new FlowLayout(0, 85, 0);
            this->flowLayout_language->setSizeConstraint(QLayout::SetMinimumSize);

            GSCommon::clearLayout(this->flowLayout_language);

            this->widget_language = new QWidget();
            this->widget_language->setLayout(this->flowLayout_language);
            this->widget_language->setContentsMargins(40, 6, 0, 0);
            this->widget_language->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            this->widget_language->setStyleSheet("background-color:transparent;");

            this->box_contents->addWidget(this->widget_language);
        }
    }


    void RoseRadioLanguageViewAll::setUIControl_Language(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Country view all");

        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    void RoseRadioLanguageViewAll::proc_wheelEvent_to_getMoreData(){

        this->request_more_LanguageData();
    }


    void RoseRadioLanguageViewAll::request_more_LanguageData(){

        if((!this->flagReqMore_language && !this->flag_lastPage_language)
                && ((this->jsonArr_language.size() == this->language_draw_cnt) && (this->flag_language_draw == false))){

            this->flagReqMore_language = true;

            // next_offset
            if(this->language_total_cnt == 0){
                this->next_offset = 0;
                this->jsonArr_language = QJsonArray();
            }
            else{
                this->next_offset++;

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }

            // request HTTP API
            roseHome::ProcCommon *proc_language = new roseHome::ProcCommon(this);
            connect(proc_language, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &RoseRadioLanguageViewAll::slot_applyResult_Language);
            proc_language->request_rose_getList_radioLanguage(this->next_offset, GET_ITEM_SIZE___ONCE);

            if(this->next_offset == 0){
                this->flag_language_draw = true;
            }
        }
    }


    void RoseRadioLanguageViewAll::request_more_LanguageDraw(){

        int start_index = this->language_draw_cnt;
        int max_cnt = ((this->language_total_cnt - this->language_draw_cnt) > GET_ITEM_SIZE___ONCE ) ? GET_ITEM_SIZE___ONCE : (this->language_total_cnt - this->language_draw_cnt);
        this->language_draw_cnt += max_cnt;

        for(int i = start_index; i < this->language_draw_cnt; i++){

            QJsonObject tmpObj = this->jsonArr_language.at(i).toObject();

            QString languageName = ProcJsonEasy::getString(tmpObj, "name");
            QWidget *language = this->setLanguage_pushButton(i, languageName);

            this->flowLayout_language->addWidget(language);

            //QCoreApplication::processEvents();
        }


        if(this->language_total_cnt == this->language_draw_cnt){
            ContentLoadingwaitingMsgHide();
            this->box_contents->addSpacing(60);
        }

        this->flag_language_draw = false;
    }


    QWidget* RoseRadioLanguageViewAll::setLanguage_pushButton(const int index, const QString name){

        QPushButton *btn_language = new QPushButton();
        btn_language->setFixedSize(735, 69);
        btn_language->setProperty("idx", index);
        btn_language->setCursor(Qt::PointingHandCursor);
        btn_language->setStyleSheet(".QPushButton{ background-color:transparent; } .QPushButton:hover { background-color:#4d4d4d; } ");

        connect(btn_language, SIGNAL(clicked()), this, SLOT(slot_clickedItemLanguage()));

        QLabel *countryName = new QLabel(btn_language);
        countryName->setStyleSheet("background-color:transparent; color:#FFFFFF; font-size:18px; line-height:0.89; font-weight:normal;");
        countryName->setGeometry(34, 25, 500, 24);
        countryName->setText(name);

        QWidget *widget_line = new QWidget();
        widget_line->setFixedSize(735, 1);
        widget_line->setStyleSheet("background-color:#4d4d4d;");

        QVBoxLayout *vbox_country = new QVBoxLayout();
        vbox_country->setContentsMargins(0, 0, 0, 0);
        vbox_country->addWidget(btn_language);
        vbox_country->addWidget(widget_line);

        QWidget *widget_language = new QWidget();
        widget_language->setFixedSize(735, 70);
        widget_language->setLayout(vbox_country);

        return widget_language;
    }


    void RoseRadioLanguageViewAll::slot_applyResult_Language(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage){

        if(jsonArr.size() > 0){
            this->flagReqMore_language = false;
            this->flag_lastPage_language = flag_lastPage;

            if(this->language_total_cnt == 0){
                this->language_total_cnt = totalCount;
                this->jsonArr_language = QJsonArray();
            }

            int start_index = this->jsonArr_language.size();
            if(this->list_languageInfo->size() == this->language_total_cnt){
                this->flag_lastPage_language = true;

                for(int i = 0; i < this->list_languageInfo->size(); i++){
                    QJsonObject tmpObj;
                    tmpObj.insert("name", this->list_languageInfo->at(i).name);
                    tmpObj.insert("id", this->list_languageInfo->at(i).id);
                    tmpObj.insert("sort", this->list_languageInfo->at(i).sort);

                    this->jsonArr_language.append(tmpObj);
                }
            }
            else{
                if(start_index == 0){
                    this->jsonArr_language = jsonArr;
                }
                else{
                    ProcJsonEasy::mergeJsonArray(this->jsonArr_language, jsonArr);
                }
            }

            if(start_index == 0){
                int max_cnt = this->jsonArr_language.size();
                this->language_draw_cnt = max_cnt;

                for(int i = start_index; i < this->language_draw_cnt; i++){

                    QJsonObject tmpObj = this->jsonArr_language.at(i).toObject();

                    QString languageName = ProcJsonEasy::getString(tmpObj, "name");
                    QWidget *language = this->setLanguage_pushButton(i, languageName);

                    this->flowLayout_language->addWidget(language);

                    //QCoreApplication::processEvents();
                }

                this->flag_flow_draw = true;
                this->flag_language_draw = false;

                if(this->language_total_cnt == this->language_draw_cnt){
                    ContentLoadingwaitingMsgHide();
                    this->box_contents->addSpacing(60);
                }

                //this->request_more_roseData();
            }
            else{
                if((this->language_total_cnt > this->language_draw_cnt) && (this->jsonArr_language.size() > this->language_draw_cnt) && (this->flag_language_draw == false)){

                    this->flag_language_draw = true;

                    this->request_more_LanguageDraw();
                }
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 290);

            this->flowLayout_language->addWidget(noData_widget);
        }
    }


    void RoseRadioLanguageViewAll::slot_clickedItemLanguage(){

        int idx = sender()->property("idx").toInt();

        QJsonObject tmpLanguage = this->jsonArr_language.at(idx).toObject();

        roseHome::PageInfo_RadioAllView data_page;
        data_page.pathTitle = ProcJsonEasy::getString(tmpLanguage, "name");
        data_page.genre_id = ProcJsonEasy::getInt(tmpLanguage, "id");
        data_page.api_subPath = "/channel?";
        data_page.type = "LanguageChannel";

        this->movePage_radio_allView(data_page);
    }
}
