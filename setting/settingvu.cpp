#include "settingvu.h"
#include "common/global.h"
#include "widget/toastmsg.h"

#include <QScrollArea>
#include <common/gscommon.h>
#include <common/networkhttp.h>

const int HTTP_VLC_SET = 99;
const int HTTP_VLC_GET = 98;

/**
 * @brief SettingVU::SettingVU 생성자
 * @param parent
 */
SettingVU::SettingVU(QWidget *parent) : QWidget(parent)
{
    this->setUIControl();
}
/**
 * @brief SettingVU::setUIControl UI 세팅
 */
void SettingVU::setUIControl(){

    qDebug() << "-SettingVU::setUIControl()25";
    // VU 테마
    // print_debug();
    this->btn_1_lb = GSCommon::getUILabelImg(":/images/setting/vu_yellow_off.png");
    this->btn_2_lb = GSCommon::getUILabelImg(":/images/setting/vu_blue_off.png");
    this->btn_3_lb = GSCommon::getUILabelImg(":/images/setting/vu_mint_off.png");
    this->btn_4_lb = GSCommon::getUILabelImg(":/images/setting/vu_purple_off.png");
    this->btn_5_lb = GSCommon::getUILabelImg(":/images/setting/vu_red_off.png");
    this->btn_6_lb = GSCommon::getUILabelImg(":/images/setting/vu_white_off.png");
    this->btn_7_lb = GSCommon::getUILabelImg(":/images/setting/vu_4_off.png");
    this->btn_8_lb = GSCommon::getUILabelImg(":/images/setting/vu_3_off.png");
    this->btn_9_lb = GSCommon::getUILabelImg(":/images/setting/vu_1_off.png");
    this->btn_10_lb = GSCommon::getUILabelImg(":/images/setting/vu_2_off.png");
    this->btn_11_lb = GSCommon::getUILabelImg(":/images/setting/vu_7_off.png");
    this->btn_12_lb = GSCommon::getUILabelImg(":/images/setting/vu_8_off.png");

    this->btn_13_lb = GSCommon::getUILabelImg(":/images/setting/vu_5_off.png");
    this->btn_14_lb = GSCommon::getUILabelImg(":/images/setting/vu_6_off.png");

 //print_debug();


    if(global.device.getDeviceType()=="RS250" || global.device.getDeviceType()=="RS250A"){
         print_debug();
         qDebug() << "-SettingVU::setUIControl()-49";
        btn_1 = this->getUIControlButton(tr("ROSE Style")   , 1, this->btn_1_lb);
        btn_2 = this->getUIControlButton(tr("Mcintosh Style"), 2, this->btn_2_lb);
        btn_3 = this->getUIControlButton(tr("Green Style")   , 3, this->btn_3_lb);
        btn_4 = this->getUIControlButton(tr("Purple Style") , 4, this->btn_4_lb);
        btn_5 = this->getUIControlButton(tr("Red Style")    , 5, this->btn_5_lb);
        btn_6 = this->getUIControlButton(tr("White Style")  , 6, this->btn_6_lb);
        btn_7 = this->getUIControlButton(tr("A style")  , 7, this->btn_7_lb);
        btn_8 = this->getUIControlButton(tr("B Style")  , 8, this->btn_8_lb);
        btn_9 = this->getUIControlButton(tr("C Style")  , 9, this->btn_9_lb);
        btn_10 = this->getUIControlButton(tr("D Style")  , 10, this->btn_10_lb);
        btn_11 = this->getUIControlButton(tr("RA180 Style")    , 11, this->btn_11_lb);
        btn_12 = this->getUIControlButton(tr("Level Meter Style")  , 12, this->btn_12_lb);
 //print_debug();
        //btn_13 = this->getUIControlButton(tr("Level Meter Style2")  , 13, this->btn_13_lb);
       // btn_14 = this->getUIControlButton(tr("Level Meter Style3")  , 14, this->btn_11_lb);


    }else if(global.device.getDeviceType()=="RS201"){
        // print_debug();
        qDebug() << "-SettingVU::setUIControl()-69";
        btn_1 = this->getUIControlButton(tr("ROSE Style")   , 1, this->btn_1_lb);
        btn_2 = this->getUIControlButton(tr("Mcintosh Style"), 2, this->btn_2_lb);
        btn_3 = this->getUIControlButton(tr("Green Style")   , 3, this->btn_3_lb);
        btn_4 = this->getUIControlButton(tr("Purple Style") , 4, this->btn_4_lb);
        btn_5 = this->getUIControlButton(tr("Red Style")    , 5, this->btn_5_lb);
        btn_6 = this->getUIControlButton(tr("White Style")  , 6, this->btn_6_lb);
        btn_7 = this->getUIControlButton(tr("A style")  , 7, this->btn_7_lb);
        btn_8 = this->getUIControlButton(tr("B Style")  , 8, this->btn_8_lb);
        btn_9 = this->getUIControlButton(tr("C Style")  , 9, this->btn_9_lb);
        btn_10 = this->getUIControlButton(tr("D Style")  , 10, this->btn_10_lb);
        btn_11 = this->getUIControlButton(tr("RA180 Style")    , 11, this->btn_11_lb);
        btn_12 = this->getUIControlButton(tr("Level Meter Style")  , 12, this->btn_12_lb);
 //print_debug();
        //btn_13 = this->getUIControlButton(tr("Level Meter Style2")  , 13, this->btn_13_lb);
        //btn_14 = this->getUIControlButton(tr("Level Meter Style3")  , 14, this->btn_14_lb);


    }else if(global.device.getDeviceType()=="RS520"){
        // print_debug();
        qDebug() << "-SettingVU::setUIControl()-69";
        btn_1 = this->getUIControlButton(tr("ROSE Style")   , 1, this->btn_1_lb);
        btn_2 = this->getUIControlButton(tr("Mcintosh Style"), 2, this->btn_2_lb);
        btn_3 = this->getUIControlButton(tr("Green Style")   , 3, this->btn_3_lb);
        btn_4 = this->getUIControlButton(tr("Purple Style") , 4, this->btn_4_lb);
        btn_5 = this->getUIControlButton(tr("Red Style")    , 5, this->btn_5_lb);
        btn_6 = this->getUIControlButton(tr("White Style")  , 6, this->btn_6_lb);
        btn_7 = this->getUIControlButton(tr("A style")  , 7, this->btn_7_lb);
        btn_8 = this->getUIControlButton(tr("B Style")  , 8, this->btn_8_lb);
        btn_9 = this->getUIControlButton(tr("C Style")  , 9, this->btn_9_lb);
        btn_10 = this->getUIControlButton(tr("D Style")  , 10, this->btn_10_lb);
        btn_11 = this->getUIControlButton(tr("RA180 Style")    , 11, this->btn_11_lb);
       // btn_12 = this->getUIControlButton(tr("Level Meter Style")  , 12, this->btn_12_lb);
 //print_debug();
        //btn_13 = this->getUIControlButton(tr("Level Meter Style2")  , 13, this->btn_13_lb);
        //btn_14 = this->getUIControlButton(tr("Level Meter Style3")  , 14, this->btn_14_lb);


    }else if(global.device.getDeviceType()=="RS150" ){
qDebug() << "-SettingVU::setUIControl()-88";
        btn_1 = this->getUIControlButton(tr("ROSE Style")   , 1, this->btn_1_lb);
        btn_2 = this->getUIControlButton(tr("Mcintosh Style"), 2, this->btn_2_lb);
        btn_3 = this->getUIControlButton(tr("Mint Style")   , 3, this->btn_3_lb);
        btn_4 = this->getUIControlButton(tr("Purple Style") , 4, this->btn_4_lb);
        btn_5 = this->getUIControlButton(tr("Red Style")    , 5, this->btn_5_lb);
        btn_6 = this->getUIControlButton(tr("White Style")  , 6, this->btn_6_lb);

        btn_7 = this->getUIControlButton(tr("A Style")  , 7, this->btn_7_lb);
        btn_8 = this->getUIControlButton(tr("B Style")  , 8, this->btn_8_lb);
        btn_9 = this->getUIControlButton(tr("C Style")  , 9, this->btn_11_lb);

        btn_10 = this->getUIControlButton(tr("D Style")  , 10, this->btn_14_lb);
        btn_11 = this->getUIControlButton(tr("E Style")    , 11, this->btn_9_lb);
        btn_12 = this->getUIControlButton(tr("F Style")  , 12, this->btn_10_lb);

        btn_13 = this->getUIControlButton(tr("RA180 Style")  , 13, this->btn_13_lb);
      //  btn_14 = this->getUIControlButton(tr("Level Meter Style")  , 14, this->btn_12_lb);


    }else if(global.device.getDeviceType()=="RS150B" ){
qDebug() << "-SettingVU::setUIControl()-109";
        btn_1 = this->getUIControlButton(tr("ROSE Style")   , 1, this->btn_1_lb);
        btn_2 = this->getUIControlButton(tr("Mcintosh Style"), 2, this->btn_2_lb);
        btn_3 = this->getUIControlButton(tr("Mint Style")   , 3, this->btn_3_lb);
        btn_4 = this->getUIControlButton(tr("Purple Style") , 4, this->btn_4_lb);
        btn_5 = this->getUIControlButton(tr("Red Style")    , 5, this->btn_5_lb);
        btn_6 = this->getUIControlButton(tr("White Style")  , 6, this->btn_6_lb);
qDebug() << "-SettingVU::setUIControl()-116";
        btn_7 = this->getUIControlButton(tr("ROSE Style1")  , 7, this->btn_7_lb);
        btn_8 = this->getUIControlButton(tr("ROSE Style2")  , 8, this->btn_8_lb);
        btn_9 = this->getUIControlButton(tr("C Style")  , 9, this->btn_11_lb);
qDebug() << "-SettingVU::setUIControl()-120";
        btn_10 = this->getUIControlButton(tr("ROSE Style3")  , 10, this->btn_14_lb);
        btn_11 = this->getUIControlButton(tr("ROSE Style4")    , 11, this->btn_9_lb);
        btn_12 = this->getUIControlButton(tr("ROSE Style5")  , 12, this->btn_10_lb);
qDebug() << "-SettingVU::setUIControl()-124";
        btn_13 = this->getUIControlButton(tr("RA180 Style")  , 13, this->btn_13_lb);
qDebug() << "-SettingVU::setUIControl()-126";

    }


    fl_vu = new FlowLayout();
    fl_vu->setContentsMargins(20,28,0,0);
    fl_vu->setSpacingHV(10, 10);
    if(global.device.getDeviceType()=="RS250" || global.device.getDeviceType()=="RS250A"){
        fl_vu->addWidget(btn_1);
        fl_vu->addWidget(btn_2);
        fl_vu->addWidget(btn_3);
        fl_vu->addWidget(btn_4);
        fl_vu->addWidget(btn_5);
        fl_vu->addWidget(btn_6);
        fl_vu->addWidget(btn_7);
        fl_vu->addWidget(btn_8);
        fl_vu->addWidget(btn_9);
        fl_vu->addWidget(btn_10);
        fl_vu->addWidget(btn_11);
        fl_vu->addWidget(btn_12);

    }else if(global.device.getDeviceType()=="RS201"){
        fl_vu->addWidget(btn_1);
        fl_vu->addWidget(btn_2);
        fl_vu->addWidget(btn_3);
        fl_vu->addWidget(btn_4);
        fl_vu->addWidget(btn_5);
        fl_vu->addWidget(btn_6);
        fl_vu->addWidget(btn_7);
        fl_vu->addWidget(btn_8);
        fl_vu->addWidget(btn_9);
        fl_vu->addWidget(btn_10);
        fl_vu->addWidget(btn_11);
        fl_vu->addWidget(btn_12);

    }else if(global.device.getDeviceType()=="RS520"){
        fl_vu->addWidget(btn_1);
        fl_vu->addWidget(btn_2);
        fl_vu->addWidget(btn_3);
        fl_vu->addWidget(btn_4);
        fl_vu->addWidget(btn_5);
        fl_vu->addWidget(btn_6);
        fl_vu->addWidget(btn_7);
        fl_vu->addWidget(btn_8);
        fl_vu->addWidget(btn_9);
        fl_vu->addWidget(btn_10);
        fl_vu->addWidget(btn_11);
        //fl_vu->addWidget(btn_12);

    }else if(global.device.getDeviceType()=="RS150"){
        fl_vu->addWidget(btn_1);
        fl_vu->addWidget(btn_2);
        fl_vu->addWidget(btn_3);
        fl_vu->addWidget(btn_4);
        fl_vu->addWidget(btn_5);
        fl_vu->addWidget(btn_6);
        fl_vu->addWidget(btn_7);
        fl_vu->addWidget(btn_8);
        fl_vu->addWidget(btn_9);
        fl_vu->addWidget(btn_10);
        fl_vu->addWidget(btn_11);
        fl_vu->addWidget(btn_12);
        fl_vu->addWidget(btn_13);
       // fl_vu->addWidget(btn_14);
        //fl_vu->addWidget(btn_15);


    }else if(global.device.getDeviceType()=="RS150B"){
        qDebug() << "-SettingVU::setUIControl()-181";
        fl_vu->addWidget(btn_1);
        fl_vu->addWidget(btn_2);
        fl_vu->addWidget(btn_3);
        fl_vu->addWidget(btn_4);
        fl_vu->addWidget(btn_5);
        fl_vu->addWidget(btn_6);
        fl_vu->addWidget(btn_7);
        fl_vu->addWidget(btn_8);
        fl_vu->addWidget(btn_9);
        fl_vu->addWidget(btn_10);
        fl_vu->addWidget(btn_11);
        fl_vu->addWidget(btn_12);
        fl_vu->addWidget(btn_13);
       // fl_vu->addWidget(btn_14);
        //fl_vu->addWidget(btn_15);


    }

    QWidget *widget_flow = new QWidget();   // FlowLayout 을 QWidget으로 한번 더 안 감싸고 VBoxLayout에 넣으면 sizeHint.Height가 이상해서 원하는 UI 가 안나와서 QWidget으로 한번 더 감싸음.
    widget_flow->setContentsMargins(0,0,0,0);
    widget_flow->setObjectName("widget_flow");
    widget_flow->setStyleSheet("border-left:1px solid #303030;");
    widget_flow->setLayout(fl_vu);


    // 전체
    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("VU theme")));
    //vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("VU 테마")));
    vl_total->addWidget(widget_flow);
    vl_total->addSpacing(25);
    vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("VU option")));
    //vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("VU 옵션")));
    vl_total->addWidget(this->getUIControlOption(tr("Stay in VU Mode")));
    //vl_total->addWidget(this->getUIControlOption(tr("VU 모드 유지")));

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

    this->setLayout(vl_scroll);
}

