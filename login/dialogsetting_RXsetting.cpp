#include "dialogsetting_RXsetting.h"
#include "dialogselect_usbdac.h"
#include "common/gscommon.h"
#include "widget/toastmsg.h"
#include "setting/settinginputoutput.h"
#include <widget/dialogconfirm.h>
#include <common/networkhttp.h>
#include "common/global.h"

#include <common/networkhttp.h>
#include <QLabel>
#include <QRadioButton>
#include <QDebug>
#include <QMessageBox>

#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";



using namespace Dialog;


/**
 * @brief 클라우드 타입 선택 Dlg
 * @param parent
 */
dialogsetting_RXsetting::dialogsetting_RXsetting(QWidget *parent)//c220708
    : QDialog(parent)
{
    setInit();
    setUIControl();
    getHdmiSettingOfsetting();
}

/**
 * @brief DialogSelect_Cloud::setInit: 초기 세팅
 */
void dialogsetting_RXsetting::setInit(){
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT - 300);
    this->setStyleSheet("background-color:transparent;");
   // HdmiDetailData = new QJsonObject();
}

/**
 * @brief DialogSelect_Cloud::setUIControl : UI 세팅
 */
void dialogsetting_RXsetting::setUIControl(){

    QJsonObject tmp_hdmisettingval = get_settingOfHdmiDetail();
    dlgConfirmHdmi = new DialogConfirm(this);
    // 타이틀
    QLabel *lb_title = new QLabel;
    lb_title->setText(tr("RX channel Setting"));//lb_title->setText(tr("클라우드 추가"));
    lb_title->setStyleSheet("font:bold;color:#FFFFFF;font-size:22px;");
    lb_title->setAlignment(Qt::AlignCenter);
    lb_title->setFixedHeight(70);

    QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":images/icon_close_gray.png");
    btn_close->setCursor(Qt::PointingHandCursor);
    btn_close->setFixedSize(QSize(70,70));
    btn_close->setParent(lb_title);
    btn_close->setGeometry(DLG_WIDTH-70, 0, 70, 70);
    btn_close->show();

    QHBoxLayout *box_top = new QHBoxLayout;
    box_top->setContentsMargins(0,0,0,0);
    box_top->setSpacing(0);
    box_top->setAlignment(Qt::AlignTop);
    box_top->addWidget(lb_title);

    QWidget *widget_title = new QWidget;
    widget_title->setObjectName("widget_title");
    widget_title->setLayout(box_top);
    widget_title->setStyleSheet("#widget_title {background-color:transparent;border-bottom:1px solid #404040;}");


    QString radio_style = "QRadioButton {font-size:20px;color:#CCCCCC;spacing:15px;padding:15px;border-bottom:1px solid #404040;}";
    radio_style += " QRadioButton::indicator {width:40px;height:40px;}";
    radio_style += " QRadioButton::indicator::unchecked {image: url(:images/icon_che_off.png);}";
    radio_style += " QRadioButton::indicator::checked {image: url(:images/icon_che_on.png);}";



    QVBoxLayout *vl_total1 = new QVBoxLayout();

    vl_total1->setContentsMargins(0,0,0,0);
    vl_total1->setSpacing(0);
    vl_total1->setAlignment(Qt::AlignTop);

    comboBoxHdmiMQA = new QComboBox();
    comboBoxHdmiMQA->setProperty("btnNo", 0);
    comboBoxHdmiPCM = new QComboBox();
    comboBoxHdmiPCM->setProperty("btnNo", 1);
    comboBoxHdmiPCM_Max = new QComboBox();
    comboBoxHdmiPCM_Max->setProperty("btnNo", 11);
    comboBoxHdmiDSD = new QComboBox();
    comboBoxHdmiDSD->setProperty("btnNo", 2);
    comboBoxHdmiRoon = new QComboBox();
    comboBoxHdmiRoon->setProperty("btnNo", 3);

    comboBoxHdmiMQA->addItem(tr("Authenticator"), 0);
    comboBoxHdmiMQA->addItem(tr("Decoder"), 1);

    QLabel *lb_MQA = new QLabel(tr("MQA type"));
    lb_MQA->setStyleSheet("color:#E5E5E4");
    lb_MQA->setFixedSize(630,70);
    lb_MQA->setText(tr("MQA type"));
    lb_MQA->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");




    comboBoxHdmiMQA->setContentsMargins(0,0,0,0);
    comboBoxHdmiMQA->setFixedHeight(35);
    comboBoxHdmiMQA->setObjectName("comboBox_hdmi");//cheon01
    comboBoxHdmiMQA->setFixedWidth(340);


    comboBoxHdmiMQA->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                );

    QHBoxLayout *hl_lb_MQA = new QHBoxLayout();
    hl_lb_MQA->setContentsMargins(33,0,30,0);
    hl_lb_MQA->setSpacing(0);
    hl_lb_MQA->addWidget(lb_MQA);
    hl_lb_MQA->addWidget(comboBoxHdmiMQA);

    QWidget *widget_total_MQA = new QWidget();
    widget_total_MQA->setObjectName("widget_total");
    widget_total_MQA->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total_MQA->setLayout(hl_lb_MQA);




    comboBoxHdmiPCM->addItem(tr(" Original"), 0);//cheon01_src
    comboBoxHdmiPCM->addItem(tr(" 44.1 kHz"), 1);//cheon01_src
    comboBoxHdmiPCM->addItem(tr(" 48 kHz"), 2);//cheon01_src
    comboBoxHdmiPCM->addItem(tr(" 88.2 kHz"), 3);//cheon01_src
    comboBoxHdmiPCM->addItem(tr(" 96 kHz"), 4);//cheon01_src
    comboBoxHdmiPCM->addItem(tr(" 176.4 kHz"), 5);//cheon01_src
    comboBoxHdmiPCM->addItem(tr(" 192 kHz"), 6);//cheon01_src

    QLabel *lb_PCM = new QLabel(tr("PCM Resampling Frequency"));
    lb_PCM->setStyleSheet("color:#E5E5E4");
    lb_PCM->setFixedSize(250,120);//c220511
    lb_PCM->setText((tr("PCM Resampling Frequency")));
    lb_PCM->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

   // this->list_keyValue.append(0);
   // this->list_keyValue.append(1);


    //comboBox = new QComboBox();
    comboBoxHdmiPCM->setContentsMargins(0,0,0,0);
    comboBoxHdmiPCM->setFixedHeight(35);
    comboBoxHdmiPCM->setObjectName("comboBoxHdmiPCM_Max");//cheon01
    comboBoxHdmiPCM->setFixedWidth(150);


    comboBoxHdmiPCM->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                );

    comboBoxHdmiPCM_Max->addItem(tr(" Auto"), 0);//cheon01_src
    comboBoxHdmiPCM_Max->addItem(tr(" 48 kHz"), 1);//cheon01_src
    comboBoxHdmiPCM_Max->addItem(tr(" 96 kHz"), 2);//cheon01_src
    comboBoxHdmiPCM_Max->addItem(tr(" 192 kHz"), 3);//cheon01_src


    QLabel *lb_PCM_Max = new QLabel(tr("PCM Resampling Frequency"));
    lb_PCM_Max->setStyleSheet("color:#E5E5E4");
    lb_PCM_Max->setFixedSize(250,70);//c220511
    lb_PCM_Max->setText((tr("PCM Resampling Frequency")));
    lb_PCM_Max->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");



    //comboBox = new QComboBox();
    comboBoxHdmiPCM_Max->setContentsMargins(0,0,0,0);
    comboBoxHdmiPCM_Max->setFixedHeight(35);
    comboBoxHdmiPCM_Max->setObjectName("comboBoxHdmiPCM_Max");//cheon01
    comboBoxHdmiPCM_Max->setFixedWidth(150);


    comboBoxHdmiPCM_Max->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                );


    QPushButton *btn_questionPCM;
    btn_questionPCM =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionPCM->setCursor(Qt::PointingHandCursor);
    btn_questionPCM->setFixedSize(QSize(30,30));

    QLabel *lb_title_MaxSample = new QLabel;
    lb_title_MaxSample->setText(tr("Max Sample Rate"));//lb_title->setText(tr("클라우드 추가"));
    lb_title_MaxSample->setStyleSheet("font:bold;color:#AAAAAA;font-size:15px;");
    lb_title_MaxSample->setAlignment(Qt::AlignCenter);
    lb_title_MaxSample->setFixedHeight(20);

    QLabel *lb_title_Resample = new QLabel;
    lb_title_Resample->setText(tr("Resample Rate"));//lb_title->setText(tr("클라우드 추가"));
    lb_title_Resample->setStyleSheet("font:bold;color:#AAAAAA;font-size:15px;");
    lb_title_Resample->setAlignment(Qt::AlignCenter);
    lb_title_Resample->setFixedHeight(20);

    QHBoxLayout *hl_lb_PCM_subMax = new QHBoxLayout();
    hl_lb_PCM_subMax->setContentsMargins(0,0,0,0);
    hl_lb_PCM_subMax->setSpacing(10);
    hl_lb_PCM_subMax->addWidget(lb_title_MaxSample);
    hl_lb_PCM_subMax->addWidget(comboBoxHdmiPCM_Max);//c220511

    QHBoxLayout *hl_lb_PCM_subRe = new QHBoxLayout();
    hl_lb_PCM_subRe->setContentsMargins(0,0,0,0);
    hl_lb_PCM_subRe->setSpacing(10);
    hl_lb_PCM_subRe->addWidget(lb_title_Resample);
    hl_lb_PCM_subRe->addWidget(comboBoxHdmiPCM);//c220511


    QVBoxLayout *vl_lb_PCM = new QVBoxLayout();
    vl_lb_PCM->setContentsMargins(33,0,0,0);
    vl_lb_PCM->setSpacing(0);
    vl_lb_PCM->addLayout(hl_lb_PCM_subMax);
    vl_lb_PCM->addLayout(hl_lb_PCM_subRe);

    QWidget *widget_PCM_and_Max = new QWidget();
    widget_PCM_and_Max->setObjectName("widget_PCM_and_Max");
    widget_PCM_and_Max->setStyleSheet("#widget_PCM_and_Max { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_PCM_and_Max->setLayout(vl_lb_PCM);

    QHBoxLayout *hl_lb_PCM = new QHBoxLayout();
    hl_lb_PCM->setContentsMargins(33,0,30,0);
    hl_lb_PCM->setSpacing(20);
    hl_lb_PCM->addWidget(lb_PCM);
    hl_lb_PCM->addWidget(btn_questionPCM);//c220511
    hl_lb_PCM->addWidget(widget_PCM_and_Max, 0, Qt::AlignRight);//c220511
    //hl_lb_PCM->addWidget(comboBoxHdmiPCM);

    QWidget *widget_total_PCM = new QWidget();
    widget_total_PCM->setObjectName("widget_total");
    widget_total_PCM->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total_PCM->setLayout(hl_lb_PCM);


   // vl_total1->addWidget(this->getUIControlOption(tr("PCM Resampling Frequency"), comboBoxHdmiPCM));//cheon01

    comboBoxHdmiDSD->addItem(tr("Native DSD(Not suported)"), 0);//c220511
    comboBoxHdmiDSD->addItem(tr("DSD over PCM(Not suported)"), 1);//c220511
    comboBoxHdmiDSD->addItem(tr("DSD to PCM(upto DSD128)"), 2);//c220511

    QPushButton *btn_questionDSD;
    btn_questionDSD =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionDSD->setCursor(Qt::PointingHandCursor);
    btn_questionDSD->setFixedSize(QSize(30,30));

    QLabel *lb_DSD = new QLabel(tr("DSD Mode"));
    lb_DSD->setStyleSheet("color:#E5E5E4");
    lb_DSD->setFixedSize(100,70);//c220511
    lb_DSD->setText((tr("DSD Mode")));
    lb_DSD->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

   // this->list_keyValue.append(0);
   // this->list_keyValue.append(1);


    //comboBox = new QComboBox();
    comboBoxHdmiDSD->setContentsMargins(0,0,0,0);
    comboBoxHdmiDSD->setFixedHeight(35);
    comboBoxHdmiDSD->setObjectName("comboBox_hdmi");//cheon01
    comboBoxHdmiDSD->setFixedWidth(340);


    comboBoxHdmiDSD->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                //"QAbstractItemView QAbstractItemView::item {  padding-left:5px; } "
                                //"QComboBox QAbstractItemView:selected { color:red;selection-background-color:red } "
                                //"QComboBox QAbstractItemView:hover { color:red;selection-background-color:red } "
                                );

    QHBoxLayout *hl_lb_DSD = new QHBoxLayout();
    hl_lb_DSD->setContentsMargins(33,0,30,0);
    hl_lb_DSD->setSpacing(0);
    hl_lb_DSD->addWidget(lb_DSD);
    hl_lb_DSD->addWidget(btn_questionDSD);//c220511
    hl_lb_DSD->addWidget(comboBoxHdmiDSD, 0, Qt::AlignRight);//c220511


    QWidget *widget_total_DSD = new QWidget();
    widget_total_DSD->setObjectName("widget_total");
    widget_total_DSD->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total_DSD->setLayout(hl_lb_DSD);


  //  vl_total1->addWidget(this->getUIControlOption(tr("DSD Mode"), comboBoxHdmiDSD));//cheon01

    QPushButton *btn_questionRoon;//c220511
    btn_questionRoon =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionRoon->setCursor(Qt::PointingHandCursor);
    btn_questionRoon->setFixedSize(QSize(30,30));

    comboBoxHdmiRoon->addItem(tr("Off"), 0);
    comboBoxHdmiRoon->addItem(tr("On"), 1);

    QLabel *lb_Roon = new QLabel(tr("Software Volume Control"));//c220511
    lb_Roon->setStyleSheet("color:#E5E5E4");
    lb_Roon->setFixedSize(230,70);
    lb_Roon->setText((tr("Software Volume Control")));
    lb_Roon->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

   // this->list_keyValue.append(0);
   // this->list_keyValue.append(1);


    //comboBox = new QComboBox();
    comboBoxHdmiRoon->setContentsMargins(0,0,0,0);
    comboBoxHdmiRoon->setFixedHeight(35);
    comboBoxHdmiRoon->setObjectName("comboBox_hdmi");//cheon01
    comboBoxHdmiRoon->setFixedWidth(340);//c220511


    comboBoxHdmiRoon->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                //"QAbstractItemView QAbstractItemView::item {  padding-left:5px; } "
                                //"QComboBox QAbstractItemView:selected { color:red;selection-background-color:red } "
                                //"QComboBox QAbstractItemView:hover { color:red;selection-background-color:red } "
                                );

    QHBoxLayout *hl_lb_Roon = new QHBoxLayout();
    hl_lb_Roon->setContentsMargins(33,0,30,0);
    hl_lb_Roon->setSpacing(20);
    hl_lb_Roon->addWidget(lb_Roon);
    hl_lb_Roon->addWidget(btn_questionRoon);//c220511
    hl_lb_Roon->addWidget(comboBoxHdmiRoon, 0, Qt::AlignRight);//c220511
    //hl_lb_Roon->addWidget(comboBoxHdmiRoon);

    QWidget *widget_total_Roon = new QWidget();
    widget_total_Roon->setObjectName("widget_total");
    widget_total_Roon->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total_Roon->setLayout(hl_lb_Roon);

    vl_total1->addWidget(widget_total_MQA);//cheon01
    vl_total1->addWidget(widget_total_PCM);//cheon01
    vl_total1->addWidget(widget_total_DSD);//cheon01
    vl_total1->addWidget(widget_total_Roon);//cheon01
  //  vl_total1->addWidget(this->getUIControlOption(tr("Roon Volume Control"), comboBoxHdmiRoon));//cheon01


    QVBoxLayout *vl_total_Pass = new QVBoxLayout();
    vl_total_Pass->setContentsMargins(0,0,0,0);
    vl_total_Pass->setSpacing(0);
    vl_total_Pass->setAlignment(Qt::AlignTop);
    //vl_total_Pass->addWidget(GSCommon::getUIControlTitleForSetting(tr("Video Option")));
    vl_total_Pass->addWidget(this->getUIControlOption_Pass(tr("HDMI Passthrough")));//print_debug();
    //onOff[0]->setValue(tmp_hdmisettingval["isHdmiPassThrough"].toBool());

    QVBoxLayout *vl_total_32 = new QVBoxLayout();//---------------------------
    vl_total_32->setContentsMargins(0,0,0,0);
    vl_total_32->setSpacing(0);
    vl_total_32->setAlignment(Qt::AlignTop);
    //vl_total_Pass->addWidget(GSCommon::getUIControlTitleForSetting(tr("Video Option")));
    vl_total_32->addWidget(this->getUIControlOption_bit32(tr("RX channel information is not used")));//print_debug();
    //onOff[1]->setValue(tmp_hdmisettingval["isClearLowerBit"].toBool());

    QVBoxLayout *vl_total_mqagain = new QVBoxLayout();
    vl_total_mqagain->setContentsMargins(0,0,0,0);
    vl_total_mqagain->setSpacing(0);
    vl_total_mqagain->setAlignment(Qt::AlignTop);

    mqadBData = tmp_hdmisettingval["mqaRelplayGain"].toInt();
    QWidget *widget_mqaGain = this->getPlusMinusUIControlOption(tr("MQA Replay Gain"),0, mqadBData);
    vl_total_mqagain->addWidget(widget_mqaGain);

    QVBoxLayout *vl_total_outputgain = new QVBoxLayout();
    vl_total_outputgain->setContentsMargins(0,0,0,0);
    vl_total_outputgain->setSpacing(0);
    vl_total_outputgain->setAlignment(Qt::AlignTop);

    outputdBData = tmp_hdmisettingval["outputGain"].toInt();
    QWidget *widget_outputGain = this->getPlusMinusUIControlOption(tr("Output Gain"), 1,outputdBData);
    vl_total_outputgain->addWidget(widget_outputGain);

    // 하단 버튼
    QPushButton *btn_ok = new QPushButton;
    btn_ok->setText(tr("Apply"));//btn_ok->setText(tr("추가"));
    btn_ok->setCursor(Qt::PointingHandCursor);
    btn_ok->setStyleSheet("font-size:18px;color:#FFFFFF;background-color:#B18658;border-radius:20px;");
    btn_ok->setFixedSize(140,40);
    QPushButton *btn_cancel = new QPushButton;
    btn_cancel->setText(tr("Cancel"));//btn_cancel->setText(tr("취소"));
    btn_cancel->setCursor(Qt::PointingHandCursor);
    btn_cancel->setStyleSheet("font-size:18px;color:#CCCCCC;border:1px solid #777777;border-radius:20px;");
    btn_cancel->setFixedSize(140,40);
    QHBoxLayout *hl_btn = new QHBoxLayout();
    hl_btn->setContentsMargins(0,0,0,30);
    hl_btn->setSpacing(0);
    hl_btn->setAlignment(Qt::AlignCenter);
    hl_btn->addWidget(btn_ok);
    hl_btn->addSpacing(30);
    hl_btn->addWidget(btn_cancel);
    notice_msg ="";

    lb_notice = new QLabel(tr("box_notice"));
    lb_notice->setStyleSheet("color:#E5E5E4");
    lb_notice->setFixedSize(630,140);
    lb_notice->setText(tr("- Notice - ") + notice_msg);
    lb_notice->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    QHBoxLayout *box_notice = new QHBoxLayout();
    box_notice->setContentsMargins(33,0,30,0);
    box_notice->setSpacing(0);
    box_notice->addWidget(lb_notice);

    widget_box_notice = new QWidget();
    widget_box_notice->setObjectName("widget_box_notice");
    widget_box_notice->setStyleSheet("#widget_box_notice {background-color:transparent;border:1px solid #404040;} ");
   // widget_box_notice->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_box_notice->setLayout(box_notice);


    QVBoxLayout *vl_total2 = new QVBoxLayout();
    vl_total2->setContentsMargins(0,0,0,0);
    vl_total2->setSpacing(0);
    vl_total2->setAlignment(Qt::AlignTop);
    vl_total2->addWidget(widget_title);

    //vl_total2->addLayout(vl_total1);
    //vl_total2->addLayout(vl_total_Pass);//print_debug();
    vl_total2->addLayout(vl_total_32);
    //vl_total2->addLayout(vl_total_outputgain);
  //  vl_total2->addWidget(widget_box_notice);//print_debug();
    vl_total2->addSpacing(30);//200

    vl_total2->addLayout(hl_btn);

