#ifndef SETTINGINPUTOUTPUT_H
#define SETTINGINPUTOUTPUT_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

#include "common/linker.h"//c221013_1

#include "widget/flowlayout.h"
#include "widget/dialogconfirm.h"
//#include "settinginoutsubsetting.h"


/**
 * @brief 버튼 형식의 라벨
 * @note 설정-입출력설정에서 사용
 */
class BtnLabel : public QPushButton
{
    Q_OBJECT
public:
    BtnLabel(QString p_text, QString p_objName, QString p_imgPath_selected, QString p_imgPath_unSelected, QWidget *parent = nullptr);
    void setStyleSelected(bool p_flagSelected);
    void setStyleText(QString a);
    void setText(QString p_text);
    void setImgPath(QString p_imgPath_selected, QString p_imgPath_unSelected);

private:
    QLabel *lb_image;
    QLabel *lb_text;
    //QString imgPath_selected;
    //QString imgPath_unSelected;

    QPixmap *pixmap_selected;
    QPixmap *pixmap_unSelected;

};

/*
QString deviceType = global.device.getDeviceType();
if( deviceType == "RS150"){

}else if( deviceType == "RS201"){

}else if( deviceType == "RS301"){

}else if( deviceType == "RS350"){

}
*/


/**
 * @brief  설정 > 입출력 옵션
 */
