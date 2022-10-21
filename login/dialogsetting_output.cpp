#include "dialogsetting_output.h"
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
dialogsetting_output::dialogsetting_output(QWidget *parent)
    : QDialog(parent)
{
    setInit();
    setUIControl();
    getOutputSettingOfsetting();
}

/**
 * @brief DialogSelect_Cloud::setInit: 초기 세팅
 */
void dialogsetting_output::setInit(){
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT+380);//200
    this->setStyleSheet("background-color:transparent;");
   // OutputDetailData = new QJsonObject();
}



/**
 * @brief DialogSelect_Cloud::setUIControl : UI 세팅
 */
void dialogsetting_output::setUIControl(){

    QJsonObject tmp_outputsettingval = get_settingOfOutputDetail();
    dlgConfirmOutput = new DialogConfirm(this);
    // 타이틀
    QLabel *lb_title = new QLabel;
    lb_title->setText(tr("Analog Output Setting"));//Analog Out Setting//lb_title->setText(tr("클라우드 추가"));
    //lb_title->setStyleSheet("font:bold;color:#FFFFFF;font-size:22px;");
    lb_title->setStyleSheet("color:#FFFFFF;font-size:22px;");
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

    QPushButton *btn_questionPreLevel;//c220511
    btn_questionPreLevel =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionPreLevel->setCursor(Qt::PointingHandCursor);
    btn_questionPreLevel->setFixedSize(QSize(30,30));
    QPushButton *btn_questionPCM;//c220511
    btn_questionPCM =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionPCM->setCursor(Qt::PointingHandCursor);
    btn_questionPCM->setFixedSize(QSize(30,30));
    QPushButton *btn_questionMQA;//c220511
    btn_questionMQA =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionMQA->setCursor(Qt::PointingHandCursor);
    btn_questionMQA->setFixedSize(QSize(30,30));
    QPushButton *btn_questionDSD;//c220511
    btn_questionDSD =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionDSD->setCursor(Qt::PointingHandCursor);
    btn_questionDSD->setFixedSize(QSize(30,30));
    QPushButton *btn_questionRoon;//c220511
    btn_questionRoon =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionRoon->setCursor(Qt::PointingHandCursor);
    btn_questionRoon->setFixedSize(QSize(30,30));
    QPushButton *btn_questionBalance;//c220511
    btn_questionBalance =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_questionBalance->setCursor(Qt::PointingHandCursor);
    btn_questionBalance->setFixedSize(QSize(30,30));


    //border-bottom:1px solid #707070;
    QVBoxLayout *vl_total_drc = new QVBoxLayout();
    vl_total_drc->setContentsMargins(0,0,0,0);
    vl_total_drc->setSpacing(0);
    vl_total_drc->setAlignment(Qt::AlignTop);
    vl_total_drc->addWidget(this->getUIControlOption_DRC(tr("DRC Mode")));

    QVBoxLayout *vl_total_bit32 = new QVBoxLayout();
    vl_total_bit32->setContentsMargins(0,0,0,0);
    vl_total_bit32->setSpacing(0);
    vl_total_bit32->setAlignment(Qt::AlignTop);
    //vl_total_Pass->addWidget(GSCommon::getUIControlTitleForSetting(tr("Video Option")));
    vl_total_bit32->addWidget(this->getUIControlOption_32(tr("The low 8bits of a 32 bit sound sources will have correct")));//print_debug();
    //onOff[0]->setValue(tmp_hdmisettingval["isHdmiPassThrough"].toBool());

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
    comboBoxOutputPreoutLevel->addItem(tr(" 3000 mV"), 8);


    QLabel *lb_PreLevel = new QLabel(tr("output MSB"));
    lb_PreLevel->setStyleSheet("color:#E5E5E4");
    lb_PreLevel->setFixedHeight(55);//c220511
    lb_PreLevel->setText(tr("Preout Level Setting"));
    lb_PreLevel->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    comboBoxOutputPreoutLevel->setContentsMargins(10,0,0,0);
    comboBoxOutputPreoutLevel->setFixedHeight(35);
    comboBoxOutputPreoutLevel->setObjectName("comboBox_outputMsb");//cheon01
    comboBoxOutputPreoutLevel->setFixedWidth(300);


    comboBoxOutputPreoutLevel->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658;}"// border: 1px solid #B18658;   } "
                                );

    QHBoxLayout *hl_lb_PreLevel = new QHBoxLayout();
    hl_lb_PreLevel->setContentsMargins(30,0,20,0);
    hl_lb_PreLevel->setSpacing(0);
    hl_lb_PreLevel->addWidget(lb_PreLevel);
    hl_lb_PreLevel->addWidget(btn_questionPreLevel, 0, Qt::AlignLeft);//c220511
    hl_lb_PreLevel->addSpacing(10);
    hl_lb_PreLevel->addWidget(comboBoxOutputPreoutLevel, 0, Qt::AlignRight);
   // hl_lb_MQA->setSpacing(30);

    QWidget *widget_total_PreLevel = new QWidget();
    widget_total_PreLevel->setObjectName("widget_total");
    widget_total_PreLevel->setStyleSheet("#widget_total { border-top:1px solid #707070;border-bottom:1px solid #707070; } ");
    widget_total_PreLevel->setLayout(hl_lb_PreLevel);



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


    QLabel *lb_PCM = new QLabel(tr("lb_PCM"));
    lb_PCM->setStyleSheet("color:#E5E5E4");
    lb_PCM->setFixedHeight(100);//c220511
    lb_PCM->setText((tr("PCM Resampling Frequency")));
    lb_PCM->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    comboBoxOutputPCMMaxSampling = new QComboBox();
    comboBoxOutputPCMMaxSampling->setProperty("btnNo", 11);//
    comboBoxOutputPCMResampling->setContentsMargins(10,0,0,0);
    comboBoxOutputPCMResampling->setFixedHeight(35);
    comboBoxOutputPCMResampling->setObjectName("comboBox_outputLsb");//cheon01
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
    comboBoxOutputPCMMaxSampling->setObjectName("comboBox_outputLsb");//cheon01
    comboBoxOutputPCMMaxSampling->setFixedWidth(150);

    comboBoxOutputPCMMaxSampling->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                                );

    QLabel *lb_title_MaxSample = new QLabel;
    lb_title_MaxSample->setText(tr("Max Sample Rate"));//lb_title->setText(tr("클라우드 추가"));
    //lb_title_MaxSample->setStyleSheet("font:bold;color:#AAAAAA;font-size:15px;");
    lb_title_MaxSample->setStyleSheet("color:#AAAAAA;font-size:15px;");
    lb_title_MaxSample->setAlignment(Qt::AlignCenter);
    lb_title_MaxSample->setFixedHeight(20);

    QLabel *lb_title_Resample = new QLabel;
    lb_title_Resample->setText(tr("Resample Rate"));//lb_title->setText(tr("클라우드 추가"));
   //lb_title_Resample->setStyleSheet("font:bold;color:#AAAAAA;font-size:15px;");
    lb_title_Resample->setStyleSheet("color:#AAAAAA;font-size:15px;");
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

    //-----------------------------------------vol level setting -----------------------------------------------------


    QLabel *lb_Vol_name= new QLabel(tr("lb_Vol"));
    lb_Vol_name->setStyleSheet("color:#E5E5E4");
    lb_Vol_name->setFixedSize(270,40);
    lb_Vol_name->setText((tr("Volume Level Setting")));//label name
    lb_Vol_name->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    QHBoxLayout *vl_vol_lavel = new QHBoxLayout();
    vl_vol_lavel->setContentsMargins(30,0,0,0);
    vl_vol_lavel->setSpacing(0);
    vl_vol_lavel->setAlignment(Qt::AlignTop);
    vl_vol_lavel->addWidget(lb_Vol_name);

   //--------------------------------------------------------------------------
    // QLabel *lb_img = GSCommon::getUILabelImg(":/images/setting/201/vol_lev_box.png");
     //lb_img->setStyleSheet("text-align:center;background-color:transparent;");
    // lb_img->setText("0dB");
     lb_volumnNumMin = new QLabel();
     lb_volumnNumMin->setStyleSheet("text-align:center;font-size:16px;color:#cccccc;font:bold; background-image: url(:/images/setting/201/vol_lev_box.png);background-repeat:no-repeat;");
     lb_volumnNumMin->setContentsMargins(10,0,0,0);
    //lb_volumnNumMin->setStyleSheet("text-align:center;");
  //   QLabel *lb_img1 = GSCommon::getUILabelImg(":/images/setting/201/vol_lev_box.png");
     lb_volumnNumMax = new QLabel();
     lb_volumnNumMax->setStyleSheet("text-align:right;font-size:16px;color:#cccccc;font:bold; background-image: url(:/images/setting/201/vol_lev_box.png);background-repeat:no-repeat;");
     lb_volumnNumMax->setContentsMargins(10,0,0,0);
   // lb_volumnNumMax->setStyleSheet("text-align:center;");
  //   lb_img->setFixedHeight(120);
   //  lb_img1->setFixedHeight(120);
     lb_volumnNumMin->setFixedSize(80,60);
     lb_volumnNumMax->setFixedSize(80,60);


     QHBoxLayout *vl_LabelvolLevelMinMax = new QHBoxLayout(this);
     vl_LabelvolLevelMinMax->setContentsMargins(100,0,0,0);
     vl_LabelvolLevelMinMax->setSpacing(0);

   //  vl_LabelvolLevelMinMax->addWidget(lb_img, 0, 0, Qt::AlignBottom | Qt::AlignCenter);
    // vl_LabelvolLevelMinMax->addWidget(lb_img, 0, Qt::AlignVCenter | Qt::AlignHCenter);
     vl_LabelvolLevelMinMax->addWidget(lb_volumnNumMin, 0, Qt::AlignVCenter | Qt::AlignHCenter);

    // vl_LabelvolLevelMinMax->addWidget(lb_img1, 0, 1, Qt::AlignBottom | Qt::AlignCenter);
     vl_LabelvolLevelMinMax->addWidget(lb_volumnNumMax, 0, Qt::AlignVCenter | Qt::AlignHCenter);

     widget_minmaxbox = new QWidget();
     widget_minmaxbox->setStyleSheet(" background-image: url(:/images/setting/201/vol_lev_bg_on.png);right: 2px; background-repeat:no-repeat; background-position:right center; ");
     //widget_minmaxbox->setStyleSheet(" background-image: url(tt);right: 2px; background-repeat:no-repeat; background-position:right center; ");
     widget_minmaxbox->setLayout(vl_LabelvolLevelMinMax);
    // widget_minmaxbox->setAlignment(Qt::AlignCenter);

     widget_minmaxbox->setFixedSize(DLG_WIDTH-80,110);

