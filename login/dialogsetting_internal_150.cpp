#include "dialogsetting_internal_150.h"
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
dialogsetting_internal_150::dialogsetting_internal_150(QWidget *parent)
    : QDialog(parent)
{
    setInit();
    setUIControl();
    getOptSettingOfsetting();
}

/**
 * @brief DialogSelect_Cloud::setInit: 초기 세팅
 */
void dialogsetting_internal_150::setInit(){
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT-300);
    this->setStyleSheet("background-color:transparent;");
   // OptDetailData = new QJsonObject();
}

/**
 * @brief DialogSelect_Cloud::setUIControl : UI 세팅
 */
void dialogsetting_internal_150::setUIControl(){

    QJsonObject tmp_optsettingval = get_settingOfOptDetail();
    dlgConfirmOpt = new DialogConfirm(this);
    // 타이틀
    QLabel *lb_title = new QLabel;
    lb_title->setText(tr("OPT Setting"));//lb_title->setText(tr("클라우드 추가"));
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

    print_debug();

    comboBoxOptopticalMsb = new QComboBox();
    comboBoxOptopticalMsb->setProperty("btnNo", 0);
    comboBoxOptopticalLsb = new QComboBox();
    comboBoxOptopticalLsb->setProperty("btnNo", 1);

    comboBoxOptopticalMsb->addItem(tr(" 16bit"), 0);//cheon01_src
    comboBoxOptopticalMsb->addItem(tr(" 17bit"), 1);//cheon01_src
    comboBoxOptopticalMsb->addItem(tr(" 18bit"), 2);//cheon01_src
    comboBoxOptopticalMsb->addItem(tr(" 19bit"), 3);//cheon01_src
    comboBoxOptopticalMsb->addItem(tr(" 20bit"), 4);//cheon01_src
    comboBoxOptopticalMsb->addItem(tr(" 21bit"), 5);//cheon01_src
    comboBoxOptopticalMsb->addItem(tr(" 22bit"), 6);//cheon01_src
    comboBoxOptopticalMsb->addItem(tr(" 23bit"), 7);
    comboBoxOptopticalMsb->addItem(tr(" 24bit"), 8);

    print_debug();

    QLabel *lb_MQA = new QLabel(tr("opt MSB"));
    lb_MQA->setStyleSheet("color:#E5E5E4");
    lb_MQA->setFixedSize(39,70);
    lb_MQA->setText(tr("MSB"));
    lb_MQA->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    comboBoxOptopticalMsb->setContentsMargins(10,0,0,0);
    comboBoxOptopticalMsb->setFixedHeight(35);
    comboBoxOptopticalMsb->setObjectName("comboBox_optMsb");//cheon01
    comboBoxOptopticalMsb->setFixedWidth(300);


    comboBoxOptopticalMsb->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658;}"// border: 1px solid #B18658;   } "
                                //"QAbstractItemView QAbstractItemView::item {  padding-left:5px; } "
                                //"QComboBox QAbstractItemView:selected { color:red;selection-background-color:red } "
                                //"QComboBox QAbstractItemView:hover { color:red;selection-background-color:red } "
                                );

    QHBoxLayout *hl_lb_MQA = new QHBoxLayout();
    hl_lb_MQA->setContentsMargins(0,0,20,0);
    hl_lb_MQA->setSpacing(0);
    hl_lb_MQA->addWidget(lb_MQA, 0, Qt::AlignCenter);
   // hl_lb_MQA->setSpacing(30);
    hl_lb_MQA->addWidget(comboBoxOptopticalMsb, 0, Qt::AlignCenter);
   // hl_lb_MQA->setSpacing(30);

    QWidget *widget_total_MQA = new QWidget();
    widget_total_MQA->setObjectName("widget_total");
    widget_total_MQA->setStyleSheet("#widget_total { border-top:1px solid #707070;border-bottom:1px solid #707070; } ");
    widget_total_MQA->setLayout(hl_lb_MQA);

    print_debug();

    comboBoxOptopticalLsb->addItem(tr(" 32kHz"), 0);//cheon01_src
    print_debug();
    comboBoxOptopticalLsb->addItem(tr(" 44.1kHz"), 1);//cheon01_src
    print_debug();
    comboBoxOptopticalLsb->addItem(tr(" 48kHz"), 2);//cheon01_src
    comboBoxOptopticalLsb->addItem(tr(" 88.2kHz"), 3);//cheon01_src
    comboBoxOptopticalLsb->addItem(tr(" 96kHz"), 4);//cheon01_src
    print_debug();

    QLabel *lb_PCM = new QLabel(tr("opt LSB"));
    lb_PCM->setStyleSheet("color:#E5E5E4");
    lb_PCM->setFixedSize(39,70);
    lb_PCM->setText((tr("LSB")));
    lb_PCM->setStyleSheet("Text-align:left; font-size:18px;color:#E5E5E4; ");

    //comboBox = new QComboBox();
    comboBoxOptopticalLsb->setContentsMargins(10,0,0,0);
    comboBoxOptopticalLsb->setFixedHeight(35);
    comboBoxOptopticalLsb->setObjectName("comboBox_optLsb");//cheon01
    comboBoxOptopticalLsb->setFixedWidth(300);

    comboBoxOptopticalLsb->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; }"//border: 1px solid #B18658;   } "
                                //"QAbstractItemView QAbstractItemView::item {  padding-left:5px; } "
                                //"QComboBox QAbstractItemView:selected { color:red;selection-background-color:red } "
                                //"QComboBox QAbstractItemView:hover { color:red;selection-background-color:red } "
                                );

    QHBoxLayout *hl_lb_PCM = new QHBoxLayout();
    hl_lb_PCM->setContentsMargins(0,0,20,0);
    hl_lb_PCM->setSpacing(0);
    hl_lb_PCM->addWidget(lb_PCM, 0, Qt::AlignCenter);
   // hl_lb_PCM->setSpacing(30);
    hl_lb_PCM->addWidget(comboBoxOptopticalLsb, 0, Qt::AlignCenter);
   // hl_lb_PCM->setSpacing(30);

    QWidget *widget_total_PCM = new QWidget();
    widget_total_PCM->setObjectName("widget_total");
    widget_total_PCM->setStyleSheet("#widget_total { border-top:1px solid #707070;border-bottom:1px solid #707070; } ");
    widget_total_PCM->setLayout(hl_lb_PCM);


   // vl_total1->addWidget(this->getUIControlOption(tr("PCM Resampling Frequency"), comboBoxOptPCM));//cheon01

    QHBoxLayout *hl_total1 = new QHBoxLayout();

    hl_total1->setContentsMargins(0,0,0,0);
    hl_total1->setSpacing(0);
    hl_total1->setAlignment(Qt::AlignTop);
    hl_total1->addWidget(widget_total_MQA);//cheon01
    hl_total1->addWidget(widget_total_PCM);//cheon01


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

    vl_total2->addLayout(hl_total1);

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


    connect(comboBoxOptopticalMsb, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOpt(int)));
    connect(comboBoxOptopticalLsb, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexOpt(int)));


    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(close()));//print_debug();
    connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clickedOptChangeSet()));

}


