#include "dialogsetting_output_250.h"
#include "dialogselect_usbdac_150.h"
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
dialogsetting_output_250::dialogsetting_output_250(QWidget *parent)
    : QDialog(parent)
{
    setInit();
    setUIControl();
    getOutputSettingOfsetting();
}


/**
 * @brief DialogSelect_Cloud::setInit: 초기 세팅
 */
void dialogsetting_output_250::setInit(){
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT+100);
    this->setStyleSheet("background-color:transparent;");
   // OutputDetailData = new QJsonObject();
}



/**
 * @brief DialogSelect_Cloud::setUIControl : UI 세팅
 */
void dialogsetting_output_250::setUIControl(){

    QJsonObject tmp_outputsettingval = get_settingOfOutputDetail();
    dlgConfirmOutput = new DialogConfirm(this);
    // 타이틀
    QLabel *lb_title = new QLabel;
    lb_title->setText(tr("Analog OUTPUT Setting"));//lb_title->setText(tr("클라우드 추가"));
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
    box_top->setAlignment(Qt::AlignCenter);
    box_top->addWidget(lb_title);

    QWidget *widget_title = new QWidget;
    widget_title->setObjectName("widget_title");
    widget_title->setLayout(box_top);
    widget_title->setStyleSheet("#widget_title {background-color:transparent;border-bottom:1px solid #404040;}");

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
    QPushButton *btn_questionPhaseInvert;//c220511
    btn_questionPhaseInvert =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionPhaseInvert->setCursor(Qt::PointingHandCursor);
    btn_questionPhaseInvert->setFixedSize(QSize(30,30));

    comboBoxOutputPreoutLevel = new QComboBox();
    comboBoxOutputPreoutLevel->setProperty("btnNo", 0);


    comboBoxOutputPreoutLevel->addItem(tr(" OFF"), 0);//cheon01_src
    comboBoxOutputPreoutLevel->addItem(tr(" 100 mV"), 1);//cheon01_src
    comboBoxOutputPreoutLevel->addItem(tr(" 150 mV"), 2);//cheon01_src
    comboBoxOutputPreoutLevel->addItem(tr(" 200 mV"), 3);//cheon01_src
    comboBoxOutputPreoutLevel->addItem(tr(" 300 mV"), 4);//cheon01_src
    comboBoxOutputPreoutLevel->addItem(tr(" 500 mV"), 5);//cheon01_src
    comboBoxOutputPreoutLevel->addItem(tr(" 1000 mV"), 6);//cheon01_src
    comboBoxOutputPreoutLevel->addItem(tr(" 2000 mV"), 7);
    comboBoxOutputPreoutLevel->addItem(tr(" 2200 mV"), 8);

    comboBoxOutputPreoutLevel->setContentsMargins(10,0,0,0);
    comboBoxOutputPreoutLevel->setFixedHeight(35);
    comboBoxOutputPreoutLevel->setObjectName("comboBox_outputMsb");//cheon01
    comboBoxOutputPreoutLevel->setFixedWidth(150);


    comboBoxOutputPreoutLevel->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658;}"// border: 1px solid #B18658;   } "
                                );

    QLabel *lb_PreLevel = new QLabel();
    lb_PreLevel->setStyleSheet("color:#E5E5E4");

    lb_PreLevel->setText(tr("Pre-out Level Setting"));
    lb_PreLevel->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");
    lb_PreLevel->setFixedHeight(70);//c220511


    QHBoxLayout *hl_lb_PreLevel = new QHBoxLayout();
    hl_lb_PreLevel->setContentsMargins(33,0,30,0);
    hl_lb_PreLevel->setSpacing(0);
    hl_lb_PreLevel->addWidget(lb_PreLevel);
    hl_lb_PreLevel->addWidget(btn_questionPreLevel, 0, Qt::AlignLeft);//c220511
    hl_lb_PreLevel->addWidget(comboBoxOutputPreoutLevel, 0, Qt::AlignRight);

    QWidget *widget_total_PreLevel = new QWidget();
    //widget_total_PreLevel->setFixedHeight(70);
    widget_total_PreLevel->setObjectName("widget_total");
    widget_total_PreLevel->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total_PreLevel->setLayout(hl_lb_PreLevel);


    comboBoxOutputPCMResampling = new QComboBox();
    comboBoxOutputPCMResampling->setProperty("btnNo", 1);

    comboBoxOutputPCMResampling->addItem(tr(" Original"), 0);//cheon01_src
    //print_debug();
    comboBoxOutputPCMResampling->addItem(tr(" 44.1 kHz"), 1);//cheon01_src

    comboBoxOutputPCMResampling->addItem(tr(" 48 kHz"), 2);//cheon01_src
    comboBoxOutputPCMResampling->addItem(tr(" 88.2 kHz"), 3);//cheon01_src
    comboBoxOutputPCMResampling->addItem(tr(" 96 kHz"), 4);//cheon01_src
    comboBoxOutputPCMResampling->addItem(tr(" 176.4 kHz"), 5);//cheon01_src
    comboBoxOutputPCMResampling->addItem(tr(" 192 kHz"), 6);//cheon01_src



    //comboBox = new QComboBox();
    comboBoxOutputPCMResampling->setContentsMargins(10,0,0,0);
    comboBoxOutputPCMResampling->setFixedHeight(35);
    comboBoxOutputPCMResampling->setObjectName("comboBox_outputLsb");//cheon01
    comboBoxOutputPCMResampling->setFixedWidth(150);

    comboBoxOutputPCMResampling->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; }"//border: 1px solid #B18658;   } "
                                );

    QLabel *lb_PCM = new QLabel(tr("output LSB"));
    lb_PCM->setStyleSheet("color:#E5E5E4");
    lb_PCM->setFixedHeight(120);//c220511
    lb_PCM->setText((tr("PCM Resampling Frequency")));
    lb_PCM->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    comboBoxOutputPCMMaxSampling = new QComboBox();
    comboBoxOutputPCMMaxSampling->setProperty("btnNo", 11);//

    comboBoxOutputPCMMaxSampling->addItem(tr(" Auto"), 0);//cheon01_src
    comboBoxOutputPCMMaxSampling->addItem(tr(" 48 kHz"), 1);//cheon01_src
    comboBoxOutputPCMMaxSampling->addItem(tr(" 96 kHz"), 2);//cheon01_src
    comboBoxOutputPCMMaxSampling->addItem(tr(" 192 kHz"), 3);//cheon01_src


    QLabel *lb_PCM_Max = new QLabel(tr("output LSB"));
    lb_PCM_Max->setStyleSheet("color:#E5E5E4");
    lb_PCM_Max->setFixedHeight(70);//c220511
    lb_PCM_Max->setText((tr("PCM Resampling Frequency")));
    lb_PCM_Max->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    //comboBox = new QComboBox();
    comboBoxOutputPCMMaxSampling->setContentsMargins(10,0,0,0);
    comboBoxOutputPCMMaxSampling->setFixedHeight(35);
    comboBoxOutputPCMMaxSampling->setObjectName("comboBox_outputLsb");//cheon01
    comboBoxOutputPCMMaxSampling->setFixedWidth(150);

    comboBoxOutputPCMMaxSampling->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; }"//border: 1px solid #B18658;   } "
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
    widget_PCM_and_Max->setStyleSheet("#widget_PCM_and_Max { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_PCM_and_Max->setLayout(vl_lb_PCM);

    QHBoxLayout *hl_lb_PCM = new QHBoxLayout();
    hl_lb_PCM->setContentsMargins(33,0,30,0);
    hl_lb_PCM->setSpacing(20);
    hl_lb_PCM->addWidget(lb_PCM);
    hl_lb_PCM->addWidget(btn_questionPCM, 0, Qt::AlignLeft);//c220511
    hl_lb_PCM->addWidget(widget_PCM_and_Max, 0, Qt::AlignRight);//c220511
    //hl_lb_PCM->addWidget(comboBoxHdmiPCM);

    QWidget *widget_total_PCM = new QWidget();
    widget_total_PCM->setObjectName("widget_total");
    widget_total_PCM->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total_PCM->setLayout(hl_lb_PCM);


//-------------------------------------------------------------------------------------------
    comboBoxDSD = new QComboBox();
    comboBoxDSD->setProperty("btnNo", 3);
    comboBoxDSD->addItem(tr(" Native DSD(upto DSD512)"), 0);//cheon01_src
    comboBoxDSD->addItem(tr(" DSD over PCM(Not supported)"), 1);//cheon01_src
    comboBoxDSD->addItem(tr(" DSD to PCM(upto DSD128)"), 2);//cheon01_src

    QLabel *lb_DSD = new QLabel(tr("output DSD"));
    lb_DSD->setStyleSheet("color:#E5E5E4");
    lb_DSD->setFixedHeight(70);//c220511
    lb_DSD->setText((tr("DSD Mode")));
    lb_DSD->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    //comboBox = new QComboBox();
    comboBoxDSD->setContentsMargins(10,0,0,0);
    comboBoxDSD->setFixedHeight(35);
    comboBoxDSD->setObjectName("comboBox_outputLsb");//cheon01
    comboBoxDSD->setFixedWidth(300);

    comboBoxDSD->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; }"//border: 1px solid #B18658;   } "
                                );

    QHBoxLayout *hl_lb_DSD = new QHBoxLayout();
    hl_lb_DSD->setContentsMargins(33,0,30,0);
    hl_lb_DSD->setSpacing(0);
    hl_lb_DSD->addWidget(lb_DSD);
    hl_lb_DSD->addWidget(btn_questionDSD, 0, Qt::AlignLeft);//c220511
    hl_lb_DSD->addWidget(comboBoxDSD, 0, Qt::AlignRight);

    QWidget *widget_combo_DSD = new QWidget();
    //widget_combo_DSD->setFixedHeight(70);
    widget_combo_DSD->setObjectName("widget_total");
    widget_combo_DSD->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_combo_DSD->setLayout(hl_lb_DSD);



   // this->list_keyValue.append(0);
   // this->list_keyValue.append(1);
    comboBoxHOI = new QComboBox();
    comboBoxHOI->setProperty("btnNo", 4);
    comboBoxHOI->addItem(tr(" 16 Ohm"), 0);//cheon01_src
    comboBoxHOI->addItem(tr(" 32 Ohm"), 1);//cheon01_src
    comboBoxHOI->addItem(tr(" 50 Ohm"), 2);//cheon01_src
    comboBoxHOI->addItem(tr(" 100 Ohm"), 3);


    //comboBox = new QComboBox();
    comboBoxHOI->setContentsMargins(0,0,0,0);
    comboBoxHOI->setFixedHeight(35);
    comboBoxHOI->setObjectName("comboBox_opt_coax");//cheon01
    comboBoxHOI->setFixedWidth(150);


    comboBoxHOI->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "

                                );

    QLabel *lb_HOI = new QLabel(tr("Headphone Output Impedence"));
    lb_HOI->setStyleSheet("color:#E5E5E4");
    lb_HOI->setFixedSize(270,70);//c220511
    lb_HOI->setText((tr("Headphone Output Impedence")));
    lb_HOI->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    QHBoxLayout *hl_lb_HOI = new QHBoxLayout();
    hl_lb_HOI->setContentsMargins(33,0,30,0);
    hl_lb_HOI->setSpacing(0);
    hl_lb_HOI->addWidget(lb_HOI);
    hl_lb_HOI->addWidget(btn_questionHOI, 0, Qt::AlignLeft);//c220511
    hl_lb_HOI->addWidget(comboBoxHOI, 0, Qt::AlignRight);

    QWidget *widget_total_HOI = new QWidget();
    //widget_total_HOI->setFixedHeight(70);
    widget_total_HOI->setObjectName("widget_total");
    widget_total_HOI->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total_HOI->setLayout(hl_lb_HOI);
    //----------------------------------------------------------------------------

    comboBoxPhaseInvert = new QComboBox();
    comboBoxPhaseInvert->setProperty("btnNo", 6);

    comboBoxPhaseInvert->addItem(tr("Blick Wall filter"), 0);
    comboBoxPhaseInvert->addItem(tr("Corrected minimum phase Fast Roll-off filter"), 1);
    comboBoxPhaseInvert->addItem(tr("Apoizing Fast Roll-off Filter"), 2);
    comboBoxPhaseInvert->addItem(tr("Minimum phase Slow Roll-off filter"), 3);
    comboBoxPhaseInvert->addItem(tr("Minimum phase Fast Roll-off filter"), 4);
    comboBoxPhaseInvert->addItem(tr("Linear phase Slow Roll-off filter"), 5);
    comboBoxPhaseInvert->addItem(tr("Linear phase Fast Roll-off filter"), 6);

    QLabel *lb_PhaseInvert = new QLabel(tr("FIR Interpolation Filter"));
    lb_PhaseInvert->setStyleSheet("color:#E5E5E4");
    lb_PhaseInvert->setFixedHeight(70);//c220511
    lb_PhaseInvert->setText((tr("FIR Interpolation Filter")));
    lb_PhaseInvert->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

   // this->list_keyValue.append(0);
   // this->list_keyValue.append(1);


    //comboBox = new QComboBox();
    comboBoxPhaseInvert->setContentsMargins(0,0,0,0);
    comboBoxPhaseInvert->setFixedHeight(35);
    comboBoxPhaseInvert->setObjectName("comboBoxPhaseInvert");//cheon01
    comboBoxPhaseInvert->setFixedWidth(350);


    comboBoxPhaseInvert->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                );




    QHBoxLayout *hl_lb_PhaseInvert = new QHBoxLayout();
    hl_lb_PhaseInvert->setContentsMargins(33,0,30,0);
    hl_lb_PhaseInvert->setSpacing(0);
    hl_lb_PhaseInvert->addWidget(lb_PhaseInvert);
    hl_lb_PhaseInvert->addWidget(btn_questionPhaseInvert, 0, Qt::AlignLeft);//c220511
    hl_lb_PhaseInvert->addWidget(comboBoxPhaseInvert, 0, Qt::AlignRight);

    QWidget *widget_total_PhaseInvert = new QWidget();
    // widget_total_Roon->setFixedHeight(70);
    widget_total_PhaseInvert->setObjectName("widget_total");
    widget_total_PhaseInvert->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total_PhaseInvert->setLayout(hl_lb_PhaseInvert);

