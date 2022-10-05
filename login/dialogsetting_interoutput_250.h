#ifndef DIALOGSETTING_INTEROUTPUT_250_H
#define DIALOGSETTING_INTEROUTPUT_250_H


#include <QDialog>
#include <QKeyEvent>
#include <QRadioButton>
#include <QWidget>
#include <QComboBox>
#include <widget/onoffwidget.h>
#include <QJsonObject>
#include "widget/dialogconfirm.h"
#include "setting/settinginputoutput.h"
#include "home/remotebtnitem.h"

namespace  Dialog{

/**
 * @brief 클라우드 타입 선택 Dlg
 *
 * @note 추후 @sa Dialog_Radio 로 대체
 */
class dialogsetting_interoutput_250 : public QDialog
{
    Q_OBJECT
public:
    explicit dialogsetting_interoutput_250(QWidget *parent = nullptr);
    QString getSelecteddeviceName() const{
        return selected_deviceName;
    };

private slots:
    void slot_clickedPCMquestion();
    void slot_clickedDSDquestion();
    void slot_clickedPreLevelquestion();
    void slot_clickedHOIquestion();
    void slot_clickedSoftwareVolquestion();
    void slot_clickedMQAquestion();
    void slot_clickedBit32question();

    void slot_clickedOpt_coaxChangeSet();
    void slot_clickedOpt_coaxsubmqaGain();
    void slot_clickedOpt_coaxaddmqaGain();
    void slot_clickedOpt_coaxsuboutputGain();
    void slot_clickedOpt_coaxaddoutputGain();
    void slot_modeOnOff();
    void slot_modeOnOff_32();
    void slot_print();
    void slot_changedIndexOpt_coax(int index);
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_showDlgOfOpt_coaxNotice();
    void slot_clickedOpt_coaxmqaGain();
    void slot_clickedOpt_coaxoutputGain();
    void slot_clickedOpt_coaxpassGain();
    void slot_clickedDVIAB();
private:

    QLabel *lb_i2s_text;
    QComboBox *comboBoxOpt_coaxMQA;
    QComboBox *comboBoxOutputPCMMaxSampling, *comboBoxOutputPCMResampling;
    QComboBox *comboBoxHOI;
    QComboBox *comboBoxOpt_coaxDSD;
    QComboBox *comboBoxOpt_coaxRoon;

    OnOffWidget *onOff, *onOff_32;
    const int DLG_WIDTH = 800;
    const int DLG_HEIGHT = 560;
    const char* PROPERTY_NAME_READABLE = "readableName";

    QString selected_deviceName;
QString notice_msg;
QLabel *lb_notice;
QWidget *widget_box_notice;
//QWidget *widget_mqaGain;
//QWidget *widget_outputGain;
QWidget *wwbtn_DVIA;
QWidget *wwbtn_DVIB;
BtnLabel *btn_DVIA;
BtnLabel *btn_DVIB;
QPushButton *lb_onOfftitle;
QPushButton *lb_gain[2];
RemoteBtnItem *btn_gain_add[2];
RemoteBtnItem *btn_gain_sub[2];
QLabel *lb_text[2];
int outputdBData;
int mqadBData;

DialogConfirm *dlgConfirmOpt_coax;
//DialogConfirm;
    QJsonObject Opt_coaxDetailData ;

    QRadioButton *radio_Unused;
    QRadioButton *radio_UsbName;

    QPushButton *comboBox_1;
    QPushButton *comboBox_2;
    QPushButton *comboBox_3;
    QPushButton *comboBox_4;

    QList<int> list_keyValue;

    void setInit();
    void setUIControl();

    void keyPressEvent(QKeyEvent *event) override{
        if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
            return;
        }
        QDialog::keyPressEvent(event);
    }

public:
    QString get_selected_deviceName();
    void set_selected_deviceName(QString usbstr);
    void setTextName(QString usbstr);
    void setPropertyName(QString usbstr);
    void setDialogOpt_coax_show();
    void setDialogOpt_coax_save(QJsonObject Opt_coax_rcv);

    QWidget* getPlusMinusUIControlOption(QString p_title, int i,int data);
    //QWidget* getUIControlOption(QString p_title);
    QWidget* getUIControlOption(QString p_title, QString p_content, int index);
    QWidget* getUIControlOption(QString p_title);
    QWidget* getUIControlOption_32(QString p_title);
    QWidget* getUIControlOption(QString p_title, QComboBox *comboBox);

    QJsonObject get_settingOfOpt_coaxDetail();
    void set_settingOfOpt_coaxDetail(QJsonObject a);
    void getOpt_coaxSettingOfsetting();
    void getOpt_coaxInOutSettingOfsetting();
    void dlgOpt_coaxPass();
    void dlgOpt_coaxMqa();
    void dlgOpt_coaxOutput();

};

};