//widget_minmaxbox->

//----------------------------------------------------------------------------------------------------------------------
    QLabel *lb_minmax_text = new QLabel();
    lb_minmax_text->setText("    -80dB                             -60dB                                  -5dB              0dB           5dB");
    lb_minmax_text->setStyleSheet("font-size:18px;color:#cccccc;");

    QVBoxLayout *vl_lb_min_Rangetext = new QVBoxLayout();
    vl_lb_min_Rangetext->setContentsMargins(50,0,0,20);
    vl_lb_min_Rangetext->setSpacing(0);
    vl_lb_min_Rangetext->addWidget(lb_minmax_text);


    QWidget *wminmax_text = new QWidget();
    wminmax_text->setStyleSheet("#wmin_text {background-color:transparent; } ");
    wminmax_text->setFixedHeight(40);
    wminmax_text->setLayout(vl_lb_min_Rangetext);
//-------------------------------------------------------------------------------------------------------------------------------
    QLabel *lb_graph = GSCommon::getUILabelImg(":/images/setting/201/vol_lev_bg_on.png");

    lb_graph->resize(200, 800);

    lb_graph->setStyleSheet(" font-size:18px;color:#FFFFFF;");
    QVBoxLayout *vl_graph = new QVBoxLayout();
    vl_graph->setContentsMargins(0,0,0,0);

    vl_graph->addWidget(lb_graph);

    QWidget *widget_graphbox = new QWidget();
    widget_graphbox->setLayout(vl_graph);
//--------------------------------------------------- slider --------------------------------------------------

    QHBoxLayout *hl_volLevel_slider = new QHBoxLayout();
    hl_volLevel_slider->setContentsMargins(80,0,80,0);
    hl_volLevel_slider->setSpacing(0);
    hl_volLevel_slider->addWidget(this->getVolumeUIControlOptionMin());
    hl_volLevel_slider->addWidget(this->getVolumeUIControlOptionMax());

    QWidget *widget_minmax_slider = new QWidget();
    widget_minmax_slider->setObjectName("widget_total_vol");
    widget_minmax_slider->setFixedHeight(50);
    widget_minmax_slider->setLayout(hl_volLevel_slider);

    QLabel *lb_Balance_vol= new QLabel(tr("lb_Vol"));
    lb_Balance_vol->setStyleSheet("color:#E5E5E4");
    lb_Balance_vol->setFixedHeight(55);
    lb_Balance_vol->setText((tr("Balance setting")));//label name
    lb_Balance_vol->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    QHBoxLayout *hl_volBlance_slider = new QHBoxLayout();
    hl_volBlance_slider->setContentsMargins(33,0,30,0);
    hl_volBlance_slider->setSpacing(20);
    hl_volBlance_slider->addWidget(lb_Balance_vol);
    hl_volBlance_slider->addWidget(btn_questionBalance, 0, Qt::AlignLeft);//c220511
    hl_volBlance_slider->addSpacing(10);
    hl_volBlance_slider->addWidget(this->getBalanceUIControlOption(), 0, Qt::AlignRight);//c220511

    QWidget *widget_balance_slider = new QWidget();
    widget_balance_slider->setObjectName("widget_total");
    widget_balance_slider->setStyleSheet("#widget_total { border-top:1px solid #707070;border-bottom:1px solid #707070; } ");
    widget_balance_slider->setLayout(hl_volBlance_slider);


    QString radio_style = "QRadioButton {font-size:20px;color:#CCCCCC;spacing:15px;padding:15px;border-bottom:1px solid #404040;}";
    radio_style += " QRadioButton::indicator {width:40px;height:40px;}";
    radio_style += " QRadioButton::indicator::unchecked {image: url(:images/icon_che_off.png);}";
    radio_style += " QRadioButton::indicator::checked {image: url(:images/icon_che_on.png);}";

    radio_vol_sel1 = new QRadioButton;
    radio_vol_sel1->setText("Normal");
    radio_vol_sel1->setStyleSheet(radio_style);
    radio_vol_sel1->setCursor(Qt::PointingHandCursor);
    radio_vol_sel1->setProperty(PROPERTY_NAME_READABLE, "Normal");
    //radio_vol_sel1->setFixedHeight(30);

    radio_vol_sel2 = new QRadioButton;
    radio_vol_sel2->setText("Low");
    radio_vol_sel2->setStyleSheet(radio_style);
    radio_vol_sel2->setCursor(Qt::PointingHandCursor);
    radio_vol_sel2->setProperty(PROPERTY_NAME_READABLE, "Low");
    //radio_vol_sel2->setFixedHeight(30);

    radio_vol_sel3 = new QRadioButton;
    radio_vol_sel3->setText("High");
    radio_vol_sel3->setStyleSheet(radio_style);
    radio_vol_sel3->setCursor(Qt::PointingHandCursor);
    radio_vol_sel3->setProperty(PROPERTY_NAME_READABLE, "High");
    //radio_vol_sel3->setFixedHeight(30);

    radio_vol_sel4 = new QRadioButton;
    radio_vol_sel4->setText("User");
    radio_vol_sel4->setStyleSheet(radio_style);
    radio_vol_sel4->setCursor(Qt::PointingHandCursor);
    radio_vol_sel4->setProperty(PROPERTY_NAME_READABLE, "User");
    //radio_vol_sel4->setFixedHeight(30);

    QHBoxLayout *hl_box_radio_vol = new QHBoxLayout;
    hl_box_radio_vol->setSpacing(0);
    hl_box_radio_vol->setContentsMargins(30,0,0,0);
    hl_box_radio_vol->addWidget(radio_vol_sel1);
    hl_box_radio_vol->addWidget(radio_vol_sel2);
    hl_box_radio_vol->addWidget(radio_vol_sel3);
    hl_box_radio_vol->addWidget(radio_vol_sel4);

    QWidget *wbox_radio_vol = new QWidget;
    wbox_radio_vol->setObjectName("wbox_radio_vol");
    wbox_radio_vol->setLayout(hl_box_radio_vol);
    wbox_radio_vol->setFixedHeight(50);
    wbox_radio_vol->setStyleSheet("#wbox_radio_vol {background-color:transparent;border-bottom:1px solid #404040;}");

    QVBoxLayout *vl_total_vol_metter = new QVBoxLayout();
    vl_total_vol_metter->setContentsMargins(0,0,0,0);
    vl_total_vol_metter->setSpacing(0);
    vl_total_vol_metter->setAlignment(Qt::AlignTop);

    vl_total_vol_metter->addWidget(widget_minmaxbox);


    vl_total_vol_metter->addWidget(widget_minmax_slider);

    vl_total_vol_metter->addWidget(wminmax_text);
    vl_total_vol_metter->addWidget(wbox_radio_vol);

    QWidget *widget_vol_totalbox= new QWidget();
    widget_vol_totalbox->setContentsMargins(0,0,0,0);
   // widget_vol_totalbox->setFixedHeight(350);

   // widget_vol_totalbox->setStyleSheet("border:1px solid #707070; ");
    widget_vol_totalbox->setLayout(vl_total_vol_metter);