void dialogsetting_internal_150::dlgOptMSB(){


    dlgConfirmOpt->setTitle(tr("Opt MQA Replay Gain Notice"));
    dlgConfirmOpt->setText(tr("Activates Replay Gaon Control when playing MQA. \nSet the desired leveling value.\nThese values apply only to MQA sound sources."));
    //dlgConfirmOpt->setText(tr("MQA 재생시 Replay Gaon Control을 활성화합니다. \n원하는 평준화값을 설정하시오.\n해당 값은 MQA  음원에만 적용됩니다."));

}
void dialogsetting_internal_150::dlgOptLSB(){


    dlgConfirmOpt->setTitle(tr("OPT Output Gain Notice"));
    dlgConfirmOpt->setText(tr("Adjusts the Digital Output Gain. \nExternal receiver Or, if distortion occurs when connecting to a DAC, adjust the Output Gain value appropriately."));
    //dlgConfirmOpt->setText(tr("Digital Output의 Gain 값을 조정합니다. \n외부 리시버 또는 DAC과 연결 시 distortion이 발생하면 Output Gain 값을 적절히 조절하여 사용합니다."));

}


void dialogsetting_internal_150::slot_showDlgOfOptNotice(){
    dlgConfirmOpt->setGeometry(DLG_WIDTH+480,350+100, 350,300);
    dlgConfirmOpt->setAlertMode();
    dlgConfirmOpt->setProperty("flagShown",false);
    if(dlgConfirmOpt->property("flagShown").toBool()==false){
        dlgConfirmOpt->setProperty("flagShown",true);

        int result = dlgConfirmOpt->exec();

        if(result == QDialog::Accepted){
            //showRemote(true);
            //qDebug() << " selectedOptPass_str: " << this->onOff->getCurrentValue();;
           // OptDetailData.insert( "isOptPassThrough", this->onOff->getCurrentValue());
            //QJsonDocument doc(OptDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_showDlgOfOptPass()-Opt_json =: " << strJson;

        }
    }
}