print_debug();


    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");

    widget_total->setLayout(vl_total2);//print_debug();
    widget_total->setStyleSheet("#widget_total { background-color:#333333;border-radius:4px;border:1px solid #4D4D4D; }");
    QVBoxLayout *vboxlayout = new QVBoxLayout();
    vboxlayout->setContentsMargins(0,0,0,0);
    vboxlayout->setSpacing(0);
    vboxlayout->addWidget(widget_total);//print_debug();

    this->setLayout(vboxlayout);//print_debug();

    // 커넥션

    connect(btn_questionDSD, SIGNAL(clicked()), this, SLOT(slot_clickedDSDquestion()));//c220511
    connect(btn_questionPCM, SIGNAL(clicked()), this, SLOT(slot_clickedPCMquestion()));//c220511
    connect(btn_questionRoon, SIGNAL(clicked()), this, SLOT(slot_clickedRXquestion()));//c220511


    //connect(comboBoxHdmiMQA, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexHdmi(int)));
    //connect(comboBoxHdmiPCM, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexHdmi(int)));
    //connect(comboBoxHdmiPCM_Max, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexHdmi(int)));
    //connect(comboBoxHdmiDSD, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexHdmi(int)));
    //connect(comboBoxHdmiRoon, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexHdmi(int)));

    //connect(lb_onOfftitle, SIGNAL(clicked()), this, SLOT(slot_clickedHdmipassGain()));
    //connect(lb_gain[0], SIGNAL(clicked()), this, SLOT(slot_clickedHdmimqaGain()));
    //connect(lb_gain[1], SIGNAL(clicked()), this, SLOT(slot_clickedHdmioutputGain()));

    //connect(btn_gain_add[0], SIGNAL(clicked()), this, SLOT(slot_clickedHdmiaddmqaGain()));
    //connect(btn_gain_sub[0], SIGNAL(clicked()), this, SLOT(slot_clickedHdmisubmqaGain()));
    //connect(btn_gain_add[1], SIGNAL(clicked()), this, SLOT(slot_clickedHdmiaddoutputGain()));
    //connect(btn_gain_sub[1], SIGNAL(clicked()), this, SLOT(slot_clickedHdmisuboutputGain()));

    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(close()));//print_debug();
    connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clickedUsbChangeSet()));

}

