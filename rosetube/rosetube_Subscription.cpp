#include "rosetube/rosetube_Subscription.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "widget/imagelabel.h"
#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"

#include <QScroller>


namespace rosetube {

    const int HTTP_SUBSCRIPTION = 99;
    const int HTTP_NETWORK_SUBSCRIBE_CANCEL = 98;


    /**
     * @brief RoseTube_Subscription::RoseTube_Subscription : 로즈튜브-구독
     * @details 로즈튜브-구독 목록 화면(로그인 필수)
     * @param parent
     */
    RoseTube_Subscription::RoseTube_Subscription(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_filter, parent) {

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_logined()), SLOT(slot_getMyInfo_loginAfter()));
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));
    }


    RoseTube_Subscription::~RoseTube_Subscription(){

        this->deleteLater();
    }


    void RoseTube_Subscription::setActivePage(){

        if(global.user.isValid() == true){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();
            this->flagNeedReload = true;

            this->next_offset = 0;
            this->subscription_total_cnt = 0;

            this->flagReqMore_subscription = false;
            this->flag_lastPage_subscription = false;

            this->flag_subscription_draw = false;

            this->subscription_Arr = QJsonArray();

            this->box_contents->removeWidget(this->widget_rose_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->box_rose_contents = new QVBoxLayout();
            this->box_rose_contents->setSpacing(0);
            this->box_rose_contents->setContentsMargins(0, 0, 0, 0);
            this->box_rose_contents->setAlignment(Qt::AlignTop);

            this->widget_rose_contents = new QWidget();
            this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_rose_contents->setLayout(this->box_rose_contents);

            this->widget_login_contents = setUIControl_LoginBefore();
            this->widget_login_contents->hide();

            this->stackedWidget_Contents = new QStackedWidget();
            this->stackedWidget_Contents->addWidget(this->widget_rose_contents);
            this->stackedWidget_Contents->addWidget(this->widget_login_contents);

            this->box_contents->addWidget(this->stackedWidget_Contents);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            if(!global.user.getAccess_token().isEmpty()){

                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

                this->setUIControl_requestSubscription();
            }

            this->widget_rose_contents->show();
            this->widget_login_contents->hide();
            this->stackedWidget_Contents->setCurrentIndex(0);
        }
        else{
            this->flagNeedReload = true;

            this->box_contents->removeWidget(this->widget_rose_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->box_rose_contents = new QVBoxLayout();
            this->box_rose_contents->setSpacing(0);
            this->box_rose_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_rose_contents = new QWidget();
            this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_rose_contents->setLayout(this->box_rose_contents);

            this->widget_login_contents = setUIControl_LoginBefore();
            this->widget_login_contents->hide();

            this->stackedWidget_Contents = new QStackedWidget();
            this->stackedWidget_Contents->addWidget(this->widget_rose_contents);
            this->stackedWidget_Contents->addWidget(this->widget_login_contents);

            this->box_contents->addWidget(this->stackedWidget_Contents);

            this->widget_rose_contents->hide();
            this->widget_login_contents->show();
            this->stackedWidget_Contents->setCurrentIndex(1);
        }
    }


    QWidget* RoseTube_Subscription::setUIControl_LoginBefore(){

        QWidget *widget_loginBefore = new QWidget();
        widget_loginBefore->setObjectName("widget_loginBefore");
        widget_loginBefore->setFixedHeight(500);
        widget_loginBefore->setStyleSheet("#widget_loginBefore { background:qlineargradient(x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #222222, stop:1 #464646); }");

        int left, top = 0;
        QLabel *lb_text = new QLabel(widget_loginBefore);
        lb_text->setText(tr("You can use various services through Login."));
        lb_text->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:30px;");

        left = (1500 - lb_text->sizeHint().width()) / 2;
        top = (500 - 55 - 30 - lb_text->sizeHint().height()) / 2;
        lb_text->setGeometry(left, top, lb_text->sizeHint().width(), lb_text->sizeHint().height());

        QPushButton *btn_login = new QPushButton(widget_loginBefore);
        btn_login->setText(tr("Log-in"));
        btn_login->setCursor(Qt::PointingHandCursor);
        btn_login->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:20px;border:3px solid #CCCCCC;border-radius:25;");
        btn_login->setFixedSize(230, 55);
        btn_login->setGeometry(635, top + lb_text->sizeHint().height() + 30, 230, 55);


        // 커넥션
        connect(btn_login, &QPushButton::clicked, this, &RoseTube_Subscription::slot_showLoginPage);

        return widget_loginBefore;
    }


    QWidget* RoseTube_Subscription::setUIControl_subscription_list(const int idx, const QString channel_id, const QString imagePath, const QString title, const QString description){

        QWidget *widget_list = new QWidget();
        widget_list->setFixedSize(1500, 70);
        widget_list->setProperty("idx", idx);

        QPushButton *list_body = new QPushButton(widget_list);
        list_body->setFixedSize(1500, 69);
        list_body->setCursor(Qt::PointingHandCursor);
        list_body->setProperty("idx", idx);
        list_body->setProperty("title", title);
        list_body->setProperty("channel_id", channel_id);

        ImageLabel *itemImage = new ImageLabel(list_body);
        itemImage->setType(ImageLabel::Type::IconSmallRound);
        itemImage->setImageUrl(imagePath);
        itemImage->setStyleSheet("background-color:transparent;");
        itemImage->setGeometry(0, 5, 60, 60);

        QLabel *itemTitle = new QLabel(list_body);
        itemTitle->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;");
        itemTitle->setAlignment(Qt::AlignVCenter);
        itemTitle->setGeometry(80, 15, 1000, 20);
        itemTitle->setText(title);

        QLabel *itemDescription = new QLabel(list_body);
        itemDescription->setStyleSheet("background-color:transparent;color:#999999;font-size:14px;");
        itemDescription->setAlignment(Qt::AlignVCenter);
        itemDescription->setGeometry(80, 38, 1000, 20);

        QString tmpDescription = GSCommon::getTextCutFromLabelWidth(description, 1000, itemDescription->font());
        QString setDescription = QString("<html><head/><body><span style='font-size:14px; color:#999999;'>%1</span></body></html>").arg(tmpDescription);
        itemDescription->setText(setDescription);

        QPushButton *itemSubs = GSCommon::getUIBtnImg("itemSubs", ":/images/tube_sub_ico.png", list_body);
        itemSubs->setProperty("channel_idx", idx);
        itemSubs->setProperty("channel_id", channel_id);
        itemSubs->setProperty("channel_name", title);
        itemSubs->setGeometry(1440, 0, 60, 70);

        QWidget *widgetLine = new QWidget(widget_list);
        widgetLine->setStyleSheet("background-color:transparent;border:1px solid #333333;");
        widgetLine->setGeometry(0, 69, 1500, 1);

        connect(list_body, SIGNAL(clicked()), SLOT(slot_clickSubsChannel()));
        connect(itemSubs, SIGNAL(clicked()), SLOT(slot_clickSubscription()));

        return widget_list;
    }


    void RoseTube_Subscription::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_subscription && !this->flag_lastPage_subscription) && (this->subscription_total_cnt > this->subscription_draw_cnt) && (this->flag_subscription_draw == false)){

            this->flag_subscription_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->setUIControl_drawSubscription();
        }
    }


    void RoseTube_Subscription::setUIControl_drawSubscription(){

        int startCnt = this->subscription_draw_cnt;
        int maxCnt = ((this->subscription_total_cnt - this->subscription_draw_cnt) > 20 ) ? 20 : (this->subscription_total_cnt - this->subscription_draw_cnt);
        this->subscription_draw_cnt = maxCnt;

        for(int i = startCnt; i < maxCnt; i++){

            QJsonObject tmpObj = this->subscription_Arr.at(i).toObject();

            QJsonObject snippet = ProcJsonEasy::getJsonObject(tmpObj, "snippet");
            QJsonObject tmpImage = ProcJsonEasy::getJsonObject(snippet, "high");

            QString key = ProcJsonEasy::getString(tmpObj, "key");
            QString title = ProcJsonEasy::getString(tmpObj, "title");
            QString description = ProcJsonEasy::getString(tmpObj, "description");
            QString imagePath = ProcJsonEasy::getString(tmpImage, "url");

            this->widget_subscription[i] = this->setUIControl_subscription_list(i, key, imagePath, title, description);

            this->box_rose_contents->addWidget(this->widget_subscription[i]);
        }

        this->flag_subscription_draw = false;
        this->slot_hide_msg();
    }


    void RoseTube_Subscription::setUIControl_requestSubscription(){

        if(!this->flagReqMore_subscription && !this->flag_lastPage_subscription){
            this->flagReqMore_subscription = true;

            // next_offset
            if(this->subscription_total_cnt == 0){
                this->next_offset = 0;
            }
            else{
                this->next_offset++;
            }

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QString url = global.legacy_v1 + QString("/member/youtube/channel?page=%1&size=20").arg(this->next_offset);
            QJsonObject json;

            network->request(HTTP_SUBSCRIPTION
                             , url
                             , json
                             , false
                             , true);
        }
    }


    void RoseTube_Subscription::setUIControl_requestSetSubscription(){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + QString("/member/youtube/channel?channelId=%1").arg(this->cancle_channel_id);
        QJsonObject json;

        network->requestDelete(HTTP_NETWORK_SUBSCRIBE_CANCEL
                               , url
                               , json
                               , true);

    }


    void RoseTube_Subscription::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << "p_jsonObj = " << strJson;*/

        if(p_id == HTTP_SUBSCRIPTION){
            slot_applyResult_subscription(p_jsonObj);
        }
        else if(p_id == HTTP_NETWORK_SUBSCRIBE_CANCEL){
            if(p_jsonObj.contains("flagOk") && p_jsonObj["flagOk"].toBool()){
                this->setActivePage();
            }
        }

        sender()->deleteLater();
    }


    void RoseTube_Subscription::slot_applyResult_subscription(const QJsonObject &jsonObj){

        const QString jsonKey_flagOk = "flagOk";
        const QString jsonKey_channel = "channel";

        if(jsonObj.contains(jsonKey_flagOk) && jsonObj[jsonKey_flagOk].toBool()){
            if(jsonObj.contains(jsonKey_channel)){

                this->flagReqMore_subscription = false;
                this->flag_lastPage_subscription = ProcJsonEasy::getBool(jsonObj, "last");

                if(this->subscription_total_cnt == 0){
                    this->subscription_total_cnt = ProcJsonEasy::getInt(jsonObj, "totalCount");
                    this->subscription_draw_cnt = 0;
                    this->subscription_Arr = QJsonArray();

                    this->flag_subscription_draw = false;
                }

                QJsonArray tmpChannel = ProcJsonEasy::getJsonArray(jsonObj, jsonKey_channel);

                if(tmpChannel.size() > 0){

                    if(this->subscription_Arr.size() == 0){

                        ProcJsonEasy::mergeJsonArray(this->subscription_Arr, tmpChannel);
                        int maxCnt = this->subscription_Arr.size();
                        this->subscription_draw_cnt = maxCnt;

                        for(int i = 0; i < maxCnt; i++){

                            QJsonObject tmpObj = tmpChannel.at(i).toObject();

                            QJsonObject snippet = ProcJsonEasy::getJsonObject(tmpObj, "snippet");
                            QJsonObject tmpImage = ProcJsonEasy::getJsonObject(snippet, "high");

                            QString key = ProcJsonEasy::getString(tmpObj, "key");
                            QString title = ProcJsonEasy::getString(tmpObj, "title");
                            QString description = ProcJsonEasy::getString(tmpObj, "description");
                            QString imagePath = ProcJsonEasy::getString(tmpImage, "url");

                            this->widget_subscription[i] = this->setUIControl_subscription_list(i, key, imagePath, title, description);

                            this->box_rose_contents->addWidget(this->widget_subscription[i]);
                        }

                        this->slot_hide_msg();
                    }
                    else{

                        ProcJsonEasy::mergeJsonArray(this->subscription_Arr, tmpChannel);
                    }

                    this->setUIControl_requestSubscription();
                }
                else{
                    this->slot_hide_msg();

                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Subscription_NoData);
                    noData_widget->setFixedSize(1500, 500);

                    this->box_rose_contents->addWidget(noData_widget);
                }
            }
            else{
                this->slot_hide_msg();

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Subscription_NoData);
                noData_widget->setFixedSize(1500, 500);

                this->box_rose_contents->addWidget(noData_widget);
            }
        }
        else{
            this->slot_hide_msg();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Subscription_NoData);
            noData_widget->setFixedSize(1500, 500);

            this->box_rose_contents->addWidget(noData_widget);
        }
    }


    void RoseTube_Subscription::slot_clickSubscription(){

        //QString title = sender()->property("channel_name").toString();
        this->cancle_channel_id = sender()->property("channel_id").toString();

        this->dlgSubscribe = new DialogConfirm(this);
        this->dlgSubscribe->setTitle(tr("SUBSCRIBE"));
        this->dlgSubscribe->setText(tr("Are you sure you want to unsubscribe from select channel?"));

        int result = this->dlgSubscribe->exec();

        if(result == 1){
            this->setUIControl_requestSetSubscription();
        }
    }


    void RoseTube_Subscription::slot_clickSubsChannel(){

        QJsonObject tmp_data;
        tmp_data.insert("viewAllMode", PAGECODE_SUB_CHANNEL);
        tmp_data.insert("title", sender()->property("title").toString());
        tmp_data.insert("channel_id", sender()->property("channel_id").toString());

        emit this->signal_clickedChannel(tmp_data);
    }


    void RoseTube_Subscription::slot_hide_msg(){

        ContentLoadingwaitingMsgHide();
    }


    void RoseTube_Subscription::slot_showLoginPage(){

        this->dlg_rose_login = new Dialog::DialogLogin(this);
        this->dlg_rose_login->exec();
    }


    void RoseTube_Subscription::slot_getMyInfo_loginAfter(){

        if(global.user.isValid() == true && this->stackedWidget_Contents->currentIndex() == 1){

            this->flagNeedReload = false;
            this->setActivePage();
        }
    }


    void RoseTube_Subscription::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_ROSE)){

            global.user.setDeviceChange(false);

            this->flagNeedReload = false;
            this->setActivePage();
        }
    }
}
