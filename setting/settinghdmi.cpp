#include "settinglib.h"//cheon29_src
#include "data/data_media_library.h"//cheon29_src
#include "settinghdmi.h"//cheon29_src
#include "widget/toastmsg.h"//cheon29_src
#include <common/gscommon.h>
#include "common/rosesettings.h"
#include <QJsonObject>//cheon29_src
#include <QScrollArea>
#include <QSettings>
#include <QComboBox>
#include <QDebug>
#include "common/global.h"//cheon29_src
#include <common/networkhttp.h>//cheon29_src


const int HTTP_HDMI_VALUE_GET = 11;
const int HTTP_HDMI_VALUE_SET = 12;
/**
 * @brief SettingHDMI::SettingHDMI 생성자
 * @param parent
 */
SettingHDMI::SettingHDMI(QWidget *parent) : QWidget(parent)
{
    this->setUIControl();
}

/**
 * @brief SettingHDMI::setUIControl 기본 UI 세팅
 */
void SettingHDMI::setUIControl()
{
    //print_debug(); //cheon29_src
    lb_hdmi_res = new QLabel();
    lb_hdmi_res->setStyleSheet("font-size:17px;color:#666666;font:bold;");
    // 전체

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("HDMI Option")));

    vl_total->addWidget(this->getUIControlOption(tr("HDMI Resolution")));//cheon29_src
    vl_total->addWidget(lb_hdmi_res );//cheon29_src


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
    //print_debug(); //cheon29_src
}


/**
 * @brief SettingHDMI::getUIControlOption 콤보박스 포함된 ROW 생성한다
 * @param p_title
 * @return
 */
QWidget* SettingHDMI::getUIControlOption(QString p_title){

    //print_debug(); //cheon29_src
    //qDebug() << "p_title : " <<p_title ;//cheon29_src
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

    comboBox_hdmi = new QComboBox();
    comboBox_hdmi->setContentsMargins(0,0,0,0);
    comboBox_hdmi->setObjectName("comboBox_hdmi");//cheon29_src
    comboBox_hdmi->setFixedWidth(340);
    comboBox_hdmi->addItem(tr("1280x720p50"), 0);//cheon29_src
    comboBox_hdmi->addItem(tr("1280x720p60"), 1);//cheon29_src
    comboBox_hdmi->addItem(tr("1920x1080i50"), 2);//cheon29_src
    comboBox_hdmi->addItem(tr("1920x1080i60"), 3);//cheon29_src
    comboBox_hdmi->addItem(tr("1920x1080p50"), 4);//cheon29_src
    comboBox_hdmi->addItem(tr("1920x1080p60"), 5);//cheon29_src
    comboBox_hdmi->addItem(tr("3840x2160p50"), 6);//cheon29_src
    comboBox_hdmi->addItem(tr("3840x2160p60"), 7);//cheon29_src
    comboBox_hdmi->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
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

    "#comboBox_hdmi:item { color:#FFFFFF;height:35px; } "
    "#comboBox_hdmi:item:selected { background-color:#B18658; } "
    "#comboBox_hdmi:item::hover { background-color:#B18658; } "
    */

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,15,47,15);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_title);
    hl_lb->addWidget(comboBox_hdmi);

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);

    // 커넥션
    //connect(comboBox, &QComboBox::currentIndexChanged, this, &SettingHDMI::slot_changedIndexPod);
    connect(comboBox_hdmi, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexHDMI(int)));

    //print_debug(); //cheon29_src
    return widget_total;
}


/**
 * @brief SettingDisplay::requestGetValueDisplay 현재 Display값을 SET 하는 API 호출한다.
 */
/**
 * @brief SettingDisplay::setTextVolumn [SLOT]
 * @param p_value
 */
