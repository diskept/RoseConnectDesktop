#include "setting/settingOutputConfig.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"


const int DLG_WIDTH = 800;
const int DLG_HEIGHT = 600;
const int DLG_HEIGHT2 = 740;
const int DLG_HEIGHT3 = 670;
const int WIDGET_HEIGHT = 70;

const int HTTP_CONFIG_GET_MODE  = 0;
const int HTTP_CONFIG_SET_MODE  = 1;


SettingOutputConfig::SettingOutputConfig(QString p_configType, QWidget *parent)
    : QDialog(parent)
{
    this->configType = p_configType;

    this->setInit();
    this->setUIControl();
}


void SettingOutputConfig::setInit(){

    this->deviceType = global.device.getDeviceType();
    this->jsonObj_config_Setting = QJsonObject();

    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setStyleSheet("background-color:transparent;");

    if(this->configType == "etcConfig"){
        this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    }
    else if(this->configType == "preConfig"){
        this->setFixedSize(DLG_WIDTH, DLG_HEIGHT2);
    }
    else if(this->configType == "spkConfig"){
        this->setFixedSize(DLG_WIDTH, DLG_HEIGHT3);
    }
}


void SettingOutputConfig::setUIControl(){

    if(this->configType == "etcConfig"){
        this->setUIControl_etcConfig();
    }
    else if(this->configType == "preConfig"){
        this->setUIControl_preampConfig();
    }
    else if(this->configType == "spkConfig"){
        this->setUIControl_speakerConfig();
    }

    QJsonObject tmp_json;
    tmp_json.insert("outputMode", 1);

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_CONFIG_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.setting.get"), tmp_json, true);
}


void SettingOutputConfig::setUIControl_etcConfig(){

    this->config_etc = new QWidget();
    this->config_etc->setStyleSheet("background-color:#333333; border:1px solid #4D4D4D;");
    this->config_etc->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    this->config_etc->hide();


    QWidget *output_title = new QWidget(this->config_etc);
    output_title->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    output_title->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    output_title->setGeometry(0, 0, 0, 0);

    QLabel *label_title = new QLabel(output_title);
    label_title->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#FFFFFF;");
    label_title->setText("OUTPUT " + tr("Setting"));
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


    QWidget *mqa_type = new QWidget(this->config_etc);
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
    if(this->deviceType == "RS130" || this->deviceType == "RS520"){
        this->comboBox_mqaType->addItem(tr("  Authenticator"), 0);
        this->comboBox_mqaType->addItem(tr("  Decoder"), 1);
    }


    QWidget *pcm_freq = new QWidget(this->config_etc);
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


    QWidget *dsd_mode = new QWidget(this->config_etc);
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
    if(this->deviceType == "RS130" || this->deviceType == "RS520"){
        this->comboBox_dsdMode->addItem(tr("  Native DSD"), 0);
        this->comboBox_dsdMode->addItem(tr("  DSD over PCM"), 1);
        this->comboBox_dsdMode->addItem(tr("  DSD to PCM"), 2);
    }


    QWidget *software_volume = new QWidget(this->config_etc);
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


    QWidget *DACs32bit = new QWidget(this->config_etc);
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


    this->btn_apply = new QPushButton(this->config_etc);
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

    this->btn_cancel = new QPushButton(this->config_etc);
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

    QVBoxLayout *vBox_config_etc = new QVBoxLayout();
    vBox_config_etc->setContentsMargins(0, 0, 0, 0);
    vBox_config_etc->setAlignment(Qt::AlignTop);

    GSCommon::clearLayout(vBox_config_etc);
    vBox_config_etc->setAlignment(Qt::AlignTop);
    vBox_config_etc->addWidget(this->config_etc);

    this->setLayout(vBox_config_etc);
}


