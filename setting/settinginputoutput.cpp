#include "settinginputoutput.h"
#include "common/global.h"

#include <QScrollArea>
#include <widget/flowlayout.h>
#include <common/gscommon.h>
#include <common/networkhttp.h>

#include "widget/toastmsg.h"
#include "widget/dialogconfirm.h"
#include "login/dialogselect_usbdac.h"
#include "login/dialogselect_usbdac_150.h"
#include "login/dialogselect_usbdac_520.h"
#include "login/dialogsetting_hdmi.h"
#include "login/dialogsetting_usbsetting.h"//c220708
#include "login/dialogsetting_RXsetting.h"//c220708
#include "login/dialogsetting_hdmi_150.h"
#include "login/dialogsetting_hdmi_520.h"
#include "login/dialogsetting_hdmi_250.h"
#include "login/dialogsetting_opt.h"
#include "login/dialogsetting_interoutput_150.h"
#include "login/dialogsetting_interoutput_520.h"

#include "login/dialogsetting_interoutput_250.h"
#include "login/dialogsetting_output.h"
#include "login/dialogsetting_output_150.h"
#include "login/dialogsetting_output_520.h"
#include "login/dialogsetting_output_150B.h"
#include "login/dialogsetting_output_250.h"

#include <QDebug>
#include <QMessageBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QDialogButtonBox>

//#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


const int INPUT_UI_MARGIN_LEFT = 33;
const int INPUT_UI_HORIZONTAL_MIN_WIDTH = 450;
const int INPUT_UI_VERTICAL_MIN_WIDTH = 640;
//const int INPUT_UI_HEIGHT = 220;
const int INPUT_UI_HEIGHT = 220;
const int ICON_SETTING_WIDTH = 70;

const int HTTP_GET_MODE = 99;
const int HTTP_SET_MODE = 98;



/**
 * @brief BtnLabel::BtnLabel 생성자
 * @param p_text
 * @param p_objName
 * @param p_imgPath_selected
 * @param p_imgPath_unSelected
 * @param parent
 */
BtnLabel::BtnLabel(QString p_text, QString p_objName, QString p_imgPath_selected, QString p_imgPath_unSelected, QWidget *parent) : QPushButton(parent)
{

    Q_UNUSED(p_objName);

    if(p_objName == "btn520_input_Model"){
        pixmap_selected = GSCommon::getUIPixmapImg_WH(p_imgPath_selected, 330, 190);
    }else{
        pixmap_selected = GSCommon::getUIPixmapImg(p_imgPath_selected);
    }
    pixmap_unSelected = GSCommon::getUIPixmapImg(p_imgPath_unSelected);

    lb_image = new QLabel();
    lb_text = new QLabel(p_text);
    if(p_text==""){
        lb_text->setVisible(false);
    }
    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(lb_image, 0, Qt::AlignHCenter);
    vl_total->addWidget(lb_text, 0, Qt::AlignHCenter);
    this->setLayout(vl_total);
    this->setStyleSheet("border:0px;");

    // default
    this->setCursor(Qt::PointingHandCursor);
    this->setStyleSelected(false);
}
void BtnLabel::setText(QString p_text){

    this->lb_text->setText(p_text);
    if(p_text==""){
        lb_text->setVisible(false);
    }else{
        lb_text->setVisible(true);
    }
}
void BtnLabel::setImgPath(QString p_imgPath_selected, QString p_imgPath_unSelected){

    pixmap_selected = GSCommon::getUIPixmapImg(p_imgPath_selected);
    pixmap_unSelected = GSCommon::getUIPixmapImg(p_imgPath_unSelected);
}

void BtnLabel::setStyleSelected(bool p_flagSelected){
    if(p_flagSelected==true){
        lb_image->setPixmap(*pixmap_selected);
        lb_image->resize(pixmap_selected->width(), pixmap_selected->height());
        lb_text->setStyleSheet("color:#B18658;font-size:18px;");
    }else{
        lb_image->setPixmap(*pixmap_unSelected);
        lb_image->resize(pixmap_unSelected->width(), pixmap_unSelected->height());
        lb_text->setStyleSheet("color:#FFFFFF;font-size:18px;");
    }
}

void BtnLabel::setStyleText(QString a){
    lb_text->setStyleSheet(a);
}

/**
 * @brief SettingInputOutput::SettingInputOutput 생성자
 * @param parent
 */
SettingInputOutput::SettingInputOutput(QWidget *parent) : QWidget(parent)
{

    linker = Linker::getInstance();//c221013_1
    this->setUIControl();

}


/**
 * @brief SettingInputOutput::setUIControl UI 세팅
 */
void SettingInputOutput::setUIControl(){
    QString deviceType = global.device.getDeviceType();
    //print_debug();//cheon0219
    //qDebug() << "setUIControl()-deviceType : " << deviceType;
    QThread::msleep(500);
    if( deviceType == "RS150"){
        setUIControl150(deviceType);
    }
    else if( deviceType == "RS150B"){
        setUIControl150(deviceType);
    }
    else if( deviceType == "RS201"){
        setUIControl201(deviceType);
    }
    else if( deviceType == "RS250"){
        setUIControl250(deviceType);
    }
    else if( deviceType == "RS250A"){
        setUIControl250(deviceType);
    }
    else if( deviceType == "RS520"){
        setUIControl520(deviceType);
    }
    else{
        ToastMsg::show(this, "", tr("Currently, only ROSE201, ROSE150, ROSE150B, ROSE250 are in service for setting function."),4000);//c220921_1
    }

}



//150input  :
const int RS150         = 0;//         - btn150_input_Model
const int LINEIN_L        = 3;//          - btn150_line_in_l
const int LINEIN_R        = 4;//          - btn150_line_in_r
const int OPTIN         = 7;//          - btn150_opt_in
const int COAX_IN         = 6;//          - btn150_HDMI_ARC_in
const int ARCIN         = 5;//          - btn150_HDMI_ARC_in
const int USBIN         = 2;//          - btn150_USB_in
const int AES_EBUIN     = 1;//        - btn150_AES_EBU_in

//150output :
const int DAC_2010      = 51;//        - btn150_dac_2010_out
const int HDMI          = 52;//       - btn150_HDMI_out


//150internal :
const int COAX_OUT      = 11;//   - btn150_coax_out
const int I2S_RJ45_OUT  = 15;//     - btn150_i2s_rj45_out
const int AES_EBU_OUT   = 14;//    - btn150_AES_EBU_out
const int I2S_DVI_OUT   = 13;//      - btn150_i2s_dvi_out
const int OPT_OUT       = 12;//      - btn150_opt_out
const int BALANCED_OUT_L  = 16;//  - btn150_balanced_out_l
const int BALANCED_OUT_R  = 17;//  - btn150_balanced_out_r
const int UNBALANCED_OUT_L  = 18;//  - btn150_unbal2_out_l
const int UNBALANCED_OUT_R  = 19;//  - btn150_unbal2_out_r
const int SPEAKER  = 51;//  - btn150_balanced_out_l
//150setting button :
const int BTN_USB150  = 31;
const int BTN_HDMI150  = 32;
const int BTN_INTERNAL150  = 33;
const int BTN_BALAMP150  = 34;





//201input  :
const int RS201         = 0;//         - btn150_input_Model
const int LINEIN_L201        = 1;//          - btn150_line_in_l
const int LINEIN_R201        = 2;//          - btn150_line_in_r
const int OPTIN201         = 3;//          - btn150_opt_in

//201output :
const int USB_201      = 21;//        - btn201_dac_2010_out
const int HDMI_201         = 22;//       - btn201_HDMI_out
const int OPT_OUT201      = 23;//        - btn201_dac_2010_out

//201internal output :
const int PREAMP_OUTL201      =31;//        - btn201_dac_2010_out
const int PREAMP_OUTR201         = 32;//       - btn201_HDMI_out
const int AMP_OUTL201      = 33;//        - btn201_dac_2010_out
const int AMP_OUTR201      = 34;//        - btn201_dac_2010_out
//201setting button :
const int BTN_USB201  = 41;
const int BTN_HDMI201  = 42;
const int BTN_OPT201  = 43;
const int BTN_PREAMP201  = 44;

//520input  ://c220922_1
const int RS520         = 0;//         - btn150_input_Model
const int LINEIN_L520        = 1;//          - btn150_line_in_l
const int LINEIN_R520        = 2;//          - btn150_line_in_r
const int OPTIN520         = 3;//          - btn150_opt_in

//520output :
const int USB_520      = 21;//        - btn520_dac_5200_out
const int HDMI_520         = 22;//       - btn520_HDMI_out
const int OPT_OUT520      = 23;//        - btn520_dac_5200_out

//520internal output :
const int PREAMP_OUTL520      =31;//        - btn520_dac_5200_out
const int PREAMP_OUTR520         = 32;//       - btn520_HDMI_out
const int AMP_OUTL520      = 33;//        - btn520_dac_5200_out
const int AMP_OUTR520      = 34;//        - btn520_dac_5200_out
//520setting button :
const int BTN_USB520  = 41;
const int BTN_HDMI520  = 42;
const int BTN_INTERNAL520  = 43;
const int BTN_UNBALANCE520  = 44;
const int BTN_SPEAKER520  = 45;