void dialogsetting_RXsetting::slot_clickedDSDquestion(){//c220511
    print_debug();
    dlgConfirmHdmi->setTitle(tr("DSD Mode"));
    dlgConfirmHdmi->setText(tr("Determines the transmission format (Native, DOP, PCM) of audio samples to be transmitted to the DAC when playing DSD sound sources. \n For HDMI output, only DSD to PCM is available."));

    dlgConfirmHdmi->setGeometry((DLG_WIDTH + 80), (350 + 100), 350, 500);
    dlgConfirmHdmi->setAlertMode();
    dlgConfirmHdmi->setProperty("flagShown",false);

    if(dlgConfirmHdmi->property("flagShown").toBool()==false){
        dlgConfirmHdmi->setProperty("flagShown",true);

        int result = dlgConfirmHdmi->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}
void dialogsetting_RXsetting::slot_clickedPCMquestion(){//c220511
    print_debug();
    dlgConfirmHdmi->setTitle(tr("PCM Resampling Frequency"));
    dlgConfirmHdmi->setText(tr("This is The menu to set sampling frequecy. if you want to output the original sound as it is, select the Original Sampling Rate, and if want to use the resampling function, select the desired sampling rate (48kHz, 96Khz, 192Khz, etc), MQA, Native DSD, DOP, Video, Bluetooth and Airplay, Roon Ready are not supported."));

    dlgConfirmHdmi->setGeometry((DLG_WIDTH + 80), (350 + 100), 350, 500);
    dlgConfirmHdmi->setAlertMode();
    dlgConfirmHdmi->setProperty("flagShown",false);

    if(dlgConfirmHdmi->property("flagShown").toBool()==false){
        dlgConfirmHdmi->setProperty("flagShown",true);

        int result = dlgConfirmHdmi->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}
void dialogsetting_RXsetting::slot_clickedPassquestion(){//c220511
    print_debug();
    dlgConfirmHdmi->setTitle(tr("HDMI Passthough"));
    dlgConfirmHdmi->setText(tr("it bypasses HD audio formats such as surround sound sources to the audio reciever though HDMI without decording."));

    dlgConfirmHdmi->setGeometry((DLG_WIDTH + 80), (350 + 100), 350, 500);
    dlgConfirmHdmi->setAlertMode();
    dlgConfirmHdmi->setProperty("flagShown",false);

    if(dlgConfirmHdmi->property("flagShown").toBool()==false){
        dlgConfirmHdmi->setProperty("flagShown",true);

        int result = dlgConfirmHdmi->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}
void dialogsetting_RXsetting::slot_clickedRXquestion(){//c220511
    print_debug();
    dlgConfirmHdmi->setAlignment(Qt::AlignLeft);
    //dlgConfirmHdmi->setTitle(tr("RX channel information is not used."));
    //dlgConfirmHdmi->setText(tr("When using digital input, RX channel information is not used."));
    dlgConfirmHdmi->setAlignment(Qt::AlignLeft);
    dlgConfirmHdmi->setTitle(tr("RX channel information is not used"));
    dlgConfirmHdmi->setText(tr("When using digital input, RX channel information is not used."));

    dlgConfirmHdmi->setGeometry((DLG_WIDTH + 80), (350 + 100), 350, 500);
    dlgConfirmHdmi->setAlertMode();
    dlgConfirmHdmi->setProperty("flagShown",false);

    if(dlgConfirmHdmi->property("flagShown").toBool()==false){
        dlgConfirmHdmi->setProperty("flagShown",true);

        int result = dlgConfirmHdmi->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}
void dialogsetting_RXsetting::slot_clickedbit32question(){//c220511
    print_debug();
    dlgConfirmHdmi->setAlignment(Qt::AlignLeft);
    //dlgConfirmHdmi->setTitle(tr("Headphone Output Impedance"));
    //dlgConfirmHdmi->setText(tr("This is a menu to set the Headphone Output Impedace.\nIf you select the output (16 Ohm, 32 Ohm, 50 Ohm, 100 Ohm) that fits your headphone specifications, you can hear better sound quality."));
    dlgConfirmHdmi->setAlignment(Qt::AlignLeft);
    dlgConfirmHdmi->setTitle(tr("The low 8bits of a 32 bit sound sources\n will have correct"));
    dlgConfirmHdmi->setText(tr("Some DACs may generate noise when playing a 32-bit sound source with a high sampling rate.\nIn this case, it is used to reduce noise by correcting the lower 8 bits."));

    dlgConfirmHdmi->setGeometry((DLG_WIDTH + 80), (350 + 100), 350, 500);
    dlgConfirmHdmi->setAlertMode();
    dlgConfirmHdmi->setProperty("flagShown",false);

    if(dlgConfirmHdmi->property("flagShown").toBool()==false){
        dlgConfirmHdmi->setProperty("flagShown",true);

        int result = dlgConfirmHdmi->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}
void dialogsetting_RXsetting::slot_clickedRoonquestion(){//c220511
    print_debug();
    dlgConfirmHdmi->setTitle(tr("Software Volume Control"));
    dlgConfirmHdmi->setText(tr("Adjust the playback volume of digital audio data using the Rose volume control software.\nCaution 1. It cannot be used in Native DSD or DSD over PCM mode. It is applied after changing to DSD to PCM(upto DSD128) mode.\nCaution 2. For digital output. MQA Stream is supported only when the volume is MAX. If it is less than MAX. MQA Stream is not supported."));
    dlgConfirmHdmi->setTextHeight(250);
    dlgConfirmHdmi->setAlignment(Qt::AlignLeft);
    dlgConfirmHdmi->setGeometry((DLG_WIDTH + 80), (350 + 100), 350, 500);
    dlgConfirmHdmi->setAlertMode();
    dlgConfirmHdmi->setProperty("flagShown",false);

    if(dlgConfirmHdmi->property("flagShown").toBool()==false){
        dlgConfirmHdmi->setProperty("flagShown",true);

        int result = dlgConfirmHdmi->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}
void dialogsetting_RXsetting::slot_clickedHdmioutputGain(){
    dlgHdmiMqa();
    QTimer::singleShot(500, this, SLOT(slot_showDlgOfHdmiNotice()));
}


void dialogsetting_RXsetting::slot_clickedHdmimqaGain(){
    dlgHdmiOutput();
    QTimer::singleShot(500, this, SLOT(slot_showDlgOfHdmiNotice()));
}


void dialogsetting_RXsetting::slot_clickedHdmipassGain(){
    dlgHdmiPass();
    QTimer::singleShot(500, this, SLOT(slot_showDlgOfHdmiNotice()));
}


void dialogsetting_RXsetting::dlgHdmiMqa(){


    dlgConfirmHdmi->setTitle(tr("HDMI MQA Replay Gain Notice"));
    dlgConfirmHdmi->setText(tr("Activates Replay Gaon Control when playing MQA. \nSet the desired leveling value.\nThese values apply only to MQA sound sources."));
    //dlgConfirmHdmi->setText(tr("MQA 재생시 Replay Gaon Control을 활성화합니다. \n원하는 평준화값을 설정하시오.\n해당 값은 MQA  음원에만 적용됩니다."));

}
void dialogsetting_RXsetting::dlgHdmiOutput(){


    dlgConfirmHdmi->setTitle(tr("HDMI Output Gain Notice"));
    dlgConfirmHdmi->setText(tr("Adjusts the Digital Output Gain. \nExternal receiver Or, if distortion occurs when connecting to a DAC, adjust the Output Gain value appropriately."));
    //dlgConfirmHdmi->setText(tr("Digital Output의 Gain 값을 조정합니다. \n외부 리시버 또는 DAC과 연결 시 distortion이 발생하면 Output Gain 값을 적절히 조절하여 사용합니다."));

}

void dialogsetting_RXsetting::dlgHdmiPass(){


    dlgConfirmHdmi->setTitle(tr("HDMI Passthrough Notice"));
    //dlgConfirmHdmi->setText(tr("디코딩없이 HDMI를 통해 \ n 오디오 수신기로 서라운드 음원과 같은 HD 오디오 형식을 우회합니다."));
    dlgConfirmHdmi->setText(tr("It bypasses HD audio formats such as surround sound sources to\n the audio receiver through HDMI without decoding."));

}


void dialogsetting_RXsetting::slot_showDlgOfHdmiNotice(){

    dlgConfirmHdmi->setTitle(tr("HDMI Passthrough Notice"));
    //dlgConfirmHdmi->setText(tr("디코딩없이 HDMI를 통해 \ n 오디오 수신기로 서라운드 음원과 같은 HD 오디오 형식을 우회합니다."));
    dlgConfirmHdmi->setText(tr("It bypasses HD audio formats such as surround sound sources to\n the audio receiver through HDMI without decoding."));

    dlgConfirmHdmi->setGeometry((DLG_WIDTH + 480), (350 + 100), 350, 300);
    dlgConfirmHdmi->setAlertMode();
    dlgConfirmHdmi->setProperty("flagShown",false);

    if(dlgConfirmHdmi->property("flagShown").toBool()==false){
        dlgConfirmHdmi->setProperty("flagShown",true);

        int result = dlgConfirmHdmi->exec();

        if(result == QDialog::Accepted){
            //showRemote(true);
            qDebug() << " selectedHdmiPass_str: " << this->onOff_Pass->getCurrentValue();;
           // HdmiDetailData.insert( "isHdmiPassThrough", this->onOff->getCurrentValue());
            //QJsonDocument doc(HdmiDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_showDlgOfHdmiPass()-Hdmi_json =: " << strJson;

        }
    }
}
/*
void dialogsetting_hdmi::slot_showDlgOfHdmiNotice(){

    dlgConfirmHdmi->setTitle(tr("HDMI Passthrough Notice"));
    //dlgConfirmHdmi->setText(tr("디코딩없이 HDMI를 통해 \ n 오디오 수신기로 서라운드 음원과 같은 HD 오디오 형식을 우회합니다."));
    dlgConfirmHdmi->setText(tr("It bypasses HD audio formats such as surround sound sources to\n the audio receiver through HDMI without decoding."));

    dlgConfirmHdmi->setGeometry((DLG_WIDTH + 480), (350 + 100), 350, 300);
    dlgConfirmHdmi->setAlertMode();
    dlgConfirmHdmi->setProperty("flagShown",false);

    if(dlgConfirmHdmi->property("flagShown").toBool()==false){
        dlgConfirmHdmi->setProperty("flagShown",true);

        int result = dlgConfirmHdmi->exec();

        if(result == QDialog::Accepted){
            //showRemote(true);
            qDebug() << " selectedHdmiPass_str: " << this->onOff_Pass->getCurrentValue();;
           // HdmiDetailData.insert( "isHdmiPassThrough", this->onOff->getCurrentValue());
            //QJsonDocument doc(HdmiDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_showDlgOfHdmiPass()-Hdmi_json =: " << strJson;

        }
    }
}
*/

void dialogsetting_RXsetting::set_settingOfHdmiDetail(QJsonObject tmpJson){
        HdmiDetailData = tmpJson;
}
QJsonObject dialogsetting_RXsetting::get_settingOfHdmiDetail(){
        return HdmiDetailData;
}

void dialogsetting_RXsetting::slot_changedIndexHdmi(int index){

    print_debug();
    int btnSenderNo = sender()->property("btnNo").toInt();
    QString selectedHdmiMQA_str;
    qDebug() << "----------------isHdmiPassThrough :" <<  HdmiDetailData["isHdmiPassThrough"].toBool();
 //   QJsonObject Hdmi_json = get_settingOfHdmiDetail();
    QJsonDocument doc0(HdmiDetailData);  QString strJson0(doc0.toJson(QJsonDocument::Compact));  qDebug() << "slot_changedIndexHdmi()pre-Hdmi_json =: " << strJson0;

    switch(btnSenderNo){
    case  0 :
        selectedHdmiMQA_str = comboBoxHdmiMQA->itemText(index);
        print_debug();
        //qDebug() << " samplingRate_index: " << index;
        HdmiDetailData.insert( "mqaType", index);
        break;
    case  1 :
        selectedHdmiMQA_str = comboBoxHdmiPCM->itemText(index);
        print_debug();
        qDebug() << " samplingRate: " << selectedHdmiMQA_str;
        HdmiDetailData.insert( "samplingRate", index);

        //for(int i = 0 ; i < 7; i ++){
        //    comboBoxHdmiPCM->removeItem(i);
        //}



        break;
    case  11 :
        selectedHdmiMQA_str = comboBoxHdmiPCM_Max->itemText(index);
        print_debug();
        qDebug() << " MaxSamplingRate_index: " << index;
        HdmiDetailData.insert( "maxSamplingRate", index);
        //comboBoxHdmiPCM->clear();
        if(index == 0){
            comboBoxHdmiPCM->setMaxVisibleItems(7);
            /*
            comboBoxHdmiPCM->addItem(tr("Original"), 0);//cheon01_src
            comboBoxHdmiPCM->addItem(tr("44.1 kHz"), 1);//cheon01_src
            comboBoxHdmiPCM->addItem(tr("48 kHz"), 2);//cheon01_src
            comboBoxHdmiPCM->addItem(tr("88.2 kHz"), 3);//cheon01_src
            comboBoxHdmiPCM->addItem(tr(" 96 kHz"), 4);//cheon01_src
            comboBoxHdmiPCM->addItem(tr(" 176.4 kHz"), 5);//cheon01_src
            comboBoxHdmiPCM->addItem(tr(" 192 kHz"), 6);//cheon01_src
            */
        }
        if(index == 1){
            comboBoxHdmiPCM->setMaxVisibleItems(3);
            /*
            comboBoxHdmiPCM->addItem(tr("Original"), 0);//cheon01_src
            comboBoxHdmiPCM->addItem(tr("44.1 kHz"), 1);//cheon01_src
            comboBoxHdmiPCM->addItem(tr("48 kHz"), 2);//cheon01_src
            */

        }
        if(index == 2){
            comboBoxHdmiPCM->setMaxVisibleItems(5);
            /*
            comboBoxHdmiPCM->addItem(tr("Original"), 0);//cheon01_src
            comboBoxHdmiPCM->addItem(tr("44.1 kHz"), 1);//cheon01_src
            comboBoxHdmiPCM->addItem(tr("48 kHz"), 2);//cheon01_src
            comboBoxHdmiPCM->addItem(tr("88.2 kHz"), 3);//cheon01_src
            comboBoxHdmiPCM->addItem(tr(" 96 kHz"), 4);//cheon01_src
            */
        }
        if(index == 3){
            comboBoxHdmiPCM->setMaxVisibleItems(7);
            /*
            comboBoxHdmiPCM->addItem(tr("Original"), 0);//cheon01_src
            comboBoxHdmiPCM->addItem(tr("44.1 kHz"), 1);//cheon01_src
            comboBoxHdmiPCM->addItem(tr("48 kHz"), 2);//cheon01_src
            comboBoxHdmiPCM->addItem(tr("88.2 kHz"), 3);//cheon01_src
            comboBoxHdmiPCM->addItem(tr(" 96 kHz"), 4);//cheon01_src
            comboBoxHdmiPCM->addItem(tr(" 176.4 kHz"), 5);//cheon01_src
            comboBoxHdmiPCM->addItem(tr(" 192 kHz"), 6);//cheon01_src
            */
        }


        break;
    case  2 :
        selectedHdmiMQA_str = comboBoxHdmiDSD->itemText(index);
        qDebug() << " dsdMode: " << selectedHdmiMQA_str;
        HdmiDetailData.insert( "dsdMode", index);
        break;
    case  3 :
        selectedHdmiMQA_str = comboBoxHdmiRoon->itemText(index);
        qDebug() << " selectedHdmi_str: " << selectedHdmiMQA_str;
        HdmiDetailData.insert( "digitalVolume", index);
        break;




    }
    QJsonDocument doc(HdmiDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_changedIndexHdmi()next_Hdmi_json =: " << strJson;



}

QWidget* dialogsetting_RXsetting::getUIControlOption_Pass(QString p_title){

    QPushButton *lb_onOfftitle = new QPushButton(p_title);
    lb_onOfftitle->setProperty("btnNo", 4);
    lb_onOfftitle->setStyleSheet("color:#E5E5E4;");
    lb_onOfftitle->setFixedSize(200,70);
    lb_onOfftitle->setText(p_title);
    lb_onOfftitle->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");
    lb_onOfftitle->setCursor(Qt::PointingHandCursor);//print_debug();
    onOff_Pass = new OnOffWidget;print_debug()
    //onOff->setValue(false);
    onOff_Pass->setCursor(Qt::PointingHandCursor);//print_debug();

    QPushButton *btn_question;
    btn_question =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_question->setCursor(Qt::PointingHandCursor);
    btn_question->setFixedSize(QSize(30,30));
    //btn_question->addLayout(btn_question);

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,0,30,0);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_onOfftitle);
    hl_lb->addWidget(btn_question);
    hl_lb->addWidget(onOff_Pass, 0, Qt::AlignRight);
    print_debug();
    QPushButton *btn_onOffHover = new QPushButton(onOff_Pass);//print_debug();
    btn_onOffHover->setProperty("btnNo", 4);
    btn_onOffHover->setFixedSize(80,40);
    btn_onOffHover->setStyleSheet("background-color:transparent;border:0px;");

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");//print_debug();
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);
    print_debug();

    // 커넥션
    connect(btn_question, SIGNAL(clicked()), this, SLOT(slot_clickedPassquestion()));

    //connect(lb_onOfftitle, SIGNAL(clicked()), this, SLOT(slot_modeOnOff()));
    connect(btn_onOffHover, SIGNAL(clicked()), this, SLOT(slot_modeOnOff_drc()));
    return widget_total;

}

QWidget* dialogsetting_RXsetting::getUIControlOption_bit32(QString p_title){

    QPushButton *lb_onOfftitle = new QPushButton(p_title);
    lb_onOfftitle->setProperty("btnNo", 4);
    lb_onOfftitle->setStyleSheet("color:#E5E5E4;");
    lb_onOfftitle->setFixedSize(500,70);
    lb_onOfftitle->setText(p_title);
    lb_onOfftitle->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");
    lb_onOfftitle->setCursor(Qt::PointingHandCursor);//print_debug();
    onOff_RX = new OnOffWidget;
    //onOff->setValue(false);
    onOff_RX->setCursor(Qt::PointingHandCursor);//print_debug();

    QPushButton *btn_question;
    btn_question =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_question->setCursor(Qt::PointingHandCursor);
    btn_question->setFixedSize(QSize(30,30));
    //btn_question->addLayout(btn_question);

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,0,30,0);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_onOfftitle);
    hl_lb->addWidget(btn_question, 0, Qt::AlignLeft);
    hl_lb->addWidget(onOff_RX, 0, Qt::AlignRight);
    print_debug();
    QPushButton *btn_onOffHover = new QPushButton(onOff_RX);//print_debug();
    btn_onOffHover->setProperty("btnNo", 4);
    btn_onOffHover->setFixedSize(80,40);
    btn_onOffHover->setStyleSheet("background-color:transparent;border:0px;");

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");//print_debug();
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);
    print_debug();

    // 커넥션
    connect(btn_question, SIGNAL(clicked()), this, SLOT(slot_clickedRXquestion()));

    //connect(lb_onOfftitle, SIGNAL(clicked()), this, SLOT(slot_modeOnOff()));
    connect(btn_onOffHover, SIGNAL(clicked()), this, SLOT(slot_modeOnOff_RX()));
    return widget_total;

}


QWidget* dialogsetting_RXsetting::getUIControlOption(QString p_title, int index){//c220511

    //QJsonObject tmp_hdmisettingval = get_settingOfHdmiDetail();
    QPushButton *lb_onOfftitle = new QPushButton(p_title);
    lb_onOfftitle->setProperty("btnNo", 4);
    lb_onOfftitle->setStyleSheet("color:#E5E5E4;");
    lb_onOfftitle->setFixedSize(180,70);
    lb_onOfftitle->setText(p_title);
    lb_onOfftitle->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");
    lb_onOfftitle->setCursor(Qt::PointingHandCursor);//print_debug();
    onOff[index] = new OnOffWidget;print_debug()
    //onOff->setValue(false);
    onOff[index]->setCursor(Qt::PointingHandCursor);//print_debug();

    QPushButton *btn_question1;
    btn_question1 =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_question1->setCursor(Qt::PointingHandCursor);
    btn_question1->setFixedSize(QSize(30,30));

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,0,30,0);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_onOfftitle);
    hl_lb->addWidget(btn_question1);
    hl_lb->addWidget(onOff[index], 0, Qt::AlignRight);//print_debug();
    QPushButton *btn_onOffHover = new QPushButton(onOff[index]);//print_debug();
    btn_onOffHover->setProperty("btnNo", 4);
    btn_onOffHover->setFixedSize(80,40);
    btn_onOffHover->setStyleSheet("background-color:transparent;border:0px;");

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");//print_debug();
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);//print_debug();
    //onOff->setValue(tmp_hdmisettingval["isHdmiPassThrough"].toBool());
    // 커넥션
    connect(btn_question1, SIGNAL(clicked()), this, SLOT(slot_clickedPassquestion()));//c220511

    //connect(lb_onOfftitle, SIGNAL(clicked()), this, SLOT(slot_modeOnOff()));
    connect(btn_onOffHover, SIGNAL(clicked()), this, SLOT(slot_modeOnOff()));
    return widget_total;
}