void SettingHDMI::slot_changedIndexHDMI(int index){



    if(global.device.getDeviceIP()!=""){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        // API 볼륨 조절
        QJsonObject json;
        json.insert("barControl","remote_bar_order.hdmi");//cheon29_src
        json.insert("strValue",comboBox_hdmi->itemText(index));//cheon29_src
        //json.insert("value","0");//cheon29_src
        json.insert("value",index);//cheon29_src
        json.insert("roseToken",global.device.getDeviceRoseToken());//cheon29_src

        //print_debug();//cheon29_src
        //QJsonDocument doc(json);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "slot_changedIndexHDMI_json : " << strJson;//cheon29_src

        network->request(HTTP_HDMI_VALUE_SET, QString("http://%1:%2/remote_bar_order").arg(global.device.getDeviceIP()).arg(global.port), json, true);
    }
    else {
            ToastMsg::show(this, "", tr("Failed to Set ROSE device IP"));//cheon29_src

        }
}


void SettingHDMI::requestGetValueHDMI(){
    if(global.device.getDeviceIP()!=""){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        // API HDMI RESOLUTION 조절
        QJsonObject json;
        json.insert("roseToken",global.device.getDeviceRoseToken());
        network->request(HTTP_HDMI_VALUE_GET, QString("http://%1:%2/get_control_info").arg(global.device.getDeviceIP()).arg(global.port), json, true);
    }else {
        ToastMsg::show(this, "", tr("Failed to get ROSE device IP"));//cheon29_src

    }
}

/**
 * @brief SettingDisplay::responseHttp : 로즈 장치 정보요청 결과 처리
 * @param p_jsonObject QJsonObject&
 */
void SettingHDMI::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){


    switch(p_id){
    case HTTP_HDMI_VALUE_GET :

        if(p_jsonObject.contains("displayInfo")){
            QString tmp_hdmi = p_jsonObject["displayInfo"].toString();
            //print_debug();//cheon29_src
            qDebug() << " tmp_hdmi : " << tmp_hdmi ;//cheon29_src
            this->lb_hdmi_res->setText("       Current HDMI Resolution  :       " + tmp_hdmi);

            comboBox_hdmi->blockSignals(true);

            if(tmp_hdmi == "1280x720p50"){
                this->comboBox_hdmi->setCurrentIndex(0);
            }
            else if(tmp_hdmi == "1280x720p60"){
                this->comboBox_hdmi->setCurrentIndex(1);
            }
            else if(tmp_hdmi == "1920x1080i50"){
                this->comboBox_hdmi->setCurrentIndex(2);
            }
            else if(tmp_hdmi == "1920x1080i60"){
                this->comboBox_hdmi->setCurrentIndex(3);
            }
            else if(tmp_hdmi == "1920x1080p50"){
                this->comboBox_hdmi->setCurrentIndex(4);
            }
            else if(tmp_hdmi == "1920x1080p60"){
                this->comboBox_hdmi->setCurrentIndex(5);
            }
            else if(tmp_hdmi == "3840x2160p50"){
                this->comboBox_hdmi->setCurrentIndex(6);
            }
            else if(tmp_hdmi == "3840x2160p60"){
                this->comboBox_hdmi->setCurrentIndex(7);
            }

            comboBox_hdmi->blockSignals(false);
            //this->list_keyValue.at
            //this->vl_total->addWidget(this->getUIControlTitleSub(tr("Current HDMI Resolution : "),t6.getHdmi_resolution()) );//cheon29_src
            //this->lb_volumnNum->setText(QString("%1").arg(tmp_brightness));
            //print_debug();//cheon29_src
            //QJsonDocument doc(p_jsonObject);
            //QString strJson(doc.toJson(QJsonDocument::Compact));
            //qDebug() << "GET----p_jsonObject : " << strJson;//cheon29_src.
        }

        break;
    case HTTP_HDMI_VALUE_SET :
        //print_debug();//cheon29_src
        //QJsonDocument doc(p_jsonObject);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "SET---p_jsonObject : " << strJson;//cheon29_src.
        break;
    }
   sender()->deleteLater();
}