void SettingInputOutput::setUIControl520(QString deviceType){//c220922_1
print_debug();
    //QString deviceTypeImgSelected = ":/images/setting/520/inout_520_in2_gold_selected.png";
    QString deviceTypeImgSelected = ":/images/setting/520/inout_520_in_selected.png";
    QString deviceTypeImgUnSelected = ":/images/setting/520/inout_520_in2.png";
    qDebug() << "setUIControl201-device_type : " << deviceType;
    // INPUT Body
    btn520_input_Model = new BtnLabel(deviceType, "btn520_input_Model", deviceTypeImgSelected, deviceTypeImgUnSelected);
    btn520_input_Model->setFixedSize(330,270);
    btn520_input_Model->setProperty("btnNo", 0);
    QVBoxLayout *vl_btn520_input_Model = new QVBoxLayout();
    vl_btn520_input_Model->setContentsMargins(0,0,0,0);
    vl_btn520_input_Model->setSpacing(0);
    vl_btn520_input_Model->addWidget(btn520_input_Model, 0, Qt::AlignCenter);

    btn520_AES_EBU_in = new BtnLabel(tr("AES/EBU IN"), "btn520_AES_EBU_in", ":/images/setting/520/inout_AES_EBU_IN_gold_selected.png", ":/images/setting/520/inout_AES_EBU_IN.png");
    btn520_AES_EBU_in->setFixedSize(192,228);
    btn520_AES_EBU_in->setProperty("btnNo", 1);
    QVBoxLayout *vl_btn520_AES_EBU_in = new QVBoxLayout();
    vl_btn520_AES_EBU_in->setContentsMargins(0,0,0,0);
    vl_btn520_AES_EBU_in->setSpacing(0);
    vl_btn520_AES_EBU_in->addWidget(btn520_AES_EBU_in);

    btn520_USB_in = new BtnLabel(tr("USB IN"), "btn520_USB_in", ":/images/setting/520/inout_USB_IN_gold_selected.png", ":/images/setting/520/inout_USB_IN.png");
    btn520_USB_in->setFixedSize(116,105);
    btn520_USB_in->setProperty("btnNo", 2);
    QVBoxLayout *vl_btn520_USB_in = new QVBoxLayout();
    vl_btn520_USB_in->setContentsMargins(0,0,0,0);
    vl_btn520_USB_in->setSpacing(0);
    vl_btn520_USB_in->addWidget(btn520_USB_in);

    btn520_line_in_l = new BtnLabel(tr("LINE IN L"), "btn_line_in_l", ":/images/setting/520/inout_line_in_gold_selected.png", ":/images/setting/520/inout_line_in.png");
    btn520_line_in_l->setFixedSize(109,109);
    btn520_line_in_l->setProperty("btnNo", 3);
    QVBoxLayout *vl_btn520_line_in_l = new QVBoxLayout();
    vl_btn520_line_in_l->setContentsMargins(0,0,0,0);
    vl_btn520_line_in_l->setSpacing(0);
    vl_btn520_line_in_l->addWidget(btn520_line_in_l);

    btn520_line_in_r = new BtnLabel(tr("LINE IN R"), "btn520_line_in_r", ":/images/setting/520/inout_line_in_gold_selected.png", ":/images/setting/520/inout_line_in.png");
    btn520_line_in_r->setFixedSize(109,109);
    btn520_line_in_r->setProperty("btnNo", 4);
    QVBoxLayout *vl_btn520_line_in_r = new QVBoxLayout();
    vl_btn520_line_in_r->setContentsMargins(0,0,0,0);
    vl_btn520_line_in_r->setSpacing(0);
    vl_btn520_line_in_r->addWidget(btn520_line_in_r);

    //--------------------usb+line l+r hollizen------------------------------------------
    QHBoxLayout *hl_btns_inputLine1 = new QHBoxLayout();
    // hl_btns_inputLine1->setContentsMargins(0,40,0,40);
    hl_btns_inputLine1->setSpacing(37);
    hl_btns_inputLine1->setAlignment(Qt::AlignCenter);
    hl_btns_inputLine1->addLayout(vl_btn520_USB_in);
    hl_btns_inputLine1->addLayout(vl_btn520_line_in_l);
    hl_btns_inputLine1->addLayout(vl_btn520_line_in_r);

    btn520_HDMI_ARC_in = new BtnLabel(tr("eARC IN"), "btn520_HDMI_ARC_in", ":/images/setting/520/inout_HDMI_ARC_IN_gold_selected.png", ":/images/setting/520/inout_HDMI_ARC_IN.png");
    btn520_HDMI_ARC_in->setFixedSize(134,98);
    btn520_HDMI_ARC_in->setProperty("btnNo", 5);
    QVBoxLayout *vl_btn520_HDMI_ARC_in = new QVBoxLayout();
    vl_btn520_HDMI_ARC_in->setContentsMargins(0,0,0,0);
    vl_btn520_HDMI_ARC_in->setSpacing(0);
    vl_btn520_HDMI_ARC_in->addWidget(btn520_HDMI_ARC_in);

    btn520_opt_in = new BtnLabel(tr("OPT IN"), "btn520_opt_in", ":/images/setting/520/inout_opt_gold_selected.png", ":/images/setting/520/inout_opt.png");
    btn520_opt_in->setFixedSize(109,109);
    btn520_opt_in->setProperty("btnNo", 7);
    QVBoxLayout *vl_btn520_opt_in = new QVBoxLayout();
    vl_btn520_opt_in->setContentsMargins(0,0,0,0);
    vl_btn520_opt_in->setSpacing(0);
    vl_btn520_opt_in->addWidget(btn520_opt_in);

    btn520_coax_in = new BtnLabel(tr("COAX IN"), "btn520_coax_in", ":/images/setting/520/inout_coax_gold_520_selected.png", ":/images/setting/520/inout_coax_520.png");
    btn520_coax_in->setFixedSize(109,109);
    btn520_coax_in->setProperty("btnNo", 6);
    QVBoxLayout *vl_btn520_coax_in = new QVBoxLayout();
    vl_btn520_coax_in->setContentsMargins(0,0,0,0);
    vl_btn520_coax_in->setSpacing(0);
    vl_btn520_coax_in->addWidget(btn520_coax_in);

    //--------------------ARC+COAX+OPT hollizen------------------------------------------
    QHBoxLayout *hl_btns_inputLine2 = new QHBoxLayout();
    //  hl_btns_inputLine2->setContentsMargins(0,40,0,40);
    hl_btns_inputLine2->setSpacing(37);
    hl_btns_inputLine2->setAlignment(Qt::AlignCenter);
    hl_btns_inputLine2->addLayout(vl_btn520_HDMI_ARC_in);
    hl_btns_inputLine2->addLayout(vl_btn520_coax_in);
    hl_btns_inputLine2->addLayout(vl_btn520_opt_in);

    //--------------------USB+ARC vertival-----------------------------------------
    QVBoxLayout *hl_btns_inputLine12 = new QVBoxLayout();
    hl_btns_inputLine12->setContentsMargins(0,40,0,40);
    hl_btns_inputLine12->setSpacing(37);
    hl_btns_inputLine12->setAlignment(Qt::AlignCenter);
    hl_btns_inputLine12->addLayout(hl_btns_inputLine1);
    hl_btns_inputLine12->addLayout(hl_btns_inputLine2);

    //-------------------  input Model ----------------------------

    QHBoxLayout *hl_btn520_input_Model_box = new QHBoxLayout();
    hl_btn520_input_Model_box->setContentsMargins(290,0,290,0);

    hl_btn520_input_Model_box->setSpacing(10);
    hl_btn520_input_Model_box->setAlignment(Qt::AlignCenter);
    hl_btn520_input_Model_box->addLayout(vl_btn520_input_Model);
    //--------------------AES/EBU + inputline12 hollizen------------------------------------------
    QHBoxLayout *hl_btns_inputLine012 = new QHBoxLayout();
    hl_btns_inputLine012->setContentsMargins(240,10,240,10);
    hl_btns_inputLine012->setSpacing(37);
    hl_btns_inputLine012->setAlignment(Qt::AlignCenter);
    //hl_btns_inputLine012->addLayout(vl_btn520_AES_EBU_in);
    hl_btns_inputLine012->addLayout(hl_btns_inputLine12);

    int fixedImgWidth =117;// 140;
    //--------------------------------------usb   -----------------------------
    btn520_dac_2010_out = new BtnLabel(tr("DAC 2010"), "btn520_dac_2010_out", ":/images/setting/520/inout_usb_out2_gold_selected.png", ":/images/setting/520/inout_usb_out2.png");
    btn520_dac_2010_out->setFixedSize(116, 151);//170);
    btn520_dac_2010_out->setProperty("btnNo", 51);
    btn520_dac_2010_out->setText("USB");
    btn520_setting_usb_2010 = GSCommon::getUIBtnImg("btn520_setting_usb_2010", ":/images/fol_edit_ico1_2.png");
    // btn_icon->setStyleSheet("#" + p_objName + " { background-color:rgba(0,0,0,0); background-image: url('" + p_url + "'); background-repeat:no-repeat; background-position:center center; } ");
    QString tobj = "btn520_setting_usb_2010";
    //if(get_InputMode520()==4)
    //    btn520_setting_usb_2010->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
    btn520_setting_usb_2010->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn520_setting_usb_2010->setProperty("btnNo", BTN_USB520);
    btn520_setting_usb_2010->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_btn520_dac_2010_out = new QHBoxLayout();
    hl_btn520_dac_2010_out->setContentsMargins(0,0,90,0);
    hl_btn520_dac_2010_out->setSpacing(0);
    hl_btn520_dac_2010_out->addWidget(btn520_setting_usb_2010, 0, Qt::AlignTop);
    hl_btn520_dac_2010_out->addWidget(btn520_dac_2010_out, 30, Qt::AlignHCenter);

    widget_box_dac_2010_out = new QWidget();
    widget_box_dac_2010_out->setObjectName("widget_box_dac_2010_out");
    widget_box_dac_2010_out->setFixedHeight(225);
    widget_box_dac_2010_out->setFixedWidth(264);
    widget_box_dac_2010_out->setStyleSheet("#widget_box_dac_2010_out { border:1px solid #707070; }");
    widget_box_dac_2010_out->setLayout(hl_btn520_dac_2010_out);
    //------------------------------------------   hdmi     -------------------------------------------------------------------------
    btn520_HDMI_out = new BtnLabel(tr("HDMI"), "btn520_HDMI_out", ":/images/setting/520/inout_HDMI_ARC_IN_gold_selected.png", ":/images/setting/520/inout_HDMI_ARC_IN.png");
    btn520_HDMI_out->setFixedSize(fixedImgWidth+40, 98);//170);
    btn520_HDMI_out->setProperty("btnNo", 52);
    btn520_setting_hdmi = GSCommon::getUIBtnImg("btn520_setting_hdmi", ":/images/fol_edit_ico1_2.png");
    tobj = "btn520_setting_hdmi";
    if(get_InputMode520()==4 && outputvalue != 3)
        btn520_setting_hdmi->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

    btn520_setting_hdmi->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn520_setting_hdmi->setProperty("btnNo", BTN_HDMI520);
    btn520_setting_hdmi->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_btn520_hdmi_out = new QHBoxLayout();
    hl_btn520_hdmi_out->setContentsMargins(0,0,90,0);
    hl_btn520_hdmi_out->setSpacing(0);
    hl_btn520_hdmi_out->addWidget(btn520_setting_hdmi, 0, Qt::AlignTop);
    hl_btn520_hdmi_out->addWidget(btn520_HDMI_out, 30, Qt::AlignHCenter);

    widget_box_hdmi_out = new QWidget();
    widget_box_hdmi_out->setObjectName("widget_box_hdmi_out");
    widget_box_hdmi_out->setFixedHeight(225);
    widget_box_hdmi_out->setFixedWidth(264);
    widget_box_hdmi_out->setStyleSheet("#widget_box_hdmi_out { border:1px solid #707070; }");
    widget_box_hdmi_out->setLayout(hl_btn520_hdmi_out);
    //---------------------------------------------------------------------------------------------------------------------------

    btn520_coax_out = new BtnLabel(tr("COAX OUT"), "btn520_coax_out", ":/images/setting/520/inout_coax_gold_520_selected.png", ":/images/setting/520/inout_coax_520.png");
    btn520_coax_out->setFixedSize(110,110);
    // btn520_coax_out->setProperty("internalMode", 64);
    btn520_coax_out->setProperty("btnNo", 11);


    btn520_opt_out = new BtnLabel(tr("OPT OUT"), "btn520_opt_out", ":/images/setting/520/inout_opt_gold_selected.png", ":/images/setting/520/inout_opt.png");
    btn520_opt_out->setFixedSize(110,110);
    // btn520_opt_out->setProperty("internalMode", 4);
    btn520_opt_out->setProperty("btnNo", 12);



    btn520_i2s_dvi_out = new BtnLabel(tr("I2S OUT (DVI)"), "btn520_i2s_dvi_out", ":/images/setting/520/inout_i2s_dvi_gold_selected.png", ":/images/setting/520/inout_i2s_dvi.png");
    btn520_i2s_dvi_out->setFixedSize(258,117);
    //QString interValue = "01000000";
    //    btn520_i2s_dvi_out->setProperty("internalMode", 8); //0x40);
    btn520_i2s_dvi_out->setProperty("btnNo", 13);


    btn520_AES_EBU_out = new BtnLabel(tr("AES/EBU OUT"), "btn520_AES_EBU_out", ":/images/setting/520/inout_AES_EBU_IN_gold_selected.png", ":/images/setting/520/inout_AES_EBU_IN_gray.png");
    btn520_AES_EBU_out->setFixedSize(192,228);
    // btn520_AES_EBU_out->setProperty("internalMode", 16);
    btn520_AES_EBU_out->setProperty("btnNo", 14);


    btn520_i2s_rj45_out = new BtnLabel(tr("I2S OUT (RJ-45)"), "btn520_i2s_rj45_out", ":/images/setting/520/inout_i2s_out2_gold_selected.png", ":/images/setting/520/inout_i2s_out2_gray.png");
    btn520_i2s_rj45_out->setFixedSize(184,247);
    //    btn520_i2s_rj45_out->setProperty("internalMode", 32);
    btn520_i2s_rj45_out->setProperty("btnNo", 15);

    //----------------------------------------------------------------------------------------
    QHBoxLayout *hl_btn520_internal123_out = new QHBoxLayout();
    hl_btn520_internal123_out->setContentsMargins(0,0,0,0);
    hl_btn520_internal123_out->setSpacing(100);
    hl_btn520_internal123_out->addWidget(btn520_coax_out, 0, Qt::AlignLeft);
    hl_btn520_internal123_out->addWidget(btn520_opt_out, 0, Qt::AlignRight);
    //hl_btn520_internal123_out->addWidget(btn520_i2s_dvi_out, 0, Qt::AlignHCenter);


    //-----------------------------------------------------------------------------------
    QVBoxLayout *vl_btn520_internal_out = new QVBoxLayout();
    vl_btn520_internal_out->setContentsMargins(0,0,0,0);
    vl_btn520_internal_out->setSpacing(0);
    vl_btn520_internal_out->addLayout(hl_btn520_internal123_out);
    //vl_btn520_internal_out->addLayout(hl_btn520_internal45_out);
    //--------------------------------------------------------------------------------

    btn520_setting_internal = GSCommon::getUIBtnImg("btn520_setting_internal", ":/images/fol_edit_ico1_2.png");
    tobj = "btn520_setting_internal";
    if(get_InputMode520()==4 && outputvalue != 3)
        btn520_setting_internal->setStyleSheet("#" +tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

    btn520_setting_internal->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn520_setting_internal->setProperty("btnNo", BTN_INTERNAL520  );
    btn520_setting_internal->setCursor(Qt::PointingHandCursor);


    widget_box_internal_out = new QWidget();
    widget_box_internal_out->setObjectName("widget_box_internal_out");
    //widget_box_internal_out->setFixedHeight(125);
    //widget_box_internal_out->setFixedWidth(385);
    // widget_box_internal_out->setStyleSheet("#widget_box_internal_out { border:1px solid #707070; }");
    widget_box_internal_out->setLayout(vl_btn520_internal_out);
    //---------------------------------------------------------------------------
    QVBoxLayout *vl_btn520_setting_internal_out = new QVBoxLayout();
    vl_btn520_setting_internal_out->setContentsMargins(0,0,0,0);
    vl_btn520_setting_internal_out->setSpacing(0);
    vl_btn520_setting_internal_out->addWidget(btn520_setting_internal, 0, Qt::AlignTop);
    vl_btn520_setting_internal_out->addWidget(widget_box_internal_out, 10, Qt::AlignHCenter);
    //-------------------------------------------------------------------------------------
    widget_box_setting_internal_out = new QWidget();
    widget_box_setting_internal_out->setObjectName("widget_box_setting_internal_out");
    // widget_box_setting_internal_out->setContentsMargins(30,0,90,0);
    widget_box_setting_internal_out->setFixedHeight(225);
    widget_box_setting_internal_out->setFixedWidth(558);
    //  widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:1px solid #707070; }");
    widget_box_setting_internal_out->setLayout(vl_btn520_setting_internal_out);
    widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:1px solid #707070; }");
    //--------------------------------------------------------------------
    // OUTPUT 1ROW
    QHBoxLayout *hl_btns_output1 = new QHBoxLayout();
    hl_btns_output1->setContentsMargins(0,0,0,0);
    //hl_btns_output1->setSpacing(15);
    //   hl_btns_output1->addWidget(widget_box_usb);

    hl_btns_output1->addWidget(widget_box_dac_2010_out);
    hl_btns_output1->addWidget(widget_box_hdmi_out);
    // OUTPUT 2ROW

    QVBoxLayout *vl_btns_output2 = new QVBoxLayout();
    vl_btns_output2->setContentsMargins(30,0,30,0);
    vl_btns_output2->setSpacing(0);
    vl_btns_output2->setAlignment(Qt::AlignCenter);
    //  vl_btns_output2->addLayout(hl_btn520_setting_internal_out);
    vl_btns_output2->addWidget(widget_box_setting_internal_out);

  //unbalance and speaker
    //--------------------------------------------------------------------------------------------
    btn520_speaker_out = new BtnLabel(tr("SPEAKER OUT"), "btn520_speaker_out", ":/images/setting/inout_speak_gold_selected.png", ":/images/setting/inout_speak.png");
    btn520_speaker_out->setFixedSize(280,400);
    btn520_speaker_out->setProperty("btnNo", SPEAKER);
    QVBoxLayout *vl_btn520_balanced_out_l = new QVBoxLayout();
    vl_btn520_balanced_out_l->setContentsMargins(0,0,0,0);
    vl_btn520_balanced_out_l->setSpacing(0);
    vl_btn520_balanced_out_l->addWidget(btn520_speaker_out);

    btn520_setting_unbal = GSCommon::getUIBtnImg("btn520_setting_unbal", ":/images/fol_edit_ico1_2.png");
    tobj = "btn520_setting_unbal";
    if(get_InputMode520()==4 && outputvalue != 3)
        btn520_setting_unbal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

    btn520_setting_unbal->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn520_setting_unbal->setProperty("btnNo", BTN_UNBALANCE520);
    btn520_setting_unbal->setCursor(Qt::PointingHandCursor);


    QHBoxLayout *hl_btn520_setting_unbal_out = new QHBoxLayout();
    hl_btn520_setting_unbal_out->setContentsMargins(0,0,20,0);//addWidget(btn520_setting_hdmi, 0, Qt::AlignTop);
    hl_btn520_setting_unbal_out->setSpacing(0);
    hl_btn520_setting_unbal_out->addWidget(btn520_setting_unbal, 0, Qt::AlignTop|Qt::AlignLeft);
    //---------------------------------------------------------------------------------


    btn520_unbalance_out_l = new BtnLabel(tr("PREAMP L"), "btn520_unbal2_out_l", ":/images/setting/inout_unbalanced_gold_selected.png",  ":/images/setting/inout_unbalanced.png");
    btn520_unbalance_out_l->setFixedSize(179,150);
    //  btn520_unbalance_out_l->setProperty("internalMode", 0);
    btn520_unbalance_out_l->setProperty("btnNo", UNBALANCED_OUT_L);



    btn520_unbalance_out_r = new BtnLabel(tr("PREAMP R"), "btn520_unbal2_out_r", ":/images/setting/inout_unbalanced_gold_selected.png", ":/images/setting/inout_unbalanced.png");
    btn520_unbalance_out_r->setFixedSize(179,150);
    btn520_unbalance_out_r->setProperty("btnNo", UNBALANCED_OUT_R);


    //-------------------------------------------------------------------------------------------------
    btn520_setting_speaker = GSCommon::getUIBtnImg("btn520_setting_bal", ":/images/fol_edit_ico1_2.png");
    tobj = "btn520_setting_bal";
    if(get_InputMode520()==4 && outputvalue != 3)
        btn520_setting_speaker->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

    btn520_setting_speaker->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn520_setting_speaker->setProperty("btnNo", BTN_SPEAKER520);
    btn520_setting_speaker->setCursor(Qt::PointingHandCursor);


    QHBoxLayout *hl_btn520_setting_speaker_out = new QHBoxLayout();
    hl_btn520_setting_speaker_out->setContentsMargins(0,0,0,0);//addWidget(btn520_setting_hdmi, 0, Qt::AlignTop);
    hl_btn520_setting_speaker_out->setSpacing(0);
    hl_btn520_setting_speaker_out->addWidget(btn520_setting_speaker, 0, Qt::AlignTop|Qt::AlignLeft);
    //---------------------------------------------------------------------------------
    QHBoxLayout *hl_btn520_unbalance_out = new QHBoxLayout();
    hl_btn520_unbalance_out->setContentsMargins(40,0,40,0);
    hl_btn520_unbalance_out->setSpacing(50);
    hl_btn520_unbalance_out->addWidget(btn520_unbalance_out_r, 0, Qt::AlignHCenter);
    hl_btn520_unbalance_out->addWidget(btn520_unbalance_out_l, 0, Qt::AlignHCenter);
    //---------------------------------------------------------------------------------
    QHBoxLayout *hl_btn520_speaker_out = new QHBoxLayout();
    hl_btn520_speaker_out->setContentsMargins(0,0,0,0);
    hl_btn520_speaker_out->setSpacing(0);
    hl_btn520_speaker_out->addWidget(btn520_speaker_out, 0, Qt::AlignBottom);

    QVBoxLayout *vl_btn520_setting_unbalance_sum_out = new QVBoxLayout();
    vl_btn520_setting_unbalance_sum_out->setContentsMargins(0,0,0,20);
    vl_btn520_setting_unbalance_sum_out->setSpacing(0);
    vl_btn520_setting_unbalance_sum_out->addLayout(hl_btn520_setting_unbal_out);
    vl_btn520_setting_unbalance_sum_out->addLayout(hl_btn520_unbalance_out);

    QVBoxLayout *vl_btn520_setting_speaker_sum_out = new QVBoxLayout();
    vl_btn520_setting_speaker_sum_out->setContentsMargins(0,50,0,150);
    vl_btn520_setting_speaker_sum_out->setSpacing(0);
    //vl_btn520_setting_speaker_sum_out->addLayout(hl_btn520_setting_speaker_out);
    vl_btn520_setting_speaker_sum_out->addLayout(hl_btn520_speaker_out);

    //-------------------------------------------------------------------------------------
    widget_box_setting_unbalance_out = new QWidget();
    widget_box_setting_unbalance_out->setObjectName("widget_box_setting_unbalance_out");
    // widget_box_setting_unbalance_out->setContentsMargins(30,0,90,0);
    widget_box_setting_unbalance_out->setFixedHeight(265);
    widget_box_setting_unbalance_out->setFixedWidth(558);
    //  widget_box_setting_unbalance_out->setStyleSheet("#widget_box_setting_internal_out { border:1px solid #707070; }");
    widget_box_setting_unbalance_out->setLayout(vl_btn520_setting_unbalance_sum_out);
    widget_box_setting_unbalance_out->setStyleSheet("#widget_box_setting_unbalance_out { border:1px solid #707070; }");
    //-----------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    widget_box_setting_speaker_out = new QWidget();
    widget_box_setting_speaker_out->setObjectName("widget_box_setting_speaker_out");
    // widget_box_setting_speaker_out->setContentsMargins(30,0,90,0);
    //widget_box_setting_speaker_out->setFixedHeight(715);
    widget_box_setting_speaker_out->setFixedWidth(294);
    //  widget_box_setting_speaker_out->setStyleSheet("#widget_box_setting_internal_out { border:1px solid #707070; }");
    widget_box_setting_speaker_out->setLayout(vl_btn520_setting_speaker_sum_out);
    widget_box_setting_speaker_out->setStyleSheet("#widget_box_setting_speaker_out { border:1px solid #707070; }");
    //-----------------------------------------------------------------------

    QHBoxLayout *hl_btn520_setting_tunbal_out = new QHBoxLayout();
    hl_btn520_setting_tunbal_out->setContentsMargins(0,0,0,0);//box를 좌로 민다.
    hl_btn520_setting_tunbal_out->setSpacing(0);
    hl_btn520_setting_tunbal_out->addWidget(widget_box_setting_unbalance_out);
    //----------------------------------------------------------

    QHBoxLayout *hl_btn520_setting_tspeaker_out = new QHBoxLayout();
    hl_btn520_setting_tspeaker_out->setContentsMargins(0,0,0,0);//box를 좌로 민다.
    hl_btn520_setting_tspeaker_out->setSpacing(0);
    hl_btn520_setting_tspeaker_out->addWidget(widget_box_setting_speaker_out);
    //----------------------------------------------------------

    QVBoxLayout *vl_output = new QVBoxLayout();
    vl_output->setContentsMargins(0,0,0,0);
    vl_output->setSpacing(10);
    vl_output->addLayout(hl_btns_output1);
    vl_output->addLayout(vl_btns_output2);
    vl_output->addLayout(hl_btn520_setting_tunbal_out);
    // hl_output->addLayout(hl_btn520_setting_bal12_out);
    //-------------------input widget ---------------
    widget_box_local = new QWidget();
    widget_box_local->setObjectName("widget_box_local");
    widget_box_local->setFixedHeight(INPUT_UI_HEIGHT);
    //    widget_box_local->setFixedWidth(910);
    widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
    widget_box_local->setLayout(hl_btn520_input_Model_box);
    print_debug();//cheon0219
    widget_box_line = new QWidget();
    widget_box_line->setObjectName("widget_box_line");
    widget_box_line->setFixedHeight(INPUT_UI_HEIGHT+141);//361
    //widget_box_line->setFixedWidth(910);//c220708
    widget_box_line->setStyleSheet("#widget_box_line { border:1px solid #707070; }");
    print_debug();//cheon0219
    widget_box_line->setLayout(hl_btns_inputLine012);
    fl_input = new FlowLayout();
    fl_input->setContentsMargins(0,0,0,0);
    fl_input->setSpacingHV(16, 10);
    qDebug() << "width() : " << this->width();
    fl_input->heightForWidth(this->width());//+280);//920
    fl_input->addWidget(widget_box_local);
    fl_input->addWidget(widget_box_line);
    print_debug();//cheon0219
    QWidget *widget_flow = new QWidget();   // FlowLayout 을 QWidget으로 한번 더 안 감싸고 VBoxLayout에 넣으면 sizeHint.Height가 이상해서 원하는 UI 가 안나와서 QWidget으로 한번 더 감싸음.
    widget_flow->setObjectName("widget_flow");
    widget_flow->setContentsMargins(0,0,0,0);
    widget_flow->setLayout(fl_input);
    print_debug();//cheon0219
    widget_flow->setStyleSheet("#widget_flow { border-left:1px solid #303030; }");

    QHBoxLayout *hl_total = new QHBoxLayout();
    print_debug();//cheon0219
    hl_total->setContentsMargins(0,0,0,0);
    hl_total->setSpacing(30);
    hl_total->setAlignment(Qt::AlignTop);

    hl_total->addLayout(vl_output);//cheon12_io
    hl_total->addLayout(hl_btn520_setting_tspeaker_out);

    //------------------전체---------------------------------------------
    QVBoxLayout *vl_total = new QVBoxLayout();
    print_debug();//cheon0219
    vl_total->setContentsMargins(30,10,0,0);
    vl_total->setSpacing(30);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(this->getUIControlTitle(tr("INPUT")+" ("+global.device.getDeviceName()+")"));//c220521
    vl_total->addSpacing(25);
    //vl_total->addWidget(widget_flow, 0, Qt::AlignHCenter); // note working.. maybe FlowLayout bugs
    print_debug();//cheon0219
    vl_total->addWidget( widget_flow);
    vl_total->addSpacing(25);
    vl_total->addWidget(this->getUIControlTitle(tr("OUTPUT")));//cheon12_io
    vl_total->addSpacing(25);//cheon12_io


    //   vl_total->addLayout(vl_btns_output0);//cheon12_io
    vl_total->addLayout(hl_total);//cheon12_io
    //cheon12_io
    //vl_total->addLayout(hl_btn520_setting_tspeaker_out);


    print_debug();//cheon0219
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

    vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);

    this->setLayout(vl_scroll);

    //--------------------------------------------------------------------

    /*
    // 커넥션
    //input  : RS520                  - btn520_input_Model
               LINEIN                   - btn520_line_in_l
               OPTIN                   - btn520_opt_in
               ARCIN                   - btn520_HDMI_ARC_in
               USBIN                   - btn520_USB_in
               AES_EBUIN             - btn520_AES_EBU_in

    //output : HDMI                 - btn520_HDMI_out
               DAC_2010              - btn520_dac_2010_out

    //internal : COAX_OUT         - btn520_coax_out
                 I2S_RJ45_OUT       - btn520_i2s_rj45_out
                 AES_EBU_OUT       - btn520_AES_EBU_out
                 I2S_DVI_OUT         - btn520_i2s_dvi_out
                 OPT_OUT             - btn520_opt_out
                 BALANCED_OUT    - btn520_balanced_out_l
    */
    //if(get_InputMode520()==4){
    //inputMode btn_HDMI_ARC_in
    connect(btn520_input_Model, SIGNAL(clicked()), this, SLOT(slot_setInput520()));
    connect(btn520_line_in_l, SIGNAL(clicked()), this, SLOT(slot_setInput520()));
    connect(btn520_line_in_r, SIGNAL(clicked()), this, SLOT(slot_setInput520()));
    connect(btn520_opt_in, SIGNAL(clicked()), this, SLOT(slot_setInput520()));
    connect(btn520_HDMI_ARC_in, SIGNAL(clicked()), this, SLOT(slot_setInput520()));
    connect(btn520_USB_in, SIGNAL(clicked()), this, SLOT(slot_setInput520()));
    connect(btn520_AES_EBU_in, SIGNAL(clicked()), this, SLOT(slot_setInput520()));
    connect(btn520_coax_in, SIGNAL(clicked()), this, SLOT(slot_setInput520()));

    //}


    //if(get_InputMode520()!=4){

        //outputMode
        connect(btn520_HDMI_out, SIGNAL(clicked()), this, SLOT(slot_setOutput520()));
        connect(btn520_dac_2010_out, SIGNAL(clicked()), this, SLOT(slot_setOutput520()));

        //internalMode
        connect(btn520_coax_out, SIGNAL(clicked()), this, SLOT(slot_setInternal520()));
        connect(btn520_opt_out, SIGNAL(clicked()), this, SLOT(slot_setInternal520()));
        connect(btn520_speaker_out, SIGNAL(clicked()), this, SLOT(slot_setInternal520()));
        connect(btn520_unbalance_out_r, SIGNAL(clicked()), this, SLOT(slot_setInternal520()));
        connect(btn520_unbalance_out_l, SIGNAL(clicked()), this, SLOT(slot_setInternal520()));



        //outputMode_sub_setting
        connect(btn520_setting_usb_2010, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting520()));
        //connect(btn_setting_hdmi, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting520()));
        connect(btn520_setting_hdmi, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting520()));
        connect(btn520_setting_internal, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting520()));
        //connect(btn520_setting_speaker, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting520()));
        connect(btn520_setting_unbal, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting520()));


    //}
}
void SettingInputOutput::setUIControl150(QString deviceType){
    QString deviceTypeImgSelected = ":/images/setting/150/inout_150_in2_gold.png";
    QString deviceTypeImgUnSelected = ":/images/setting/150/inout_150_in2.png";
    // INPUT Body
    qDebug() << "setUIControl150-device_type : " << deviceType;
    btn150_input_Model = new BtnLabel(deviceType, "btn150_input_Model", deviceTypeImgSelected, deviceTypeImgUnSelected);
    btn150_input_Model->setFixedSize(330,270);
    btn150_input_Model->setProperty("btnNo", 0);
    QVBoxLayout *vl_btn150_input_Model = new QVBoxLayout();
    vl_btn150_input_Model->setContentsMargins(0,0,0,0);
    vl_btn150_input_Model->setSpacing(0);
    vl_btn150_input_Model->addWidget(btn150_input_Model, 0, Qt::AlignCenter);

    btn150_AES_EBU_in = new BtnLabel(tr("AES/EBU IN"), "btn150_AES_EBU_in", ":/images/setting/150/inout_AES_EBU_IN_gold_selected.png", ":/images/setting/150/inout_AES_EBU_IN.png");
    btn150_AES_EBU_in->setFixedSize(192,228);
    btn150_AES_EBU_in->setProperty("btnNo", 1);
    QVBoxLayout *vl_btn150_AES_EBU_in = new QVBoxLayout();
    vl_btn150_AES_EBU_in->setContentsMargins(0,0,0,0);
    vl_btn150_AES_EBU_in->setSpacing(0);
    vl_btn150_AES_EBU_in->addWidget(btn150_AES_EBU_in);

    btn150_USB_in = new BtnLabel(tr("USB IN"), "btn150_USB_in", ":/images/setting/150/inout_USB_IN_gold_selected.png", ":/images/setting/150/inout_USB_IN.png");
    btn150_USB_in->setFixedSize(116,105);
    btn150_USB_in->setProperty("btnNo", 2);
    QVBoxLayout *vl_btn150_USB_in = new QVBoxLayout();
    vl_btn150_USB_in->setContentsMargins(0,0,0,0);
    vl_btn150_USB_in->setSpacing(0);
    vl_btn150_USB_in->addWidget(btn150_USB_in);

    btn150_line_in_l = new BtnLabel(tr("LINE IN L"), "btn_line_in_l", ":/images/setting/150/inout_line_in_gold_selected.png", ":/images/setting/150/inout_line_in.png");
    btn150_line_in_l->setFixedSize(109,109);
    btn150_line_in_l->setProperty("btnNo", 3);
    QVBoxLayout *vl_btn150_line_in_l = new QVBoxLayout();
    vl_btn150_line_in_l->setContentsMargins(0,0,0,0);
    vl_btn150_line_in_l->setSpacing(0);
    vl_btn150_line_in_l->addWidget(btn150_line_in_l);

    btn150_line_in_r = new BtnLabel(tr("LINE IN R"), "btn150_line_in_r", ":/images/setting/150/inout_line_in_gold_selected.png", ":/images/setting/150/inout_line_in.png");
    btn150_line_in_r->setFixedSize(109,109);
    btn150_line_in_r->setProperty("btnNo", 4);
    QVBoxLayout *vl_btn150_line_in_r = new QVBoxLayout();
    vl_btn150_line_in_r->setContentsMargins(0,0,0,0);
    vl_btn150_line_in_r->setSpacing(0);
    vl_btn150_line_in_r->addWidget(btn150_line_in_r);

    //--------------------usb+line l+r hollizen------------------------------------------
    QHBoxLayout *hl_btns_inputLine1 = new QHBoxLayout();
    // hl_btns_inputLine1->setContentsMargins(0,40,0,40);
    hl_btns_inputLine1->setSpacing(37);
    hl_btns_inputLine1->setAlignment(Qt::AlignCenter);
    hl_btns_inputLine1->addLayout(vl_btn150_USB_in);
    hl_btns_inputLine1->addLayout(vl_btn150_line_in_l);
    hl_btns_inputLine1->addLayout(vl_btn150_line_in_r);

    btn150_HDMI_ARC_in = new BtnLabel(tr("ARC IN"), "btn150_HDMI_ARC_in", ":/images/setting/150/inout_HDMI_ARC_IN_gold_selected.png", ":/images/setting/150/inout_HDMI_ARC_IN.png");
    btn150_HDMI_ARC_in->setFixedSize(134,98);
    btn150_HDMI_ARC_in->setProperty("btnNo", 5);
    QVBoxLayout *vl_btn150_HDMI_ARC_in = new QVBoxLayout();
    vl_btn150_HDMI_ARC_in->setContentsMargins(0,0,0,0);
    vl_btn150_HDMI_ARC_in->setSpacing(0);
    vl_btn150_HDMI_ARC_in->addWidget(btn150_HDMI_ARC_in);

    btn150_opt_in = new BtnLabel(tr("OPT IN"), "btn150_opt_in", ":/images/setting/150/inout_opt_gold_selected.png", ":/images/setting/150/inout_opt.png");
    btn150_opt_in->setFixedSize(109,109);
    btn150_opt_in->setProperty("btnNo", 7);
    QVBoxLayout *vl_btn150_opt_in = new QVBoxLayout();
    vl_btn150_opt_in->setContentsMargins(0,0,0,0);
    vl_btn150_opt_in->setSpacing(0);
    vl_btn150_opt_in->addWidget(btn150_opt_in);

    btn150_coax_in = new BtnLabel(tr("COAX IN"), "btn150_coax_in", ":/images/setting/150/inout_coax_gold_selected.png", ":/images/setting/150/inout_coax.png");
    btn150_coax_in->setFixedSize(109,109);
    btn150_coax_in->setProperty("btnNo", 6);
    QVBoxLayout *vl_btn150_coax_in = new QVBoxLayout();
    vl_btn150_coax_in->setContentsMargins(0,0,0,0);
    vl_btn150_coax_in->setSpacing(0);
    vl_btn150_coax_in->addWidget(btn150_coax_in);

    //--------------------ARC+COAX+OPT hollizen------------------------------------------
    QHBoxLayout *hl_btns_inputLine2 = new QHBoxLayout();
    //  hl_btns_inputLine2->setContentsMargins(0,40,0,40);
    hl_btns_inputLine2->setSpacing(37);
    hl_btns_inputLine2->setAlignment(Qt::AlignCenter);
    hl_btns_inputLine2->addLayout(vl_btn150_HDMI_ARC_in);
    hl_btns_inputLine2->addLayout(vl_btn150_coax_in);
    hl_btns_inputLine2->addLayout(vl_btn150_opt_in);

    //--------------------USB+ARC vertival-----------------------------------------
    QVBoxLayout *hl_btns_inputLine12 = new QVBoxLayout();
    hl_btns_inputLine12->setContentsMargins(0,40,0,40);
    hl_btns_inputLine12->setSpacing(37);
    hl_btns_inputLine12->setAlignment(Qt::AlignCenter);
    hl_btns_inputLine12->addLayout(hl_btns_inputLine1);
    hl_btns_inputLine12->addLayout(hl_btns_inputLine2);

    //-------------------  input Model ----------------------------

    QHBoxLayout *hl_btn150_input_Model_box = new QHBoxLayout();
    hl_btn150_input_Model_box->setContentsMargins(290,0,290,0);

    hl_btn150_input_Model_box->setSpacing(10);
    hl_btn150_input_Model_box->setAlignment(Qt::AlignCenter);
    hl_btn150_input_Model_box->addLayout(vl_btn150_input_Model);
    //--------------------AES/EBU + inputline12 hollizen------------------------------------------
    QHBoxLayout *hl_btns_inputLine012 = new QHBoxLayout();
    hl_btns_inputLine012->setContentsMargins(125,10,125,10);
    hl_btns_inputLine012->setSpacing(37);
    hl_btns_inputLine012->setAlignment(Qt::AlignCenter);
    hl_btns_inputLine012->addLayout(vl_btn150_AES_EBU_in);
    hl_btns_inputLine012->addLayout(hl_btns_inputLine12);

    int fixedImgWidth =117;// 140;
    //--------------------------------------usb   -----------------------------
    btn150_dac_2010_out = new BtnLabel(tr("DAC 2010"), "btn150_dac_2010_out", ":/images/setting/150/inout_usb_out2_gold_selected.png", ":/images/setting/150/inout_usb_out2.png");
    btn150_dac_2010_out->setFixedSize(116, 151);//170);
    btn150_dac_2010_out->setProperty("btnNo", 51);
    btn150_dac_2010_out->setText("USB");
    btn150_setting_usb_2010 = GSCommon::getUIBtnImg("btn150_setting_usb_2010", ":/images/fol_edit_ico1_2.png");
    // btn_icon->setStyleSheet("#" + p_objName + " { background-color:rgba(0,0,0,0); background-image: url('" + p_url + "'); background-repeat:no-repeat; background-position:center center; } ");
    QString tobj = "btn150_setting_usb_2010";
    if(get_InputMode150()==4)
        btn150_setting_usb_2010->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
    btn150_setting_usb_2010->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn150_setting_usb_2010->setProperty("btnNo", BTN_USB150);
    btn150_setting_usb_2010->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_btn150_dac_2010_out = new QHBoxLayout();
    hl_btn150_dac_2010_out->setContentsMargins(0,0,90,0);
    hl_btn150_dac_2010_out->setSpacing(0);
    hl_btn150_dac_2010_out->addWidget(btn150_setting_usb_2010, 0, Qt::AlignTop);
    hl_btn150_dac_2010_out->addWidget(btn150_dac_2010_out, 30, Qt::AlignHCenter);

    widget_box_dac_2010_out = new QWidget();
    widget_box_dac_2010_out->setObjectName("widget_box_dac_2010_out");
    widget_box_dac_2010_out->setFixedHeight(225);
    widget_box_dac_2010_out->setFixedWidth(264);
    widget_box_dac_2010_out->setStyleSheet("#widget_box_dac_2010_out { border:1px solid #707070; }");
    widget_box_dac_2010_out->setLayout(hl_btn150_dac_2010_out);
    //------------------------------------------   hdmi     -------------------------------------------------------------------------
    btn150_HDMI_out = new BtnLabel(tr("HDMI"), "btn150_HDMI_out", ":/images/setting/150/inout_HDMI_ARC_IN_gold_selected.png", ":/images/setting/150/inout_HDMI_ARC_IN.png");
    btn150_HDMI_out->setFixedSize(fixedImgWidth+40, 98);//170);
    btn150_HDMI_out->setProperty("btnNo", 52);
    btn150_setting_hdmi = GSCommon::getUIBtnImg("btn150_setting_hdmi", ":/images/fol_edit_ico1_2.png");
    tobj = "btn150_setting_hdmi";
    if(get_InputMode150()==4)
        btn150_setting_hdmi->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

    btn150_setting_hdmi->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn150_setting_hdmi->setProperty("btnNo", BTN_HDMI150);
    btn150_setting_hdmi->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_btn150_hdmi_out = new QHBoxLayout();
    hl_btn150_hdmi_out->setContentsMargins(0,0,90,0);
    hl_btn150_hdmi_out->setSpacing(0);
    hl_btn150_hdmi_out->addWidget(btn150_setting_hdmi, 0, Qt::AlignTop);
    hl_btn150_hdmi_out->addWidget(btn150_HDMI_out, 30, Qt::AlignHCenter);

    widget_box_hdmi_out = new QWidget();
    widget_box_hdmi_out->setObjectName("widget_box_hdmi_out");
    widget_box_hdmi_out->setFixedHeight(225);
    widget_box_hdmi_out->setFixedWidth(264);
    widget_box_hdmi_out->setStyleSheet("#widget_box_hdmi_out { border:1px solid #707070; }");
    widget_box_hdmi_out->setLayout(hl_btn150_hdmi_out);
    //---------------------------------------------------------------------------------------------------------------------------

    btn150_coax_out = new BtnLabel(tr("COAX OUT"), "btn150_coax_out", ":/images/setting/150/inout_coax_gold_selected.png", ":/images/setting/150/inout_coax.png");
    btn150_coax_out->setFixedSize(110,110);
    // btn150_coax_out->setProperty("internalMode", 64);
    btn150_coax_out->setProperty("btnNo", 11);
    QVBoxLayout *vl_btn150_coax_out = new QVBoxLayout();
    vl_btn150_coax_out->setContentsMargins(0,0,0,0);
    vl_btn150_coax_out->setSpacing(0);
    vl_btn150_coax_out->addWidget(btn150_coax_out);

    btn150_opt_out = new BtnLabel(tr("OPT OUT"), "btn150_opt_out", ":/images/setting/150/inout_opt_gold_selected.png", ":/images/setting/150/inout_opt.png");
    btn150_opt_out->setFixedSize(110,110);
    // btn150_opt_out->setProperty("internalMode", 4);
    btn150_opt_out->setProperty("btnNo", 12);
    QVBoxLayout *vl_btn150_opt_out = new QVBoxLayout();
    vl_btn150_opt_out->setContentsMargins(0,0,0,0);
    vl_btn150_opt_out->setSpacing(0);
    vl_btn150_opt_out->addWidget(btn150_opt_out);


    btn150_i2s_dvi_out = new BtnLabel(tr("I2S OUT (DVI)"), "btn150_i2s_dvi_out", ":/images/setting/150/inout_i2s_dvi_gold_selected.png", ":/images/setting/150/inout_i2s_dvi.png");
    btn150_i2s_dvi_out->setFixedSize(258,117);
    //QString interValue = "01000000";
    //    btn150_i2s_dvi_out->setProperty("internalMode", 8); //0x40);
    btn150_i2s_dvi_out->setProperty("btnNo", 13);
    QVBoxLayout *vl_btn150_i2s_dvi_out = new QVBoxLayout();
    vl_btn150_i2s_dvi_out->setContentsMargins(0,0,0,0);
    vl_btn150_i2s_dvi_out->setSpacing(0);
    vl_btn150_i2s_dvi_out->addWidget(btn150_i2s_dvi_out);

    btn150_AES_EBU_out = new BtnLabel(tr("AES/EBU OUT"), "btn150_AES_EBU_out", ":/images/setting/150/inout_AES_EBU_IN_gold_selected.png", ":/images/setting/150/inout_AES_EBU_IN.png");
    btn150_AES_EBU_out->setFixedSize(192,228);
    // btn150_AES_EBU_out->setProperty("internalMode", 16);
    btn150_AES_EBU_out->setProperty("btnNo", 14);
    QVBoxLayout *vl_btn150_AES_EBU_out = new QVBoxLayout();
    vl_btn150_AES_EBU_out->setContentsMargins(0,0,0,0);
    vl_btn150_AES_EBU_out->setSpacing(0);
    vl_btn150_AES_EBU_out->addWidget(btn150_AES_EBU_out);

    btn150_i2s_rj45_out = new BtnLabel(tr("I2S OUT (RJ-45)"), "btn150_i2s_rj45_out", ":/images/setting/150/inout_i2s_out2_gold_selected.png", ":/images/setting/150/inout_i2s_out2.png");
    btn150_i2s_rj45_out->setFixedSize(184,247);
    //    btn150_i2s_rj45_out->setProperty("internalMode", 32);
    btn150_i2s_rj45_out->setProperty("btnNo", 15);
    QVBoxLayout *vl_btn150_i2s_rj45_out = new QVBoxLayout();
    vl_btn150_i2s_rj45_out->setContentsMargins(0,0,0,0);
    vl_btn150_i2s_rj45_out->setSpacing(0);
    vl_btn150_i2s_rj45_out->addWidget(btn150_i2s_rj45_out);
    //----------------------------------------------------------------------------------------
    QHBoxLayout *hl_btn150_internal123_out = new QHBoxLayout();
    hl_btn150_internal123_out->setContentsMargins(0,0,20,0);
    hl_btn150_internal123_out->setSpacing(0);
    hl_btn150_internal123_out->addWidget(btn150_coax_out, 0, Qt::AlignHCenter);
    hl_btn150_internal123_out->addWidget(btn150_opt_out, 0, Qt::AlignHCenter);
    hl_btn150_internal123_out->addWidget(btn150_i2s_dvi_out, 0, Qt::AlignHCenter);

    QHBoxLayout *hl_btn150_internal45_out = new QHBoxLayout();
    hl_btn150_internal45_out->setContentsMargins(0,0,0,0);
    hl_btn150_internal45_out->setSpacing(0);
    hl_btn150_internal45_out->addWidget(btn150_AES_EBU_out, 0, Qt::AlignHCenter);
    hl_btn150_internal45_out->addWidget(btn150_i2s_rj45_out, 0, Qt::AlignHCenter);
    //-----------------------------------------------------------------------------------
    QVBoxLayout *vl_btn150_internal_out = new QVBoxLayout();
    vl_btn150_internal_out->setContentsMargins(0,0,90,0);
    vl_btn150_internal_out->setSpacing(0);
    vl_btn150_internal_out->addLayout(hl_btn150_internal123_out);
    vl_btn150_internal_out->addLayout(hl_btn150_internal45_out);
    //--------------------------------------------------------------------------------

    btn150_setting_internal = GSCommon::getUIBtnImg("btn150_setting_internal", ":/images/fol_edit_ico1_2.png");
    tobj = "btn150_setting_internal";
    if(get_InputMode150()==4)
        btn150_setting_internal->setStyleSheet("#" +tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

    btn150_setting_internal->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn150_setting_internal->setProperty("btnNo", BTN_INTERNAL150  );
    btn150_setting_internal->setCursor(Qt::PointingHandCursor);


    widget_box_internal_out = new QWidget();
    widget_box_internal_out->setObjectName("widget_box_internal_out");
    widget_box_internal_out->setFixedHeight(465);
    widget_box_internal_out->setFixedWidth(585);
    // widget_box_internal_out->setStyleSheet("#widget_box_internal_out { border:1px solid #707070; }");
    widget_box_internal_out->setLayout(vl_btn150_internal_out);
    //---------------------------------------------------------------------------
    QHBoxLayout *hl_btn150_setting_internal_out = new QHBoxLayout();
    hl_btn150_setting_internal_out->setContentsMargins(0,0,90,0);
    hl_btn150_setting_internal_out->setSpacing(0);
    hl_btn150_setting_internal_out->addWidget(btn150_setting_internal, 0, Qt::AlignTop);
    hl_btn150_setting_internal_out->addWidget(widget_box_internal_out, 30, Qt::AlignHCenter);
    //-------------------------------------------------------------------------------------
    widget_box_setting_internal_out = new QWidget();
    widget_box_setting_internal_out->setObjectName("widget_box_setting_internal_out");
    // widget_box_setting_internal_out->setContentsMargins(30,0,90,0);
    widget_box_setting_internal_out->setFixedHeight(465);
    widget_box_setting_internal_out->setFixedWidth(615);
    //  widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:1px solid #707070; }");
    widget_box_setting_internal_out->setLayout(hl_btn150_setting_internal_out);
    widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:1px solid #707070; }");
    //--------------------------------------------------------------------
    // OUTPUT 1ROW
    QVBoxLayout *vl_btns_output1 = new QVBoxLayout();
    vl_btns_output1->setContentsMargins(0,0,0,0);
    vl_btns_output1->setSpacing(15);
    //   hl_btns_output1->addWidget(widget_box_usb);

    vl_btns_output1->addWidget(widget_box_dac_2010_out);
    vl_btns_output1->addWidget(widget_box_hdmi_out);
    // OUTPUT 2ROW

    QVBoxLayout *vl_btns_output2 = new QVBoxLayout();
    vl_btns_output2->setContentsMargins(30,0,30,0);
    vl_btns_output2->setSpacing(0);
    vl_btns_output2->setAlignment(Qt::AlignCenter);
    //  vl_btns_output2->addLayout(hl_btn150_setting_internal_out);
    vl_btns_output2->addWidget(widget_box_setting_internal_out);

    //--------------------------------------------------------------------------------------------
    btn150_balanced_out_l = new BtnLabel(tr("BALANCED L"), "btn150_balanced_out_l", ":/images/setting/150/inout_balanced_gold_selected.png", ":/images/setting/150/inout_balanced.png");
    btn150_balanced_out_l->setFixedSize(192,238);
    btn150_balanced_out_l->setProperty("btnNo", 16);
    QVBoxLayout *vl_btn150_balanced_out_l = new QVBoxLayout();
    vl_btn150_balanced_out_l->setContentsMargins(0,0,0,0);
    vl_btn150_balanced_out_l->setSpacing(0);
    vl_btn150_balanced_out_l->addWidget(btn150_balanced_out_l);


    btn150_balanced_out_r = new BtnLabel(tr("BALANCED R"), "btn150_balanced_out_r", ":/images/setting/150/inout_balanced_gold_selected.png", ":/images/setting/150/inout_balanced.png");
    btn150_balanced_out_r->setFixedSize(192,238);
    //  btn150_balanced_out_r->setProperty("internalMode", 2);
    btn150_balanced_out_r->setProperty("btnNo", 17);
    QVBoxLayout *vl_btn150_balanced_out_r = new QVBoxLayout();
    vl_btn150_balanced_out_r->setContentsMargins(0,0,0,0);
    vl_btn150_balanced_out_r->setSpacing(0);
    vl_btn150_balanced_out_r->addWidget(btn150_balanced_out_r);

    btn150_unbal2_out_l = new BtnLabel(tr("UNBALANCED L"), "btn150_unbal2_out_l", ":/images/setting/150/inout_unbalanced_gold_selected.png",  ":/images/setting/150/inout_unbal2.png");
    btn150_unbal2_out_l->setFixedSize(179,166);
    //  btn150_unbal2_out_l->setProperty("internalMode", 0);
    btn150_unbal2_out_l->setProperty("btnNo", 18);
    QVBoxLayout *vl_btn150_unbal2_out_l = new QVBoxLayout();
    vl_btn150_unbal2_out_l->setContentsMargins(0,0,0,0);
    vl_btn150_unbal2_out_l->setSpacing(0);
    vl_btn150_unbal2_out_l->addWidget(btn150_unbal2_out_l);


    btn150_unbal2_out_r = new BtnLabel(tr("UNBALANCED R"), "btn150_unbal2_out_r", ":/images/setting/150/inout_unbalanced_gold_selected.png", ":/images/setting/150/inout_unbal2.png");
    btn150_unbal2_out_r->setFixedSize(179,166);
    btn150_unbal2_out_r->setProperty("btnNo", 19);
    QVBoxLayout *vl_btn150_unbal2_out_r = new QVBoxLayout();
    vl_btn150_unbal2_out_r->setContentsMargins(0,0,0,0);
    vl_btn150_unbal2_out_r->setSpacing(0);
    vl_btn150_unbal2_out_r->addWidget(btn150_unbal2_out_r);
    //-------------------------------------------------------------------------------------------------
    btn150_setting_bal = GSCommon::getUIBtnImg("btn150_setting_bal", ":/images/fol_edit_ico1_2.png");
    tobj = "btn150_setting_bal";
    if(get_InputMode150()==4)
        btn150_setting_bal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

    btn150_setting_bal->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn150_setting_bal->setProperty("btnNo", BTN_BALAMP150);
    btn150_setting_bal->setCursor(Qt::PointingHandCursor);


    QHBoxLayout *hl_btn150_setting_bal_out = new QHBoxLayout();
    hl_btn150_setting_bal_out->setContentsMargins(0,0,20,0);//addWidget(btn150_setting_hdmi, 0, Qt::AlignTop);
    hl_btn150_setting_bal_out->setSpacing(0);
    hl_btn150_setting_bal_out->addWidget(btn150_setting_bal, 0, Qt::AlignTop);
    //---------------------------------------------------------------------------------
    QHBoxLayout *hl_btn150_bal12_out = new QHBoxLayout();
    hl_btn150_bal12_out->setContentsMargins(0,0,60,30);
    hl_btn150_bal12_out->setSpacing(0);
    hl_btn150_bal12_out->addWidget(btn150_balanced_out_l, 0, Qt::AlignBottom);
    hl_btn150_bal12_out->addWidget(btn150_balanced_out_r, 0, Qt::AlignBottom);

    hl_btn150_bal12_out->addWidget(btn150_unbal2_out_r, 0, Qt::AlignBottom);
    hl_btn150_bal12_out->addWidget(btn150_unbal2_out_l, 0, Qt::AlignBottom);
    //---------------------------------------------------------------------------------
    QHBoxLayout *hl_btn150_setting_bal12_out = new QHBoxLayout();
    hl_btn150_setting_bal12_out->setContentsMargins(0,0,0,0);
    hl_btn150_setting_bal12_out->setSpacing(0);
    hl_btn150_setting_bal12_out->addLayout(hl_btn150_setting_bal_out);
    hl_btn150_setting_bal12_out->addLayout(hl_btn150_bal12_out);
    //-------------------------------------------------------------------------------------
    widget_box_setting_bal12_out = new QWidget();
    widget_box_setting_bal12_out->setObjectName("widget_box_setting_bal12_out");
    // widget_box_setting_internal_out->setContentsMargins(30,0,90,0);
    widget_box_setting_bal12_out->setFixedHeight(302);
    widget_box_setting_bal12_out->setFixedWidth(910);
    //  widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:1px solid #707070; }");
    widget_box_setting_bal12_out->setLayout(hl_btn150_setting_bal12_out);
    widget_box_setting_bal12_out->setStyleSheet("#widget_box_setting_internal_out { border:1px solid #707070; }");
    //-----------------------------------------------------------------------
    QHBoxLayout *hl_btn150_setting_tbal_out = new QHBoxLayout();
    hl_btn150_setting_tbal_out->setContentsMargins(0,0,33,0);//box를 좌로 민다.
    hl_btn150_setting_tbal_out->setSpacing(0);
    hl_btn150_setting_tbal_out->addWidget(widget_box_setting_bal12_out);
    //----------------------------------------------------------
    QHBoxLayout *hl_output = new QHBoxLayout();
    hl_output->setContentsMargins(33,0,33,0);
    hl_output->setSpacing(0);
    hl_output->addLayout(vl_btns_output1);
    hl_output->addLayout(vl_btns_output2);
    // hl_output->addLayout(hl_btn150_setting_bal12_out);
    //-------------------input widget ---------------
    widget_box_local = new QWidget();
    widget_box_local->setObjectName("widget_box_local");
    widget_box_local->setFixedHeight(INPUT_UI_HEIGHT);
    //    widget_box_local->setFixedWidth(910);
    widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
    widget_box_local->setLayout(hl_btn150_input_Model_box);
    print_debug();//cheon0219
    widget_box_line = new QWidget();
    widget_box_line->setObjectName("widget_box_line");
    widget_box_line->setFixedHeight(INPUT_UI_HEIGHT+141);//361
    //widget_box_line->setFixedWidth(910);//c220708
    widget_box_line->setStyleSheet("#widget_box_line { border:1px solid #707070; }");
    print_debug();//cheon0219
    widget_box_line->setLayout(hl_btns_inputLine012);
    fl_input = new FlowLayout();
    fl_input->setContentsMargins(0,0,0,0);
    fl_input->setSpacingHV(16, 10);
    qDebug() << "width() : " << this->width();
    fl_input->heightForWidth(this->width());//+280);//920
    fl_input->addWidget(widget_box_local);
    fl_input->addWidget(widget_box_line);
    print_debug();//cheon0219
    QWidget *widget_flow = new QWidget();   // FlowLayout 을 QWidget으로 한번 더 안 감싸고 VBoxLayout에 넣으면 sizeHint.Height가 이상해서 원하는 UI 가 안나와서 QWidget으로 한번 더 감싸음.
    widget_flow->setObjectName("widget_flow");
    widget_flow->setContentsMargins(0,0,0,0);
    widget_flow->setLayout(fl_input);
    print_debug();//cheon0219
    widget_flow->setStyleSheet("#widget_flow { border-left:1px solid #303030; }");

    //------------------전체---------------------------------------------
    QVBoxLayout *vl_total = new QVBoxLayout();
    print_debug();//cheon0219
    vl_total->setContentsMargins(30,10,0,0);
    vl_total->setSpacing(30);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(this->getUIControlTitle(tr("INPUT")+" ("+global.device.getDeviceName()+")"));//c220521
    vl_total->addSpacing(25);
    //vl_total->addWidget(widget_flow, 0, Qt::AlignHCenter); // note working.. maybe FlowLayout bugs
    print_debug();//cheon0219
    vl_total->addWidget( widget_flow);
    vl_total->addSpacing(25);
    vl_total->addWidget(this->getUIControlTitle(tr("OUTPUT")));//cheon12_io
    vl_total->addSpacing(25);//cheon12_io


    //   vl_total->addLayout(vl_btns_output0);//cheon12_io
    vl_total->addLayout(hl_output);//cheon12_io
    vl_total->addLayout(hl_btn150_setting_tbal_out);//cheon12_io
    print_debug();//cheon0219
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

    vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);

    this->setLayout(vl_scroll);

    //--------------------------------------------------------------------

    /*
    // 커넥션
    //input  : RS150                  - btn150_input_Model
               LINEIN                   - btn150_line_in_l
               OPTIN                   - btn150_opt_in
               ARCIN                   - btn150_HDMI_ARC_in
               USBIN                   - btn150_USB_in
               AES_EBUIN             - btn150_AES_EBU_in

    //output : HDMI                 - btn150_HDMI_out
               DAC_2010              - btn150_dac_2010_out

    //internal : COAX_OUT         - btn150_coax_out
                 I2S_RJ45_OUT       - btn150_i2s_rj45_out
                 AES_EBU_OUT       - btn150_AES_EBU_out
                 I2S_DVI_OUT         - btn150_i2s_dvi_out
                 OPT_OUT             - btn150_opt_out
                 BALANCED_OUT    - btn150_balanced_out_l
    */
    //if(get_InputMode150()==4){
    //inputMode btn_HDMI_ARC_in
    connect(btn150_input_Model, SIGNAL(clicked()), this, SLOT(slot_setInput150()));
    connect(btn150_line_in_l, SIGNAL(clicked()), this, SLOT(slot_setInput150()));
    connect(btn150_line_in_r, SIGNAL(clicked()), this, SLOT(slot_setInput150()));
    connect(btn150_opt_in, SIGNAL(clicked()), this, SLOT(slot_setInput150()));
    connect(btn150_HDMI_ARC_in, SIGNAL(clicked()), this, SLOT(slot_setInput150()));
    connect(btn150_USB_in, SIGNAL(clicked()), this, SLOT(slot_setInput150()));
    connect(btn150_AES_EBU_in, SIGNAL(clicked()), this, SLOT(slot_setInput150()));
    connect(btn150_coax_in, SIGNAL(clicked()), this, SLOT(slot_setInput150()));

    //}


    if(get_InputMode150()!=4){

        //outputMode
        connect(btn150_HDMI_out, SIGNAL(clicked()), this, SLOT(slot_setOutput150()));
        connect(btn150_dac_2010_out, SIGNAL(clicked()), this, SLOT(slot_setOutput150()));

        //internalMode
        connect(btn150_coax_out, SIGNAL(clicked()), this, SLOT(slot_setInternal150()));
        connect(btn150_i2s_rj45_out, SIGNAL(clicked()), this, SLOT(slot_setInternal150()));
        connect(btn150_AES_EBU_out, SIGNAL(clicked()), this, SLOT(slot_setInternal150()));
        connect(btn150_i2s_dvi_out, SIGNAL(clicked()), this, SLOT(slot_setInternal150()));
        connect(btn150_opt_out, SIGNAL(clicked()), this, SLOT(slot_setInternal150()));
        connect(btn150_balanced_out_l, SIGNAL(clicked()), this, SLOT(slot_setInternal150()));
        connect(btn150_balanced_out_r, SIGNAL(clicked()), this, SLOT(slot_setInternal150()));
        connect(btn150_unbal2_out_l, SIGNAL(clicked()), this, SLOT(slot_setInternal150()));
        connect(btn150_unbal2_out_r, SIGNAL(clicked()), this, SLOT(slot_setInternal150()));



        //outputMode_sub_setting
        connect(btn150_setting_usb_2010, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting150()));
        //connect(btn_setting_hdmi, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting150()));
        connect(btn150_setting_hdmi, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting150()));
        connect(btn150_setting_internal, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting150()));
        connect(btn150_setting_bal, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting150()));
    }
}