//"font:bold;color:#FFFFFF;font-size:22px;"
//"#widget_title {background-color:transparent;border-bottom:1px solid #404040;}"

QWidget* dialogsetting_RXsetting::getPlusMinusUIControlOption(QString p_title, int i, int data){

    lb_gain[i] = new QPushButton(p_title);
    lb_gain[i]->setStyleSheet("color:#E5E5E4;");
    lb_gain[i]->setFixedSize(330,70);
    lb_gain[i]->setText(p_title);
    lb_gain[i]->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4;");//background-color:707070; ");
    lb_gain[i]->setCursor(Qt::PointingHandCursor);//print_debug();

   // QLabel *lb_mqagain = new QLabel(p_title);
   // lb_mqagain->setAlignment(Qt::AlignCenter);
   // lb_mqagain->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    btn_gain_add[i] = new RemoteBtnItem();
    btn_gain_add[i]->setProperty("btnNo", 5+2*i);
    btn_gain_add[i]->setData(":/images/vol_add_remote.png", "");
   // btn_vol_add->setProperty("httpId",HTTP_REMOTE_VOLUME_ADD);
    btn_gain_add[i]->setCircle(35);
    btn_gain_add[i]->setCursor(Qt::PointingHandCursor);
    btn_gain_sub[i] = new RemoteBtnItem();
    btn_gain_sub[i]->setProperty("btnNo", 5+(2*i+1));
    btn_gain_sub[i]->setData(":/images/vol_sub_remote.png", "");
   // btn_vol_sub->setProperty("httpId",HTTP_REMOTE_VOLUME_SUB);
    btn_gain_sub[i]->setCircle(35);
    btn_gain_sub[i]->setCursor(Qt::PointingHandCursor);

    lb_text[i] = new QLabel(tr("text"));

    lb_text[i]->setText(QString("%1").arg(data) + tr("           ") + tr("dB"));
    lb_text[i]->setContentsMargins(0,10,15,10);
    lb_text[i]->setAlignment(Qt::AlignRight);
    lb_text[i]->setStyleSheet("font:bold; color:#FFFFFF;font-size:22px;");//background-color:#707070;");
    lb_text[i]->setFixedSize(300,40); //100x556
  //  lb_text->setStyleSheet("#lb_text { background-color:#707070;border-radius:10px; }");
    QHBoxLayout *box_text = new QHBoxLayout;
    box_text->setContentsMargins(0,0,0,0);
    box_text->setSpacing(0);
    box_text->setAlignment(Qt::AlignTop);
    box_text->addWidget(lb_text[i]);

    QWidget *widget_text = new QWidget;
    widget_text->setObjectName("widget_title");
    widget_text->setLayout(box_text);
    widget_text->setStyleSheet("#widget_title {background-color:#4d4d4d;border-bottom:1px solid #404040;}"); //text_box_color -> background-color:#FFFFFF

    QHBoxLayout *hl_mqagain = new QHBoxLayout();
    hl_mqagain->setContentsMargins(33,0,30,0);
    //hl_mqagain->setContentsMargins(0,35,0,0);
    hl_mqagain->setSpacing(0);
    hl_mqagain->addWidget(lb_gain[i], 0, Qt::AlignTop);
    hl_mqagain->setSpacing(0);
   // vl_sound->addWidget(lb_volumnNum, 0, Qt::AlignTop);
    hl_mqagain->addWidget(btn_gain_sub[i], 1, Qt::AlignCenter);
    hl_mqagain->setSpacing(10);
    hl_mqagain->addWidget(widget_text, 0, Qt::AlignCenter);
    hl_mqagain->setSpacing(10);
    hl_mqagain->addWidget(btn_gain_add[i], 1, Qt::AlignCenter);

    QWidget *widget_mqagain = new QWidget();
    widget_mqagain->setObjectName("widget_mqagain");
    widget_mqagain->setStyleSheet("#widget_mqagain {background-color:transparent;border:1px solid #303030;} ");
    //widget_mqagain->setStyleSheet("#widget_mqagain { border-top:1px solid #303030;border-bottom:1px solid #303030;border-left:1px solid #303030; }");
  //  widget_mqagain->setFixedSize(630,70); //100x556
    widget_mqagain->setLayout(hl_mqagain);

    QVBoxLayout *vl_volumn = new QVBoxLayout();
    vl_volumn->setContentsMargins(0,0,0,0);
    vl_volumn->setSpacing(0);
    vl_volumn->addWidget(widget_mqagain);




    return widget_mqagain;
}

