#include "settinglib.h"//cheon01
#include "data/data_media_library.h"//cheon01
#include "settingtimer.h"//cheon01
#include "widget/toastmsg.h"//cheon01
#include <common/gscommon.h>
#include "common/rosesettings.h"
#include <QJsonObject>//cheon01
#include <QScrollArea>
#include <QSettings>
#include <QComboBox>

#include "common/global.h"//cheon01
#include <common/networkhttp.h>//cheon01
#include <QDebug>


const int HTTP_TIMER_VALUE_GET = 103;
const int HTTP_TIMER_VALUE_SET = 104;


/**
 * @brief SettingTimer::SettingTimer 생성자
 * @param parent
 */
SettingTimer::SettingTimer(QWidget *parent) : QWidget(parent)
{
    this->setUIControl();
}

/**
 * @brief SettingTimer::setUIControl 기본 UI 세팅
 */
void SettingTimer::setUIControl()
{
    //print_debug(); //cheon01
    lb_timer_res = new QLabel();
    lb_timer_res->setStyleSheet("font-size:17px;color:#666666;font:bold;");
    // 전체

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("Timer Option")));//cheon01

    vl_total->addWidget(this->getUIControlOption(tr("Timer Setting(min)")));//cheon01
    vl_total->addWidget(lb_timer_res );//cheon01


    // 스크롤
    QWidget *widget_scrollArea = new QWidget();
    widget_scrollArea->setContentsMargins(0,0,0,0);
    widget_scrollArea->setObjectName("widget_scrollArea");
    widget_scrollArea->setLayout(vl_total);
    widget_scrollArea->setStyleSheet("#widget_scrollArea { border-left:1px solid #303030; }");

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(widget_scrollArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("#scrollArea { border:0px; }");

    QVBoxLayout *vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);

    this->setLayout(vl_scroll);
    //print_debug(); //cheon01
}


/**
 * @brief SettingTimer::getUIControlOption 콤보박스 포함된 ROW 생성한다
 * @param p_title
 * @return
 */
QWidget* SettingTimer::getUIControlOption(QString p_title){

    //print_debug(); //cheon01
    //qDebug() << "p_title : " <<p_title ;//cheon01
    QLabel *lb_title = new QLabel(p_title);
    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;");

    this->list_keyValue.append(0);
    this->list_keyValue.append(1);
    this->list_keyValue.append(2);
    this->list_keyValue.append(3);
    this->list_keyValue.append(4);
    this->list_keyValue.append(5);
    this->list_keyValue.append(6);
    this->list_keyValue.append(7);
    this->list_keyValue.append(8);
    this->list_keyValue.append(9);

    comboBox_timer = new QComboBox();
    comboBox_timer->setContentsMargins(0,0,0,0);
    comboBox_timer->setObjectName("comboBox_timer");//cheon01
    comboBox_timer->setFixedWidth(340);
    comboBox_timer->addItem(tr("90"), 0);//cheon01_src
    comboBox_timer->addItem(tr("0"), 1);//cheon01_src
    comboBox_timer->addItem(tr("10"), 2);//cheon01_src
    comboBox_timer->addItem(tr("20"), 3);//cheon01_src
    comboBox_timer->addItem(tr("30"), 4);//cheon01_src
    comboBox_timer->addItem(tr("40"), 5);//cheon01_src
    comboBox_timer->addItem(tr("50"), 6);//cheon01_src
    comboBox_timer->addItem(tr("60"), 7);//cheon01_src
    comboBox_timer->addItem(tr("70"), 8);//cheon01_src
    comboBox_timer->addItem(tr("80"), 9);//cheon01_src
    comboBox_timer->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                //"QAbstractItemView QAbstractItemView::item {  padding-left:5px; } "
                                //"QComboBox QAbstractItemView:selected { color:red;selection-background-color:red } "
                                //"QComboBox QAbstractItemView:hover { color:red;selection-background-color:red } "
                                );
    /*
    "QComboBox::item { font-size:18px;color:red; background-color:#333333; }"

    " QListView::item { color:red; } "
    " QListView::item:selected { border: 1px solid #B18658;background-color:#B18658;  }"
    " QListView::item:hover { background-color:#B18658; } "
    "QListView::item:selected:active { background-color:#B18658; } "

    "#comboBox_timer:item { color:#FFFFFF;height:35px; } "
    "#comboBox_timer:item:selected { background-color:#B18658; } "
    "#comboBox_timer:item::hover { background-color:#B18658; } "
    */

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,15,47,15);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_title);
    hl_lb->addWidget(comboBox_timer);

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);

    // 커넥션
    //connect(comboBox, &QComboBox::currentIndexChanged, this, &SettingTimer::slot_changedIndexPod);
    connect(comboBox_timer, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexTimer(int)));

    //print_debug(); //cheon01
    return widget_total;
}