void SettingInputOutput::setUIControl250(QString deviceType){
    QString deviceTypeImgSelected = ":/images/setting/inout_201_in2_gold_selected.png";
    QString deviceTypeImgUnSelected = ":/images/setting/inout_201_in2.png";
    // INPUT Body
    qDebug() << "setUIControl150-device_type : " << deviceType;
    btn150_input_Model = new BtnLabel(deviceType, "btn150_input_Model", deviceTypeImgSelected, deviceTypeImgUnSelected);
    btn150_input_Model->setFixedSize(330,270);
    btn150_input_Model->setProperty("btnNo", 0);
    QVBoxLayout *vl_btn150_input_Model = new QVBoxLayout();
    vl_btn150_input_Model->setContentsMargins(0,0,0,0);
    vl_btn150_input_Model->setSpacing(0);
    vl_btn150_input_Model->addWidget(btn150_input_Model, 0, Qt::AlignCenter);

//print_debug();
    btn150_USB_in = new BtnLabel(tr("USB IN"), "btn150_USB_in", ":/images/setting/150/inout_USB_IN_gold_selected.png", ":/images/setting/150/inout_USB_IN_gray.png");
    btn150_USB_in->setFixedSize(116,105);
    btn150_USB_in->setProperty("btnNo", 2);
    QVBoxLayout *vl_btn150_USB_in = new QVBoxLayout();
    vl_btn150_USB_in->setContentsMargins(0,0,0,0);
    vl_btn150_USB_in->setSpacing(0);
    vl_btn150_USB_in->addWidget(btn150_USB_in);

    btn150_line_in_l = new BtnLabel(tr("ANALOG IN L"), "btn_line_in_l", ":/images/setting/150/inout_line_in_gold_selected.png", ":/images/setting/150/inout_line_in_gray.png");
    btn150_line_in_l->setFixedSize(120,109);
    btn150_line_in_l->setProperty("btnNo", 3);
    QVBoxLayout *vl_btn150_line_in_l = new QVBoxLayout();
    vl_btn150_line_in_l->setContentsMargins(0,0,0,0);
    vl_btn150_line_in_l->setSpacing(0);
    vl_btn150_line_in_l->addWidget(btn150_line_in_l);

    btn150_line_in_r = new BtnLabel(tr("ANALOG IN R"), "btn150_line_in_r", ":/images/setting/150/inout_line_in_gold_selected.png", ":/images/setting/150/inout_line_in_gray.png");
    btn150_line_in_r->setFixedSize(120,109);
    btn150_line_in_r->setProperty("btnNo", 4);
    QVBoxLayout *vl_btn150_line_in_r = new QVBoxLayout();
    vl_btn150_line_in_r->setContentsMargins(0,0,0,0);
    vl_btn150_line_in_r->setSpacing(0);
    vl_btn150_line_in_r->addWidget(btn150_line_in_r);
//print_debug();

    btn150_opt_in = new BtnLabel(tr("OPT IN"), "btn150_opt_in", ":/images/setting/150/inout_opt_gold_selected.png", ":/images/setting/150/inout_opt_gray.png");
    btn150_opt_in->setFixedSize(109,109);
    btn150_opt_in->setProperty("btnNo", 7);
    QVBoxLayout *vl_btn150_opt_in = new QVBoxLayout();
    vl_btn150_opt_in->setContentsMargins(0,0,0,0);
    vl_btn150_opt_in->setSpacing(0);
    vl_btn150_opt_in->addWidget(btn150_opt_in);

    btn150_coax_in = new BtnLabel(tr("COAX IN"), "btn150_coax_in", ":/images/setting/150/inout_coax_gold_selected.png", ":/images/setting/150/inout_coax_gray.png");
    btn150_coax_in->setFixedSize(109,109);
    btn150_coax_in->setProperty("btnNo", 6);
    QVBoxLayout *vl_btn150_coax_in = new QVBoxLayout();
    vl_btn150_coax_in->setContentsMargins(0,0,0,0);
    vl_btn150_coax_in->setSpacing(0);
    vl_btn150_coax_in->addWidget(btn150_coax_in);
//print_debug();
    //--------------------COAX+OPT hollizen------------------------------------------
    QVBoxLayout *vl_btns_inputLine1 = new QVBoxLayout();
    //  hl_btns_inputLine2->setContentsMargins(0,40,0,40);
    vl_btns_inputLine1->setSpacing(37);
    vl_btns_inputLine1->setAlignment(Qt::AlignCenter);
   // hl_btns_inputLine2->addLayout(vl_btn150_HDMI_ARC_in);
    vl_btns_inputLine1->addWidget(btn150_opt_in);
    vl_btns_inputLine1->addWidget(btn150_coax_in);
    //--------------------usb+line l+r hollizen------------------------------------------
        QVBoxLayout *vl_btns_inputLine2 = new QVBoxLayout();
       // hl_btns_inputLine1->setContentsMargins(0,40,0,40);
        vl_btns_inputLine2->setSpacing(37);
        vl_btns_inputLine2->setAlignment(Qt::AlignCenter);
     //   hl_btns_inputLine1->addLayout(vl_btn150_USB_in);
        vl_btns_inputLine2->addWidget(btn150_line_in_l);
        vl_btns_inputLine2->addWidget(btn150_line_in_r);
    //--------------------USB+ARC vertival-----------------------------------------
    QHBoxLayout *hl_btns_inputLine12usb = new QHBoxLayout();
    hl_btns_inputLine12usb->setContentsMargins(0,40,0,40);
    hl_btns_inputLine12usb->setSpacing(37);
    hl_btns_inputLine12usb->setAlignment(Qt::AlignCenter);
    hl_btns_inputLine12usb->addWidget(btn150_coax_in);
    hl_btns_inputLine12usb->addWidget(btn150_opt_in);
    hl_btns_inputLine12usb->addWidget(btn150_line_in_l);
    hl_btns_inputLine12usb->addWidget(btn150_line_in_r);
    hl_btns_inputLine12usb->addWidget(btn150_USB_in);

    //-------------------  input Model ----------------------------

    QHBoxLayout *hl_btn150_input_Model_box = new QHBoxLayout();
    hl_btn150_input_Model_box->setContentsMargins(290,0,290,0);

    hl_btn150_input_Model_box->setSpacing(10);
    hl_btn150_input_Model_box->setAlignment(Qt::AlignCenter);
    hl_btn150_input_Model_box->addLayout(vl_btn150_input_Model);
    //--------------------AES/EBU + inputline12 hollizen------------------------------------------
    QPushButton *btn150_setting_line = GSCommon::getUIBtnImg("btn150_setting_hdmi", ":/images/fol_edit_ico1_2.png");
    btn150_setting_line->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn150_setting_line->setProperty("btnNo", BTN_HDMI150);
    btn150_setting_line->setCursor(Qt::PointingHandCursor);

    QWidget *widget_box_tmp = new QWidget();
    widget_box_tmp->setObjectName("widget_box_local");
    widget_box_tmp->setFixedHeight(INPUT_UI_HEIGHT-20);
    widget_box_tmp->setLayout(hl_btns_inputLine12usb);

    //hl_btn150_hdmi_out->addWidget(btn150_setting_hdmi, 0, Qt::AlignTop);
    //hl_btn150_hdmi_out->addWidget(btn150_HDMI_out, 30, Qt::AlignHCenter);
    QHBoxLayout *hl_btns_inputLine012 = new QHBoxLayout();
    hl_btns_inputLine012->setContentsMargins(0,10,90,10);//c220708
    hl_btns_inputLine012->setSpacing(0);
    hl_btns_inputLine012->setAlignment(Qt::AlignCenter);
    hl_btns_inputLine012->addWidget(btn150_setting_line, 0, Qt::AlignTop);
    hl_btns_inputLine012->addWidget(widget_box_tmp, 30, Qt::AlignHCenter);

print_debug(); qDebug() << "get_InputMode250():" << get_InputMode150();

    int fixedImgWidth =117;// 140;
    //--------------------------------------usb   -----------------------------
    btn150_dac_2010_out = new BtnLabel(tr("DAC 2010"), "btn150_dac_2010_out", ":/images/setting/150/inout_usb_out2_gold_selected.png", ":/images/setting/150/inout_usb_out2.png");
    btn150_dac_2010_out->setFixedSize(116, 151);//170);
    btn150_dac_2010_out->setProperty("btnNo", 51);
    btn150_dac_2010_out->setText("USB");
    btn150_setting_usb_2010 = GSCommon::getUIBtnImg("btn150_setting_usb_2010", ":/images/fol_edit_ico1_2.png");
    QString tobj = "btn150_setting_usb_2010";
    if(get_InputMode250()==4)
        btn150_setting_usb_2010->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

    btn150_setting_usb_2010->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn150_setting_usb_2010->setProperty("btnNo", BTN_USB150);
    btn150_setting_usb_2010->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_btn150_dac_2010_out = new QHBoxLayout();
    hl_btn150_dac_2010_out->setContentsMargins(0,0,90,0);
    hl_btn150_dac_2010_out->setSpacing(0);
    hl_btn150_dac_2010_out->addWidget(btn150_setting_usb_2010, 0, Qt::AlignTop);
    hl_btn150_dac_2010_out->addWidget(btn150_dac_2010_out, 30, Qt::AlignHCenter);

    widget_box_dac_2010_out = new QWidget();
    widget_box_dac_2010_out->setObjectName("widget_box_dac_2010_out");
    widget_box_dac_2010_out->setFixedHeight(190);
    widget_box_dac_2010_out->setFixedWidth(298);
    widget_box_dac_2010_out->setStyleSheet("#widget_box_dac_2010_out { border:1px solid #707070; }");
    widget_box_dac_2010_out->setLayout(hl_btn150_dac_2010_out);
    //------------------------------------------   hdmi     -------------------------------------------------------------------------
    btn150_HDMI_out = new BtnLabel(tr("HDMI"), "btn150_HDMI_out", ":/images/setting/150/inout_HDMI_ARC_IN_gold_selected.png", ":/images/setting/150/inout_HDMI_ARC_IN.png");
    btn150_HDMI_out->setFixedSize(fixedImgWidth+40, 98);//170);
    btn150_HDMI_out->setProperty("btnNo", 52);
    btn150_setting_hdmi = GSCommon::getUIBtnImg("btn150_setting_hdmi", ":/images/fol_edit_ico1_2.png");
    tobj = "btn150_setting_hdmi";
    if(get_InputMode250()==4)
        btn150_setting_hdmi->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

    btn150_setting_hdmi->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn150_setting_hdmi->setProperty("btnNo", BTN_HDMI150);
    btn150_setting_hdmi->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_btn150_hdmi_out = new QHBoxLayout();
    hl_btn150_hdmi_out->setContentsMargins(0,0,90,0);
    hl_btn150_hdmi_out->setSpacing(0);
    hl_btn150_hdmi_out->addWidget(btn150_setting_hdmi, 0, Qt::AlignTop);
    hl_btn150_hdmi_out->addWidget(btn150_HDMI_out, 30, Qt::AlignHCenter);
//print_debug();
    widget_box_hdmi_out = new QWidget();
    widget_box_hdmi_out->setObjectName("widget_box_hdmi_out");
    widget_box_hdmi_out->setFixedHeight(190);
    widget_box_hdmi_out->setFixedWidth(298);
    widget_box_hdmi_out->setStyleSheet("#widget_box_hdmi_out { border:1px solid #707070; }");
    widget_box_hdmi_out->setLayout(hl_btn150_hdmi_out);
    //---------------------------------------------------------------------------------------------------------------------------

    btn150_coax_out = new BtnLabel(tr("COAX OUT"), "btn150_coax_out", ":/images/setting/150/inout_coax_gold_selected.png", ":/images/setting/150/inout_coax_gray.png");
    btn150_coax_out->setFixedSize(110,110);
   // btn150_coax_out->setProperty("internalMode", 64);
    btn150_coax_out->setProperty("btnNo", 11);

    QVBoxLayout *vl_btn150_coax_out = new QVBoxLayout();
    vl_btn150_coax_out->setContentsMargins(40,0,0,0);
    vl_btn150_coax_out->setSpacing(0);
    vl_btn150_coax_out->addWidget(btn150_coax_out);

    btn150_opt_out = new BtnLabel(tr("OPT OUT"), "btn150_opt_out", ":/images/setting/150/inout_opt_gold_selected.png", ":/images/setting/150/inout_opt_gray.png");
    btn150_opt_out->setFixedSize(110,110);
   // btn150_opt_out->setProperty("internalMode", 4);
    btn150_opt_out->setProperty("btnNo", 12);

    QVBoxLayout *vl_btn150_opt_out = new QVBoxLayout();
    vl_btn150_opt_out->setContentsMargins(40,0,0,0);
    vl_btn150_opt_out->setSpacing(0);
    vl_btn150_opt_out->addWidget(btn150_opt_out);

//print_debug();
//----------------------------------------------------------------------------------------
    QVBoxLayout *vl_btn150_internal123_out = new QVBoxLayout();
    vl_btn150_internal123_out->setContentsMargins(70,0,20,0);
    vl_btn150_internal123_out->setSpacing(80);
    vl_btn150_internal123_out->addWidget(btn150_coax_out, 0, Qt::AlignTop);
    vl_btn150_internal123_out->addWidget(btn150_opt_out, 0, Qt::AlignTop);
    //hl_btn150_internal123_out->addWidget(btn150_i2s_dvi_out, 0, Qt::AlignHCenter);

    QHBoxLayout *hl_btn150_internal45_out = new QHBoxLayout();
    hl_btn150_internal45_out->setContentsMargins(0,0,0,0);
    hl_btn150_internal45_out->setSpacing(0);
   // hl_btn150_internal45_out->addWidget(btn150_AES_EBU_out, 0, Qt::AlignHCenter);
   // hl_btn150_internal45_out->addWidget(btn150_i2s_rj45_out, 0, Qt::AlignHCenter);
//-----------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

    btn150_setting_internal = GSCommon::getUIBtnImg("btn150_setting_internal", ":/images/fol_edit_ico1_2.png");
    tobj = "btn150_setting_internal";
    if(get_InputMode250()==4)
        btn150_setting_internal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

    btn150_setting_internal->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn150_setting_internal->setProperty("btnNo", BTN_INTERNAL150  );
    btn150_setting_internal->setCursor(Qt::PointingHandCursor);

//print_debug();
/*
    widget_box_internal_out = new QWidget();
    widget_box_internal_out->setObjectName("widget_box_internal_out");
    widget_box_internal_out->setFixedHeight(225);
    widget_box_internal_out->setFixedWidth(415);
   // widget_box_internal_out->setStyleSheet("#widget_box_internal_out { border:1px solid #707070; }");
    widget_box_internal_out->setLayout(vl_btn150_internal123_out);
    */
//---------------------------------------------------------------------------
    QVBoxLayout *vl_btn150_setting_internal_out = new QVBoxLayout();
    vl_btn150_setting_internal_out->setContentsMargins(0,0,10,70);

    vl_btn150_setting_internal_out->addWidget(btn150_setting_internal, 0, Qt::AlignTop);
    vl_btn150_setting_internal_out->addWidget(btn150_coax_out, 0, Qt::AlignHCenter);
    vl_btn150_setting_internal_out->setSpacing(50);
    vl_btn150_setting_internal_out->addWidget(btn150_opt_out, 0, Qt::AlignHCenter);
//-------------------------------------------------------------------------------------
    widget_box_setting_internal_out = new QWidget();
    widget_box_setting_internal_out->setObjectName("widget_box_setting_internal_out");
   // widget_box_setting_internal_out->setContentsMargins(30,0,90,0);
    widget_box_setting_internal_out->setFixedHeight(446);//465
    widget_box_setting_internal_out->setFixedWidth(283);
    widget_box_setting_internal_out->setLayout(vl_btn150_setting_internal_out);
    widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:1px solid #707070; }");
    //--------------------------------------------------------------------
    // OUTPUT 1ROW
    QHBoxLayout *hl_btns_output1 = new QHBoxLayout();
    hl_btns_output1->setContentsMargins(0,0,0,0);
    hl_btns_output1->setSpacing(15);
 //   hl_btns_output1->addWidget(widget_box_usb);

    hl_btns_output1->addWidget(widget_box_dac_2010_out);
    hl_btns_output1->addWidget(widget_box_hdmi_out);
    // OUTPUT 2ROW



    btn150_unbal2_out_l = new BtnLabel(tr("ANALOG OUT L"), "btn150_balanced_out_l", ":/images/setting/150/inout_unbalanced_gold_selected.png",  ":/images/setting/150/inout_unbal2.png");
    btn150_unbal2_out_l->setFixedSize(179,166);
  //  btn150_unbal2_out_l->setProperty("internalMode", 0);
    btn150_unbal2_out_l->setProperty("btnNo", 18);
    QVBoxLayout *vl_btn150_unbal2_out_l = new QVBoxLayout();
    vl_btn150_unbal2_out_l->setContentsMargins(0,0,0,0);
    vl_btn150_unbal2_out_l->setSpacing(0);
    vl_btn150_unbal2_out_l->addWidget(btn150_unbal2_out_l);

//print_debug();
    btn150_unbal2_out_r = new BtnLabel(tr("ANALOG OUT R"), "btn150_balanced_out_r", ":/images/setting/150/inout_unbalanced_gold_selected.png", ":/images/setting/150/inout_unbal2.png");
    btn150_unbal2_out_r->setFixedSize(179,166);
    btn150_unbal2_out_r->setProperty("btnNo", 19);
    QVBoxLayout *vl_btn150_unbal2_out_r = new QVBoxLayout();
    vl_btn150_unbal2_out_r->setContentsMargins(0,0,0,0);
    vl_btn150_unbal2_out_r->setSpacing(0);
    vl_btn150_unbal2_out_r->addWidget(btn150_unbal2_out_r);
//-------------------------------------------------------------------------------------------------
    btn150_setting_bal = GSCommon::getUIBtnImg("btn150_setting_bal", ":/images/fol_edit_ico1_2.png");
    tobj = "btn150_setting_bal";
    if(get_InputMode250()==4)
        btn150_setting_bal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

    btn150_setting_bal->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn150_setting_bal->setProperty("btnNo", BTN_BALAMP150);
    btn150_setting_bal->setCursor(Qt::PointingHandCursor);


    QHBoxLayout *hl_btn150_setting_bal_out = new QHBoxLayout();
    hl_btn150_setting_bal_out->setContentsMargins(0,0,20,0);//addWidget(btn150_setting_hdmi, 0, Qt::AlignTop);
    hl_btn150_setting_bal_out->setSpacing(0);
    hl_btn150_setting_bal_out->addWidget(btn150_setting_bal, 0, Qt::AlignTop);
    //---------------------------------------------------------------------------------
    QHBoxLayout *hl_btn150_bal12_out = new QHBoxLayout();
    hl_btn150_bal12_out->setContentsMargins(0,0,60,30);
    hl_btn150_bal12_out->setSpacing(40);
    //hl_btn150_bal12_out->addWidget(btn150_balanced_out_l, 0, Qt::AlignBottom);
    //hl_btn150_bal12_out->addWidget(btn150_balanced_out_r, 0, Qt::AlignBottom);
    hl_btn150_bal12_out->addWidget(btn150_unbal2_out_l, 0, Qt::AlignBottom);
    hl_btn150_bal12_out->addWidget(btn150_unbal2_out_r, 0, Qt::AlignBottom);
    //---------------------------------------------------------------------------------
    QHBoxLayout *hl_btn150_setting_bal12_out = new QHBoxLayout();
    hl_btn150_setting_bal12_out->setContentsMargins(0,0,30,0);
    hl_btn150_setting_bal12_out->setSpacing(40);
    hl_btn150_setting_bal12_out->addLayout(hl_btn150_setting_bal_out);
    hl_btn150_setting_bal12_out->addLayout(hl_btn150_bal12_out);
    //-------------------------------------------------------------------------------------
        widget_box_setting_bal12_out = new QWidget();
        widget_box_setting_bal12_out->setObjectName("widget_box_setting_bal12_out");
       // widget_box_setting_internal_out->setContentsMargins(30,0,90,0);
        widget_box_setting_bal12_out->setFixedHeight(241);
        widget_box_setting_bal12_out->setFixedWidth(611);
        widget_box_setting_bal12_out->setLayout(hl_btn150_setting_bal12_out);
        widget_box_setting_bal12_out->setStyleSheet("#widget_box_setting_bal12_out { border:1px solid #707070; }");
   //-----------------------------------------------------------------------
        QHBoxLayout *hl_btn150_setting_tbal_out = new QHBoxLayout();
        hl_btn150_setting_tbal_out->setContentsMargins(0,0,33,0);//box를 좌로 민다.
        hl_btn150_setting_tbal_out->setSpacing(0);
        hl_btn150_setting_tbal_out->addWidget(widget_box_setting_bal12_out);

        QHBoxLayout *hl_btns_output2 = new QHBoxLayout();
        hl_btns_output2->setContentsMargins(0,0,0,0);
        hl_btns_output2->setSpacing(15);
        hl_btns_output2->setAlignment(Qt::AlignCenter);
     //   vl_btns_output2->addLayout(hl_btn150_setting_internal_out);
      //  vl_btns_output2->addWidget(widget_box_setting_internal_out);
        hl_btns_output2->addWidget(widget_box_setting_bal12_out);
    //----------------------------------------------------------
    QVBoxLayout *vl_output = new QVBoxLayout();
    vl_output->setContentsMargins(0,0,0,0);
    vl_output->setSpacing(16);
    vl_output->addLayout(hl_btns_output1);
    vl_output->addLayout(hl_btns_output2);
   // hl_output->addLayout(hl_btn150_setting_bal12_out);
    QHBoxLayout *hl_btns_total_output = new QHBoxLayout();
    hl_btns_total_output->setContentsMargins(0,0,0,0);
    hl_btns_total_output->setSpacing(15);
    hl_btns_total_output->setAlignment(Qt::AlignCenter);
    hl_btns_total_output->addLayout(vl_output);
    hl_btns_total_output->addWidget(widget_box_setting_internal_out);
    //-------------------input widget ---------------
    widget_box_local = new QWidget();
    widget_box_local->setObjectName("widget_box_local");
    widget_box_local->setFixedHeight(INPUT_UI_HEIGHT);
//    widget_box_local->setFixedWidth(910);
    widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
    widget_box_local->setLayout(hl_btn150_input_Model_box);

  //--------------------------------------------------------


    widget_box_line = new QWidget();
    widget_box_line->setObjectName("widget_box_line");
    widget_box_line->setFixedHeight(INPUT_UI_HEIGHT);//361
  //  widget_box_line->setFixedWidth(910);//361
    widget_box_line->setStyleSheet("#widget_box_line { border:1px solid #707070; }");
    widget_box_line->setLayout(hl_btns_inputLine012);

    //------------------------------------------------------

    fl_input = new FlowLayout();
    fl_input->setContentsMargins(0,0,0,0);
    fl_input->setSpacingHV(16, 10);
    qDebug() << "width() : " << this->width();
    fl_input->heightForWidth(this->width());//+280);//920
    fl_input->addWidget(widget_box_local);
    fl_input->addWidget(widget_box_line);
    QWidget *widget_flow = new QWidget();   // FlowLayout 을 QWidget으로 한번 더 안 감싸고 VBoxLayout에 넣으면 sizeHint.Height가 이상해서 원하는 UI 가 안나와서 QWidget으로 한번 더 감싸음.
    widget_flow->setObjectName("widget_flow");
    widget_flow->setContentsMargins(0,0,0,0);
    widget_flow->setLayout(fl_input);
    widget_flow->setStyleSheet("#widget_flow { border-left:1px solid #303030; }");
//print_debug();
    //------------------전체---------------------------------------------
    QVBoxLayout *vl_total = new QVBoxLayout();
    print_debug();//cheon0219
    vl_total->setContentsMargins(30,10,0,0);
    vl_total->setSpacing(30);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(this->getUIControlTitle(tr("INPUT")+" ("+global.device.getDeviceName()+")"));//c220521
    vl_total->addSpacing(25);
    //vl_total->addWidget(widget_flow, 0, Qt::AlignHCenter); // note working.. maybe FlowLayout bugs
    vl_total->addWidget( widget_flow);
    vl_total->addSpacing(25);
    vl_total->addWidget(this->getUIControlTitle(tr("OUTPUT")));//cheon12_io
    vl_total->addSpacing(25);//cheon12_io

//print_debug();
     //   vl_total->addLayout(vl_btns_output0);//cheon12_io
    vl_total->addLayout(hl_btns_total_output);//cheon12_io
 //   vl_total->addLayout(hl_btn150_setting_tbal_out);//cheon12_io
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

    vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);

    this->setLayout(vl_scroll);
    //--------------------------------------------------------------------

/*
    // 커넥션
    //input  : RS150                  - btn150_input_Model
               LINEIN                   - btn150_line_in_l
               OPTIN                   - btn150_opt_in
               ARCIN                   - btn150_HDMI_ARC_in
               USBIN                   - btn150_USB_in
               AES_EBUIN             - btn150_AES_EBU_in

    //output : HDMI                 - btn150_HDMI_out
               DAC_2010              - btn150_dac_2010_out

    //internal : COAX_OUT         - btn150_coax_out
                 I2S_RJ45_OUT       - btn150_i2s_rj45_out
                 AES_EBU_OUT       - btn150_AES_EBU_out
                 I2S_DVI_OUT         - btn150_i2s_dvi_out
                 OPT_OUT             - btn150_opt_out
                 BALANCED_OUT    - btn150_balanced_out_l
    */
print_debug();
     //inputMode btn_HDMI_ARC_in
     connect(btn150_input_Model, SIGNAL(clicked()), this, SLOT(slot_setInput150()));
     connect(btn150_line_in_l, SIGNAL(clicked()), this, SLOT(slot_setInput150()));
     connect(btn150_line_in_r, SIGNAL(clicked()), this, SLOT(slot_setInput150()));
     connect(btn150_opt_in, SIGNAL(clicked()), this, SLOT(slot_setInput150()));
    // connect(btn150_HDMI_ARC_in, SIGNAL(clicked()), this, SLOT(slot_setInput150()));
     connect(btn150_USB_in, SIGNAL(clicked()), this, SLOT(slot_setInput250()));
    // connect(btn150_AES_EBU_in, SIGNAL(clicked()), this, SLOT(slot_setInput150()));
     connect(btn150_coax_in, SIGNAL(clicked()), this, SLOT(slot_setInput150()));

    connect(btn150_setting_line, SIGNAL(clicked()), this, SLOT(slot_setInputSetting250()));

     //internalMode
  if(get_InputMode150()!=4){

      //outputMode
     connect(btn150_HDMI_out, SIGNAL(clicked()), this, SLOT(slot_setOutput150()));
     connect(btn150_dac_2010_out, SIGNAL(clicked()), this, SLOT(slot_setOutput150()));

     connect(btn150_coax_out, SIGNAL(clicked()), this, SLOT(slot_setInternal250()));
   //  connect(btn150_i2s_rj45_out, SIGNAL(clicked()), this, SLOT(slot_setInternal250()));
   //  connect(btn150_AES_EBU_out, SIGNAL(clicked()), this, SLOT(slot_setInternal250()));
   //  connect(btn150_i2s_dvi_out, SIGNAL(clicked()), this, SLOT(slot_setInternal250()));
     connect(btn150_opt_out, SIGNAL(clicked()), this, SLOT(slot_setInternal250()));
   //  connect(btn150_balanced_out_l, SIGNAL(clicked()), this, SLOT(slot_setInternal250()));
   //  connect(btn150_balanced_out_r, SIGNAL(clicked()), this, SLOT(slot_setInternal250()));
     connect(btn150_unbal2_out_l, SIGNAL(clicked()), this, SLOT(slot_setInternal250()));
     connect(btn150_unbal2_out_r, SIGNAL(clicked()), this, SLOT(slot_setInternal250()));


    //outputMode_sub_setting
    connect(btn150_setting_usb_2010, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting250()));
    connect(btn150_setting_hdmi, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting250()));
    connect(btn150_setting_internal, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting250()));
    connect(btn150_setting_bal, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting250()));
  }