//------------------------------------------------------------
    comboBoxSofwareVol = new QComboBox();
    comboBoxSofwareVol->setProperty("btnNo", 5);

    comboBoxSofwareVol->addItem(tr("OFF"), 0);//c220518
    comboBoxSofwareVol->addItem(tr("ON"), 1);//c220518

    QLabel *lb_Roon = new QLabel(tr("Software Volume Control"));
    lb_Roon->setStyleSheet("color:#E5E5E4");
    lb_Roon->setFixedHeight(70);//c220511
    lb_Roon->setText((tr("Software Volume Control")));
    lb_Roon->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

   // this->list_keyValue.append(0);
   // this->list_keyValue.append(1);


    //comboBox = new QComboBox();
    comboBoxSofwareVol->setContentsMargins(0,0,0,0);
    comboBoxSofwareVol->setFixedHeight(35);
    comboBoxSofwareVol->setObjectName("comboBoxSofwareVol");//cheon01
    comboBoxSofwareVol->setFixedWidth(150);


    comboBoxSofwareVol->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                );




    QHBoxLayout *hl_lb_Roon = new QHBoxLayout();
    hl_lb_Roon->setContentsMargins(33,0,30,0);
    hl_lb_Roon->setSpacing(20);
    hl_lb_Roon->addWidget(lb_Roon);
    hl_lb_Roon->addWidget(btn_questionRoon, 0, Qt::AlignLeft);//c220511
    hl_lb_Roon->addWidget(comboBoxSofwareVol, 0, Qt::AlignRight);

    QWidget *widget_total_Roon = new QWidget();
    // widget_total_Roon->setFixedHeight(70);
    widget_total_Roon->setObjectName("widget_total");
    widget_total_Roon->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total_Roon->setLayout(hl_lb_Roon);

