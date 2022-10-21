#include "dialogsetting_interoutput_520.h"
#include "dialogselect_usbdac_520.h"
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

//#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";



using namespace Dialog;


/**
 * @brief 클라우드 타입 선택 Dlg
 * @param parent
 */
dialogsetting_interoutput_520::dialogsetting_interoutput_520(QWidget *parent)
    : QDialog(parent)
{
    setInit();
    setUIControl();
    getOpt_coaxSettingOfsetting();
}

/**
 * @brief DialogSelect_Cloud::setInit: 초기 세팅
 */
void dialogsetting_interoutput_520::setInit(){
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    this->setStyleSheet("background-color:transparent;");
   // Opt_coaxDetailData = new QJsonObject();
}

/**
 * @brief DialogSelect_Cloud::setUIControl : UI 세팅
 */
void dialogsetting_interoutput_520::setUIControl(){

    QJsonObject tmp_opt_coaxsettingval = get_settingOfOpt_coaxDetail();
    dlgConfirmOpt_coax = new DialogConfirm(this);
    // 타이틀
    QLabel *lb_title = new QLabel;
    lb_title->setText(tr(" Digital Out Setting"));//lb_title->setText(tr("클라우드 추가"));
    lb_title->setStyleSheet("font:bold;color:#FFFFFF;font-size:22px;");
    lb_title->setAlignment(Qt::AlignCenter);
    lb_title->setFixedHeight(60);

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

    QPushButton *btn_questionMQA;//c220511
    btn_questionMQA =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionMQA->setCursor(Qt::PointingHandCursor);
    btn_questionMQA->setFixedSize(QSize(30,30));
    QPushButton *btn_questionPreLevel;//c220511
    btn_questionPreLevel =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionPreLevel->setCursor(Qt::PointingHandCursor);
    btn_questionPreLevel->setFixedSize(QSize(30,30));
    QPushButton *btn_questionPCM;//c220511
    btn_questionPCM =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionPCM->setCursor(Qt::PointingHandCursor);
    btn_questionPCM->setFixedSize(QSize(30,30));
    QPushButton *btn_questionHOI;//c220511
    btn_questionHOI =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionHOI->setCursor(Qt::PointingHandCursor);
    btn_questionHOI->setFixedSize(QSize(30,30));
    QPushButton *btn_questionDSD;//c220511
    btn_questionDSD =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionDSD->setCursor(Qt::PointingHandCursor);
    btn_questionDSD->setFixedSize(QSize(30,30));
    QPushButton *btn_questionRoon;//c220511
    btn_questionRoon =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionRoon->setCursor(Qt::PointingHandCursor);
    btn_questionRoon->setFixedSize(QSize(30,30));
    QPushButton *btn_questionI2S;//c220511
    btn_questionI2S =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionI2S->setCursor(Qt::PointingHandCursor);
    btn_questionI2S->setFixedSize(QSize(30,30));

    QString radio_style = "QRadioButton {font-size:20px;color:#CCCCCC;spacing:15px;padding:15px;border-bottom:1px solid #404040;}";
    radio_style += " QRadioButton::indicator {width:40px;height:40px;}";
    radio_style += " QRadioButton::indicator::unchecked {image: url(:images/icon_che_off.png);}";
    radio_style += " QRadioButton::indicator::checked {image: url(:images/icon_che_on.png);}";

    comboBoxOpt_coaxMQA = new QComboBox();
    comboBoxOpt_coaxMQA->setProperty("btnNo", 0);

    comboBoxOpt_coaxDSD = new QComboBox();
    comboBoxOpt_coaxDSD->setProperty("btnNo", 2);
    comboBoxOpt_coaxRoon = new QComboBox();
    comboBoxOpt_coaxRoon->setProperty("btnNo", 3);

    comboBoxOpt_coaxMQA->addItem(tr("Authenticator"), 0);
    comboBoxOpt_coaxMQA->addItem(tr("Decoder"), 1);



    comboBoxOpt_coaxMQA->setContentsMargins(0,0,0,0);
    comboBoxOpt_coaxMQA->setFixedHeight(35);
    comboBoxOpt_coaxMQA->setObjectName("comboBox_opt_coax");//cheon01
    comboBoxOpt_coaxMQA->setFixedWidth(340);


    comboBoxOpt_coaxMQA->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                );

    QLabel *lb_MQA = new QLabel(tr("MQA type"));
    lb_MQA->setStyleSheet("color:#E5E5E4");
    lb_MQA->setFixedHeight(55);//c220511
    lb_MQA->setText(tr("MQA type"));
    lb_MQA->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    QHBoxLayout *hl_lb_MQA = new QHBoxLayout();
    hl_lb_MQA->setContentsMargins(37,0,30,0);
    hl_lb_MQA->setSpacing(0);
    hl_lb_MQA->addWidget(lb_MQA);
    hl_lb_MQA->addWidget(btn_questionMQA, 0, Qt::AlignLeft);//c220511
    hl_lb_MQA->addWidget(comboBoxOpt_coaxMQA, 0, Qt::AlignRight);//c220511

    QWidget *widget_total_MQA = new QWidget();
    widget_total_MQA->setObjectName("widget_total_MQA");
    widget_total_MQA->setStyleSheet("#widget_total_MQA { border-bottom:1px solid #505050;border-left:1px solid #303030; } ");
    widget_total_MQA->setLayout(hl_lb_MQA);


    comboBoxOutputPCMResampling = new QComboBox();
    comboBoxOutputPCMResampling->setProperty("btnNo", 1);//

    comboBoxOutputPCMResampling->addItem(tr(" Original"), 0);//cheon01_src
    print_debug();
    comboBoxOutputPCMResampling->addItem(tr(" 44.1 kHz"), 1);//cheon01_src

    comboBoxOutputPCMResampling->addItem(tr(" 48 kHz"), 2);//cheon01_src
    comboBoxOutputPCMResampling->addItem(tr(" 88.2 kHz"), 3);//cheon01_src
    comboBoxOutputPCMResampling->addItem(tr(" 96 kHz"), 4);//cheon01_src
    comboBoxOutputPCMResampling->addItem(tr(" 176.4 kHz"), 5);//cheon01_src
    comboBoxOutputPCMResampling->addItem(tr(" 192 kHz"), 6);//cheon01_src


    QLabel *lb_PCM = new QLabel(tr("output LSB"));
    lb_PCM->setStyleSheet("color:#E5E5E4");
    lb_PCM->setFixedHeight(100);//c220511
    lb_PCM->setText((tr("PCM Resampling Frequency")));
    lb_PCM->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    comboBoxOutputPCMMaxSampling = new QComboBox();
    comboBoxOutputPCMMaxSampling->setProperty("btnNo", 11);//
    comboBoxOutputPCMResampling->setContentsMargins(10,0,0,0);
    comboBoxOutputPCMResampling->setFixedHeight(35);
    comboBoxOutputPCMResampling->setObjectName("comboBoxOutputPCMResampling");//cheon01
    comboBoxOutputPCMResampling->setFixedWidth(150);

    comboBoxOutputPCMResampling->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                               "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                               "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                               );

    comboBoxOutputPCMMaxSampling->addItem(tr(" Auto"), 0);//cheon01_src
    comboBoxOutputPCMMaxSampling->addItem(tr(" 48 kHz"), 1);//cheon01_src
    comboBoxOutputPCMMaxSampling->addItem(tr(" 96 kHz"), 2);//cheon01_src
    comboBoxOutputPCMMaxSampling->addItem(tr(" 192 kHz"), 3);//cheon01_src




    //comboBox = new QComboBox();
    comboBoxOutputPCMMaxSampling->setContentsMargins(10,0,0,0);
    comboBoxOutputPCMMaxSampling->setFixedHeight(35);
    comboBoxOutputPCMMaxSampling->setObjectName("comboBoxOutputPCMMaxSampling");//cheon01
    comboBoxOutputPCMMaxSampling->setFixedWidth(150);

    comboBoxOutputPCMMaxSampling->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                                );

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
    hl_lb_PCM_subMax->addWidget(comboBoxOutputPCMMaxSampling);//c220511

    QHBoxLayout *hl_lb_PCM_subRe = new QHBoxLayout();
    hl_lb_PCM_subRe->setContentsMargins(0,0,0,0);
    hl_lb_PCM_subRe->setSpacing(10);
    hl_lb_PCM_subRe->addWidget(lb_title_Resample);
    hl_lb_PCM_subRe->addWidget(comboBoxOutputPCMResampling);//c220511


    QVBoxLayout *vl_lb_PCM = new QVBoxLayout();
    vl_lb_PCM->setContentsMargins(33,0,0,0);
    vl_lb_PCM->setSpacing(0);
    vl_lb_PCM->addLayout(hl_lb_PCM_subMax);
    vl_lb_PCM->addLayout(hl_lb_PCM_subRe);

    QWidget *widget_PCM_and_Max = new QWidget();
    widget_PCM_and_Max->setObjectName("widget_PCM_and_Max");
    widget_PCM_and_Max->setStyleSheet("#widget_PCM_and_Max { border-bottom:1px solid #505050;border-left:1px solid #303030; } ");
    widget_PCM_and_Max->setLayout(vl_lb_PCM);

    QHBoxLayout *hl_lb_PCM = new QHBoxLayout();
    hl_lb_PCM->setContentsMargins(33,0,30,0);
    hl_lb_PCM->setSpacing(20);
    hl_lb_PCM->addWidget(lb_PCM);
    hl_lb_PCM->addWidget(btn_questionPCM, 0, Qt::AlignLeft);//c220511
    hl_lb_PCM->addWidget(widget_PCM_and_Max, 0, Qt::AlignRight);//c220511
    //hl_lb_PCM->addWidget(comboBoxHdmiPCM);

    QWidget *widget_total_PCM = new QWidget();
    widget_total_PCM->setObjectName("widget_total_PCM");
    widget_total_PCM->setStyleSheet("#widget_total_PCM { border-bottom:1px solid #505050;border-left:1px solid #303030; } ");
    widget_total_PCM->setLayout(hl_lb_PCM);

   // vl_total1->addWidget(this->getUIControlOption(tr("PCM Resampling Frequency"), comboBoxOpt_coaxPCM));//cheon01

    comboBoxOpt_coaxDSD->addItem(tr("Native DSD(Not supported)"), 0);
    comboBoxOpt_coaxDSD->addItem(tr("DSD over PCM (Upto DSD64)"), 1);
    comboBoxOpt_coaxDSD->addItem(tr("DSD to PCM (Upto DSD128)"), 2);

    QLabel *lb_DSD = new QLabel(tr("DSD Mode"));
    lb_DSD->setStyleSheet("color:#E5E5E4");
    lb_DSD->setFixedHeight(55);//c220511
    lb_DSD->setText((tr("DSD Mode")));
    lb_DSD->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

   // this->list_keyValue.append(0);
   // this->list_keyValue.append(1);


    //comboBox = new QComboBox();
    comboBoxOpt_coaxDSD->setContentsMargins(0,0,0,0);
    comboBoxOpt_coaxDSD->setFixedHeight(35);
    comboBoxOpt_coaxDSD->setObjectName("comboBoxOpt_coaxDSD");//cheon01
    comboBoxOpt_coaxDSD->setFixedWidth(340);


    comboBoxOpt_coaxDSD->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                       "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                       "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                       );

    QHBoxLayout *hl_lb_DSD = new QHBoxLayout();
    hl_lb_DSD->setContentsMargins(33,0,30,0);
    hl_lb_DSD->setSpacing(0);
    hl_lb_DSD->addWidget(lb_DSD);
    hl_lb_DSD->addWidget(btn_questionDSD, 0, Qt::AlignLeft);//c220511
    hl_lb_DSD->addWidget(comboBoxOpt_coaxDSD, 0, Qt::AlignRight);//c220511

    QWidget *widget_total_DSD = new QWidget();
    widget_total_DSD->setObjectName("widget_total_DSD");
    widget_total_DSD->setStyleSheet("#widget_total_DSD { border-bottom:1px solid #505050;border-left:1px solid #303030; } ");
    widget_total_DSD->setLayout(hl_lb_DSD);


    comboBoxOpt_coaxRoon->addItem(tr("OFF"), 0);//c220518
    comboBoxOpt_coaxRoon->addItem(tr("ON"), 1);//c220518



    QLabel *lb_Roon = new QLabel(tr("Software Volume Control"));
    lb_Roon->setStyleSheet("color:#E5E5E4");
    lb_Roon->setFixedHeight(55);//c220511
    lb_Roon->setText((tr("Software Volume Control")));
    lb_Roon->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

   // this->list_keyValue.append(0);
   // this->list_keyValue.append(1);


    //comboBox = new QComboBox();
    comboBoxOpt_coaxRoon->setContentsMargins(0,0,0,0);
    comboBoxOpt_coaxRoon->setFixedHeight(35);
    comboBoxOpt_coaxRoon->setObjectName("comboBoxOpt_coaxRoon");//cheon01
    comboBoxOpt_coaxRoon->setFixedWidth(340);


    comboBoxOpt_coaxRoon->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                        "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                        "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                        );

    QHBoxLayout *hl_lb_Roon = new QHBoxLayout();
    hl_lb_Roon->setContentsMargins(33,0,30,0);
    hl_lb_Roon->setSpacing(20);
    hl_lb_Roon->addWidget(lb_Roon);
    hl_lb_Roon->addWidget(btn_questionRoon, 0, Qt::AlignLeft);//c220511
    hl_lb_Roon->addWidget(comboBoxOpt_coaxRoon, 0, Qt::AlignRight);//c220511

    QWidget *widget_total_Roon = new QWidget();
    widget_total_Roon->setObjectName("widget_total_Roon");
    widget_total_Roon->setStyleSheet("#widget_total_Roon { border-bottom:1px solid #505050;border-left:1px solid #303030; } ");
    widget_total_Roon->setLayout(hl_lb_Roon);

    ////vl_total1->addWidget(widget_total_Roon);//cheon01
