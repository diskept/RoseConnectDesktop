#include "setting/settingOutputUSB.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"


const int DLG_WIDTH = 800;
const int DLG_HEIGHT = 600;
const int WIDGET_HEIGHT = 70;

const int HTTP_USB_GET_MODE  = 0;
const int HTTP_USB_SET_MODE  = 1;


SettingOutputUSB::SettingOutputUSB(QWidget *parent)//c220708
    : QDialog(parent)
{
    this->setInit();
    this->setUIControl();
}


void SettingOutputUSB::setInit(){

    this->deviceType = global.device.getDeviceType();
    this->jsonObj_USB_Setting = QJsonObject();

    NetworkHttp *network = new NetworkHttp;
    QJsonObject tmp_json;
    tmp_json.insert("outputMode", 3);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_USB_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.setting.get"), tmp_json, true);

    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    this->setStyleSheet("background-color:transparent;");
}


void SettingOutputUSB::setUIControl(){

    this->config_usb = new QWidget();
    this->config_usb->setStyleSheet("background-color:#333333; border:1px solid #4D4D4D;");
    this->config_usb->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    this->config_usb->hide();

    QWidget *usb_title = new QWidget(this->config_usb);
    usb_title->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    usb_title->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    usb_title->setGeometry(0, 0, 0, 0);

    QLabel *label_title = new QLabel(usb_title);
    label_title->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#FFFFFF;");
    label_title->setText("USB DAC " + tr("Setting"));
    label_title->setAlignment(Qt::AlignCenter);
    label_title->setFixedSize(DLG_WIDTH, 70);
    label_title->setGeometry(0, 0, 0, 0);

    QPushButton *btn_close = new QPushButton(usb_title);
    btn_close->setStyleSheet("background-color:transparent; border:none;");
    btn_close->setCursor(Qt::PointingHandCursor);
    btn_close->setFixedSize(60, 60);
    btn_close->setGeometry(DLG_WIDTH - 60, 0, 0, 0);

    QLabel *img_close = new QLabel(btn_close);
    img_close->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/icon_close_gray.png); border:none;");
    img_close->setFixedSize(60, 60);
    img_close->setGeometry(0, 0, 0, 0);

    QWidget *mqa_type = new QWidget(this->config_usb);
    mqa_type->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    mqa_type->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    mqa_type->setGeometry(0, 69, 0, 0);

    QLabel *label_mqaType = new QLabel(mqa_type);
    label_mqaType->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_mqaType->setText("MQA Type");
    label_mqaType->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_mqaType->setFixedSize(DLG_WIDTH - 35, 70);
    label_mqaType->setGeometry(35, 0, 0, 0);

    QPushButton *btn_mqaType = new QPushButton(mqa_type);
    btn_mqaType->setProperty("type", "mqaType");
    btn_mqaType->setStyleSheet("background-color:transparent; border:none;");
    btn_mqaType->setCursor(Qt::PointingHandCursor);
    btn_mqaType->setFixedSize(30, 30);
    btn_mqaType->setGeometry(140, 20, 0, 0);

    QLabel *img_mqaType = new QLabel(btn_mqaType);
    img_mqaType->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/201/tip_icon.png); border:none;");
    img_mqaType->setFixedSize(30, 30);
    img_mqaType->setGeometry(0, 0, 0, 0);

    this->comboBox_mqaType = new QComboBox(mqa_type);
    this->comboBox_mqaType->setObjectName("comboBox_mqaType");
    this->comboBox_mqaType->setStyleSheet("QComboBox {background-color:#212121; border:0px solid #212121; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:left; color:#FFFFFF;}"
                                    "QComboBox QListView {background-color:#212121; border:1px solid #B18658; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:center; color:#FFFFFF;}"
                                    "QComboBox QAbstractItemView {selection-color:#FFFFFF; selection-background-color:#B18658; border:1px solid #B18658;}");
    this->comboBox_mqaType->setCursor(Qt::PointingHandCursor);
    this->comboBox_mqaType->setFixedSize(300, 35);
    this->comboBox_mqaType->setGeometry(471, 16, 0, 0);
    this->comboBox_mqaType->addItem(tr("  Authenticator"), 0);
    this->comboBox_mqaType->addItem(tr("  Decoder"), 1);

    QWidget *pcm_freq = new QWidget(this->config_usb);
    pcm_freq->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    pcm_freq->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT * 2 + 10);
    pcm_freq->setGeometry(0, 138, 0, 0);

    QLabel *label_pcm_freq = new QLabel(pcm_freq);
    label_pcm_freq->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_pcm_freq->setText("PCM Resampling Frequency");
    label_pcm_freq->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_pcm_freq->setFixedSize(DLG_WIDTH - 35, WIDGET_HEIGHT * 2);
    label_pcm_freq->setGeometry(35, 0, 0, 0);

    QPushButton *btn_pcm_freq = new QPushButton(pcm_freq);
    btn_pcm_freq->setProperty("type", "pcmFreq");
    btn_pcm_freq->setStyleSheet("background-color:transparent; border:none;");
    btn_pcm_freq->setCursor(Qt::PointingHandCursor);
    btn_pcm_freq->setFixedSize(30, 30);
    btn_pcm_freq->setGeometry(305, 60, 0, 0);

    QLabel *img_pcm_freq = new QLabel(btn_pcm_freq);
    img_pcm_freq->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/201/tip_icon.png); border:none;");
    img_pcm_freq->setFixedSize(30, 30);
    img_pcm_freq->setGeometry(0, 0, 0, 0);

    QLabel *label_maxSampleRate = new QLabel(pcm_freq);
    label_maxSampleRate->setStyleSheet("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_maxSampleRate->setText("Max Sample Rate");
    label_maxSampleRate->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_maxSampleRate->setFixedSize(300, 35);
    label_maxSampleRate->setGeometry(421, 14, 0, 0);

    this->comboBox_maxSampleRate = new QComboBox(pcm_freq);
    this->comboBox_maxSampleRate->setObjectName("comboBox_maxSampleRate");
    this->comboBox_maxSampleRate->setStyleSheet("QComboBox {background-color:#212121; border:0px solid #212121; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:left; color:#FFFFFF;}"
                                    "QComboBox QListView {background-color:#212121; border:1px solid #B18658; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:center; color:#FFFFFF;}"
                                    "QComboBox QAbstractItemView {selection-color:#FFFFFF; selection-background-color:#B18658; border:1px solid #B18658;}");
    this->comboBox_maxSampleRate->setCursor(Qt::PointingHandCursor);
    this->comboBox_maxSampleRate->setFixedSize(150, 35);
    this->comboBox_maxSampleRate->setGeometry(621, 14, 0, 0);
    if(this->deviceType == "RS130" || this->deviceType == "RS520"){
        this->comboBox_maxSampleRate->addItem(tr("  Auto"), 0);
        this->comboBox_maxSampleRate->addItem(tr("  48kHz"), 1);
        this->comboBox_maxSampleRate->addItem(tr("  96kHz"), 2);
        this->comboBox_maxSampleRate->addItem(tr("  192kHz"), 3);
    }


    QLabel *label_resampleRate = new QLabel(pcm_freq);
    label_resampleRate->setStyleSheet("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_resampleRate->setText("Resample Rate");
    label_resampleRate->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_resampleRate->setFixedSize(300, 35);
    label_resampleRate->setGeometry(421, 61, 0, 0);

    this->comboBox_sampleRate = new QComboBox(pcm_freq);
    this->comboBox_sampleRate->setObjectName("comboBox_sampleRate");
    this->comboBox_sampleRate->setStyleSheet("QComboBox {background-color:#212121; border:0px solid #212121; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:left; color:#FFFFFF;}"
                                    "QComboBox QListView {background-color:#212121; border:1px solid #B18658; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:center; color:#FFFFFF;}"
                                    "QComboBox QAbstractItemView {selection-color:#FFFFFF; selection-background-color:#B18658; border:1px solid #B18658;}");
    this->comboBox_sampleRate->setCursor(Qt::PointingHandCursor);
    this->comboBox_sampleRate->setFixedSize(150, 35);
    this->comboBox_sampleRate->setGeometry(621, 61, 0, 0);
    if(this->deviceType == "RS130" || this->deviceType == "RS520"){
        this->comboBox_sampleRate->addItem(tr("  Original"), 0);
        this->comboBox_sampleRate->addItem(tr("  44.1kHz"), 1);
        this->comboBox_sampleRate->addItem(tr("  48kHz"), 2);
        this->comboBox_sampleRate->addItem(tr("  88.2kHz"), 3);
        this->comboBox_sampleRate->addItem(tr("  96kHz"), 4);
        this->comboBox_sampleRate->addItem(tr("  176.4kHz"), 5);
        this->comboBox_sampleRate->addItem(tr("  192kHz"), 6);
        this->comboBox_sampleRate->addItem(tr("  352.8kHz"), 7);
        this->comboBox_sampleRate->addItem(tr("  384kHz"), 8);
    }


    this->btn_mqaStream = new QPushButton(pcm_freq);
    this->btn_mqaStream->setStyleSheet("background-color:transparent; border:none;");
    this->btn_mqaStream->setCursor(Qt::PointingHandCursor);
    this->btn_mqaStream->setFixedSize(370, 50);
    this->btn_mqaStream->setGeometry(402, 100, 0, 0);

    this->checkbox_mqaStream = new QCheckBox(this->btn_mqaStream);
    this->checkbox_mqaStream->setStyleSheet("QCheckBox::indicator {width:50px; height:50px;}"
                                            "QCheckBox::indicator:checked {image:url(:/images/playlist/list_check_on.png);}"
                                            "QCheckBox::indicator:unchecked {image:url(:/images/playlist/list_check_off.png);}");
    this->checkbox_mqaStream->setGeometry(0, 0, 50, 50);

    QLabel *label_mqaStream = new QLabel(this->btn_mqaStream);
    label_mqaStream->setStyleSheet("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#FFFFFF;");
    label_mqaStream->setText(tr("Allow resampling on MQA stream"));
    label_mqaStream->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_mqaStream->setFixedSize(320, 50);
    label_mqaStream->setGeometry(50, -1, 0, 0);

    QWidget *dsd_mode = new QWidget(this->config_usb);
    dsd_mode->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    dsd_mode->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    dsd_mode->setGeometry(0, 287, 0, 0);

    QLabel *label_dsdMode = new QLabel(dsd_mode);
    label_dsdMode->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_dsdMode->setText("DSD Mode");
    label_dsdMode->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_dsdMode->setFixedSize(DLG_WIDTH - 35, 70);
    label_dsdMode->setGeometry(35, 0, 0, 0);

    QPushButton *btn_dsdMode = new QPushButton(dsd_mode);
    btn_dsdMode->setProperty("type", "dsdMode");
    btn_dsdMode->setStyleSheet("background-color:transparent; border:none;");
    btn_dsdMode->setCursor(Qt::PointingHandCursor);
    btn_dsdMode->setFixedSize(30, 30);
    btn_dsdMode->setGeometry(140, 20, 0, 0);

    QLabel *img_dsdMode = new QLabel(btn_dsdMode);
    img_dsdMode->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/201/tip_icon.png); border:none;");
    img_dsdMode->setFixedSize(30, 30);
    img_dsdMode->setGeometry(0, 0, 0, 0);

    this->comboBox_dsdMode = new QComboBox(dsd_mode);
    this->comboBox_dsdMode->setObjectName("comboBox_dsdMode");
    this->comboBox_dsdMode->setStyleSheet("QComboBox {background-color:#212121; border:0px solid #212121; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:left; color:#FFFFFF;}"
                                    "QComboBox QListView {background-color:#212121; border:1px solid #B18658; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:center; color:#FFFFFF;}"
                                    "QComboBox QAbstractItemView {selection-color:#FFFFFF; selection-background-color:#B18658; border:1px solid #B18658;}");
    this->comboBox_dsdMode->setCursor(Qt::PointingHandCursor);
    this->comboBox_dsdMode->setFixedSize(300, 35);
    this->comboBox_dsdMode->setGeometry(471, 16, 0, 0);
    if(this->deviceType == "RS130"){
        this->comboBox_dsdMode->addItem(tr("  Native DSD"), 0);
        this->comboBox_dsdMode->addItem(tr("  DSD over PCM"), 1);
        this->comboBox_dsdMode->addItem(tr("  DSD to PCM(upto DSD512)"), 2);
    }
    else if(this->deviceType == "RS520"){
        this->comboBox_dsdMode->addItem(tr("  Native DSD"), 0);
        this->comboBox_dsdMode->addItem(tr("  DSD over PCM"), 1);
        this->comboBox_dsdMode->addItem(tr("  DSD to PCM(upto DSD512)"), 2);
    }


    QWidget *software_volume = new QWidget(this->config_usb);
    software_volume->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    software_volume->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    software_volume->setGeometry(0, 356, 0, 0);

    QLabel *label_svc = new QLabel(software_volume);
    label_svc->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_svc->setText("SoftWare Volume Control");
    label_svc->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_svc->setFixedSize(DLG_WIDTH - 35, 70);
    label_svc->setGeometry(35, 0, 0, 0);

    QPushButton *btn_svc = new QPushButton(software_volume);
    btn_svc->setProperty("type", "svc");
    btn_svc->setStyleSheet("background-color:transparent; border:none;");
    btn_svc->setCursor(Qt::PointingHandCursor);
    btn_svc->setFixedSize(30, 30);
    btn_svc->setGeometry(285, 20, 0, 0);

    QLabel *img_svc = new QLabel(btn_svc);
    img_svc->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/201/tip_icon.png); border:none;");
    img_svc->setFixedSize(30, 30);
    img_svc->setGeometry(0, 0, 0, 0);

    this->onoff_svc = new OnOffWidget(software_volume);
    this->onoff_svc->setStyleSheet("border:none;");
    this->onoff_svc->setCursor(Qt::PointingHandCursor);
    this->onoff_svc->setFixedSize(74, 34);
    this->onoff_svc->setGeometry(697, 19, 0, 0);

    QWidget *DACs32bit = new QWidget(this->config_usb);
    DACs32bit->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    DACs32bit->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    DACs32bit->setGeometry(0, 425, 0, 0);

    QLabel *label_32bit = new QLabel(DACs32bit);
    label_32bit->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_32bit->setText(tr("The low 8bits of a 32-bit sound sources will have corrected"));
    label_32bit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_32bit->setFixedSize(DLG_WIDTH - 35, 70);
    label_32bit->setGeometry(35, 0, 0, 0);

    QPushButton *btn_32bit = new QPushButton(DACs32bit);
    btn_32bit->setProperty("type", "32bit");
    btn_32bit->setStyleSheet("background-color:transparent; border:none;");
    btn_32bit->setCursor(Qt::PointingHandCursor);
    btn_32bit->setFixedSize(30, 30);
    btn_32bit->setGeometry(590, 20, 0, 0);

    if(global.lang == 1){
         btn_32bit->setGeometry(292, 20, 0, 0);
     }

    QLabel *img_32bit = new QLabel(btn_32bit);
    img_32bit->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/201/tip_icon.png); border:none;");
    img_32bit->setFixedSize(30, 30);
    img_32bit->setGeometry(0, 0, 0, 0);

    this->onoff_32bit = new OnOffWidget(DACs32bit);
    this->onoff_32bit->setStyleSheet("border:none;");
    this->onoff_32bit->setCursor(Qt::PointingHandCursor);
    this->onoff_32bit->setFixedSize(74, 34);
    this->onoff_32bit->setGeometry(697, 19, 0, 0);

    this->btn_apply = new QPushButton(this->config_usb);
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

    this->btn_cancel = new QPushButton(this->config_usb);
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

    connect(btn_mqaType, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));
    connect(btn_pcm_freq, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));
    connect(btn_dsdMode, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));
    connect(btn_svc, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));
    connect(btn_32bit, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));

    connect(this->btn_mqaStream, SIGNAL(clicked()), this, SLOT(slot_clickedCheckbox()));

    connect(this->btn_apply, SIGNAL(clicked()), this, SLOT(slot_clickedApply()));
    connect(this->btn_cancel, SIGNAL(clicked()), this, SLOT(close()));

    QVBoxLayout *vBox_config_usb = new QVBoxLayout();
    vBox_config_usb->setContentsMargins(0, 0, 0, 0);
    vBox_config_usb->setAlignment(Qt::AlignTop);

    GSCommon::clearLayout(vBox_config_usb);
    vBox_config_usb->setAlignment(Qt::AlignTop);
    vBox_config_usb->addWidget(this->config_usb);

    this->setLayout(vBox_config_usb);
}


