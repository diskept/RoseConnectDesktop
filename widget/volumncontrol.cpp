#include "volumncontrol.h"
#include "common/gscommon.h"
#include "common/global.h"

#include <QVBoxLayout>
#include <QSlider>
#include <QJsonObject>
#include <QEvent>

#include <home/remotebtnitem.h>

#include <common/networkhttp.h>

const int VOLUMN_W = 90;
const int VOLUMN_H = 585;

const int HTTP_REMOTE_VOLUME_GET = 12;
const int HTTP_REMOTE_VOLUME_ADD = 13;
const int HTTP_REMOTE_VOLUME_SUB = 14;


/**
 * @brief VolumnControl::VolumnControl 생성자
 * @param parent
 */
VolumnControl::VolumnControl(QWidget *parent) : QWidget(parent)
{
    this->linker = Linker::getInstance();
    connect(this->linker, SIGNAL(signal_volume_change(int&)), this, SLOT(slot_volume_change(int&)));

    this->timer = new QTimer(this);
    this->timer->setInterval(50);      ///< 1초 1000 timer interval
    connect(this->timer, SIGNAL(timeout()), SLOT(slot_slider_auto_change()));

    this->setFixedSize(VOLUMN_W, VOLUMN_H);
    this->setUIControl();
}


VolumnControl::~VolumnControl(){

    this->deleteLater();
}


/**
 * @brief VolumnControl::setValueVolumn 외부에서 해당 value 의 볼륨을 세팅하고자 하는 경우 사용하는 함수
 * @param p_value
 */
void VolumnControl::setValueVolume(int p_value){

    this->slider_vol->setValue(p_value);
    this->lb_volumnNum->setText(QString::number(p_value));

    emit this->linker->signal_volume_change(p_value);
}


/**
 * @brief VolumnControl::setUIControl UI 생성
 */
void VolumnControl::setUIControl(){

    int left = 0;
    int width = 0;

    QWidget *widget_volumn = new QWidget();
    widget_volumn->setObjectName("widget_volumn");
    widget_volumn->setStyleSheet("#widget_volumn { background-color: #4D4D4D; border-radius:10px; }");
    widget_volumn->setFixedSize(VOLUMN_W, VOLUMN_H);

    QLabel *lb_volume = new QLabel(widget_volumn);
    lb_volume->setStyleSheet("background-color: transparent; font-size: 16px; font-weight: normal; line-height: 1.13;  text-align: center; color: #FFFFFF; ");
    lb_volume->setText(tr("Volume"));

    width = lb_volume->sizeHint().width();
    left = (VOLUMN_W - width) / 2;
    lb_volume->setGeometry(left, 25, width, 18);

    this->lb_volumnNum = new QLabel(widget_volumn);
    this->lb_volumnNum->setAlignment(Qt::AlignCenter);
    this->lb_volumnNum->setStyleSheet("background-color: transparent; font-size: 20px; font-weight: normal; line-height: 1.1;  text-align: center; color: #FFFFFF; ");
    this->lb_volumnNum->setText("0");
    this->lb_volumnNum->setGeometry(33, 53, 24, 22);

    RemoteBtnItem *btn_vol_add = new RemoteBtnItem(widget_volumn);
    btn_vol_add->setData(":/images/vol_add_remote.png", "");
    btn_vol_add->setProperty("httpId", HTTP_REMOTE_VOLUME_ADD);
    btn_vol_add->setCircle(47);
    btn_vol_add->setCursor(Qt::PointingHandCursor);
    btn_vol_add->setGeometry(22, 85, 90, 90);

    connect(btn_vol_add, SIGNAL(clicked()), this, SLOT(slot_volumeBtn_Click()));                    ///< btn_vol_add connect

    RemoteBtnItem *btn_vol_sub = new RemoteBtnItem(widget_volumn);
    btn_vol_sub->setData(":/images/vol_sub_remote.png", "");
    btn_vol_sub->setProperty("httpId", HTTP_REMOTE_VOLUME_SUB);
    btn_vol_sub->setCircle(47);
    btn_vol_sub->setCursor(Qt::PointingHandCursor);
    btn_vol_sub->setGeometry(22, 475, 90, 90);

    connect(btn_vol_sub, SIGNAL(clicked()), this, SLOT(slot_volumeBtn_Click()));                    ///< btn_vol_sub connect

    this->slider_vol = new VolSlider(widget_volumn);
    this->slider_vol->setObjectName("slider_vol");
    this->slider_vol->setMaximum(99);
    this->slider_vol->setFixedHeight(297);
    this->slider_vol->setCursor(Qt::PointingHandCursor);
    this->slider_vol->setStyleSheet(" QSlider::groove:vertical { background: #333333; width: 4px; } "
                                    " QSlider::handle:vertical { background: #FFFFFF; height: 16px; border-radius: 8px; margin: 0 -6px; } "
                                    " QSlider::add-page:vertical { background: #CCCCCC; } "
                                    " QSlider::sub-page:vertical { background: #333333; } ");
    this->slider_vol->setGeometry(37, 178, 16, 297);

    connect(this->slider_vol, SIGNAL(signal_vol_slider_clicked(int)) ,this, SLOT(slot_volumeSlide_change(int)));        ///< this->slider_vol connect

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget_volumn);

    this->setLayout(vl_total);
}