void dialogsetting_RXsetting::slot_clickedHdmiaddmqaGain(){
//btn_gain_add[i]
    print_debug();
    if( mqadBData < 0 )
        mqadBData += 1;
    if(outputdBData == 0) lb_text[0]->setCursor(Qt::BlankCursor);
    lb_text[0]->setText(QString("%1").arg(mqadBData) + tr("           ") + tr("dB"));
    HdmiDetailData.insert( "mqaRelplayGain", mqadBData);
    qDebug() << "mqadBData : " << mqadBData;

}
void dialogsetting_RXsetting::slot_clickedHdmisubmqaGain(){
    print_debug();
    if( mqadBData > -20 ){
        mqadBData -= 1;
    }
    if(outputdBData == -20) lb_text[0]->setCursor(Qt::BlankCursor);
    lb_text[0]->setText(QString("%1").arg(mqadBData) + tr("           ") + tr("dB"));
    HdmiDetailData.insert( "mqaRelplayGain", mqadBData);
    qDebug() << "mqadBData : " << mqadBData;
}

void dialogsetting_RXsetting::slot_clickedHdmiaddoutputGain(){

    print_debug();
    if( outputdBData < 0 ){
        print_debug();
        outputdBData += 1;
        HdmiDetailData.insert( "outputGain", outputdBData);
    }
    if(outputdBData == 0){
        print_debug();
        btn_gain_add[1]->setCursor(Qt::BlankCursor);
        btn_gain_sub[1]->setCursor(Qt::PointingHandCursor);
    }
    lb_text[1]->setText(QString("%1").arg(outputdBData) + tr("           ") + tr("dB"));
    //HdmiDetailData.insert( "outputGain", outputdBData);
    qDebug() << "outputdBData : " << outputdBData;

}
void dialogsetting_RXsetting::slot_clickedHdmisuboutputGain(){
    print_debug();
    if( outputdBData > -20 ){
        print_debug();
        outputdBData -= 1;
        HdmiDetailData.insert( "outputGain", outputdBData);
    }
    if(outputdBData == -20){
        print_debug();
        btn_gain_add[1]->setCursor(Qt::PointingHandCursor);
        btn_gain_sub[1]->setCursor(Qt::BlankCursor);
    }
    lb_text[1]->setText(QString("%1").arg(outputdBData) + tr("           ") + tr("dB"));

    qDebug() << "outputdBData : " << outputdBData;
}
/**
 * @brief SettingTimer::getUIControlOption 콤보박스 포함된 ROW 생성한다
 * @param p_title
 * @return
 */