print_debug();
}

/**
 * @brief MusicFolder_Colud::showDlgOfSelectCloudType : 클라우드 타입 선택 다이얼로그 show
 */
void SettingInputOutput::showDlgOfOPT_setting(){

    Dialog::dialogsetting_opt *dlg_opt_setting = new Dialog::dialogsetting_opt(this);


   // dlg_opt_setting->set_selected_deviceName("Opt Setting");
    int result = dlg_opt_setting->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_opt_setting;
}

/**
 * @brief MusicFolder_Colud::showDlgOfSelectCloudType : 클라우드 타입 선택 다이얼로그 show
 */
void SettingInputOutput::showDlgOfINTERAL_setting150(){

    Dialog::dialogsetting_interoutput_150 *dlg_interoutput_setting = new Dialog::dialogsetting_interoutput_150(this);


   // dlg_opt_setting->set_selected_deviceName("Opt Setting");
    int result = dlg_interoutput_setting->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_interoutput_setting;
}
void SettingInputOutput::showDlgOfINTERAL_setting520(){

    Dialog::dialogsetting_interoutput_520 *dlg_interoutput_setting = new Dialog::dialogsetting_interoutput_520(this);


   // dlg_opt_setting->set_selected_deviceName("Opt Setting");
    int result = dlg_interoutput_setting->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_interoutput_setting;
}
/**
 * @brief MusicFolder_Colud::showDlgOfSelectCloudType : 클라우드 타입 선택 다이얼로그 show
 */
void SettingInputOutput::showDlgOfINTERAL_setting250(){

    Dialog::dialogsetting_interoutput_250 *dlg_interoutput_setting = new Dialog::dialogsetting_interoutput_250(this);


   // dlg_opt_setting->set_selected_deviceName("Opt Setting");
    int result = dlg_interoutput_setting->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_interoutput_setting;
}
//setting button :
//const int BTN_USB201  = 41;
//const int BTN_HDMI201  = 42;
//const int BTN_OPT201  = 43;
//const int BTN_PREAMP201  = 44;

void SettingInputOutput::slot_setOutputSetting150()
{
    QString deviceType = global.device.getDeviceType();

    print_debug();
    qDebug() << "Internal: sender()->property(btnNo).toInt()  = " << sender()->property("btnNo").toInt();
    int btnSenderNo = sender()->property("btnNo").toInt();

    if(inputvalue150 == 4) {
        ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));

        return;
    }

    switch(btnSenderNo){
    case BTN_HDMI150  ://
        if(inputvalue150 == 4) {
            ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));

            return;
        }
        //    ToastMsg::show(this, "", tr("click %1-slot_setOutputSetting150.").arg(BTN_HDMI150));
        qDebug() << tr("click %1-slot_setOutputSetting150.").arg(BTN_HDMI150);
        showDlgOfHDMI_setting150();
        break;
    case BTN_USB150://
        if(inputvalue150 == 4) {
            ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));

            return;
        }
        //   ToastMsg::show(this, "", tr("click %1-slot_setOutputSetting150.").arg(BTN_USB150));
        qDebug() << tr("click %1-slot_setOutputSetting150.").arg(BTN_USB150);
        showDlgOfUSB_setting150();//c220708
        break;
    case BTN_INTERNAL150://
        //   ToastMsg::show(this, "", tr("click %1-slot_setOutputSetting150.").arg(BTN_INTERNAL150));
        qDebug() << tr("click %1-slot_setOutputSetting150.").arg(BTN_INTERNAL150);
        showDlgOfINTERAL_setting150();
        break;
    case BTN_BALAMP150://
        if(deviceType == "RS150"){
            qDebug() << tr("click %1-slot_setOutputSetting150.").arg(BTN_BALAMP150);
            showDlgOfOUTPUT_setting150();
        }else if(deviceType == "RS150B"){
            qDebug() << tr("click %1-slot_setOutputSetting150B.").arg(BTN_BALAMP150);
            showDlgOfOUTPUT_setting150B();

        }
        break;
    }
}

void SettingInputOutput::slot_setOutputSetting520()
{
    QString deviceType = global.device.getDeviceType();

    print_debug();
    qDebug() << "inputvalue520=" << inputvalue520;
    qDebug() << "Internal: sender()->property(btnNo).toInt()  = " << sender()->property("btnNo").toInt();
    int btnSenderNo = sender()->property("btnNo").toInt();

    if(inputvalue520 == 4) {
        print_debug();
        qDebug() << "inputvalue520=" << inputvalue520;
        ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));

        return;
    }

    switch(btnSenderNo){
    case BTN_HDMI520  ://
        if(inputvalue520 == 4) {
            print_debug();
            qDebug() << "inputvalue520=" << inputvalue520;
            ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));

            return;
        }
        //    ToastMsg::show(this, "", tr("click %1-slot_setOutputSetting520.").arg(BTN_HDMI520));
        qDebug() << tr("click %1-slot_setOutputSetting520.").arg(BTN_HDMI520);
        showDlgOfHDMI_setting520();
        break;
    case BTN_USB520://
        if(inputvalue520 == 4) {
            print_debug();
            qDebug() << "inputvalue520=" << inputvalue520;
            ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));

            return;
        }
        //   ToastMsg::show(this, "", tr("click %1-slot_setOutputSetting520.").arg(BTN_USB520));
        qDebug() << tr("click %1-slot_setOutputSetting520.").arg(BTN_USB520);
        showDlgOfUSB_setting520();//c220708
        break;
    case BTN_INTERNAL520://
        //   ToastMsg::show(this, "", tr("click %1-slot_setOutputSetting520.").arg(BTN_INTERNAL520));
        qDebug() << tr("click %1-slot_setOutputSetting520.").arg(BTN_INTERNAL520);
        showDlgOfINTERAL_setting520();
        break;
    case BTN_UNBALANCE520://
            qDebug() << tr("click %1-slot_setOutputSetting520.").arg(BTN_UNBALANCE520);
            showDlgOfOUTPUT_setting520();
        break;
    case BTN_SPEAKER520://
            qDebug() << tr("click %1-slot_setOutputSetting520.").arg(BTN_SPEAKER520);
            showDlgOfOUTPUT_setting520();
        break;
    }
}

//setting button :
//const int BTN_USB201  = 41;
//const int BTN_HDMI201  = 42;
//const int BTN_OPT201  = 43;
//const int BTN_PREAMP201  = 44;

void SettingInputOutput::slot_setInputSetting250()
{
    print_debug();

    showDlgOfRX_setting250();


}



void SettingInputOutput::slot_setOutputSetting250()
{

    qDebug() << "Internal: sender()->property(btnNo).toInt()  = " << sender()->property("btnNo").toInt();
    int btnSenderNo = sender()->property("btnNo").toInt();

    if(inputvalue250 == 4) {

        ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));
        return;
    }

    switch(btnSenderNo){
    case BTN_HDMI150  ://
        if(inputvalue250 == 4) {
            ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));
            return;
        }
       // ToastMsg::show(this, "", tr("click %1-slot_setOutputSetting250.").arg(BTN_HDMI150));
        qDebug() << tr("click %1-slot_setOutputSetting250.").arg(BTN_HDMI150);
        showDlgOfHDMI_setting250();
        break;
    case BTN_USB150://
        if(inputvalue250 == 4) {
            ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));
            return;
        }
       // ToastMsg::show(this, "", tr("click %1-slot_setOutputSetting250.").arg(BTN_USB150));
        qDebug() << tr("click %1-slot_setOutputSetting250.").arg(BTN_USB150);
        showDlgOfUSB_setting250();
        break;
    case BTN_INTERNAL150://
       // ToastMsg::show(this, "", tr("click %1-slot_setOutputSetting250.").arg(BTN_INTERNAL150));
        qDebug() << tr("click %1-slot_setOutputSetting250.").arg(BTN_INTERNAL150);
        showDlgOfINTERAL_setting250();
        break;
    case BTN_BALAMP150://
       // ToastMsg::show(this, "", tr("click %1-slot_setOutputSetting250.").arg(BTN_BALAMP150));
        qDebug() << tr("click %1-slot_setOutputSetting250.").arg(BTN_BALAMP150);
        showDlgOfOUTPUT_setting250();
        break;

    }


}

/**
 * @brief MusicFolder_Colud::showDlgOfSelectCloudType : 클라우드 타입 선택 다이얼로그 show
 */
void SettingInputOutput::showDlgOfOUTPUT_setting150(){

    Dialog::dialogsetting_output_150 *dlg_output_setting_150 = new Dialog::dialogsetting_output_150(this);


   // dlg_opt_setting->set_selected_deviceName("Opt Setting");
    int result = dlg_output_setting_150->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_output_setting_150;
}
void SettingInputOutput::showDlgOfOUTPUT_setting520(){

    Dialog::dialogsetting_output_520 *dlg_output_setting_520 = new Dialog::dialogsetting_output_520(this);


   // dlg_opt_setting->set_selected_deviceName("Opt Setting");
    int result = dlg_output_setting_520->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_output_setting_520;
}
void SettingInputOutput::showDlgOfOUTPUT_setting150B(){

    Dialog::dialogsetting_output_150B *dlg_output_setting_150B = new Dialog::dialogsetting_output_150B(this);


   // dlg_opt_setting->set_selected_deviceName("Opt Setting");
    int result = dlg_output_setting_150B->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_output_setting_150B;
}

/**
 * @brief MusicFolder_Colud::showDlgOfSelectCloudType : 클라우드 타입 선택 다이얼로그 show
 */
void SettingInputOutput::showDlgOfOUTPUT_setting250(){

    Dialog::dialogsetting_output_250 *dlg_output_setting_250 = new Dialog::dialogsetting_output_250(this);


   // dlg_opt_setting->set_selected_deviceName("Opt Setting");
    int result = dlg_output_setting_250->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_output_setting_250;
}
void SettingInputOutput::setUsbOutput150(){
 //   ToastMsg::show(this, "", tr("Service is being prepared."));
//print_debug(); //cheon10-io
    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    QString usb_name;

    int output_data;
    print_debug(); //cheon10-io
    output_data = get_outputMode();
qDebug() << "output_data = " << output_data;
    if(output_data == 3){
        //print_debug(); //cheon10-io
        bool flag = showDlgOfDACMsg(tr("Internal Mode"), tr("Are you sure you want to disable current usb output mode?"));
        if(flag ) {

            tmp_json.insert("outputMode", "1");
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);

        }
    }else{
        //print_debug(); //cheon10-io
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_usb_status(int, QJsonObject)));
        network->request(HTTP_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("usb.dac.get"), tmp_json, true);
print_debug();
        bool flag = showDlgOfDACMsg(tr("Internal Mode"), tr("Are you sure you want to enable usb dac output mode?"));
        if(flag ) {
            QString tmp_selectType = get_UsbName();
            if(tmp_selectType==""){
                bool flag = showDlgOfDACMsg(tr("USB DAC"), tr("You didn't find the USB DAC."));
                if(flag ) {

                }
            }else{
                showDlgOfSelectUsbDac();
            }
        }else{

        }

    }
}

void SettingInputOutput::setUsbOutput520(){
 //   ToastMsg::show(this, "", tr("Service is being prepared."));
//print_debug(); //cheon10-io
    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    QString usb_name;

    int output_data;
    print_debug(); //cheon10-io
    output_data = get_outputMode();
qDebug() << "output_data = " << output_data;
    if(output_data == 3){
        //print_debug(); //cheon10-io
        bool flag = showDlgOfDACMsg(tr("Internal Mode"), tr("Are you sure you want to disable current usb output mode?"));
        if(flag ) {

            tmp_json.insert("outputMode", "1");
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);

        }
    }else{
        //print_debug(); //cheon10-io
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_usb_status(int, QJsonObject)));
        network->request(HTTP_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("usb.dac.get"), tmp_json, true);
print_debug();
        bool flag = showDlgOfDACMsg(tr("Internal Mode"), tr("Are you sure you want to enable usb dac output mode?"));
        if(flag ) {
            QString tmp_selectType = get_UsbName();
            if(tmp_selectType==""){
                bool flag = showDlgOfDACMsg(tr("USB DAC"), tr("You didn't find the USB DAC."));
                if(flag ) {

                }
            }else{
                showDlgOfSelectUsbDac();
            }
        }else{

        }

    }
}
void SettingInputOutput::select_setUsbOutput150(){
    setUsbOutput150();
}


void SettingInputOutput::showDlgOfUSB_setting150(){

    if(outputvalue != 3) {
        print_debug();
        qDebug() << "outputvalue=" << outputvalue;
        ToastMsg::show(this, "", tr("Please select a usb DAC"));
        return;
    }
    Dialog::dialogsetting_usbsetting *dlg_hdmi_setting_150 = new Dialog::dialogsetting_usbsetting(this);


   // dlg_hdmi_setting->set_selected_deviceName("HDMI Setting");
    int result = dlg_hdmi_setting_150->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_hdmi_setting_150;
}
void SettingInputOutput::showDlgOfUSB_setting520(){

    if(outputvalue != 3) {
        print_debug();
        qDebug() << "outputvalue=" << outputvalue;
        ToastMsg::show(this, "", tr("Please select a usb DAC"));
        return;
    }
    Dialog::dialogsetting_usbsetting *dlg_hdmi_setting_520 = new Dialog::dialogsetting_usbsetting(this);


   // dlg_hdmi_setting->set_selected_deviceName("HDMI Setting");
    int result = dlg_hdmi_setting_520->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_hdmi_setting_520;
}
/**
 * @brief MusicFolder_Colud::showDlgOfSelectCloudType : 클라우드 타입 선택 다이얼로그 show
 */
void SettingInputOutput::showDlgOfHDMI_setting520(){

    Dialog::dialogsetting_hdmi_520 *dlg_hdmi_setting_520 = new Dialog::dialogsetting_hdmi_520(this);


   // dlg_hdmi_setting->set_selected_deviceName("HDMI Setting");
    int result = dlg_hdmi_setting_520->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_hdmi_setting_520;
}

void SettingInputOutput::showDlgOfHDMI_setting150(){

    Dialog::dialogsetting_hdmi_150 *dlg_hdmi_setting_150 = new Dialog::dialogsetting_hdmi_150(this);


   // dlg_hdmi_setting->set_selected_deviceName("HDMI Setting");
    int result = dlg_hdmi_setting_150->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_hdmi_setting_150;
}

/**
 * @brief MusicFolder_Colud::showDlgOfSelectCloudType : 클라우드 타입 선택 다이얼로그 show
 */
void SettingInputOutput::showDlgOfHDMI_setting250(){

    Dialog::dialogsetting_hdmi_250 *dlg_hdmi_setting_250 = new Dialog::dialogsetting_hdmi_250(this);


   // dlg_hdmi_setting->set_selected_deviceName("HDMI Setting");
    int result = dlg_hdmi_setting_250->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_hdmi_setting_250;
}

void SettingInputOutput::showDlgOfUSB_setting250(){

    Dialog::dialogsetting_usbsetting *dlg_hdmi_setting_250 = new Dialog::dialogsetting_usbsetting(this);


   // dlg_hdmi_setting->set_selected_deviceName("HDMI Setting");
    int result = dlg_hdmi_setting_250->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_hdmi_setting_250;
}

void SettingInputOutput::showDlgOfRX_setting250(){

    Dialog::dialogsetting_RXsetting *dlg_hdmi_setting_250 = new Dialog::dialogsetting_RXsetting(this);


   // dlg_hdmi_setting->set_selected_deviceName("HDMI Setting");
    int result = dlg_hdmi_setting_250->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_hdmi_setting_250;
}

void SettingInputOutput::setInput150(int inputNo){
    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    qDebug() << "setInput150(int inputNo) inputNo = " << inputNo;
    int curInputNo = get_InputMode150();
    if(inputNo == curInputNo ){
        tmp_json.insert("funcMode", RS150 );
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("input.mode.set"), tmp_json, true);

    }else{
        tmp_json.insert("funcMode", inputNo );
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("input.mode.set"), tmp_json, true);

    }
}
void SettingInputOutput::setInput520(int inputNo){
    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    qDebug() << "setInput520(int inputNo) inputNo = " << inputNo;
    int curInputNo = get_InputMode520();
    if(inputNo == curInputNo ){
        tmp_json.insert("funcMode", RS520 );
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("input.mode.set"), tmp_json, true);

    }else{
        tmp_json.insert("funcMode", inputNo );
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("input.mode.set"), tmp_json, true);

    }
}
void SettingInputOutput::slot_setInput150(){

    qDebug() << "input: sender()->property(btnNo).toInt()  = " << sender()->property("btnNo").toInt();
    int btnSenderNo = sender()->property("btnNo").toInt();

    switch(btnSenderNo){

    case RS150  ://       = 0;//         - btn150_input_Model
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(RS150));
        qDebug() << tr("click %1-setInput150.").arg(RS150);
        //requestInputOutputMode();
        setInput150(0);
        break;
    case LINEIN_L://        = 3;//          - btn150_line_in_l
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(LINEIN_L));
        qDebug() << tr("click %1-setInput150.").arg(LINEIN_L);
        setInput150(1);
        //setOutput150(4);
       // setInternalOutput_150("0,1,1,1,1,1,1,0");
      //  setOutput150(4);//cheon210809-output
        break;
    case LINEIN_R://        = 4;//          - btn150_line_in_r
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(LINEIN_R));
        qDebug() << tr("click %1-setInput150.").arg(LINEIN_R);
        setInput150(1);

       // setInternalOutput_150("0,1,1,1,1,1,1,0");
      //  setOutput150(4);//cheon210809-output
        break;
    case OPTIN://         = 3;//          - btn150_opt_in
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(OPTIN));
        qDebug() << tr("click %1-setInput150.").arg(OPTIN);
        setInput150(2);

       // setInternalOutput_150("0,1,1,1,1,1,1,0");
       // setOutput150(4);//cheon210809-output
        break;
    case ARCIN://         = 5;//          - btn150_HDMI_ARC_in
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(ARCIN));
        qDebug() << tr("click %1-setInput150.").arg(ARCIN);

        setInput150(3);
        //setOutput150(4);
      //  setOutput150(4);//cheon210809-output
        setInternalOutput_forInput_150("0,0,1,1,1,1,1,0");

        break;
    case USBIN://         = 7;//          - btn150_USB_in
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(USBIN));
        qDebug() << tr("click %1-setInput150.").arg(USBIN);
        qDebug() << "11111==================================================================================";
       // setOutput150(4);//cheon210809-output

        setInput150(4);
      //  QThread::msleep(2000);
        qDebug() << "22222==================================================================================";

      //  QThread::msleep(2000);

       // setOutput150(2);//cheon210809-output
        qDebug() << "33333==================================================================================";
        //setInternalOutput_forInput_150("0,1,1,0,1,1,1,0");//cheon210809-output
        qDebug() << "44444==================================================================================";

        break;
    case COAX_IN://         = 6;//          - btn150_USB_in
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(COAX_IN));
        qDebug() << tr("click %1-setInput150.").arg(COAX_IN);
        setInput150(130);
        //setOutput150(4);
       // setInternalOutput_150("0,1,1,1,1,1,1,0");
        //setOutput150(4);//cheon210809-output
        break;
    case AES_EBUIN://     = 1;//        - btn150_AES_EBU_in
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(AES_EBUIN));
        qDebug() << tr("click %1-setInput150.").arg(AES_EBUIN);
        //qDebug() << "sender()->property(funcMode).toInt()  = " << sender()->property("funcMode").toInt();
        setInput150(5);
        //setOutput150(4);
      //  setInternalOutput_150("0,1,1,1,1,1,1,0");
        //setOutput150(4);//cheon210809-output
        break;
    }

}

void SettingInputOutput::slot_setInput520(){

    qDebug() << "input: sender()->property(btnNo).toInt()  = " << sender()->property("btnNo").toInt();
    int btnSenderNo = sender()->property("btnNo").toInt();

    switch(btnSenderNo){

    case RS520  ://       = 0;//         - btn520_input_Model
        //ToastMsg::show(this, "", tr("click %1-setInput520.").arg(RS520));
        qDebug() << tr("click %1-setInput520.").arg(RS520);
        //requestInputOutputMode();
        setInput520(0);
        break;
    case LINEIN_L://        = 3;//          - btn520_line_in_l
        //ToastMsg::show(this, "", tr("click %1-setInput520.").arg(LINEIN_L));
        qDebug() << tr("click %1-setInput520.").arg(LINEIN_L);
        setInput520(1);
        //setOutput520(4);
       // setInternalOutput_520("0,1,1,1,1,1,1,0");
      //  setOutput520(4);//cheon210809-output
        break;
    case LINEIN_R://        = 4;//          - btn520_line_in_r
        //ToastMsg::show(this, "", tr("click %1-setInput520.").arg(LINEIN_R));
        qDebug() << tr("click %1-setInput520.").arg(LINEIN_R);
        setInput520(1);

       // setInternalOutput_520("0,1,1,1,1,1,1,0");
      //  setOutput520(4);//cheon210809-output
        break;
    case OPTIN://         = 3;//          - btn520_opt_in
        //ToastMsg::show(this, "", tr("click %1-setInput520.").arg(OPTIN));
        qDebug() << tr("click %1-setInput520.").arg(OPTIN);
        setInput520(2);

       // setInternalOutput_520("0,1,1,1,1,1,1,0");
       // setOutput520(4);//cheon210809-output
        break;
    case ARCIN://         = 5;//          - btn520_HDMI_ARC_in
        //ToastMsg::show(this, "", tr("click %1-setInput520.").arg(ARCIN));
        qDebug() << tr("click %1-setInput520.").arg(ARCIN);

        setInput520(3);
        //setOutput520(4);
      //  setOutput520(4);//cheon210809-output
        setInternalOutput_forInput_520("0,0,1,1,1,1,1,0");

        break;
    case USBIN://         = 7;//          - btn520_USB_in
        //ToastMsg::show(this, "", tr("click %1-setInput520.").arg(USBIN));
        qDebug() << tr("click %1-setInput520.").arg(USBIN);
        qDebug() << "11111==================================================================================";
       // setOutput520(4);//cheon210809-output

        setInput520(4);
      //  QThread::msleep(2000);
        qDebug() << "22222==================================================================================";

      //  QThread::msleep(2000);

       // setOutput520(2);//cheon210809-output
        qDebug() << "33333==================================================================================";
        //setInternalOutput_forInput_520("0,1,1,0,1,1,1,0");//cheon210809-output
        qDebug() << "44444==================================================================================";

        break;
    case COAX_IN://         = 6;//          - btn520_USB_in
        //ToastMsg::show(this, "", tr("click %1-setInput520.").arg(COAX_IN));
        qDebug() << tr("click %1-setInput520.").arg(COAX_IN);
        setInput520(130);
        //setOutput520(4);
       // setInternalOutput_520("0,1,1,1,1,1,1,0");
        //setOutput520(4);//cheon210809-output
        break;
    case AES_EBUIN://     = 1;//        - btn520_AES_EBU_in
        //ToastMsg::show(this, "", tr("click %1-setInput520.").arg(AES_EBUIN));
        qDebug() << tr("click %1-setInput520.").arg(AES_EBUIN);
        //qDebug() << "sender()->property(funcMode).toInt()  = " << sender()->property("funcMode").toInt();
        setInput520(5);
        //setOutput520(4);
      //  setInternalOutput_520("0,1,1,1,1,1,1,0");
        //setOutput520(4);//cheon210809-output
        break;
    }

}

void SettingInputOutput::slot_setInput250(){

    qDebug() << "input: sender()->property(btnNo).toInt()  = " << sender()->property("btnNo").toInt();
    int btnSenderNo = sender()->property("btnNo").toInt();

    switch(btnSenderNo){

    case RS150  ://       = 0;//         - btn150_input_Model
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(RS150));
        qDebug() << tr("click %1-setInput250.").arg(RS150);
        //requestInputOutputMode();
        setInput150(0);
        break;
    case LINEIN_L://        = 3;//          - btn150_line_in_l
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(LINEIN_L));
        //qDebug() << tr("click %1-setInput250.").arg(LINEIN_L);
        setInput150(1);
        //setOutput150(4);
       // setInternalOutput_150("0,1,1,1,1,1,1,0");
       // setOutput150(4);//cheon210809-output
        break;
    case LINEIN_R://        = 4;//          - btn150_line_in_r
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(LINEIN_R));
        //qDebug() << tr("click %1-setInput250.").arg(LINEIN_R);
        setInput150(1);

       // setInternalOutput_150("0,1,1,1,1,1,1,0");
       // setOutput150(4);//cheon210809-output
        break;
    case OPTIN://         = 3;//          - btn150_opt_in
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(OPTIN));
        qDebug() << tr("click %1-setInput250.").arg(OPTIN);
        setInput150(2);

       // setInternalOutput_150("0,1,1,1,1,1,1,0");
        //setOutput150(4);//cheon210809-output
        break;
    case ARCIN://         = 5;//          - btn150_HDMI_ARC_in
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(ARCIN));
        qDebug() << tr("click %1-setInput250.").arg(ARCIN);

        setInput150(3);
        //setOutput150(4);
        //setOutput150(4);//cheon210809-output
        //setInternalOutput_forInput_150("0,0,1,1,1,1,1,0");//cheon210809-output

        break;
    case USBIN://         = 7;//          - btn150_USB_in
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(USBIN));
        qDebug() << tr("click %1-setInput250.").arg(USBIN);
        qDebug() << "11111==================================================================================";
       // setOutput150(4);//cheon210809-output

        setInput150(4);
      //  QThread::msleep(2000);
        qDebug() << "22222==================================================================================";

      //  QThread::msleep(2000);

       // setOutput150(2);//cheon210809-output
        qDebug() << "33333==================================================================================";
       // setInternalOutput_forInput_250("1,1,1");//cheon210809-output
        qDebug() << "44444==================================================================================";

        break;
    case COAX_IN://         = 6;//          - btn150_USB_in
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(COAX_IN));
        qDebug() << tr("click %1-setInput250.").arg(COAX_IN);
        setInput150(130);
        //setOutput150(4);
       // setInternalOutput_150("0,1,1,1,1,1,1,0");
        //setOutput150(4);//cheon210809-output
        break;
    case AES_EBUIN://     = 1;//        - btn150_AES_EBU_in
        //ToastMsg::show(this, "", tr("click %1-setInput150.").arg(AES_EBUIN));
        qDebug() << tr("click %1-setInput250.").arg(AES_EBUIN);
        //qDebug() << "sender()->property(funcMode).toInt()  = " << sender()->property("funcMode").toInt();
        setInput150(5);
        //setOutput150(4);
      //  setInternalOutput_150("0,1,1,1,1,1,1,0");
       // setOutput150(4);//cheon210809-output
        break;
    }

}
ushort SettingInputOutput::bit2str_Convert(QString r){
    print_debug();
    qDebug() << "r=" << r;
        unsigned char uc = 0;
        if(r[0] == '1') uc |= 0x80;
        if(r[2] == '1') uc |= 0x40;
        if(r[4] == '1') uc |= 0x20;
        if(r[6] == '1') uc |= 0x10;
        if(r[8] == '1') uc |= 0x08;
        if(r[10] == '1') uc |= 0x04;
        if(r[12] == '1') uc |= 0x02;
        if(r[14] == '1') uc |= 0x01;

        return uc;
}
QString SettingInputOutput::str2bit_Convert(ushort uc){
        QString s = "0,0,0,0,0,0,0,0";
        QByteArray byteName = s.toLocal8Bit();
        char *c = byteName.data();
      //  if ((uc & (1<<7)) == 128 ) {qDebug("1"); c[0] = '1';}
        if ((uc & (1<<6)) == 64 ) {qDebug("1"); c[2] = '1';}
        if ((uc & (1<<5)) == 32 ) {qDebug("1");c[4] = '1';}
        if ((uc & (1<<4)) == 16 ) {qDebug("1");c[6] = '1';}
        if ((uc & (1<<3)) == 8 ) {qDebug("1");c[8] = '1';}
        if ((uc & (1<<2)) == 4 ) {qDebug("1");c[10] = '1';}
        if ((uc & (1<<1)) == 2 ) {qDebug("1");c[12] = '1';}
       // if ((uc & (1<<0)) == 1 ) {qDebug("1");c[14] = '1';}
        s = QString::fromLocal8Bit(c);
        qDebug() << "str2bit_Convert : " << s;
        return s;
}

QString SettingInputOutput::str2bit_Convert250(ushort uc){
        QString s = "0,0,0,0,0,0,0,0";
        QByteArray byteName = s.toLocal8Bit();
        char *c = byteName.data();
        if ((uc & (1<<7)) == 128 ) {qDebug("1"); c[0] = '1';}
        if ((uc & (1<<6)) == 64 ) {qDebug("1"); c[2] = '1';}
        if ((uc & (1<<5)) == 32 ) {qDebug("1");c[4] = '1';}
        if ((uc & (1<<4)) == 16 ) {qDebug("1");c[6] = '1';}
        if ((uc & (1<<3)) == 8 ) {qDebug("1");c[8] = '1';}
        if ((uc & (1<<2)) == 4 ) {qDebug("1");c[10] = '1';}
        if ((uc & (1<<1)) == 2 ) {qDebug("1");c[12] = '1';}
        if ((uc & (1<<0)) == 1 ) {qDebug("1");c[14] = '1';}
        s = QString::fromLocal8Bit(c);
        qDebug() << "str2bit_Convert250 : " << s;
        QString ret = QString("%1,%2,%3").arg(s[0]).arg(s[2]).arg(s[4]);
        qDebug() << "str2bit_Convert250-ret : " << ret;

        return ret;
}
QString SettingInputOutput::str2bit_Convert520(ushort uc){
    print_debug();
        QString s = "0,0,0,0,0,0,0,0";
        QByteArray byteName = s.toLocal8Bit();
        char *c = byteName.data();
        if ((uc & (1<<7)) == 128 ) {qDebug("7-1"); c[0] = '1';}
        if ((uc & (1<<6)) == 64 ) {qDebug("6-1"); c[2] = '1';}
        if ((uc & (1<<5)) == 32 ) {qDebug("5-1");c[4] = '1';}
        if ((uc & (1<<4)) == 16 ) {qDebug("4-1");c[6] = '1';}
        if ((uc & (1<<3)) == 8 ) {qDebug("3-1");c[8] = '1';}
        if ((uc & (1<<2)) == 4 ) {qDebug("2-1");c[10] = '1';}
        if ((uc & (1<<1)) == 2 ) {qDebug("1-1");c[12] = '1';}
        if ((uc & (1<<0)) == 1 ) {qDebug("0-1");c[14] = '1';}
        s = QString::fromLocal8Bit(c);
        qDebug() << "str2bit_Convert520 : " << s;
        QString ret = QString("%1,%2,%3,%4").arg(s[0]).arg(s[2]).arg(s[4]).arg(s[6]);
        qDebug() << "str2bit_Convert520-ret : " << ret;

        return ret;
}
ushort SettingInputOutput::internalChange(ushort a, ushort b){
    ushort c = 0;
     if(((a & (1<<7)) == 0 || (b & (1<<7)) == 0) || ((a & (1<<1)) == 0 || (b & (1<<1)) == 0) ){
         //qDebug("1"); c += 128;
     }else return 0xFF;

    if ( ((a & (1<<6)) == 64 && (b & (1<<6)) == 64 ) || ((a & (1<<6)) == 0 && (b & (1<<6)) == 0 ) ) {qDebug("1"); c += 64;}
    if ( ((a & (1<<5)) == 32 && (b & (1<<5)) == 32 ) || ((a & (1<<5)) == 0 && (b & (1<<5)) == 0 ) ) {qDebug("1"); c += 32;}
    if ( ((a & (1<<4)) == 16 && (b & (1<<4)) == 16 ) || ((a & (1<<4)) == 0 && (b & (1<<4)) == 0 ) ) {qDebug("1"); c += 16;}
    if ( ((a & (1<<3)) == 8 && (b & (1<<3)) == 8 ) || ((a & (1<<3)) == 0 && (b & (1<<3)) == 0 ) ) {qDebug("1"); c += 8;}
    if ( ((a & (1<<2)) == 4 && (b & (1<<2)) == 4 ) || ((a & (1<<2)) == 0 && (b & (1<<2)) == 0 ) ) {qDebug("1"); c += 4;}
    if ( ((a & (1<<1)) == 2 && (b & (1<<1)) == 2 ) || ((a & (1<<1)) == 0 && (b & (1<<1)) == 0 ) ) {qDebug("1"); c += 2;}

    return c;

}