void SettingOutputUSB::getOutputSettingUSB(){

    int tmpValue = 0;
    bool tmpbool = false;

    if(this->deviceType == "RS130" || this->deviceType == "RS520"){
        if(this->jsonObj_USB_Setting.contains("mqaType")){
            tmpValue = ProcJsonEasy::getInt(this->jsonObj_USB_Setting, "mqaType");
            this->comboBox_mqaType->setCurrentIndex(tmpValue);
        }

        if(this->jsonObj_USB_Setting.contains("maxSamplingRate")){
            tmpValue = ProcJsonEasy::getInt(this->jsonObj_USB_Setting, "maxSamplingRate");
            this->comboBox_maxSampleRate->setCurrentIndex(tmpValue);
        }

        if(this->jsonObj_USB_Setting.contains("samplingRate")){
            tmpValue = ProcJsonEasy::getInt(this->jsonObj_USB_Setting, "samplingRate");
            this->comboBox_sampleRate->setCurrentIndex(tmpValue);
        }

        if(this->jsonObj_USB_Setting.contains("mqaResampling")){
            tmpValue = ProcJsonEasy::getInt(this->jsonObj_USB_Setting, "mqaResampling");
            this->checkbox_mqaStream->setChecked(tmpValue == 1 ? true : false);
        }

        if(this->jsonObj_USB_Setting.contains("dsdMode")){
            tmpValue = ProcJsonEasy::getInt(this->jsonObj_USB_Setting, "dsdMode");
            this->comboBox_dsdMode->setCurrentIndex(tmpValue);
        }

        if(this->jsonObj_USB_Setting.contains("digitalVolume")){
            tmpValue = ProcJsonEasy::getInt(this->jsonObj_USB_Setting, "digitalVolume");
            this->onoff_svc->setValue(tmpValue == 1 ? true : false);
        }

        if(this->jsonObj_USB_Setting.contains("isClearLowerBit")){
            tmpbool = ProcJsonEasy::getBool(this->jsonObj_USB_Setting, "isClearLowerBit");
            this->onoff_32bit->setValue(tmpbool);
        }
    }

    if(this->config_usb->isVisible() == false){
        this->config_usb->show();
    }
}


