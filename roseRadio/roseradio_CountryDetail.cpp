#include "roseRadio/roseradio_CountryDetail.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/rosesettings.h"

#include "login/dialoglogin.h"

#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"

#include <QDebug>
#include <QScroller>

namespace roseRadio {

    // subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX_SUBTITLE_Region = 0;
    const int BTN_IDX_SUBTITLE_PouplarChannel = 1;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    //const int SECTION_FOR_MORE_POPUP___Region = 0;
    const int SECTION_FOR_MORE_POPUP___PouplarChannel = 1;

    const int HTTP_SET_QUEUE = 0;


    RoseRadioCountryDetail::RoseRadioCountryDetail(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_rosetube, parent) {

        this->label_mainTitle = this->get_addUIControl_mainTitle("Country detail");
    }


    RoseRadioCountryDetail::~RoseRadioCountryDetail(){

        this->deleteLater();
    }


    void RoseRadioCountryDetail::setJsonObject_forData(const QJsonObject& jsonObj){

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
            this->jsonArr_region = QJsonArray();
            this->jsonArr_popular = QJsonArray();

            if(this->flag_region[0] == true && this->flag_popular[0] == true){
                this->widget_region_contents->hide();
                this->widget_popular_contents->hide();
            }

            this->flag_region[0] = false;
            this->flag_region[1] = false;

            this->flag_popular[0] = false;
            this->flag_popular[1] = false;
        }
    }


    /**
     * @brief RoseRadioCountryDetail::setActivePage
     */
    void RoseRadioCountryDetail::setActivePage(){

        if(this->flagNeedReload){

            if(global.user.isValid() == true){

                // 항상 부모클래스의 함수 먼저 호출
                AbstractRoseHomeSubWidget::setActivePage();
                this->flagNeedReload = true;

                this->box_contents->removeWidget(this->widget_region_contents);
                this->box_contents->removeWidget(this->widget_popular_contents);

                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->box_region_contents = new QVBoxLayout();
                this->box_region_contents->setSpacing(0);
                this->box_region_contents->setContentsMargins(70, 30, 60, 60);
                this->box_region_contents->setAlignment(Qt::AlignTop);

                this->widget_region_contents = new QWidget();
                this->widget_region_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_region_contents->setLayout(this->box_region_contents);

                this->box_contents->addWidget(this->widget_region_contents, 0, Qt::AlignTop);

                this->box_popular_contents = new QVBoxLayout();
                this->box_popular_contents->setSpacing(0);
                this->box_popular_contents->setContentsMargins(70, 0, 60, 0);
                this->box_popular_contents->setAlignment(Qt::AlignTop);

                this->widget_popular_contents = new QWidget();
                this->widget_popular_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_popular_contents->setLayout(this->box_popular_contents);

                this->box_contents->addWidget(this->widget_popular_contents, 0, Qt::AlignTop);
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                // init UI
                this->vBox_region = new QVBoxLayout();
                this->vBox_region->setAlignment(Qt::AlignTop);
                GSCommon::clearLayout(this->vBox_region);

                this->flow_region = new FlowLayout(0, 20, 20);
                GSCommon::clearLayout(this->flow_region);

                this->vBox_popular = new QVBoxLayout();
                this->vBox_popular->setAlignment(Qt::AlignTop);
                GSCommon::clearLayout(this->vBox_popular);

                this->flow_popular = new FlowLayout(0, 0, 20);
                GSCommon::clearLayout(this->flow_popular);

                // sub Title UI
                for(int i = 0; i < 14; i++){
                    this->region_popular[i] = new roseRadio::ItemSquare(i, SECTION_FOR_MORE_POPUP___PouplarChannel, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->region_popular[i], &roseRadio::ItemSquare::square_clicked, this, &RoseRadioCountryDetail::slot_clickedItemRoseRadio);
                }

                // request HTTP API
                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

                this->setUIControl_requestRegion();
            }
        }
    }


    void RoseRadioCountryDetail::slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj){

        if(p_id == HTTP_SET_QUEUE){

           if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
           }
        }

        sender()->deleteLater();
    }


    void RoseRadioCountryDetail::setUIControl_requestRegion(){

        roseHome::ProcCommon *proc_region = new roseHome::ProcCommon(this);
        connect(proc_region, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &RoseRadioCountryDetail::slot_applyResult_Region);
        proc_region->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Radio_Region, "/region?", this->regionId, "", 0, 10);

        roseHome::ProcCommon *proc_popular = new roseHome::ProcCommon(this);
        connect(proc_popular, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &RoseRadioCountryDetail::slot_applyResult_Popular);
        proc_popular->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Item_Local, "/channel?", this->regionId, "POPULAR", 0, 14);
    }


    void RoseRadioCountryDetail::setUIControl_appendWidget_region(){

        if(this->flag_region[0] == true && this->flag_popular[0] == true){

            ContentLoadingwaitingMsgHide();

            if(this->flag_region[0] == true){

                this->widget_region = new QWidget();
                this->widget_region->setFixedSize(1550, 30);

                this->lb_subTitle[BTN_IDX_SUBTITLE_Region] = new QLabel(this->widget_region);
                this->lb_subTitle[BTN_IDX_SUBTITLE_Region]->setText(tr("Regions"));
                this->lb_subTitle[BTN_IDX_SUBTITLE_Region]->setStyleSheet("font-size:24px;font-weight:bold;color:#FFFFFF;");
                this->lb_subTitle[BTN_IDX_SUBTITLE_Region]->setGeometry(0, 1, 1000, 28);

                this->btnView_all[BTN_IDX_SUBTITLE_Region] = new QPushButton(this->widget_region);
                this->btnView_all[BTN_IDX_SUBTITLE_Region]->setText("View All");
                this->btnView_all[BTN_IDX_SUBTITLE_Region]->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
                this->btnView_all[BTN_IDX_SUBTITLE_Region]->setProperty("idx", BTN_IDX_SUBTITLE_Region);
                this->btnView_all[BTN_IDX_SUBTITLE_Region]->setCursor(Qt::PointingHandCursor);
                this->btnView_all[BTN_IDX_SUBTITLE_Region]->setGeometry(1435, 3, 100, 24);

                connect(this->btnView_all[BTN_IDX_SUBTITLE_Region], SIGNAL(clicked()), this, SLOT(slot_clickBtn_subTitle_viewAll()));

                this->vBox_region->addWidget(this->widget_region, 0, Qt::AlignTop);
                this->vBox_region->addSpacing(10);

                if(this->flag_region[1] == true){

                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->flow_region->setAlignment(Qt::AlignTop);
                    this->flow_region->setSizeConstraint(QLayout::SetMinimumSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->flow_region);
                    widget_content->setContentsMargins(0, 0, 0, 0);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_region->addWidget(widget_content, 0, Qt::AlignTop);

                    int maxCount = 0;
                    if(this->jsonArr_region.size() > 10){
                        maxCount = 10;
                    }
                    else{
                        maxCount = this->jsonArr_region.size();
                    }

                    for(int i = 0 ; i < maxCount; i++){
                        QJsonObject jsonData = this->jsonArr_region.at(i).toObject();

                        int tmp_id = 0;
                        QString tmp_nm = "";

                        if(jsonData.contains("id")){
                            tmp_id = jsonData["id"].toInt();
                        }

                        if(jsonData.contains("name")){
                            tmp_nm = jsonData["name"].toString();

                            if(tmp_nm.contains("&")){
                                tmp_nm.replace("&", "&&");
                            }
                        }


                        QString btn_style = "QPushButton{ ";
                        btn_style += "padding-left:37px; padding-right:37px; padding-top:10px; padding-bottom:10px;";
                        btn_style += "background:transparent;color:#FFFFFF;font-size:18px;";
                        btn_style += "border:2px solid #494949; border-radius:20px;}";
                        btn_style += "QPushButton:hover{background-color:#BE905F;}";


                        QPushButton *btnRegion = new QPushButton;
                        btnRegion->setText(tmp_nm);
                        btnRegion->setStyleSheet(btn_style);
                        btnRegion->setProperty("id", tmp_id);
                        btnRegion->setProperty("name", tmp_nm);
                        btnRegion->setProperty("title", tmp_nm);
                        btnRegion->setCursor(Qt::PointingHandCursor);

                        connect(btnRegion, SIGNAL(clicked()), this, SLOT(slot_clickedItemRegion()));

                        this->flow_region->addWidget(btnRegion);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Radio_NoData);
                    noData_widget->setFixedSize(1500, 110);

                    this->vBox_region->addWidget(noData_widget);
                }

                this->box_region_contents->addLayout(this->vBox_region);
            }

            if(this->flag_popular[0] == true){

                this->widget_popular = new QWidget();
                this->widget_popular->setFixedSize(1550, 30);

                this->lb_subTitle[BTN_IDX_SUBTITLE_PouplarChannel] = new QLabel(this->widget_popular);
                this->lb_subTitle[BTN_IDX_SUBTITLE_PouplarChannel]->setText(tr("Broadcast stations"));
                this->lb_subTitle[BTN_IDX_SUBTITLE_PouplarChannel]->setStyleSheet("font-size:24px;font-weight:bold;color:#FFFFFF;");
                this->lb_subTitle[BTN_IDX_SUBTITLE_PouplarChannel]->setGeometry(0, 1, 1000, 28);

                this->btnView_all[BTN_IDX_SUBTITLE_PouplarChannel] = new QPushButton(this->widget_popular);
                this->btnView_all[BTN_IDX_SUBTITLE_PouplarChannel]->setText("View All");
                this->btnView_all[BTN_IDX_SUBTITLE_PouplarChannel]->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
                this->btnView_all[BTN_IDX_SUBTITLE_PouplarChannel]->setProperty("idx", BTN_IDX_SUBTITLE_PouplarChannel);
                this->btnView_all[BTN_IDX_SUBTITLE_PouplarChannel]->setCursor(Qt::PointingHandCursor);
                this->btnView_all[BTN_IDX_SUBTITLE_PouplarChannel]->setGeometry(1435, 3, 100, 24);

                connect(this->btnView_all[BTN_IDX_SUBTITLE_PouplarChannel], SIGNAL(clicked()), this, SLOT(slot_clickBtn_subTitle_viewAll()));

                this->vBox_popular->addWidget(this->widget_popular, 0, Qt::AlignTop);
                this->vBox_popular->addSpacing(10);

                if(this->flag_popular[1] == true){

                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->flow_popular->setAlignment(Qt::AlignTop);
                    this->flow_popular->setSizeConstraint(QLayout::SetMinimumSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->flow_popular);
                    widget_content->setContentsMargins(0, 0, 0, 0);
                    widget_content->setFixedHeight(610);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_popular->addWidget(widget_content, 0, Qt::AlignTop);

                    int maxCount = 0;
                    if(this->jsonArr_popular.size() > 14){
                        maxCount = 14;
                    }
                    else{
                        maxCount = this->jsonArr_popular.size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->region_popular[i]->setData(this->jsonArr_popular.at(i).toObject());
                        this->flow_popular->addWidget(this->region_popular[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Radio_NoData);
                    noData_widget->setFixedSize(1500, 610);

                    this->vBox_popular->addWidget(noData_widget);
                }

                this->box_popular_contents->addLayout(this->vBox_popular);
            }
        }
    }


    QWidget* RoseRadioCountryDetail::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

        // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.
        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        tmp_hBox->setSpacing(0);
        tmp_hBox->setContentsMargins(0, 0, 0, 0);
        tmp_hBox->setAlignment(Qt::AlignTop);

        QWidget *widget_box_subTitle = new QWidget();
        widget_box_subTitle->setContentsMargins(0, 0, 0, 0);
        widget_box_subTitle->setLayout(tmp_hBox);

        this->lb_subTitle[btnId]->setText(subTitle);
        this->lb_subTitle[btnId]->setStyleSheet("font-size:24px;font-weight:bold;color:#FFFFFF;");
        tmp_hBox->addWidget(this->lb_subTitle[btnId], 0, Qt::AlignVCenter);

        // side button - it works when the button text is not empty
        if(btnText.isEmpty() == false){
            this->btnView_all[btnId]->setText(btnText);
            this->btnView_all[btnId]->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
            this->btnView_all[btnId]->setProperty("idx", btnId);
            this->btnView_all[btnId]->setCursor(Qt::PointingHandCursor);
            this->btnView_all[btnId]->setContentsMargins(0, 0, 0, 0);
            connect(this->btnView_all[btnId], SIGNAL(clicked()), this, SLOT(slot_clickBtn_subTitle_viewAll()));

            tmp_hBox->addWidget(this->btnView_all[btnId], 0, Qt::AlignVCenter | Qt::AlignRight);
        }

        // Apply Main Layout with spacing
        p_layout->addWidget(widget_box_subTitle);

        return widget_box_subTitle;
    }


    void RoseRadioCountryDetail::slot_applyResult_Region(const QJsonArray &jsonArr, const int totalCount, const bool flag){

        Q_UNUSED(totalCount);
        Q_UNUSED(flag);

        if(jsonArr.size() > 0){
            this->flag_region[1] = true;
            this->jsonArr_region = QJsonArray();
            this->jsonArr_region = jsonArr;
        }

        this->flag_region[0] = true;
        this->setUIControl_appendWidget_region();
    }


    void RoseRadioCountryDetail::slot_applyResult_Popular(const QJsonArray &jsonArr, const int totalCount, const bool flag){

        Q_UNUSED(totalCount);
        Q_UNUSED(flag);

        if(jsonArr.size() > 0){
            this->flag_popular[1] = true;
            this->jsonArr_popular = QJsonArray();
            this->jsonArr_popular = jsonArr;
        }

        this->flag_popular[0] = true;
        this->setUIControl_appendWidget_region();
    }


    void RoseRadioCountryDetail::slot_applyResult_Favorite_state(const QJsonObject &jsonObj){

        if(jsonObj.contains("code") && jsonObj.value("code").toString() == "200"){

            this->region_popular[this->fav_index]->setFavorite(this->fav_flag);
        }
    }


    void RoseRadioCountryDetail::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_Region){

            roseHome::PageInfo_RadioAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = "/region?";
            data_page.genre_id = this->regionId;
            data_page.type = "Region";

            this->movePage_radio_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_PouplarChannel){

            roseHome::PageInfo_RadioAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = "/channel?";
            data_page.genre_id = this->regionId;
            data_page.type = "CountryPopular";

            this->movePage_radio_allView(data_page);
        }
    }


    void RoseRadioCountryDetail::slot_clickedItemRegion(){

        roseHome::PageInfo_RadioAllView data_page;
        data_page.pathTitle = sender()->property("name").toString().replace("&&", "&");
        data_page.genre_id = sender()->property("id").toInt();
        data_page.api_subPath = "/channel?";
        data_page.type = "RegionChannel";

        this->movePage_radio_allView(data_page);
    }


    void RoseRadioCountryDetail::slot_clickedItemRoseRadio(const int section, const int index, const roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio clickMode){

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___PouplarChannel){

            if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::AllBox || clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::PlayBtn){

                QJsonObject channel = this->jsonArr_popular.at(index).toObject();

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

                QJsonObject channel = this->jsonArr_popular.at(index).toObject();

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
                connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &RoseRadioCountryDetail::slot_applyResult_Favorite_state);
                proc_fav->request_rose_setFavorite_Radio(json);

            }
            else if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Delete){

                QJsonObject channel = this->jsonArr_popular.at(index).toObject();

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
                connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &RoseRadioCountryDetail::slot_applyResult_Favorite_state);
                proc_fav->request_rose_setFavorite_Radio(json);

            }
        }
    }
}