/**
 * @brief SettingVU::getUIControlButton
 * @param text
 * @param p_lb
 * @return
 */
QPushButton* SettingVU::getUIControlButton(QString text, int vu_mode, QLabel* p_lb){
    QLabel *lb_style_rose = new QLabel(text);
    lb_style_rose->setStyleSheet("font-size:17px;color:#E5E5E4;background-color:transparent");

    QPushButton *btn_style_rose = new QPushButton();
    btn_style_rose->setCursor(Qt::PointingHandCursor);
    btn_style_rose->setObjectName("btn_style_rose");
    btn_style_rose->setProperty("vu_mode", vu_mode);
    QVBoxLayout *vl_style_rose = new QVBoxLayout();
    vl_style_rose->setContentsMargins(0,0,0,0);
    vl_style_rose->setSpacing(0);
    vl_style_rose->setAlignment(Qt::AlignCenter);
    //vl_style_rose->addWidget(img_style_rose_selected);
    vl_style_rose->addWidget(p_lb);
    vl_style_rose->addSpacing(20);
    vl_style_rose->addWidget(lb_style_rose, 0, Qt::AlignCenter);
    btn_style_rose->setLayout(vl_style_rose);
    btn_style_rose->setFixedSize(311,186);
    btn_style_rose->setStyleSheet("#btn_style_rose { background-color:#000000;border:1px solid #707070; }");

    // 커넥션
    connect(btn_style_rose, SIGNAL(clicked()), this, SLOT(slot_changedThema()));

    return btn_style_rose;
}