/**
 * @brief SettingDisplay::requestGetValueDisplay 현재 Display값을 SET 하는 API 호출한다.
 */
/**
 * @brief SettingDisplay::setTextVolumn [SLOT]
 * @param p_value
 */
void SettingTimer::slot_changedIndexTimer(int index){

  //  int tmp_httpId = sender()->property("httpId").toInt();  //cheon01

    if(global.device.getDeviceIP()!=""){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //print_debug();//cheon01
        //qDebug() << "index : " << index;//cheon01
        // API 볼륨 조절
        QJsonObject json;
        json.insert("barControl","remote_bar_order.timer");//cheon01
        //json.insert("strValue",comboBox_timer->itemText(index));//cheon01
        //json.insert("value",0);//cheon01
        json.insert("value",index);//cheon01
        json.insert("roseToken",global.device.getDeviceRoseToken());//cheon01

        //print_debug();//cheon01
        //QJsonDocument doc(json);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "slot_changedIndexTimer_json : " << strJson;//cheon01
        network->request(HTTP_TIMER_VALUE_SET, QString("http://%1:%2/remote_bar_order").arg(global.device.getDeviceIP()).arg(global.port), json, true);

        //network->request(tmp_httpId, QString("http://%1:%2/remote_bar_order").arg(global.device.getDeviceIP()).arg(global.port), json, true);

    }
    else {
            ToastMsg::show(this, "", tr("Failed to Set ROSE device IP"));//cheon01

        }
}


void SettingTimer::requestGetValueTimer(){
    //print_debug(); //cheon01
    if(global.device.getDeviceIP()!=""){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        // API Timer RESOLUTION 조절
        QJsonObject json;
        //json.insert("roseToken",global.device.getDeviceRoseToken());
        //qDebug() << "QString("http://%1:%2/sleep.time.get").arg(global.device.getDeviceIP()).arg(global.port)" << QString("http://%1:%2/sleep.time.get").arg(global.device.getDeviceIP()).arg(global.port);
        //QJsonDocument doc(json);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "GET----Timer_p_jsonObject : " << strJson;//cheon01.
        network->request(HTTP_TIMER_VALUE_GET, QString("http://%1:%2/sleep.time.get").arg(global.device.getDeviceIP()).arg(global.port), json, true);
    }else {
        ToastMsg::show(this, "", tr("Failed to get ROSE device IP"));//cheon01

    }
}

/**
 * @brief SettingDisplay::responseHttp : 로즈 장치 정보요청 결과 처리
 * @param p_jsonObject QJsonObject&
 */
void SettingTimer::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    //print_debug();//cheon01
    //qDebug() << "GET----Timer_p_id : " << p_id;//cheon01_src.

    switch(p_id){
    case HTTP_TIMER_VALUE_GET :

        if(p_jsonObject.contains("timer")){//cheon01_src
            int tmp_timer = p_jsonObject["timer"].toInt();//cheon01_src

            //print_debug();//cheon01
            //qDebug() << " tmp_timer : " << tmp_timer ;//cheon01
            if(tmp_timer == 0){
                this->lb_timer_res->setText("       Current Timer :       " + QString(tr("Cancel:0")));
            }
            else {
                this->lb_timer_res->setText("       Current Timer :       " + QString::number(tmp_timer));
            }

            //print_debug();//cheon01
            //QJsonDocument doc(p_jsonObject);
            //QString strJson(doc.toJson(QJsonDocument::Compact));
            //qDebug() << "GET----Timer_p_jsonObject : " << strJson;//cheon01.
        }

        break;
    case HTTP_TIMER_VALUE_SET :
        //print_debug();//cheon01
        //QJsonDocument doc(p_jsonObject);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "SET---Timer_p_jsonObject : " << strJson;//cheon01.
        break;
    }
   sender()->deleteLater();
}