void SettingOutputConfig::setUIControl_preampConfig(){

    this->config_pre = new QWidget();
    this->config_pre->setStyleSheet("background-color:#333333; border:1px solid #4D4D4D;");
    this->config_pre->setFixedSize(DLG_WIDTH, DLG_HEIGHT2);
    this->config_pre->hide();


    QWidget *output_title = new QWidget(this->config_pre);
    output_title->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    output_title->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    output_title->setGeometry(0, 0, 0, 0);

    QLabel *label_title = new QLabel(output_title);
    label_title->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#FFFFFF;");
    label_title->setText("OUTPUT " + tr("Setting"));
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


    this->widget_preout_level = new QWidget(this->config_pre);
    this->widget_preout_level->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    this->widget_preout_level->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    this->widget_preout_level->setGeometry(0, 69, 0, 0);

    QLabel *label_preoutLevel = new QLabel(this->widget_preout_level);
    label_preoutLevel->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_preoutLevel->setText(tr("Pre-out level setting"));
    label_preoutLevel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_preoutLevel->setFixedSize(DLG_WIDTH - 35, 70);
    label_preoutLevel->setGeometry(35, 0, 0, 0);

    QPushButton *btn_preoutLevel = new QPushButton(this->widget_preout_level);
    btn_preoutLevel->setProperty("type", "preoutLevel");
    btn_preoutLevel->setStyleSheet("background-color:transparent; border:none;");
    btn_preoutLevel->setCursor(Qt::PointingHandCursor);
    btn_preoutLevel->setFixedSize(30, 30);
    btn_preoutLevel->setGeometry(250, 20, 0, 0);

    if(global.lang == 1){
        btn_preoutLevel->setGeometry(205, 20, 0, 0);
    }

    QLabel *img_preoutLevel = new QLabel(btn_preoutLevel);
    img_preoutLevel->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/201/tip_icon.png); border:none;");
    img_preoutLevel->setFixedSize(30, 30);
    img_preoutLevel->setGeometry(0, 0, 0, 0);

    this->onoff_pre = new OnOffWidget(this->widget_preout_level);
    this->onoff_pre->setStyleSheet("border:none;");
    this->onoff_pre->setEnabled(false);
    this->onoff_pre->setFixedSize(74, 34);
    this->onoff_pre->setGeometry(421, 16, 0, 0);

    this->comboBox_preoutLevel = new QComboBox(this->widget_preout_level);
    this->comboBox_preoutLevel->setObjectName("comboBox_preoutLevel");
    this->comboBox_preoutLevel->setStyleSheet("QComboBox {background-color:#212121; border:0px solid #212121; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:left; color:#FFFFFF;}"
                                              "QComboBox QListView {background-color:#212121; border:1px solid #B18658; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:center; color:#FFFFFF;}"
                                              "QComboBox QAbstractItemView {selection-color:#FFFFFF; selection-background-color:#B18658; border:1px solid #B18658;}"
                                              "QAbstractScrollArea QScrollBar:vertical {border: none; background-color: transparent; width: 6px; margin: 12px 0px 12px 0px;}"
                                              "QAbstractScrollArea QScrollBar::handle:vertical {background-color: #b18658; max-height: 30px; border-radius: 3px;}"
                                              "QAbstractScrollArea QScrollBar::add-line:vertical {height: 0px; subcontrol-position: bottom; subcontrol-origin: margin;}"
                                              "QAbstractScrollArea QScrollBar::sub-line:vertical {height: 0px; subcontrol-position: top; subcontrol-origin: margin;}"
                                              "QAbstractScrollArea QScrollBar::add-page:vertical, QAbstractScrollArea QScrollBar::sub-page:vertical {background: none;}");
    this->comboBox_preoutLevel->setCursor(Qt::PointingHandCursor);
    this->comboBox_preoutLevel->setFixedSize(200, 35);
    this->comboBox_preoutLevel->setGeometry(571, 16, 0, 0);
    if(this->deviceType == "RS520"){
        this->comboBox_preoutLevel->addItem(tr("  None"), 0);
        this->comboBox_preoutLevel->addItem(tr("  30 mV"), 1);
        this->comboBox_preoutLevel->addItem(tr("  60 mV"), 2);
        this->comboBox_preoutLevel->addItem(tr("  95 mV"), 3);
        this->comboBox_preoutLevel->addItem(tr("  120 mV"), 4);
        this->comboBox_preoutLevel->addItem(tr("  160 mV"), 5);
        this->comboBox_preoutLevel->addItem(tr("  300 mV"), 6);
        this->comboBox_preoutLevel->addItem(tr("  600 mV"), 7);
        this->comboBox_preoutLevel->addItem(tr("  1000 mV"), 8);
        this->comboBox_preoutLevel->addItem(tr("  1300 mV"), 9);
        this->comboBox_preoutLevel->addItem(tr("  1600 mV"), 10);
        this->comboBox_preoutLevel->addItem(tr("  1900 mV"), 11);
        this->comboBox_preoutLevel->addItem(tr("  2000 mV"), 12);
    }


    this->widget_speaker_type = new QWidget(this->config_pre);
    this->widget_speaker_type->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    this->widget_speaker_type->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    this->widget_speaker_type->setGeometry(0, 138, 0, 0);

    QLabel *label_speakerType = new QLabel(this->widget_speaker_type);
    label_speakerType->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_speakerType->setText(tr("Speaker Type Selection"));
    label_speakerType->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_speakerType->setFixedSize(DLG_WIDTH - 35, 70);
    label_speakerType->setGeometry(35, 0, 0, 0);

    QPushButton *btn_speakerType = new QPushButton(this->widget_speaker_type);
    btn_speakerType->setProperty("type", "speakType");
    btn_speakerType->setStyleSheet("background-color:transparent; border:none;");
    btn_speakerType->setCursor(Qt::PointingHandCursor);
    btn_speakerType->setFixedSize(30, 30);
    btn_speakerType->setGeometry(270, 20, 0, 0);

    if(global.lang == 1){
        btn_speakerType->setGeometry(210, 20, 0, 0);
    }

    QLabel *img_speakerType = new QLabel(btn_speakerType);
    img_speakerType->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/201/tip_icon.png); border:none;");
    img_speakerType->setFixedSize(30, 30);
    img_speakerType->setGeometry(0, 0, 0, 0);

    this->comboBox_speakerType = new QComboBox(this->widget_speaker_type);
    this->comboBox_speakerType->setObjectName("comboBox_speakerType");
    this->comboBox_speakerType->setStyleSheet("QComboBox {background-color:#212121; border:0px solid #212121; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:left; color:#FFFFFF;}"
                                              "QComboBox QListView {background-color:#212121; border:1px solid #B18658; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:center; color:#FFFFFF;}"
                                              "QComboBox QAbstractItemView {selection-color:#FFFFFF; selection-background-color:#B18658; border:1px solid #B18658;}");
    this->comboBox_speakerType->setCursor(Qt::PointingHandCursor);
    this->comboBox_speakerType->setFixedSize(300, 35);
    this->comboBox_speakerType->setGeometry(471, 16, 0, 0);
    if(this->deviceType == "RS520"){
        this->comboBox_speakerType->addItem(tr("  BookShelf Speker"), 0);
        this->comboBox_speakerType->addItem(tr("  TallBoy Speaker"), 1);
        this->comboBox_speakerType->addItem(tr("  Floor Standing Speaker"), 2);
        this->comboBox_speakerType->addItem(tr("  Default Volume Curve"), 3);
    }


    this->widget_software_volume = new QWidget(this->config_pre);
    this->widget_software_volume->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    this->widget_software_volume->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    this->widget_software_volume->setGeometry(0, 207, 0, 0);

    QLabel *label_svc = new QLabel(this->widget_software_volume);
    label_svc->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_svc->setText("SoftWare Volume Control");
    label_svc->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_svc->setFixedSize(DLG_WIDTH - 35, 70);
    label_svc->setGeometry(35, 0, 0, 0);

    QPushButton *btn_svc = new QPushButton(this->widget_software_volume);
    btn_svc->setProperty("type", "svc");
    btn_svc->setStyleSheet("background-color:transparent; border:none;");
    btn_svc->setCursor(Qt::PointingHandCursor);
    btn_svc->setFixedSize(30, 30);
    btn_svc->setGeometry(285, 20, 0, 0);

    QLabel *img_svc = new QLabel(btn_svc);
    img_svc->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/201/tip_icon.png); border:none;");
    img_svc->setFixedSize(30, 30);
    img_svc->setGeometry(0, 0, 0, 0);

    this->onoff_svc = new OnOffWidget(this->widget_software_volume);
    this->onoff_svc->setStyleSheet("border:none;");
    this->onoff_svc->setCursor(Qt::PointingHandCursor);
    this->onoff_svc->setFixedSize(74, 34);
    this->onoff_svc->setGeometry(697, 19, 0, 0);


    this->widget_DACs32bit = new QWidget(this->config_pre);
    this->widget_DACs32bit->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    this->widget_DACs32bit->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    this->widget_DACs32bit->setGeometry(0, 276, 0, 0);

    QLabel *label_32bit = new QLabel(this->widget_DACs32bit);
    label_32bit->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_32bit->setText(tr("The low 8bits of a 32-bit sound sources will have corrected"));
    label_32bit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_32bit->setFixedSize(DLG_WIDTH - 35, 70);
    label_32bit->setGeometry(35, 0, 0, 0);

    QPushButton *btn_32bit = new QPushButton(this->widget_DACs32bit);
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

    this->onoff_32bit = new OnOffWidget(this->widget_DACs32bit);
    this->onoff_32bit->setStyleSheet("border:none;");
    this->onoff_32bit->setCursor(Qt::PointingHandCursor);
    this->onoff_32bit->setFixedSize(74, 34);
    this->onoff_32bit->setGeometry(697, 19, 0, 0);


    this->widget_firFilter = new QWidget(this->config_pre);
    this->widget_firFilter->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    this->widget_firFilter->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    this->widget_firFilter->setGeometry(0, 345, 0, 0);

    QLabel *label_firFilter = new QLabel(this->widget_firFilter);
    label_firFilter->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_firFilter->setText("FIR interpoation phase Filter");
    label_firFilter->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_firFilter->setFixedSize(DLG_WIDTH - 35, 70);
    label_firFilter->setGeometry(35, 0, 0, 0);

    QPushButton *btn_firFilter = new QPushButton(this->widget_firFilter);
    btn_firFilter->setProperty("type", "firFilter");
    btn_firFilter->setStyleSheet("background-color:transparent; border:none;");
    btn_firFilter->setCursor(Qt::PointingHandCursor);
    btn_firFilter->setFixedSize(30, 30);
    btn_firFilter->setGeometry(310, 20, 0, 0);

    QLabel *img_firFilter = new QLabel(btn_firFilter);
    img_firFilter->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/201/tip_icon.png); border:none;");
    img_firFilter->setFixedSize(30, 30);
    img_firFilter->setGeometry(0, 0, 0, 0);

    this->comboBox_firFilter = new QComboBox(this->widget_firFilter);
    this->comboBox_firFilter->setObjectName("comboBox_mqaType");
    this->comboBox_firFilter->setStyleSheet("QComboBox {background-color:#212121; border:0px solid #212121; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:left; color:#FFFFFF;}"
                                            "QComboBox QListView {background-color:#212121; border:1px solid #B18658; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:center; color:#FFFFFF;}"
                                            "QComboBox QAbstractItemView {selection-color:#FFFFFF; selection-background-color:#B18658; border:1px solid #B18658;}");
    this->comboBox_firFilter->setCursor(Qt::PointingHandCursor);
    this->comboBox_firFilter->setFixedSize(400, 35);
    this->comboBox_firFilter->setGeometry(371, 16, 0, 0);
    if(this->deviceType == "RS520"){
        this->comboBox_firFilter->addItem(tr("  Brick Wall filter"), 0);
        this->comboBox_firFilter->addItem(tr("  Corrected minimum phase Fast Roll-off filter"), 1);
        this->comboBox_firFilter->addItem(tr("  Apodizing Fast Roll-off filter"), 2);
        this->comboBox_firFilter->addItem(tr("  Minimum phase Slow Roll-off filter"), 3);
        this->comboBox_firFilter->addItem(tr("  Minimum phase Fast Roll-off filter"), 4);
        this->comboBox_firFilter->addItem(tr("  Linear phase Slow Roll-off filter"), 5);
        this->comboBox_firFilter->addItem(tr("  Linear phase Fast Roll-off filter"), 6);
    }


    this->widget_pcm_freq = new QWidget(this->config_pre);
    this->widget_pcm_freq->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    this->widget_pcm_freq->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT * 2 + 10);
    this->widget_pcm_freq->setGeometry(0, 414, 0, 0);

    QLabel *label_pcm_freq = new QLabel(this->widget_pcm_freq);
    label_pcm_freq->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_pcm_freq->setText("PCM Resampling Frequency");
    label_pcm_freq->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_pcm_freq->setFixedSize(DLG_WIDTH - 35, WIDGET_HEIGHT * 2);
    label_pcm_freq->setGeometry(35, 0, 0, 0);

    QPushButton *btn_pcm_freq = new QPushButton(this->widget_pcm_freq);
    btn_pcm_freq->setProperty("type", "pcmFreq");
    btn_pcm_freq->setStyleSheet("background-color:transparent; border:none;");
    btn_pcm_freq->setCursor(Qt::PointingHandCursor);
    btn_pcm_freq->setFixedSize(30, 30);
    btn_pcm_freq->setGeometry(305, 60, 0, 0);

    QLabel *img_pcm_freq = new QLabel(btn_pcm_freq);
    img_pcm_freq->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/201/tip_icon.png); border:none;");
    img_pcm_freq->setFixedSize(30, 30);
    img_pcm_freq->setGeometry(0, 0, 0, 0);

    QLabel *label_maxSampleRate = new QLabel(this->widget_pcm_freq);
    label_maxSampleRate->setStyleSheet("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_maxSampleRate->setText("Max Sample Rate");
    label_maxSampleRate->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_maxSampleRate->setFixedSize(300, 35);
    label_maxSampleRate->setGeometry(421, 14, 0, 0);

    this->comboBox_maxSampleRate = new QComboBox(this->widget_pcm_freq);
    this->comboBox_maxSampleRate->setObjectName("comboBox_maxSampleRate");
    this->comboBox_maxSampleRate->setStyleSheet("QComboBox {background-color:#212121; border:0px solid #212121; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:left; color:#FFFFFF;}"
                                                "QComboBox QListView {background-color:#212121; border:1px solid #B18658; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:center; color:#FFFFFF;}"
                                                "QComboBox QAbstractItemView {selection-color:#FFFFFF; selection-background-color:#B18658; border:1px solid #B18658;}");
    this->comboBox_maxSampleRate->setCursor(Qt::PointingHandCursor);
    this->comboBox_maxSampleRate->setFixedSize(150, 35);
    this->comboBox_maxSampleRate->setGeometry(621, 14, 0, 0);
    if(this->deviceType == "RS520"){
        this->comboBox_maxSampleRate->addItem(tr("  Auto"), 0);
        this->comboBox_maxSampleRate->addItem(tr("  48kHz"), 1);
        this->comboBox_maxSampleRate->addItem(tr("  96kHz"), 2);
        this->comboBox_maxSampleRate->addItem(tr("  192kHz"), 3);
    }

    QLabel *label_resampleRate = new QLabel(this->widget_pcm_freq);
    label_resampleRate->setStyleSheet("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_resampleRate->setText("Resample Rate");
    label_resampleRate->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_resampleRate->setFixedSize(300, 35);
    label_resampleRate->setGeometry(421, 61, 0, 0);

    this->comboBox_sampleRate = new QComboBox(this->widget_pcm_freq);
    this->comboBox_sampleRate->setObjectName("comboBox_sampleRate");
    this->comboBox_sampleRate->setStyleSheet("QComboBox {background-color:#212121; border:0px solid #212121; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:left; color:#FFFFFF;}"
                                             "QComboBox QListView {background-color:#212121; border:1px solid #B18658; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:center; color:#FFFFFF;}"
                                             "QComboBox QAbstractItemView {selection-color:#FFFFFF; selection-background-color:#B18658; border:1px solid #B18658;}");
    this->comboBox_sampleRate->setCursor(Qt::PointingHandCursor);
    this->comboBox_sampleRate->setFixedSize(150, 35);
    this->comboBox_sampleRate->setGeometry(621, 61, 0, 0);
    if(this->deviceType == "RS520"){
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

    this->btn_mqaStream = new QPushButton(this->widget_pcm_freq);
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


    this->widget_dsd_mode = new QWidget(this->config_pre);
    this->widget_dsd_mode->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    this->widget_dsd_mode->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    this->widget_dsd_mode->setGeometry(0, 563, 0, 0);

    QLabel *label_dsdMode = new QLabel(this->widget_dsd_mode);
    label_dsdMode->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_dsdMode->setText("DSD Mode");
    label_dsdMode->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_dsdMode->setFixedSize(DLG_WIDTH - 35, 70);
    label_dsdMode->setGeometry(35, 0, 0, 0);

    QPushButton *btn_dsdMode = new QPushButton(this->widget_dsd_mode);
    btn_dsdMode->setProperty("type", "dsdMode");
    btn_dsdMode->setStyleSheet("background-color:transparent; border:none;");
    btn_dsdMode->setCursor(Qt::PointingHandCursor);
    btn_dsdMode->setFixedSize(30, 30);
    btn_dsdMode->setGeometry(140, 20, 0, 0);

    QLabel *img_dsdMode = new QLabel(btn_dsdMode);
    img_dsdMode->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/201/tip_icon.png); border:none;");
    img_dsdMode->setFixedSize(30, 30);
    img_dsdMode->setGeometry(0, 0, 0, 0);

    this->comboBox_dsdMode = new QComboBox(this->widget_dsd_mode);
    this->comboBox_dsdMode->setObjectName("comboBox_dsdMode");
    this->comboBox_dsdMode->setStyleSheet("QComboBox {background-color:#212121; border:0px solid #212121; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:left; color:#FFFFFF;}"
                                          "QComboBox QListView {background-color:#212121; border:1px solid #B18658; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:center; color:#FFFFFF;}"
                                          "QComboBox QAbstractItemView {selection-color:#FFFFFF; selection-background-color:#B18658; border:1px solid #B18658;}");
    this->comboBox_dsdMode->setCursor(Qt::PointingHandCursor);
    this->comboBox_dsdMode->setFixedSize(300, 35);
    this->comboBox_dsdMode->setGeometry(471, 16, 0, 0);
    if(this->deviceType == "RS520"){
        this->comboBox_dsdMode->addItem(tr("  Native DSD(upto DSD512)"), 0);
        this->comboBox_dsdMode->addItem(tr("  DSD over PCM(Not supported)"), 1);
        this->comboBox_dsdMode->addItem(tr("  DSD to PCM(upto DSD512)"), 2);
    }


    this->btn_apply = new QPushButton(this->config_pre);
    this->btn_apply->setStyleSheet("background-color:#B18658; border: 1px solid #B18658; border-radius:20px;");
    this->btn_apply->setCursor(Qt::PointingHandCursor);
    this->btn_apply->setFixedSize(140, 40);
    this->btn_apply->setGeometry(245, 668, 0, 0);

    QLabel *label_apply = new QLabel(this->btn_apply);
    label_apply->setStyleSheet("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:1.39; text-align:center; color:#FFFFFF;");
    label_apply->setText(tr("Apply"));
    label_apply->setAlignment(Qt::AlignCenter);
    label_apply->setFixedSize(140, 37);
    label_apply->setGeometry(0, 0, 0, 0);

    this->btn_cancel = new QPushButton(this->config_pre);
    this->btn_cancel->setStyleSheet("background-color:#333333; border: 1px solid #777777; border-radius:20px;");
    this->btn_cancel->setCursor(Qt::PointingHandCursor);
    this->btn_cancel->setFixedSize(140, 40);
    this->btn_cancel->setGeometry(415, 668, 0, 0);

    QLabel *label_cancel = new QLabel(this->btn_cancel);
    label_cancel->setStyleSheet("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:1.39; text-align:center; color:#FFFFFF;");
    label_cancel->setText(tr("Cancel"));
    label_cancel->setAlignment(Qt::AlignCenter);
    label_cancel->setFixedSize(140, 37);
    label_cancel->setGeometry(0, 0, 0, 0);


    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));

    connect(btn_preoutLevel, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));
    connect(btn_speakerType, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));
    connect(btn_svc, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));
    connect(btn_32bit, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));
    connect(btn_firFilter, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));
    connect(btn_pcm_freq, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));
    connect(btn_dsdMode, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));

    connect(this->comboBox_preoutLevel, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_clickedPreOutLevel(int)));
    connect(this->onoff_pre, &OnOffWidget::signal_changed, this, &SettingOutputConfig::slot_clickedPreOutLevelOnOff);
    connect(this->onoff_svc, &OnOffWidget::signal_changed, this, &SettingOutputConfig::slot_clickedSoftwareVolumeOnOff);

    connect(this->btn_mqaStream, SIGNAL(clicked()), this, SLOT(slot_clickedCheckbox()));

    connect(this->btn_apply, SIGNAL(clicked()), this, SLOT(slot_clickedApply()));
    connect(this->btn_cancel, SIGNAL(clicked()), this, SLOT(close()));

    QVBoxLayout *vBox_config_pre = new QVBoxLayout();
    vBox_config_pre->setContentsMargins(0, 0, 0, 0);
    vBox_config_pre->setAlignment(Qt::AlignTop);

    GSCommon::clearLayout(vBox_config_pre);
    vBox_config_pre->setAlignment(Qt::AlignTop);
    vBox_config_pre->addWidget(this->config_pre);

    this->setLayout(vBox_config_pre);
}


