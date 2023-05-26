#include "setting/settingInputConfig.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"


const int DLG_WIDTH = 800;
const int DLG_HEIGHT = 600;
const int WIDGET_HEIGHT = 70;

const int HTTP_CONFIG_GET_MODE  = 0;
const int HTTP_CONFIG_SET_MODE  = 1;


SettingInputConfig::SettingInputConfig(QString p_configType, QWidget *parent)
    : QDialog(parent)
{
    this->configType = p_configType;

    this->setInit();
    this->setUIControl();
}


void SettingInputConfig::setInit(){

    this->deviceType = global.device.getDeviceType();
    this->jsonObj_config_Setting = QJsonObject();

    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setStyleSheet("background-color:transparent;");

    if(this->configType == "inputConfig"){
        this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    }
}


void SettingInputConfig::setUIControl(){

    if(this->configType == "inputConfig"){
        this->setUIControl_inputConfig();
    }

    QJsonObject tmp_json;

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_CONFIG_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("input.setting.get"), tmp_json, true);
}


void SettingInputConfig::setUIControl_inputConfig(){

    this->config_input = new QWidget();
    this->config_input->setStyleSheet("background-color:#333333; border:1px solid #4D4D4D;");
    this->config_input->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    this->config_input->hide();

    QWidget *output_title = new QWidget(this->config_input);
    output_title->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    output_title->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    output_title->setGeometry(0, 0, 0, 0);

    QLabel *label_title = new QLabel(output_title);
    label_title->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#FFFFFF;");
    label_title->setText("INPUT " + tr("Setting"));
    label_title->setAlignment(Qt::AlignCenter);
    label_title->setFixedSize(DLG_WIDTH, 70);
    label_title->setGeometry(0, 0, 0, 0);

    QPushButton *btn_close = new QPushButton(output_title);
    btn_close->setStyleSheet("background-color:transparent; border:none;");
    btn_close->setCursor(Qt::PointingHandCursor);
    btn_close->setFixedSize(60, 60);
    btn_close->setGeometry(DLG_WIDTH - 60, 0, 0, 0);

    QLabel *img_close = new QLabel(btn_close);
    img_close->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/icon_close_gray.png); border:none;");
    img_close->setFixedSize(60, 60);
    img_close->setGeometry(0, 0, 0, 0);


    QWidget *eARC_power = new QWidget(this->config_input);
    eARC_power->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    eARC_power->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    eARC_power->setGeometry(0, 69, 0, 0);

    QLabel *label_eARC_power = new QLabel(eARC_power);
    label_eARC_power->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_eARC_power->setText("eARC power interlock");
    label_eARC_power->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_eARC_power->setFixedSize(DLG_WIDTH - 35, 70);
    label_eARC_power->setGeometry(35, 0, 0, 0);

    QPushButton *btn_eARC_power = new QPushButton(eARC_power);
    btn_eARC_power->setProperty("type", "eARC");
    btn_eARC_power->setStyleSheet("background-color:transparent; border:none;");
    btn_eARC_power->setCursor(Qt::PointingHandCursor);
    btn_eARC_power->setFixedSize(30, 30);
    btn_eARC_power->setGeometry(255, 20, 0, 0);

    QLabel *img_eARC_power = new QLabel(btn_eARC_power);
    img_eARC_power->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/201/tip_icon.png); border:none;");
    img_eARC_power->setFixedSize(30, 30);
    img_eARC_power->setGeometry(0, 0, 0, 0);

    this->onoff_eARC_power = new OnOffWidget(eARC_power);
    this->onoff_eARC_power->setStyleSheet("border:none;");
    this->onoff_eARC_power->setCursor(Qt::PointingHandCursor);
    this->onoff_eARC_power->setFixedSize(74, 34);
    this->onoff_eARC_power->setGeometry(697, 19, 0, 0);


    this->btn_apply = new QPushButton(this->config_input);
    this->btn_apply->setStyleSheet("background-color:#B18658; border: 1px solid #B18658; border-radius:20px;");
    this->btn_apply->setCursor(Qt::PointingHandCursor);
    this->btn_apply->setFixedSize(140, 40);
    this->btn_apply->setGeometry(245, 529, 0, 0);

    QLabel *label_apply = new QLabel(this->btn_apply);
    label_apply->setStyleSheet("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:1.39; text-align:center; color:#FFFFFF;");
    label_apply->setText(tr("Apply"));
    label_apply->setAlignment(Qt::AlignCenter);
    label_apply->setFixedSize(140, 37);
    label_apply->setGeometry(0, 0, 0, 0);

    this->btn_cancel = new QPushButton(this->config_input);
    this->btn_cancel->setStyleSheet("background-color:#333333; border: 1px solid #777777; border-radius:20px;");
    this->btn_cancel->setCursor(Qt::PointingHandCursor);
    this->btn_cancel->setFixedSize(140, 40);
    this->btn_cancel->setGeometry(415, 529, 0, 0);

    QLabel *label_cancel = new QLabel(this->btn_cancel);
    label_cancel->setStyleSheet("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:1.39; text-align:center; color:#FFFFFF;");
    label_cancel->setText(tr("Cancel"));
    label_cancel->setAlignment(Qt::AlignCenter);
    label_cancel->setFixedSize(140, 37);
    label_cancel->setGeometry(0, 0, 0, 0);


    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));

    connect(btn_eARC_power, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));

    connect(this->btn_apply, SIGNAL(clicked()), this, SLOT(slot_clickedApply()));
    connect(this->btn_cancel, SIGNAL(clicked()), this, SLOT(close()));


    QVBoxLayout *vBox_config_input = new QVBoxLayout();
    vBox_config_input->setContentsMargins(0, 0, 0, 0);
    vBox_config_input->setAlignment(Qt::AlignTop);

    GSCommon::clearLayout(vBox_config_input);
    vBox_config_input->setAlignment(Qt::AlignTop);
    vBox_config_input->addWidget(this->config_input);

    this->setLayout(vBox_config_input);
}