//-------------------------------------------------------------------------
    QVBoxLayout *vl_total_32 = new QVBoxLayout();
    vl_total_32->setContentsMargins(0,0,0,0);
    vl_total_32->setSpacing(0);
    vl_total_32->setAlignment(Qt::AlignTop);
    vl_total_32->addWidget(this->getUIControlOption_bit32(tr("The low 8bits of a 32 bit sound sources will have correct")));//print_debug();


    QVBoxLayout *vl_total1 = new QVBoxLayout();
    vl_total1->setContentsMargins(0,0,0,0);
    vl_total1->setSpacing(0);
    vl_total1->setAlignment(Qt::AlignTop);
    vl_total1->addWidget(widget_total_PreLevel);//, 0, Qt::AlignVCenter);
    vl_total1->addWidget(widget_total_PCM);//, 0, Qt::AlignVCenter);
    vl_total1->addWidget(widget_total_HOI);//, 0, Qt::AlignVCenter);
    vl_total1->addWidget(widget_combo_DSD);//, 0, Qt::AlignVCenter);
    vl_total1->addWidget(widget_total_Roon);//, 0, Qt::AlignVCenter);
    vl_total1->addWidget(widget_total_PhaseInvert);//, 0, Qt::AlignVCenter);
    vl_total1->addLayout(vl_total_32);




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
    vl_total2->setAlignment(Qt::AlignCenter);
    vl_total2->addWidget(widget_title);
    vl_total2->addLayout(vl_total1);
    vl_total2->addSpacing(30);//200

    vl_total2->addLayout(hl_btn);


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
    connect(btn_questionRoon, SIGNAL(clicked()), this, SLOT(slot_clickedRoonquestion()));//c220511
    connect(btn_questionHOI, SIGNAL(clicked()), this, SLOT(slot_clickedHOIquestion()));//c220511
    connect(btn_questionPreLevel, SIGNAL(clicked()), this, SLOT(slot_clickedPreLevelquestion()));//c220511
    connect(btn_questionPhaseInvert, SIGNAL(clicked()), this, SLOT(slot_clickedPhaseInvertquestion()));//c220511

    connect(comboBoxOutputPreoutLevel, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOutput(int)));
    connect(comboBoxOutputPCMResampling, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOutput(int)));
    connect(comboBoxOutputPCMMaxSampling, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOutput(int)));

    connect(comboBoxDSD, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOutput(int)));
    connect(comboBoxHOI, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOutput(int)));
    connect(comboBoxSofwareVol, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOutput(int)));
    connect(comboBoxPhaseInvert, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOutput(int)));

    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(close()));//print_debug();
    connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clickedOutputChangeSet()));

}