ushort SettingInputOutput::internalChange520(ushort a, ushort b){
    ushort c = 0;
    print_debug();
    qDebug() << "a=" << a;
    qDebug() << "b=" << b;
     if(((a & (1<<7)) == 128 && (b & (1<<7)) == 128) || ((a & (1<<7)) == 0 && (b & (1<<7)) == 0) ){
         qDebug("7-1"); c += 0;
     }else{
         qDebug("7-0"); c += 128;
     }//else return 0xFF;

    if ( ((a & (1<<6)) == 64 && (b & (1<<6)) == 64 ) || ((a & (1<<6)) == 0 && (b & (1<<6)) == 0 ) ) {qDebug("6-1"); c += 0;}
    else{
             qDebug("6-0"); c += 64;
    }
    if ( ((a & (1<<5)) == 32 && (b & (1<<5)) == 32 ) || ((a & (1<<5)) == 0 && (b & (1<<5)) == 0 ) ) {qDebug("5-1"); c += 0;}
    else{
             qDebug("5-0"); c += 32;
    }
    if ( ((a & (1<<4)) == 16 && (b & (1<<4)) == 16 ) || ((a & (1<<4)) == 0 && (b & (1<<4)) == 0 ) ) {qDebug("4-1"); c += 0;}
    else{
             qDebug("4-0"); c += 16;
    }
    if ( ((a & (1<<3)) == 8 && (b & (1<<3)) == 8 ) || ((a & (1<<3)) == 0 && (b & (1<<3)) == 0 ) ) {qDebug("3-1"); c += 0;}
    else{
             qDebug("3-0"); c += 8;
    }
    if ( ((a & (1<<2)) == 4 && (b & (1<<2)) == 4 ) || ((a & (1<<2)) == 0 && (b & (1<<2)) == 0 ) ) {qDebug("2-1"); c += 0;}
    else{
             qDebug("2-0"); c += 4;
    }
    if ( ((a & (1<<1)) == 2 && (b & (1<<1)) == 2 ) || ((a & (1<<1)) == 0 && (b & (1<<1)) == 0 ) ) {qDebug("1-1"); c += 0;}
    else{
             qDebug("1-0"); c += 2;
    }
    if ( ((a & (1<<0)) == 1 && (b & (1<<0)) == 1 ) || ((a & (1<<0)) == 0 && (b & (1<<0)) == 0 ) ) {qDebug("0-1"); c += 0;}
    else{
             qDebug("0-0"); c += 1;
    }

    return c;

}
void SettingInputOutput::setInternalOutput_forInput_150(QString req_internalData){
 //   ToastMsg::show(this, "", tr("Service is being prepared."));
print_debug(); //cheon10-io
    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
 //   int output_data, itm8_data[8];//, itm8_data[8];
 //   output_data = get_data( 8, itm8_data);
    //print_debug(); //cheon10-io
  //  QString cur_internalData  = get_data();

    qDebug() << "origin_req_internalData : " << req_internalData;

        unsigned char req = bit2str_Convert(req_internalData);
   //     unsigned char cur = bit2str_Convert(cur_internalData);
        qDebug() << "before_req = " << req;
     //   qDebug() << "cur = " << cur;
    //    req = internalChange(req, cur);//req &= cur;
        if(req == 0xff) {qDebug()<< "FF is returned!!!!!!!!!!!"; return;}
        qDebug() << "after_req = " << req;
       // unsigned char expire = req & cur;
    //    expire_internalData = str2bit_Convert(expire);

            req_internalData = str2bit_Convert(req);
            print_debug();
            qDebug() << "req_internalData : " << req_internalData;
            tmp_json.insert("internalMode", req_internalData);
       //     connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);

}


void SettingInputOutput::setInternalOutput_forInput_520(QString req_internalData){
 //   ToastMsg::show(this, "", tr("Service is being prepared."));
print_debug(); //cheon10-io
    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
 //   int output_data, itm8_data[8];//, itm8_data[8];
 //   output_data = get_data( 8, itm8_data);
    //print_debug(); //cheon10-io
  //  QString cur_internalData  = get_data();

    qDebug() << "origin_req_internalData : " << req_internalData;

        unsigned char req = bit2str_Convert(req_internalData);
   //     unsigned char cur = bit2str_Convert(cur_internalData);
        qDebug() << "before_req = " << req;
     //   qDebug() << "cur = " << cur;
    //    req = internalChange(req, cur);//req &= cur;
        if(req == 0xff) {qDebug()<< "FF is returned!!!!!!!!!!!"; return;}
        qDebug() << "after_req = " << req;
       // unsigned char expire = req & cur;
    //    expire_internalData = str2bit_Convert(expire);

            req_internalData = str2bit_Convert(req);
            print_debug();
            qDebug() << "req_internalData : " << req_internalData;
            tmp_json.insert("internalMode", req_internalData);
       //     connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);

}

void SettingInputOutput::setInternalOutput_forInput_250(QString req_internalData){
 //   ToastMsg::show(this, "", tr("Service is being prepared."));
    print_debug(); //cheon10-io
        QJsonObject tmp_json;
        NetworkHttp *network = new NetworkHttp;
        int output_data, itm3_data[3];//, itm8_data[8];
        output_data = get_data( 3, itm3_data);
        //print_debug(); //cheon10-io
        QString cur_internalData  = get_data();

        qDebug() << "origin_req_internalData : " << req_internalData;

  //  unsigned char req = bit2str_Convert(req_internalData);
  //  unsigned char cur = bit2str_Convert(cur_internalData);

        print_debug();
     //   qDebug() << "else ->cur_internalData : " << cur_internalData << "req_internalData : " << req_internalData << "inter_internalData : " << inter_internalData;
        tmp_json.insert("internalMode", req_internalData);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);
   // }
}


void SettingInputOutput::setInternalOutput_250(QString req_internalData){
 //   ToastMsg::show(this, "", tr("Service is being prepared."));
print_debug(); //cheon10-io
    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    int output_data, itm3_data[3];//, itm8_data[8];
    output_data = get_data( 3, itm3_data);
    //print_debug(); //cheon10-io
    QString cur_internalData  = get_data();

    qDebug() << "origin_req_internalData : " << req_internalData;

    if(output_data == 2 || output_data == 3){
        bool flag = showDlgOfDACMsg(tr("Internal Mode"), tr("Would you like to set it to COAX_OUT mode?"));
        if(flag ) {

            tmp_json.insert("outputMode", "1");
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);
         }
    }else{
        unsigned char req = bit2str_Convert(req_internalData);
        unsigned char cur = bit2str_Convert(cur_internalData);
        qDebug() << "before_req = " << req;
        qDebug() << "cur = " << cur;
      //  req = internalChange(req, cur);//req &= cur;
        if(req == 0xff) {qDebug()<< "FF is returned!!!!!!!!!!!"; return;}
     //   qDebug() << "after_req = " << req;
        unsigned char a1 = 196;
        unsigned char a2 = 32;
        unsigned char inter1, inter2;
        if(req  != a2){
            inter1 = (req ^ cur);
            inter2 = 0;
        }else {
            inter1 = 0;
            inter2 = req ^ cur;
        }
        unsigned char inter = (inter1 & a1) + (inter2 & a2);
        qDebug() << "inter_a1 : " << (inter1&a1) << ", inter1_a2 :" << (inter2&a2) << ", inter :" << inter;
      //  unsigned char expire = (req ^ cur);
        //   qDebug() << "after_expire = " << expire;
        QString inter_internalData = str2bit_Convert250(inter);
        qDebug() << "inter_internalData = " << inter_internalData;

      //  if(cur == req){
      //      print_debug();
       //     qDebug() << "if->cur_internalData : " << cur_internalData << "req_internalData : " << req_internalData;
       //     tmp_json.insert("internalMode", expire_internalData);
        //    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        //    network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);

      //  } else{
            //req_internalData = str2bit_Convert250(req);
            print_debug();
            qDebug() << "else ->cur_internalData : " << cur_internalData << "req_internalData : " << req_internalData << "inter_internalData : " << inter_internalData;
            tmp_json.insert("internalMode", inter_internalData);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);
       // }

    }
}



void SettingInputOutput::slot_setInternal250(){
  //  ToastMsg::show(this, "", tr("2-setInternal150."));

    //qDebug() << "Internal: sender()->property(btnNo).toInt()  = " << sender()->property("btnNo").toInt();
    int btnSenderNo = sender()->property("btnNo").toInt();
    if(inputvalue250 == 4) {
        ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));
        return;
    }
    switch(btnSenderNo){
    case COAX_OUT://      = 11;//   - btn150_coax_out
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput150.").arg(COAX_OUT));
        //qDebug() << tr("click %1-setInternalOutput150.").arg(COAX_OUT);

        setInternalOutput_250("1,0,0");

        break;

    case OPT_OUT    ://   = 12;//      - btn150_opt_out
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput150.").arg(OPT_OUT));
        //qDebug() << tr("click %1-setInternalOutput150.").arg(OPT_OUT);

        setInternalOutput_250("0,1,0");
        break;

    case UNBALANCED_OUT_L:// = 18;//  - btn150_unbal2_out_l
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput150.").arg(UNBALANCED_OUT_L));
        //qDebug() << tr("click %1-setInternalOutput150.").arg(UNBALANCED_OUT_L);
        setInternalOutput_250("0,0,1");
        break;
    case UNBALANCED_OUT_R://  = 19;//  - btn150_unbal2_out_r
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput150.").arg(UNBALANCED_OUT_R));
        //qDebug() << tr("click %1-setInternalOutput150.").arg(UNBALANCED_OUT_R);
        setInternalOutput_250("0,0,1");
        break;
    }

}


void SettingInputOutput::setInternalOutput_150(QString req_internalData,  QString expire_internalData){
 //   ToastMsg::show(this, "", tr("Service is being prepared."));
print_debug(); //cheon10-io
    QJsonObject tmp_json;

    int output_data, itm8_data[8];//, itm8_data[8];
    output_data = get_data( 8, itm8_data);
    //print_debug(); //cheon10-io
    QString cur_internalData  = get_data();

    qDebug() << "origin_req_internalData : " << req_internalData;

    if(output_data == 2 || output_data == 3){
        bool flag = showDlgOfDACMsg(tr("Internal Mode"), tr("Would you like to set it to COAX_OUT mode?"));
        if(flag ) {

            NetworkHttp *network = new NetworkHttp;
            tmp_json.insert("outputMode", "1");
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);
         }
    }else{
        unsigned char req = bit2str_Convert(req_internalData);
        unsigned char cur = bit2str_Convert(cur_internalData);
        qDebug() << "before_req = " << req;
        qDebug() << "cur = " << cur;
        req = internalChange(req, cur);//req &= cur;
        if(req == 0xff) {qDebug()<< "FF is returned!!!!!!!!!!!"; return;}
        qDebug() << "after_req = " << req;
        unsigned char expire = req & cur;
        expire_internalData = str2bit_Convert(expire);

        if(cur == req){
            print_debug();
            NetworkHttp *network = new NetworkHttp;
            qDebug() << "if->cur_internalData : " << cur_internalData << "req_internalData : " << req_internalData;
            tmp_json.insert("internalMode", expire_internalData);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);

        } else{
            req_internalData = str2bit_Convert(req);
            print_debug();
            NetworkHttp *network = new NetworkHttp;
            qDebug() << "else ->cur_internalData : " << cur_internalData << "req_internalData : " << req_internalData;
            tmp_json.insert("internalMode", req_internalData);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);
        }

    }
}

void SettingInputOutput::setInternalOutput_520(QString req_internalData,  QString expire_internalData){//c221013_1
 //   ToastMsg::show(this, "", tr("Service is being prepared."));
print_debug(); //cheon10-io
    QJsonObject tmp_json;

    int output_data, itm8_data[8];//, itm8_data[8];
    output_data = get_data( 8, itm8_data);
    //print_debug(); //cheon10-io
    QString cur_internalData  = get_data();

    qDebug() << "origin_req_internalData : " << req_internalData;

    /*
        bool flag = showDlgOfDACMsg(tr("Notice"), tr("Are you sure you want to change the output settings?"));
        if(flag ) {
        }
*/

        print_debug();
        unsigned char req = bit2str_Convert(req_internalData);
        unsigned char cur = bit2str_Convert(cur_internalData);
        qDebug() << "before_req = " << req;
        qDebug() << "cur = " << cur;
        req = internalChange520(req, cur);//req &= cur;
        if(req == 0xff) {qDebug()<< "FF is returned!!!!!!!!!!!"; return;}
        qDebug() << "after_req = " << req;
        unsigned char expire = req & cur;
        expire_internalData = str2bit_Convert520(expire);

        if(cur == req){
            print_debug();
            NetworkHttp *network = new NetworkHttp;
            qDebug() << "if->cur_internalData : " << cur_internalData << "req_internalData : " << req_internalData;
            tmp_json.insert("internalMode", expire_internalData.left(7));
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);

        } else{
            req_internalData = str2bit_Convert520(req);
            print_debug();

            NetworkHttp *network = new NetworkHttp;
            qDebug() << "cur_internalData : " << cur_internalData << "req_internalData : " << req_internalData;
            qDebug() << "req_internalData.left(7)=" << req_internalData.left(7);
            tmp_json.insert("internalMode", req_internalData.left(7));
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);



        }


}

void SettingInputOutput::slot_setInternal150(){
  //  ToastMsg::show(this, "", tr("2-setInternal150."));

    //qDebug() << "Internal: sender()->property(btnNo).toInt()  = " << sender()->property("btnNo").toInt();
    int btnSenderNo = sender()->property("btnNo").toInt();
    if(inputvalue150 == 4) {
        ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));
        return;
    }
    switch(btnSenderNo){
    case COAX_OUT://      = 11;//   - btn150_coax_out
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput150.").arg(COAX_OUT));
        //qDebug() << tr("click %1-setInternalOutput150.").arg(COAX_OUT);

        setInternalOutput_150("0,0,1,1,1,1,1,0", "0,1,1,1,1,1,1,0");

        break;
    case I2S_RJ45_OUT://  = 15;//     - btn150_i2s_rj45_out
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput150.").arg(I2S_RJ45_OUT));
        //qDebug() << tr("click %1-setInternalOutput150.").arg(I2S_RJ45_OUT);

        setInternalOutput_150("0,1,0,1,1,1,1,0", "0,1,1,1,1,1,1,0");

        break;
    case AES_EBU_OUT://   = 14;//    - btn150_AES_EBU_out
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput150.").arg(AES_EBU_OUT));
        //qDebug() << tr("click %1-setInternalOutput150.").arg(AES_EBU_OUT);
        if(inputvalue150 == 4) {
            ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));
            return;
        }
        setInternalOutput_150("0,1,1,0,1,1,1,0", "0,1,1,1,1,1,1,0");

        break;
    case I2S_DVI_OUT://   = 13;//      - btn150_i2s_dvi_out
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput150.").arg(I2S_DVI_OUT));
        //qDebug() << tr("click %1-setInternalOutput150.").arg(I2S_DVI_OUT);

        setInternalOutput_150("0,1,1,1,0,1,1,0", "0,1,1,1,1,1,1,0");
        break;
    case OPT_OUT    ://   = 12;//      - btn150_opt_out
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput150.").arg(OPT_OUT));
        //qDebug() << tr("click %1-setInternalOutput150.").arg(OPT_OUT);

        setInternalOutput_150("0,1,1,1,1,0,1,0", "0,1,1,1,1,1,1,0");
        break;
    case BALANCED_OUT_L://  = 16;//  - btn150_balanced_out_l
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput150.").arg(BALANCED_OUT_L));
        //qDebug() << tr("click %1-setInternalOutput150.").arg(BALANCED_OUT_L);

        setInternalOutput_150("0,1,1,1,1,1,0,0", "0,1,1,1,1,1,1,0");
        break;
    case BALANCED_OUT_R://  = 17;//  - btn150_balanced_out_r
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput150.").arg(BALANCED_OUT_R));
        //qDebug() << tr("click %1-setInternalOutput150.").arg(BALANCED_OUT_R);

        setInternalOutput_150("0,1,1,1,1,1,0,0", "0,1,1,1,1,1,1,0");
        break;
    case UNBALANCED_OUT_L:// = 18;//  - btn150_unbal2_out_l
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput150.").arg(UNBALANCED_OUT_L));
        //qDebug() << tr("click %1-setInternalOutput150.").arg(UNBALANCED_OUT_L);
        setInternalOutput_150("0,1,1,1,1,1,0,0", "0,1,1,1,1,1,1,0");
        break;
    case UNBALANCED_OUT_R://  = 19;//  - btn150_unbal2_out_r
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput150.").arg(UNBALANCED_OUT_R));
        //qDebug() << tr("click %1-setInternalOutput150.").arg(UNBALANCED_OUT_R);
        setInternalOutput_150("0,1,1,1,1,1,0,0", "0,1,1,1,1,1,1,0");
        break;
    }

}