//---------------------------------------------------------------------------------------------------
    QLabel *lb_i2s= new QLabel(tr("lb_i2s"));

    lb_i2s->setStyleSheet("color:#E5E5E4");
    lb_i2s->setFixedHeight(55);//c220511
    lb_i2s->setText((tr("I2S OUT (DVI)")));
    lb_i2s->setStyleSheet("text-align:left; font-size:18px;color:#E5E5E4; ");


    lb_i2s_text = new QLabel(tr("lb_i2s_text"));\

    lb_i2s_text->setStyleSheet("color:#E5E5E4");
    lb_i2s_text->setFixedSize(100,55);
    lb_i2s_text->setText("A Mode");
    lb_i2s_text->setStyleSheet("text-align:right; font-size:18px;color:#E5E5E4; ");


    QHBoxLayout *hl_lb_i2s_text = new QHBoxLayout();
    hl_lb_i2s_text->setContentsMargins(33,0,0,0);

    hl_lb_i2s_text->setSpacing(20);
    hl_lb_i2s_text->setAlignment(Qt::AlignTop);//

    hl_lb_i2s_text->addWidget(lb_i2s);//c220511
    hl_lb_i2s_text->addWidget(btn_questionI2S, 0, Qt::AlignLeft);//c220511
    hl_lb_i2s_text->addWidget(lb_i2s_text);

    //rose_check_iosetting.png
    btn_DVIA = new BtnLabel(tr("A Mode"), "btn150_input_Model", ":images//setting/150/DVI_A_mode.png", ":images//setting/150/DVI_A_mode.png");
    btn_DVIA->setText("A Mode");
    btn_DVIA->setFixedSize(320,270);
    btn_DVIA->setProperty("btnNo", 77);
    QVBoxLayout *vl_btnDVIA = new QVBoxLayout();
    vl_btnDVIA->setContentsMargins(10,20,10,10);
    vl_btnDVIA->setSpacing(0);
    vl_btnDVIA->addWidget(btn_DVIA, 0, Qt::AlignCenter);

    wwbtn_DVIA = new QWidget();
   // wbtnDVIA->setStyleSheet("#wi2s_text {background-color:transparent;border-top:1px solid #404040; } ");
    wwbtn_DVIA->setFixedSize(340, 270);
    wwbtn_DVIA->setLayout(vl_btnDVIA);

    btn_DVIB = new BtnLabel(tr("B Mode"), "btn150_input_Model", ":images//setting/150/DVI_B_mode.png", ":images//setting/150/DVI_B_mode.png");
    btn_DVIB->setFixedSize(320,270);
    btn_DVIB->setText("B Mode");
    btn_DVIB->setProperty("btnNo", 78);
    QVBoxLayout *vl_btnDVIB = new QVBoxLayout();
    vl_btnDVIB->setContentsMargins(10,20,10,10);
    vl_btnDVIB->setSpacing(0);
    vl_btnDVIB->addWidget(btn_DVIB, 0, Qt::AlignCenter);

    wwbtn_DVIB = new QWidget();
   // wbtnDVIA->setStyleSheet("#wi2s_text {background-color:transparent;border-top:1px solid #404040; } ");
   // wwbtn_DVIB->setFixedHeight(70);
    wwbtn_DVIB->setFixedSize(340, 270);
    wwbtn_DVIB->setLayout(vl_btnDVIB);

    QHBoxLayout *hl_lb_i2s_dvi = new QHBoxLayout();
    hl_lb_i2s_dvi->setContentsMargins(33,0,60,0);
    hl_lb_i2s_dvi->setSpacing(20);
    hl_lb_i2s_dvi->setAlignment(Qt::AlignTop|Qt::AlignCenter);
    hl_lb_i2s_dvi->addWidget(wwbtn_DVIA);
    hl_lb_i2s_dvi->addWidget(wwbtn_DVIB);

    QWidget *wi2s_text = new QWidget();
    wi2s_text->setStyleSheet("#wi2s_text {background-color:transparent;border-top:1px solid #404040; } ");
    wi2s_text->setFixedHeight(55);
    wi2s_text->setLayout(hl_lb_i2s_text);

    QVBoxLayout *vl_lb_i2s_dvi = new QVBoxLayout();
    vl_lb_i2s_dvi->setContentsMargins(0,0,0,0);
    vl_lb_i2s_dvi->setSpacing(20);
    vl_lb_i2s_dvi->setAlignment(Qt::AlignTop);
    vl_lb_i2s_dvi->addWidget(wi2s_text);
    vl_lb_i2s_dvi->addLayout(hl_lb_i2s_dvi);


    QVBoxLayout *vl_total_bit32 = new QVBoxLayout();
    vl_total_bit32->setContentsMargins(0,0,0,0);
    vl_total_bit32->setSpacing(0);
    vl_total_bit32->setAlignment(Qt::AlignTop);
    vl_total_bit32->addWidget(this->getUIControlOption_32(tr("The low 8bits of a 32 bit sound sources will have correct")));//print_debug();