void dialogsetting_output_250::slot_clickedDSDquestion(){//c220511
    print_debug();
    dlgConfirmOutput->setAlignment(Qt::AlignLeft);
    dlgConfirmOutput->setTitle(tr("DSD Mode"));
    dlgConfirmOutput->setText(tr("Determines the transmission format (Native, DOP, PCM) of audio samples to be transmitted to the DAC when playing DSD sound sources. \n For HDMI output, only DSD to PCM is available."));

    dlgConfirmOutput->setGeometry((DLG_WIDTH + 80), (350 + 100), 350, 500);
    dlgConfirmOutput->setAlertMode();
    dlgConfirmOutput->setProperty("flagShown",false);

    if(dlgConfirmOutput->property("flagShown").toBool()==false){
        dlgConfirmOutput->setProperty("flagShown",true);

        int result = dlgConfirmOutput->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}

void dialogsetting_output_250::slot_clickedPCMquestion(){//c220511
    print_debug();
    dlgConfirmOutput->setAlignment(Qt::AlignLeft);
    dlgConfirmOutput->setTitle(tr("PCM Resampling Frequency"));
    dlgConfirmOutput->setText(tr("This is The menu to set sampling frequecy. if you want to output the original sound as it is, select the Original Sampling Rate, and if want to use the resampling function, select the desired sampling rate (48kHz, 96Khz, 192Khz, etc), MQA, Native DSD, DOP, Video, Bluetooth and Airplay are not supported."));

    dlgConfirmOutput->setGeometry((DLG_WIDTH + 80), (350 + 100), 350, 500);
    dlgConfirmOutput->setAlertMode();
    dlgConfirmOutput->setProperty("flagShown",false);

    if(dlgConfirmOutput->property("flagShown").toBool()==false){
        dlgConfirmOutput->setProperty("flagShown",true);

        int result = dlgConfirmOutput->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}

void dialogsetting_output_250::slot_clickedPreLevelquestion(){//c220511
    print_debug();
    dlgConfirmOutput->setAlignment(Qt::AlignLeft);
    dlgConfirmOutput->setTitle(tr("PCM Resampling Frequency"));
    dlgConfirmOutput->setText(tr("This is The menu to set sampling frequecy. if you want to output the original sound as it is, select the Original Sampling Rate, and if want to use the resampling function, select the desired sampling rate (48kHz, 96Khz, 192Khz, etc), MQA, Native DSD, DOP, Video, Bluetooth and Airplay are not supported."));

    dlgConfirmOutput->setGeometry((DLG_WIDTH + 80), (350 + 100), 350, 500);
    dlgConfirmOutput->setAlertMode();
    dlgConfirmOutput->setProperty("flagShown",false);

    if(dlgConfirmOutput->property("flagShown").toBool()==false){
        dlgConfirmOutput->setProperty("flagShown",true);

        int result = dlgConfirmOutput->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}
void dialogsetting_output_250::slot_clickedPhaseInvertquestion(){//c220511
    print_debug();
    dlgConfirmOutput->setAlignment(Qt::AlignLeft);
    dlgConfirmOutput->setTitle(tr("FIR Interpolation phase Filter"));
    dlgConfirmOutput->setText(tr("If you use MQA stream, it is recommended to use 'Minimum phase Fast Roll-off filter'.\nFIR filter is applied only to PCM data, not to DSD."));

    dlgConfirmOutput->setGeometry((DLG_WIDTH + 80), (350 + 100), 350, 500);
    dlgConfirmOutput->setAlertMode();
    dlgConfirmOutput->setProperty("flagShown",false);

    if(dlgConfirmOutput->property("flagShown").toBool()==false){
        dlgConfirmOutput->setProperty("flagShown",true);

        int result = dlgConfirmOutput->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}


void dialogsetting_output_250::slot_clickedHOIquestion(){//c220511
    print_debug();
    dlgConfirmOutput->setAlignment(Qt::AlignLeft);
    dlgConfirmOutput->setTitle(tr("Headphone Output Impedance"));
    dlgConfirmOutput->setText(tr("This is a menu to set the Headphone Output Impedace.\nIf you select the output (16 Ohm, 32 Ohm, 50 Ohm, 100 Ohm) that matches your headphone specifications, you can enjoy better sound."));

    dlgConfirmOutput->setGeometry((DLG_WIDTH + 80), (350 + 100), 350, 500);
    dlgConfirmOutput->setAlertMode();
    dlgConfirmOutput->setProperty("flagShown",false);

    if(dlgConfirmOutput->property("flagShown").toBool()==false){
        dlgConfirmOutput->setProperty("flagShown",true);

        int result = dlgConfirmOutput->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}


void dialogsetting_output_250::slot_clickedSoftwareVolquestion(){//c220511
    print_debug();
    dlgConfirmOutput->setAlignment(Qt::AlignLeft);
    dlgConfirmOutput->setTitle(tr("Software Volume Control"));
    dlgConfirmOutput->setText(tr("Adjust the playback volume of digital audio data using the Rose volume control software.\nCaution 1. It cannot be used in Native DSD or DSD over PCM mode. It is applied after changing to DSD to PCM(upto DSD128) mode.\nCaution 2. For digital output. MQA Stream is supported only when the volume is MAX. If it is less than MAX. MQA Stream is not supported."));
    dlgConfirmOutput->setTextHeight(250);
    dlgConfirmOutput->setAlignment(Qt::AlignLeft);
    dlgConfirmOutput->setGeometry((DLG_WIDTH + 80), (350 + 100), 350, 500);

    dlgConfirmOutput->setAlertMode();
    dlgConfirmOutput->setProperty("flagShown",false);

    if(dlgConfirmOutput->property("flagShown").toBool()==false){
        dlgConfirmOutput->setProperty("flagShown",true);

        int result = dlgConfirmOutput->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}

QWidget* dialogsetting_output_250::getUIControlOption_bit32(QString p_title){

    QPushButton *lb_onOfftitle = new QPushButton(p_title);
    lb_onOfftitle->setProperty("btnNo", 4);
    lb_onOfftitle->setStyleSheet("color:#E5E5E4;");
    lb_onOfftitle->setFixedSize(500,70);
    lb_onOfftitle->setText(p_title);
    lb_onOfftitle->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");
    lb_onOfftitle->setCursor(Qt::PointingHandCursor);//print_debug();
    onOff_32 = new OnOffWidget;print_debug()
    //onOff->setValue(false);
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
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);
    print_debug();

    // 커넥션
    connect(btn_question, SIGNAL(clicked()), this, SLOT(slot_clickedbit32question()));

    //connect(lb_onOfftitle, SIGNAL(clicked()), this, SLOT(slot_modeOnOff()));
    connect(btn_onOffHover, SIGNAL(clicked()), this, SLOT(slot_modeOnOff_32()));
    return widget_total;

}
void dialogsetting_output_250::slot_modeOnOff_32(){


       int ispass = !this->onOff_32->getCurrentValue();
       if(ispass) {
           OutputDetailData.insert("isClearLowerBit", true);
           this->onOff_32->setValue(true);

       }else{
           OutputDetailData.insert("isClearLowerBit", false);
           this->onOff_32->setValue(false);

       }

       QTimer::singleShot(500, this, SLOT(slot_clickedbit32question()));

       QJsonDocument doc(OutputDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_modeOnOff()-slot_modeOnOff_32 =: " << strJson;



}
void dialogsetting_output_250::slot_clickedbit32question(){//c220511
    print_debug();
    dlgConfirmOutput->setAlignment(Qt::AlignLeft);
    //dlgConfirmOutput->setTitle(tr("Headphone Output Impedance"));
    //dlgConfirmOutput->setText(tr("This is a menu to set the Headphone Output Impedace.\nIf you select the output (16 Ohm, 32 Ohm, 50 Ohm, 100 Ohm) that matches your headphone specifications, you can enjoy better sound."));
    dlgConfirmOutput->setAlignment(Qt::AlignLeft);
    dlgConfirmOutput->setTitle(tr("The low 8bits of a 32 bit sound sources\n will have correct"));
    dlgConfirmOutput->setText(tr("Some DACs may generate noise when playing a 32-bit sound source with a high sampling rate.\nIn this case, it is used to reduce noise by correcting the lower 8 bits."));

    dlgConfirmOutput->setGeometry((DLG_WIDTH + 80), (350 + 100), 350, 500);
    dlgConfirmOutput->setAlertMode();
    dlgConfirmOutput->setProperty("flagShown",false);

    if(dlgConfirmOutput->property("flagShown").toBool()==false){
        dlgConfirmOutput->setProperty("flagShown",true);

        int result = dlgConfirmOutput->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }
    }
}

void dialogsetting_output_250::slot_clickedRadioVolLev(){
    int notmalMinval = -70;
    int notmalMaxval = 0;
    int lowMinval = -80;
    int lowMaxval = -5;
    int highMinval = -60;
    int highMaxval = -5;
    int volumeMinLevel =-100, volumeMaxLevel=100;

    if(radio_vol_sel1->isChecked()){
        this->lb_volumnNumMin->setText(QString("MIN\n%1dB").arg(notmalMinval));
        this->lb_volumnNumMax->setText(QString("MAX\n%1dB").arg(notmalMaxval));
        OutputDetailData.insert( "volumeMinLevel", notmalMinval);
        OutputDetailData.insert( "volumeMaxLevel", notmalMaxval);
        slider_volMin->setValue(notmalMinval);
        slider_volMax->setValue(notmalMaxval);
        qDebug() << "volumeMinLevel : " << notmalMinval;
        qDebug() << "volumeMaxLevel : " << notmalMaxval;
        volumeMinLevel = notmalMinval;
        volumeMaxLevel = notmalMaxval;
    }
    else if(radio_vol_sel2->isChecked()){
        this->lb_volumnNumMin->setText(QString("MIN\n%1dB").arg(lowMinval));
        this->lb_volumnNumMax->setText(QString("MAX\n%1dB").arg(lowMaxval));
        OutputDetailData.insert( "volumeMinLevel", lowMinval);
        OutputDetailData.insert( "volumeMaxLevel", lowMaxval);
        slider_volMin->setValue(lowMinval);
        slider_volMax->setValue(lowMaxval);
        qDebug() << "volumeMinLevel : " << lowMinval;
        qDebug() << "volumeMaxLevel : " << lowMaxval;
        volumeMinLevel = lowMinval;
        volumeMaxLevel = lowMaxval;
    }else if(radio_vol_sel3->isChecked()){
        this->lb_volumnNumMin->setText(QString("MIN\n%1dB").arg(highMinval));
        this->lb_volumnNumMax->setText(QString("MAX\n%1dB").arg(highMaxval));
        OutputDetailData.insert( "volumeMinLevel", highMinval);
        OutputDetailData.insert( "volumeMaxLevel", highMaxval);
        slider_volMin->setValue(highMinval);
        slider_volMax->setValue(highMaxval);
        qDebug() << "volumeMinLevel : " << highMinval;
        qDebug() << "volumeMaxLevel : " << highMaxval;
        volumeMinLevel = highMinval;
        volumeMaxLevel = highMaxval;
    }else if(radio_vol_sel4->isChecked()){
        if(volumeMinLevel == -100 && volumeMaxLevel == 100){

        }else{

        }
        qDebug() << "volumeMinLevel : " << volumeMinLevel;
        qDebug() << "volumeMaxLevel : " << volumeMaxLevel;
    }
    else{
        qDebug() << "volumeMinLevel : " << volumeMinLevel;
        qDebug() << "volumeMaxLevel : " << volumeMaxLevel;
        ToastMsg::show(this, "", tr("Please select a USB DAC."));//ToastMsg::show(this, "", tr("클라우드 타입을 선택해주세요."));
    }
}
/*
void dialogsetting_output_250::set_selectedUsbName(QString usbstr){
    selectedUsbName = usbstr;
}
QString dialogsetting_output_250::get_selectedUsbName(){
    return selectedUsbName;
}
void dialogsetting_output_250::setTextName(QString usbstr){
    radio_UsbName->setText(usbstr);
}

void dialogsetting_output_250::setPropertyName(QString usbstr){
    radio_UsbName->setProperty(PROPERTY_NAME_READABLE, usbstr);
}
*/
/**
 * @brief SettingDisplay::setTextVolumn [SLOT]
 * @param p_value
 */
void dialogsetting_output_250::slot_setTextVolumnMin(int p_value){
    print_debug();
    int min_volLevel = -60;

    qDebug() << "lb_volumnNumMin = " << QString("Min:%1").arg(p_value) ;
    if(p_value > -60){
        this->slider_volMin->setValue(-60);
        min_volLevel = -60;
        qDebug() << "min_volLevel = " << min_volLevel;
    }else{
        min_volLevel = p_value;
    }
    this->lb_volumnNumMin->setText(QString("MIN\n%1dB").arg(min_volLevel));
    OutputDetailData.insert( "volumeMinLevel", min_volLevel);
    qDebug() << "volumeMinLevel : " << min_volLevel;
    //print_debug();
}

void dialogsetting_output_250::slot_setTextVolumnMax(int p_value){
    print_debug();
    float max_volLevel = -5;

    qDebug() << "lb_volumnNumMax = " << QString("Max:%1").arg(p_value) ;

    if(p_value > -50){
        int int_max_volLevel = (p_value/10);
        qDebug() << "int_max_volLevel = " << int_max_volLevel;
        float float_max_volLevel = (p_value/10.0);
        qDebug() << "float_max_volLevel = " << float_max_volLevel;
        float f = float_max_volLevel - int_max_volLevel ;
        qDebug() << "f = " << f;
        float f2;
        if(f>0){
            if( f > 0.5) {f2 = 0.5;}
            else f2 = 0.0;
        }else{
            if( f <= -0.5) {f2 = -0.5;}
            else f2 = 0.0;
        }

        qDebug() << "f2 = " << f2;
        max_volLevel = int_max_volLevel + f2 ;
        qDebug() << "max_volLevel = " << max_volLevel;
        this->lb_volumnNumMax->setText(QString("MAX\n%1dB").arg(max_volLevel));
    }else{
        this->slider_volMax->setValue(-50);
        this->lb_volumnNumMax->setText(QString("MAX\n%1dB").arg(-5));
    }

    OutputDetailData.insert( "volumeMaxLevel", (int)max_volLevel);
    qDebug() << "volumeMaxLevel : " << max_volLevel;

}
/**
 * @brief SettingDisplay::getUIControlOption 화면밝기 조절 컨트롤
 */
QWidget* dialogsetting_output_250::getVolumeUIControlOptionMin(){



    this->slider_volMin = new QSlider(Qt::Horizontal);
    this->slider_volMin->setObjectName("slider_vol_Min");
    this->slider_volMin->setTickInterval(1);
    this->slider_volMin->setRange(-80, -32);
   // this->slider_volMin->setMaximum(24);
    //this->slider_vol->setMaximumWidth(600);

    slider_volMin->setCursor(Qt::PointingHandCursor);

    //range slider   image: url(:/images/setting/201/vol_lev_btn.png);   image: url(:/images/setting/201/vol_lev_btn.png);
    slider_volMin->setStyleSheet("QSlider::groove:horizontal { background:#333333;height:11px;border-radius:2px;margin:0px 0px; } "
                                 "QSlider::handle:horizontal { background:white;width:14px;border-radius:7px;margin:-6px 0px; } "  /* handle(y,x) is placed by default on the contents rect of the groove. Expand outside the groove */
                                 "QSlider::add-page:horizontal { border: 2px solid black; background: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1, stop: 0 #ffff7f, stop: 1 #ffdf3c); "
                                 "background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1, stop: 0 #ffdf3c, stop: 1 #ff0000);} "
                                 "QSlider::sub-page:horizontal {border: 2px solid black; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e5e5e5, stop: 1 #383838);  } ");

    QLabel *lb_min = GSCommon::getUILabelImg(":/images/setting/201/vol_lev_db1.png");
    //QLabel *lb_max = GSCommon::getUILabelImg(":/images/setting/201/vol_lev_db2.png");
    lb_min->setStyleSheet("font-size:18px;color:#FFFFFF;");

    QVBoxLayout *vl_lb_min = new QVBoxLayout();
    vl_lb_min->setContentsMargins(0,0,0,0);
    vl_lb_min->setSpacing(0);
    vl_lb_min->addWidget(lb_min);


    QVBoxLayout *vl_slider = new QVBoxLayout();
    vl_slider->setContentsMargins(0,0,0,0);
    vl_slider->setSpacing(0);
    vl_slider->setAlignment(Qt::AlignVCenter);
    vl_slider->addWidget(slider_volMin);
    vl_slider->addLayout(vl_lb_min);

    QWidget *widget_sliderbox = new QWidget();
    widget_sliderbox->setFixedSize(330,70);
    widget_sliderbox->setLayout(vl_slider);

    QHBoxLayout *hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(0,0,0,0);
    hl_total->setSpacing(0);

    hl_total->addWidget(widget_sliderbox);

    QWidget *widget = new QWidget();

    widget->setLayout(hl_total);


    return widget;

}
QWidget* dialogsetting_output_250::getVolumeUIControlOptionMax(){



    this->slider_volMax = new QSlider(Qt::Horizontal);
    this->slider_volMax->setObjectName("slider_vol_Max");
    this->slider_volMax->setTickInterval(5);
    this->slider_volMax->setRange(-190, 50);
    //this->slider_vol->setMaximumWidth(600);

    slider_volMax->setCursor(Qt::PointingHandCursor);

    //range slider
    slider_volMax->setStyleSheet("QSlider::groove:horizontal { background:#333333;height:11px;border-radius:2px;margin:0px 0px; } "
                                 "QSlider::handle:horizontal { background:white;width:14px;border-radius:7px;margin:-6px 0px; } "  /* handle(y,x) is placed by default on the contents rect of the groove. Expand outside the groove */
                                "QSlider::sub-page:horizontal { border: 2px solid black; background: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1, stop: 0 #ffff7f, stop: 1 #ffdf3c); "
                                 "background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1, stop: 0 #ffdf3c, stop: 1 #ff0000);} "
                                 "QSlider::add-page:horizontal {border: 2px solid black; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e5e5e5, stop: 1 #383838);  } ");


    QLabel *lb_max = GSCommon::getUILabelImg(":/images/setting/201/vol_lev_db2.png");
  //  lb_max->setFixedSize(300,110);
  //  lb_max->setText("-5dB      5dB");
    lb_max->setStyleSheet("font-size:18px;color:#FFFFFF;");
    QHBoxLayout *hl_lb_max = new QHBoxLayout();
    hl_lb_max->setContentsMargins(0,0,0,0);
    hl_lb_max->setAlignment(Qt::AlignRight);
    hl_lb_max->addWidget(lb_max);

    QVBoxLayout *vl_slider = new QVBoxLayout();
    vl_slider->setContentsMargins(0,0,0,0);
    vl_slider->setSpacing(0);
    vl_slider->setAlignment(Qt::AlignVCenter);
    vl_slider->addWidget(slider_volMax);
    vl_slider->addLayout(hl_lb_max);

    QWidget *widget_sliderbox = new QWidget();
    widget_sliderbox->setFixedSize(330,70);
    widget_sliderbox->setLayout(vl_slider);

    QHBoxLayout *hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(0,0,0,0);

    hl_total->addWidget(widget_sliderbox);

    QWidget *widget = new QWidget();
    widget->setLayout(hl_total);

    return widget;

}

/*
QWidget* dialogsetting_output_250::tmp_getVolumeUIControlOption(){

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
*/

void dialogsetting_output_250::dlgOutputMqa(){


    dlgConfirmOutput->setTitle(tr("HDMI MQA Replay Gain Notice"));
    dlgConfirmOutput->setText(tr("Activates Replay Gaon Control when playing MQA. \nSet the desired leveling value.\nThese values apply only to MQA sound sources."));
    //dlgConfirmHdmi->setText(tr("MQA 재생시 Replay Gaon Control을 활성화합니다. \n원하는 평준화값을 설정하시오.\n해당 값은 MQA  음원에만 적용됩니다."));

}
void dialogsetting_output_250::dlgOutputCodecTrim(){


    dlgConfirmOutput->setTitle(tr("CodecTrim Setting Notice"));
    dlgConfirmOutput->setText(tr("This is used to improve clipping. \n\n If the sound level of a digital sound source is encoded higher than normal and produced, clipping (distortion) occurs when this sound source is played. \n\n This is a unique function of RS201 that maintains the output level without distortion by compensating by lowering the input level as much as the codec trim and raising the output level."));
    //dlgConfirmHdmi->setText(tr("클리핑 현상을 개선하기 위하여 사용합니다. \n\n 디지탈 음원의 사운드 레벨이 정상보다 높게 엔코딩 되어 제작된 경우, 이 음원을 재생하게되면 클리핑(음찌그러짐) 현상이 발생합니다. \n\n 코덱트림만큼 입력레벨은 낮추고, 출력레벨은 올려주어 보상(Compensation)함으로써 음 왜곡없이 출력레벨을 유지시켜주는 RS201 고유의 기능입니다."));

}

void dialogsetting_output_250::slot_clickedOutputcodecTrim(){
    dlgOutputCodecTrim();
    QTimer::singleShot(500, this, SLOT(slot_showDlgOfOutputNotice()));
}
void dialogsetting_output_250::slot_clickedOutputmqaGain(){
    dlgOutputMqa();
    QTimer::singleShot(500, this, SLOT(slot_showDlgOfOutputNotice()));
}

void dialogsetting_output_250::slot_clickedOutputaddmqaGain(){

    if( mqadBData < 0 )
        mqadBData += 1;
    lb_text[0]->setText(QString("%1").arg(mqadBData) + tr("           ") + tr("dB"));
    OutputDetailData.insert( "mqaRelplayGain", mqadBData);
    qDebug() << "mqadBData : " << mqadBData;

}
void dialogsetting_output_250::slot_clickedOutputsubmqaGain(){
    if( mqadBData > -20 )
        mqadBData -= 1;
    lb_text[0]->setText(QString("%1").arg(mqadBData) + tr("           ") + tr("dB"));
    OutputDetailData.insert( "mqaRelplayGain", mqadBData);
    qDebug() << "mqadBData : " << mqadBData;
}

void dialogsetting_output_250::slot_clickedOutputaddcodecTrim(){

    if( outputdBData < 0 )
        outputdBData += 1;
    lb_text[1]->setText(QString("%1").arg(outputdBData) + tr("           ") + tr("dB"));
    OutputDetailData.insert( "codecTrim", outputdBData);
    qDebug() << "outputdBData : " << outputdBData;

}
void dialogsetting_output_250::slot_clickedOutputsubcodecTrim(){
    if( outputdBData > -20 )
        outputdBData -= 1;
    lb_text[1]->setText(QString("%1").arg(outputdBData) + tr("           ") + tr("dB"));
    OutputDetailData.insert( "codecTrim", outputdBData);
    qDebug() << "outputdBData : " << outputdBData;
}
QWidget* dialogsetting_output_250::getPlusMinusUIControlOption(QString p_title, int i, int data){

    lb_gain[i] = new QPushButton(p_title);
    lb_gain[i]->setStyleSheet("color:#E5E5E4;");
    lb_gain[i]->setFixedSize(200,70);
    lb_gain[i]->setText(p_title);
    lb_gain[i]->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4;");//background-color:707070; ");
    lb_gain[i]->setCursor(Qt::PointingHandCursor);//print_debug();

   // QLabel *lb_mqagain = new QLabel(p_title);
   // lb_mqagain->setAlignment(Qt::AlignCenter);
   // lb_mqagain->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    btn_gain_add[i] = new RemoteBtnItem();
    btn_gain_add[i]->setProperty("btnNo", 0+2*i);
    btn_gain_add[i]->setData(":/images/vol_add_remote.png", "");
   // btn_vol_add->setProperty("httpId",HTTP_REMOTE_VOLUME_ADD);
    btn_gain_add[i]->setCircle(35);
    btn_gain_add[i]->setCursor(Qt::PointingHandCursor);
    btn_gain_sub[i] = new RemoteBtnItem();
    btn_gain_sub[i]->setProperty("btnNo", 0+(2*i+1));
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
    widget_text->setStyleSheet("#widget_title {background-color:#4d4d4d;}");//border-bottom:1px solid #404040;}"); //text_box_color -> background-color:#FFFFFF

    QPushButton *btn_question[2];
    btn_question[i] =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_question[i]->setCursor(Qt::PointingHandCursor);
    btn_question[i]->setFixedSize(QSize(30,30));


    QHBoxLayout *hl_mqagain = new QHBoxLayout();
    hl_mqagain->setContentsMargins(33,0,30,0);
    //hl_mqagain->setContentsMargins(0,35,0,0);
    hl_mqagain->setSpacing(0);
    hl_mqagain->addWidget(lb_gain[i], 0, Qt::AlignTop);
    hl_mqagain->setSpacing(0);
    hl_mqagain->addWidget(btn_question[i]);

   // hl_mqagain->setSpacing(10);
    hl_mqagain->addWidget(btn_gain_sub[i], 1, Qt::AlignRight);
   // hl_mqagain->setSpacing(10);
    hl_mqagain->addWidget(widget_text, 0, Qt::AlignCenter);
    hl_mqagain->setSpacing(10);
    hl_mqagain->addWidget(btn_gain_add[i]);//, 0, Qt::AlignRight);

    QWidget *widget_mqagain = new QWidget();
    widget_mqagain->setObjectName("widget_mqagain");
    widget_mqagain->setStyleSheet("#widget_mqagain {background-color:transparent;border-bottom:1px solid #707070;border-Top:1px solid #707070;} ");
    //widget_mqagain->setStyleSheet("#widget_mqagain { border-top:1px solid #303030;border-bottom:1px solid #303030;border-left:1px solid #303030; }");
  //  widget_mqagain->setFixedSize(630,70); //100x556
    widget_mqagain->setLayout(hl_mqagain);

    QVBoxLayout *vl_volumn = new QVBoxLayout();
    vl_volumn->setContentsMargins(0,0,0,0);
    vl_volumn->setSpacing(0);
    vl_volumn->addWidget(widget_mqagain);

    if(i == 0)
        connect(btn_question[0], SIGNAL(clicked()), this, SLOT(slot_clickedOutputcodecTrim()));
    if(i == 1)
        connect(btn_question[1], SIGNAL(clicked()), this, SLOT(slot_clickedOutputmqaGain()));

    return widget_mqagain;
}




/*
void dialogsetting_output_250::dlgOutputMqa(){


    dlgConfirmOutput->setTitle(tr("Output MQA Replay Gain Notice"));
    dlgConfirmOutput->setText(tr("Activates Replay Gaon Control when playing MQA. \nSet the desired leveling value.\nThese values apply only to MQA sound sources."));
    //dlgConfirmOutput->setText(tr("MQA 재생시 Replay Gaon Control을 활성화합니다. \n원하는 평준화값을 설정하시오.\n해당 값은 MQA  음원에만 적용됩니다."));

}
void dialogsetting_output_250::dlgOutputOutput(){


    dlgConfirmOutput->setTitle(tr("OUTPUT Output Gain Notice"));
    dlgConfirmOutput->setText(tr("Adjusts the Digital Output Gain. \nExternal receiver Or, if distortion occurs when connecting to a DAC, adjust the Output Gain value appropriately."));
    //dlgConfirmOutput->setText(tr("Digital Output의 Gain 값을 조정합니다. \n외부 리시버 또는 DAC과 연결 시 distortion이 발생하면 Output Gain 값을 적절히 조절하여 사용합니다."));

}
*/

void dialogsetting_output_250::slot_showDlgOfOutputNotice(){
    dlgConfirmOutput->setGeometry(DLG_WIDTH+480,350+100, 350,300);
    dlgConfirmOutput->setAlertMode();
    dlgConfirmOutput->setProperty("flagShown",false);
    if(dlgConfirmOutput->property("flagShown").toBool()==false){
        dlgConfirmOutput->setProperty("flagShown",true);

        int result = dlgConfirmOutput->exec();

        if(result == QDialog::Accepted){
            //showRemote(true);
            //qDebug() << " selectedOutputPass_str: " << this->onOff->getCurrentValue();;
           // OutputDetailData.insert( "isOutputPassThrough", this->onOff->getCurrentValue());
            //QJsonDocument doc(OutputDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_showDlgOfOutputPass()-Output_json =: " << strJson;

        }
    }
}


void dialogsetting_output_250::set_settingOfOutputDetail(QJsonObject tmpJson){
        OutputDetailData = tmpJson;
}
QJsonObject dialogsetting_output_250::get_settingOfOutputDetail(){
        return OutputDetailData;
}

void dialogsetting_output_250::slot_changedIndexOutput(int index){

    int btnSenderNo = sender()->property("btnNo").toInt();
    QString selectedOutput_str;
    //qDebug() << "----------------isOutputPassThrough :" <<  OutputDetailData["isOutputPassThrough"].toBool();
 //   QJsonObject Output_json = get_settingOfOutputDetail();
    QJsonDocument doc0(OutputDetailData);  QString strJson0(doc0.toJson(QJsonDocument::Compact));  qDebug() << "slot_changedIndexOutput()pre-Output_json =: " << strJson0;

    switch(btnSenderNo){
    case  0 :
        selectedOutput_str = comboBoxOutputPreoutLevel->itemText(index);
        qDebug() << " selectedOutput_str: " << selectedOutput_str;
        OutputDetailData.insert( "preoutLevelInfo", index-1);
    //    set_settingOfOutputDetail(OutputDetailData);

        break;
    case  1 :
        selectedOutput_str = comboBoxOutputPCMResampling->itemText(index);
        qDebug() << " selectedOutput_str: " << selectedOutput_str;
        OutputDetailData.insert( "samplingRate", index);
     //   set_settingOfOutputDetail(OutputDetailData);


        break;
    case  11 :
        selectedOutput_str = comboBoxOutputPCMMaxSampling->itemText(index);
        print_debug();
        qDebug() << " MaxSamplingRate_index: " << index;
        OutputDetailData.insert( "maxSamplingRate", index);
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
    case  3 :
        selectedOutput_str = comboBoxDSD->itemText(index);
        qDebug() << " selectedOutput_str: " << selectedOutput_str;
        OutputDetailData.insert( "dsdMode", index);
     //   set_settingOfOutputDetail(OutputDetailData);


        break;
    case  4 :
        selectedOutput_str = comboBoxHOI->itemText(index);
        qDebug() << " selectedOutput_str: " << selectedOutput_str;
        OutputDetailData.insert( "hpOhm", index);
     //   set_settingOfOutputDetail(OutputDetailData);


        break;
    case  5 :
        selectedOutput_str = comboBoxSofwareVol->itemText(index);
        qDebug() << " selectedOutput_str: " << selectedOutput_str;
        OutputDetailData.insert( "digitalVolume", index);
     //   set_settingOfOutputDetail(OutputDetailData);


        break;

    case  6 :
        selectedOutput_str = comboBoxPhaseInvert->itemText(index);
        qDebug() << " selectedOutput_str: " << selectedOutput_str;
        OutputDetailData.insert( "dacFilterType", index);
     //   set_settingOfOutputDetail(OutputDetailData);


        break;

    }
 //   Output_json = get_settingOfOutputDetail();
    QJsonDocument doc(OutputDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "Output_json =: " << strJson;



}



/**
 * @brief SettingTimer::getUIControlOutpution 콤보박스 포함된 ROW 생성한다
 * @param p_title
 * @return
 */



/**
 * @brief DialogSelect_Cloud::slot_clickedAdd : [슬롯] 추가 버튼 클릭
 */

const int HTTP_OUTPUT_GET_MODE  = 158;
const int HTTP_OUTPUT_SET_MODE  = 159;

void dialogsetting_output_250::getOutputSettingOfsetting(){

   NetworkHttp *network = new NetworkHttp;
   QJsonObject tmp_json;
   tmp_json.insert("outputMode", 1);
   connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
   network->request(HTTP_OUTPUT_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.setting.get"), tmp_json, true);

}

void dialogsetting_output_250::getOutputInOutSettingOfsetting(){

   NetworkHttp *network = new NetworkHttp;
   QJsonObject tmp_json;
   connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
   network->request(HTTP_OUTPUT_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("in.out.mode.get"), tmp_json, true);

}
void dialogsetting_output_250::slot_clickedOutputChangeSet(){
    print_debug();

   NetworkHttp *network = new NetworkHttp;
   QJsonObject tmp_json = get_settingOfOutputDetail();
   QJsonObject tmp_json_set;
   tmp_json_set.insert("data", tmp_json);
   tmp_json_set.insert("outputMode", 1);


   QJsonDocument doc(tmp_json_set);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"slot_clickedOutputChangeSet()-tmp_json_set :"<< strJson;//cheon09_io

   connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
   network->request(HTTP_OUTPUT_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.setting.set"), tmp_json_set, true);

}

void dialogsetting_output_250::setDialogOutput_show(){
   // QJsonObject *Output_json = get_settingOfOutputDetail();

   QJsonDocument doc(OutputDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"setDialogOutput_show()-OutputDetailData :"<< strJson;//cheon09_io
//comboBoxDSD
    comboBoxOutputPreoutLevel->setCurrentIndex( OutputDetailData["preoutLevelInfo"].toInt()+1 );
    comboBoxOutputPCMResampling->setCurrentIndex( OutputDetailData["samplingRate"].toInt() );
    comboBoxOutputPCMMaxSampling->setCurrentIndex( OutputDetailData["maxSamplingRate"].toInt() );

    comboBoxHOI->setCurrentIndex( OutputDetailData["hpOhm"].toInt() );
    comboBoxDSD->setCurrentIndex( OutputDetailData["dsdMode"].toInt() );
    comboBoxSofwareVol->setCurrentIndex( OutputDetailData["digitalVolume"].toInt() );

    comboBoxPhaseInvert->setCurrentIndex( OutputDetailData["phaseInvert"].toInt() );

    onOff_32->setValue(OutputDetailData["isClearLowerBit"].toBool());
    qDebug() << "DLG_WIDTH :" <<  DLG_WIDTH;
    qDebug() << "DLG_HEIGHT :" <<  DLG_HEIGHT;

    //ToastMsg::show(this, "", tr("The current OUTPUT setting information is displayed on the screen."));

}


void dialogsetting_output_250::setDialogOutput_save(QJsonObject p_jsonObject){
    QJsonObject tmp_Json;
    if(p_jsonObject.contains("data")){
        tmp_Json = p_jsonObject["data"].toObject(); //cheon09_io

    }
   // QJsonDocument doc(tmp_Json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"\npre-setDialogOutput_save()-tmp_Json :"<< strJson;//cheon09_io
     int codecTrim = tmp_Json["codecTrim"].toInt();
     int dacFilterType = tmp_Json["dacFilterType"].toInt();
     int drcInfo = tmp_Json["drcInfo"].toInt();
     int dsdMode = tmp_Json["dsdMode"].toInt();
     int hpOhm = tmp_Json["hpOhm"].toInt();
     bool isDigitalPassThrough = tmp_Json["isDigitalPassThrough"].toBool();
     bool isClearLowerBit = tmp_Json["isClearLowerBit"].toBool();
     OutputDetailData.insert( "isClearLowerBit", isClearLowerBit);
     bool isOptPassThrough = tmp_Json["isOptPassThrough"].toBool();
     int javs = tmp_Json["javs"].toInt();
     int mqaRelplayGain = tmp_Json["mqaRelplayGain"].toInt();
     int mqaType = tmp_Json["mqaType"].toInt();
   //  int opticalApwMode = tmp_Json["opticalApwMode"].toInt();
    // int opticalApwValue = tmp_Json["opticalApwValue"].toInt();
     int opticalLsb = tmp_Json["opticalLsb"].toInt();
     int opticalMsb = tmp_Json["opticalMsb"].toInt();
     int outputGain = tmp_Json["outputGain"].toInt();
     int phaseInvert = tmp_Json["phaseInvert"].toInt();
     int balanceValue = tmp_Json["balanceValue"].toInt();
     OutputDetailData.insert( "balanceValue", balanceValue);
     int preoutLevelInfo = tmp_Json["preoutLevelInfo"].toInt();
     int digitalVolume = tmp_Json["digitalVolume"].toInt();
     int samplingRate = tmp_Json["samplingRate"].toInt();
     int maxSamplingRate = tmp_Json["maxSamplingRate"].toInt();
     OutputDetailData.insert( "maxSamplingRate", maxSamplingRate);
     int volumeMaxLevel = tmp_Json["volumeMaxLevel"].toInt();
     int volumeMinLevel = tmp_Json["volumeMinLevel"].toInt();

     qDebug() << "isOptPassThrough : " << isOptPassThrough;

     OutputDetailData.insert( "codecTrim", codecTrim);
     OutputDetailData.insert( "dacFilterType", dacFilterType);

     OutputDetailData.insert( "drcInfo", drcInfo);
     OutputDetailData.insert( "dsdMode", dsdMode);
     OutputDetailData.insert( "hpOhm", hpOhm);
     OutputDetailData.insert( "isDigitalPassThrough", isDigitalPassThrough);
     OutputDetailData.insert( "isOptPassThrough", isOptPassThrough);
     OutputDetailData.insert( "javs", javs);
     OutputDetailData.insert( "mqaRelplayGain", mqaRelplayGain);
     OutputDetailData.insert( "mqaType", mqaType);
  //   OutputDetailData.insert( "opticalApwMode", opticalApwMode);
    // OutputDetailData.insert( "opticalApwValue", opticalApwValue);
     OutputDetailData.insert( "opticalLsb", opticalLsb);
     OutputDetailData.insert( "opticalMsb", opticalMsb);
     OutputDetailData.insert( "outputGain", outputGain);
     OutputDetailData.insert( "phaseInvert", phaseInvert);
     OutputDetailData.insert( "preoutLevelInfo", preoutLevelInfo);
     OutputDetailData.insert( "digitalVolume", digitalVolume);
     OutputDetailData.insert( "samplingRate", samplingRate);
     OutputDetailData.insert( "volumeMaxLevel", volumeMaxLevel);
     OutputDetailData.insert( "volumeMinLevel", volumeMinLevel);

     qDebug() << "json-isOptPassThrough : " << OutputDetailData["isOptPassThrough"].toBool();


     QJsonDocument doc1(OutputDetailData);  QString strJson1(doc1.toJson(QJsonDocument::Compact));  qDebug() <<"\nsetDialogOutput_save()-tmp_Json :"<< strJson1 << "\n";//cheon09_io

     //ToastMsg::show(this, "", tr("Save the information from the Rose deive."));
}

void dialogsetting_output_250::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    QString deviceType = global.device.getDeviceType();
  //  print_debug();//cheon0219

    print_debug();  QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nslot_responseHttp =: " << strJson;//cheon12_io
    //qDebug() << "p_id : " << p_id;
    QString internalMode;

    switch(p_id){
    case HTTP_OUTPUT_GET_MODE :

        if( deviceType == "RS150" || deviceType == "RS150B" || deviceType == "RS250" || deviceType == "RS250A"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                setDialogOutput_save(p_jsonObject);
                setDialogOutput_show();
            }
        }
        else if( deviceType == "RS201"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                setDialogOutput_save(p_jsonObject);
                setDialogOutput_show();
                int volumeMaxLevel = OutputDetailData["volumeMaxLevel"].toInt();
                int volumeMinLevel = OutputDetailData["volumeMinLevel"].toInt();
                this->lb_volumnNumMin->setText(QString("Min\n%1dB").arg(volumeMaxLevel));
                this->lb_volumnNumMax->setText(QString("MAX\n%1dB").arg(volumeMinLevel));
               // setDialogOutput_save(p_jsonObject);

            }
        }
        else if( deviceType == "RS301"){

        }
        else if( deviceType == "RS350"){

        }
        break;
    case HTTP_OUTPUT_SET_MODE :

        if( deviceType == "RS150" || deviceType == "RS150B" || deviceType == "RS250" || deviceType == "RS250A"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                getOutputInOutSettingOfsetting();

                ToastMsg::show(this, "", tr("Setting modification was successful."));
                 setResult(QDialog::Accepted);
                 this->hide();

            }else{
                setResult(QDialog::Rejected);
                this->hide();
            }
        }
        else if( deviceType == "RS201"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                 getOutputInOutSettingOfsetting();

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


QWidget* dialogsetting_output_250::getUIControlOption(QString p_title){

    lb_onOfftitle = new QPushButton(p_title);
    lb_onOfftitle->setProperty("btnNo", 4);
    lb_onOfftitle->setStyleSheet("color:#E5E5E4;");
    lb_onOfftitle->setFixedSize(110,70);
    lb_onOfftitle->setText(p_title);
    lb_onOfftitle->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");
    lb_onOfftitle->setCursor(Qt::PointingHandCursor);//print_debug();
    onOff = new OnOffWidget;print_debug()
    //onOff->setValue(false);
    onOff->setCursor(Qt::PointingHandCursor);//print_debug();

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
    hl_lb->addWidget(onOff, 0, Qt::AlignRight);
    print_debug();
    QPushButton *btn_onOffHover = new QPushButton(onOff);//print_debug();
    btn_onOffHover->setProperty("btnNo", 4);
    btn_onOffHover->setFixedSize(80,40);
    btn_onOffHover->setStyleSheet("background-color:transparent;border:0px;");

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");//print_debug();
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);
    print_debug();

    // 커넥션
    connect(btn_question, SIGNAL(clicked()), this, SLOT(slot_DRC_question_popup()));

    connect(lb_onOfftitle, SIGNAL(clicked()), this, SLOT(slot_modeOnOff()));
    connect(btn_onOffHover, SIGNAL(clicked()), this, SLOT(slot_modeOnOff()));
    return widget_total;
}


void dialogsetting_output_250::slot_DRC_question_popup(){

    dlgOutputDRC();
    QTimer::singleShot(500, this, SLOT(slot_showDlgOfOutputNotice()));

}


void dialogsetting_output_250::slot_CodecTrim_question_popup(){

    dlgOutputCodecTrim();
    QTimer::singleShot(500, this, SLOT(slot_showDlgOfOutputNotice()));

}


/**
 * @brief SettingVU::slot_modeOnOff [SLOT] VU 모드 ON/OFF 변경 API 호출
 */
//void SettingVideo::slot_modeOnOff(bool flagOn){
void dialogsetting_output_250::slot_modeOnOff(){

   // DialogConfirm *dlg = new DialogConfirm(this);
   // dlg->setTitle(tr("HDMI Passthrough"));

    //dlg->setText(tr("Do you want to change it?"));

   // int result = dlg->exec();
   // if(result==QDialog::Accepted){
    //}

       int ispass = !this->onOff->getCurrentValue();
       if(ispass) {
           OutputDetailData.insert("drcInfo", 1);

       }else{
           OutputDetailData.insert("drcInfo", 0);

       }
       this->onOff->setValue(ispass);
       //QMessageBox::information(this,"Title","dddddddddddd");
       OutputDetailData.insert( "drcInfo", ispass);


      // notice_msg = tr("It bypasses HD audio formats such as surround sound sources to the audio receiver through HDMI without decoding.");
      // lb_notice->setText(tr("- Notice - ") + notice_msg);
       QJsonDocument doc(OutputDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_modeOnOff()-Hdmi_json =: " << strJson;



}


void dialogsetting_output_250::dlgOutputDRC(){

    dlgConfirmOutput->setTitle(tr("Dynamic Range Control Notice"));
    //dlgConfirmHdmi->setText(tr("음원의 음압레벨이 높고 음량을 과하게 조절할 경우 출력신호에 Clipping 현상이 발생할 수 있습니다. \n \n DRC 모드를 사용하면 출력 크기를 자동으로 조절하여 Clipping 현상을 방지하여 편안한 사운드를 출력합니다."));
    dlgConfirmOutput->setText(tr("If the sound pressure level of the sound source is high and the volume is excessively adjusted, clipping may occur in the output signal. \n\n When DRC mode is used, the output size is automatically adjusted to prevent clipping, resulting in a comfortable sound."));

}


void dialogsetting_output_250::set_selected_deviceName(QString usbstr){

    selected_deviceName = usbstr;
}


QString dialogsetting_output_250::get_selected_deviceName(){

    return selected_deviceName;
}


void dialogsetting_output_250::setTextName(QString usbstr){

    radio_UsbName->setText(usbstr);
}


void dialogsetting_output_250::setPropertyName(QString usbstr){

    radio_UsbName->setProperty(PROPERTY_NAME_READABLE, usbstr);
}