void SettingOutputUSB::setOutputSettingUSB(){

    int tmpInt = 0;
    int tmpCompareInt = 0;

    bool tmpBool = false;
    bool tmpConpareBool = false;

    QJsonObject tmpJson;

    if(this->deviceType == "RS130" || this->deviceType == "RS520"){
        this->jsonObj_USB_Setting.remove("code");
        this->jsonObj_USB_Setting.remove("status");
        this->jsonObj_USB_Setting.remove("version");

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_USB_Setting, "mqaType");
        tmpCompareInt = this->comboBox_mqaType->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_USB_Setting.remove("mqaType");
            this->jsonObj_USB_Setting.insert("mqaType", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_USB_Setting, "maxSamplingRate");
        tmpCompareInt = this->comboBox_maxSampleRate->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_USB_Setting.remove("maxSamplingRate");
            this->jsonObj_USB_Setting.insert("maxSamplingRate", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_USB_Setting, "samplingRate");
        tmpCompareInt = this->comboBox_sampleRate->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_USB_Setting.remove("samplingRate");
            this->jsonObj_USB_Setting.insert("samplingRate", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_USB_Setting, "mqaResampling");
        tmpCompareInt = this->checkbox_mqaStream->isChecked() == true ? 1 : 0;

        if(tmpInt != tmpCompareInt){
            this->jsonObj_USB_Setting.remove("mqaResampling");
            this->jsonObj_USB_Setting.insert("mqaResampling", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_USB_Setting, "dsdMode");
        tmpCompareInt = this->comboBox_dsdMode->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_USB_Setting.remove("dsdMode");
            this->jsonObj_USB_Setting.insert("dsdMode", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_USB_Setting, "digitalVolume");
        tmpCompareInt = this->onoff_svc->getCurrentValue() == true ? 1 : 0;

        if(tmpInt != tmpCompareInt){
            this->jsonObj_USB_Setting.remove("digitalVolume");
            this->jsonObj_USB_Setting.insert("digitalVolume", tmpCompareInt);
        }

        tmpBool = ProcJsonEasy::getBool(this->jsonObj_USB_Setting, "isClearLowerBit");
        tmpConpareBool = this->onoff_32bit->getCurrentValue();

        if(tmpBool != tmpConpareBool){
            this->jsonObj_USB_Setting.remove("isClearLowerBit");
            this->jsonObj_USB_Setting.insert("isClearLowerBit", tmpConpareBool);
        }

        tmpJson.insert("data", this->jsonObj_USB_Setting);
        tmpJson.insert("outputMode", 3);
    }

//    QJsonDocument doc(tmpJson);
//    QString strJson(doc.toJson(QJsonDocument::Compact));
//    qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << strJson;

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_USB_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.setting.set"), tmpJson, true);
}