//------------------------------------------------------------------------------------------------------------------

    QVBoxLayout *vl_total_Pass = new QVBoxLayout();
    vl_total_Pass->setContentsMargins(0,0,0,0);
    vl_total_Pass->setSpacing(0);
    vl_total_Pass->setAlignment(Qt::AlignTop);
    //vl_total_Pass->addWidget(GSCommon::getUIControlTitleForSetting(tr("Video Option")));
    vl_total_Pass->addWidget(this->getUIControlOption(tr("Digital Out Passthrough (OPT/COX)")));//print_debug();
    onOff->setValue(tmp_opt_coaxsettingval["isDigitalPassThrough"].toBool());

    QVBoxLayout *vl_total_mqagain = new QVBoxLayout();
    vl_total_mqagain->setContentsMargins(0,0,0,0);
    vl_total_mqagain->setSpacing(0);
    vl_total_mqagain->setAlignment(Qt::AlignTop);

    mqadBData = tmp_opt_coaxsettingval["mqaRelplayGain"].toInt();
    QWidget *widget_mqaGain = this->getPlusMinusUIControlOption(tr("MQA Replay Gain"),0, mqadBData);
    vl_total_mqagain->addWidget(widget_mqaGain);

    QVBoxLayout *vl_total_outputgain = new QVBoxLayout();
    vl_total_outputgain->setContentsMargins(0,0,0,0);
    vl_total_outputgain->setSpacing(0);
    vl_total_outputgain->setAlignment(Qt::AlignTop);

    outputdBData = tmp_opt_coaxsettingval["outputGain"].toInt();
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
   // widget_box_notice->setStyleSheet("#widget_total { border-bottom:1px solid #505050;border-left:1px solid #303030; } ");
    widget_box_notice->setLayout(box_notice);

    QVBoxLayout *vl_total1 = new QVBoxLayout();

    vl_total1->setContentsMargins(0,0,0,0);
    vl_total1->setSpacing(0);
    vl_total1->setAlignment(Qt::AlignTop);

    //cheon01
   // widget_total_PCM->setStyleSheet(" border-top:1px solid #404040;border-bottom:1px solid #404040; ");
    //widget_total_DSD->setStyleSheet(" border-top:1px solid #404040;border-bottom:1px solid #404040;");
    vl_total1->addWidget(widget_total_PCM);//cheon01
    vl_total1->addWidget(widget_total_DSD);//cheon01

    QVBoxLayout *vl_total2 = new QVBoxLayout();
    vl_total2->setContentsMargins(0,0,0,0);
    vl_total2->setSpacing(0);
    vl_total2->setAlignment(Qt::AlignTop);
    vl_total2->addWidget(widget_title);

    vl_total2->addWidget(widget_total_MQA);
    //vl_total2->addLayout(hl_lb_MQA);
    //vl_total2->addLayout(vl_total_Pass);//print_debug();
    wi2s_text->setStyleSheet(" border-top:1px solid #404040;border-bottom:1px solid #404040;");
    //vl_total2->addWidget(wi2s_text);
    //vl_total2->addLayout(vl_lb_i2s_dvi);

    vl_total2->addLayout(vl_total1);
    vl_total2->addWidget(widget_total_Roon);//cheon01
    vl_total2->addLayout(vl_total_bit32);

   // vl_total2->addLayout(vl_total_mqagain);
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


    connect(btn_questionDSD, SIGNAL(clicked()), this, SLOT(slot_clickedDSDquestion()));//c220511
    connect(btn_questionPCM, SIGNAL(clicked()), this, SLOT(slot_clickedPCMquestion()));//c220511
    connect(btn_questionI2S, SIGNAL(clicked()), this, SLOT(slot_clickedI2Squestion()));//c220511
    connect(btn_questionMQA, SIGNAL(clicked()), this, SLOT(slot_clickedMQAquestion()));//c220511
    connect(btn_questionRoon, SIGNAL(clicked()), this, SLOT(slot_clickedSoftwareVolquestion()));//c220511
    //connect(btn_questionPreLevel, SIGNAL(clicked()), this, SLOT(slot_clickedPreLevelquestion()));//c220511

    // 커넥션
    connect(btn_DVIA, SIGNAL(clicked()), this, SLOT(slot_clickedDVIAB()));
    connect(btn_DVIB, SIGNAL(clicked()), this, SLOT(slot_clickedDVIAB()));;

    connect(comboBoxOpt_coaxMQA, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOpt_coax(int)));
    connect(comboBoxOutputPCMMaxSampling, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOpt_coax(int)));
    connect(comboBoxOutputPCMResampling, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOpt_coax(int)));
    connect(comboBoxOpt_coaxDSD, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOpt_coax(int)));
    connect(comboBoxOpt_coaxRoon, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOpt_coax(int)));

    connect(lb_onOfftitle, SIGNAL(clicked()), this, SLOT(slot_clickedOpt_coaxonOffGain()));
    connect(lb_gain[0], SIGNAL(clicked()), this, SLOT(slot_clickedOpt_coaxmqaGain()));
    connect(lb_gain[1], SIGNAL(clicked()), this, SLOT(slot_clickedOpt_coaxoutputGain()));

    connect(btn_gain_add[0], SIGNAL(clicked()), this, SLOT(slot_clickedOpt_coaxaddmqaGain()));
    connect(btn_gain_sub[0], SIGNAL(clicked()), this, SLOT(slot_clickedOpt_coaxsubmqaGain()));
    connect(btn_gain_add[1], SIGNAL(clicked()), this, SLOT(slot_clickedOpt_coaxaddoutputGain()));
    connect(btn_gain_sub[1], SIGNAL(clicked()), this, SLOT(slot_clickedOpt_coaxsuboutputGain()));

    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(close()));//print_debug();
    connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clickedOpt_coaxChangeSet()));

}