//--------------------------------------------------------------------------------------------------------
    comboBoxHdmiDSD = new QComboBox();
    comboBoxHdmiDSD->setProperty("btnNo", 2);

    comboBoxHdmiDSD->addItem(tr(" Native DSD(upto DSD512)"), 0);//cheon01_src
    comboBoxHdmiDSD->addItem(tr(" DSD over PCM(Not supported)"), 1);//cheon01_src
    comboBoxHdmiDSD->addItem(tr(" DSD to PCM(upto DSD128)"), 2);//cheon01_src


    QLabel *lb_DSD = new QLabel(tr("DSD Mode"));
    lb_DSD->setStyleSheet("color:#E5E5E4");
    lb_DSD->setFixedHeight(55);//c220511
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
                                );

    QHBoxLayout *hl_lb_DSD = new QHBoxLayout();
    hl_lb_DSD->setContentsMargins(33,0,30,0);
    hl_lb_DSD->setSpacing(0);
    hl_lb_DSD->addWidget(lb_DSD);
    hl_lb_DSD->addWidget(btn_questionDSD, 0, Qt::AlignLeft);//c220511
    hl_lb_DSD->addSpacing(10);
    hl_lb_DSD->addWidget(comboBoxHdmiDSD);

    QWidget *widget_total_DSD = new QWidget();
    widget_total_DSD->setObjectName("widget_total");
    widget_total_DSD->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total_DSD->setLayout(hl_lb_DSD);



    QVBoxLayout *vl_total_codecTrim = new QVBoxLayout();
    vl_total_codecTrim->setContentsMargins(0,0,0,0);
    vl_total_codecTrim->setSpacing(0);
    vl_total_codecTrim->setAlignment(Qt::AlignTop);

    outputdBData = tmp_outputsettingval["codecTrim"].toInt();
    QWidget *widget_codecTrim = this->getPlusMinusUIControlOption(tr("CodecTrim Setting"), 0,outputdBData);
    vl_total_codecTrim->addWidget(widget_codecTrim);
   // print_debug();
    QVBoxLayout *vl_total_mqagain = new QVBoxLayout();
    vl_total_mqagain->setContentsMargins(0,0,0,0);
    vl_total_mqagain->setSpacing(0);
    vl_total_mqagain->setAlignment(Qt::AlignTop);
   // print_debug();
    mqadBData = tmp_outputsettingval["mqaRelplayGain"].toInt();
    QWidget *widget_mqaGain = this->getPlusMinusUIControlOption(tr("MQA Replay Gain"),1, mqadBData);
    vl_total_mqagain->addWidget(widget_mqaGain);


    QVBoxLayout *vl_total1 = new QVBoxLayout();
    vl_total1->setContentsMargins(0,0,0,0);
    vl_total1->setSpacing(0);
    vl_total1->setAlignment(Qt::AlignTop);
    vl_total1->addWidget(widget_total_PreLevel);//cheon01

    vl_total1->addLayout(vl_vol_lavel);

    vl_total1->addWidget(widget_vol_totalbox);

  //  vl_total1->addWidget(wbox_radio_vol);

    vl_total1->addLayout(vl_total_codecTrim);

  //  vl_total1->addLayout(vl_total_mqagain);

    vl_total1->addWidget(widget_total_PCM);//cheon01
    vl_total1->addWidget(widget_total_DSD);//cheon01//
    vl_total1->addWidget(widget_balance_slider);//cheon01//


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
    vl_total2->addLayout(vl_total_drc);
    vl_total2->addLayout(vl_total_bit32);
    vl_total2->addLayout(vl_total1);
    vl_total2->addSpacing(20);//200

//    vl_total2->addLayout(layout);//example

    vl_total2->addLayout(hl_btn);


    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setLayout(vl_total2);//print_debug();
    //widget_total->setFixedHeight(900);
    widget_total->setStyleSheet("#widget_total { background-color:#333333;border-radius:4px;border:1px solid #4D4D4D; }");

    QVBoxLayout *vboxlayout = new QVBoxLayout();
    vboxlayout->setContentsMargins(0,0,0,0);
    vboxlayout->setSpacing(0);
    vboxlayout->addWidget(widget_total);//print_debug();

    this->setLayout(vboxlayout);//print_debug();

    radioVolSel = 1;


    connect(btn_questionDSD, SIGNAL(clicked()), this, SLOT(slot_clickedDSDquestion()));//c220511
    connect(btn_questionPCM, SIGNAL(clicked()), this, SLOT(slot_clickedPCMquestion()));//c220511
    connect(btn_questionRoon, SIGNAL(clicked()), this, SLOT(slot_clickedRoonquestion()));//c220511
    connect(btn_questionMQA, SIGNAL(clicked()), this, SLOT(slot_clickedHOIquestion()));//c220511
    connect(btn_questionPreLevel, SIGNAL(clicked()), this, SLOT(slot_clickedPreLevelquestion()));//c220511
    connect(btn_questionBalance, SIGNAL(clicked()), this, SLOT(slot_clickedBalancequestion()));//c220511

    // 커넥션
    //this->slider_volMin->setValue(-80);
    //this->slider_volMax->setValue(-50);

   // slider_volMin->setTickPosition(QSlider::NoTicks);
   // slider_volMax->setTickPosition(QSlider::NoTicks);
    connect(slider_volMin, SIGNAL(valueChanged(int)) ,this, SLOT(slot_setTextVolumnMin(int)));
    connect(slider_volMax, SIGNAL(valueChanged(int)) ,this, SLOT(slot_setTextVolumnMax(int)));

    connect(comboBoxOutputPreoutLevel, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOutput(int)));
    connect(comboBoxOutputPCMMaxSampling, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOutput(int)));
    connect(comboBoxOutputPCMResampling, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOutput(int)));
    connect(comboBoxHdmiDSD, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOutput(int)));
    connect(btn_gain_add[1], SIGNAL(clicked()), this, SLOT(slot_clickedOutputaddmqaGain()));
    connect(btn_gain_sub[1], SIGNAL(clicked()), this, SLOT(slot_clickedOutputsubmqaGain()));
    connect(btn_gain_add[0], SIGNAL(clicked()), this, SLOT(slot_clickedOutputaddcodecTrim()));
    connect(btn_gain_sub[0], SIGNAL(clicked()), this, SLOT(slot_clickedOutputsubcodecTrim()));
//if( radioVolSel == 1 || radioVolSel == 2 || radioVolSel == 3 ){
    connect(radio_vol_sel1, SIGNAL(clicked()), this, SLOT(slot_clickedRadioVolLev()));
    connect(radio_vol_sel2, SIGNAL(clicked()), this, SLOT(slot_clickedRadioVolLev()));
    connect(radio_vol_sel3, SIGNAL(clicked()), this, SLOT(slot_clickedRadioVolLev()));
//}
//if( radioVolSel == 4 ){
    connect(radio_vol_sel4, SIGNAL(clicked()), this, SLOT(slot_clickedRadioVolLev()));
//}
    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(close()));//print_debug();
    connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clickedOutputChangeSet()));

}