void SettingOutputUSB::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";

//    QJsonDocument doc(p_jsonObject);
//    QString strJson(doc.toJson(QJsonDocument::Compact));
//    qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << "RCV ID :" << p_id << strJson;

    switch(p_id){
        case HTTP_USB_GET_MODE:
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                if(this->deviceType == "RS130" || this->deviceType == "RS520"){
                    if(p_jsonObject.contains("data")){
                        this->jsonObj_USB_Setting = ProcJsonEasy::getJsonObject(p_jsonObject, "data");
                        this->getOutputSettingUSB();

                        emit this->signal_loading_stateUSB(false);
                    }
                }
            }
            break;

        case HTTP_USB_SET_MODE:
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                if(this->deviceType == "RS130" || this->deviceType == "RS520"){
                    emit this->signal_setOutput_stateUSB(p_jsonObject);
                    emit this->signal_loading_stateUSB(false);

                    this->setResult(QDialog::Accepted);
                    this->close();
                }
            }
            break;
    }

    sender()->deleteLater();
}


void SettingOutputUSB::slot_clickedQuestion_clarification(){

    QString type = sender()->property("type").toString();

    DialogConfirm *notice = new DialogConfirm();
    notice->setAlertMode();

    int left = 0;
    int top = 0;

    if(type == "mqaType"){
        notice->setTitle("MQA Type");
        notice->setTextHeight(110);
        notice->setAlignmentContent(Qt::AlignLeft);
        notice->setStyleSheetContent("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:2.52; text-align:center; color:#FFFFFF;");
        notice->setText(tr("Set the MQA playback mode.\nYou can select either Authenticator or Decoder mode."
                           "\nOnly original sound or decoded audio data can be exported to digital output, not fully decoded audio data."));

        notice->setFixedDialogSize(660, -1);

        left = this->geometry().left() + (DLG_WIDTH - 660) / 2;
        top = this->geometry().top() + (DLG_HEIGHT - notice->sizeHint().height()) / 2;
        notice->setGeometry(left, top, 0, 0);
    }
    else if(type == "pcmFreq"){
        notice->setTitle("PCM Resampling Frequency");
        notice->setTextHeight(170);
        notice->setAlignmentContent(Qt::AlignLeft);
        notice->setStyleSheetContent("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:2.52; text-align:center; color:#FFFFFF;");
        notice->setText(tr("This is the menu to set sampling frequecy. if you want to output the original sound as it is,"
                           " select the Original Sampling Rate, and if you want to use the resampling function, select the desired sampling rate (48kHz, 96kHz, 192kHz, etc.)."
                           "\nMQA, Native DSD, DOP, Video, Bluetooth and Airplay, Roon Ready are not supported."));

        notice->setFixedDialogSize(660, -1);

        left = this->geometry().left() + (DLG_WIDTH - 660) / 2;
        top = this->geometry().top() + (DLG_HEIGHT - notice->sizeHint().height()) / 2;
        notice->setGeometry(left, top, 0, 0);
    }
    else if(type == "dsdMode"){
        notice->setTitle("DSD Mode");
        notice->setTextHeight(150);
        notice->setAlignmentContent(Qt::AlignLeft);
        notice->setStyleSheetContent("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:2.52; text-align:center; color:#FFFFFF;");
        notice->setText(tr("Determines the transmission format (Native, DOP, PCM) of audio samples to be delivered to the DAC when playing DSD sound sources."
                           "\nIn the case of USB DAC, you can play DSD sound source only after selecting Native, DOP, DSD to PCM depending on whether the connected DAC is supported."));

        notice->setFixedDialogSize(680, -1);

        left = this->geometry().left() + (DLG_WIDTH - 680) / 2;
        top = this->geometry().top() + (DLG_HEIGHT - notice->sizeHint().height()) / 2;
        notice->setGeometry(left, top, 0, 0);
    }
    else if(type == "svc"){
        notice->setTitle("Software Volume Control");
        notice->setTextHeight(200);
        notice->setAlignmentContent(Qt::AlignLeft);
        notice->setStyleSheetContent("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:2.52; text-align:center; color:#FFFFFF;");
        notice->setText(tr("Adjust the playback volume of digital audio data using the Rose volume control software.\n"
                           "\nCaution 1. It cannot be used in Native DSD or DSD over PCM mode. It is applied after changing to DSD to PCM(upto DSD128) mode."
                           "\nCaution 2. For digital output. MQA Stream is supported only when the volume is MAX. If it is less than MAX. MQA Stream is not supported."));

        notice->setFixedDialogSize(680, -1);

        left = this->geometry().left() + (DLG_WIDTH - 680) / 2;
        top = this->geometry().top() + (DLG_HEIGHT - notice->sizeHint().height()) / 2;
        notice->setGeometry(left, top, 0, 0);
    }
    else if(type == "32bit"){
        notice->setAlignmentTitle(Qt::AlignLeft | Qt::AlignVCenter);
        notice->setStyleSheetTitle("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:1.39; text-align:center; color:#FFFFFF;");
        notice->setTitle(tr("         The low 8bits of a 32-bit sound sources will have corrected"));
        notice->setTextHeight(100);
        notice->setAlignmentContent(Qt::AlignLeft);
        notice->setStyleSheetContent("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:2.52; text-align:center; color:#FFFFFF;");
        notice->setText(tr("Some DACs may generate noise when playing a 32-bit sound source with a high sampling rate."
                           "\nIn this case, it is used to reduce noise by correcting the lower 8 bits."));

        notice->setFixedDialogSize(660, -1);

        left = this->geometry().left() + (DLG_WIDTH - 660) / 2;
        top = this->geometry().top() + (DLG_HEIGHT - notice->sizeHint().height()) / 2;
        notice->setGeometry(left, top, 0, 0);
    }

    int result = notice->exec();

    if(result == QDialog::Accepted){

    }

    delete notice;
}


void SettingOutputUSB::slot_clickedCheckbox(){

    if(this->checkbox_mqaStream->isChecked() == true){
        this->checkbox_mqaStream->setChecked(false);
    }
    else if(this->checkbox_mqaStream->isChecked() == false){
        this->checkbox_mqaStream->setChecked(true);
    }
}


void SettingOutputUSB::slot_clickedApply(){

    emit this->signal_loading_stateUSB(true);

    this->setOutputSettingUSB();
}