QWidget* dialogsetting_RXsetting::getUIControlOption(QString p_title, QString p_content, int index){

    //print_debug(); //cheon01
    //qDebug() << "p_title : " <<p_title ;//cheon01
    QPushButton *lbl_title = new QPushButton(p_title);
    lbl_title->setProperty("btnNo", index);
    lbl_title->setFixedSize(400,70);
    lbl_title->setText(p_title);
    //lbl_title->setStyleSheet("Text-align:left");
    lbl_title->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");
    lbl_title->setCursor(Qt::PointingHandCursor);
    QPushButton *lbr_content = new QPushButton(p_content);
    lbr_content->setProperty("btnNo", index);
    lbr_content->setFixedSize(300,70);
    lbr_content->setText(p_content);
    lbr_content->setStyleSheet("Text-align:right; font-size:18px;color:#707070; padding:5px;");
   // lbr_title->setStyleSheet("#lbr_title { Text-align:right;  font-size:18px;color:#707070; background-color:#333333; padding:5px;}");
    lbr_content->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *Vl_lbl = new QVBoxLayout();
    Vl_lbl->setContentsMargins(0,0,0,0);
    Vl_lbl->setSpacing(0);

    //hl_lb->addWidget(pushName, 0, Qt::AlignLeft);
    Vl_lbl->addWidget(lbl_title);

    QVBoxLayout *vl_lbr = new QVBoxLayout();
    vl_lbr->setContentsMargins(0,0,0,0);//(33,15,47,15);
    vl_lbr->setSpacing(0);

    //hl_lb->addWidget(pushName, 0, Qt::AlignLeft);
    vl_lbr->addWidget(lbr_content);
    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(0,0,0,0);//(33,15,47,15);
    hl_lb->setSpacing(0);
    hl_lb->addLayout(Vl_lbl);
    hl_lb->addLayout(vl_lbr);

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);


    // 커넥션
    //connect(comboBox, &QComboBox::currentIndexChanged, this, &SettingTimer::slot_changedIndexPod);
    //connect(comboName, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexLanguage(int)));
    connect(lbl_title, SIGNAL(clicked()), this, SLOT(slot_print()));
    connect(lbr_content, SIGNAL(clicked()), this, SLOT(slot_print()));


    //print_debug(); //cheon01
    return widget_total;
}