void SettingOutputConfig::setUIControl_speakerConfig(){

    this->config_spk = new QWidget();
    this->config_spk->setStyleSheet("background-color:#333333; border:1px solid #4D4D4D;");
    this->config_spk->setFixedSize(DLG_WIDTH, DLG_HEIGHT3);
    this->config_spk->hide();


    QWidget *output_title = new QWidget(this->config_spk);
    output_title->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    output_title->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    output_title->setGeometry(0, 0, 0, 0);

    QLabel *label_title = new QLabel(output_title);
    label_title->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#FFFFFF;");
    label_title->setText("OUTPUT " + tr("Setting"));
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


    QWidget *software_volume = new QWidget(this->config_spk);
    software_volume->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    software_volume->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    software_volume->setGeometry(0, 69, 0, 0);

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


    QWidget *speaker_type = new QWidget(this->config_spk);
    speaker_type->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    speaker_type->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    speaker_type->setGeometry(0, 138, 0, 0);

    QLabel *label_speakerType = new QLabel(speaker_type);
    label_speakerType->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_speakerType->setText(tr("Speaker Type Selection"));
    label_speakerType->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_speakerType->setFixedSize(DLG_WIDTH - 35, 70);
    label_speakerType->setGeometry(35, 0, 0, 0);

    QPushButton *btn_speakerType = new QPushButton(speaker_type);
    btn_speakerType->setProperty("type", "speakType");
    btn_speakerType->setStyleSheet("background-color:transparent; border:none;");
    btn_speakerType->setCursor(Qt::PointingHandCursor);
    btn_speakerType->setFixedSize(30, 30);
    btn_speakerType->setGeometry(270, 20, 0, 0);

    if(global.lang == 1){
        btn_speakerType->setGeometry(210, 20, 0, 0);
    }

    QLabel *img_speakerType = new QLabel(btn_speakerType);
    img_speakerType->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/201/tip_icon.png); border:none;");
    img_speakerType->setFixedSize(30, 30);
    img_speakerType->setGeometry(0, 0, 0, 0);

    this->comboBox_speakerType = new QComboBox(speaker_type);
    this->comboBox_speakerType->setObjectName("comboBox_speakerType");
    this->comboBox_speakerType->setStyleSheet("QComboBox {background-color:#212121; border:0px solid #212121; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:left; color:#FFFFFF;}"
                                              "QComboBox QListView {background-color:#212121; border:1px solid #B18658; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:center; color:#FFFFFF;}"
                                              "QComboBox QAbstractItemView {selection-color:#FFFFFF; selection-background-color:#B18658; border:1px solid #B18658;}");
    this->comboBox_speakerType->setCursor(Qt::PointingHandCursor);
    this->comboBox_speakerType->setFixedSize(300, 35);
    this->comboBox_speakerType->setGeometry(471, 16, 0, 0);
    if(this->deviceType == "RS520"){
        this->comboBox_speakerType->addItem(tr("  BookShelf Speker"), 0);
        this->comboBox_speakerType->addItem(tr("  TallBoy Speaker"), 1);
        this->comboBox_speakerType->addItem(tr("  Floor Standing Speaker"), 2);
        this->comboBox_speakerType->addItem(tr("  Default Volume Curve"), 3);
    }


    QWidget *DACs32bit = new QWidget(this->config_spk);
    DACs32bit->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    DACs32bit->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    DACs32bit->setGeometry(0, 207, 0, 0);

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


    QWidget *firFilter = new QWidget(this->config_spk);
    firFilter->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    firFilter->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    firFilter->setGeometry(0, 276, 0, 0);

    QLabel *label_firFilter = new QLabel(firFilter);
    label_firFilter->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.65; text-align:center; color:#CCCCCC;");
    label_firFilter->setText("FIR interpoation phase Filter");
    label_firFilter->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label_firFilter->setFixedSize(DLG_WIDTH - 35, 70);
    label_firFilter->setGeometry(35, 0, 0, 0);

    QPushButton *btn_firFilter = new QPushButton(firFilter);
    btn_firFilter->setProperty("type", "firFilter");
    btn_firFilter->setStyleSheet("background-color:transparent; border:none;");
    btn_firFilter->setCursor(Qt::PointingHandCursor);
    btn_firFilter->setFixedSize(30, 30);
    btn_firFilter->setGeometry(310, 20, 0, 0);

    QLabel *img_firFilter = new QLabel(btn_firFilter);
    img_firFilter->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/201/tip_icon.png); border:none;");
    img_firFilter->setFixedSize(30, 30);
    img_firFilter->setGeometry(0, 0, 0, 0);

    this->comboBox_firFilter = new QComboBox(firFilter);
    this->comboBox_firFilter->setObjectName("comboBox_mqaType");
    this->comboBox_firFilter->setStyleSheet("QComboBox {background-color:#212121; border:0px solid #212121; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:left; color:#FFFFFF;}"
                                            "QComboBox QListView {background-color:#212121; border:1px solid #B18658; font-size:18px; font-weight:normal; font-style:normal; line-height:0.89; text-align:center; color:#FFFFFF;}"
                                            "QComboBox QAbstractItemView {selection-color:#FFFFFF; selection-background-color:#B18658; border:1px solid #B18658;}");
    this->comboBox_firFilter->setCursor(Qt::PointingHandCursor);
    this->comboBox_firFilter->setFixedSize(400, 35);
    this->comboBox_firFilter->setGeometry(371, 16, 0, 0);
    if(this->deviceType == "RS520"){
        this->comboBox_firFilter->addItem(tr("  Brick Wall filter"), 0);
        this->comboBox_firFilter->addItem(tr("  Corrected minimum phase Fast Roll-off filter"), 1);
        this->comboBox_firFilter->addItem(tr("  Apodizing Fast Roll-off filter"), 2);
        this->comboBox_firFilter->addItem(tr("  Minimum phase Slow Roll-off filter"), 3);
        this->comboBox_firFilter->addItem(tr("  Minimum phase Fast Roll-off filter"), 4);
        this->comboBox_firFilter->addItem(tr("  Linear phase Slow Roll-off filter"), 5);
        this->comboBox_firFilter->addItem(tr("  Linear phase Fast Roll-off filter"), 6);
    }


    QWidget *pcm_freq = new QWidget(this->config_spk);
    pcm_freq->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    pcm_freq->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT * 2 + 10);
    pcm_freq->setGeometry(0, 345, 0, 0);

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
    if(this->deviceType == "RS520"){
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
    if(this->deviceType == "RS520"){
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


    QWidget *dsd_mode = new QWidget(this->config_spk);
    dsd_mode->setStyleSheet("background-color:transparent; border:1px solid #4D4D4D;");
    dsd_mode->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    dsd_mode->setGeometry(0, 494, 0, 0);

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
    if(this->deviceType == "RS520"){
        this->comboBox_dsdMode->addItem(tr("  Native DSD(upto DSD512)"), 0);
        this->comboBox_dsdMode->addItem(tr("  DSD over PCM(Not supported)"), 1);
        this->comboBox_dsdMode->addItem(tr("  DSD to PCM(upto DSD512)"), 2);
    }


    this->btn_apply = new QPushButton(this->config_spk);
    this->btn_apply->setStyleSheet("background-color:#B18658; border: 1px solid #B18658; border-radius:20px;");
    this->btn_apply->setCursor(Qt::PointingHandCursor);
    this->btn_apply->setFixedSize(140, 40);
    this->btn_apply->setGeometry(245, 599, 0, 0);

    QLabel *label_apply = new QLabel(this->btn_apply);
    label_apply->setStyleSheet("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:1.39; text-align:center; color:#FFFFFF;");
    label_apply->setText(tr("Apply"));
    label_apply->setAlignment(Qt::AlignCenter);
    label_apply->setFixedSize(140, 37);
    label_apply->setGeometry(0, 0, 0, 0);

    this->btn_cancel = new QPushButton(this->config_spk);
    this->btn_cancel->setStyleSheet("background-color:#333333; border: 1px solid #777777; border-radius:20px;");
    this->btn_cancel->setCursor(Qt::PointingHandCursor);
    this->btn_cancel->setFixedSize(140, 40);
    this->btn_cancel->setGeometry(415, 599, 0, 0);

    QLabel *label_cancel = new QLabel(this->btn_cancel);
    label_cancel->setStyleSheet("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:1.39; text-align:center; color:#FFFFFF;");
    label_cancel->setText(tr("Cancel"));
    label_cancel->setAlignment(Qt::AlignCenter);
    label_cancel->setFixedSize(140, 37);
    label_cancel->setGeometry(0, 0, 0, 0);


    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));

    connect(btn_svc, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));
    connect(btn_speakerType, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));
    connect(btn_32bit, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));
    connect(btn_firFilter, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));
    connect(btn_pcm_freq, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));
    connect(btn_dsdMode, SIGNAL(clicked()), this, SLOT(slot_clickedQuestion_clarification()));

    connect(this->btn_mqaStream, SIGNAL(clicked()), this, SLOT(slot_clickedCheckbox()));

    connect(this->btn_apply, SIGNAL(clicked()), this, SLOT(slot_clickedApply()));
    connect(this->btn_cancel, SIGNAL(clicked()), this, SLOT(close()));

    QVBoxLayout *vBox_config_spk = new QVBoxLayout();
    vBox_config_spk->setContentsMargins(0, 0, 0, 0);
    vBox_config_spk->setAlignment(Qt::AlignTop);

    GSCommon::clearLayout(vBox_config_spk);
    vBox_config_spk->setAlignment(Qt::AlignTop);
    vBox_config_spk->addWidget(this->config_spk);

    this->setLayout(vBox_config_spk);
}