void SettingInputConfig::getInputSettingConfig(){

    int tmpValue = 0;
    bool tmpbool = false;

    if(this->configType == "inputConfig"){
        if(this->jsonObj_config_Setting.contains("eARCPowerInfo")){
            tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "eARCPowerInfo");
            this->onoff_eARC_power->setValue(tmpValue == 1 ? false : true);
        }

        if(this->jsonObj_config_Setting.contains("opticalApwMode")){
            tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "opticalApwMode");

        }

        if(this->jsonObj_config_Setting.contains("opticalApwValue")){
            tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "opticalApwValue");

        }

        if(this->jsonObj_config_Setting.contains("rateDetect")){
            tmpbool = ProcJsonEasy::getBool(this->jsonObj_config_Setting, "rateDetect");

        }

        if(this->config_input->isVisible() == false){
            this->config_input->show();
        }
    }
}


void SettingInputConfig::setInputSettingConfig(){

    int tmpInt = 0;
    int tmpCompareInt = 0;

//    bool tmpBool = false;
//    bool tmpConpareBool = false;

    QJsonObject tmpJson;

    if(this->configType == "inputConfig"){
        this->jsonObj_config_Setting.remove("code");
        this->jsonObj_config_Setting.remove("status");
        this->jsonObj_config_Setting.remove("version");

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "eARCPowerInfo");
        tmpCompareInt = this->onoff_eARC_power->getCurrentValue() == true ? 0 : 1;

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("eARCPowerInfo");
            this->jsonObj_config_Setting.insert("eARCPowerInfo", tmpCompareInt);
        }

        tmpJson.insert("data", this->jsonObj_config_Setting);
        tmpJson.insert("version", this->version);
    }

//    QJsonDocument doc(tmpJson);
//    QString strJson(doc.toJson(QJsonDocument::Compact));
//    qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << strJson;

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_CONFIG_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("input.setting.set"), tmpJson, true);
}


void SettingInputConfig::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_code = "code";

//    QJsonDocument doc(p_jsonObject);
//    QString strJson(doc.toJson(QJsonDocument::Compact));
//    qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << "RCV ID :" << p_id << strJson;

    switch(p_id){
        case HTTP_CONFIG_GET_MODE:
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                if(p_jsonObject.contains("data")){
                    this->jsonObj_config_Setting = ProcJsonEasy::getJsonObject(p_jsonObject, "data");
                    this->version = ProcJsonEasy::getString(p_jsonObject, "version");
                    this->getInputSettingConfig();

                    emit this->signal_loading_stateConfig(false);
                }
                else{
                    emit this->signal_loading_stateConfig(false);

                    this->setResult(QDialog::Accepted);
                    this->close();
                }
            }
            break;

        case HTTP_CONFIG_SET_MODE:
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                emit this->signal_setInput_stateConfig(p_jsonObject);
                emit this->signal_loading_stateConfig(false);

                this->setResult(QDialog::Accepted);
                this->close();
            }
            break;
    }

    sender()->deleteLater();
}


void SettingInputConfig::slot_clickedQuestion_clarification(){

    QString type = sender()->property("type").toString();
    //qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << "type :" << type;

    DialogConfirm *notice = new DialogConfirm();
    notice->setAlertMode();

    int left = 0;
    int top = 0;

    if(type == "eARC"){
        notice->setTitle(tr("eARC power interlock"));
        notice->setTextHeight(110);
        notice->setAlignmentContent(Qt::AlignLeft);
        notice->setStyleSheetContent("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:2.52; text-align:center; color:#FFFFFF;");
        notice->setText(tr("When the TV power is turned on/off, the power of the Rose is also linked."));

        notice->setFixedDialogSize(660, -1);

        left = this->geometry().left() + (DLG_WIDTH - 660) / 2;
        if(this->configType == "inputConfig"){
            top = this->geometry().top() + (DLG_HEIGHT - notice->sizeHint().height()) / 2;
        }
        notice->setGeometry(left, top, 0, 0);
    }

    int result = notice->exec();

    if(result == QDialog::Accepted){

    }

    delete notice;
}


void SettingInputConfig::slot_clickedApply(){

    emit this->signal_loading_stateConfig(true);

    this->setInputSettingConfig();
}