/**
 * @brief VolumnControl::slot_volumeBtn_Click [SLOT]
 */
void VolumnControl::slot_volumeBtn_Click(){

    int value = 0;
    int tmp_httpId = sender()->property("httpId").toInt();

    if(tmp_httpId==HTTP_REMOTE_VOLUME_ADD){
        value = (this->slider_vol->value() + 1);    // this->slider_vol->tickInterval()
        if(value >= 99) {
            value = 99;
        }
    }
    else{
        value = (this->slider_vol->value() - 1);
        if(value <= 0) {
            value = 0;
        }
    }

    this->slider_vol->setValue(value);
    this->complete_chagne_volumeLevel(value);
}


/**
 * @brief VolumnControl::slot_volumeSlide_Click [SLOT]
 */
void VolumnControl::slot_volumeSlide_change(int value){

    if(this->flag_auto_change == false && global.setMuteState == false){
        this->slider_vol->setValue(value);
        this->complete_chagne_volumeLevel(value);
    }
}



void VolumnControl::slot_volume_change(int &p_value){

    global.setVolumeSync = p_value;

    this->lb_volumnNum->setText(QString::number(p_value));

    this->flag_auto_change = true;

    if(this->timer->isActive() == false){
        this->timer->start();
    }
}


void VolumnControl::slot_slider_auto_change(){

    if(global.setVolumeSync != this->slider_vol->value()){

        int compareInt = this->slider_vol->value();
        if(global.setVolumeSync > compareInt){
            compareInt++;
            this->slider_vol->setValue(compareInt);
        }
        else if(global.setVolumeSync < compareInt){
            compareInt--;
            this->slider_vol->setValue(compareInt);
        }
        else{
            this->flag_auto_change = false;
            this->timer->stop();
        }
    }
    else{
        this->flag_auto_change = false;
        this->timer->stop();
    }
}


/**
 * @brief VolumnControl::complete_chagne_volumeLevel
 * @param p_value
 */
void VolumnControl::complete_chagne_volumeLevel(int p_value){

    global.setVolumeSync = p_value;

    this->lb_volumnNum->setText(QString::number(p_value));

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QString Url = QString("http://%1:").arg(global.device.getDeviceIP()) + QString("%1/volume").arg(global.port);
    QJsonObject json;
    json.insert("volumeType", "volume_set");
    json.insert("volumeValue", p_value);
    network->request(HTTP_REMOTE_VOLUME_ADD
                     , Url
                     , json
                     , true);

    emit this->linker->signal_volume_change(p_value);
}


/**
 * @brief VolumnControl::responseHttp : 로즈 장치 정보요청 결과 처리
 * @param p_jsonObject QJsonObject&
 */
void VolumnControl::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    Q_UNUSED(p_jsonObject);

    switch(p_id){
    case HTTP_REMOTE_VOLUME_ADD :
        break;
    case HTTP_REMOTE_VOLUME_SUB :
        break;
    case HTTP_REMOTE_VOLUME_GET :
        if(p_jsonObject.contains("volumeValue")){
            this->setValueVolume(p_jsonObject["volumeValue"].toInt());
        }
        break;
    }

   sender()->deleteLater();
}


/**
 * @brief VolumnControl::showEvent [override] 볼륨정보 가져와서 세팅한다.
 * @param event
 */
void VolumnControl::showEvent(QShowEvent *event){

    Q_UNUSED(event);

    if(global.device.getDeviceIP().isEmpty()==false){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        QJsonObject json;
        json.insert("volumeType", "volume_get");
        network->request(HTTP_REMOTE_VOLUME_GET, QString("http://%1:%2/volume").arg(global.device.getDeviceIP()).arg(global.port), json, true);
    }

    //this->slider_vol->setValue(global.setVolumeSync);
    //this->lb_volumnNum->setText(QString::number(global.setVolumeSync));
}


void VolumnControl::hideEvent(QHideEvent *event){

    Q_UNUSED(event);

    this->flag_auto_change = false;
    this->timer->stop();

    emit this->linker->signal_volume_widget_hide();
}