class SettingInputOutput : public QWidget
{
    Q_OBJECT
public:
    explicit SettingInputOutput(QWidget *parent = nullptr);
    void requestInputOutputMode();
signals:

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_responseHttp_usb(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_responseHttp_usb_status(const int &p_id, const QJsonObject &p_jsonObject);
   // void slot_responseHttp_usb_status150(const int &p_id, const QJsonObject &p_jsonObject);

   // void slot_setInput();
   // void slot_setOutput();

    void slot_setHdmiOutput201();
    void slot_setOptOutput201();

    void slot_setInternalOutput_201_1();
    void slot_setInternalOutput_201_2();

    void slot_setInputSetting520();

    void slot_setOutputSetting130();
    void slot_setOutputSetting150();
    void slot_setOutputSetting201();
    void slot_setOutputSetting250();
    void slot_setOutputSetting520();

    void slot_setInputSetting250();

 //   void slot_chkNo();
 //   void slot_chkYes();
 //   void slot_chkRadio1();
  //  void slot_chkRadio2();

    void slot_setInput130();
    void slot_setInput150();
    void slot_setInput201();
    void slot_setInput250();
    void slot_setInput520();

    void slot_setInternal130();
    void slot_setInternal150();
    void slot_setInternal250();
    void slot_setInternal520();

    void slot_setOutput130();
    void slot_setOutput150();
    void slot_setOutput520();

    void slot_loading_state(const bool);
    void slot_setOutput_state(const QJsonObject);
    void slot_setUSBDAC_Info(const int, const QString, const int);

private:
    void setUIControl();
    void setUIControl130(QString deviceType);   //j230508
    void setUIControl150(QString deviceType);
    void setUIControl201(QString deviceType);
    void setUIControl250(QString deviceType);
    void setUIControl520(QString deviceType);   //j230516

    QWidget* getUIControlTitle(QString p_title);

    int get_curr_input_mode(QString deviceType);

    int get_InputMode130();
    int get_InputMode150();
    int get_InputMode201();
    int get_InputMode250();

    void set_InputMode130(int inputMode, int clockInfo, int javsMode);    
    void set_InputMode150(int a);
    void set_InputMode201(int a);
    void set_InputMode250(int a);
    void set_InputMode520(int inputMode);

    bool compareInternalMode(int *itd, int n, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8 );
    void clickUnusedConfirm();
    void clickUsbConfirm();
    void showDlgOfSelectUsbDac();
    bool showDlgOfDACMsg(QString title, QString msg);
    void setConvertMode(QString modeName, QString modeNum, QString cmd);
    bool  yes_no_Msgbox(QString a);
    void yes_no_usbMsgbox();
    void get_internalData(QString i, int *itm);
    QString get_UsbName();
    QString  get_UsbName(int *a, int *c);
    void set_UsbName(int a, QString b, int c);
    void set_UsbName(QString a);
    int get_data(int n, int *itm);
    QString get_data();
    int get_outputMode();
    void set_data(int output, int n, int *itm);
    void set_data(QString internaldata);
 //   void save_dacItem(QJsonObject &tmpJ);
 //   void get_dacItem(QJsonObject &tmpJ);
    QJsonObject ObjectFromString(const QString& in);

    void select_setUsbOutput201();
    void select_setUsbOutput150();
    void showDlgOfHDMI_setting();
    void showDlgOfUSB_setting();//c220708
    void showDlgOfHDMI_setting150();
    void showDlgOfHDMI_setting520();
    void showDlgOfUSB_setting150();//c220708
    void showDlgOfUSB_setting520();
    void showDlgOfHDMI_setting250();
    void showDlgOfUSB_setting250();//c220708
    void showDlgOfRX_setting250();//c220708
    void showDlgOfOUTPUT_setting();
    void showDlgOfOUTPUT_setting150();
    void showDlgOfOUTPUT_setting150B();
    void showDlgOfOUTPUT_setting520();
    void showDlgOfOUTPUT_setting250();
    void showDlgOfOPT_setting();
    void showDlgOfINTERAL_setting150();
    void showDlgOfINTERAL_setting520();
    void showDlgOfINTERAL_setting250();
    void setUsbOutput150();
    void setUsbOutput520();
    void setUsbOutput201();
    void setInput150(int a);
    void setInput201(int a);
    void setOutput150(int a);
    void setOutput201(int outputNo);
    void setInputData(int inputMode);
    void setOutputData(int outMode);//cheon09_io
    void setInternalData();//cheon09_io
    void setInternalOutput_150(QString req, QString exp);
    void setInternalOutput_520(QString req, QString exp);
    void setInternalOutput_250(QString req);
    void setInternalOutput_forInput_150(QString req);
    void setInternalOutput_forInput_520(QString req);
    void setInternalOutput_forInput_250(QString req);
    void resizeEvent(QResizeEvent *event) override;
    ushort bit2str_Convert(QString r);
    QString str2bit_Convert(ushort uc);
    QString str2bit_Convert250(ushort uc);
    QString str2bit_Convert520(ushort uc);
    void internalMode_display(QString status);
    void internalMode_display130(QString status);
    void internalMode_display250(QString status);
    void internalMode_display520(QString status);
    ushort internalChange(ushort a, ushort b);
    ushort internalChange520(ushort a, ushort b);

//520
    void select_setUsbOutput520();
    void setInput520(int a);
    void setOutput520(int outputNo);
    int get_InputMode520();

    void loding_ani_init();
    void loding_ani_show();
    void loding_ani_hide();

private:
//  settingInoutSubsetting *SettingInoutSubsetting=nullptr;

    Linker *linker;//c221013_1

    QDialog *loding_ani_dialog = nullptr;

    QString usbdacItem;
    QString mProductName;
    int mProductId;
    int mVendorId;
    int usbSelected;
    int usbUnused;
    QString usbName_str ;
    int widgetEnd ;
    QString internalDataString;
    QVBoxLayout *vl_scroll;
    // input-201
    BtnLabel *btn_input_Model;
    BtnLabel *btn_opt_in;
    BtnLabel *btn_line_in_l;
    BtnLabel *btn_line_in_r;

    // output 201
    BtnLabel *btn_usb;
    BtnLabel *btn_hdmi;
    BtnLabel *btn_opt_out;

    // internal output 201
    BtnLabel *btn_preampl_out;
    BtnLabel *btn_preampr_out;
    BtnLabel *btn_ampl_out;
    BtnLabel *btn_ampr_out;

    QPushButton *btn_setting_usb;//cheon09_io
    QPushButton *btn_setting_hdmi;//cheon09_io
    QPushButton *btn_setting_opt;//cheon09_io
    QPushButton *btn_setting_preamp;//cheon09_io
    //    QPushButton *btn_setting_preampr;//cheon09_io

    FlowLayout *fl_input;
    QWidget *widget_box_local;
    QWidget *widget_box_line;
    QWidget *widget_box_usb;//cheon09_io
    QWidget *widget_box_hdmi;//cheon09_io
    QWidget *widget_box_opt_out;//cheon09_io
    QWidget *widget_box_preamp_out;//cheon09_io
    QWidget *widget_box_amp_out;//cheon09_io
    QWidget *widget_box_tpreamp_out;//cheon09_io
    //----------- 150 ------------------------------------
    // input-150

    BtnLabel *btn150_input_Model;
    BtnLabel *btn150_AES_EBU_in;
    BtnLabel *btn150_USB_in;
    BtnLabel *btn150_line_in_l;
    BtnLabel *btn150_line_in_r;
    BtnLabel *btn150_HDMI_ARC_in;
    BtnLabel *btn150_opt_in;
    BtnLabel *btn150_coax_in;
    //  BtnLabel *;

    // output 150

    BtnLabel *btn150_dac_2010_out;
    BtnLabel *btn150_HDMI_out;


    // internal output 150
    BtnLabel *btn150_opt_out;
    BtnLabel *btn150_coax_out;
    BtnLabel *btn150_i2s_dvi_out;
    BtnLabel *btn150_AES_EBU_out;
    BtnLabel *btn150_i2s_rj45_out;
    BtnLabel *btn150_balanced_out_l;
    BtnLabel *btn150_balanced_out_r;
    BtnLabel *btn150_unbal2_out_l;
    BtnLabel *btn150_unbal2_out_r;

    //  QPushButton *btn150_setting_usb;//cheon09_io
    //  QPushButton *btn150_setting_hdmi;//cheon09_io
    //  QPushButton *btn150_setting_opt;//cheon09_io
    //  QPushButton *btn150_setting_preamp;//cheon09_io
    //    QPushButton *btn_setting_preampr;//cheon09_io

    //FlowLayout *fl_input;
    QWidget *widget_box_setting_amp12_out;
    QWidget *widget_box_setting_bal12_out;
    QWidget *widget_box_setting_unbalance_out;
    QWidget *widget_box_setting_speaker_out;
    QWidget *widget_box_setting_internal_out;
    QWidget *widget_box_dac_2010_out;
    QWidget *widget_box_hdmi_out;
    QWidget *widget150_box_local;
    QWidget *widget150_box_line;
    QWidget *widget150_box_usb;//cheon09_io
    QWidget *widget150_box_hdmi;//cheon09_io
    QWidget *widget150_box_opt_out;//cheon09_io
    QWidget *widget_box_internal_out;//cheon09_io
    QWidget *widget150_box_amp_out;//cheon09_io
    QWidget *widget150_box_tpreamp_out;//cheon09_io
    //--------------------------------------------------
    QPushButton *btn150_setting_usb_2010;
    QPushButton *btn150_setting_hdmi;
    QPushButton *btn150_setting_internal;
    QPushButton *btn150_setting_bal;
    //
    //
    //
    //----------- 520 ------------------------------------
    // input-520

    BtnLabel *btn520_input_Model;
    BtnLabel *btn520_AES_EBU_in;
    BtnLabel *btn520_USB_in;
    BtnLabel *btn520_line_in_l;
    BtnLabel *btn520_line_in_r;
    BtnLabel *btn520_HDMI_ARC_in;
    BtnLabel *btn520_opt_in;
    BtnLabel *btn520_coax_in;
    //  BtnLabel *;

    // output 520

    BtnLabel *btn520_dac_2010_out;
    BtnLabel *btn520_HDMI_out;


    // internal output 520
    BtnLabel *btn520_opt_out;
    BtnLabel *btn520_coax_out;
    BtnLabel *btn520_i2s_dvi_out;
    BtnLabel *btn520_AES_EBU_out;
    BtnLabel *btn520_i2s_rj45_out;
    //BtnLabel *btn520_balanced_out_l;
    //BtnLabel *btn520_balanced_out_r;
    //BtnLabel *btn520_unbal2_out_l;
    //BtnLabel *btn520_unbal2_out_r;
    BtnLabel *btn520_speaker_out;
    BtnLabel *btn520_unbalance_out_l;
    BtnLabel *btn520_unbalance_out_r;

    //  QPushButton *btn520_setting_usb;//cheon09_io
    //  QPushButton *btn520_setting_hdmi;//cheon09_io
    //  QPushButton *btn520_setting_opt;//cheon09_io
    //  QPushButton *btn520_setting_preamp;//cheon09_io
    //    QPushButton *btn_setting_preampr;//cheon09_io

    //FlowLayout *fl_input;
    //QWidget *widget_box_setting_amp12_out;
    //QWidget *widget_box_setting_bal12_out;
    //QWidget *widget_box_setting_internal_out;
    //QWidget *widget_box_dac_2010_out;
    //QWidget *widget_box_hdmi_out;
    QWidget *widget520_box_local;
    QWidget *widget520_box_line;
    QWidget *widget520_box_usb;//cheon09_io
    QWidget *widget520_box_hdmi;//cheon09_io
    QWidget *widget520_box_opt_out;//cheon09_io
    //QWidget *widget_box_internal_out;//cheon09_io
    QWidget *widget520_box_amp_out;//cheon09_io
    QWidget *widget520_box_tpreamp_out;//cheon09_io
    //--------------------------------------------------
    QPushButton *btn520_setting_usb_2010;
    QPushButton *btn520_setting_hdmi;
    QPushButton *btn520_setting_internal;
    QPushButton *btn520_setting_unbal;
    QPushButton *btn520_setting_speaker;

    DialogConfirm *dlgConfirm520;
    //
    //
    //
    // QWidget *widget_box_preamp_out;//cheon09_io

    // QWidget *widget_box_preampr_out;//

    int itm3[3];// 201, cheon13_io
    int itm8[8];// 150, cheon13_io

    bool input_mode_flag = false;
    bool output_mode_flag = false;
    int pre_input_mode = -1;

    int inputvalue130;  //clock Mode
    int clockInfo130;
    int javsMode130;
    int inputvalue150;
    int inputvalue201;
    int inputvalue250;
    int inputvalue520;

    int outputvalue;

private:
    QWidget *widget_IO_todal = nullptr;

    // input source
    QWidget *widget_input = nullptr;
    QWidget *widget_input_header = nullptr;
    QWidget *widget_input_device = nullptr;
    QWidget *widget_input_source = nullptr;

    QPushButton *btn_input_device = nullptr;
    QLabel *img_device = nullptr;
    QLabel *label_device = nullptr;

    QPushButton *btn_config_source = nullptr;
    QLabel *img_config_source = nullptr;

    QPushButton *btn_input_internal = nullptr;
    QLabel *img_inernal = nullptr;
    QLabel *label_inernal = nullptr;

    QPushButton *btn_input_extern50 = nullptr;
    QLabel *img_extern50 = nullptr;
    QLabel *label_extern50 = nullptr;

    QPushButton *btn_input_extern75 = nullptr;
    QLabel *img_extern75 = nullptr;
    QLabel *label_extern75 = nullptr;

    QPushButton *btn_input_COAX = nullptr;
    QLabel *img_inputCOAX = nullptr;
    QLabel *label_inputCOAX = nullptr;

    QPushButton *btn_input_OPT = nullptr;
    QLabel *img_inputOPT = nullptr;
    QLabel *label_inputOPT = nullptr;

    QPushButton *btn_input_LINE = nullptr;
    QLabel *img_inputLINEL = nullptr;
    QLabel *label_inputLINEL = nullptr;
    QLabel *img_inputLINER = nullptr;
    QLabel *label_inputLINER = nullptr;

    QPushButton *btn_input_USB = nullptr;
    QLabel *img_inputUSB = nullptr;
    QLabel *label_inputUSB = nullptr;

    QPushButton *btn_input_eARC = nullptr;
    QLabel *img_inputeARC = nullptr;
    QLabel *label_inputeARC = nullptr;


    // output source
    QWidget *widget_output = nullptr;
    QWidget *widget_output_header = nullptr;
    QWidget *widget_output_I2S = nullptr;
    QWidget *widget_output_PREAMP = nullptr;
    QWidget *widget_output_SPEAKER = nullptr;
    QWidget *widget_output_etc = nullptr;

    QPushButton *btn_output_USB = nullptr;
    QPushButton *btn_config_USB = nullptr;
    QLabel *img_config_USB = nullptr;
    QLabel *img_USB = nullptr;
    QLabel *label_USB = nullptr;

    QPushButton *btn_output_HDMI = nullptr;
    QPushButton *btn_config_HDMI = nullptr;
    QLabel *img_config_HDMI = nullptr;
    QLabel *img_HDMI = nullptr;
    QLabel *label_HDMI = nullptr;

    QPushButton *btn_output_I2SA = nullptr;
    QLabel *img_I2SA = nullptr;

    QPushButton *btn_output_I2SB = nullptr;
    QLabel *img_I2SB = nullptr;

    QLabel *label_I2S = nullptr;
    QLabel *img_config_I2S = nullptr;

    QPushButton *btn_output_COAX = nullptr;
    QLabel *img_COAX = nullptr;
    QLabel *label_COAX = nullptr;

    QPushButton *btn_output_OPT = nullptr;
    QLabel *img_OPT = nullptr;
    QLabel *label_OPT = nullptr;

    QPushButton *btn_output_AES_EBU = nullptr;
    QLabel *img_AES_EBU = nullptr;
    QLabel *label_AES_EBU = nullptr;

    QPushButton *btn_output_PREAMP = nullptr;
    QPushButton *btn_config_PREAMP = nullptr;
    QLabel *img_config_PREAMP = nullptr;
    QLabel *img_PREAMPL = nullptr;
    QLabel *label_PREAMPL = nullptr;
    QLabel *img_PREAMPR = nullptr;
    QLabel *label_PREAMPR = nullptr;

    QPushButton *btn_output_SPEAKER = nullptr;
    QPushButton *btn_config_SPEAKER = nullptr;
    QLabel *img_config_SPEAKER = nullptr;
    QLabel *img_SPEAKER = nullptr;
    QLabel *label_SPEAKER = nullptr;

    QPushButton *btn_config_ETC = nullptr;
    QLabel *img_config_etc = nullptr;

    /*QWidget *output_I2S_DVI = nullptr;
    QWidget *output_I2S_RJ45 = nullptr;
    QWidget *output_COAX = nullptr;
    QWidget *output_OPT = nullptr;
    QWidget *output_AES_EBU = nullptr;
    QWidget *output_BALANCED = nullptr;
    QWidget *output_DAC2010 = nullptr;*/

    QPushButton *btn_Activation_input;
    QPushButton *btn_Activation_output;

    QJsonObject jsonObj_usbDACList;

    int before_width = 0;
};
#endif // SETTINGINPUTOUTPUT_H