QWidget* dialogsetting_interoutput_520::getUIControlOption_32(QString p_title){

    QPushButton *lb_onOfftitle = new QPushButton(p_title);
    lb_onOfftitle->setProperty("btnNo", 4);
    lb_onOfftitle->setStyleSheet("color:#E5E5E4;");
    lb_onOfftitle->setFixedSize(500,55);
    lb_onOfftitle->setText(p_title);
    lb_onOfftitle->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");
    lb_onOfftitle->setCursor(Qt::PointingHandCursor);//print_debug();
    onOff_32 = new OnOffWidget;print_debug()
    onOff_32->setCursor(Qt::PointingHandCursor);//print_debug();

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
    hl_lb->addWidget(onOff_32, 0, Qt::AlignRight);
    print_debug();
    QPushButton *btn_onOffHover = new QPushButton(onOff_32);//print_debug();
    btn_onOffHover->setProperty("btnNo", 4);
    btn_onOffHover->setFixedSize(80,40);
    btn_onOffHover->setStyleSheet("background-color:transparent;border:0px;");

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");//print_debug();
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #505050;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);
    print_debug();

    // 커넥션
    connect(btn_question, SIGNAL(clicked()), this, SLOT(slot_clickedBit32question()));

    connect(btn_onOffHover, SIGNAL(clicked()), this, SLOT(slot_modeOnOff_32()));
    return widget_total;

}

