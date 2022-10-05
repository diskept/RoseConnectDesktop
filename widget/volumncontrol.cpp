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
const int VOLUMN_H = 556;
const int HTTP_REMOTE_VOLUME_GET = 12;
const int HTTP_REMOTE_VOLUME_ADD = 13;
const int HTTP_REMOTE_VOLUME_SUB = 14;

/**
 * @brief VolumnControl::VolumnControl 생성자
 * @param parent
 */
VolumnControl::VolumnControl(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(VOLUMN_W, VOLUMN_H);
    this->setUIControl();
}


/**
 * @brief VolumnControl::setUIControl UI 생성
 */
void VolumnControl::setUIControl(){

    QLabel *lb_volumn = new QLabel(tr("Volume"));
    //QLabel *lb_volumn = new QLabel(tr("음량"));
    lb_volumn->setAlignment(Qt::AlignCenter);
    lb_volumn->setStyleSheet("color:white;font-size:16px;");
    this->lb_volumnNum = new QLabel("0");
    lb_volumnNum->setAlignment(Qt::AlignCenter);
    lb_volumnNum->setStyleSheet("color:white;font-size:20px;font:bold;margin-top:10px;");
    RemoteBtnItem *btn_vol_add = new RemoteBtnItem();
    btn_vol_add->setData(":/images/vol_add_remote.png", "");
    btn_vol_add->setProperty("httpId",HTTP_REMOTE_VOLUME_ADD);
    btn_vol_add->setCircle(47);
    btn_vol_add->setCursor(Qt::PointingHandCursor);
    RemoteBtnItem *btn_vol_sub = new RemoteBtnItem();
    btn_vol_sub->setData(":/images/vol_sub_remote.png", "");
    btn_vol_sub->setProperty("httpId",HTTP_REMOTE_VOLUME_SUB);
    btn_vol_sub->setCircle(47);
    btn_vol_sub->setCursor(Qt::PointingHandCursor);
    this->slider_vol = new QSlider();
    this->slider_vol->setObjectName("slider_vol");
    this->slider_vol->setPageStep(1);
    this->slider_vol->setMaximum(99);
    slider_vol->setFixedHeight(268);
    slider_vol->setCursor(Qt::PointingHandCursor);
    slider_vol->setStyleSheet("QSlider::groove:vertical { background:#333333;width:4px; } QSlider::handle:vertical { background:white;height:16px;border-radius:8px;margin:0 -6px; } QSlider::add-page:vertical { background:#CCCCCC; } QSlider::sub-page:vertical { background:#333333; } ");

    QVBoxLayout *vl_sound = new QVBoxLayout();
    vl_sound->setContentsMargins(0,35,0,0);
    vl_sound->setSpacing(0);
    vl_sound->addWidget(lb_volumn, 0, Qt::AlignTop);
    vl_sound->addWidget(lb_volumnNum, 0, Qt::AlignTop);
    vl_sound->addWidget(btn_vol_add, 1, Qt::AlignCenter);
    vl_sound->addWidget(slider_vol, 1, Qt::AlignCenter);
    vl_sound->addWidget(btn_vol_sub, 1, Qt::AlignCenter);

    QWidget *widget_volumn = new QWidget();
    widget_volumn->setObjectName("widget_volumn");
    widget_volumn->setStyleSheet("#widget_volumn { background-color:#4D4D4D;border-radius:10px; }");
    widget_volumn->setFixedSize(90, 556);
    widget_volumn->setLayout(vl_sound);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget_volumn);

    this->setLayout(vl_total);


    // 커넥트
    //connect(slider_vol, SIGNAL(valueChanged(int)) ,this, SIGNAL(clickedVolumnSlide(int)));
    connect(slider_vol, SIGNAL(valueChanged(int)) ,this, SLOT(setTextVolumn(int)));
    connect(btn_vol_add, SIGNAL(clicked()), this, SLOT(setValueVolumn()));
    connect(btn_vol_sub, SIGNAL(clicked()), this, SLOT(setValueVolumn()));

}


/**
 * @brief VolumnControl::setTextVolumn [SLOT]
 * @param p_value
 */
void VolumnControl::setTextVolumn(int p_value){
    global.setTextVolumn = p_value;//c220609
    print_debug();
    qDebug() << "global.setTextVolumn=" << global.setTextVolumn;
    this->lb_volumnNum->setText(QString("%1").arg(p_value));

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject json;
    json.insert("volumeType","volume_set");
    json.insert("volumeValue",p_value);
    network->request(HTTP_REMOTE_VOLUME_ADD, QString("http://%1:%2/volume").arg(global.device.getDeviceIP()).arg(global.port), json, true);
}


/**
 * @brief VolumnControl::setValueVolumn [SLOT]
 */
void VolumnControl::setValueVolumn(){
    int value = 0;
    int tmp_httpId = sender()->property("httpId").toInt();

    if(tmp_httpId==HTTP_REMOTE_VOLUME_ADD){
        value = (this->slider_vol->value() + 1);    // this->slider_vol->tickInterval()
        if(value >= 99){ value = 99; }
    }else{
        value = (this->slider_vol->value() - 1);
        if(value <= 0){ value = 0; }
    }
    this->slider_vol->setValue(value);
}


/**
 * @brief VolumnControl::setValueVolumn 외부에서 해당 value 의 볼륨을 세팅하고자 하는 경우 사용하는 함수
 * @param p_value
 */
void VolumnControl::setValueVolumn(int p_value){

    this->slider_vol->disconnect();

    this->slider_vol->setValue(p_value);
    this->lb_volumnNum->setText(QString("%1").arg(p_value));

    connect(slider_vol, SIGNAL(valueChanged(int)) ,this, SIGNAL(clickedVolumnSlide(int)));
    connect(slider_vol, SIGNAL(valueChanged(int)) ,this, SLOT(setTextVolumn(int)));
}


/**
 * @brief VolumnControl::responseHttp : 로즈 장치 정보요청 결과 처리
 * @param p_jsonObject QJsonObject&
 */
void VolumnControl::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    Q_UNUSED(p_jsonObject);

    switch(p_id){
    case HTTP_REMOTE_VOLUME_ADD :
        //ToastMsg::show(this, "", tr("You have selected the volume."));
        //ToastMsg::show(this, "", tr("음량을 선택하였습니다."));
        break;
    case HTTP_REMOTE_VOLUME_SUB :
        break;
    case HTTP_REMOTE_VOLUME_GET :
        if(p_jsonObject.contains("volumeValue")){
            this->setValueVolumn(p_jsonObject["volumeValue"].toInt());
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
        json.insert("volumeType","volume_get");
        network->request(HTTP_REMOTE_VOLUME_GET, QString("http://%1:%2/volume").arg(global.device.getDeviceIP()).arg(global.port), json, true);
    }
}