void SettingOutputConfig::getOutputSettingConfig(){

    int tmpValue = 0;
    bool tmpbool = false;

    if(this->configType == "etcConfig"){
        if(this->jsonObj_config_Setting.contains("mqaType")){
            tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "mqaType");
            this->comboBox_mqaType->setCurrentIndex(tmpValue);
        }

        if(this->jsonObj_config_Setting.contains("maxSamplingRate")){
            tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "maxSamplingRate");
            this->comboBox_maxSampleRate->setCurrentIndex(tmpValue);
        }

        if(this->jsonObj_config_Setting.contains("samplingRate")){
            tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "samplingRate");
            this->comboBox_sampleRate->setCurrentIndex(tmpValue);
        }

        if(this->jsonObj_config_Setting.contains("mqaResampling")){
            tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "mqaResampling");
            this->checkbox_mqaStream->setChecked(tmpValue == 1 ? true : false);
        }

        if(this->jsonObj_config_Setting.contains("dsdMode")){
            tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "dsdMode");
            this->comboBox_dsdMode->setCurrentIndex(tmpValue);
        }

        if(this->jsonObj_config_Setting.contains("digitalVolume")){
            tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "digitalVolume");
            this->onoff_svc->setValue(tmpValue == 1 ? true : false);
        }

        if(this->jsonObj_config_Setting.contains("isClearLowerBit")){
            tmpbool = ProcJsonEasy::getBool(this->jsonObj_config_Setting, "isClearLowerBit");
            this->onoff_32bit->setValue(tmpbool);
        }

        if(this->config_etc->isVisible() == false){
            this->config_etc->show();
        }
    }
    else if(this->configType == "preConfig"){
        if(this->deviceType == "RS520"){
            if(this->jsonObj_config_Setting.contains("preoutLevelInfo")){
                tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "preoutLevelInfo") + 1;
                this->comboBox_preoutLevel->setCurrentIndex(tmpValue);

                if(tmpValue == 0){
                    this->onoff_pre->setEnabled(false);
                    this->onoff_pre->setValue(false);
                }
                else {
                    this->onoff_pre->setEnabled(true);
                    this->onoff_pre->setValue(true);
                }
            }

            if(this->jsonObj_config_Setting.contains("volumeTable")){
                tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "volumeTable");

                int changeIDX = 0;
                switch (tmpValue) {
                case 0:
                    changeIDX = 0;
                    break;

                case 1:
                    changeIDX = 3;
                    break;

                case 2:
                    changeIDX = 1;
                    break;

                case 3:
                    changeIDX = 2;
                    break;
                }
                this->comboBox_speakerType->setCurrentIndex(changeIDX);
            }

            if(this->jsonObj_config_Setting.contains("digitalVolume")){
                tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "digitalVolume");
                this->onoff_svc->setValue(tmpValue == 1 ? true : false);
            }

            if(this->jsonObj_config_Setting.contains("isClearLowerBit")){
                tmpbool = ProcJsonEasy::getBool(this->jsonObj_config_Setting, "isClearLowerBit");
                this->onoff_32bit->setValue(tmpbool);
            }

            if(this->jsonObj_config_Setting.contains("dacFilterType")){
                tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "dacFilterType");
                this->comboBox_firFilter->setCurrentIndex(tmpValue);
            }

            if(this->jsonObj_config_Setting.contains("maxSamplingRate")){
                tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "maxSamplingRate");
                this->comboBox_maxSampleRate->setCurrentIndex(tmpValue);
            }

            if(this->jsonObj_config_Setting.contains("samplingRate")){
                tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "samplingRate");
                this->comboBox_sampleRate->setCurrentIndex(tmpValue);
            }

            if(this->jsonObj_config_Setting.contains("mqaResampling")){
                tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "mqaResampling");
                this->checkbox_mqaStream->setChecked(tmpValue == 1 ? true : false);
            }

            if(this->jsonObj_config_Setting.contains("dsdMode")){
                tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "dsdMode");
                this->comboBox_dsdMode->setCurrentIndex(tmpValue);
            }
        }

        if(this->onoff_pre->getCurrentValue() == true){
            this->widget_preout_level->setGeometry(0, 69, 0, 0);
            this->widget_speaker_type->setGeometry(0, 138, 0, 0);
            this->widget_speaker_type->hide();
            this->widget_software_volume->setGeometry(0, 138, 0, 0);
            this->widget_DACs32bit->setGeometry(0, 207, 0, 0);
            this->widget_firFilter->setGeometry(0, 276, 0, 0);
            this->widget_pcm_freq->setGeometry(0, 345, 0, 0);
            this->widget_dsd_mode->setGeometry(0, 494, 0, 0);
        }
        else if(this->onoff_pre->getCurrentValue() == false){
            this->widget_preout_level->setGeometry(0, 69, 0, 0);
            this->widget_speaker_type->setGeometry(0, 138, 0, 0);
            this->widget_speaker_type->show();
            this->widget_software_volume->setGeometry(0, 207, 0, 0);
            this->widget_DACs32bit->setGeometry(0, 276, 0, 0);
            this->widget_firFilter->setGeometry(0, 345, 0, 0);
            this->widget_pcm_freq->setGeometry(0, 414, 0, 0);
            this->widget_dsd_mode->setGeometry(0, 563, 0, 0);
        }

        if(this->config_pre->isVisible() == false){
            this->config_pre->show();
        }
    }
    else if(this->configType == "spkConfig"){
        if(this->deviceType == "RS520"){
            if(this->jsonObj_config_Setting.contains("digitalVolume")){
                tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "digitalVolume");
                this->onoff_svc->setValue(tmpValue == 1 ? true : false);
            }

            if(this->jsonObj_config_Setting.contains("volumeTable")){
                tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "volumeTable");

                int changeIDX = 0;
                switch (tmpValue) {
                case 0:
                    changeIDX = 0;
                    break;

                case 1:
                    changeIDX = 3;
                    break;

                case 2:
                    changeIDX = 1;
                    break;

                case 3:
                    changeIDX = 2;
                    break;
                }
                this->comboBox_speakerType->setCurrentIndex(changeIDX);
            }

            if(this->jsonObj_config_Setting.contains("isClearLowerBit")){
                tmpbool = ProcJsonEasy::getBool(this->jsonObj_config_Setting, "isClearLowerBit");
                this->onoff_32bit->setValue(tmpbool);
            }

            if(this->jsonObj_config_Setting.contains("dacFilterType")){
                tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "dacFilterType");
                this->comboBox_firFilter->setCurrentIndex(tmpValue);
            }

            if(this->jsonObj_config_Setting.contains("maxSamplingRate")){
                tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "maxSamplingRate");
                this->comboBox_maxSampleRate->setCurrentIndex(tmpValue);
            }

            if(this->jsonObj_config_Setting.contains("samplingRate")){
                tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "samplingRate");
                this->comboBox_sampleRate->setCurrentIndex(tmpValue);
            }

            if(this->jsonObj_config_Setting.contains("mqaResampling")){
                tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "mqaResampling");
                this->checkbox_mqaStream->setChecked(tmpValue == 1 ? true : false);
            }

            if(this->jsonObj_config_Setting.contains("dsdMode")){
                tmpValue = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "dsdMode");
                this->comboBox_dsdMode->setCurrentIndex(tmpValue);
            }
        }

        if(this->config_spk->isVisible() == false){
            this->config_spk->show();
        }
    }
}