void dialogsetting_interoutput_520::slot_clickedBit32question(){//c220511
    print_debug();
    dlgConfirmOpt_coax->setAlignment(Qt::AlignLeft);
    //dlgConfirmOutput->setTitle(tr("Headphone Output Impedance"));
    //dlgConfirmOutput->setText(tr("This is a menu to set the Headphone Output Impedace.\nIf you select the output (16 Ohm, 32 Ohm, 50 Ohm, 100 Ohm) that matches your headphone specifications, you can enjoy better sound."));
    dlgConfirmOpt_coax->setAlignment(Qt::AlignLeft);
    dlgConfirmOpt_coax->setTitle(tr("The low 8bits of a 32 bit sound sources\n will have correct"));
    dlgConfirmOpt_coax->setText(tr("Some DACs may generate noise when playing a 32-bit sound source with a high sampling rate.\nIn this case, it is used to reduce noise by correcting the lower 8 bits."));

    dlgConfirmOpt_coax->setGeometry((DLG_WIDTH + 80), (350 + 100), 350, 500);
    dlgConfirmOpt_coax->setAlertMode();
    dlgConfirmOpt_coax->setProperty("flagShown",false);

    if(dlgConfirmOpt_coax->property("flagShown").toBool()==false){
        dlgConfirmOpt_coax->setProperty("flagShown",true);

        int result = dlgConfirmOpt_coax->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}

void dialogsetting_interoutput_520::slot_clickedMQAquestion(){//c220511
    print_debug();
    dlgConfirmOpt_coax->setTitle(tr("MQA Type"));
    dlgConfirmOpt_coax->setText(tr("Set the MQA playback mode.\nYou can select either Authenticator or Decoder mode.\nOnly the original sound or decoded audio data can be exported as digital output, not fully decoded audio data."));

    dlgConfirmOpt_coax->setGeometry((DLG_WIDTH + 80), (350 + 100), 350, 500);
    dlgConfirmOpt_coax->setAlertMode();
    dlgConfirmOpt_coax->setProperty("flagShown",false);

    if(dlgConfirmOpt_coax->property("flagShown").toBool()==false){
        dlgConfirmOpt_coax->setProperty("flagShown",true);

        int result = dlgConfirmOpt_coax->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}

void dialogsetting_interoutput_520::slot_clickedDSDquestion(){//c220511
    print_debug();
    dlgConfirmOpt_coax->setAlignment(Qt::AlignLeft);
    dlgConfirmOpt_coax->setTitle(tr("DSD Mode"));
    dlgConfirmOpt_coax->setText(tr("Determines the transmission format (Native, DOP, PCM) of audio samples to be delivered to the DAC when playing DSD music. \n When in Native mode, analog output supports up to DSD512 when playing DSD, but digital output does not produce sound. When in DOP mode, analog output does not produce sound and digital output transmits audio data uo to DSD256. For optical, and COAX, it supports only uo to DSD64. In case of DSD to PCM mode, it supprts up to DSD128 and supprts both digital and analog output."));
    dlgConfirmOpt_coax->setTextHeight(330);
    dlgConfirmOpt_coax->setGeometry((DLG_WIDTH + 80), (150 + 100), 350, 500);
    dlgConfirmOpt_coax->setAlertMode();
    dlgConfirmOpt_coax->setProperty("flagShown",false);

    if(dlgConfirmOpt_coax->property("flagShown").toBool()==false){
        dlgConfirmOpt_coax->setProperty("flagShown",true);

        int result = dlgConfirmOpt_coax->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}
void dialogsetting_interoutput_520::slot_clickedI2Squestion(){//c220511
    print_debug();
    dlgConfirmOpt_coax->setAlignment(Qt::AlignLeft);
    dlgConfirmOpt_coax->setTitle(tr("I2S OUT(DVI)"));
    dlgConfirmOpt_coax->setText(tr("Set I2S output.\n There are two types, A type and B type."));

    dlgConfirmOpt_coax->setGeometry((DLG_WIDTH + 80), (150 + 100), 350, 500);
    dlgConfirmOpt_coax->setAlertMode();
    dlgConfirmOpt_coax->setProperty("flagShown",false);

    if(dlgConfirmOpt_coax->property("flagShown").toBool()==false){
        dlgConfirmOpt_coax->setProperty("flagShown",true);

        int result = dlgConfirmOpt_coax->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}

void dialogsetting_interoutput_520::slot_clickedPCMquestion(){//c220511
    print_debug();
    dlgConfirmOpt_coax->setAlignment(Qt::AlignLeft);
    dlgConfirmOpt_coax->setTitle(tr("PCM Resampling Frequency"));
    dlgConfirmOpt_coax->setText(tr("This is The menu to set sampling frequecy. if you want to output the original sound as it is, select the Original Sampling Rate, and if want to use the resampling function, select the desired sampling rate (48kHz, 96Khz, 192Khz, etc), MQA, Native DSD, DOP, Video, Bluetooth and Airplay, Roon Ready are not supported."));

    dlgConfirmOpt_coax->setGeometry((DLG_WIDTH + 80), (150 + 100), 350, 500);
    dlgConfirmOpt_coax->setAlertMode();
    dlgConfirmOpt_coax->setProperty("flagShown",false);

    if(dlgConfirmOpt_coax->property("flagShown").toBool()==false){
        dlgConfirmOpt_coax->setProperty("flagShown",true);

        int result = dlgConfirmOpt_coax->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}

void dialogsetting_interoutput_520::slot_clickedSoftwareVolquestion(){//c220511
    print_debug();
    dlgConfirmOpt_coax->setAlignment(Qt::AlignLeft);
    dlgConfirmOpt_coax->setTitle(tr("Software Volume Control"));
    dlgConfirmOpt_coax->setText(tr("Adjust the playback volume of digital audio data using the Rose volume control software.\nCaution 1. It cannot be used in Native DSD or DSD over PCM mode. It is applied after changing to DSD to PCM(upto DSD128) mode.\nCaution 2. For digital output. MQA Stream is supported only when the volume is MAX. If it is less than MAX. MQA Stream is not supported."));
    dlgConfirmOpt_coax->setTextHeight(250);
    dlgConfirmOpt_coax->setAlignment(Qt::AlignLeft);
    dlgConfirmOpt_coax->setGeometry((DLG_WIDTH + 80), (150 + 100), 350, 500);

    dlgConfirmOpt_coax->setAlertMode();
    dlgConfirmOpt_coax->setProperty("flagShown",false);

    if(dlgConfirmOpt_coax->property("flagShown").toBool()==false){
        dlgConfirmOpt_coax->setProperty("flagShown",true);

        int result = dlgConfirmOpt_coax->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}

void dialogsetting_interoutput_520::slot_clickedOpt_coaxoutputGain(){
    this->dlgOpt_coaxMqa();
    QTimer::singleShot(500, this, SLOT(slot_showDlgOfOpt_coaxNotice()));
}
void dialogsetting_interoutput_520::slot_clickedOpt_coaxmqaGain(){
    this->dlgOpt_coaxOutput();
    QTimer::singleShot(500, this, SLOT(slot_showDlgOfOpt_coaxNotice()));
}
void dialogsetting_interoutput_520::slot_clickedOpt_coaxpassGain(){
    this->dlgOpt_coaxPass();
    QTimer::singleShot(500, this, SLOT(slot_showDlgOfOpt_coaxNotice()));
}

void dialogsetting_interoutput_520::dlgOpt_coaxMqa(){


    this->dlgConfirmOpt_coax->setTitle(tr("HDMI MQA Replay Gain Notice"));
    this->dlgConfirmOpt_coax->setText(tr("Activates Replay Gaon Control when playing MQA. \nSet the desired leveling value.\nThese values apply only to MQA sound sources."));
    //dlgConfirmOpt_coax->setText(tr("MQA 재생시 Replay Gaon Control을 활성화합니다. \n원하는 평준화값을 설정하시오.\n해당 값은 MQA  음원에만 적용됩니다."));

}
void dialogsetting_interoutput_520::dlgOpt_coaxOutput(){


    this->dlgConfirmOpt_coax->setTitle(tr("HDMI Output Gain Notice"));
    this->dlgConfirmOpt_coax->setText(tr("Adjusts the Digital Output Gain. \nExternal receiver Or, if distortion occurs when connecting to a DAC, adjust the Output Gain value appropriately."));
    //dlgConfirmOpt_coax->setText(tr("Digital Output의 Gain 값을 조정합니다. \n외부 리시버 또는 DAC과 연결 시 distortion이 발생하면 Output Gain 값을 적절히 조절하여 사용합니다."));

}

void dialogsetting_interoutput_520::dlgOpt_coaxPass(){


    dlgConfirmOpt_coax->setTitle(tr("HDMI Passthrough Notice"));
    //dlgConfirmOpt_coax->setText(tr("디코딩없이 HDMI를 통해 \ n 오디오 수신기로 서라운드 음원과 같은 HD 오디오 형식을 우회합니다."));
    dlgConfirmOpt_coax->setText(tr("It bypasses HD audio formats such as surround sound sources to\n the audio receiver through HDMI without decoding."));

}


void dialogsetting_interoutput_520::slot_showDlgOfOpt_coaxNotice(){
    this->dlgConfirmOpt_coax->setGeometry(DLG_WIDTH+480,350+100, 350,300);
    this->dlgConfirmOpt_coax->setAlertMode();
    this->dlgConfirmOpt_coax->setProperty("flagShown",false);
    if(this->dlgConfirmOpt_coax->property("flagShown").toBool()==false){
        this->dlgConfirmOpt_coax->setProperty("flagShown",true);

        int result = this->dlgConfirmOpt_coax->exec();

        if(result == QDialog::Accepted){
            //showRemote(true);
            qDebug() << " selectedOpt_coaxPass_str: " << this->onOff->getCurrentValue();;
           // Opt_coaxDetailData.insert( "isOpt_coaxPassThrough", this->onOff->getCurrentValue());
            //QJsonDocument doc(Opt_coaxDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_showDlgOfOpt_coaxPass()-Opt_coax_json =: " << strJson;

        }
    }
}


void dialogsetting_interoutput_520::set_settingOfOpt_coaxDetail(QJsonObject tmpJson){
        this->Opt_coaxDetailData = tmpJson;
}

QJsonObject dialogsetting_interoutput_520::get_settingOfOpt_coaxDetail(){
        return this->Opt_coaxDetailData;
}

void dialogsetting_interoutput_520::slot_clickedDVIAB(){

        int btnSenderNo = sender()->property("btnNo").toInt();
        //int selMode =0;
        //qDebug() << "----------------isOpt_coaxPassThrough :" <<  this->Opt_coaxDetailData["isDigitalPassThrough"].toBool();
     //   QJsonObject Opt_coax_json = get_settingOfOpt_coaxDetail();
        QJsonDocument doc0(this->Opt_coaxDetailData);  QString strJson0(doc0.toJson(QJsonDocument::Compact));  qDebug() << "slot_changedIndexOpt_coax()pre-Opt_coax_json =: " << strJson0;

        switch(btnSenderNo){
        case  77 :

            lb_i2s_text->setText("A Mode");
            btn_DVIA->setStyleText("color:#B18658;");
            btn_DVIB->setStyleText("color:#707070;");

            wwbtn_DVIA->setStyleSheet("border:3px solid #B18658;");
            wwbtn_DVIB->setStyleSheet("border:1px solid #707070;");
            //qDebug() << " selectedDVI_mode: A Mode " ;
            ToastMsg::show(this, "", tr("PS Audio output is selected."));
            this->Opt_coaxDetailData.insert( "javs", 0);

            break;
        case  78 :
            lb_i2s_text->setText("B Mode");
            btn_DVIB->setStyleText("color:#B18658;");
            btn_DVIA->setStyleText("color:#707070;");
            wwbtn_DVIA->setStyleSheet("border:1px solid #707070;");
            wwbtn_DVIB->setStyleSheet("border:3px solid #B18658;");

            //qDebug() << " selectedDVI_mode: B Mode " ;
            ToastMsg::show(this, "", tr("JAVS Audio output is selected."));
            this->Opt_coaxDetailData.insert( "javs", 1);

            break;

        }

        QJsonDocument doc(this->Opt_coaxDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "Opt_coax_json =: " << strJson;

}

void dialogsetting_interoutput_520::slot_changedIndexOpt_coax(int index){

    int btnSenderNo = sender()->property("btnNo").toInt();
    QString selectedOpt_coaxMQA_str;
    //qDebug() << "----------------isOpt_coaxPassThrough :" <<  this->Opt_coaxDetailData["isDigitalPassThrough"].toBool();
    //QJsonObject Opt_coax_json = get_settingOfOpt_coaxDetail();
    QJsonDocument doc0(this->Opt_coaxDetailData);  QString strJson0(doc0.toJson(QJsonDocument::Compact));  qDebug() << "slot_changedIndexOpt_coax()pre-Opt_coax_json =: " << strJson0;

    switch(btnSenderNo){
    case  0 :
        selectedOpt_coaxMQA_str = this->comboBoxOpt_coaxMQA->itemText(index);
        //qDebug() << " selectedOpt_coax_str: " << selectedOpt_coaxMQA_str;
        this->Opt_coaxDetailData.insert( "mqaType", index);
        //set_settingOfOpt_coaxDetail(Opt_coaxDetailData);

        break;
    case  1 :
        selectedOpt_coaxMQA_str = comboBoxOutputPCMResampling->itemText(index);
        qDebug() << " selectedOutput_str: " << selectedOpt_coaxMQA_str;
        Opt_coaxDetailData.insert( "samplingRate", index);
     //   set_settingOfOutputDetail(OutputDetailData);

//Opt_coaxDetailData
        break;
    case  11 :
        selectedOpt_coaxMQA_str = comboBoxOutputPCMMaxSampling->itemText(index);
        print_debug();
        qDebug() << " MaxSamplingRate_index: " << index;
        Opt_coaxDetailData.insert( "maxSamplingRate", index);
        //comboBoxHdmiPCM->clear();
        if(index == 0){
            comboBoxOutputPCMResampling->setMaxVisibleItems(7);

        }
        if(index == 1){
            comboBoxOutputPCMResampling->setMaxVisibleItems(3);


        }
        if(index == 2){
            comboBoxOutputPCMResampling->setMaxVisibleItems(5);

        }
        if(index == 3){
            comboBoxOutputPCMResampling->setMaxVisibleItems(7);

        }

        break;
    case  2 :
        selectedOpt_coaxMQA_str = this->comboBoxOpt_coaxDSD->itemText(index);
        //qDebug() << " selectedOpt_coax_str: " << selectedOpt_coaxMQA_str;
        this->Opt_coaxDetailData.insert( "dsdMode", index);
        //set_settingOfOpt_coaxDetail(Opt_coaxDetailData);


        break;
    case  3 :
        selectedOpt_coaxMQA_str = this->comboBoxOpt_coaxRoon->itemText(index);
        //qDebug() << " selectedOpt_coax_str: " << selectedOpt_coaxMQA_str;
        this->Opt_coaxDetailData.insert( "digitalVolume", index);
        //set_settingOfOpt_coaxDetail(Opt_coaxDetailData);
        break;



    }
 //   Opt_coax_json = get_settingOfOpt_coaxDetail();
    QJsonDocument doc(this->Opt_coaxDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "Opt_coax_json =: " << strJson;



}


QWidget* dialogsetting_interoutput_520::getUIControlOption(QString p_title){

    this->lb_onOfftitle = new QPushButton(p_title);
    this->lb_onOfftitle->setProperty("btnNo", 4);
    this->lb_onOfftitle->setStyleSheet("color:#E5E5E4;");
    this->lb_onOfftitle->setFixedSize(630,70);
    this->lb_onOfftitle->setText(p_title);
    this->lb_onOfftitle->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");
    this->lb_onOfftitle->setCursor(Qt::PointingHandCursor);//print_debug();
    onOff = new OnOffWidget;
    //print_debug()
    //onOff->setValue(false);
    onOff->setCursor(Qt::PointingHandCursor);//print_debug();

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,0,30,0);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(this->lb_onOfftitle);
    hl_lb->addWidget(onOff, 0, Qt::AlignRight);//print_debug();
    QPushButton *btn_onOffHover = new QPushButton(onOff);//print_debug();
    btn_onOffHover->setProperty("btnNo", 4);
    btn_onOffHover->setFixedSize(80,40);
    btn_onOffHover->setStyleSheet("background-color:transparent;border:0px;");

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");//print_debug();
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);//print_debug();

    // 커넥션

    connect(this->lb_onOfftitle, SIGNAL(clicked()), this, SLOT(slot_modeOnOff()));
    connect(btn_onOffHover, SIGNAL(clicked()), this, SLOT(slot_modeOnOff()));
    return widget_total;
}

//"font:bold;color:#FFFFFF;font-size:22px;"
//"#widget_title {background-color:transparent;border-bottom:1px solid #404040;}"

QWidget* dialogsetting_interoutput_520::getPlusMinusUIControlOption(QString p_title, int i, int data){

    this->lb_gain[i] = new QPushButton(p_title);
    this->lb_gain[i]->setStyleSheet("color:#E5E5E4;");
    this->lb_gain[i]->setFixedSize(330,70);
    this->lb_gain[i]->setText(p_title);
    this->lb_gain[i]->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4;");//background-color:707070; ");
    this->lb_gain[i]->setCursor(Qt::PointingHandCursor);//print_debug();

   // QLabel *lb_mqagain = new QLabel(p_title);
   // lb_mqagain->setAlignment(Qt::AlignCenter);
   // lb_mqagain->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    this->btn_gain_add[i] = new RemoteBtnItem();
    this->btn_gain_add[i]->setProperty("btnNo", 5+2*i);
    this->btn_gain_add[i]->setData(":/images/vol_add_remote.png", "");
   // btn_vol_add->setProperty("httpId",HTTP_REMOTE_VOLUME_ADD);
    this->btn_gain_add[i]->setCircle(35);
    this->btn_gain_add[i]->setCursor(Qt::PointingHandCursor);
    this->btn_gain_sub[i] = new RemoteBtnItem();
    this->btn_gain_sub[i]->setProperty("btnNo", 5+(2*i+1));
    this->btn_gain_sub[i]->setData(":/images/vol_sub_remote.png", "");
   // btn_vol_sub->setProperty("httpId",HTTP_REMOTE_VOLUME_SUB);
    this->btn_gain_sub[i]->setCircle(35);
    this->btn_gain_sub[i]->setCursor(Qt::PointingHandCursor);

    this->lb_text[i] = new QLabel(tr("text"));

    this->lb_text[i]->setText(QString("%1").arg(data) + tr("           ") + tr("dB"));
    this->lb_text[i]->setContentsMargins(0,10,15,10);
    this->lb_text[i]->setAlignment(Qt::AlignRight);
    this->lb_text[i]->setStyleSheet("font:bold; color:#FFFFFF;font-size:22px;");//background-color:#707070;");
    this->lb_text[i]->setFixedSize(300,40); //100x556
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
    hl_mqagain->addWidget(btn_gain_add[i], 1, Qt::AlignCenter);
    hl_mqagain->setSpacing(10);
    hl_mqagain->addWidget(widget_text, 0, Qt::AlignCenter);
    hl_mqagain->setSpacing(10);
    hl_mqagain->addWidget(btn_gain_sub[i], 1, Qt::AlignCenter);

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

void dialogsetting_interoutput_520::slot_clickedOpt_coaxaddmqaGain(){

    if( this->mqadBData < 0 )
        this->mqadBData += 1;
    this->lb_text[0]->setText(QString("%1").arg(this->mqadBData) + tr("           ") + tr("dB"));
    this->Opt_coaxDetailData.insert( "mqaRelplayGain", this->mqadBData);
    //qDebug() << "mqadBData : " << this->mqadBData;

}
void dialogsetting_interoutput_520::slot_clickedOpt_coaxsubmqaGain(){
    if( mqadBData > -20 )
        mqadBData -= 1;
    this->lb_text[0]->setText(QString("%1").arg(this->mqadBData) + tr("           ") + tr("dB"));
    this->Opt_coaxDetailData.insert( "mqaRelplayGain", this->mqadBData);
    //qDebug() << "mqadBData : " << this->mqadBData;
}

void dialogsetting_interoutput_520::slot_clickedOpt_coaxaddoutputGain(){

    if( outputdBData < 0 )
        outputdBData += 1;
    this->lb_text[1]->setText(QString("%1").arg(outputdBData) + tr("           ") + tr("dB"));
    this->Opt_coaxDetailData.insert( "outputGain", outputdBData);
    //qDebug() << "outputdBData : " << outputdBData;

}
void dialogsetting_interoutput_520::slot_clickedOpt_coaxsuboutputGain(){
    if( outputdBData > -20 )
        outputdBData -= 1;
    lb_text[1]->setText(QString("%1").arg(outputdBData) + tr("           ") + tr("dB"));
    Opt_coaxDetailData.insert( "outputGain", outputdBData);
    //qDebug() << "outputdBData : " << outputdBData;
}
/**
 * @brief SettingTimer::getUIControlOption 콤보박스 포함된 ROW 생성한다
 * @param p_title
 * @return
 */
QWidget* dialogsetting_interoutput_520::getUIControlOption(QString p_title, QString p_content, int index){

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
void dialogsetting_interoutput_520::slot_modeOnOff(){

   // DialogConfirm *dlg = new DialogConfirm(this);
   // dlg->setTitle(tr("HDMI Passthrough"));

    //dlg->setText(tr("Do you want to change it?"));

   // int result = dlg->exec();
   // if(result==QDialog::Accepted){
    //}

       bool ispass = !this->onOff->getCurrentValue();
       if(ispass) {
           this->Opt_coaxDetailData.insert("isDigitalPassThrough", true);

       }else{
           this->Opt_coaxDetailData.insert("isDigitalPassThrough", false);

       }
       this->onOff->setValue(ispass);
       //QMessageBox::information(this,"Title","dddddddddddd");
       this->Opt_coaxDetailData.insert( "isDigitalPassThrough", ispass);

       dlgOpt_coaxPass();
       QTimer::singleShot(500, this, SLOT(slot_showDlgOfOpt_coaxNotice()));

      // notice_msg = tr("It bypasses HD audio formats such as surround sound sources to the audio receiver through HDMI without decoding.");
      // lb_notice->setText(tr("- Notice - ") + notice_msg);
       QJsonDocument doc(this->Opt_coaxDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_modeOnOff()-Opt_coax_json =: " << strJson;



}


void dialogsetting_interoutput_520::slot_print(){
    //qDebug() << "ffffffffffffffffffffffff";
}

/**
 * @brief DialogSelect_Cloud::slot_clickedAdd : [슬롯] 추가 버튼 클릭
 */

const int HTTP_HDMI_GET_MODE  = 155;
const int HTTP_HDMI_SET_MODE  = 156;

void dialogsetting_interoutput_520::getOpt_coaxSettingOfsetting(){

   NetworkHttp *network = new NetworkHttp;
   QJsonObject tmp_json;
   tmp_json.insert("outputMode", 1);
   connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
   network->request(HTTP_HDMI_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.setting.get"), tmp_json, true, true);

}

void dialogsetting_interoutput_520::getOpt_coaxInOutSettingOfsetting(){

   NetworkHttp *network = new NetworkHttp;
   QJsonObject tmp_json;
   connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
   network->request(HTTP_HDMI_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("in.out.mode.get"), tmp_json, true);

}
void dialogsetting_interoutput_520::slot_clickedOpt_coaxChangeSet(){
   //print_debug();

   NetworkHttp *network = new NetworkHttp;
   QJsonObject tmp_json = get_settingOfOpt_coaxDetail();
   QJsonObject tmp_json_set;
   tmp_json_set.insert("data", tmp_json);
   tmp_json_set.insert("outputMode", 1);

   //ToastMsg::show(this, "", tr("Save the information from the Rose deive."));
   QJsonDocument doc(tmp_json_set);  QString strJson(doc.toJson(QJsonDocument::Compact));
   //qDebug() <<"slot_clickedOpt_coaxChangeSet()-tmp_json_set :"<< strJson;//cheon09_io

   connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
   network->request(HTTP_HDMI_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.setting.set"), tmp_json_set, true, true);

}

void dialogsetting_interoutput_520::slot_modeOnOff_32(){

   // DialogConfirm *dlg = new DialogConfirm(this);
   // dlg->setTitle(tr("HDMI Passthrough"));

    //dlg->setText(tr("Do you want to change it?"));

   // int result = dlg->exec();
   // if(result==QDialog::Accepted){
    //}

       int ispass = !this->onOff_32->getCurrentValue();
       if(ispass) {
           Opt_coaxDetailData.insert("isClearLowerBit", true);
           this->onOff_32->setValue(true);

       }else{
           Opt_coaxDetailData.insert("isClearLowerBit", false);
           this->onOff_32->setValue(false);

       }

       //QMessageBox::information(this,"Title","dddddddddddd");
      // OutputDetailData.insert( "isClearLowerBit", ispass);


      // notice_msg = tr("It bypasses HD audio formats such as surround sound sources to the audio receiver through HDMI without decoding.");
      // lb_notice->setText(tr("- Notice - ") + notice_msg);
       QJsonDocument doc(Opt_coaxDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_modeOnOff()-Hdmi_json =: " << strJson;



}

void dialogsetting_interoutput_520::setDialogOpt_coax_show(){
   // QJsonObject *Opt_coax_json = get_settingOfOpt_coaxDetail();

   QJsonDocument doc(this->Opt_coaxDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));
   //qDebug() <<"setDialogOpt_coax_show()-Opt_coaxDetailData :"<< strJson;//cheon09_io

    this->comboBoxOpt_coaxMQA->setCurrentIndex( this->Opt_coaxDetailData["mqaType"].toInt() );
    comboBoxOutputPCMMaxSampling->setCurrentIndex( Opt_coaxDetailData["maxSamplingRate"].toInt() );
    comboBoxOutputPCMResampling->setCurrentIndex( Opt_coaxDetailData["samplingRate"].toInt() );
    this->comboBoxOpt_coaxDSD->setCurrentIndex( this->Opt_coaxDetailData["dsdMode"].toInt() );
    this->comboBoxOpt_coaxRoon->setCurrentIndex( this->Opt_coaxDetailData["digitalVolume"].toInt() );
    this->mqadBData = this->Opt_coaxDetailData["mqaRelplayGain"].toInt();


    this->lb_text[0]->setText(QString("%1").arg(this->mqadBData) + tr("           ") + tr("dB"));
    //qDebug() << "setCurrentIndex_mqaRelplayGain :" <<  this->Opt_coaxDetailData["mqaRelplayGain"].toInt();

    this->outputdBData = this->Opt_coaxDetailData["outputGain"].toInt();
    lb_text[1]->setText(QString("%1").arg(this->outputdBData) + tr("           ") + tr("dB"));

    onOff_32->setValue(Opt_coaxDetailData["isClearLowerBit"].toBool());
    //this->onOff->setValue(this->outputdBData);

 //-----------------------------------------------------------------
    if( this->Opt_coaxDetailData["javs"].toInt()==0){
        lb_i2s_text->setText("A Mode");
        btn_DVIA->setStyleText("color:#B18658;");
        btn_DVIB->setStyleText("color:#707070;");

        wwbtn_DVIA->setStyleSheet("border:3px solid #B18658;");
        wwbtn_DVIB->setStyleSheet("border:1px solid #707070;");
    }else{
        lb_i2s_text->setText("B Mode");
        btn_DVIB->setStyleText("color:#B18658;");
        btn_DVIA->setStyleText("color:#707070;");

        wwbtn_DVIB->setStyleSheet("border:3px solid #B18658;");
        wwbtn_DVIA->setStyleSheet("border:1px solid #707070;");
    }


//-------------------------------------------------------------------------------

   // ToastMsg::show(this, "", tr("The current HDMI setting information is displayed on the screen."));

}


void dialogsetting_interoutput_520::setDialogOpt_coax_save(QJsonObject p_jsonObject){
    QJsonObject tmp_Json;
    if(p_jsonObject.contains("data")){
        tmp_Json = p_jsonObject["data"].toObject(); //cheon09_io

    }
   // QJsonDocument doc(tmp_Json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"\npre-setDialogOpt_coax_save()-tmp_Json :"<< strJson;//cheon09_io
     int codecTrim = tmp_Json["codecTrim"].toInt();
     int dacFilterType = tmp_Json["dacFilterType"].toInt();
     int drcInfo = tmp_Json["drcInfo"].toInt();
     int dsdMode = tmp_Json["dsdMode"].toInt();
     int hpOhm = tmp_Json["hpOhm"].toInt();
     bool isDigitalPassThrough = tmp_Json["isDigitalPassThrough"].toBool();
     bool isClearLowerBit = tmp_Json["isClearLowerBit"].toBool();
     this->Opt_coaxDetailData.insert( "isClearLowerBit", isClearLowerBit);
     bool isHdmiPassThrough = tmp_Json["isHdmiPassThrough"].toBool();
     int javs = tmp_Json["javs"].toInt();
     int mqaRelplayGain = tmp_Json["mqaRelplayGain"].toInt();
     int mqaType = tmp_Json["mqaType"].toInt();
   //  int opticalApwMode = tmp_Json["opticalApwMode"].toInt();
    // int opticalApwValue = tmp_Json["opticalApwValue"].toInt();
     int opticalLsb = tmp_Json["opticalLsb"].toInt();
     int opticalMsb = tmp_Json["opticalMsb"].toInt();
     int outputGain = tmp_Json["outputGain"].toInt();
     int phaseInvert = tmp_Json["phaseInvert"].toInt();
     int preoutLevelInfo = tmp_Json["preoutLevelInfo"].toInt();
     int digitalVolume = tmp_Json["digitalVolume"].toInt();
     int samplingRate = tmp_Json["samplingRate"].toInt();
     int maxSamplingRate = tmp_Json["maxSamplingRate"].toInt();
     Opt_coaxDetailData.insert( "maxSamplingRate", maxSamplingRate);
     int volumeMaxLevel = tmp_Json["volumeMaxLevel"].toInt();
     int volumeMinLevel = tmp_Json["volumeMinLevel"].toInt();

     //qDebug() << "isDigitalPassThrough : " << isDigitalPassThrough;

     this->Opt_coaxDetailData.insert( "codecTrim", codecTrim);
     this->Opt_coaxDetailData.insert( "dacFilterType", dacFilterType);

     this->Opt_coaxDetailData.insert( "drcInfo", drcInfo);
     this->Opt_coaxDetailData.insert( "dsdMode", dsdMode);
     this->Opt_coaxDetailData.insert( "hpOhm", hpOhm);
     this->Opt_coaxDetailData.insert( "isDigitalPassThrough", isDigitalPassThrough);
     this->Opt_coaxDetailData.insert( "isHdmiPassThrough", isHdmiPassThrough);
     this->Opt_coaxDetailData.insert( "javs", javs);
     this->Opt_coaxDetailData.insert( "mqaRelplayGain", mqaRelplayGain);
     this->Opt_coaxDetailData.insert( "mqaType", mqaType);
  //   Opt_coaxDetailData.insert( "opticalApwMode", opticalApwMode);
    // Opt_coaxDetailData.insert( "opticalApwValue", opticalApwValue);
     this->Opt_coaxDetailData.insert( "opticalLsb", opticalLsb);
     this->Opt_coaxDetailData.insert( "opticalMsb", opticalMsb);
     this->Opt_coaxDetailData.insert( "outputGain", outputGain);
     this->Opt_coaxDetailData.insert( "phaseInvert", phaseInvert);
     this->Opt_coaxDetailData.insert( "preoutLevelInfo", preoutLevelInfo);
     this->Opt_coaxDetailData.insert( "digitalVolume", digitalVolume);
     this->Opt_coaxDetailData.insert( "samplingRate", samplingRate);
     this->Opt_coaxDetailData.insert( "volumeMaxLevel", volumeMaxLevel);
     this->Opt_coaxDetailData.insert( "volumeMinLevel", volumeMinLevel);

     //qDebug() << "json-isDigitalPassThrough : " << this->Opt_coaxDetailData["isDigitalPassThrough"].toBool();


     QJsonDocument doc1(this->Opt_coaxDetailData);  QString strJson1(doc1.toJson(QJsonDocument::Compact));  qDebug() <<"\nsetDialogOpt_coax_save()-tmp_Json :"<< strJson1 << "\n";//cheon09_io

     //ToastMsg::show(this, "", tr("Save the information from the Rose deive."));
}

void dialogsetting_interoutput_520::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    QString deviceType = global.device.getDeviceType();
  //  print_debug();//cheon0219

    print_debug();  QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nslot_responseHttp =: " << strJson;//cheon12_io
    //qDebug() << "p_id : " << p_id;
    QString internalMode;

    switch(p_id){
    case HTTP_HDMI_GET_MODE :

        if( deviceType == "RS150" || deviceType == "RS150B"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                setDialogOpt_coax_save(p_jsonObject);
                setDialogOpt_coax_show();

            }

        }  else      if( deviceType == "RS520"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                setDialogOpt_coax_save(p_jsonObject);
                setDialogOpt_coax_show();

            }

        }else if( deviceType == "RS201"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                setDialogOpt_coax_save(p_jsonObject);
                setDialogOpt_coax_show();
               // setDialogOpt_coax_save(p_jsonObject);

            }

        }else if( deviceType == "RS301"){

        }else if( deviceType == "RS350"){

        }
        break;
    case HTTP_HDMI_SET_MODE :

        if( deviceType == "RS150" || deviceType == "RS150B"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                getOpt_coaxInOutSettingOfsetting();
                ToastMsg::show(this, "", tr("Setting modification was successful."));

                 setResult(QDialog::Accepted);
                 this->hide();

            }else{
                setResult(QDialog::Rejected);
                this->hide();
            }


        } else       if( deviceType == "RS520"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                getOpt_coaxInOutSettingOfsetting();
                ToastMsg::show(this, "", tr("Setting modification was successful."));

                 setResult(QDialog::Accepted);
                 this->hide();

            }else{
                setResult(QDialog::Rejected);
                this->hide();
            }


        }else if( deviceType == "RS201"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                 getOpt_coaxInOutSettingOfsetting();
                 ToastMsg::show(this, "", tr("Setting modification was successful."));
               //  setResult(QDialog::Accepted);
               //  this->hide();

            }else{
                ToastMsg::show(this, "", tr("Failed to modify settings."));
                setResult(QDialog::Rejected);
                this->hide();
            }

        }else if( deviceType == "RS301"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){

            }
        }else if( deviceType == "RS350"){

        }
        break;
    }
    sender()->deleteLater();
}


void dialogsetting_interoutput_520::set_selected_deviceName(QString usbstr){
    this->selected_deviceName = usbstr;
}
QString dialogsetting_interoutput_520::get_selected_deviceName(){
    return this->selected_deviceName;
}
void dialogsetting_interoutput_520::setTextName(QString usbstr){
    this->radio_UsbName->setText(usbstr);
}

void dialogsetting_interoutput_520::setPropertyName(QString usbstr){
    this->radio_UsbName->setProperty(PROPERTY_NAME_READABLE, usbstr);
}