/**
 * @brief SettingVU::getUIControlOption VU 옵션 UI
 * @param p_title
 * @return
 */
QWidget* SettingVU::getUIControlOption(QString p_title){

    QLabel *lb_title = new QLabel(p_title);
    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;");

    onOff = new OnOffWidget;
    //onOff->setValue(false);
    onOff->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,15,47,15);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_title);
    hl_lb->addWidget(onOff, 0, Qt::AlignRight);

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");

    widget_total->setLayout(hl_lb);

    // 커넥션
    connect(onOff, SIGNAL(signal_changed(bool)), this, SLOT(slot_modeOnOff(bool)));
    return widget_total;
}

/**
 * @brief SettingVU::requestVUData 현재 VU모드 가져오기
 */
void SettingVU::requestVUData(){

    this->clearUnSelectMode();  // 클리어

    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_VLC_GET, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("vu.mode.type.get"), tmp_json, true);
}

/**
 * @brief SettingVU::slot_changedThema [SLOT] 테마 변경 세팅 API 호출
 */
void SettingVU::slot_changedThema(){
    if(global.device.CurrPlayType == "VIDEO" || global.device.CurrPlayType == "YOUTUBE" || global.device.CurrPlayType == "TIDAL_VIDEO" || global.device.CurrPlayType == "BUGS_MV"){
        ToastMsg::show(this, "", tr("Not supported in current mode."));
    }
    else
    {
        this->clearUnSelectMode();  // 클리어

        //int vu_mode = sender()->property("vu_mode").toInt();
        this->curr_vu_sytle = sender()->property("vu_mode").toInt();
        print_debug();
        qDebug() << "this->curr_vu_sytle= " << this->curr_vu_sytle;
        //this->setVUMode(vu_mode);
        this->setVUMode(this->curr_vu_sytle);
        //dynamic_cast<QPushButton*>(sender())->setStyleSheet(dynamic_cast<QPushButton*>(sender())->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        QJsonObject tmp_json;
        //tmp_json.insert("vu_mode", vu_mode);
        tmp_json.insert("vu_mode", this->curr_vu_sytle);
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_VLC_SET, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("vu.mode.type.set"), tmp_json, true);
    }
}
/**
 * @brief SettingVU::slot_modeOnOff [SLOT] VU 모드 ON/OFF 변경 API 호출
 * @param flagOn
 */