/*
 *
    int btnSenderNo = sender()->property("btnNo").toInt();
    QString selectedOpt_coaxMQA_str;

 //   QJsonObject Hdmi_json = get_settingOfHdmiDetail();
    QJsonDocument doc0(HdmiDetailData);  QString strJson0(doc0.toJson(QJsonDocument::Compact));  qDebug() << "pre-Hdmi_json =: " << strJson0;

    switch(btnSenderNo){
    case  0 :
        selectedHdmiMQA_str = comboBoxHdmiMQA->itemText(index);
        qDebug() << " selectedHdmi_str: " << selectedHdmiMQA_str;
        HdmiDetailData.insert( "mqaType", selectedHdmiMQA_str);
    //    set_settingOfHdmiDetail(HdmiDetailData);

        break;
    case  1 :
        selectedHdmiMQA_str = comboBoxHdmiPCM->itemText(index);
        qDebug() << " selectedHdmi_str: " << selectedHdmiMQA_str;
        HdmiDetailData.insert( "samplingRate", selectedHdmiMQA_str);
     //   set_settingOfHdmiDetail(HdmiDetailData);


        break;
    case  2 :
        selectedHdmiMQA_str = comboBoxHdmiDSD->itemText(index);
        qDebug() << " selectedHdmi_str: " << selectedHdmiMQA_str;
        HdmiDetailData.insert( "dsdMode", selectedHdmiMQA_str);
      //  set_settingOfHdmiDetail(HdmiDetailData);


        break;
    case  3 :
        selectedHdmiMQA_str = comboBoxHdmiRoon->itemText(index);
        qDebug() << " selectedHdmi_str: " << selectedHdmiMQA_str;
        HdmiDetailData.insert( "digitalVolume", selectedHdmiMQA_str);
       // set_settingOfHdmiDetail(HdmiDetailData);
        break;
    case  4 :
       // selectedHdmiMQA_str = comboBoxHdmiRoon->itemText(index);
        qDebug() << " selectedHdmiPass_str: " << this->onOff->getCurrentValue();;
        HdmiDetailData.insert( "isHdmiPassThrough", this->onOff->getCurrentValue());

        break;


    }
 //   Hdmi_json = get_settingOfHdmiDetail();
    QJsonDocument doc(HdmiDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "Hdmi_json =: " << strJson;




 *
    QLabel *lb_volumn = new QLabel(tr("Volume"));
    lb_volumn->setAlignment(Qt::AlignCenter);
    lb_volumn->setStyleSheet("color:white;font-size:16px;");
    this->lb_volumnNum = new QLabel("0");
    lb_volumnNum->setAlignment(Qt::AlignCenter);
    lb_volumnNum->setStyleSheet("color:white;font-size:20px;font:bold;margin-top:10px;");
    RemoteBtnItem *btn_vol_add = new RemoteBtnItem();
    btn_vol_add->setData(":/images/vol_add_remote.png", "");
    btn_vol_add->setProperty("httpId",HTTP_REMOTE_VOLUME_ADD);
    btn_vol_add->setCircle(47);
    btn_vol_add->setCursor(Qt::PointingHandCursor);
    RemoteBtnItem *btn_vol_sub = new RemoteBtnItem();
    btn_vol_sub->setData(":/images/vol_sub_remote.png", "");
    btn_vol_sub->setProperty("httpId",HTTP_REMOTE_VOLUME_SUB);
    btn_vol_sub->setCircle(47);
    btn_vol_sub->setCursor(Qt::PointingHandCursor);
    this->slider_vol = new QSlider();
    this->slider_vol->setObjectName("slider_vol");
    this->slider_vol->setPageStep(1);
    this->slider_vol->setMaximum(100);
    slider_vol->setFixedHeight(268);
    slider_vol->setCursor(Qt::PointingHandCursor);
    slider_vol->setStyleSheet("QSlider::groove:vertical { background:#333333;width:4px; } QSlider::handle:vertical { background:white;height:16px;border-radius:8px;margin:0 -6px; } QSlider::add-page:vertical { background:#CCCCCC; } QSlider::sub-page:vertical { background:#333333; } ");

    QVBoxLayout *vl_sound = new QVBoxLayout();
    vl_sound->setContentsMargins(0,35,0,0);
    vl_sound->setSpacing(0);
    vl_sound->addWidget(lb_volumn, 0, Qt::AlignTop);
    vl_sound->addWidget(lb_volumnNum, 0, Qt::AlignTop);
    vl_sound->addWidget(btn_vol_add, 1, Qt::AlignCenter);
    vl_sound->addWidget(slider_vol, 1, Qt::AlignCenter);
    vl_sound->addWidget(btn_vol_sub, 1, Qt::AlignCenter);

    QWidget *widget_volumn = new QWidget();
    widget_volumn->setObjectName("widget_volumn");
    widget_volumn->setStyleSheet("#widget_volumn { background-color:#4D4D4D;border-radius:10px; }");
    widget_volumn->setFixedSize(100,556); //100x556
    widget_volumn->setLayout(vl_sound);
    QVBoxLayout *vl_volumn = new QVBoxLayout();
    vl_volumn->setContentsMargins(0,0,0,0);
    vl_volumn->setSpacing(0);
    vl_volumn->addWidget(widget_volumn);


    -------------------------------

       void HomeMain::slot_showDlgOfDisconnect(){
           if(dlgConfirm->property("flagShown").toBool()==false){
               dlgConfirm->setProperty("flagShown",true);

               int result = dlgConfirm->exec();

               if(result == QDialog::Accepted){
                   showRemote(true);
                   if(this->hlayout_content->count() >0){
                       QWidget *w_currMainMenu = this->hlayout_content->itemAt(0)->widget();
                       dynamic_cast<AbstractMainContent*>(w_currMainMenu)->showhideRemoteWidget(true);
                   }
               }
           }
       }
*/
#endif // DIALOGSETTING_HDMI_H
