#include "dialogselect_usbdac_520.h"
#include "common/gscommon.h"
#include "widget/toastmsg.h"
#include "setting/settinginputoutput.h"

#include <QLabel>
#include <QRadioButton>
#include <QDebug>

using namespace Dialog;


/**
 * @brief 클라우드 타입 선택 Dlg
 * @param parent
 */
DialogSelect_Usbdac_520::DialogSelect_Usbdac_520(QWidget *parent)
    : QDialog(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief DialogSelect_Cloud::setInit: 초기 세팅
 */
void DialogSelect_Usbdac_520::setInit(){
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    this->setStyleSheet("background-color:transparent;");
}

/**
 * @brief DialogSelect_Cloud::setUIControl : UI 세팅
 */
void DialogSelect_Usbdac_520::setUIControl(){

    // 타이틀
    QLabel *lb_title = new QLabel;
    lb_title->setText(tr("USB DAC"));//lb_title->setText(tr("클라우드 추가"));
    lb_title->setStyleSheet("font:bold;color:#FFFFFF;font-size:22px;");
    lb_title->setAlignment(Qt::AlignCenter);
    lb_title->setFixedHeight(70);

    QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":images/icon_close_gray.png");
    btn_close->setCursor(Qt::PointingHandCursor);
    btn_close->setFixedSize(QSize(70,70));
    btn_close->setParent(lb_title);
    btn_close->setGeometry(530, 0, 70, 70);
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

    radio_Unused = new QRadioButton;
    radio_Unused->setText("Unused");
    radio_Unused->setStyleSheet(radio_style);
    radio_Unused->setCursor(Qt::PointingHandCursor);
    radio_Unused->setProperty(PROPERTY_NAME_READABLE, "Unused");


    radio_UsbName = new QRadioButton;
    radio_UsbName->setText("EmptyUsb");
    radio_UsbName->setStyleSheet(radio_style);
    radio_UsbName->setCursor(Qt::PointingHandCursor);
    radio_UsbName->setProperty(PROPERTY_NAME_READABLE, "EmptyUsb");

    QVBoxLayout *box_radio = new QVBoxLayout;
    box_radio->setSpacing(0);
    box_radio->setContentsMargins(0,0,0,0);
    box_radio->addWidget(radio_Unused);
    box_radio->addWidget(radio_UsbName);

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
    hl_btn->setContentsMargins(0,0,0,0);
    hl_btn->setSpacing(0);
    hl_btn->setAlignment(Qt::AlignCenter);
    hl_btn->addWidget(btn_ok);
    hl_btn->addSpacing(30);
    hl_btn->addWidget(btn_cancel);



    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(widget_title);
    vl_total->addLayout(box_radio);
    vl_total->addSpacing(100);
    vl_total->addLayout(hl_btn);


    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setLayout(vl_total);
    widget_total->setStyleSheet("#widget_total { background-color:#333333;border-radius:4px;border:1px solid #4D4D4D; }");
    QVBoxLayout *vboxlayout = new QVBoxLayout();
    vboxlayout->setContentsMargins(0,0,0,0);
    vboxlayout->setSpacing(0);
    vboxlayout->addWidget(widget_total);

    this->setLayout(vboxlayout);

    //connect(btn_questionDSD, SIGNAL(clicked()), this, SLOT(slot_clickedDSDquestion()));//
    //connect(btn_questionMQA, SIGNAL(clicked()), this, SLOT(slot_clickedPCMquestion()));//

    // 커넥션
    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clickedAdd()));

}
/*
void DialogSelect_Usbdac_520::slot_clickedDSDquestion(){//c220511
    print_debug();
    DialogConfirm *dlgConfirmOutput = new DialogConfirm(this);
    dlgConfirmOutput->setAlignment(Qt::AlignLeft);
    dlgConfirmOutput->setTitle(tr("DSD Mode"));
    dlgConfirmOutput->setText(tr("Determines the transmission format (Native, DOP, PCM) of audio samples to be delivered to the DAC when playing DSD music. \n When in Native mode, analog output supports up to DSD512 when playing DSD, but digital output does not produce sound. When in DOP mode, analog output does not produce sound and digital output transmits audio data uo to DSD256. For optical, and COAX, it supports only uo to DSD64. In case of DSD to PCM mode, it supprts up to DSD128 and supprts both digital and analog output."));

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

void DialogSelect_Usbdac_520::slot_clickedMQAquestion(){//c220511
    print_debug();
    DialogConfirm *dlgConfirmOpt_coax = new DialogConfirm(this);
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
*/
/**
 * @brief DialogSelect_Cloud::slot_clickedAdd : [슬롯] 추가 버튼 클릭
 */
void DialogSelect_Usbdac_520::slot_clickedAdd(){

    if(radio_Unused->isChecked()){
        selectedUsbName = "Unused";
        setResult(QDialog::Accepted);
        hide();
    }
    else if(radio_UsbName->isChecked()){
        selectedUsbName = get_selectedUsbName();
        setResult(QDialog::Accepted);
        hide();
    }
    else{
        ToastMsg::show(this, "", tr("Please select a USB DAC."));//ToastMsg::show(this, "", tr("클라우드 타입을 선택해주세요."));
    }
}

void DialogSelect_Usbdac_520::set_selectedUsbName(QString usbstr){
    selectedUsbName = usbstr;
}
QString DialogSelect_Usbdac_520::get_selectedUsbName(){
    return selectedUsbName;
}
void DialogSelect_Usbdac_520::setTextName(QString usbstr){
    radio_UsbName->setText(usbstr);
}

void DialogSelect_Usbdac_520::setPropertyName(QString usbstr){
    radio_UsbName->setProperty(PROPERTY_NAME_READABLE, usbstr);
}