void SettingVU::slot_modeOnOff(bool flagOn){

    bool curr_onOff = this->onOff->getCurrentValue();
    if(global.device.CurrPlayType == "VIDEO" || global.device.CurrPlayType == "YOUTUBE" || global.device.CurrPlayType == "TIDAL_VIDEO" || global.device.CurrPlayType == "BUGS_MV"){
        ToastMsg::show(this, "", tr("Not supported in current mode."));
    }
    else
    {
        if(this->delay_flag){
            print_debug();
            this->delay_flag = false;
            qDebug() << "this->delay_flag=" << this->delay_flag;

            this->clearUnSelectMode();  // 클리어
            //int vu_mode = 0;
            print_debug();
            qDebug() << "this->curr_vu_sytle=" << this->curr_vu_sytle;
            if(flagOn==false){
                print_debug();
                //this->setVUMode(0);
                //this->curr_vu_on_off = 0;
                this->curr_vu_sytle = 0;

            }else{

                if(this->curr_vu_sytle <= 0){
                    this->curr_vu_sytle = 1;
                }
                print_debug();
                //this->setVUMode(this->curr_vu_sytle);
                //this->curr_vu_on_off = 1;

            }
            this->setVUMode(this->curr_vu_sytle);

            QJsonObject tmp_json;
            tmp_json.insert("vu_mode", this->curr_vu_sytle);

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_VLC_SET, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("vu.mode.type.set"), tmp_json, true);
        }else{
            print_debug();
            this->delay_flag = false;
            //this->onOff->setValue(curr_onOff);
            qDebug() << "slow click......";
        }
    }
}
/**
 * @brief SettingVU::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void SettingVU::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    print_debug();
    QJsonDocument doc(p_jsonObject);    QString strJson(doc.toJson(QJsonDocument::Compact));    qDebug() << strJson;

    switch(p_id){
    case HTTP_VLC_SET :
        if(p_jsonObject.contains("flagOk")&& p_jsonObject["flagOk"].toBool()){

            this->delay_flag = false;

            print_debug();
            requestVUData();

        }else{
            this->delay_flag = false;
            //this->onOff->setValue(!this->onOff->getCurrentValue());
        }

        break;
    case HTTP_VLC_GET :

        if(p_jsonObject.contains("vu_mode")){

            this->delay_flag = true;

            this->curr_vu_sytle = p_jsonObject["vu_mode"].toInt();
            this->curr_vu_on_off = p_jsonObject["vu_on_off"].toInt();
            if(this->curr_vu_sytle > 0){
                this->onOff->setValue(true);
                qDebug() << "-SettingVU::slot_responseHttp";
                this->curr_vu_on_off = 1;
                this->setVUMode(this->curr_vu_sytle);
            }else{
                this->onOff->setValue(false);
            }
        }else{
            this->delay_flag = false;
             this->onOff->setValue(false);
             ToastMsg::show(this, "", tr("Not supported in current mode."));
        }

    }

    sender()->deleteLater();
}

/**
 * @brief SettingVU::clearUnSelectMode 스타일 초기화
 */