void SettingInputOutput::slot_setInternal520(){//c221013_1
  //  ToastMsg::show(this, "", tr("2-setInternal520."));

    QJsonObject tmp_json;
    //qDebug() << "Internal: sender()->property(btnNo).toInt()  = " << sender()->property("btnNo").toInt();
    int btnSenderNo = sender()->property("btnNo").toInt();
    print_debug();
    qDebug() <<"btnSenderNo="<<btnSenderNo;
    QString cur_internalData  = get_data();
    if(inputvalue520 == 4) {
        print_debug();
        qDebug() << "inputvalue520=" << inputvalue520;
        //ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));
        //return;
    }
    if(4 != get_outputMode()){
        dlgConfirm520 = new DialogConfirm(this, DialogConfirm::DlgConfirmType::OKCancel);
        this->dlgConfirm520->setTitle(tr("Notice"));
        this->dlgConfirm520->setText(tr("Are you sure you want to change the output settings?"));

        int result = this->dlgConfirm520->exec();
//1000
        if(result == QDialog::Accepted){
            setOutput520(4);
            return;
        }else{
            return;
        }

    }

    switch(btnSenderNo){//0246
    case COAX_OUT:// 1,0,0,0     = 11;//   - btn520_coax_out
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput520.").arg(COAX_OUT));
        //qDebug() << tr("click %1-setInternalOutput520.").arg(COAX_OUT);
print_debug();
qDebug() << "cur_internalData=" << cur_internalData;
qDebug() << "outputvalue=" << get_outputMode();


            if(cur_internalData.at(4) == "1" || cur_internalData.at(6) == "1"){
                if(inputvalue520 != 4) {
                    dlgConfirm520 = new DialogConfirm(this, DialogConfirm::DlgConfirmType::OKCancel);
                    this->dlgConfirm520->setTitle(tr("Notice"));
                    this->dlgConfirm520->setText(tr("Are you sure you want to change the output settings?"));

                    int result = this->dlgConfirm520->exec();
        //1000
                    if(result == QDialog::Accepted){

                        if(cur_internalData.at(4) == "1" && cur_internalData.at(6) != "1"){
                            print_debug();
                            setInternalOutput_520("1,0,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }else if(cur_internalData.at(4) != "1" && cur_internalData.at(6) == "1"){
                            print_debug();
                            setInternalOutput_520("1,0,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }else if(cur_internalData.at(4) == "1" && cur_internalData.at(6) == "1"){
                            print_debug();
                            setInternalOutput_520("1,0,1,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }
                        //    setInternalOutput_520("1,0,0,0,0,0,0,0", "1,1,1,1,0,0,0,0");


                    }
                }else{
                    if(cur_internalData.at(4) == "1" && cur_internalData.at(6) != "1"){
                        print_debug();
                        setInternalOutput_520("1,0,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                    }else if(cur_internalData.at(4) != "1" && cur_internalData.at(6) == "1"){
                        print_debug();
                        setInternalOutput_520("1,0,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                    }else if(cur_internalData.at(4) == "1" && cur_internalData.at(6) == "1"){
                        print_debug();
                        setInternalOutput_520("1,0,1,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                    }
                }

            }else{
                if(cur_internalData.at(0) == "1" && cur_internalData.at(2) == "1"){
                    print_debug();
                    setInternalOutput_520("1,0,0,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                }else if(cur_internalData.at(0) == "0" && cur_internalData.at(2) == "1"){

                    print_debug();
                    setInternalOutput_520("1,0,0,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                }else if(cur_internalData.at(0) == "1" && cur_internalData.at(2) == "0"){

                    print_debug();
                    dlgConfirm520 = new DialogConfirm(this, DialogConfirm::DlgConfirmType::OKCancel);
                    this->dlgConfirm520->setTitle(tr("Notice"));
                    this->dlgConfirm520->setText(tr("Are you sure you want to change the output settings?"));

                    int result = this->dlgConfirm520->exec();
        //1000
                    if(result == QDialog::Accepted){
                        setInternalOutput_520("1,0,1,1,0,0,0,0", "1,1,1,1,0,0,0,0");

                    }
                }else if(cur_internalData.at(0) == "0" && cur_internalData.at(2) == "0"){

                    print_debug();
                    dlgConfirm520 = new DialogConfirm(this, DialogConfirm::DlgConfirmType::OKCancel);
                    this->dlgConfirm520->setTitle(tr("Notice"));
                    this->dlgConfirm520->setText(tr("Are you sure you want to change the output settings?"));

                    int result = this->dlgConfirm520->exec();
        //1000
                    if(result == QDialog::Accepted){
                        setInternalOutput_520("1,0,0,0,0,0,0,0", "1,1,1,1,0,0,0,0");

                    }
                }
            }




        break;

    case OPT_OUT    ://0100   = 12;//      - btn520_opt_out
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput520.").arg(OPT_OUT));
        //qDebug() << tr("click %1-setInternalOutput520.").arg(OPT_OUT);
        print_debug();
        qDebug() << "cur_internalData=" << cur_internalData;
        qDebug() << "outputvalue=" << get_outputMode();
        //dlgConfirm520 = new DialogConfirm(this, DialogConfirm::DlgConfirmType::OKCancel);

            if(cur_internalData.at(4) == "1" || cur_internalData.at(6) == "1"){
                if(inputvalue520 != 4) {
                    dlgConfirm520 = new DialogConfirm(this, DialogConfirm::DlgConfirmType::OKCancel);
                    this->dlgConfirm520->setTitle(tr("Notice"));
                    this->dlgConfirm520->setText(tr("Are you sure you want to change the output settings?"));

                    int result = this->dlgConfirm520->exec();
        //1000
                    if(result == QDialog::Accepted){

                        if(cur_internalData.at(4) == "1" && cur_internalData.at(6) != "1"){
                            print_debug();
                            setInternalOutput_520("0,1,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }else if(cur_internalData.at(4) != "1" && cur_internalData.at(6) == "1"){
                            print_debug();
                            setInternalOutput_520("0,1,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }else if(cur_internalData.at(4) == "1" && cur_internalData.at(6) == "1"){
                            print_debug();
                            setInternalOutput_520("0,1,1,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }
                          //  setInternalOutput_520("0,1,0,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                    }
                }else{
                    if(cur_internalData.at(4) == "1" && cur_internalData.at(6) != "1"){
                        print_debug();
                        setInternalOutput_520("0,1,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                    }else if(cur_internalData.at(4) != "1" && cur_internalData.at(6) == "1"){
                        print_debug();
                        setInternalOutput_520("0,1,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                    }else if(cur_internalData.at(4) == "1" && cur_internalData.at(6) == "1"){
                        print_debug();
                        setInternalOutput_520("0,1,1,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                    }
                }

            }else{

                if(cur_internalData.at(0) == "1" && cur_internalData.at(2) == "1"){
                    print_debug();
                    setInternalOutput_520("0,1,0,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                }else if(cur_internalData.at(0) == "0" && cur_internalData.at(2) == "1"){

                    print_debug();
                    dlgConfirm520 = new DialogConfirm(this, DialogConfirm::DlgConfirmType::OKCancel);
                    this->dlgConfirm520->setTitle(tr("Notice"));
                    this->dlgConfirm520->setText(tr("Are you sure you want to change the output settings?"));

                    int result = this->dlgConfirm520->exec();
        //1000
                    if(result == QDialog::Accepted){
                        setInternalOutput_520("0,1,1,1,0,0,0,0", "1,1,1,1,0,0,0,0");

                    }

                }else if(cur_internalData.at(0) == "1" && cur_internalData.at(2) == "0"){

                    print_debug();
                    setInternalOutput_520("0,1,0,0,0,0,0,0", "1,1,1,1,0,0,0,0");

                }else if(cur_internalData.at(0) == "0" && cur_internalData.at(2) == "0"){

                    print_debug();
                    dlgConfirm520 = new DialogConfirm(this, DialogConfirm::DlgConfirmType::OKCancel);
                    this->dlgConfirm520->setTitle(tr("Notice"));
                    this->dlgConfirm520->setText(tr("Are you sure you want to change the output settings?"));

                    int result = this->dlgConfirm520->exec();
        //1000
                    if(result == QDialog::Accepted){
                        setInternalOutput_520("0,1,0,0,0,0,0,0", "1,1,1,1,0,0,0,0");

                    }
                }

            }


        break;
    case SPEAKER://0001  = 51;//  - btn520_speaker
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput520.").arg(SPEAKER));
        //qDebug() << tr("click %1-setInternalOutput520.").arg(SPEAKER);
        print_debug();
        qDebug() << "cur_internalData=" << cur_internalData;

            if(cur_internalData.at(0) == "1" || cur_internalData.at(2) == "1"){
                if(inputvalue520 != 4) {
                    dlgConfirm520 = new DialogConfirm(this, DialogConfirm::DlgConfirmType::OKCancel);
                    this->dlgConfirm520->setTitle(tr("Notice"));
                    this->dlgConfirm520->setText(tr("Are you sure you want to change the output settings?"));

                    int result = this->dlgConfirm520->exec();

                    if(result == QDialog::Accepted){
                        if(cur_internalData.at(0) == "1" && cur_internalData.at(2) != "1"){
                            print_debug();
                            setInternalOutput_520("1,0,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }else if(cur_internalData.at(0) != "1" && cur_internalData.at(2) == "1"){
                            print_debug();
                            setInternalOutput_520("0,1,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }else if(cur_internalData.at(0) == "1" && cur_internalData.at(2) == "1"){
                            print_debug();
                            setInternalOutput_520("1,1,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }else if(cur_internalData.at(0) == "0" && cur_internalData.at(2) == "0"){
                            print_debug();
                            setInternalOutput_520("0,0,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }


                    }
                }else{
                    if(cur_internalData.at(0) == "1" && cur_internalData.at(2) != "1"){
                        print_debug();
                        setInternalOutput_520("1,0,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                    }else if(cur_internalData.at(0) != "1" && cur_internalData.at(2) == "1"){
                        print_debug();
                        setInternalOutput_520("0,1,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                    }else if(cur_internalData.at(0) == "1" && cur_internalData.at(2) == "1"){
                        print_debug();
                        setInternalOutput_520("1,1,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                    }else if(cur_internalData.at(0) == "0" && cur_internalData.at(2) == "0"){
                        print_debug();
                        setInternalOutput_520("0,0,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                    }
                }

            }else{
                if(cur_internalData.at(4) == "0"){
                    print_debug();
                    dlgConfirm520 = new DialogConfirm(this, DialogConfirm::DlgConfirmType::OKCancel);
                    this->dlgConfirm520->setTitle(tr("Notice"));
                    this->dlgConfirm520->setText(tr("Are you sure you want to change the output settings?"));

                    int result = this->dlgConfirm520->exec();

                    if(result == QDialog::Accepted){
                        print_debug();
                        if(cur_internalData.at(6) == "1"){
                            setInternalOutput_520("1,1,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }else{
                            setInternalOutput_520("0,0,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }
                    }


                }else{
                    print_debug();
                    setInternalOutput_520("0,0,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");

                }
                //
            }


            //setInternalOutput_520("0,0,0,1,0,0,0,0", "1,1,1,1,0,0,0,0");
        break;

    case UNBALANCED_OUT_L://0010 = 18;//  - btn520_unbal2_out_l
    case UNBALANCED_OUT_R:
        print_debug();
        qDebug() << "cur_internalData=" << cur_internalData;
        //ToastMsg::show(this, "", tr("click %1-setInternalOutput520.").arg(UNBALANCED_OUT_L));
        //qDebug() << tr("click %1-setInternalOutput520.").arg(UNBALANCED_OUT_L);


        if(cur_internalData.at(0) == "1" || cur_internalData.at(2) == "1"){
            if(inputvalue520 != 4) {
                dlgConfirm520 = new DialogConfirm(this, DialogConfirm::DlgConfirmType::OKCancel);
                this->dlgConfirm520->setTitle(tr("Notice"));
                    this->dlgConfirm520->setText(tr("Are you sure you want to change the output settings?"));

                    int result = this->dlgConfirm520->exec();

                    if(result == QDialog::Accepted){
                        if(cur_internalData.at(0) == "1" && cur_internalData.at(2) != "1"){
                            print_debug();
                            setInternalOutput_520("1,0,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }else if(cur_internalData.at(0) != "1" && cur_internalData.at(2) == "1"){
                            print_debug();
                            setInternalOutput_520("0,1,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }else if(cur_internalData.at(0) == "1" && cur_internalData.at(2) == "1"){
                            print_debug();
                            setInternalOutput_520("1,1,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }else if(cur_internalData.at(0) == "0" && cur_internalData.at(2) == "0"){
                            print_debug();
                            setInternalOutput_520("0,0,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }


                    }
                }else{
                    if(cur_internalData.at(0) == "1" && cur_internalData.at(2) != "1"){
                        print_debug();
                        setInternalOutput_520("1,0,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                    }else if(cur_internalData.at(0) != "1" && cur_internalData.at(2) == "1"){
                        print_debug();
                        setInternalOutput_520("0,1,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                    }else if(cur_internalData.at(0) == "1" && cur_internalData.at(2) == "1"){
                        print_debug();
                        setInternalOutput_520("1,1,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                    }else if(cur_internalData.at(0) == "0" && cur_internalData.at(2) == "0"){
                        print_debug();
                        setInternalOutput_520("0,0,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                    }
                }

            }else{
                if(cur_internalData.at(6) == "0"){
                    print_debug();
                    dlgConfirm520 = new DialogConfirm(this, DialogConfirm::DlgConfirmType::OKCancel);
                    this->dlgConfirm520->setTitle(tr("Notice"));
                    this->dlgConfirm520->setText(tr("Are you sure you want to change the output settings?"));

                    int result = this->dlgConfirm520->exec();

                    if(result == QDialog::Accepted){
                        print_debug();
                        if(cur_internalData.at(4) == "1"){
                            setInternalOutput_520("1,1,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }else{
                            setInternalOutput_520("0,0,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");
                        }



                    }

                }else {
                    print_debug();
                    setInternalOutput_520("0,0,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");

                }
                   //
            }



        //setInternalOutput_520("0,0,1,0,0,0,0,0", "1,1,1,1,0,0,0,0");
        break;

    }

}


void SettingInputOutput::setUsbOutput201(){
 //   ToastMsg::show(this, "", tr("Service is being prepared."));
//print_debug(); //cheon10-io
    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    QString usb_name;

    int output_data;
    print_debug(); //cheon10-io
    output_data = get_outputMode();

    if(output_data == 3){
        //print_debug(); //cheon10-io
        bool flag = showDlgOfDACMsg(tr("Internal Mode"), tr("Are you sure you want to disable current usb output mode?"));
        if(flag ) {

            tmp_json.insert("outputMode", "1");
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);
        }
    }else{
        print_debug(); //cheon10-io
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_usb_status(int, QJsonObject)));
        network->request(HTTP_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("usb.dac.get"), tmp_json, true);

        bool flag = showDlgOfDACMsg(tr("Internal Mode"), tr("Are you sure you want to enable usb dac output mode?"));
        if(flag ) {
            QString tmp_selectType = get_UsbName();
            if(tmp_selectType==""){
                bool flag = showDlgOfDACMsg(tr("USB DAC"), tr("You didn't find the USB DAC."));
                if(flag ) {

                }
            }else{
                showDlgOfSelectUsbDac();
            }
        }else{

        }
    }
}

void SettingInputOutput::setOutput201(int outputNo){
    QJsonObject tmp_json;
    //NetworkHttp *network = new NetworkHttp;
    qDebug() << "setOutput150(int outputNo) outputNo = " << outputNo;

    if(outputNo == get_outputMode()){//c221013_1
        print_debug();
        NetworkHttp *network = new NetworkHttp;
        tmp_json.insert("outputMode", 4);
        //qDebug() << "=====================else--------setOutput520(int outputNo) outputNo = " << outputNo;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);

        //linker->signal_reload_setting_clicked("inputOutput");//c221013_1

    }else{
        if(outputNo == 3){
            setUsbOutput201();
        }else if(outputNo != 3){
            NetworkHttp *network = new NetworkHttp;
            tmp_json.insert("outputMode", outputNo);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);
        }
    }
    }



void SettingInputOutput::setOutput150(int outputNo){
    QJsonObject tmp_json;

    //qDebug() << "setOutput150(int outputNo) outputNo = " << outputNo;

    if(outputNo == get_outputMode()){//c221013_1
        print_debug();
        NetworkHttp *network = new NetworkHttp;
        tmp_json.insert("outputMode", 4);
        //qDebug() << "=====================else--------setOutput520(int outputNo) outputNo = " << outputNo;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);

        //linker->signal_reload_setting_clicked("inputOutput");//c221013_1

    }else{
        if(outputNo == 3){
            setUsbOutput150();
        }else if(outputNo != 3){
            NetworkHttp *network = new NetworkHttp;
            tmp_json.insert("outputMode", outputNo);
            //qDebug() << "=====================else--------setOutput150(int outputNo) outputNo = " << outputNo;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);
        }
    }

}

void SettingInputOutput::setOutput520(int outputNo){
    QJsonObject tmp_json;

    //qDebug() << "setOutput150(int outputNo) outputNo = " << outputNo;

    if(outputNo == get_outputMode()){//c221013_1
        print_debug();
        NetworkHttp *network = new NetworkHttp;
        tmp_json.insert("outputMode", 4);
        //qDebug() << "=====================else--------setOutput520(int outputNo) outputNo = " << outputNo;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);

        //linker->signal_reload_setting_clicked("inputOutput");//c221013_1

    }else{
        if(outputNo == 3){
            setUsbOutput520();
        }else if(outputNo != 3){

            NetworkHttp *network = new NetworkHttp;
            tmp_json.insert("outputMode", outputNo);
            //qDebug() << "=====================else--------setOutput520(int outputNo) outputNo = " << outputNo;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);



        }
    }

}

void SettingInputOutput::slot_setOutput150(){
    //ToastMsg::show(this, "", tr("3-setOutput150."));//c220511

    //qDebug() << "output: sender()->property(btnNo).toInt()  = " << sender()->property("btnNo").toInt();
    int btnSenderNo = sender()->property("btnNo").toInt();
  //  int usbDacFlag = false;
    if(inputvalue150 == 4 ) {
        print_debug();
        //ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));
        return;
    }
    switch(btnSenderNo){
    case HDMI ://         = 52;//       - btn150_HDMI_out
        //ToastMsg::show(this, "", tr("click %1-setOutput150.").arg(HDMI));
        //qDebug() << tr("click %1-setOutput150.").arg(HDMI);
        setOutput150(2);
        break;
    case DAC_2010://      = 51;//        - btn150_dac_2010_out
        //ToastMsg::show(this, "", tr("click %1-setOutput150.").arg(DAC_2010));
        //qDebug() << tr("click %1-setOutput150.").arg(DAC_2010);

            setOutput150(3);
       // }

        break;
    }

}

void SettingInputOutput::slot_setOutput520(){//c221013_1
    //ToastMsg::show(this, "", tr("3-setOutput520."));//c220511

    //qDebug() << "output: sender()->property(btnNo).toInt()  = " << sender()->property("btnNo").toInt();
    int btnSenderNo = sender()->property("btnNo").toInt();
  //  int usbDacFlag = false;
    if(inputvalue520 == 4) {
        print_debug();
        qDebug() << "inputvalue520=" << inputvalue520;
        ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));
        return;
    }
    if(this->outputvalue == 4){
        dlgConfirm520 = new DialogConfirm(this, DialogConfirm::DlgConfirmType::OKCancel);
        this->dlgConfirm520->setTitle(tr("Notice"));
        this->dlgConfirm520->setText(tr("Are you sure you want to change the output settings?"));

        int result = this->dlgConfirm520->exec();

        if(result == QDialog::Accepted){
            print_debug();
        }else{
            return;
        }

    }
    switch (btnSenderNo){
    case HDMI ://         = 52;//       - btn520_HDMI_out
        //ToastMsg::show(this, "", tr("click %1-setOutput520.").arg(HDMI));
        //qDebug() << tr("click %1-setOutput520.").arg(HDMI);
        setOutput520(2);
        break;
    case DAC_2010://      = 51;//        - btn520_dac_2010_out
        //ToastMsg::show(this, "", tr("click %1-setOutput520.").arg(DAC_2010));
        //qDebug() << tr("click %1-setOutput520.").arg(DAC_2010);

            setOutput520(3);
       // }

        break;
    }

}

void SettingInputOutput::setUIControl201(QString deviceType){
print_debug();
    QString deviceTypeImgSelected = ":/images/setting/inout_201_in2_gold_selected.png";
    QString deviceTypeImgUnSelected = ":/images/setting/inout_201_in2.png";
    qDebug() << "setUIControl201-device_type : " << deviceType;
    // INPUT Body
    btn_input_Model = new BtnLabel(deviceType, "btn_input_201", deviceTypeImgSelected, deviceTypeImgUnSelected);
    btn_input_Model->setProperty("btnNo", RS201);
    QVBoxLayout *vl_btn_input_Model = new QVBoxLayout();
    vl_btn_input_Model->setContentsMargins(0,30,0,30);
    vl_btn_input_Model->setSpacing(0);
    vl_btn_input_Model->addWidget(btn_input_Model, 0, Qt::AlignCenter);

    btn_opt_in = new BtnLabel(tr("OPT IN"), "btn_opt_in", ":/images/inout_opt_gold_selected.png", ":/images/inout_opt.png");
    btn_opt_in->setFixedSize(100,120);
    btn_opt_in->setProperty("btnNo", OPTIN201);
    QVBoxLayout *vl_btn_opt_in = new QVBoxLayout();
    vl_btn_opt_in->setContentsMargins(0,0,0,0);
    vl_btn_opt_in->setSpacing(0);
    vl_btn_opt_in->addWidget(btn_opt_in);

    btn_line_in_l = new BtnLabel(tr("LINE IN L"), "btn_line_in_l", ":/images/inout_line_in_gold_selected.png", ":/images/inout_line_in.png");
    btn_line_in_l->setFixedSize(100,120);
    btn_line_in_l->setProperty("btnNo", LINEIN_L201);
    QVBoxLayout *vl_btn_line_in_l = new QVBoxLayout();
    vl_btn_line_in_l->setContentsMargins(0,0,0,0);
    vl_btn_line_in_l->setSpacing(0);
    vl_btn_line_in_l->addWidget(btn_line_in_l);
    btn_line_in_r = new BtnLabel(tr("LINE IN R"), "btn_line_in_r", ":/images/inout_line_in_gold_selected.png", ":/images/inout_line_in.png");
    btn_line_in_r->setFixedSize(100,120);
    btn_line_in_r->setProperty("btnNo", LINEIN_R201);
    QVBoxLayout *vl_btn_line_in_r = new QVBoxLayout();
    vl_btn_line_in_r->setContentsMargins(0,0,0,0);
    vl_btn_line_in_r->setSpacing(0);
    vl_btn_line_in_r->addWidget(btn_line_in_r);

    QHBoxLayout *hl_btns_inputLine = new QHBoxLayout();
    hl_btns_inputLine->setContentsMargins(0,40,0,40);
    hl_btns_inputLine->setSpacing(37);
    hl_btns_inputLine->setAlignment(Qt::AlignCenter);
    hl_btns_inputLine->addLayout(vl_btn_opt_in);
    hl_btns_inputLine->addLayout(vl_btn_line_in_l);
    hl_btns_inputLine->addLayout(vl_btn_line_in_r);

    widget_box_local = new QWidget();
    widget_box_local->setObjectName("widget_box_local");
    widget_box_local->setFixedHeight(INPUT_UI_HEIGHT);
    widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
    widget_box_local->setLayout(vl_btn_input_Model);
    widget_box_line = new QWidget();
    widget_box_line->setObjectName("widget_box_line");
    widget_box_line->setFixedHeight(INPUT_UI_HEIGHT);
    widget_box_line->setStyleSheet("#widget_box_line { border:1px solid #707070; }");
    widget_box_line->setLayout(hl_btns_inputLine);

    // OUTPUT Body
    int fixedImgWidth = 140;
    btn_usb = new BtnLabel(tr("USB"), "btn_usb", ":/images/inout_usb_out2_gold_selected.png", ":/images/inout_usb_out2.png");
    btn_usb->setProperty("btnNo", USB_201);
    btn_usb->setFixedSize(fixedImgWidth,170);
    btn_setting_usb = GSCommon::getUIBtnImg("btn_setting_usb", ":/images/fol_edit_ico1_2.png");
    btn_setting_usb->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn_setting_usb->setProperty("btnNo", BTN_USB201);
    btn_setting_usb->setContentsMargins(0,0,0,0);
    btn_setting_usb->setFixedHeight(34);
    btn_setting_usb->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_btn_usb = new QHBoxLayout();
    hl_btn_usb->setContentsMargins(19,19,20,0);
    hl_btn_usb->setSpacing(0);
    hl_btn_usb->addWidget(btn_setting_usb, 0, Qt::AlignTop);
    hl_btn_usb->addWidget(btn_usb, 0, Qt::AlignHCenter);

    widget_box_usb = new QWidget();
    widget_box_usb->setObjectName("widget_box_usb");
    widget_box_usb->setFixedHeight(190);
    widget_box_usb->setStyleSheet("#widget_box_usb { border:1px solid #707070; }");
    widget_box_usb->setLayout(hl_btn_usb);

    btn_hdmi = new BtnLabel(tr("HDMI"), "btn_hdmi", ":/images/inout_HDMI_ARC_IN_gold_selected.png", ":/images/inout_HDMI_ARC_IN.png");
    btn_hdmi->setFixedSize(fixedImgWidth,120);
    btn_hdmi->setProperty("btnNo", HDMI_201);
    btn_setting_hdmi = GSCommon::getUIBtnImg("btn_setting_hdmi", ":/images/fol_edit_ico1_2.png");
    btn_setting_hdmi->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn_setting_hdmi->setProperty("btnNo", BTN_HDMI201);
    btn_setting_hdmi->setContentsMargins(0,0,0,0);
    btn_setting_hdmi->setFixedHeight(34);
    btn_setting_hdmi->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_btn_hdmi = new QHBoxLayout();
    hl_btn_hdmi->setContentsMargins(19,19,20,0);
    hl_btn_hdmi->setSpacing(0);
    hl_btn_hdmi->addWidget(btn_setting_hdmi, 0, Qt::AlignTop);
    hl_btn_hdmi->addWidget(btn_hdmi, 0, Qt::AlignHCenter);
    widget_box_hdmi = new QWidget();
    widget_box_hdmi->setObjectName("widget_box_hdmi");
    widget_box_hdmi->setFixedHeight(190);
    widget_box_hdmi->setStyleSheet("#widget_box_hdmi { border:1px solid #707070; }");
    widget_box_hdmi->setLayout(hl_btn_hdmi);

    btn_opt_out = new BtnLabel(tr("OPT OUT"), "btn_opt_out", ":/images/inout_opt_gold_selected.png", ":/images/inout_opt.png");
    btn_opt_out->setFixedSize(fixedImgWidth,120);
    btn_opt_out->setProperty("btnNo", OPT_OUT201);
    btn_setting_opt = GSCommon::getUIBtnImg("btn_setting_opt", ":/images/fol_edit_ico1_2.png");
    btn_setting_opt->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn_setting_opt->setProperty("btnNo", BTN_OPT201);
    btn_setting_opt->setContentsMargins(0,0,0,0);
    btn_setting_opt->setFixedHeight(34);
    btn_setting_opt->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_btn_opt_out = new QHBoxLayout();
    hl_btn_opt_out->setContentsMargins(19,19,20,0);
    hl_btn_opt_out->setSpacing(0);
    hl_btn_opt_out->addWidget(btn_setting_opt, 0, Qt::AlignTop);
    hl_btn_opt_out->addWidget(btn_opt_out, 0, Qt::AlignHCenter);
    widget_box_opt_out = new QWidget();
    widget_box_opt_out->setObjectName("widget_box_opt_out");
    widget_box_opt_out->setFixedHeight(190);
    widget_box_opt_out->setStyleSheet("#widget_box_opt_out { border:1px solid #707070; }");
    widget_box_opt_out->setLayout(hl_btn_opt_out);
print_debug();
    //-----------------------------------------------------------------------------------------------------
    btn_setting_preamp = GSCommon::getUIBtnImg("btn_setting_preamp", ":/images/fol_edit_ico1_2.png");
    btn_setting_preamp->setFixedSize(ICON_SETTING_WIDTH,ICON_SETTING_WIDTH);
    btn_setting_preamp->setProperty("btnNo", BTN_PREAMP201);
    btn_setting_preamp->setContentsMargins(0,0,0,0);
    btn_setting_preamp->setFixedHeight(34);
    btn_setting_preamp->setCursor(Qt::PointingHandCursor);
    //btn_setting_preamp->setText(tr("Log-out"));
    //btn_logout->setText(tr("로그아웃"));
    //btn_setting_preamp->setStyleSheet(".QPushButton { "
    //                                "background-color:transparent; color:#e6e6e6; font-size:15px; "
    //                                "border:2px solid #666666; border-radius:17px; padding-left:23px; padding-right:23px; "
    //                            "}" );


    btn_preampl_out = new BtnLabel(tr("PREAMP OUT L"), "btn_preampl_out", ":/images/setting/inout_preout_gold_selected.png", ":/images/setting/inout_preout.png");
    btn_preampl_out->setFixedSize(162,120);
    btn_preampl_out->setProperty("btnNo", PREAMP_OUTL201);
    QVBoxLayout *vl_btn_preampl_out = new QVBoxLayout();
    vl_btn_preampl_out->setContentsMargins(0,10,0,10);
    vl_btn_preampl_out->setSpacing(0);
    //  vl_btn_preampl_out->addWidget(btn_setting_preamptt, 0, Qt::AlignTop);
    vl_btn_preampl_out->addWidget(btn_preampl_out, 0, Qt::AlignHCenter);

    btn_preampr_out = new BtnLabel(tr("PREAMP OUT R"), "btn_preampr_out", ":/images/setting/inout_preout_gold_selected.png", ":/images/setting/inout_preout.png");
    btn_preampr_out->setFixedSize(162,120);
    btn_preampr_out->setProperty("btnNo", PREAMP_OUTR201);
    QVBoxLayout *vl_btn_preampr_out = new QVBoxLayout();
    vl_btn_preampr_out->setContentsMargins(0,10,0,10);
    vl_btn_preampr_out->setSpacing(20);
    vl_btn_preampr_out->addWidget(btn_preampr_out, 0, Qt::AlignHCenter);

    btn_ampl_out = new BtnLabel(tr("AMP OUT L"), "btn_ampl_out", ":/images/setting/inout_amp_out2_gold_selected.png", ":/images/setting/inout_amp_out2.png");
    btn_ampl_out->setFixedSize(182,120);
    btn_ampl_out->setProperty("btnNo", AMP_OUTL201);
    QVBoxLayout *vl_btn_ampl_out = new QVBoxLayout();
    vl_btn_ampl_out->setContentsMargins(0,10,0,10);
    vl_btn_ampl_out->setSpacing(0);
    vl_btn_ampl_out->addWidget(btn_ampl_out, 0, Qt::AlignHCenter);

    btn_ampr_out = new BtnLabel(tr("AMP OUT R"), "btn_ampr_out", ":/images/setting/inout_amp_out2_gold_selected.png", ":/images/setting/inout_amp_out2.png");
    btn_ampr_out->setFixedSize(182,120);
    btn_ampr_out->setProperty("btnNo", AMP_OUTR201);
    QHBoxLayout *vl_btn_ampr_out = new QHBoxLayout();
    vl_btn_ampr_out->setContentsMargins(0,10,0,10);
    vl_btn_ampr_out->setSpacing(0);
    vl_btn_ampr_out->addWidget(btn_ampr_out, 0, Qt::AlignHCenter);

print_debug();
    //----------------------------- internal output -----------------------------------------------
    QVBoxLayout *vl_btn_setting_preamp_out = new QVBoxLayout();
    vl_btn_setting_preamp_out->setContentsMargins(0,10,0,10);
    vl_btn_setting_preamp_out->setSpacing(0);
    vl_btn_setting_preamp_out->addWidget(btn_setting_preamp, 0, Qt::AlignTop);
    //---------------------------------------------------------------------------------
    QHBoxLayout *hl_btn201_setting_amp_out = new QHBoxLayout();
    hl_btn201_setting_amp_out->setContentsMargins(0,0,50,0);
    hl_btn201_setting_amp_out->setSpacing(0);
    hl_btn201_setting_amp_out->addLayout(vl_btn_setting_preamp_out);

    hl_btn201_setting_amp_out->addLayout(vl_btn_preampl_out);
    hl_btn201_setting_amp_out->addLayout(vl_btn_preampr_out);
    hl_btn201_setting_amp_out->addLayout(vl_btn_ampl_out);
    hl_btn201_setting_amp_out->addLayout(vl_btn_ampr_out);
    //-------------------------------------------------------------------------------------
    widget_box_setting_amp12_out = new QWidget();
    widget_box_setting_amp12_out->setObjectName("widget_box_setting_amp12_out");
    // widget_box_setting_internal_out->setContentsMargins(30,0,90,0);
    widget_box_setting_amp12_out->setFixedHeight(241);
    widget_box_setting_amp12_out->setFixedWidth(924);

    widget_box_setting_amp12_out->setLayout(hl_btn201_setting_amp_out);

    widget_box_setting_amp12_out->setStyleSheet("#widget_box_setting_amp12_out { border:1px solid #707070; }");

    // OUTPUT 1ROW
    QHBoxLayout *hl_btns_output1 = new QHBoxLayout();
    hl_btns_output1->setContentsMargins(0,10,0,10);
    hl_btns_output1->setSpacing(15);
    hl_btns_output1->addWidget(widget_box_usb);
    hl_btns_output1->addWidget(widget_box_hdmi);
    hl_btns_output1->addWidget(widget_box_opt_out);

    // OUTPUT 2ROW
    QHBoxLayout *hl_btns_output2 = new QHBoxLayout();
    hl_btns_output2->setContentsMargins(0,10,0,10);
    hl_btns_output2->setSpacing(0);
    hl_btns_output2->setAlignment(Qt::AlignCenter);

    hl_btns_output2->addWidget(widget_box_setting_amp12_out);

    QVBoxLayout *vl_output = new QVBoxLayout();
    vl_output->setContentsMargins(33,10,33,10);
    vl_output->setSpacing(0);
    vl_output->addLayout(hl_btns_output1);
    vl_output->addLayout(hl_btns_output2);
    //vl_output->addLayout(hl_btn201_setting_tamp_out);

    fl_input = new FlowLayout();
    fl_input->setContentsMargins(0,0,0,0);
    fl_input->setSpacingHV(16, 10);
    fl_input->heightForWidth(this->width());
    fl_input->addWidget(widget_box_local);
    fl_input->addWidget(widget_box_line);
    QWidget *widget_flow = new QWidget();   // FlowLayout 을 QWidget으로 한번 더 안 감싸고 VBoxLayout에 넣으면 sizeHint.Height가 이상해서 원하는 UI 가 안나와서 QWidget으로 한번 더 감싸음.
    widget_flow->setObjectName("widget_flow");
    widget_flow->setContentsMargins(0,0,0,0);
    widget_flow->setLayout(fl_input);
    widget_flow->setStyleSheet("#widget_flow { border-left:1px solid #303030; }");

    // 전체
    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(this->getUIControlTitle(tr("INPUT")+" ("+global.device.getDeviceName()+")"));//c220521
    vl_total->addSpacing(25);
    //vl_total->addWidget(widget_flow, 0, Qt::AlignHCenter); // note working.. maybe FlowLayout bugs
    vl_total->addWidget(widget_flow);
    vl_total->addSpacing(25);
    vl_total->addWidget(this->getUIControlTitle(tr("OUTPUT")));//cheon12_io
    vl_total->addSpacing(25);//cheon12_io
    vl_total->addLayout(vl_output);//cheon12_io

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

//print_debug();

    // 커넥션
    connect(btn_input_Model, SIGNAL(clicked()), this, SLOT(slot_setInput201()));
    connect(btn_opt_in, SIGNAL(clicked()), this, SLOT(slot_setInput201()));
    connect(btn_line_in_l, SIGNAL(clicked()), this, SLOT(slot_setInput201()));
    connect(btn_line_in_r, SIGNAL(clicked()), this, SLOT(slot_setInput201()));

    connect(btn_usb, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting201()));
    connect(btn_hdmi, SIGNAL(clicked()), this, SLOT(slot_setHdmiOutput201()));
    connect(btn_opt_out, SIGNAL(clicked()), this, SLOT(slot_setOptOutput201()));

    connect(btn_preampl_out, SIGNAL(clicked()), this, SLOT(slot_setInternalOutput_201_1()));
    connect(btn_preampr_out, SIGNAL(clicked()), this, SLOT(slot_setInternalOutput_201_1()));
    connect(btn_ampl_out, SIGNAL(clicked()), this, SLOT(slot_setInternalOutput_201_2()));
    connect(btn_ampr_out, SIGNAL(clicked()), this, SLOT(slot_setInternalOutput_201_2()));

    connect(btn_setting_usb, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting201()));
    connect(btn_setting_hdmi, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting201()));
    connect(btn_setting_opt, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting201()));
    connect(btn_setting_preamp, SIGNAL(clicked()), this, SLOT(slot_setOutputSetting201()));

    this->setLayout(vl_scroll);
}


/**
 * @brief MusicFolder_Colud::showDlgOfSelectCloudType : 클라우드 타입 선택 다이얼로그 show
 */
void SettingInputOutput::showDlgOfHDMI_setting(){

    Dialog::dialogsetting_hdmi *dlg_hdmi_setting = new Dialog::dialogsetting_hdmi(this);


   // dlg_hdmi_setting->set_selected_deviceName("HDMI Setting");
    int result = dlg_hdmi_setting->exec();
    print_debug();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_hdmi_setting;
}

void SettingInputOutput::showDlgOfUSB_setting(){//c220708

    Dialog::dialogsetting_usbsetting *dlg_usb_setting = new Dialog::dialogsetting_usbsetting(this);


   // dlg_hdmi_setting->set_selected_deviceName("HDMI Setting");
    int result = dlg_usb_setting->exec();
    print_debug();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_usb_setting;
}
/**
 * @brief MusicFolder_Colud::showDlgOfSelectCloudType : 클라우드 타입 선택 다이얼로그 show
 */
void SettingInputOutput::showDlgOfOUTPUT_setting(){

    Dialog::dialogsetting_output *dlg_output_setting = new Dialog::dialogsetting_output(this);


   // dlg_opt_setting->set_selected_deviceName("Opt Setting");
    int result = dlg_output_setting->exec();
    qDebug() << "pre-QDialog::Accepted is cliecked";
    if(result == QDialog::Accepted){
        qDebug() << "QDialog::Accepted is cliecked";
    }

    delete  dlg_output_setting;
}
//setting button :
//const int BTN_USB201  = 41;
//const int BTN_HDMI201  = 42;
//const int BTN_OPT201  = 43;
//const int BTN_PREAMP201  = 44;
void SettingInputOutput::slot_setOutputSetting201()
{

    qDebug() << "Internal: sender()->property(btnNo).toInt()  = " << sender()->property("btnNo").toInt();
    int btnSenderNo = sender()->property("btnNo").toInt();
    switch(btnSenderNo){
    case BTN_HDMI201://
        print_debug();
        showDlgOfHDMI_setting();
        break;
    case BTN_USB201://
        showDlgOfUSB_setting();//c220708
       // print_debug();
        break;
    case USB_201://
        select_setUsbOutput201();
       // print_debug();
        break;
    case BTN_OPT201://
        showDlgOfOPT_setting();
        break;
    case BTN_PREAMP201://
        showDlgOfOUTPUT_setting();
        break;

    }


}




void SettingInputOutput::slot_setInput201(){//c220511

    qDebug() << "input: sender()->property(btnNo).toInt()  = " << sender()->property("btnNo").toInt();
    int btnSenderNo = sender()->property("btnNo").toInt();

    switch(btnSenderNo){

    case RS201  ://       = 0;//         - btn201_input_Model
        //ToastMsg::show(this, "", tr("click %1-setInput201.").arg(RS201));
        qDebug() << tr("click %1-setInput201.").arg(RS201);
        //requestInputOutputMode();
        setInput201(RS201);
        break;
    case LINEIN_L201://        = 3;//          - btn201_line_in_l
       // ToastMsg::show(this, "", tr("click %1-setInput201.").arg(LINEIN_L201));
        qDebug() << tr("click %1-setInput201.").arg(LINEIN_L201);
        setInput201(1);
        break;
    case LINEIN_R201://        = 4;//          - btn201_line_in_r
        //ToastMsg::show(this, "", tr("click %1-setInput201.").arg(LINEIN_R201));
        qDebug() << tr("click %1-setInput201.").arg(LINEIN_R201);
        setInput201(1);
        break;
    case OPTIN201://         = 3;//          - btn201_opt_in
        //ToastMsg::show(this, "", tr("click %1-setInput201.").arg(OPTIN201));
        qDebug() << tr("click %1-setInput201.").arg(OPTIN201);
        setInput201(2);
        break;

    }

}

int SettingInputOutput::get_InputMode150()
{
    int a;
    a = inputvalue150;
    return a;
}

int SettingInputOutput::get_InputMode520()
{
    int a;
    a = inputvalue520;
    return a;
}


int SettingInputOutput::get_InputMode250()
{
    int a;
    a = inputvalue250;
    return a;
}
void SettingInputOutput::set_InputMode150(int a)
{

    inputvalue150 =a;

}
void SettingInputOutput::set_InputMode520(int a)
{

    inputvalue520 =a;

}
void SettingInputOutput::set_InputMode250(int a)
{

    inputvalue250  =a;
    qDebug() << "set_InputMode250()-inputvalue250 : " << inputvalue250;

}
int SettingInputOutput::get_InputMode201()
{
    int a;
    a = inputvalue201;
    return a;
}

void SettingInputOutput::set_InputMode201(int a)
{

    inputvalue201  =a;
    qDebug() << "set_InputMode201()-inputvalue201 : " << inputvalue201;

}


/**
 * @brief SettingInputOutput::slot_setInput [SLOT] input 설정
 */
void SettingInputOutput::setInput201(int inputNo){
    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    qDebug() << "setInput201(int inputNo) inputNo = " << inputNo;
    int curInputNo = get_InputMode201();
    qDebug() << "setInput201(int inputNo) curInputNo = " << curInputNo;
    if(inputNo == curInputNo ){
        tmp_json.insert("funcMode", RS201 );
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("input.mode.set"), tmp_json, true);

    }else{
        tmp_json.insert("funcMode", inputNo );
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("input.mode.set"), tmp_json, true);

    }


}


/**
 * @brief SettingInputOutput::getUIControlTitle 타이틀 UI 생성
 * @param p_title
 * @return
 */
QWidget* SettingInputOutput::getUIControlTitle(QString p_title){

    // Text
    QLabel *lb_title = new QLabel(p_title);
  //  QLabel *lb_selected = new QLabel(tr("Selected"));
    QLabel *lb_connected = new QLabel(tr("Enable"));
    QLabel *lb_unConnected = new QLabel(tr("Disable"));

    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;font:bold;");
  //  lb_selected->setStyleSheet("color:#CCCCCC;font-size:18px;");
    lb_connected->setStyleSheet("color:#CCCCCC;font-size:18px;");
    lb_unConnected->setStyleSheet("color:#CCCCCC;font-size:18px;");

    // 범례 Circle
  //  QLabel *lb_selected_legend = new QLabel();
  //  lb_selected_legend->setFixedSize(20,20);
  //  lb_selected_legend->setStyleSheet("background-color:#B18658;border-radius:10px;");
    QLabel *lb_connected_legend = new QLabel();
    lb_connected_legend->setFixedSize(20,20);
    lb_connected_legend->setStyleSheet("background-color:#B18658;border-radius:10px;");
    QLabel *lb_unConnected_legend = new QLabel();
    lb_unConnected_legend->setFixedSize(20,20);
    lb_unConnected_legend->setStyleSheet("background-color:#FFFFFF;border-radius:10px;");

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,15,33,15);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_title, 5);
 //   hl_lb->addWidget(lb_selected_legend);
 //   hl_lb->addSpacing(5);
  //  hl_lb->addWidget(lb_selected);
  //  hl_lb->addSpacing(20);
    hl_lb->addWidget(lb_connected_legend);
    hl_lb->addSpacing(5);
    hl_lb->addWidget(lb_connected);
    hl_lb->addSpacing(20);
    hl_lb->addWidget(lb_unConnected_legend);
    hl_lb->addSpacing(5);
    hl_lb->addWidget(lb_unConnected);

    QWidget *widget_title = new QWidget();
    widget_title->setObjectName("widget_title");
    widget_title->setContentsMargins(0,0,0,0);
    widget_title->setFixedWidth(990);
    widget_title->setLayout(hl_lb);
    widget_title->setStyleSheet("#widget_title { background-color:#333333; } QLabel { background-color:transparent; }");

    return widget_title;
}

/**
 * @brief SettingInputOutput::requestInputOutputMode Input/Output 정보를 가져온다
 */
void SettingInputOutput::requestInputOutputMode(){

    QString deviceType = global.device.getDeviceType();
    print_debug();//cheon0219

    qDebug() << "requestInputOutputMode()start-deviceType : " << deviceType  << "global.device.getDeviceIP() : " << global.device.getDeviceIP();
    if( deviceType == "RS150"){

        btn150_input_Model->setFixedSize(330,150);
        btn150_input_Model->setText(deviceType);
    }
    if( deviceType == "RS150B"){

        btn150_input_Model->setFixedSize(330,150);
        btn150_input_Model->setText(deviceType);
    }
    if( deviceType == "RS201"){

        btn_input_Model->setFixedSize(330,150);
        btn_input_Model->setText(deviceType);
    }
    if( deviceType == "RS520"){

        btn520_input_Model->setFixedSize(330,150);
        btn520_input_Model->setText(deviceType);
    }
    if( deviceType == "RS250"){

        btn150_input_Model->setFixedSize(330,150);
        btn150_input_Model->setText(deviceType);
    }
    if( deviceType == "RS250A"){

        btn150_input_Model->setFixedSize(330,150);
        btn150_input_Model->setText(deviceType);
    }
    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("in.out.mode.get"), tmp_json, true);
    qDebug() << "requestInputOutputMode()end-deviceType : " << deviceType;
}



/**
 * @brief SettingInputOutput::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */

void SettingInputOutput::get_internalData(QString i, int *itm )
{
    QString deviceType = global.device.getDeviceType();
    print_debug();//cheon0219
    QByteArray bytearr = i.toLocal8Bit();
    char *cstr = bytearr.data();
    qDebug() << "deviceType : " << deviceType;
    if( deviceType == "RS150" || deviceType == "RS150B"){
        //qDebug() << "internalMode : " << i;
        itm[0] = cstr[0] - '0';
        itm[1] = cstr[2] - '0';
        itm[2] = cstr[4] - '0';
        itm[3] = cstr[6] - '0';
        itm[4] = cstr[8] - '0';
        itm[5] = cstr[10] - '0';
        itm[6] = cstr[12] - '0';
        itm[7] = cstr[14] - '0';

        QString InternalData;
        for(int i=0; i< 8 ; i++)
        {
           InternalData.append(QString::number(itm[i]));
        }
       // print_debug();//cheon0219
        qDebug() << "get_InternalData : " << InternalData; //cheon10-io
        //qDebug() << "itm0 : " << itm[0] ;
        //qDebug() << "itm2 : " << itm[1] ;
        //qDebug() << "itm4 : " << itm[2] ;
    }
    else if( deviceType == "RS520"){
        //qDebug() << "internalMode : " << i;
        itm[0] = cstr[0] - '0';
        itm[1] = cstr[2] - '0';
        itm[2] = cstr[4] - '0';
        itm[3] = cstr[6] - '0';
        itm[4] = cstr[8] - '0';
        itm[5] = cstr[10] - '0';
        itm[6] = cstr[12] - '0';
        itm[7] = cstr[14] - '0';

        QString InternalData;
        for(int i=0; i< 8 ; i++)
        {
           InternalData.append(QString::number(itm[i]));
        }
       // print_debug();//cheon0219
        qDebug() << "get_InternalData : " << InternalData; //cheon10-io
        //qDebug() << "itm0 : " << itm[0] ;
        //qDebug() << "itm2 : " << itm[1] ;
        //qDebug() << "itm4 : " << itm[2] ;
    }
    else if( deviceType == "RS201" || deviceType == "RS250" || deviceType == "RS250A"){
        //qDebug() << "internalMode : " << i;

        itm[0] = cstr[0] - '0';
        itm[1] = cstr[2] - '0';
        itm[2] = cstr[4] - '0';
        QString InternalData;
        for(int i=0; i< 3 ; i++)
        {
           InternalData.append(QString::number(itm[i]));
        }
       // print_debug();//cheon0219
        qDebug() << "get_InternalData : " << InternalData; //cheon10-io
        //qDebug() << "itm0 : " << itm[0] ;
        //qDebug() << "itm2 : " << itm[1] ;
        //qDebug() << "itm4 : " << itm[2] ;
    }
    else if( deviceType == "RS301"){

    }
    else if( deviceType == "RS350"){

    }

}

void SettingInputOutput::set_data(int o, int n, int *iarray)
{
    outputvalue = o;
    QString deviceType = global.device.getDeviceType();
    print_debug();//cheon0219

    qDebug() << "set_data-outputvalue:  " << outputvalue;
    if( n == 8 && (deviceType == "RS150" || deviceType == "RS150B")){
        itm8[0] = iarray[0];
        itm8[1] = iarray[1];
        itm8[2] = iarray[2];
        itm8[3] = iarray[3];
        itm8[4] = iarray[4];
        itm8[5] = iarray[5];
        itm8[6] = iarray[6];
        itm8[7] = iarray[7];
        QString InternalData;
        for(int i=0; i< 8 ; i++)
        {
           InternalData.append(QString::number(iarray[i]));
        }
       // print_debug();//cheon0219
        qDebug() << "set_data()_InternalData : " << InternalData; //cheon10-io
    }
    else if( n == 3 && deviceType == "RS201"){
        itm3[0] = iarray[0];
        itm3[1] = iarray[1];
        itm3[2] = iarray[2];
        QString InternalData;
        for(int i=0; i< 3 ; i++)
        {
           InternalData.append(QString::number(iarray[i]));
        }
       // print_debug();//cheon0219
        qDebug() << "set_data()InternalData : " << InternalData; //cheon10-io
    }
    else if( n == 8 && deviceType == "RS520"){
        itm8[0] = iarray[0];
        itm8[1] = iarray[1];
        itm8[2] = iarray[2];
        itm8[3] = iarray[3];
        itm8[4] = iarray[4];
        itm8[5] = iarray[5];
        itm8[6] = iarray[6];
        itm8[7] = iarray[7];
        QString InternalData;
        for(int i=0; i< 8 ; i++)
        {
           InternalData.append(QString::number(iarray[i]));
        }
       // print_debug();//cheon0219
        qDebug() << "set_data()_InternalData : " << InternalData;
    }
    else if( n == 3 && deviceType == "RS250"){
        itm3[0] = iarray[0];
        itm3[1] = iarray[1];
        itm3[2] = iarray[2];
        QString InternalData;
        for(int i=0; i< 3 ; i++)
        {
           InternalData.append(QString::number(iarray[i]));
        }
       // print_debug();//cheon0219
        qDebug() << "set_data()InternalData : " << InternalData; //cheon10-io
    }
    else if( n == 3 && deviceType == "RS250A"){
        itm3[0] = iarray[0];
        itm3[1] = iarray[1];
        itm3[2] = iarray[2];
        QString InternalData;
        for(int i=0; i< 3 ; i++)
        {
           InternalData.append(QString::number(iarray[i]));
        }
       // print_debug();//cheon0219
        qDebug() << "set_data()InternalData : " << InternalData; //cheon10-io
    }
    else if( deviceType == "RS301"){

    }
    else if( deviceType == "RS350"){

    }

}
void SettingInputOutput::set_data(QString internaldata)
{

    QString deviceType = global.device.getDeviceType();
    print_debug();//cheon0219
    if( deviceType == "RS150" || deviceType == "RS150B"){
        internalDataString = internaldata;
        qDebug() << "set_data(QString internaldata) : " << internalDataString ;
    }else if(  deviceType == "RS520" || deviceType == "RS201" || deviceType == "RS250" || deviceType == "RS250A"){
        internalDataString = internaldata;
        qDebug() << "set_data(QString internaldata) : " << internalDataString ;
    }else if( deviceType == "RS301"){

    }else if( deviceType == "RS350"){

    }
}

QString  SettingInputOutput::get_data()
{

    QString deviceType = global.device.getDeviceType();
    print_debug();//cheon0219


    if( deviceType == "RS150" || deviceType == "RS150B"){
        QString  internaldata = internalDataString;
        qDebug() << "QString  SettingInputOutput::get_data() : " << internaldata ;
        return internaldata;
    }else if( deviceType == "RS250" || deviceType == "RS250A"){
        QString  internaldata = internalDataString;
        qDebug() << "QString  SettingInputOutput::get_data() : " << internaldata ;
        return internaldata;

    }else if( deviceType == "RS201"){
        QString  internaldata = internalDataString;
        qDebug() << "QString  SettingInputOutput::get_data() : " << internaldata ;
        return internaldata;
    }else if( deviceType == "RS520"){
        QString  internaldata = internalDataString;
        qDebug() << "QString  SettingInputOutput::get_data() : " << internaldata ;
        return internaldata;
    }else if( deviceType == "RS301"){

    }else if( deviceType == "RS350"){

    }
       return nullptr;
}

int SettingInputOutput::get_data(int n, int *iarray)
{
    int o = outputvalue;
    QString deviceType = global.device.getDeviceType();
  //  print_debug();//cheon0219

    if(n == 8 &&  (deviceType == "RS150" || deviceType == "RS150B")){
        qDebug() << "*get_data-outputvalue:  " << outputvalue;
        iarray[0] = itm8[0];
   //     print_debug();//cheon0219
        iarray[1] = itm8[1];//print_debug();//cheon0219
        iarray[2] = itm8[2];
        iarray[3] = itm8[3];//print_debug();//cheon0219
        iarray[4] = itm8[4];
        iarray[5] = itm8[5];
        iarray[6] = itm8[6];
        iarray[7] = itm8[7];//print_debug();//cheon0219
        QString InternalData;
        for(int i=0; i< 8 ; i++)
        {
           InternalData.append(QString::number(iarray[i]));
        }
       // print_debug();//cheon0219
        qDebug() << "get_data()InternalData : " << InternalData; //cheon10-io
        return o;
    }else if(n == 3 && deviceType == "RS201"){
     //   qDebug() << "*get_data-outputvalue:  " << outputvalue;
        iarray[0] = itm3[0];//print_debug();//cheon0219
        iarray[1] = itm3[1];
        iarray[2] = itm3[2];
        QString InternalData;
        for(int i=0; i< 3 ; i++)
        {
           InternalData.append(QString::number(iarray[i]));
        }
       // print_debug();//cheon0219
        qDebug() << "get_data()InternalData : " << InternalData; //cheon10-io
        return o;
    }else if(n == 8 && deviceType == "RS520"){
        qDebug() << "*get_data-outputvalue:  " << outputvalue;
        iarray[0] = itm8[0];
   //     print_debug();//cheon0219
        iarray[1] = itm8[1];//print_debug();//cheon0219
        iarray[2] = itm8[2];
        iarray[3] = itm8[3];//print_debug();//cheon0219
        iarray[4] = itm8[4];
        iarray[5] = itm8[5];
        iarray[6] = itm8[6];
        iarray[7] = itm8[7];//print_debug();//cheon0219
        QString InternalData;
        for(int i=0; i< 8 ; i++)
        {
           InternalData.append(QString::number(iarray[i]));
        }
       // print_debug();//cheon0219
        qDebug() << "get_data()InternalData : " << InternalData; //cheon10-io
        return o;
       }else if(n == 3 && (deviceType == "RS250" || deviceType == "RS250A")){
        //   qDebug() << "*get_data-outputvalue:  " << outputvalue;
           iarray[0] = itm3[0];//print_debug();//cheon0219
           iarray[1] = itm3[1];
           iarray[2] = itm3[2];
           QString InternalData;
           for(int i=0; i< 3 ; i++)
           {
              InternalData.append(QString::number(iarray[i]));
           }
          // print_debug();//cheon0219
           qDebug() << "get_data()InternalData : " << InternalData; //cheon10-io
           return o;
    }else if( deviceType == "RS301"){
        return o;
    }else if( deviceType == "RS350"){
        return o;
    }
    return 0;
}

QString SettingInputOutput::get_UsbName()
{
  //  qDebug() << "*get-mProductName:  " << mProductName;

    QString a = mProductName;
    qDebug() << "get-mProductName:  " << a;
    return a;
}

QString SettingInputOutput::get_UsbName(int *a, int *c)
{

    *a = mProductId;
    QString b = mProductName;
    *c = mVendorId;
    qDebug() << "get-mProductId:  " << *a;
    qDebug() << "get-mProductName:  " << b;
    qDebug() << "get-mVendorId:  " << *c;
     return b;
}

void SettingInputOutput::set_UsbName(QString a)
{

    mProductName = a;
    QString deviceType = global.device.getDeviceType();
    if( deviceType == "RS150" || deviceType == "RS150B" ||deviceType == "RS250" || deviceType == "RS250B"){
        btn150_dac_2010_out->setText(a);
    }else if( deviceType == "RS520" || deviceType == "RS520B"){
            btn520_dac_2010_out->setText(a);

    }else if( deviceType == "RS201" ){
        btn_usb->setText(a);

    }
    qDebug() << "*set-mProductName:  " << mProductName;
}

void SettingInputOutput::set_UsbName(int  a, QString b, int c)
{
    mProductId = a;
    mProductName = b;
    mVendorId = c;
    QString deviceType = global.device.getDeviceType();
    if( deviceType == "RS150" || deviceType == "RS150B" ||deviceType == "RS250" || deviceType == "RS250B"){
        btn150_dac_2010_out->setText(b);
    }else if( deviceType == "RS520" || deviceType == "RS520B"){
            btn520_dac_2010_out->setText(b);

    }else if( deviceType == "RS201" ){
        btn_usb->setText(b);

    }
    qDebug() << "*set-mProductName:  " << mProductName;
}


QJsonObject SettingInputOutput::ObjectFromString(const QString& in)
{
    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(in.toUtf8());
    // check validity of the document
    if(!doc.isNull())
    {
        if(doc.isObject()) {
            obj = doc.object();
        }
        else {
            //qDebug() << "Document is not an object" << endl;
        }
    } else {
        //qDebug() << "Invalid JSON...\n" << in << endl;
    }
    return obj;
}

void SettingInputOutput::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    QString deviceType = global.device.getDeviceType();
  //  print_debug();//cheon0219
qDebug() << "requestInputOutputMode()start-deviceType : " << deviceType  << ", global.device.getDeviceIP() : " << global.device.getDeviceIP();
    print_debug();//cheon0219
   // qDebug() << "setUIControl()-deviceType : " << deviceType;
    print_debug();  QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_responseHttp =: " << strJson;//cheon12_io
    //qDebug() << "p_id : " << p_id;
    QString internalMode;
    int outputMode=0; int inputMode = 0, itm3[3], itm8[8];
    switch(p_id){

    case HTTP_GET_MODE :
 //        print_debug(); //cheon14-io
        if(p_jsonObject.contains("funcMode")){
            int inputMode = p_jsonObject["funcMode"].toInt();
            print_debug(); qDebug() << "inputMode : " << inputMode;


            if( deviceType == "RS150" || deviceType == "RS150B" ){
                if(inputMode==4){
                    QString tobj = "btn150_setting_usb_2010";
                    btn150_setting_usb_2010->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn150_setting_hdmi";
                    btn150_setting_hdmi->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn150_setting_internal";
                    btn150_setting_internal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn150_setting_bal";
                    btn150_setting_bal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

                }else{
                    QString tobj = "btn150_setting_usb_2010";
                    btn150_setting_usb_2010->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn150_setting_hdmi";
                    btn150_setting_hdmi->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn150_setting_internal";
                    btn150_setting_internal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn150_setting_bal";
                    btn150_setting_bal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

                }
                set_InputMode150(inputMode);
                this->setInputData(inputMode);
            }else if( deviceType == "RS201"){
                set_InputMode201(inputMode);
                this->setInputData(inputMode);
            }else if( deviceType == "RS520"){

                if(inputMode==4){
                    QString tobj = "btn520_setting_usb_2010";
                    btn520_setting_usb_2010->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn520_setting_hdmi";
                    btn520_setting_hdmi->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn520_setting_internal";
                    btn520_setting_internal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn520_setting_unbal";
                    btn520_setting_unbal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    btn520_setting_speaker->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

                }else{
                    QString tobj = "btn520_setting_usb_2010";
                    btn520_setting_usb_2010->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn520_setting_hdmi";
                    btn520_setting_hdmi->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn520_setting_internal";
                    btn520_setting_internal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn520_setting_unbal";
                    btn520_setting_unbal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    btn520_setting_speaker->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

                }

                set_InputMode520(inputMode);
                this->setInputData(inputMode);
            }else if( deviceType == "RS250" || deviceType == "RS250A"){
                if(inputMode==4){
                    QString tobj = "btn150_setting_usb_2010";
                    btn150_setting_usb_2010->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn150_setting_hdmi";
                    btn150_setting_hdmi->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn150_setting_internal";
                    btn150_setting_internal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn150_setting_bal";
                    btn150_setting_bal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

                }else{
                    QString tobj = "btn150_setting_usb_2010";
                    btn150_setting_usb_2010->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn150_setting_hdmi";
                    btn150_setting_hdmi->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn150_setting_internal";
                    btn150_setting_internal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                    tobj = "btn150_setting_bal";
                    btn150_setting_bal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

                }
                set_InputMode250(inputMode);
                this->setInputData(inputMode);
            }else if( deviceType == "RS350"){

            }
        }

        if(p_jsonObject.contains("internalMode")){
            internalMode = p_jsonObject["internalMode"].toString(); //cheon09_io
            set_data(internalMode);
            print_debug();
            qDebug() << "internalMode : " << internalMode;
            qDebug() << "get-internalMode : " << get_data();
            if( deviceType == "RS150" || deviceType == "RS150B" ){
                this->get_internalData(internalMode, itm8);
            }else if( deviceType == "RS201"){
                this->get_internalData(internalMode, itm3);
            }else if( deviceType == "RS520"){
                this->get_internalData(internalMode, itm8);//c221017_1
            }else if( deviceType == "RS250" || deviceType == "RS250A"){
                this->get_internalData(internalMode, itm3);
            }else if( deviceType == "RS350"){

            }
        }
        if(p_jsonObject.contains("outputMode")){

          //  this->setOutputData(outputMode, itm); //cheon09_io
            if( deviceType == "RS150" || deviceType == "RS150B" ){
                outputMode = p_jsonObject["outputMode"].toInt(); //cheon09_io
                global.device_outputMode = outputMode;
                qDebug() << "outputMode : " << outputMode;
            }else if( deviceType == "RS201"){
                outputMode = p_jsonObject["outputMode"].toInt(); //cheon09_io
                global.device_outputMode = outputMode;
                qDebug() << "outputMode : " << outputMode;
            }else if( deviceType == "RS520"){
                outputMode = p_jsonObject["outputMode"].toInt(); //cheon09_io
                global.device_outputMode = outputMode;
                qDebug() << "outputMode : " << outputMode;
            }else if( deviceType == "RS250" || deviceType == "RS250A"){
                outputMode = p_jsonObject["outputMode"].toInt(); //cheon09_io
                global.device_outputMode = outputMode;
                qDebug() << "outputMode : " << outputMode;
            }else if( deviceType == "RS350"){

            }
        }
        if(p_jsonObject.contains("usbDacItem")){
             QJsonObject tmp_Json = p_jsonObject["usbDacItem"].toObject(); //cheon09_io

             QString tmp_str = tmp_Json["mProductName"].toString();
             if(tmp_Json.contains("mProductName")){
                //qDebug() << "mProductName_is contained";
             }
             QJsonDocument doc(tmp_Json);  QString strJson(doc.toJson(QJsonDocument::Compact));  //qDebug() <<"strJson :"<< strJson;//cheon09_io
             usbName_str = strJson;
             QJsonObject tJson = ObjectFromString(usbName_str);
             QJsonDocument doc1(tJson);  QString strJson1(doc1.toJson(QJsonDocument::Compact));  //qDebug() <<"strJson1 :"<< strJson1;
             //qDebug() << "usbDacItem-mProductName : " << tmp_str;
             set_UsbName(tmp_str);
          //  this->setOutputData(outputMode, itm); //cheon09_io

        }
        if( deviceType == "RS150" || deviceType == "RS150B" ){
            set_data(outputMode, 8, itm8);
            print_debug();//cheon0219
            outputMode = get_data( 8, itm8);
            print_debug();//cheon0219
            //this->setInputData(inputMode);
            this->setOutputData(outputMode); //cheon09_io
            print_debug();//cheon0219
            this->setInternalData(); //cheon09_io
            print_debug();//cheon0219
        }else if( deviceType == "RS201"){
            set_data(outputMode, 3, itm3);
            outputMode = get_data( 3, itm3);
            print_debug();//cheon0219
            //this->setInputData(inputMode);
            this->setOutputData(outputMode); //cheon09_io
            this->setInternalData(); //cheon09_io
        }else if( deviceType == "RS520"){
            set_data(outputMode, 8, itm8);
            print_debug();//cheon0219
            outputMode = get_data( 8, itm8);
            print_debug();//cheon0219
            //this->setInputData(inputMode);
            this->setOutputData(outputMode); //cheon09_io
            print_debug();//cheon0219
            this->setInternalData(); //cheon09_io
            print_debug();//cheon0219
        }else if( deviceType == "RS250" || deviceType == "RS250A"){
            set_data(outputMode, 3, itm3);
            outputMode = get_data( 3, itm3);
            print_debug();//cheon0219
            //this->setInputData(inputMode);
            this->setOutputData(outputMode); //cheon09_io
            this->setInternalData(); //cheon09_io
        }else if( deviceType == "RS301"){

        }else if( deviceType == "RS350"){

        }
        break;
    case HTTP_SET_MODE :
// print_debug(); //cheon14-io

        if(p_jsonObject.contains("funcMode")){
            inputMode = p_jsonObject["funcMode"].toInt();
            //print_debug(); //cheon14-io
            qDebug() << "HTTP_SET_MODEfuncMode  = " << inputMode;


            if( deviceType == "RS150" || deviceType == "RS150B" ){
                set_InputMode150(inputMode);
                this->setInputData(inputMode);
            }else if( deviceType == "RS201"){
                set_InputMode201(inputMode);
                this->setInputData(inputMode);
            }else if( deviceType == "RS520"){
                set_InputMode520(inputMode);
                this->setInputData(inputMode);
            }else if( deviceType == "RS250" || deviceType == "RS250A"){
                set_InputMode250(inputMode);
                this->setInputData(inputMode);
            }else if( deviceType == "RS350"){

            }
//            this->setInputData(inputMode);
        }
        if(p_jsonObject.contains("internalMode")){
            internalMode = p_jsonObject["internalMode"].toString(); //cheon14_io
            set_data(p_jsonObject["internalMode"].toString());
            //print_debug(); //cheon14-io
            qDebug() << "HTTP_SET_MODEinternalMode  = " << internalMode;
            if( deviceType == "RS150" || deviceType == "RS150B" ){
                this->get_internalData(internalMode, itm8);
            }else if( deviceType == "RS201"){
                this->get_internalData(internalMode, itm3);
            }else if( deviceType == "RS520"){
                this->get_internalData(internalMode, itm3);
            }else if( deviceType == "RS301"){

            }else if( deviceType == "RS350"){

            }
        }
        if(p_jsonObject.contains("outputMode")){
            print_debug();
            outputMode = p_jsonObject["outputMode"].toInt(); //cheon14_io
            //print_debug(); //cheon14-io
            qDebug() << "HTTP_SET_MODEoutputMode  = " << outputMode;

        }
        if( deviceType == "RS150" || deviceType == "RS150B" ){
            set_data(outputMode, 8, itm8);
            print_debug();//cheon0219
            outputMode = get_data( 8, itm8);
            print_debug();//cheon0219
            if(inputMode==4){
                QString tobj = "btn150_setting_usb_2010";
                btn150_setting_usb_2010->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn150_setting_hdmi";
                btn150_setting_hdmi->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn150_setting_internal";
                btn150_setting_internal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn150_setting_bal";
                btn150_setting_bal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

            }else{
                QString tobj = "btn150_setting_usb_2010";
                btn150_setting_usb_2010->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn150_setting_hdmi";
                btn150_setting_hdmi->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn150_setting_internal";
                btn150_setting_internal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn150_setting_bal";
                btn150_setting_bal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

            }
            this->setInputData(inputMode);

            this->setOutputData(outputMode);
            this->setInternalData();

        }else if( deviceType == "RS201"){
            set_data(outputMode, 3, itm3);
            print_debug();//cheon0219
            outputMode = get_data( 3, itm3);
            print_debug();//cheon0219
            this->setInputData(inputMode);

            this->setOutputData(outputMode);
            this->setInternalData();

        }else if( deviceType == "RS520"){
            set_data(outputMode, 8, itm8);
            print_debug();//cheon0219
            outputMode = get_data( 8, itm8);
            print_debug();//cheon0219

            if(inputMode==4){
                QString tobj = "btn520_setting_usb_2010";
                btn520_setting_usb_2010->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn520_setting_hdmi";
                btn520_setting_hdmi->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn520_setting_internal";
                btn520_setting_internal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn520_setting_unbal";
                btn520_setting_unbal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                btn520_setting_speaker->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

            }else{
                QString tobj = "btn520_setting_usb_2010";
                btn520_setting_usb_2010->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn520_setting_hdmi";
                btn520_setting_hdmi->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn520_setting_internal";
                btn520_setting_internal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn520_setting_unbal";
                btn520_setting_unbal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                btn520_setting_speaker->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

            }
            this->setInputData(inputMode);

            this->setOutputData(outputMode);
            this->setInternalData();

        }else if( deviceType == "RS250" || deviceType == "RS250A"){
            set_data(outputMode, 3, itm3);
            print_debug();//cheon0219
            outputMode = get_data( 3, itm3);
            print_debug();//cheon0219
            if(inputMode==4){
                QString tobj = "btn150_setting_usb_2010";
                btn150_setting_usb_2010->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn150_setting_hdmi";
                btn150_setting_hdmi->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn150_setting_internal";
                btn150_setting_internal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn150_setting_bal";
                btn150_setting_bal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

            }else{
                QString tobj = "btn150_setting_usb_2010";
                btn150_setting_usb_2010->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn150_setting_hdmi";
                btn150_setting_hdmi->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn150_setting_internal";
                btn150_setting_internal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");
                tobj = "btn150_setting_bal";
                btn150_setting_bal->setStyleSheet("#" + tobj + " { background-color:rgba(0,0,0,0); background-image: url('" + ":/images/fol_edit_ico1_2.png" + "'); background-repeat:no-repeat; background-position:center center; } ");

            }
            this->setInputData(inputMode);

            this->setOutputData(outputMode);
            this->setInternalData();

        }else if( deviceType == "RS301"){

        }else if( deviceType == "RS350"){

        }
        break;
    }
    sender()->deleteLater();
}

/**
 * @brief SettingInputOutput::setInputData InputMode에 따라서 데이터 세팅
 * @param inputMode
 */
void SettingInputOutput::setInputData(int inputMode){

    QString deviceType = global.device.getDeviceType();
    print_debug();//cheon0219
    qDebug() << "inputMode: " <<inputMode ;
    if( deviceType == "RS150" || deviceType == "RS150B" ){
        if(inputMode==0){
            widget_box_local->setStyleSheet("#widget_box_local { border:3px solid #B18658; }");//print_debug();//cheon0219

            widget_box_line->setStyleSheet("#widget_box_line { border:1px solid #707070; }");//print_debug();//cheon0219
            btn150_input_Model->setStyleSelected(true);//print_debug();//cheon0219
            btn150_line_in_l->setStyleSelected(false);//print_debug();//cheon0219
            btn150_line_in_r->setStyleSelected(false);//print_debug();//cheon0219
            btn150_opt_in->setStyleSelected(false);//print_debug();//cheon0219
            btn150_coax_in->setStyleSelected(false);
            btn150_HDMI_ARC_in->setStyleSelected(false);//print_debug();//cheon0219
            btn150_USB_in->setStyleSelected(false);//print_debug();//cheon0219
            btn150_AES_EBU_in->setStyleSelected(false);//print_debug();//cheon0219

        }else if(inputMode==1){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn150_input_Model->setStyleSelected(false);
            btn150_line_in_l->setStyleSelected(true);
            btn150_line_in_r->setStyleSelected(true);
            btn150_opt_in->setStyleSelected(false);
            btn150_coax_in->setStyleSelected(false);
            btn150_HDMI_ARC_in->setStyleSelected(false);
            btn150_USB_in->setStyleSelected(false);
            btn150_AES_EBU_in->setStyleSelected(false);
    print_debug();//cheon0219
        }else if(inputMode==2){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn150_input_Model->setStyleSelected(false);
            btn150_line_in_l->setStyleSelected(false);
            btn150_line_in_r->setStyleSelected(false);
            btn150_opt_in->setStyleSelected(true);
            btn150_coax_in->setStyleSelected(false);
            btn150_HDMI_ARC_in->setStyleSelected(false);
            btn150_USB_in->setStyleSelected(false);
            btn150_AES_EBU_in->setStyleSelected(false);
    print_debug();//cheon0219
        }else if(inputMode==3){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn150_input_Model->setStyleSelected(false);
            btn150_line_in_l->setStyleSelected(false);
            btn150_line_in_r->setStyleSelected(false);
            btn150_opt_in->setStyleSelected(false);
            btn150_coax_in->setStyleSelected(false);
            btn150_HDMI_ARC_in->setStyleSelected(true);
            btn150_USB_in->setStyleSelected(false);
            btn150_AES_EBU_in->setStyleSelected(false);
    print_debug();//cheon0219
        }else if(inputMode==4){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn150_input_Model->setStyleSelected(false);
            btn150_line_in_l->setStyleSelected(false);
            btn150_line_in_r->setStyleSelected(false);
            btn150_opt_in->setStyleSelected(false);
            btn150_coax_in->setStyleSelected(false);
            btn150_HDMI_ARC_in->setStyleSelected(false);
            btn150_USB_in->setStyleSelected(true);
            btn150_AES_EBU_in->setStyleSelected(false);
    print_debug();//cheon0219
        }else if(inputMode==5){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn150_input_Model->setStyleSelected(false);
            btn150_line_in_l->setStyleSelected(false);
            btn150_line_in_r->setStyleSelected(false);
            btn150_opt_in->setStyleSelected(false);
            btn150_coax_in->setStyleSelected(false);
            btn150_HDMI_ARC_in->setStyleSelected(false);
            btn150_USB_in->setStyleSelected(false);
            btn150_AES_EBU_in->setStyleSelected(true);
    print_debug();//cheon0219
        }else if(inputMode==130){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn150_input_Model->setStyleSelected(false);
            btn150_line_in_l->setStyleSelected(false);
            btn150_line_in_r->setStyleSelected(false);
            btn150_opt_in->setStyleSelected(false);
            btn150_coax_in->setStyleSelected(true);
            btn150_HDMI_ARC_in->setStyleSelected(false);
            btn150_USB_in->setStyleSelected(false);
            btn150_AES_EBU_in->setStyleSelected(false);
    print_debug();//cheon0219
        }

    }else if( deviceType == "RS520"  ){
        if(inputMode==0){
            widget_box_local->setStyleSheet("#widget_box_local { border:3px solid #B18658; }");//print_debug();//cheon0219

            widget_box_line->setStyleSheet("#widget_box_line { border:1px solid #707070; }");//print_debug();//cheon0219
            btn520_input_Model->setStyleSelected(true);//print_debug();//cheon0219
            btn520_line_in_l->setStyleSelected(false);//print_debug();//cheon0219
            btn520_line_in_r->setStyleSelected(false);//print_debug();//cheon0219
            btn520_opt_in->setStyleSelected(false);//print_debug();//cheon0219
            btn520_coax_in->setStyleSelected(false);
            btn520_HDMI_ARC_in->setStyleSelected(false);//print_debug();//cheon0219
            btn520_USB_in->setStyleSelected(false);//print_debug();//cheon0219
            btn520_AES_EBU_in->setStyleSelected(false);//print_debug();//cheon0219

        }else if(inputMode==1){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn520_input_Model->setStyleSelected(false);
            btn520_line_in_l->setStyleSelected(true);
            btn520_line_in_r->setStyleSelected(true);
            btn520_opt_in->setStyleSelected(false);
            btn520_coax_in->setStyleSelected(false);
            btn520_HDMI_ARC_in->setStyleSelected(false);
            btn520_USB_in->setStyleSelected(false);
            btn520_AES_EBU_in->setStyleSelected(false);
    print_debug();//cheon0219
        }else if(inputMode==2){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn520_input_Model->setStyleSelected(false);
            btn520_line_in_l->setStyleSelected(false);
            btn520_line_in_r->setStyleSelected(false);
            btn520_opt_in->setStyleSelected(true);
            btn520_coax_in->setStyleSelected(false);
            btn520_HDMI_ARC_in->setStyleSelected(false);
            btn520_USB_in->setStyleSelected(false);
            btn520_AES_EBU_in->setStyleSelected(false);
    print_debug();//cheon0219
        }else if(inputMode==3){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn520_input_Model->setStyleSelected(false);
            btn520_line_in_l->setStyleSelected(false);
            btn520_line_in_r->setStyleSelected(false);
            btn520_opt_in->setStyleSelected(false);
            btn520_coax_in->setStyleSelected(false);
            btn520_HDMI_ARC_in->setStyleSelected(true);
            btn520_USB_in->setStyleSelected(false);
            btn520_AES_EBU_in->setStyleSelected(false);
    print_debug();//cheon0219
        }else if(inputMode==4){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn520_input_Model->setStyleSelected(false);
            btn520_line_in_l->setStyleSelected(false);
            btn520_line_in_r->setStyleSelected(false);
            btn520_opt_in->setStyleSelected(false);
            btn520_coax_in->setStyleSelected(false);
            btn520_HDMI_ARC_in->setStyleSelected(false);
            btn520_USB_in->setStyleSelected(true);
            btn520_AES_EBU_in->setStyleSelected(false);
    print_debug();//cheon0219
        }else if(inputMode==5){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn520_input_Model->setStyleSelected(false);
            btn520_line_in_l->setStyleSelected(false);
            btn520_line_in_r->setStyleSelected(false);
            btn520_opt_in->setStyleSelected(false);
            btn520_coax_in->setStyleSelected(false);
            btn520_HDMI_ARC_in->setStyleSelected(false);
            btn520_USB_in->setStyleSelected(false);
            btn520_AES_EBU_in->setStyleSelected(true);
    print_debug();//cheon0219
        }else if(inputMode==130){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn520_input_Model->setStyleSelected(false);
            btn520_line_in_l->setStyleSelected(false);
            btn520_line_in_r->setStyleSelected(false);
            btn520_opt_in->setStyleSelected(false);
            btn520_coax_in->setStyleSelected(true);
            btn520_HDMI_ARC_in->setStyleSelected(false);
            btn520_USB_in->setStyleSelected(false);
            btn520_AES_EBU_in->setStyleSelected(false);
    print_debug();//cheon0219
        }

    }else if( deviceType == "RS201"){
        if(inputMode==0){
            // LOCAL
            //print_debug(); //cheon10-io
            //qDebug() << "inputMode : " << inputMode;
            btn_input_Model->setStyleSelected(true);
            widget_box_local->setStyleSheet("#widget_box_local { border:3px solid #B18658; }");
            btn_opt_in->setStyleSelected(false);
            btn_line_in_l->setStyleSelected(false);
            btn_line_in_r->setStyleSelected(false);
            widget_box_line->setStyleSheet("#widget_box_line { border:1px solid #707070; }");

        }else if(inputMode==1){
            // LINE IN
            //print_debug(); //cheon10-io
            //qDebug() << "inputMode : " << inputMode;
            btn_input_Model->setStyleSelected(false);
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            btn_opt_in->setStyleSelected(false);
            btn_line_in_l->setStyleSelected(true);
            btn_line_in_r->setStyleSelected(true);
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");

        }else if(inputMode==2){
            // OPT IN
            //print_debug(); //cheon10-io
            //qDebug() << "inputMode : " << inputMode;
            btn_input_Model->setStyleSelected(false);
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            btn_opt_in->setStyleSelected(true);
            btn_line_in_l->setStyleSelected(false);
            btn_line_in_r->setStyleSelected(false);
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");

        }
    }else if( deviceType == "RS520"){
        if(inputMode==0){
            // LOCAL
            //print_debug(); //cheon10-io
            //qDebug() << "inputMode : " << inputMode;
            btn_input_Model->setStyleSelected(true);
            widget_box_local->setStyleSheet("#widget_box_local { border:3px solid #B18658; }");
            btn_opt_in->setStyleSelected(false);
            btn_line_in_l->setStyleSelected(false);
            btn_line_in_r->setStyleSelected(false);
            widget_box_line->setStyleSheet("#widget_box_line { border:1px solid #707070; }");

        }else if(inputMode==1){
            // LINE IN
            //print_debug(); //cheon10-io
            //qDebug() << "inputMode : " << inputMode;
            btn_input_Model->setStyleSelected(false);
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            btn_opt_in->setStyleSelected(false);
            btn_line_in_l->setStyleSelected(true);
            btn_line_in_r->setStyleSelected(true);
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");

        }else if(inputMode==2){
            // OPT IN
            //print_debug(); //cheon10-io
            //qDebug() << "inputMode : " << inputMode;
            btn_input_Model->setStyleSelected(false);
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            btn_opt_in->setStyleSelected(true);
            btn_line_in_l->setStyleSelected(false);
            btn_line_in_r->setStyleSelected(false);
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");

        }
    }else if( deviceType == "RS250" || deviceType == "RS250A"){
        if(inputMode==0){
            widget_box_local->setStyleSheet("#widget_box_local { border:3px solid #B18658; }");//print_debug();//cheon0219

            widget_box_line->setStyleSheet("#widget_box_line { border:1px solid #707070; }");//print_debug();//cheon0219
            btn150_input_Model->setStyleSelected(true);//print_debug();//cheon0219
            btn150_line_in_l->setStyleSelected(false);//print_debug();//cheon0219
            btn150_line_in_r->setStyleSelected(false);//print_debug();//cheon0219
            btn150_opt_in->setStyleSelected(false);//print_debug();//cheon0219
            btn150_coax_in->setStyleSelected(false);
         //   btn150_HDMI_ARC_in->setStyleSelected(false);//print_debug();//cheon0219
            btn150_USB_in->setStyleSelected(false);//print_debug();//cheon0219
          //  btn150_AES_EBU_in->setStyleSelected(false);//print_debug();//cheon0219

        }else if(inputMode==1){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn150_input_Model->setStyleSelected(false);
            btn150_line_in_l->setStyleSelected(true);
            btn150_line_in_r->setStyleSelected(true);
            btn150_opt_in->setStyleSelected(false);
            btn150_coax_in->setStyleSelected(false);
          //  btn150_HDMI_ARC_in->setStyleSelected(false);
            btn150_USB_in->setStyleSelected(false);
          //  btn150_AES_EBU_in->setStyleSelected(false);
    print_debug();//cheon0219
        }else if(inputMode==2){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn150_input_Model->setStyleSelected(false);
            btn150_line_in_l->setStyleSelected(false);
            btn150_line_in_r->setStyleSelected(false);
            btn150_opt_in->setStyleSelected(true);
            btn150_coax_in->setStyleSelected(false);
         //   btn150_HDMI_ARC_in->setStyleSelected(false);
            btn150_USB_in->setStyleSelected(false);
         //   btn150_AES_EBU_in->setStyleSelected(false);
    print_debug();//cheon0219
        }else if(inputMode==3){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn150_input_Model->setStyleSelected(false);
            btn150_line_in_l->setStyleSelected(false);
            btn150_line_in_r->setStyleSelected(false);
            btn150_opt_in->setStyleSelected(false);
            btn150_coax_in->setStyleSelected(false);
          //  btn150_HDMI_ARC_in->setStyleSelected(true);
            btn150_USB_in->setStyleSelected(false);
          //  btn150_AES_EBU_in->setStyleSelected(false);
    print_debug();//cheon0219
        }else if(inputMode==4){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn150_input_Model->setStyleSelected(false);
            btn150_line_in_l->setStyleSelected(false);
            btn150_line_in_r->setStyleSelected(false);
            btn150_opt_in->setStyleSelected(false);
            btn150_coax_in->setStyleSelected(false);
         //   btn150_HDMI_ARC_in->setStyleSelected(false);
            btn150_USB_in->setStyleSelected(true);
          //  btn150_AES_EBU_in->setStyleSelected(false);
    print_debug();//cheon0219
        }else if(inputMode==5){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn150_input_Model->setStyleSelected(false);
            btn150_line_in_l->setStyleSelected(false);
            btn150_line_in_r->setStyleSelected(false);
            btn150_opt_in->setStyleSelected(false);
            btn150_coax_in->setStyleSelected(false);
         //   btn150_HDMI_ARC_in->setStyleSelected(false);
            btn150_USB_in->setStyleSelected(false);
           // btn150_AES_EBU_in->setStyleSelected(true);
    print_debug();//cheon0219
        }else if(inputMode==130){
            widget_box_local->setStyleSheet("#widget_box_local { border:1px solid #707070; }");
            widget_box_line->setStyleSheet("#widget_box_line { border:3px solid #B18658; }");
            btn150_input_Model->setStyleSelected(false);
            btn150_line_in_l->setStyleSelected(false);
            btn150_line_in_r->setStyleSelected(false);
            btn150_opt_in->setStyleSelected(false);
            btn150_coax_in->setStyleSelected(true);
         //   btn150_HDMI_ARC_in->setStyleSelected(false);
            btn150_USB_in->setStyleSelected(false);
          //  btn150_AES_EBU_in->setStyleSelected(false);
    print_debug();//cheon0219
        }
    }else if( deviceType == "RS350"){

    }

}

bool SettingInputOutput::compareInternalMode(int *itd, int n, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8 )
{
    if(n == 3){
        if(itd[0]==a1 && itd[1]==a2 && itd[2]==a3){
            return true;
        }else return false;
    }else if(n == 8){
        if(itd[0]==a1 && itd[1]==a2 && itd[2]==a3  && itd[3]==a4 && itd[4]==a5 && itd[5]==a6 && itd[6]==a7 && itd[7]==a8){
            return true;
        }else return false;
    }

   return false;
}

void SettingInputOutput::internalMode_display(QString status){
    if(status[2] == '1') {
        btn150_coax_out->setStyleSelected(true);
        widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:3px solid #B18658; }");
    }
    if(status[4] == '1') {
        btn150_i2s_rj45_out->setStyleSelected(true);
        widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:3px solid #B18658; }");
    }
    if(status[6] == '1') {
        btn150_AES_EBU_out->setStyleSelected(true);
        widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:3px solid #B18658; }");
    }
    if(status[8] == '1') {
        btn150_i2s_dvi_out->setStyleSelected(true);
        widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:3px solid #B18658; }");
    }
    if(status[10] == '1') {
        btn150_opt_out->setStyleSelected(true);
        widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:3px solid #B18658; }");
    }
    if(status[12] == '1') {
        btn150_balanced_out_l->setStyleSelected(true);
        btn150_balanced_out_r->setStyleSelected(true);
        widget_box_setting_bal12_out->setStyleSheet("#widget_box_setting_bal12_out { border:3px solid #B18658; }");

        btn150_unbal2_out_l->setStyleSelected(true);
        btn150_unbal2_out_r->setStyleSelected(true);
    }
}

void SettingInputOutput::internalMode_display520(QString status){
    print_debug();
    qDebug() << "status.size()=" << status.size();
    qDebug() << "status)=" << status;
    if(status[0] == '1') {
        btn520_coax_out->setStyleSelected(true);
        widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:3px solid #B18658; }");
    }
    if(status[2] == '1') {
        btn520_opt_out->setStyleSelected(true);
        widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:3px solid #B18658; }");
    }
    if(status[4] == '1') {
        btn520_unbalance_out_l->setStyleSelected(true);
        btn520_unbalance_out_r->setStyleSelected(true);
         widget_box_setting_unbalance_out->setStyleSheet("#widget_box_setting_unbalance_out { border:3px solid #B18658; }");
            }
    if(status[6] == '1') {
        btn520_speaker_out->setStyleSelected(true);
        btn520_speaker_out->setStyleSelected(true);
        widget_box_setting_speaker_out->setStyleSheet("#widget_box_setting_speaker_out { border:3px solid #B18658; }");


    }
}

void SettingInputOutput::internalMode_display250(QString status){
    if(status[0] == '1') {
        print_debug();
        btn150_coax_out->setStyleSelected(true);
        widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:3px solid #B18658; }");
    }

    if(status[2] == '1') {
        print_debug();
        btn150_opt_out->setStyleSelected(true);
        widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:3px solid #B18658; }");
    }
    if(status[4] == '1') {
        print_debug();
        widget_box_setting_bal12_out->setStyleSheet("#widget_box_setting_bal12_out { border:3px solid #B18658; }");

        btn150_unbal2_out_l->setStyleSelected(true);
        btn150_unbal2_out_r->setStyleSelected(true);
    }
}

void SettingInputOutput::setInternalData(){  //c221013_2

    int output_data, itm3_data[3], itm8_data[8];
    QString deviceType = global.device.getDeviceType();
    print_debug();
    int curr_input_mode = get_curr_input_mode(deviceType);
    /*
    if(pre_input_mode >= 0){
        if(curr_input_mode != pre_input_mode){
            print_debug();
            input_mode_flag = true;
           // return;
        }else{
            input_mode_flag = false;
        }
        //return;
    }else{
        input_mode_flag = true;
    }
    pre_input_mode = curr_input_mode;
    */
    if( deviceType == "RS150" || deviceType == "RS150B"){

        output_data = get_data(8, itm8_data);
        QString status = get_data();
        btn150_coax_out->setStyleSelected(false);
        btn150_i2s_rj45_out->setStyleSelected(false);
        btn150_AES_EBU_out->setStyleSelected(false);
        btn150_i2s_dvi_out->setStyleSelected(false);
        btn150_opt_out->setStyleSelected(false);
        btn150_balanced_out_l->setStyleSelected(false);
        btn150_balanced_out_r->setStyleSelected(false);
        btn150_unbal2_out_l->setStyleSelected(false);
        btn150_unbal2_out_r->setStyleSelected(false);
        widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:1px solid #707070; }");
        widget_box_setting_bal12_out->setStyleSheet("#widget_box_setting_bal12_out { border:1px solid #707070; }");
        print_debug();//cheon0219
        qDebug() << "output_data: " <<  output_data <<  ",inputvalue150 :  " <<inputvalue150 ;
        if(output_data == 1 || output_data == 4 ){//|| inputvalue150 == 4 ){//c221017_1
        //if(output_data == 1 || output_data == 4 ){
          print_debug();//cheon0219
          internalMode_display(status);

        }

    }else if( deviceType == "RS250" || deviceType == "RS250A"){

        output_data = get_data(3, itm3_data);
        QString status = get_data();
        btn150_coax_out->setStyleSelected(false);
      //  btn150_i2s_rj45_out->setStyleSelected(false);
      //  btn150_AES_EBU_out->setStyleSelected(false);
      //  btn150_i2s_dvi_out->setStyleSelected(false);
        btn150_opt_out->setStyleSelected(false);
      //  btn150_balanced_out_l->setStyleSelected(false);
      //  btn150_balanced_out_r->setStyleSelected(false);
        btn150_unbal2_out_l->setStyleSelected(false);
        btn150_unbal2_out_r->setStyleSelected(false);
        widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:1px solid #707070; }");
        widget_box_setting_bal12_out->setStyleSheet("#widget_box_setting_bal12_out { border:1px solid #707070; }");
        print_debug();//cheon0219
        qDebug() << "output_data: " << output_data << ",inputvalue250 :  " << inputvalue250 << ", status :" << status;
        if(inputvalue250 == 4 ){//c221017_1
          print_debug();//cheon0219
          internalMode_display250(status);

        }

    }else if( deviceType == "RS201"){
        //print_debug(); //cheon10-io
        print_debug();//cheon0219
        output_data = get_data(3, itm3_data);
        print_debug();//cheon0219

        //qDebug() << "outputvalue : " << output_data; //cheon10-io
        btn_preampl_out->setStyleSelected(false);
        btn_preampr_out->setStyleSelected(false);
        btn_ampl_out->setStyleSelected(false);
        btn_ampr_out->setStyleSelected(false);
        btn_opt_out->setStyleSelected(false);
        if(outputvalue == 1){
          if(compareInternalMode(itm3_data, 3, 0,1,0,0,0,0,0,0)){
          //if(itd[0]==0 && itd[1]==1 && itd[2]==0){
            // preamp

            btn_preampl_out->setStyleSelected(true);
            btn_preampr_out->setStyleSelected(true);
            btn_ampl_out->setStyleSelected(false);
            btn_ampr_out->setStyleSelected(false);
            btn_opt_out->setStyleSelected(false);

          }else if(compareInternalMode(itm3_data, 3, 0,1,1,0,0,0,0,0)){// if(itd[0]==0 && itd[1]==1 && itd[2]==1){
            // preamp. amp

              btn_preampl_out->setStyleSelected(true);
              btn_preampr_out->setStyleSelected(true);
              btn_ampl_out->setStyleSelected(true);
              btn_ampr_out->setStyleSelected(true);
              btn_opt_out->setStyleSelected(false);

          }else if(compareInternalMode(itm3_data, 3, 0,0,1,0,0,0,0,0)){//if(itd[0]==0 && itd[1]==0 && itd[2]==1){
            // amp
              btn_preampl_out->setStyleSelected(false);
              btn_preampr_out->setStyleSelected(false);
              btn_ampl_out->setStyleSelected(true);
              btn_ampr_out->setStyleSelected(true);
              btn_opt_out->setStyleSelected(false);
          }else if(compareInternalMode(itm3_data, 3, 1,0,0,0,0,0,0,0)){//if(itd[0]==1 && itd[1]==0 && itd[2]==0){
              // amp

                btn_preampl_out->setStyleSelected(false);
                btn_preampr_out->setStyleSelected(false);
                btn_ampl_out->setStyleSelected(false);
                btn_ampr_out->setStyleSelected(false);
                btn_opt_out->setStyleSelected(true);
          }
        }
    }else if( deviceType == "RS520"){
        print_debug();
        output_data = get_data(8, itm8_data);
        QString status = get_data();
        btn520_coax_out->setStyleSelected(false);
        btn520_i2s_rj45_out->setStyleSelected(false);
        btn520_AES_EBU_out->setStyleSelected(false);
        btn520_i2s_dvi_out->setStyleSelected(false);
        btn520_opt_out->setStyleSelected(false);
        btn520_speaker_out->setStyleSelected(false);
        //btn520_balanced_out_r->setStyleSelected(false);
        btn520_unbalance_out_l->setStyleSelected(false);
        btn520_unbalance_out_r->setStyleSelected(false);
        widget_box_setting_internal_out->setStyleSheet("#widget_box_setting_internal_out { border:1px solid #707070; }");
        widget_box_setting_speaker_out->setStyleSheet("#widget_box_setting_speaker_out { border:1px solid #707070; }");
        widget_box_setting_unbalance_out->setStyleSheet("#widget_box_setting_unbalance_out { border:1px solid #707070; }");

        print_debug();
        qDebug() << "output_mode_flag---" << output_mode_flag;
        qDebug() << "output_data: " <<  output_data <<  ",inputvalue520 :  " <<inputvalue520 ;
        if(!output_mode_flag) {
            print_debug();//cheon0219
            internalMode_display520(status);
        }

    }else if( deviceType == "RS301"){

    }else if( deviceType == "RS350"){

    }

}

int SettingInputOutput::get_curr_input_mode(QString deviceType){
    if(deviceType == "RS201"){
        return inputvalue201;
    }else if(deviceType == "RS250A"){
        return inputvalue250;
    }else if(deviceType == "RS250"){
        return inputvalue250;
    }else if(deviceType == "RS150"){
        return inputvalue150;
    }else if(deviceType == "RS150B"){
        return inputvalue150;
    }else if(deviceType == "RS520"){
        return inputvalue520;
    }else{
        return inputvalue520;
    }

}
void SettingInputOutput::setOutputData(int outputMode){  //c221013_2

    QString deviceType = global.device.getDeviceType();
    print_debug();//cheon0219
    qDebug() << "outputMode = " << outputMode;
    int curr_input_mode = get_curr_input_mode(deviceType);
    /*
    if(pre_input_mode >= 0){
        if(curr_input_mode != pre_input_mode){
            print_debug();
            input_mode_flag = true;
            return;
        }else{
            input_mode_flag = false;
        }
        //return;
    }else{
        input_mode_flag = true;
    }
    */
    QString cur_internalData  = get_data();
    qDebug() << "outputvalue = " << outputvalue;
    qDebug() << "cur_internalData = " << cur_internalData;
    qDebug() << "inputvalue520 = " << inputvalue520;
    if( deviceType == "RS150" || deviceType == "RS150B"){
        btn150_dac_2010_out->setStyleSelected(false);
        btn150_HDMI_out->setStyleSelected(false);

        widget_box_hdmi_out->setStyleSheet("#widget_box_hdmi_out { border:1px solid #707070;  }");
        widget_box_dac_2010_out->setStyleSheet("#widget_box_dac_2010_out { border:1px solid #707070;  }");

    }else if( deviceType == "RS201"){
        btn_usb->setStyleSelected(false);
        btn_hdmi->setStyleSelected(false);
        btn_opt_out->setStyleSelected(false);
        widget_box_hdmi->setStyleSheet("#widget_box_hdmi { border:1px solid #707070;  }");
        widget_box_usb->setStyleSheet("#widget_box_usb { border:1px solid #707070;  }");
        widget_box_opt_out->setStyleSheet("#widget_box_opt_out { border:1px solid #707070; }");
    }else if( deviceType == "RS520"){//c221013_2


        btn520_dac_2010_out->setStyleSelected(false);
        btn520_HDMI_out->setStyleSelected(false);

        widget_box_hdmi_out->setStyleSheet("#widget_box_hdmi_out { border:1px solid #707070;  }");
        widget_box_dac_2010_out->setStyleSheet("#widget_box_dac_2010_out { border:1px solid #707070;  }");



    }else if( deviceType == "RS250" || deviceType == "RS250A"){
        btn150_dac_2010_out->setStyleSelected(false);
        btn150_HDMI_out->setStyleSelected(false);

        widget_box_hdmi_out->setStyleSheet("#widget_box_hdmi_out { border:1px solid #707070;  }");
        widget_box_dac_2010_out->setStyleSheet("#widget_box_dac_2010_out { border:1px solid #707070;  }");
    }else if( deviceType == "RS350"){

    }
    if(inputvalue150 == 4 || inputvalue250 == 4) {
        print_debug();//c221017_1
        ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));//c221017_1
        return;
    }
    if(inputvalue520 == 4) {
        print_debug();
        //qDebug() << "inputvalue520=" << inputvalue520;
       // ToastMsg::show(this, "", tr("Please change the usb input mode and try again"));
        //return;
    }
    print_debug();
    qDebug() << "input_mode_flag---" << input_mode_flag;
    qDebug() << "outputvalue---" << outputvalue;
    qDebug() << "get_UsbName()---" << get_UsbName();
    if( outputvalue==3){
//QString decName = get_UsbName();
        //        QString deviceType = global.device.getDeviceType();
        if( deviceType == "RS150" || deviceType == "RS150B" ){
            btn150_dac_2010_out->setStyleSelected(true);
            widget_box_dac_2010_out->setStyleSheet("#widget_box_dac_2010_out { border:3px solid #B18658; }");
        }else  if( deviceType == "RS520"){
            print_debug();
            if(outputMode != 4 && (inputvalue520 != 4 && !get_UsbName().isEmpty())) {
                print_debug();
                output_mode_flag = true;
                btn520_dac_2010_out->setStyleSelected(true);
                widget_box_dac_2010_out->setStyleSheet("#widget_box_dac_2010_out { border:3px solid #B18658; }");
            }else{
                output_mode_flag = false;
            }
        }else if( deviceType == "RS201"){
            btn_usb->setStyleSelected(true);
            widget_box_usb->setStyleSheet("#widget_box_usb { border:3px solid #B18658; }");

        }else if( deviceType == "RS250" || deviceType == "RS250A"){
            btn150_dac_2010_out->setStyleSelected(true);
            widget_box_dac_2010_out->setStyleSheet("#widget_box_dac_2010_out { border:3px solid #B18658; }");
        }else if( deviceType == "RS350"){

        }

    }else if(outputvalue==2){
print_debug();
qDebug() << "deviceType=" << deviceType;
        //       QString deviceType = global.device.getDeviceType();
        if( deviceType == "RS150" || deviceType == "RS150B" ){
            btn150_HDMI_out->setStyleSelected(true);
            widget_box_hdmi_out->setStyleSheet("#widget_box_hdmi_out { border:3px solid #B18658; }");
        }else  if( deviceType == "RS520"){
            if(outputMode != 4 && inputvalue520 != 4) {
                print_debug();
                output_mode_flag = true;
                btn520_HDMI_out->setStyleSelected(true);
                widget_box_hdmi_out->setStyleSheet("#widget_box_hdmi_out { border:3px solid #B18658; }");
            }else{
                output_mode_flag = false;
            }
        }else if( deviceType == "RS201"){
            btn_hdmi->setStyleSelected(true);
            widget_box_hdmi->setStyleSheet("#widget_box_hdmi { border:3px solid #B18658; }");
        }else if( deviceType == "RS250" || deviceType == "RS250A"){
            btn150_HDMI_out->setStyleSelected(true);
            widget_box_hdmi_out->setStyleSheet("#widget_box_hdmi_out { border:3px solid #B18658; }");
        }else if( deviceType == "RS350"){

        }
    }else{
        output_mode_flag = false;
    }
}


bool SettingInputOutput::showDlgOfDACMsg(QString title, QString msg){
    DialogConfirm *dlgConfirm = new DialogConfirm(this);

    dlgConfirm->setText(msg);
    //dlgConfirm->setText(tr("DB 초기화시 로즈가 재시작됩니다.\n초기화 하시겠습니까?"));
    dlgConfirm->setTitle(title);
    //dlgConfirm->setTitle(tr("DB 초기화"));
    int tmp_result = dlgConfirm->exec();

    if(tmp_result == QDialog::Accepted){
        delete dlgConfirm;
        return true;

    }else{
        delete dlgConfirm;
        return false;
    }
}


void SettingInputOutput::setConvertMode(QString modeName, QString modeNum, QString cmd){

    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    tmp_json.insert(modeName, modeNum);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg(cmd), tmp_json, true);

}

void SettingInputOutput::slot_setInternalOutput_201_1(){
 //   ToastMsg::show(this, "", tr("Service is being prepared."));

    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    int output_data, itm3_data[3];//, itm8_data[8];

    //print_debug(); //cheon10-io
    print_debug();//cheon0219
    output_data = get_data(3, itm3_data);
    print_debug();//cheon0219

    if(output_data == 2 || output_data == 3){

         bool flag = showDlgOfDACMsg(tr("Internal Mode"), tr("Would you like to set it to Internal mode?"));
         if(flag ) setConvertMode("outputMode", "1", "output.mode.set");
    }else{
        if(compareInternalMode(itm3_data, 3, 0,1,1,0,0,0,0,0)|| compareInternalMode(itm3_data, 3, 0,1,0,0,0,0,0,0)){

        //if(itm_data[0] == 0 && ((itm_data[1] == 1 && itm_data[2] == 1)||(itm_data[1] == 1 && itm_data[2] == 0))){
            tmp_json.insert("internalMode", "1,0,0");
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);

        }
        else if(compareInternalMode(itm3_data, 3, 0,0,1,0,0,0,0,0)){//if(itm_data[1] == 0 && itm_data[2] == 1){
            tmp_json.insert("internalMode", "0,1,1");
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);

        }else{
           tmp_json.insert("internalMode", "0,1,0");
           connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
           network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);
        }
    }
}

void SettingInputOutput::slot_setInternalOutput_201_2(){
 //   ToastMsg::show(this, "", tr("Service is being prepared."));
//print_debug(); //cheon10-io
    QJsonObject tmp_json;

    int output_data, itm3_data[3];//, itm8_data[8];

    //print_debug(); //cheon10-io
    output_data = get_data(3, itm3_data);

    if(output_data == 2 || output_data == 3){
        bool flag = showDlgOfDACMsg(tr("Internal Mode"), tr("Would you like to set it to amp mode?"));
        if(flag ) {

            NetworkHttp *network = new NetworkHttp;
            tmp_json.insert("outputMode", "1");
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);
         }
    }else{
        if(compareInternalMode(itm3_data, 3, 0,1,1,0,0,0,0,0)|| compareInternalMode(itm3_data, 3, 0,0,1,0,0,0,0,0)){

            NetworkHttp *network = new NetworkHttp;
        //if(itm_data[0] == 0 && ((itm_data[1] == 1 && itm_data[2] == 1)||(itm_data[1] == 1 && itm_data[2] == 0))){
            tmp_json.insert("internalMode", "1,0,0");
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);

        }
        else if(compareInternalMode(itm3_data, 3, 0,1,0,0,0,0,0,0)){//if(itm_data[1] == 0 && itm_data[2] == 1){
            tmp_json.insert("internalMode", "0,1,1");
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);

        }else{
           tmp_json.insert("internalMode", "0,0,1");
           NetworkHttp *network = new NetworkHttp;
           connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
           network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);
        }
    }
}

/**
 * @brief MusicFolder_Colud::showDlgOfSelectCloudType : 클라우드 타입 선택 다이얼로그 show
 */
void SettingInputOutput::showDlgOfSelectUsbDac(){

    Dialog::DialogSelect_Usbdac *dlg_usbdac = new Dialog::DialogSelect_Usbdac(this);

    dlg_usbdac->set_selectedUsbName(get_UsbName());
    dlg_usbdac->setTextName(get_UsbName());
    dlg_usbdac->setPropertyName(get_UsbName());

    int result = dlg_usbdac->exec();

    QString tmp_selectType = dlg_usbdac->getSelectedUsbName();
    qDebug() << "tmp_selectType : " << tmp_selectType;
    if(tmp_selectType=="EmptyUsb"){

    }
    if(result == QDialog::Accepted){

        if(tmp_selectType=="Unused"){

            clickUnusedConfirm();
        }
        else if(tmp_selectType!="EmptyUsb"){

            clickUsbConfirm();
        }
    }

    delete  dlg_usbdac;
}


void SettingInputOutput::clickUnusedConfirm(){
   // usbSelected = 1;
    //print_debug(); //cheon10-io
    QJsonObject tmp_json, tmp_json_inner;
    NetworkHttp *network = new NetworkHttp;

        tmp_json.insert("outputMode", "1");
        //QJsonDocument doc2(tmp_json);  QString strJson2(doc2.toJson(QJsonDocument::Compact));  qDebug() <<"strJson2 :"<< strJson2;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_usb(int, QJsonObject)));
        network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);

}

void SettingInputOutput::clickUsbConfirm(){
   // usbSelected = 1;
    print_debug(); //cheon10-io
    qDebug() << "QString().arg(global.device.getDeviceIP()).arg(global.port).arg() = " << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("usb.dac.set");
    QJsonObject tmp_json, tmp_json_inner;
    NetworkHttp *network = new NetworkHttp;
        //////////tmp_json.insert("outputMode", "3");
        //QJsonDocument doc2(tmp_json);  QString strJson2(doc2.toJson(QJsonDocument::Compact));  qDebug() <<"strJson2 :"<< strJson2;
        QJsonObject usbDac_nested_Json;
        QString usbdacitemName;
        int usbdacitemId;
        int usbdacitemVendorId;
        usbdacitemName = get_UsbName(&usbdacitemId, &usbdacitemVendorId);
        qDebug() << "usbdacitemId:" << usbdacitemId << "usbdacitemName" <<usbdacitemName << "usbdacitemVendorId" <<usbdacitemVendorId ;
        usbDac_nested_Json.insert("mProductId", usbdacitemId);
        usbDac_nested_Json.insert("mProductName", usbdacitemName);
        usbDac_nested_Json.insert("mVendorId", usbdacitemVendorId);

        tmp_json.insert("usbDacItem", usbDac_nested_Json);

        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_usb(int, QJsonObject)));
        network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("usb.dac.set"), tmp_json, true);

        ToastMsg::show(this, "", tr("You have choosen USB."));

}

void SettingInputOutput::slot_responseHttp_usb(const int &p_id, const QJsonObject &p_jsonObject){

    //print_debug();  QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_responseHttp_usb =: " << strJson;//cheon12_io
    //qDebug() << "p_id : " << p_id;
    QString internalMode;
    QJsonObject tmp_json;
  //  NetworkHttp *network = new NetworkHttp;
    int outputMode=0, inputMode = 0;
    QString deviceType = global.device.getDeviceType();


    switch(p_id){
    case HTTP_GET_MODE :
        break;
    case HTTP_SET_MODE :
// print_debug(); //cheon14-io
 print_debug();//cheon0219
        QJsonDocument doc2(p_jsonObject);  QString strJson2(doc2.toJson(QJsonDocument::Compact));  qDebug() <<"strJson2 :"<< strJson2;

        if(p_jsonObject.contains("funcMode")){
            inputMode = p_jsonObject["funcMode"].toInt();
            this->setInputData(inputMode);
        }
        if(p_jsonObject.contains("internalMode")){
            internalMode = p_jsonObject["internalMode"].toString(); //cheon14_io
        }
        if(p_jsonObject.contains("outputMode")){
            outputMode = p_jsonObject["outputMode"].toInt(); //cheon14_io
            //print_debug(); //cheon14-io
            //qDebug() << "HTTP_SET_MODEoutputMode  = " << outputMode;

        }
        if(p_jsonObject.contains("usbDacItem")){
             QJsonObject tmp_Json = p_jsonObject["usbDacItem"].toObject(); //cheon09_io

             QString tmp_str = tmp_Json["mProductName"].toString();
             if(tmp_Json.contains("mProductName")){
                //qDebug() << "mProductName_is contained";
             }
             QJsonDocument doc(tmp_Json);  QString strJson(doc.toJson(QJsonDocument::Compact));  //qDebug() <<"strJson :"<< strJson;//cheon09_io
             usbName_str = strJson;
             QJsonObject tJson = ObjectFromString(usbName_str);
             QJsonDocument doc1(tJson);  QString strJson1(doc1.toJson(QJsonDocument::Compact));  //qDebug() <<"strJson1 :"<< strJson1;
             //qDebug() << "usbDacItem-mProductName : " << tmp_str;
             set_UsbName(tmp_str);
          //  this->setOutputData(outputMode, itm); //cheon09_io

        }
        if( deviceType == "RS150" || deviceType == "RS150B"){//||  deviceType == "RS250"){//cheon210809-output
            set_data(outputMode, 8, itm8);
            print_debug();//cheon0219
            outputMode = get_data( 8, itm8);
            print_debug();//cheon0219

            this->setInputData(inputMode);

            this->setOutputData(outputMode);
            this->get_internalData(internalMode, itm8);
            this->setInternalData();

        }else if(deviceType == "RS520" ||   deviceType == "RS201" ||  deviceType == "RS250" || deviceType == "RS250A"){//c220117
            set_data(outputMode, 3, itm3);
            outputMode = get_data( 3, itm3);
            this->setInputData(inputMode);

            this->setOutputData(outputMode);
            this->get_internalData(internalMode, itm3);
            this->setInternalData();

        }else if( deviceType == "RS301"){

        }else if( deviceType == "RS350"){

        }
        if(outputvalue == 3){

            ToastMsg::show(this, "", tr("usb Output is selected."));
        }
        break;
    }
    sender()->deleteLater();
}


void SettingInputOutput::slot_responseHttp_usb_status(const int &p_id, const QJsonObject &p_jsonObject){

    //qDebug() << "p_id : " << p_id;
    QString internalMode;
    QJsonObject tmp_json;
  //  NetworkHttp *network = new NetworkHttp;
    int outputMode=0, inputMode = 0;
    QString deviceType = global.device.getDeviceType();
    print_debug();//cheon0219
    qDebug() << "deviceType : " << deviceType;
    print_debug();  QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_responseHttp_usb_slatus =: " << strJson;//cheon12_io

    if( deviceType == "RS150" || deviceType == "RS150B" ){//||  deviceType == "RS250"){//cheon210809-output
        switch(p_id){
        case HTTP_GET_MODE :
            if(p_jsonObject.contains("funcMode")){
                inputMode = p_jsonObject["funcMode"].toInt();
            }
            if(p_jsonObject.contains("internalMode")){
                internalMode = p_jsonObject["internalMode"].toString(); //cheon14_io
            }
            if(p_jsonObject.contains("outputMode")){
                outputMode = p_jsonObject["outputMode"].toInt(); //cheon14_io
            }
            if(p_jsonObject.contains("arr")){
                QJsonDocument jsonDoc(p_jsonObject);
                int usb_mProductId = jsonDoc.object().value("arr").toArray()[0].toObject().value("mProductId").toInt();
                QString usb_mProductName = jsonDoc.object().value("arr").toArray()[0].toObject().value("mProductName").toString();
                int  usb_mVendorId = jsonDoc.object().value("arr").toArray()[0].toObject().value("mVendorId").toInt();
                 qDebug() << "set_usb_dac : " << usb_mProductId << ", " << usb_mProductName << ", " << usb_mVendorId;
                // qDebug() << "usbDacItem- : " << tmp_usbDacName;
                 set_UsbName(usb_mProductId, usb_mProductName, usb_mVendorId);
            }
            break;
        case HTTP_SET_MODE :

            break;
        }

    }else if(deviceType == "RS520"||   deviceType == "RS201"||  deviceType == "RS250" || deviceType == "RS250A"){//||  deviceType == "RS250"){//cheon210809-output
        switch(p_id){
        case HTTP_GET_MODE :
            print_debug(); //cheon14-io


                   if(p_jsonObject.contains("funcMode")){
                       inputMode = p_jsonObject["funcMode"].toInt();

                   }
                   if(p_jsonObject.contains("internalMode")){
                       internalMode = p_jsonObject["internalMode"].toString(); //cheon14_io


                     //  this->get_internalData(internalMode, itm);
                    //   this->setInternalData(itm);

                   }
                   if(p_jsonObject.contains("outputMode")){
                       outputMode = p_jsonObject["outputMode"].toInt(); //cheon14_io

                   }
                   if(p_jsonObject.contains("arr")){
                       QJsonDocument jsonDoc(p_jsonObject);
                       int usb_mProductId = jsonDoc.object().value("arr").toArray()[0].toObject().value("mProductId").toInt();
                       QString usb_mProductName = jsonDoc.object().value("arr").toArray()[0].toObject().value("mProductName").toString();
                       int  usb_mVendorId = jsonDoc.object().value("arr").toArray()[0].toObject().value("mVendorId").toInt();
                        qDebug() << "set_usb_dac : " << usb_mProductId << ", " << usb_mProductName << ", " << usb_mVendorId;
                       // qDebug() << "usbDacItem- : " << tmp_usbDacName;
                        set_UsbName(usb_mProductId, usb_mProductName, usb_mVendorId);

                   }
                   break;

        case HTTP_SET_MODE :
            break;
        }
    }
    sender()->deleteLater();

}

int SettingInputOutput::get_outputMode()
{
    return outputvalue;
}

void SettingInputOutput::select_setUsbOutput201(){
    setUsbOutput201();

}


void SettingInputOutput::slot_setHdmiOutput201(){
 //   ToastMsg::show(this, "", tr("Service is being prepared."));
//print_debug(); //cheon10-io
    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    int output_data;
    print_debug(); //cheon10-io


    output_data = get_outputMode();

    if(output_data == 2){
        tmp_json.insert("outputMode", "1");
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);
    }else{
        qDebug() << "#######################################################################################################";
        qDebug() << "#######################################################################################################";
        tmp_json.insert("outputMode", "2");
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);
    }

}

void SettingInputOutput::slot_setOptOutput201(){

    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;


    int output_data;
    print_debug(); //cheon10-io


    output_data = get_outputMode();
    //HDMI -> opt, pre 시에는 이전으로

    if(output_data == 2 || output_data == 3){
        bool flag = showDlgOfDACMsg(tr("Internal Mode"), tr("Would you like to set it to opt output mode?"));
        if(flag ) {

            tmp_json.insert("outputMode", "1");
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.mode.set"), tmp_json, true);


         }
    }else{
        QString deviceType = global.device.getDeviceType();
        print_debug();//cheon0219

        if( deviceType == "RS150" || deviceType == "RS150B" ){

        }else if( deviceType == "RS201"){
            tmp_json.insert("internalMode", "1,0,0");
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("internal.set"), tmp_json, true);

        }else if( deviceType == "RS301"){

        }else if( deviceType == "RS350"){

        }
    }

}

/**
 * @brief SettingInputOutput::resizeEvent [override] 리사이즈 이벤트
 * @note FlowLayout이 AlignHCenter 버그인지 not working 하여 수동으로 배치
 * @param event
 */
void SettingInputOutput::resizeEvent(QResizeEvent *event){
    Q_UNUSED(event);            // 미사용 처리 by sunnyfish

    if(this->width()>=(INPUT_UI_HORIZONTAL_MIN_WIDTH + INPUT_UI_HORIZONTAL_MIN_WIDTH + INPUT_UI_MARGIN_LEFT)){
        // 가로 방향 배치 세팅되도록 유도
        fl_input->setContentsMargins(INPUT_UI_MARGIN_LEFT,0,0,0);
        widget_box_local->setMinimumWidth(INPUT_UI_HORIZONTAL_MIN_WIDTH);
        widget_box_line->setMinimumWidth(INPUT_UI_HORIZONTAL_MIN_WIDTH);
    }
    else{
        // 세로 방향 배치 세팅되도록 유도
        int leftMargin = (this->width()-INPUT_UI_VERTICAL_MIN_WIDTH)/2;
        fl_input->setContentsMargins(leftMargin,0,0,0);
        widget_box_local->setMinimumWidth(INPUT_UI_VERTICAL_MIN_WIDTH);
        widget_box_line->setMinimumWidth(INPUT_UI_VERTICAL_MIN_WIDTH);
    }
}