void dialogsetting_output::slot_clickedDSDquestion(){//c220511
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

void dialogsetting_output::slot_clickedPCMquestion(){//c220511
    print_debug();
    dlgConfirmOutput->setAlignment(Qt::AlignLeft);
    dlgConfirmOutput->setTitle(tr("PCM Resampling Frequency"));
    dlgConfirmOutput->setText(tr("This is The menu to set sampling frequecy. if you want to output the original sound as it is, select the Original Sampling Rate, and if want to use the resampling function, select the desired sampling rate (48kHz, 96Khz, 192Khz, etc), MQA, Native DSD, DOP, Video, Bluetooth and Airplay, Roon Ready are not supported."));

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

void dialogsetting_output::slot_clickedPreLevelquestion(){//c220511
    print_debug();
    dlgConfirmOutput->setAlignment(Qt::AlignLeft);
    dlgConfirmOutput->setTitle(tr("PCM Resampling Frequency"));
    dlgConfirmOutput->setText(tr("Fixes the pre-out output level. You can use the output level of the ROSE by holding the output level of the ROSE as an input within the acceptable range of the AMP, you connected without using the volume control of the ROSE."));

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

void dialogsetting_output::slot_clickedBalancequestion(){//c220511
    print_debug();
    dlgConfirmOutput->setAlignment(Qt::AlignLeft);
    dlgConfirmOutput->setTitle(tr("Balance setting"));
    dlgConfirmOutput->setText(tr("This is to set the output with a relative deviation in the volume between the Left and Right channels. (1~25 range). if you want to make the L side(R side) output larger than R side(L side). please set the L side(T side) value(1~25 range)."));

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


void dialogsetting_output::slot_clickedHOIquestion(){//c220511
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


void dialogsetting_output::slot_clickedSoftwareVolquestion(){//c220511
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


void dialogsetting_output::slot_clickedRadioVolLev(){
    int notmalMinval = -70;
    int notmalMaxval = 0;
    int lowMinval = -80;
    int lowMaxval = -50;
    int highMinval = -60;
    int highMaxval = 50;
    int volumeMinLevel =-100, volumeMaxLevel=100;

    qDebug() << "slot_clickedRadioVolLev()-radioVolSel : " << radioVolSel;


    if(radio_vol_sel1->isChecked()){


        this->lb_volumnNumMin->setText(QString("MIN\n%1dB").arg(notmalMinval));
        this->lb_volumnNumMax->setText(QString("MAX\n%1dB").arg(notmalMaxval));

        OutputDetailData.insert( "volumeMinLevel", 10);
        OutputDetailData.insert( "volumeMaxLevel", 10);
        slider_volMin->setValue(notmalMinval);
        slider_volMax->setValue(notmalMaxval);
        qDebug() << "volumeMinLevel : " << notmalMinval;
        qDebug() << "volumeMaxLevel : " << notmalMaxval;
        volumeMinLevel = notmalMinval;
        volumeMaxLevel = notmalMaxval;
        radioVolSel = 1;
         radio_vol_sel1->setChecked(1) ;
         radio_vol_sel2->setChecked(0) ;
         radio_vol_sel3->setChecked(0) ;
         radio_vol_sel4->setChecked(0) ;
    }
    else if(radio_vol_sel2->isChecked()){


        qDebug() << "this->width() : " << this->width();
        qDebug() << "this->height() : " << this->height();



        this->lb_volumnNumMin->setText(QString("MIN\n%1dB").arg(lowMinval));
        this->lb_volumnNumMax->setText(QString("MAX\n%1dB").arg(lowMaxval));
        OutputDetailData.insert( "volumeMinLevel", 0);
        OutputDetailData.insert( "volumeMaxLevel", 0);
        slider_volMin->setValue(lowMinval);
        slider_volMax->setValue(lowMaxval);
        qDebug() << "volumeMinLevel : " << lowMinval;
        qDebug() << "volumeMaxLevel : " << lowMaxval;
        volumeMinLevel = lowMinval;
        volumeMaxLevel = lowMaxval;
        radioVolSel = 2;
        radio_vol_sel1->setChecked(0) ;
        radio_vol_sel2->setChecked(1) ;
        radio_vol_sel3->setChecked(0) ;
        radio_vol_sel4->setChecked(0) ;
    }else if(radio_vol_sel3->isChecked()){

        this->lb_volumnNumMin->setText(QString("MIN\n%1dB").arg(highMinval));
        this->lb_volumnNumMax->setText(QString("MAX\n%1dB").arg(highMaxval));
        OutputDetailData.insert( "volumeMinLevel", 20);
        OutputDetailData.insert( "volumeMaxLevel", 20);
        slider_volMin->setValue(highMinval);
        slider_volMax->setValue(highMaxval);
        qDebug() << "volumeMinLevel : " << highMinval;
        qDebug() << "volumeMaxLevel : " << highMaxval;
        volumeMinLevel = highMinval;
        volumeMaxLevel = highMaxval;
        radioVolSel = 3;
        radio_vol_sel1->setChecked(0) ;
        radio_vol_sel2->setChecked(0) ;
        radio_vol_sel3->setChecked(1) ;
        radio_vol_sel4->setChecked(0) ;

    }else if(radio_vol_sel4->isChecked()){
        radioVolSel = 4;


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
void dialogsetting_output::set_selectedUsbName(QString usbstr){
    selectedUsbName = usbstr;
}
QString dialogsetting_output::get_selectedUsbName(){
    return selectedUsbName;
}
void dialogsetting_output::setTextName(QString usbstr){
    radio_UsbName->setText(usbstr);
}

void dialogsetting_output::setPropertyName(QString usbstr){
    radio_UsbName->setProperty(PROPERTY_NAME_READABLE, usbstr);
}
*/
/**
 * @brief SettingDisplay::setTextVolumn [SLOT]
 * @param p_value
 */
void dialogsetting_output::slot_setTextVolumnMin(int p_value){
    print_debug();
    int min_volLevel = -60;





    //radioVolSel = 4;
    //radio_vol_sel4->setChecked(1) ;
    qDebug() << "slot_setTextVolumnMin()-radioVolSel : " << radioVolSel;

    qDebug() << "lb_volumnNumMin = " << QString(" Min:%1").arg(p_value) ;
    if(p_value > -60){
        this->slider_volMin->setValue(-60);
        min_volLevel = -60;
        qDebug() << "min_volLevel = " << min_volLevel;
    }else{
        min_volLevel = p_value;
    }
    this->lb_volumnNumMin->setText(QString(" MIN\n %1dB").arg(min_volLevel));
    int tmp_min_volLevel = min_volLevel+80;
    OutputDetailData.insert( "volumeMinLevel", tmp_min_volLevel);
    //-------------------------------------------------------
   // QRect screenGeometry = this->geometry();
   // int height = screenGeometry.height();
   // int width = screenGeometry.width();

    //widget_maxbox->setContentsMargins(300,0,0,30);

    //--------------------------------------------------------------
    qDebug() << "volumeMinLevel : " << min_volLevel;
    qDebug() << "tmp_volumeMinLevel : " << tmp_min_volLevel;
    //print_debug();
}

void dialogsetting_output::slot_setTextVolumnMax(int p_value){
    print_debug();
    float max_volLevel = -5;
    radioVolSel = 4;
    radio_vol_sel4->setChecked(1) ;
    qDebug() << "slot_setTextVolumnMax()-radioVolSel : " << radioVolSel;

  //  if(radioVolSel != 4){
  //      slider_volMin->setTickPosition(QSlider::NoTicks);
  //     slider_volMax->setTickPosition(QSlider::NoTicks);

   // }else if(radioVolSel == 4){
   //     slider_volMin->setTickPosition(QSlider::TicksBothSides);
    //    slider_volMax->setTickPosition(QSlider::TicksBothSides);
   // }
    qDebug() << "lb_volumnNumMax = " << QString(" Max:%1").arg(p_value) ;

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
        this->lb_volumnNumMax->setText(QString(" MAX\n %1dB").arg(max_volLevel));
    }else{
        this->slider_volMax->setValue(-50);
        this->lb_volumnNumMax->setText(QString(" MAX\n %1dB").arg(-5));
    }
    int tmp_max_volLevel = (int)(max_volLevel + 5)/0.5;
    OutputDetailData.insert( "volumeMaxLevel", tmp_max_volLevel);
    qDebug() << "volumeMaxLevel : " << max_volLevel;
    qDebug() << "tmp_volumeMaxLevel : " << tmp_max_volLevel;


}

/**
  * @brief GSCommon::getUIPixmapImg 리소스이미지를 올린 QPixmap을 반환한다.
  * @param p_url 리소스이미지 경로
  * @param parent
  * @return QLabel
  */
QPixmap* dialogsetting_output::getcheonUIPixmapImg(QString p_url){//max

    //QImage *img = new QImage();
    QImage img;
    QPixmap *pixmap_icon = new QPixmap();
    if(img.load(p_url)){
        *pixmap_icon = QPixmap::fromImage(img);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_icon = pixmap_icon->scaled(img.width()+60, img.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);                //이미지 사이즈 조절
    }else{
        if(!p_url.isEmpty()){//c220804
            QMessageBox::about(0, "Image load Error", p_url+":Image load Error[Common]...Retry.. (p_url :"  + p_url);
        }else{
            qDebug() << "GSCommon::getUIPixmapImg===============================================================================p_url: is null";
        }
    }
    return pixmap_icon;
}

QPixmap* dialogsetting_output::getcheonUIPixmapImg2(QString p_url){//min

    //QImage *img = new QImage();
    QImage img;
    QPixmap *pixmap_icon = new QPixmap();
    if(img.load(p_url)){
        *pixmap_icon = QPixmap::fromImage(img);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_icon = pixmap_icon->scaled(img.width()+58, img.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);                //이미지 사이즈 조절
    }else{
        if(!p_url.isEmpty()){//c220804
            QMessageBox::about(0, "Image load Error", p_url+":Image load Error[Common]...Retry.. (p_url :"  + p_url);
        }else{
            qDebug() << "GSCommon::getUIPixmapImg===============================================================================p_url: is null";
        }
    }
    return pixmap_icon;
}
QPixmap* dialogsetting_output::getcheonUIPixmapImg_balance(QString p_url){

    //QImage *img = new QImage();
    QImage img;
    QPixmap *pixmap_icon = new QPixmap();
    if(img.load(p_url)){
        *pixmap_icon = QPixmap::fromImage(img);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_icon = pixmap_icon->scaled(img.width(), img.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);                //이미지 사이즈 조절
    }else{
        if(!p_url.isEmpty()){//c220804
            QMessageBox::about(0, "Image load Error", p_url+":Image load Error[Common]...Retry.. (p_url :"  + p_url);
        }else{
            qDebug() << "GSCommon::getUIPixmapImg===============================================================================p_url: is null";
        }
    }
    return pixmap_icon;
}

/**
  * @brief GSCommon::getUILabelImg 리소스이미지를 올린 QLabel을 반환한다.
  * @param p_url 리소스이미지 경로 (ex> :/images/playBar/net_play_ico.png )
  * @param parent
  * @return QLabel
  */
QLabel* dialogsetting_output::getcheonUILabelImg_balance(QString p_url){

    QPixmap *pixmap_icon = getcheonUIPixmapImg_balance(p_url);
    QLabel *lb_icon = new QLabel();
    lb_icon->setPixmap(*pixmap_icon);                                   //버퍼에 있는 이미지를 QLabel에 출력
    lb_icon->resize(pixmap_icon->width(), pixmap_icon->height());       //QLabel의 크기를 이미지 사이즈에 맞추어 조절한다.
    //lb_icon->resize(600, pixmap_icon->height());
    return lb_icon;
}

QLabel* dialogsetting_output::getcheonUILabelImg2(QString p_url){//min

    QPixmap *pixmap_icon = getcheonUIPixmapImg2(p_url);
    QLabel *lb_icon = new QLabel();
    lb_icon->setPixmap(*pixmap_icon);                                   //버퍼에 있는 이미지를 QLabel에 출력
    lb_icon->resize(pixmap_icon->width(), pixmap_icon->height());       //QLabel의 크기를 이미지 사이즈에 맞추어 조절한다.
    //lb_icon->resize(600, pixmap_icon->height());
    return lb_icon;
}
QLabel* dialogsetting_output::getcheonUILabelImg(QString p_url){//max

    QPixmap *pixmap_icon = getcheonUIPixmapImg(p_url);
    QLabel *lb_icon = new QLabel();
    lb_icon->setPixmap(*pixmap_icon);                                   //버퍼에 있는 이미지를 QLabel에 출력
    lb_icon->resize(pixmap_icon->width(), pixmap_icon->height());       //QLabel의 크기를 이미지 사이즈에 맞추어 조절한다.
    //lb_icon->resize(600, pixmap_icon->height());
    return lb_icon;
}

/**
 * @brief SettingDisplay::getUIControlOption 화면밝기 조절 컨트롤
 */
QWidget* dialogsetting_output::getVolumeUIControlOptionMin(){



    this->slider_volMin = new QSlider(Qt::Horizontal);
   // this->slider_volMin->setContentsMargins(10,0,0,0);
    this->slider_volMin->setObjectName("slider_vol_Min");
    this->slider_volMin->setTickInterval(1);
    //this->slider_volMin->setRange(-80, -32);
    this->slider_volMin->setRange(-80, -50);
   // this->slider_volMin->setMaximum(24);
    //this->slider_vol->setMaximumWidth(600);

    slider_volMin->setCursor(Qt::PointingHandCursor);

    //range slider   image: url(:/images/setting/201/vol_lev_btn.png);   image: url(:/images/setting/201/vol_lev_btn.png);
    slider_volMin->setStyleSheet("QSlider::groove:horizontal { background:#333333;height:11px;border-radius:2px;margin:0px 0px; } "
                                 "QSlider::handle:horizontal { border: 1px solid #d6d6d6; background: qlineargradient(x0: 0, y0: 0, x1: 0.5, y1: 0, x2: 1, y2: 1,stop: 0 #303030, stop: 1 #c8c8c8, stop: 2 #303030); width:38px; border-radius:11px;margin:-6px 0px; } "  /* handle(y,x) is placed by default on the contents rect of the groove. Expand outside the groove */
                                 "QSlider::add-page:horizontal { border: 2px solid black; background: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1, stop: 0 #ffff7f, stop: 1 #ffdf3c); "
                                 "background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1, stop: 0 #ffdf3c, stop: 1 #ff0000);} "
                                 "QSlider::sub-page:horizontal {border: 2px solid black; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e5e5e5, stop: 1 #383838);  } ");

    QLabel *lb_min = getcheonUILabelImg2(":/images/setting/201/vol_lev_db1.png");
    //QLabel *lb_max = GSCommon::getUILabelImg(":/images/setting/201/vol_lev_db2.png");
    lb_min->setStyleSheet("font-size:18px;color:#FFFFFF;");
    //lb_min->setFixedSize(330,55);

    QVBoxLayout *vl_lb_min = new QVBoxLayout();
    vl_lb_min->setContentsMargins(20,0,10,0);
    vl_lb_min->setSpacing(0);
    vl_lb_min->addWidget(lb_min);


    QVBoxLayout *vl_slider = new QVBoxLayout();
    vl_slider->setContentsMargins(0,0,0,0);
    vl_slider->setSpacing(0);
    vl_slider->setAlignment(Qt::AlignVCenter);
    vl_slider->addWidget(slider_volMin);
    vl_slider->addLayout(vl_lb_min);

    QWidget *widget_sliderbox = new QWidget();
    widget_sliderbox->setFixedSize(330,55);
    widget_sliderbox->setLayout(vl_slider);

    QHBoxLayout *hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(0,0,0,0);
    hl_total->setSpacing(0);

    hl_total->addWidget(widget_sliderbox);

    QWidget *widget = new QWidget();

    widget->setLayout(hl_total);


    return widget;

}

QPixmap dialogsetting_output::drawCMPattern(qreal dpi) {
    qreal dpcm = dpi / 2.54;
    QPixmap rulerCache(dpcm, dpcm / 2);
    rulerCache.fill();
    qreal dpmm = dpcm / 10;
    QPainter p(&rulerCache);
    p.setRenderHint(QPainter::HighQualityAntialiasing);
    qreal lineWidth = dpmm / 5;
    p.setPen(QPen(Qt::black, lineWidth));
    qreal xpos = lineWidth / 2;
    for (int i = 0; i < 10; ++i) {
        if (i == 0) p.drawLine(QLineF(xpos, 0, xpos, rulerCache.height()));
        else if (i == 5) p.drawLine(QLineF(xpos, 0, xpos, rulerCache.height() / 2));
        else p.drawLine(QLineF(xpos, 0, xpos, rulerCache.height() / 4));
        xpos += dpmm;
    }
    return rulerCache;
}

QWidget* dialogsetting_output::getBalanceUIControlOption(){



    this->slider_volBalance = new QSlider(Qt::Horizontal);
    this->slider_volBalance->setObjectName("slider_vol_Max");
    this->slider_volBalance->setTickInterval(1);
   // this->slider_volMax->setRange(-190, 50);
    this->slider_volBalance->setRange(0, 50);
    this->slider_volBalance->setMaximumWidth(510);

    slider_volBalance->setCursor(Qt::PointingHandCursor);

    //range slider
    slider_volBalance->setStyleSheet("QSlider::groove:horizontal { background:#dddddd;height:3px;border-radius:2px;margin:0px 0px; } "
                                 "QSlider::handle:horizontal { border: 1px solid #d6d6d6; background: qlineargradient(x0: 0, y0: 0, x1: 0.3, y1: 0, x2: 1, y2: 0,stop: 0 #c8c8c8, stop: 0.3 #303030, stop: 1 #c8c8c8); width:38px; border-radius:11px;margin:-16px -16px; } "  /* handle(y,x) is placed by default on the contents rect of the groove. Expand outside the groove */
                               // "QSlider::sub-page:horizontal { border: 2px solid black; background: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1, stop: 0 #ffff7f, stop: 1 #ffdf3c); "
                                // "background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1, stop: 0 #ffdf3c, stop: 1 #ff0000);} "
                                // "QSlider::add-page:horizontal {border: 2px solid black; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e5e5e5, stop: 1 #383838);  } "
                                 );
                         //"QSlider::handle:horizontal { background:white;width:14px;border-radius:7px;margin:-6px 0px;}"  /* handle(y,x) is placed by default on the contents rect of the groove. Expand outside the groove */


    QLabel *lb_max = getcheonUILabelImg_balance(":/images/setting/150/balance_ruler.png");

    //QLabel *lb_max = getcheonUILabelImg(":/images/setting/201/vol_lev_db2.png");
    lb_max->setFixedHeight(50);
  //  lb_max->setText("-5dB      5dB");
    QLabel *lb_title_l25 = new QLabel;
#if defined(Q_OS_WINDOWS)
    lb_title_l25->setText(tr("L25                                                        0                                                      R25"));//lb_title->setText(tr("클라우드 추가"));
#endif
#if defined(Q_OS_MAC)
    lb_title_l25->setText(tr("L25                                                      0                                                      R25"));//lb_title->setText(tr("클라우드 추가"));
#endif

    //lb_title_l25->setStyleSheet("font:bold;color:#FFFFFF;font-size:15px;");
    lb_title_l25->setStyleSheet("color:#FFFFFF;font-size:15px;");
    lb_title_l25->setAlignment(Qt::AlignLeft);
    lb_title_l25->setFixedHeight(20);



    QVBoxLayout *vl_balance = new QVBoxLayout();
    vl_balance->setContentsMargins(0,0,0,0);
    vl_balance->setSpacing(0);
    vl_balance->setAlignment(Qt::AlignTop);
    vl_balance->addWidget(lb_title_l25);
    vl_balance->addWidget(lb_max);

    QHBoxLayout *hl_lb_max = new QHBoxLayout();
    hl_lb_max->setContentsMargins(0,0,0,0);
   // hl_lb_max->setAlignment(Qt::AlignRight);
    hl_lb_max->addLayout(vl_balance);
    //hl_lb_max->setSpacing(10);

    QVBoxLayout *vl_slider = new QVBoxLayout();
    vl_slider->setContentsMargins(0,0,0,0);
    vl_slider->setSpacing(0);
    vl_slider->setAlignment(Qt::AlignVCenter);
    vl_slider->addLayout(hl_lb_max);
    vl_slider->addWidget(slider_volBalance);


    QWidget *widget_sliderbox = new QWidget();
    widget_sliderbox->setFixedSize(510,100);
    widget_sliderbox->setLayout(vl_slider);

    QHBoxLayout *hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(0,0,0,0);

    hl_total->addWidget(widget_sliderbox);

    QWidget *widget = new QWidget();
    widget->setLayout(hl_total);

    connect(slider_volBalance, SIGNAL(valueChanged(int)) ,this, SLOT(slot_setVolumnBalance(int)));

    return widget;

}

void dialogsetting_output::slot_setVolumnBalance(int p_value){
    print_debug();
    int balanceValue = 0;
    if(p_value < 0){
        //balanceValue = absp_value - 25;
    }else{
       // balanceValue = 25 - p_value ;
    }
    balanceValue = p_value ;
    qDebug() << "balanceValue = " << balanceValue;
       // this->slider_volMax->setValue(balanceValue);

    OutputDetailData.insert( "balanceValue", balanceValue);
    qDebug() << "balanceValue : " << balanceValue;


}

QWidget* dialogsetting_output::getVolumeUIControlOptionMax(){



    this->slider_volMax = new QSlider(Qt::Horizontal);
    this->slider_volMax->setObjectName("slider_vol_Max");
    this->slider_volMax->setTickInterval(5);
   // this->slider_volMax->setRange(-190, 50);
    this->slider_volMax->setRange(-100, 50);
    //this->slider_vol->setMaximumWidth(600);

    slider_volMax->setCursor(Qt::PointingHandCursor);

    //range slider
    slider_volMax->setStyleSheet("QSlider::groove:horizontal { background:#333333;height:11px;border-radius:2px;margin:0px 0px; } "
                                 "QSlider::handle:horizontal { border: 1px solid #d6d6d6; background: qlineargradient(x0: 0, y0: 0, x1: 0.5, y1: 0, x2: 1, y2: 1,stop: 0 #303030, stop: 1 #c8c8c8, stop: 2 #303030); width:38px; border-radius:11px;margin:-6px 0px; } "  /* handle(y,x) is placed by default on the contents rect of the groove. Expand outside the groove */
                                "QSlider::sub-page:horizontal { border: 2px solid black; background: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1, stop: 0 #ffff7f, stop: 1 #ffdf3c); "
                                 "background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1, stop: 0 #ffdf3c, stop: 1 #ff0000);} "
                                 "QSlider::add-page:horizontal {border: 2px solid black; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e5e5e5, stop: 1 #383838);  } ");
                         //"QSlider::handle:horizontal { background:white;width:14px;border-radius:7px;margin:-6px 0px; } "  /* handle(y,x) is placed by default on the contents rect of the groove. Expand outside the groove */

    QLabel *lb_max = getcheonUILabelImg(":/images/setting/201/vol_lev_db2.png");
  //  lb_max->setFixedSize(300,110);
  //  lb_max->setText("-5dB      5dB");
    lb_max->setStyleSheet("font-size:18px;color:#FFFFFF;");
    QHBoxLayout *hl_lb_max = new QHBoxLayout();
    hl_lb_max->setContentsMargins(115,0,0,0);
   // hl_lb_max->setAlignment(Qt::AlignRight);
    hl_lb_max->addWidget(lb_max);
    //hl_lb_max->setSpacing(10);

    QVBoxLayout *vl_slider = new QVBoxLayout();
    vl_slider->setContentsMargins(0,0,0,0);
    vl_slider->setSpacing(0);
    vl_slider->setAlignment(Qt::AlignVCenter);
    vl_slider->addWidget(slider_volMax);
    vl_slider->addLayout(hl_lb_max);

    QWidget *widget_sliderbox = new QWidget();
    widget_sliderbox->setFixedSize(330,55);
    widget_sliderbox->setLayout(vl_slider);

    QHBoxLayout *hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(0,0,0,0);

    hl_total->addWidget(widget_sliderbox);

    QWidget *widget = new QWidget();
    widget->setLayout(hl_total);

    return widget;

}

/*
QWidget* dialogsetting_output::tmp_getVolumeUIControlOption(){

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

void dialogsetting_output::dlgOutputMqa(){


    dlgConfirmOutput->setTitle(tr("HDMI MQA Replay Gain Notice"));
    dlgConfirmOutput->setText(tr("Activates Replay Gaon Control when playing MQA. \nSet the desired leveling value.\nThese values apply only to MQA sound sources."));
    //dlgConfirmHdmi->setText(tr("MQA 재생시 Replay Gaon Control을 활성화합니다. \n원하는 평준화값을 설정하시오.\n해당 값은 MQA  음원에만 적용됩니다."));

}
void dialogsetting_output::dlgOutputCodecTrim(){


    dlgConfirmOutput->setTitle(tr("CodecTrim Setting Notice"));
    dlgConfirmOutput->setText(tr("This is used to improve clipping. \n\n If the sound level of a digital sound source is encoded higher than normal and produced, clipping (distortion) occurs when this sound source is played. \n\n This is a unique function of RS201 that maintains the output level without distortion by compensating by lowering the input level as much as the codec trim and raising the output level."));
    //dlgConfirmHdmi->setText(tr("클리핑 현상을 개선하기 위하여 사용합니다. \n\n 디지탈 음원의 사운드 레벨이 정상보다 높게 엔코딩 되어 제작된 경우, 이 음원을 재생하게되면 클리핑(음찌그러짐) 현상이 발생합니다. \n\n 코덱트림만큼 입력레벨은 낮추고, 출력레벨은 올려주어 보상(Compensation)함으로써 음 왜곡없이 출력레벨을 유지시켜주는 RS201 고유의 기능입니다."));

}

void dialogsetting_output::slot_clickedOutputcodecTrim(){
    dlgOutputCodecTrim();
    QTimer::singleShot(500, this, SLOT(slot_showDlgOfOutputNotice()));
}
void dialogsetting_output::slot_clickedOutputmqaGain(){
    dlgOutputMqa();
    QTimer::singleShot(500, this, SLOT(slot_showDlgOfOutputNotice()));
}

void dialogsetting_output::slot_clickedOutputaddmqaGain(){
    //QPoint pos_mainWindow = QWidget::mapFromGlobal(this->pos());
    QPoint pos = QCursor::pos();
    int tmp_posX = pos.x();
    int tmp_posY = pos.y();
    qDebug() << "tmp_posX : " << tmp_posX;
    qDebug() << "tmp_posY : " << tmp_posY;

 //   print_debug();
 //   lb_volumnNumMax->setGeometry(this->width()-100, this->height()-100, 800, 130);
  //  lb_volumnNumMax->hide();
 //   lb_volumnNumMax->show();
 //   widget_minmaxbox->raise();
    if( mqadBData < 0 )
        mqadBData += 1;
    lb_text[1]->setText(QString("%1").arg(mqadBData) + tr("           ") + tr("dB"));
    OutputDetailData.insert( "mqaRelplayGain", mqadBData);
    qDebug() << "mqadBData : " << mqadBData;

}
void dialogsetting_output::slot_clickedOutputsubmqaGain(){

    if( mqadBData > -20 )
        mqadBData -= 1;
    lb_text[1]->setText(QString("%1").arg(mqadBData) + tr("           ") + tr("dB"));
    OutputDetailData.insert( "mqaRelplayGain", mqadBData);
    qDebug() << "mqadBData : " << mqadBData;
}

void dialogsetting_output::slot_clickedOutputaddcodecTrim(){

    if( outputdBData < 0 )
        outputdBData += 1;
    lb_text[0]->setText(QString("%1").arg(float(outputdBData/2.0)) + tr("           ") + tr("dB"));
    OutputDetailData.insert( "codecTrim", outputdBData);
    qDebug() << "outputdBData : " << outputdBData;

}
void dialogsetting_output::slot_clickedOutputsubcodecTrim(){
    if( outputdBData > -20 )
        outputdBData -= 1;
    lb_text[0]->setText(QString("%1").arg(float(outputdBData/2.0)) + tr("           ") + tr("dB"));
    OutputDetailData.insert( "codecTrim", outputdBData);
    qDebug() << "outputdBData : " << outputdBData;
}
QWidget* dialogsetting_output::getPlusMinusUIControlOption(QString p_title, int i, int data){

    lb_gain[i] = new QPushButton(p_title);
    lb_gain[i]->setStyleSheet("color:#E5E5E4;");
    lb_gain[i]->setFixedSize(200,55);
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
    lb_text[i]->setStyleSheet(" color:#FFFFFF;font-size:20px;");//background-color:#707070;");
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
void dialogsetting_output::dlgOutputMqa(){


    dlgConfirmOutput->setTitle(tr("Output MQA Replay Gain Notice"));
    dlgConfirmOutput->setText(tr("Activates Replay Gaon Control when playing MQA. \nSet the desired leveling value.\nThese values apply only to MQA sound sources."));
    //dlgConfirmOutput->setText(tr("MQA 재생시 Replay Gaon Control을 활성화합니다. \n원하는 평준화값을 설정하시오.\n해당 값은 MQA  음원에만 적용됩니다."));

}
void dialogsetting_output::dlgOutputOutput(){


    dlgConfirmOutput->setTitle(tr("OUTPUT Output Gain Notice"));
    dlgConfirmOutput->setText(tr("Adjusts the Digital Output Gain. \nExternal receiver Or, if distortion occurs when connecting to a DAC, adjust the Output Gain value appropriately."));
    //dlgConfirmOutput->setText(tr("Digital Output의 Gain 값을 조정합니다. \n외부 리시버 또는 DAC과 연결 시 distortion이 발생하면 Output Gain 값을 적절히 조절하여 사용합니다."));

}
*/

void dialogsetting_output::slot_showDlgOfOutputNotice(){
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


void dialogsetting_output::set_settingOfOutputDetail(QJsonObject tmpJson){
        OutputDetailData = tmpJson;
}
QJsonObject dialogsetting_output::get_settingOfOutputDetail(){
        return OutputDetailData;
}

void dialogsetting_output::slot_changedIndexOutput(int index){

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
    case  2 :
        selectedOutput_str = comboBoxHdmiDSD->itemText(index);
        qDebug() << " dsdMode: " << selectedOutput_str;
        OutputDetailData.insert( "dsdMode", index);
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

void dialogsetting_output::getOutputSettingOfsetting(){

   NetworkHttp *network = new NetworkHttp;
   QJsonObject tmp_json;
   tmp_json.insert("outputMode", 1);
   print_debug();
   connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
   network->request(HTTP_OUTPUT_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.setting.get"), tmp_json, true);

}

void dialogsetting_output::getOutputInOutSettingOfsetting(){

   NetworkHttp *network = new NetworkHttp;
   QJsonObject tmp_json;
   connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
   network->request(HTTP_OUTPUT_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("in.out.mode.get"), tmp_json, true);

}
void dialogsetting_output::slot_clickedOutputChangeSet(){
    print_debug();

   NetworkHttp *network = new NetworkHttp;
   QJsonObject tmp_json = get_settingOfOutputDetail();
   QJsonObject tmp_json_set;
   tmp_json_set.insert("data", tmp_json);
   tmp_json_set.insert("outputMode", 1);


   QJsonDocument doc(tmp_json_set);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"slot_clickedOutputChangeSet()-tmp_json_set :"<< strJson;//cheon09_io

   connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
   network->request(HTTP_OUTPUT_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.setting.set"), tmp_json_set, true, true);

}

void dialogsetting_output::setDialogOutput_show(){
   // QJsonObject *Output_json = get_settingOfOutputDetail();

    print_debug();
   QJsonDocument doc(OutputDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"setDialogOutput_show()-OutputDetailData :"<< strJson;//cheon09_io

    comboBoxOutputPreoutLevel->setCurrentIndex( OutputDetailData["preoutLevelInfo"].toInt()+1 );
    //comboBoxOutputPCMResampling->setCurrentIndex( OutputDetailData["samplingRate"].toInt() );
    comboBoxOutputPCMMaxSampling->setCurrentIndex( OutputDetailData["maxSamplingRate"].toInt() );
    comboBoxOutputPCMResampling->setCurrentIndex( OutputDetailData["samplingRate"].toInt() );
    comboBoxHdmiDSD->setCurrentIndex(OutputDetailData["dsdMode"].toInt());

    this->slider_volBalance->setValue(OutputDetailData["balanceValue"].toInt());

    int codecTrim = OutputDetailData["codecTrim"].toInt();
    outputdBData = codecTrim;
    lb_text[0]->setText(QString("%1").arg(float(outputdBData/2.0)) + tr("           ") + tr("dB"));


    qDebug() << "DLG_WIDTH :" <<  DLG_WIDTH;
    qDebug() << "DLG_HEIGHT :" <<  DLG_HEIGHT;

    qDebug() << "radioVolSel :" <<  radioVolSel;
    //qDebug() << "DLG_HEIGHT :" <<  DLG_HEIGHT;
    onOff_DRC->setValue(OutputDetailData["drcInfo"].toInt());
    onOff_32->setValue(OutputDetailData["isClearLowerBit"].toBool());
   // onOff->setValue(1);
    qDebug() << "drcInfo : " << OutputDetailData["drcInfo"].toInt();

    int volumeMinLevel = OutputDetailData["volumeMinLevel"].toInt();
    int volumeMaxLevel = OutputDetailData["volumeMaxLevel"].toInt();
    qDebug() << "volumeMinLevel : " << volumeMinLevel;
    qDebug() << "volumeMaxLevel : " << volumeMaxLevel;

    /*
    if(radioVolSel == 1){
        radio_vol_sel1->setChecked(1) ;
    }else radio_vol_sel1->setChecked(0) ;
    if(radioVolSel == 2){
        radio_vol_sel2->setChecked(1) ;
    }else radio_vol_sel2->setChecked(0) ;
    if(radioVolSel == 3){
       radio_vol_sel2->setChecked(1) ;
    }else radio_vol_sel2->setChecked(0) ;
    if(radioVolSel == 4){
        radio_vol_sel4->setChecked(1) ;
    }else radio_vol_sel4->setChecked(0) ;
*/
    if(volumeMinLevel == 10 && volumeMaxLevel == 10){
        print_debug();
        radio_vol_sel1->setChecked(1) ;
        radio_vol_sel2->setChecked(0) ;
        radio_vol_sel3->setChecked(0) ;
        radio_vol_sel4->setChecked(0) ;
    }else    if(volumeMinLevel == 0 && volumeMaxLevel == 0){
        radio_vol_sel1->setChecked(0) ;
        radio_vol_sel2->setChecked(1) ;
        radio_vol_sel3->setChecked(0) ;
        radio_vol_sel4->setChecked(0) ;
    }else     if(volumeMinLevel == 20 && volumeMaxLevel == 20){
        radio_vol_sel1->setChecked(0) ;
        radio_vol_sel2->setChecked(0) ;
        radio_vol_sel3->setChecked(1) ;
        radio_vol_sel4->setChecked(0) ;
    }else {
        radio_vol_sel1->setChecked(0) ;
        radio_vol_sel2->setChecked(0) ;
        radio_vol_sel3->setChecked(0) ;
        radio_vol_sel4->setChecked(1) ;
    }






if(volumeMinLevel <= -60 && volumeMinLevel >= -80){
    this->lb_volumnNumMin->setText(QString(" Min\n %1dB").arg(volumeMinLevel));
    this->slider_volMin->setValue(volumeMinLevel);
}else{
    this->lb_volumnNumMin->setText(QString(" Min\n %1dB").arg(-70));
    this->slider_volMin->setValue(-70);
}

 if(volumeMinLevel <= 5 && volumeMinLevel >= -5){
    this->lb_volumnNumMax->setText(QString(" MAX\n %1dB").arg(volumeMaxLevel));
    this->slider_volMax->setValue(volumeMaxLevel);
}else{
     this->lb_volumnNumMax->setText(QString(" MAX\n %1dB").arg(0));
     this->slider_volMax->setValue(0);
 }

   // ToastMsg::show(this, "", tr("The current OUTPUT setting information is displayed on the screen."));

}


void dialogsetting_output::setDialogOutput_save(QJsonObject p_jsonObject){
    QJsonObject tmp_Json;
    if(p_jsonObject.contains("data")){
        tmp_Json = p_jsonObject["data"].toObject(); //cheon09_io

    }

   // QJsonDocument doc(tmp_Json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"\npre-setDialogOutput_save()-tmp_Json :"<< strJson;//cheon09_io
     int codecTrim = tmp_Json["codecTrim"].toInt();
     int dacFilterType = tmp_Json["dacFilterType"].toInt();
     int drcInfo = tmp_Json["drcInfo"].toInt();
     int dsdMode = tmp_Json["dsdMode"].toInt();
     int hpOhm = tmp_Json["hpOhm"].toInt();//isClearLowerBit
     bool isDigitalPassThrough = tmp_Json["isDigitalPassThrough"].toBool();
     bool isClearLowerBit = tmp_Json["isClearLowerBit"].toBool();
     //OutputDetailData.insert( "isClearLowerBit", isClearLowerBit);
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
     OutputDetailData.insert( "isClearLowerBit", isClearLowerBit);
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

     //radioVolSel = 2;

     QJsonDocument doc1(OutputDetailData);  QString strJson1(doc1.toJson(QJsonDocument::Compact));  qDebug() <<"\nsetDialogOutput_save()-tmp_Json :"<< strJson1 << "\n";//cheon09_io

    // ToastMsg::show(this, "", tr("Save the information from the Rose deive."));
}

void dialogsetting_output::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    QString deviceType = global.device.getDeviceType();
  //  print_debug();//cheon0219

    print_debug();  QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nslot_responseHttp =: " << strJson;//cheon12_io
    //qDebug() << "p_id : " << p_id;
    QString internalMode;

    switch(p_id){
    case HTTP_OUTPUT_GET_MODE :

        if( deviceType == "RS150" || deviceType == "RS150B"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                setDialogOutput_save(p_jsonObject);
                setDialogOutput_show();
            }

        }else if( deviceType == "RS201"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                setDialogOutput_save(p_jsonObject);
                setDialogOutput_show();

               // setDialogOutput_save(p_jsonObject);

            }

        }else if( deviceType == "RS301"){

        }else if( deviceType == "RS350"){

        }
        break;
    case HTTP_OUTPUT_SET_MODE :

        if( deviceType == "RS150" || deviceType == "RS150B"){
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


        }else if( deviceType == "RS201"){
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

QWidget* dialogsetting_output::getUIControlOption_DRC(QString p_title){

    QPushButton *lb_onOfftitle = new QPushButton(p_title);
    lb_onOfftitle->setProperty("btnNo", 4);
    lb_onOfftitle->setStyleSheet("color:#E5E5E4;");
    lb_onOfftitle->setFixedSize(110,55);
    lb_onOfftitle->setText(p_title);
    lb_onOfftitle->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");
    lb_onOfftitle->setCursor(Qt::PointingHandCursor);//print_debug();
    onOff_DRC = new OnOffWidget;print_debug()
    //onOff->setValue(false);
    onOff_DRC->setCursor(Qt::PointingHandCursor);//print_debug();

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
    hl_lb->addWidget(onOff_DRC, 0, Qt::AlignRight);
    print_debug();
    QPushButton *btn_onOffHover = new QPushButton(onOff_DRC);//print_debug();
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

    //connect(lb_onOfftitle, SIGNAL(clicked()), this, SLOT(slot_modeOnOff()));
    connect(btn_onOffHover, SIGNAL(clicked()), this, SLOT(slot_modeOnOff_drc()));
    return widget_total;

}

QWidget* dialogsetting_output::getUIControlOption_32(QString p_title){

    QPushButton *lb_onOfftitle = new QPushButton(p_title);
    lb_onOfftitle->setProperty("btnNo", 4);
    lb_onOfftitle->setStyleSheet("color:#E5E5E4;");
    lb_onOfftitle->setFixedSize(500,55);
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
    connect(btn_question, SIGNAL(clicked()), this, SLOT(slot_clickedHOIquestion()));

    //connect(lb_onOfftitle, SIGNAL(clicked()), this, SLOT(slot_modeOnOff()));
    connect(btn_onOffHover, SIGNAL(clicked()), this, SLOT(slot_modeOnOff_32()));
    return widget_total;

}

//QWidget* dialogsetting_output::getUIControlOption(QString p_title, int index){//c220511
/*
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
    */
//}

void dialogsetting_output::slot_DRC_question_popup(){
    dlgOutputDRC();
    QTimer::singleShot(500, this, SLOT(slot_showDlgOfOutputNotice()));

}


void dialogsetting_output::slot_CodecTrim_question_popup(){
    dlgOutputCodecTrim();
    QTimer::singleShot(500, this, SLOT(slot_showDlgOfOutputNotice()));

}

/**
 * @brief SettingVU::slot_modeOnOff [SLOT] VU 모드 ON/OFF 변경 API 호출
 */
//void SettingVideo::slot_modeOnOff(bool flagOn){
void dialogsetting_output::slot_modeOnOff_drc(){

   // DialogConfirm *dlg = new DialogConfirm(this);
   // dlg->setTitle(tr("HDMI Passthrough"));

    //dlg->setText(tr("Do you want to change it?"));

   // int result = dlg->exec();
   // if(result==QDialog::Accepted){
    //}

       int ispass = !this->onOff_DRC->getCurrentValue();
       if(ispass) {
           OutputDetailData.insert("drcInfo", 1);

       }else{
           OutputDetailData.insert("drcInfo", 0);

       }
       this->onOff_DRC->setValue(ispass);
       //QMessageBox::information(this,"Title","dddddddddddd");
       OutputDetailData.insert( "drcInfo", ispass);


      // notice_msg = tr("It bypasses HD audio formats such as surround sound sources to the audio receiver through HDMI without decoding.");
      // lb_notice->setText(tr("- Notice - ") + notice_msg);
       QJsonDocument doc(OutputDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_modeOnOff()-Hdmi_json =: " << strJson;



}
void dialogsetting_output::slot_modeOnOff_32(){

   // DialogConfirm *dlg = new DialogConfirm(this);
   // dlg->setTitle(tr("HDMI Passthrough"));

    //dlg->setText(tr("Do you want to change it?"));

   // int result = dlg->exec();
   // if(result==QDialog::Accepted){
    //}

       int ispass = !this->onOff_32->getCurrentValue();
       if(ispass) {
           OutputDetailData.insert("isClearLowerBit", true);
           this->onOff_32->setValue(true);

       }else{
           OutputDetailData.insert("isClearLowerBit", false);
           this->onOff_32->setValue(false);

       }

       //QMessageBox::information(this,"Title","dddddddddddd");
      // OutputDetailData.insert( "isClearLowerBit", ispass);


      // notice_msg = tr("It bypasses HD audio formats such as surround sound sources to the audio receiver through HDMI without decoding.");
      // lb_notice->setText(tr("- Notice - ") + notice_msg);
       QJsonDocument doc(OutputDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_modeOnOff()-Hdmi_json =: " << strJson;



}


void dialogsetting_output::dlgOutputDRC(){
    dlgConfirmOutput->setTitle(tr("Dynamic Range Control Notice"));
    //dlgConfirmHdmi->setText(tr("음원의 음압레벨이 높고 음량을 과하게 조절할 경우 출력신호에 Clipping 현상이 발생할 수 있습니다. \n \n DRC 모드를 사용하면 출력 크기를 자동으로 조절하여 Clipping 현상을 방지하여 편안한 사운드를 출력합니다."));
    dlgConfirmOutput->setText(tr("If the sound pressure level of the sound source is high and the volume is excessively adjusted, clipping may occur in the output signal. \n\n When DRC mode is used, the output size is automatically adjusted to prevent clipping, resulting in a comfortable sound."));

}


void dialogsetting_output::set_selected_deviceName(QString usbstr){
    selected_deviceName = usbstr;
}
QString dialogsetting_output::get_selected_deviceName(){
    return selected_deviceName;
}
void dialogsetting_output::setTextName(QString usbstr){
    radio_UsbName->setText(usbstr);
}

void dialogsetting_output::setPropertyName(QString usbstr){
    radio_UsbName->setProperty(PROPERTY_NAME_READABLE, usbstr);
}