void SettingVU::clearUnSelectMode(){
    //this->onOff->setValue(false);
qDebug() << "-SettingVU::clearUnSelectMode()";// << p_vu_mode;
    //print_debug();
    // 이미지 초기화
    this->btn_1_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_yellow_off.png"));
    this->btn_2_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_blue_off.png"));
    this->btn_3_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_mint_off.png"));
    this->btn_4_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_purple_off.png"));
    this->btn_5_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_red_off.png"));
    this->btn_6_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_white_off.png"));
    this->btn_7_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_4_off.png"));
    this->btn_8_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_3_off.png"));
    this->btn_9_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_1_off.png"));
    this->btn_10_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_2_off.png"));

    this->btn_11_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_7_off.png"));
    this->btn_12_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_8_off.png"));
    this->btn_13_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_5_off.png"));
    this->btn_14_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_6_off.png"));
    //print_debug();


    if(global.device.getDeviceType()=="RS250" || global.device.getDeviceType()=="RS250A"){
        this->btn_1->setStyleSheet(this->btn_1->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_2->setStyleSheet(this->btn_2->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_3->setStyleSheet(this->btn_3->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_4->setStyleSheet(this->btn_4->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_5->setStyleSheet(this->btn_5->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_6->setStyleSheet(this->btn_6->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_7->setStyleSheet(this->btn_7->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_8->setStyleSheet(this->btn_8->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_9->setStyleSheet(this->btn_9->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_10->setStyleSheet(this->btn_10->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));

        this->btn_11->setStyleSheet(this->btn_11->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_12->setStyleSheet(this->btn_12->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));

    }else if(global.device.getDeviceType()=="RS201"){
        this->btn_1->setStyleSheet(this->btn_1->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_2->setStyleSheet(this->btn_2->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_3->setStyleSheet(this->btn_3->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_4->setStyleSheet(this->btn_4->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_5->setStyleSheet(this->btn_5->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_6->setStyleSheet(this->btn_6->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_7->setStyleSheet(this->btn_7->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_8->setStyleSheet(this->btn_8->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_9->setStyleSheet(this->btn_9->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_10->setStyleSheet(this->btn_10->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));

        this->btn_11->setStyleSheet(this->btn_11->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_12->setStyleSheet(this->btn_12->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));


    }else if(global.device.getDeviceType()=="RS520"){
        this->btn_1->setStyleSheet(this->btn_1->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_2->setStyleSheet(this->btn_2->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_3->setStyleSheet(this->btn_3->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_4->setStyleSheet(this->btn_4->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_5->setStyleSheet(this->btn_5->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_6->setStyleSheet(this->btn_6->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_7->setStyleSheet(this->btn_7->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_8->setStyleSheet(this->btn_8->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_9->setStyleSheet(this->btn_9->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_10->setStyleSheet(this->btn_10->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));

        this->btn_11->setStyleSheet(this->btn_11->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        //this->btn_12->setStyleSheet(this->btn_12->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));


    }else if(global.device.getDeviceType()=="RS150" ){
        qDebug() << "-SettingVU::clearUnSelectMode()";
        this->btn_1->setStyleSheet(this->btn_1->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_2->setStyleSheet(this->btn_2->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_3->setStyleSheet(this->btn_3->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_4->setStyleSheet(this->btn_4->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_5->setStyleSheet(this->btn_5->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_6->setStyleSheet(this->btn_6->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_7->setStyleSheet(this->btn_7->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_8->setStyleSheet(this->btn_8->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_9->setStyleSheet(this->btn_9->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_10->setStyleSheet(this->btn_10->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));

        this->btn_11->setStyleSheet(this->btn_11->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_12->setStyleSheet(this->btn_12->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));

        this->btn_13->setStyleSheet(this->btn_13->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
       // this->btn_14->setStyleSheet(this->btn_14->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        //print_debug();
    }else if(global.device.getDeviceType()=="RS150B" ){
        qDebug() << "-SettingVU::clearUnSelectMode()";
        this->btn_1->setStyleSheet(this->btn_1->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_2->setStyleSheet(this->btn_2->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_3->setStyleSheet(this->btn_3->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_4->setStyleSheet(this->btn_4->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_5->setStyleSheet(this->btn_5->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_6->setStyleSheet(this->btn_6->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_7->setStyleSheet(this->btn_7->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_8->setStyleSheet(this->btn_8->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_9->setStyleSheet(this->btn_9->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_10->setStyleSheet(this->btn_10->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));

        this->btn_11->setStyleSheet(this->btn_11->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));
        this->btn_12->setStyleSheet(this->btn_12->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));

        this->btn_13->setStyleSheet(this->btn_13->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));

    }

    //this->btn_15->setStyleSheet(this->btn_15->styleSheet().replace("border:3px solid #B18658;", "border:1px solid #707070;"));


}

/**
 * @brief SettingVU::setVUMode 해당 모드로 스타일 세팅
 * @param p_vu_mode
 */
void SettingVU::setVUMode(int p_vu_mode){//c220713

    qDebug() << "SettingVU::setVUMode---p_vu_mode= " << p_vu_mode;
    if(global.device.getDeviceType()=="RS250" || global.device.getDeviceType()=="RS250A"){
       // print_debug();
        if(p_vu_mode==1){
            this->btn_1_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_yellow.png"));
            this->btn_1->setStyleSheet(this->btn_1->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==2){
            this->btn_2_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_blue.png"));
            this->btn_2->setStyleSheet(this->btn_2->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==3){
            this->btn_3_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_mint.png"));
            this->btn_3->setStyleSheet(this->btn_3->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==4){
            this->btn_4_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_purple.png"));
            this->btn_4->setStyleSheet(this->btn_4->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==5){
            this->btn_5_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_red.png"));
            this->btn_5->setStyleSheet(this->btn_5->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==6){
            this->btn_6_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_white.png"));
            this->btn_6->setStyleSheet(this->btn_6->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==7){
            this->btn_7_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_4.png"));
            this->btn_7->setStyleSheet(this->btn_7->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==8){
            this->btn_8_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_3.png"));
            this->btn_8->setStyleSheet(this->btn_8->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==9){
            this->btn_9_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_1.png"));
            this->btn_9->setStyleSheet(this->btn_9->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==10){
            this->btn_10_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_2.png"));
            this->btn_10->setStyleSheet(this->btn_10->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==11){
            this->btn_11_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_7.png"));
            this->btn_11->setStyleSheet(this->btn_11->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==12){
            this->btn_12_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_8.png"));
            this->btn_12->setStyleSheet(this->btn_12->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }
        // print_debug();

    }else if(global.device.getDeviceType()=="RS201"){
        if(p_vu_mode==1){
            this->btn_1_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_yellow.png"));
            this->btn_1->setStyleSheet(this->btn_1->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==2){
            this->btn_2_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_blue.png"));
            this->btn_2->setStyleSheet(this->btn_2->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==3){
            this->btn_3_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_mint.png"));
            this->btn_3->setStyleSheet(this->btn_3->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==4){
            this->btn_4_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_purple.png"));
            this->btn_4->setStyleSheet(this->btn_4->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==5){
            this->btn_5_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_red.png"));
            this->btn_5->setStyleSheet(this->btn_5->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==6){
            this->btn_6_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_white.png"));
            this->btn_6->setStyleSheet(this->btn_6->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==7){
            this->btn_7_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_4.png"));
            this->btn_7->setStyleSheet(this->btn_7->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==8){
            this->btn_8_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_3.png"));
            this->btn_8->setStyleSheet(this->btn_8->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==9){
            this->btn_9_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_1.png"));
            this->btn_9->setStyleSheet(this->btn_9->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==10){
            this->btn_10_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_2.png"));
            this->btn_10->setStyleSheet(this->btn_10->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==11){
            this->btn_11_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_7.png"));
            this->btn_11->setStyleSheet(this->btn_11->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==12){
            this->btn_12_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_8.png"));
            this->btn_12->setStyleSheet(this->btn_12->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }

    }else if(global.device.getDeviceType()=="RS520"){
        if(p_vu_mode==1){
            this->btn_1_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_yellow.png"));
            this->btn_1->setStyleSheet(this->btn_1->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==2){
            this->btn_2_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_blue.png"));
            this->btn_2->setStyleSheet(this->btn_2->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==3){
            this->btn_3_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_mint.png"));
            this->btn_3->setStyleSheet(this->btn_3->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==4){
            this->btn_4_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_purple.png"));
            this->btn_4->setStyleSheet(this->btn_4->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==5){
            this->btn_5_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_red.png"));
            this->btn_5->setStyleSheet(this->btn_5->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==6){
            this->btn_6_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_white.png"));
            this->btn_6->setStyleSheet(this->btn_6->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==7){
            this->btn_7_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_4.png"));
            this->btn_7->setStyleSheet(this->btn_7->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==8){
            this->btn_8_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_3.png"));
            this->btn_8->setStyleSheet(this->btn_8->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==9){
            this->btn_9_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_1.png"));
            this->btn_9->setStyleSheet(this->btn_9->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==10){
            this->btn_10_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_2.png"));
            this->btn_10->setStyleSheet(this->btn_10->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==11){
            this->btn_11_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_7.png"));
            this->btn_11->setStyleSheet(this->btn_11->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==12){
           // this->btn_12_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_8.png"));
           // this->btn_12->setStyleSheet(this->btn_12->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }

    }else if(global.device.getDeviceType()=="RS150" ){
        qDebug() << "SettingVU::setVUMode-RS150--p_vu_mode= " << p_vu_mode;
        if(p_vu_mode==1){
            this->btn_1_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_yellow.png"));
            this->btn_1->setStyleSheet(this->btn_1->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==2){
            this->btn_2_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_blue.png"));
            this->btn_2->setStyleSheet(this->btn_2->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==3){
            this->btn_3_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_mint.png"));
            this->btn_3->setStyleSheet(this->btn_3->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==4){
            this->btn_4_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_purple.png"));
            this->btn_4->setStyleSheet(this->btn_4->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==5){
            this->btn_5_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_red.png"));
            this->btn_5->setStyleSheet(this->btn_5->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==6){
            this->btn_6_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_white.png"));
            this->btn_6->setStyleSheet(this->btn_6->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==7){
            this->btn_7_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_4.png"));
            this->btn_7->setStyleSheet(this->btn_7->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==8){
            this->btn_8_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_3.png"));
            this->btn_8->setStyleSheet(this->btn_8->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==9){
            this->btn_11_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_7.png"));
            this->btn_9->setStyleSheet(this->btn_9->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==10){
            this->btn_14_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_6.png"));
            this->btn_10->setStyleSheet(this->btn_10->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==11){
            this->btn_9_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_1.png"));
            this->btn_11->setStyleSheet(this->btn_11->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==12){
            this->btn_10_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_2.png"));
            this->btn_12->setStyleSheet(this->btn_12->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==13){
            this->btn_13_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_5.png"));
            this->btn_13->setStyleSheet(this->btn_13->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }
        /*else if(p_vu_mode==14){
            this->btn_12_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_8.png"));
            this->btn_14->setStyleSheet(this->btn_14->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }*/


    }else if(global.device.getDeviceType()=="RS150B" ){
        qDebug() << "SettingVU::setVUMode-RS150B--p_vu_mode= " << p_vu_mode;
        if(p_vu_mode==1){
            this->btn_1_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_yellow.png"));
            this->btn_1->setStyleSheet(this->btn_1->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==2){
            this->btn_2_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_blue.png"));
            this->btn_2->setStyleSheet(this->btn_2->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==3){
            this->btn_3_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_mint.png"));
            this->btn_3->setStyleSheet(this->btn_3->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==4){
            this->btn_4_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_purple.png"));
            this->btn_4->setStyleSheet(this->btn_4->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==5){
            this->btn_5_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_red.png"));
            this->btn_5->setStyleSheet(this->btn_5->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==6){
            this->btn_6_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_white.png"));
            this->btn_6->setStyleSheet(this->btn_6->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==7){
            this->btn_7_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_4.png"));
            this->btn_7->setStyleSheet(this->btn_7->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==8){
            this->btn_8_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_3.png"));
            this->btn_8->setStyleSheet(this->btn_8->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==9){
            this->btn_11_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_7.png"));
            this->btn_9->setStyleSheet(this->btn_9->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==10){
            this->btn_14_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_6.png"));
            this->btn_10->setStyleSheet(this->btn_10->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==11){
            this->btn_9_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_1.png"));
            this->btn_11->setStyleSheet(this->btn_11->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==12){
            this->btn_10_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_2.png"));
            this->btn_12->setStyleSheet(this->btn_12->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }else if(p_vu_mode==13){
            this->btn_13_lb->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/vu_5.png"));
            this->btn_13->setStyleSheet(this->btn_13->styleSheet().replace("border:1px solid #707070;", "border:3px solid #B18658;"));
        }

    }


}