void SettingOutputConfig::setOutputSettingConfig(){

    int tmpInt = 0;
    int tmpCompareInt = 0;

    bool tmpBool = false;
    bool tmpConpareBool = false;

    QJsonObject tmpJson;

    if(this->configType == "etcConfig"){
        this->jsonObj_config_Setting.remove("code");
        this->jsonObj_config_Setting.remove("status");
        this->jsonObj_config_Setting.remove("version");

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "mqaType");
        tmpCompareInt = this->comboBox_mqaType->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("mqaType");
            this->jsonObj_config_Setting.insert("mqaType", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "maxSamplingRate");
        tmpCompareInt = this->comboBox_maxSampleRate->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("maxSamplingRate");
            this->jsonObj_config_Setting.insert("maxSamplingRate", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "samplingRate");
        tmpCompareInt = this->comboBox_sampleRate->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("samplingRate");
            this->jsonObj_config_Setting.insert("samplingRate", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "mqaResampling");
        tmpCompareInt = this->checkbox_mqaStream->isChecked() == true ? 1 : 0;

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("mqaResampling");
            this->jsonObj_config_Setting.insert("mqaResampling", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "dsdMode");
        tmpCompareInt = this->comboBox_dsdMode->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("dsdMode");
            this->jsonObj_config_Setting.insert("dsdMode", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "digitalVolume");
        tmpCompareInt = this->onoff_svc->getCurrentValue() == true ? 1 : 0;

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("digitalVolume");
            this->jsonObj_config_Setting.insert("digitalVolume", tmpCompareInt);
        }

        tmpBool = ProcJsonEasy::getBool(this->jsonObj_config_Setting, "isClearLowerBit");
        tmpConpareBool = this->onoff_32bit->getCurrentValue();

        if(tmpBool != tmpConpareBool){
            this->jsonObj_config_Setting.remove("isClearLowerBit");
            this->jsonObj_config_Setting.insert("isClearLowerBit", tmpConpareBool);
        }

        tmpJson.insert("data", this->jsonObj_config_Setting);
        tmpJson.insert("outputMode", 1);
    }
    else if(this->configType == "preConfig"){
        this->jsonObj_config_Setting.remove("code");
        this->jsonObj_config_Setting.remove("status");
        this->jsonObj_config_Setting.remove("version");

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "preoutLevelInfo");
        tmpCompareInt = this->comboBox_preoutLevel->currentIndex() - 1;

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("preoutLevelInfo");
            this->jsonObj_config_Setting.insert("preoutLevelInfo", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "volumeTable");
        tmpCompareInt = this->comboBox_speakerType->currentIndex();

        int changeIDX = 0;
        switch (tmpCompareInt) {
        case 0:
            changeIDX = 0;
            break;

        case 1:
            changeIDX = 2;
            break;

        case 2:
            changeIDX = 3;
            break;

        case 3:
            changeIDX = 1;
            break;
        }

        if(tmpInt != changeIDX){
            this->jsonObj_config_Setting.remove("volumeTable");
            this->jsonObj_config_Setting.insert("volumeTable", changeIDX);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "digitalVolume");
        tmpCompareInt = this->onoff_svc->getCurrentValue() == true ? 1 : 0;

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("digitalVolume");
            this->jsonObj_config_Setting.insert("digitalVolume", tmpCompareInt);
        }

        tmpBool = ProcJsonEasy::getBool(this->jsonObj_config_Setting, "isClearLowerBit");
        tmpConpareBool = this->onoff_32bit->getCurrentValue();

        if(tmpBool != tmpConpareBool){
            this->jsonObj_config_Setting.remove("isClearLowerBit");
            this->jsonObj_config_Setting.insert("isClearLowerBit", tmpConpareBool);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "dacFilterType");
        tmpCompareInt = this->comboBox_firFilter->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("dacFilterType");
            this->jsonObj_config_Setting.insert("dacFilterType", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "maxSamplingRate");
        tmpCompareInt = this->comboBox_maxSampleRate->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("maxSamplingRate");
            this->jsonObj_config_Setting.insert("maxSamplingRate", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "samplingRate");
        tmpCompareInt = this->comboBox_sampleRate->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("samplingRate");
            this->jsonObj_config_Setting.insert("samplingRate", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "mqaResampling");
        tmpCompareInt = this->checkbox_mqaStream->isChecked() == true ? 1 : 0;

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("mqaResampling");
            this->jsonObj_config_Setting.insert("mqaResampling", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "dsdMode");
        tmpCompareInt = this->comboBox_dsdMode->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("dsdMode");
            this->jsonObj_config_Setting.insert("dsdMode", tmpCompareInt);
        }

        tmpJson.insert("data", this->jsonObj_config_Setting);
        tmpJson.insert("outputMode", 1);
    }
    else if(this->configType == "spkConfig"){
        this->jsonObj_config_Setting.remove("code");
        this->jsonObj_config_Setting.remove("status");
        this->jsonObj_config_Setting.remove("version");

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "digitalVolume");
        tmpCompareInt = this->onoff_svc->getCurrentValue() == true ? 1 : 0;

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("digitalVolume");
            this->jsonObj_config_Setting.insert("digitalVolume", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "volumeTable");
        tmpCompareInt = this->comboBox_speakerType->currentIndex();

        int changeIDX = 0;
        switch (tmpCompareInt) {
        case 0:
            changeIDX = 0;
            break;

        case 1:
            changeIDX = 2;
            break;

        case 2:
            changeIDX = 3;
            break;

        case 3:
            changeIDX = 1;
            break;
        }

        if(tmpInt != changeIDX){
            this->jsonObj_config_Setting.remove("volumeTable");
            this->jsonObj_config_Setting.insert("volumeTable", changeIDX);
        }

        tmpBool = ProcJsonEasy::getBool(this->jsonObj_config_Setting, "isClearLowerBit");
        tmpConpareBool = this->onoff_32bit->getCurrentValue();

        if(tmpBool != tmpConpareBool){
            this->jsonObj_config_Setting.remove("isClearLowerBit");
            this->jsonObj_config_Setting.insert("isClearLowerBit", tmpConpareBool);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "dacFilterType");
        tmpCompareInt = this->comboBox_firFilter->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("dacFilterType");
            this->jsonObj_config_Setting.insert("dacFilterType", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "maxSamplingRate");
        tmpCompareInt = this->comboBox_maxSampleRate->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("maxSamplingRate");
            this->jsonObj_config_Setting.insert("maxSamplingRate", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "samplingRate");
        tmpCompareInt = this->comboBox_sampleRate->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("samplingRate");
            this->jsonObj_config_Setting.insert("samplingRate", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "mqaResampling");
        tmpCompareInt = this->checkbox_mqaStream->isChecked() == true ? 1 : 0;

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("mqaResampling");
            this->jsonObj_config_Setting.insert("mqaResampling", tmpCompareInt);
        }

        tmpInt = ProcJsonEasy::getInt(this->jsonObj_config_Setting, "dsdMode");
        tmpCompareInt = this->comboBox_dsdMode->currentIndex();

        if(tmpInt != tmpCompareInt){
            this->jsonObj_config_Setting.remove("dsdMode");
            this->jsonObj_config_Setting.insert("dsdMode", tmpCompareInt);
        }

        tmpJson.insert("data", this->jsonObj_config_Setting);
        tmpJson.insert("outputMode", 1);
    }

    //    QJsonDocument doc(tmpJson);
    //    QString strJson(doc.toJson(QJsonDocument::Compact));
    //    qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << strJson;

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_CONFIG_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.setting.set"), tmpJson, true);
}