void dialogsetting_internal_150::set_settingOfOptDetail(QJsonObject tmpJson){
        OptDetailData = tmpJson;
}
QJsonObject dialogsetting_internal_150::get_settingOfOptDetail(){
        return OptDetailData;
}

void dialogsetting_internal_150::slot_changedIndexOpt(int index){

    int btnSenderNo = sender()->property("btnNo").toInt();
    QString selectedOpt_str;
    //qDebug() << "----------------isOptPassThrough :" <<  OptDetailData["isOptPassThrough"].toBool();
 //   QJsonObject Opt_json = get_settingOfOptDetail();
    QJsonDocument doc0(OptDetailData);  QString strJson0(doc0.toJson(QJsonDocument::Compact));  qDebug() << "slot_changedIndexOpt()pre-Opt_json =: " << strJson0;

    switch(btnSenderNo){
    case  0 :
        selectedOpt_str = comboBoxOptopticalMsb->itemText(index);
        qDebug() << " selectedOpt_str: " << selectedOpt_str;
        OptDetailData.insert( "opticalMsb", index);
    //    set_settingOfOptDetail(OptDetailData);

        break;
    case  1 :
        selectedOpt_str = comboBoxOptopticalLsb->itemText(index);
        qDebug() << " selectedOpt_str: " << selectedOpt_str;
        OptDetailData.insert( "opticalLsb", index);
     //   set_settingOfOptDetail(OptDetailData);


        break;

    }
 //   Opt_json = get_settingOfOptDetail();
    QJsonDocument doc(OptDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "Opt_json =: " << strJson;



}



/**
 * @brief SettingTimer::getUIControlOption 콤보박스 포함된 ROW 생성한다
 * @param p_title
 * @return
 */



/**
 * @brief DialogSelect_Cloud::slot_clickedAdd : [슬롯] 추가 버튼 클릭
 */

const int HTTP_OPT_GET_MODE  = 158;
const int HTTP_OPT_SET_MODE  = 159;

void dialogsetting_internal_150::getOptSettingOfsetting(){

   NetworkHttp *network = new NetworkHttp;
   QJsonObject tmp_json;
   tmp_json.insert("outputMode", 1);
   connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
   network->request(HTTP_OPT_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.setting.get"), tmp_json, true);

}

void dialogsetting_internal_150::getOptInOutSettingOfsetting(){

   NetworkHttp *network = new NetworkHttp;
   QJsonObject tmp_json;
   connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
   network->request(HTTP_OPT_GET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("in.out.mode.get"), tmp_json, true);

}
void dialogsetting_internal_150::slot_clickedOptChangeSet(){
    print_debug();

   NetworkHttp *network = new NetworkHttp;
   QJsonObject tmp_json = get_settingOfOptDetail();
   QJsonObject tmp_json_set;
   tmp_json_set.insert("data", tmp_json);
   tmp_json_set.insert("outputMode", 1);


   QJsonDocument doc(tmp_json_set);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"slot_clickedOptChangeSet()-tmp_json_set :"<< strJson;//cheon09_io

   connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
   network->request(HTTP_OPT_SET_MODE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("output.setting.set"), tmp_json_set, true);

}

void dialogsetting_internal_150::setDialogOpt_show(){
   // QJsonObject *Opt_json = get_settingOfOptDetail();

   QJsonDocument doc(OptDetailData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"setDialogOpt_show()-OptDetailData :"<< strJson;//cheon09_io

    comboBoxOptopticalMsb->setCurrentIndex( OptDetailData["opticalMsb"].toInt() );
    comboBoxOptopticalLsb->setCurrentIndex( OptDetailData["opticalLsb"].toInt() );
    qDebug() << "DLG_WIDTH :" <<  DLG_WIDTH;
    qDebug() << "DLG_HEIGHT :" <<  DLG_HEIGHT;

    ToastMsg::show(this, "", tr("The current OPT setting information is displayed on the screen."));

}


void dialogsetting_internal_150::setDialogOpt_save(QJsonObject p_jsonObject){
    QJsonObject tmp_Json;
    if(p_jsonObject.contains("data")){
        tmp_Json = p_jsonObject["data"].toObject(); //cheon09_io

    }
   // QJsonDocument doc(tmp_Json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"\npre-setDialogOpt_save()-tmp_Json :"<< strJson;//cheon09_io
     int codecTrim = tmp_Json["codecTrim"].toInt();
     int dacFilterType = tmp_Json["dacFilterType"].toInt();
     int drcInfo = tmp_Json["drcInfo"].toInt();
     int dsdMode = tmp_Json["dsdMode"].toInt();
     int hpOhm = tmp_Json["hpOhm"].toInt();
     bool isDigitalPassThrough = tmp_Json["isDigitalPassThrough"].toBool();
     bool isClearLowerBit = tmp_Json["isClearLowerBit"].toBool();
     OptDetailData.insert( "isClearLowerBit", isClearLowerBit);
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
     int preoutLevelInfo = tmp_Json["preoutLevelInfo"].toInt();
     int digitalVolume = tmp_Json["digitalVolume"].toInt();
     int samplingRate = tmp_Json["samplingRate"].toInt();
     int maxSamplingRate = tmp_Json["maxSamplingRate"].toInt();
     OptDetailData.insert( "maxSamplingRate", maxSamplingRate);
     int volumeMaxLevel = tmp_Json["volumeMaxLevel"].toInt();
     int volumeMinLevel = tmp_Json["volumeMinLevel"].toInt();

     qDebug() << "isOptPassThrough : " << isOptPassThrough;

     OptDetailData.insert( "codecTrim", codecTrim);
     OptDetailData.insert( "dacFilterType", dacFilterType);

     OptDetailData.insert( "drcInfo", drcInfo);
     OptDetailData.insert( "dsdMode", dsdMode);
     OptDetailData.insert( "hpOhm", hpOhm);
     OptDetailData.insert( "isDigitalPassThrough", isDigitalPassThrough);
     OptDetailData.insert( "isOptPassThrough", isOptPassThrough);
     OptDetailData.insert( "javs", javs);
     OptDetailData.insert( "mqaRelplayGain", mqaRelplayGain);
     OptDetailData.insert( "mqaType", mqaType);
  //   OptDetailData.insert( "opticalApwMode", opticalApwMode);
    // OptDetailData.insert( "opticalApwValue", opticalApwValue);
     OptDetailData.insert( "opticalLsb", opticalLsb);
     OptDetailData.insert( "opticalMsb", opticalMsb);
     OptDetailData.insert( "outputGain", outputGain);
     OptDetailData.insert( "phaseInvert", phaseInvert);
     OptDetailData.insert( "preoutLevelInfo", preoutLevelInfo);
     OptDetailData.insert( "digitalVolume", digitalVolume);
     OptDetailData.insert( "samplingRate", samplingRate);
     OptDetailData.insert( "volumeMaxLevel", volumeMaxLevel);
     OptDetailData.insert( "volumeMinLevel", volumeMinLevel);

     qDebug() << "json-isOptPassThrough : " << OptDetailData["isOptPassThrough"].toBool();


     QJsonDocument doc1(OptDetailData);  QString strJson1(doc1.toJson(QJsonDocument::Compact));  qDebug() <<"\nsetDialogOpt_save()-tmp_Json :"<< strJson1 << "\n";//cheon09_io

     ToastMsg::show(this, "", tr("Save the information from the Rose deive."));
}

void dialogsetting_internal_150::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    QString deviceType = global.device.getDeviceType();
  //  print_debug();//cheon0219

    print_debug();  QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nslot_responseHttp =: " << strJson;//cheon12_io
    //qDebug() << "p_id : " << p_id;
    QString internalMode;

    switch(p_id){
    case HTTP_OPT_GET_MODE :

        if( deviceType == "RS150" || deviceType == "RS150B"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){

            }

        }else if( deviceType == "RS201"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                setDialogOpt_save(p_jsonObject);
                setDialogOpt_show();
               // setDialogOpt_save(p_jsonObject);

            }

        }else if( deviceType == "RS301"){

        }else if( deviceType == "RS350"){

        }
        break;
    case HTTP_OPT_SET_MODE :

        if( deviceType == "RS150" || deviceType == "RS150B"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){


                 setResult(QDialog::Accepted);
                 this->hide();

            }else{
                setResult(QDialog::Rejected);
                this->hide();
            }


        }else if( deviceType == "RS201"){
            const QString jsonKey_flagOk = "flagOk";
            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                // getOptInOutSettingOfsetting();

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


void dialogsetting_internal_150::set_selected_deviceName(QString usbstr){
    selected_deviceName = usbstr;
}
QString dialogsetting_internal_150::get_selected_deviceName(){
    return selected_deviceName;
}
void dialogsetting_internal_150::setTextName(QString usbstr){
    radio_UsbName->setText(usbstr);
}

void dialogsetting_internal_150::setPropertyName(QString usbstr){
    radio_UsbName->setProperty(PROPERTY_NAME_READABLE, usbstr);
}
