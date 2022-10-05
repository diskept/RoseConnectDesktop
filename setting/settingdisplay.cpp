#include "settingdisplay.h"

#include "common/global.h"
#include <common/gscommon.h>
#include <common/networkhttp.h>

#include <QScrollArea>

const int HTTP_DISPLAY_VALUE_GET = 11;
const int HTTP_DISPLAY_VALUE_SET = 12;
/**
 * @brief SettingDisplay::SettingDisplay 생성자
 * @param parent
 */
SettingDisplay::SettingDisplay(QWidget *parent) : QWidget(parent)
{
    this->setUIControl();
}
/**
 * @brief SettingDisplay::setUIControl 기본 UI 세팅
 */
void SettingDisplay::setUIControl()
{
    // 전체
    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("Screen Brightness")));
    //vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("화면밝기")));
    vl_total->addWidget(this->getUIControlOption());

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
}

/**
 * @brief SettingDisplay::getUIControlOption 화면밝기 조절 컨트롤
 */
QWidget* SettingDisplay::getUIControlOption(){

    QLabel *lb_img = GSCommon::getUILabelImg(":/images/setting/icon_b_bright.png");
    lb_volumnNum = new QLabel();
    lb_volumnNum->setStyleSheet("font-size:28px;color:#FFFFFF;font:bold;");
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(0,0,0,0);
    gridLayout->setSpacing(0);
    gridLayout->addWidget(lb_img, 0, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    gridLayout->addWidget(lb_volumnNum, 0, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    QWidget *widget_sun = new QWidget();
    widget_sun->setLayout(gridLayout);
    widget_sun->setFixedSize(173,173);

    this->slider_vol = new QSlider(Qt::Horizontal);
    this->slider_vol->setObjectName("slider_vol");
    this->slider_vol->setTickInterval(1);
    this->slider_vol->setMaximum(15);
    //this->slider_vol->setMaximumWidth(600);

    slider_vol->setCursor(Qt::PointingHandCursor);
    slider_vol->setStyleSheet("QSlider::groove:horizontal { background:#333333;height:3px; } QSlider::handle:horizontal { background:white;width:14px;border-radius:7px;margin:-6px 0px; } QSlider::add-page:horizontal { background:#555555; } QSlider::sub-page:horizontal { background:#CCCCCC; } ");

    QLabel *lb_dark = new QLabel(tr("Dark"));
    QLabel *lb_bright = new QLabel(tr("Bright"));
    lb_dark->setStyleSheet("font-size:18px;color:#FFFFFF;");
    lb_bright->setStyleSheet("font-size:18px;color:#FFFFFF;");
    QHBoxLayout *hl_note = new QHBoxLayout();
    hl_note->setContentsMargins(0,0,0,0);
    hl_note->setSpacing(0);
    hl_note->addWidget(lb_dark);
    hl_note->addStretch(1);
    hl_note->addWidget(lb_bright);
    QVBoxLayout *vl_slider = new QVBoxLayout();
    vl_slider->setContentsMargins(0,0,0,0);
    vl_slider->setSpacing(0);
    vl_slider->setAlignment(Qt::AlignVCenter);
    vl_slider->addWidget(slider_vol);
    vl_slider->addLayout(hl_note);
    QWidget *widget_sliderbox = new QWidget();
    widget_sliderbox->setLayout(vl_slider);

    QHBoxLayout *hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(77,55,77,0);
    hl_total->setSpacing(0);
    hl_total->addWidget(widget_sun);
    hl_total->addSpacing(65);
    hl_total->addWidget(widget_sliderbox);

    QWidget *widget = new QWidget();
    widget->setLayout(hl_total);

    // 커넥트
    connect(slider_vol, SIGNAL(valueChanged(int)) ,this, SLOT(setTextVolumn(int)));

    return widget;
}

/**
 * @brief SettingDisplay::setTextVolumn [SLOT]
 * @param p_value
 */
void SettingDisplay::setTextVolumn(int p_value){
    this->lb_volumnNum->setText(QString("%1").arg(p_value));

    if(global.device.getDeviceIP()!=""){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        // API 볼륨 조절
        QJsonObject json;
        json.insert("brightness",p_value);
        json.insert("roseToken",global.device.getDeviceRoseToken());
        network->request(HTTP_DISPLAY_VALUE_SET, QString("http://%1:%2/display_value_set").arg(global.device.getDeviceIP()).arg(global.port), json, true);
    }
}

/**
 * @brief SettingDisplay::requestGetValueDisplay 현재 Display값을 GET 하는 API 호출한다.
 */
void SettingDisplay::requestGetValueDisplay(){
    if(global.device.getDeviceIP()!=""){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        // API 볼륨 조절
        QJsonObject json;
        json.insert("roseToken",global.device.getDeviceRoseToken());
        network->request(HTTP_DISPLAY_VALUE_GET, QString("http://%1:%2/display_value_get").arg(global.device.getDeviceIP()).arg(global.port), json, true);
    }
}

/**
 * @brief SettingDisplay::responseHttp : 로즈 장치 정보요청 결과 처리
 * @param p_jsonObject QJsonObject&
 */
void SettingDisplay::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    switch(p_id){
    case HTTP_DISPLAY_VALUE_GET :
        if(p_jsonObject.contains("brightness")){
            int tmp_brightness = p_jsonObject["brightness"].toInt();
            this->slider_vol->setValue(tmp_brightness);
            this->lb_volumnNum->setText(QString("%1").arg(tmp_brightness));
        }

        break;
    case HTTP_DISPLAY_VALUE_SET :
        // Nothing..
        break;
    }
   sender()->deleteLater();
}