void SettingOutputConfig::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

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
                    this->getOutputSettingConfig();

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
                emit this->signal_setOutput_stateConfig(p_jsonObject);
                emit this->signal_loading_stateConfig(false);

                this->setResult(QDialog::Accepted);
                this->close();
            }
            break;
    }

    sender()->deleteLater();
}


void SettingOutputConfig::slot_clickedQuestion_clarification(){

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
        if(this->configType == "etcConfig"){
            top = this->geometry().top() + (DLG_HEIGHT - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "preConfig"){
            top = this->geometry().top() + (DLG_HEIGHT2 - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "spkConfig"){
            top = this->geometry().top() + (DLG_HEIGHT3 - notice->sizeHint().height()) / 2;
        }
        notice->setGeometry(left, top, 0, 0);
    }
    else if(type == "pcmFreq"){
        notice->setTitle("PCM Resampling Frequency");
        notice->setTextHeight(170);
        notice->setAlignmentContent(Qt::AlignLeft);
        notice->setStyleSheetContent("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:2.52; text-align:center; color:#FFFFFF;");
        notice->setText(tr("This is the menu to set sampling frequecy. if you want to output the original sound as it is,"
                           " select the Original Sampling Rate, and if want to use the resampling function, select the desired sampling rate (48kHz, 96kHz, 192kHz, etc.)."
                           "\nMQA, Native DSD, DOP, Video, Bluetooth and Airplay, Roon Ready are not supported."));

        notice->setFixedDialogSize(660, -1);

        left = this->geometry().left() + (DLG_WIDTH - 660) / 2;
        if(this->configType == "etcConfig"){
            top = this->geometry().top() + (DLG_HEIGHT - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "preConfig"){
            top = this->geometry().top() + (DLG_HEIGHT2 - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "spkConfig"){
            top = this->geometry().top() + (DLG_HEIGHT3 - notice->sizeHint().height()) / 2;
        }
        notice->setGeometry(left, top, 0, 0);
    }
    else if(type == "dsdMode"){
        notice->setTitle("DSD Mode");
        if(global.lang == 1){
            notice->setTextHeight(220);
        }
        else{
            notice->setTextHeight(200);
        }

        notice->setAlignmentContent(Qt::AlignLeft);
        notice->setStyleSheetContent("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:2.52; text-align:center; color:#FFFFFF;");
        notice->setText(tr("Determines the transmission format (Native, DOP, PCM) of audio samples to be delivered to the DAC when playing DSD music."
                           "When in Native mode, analog output supports up to DSD512 when playing DSD, but digital output does not produce sound."
                           "When in DOP mode, analog output does not produce sound and digital output transmits audio data up to DSD256."
                           "DSD to PCM mode supports up to DSD512 and sounds simultaneously from digital and analog outputs."));

        notice->setFixedDialogSize(680, -1);

        left = this->geometry().left() + (DLG_WIDTH - 680) / 2;
        if(this->configType == "etcConfig"){
            top = this->geometry().top() + (DLG_HEIGHT - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "preConfig"){
            top = this->geometry().top() + (DLG_HEIGHT2 - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "spkConfig"){
            top = this->geometry().top() + (DLG_HEIGHT3 - notice->sizeHint().height()) / 2;
        }
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
        if(this->configType == "etcConfig"){
            top = this->geometry().top() + (DLG_HEIGHT - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "preConfig"){
            top = this->geometry().top() + (DLG_HEIGHT2 - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "spkConfig"){
            top = this->geometry().top() + (DLG_HEIGHT3 - notice->sizeHint().height()) / 2;
        }
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
        if(this->configType == "etcConfig"){
            top = this->geometry().top() + (DLG_HEIGHT - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "preConfig"){
            top = this->geometry().top() + (DLG_HEIGHT2 - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "spkConfig"){
            top = this->geometry().top() + (DLG_HEIGHT3 - notice->sizeHint().height()) / 2;
        }
        notice->setGeometry(left, top, 0, 0);
    }
    else if(type == "preoutLevel"){
        notice->setTitle(tr("Pre-out level setting"));
        notice->setTextHeight(110);
        notice->setAlignmentContent(Qt::AlignLeft);
        notice->setStyleSheetContent("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:2.52; text-align:center; color:#FFFFFF;");
        notice->setText(tr("Fixes the pre-out output level. You can use the output level of the ROSE by holding"
                           " the output level of the ROSE as an input within the acceptable range of the"
                           " AMP you connected without usinig the volume control of the ROSE."));

        notice->setFixedDialogSize(660, -1);

        left = this->geometry().left() + (DLG_WIDTH - 660) / 2;
        if(this->configType == "etcConfig"){
            top = this->geometry().top() + (DLG_HEIGHT - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "preConfig"){
            top = this->geometry().top() + (DLG_HEIGHT2 - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "spkConfig"){
            top = this->geometry().top() + (DLG_HEIGHT3 - notice->sizeHint().height()) / 2;
        }
        notice->setGeometry(left, top, 0, 0);
    }
    else if(type == "speakType"){
        notice->setTitle(tr("Speaker Type Selection"));
        notice->setTextHeight(130);
        notice->setAlignmentContent(Qt::AlignLeft);
        notice->setStyleSheetContent("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:2.52; text-align:center; color:#FFFFFF;");
        notice->setText(tr("Set volume table according to output. For speaker output, volume table between -73.5dB to 0dB is used."
                           " For pre-output, volume table between -92.5dB to 0dB is used."
                           " Select and use your preferred speaker output according to the sensitivity of the speaker."));

        notice->setFixedDialogSize(660, -1);

        left = this->geometry().left() + (DLG_WIDTH - 660) / 2;
        if(this->configType == "etcConfig"){
            top = this->geometry().top() + (DLG_HEIGHT - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "preConfig"){
            top = this->geometry().top() + (DLG_HEIGHT2 - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "spkConfig"){
            top = this->geometry().top() + (DLG_HEIGHT3 - notice->sizeHint().height()) / 2;
        }
        notice->setGeometry(left, top, 0, 0);
    }
    else if(type == "firFilter"){
        notice->setTitle("FIR interpoation phase Filter");
        notice->setTextHeight(110);
        notice->setAlignmentContent(Qt::AlignLeft);
        notice->setStyleSheetContent("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:2.52; text-align:center; color:#FFFFFF;");
        notice->setText(tr("Even if a specific filter is selected by the user,"
                           " the 'Minimum phase Slow Roll Off filter' is automatically selected to provide the best audio quality during MQA playback."
                           "\nFIR filters only apply to PCM data, not to DSD"));

        notice->setFixedDialogSize(660, -1);

        left = this->geometry().left() + (DLG_WIDTH - 660) / 2;
        if(this->configType == "etcConfig"){
            top = this->geometry().top() + (DLG_HEIGHT - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "preConfig"){
            top = this->geometry().top() + (DLG_HEIGHT2 - notice->sizeHint().height()) / 2;
        }
        else if(this->configType == "spkConfig"){
            top = this->geometry().top() + (DLG_HEIGHT3 - notice->sizeHint().height()) / 2;
        }
        notice->setGeometry(left, top, 0, 0);
    }

    int result = notice->exec();

    if(result == QDialog::Accepted){

    }

    delete notice;
}


void SettingOutputConfig::slot_clickedCheckbox(){

    if(this->checkbox_mqaStream->isChecked() == true){
        this->checkbox_mqaStream->setChecked(false);
    }
    else if(this->checkbox_mqaStream->isChecked() == false){
        this->checkbox_mqaStream->setChecked(true);
    }
}


void SettingOutputConfig::slot_clickedPreOutLevel(int idx){

    if(idx == 0){
        this->onoff_pre->setEnabled(false);
        this->onoff_pre->setValue(false);
    }
    else{
        this->onoff_pre->setValue(true);
        this->onoff_pre->setEnabled(true);

        this->onoff_svc->setValue(false);
    }

    if(this->onoff_pre->getCurrentValue() == true){
        this->widget_preout_level->setGeometry(0, 69, 0, 0);
        this->widget_speaker_type->setGeometry(0, 138, 0, 0);
        this->widget_speaker_type->hide();
        this->widget_software_volume->setGeometry(0, 138, 0, 0);
        this->widget_DACs32bit->setGeometry(0, 207, 0, 0);
        this->widget_firFilter->setGeometry(0, 276, 0, 0);
        this->widget_pcm_freq->setGeometry(0, 345, 0, 0);
        this->widget_dsd_mode->setGeometry(0, 494, 0, 0);
    }
    else if(this->onoff_pre->getCurrentValue() == false){
        this->widget_preout_level->setGeometry(0, 69, 0, 0);
        this->widget_speaker_type->setGeometry(0, 138, 0, 0);
        this->widget_speaker_type->show();
        this->widget_software_volume->setGeometry(0, 207, 0, 0);
        this->widget_DACs32bit->setGeometry(0, 276, 0, 0);
        this->widget_firFilter->setGeometry(0, 345, 0, 0);
        this->widget_pcm_freq->setGeometry(0, 414, 0, 0);
        this->widget_dsd_mode->setGeometry(0, 563, 0, 0);
    }
}


void SettingOutputConfig::slot_clickedPreOutLevelOnOff(bool flagOn){

    if(flagOn == false){
        this->onoff_pre->setEnabled(flagOn);
        this->comboBox_preoutLevel->setCurrentIndex(0);
    }
    else if(flagOn == true){
        this->onoff_pre->setEnabled(flagOn);

        this->onoff_svc->setValue(false);
    }

    if(this->onoff_pre->getCurrentValue() == true){
        this->widget_preout_level->setGeometry(0, 69, 0, 0);
        this->widget_speaker_type->setGeometry(0, 138, 0, 0);
        this->widget_speaker_type->hide();
        this->widget_software_volume->setGeometry(0, 138, 0, 0);
        this->widget_DACs32bit->setGeometry(0, 207, 0, 0);
        this->widget_firFilter->setGeometry(0, 276, 0, 0);
        this->widget_pcm_freq->setGeometry(0, 345, 0, 0);
        this->widget_dsd_mode->setGeometry(0, 494, 0, 0);
    }
    else if(this->onoff_pre->getCurrentValue() == false){
        this->widget_preout_level->setGeometry(0, 69, 0, 0);
        this->widget_speaker_type->setGeometry(0, 138, 0, 0);
        this->widget_speaker_type->show();
        this->widget_software_volume->setGeometry(0, 207, 0, 0);
        this->widget_DACs32bit->setGeometry(0, 276, 0, 0);
        this->widget_firFilter->setGeometry(0, 345, 0, 0);
        this->widget_pcm_freq->setGeometry(0, 414, 0, 0);
        this->widget_dsd_mode->setGeometry(0, 563, 0, 0);
    }
}


void SettingOutputConfig::slot_clickedSoftwareVolumeOnOff(bool flagOn){

    if(flagOn == false){
        this->onoff_svc->setValue(flagOn);
    }
    else if(flagOn == true){
        this->onoff_svc->setValue(flagOn);

        this->comboBox_preoutLevel->setCurrentIndex(0);
    }
}


void SettingOutputConfig::slot_clickedApply(){

    emit this->signal_loading_stateConfig(true);

    this->setOutputSettingConfig();
}
