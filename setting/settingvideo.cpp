#include "settingvideo.h"
#include "common/global.h"
#include "widget/toastmsg.h"

#include <QScrollArea>
#include <common/gscommon.h>
#include <common/networkhttp.h>
#include <widget/dialogconfirm.h>

const int HTTP_VLC_SET = 99;
const int HTTP_VLC_GET = 98;
/**
 * @brief SettingVideo::SettingVideo 생성자
 * @param parent
 */
SettingVideo::SettingVideo(QWidget *parent) : QWidget(parent)
{
    this->setUIControl();
}

/**
 * @brief SettingVideo::setUIControl UI 세팅
 */
void SettingVideo::setUIControl()
{
    // 전체
    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("Video Option")));
    //vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("비디오 옵션")));
    vl_total->addWidget(this->getUIControlOption(tr("VLC Player Use")));
    //vl_total->addWidget(this->getUIControlOption(tr("VLC 플레이어 사용")));

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
 * @brief SettingVideo::getUIControlOption VLC 플레이어 사용 on/off
 * @param p_title
 * @return
 */
QWidget* SettingVideo::getUIControlOption(QString p_title){

    QLabel *lb_title = new QLabel(p_title);
    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;");

    onOff = new OnOffWidget;
    //onOff->setValue(false);
    onOff->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,15,47,15);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_title);
    hl_lb->addWidget(onOff, 0, Qt::AlignRight);
    QPushButton *btn_onOffHover = new QPushButton(onOff);
    btn_onOffHover->setFixedSize(80,40);
    btn_onOffHover->setStyleSheet("background-color:transparent;border:0px;");

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);

    // 커넥션
    //connect(onOff, SIGNAL(signal_changed(bool)), this, SLOT(slot_modeOnOff(bool)));
    connect(btn_onOffHover, SIGNAL(clicked()), this, SLOT(slot_modeOnOff()));
    return widget_total;
}

/**
 * @brief SettingVideo::requestVideoSetData 현재 VLC 플레이어 사용 유무 가져오기
 */
void SettingVideo::requestVideoSetData(){

    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_VLC_GET, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("get_video_setting_value"), tmp_json, true);
}

/**
 * @brief SettingVU::slot_modeOnOff [SLOT] VU 모드 ON/OFF 변경 API 호출
 */
//void SettingVideo::slot_modeOnOff(bool flagOn){
void SettingVideo::slot_modeOnOff(){

    DialogConfirm *dlg = new DialogConfirm(this);
    dlg->setTitle(tr("Video Player Change"));
    //dlg->setTitle(tr("비디오 플레이어 변경"));
    dlg->setText(tr("When changing the video player, the currently playing video will be terminated.\nDo you want to change it?"));
    //dlg->setText(tr("비디오 플레이어 변경시 재생중인 비디오가 종료됩니다.\n변경 하시겠습니까?"));
    int result = dlg->exec();
    if(result==QDialog::Accepted){

       bool isVLC = !this->onOff->getCurrentValue();
       this->onOff->setValue(isVLC);

       QJsonObject tmp_json;
       tmp_json.insert("settingType", "VIDEO_VLC_USE");
       tmp_json.insert("isVLC", isVLC);
       NetworkHttp *network = new NetworkHttp;
       connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
       network->request(HTTP_VLC_SET, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("set_video_setting_value"), tmp_json, true);
    }
}
/**
 * @brief SettingVideo::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void SettingVideo::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    switch(p_id){
    case HTTP_VLC_SET :
        break;
    case HTTP_VLC_GET :
        bool isInstallVlc = false;
        if(p_jsonObject.contains("isInstallVlc")){
            isInstallVlc = p_jsonObject["isInstallVlc"].toBool();
        }
        if(isInstallVlc==false){
            ToastMsg::show(this, "", tr("VLC app is not installed. You can use it after installing it in the market"));
            //ToastMsg::show(this, "", tr("VLC 앱이 설치되어 있지 않습니다. 마켓에서 설치 후 사용이 가능합니다."));
            this->onOff->setValue(false);
            this->onOff->setEnabled(false);
        }else{
            this->onOff->setEnabled(true);
            if(p_jsonObject.contains("isVLC")){
                bool isVLC = p_jsonObject["isVLC"].toBool();
                this->onOff->setValue(isVLC);
            }
        }
        break;
    }

    sender()->deleteLater();
}