/**
 * @brief SettingVU::slot_modeOnOff [SLOT] VU 모드 ON/OFF 변경 API 호출
 */
//void SettingVideo::slot_modeOnOff(bool flagOn){
void dialogsetting_RXsetting::slot_modeOnOff_Pass(){

   // DialogConfirm *dlg = new DialogConfirm(this);
   // dlg->setTitle(tr("HDMI Passthrough"));

    //dlg->setText(tr("Do you want to change it?"));

   // int result = dlg->exec();
   // if(result==QDialog::Accepted){
    //}

    int ispass = !this->onOff_Pass->getCurrentValue();
    if(ispass) {
        HdmiDetailData.insert("isClearLowerBit", true);
        this->onOff_Pass->setValue(true);

    }else{
        HdmiDetailData.insert("isClearLowerBit", false);
        this->onOff_Pass->setValue(false);

    }

    QTimer::singleShot(500, this, SLOT(slot_showDlgOfHdmiNotice()));


      // notice_msg = tr("It bypasses HD audio formats such as surround sound sources to the audio receiver through HDMI without decoding.");
      // lb_notice->setText(tr("- Notice - ") + notice_msg);
       QJsonDocument doc(HdmiDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_modeOnOff()-slot_modeOnOff_Pass =: " << strJson;



}


void dialogsetting_RXsetting::slot_modeOnOff_RX(){


       int ispass = !this->onOff_RX->getCurrentValue();
       if(ispass) {
           HdmiDetailData.insert("rateDetect", true);
           this->onOff_RX->setValue(true);

       }else{
           HdmiDetailData.insert("rateDetect", false);
           this->onOff_RX->setValue(false);

       }

      // QTimer::singleShot(500, this, SLOT(slot_clickedRXquestion()));

       QJsonDocument doc(HdmiDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_modeOnOff()-slot_modeOnOff_32 =: " << strJson;



}


void dialogsetting_RXsetting::slot_modeOnOff_32(){


       int ispass = !this->onOff_32->getCurrentValue();
       if(ispass) {
           HdmiDetailData.insert("isClearLowerBit", true);
           this->onOff_32->setValue(true);

       }else{
           HdmiDetailData.insert("isClearLowerBit", false);
           this->onOff_32->setValue(false);

       }

       QTimer::singleShot(500, this, SLOT(slot_clickedbit32question()));

       QJsonDocument doc(HdmiDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_modeOnOff()-slot_modeOnOff_32 =: " << strJson;



}

void dialogsetting_RXsetting::slot_print(){
    qDebug() << "ffffffffffffffffffffffff";
}

/**
 * @brief DialogSelect_Cloud::slot_clickedAdd : [슬롯] 추가 버튼 클릭
 */

const int HTTP_RX_GET_MODE  = 154;
const int HTTP_USB_GET_MODE  = 155;
const int HTTP_USB_SET_MODE  = 156;
const int HTTP_RX_SET_MODE  = 157;

void dialogsetting_RXsetting::getHdmiSettingOfsetting(){

   NetworkHttp *network = new NetworkHttp;
   QJsonObject tmp_json;
   //tmp_json.insert("outputMode", 3);
   connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
   network->request(HTTP_RX_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("input.setting.get"), tmp_json, true);

}

void dialogsetting_RXsetting::getHdmiInOutSettingOfsetting(){

   NetworkHttp *network = new NetworkHttp;
   QJsonObject tmp_json;
   connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
   network->request(HTTP_RX_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("iinput.setting.get"), tmp_json, true);

}
void dialogsetting_RXsetting::slot_clickedUsbChangeSet(){
    print_debug();

   NetworkHttp *network = new NetworkHttp;
   QJsonObject tmp_json = get_settingOfHdmiDetail();
   QJsonDocument doc1(tmp_json);  QString strJson1(doc1.toJson(QJsonDocument::Compact));  qDebug() <<"dialogsetting_hdmi::slot_clickedHdmiChangeSet() :"<< strJson1;//cheon09_io

   QJsonObject tmp_json_set;
   tmp_json_set.insert("data", tmp_json);
  // tmp_json_set.insert("outputMode", 3);


   QJsonDocument doc(tmp_json_set);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"slot_clickedHdmiChangeSet()-tmp_json_set :"<< strJson;//cheon09_io

   connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
   network->request(HTTP_RX_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("input.setting.set"), tmp_json_set, true);

}

void dialogsetting_RXsetting::setDialogHdmi_show(){
   // QJsonObject *Hdmi_json = get_settingOfHdmiDetail();

    print_debug();
   QJsonDocument doc(HdmiDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"*setDialogHdmi_show()-HdmiDetailData :"<< strJson;//cheon09_io

    onOff_RX->setValue(HdmiDetailData["rateDetect"].toBool());

   // ToastMsg::show(this, "", tr("The current HDMI setting information is displayed on the screen."));

}


void dialogsetting_RXsetting::setDialogHdmi_save(QJsonObject p_jsonObject){
    print_debug();
    QJsonObject tmp_Json;
    if(p_jsonObject.contains("data")){
        tmp_Json = p_jsonObject["data"].toObject(); //cheon09_io

    }
   // QJsonDocument doc(tmp_Json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"\npre-setDialogHdmi_save()-tmp_Json :"<< strJson;//cheon09_io
     int opticalApwValue = tmp_Json["opticalApwValue"].toInt();
     int opticalApwMode = tmp_Json["opticalApwMode"].toInt();

     bool rateDetect = tmp_Json["rateDetect"].toBool();

    HdmiDetailData.insert( "opticalApwValue", opticalApwValue);
    HdmiDetailData.insert( "opticalApwMode", opticalApwMode);
    HdmiDetailData.insert( "rateDetect", rateDetect);
     qDebug() << "json-rateDetect : " << HdmiDetailData["rateDetect"].toBool();


     QJsonDocument doc1(HdmiDetailData);  QString strJson1(doc1.toJson(QJsonDocument::Compact));  qDebug() <<"\nsetDialogRX_save()-tmp_Json :"<< strJson1 << "\n";//cheon09_io

     ToastMsg::show(this, "", tr("Save the information from the Rose deive."));
}

void dialogsetting_RXsetting::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    QString deviceType = global.device.getDeviceType();
  //  print_debug();//cheon0219

    print_debug();  QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nslot_responseHttp =: " << strJson;//cheon12_io
    //qDebug() << "p_id : " << p_id;
    QString internalMode;

    switch(p_id){
    case HTTP_USB_GET_MODE :

        if(deviceType == "RS150" || deviceType == "RS150B"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                setDialogHdmi_save(p_jsonObject);
                setDialogHdmi_show();

            }
        }
        else if(deviceType == "RS201" || deviceType == "RS250" || deviceType == "RS250A"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                setDialogHdmi_save(p_jsonObject);
                setDialogHdmi_show();
               // setDialogHdmi_save(p_jsonObject);

            }
        }
        else if(deviceType == "RS301"){

        }
        else if(deviceType == "RS350"){

        }
        break;
    case HTTP_RX_GET_MODE :

        if(deviceType == "RS150" || deviceType == "RS150B"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                setDialogHdmi_save(p_jsonObject);
                setDialogHdmi_show();

            }
        }
        else if(deviceType == "RS201"|| deviceType == "RS250" || deviceType == "RS250A"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                setDialogHdmi_save(p_jsonObject);
                setDialogHdmi_show();
               // setDialogHdmi_save(p_jsonObject);

            }
        }
        else if( deviceType == "RS301"){

        }
        else if( deviceType == "RS350"){

        }
        break;
    case HTTP_USB_SET_MODE :

        if(deviceType == "RS150" || deviceType == "RS150B" || deviceType == "RS201" || deviceType == "RS250" || deviceType == "RS250A"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                 getHdmiInOutSettingOfsetting();

                 ToastMsg::show(this, "", tr("Setting modification was successful."));
                 setResult(QDialog::Accepted);
                 this->hide();
            }
            else{
                ToastMsg::show(this, "", tr("Failed to modify settings."));
                setResult(QDialog::Rejected);
                this->hide();
            }
        }
        else if( deviceType == "RS301"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){

            }
        }
        else if( deviceType == "RS350"){

        }
        break;

    case HTTP_RX_SET_MODE :

        if(deviceType == "RS150" || deviceType == "RS150B" || deviceType == "RS201" || deviceType == "RS250" || deviceType == "RS250A"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                 getHdmiInOutSettingOfsetting();

                 ToastMsg::show(this, "", tr("Setting modification was successful."));
                 setResult(QDialog::Accepted);
                 this->hide();

            }else{
                ToastMsg::show(this, "", tr("Failed to modify settings."));
                setResult(QDialog::Rejected);
                this->hide();
            }
        }
        else if( deviceType == "RS301"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){

            }
        }
        else if( deviceType == "RS350"){

        }
        break;
    }
    sender()->deleteLater();
}


void dialogsetting_RXsetting::set_selected_deviceName(QString usbstr){

    selected_deviceName = usbstr;
}


QString dialogsetting_RXsetting::get_selected_deviceName(){

    return selected_deviceName;
}


void dialogsetting_RXsetting::setTextName(QString usbstr){

    radio_UsbName->setText(usbstr);
}


void dialogsetting_RXsetting::setPropertyName(QString usbstr){

    radio_UsbName->setProperty(PROPERTY_NAME_READABLE, usbstr);
}
