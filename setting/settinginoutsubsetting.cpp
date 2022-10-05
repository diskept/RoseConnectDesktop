#include "settinginoutsubsetting.h"
#include "common/global.h"

#include <QScrollArea>
#include <widget/flowlayout.h>
#include <common/gscommon.h>
#include <common/networkhttp.h>

#include "widget/toastmsg.h"
#include "widget/dialogconfirm.h"
#include "login/dialogselect_usbdac.h"
#include <QDebug>
#include <QMessageBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QStandardPaths>
#include <QApplication>

#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";

const int HTTP_LANGUAGE_VALUE_GET = 103;
const int HTTP_LANGUAGE_VALUE_SET = 104;

settingInoutSubsetting::settingInoutSubsetting(QWidget *parent) : QWidget(parent)
{
    setUIControl();
}


void settingInoutSubsetting::setUIControl(){

    //qDebug() << "settingInoutSubsetting::setUIControl() is run---" ;

    lb_language_res = new QLabel();
    lb_language_res->setStyleSheet("font-size:17px;color:#666666;font:bold;");
    // 전체

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("Language Option")));//cheon01

    vl_total->addWidget(this->getUIControlOption(tr("Language Setting")));//cheon01
    vl_total->addWidget(lb_language_res );//cheon01


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
 * @brief SettingTimer::getUIControlOption 콤보박스 포함된 ROW 생성한다
 * @param p_title
 * @return
 */
QWidget* settingInoutSubsetting::getUIControlOption(QString p_title){

    //print_debug(); //cheon01
    //qDebug() << "p_title : " <<p_title ;//cheon01
    QLabel *lb_title = new QLabel(p_title);
    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;");

    this->list_keyValue.append(0);
    this->list_keyValue.append(1);


    comboBox_language = new QComboBox();
    comboBox_language->setContentsMargins(0,0,0,0);
    comboBox_language->setObjectName("comboBox_timer");//cheon01
    comboBox_language->setFixedWidth(340);
    comboBox_language->addItem(tr("English"), 0);//cheon01_src
    comboBox_language->addItem(tr("Korea"), 1);//cheon01_src

    comboBox_language->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                //"QAbstractItemView QAbstractItemView::item {  padding-left:5px; } "
                                //"QComboBox QAbstractItemView:selected { color:red;selection-background-color:red } "
                                //"QComboBox QAbstractItemView:hover { color:red;selection-background-color:red } "
                                );
    /*
    "QComboBox::item { font-size:18px;color:red; background-color:#333333; }"

    " QListView::item { color:red; } "
    " QListView::item:selected { border: 1px solid #B18658;background-color:#B18658;  }"
    " QListView::item:hover { background-color:#B18658; } "
    "QListView::item:selected:active { background-color:#B18658; } "

    "#comboBox_timer:item { color:#FFFFFF;height:35px; } "
    "#comboBox_timer:item:selected { background-color:#B18658; } "
    "#comboBox_timer:item::hover { background-color:#B18658; } "
    */

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,15,47,15);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_title);
    hl_lb->addWidget(comboBox_language);

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);

    // 커넥션
    //connect(comboBox, &QComboBox::currentIndexChanged, this, &SettingTimer::slot_changedIndexPod);
    connect(comboBox_language, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexLanguage(int)));

    //print_debug(); //cheon01
    return widget_total;
}


/**
 * @brief SettingDisplay::requestGetValueDisplay 현재 Display값을 SET 하는 API 호출한다.
 */
/**
 * @brief SettingDisplay::setTextVolumn [SLOT]
 * @param p_value
 */
void settingInoutSubsetting::slot_changedIndexLanguage(int index){

    QString stdPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString srclangFilePath = qApp->applicationDirPath()+"/myapp_ko.qm";
    QString tgtlangFilePath1 = QString("%1%2").arg(stdPath).arg("/translations/myapp_ko.qm");
    QString tgtlangFilePath2 = qApp->applicationDirPath()+"/translations";

    QString selectedLanguage_str = comboBox_language->itemText(index);

    //qDebug() << " stdPath: " << stdPath;
    //installer.value("HomeDir") + "/AppData/Local/YoutubeGetData/ROSE CONNECT/translations/myapp_ko.qm"
    //QFile src_qmfile, tgt_qmfile;
    //src_qmfile.setFileName(srclangFilePath);
    //src_qmfile.setPermissions(QFile::ReadOwner| QFile::WriteOwner |QFile::ReadUser| QFile::WriteUser | QFile::ReadOther | QFile::WriteOther);

    //tgt_qmfile.setFileName(tgtlangFilePath1);
   // tgt_qmfile.setPermissions(QFile::ReadOwner| QFile::WriteOwner |QFile::ReadUser| QFile::WriteUser | QFile::ReadOther | QFile::WriteOther);
//ToastMsg::show(this, "", stdPath);
    if(index == 0){
         //proc->start(QString("del %1").arg(tgtlangFilePath1));
        if(QFile::exists(tgtlangFilePath1)){
            if(QFile::remove(tgtlangFilePath1)){
                  //  ToastMsg::show(this, "", tgtlangFilePath1);
                    ToastMsg::show(this, "", tr("Run the program again to change to the selected english language mode."));
                  //  ToastMsg::show(this, "", tr("선택한 영어 언어 모드로 변경을 위해서는  프로그램을 다시 실행하세요."));
                    //print_debug(); qDebug() << " cmd: " << QString("del %1").arg(tgtlangFilePath1);  //cheon06-mypage//cheon06-mypage
            }


        }else{
          //  ToastMsg::show(this, "", tr("이미 영어 모드를 선택하였습니다."));//cheon01
            ToastMsg::show(this, "", tr("You have already selected the English mode."));//cheon01
        }

    }else if(index == 1){

        // proc->start(QString("copy %1 %2").arg(srclangFilePath).arg(tgtlangFilePath2));
        if(!QFile::exists(tgtlangFilePath1)){
         if(QFile::copy(srclangFilePath, tgtlangFilePath1)){
           //  ToastMsg::show(this, "", srclangFilePath);
             ToastMsg::show(this, "", tr("Run the program again to change to the selected korean language mode."));
           //  ToastMsg::show(this, "", tr("선택한 한국어 언어 모드로 변경을 위해서는  프로그램을 다시 실행하세요."));
             //print_debug(); qDebug() << " cmd: " << QString("copy %1 %2").arg(srclangFilePath).arg(tgtlangFilePath1);  //cheon06-mypage//cheon06-mypage

         }
        }else{
            //  ToastMsg::show(this, "", tr("이미 한국어 모드를 선택하였습니다."));//cheon01
              ToastMsg::show(this, "", tr("You have already selected the korean language mode."));//cheon01
        }
    }else{
         ToastMsg::show(this, "", tr("There is no  language selected."));//cheon01
    }

}


void settingInoutSubsetting::requestGetValueLanguage(){
    //print_debug(); //cheon01
    if(global.device.getDeviceIP()!=""){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        // API Timer RESOLUTION 조절
        QJsonObject json;
        //json.insert("roseToken",global.device.getDeviceRoseToken());
        //qDebug() << "QString("http://%1:%2/sleep.time.get").arg(global.device.getDeviceIP()).arg(global.port)" << QString("http://%1:%2/sleep.time.get").arg(global.device.getDeviceIP()).arg(global.port);
        //QJsonDocument doc(json);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "GET----Timer_p_jsonObject : " << strJson;//cheon01.
        network->request(HTTP_LANGUAGE_VALUE_GET, QString("http://%1:%2/sleep.time.get").arg(global.device.getDeviceIP()).arg(global.port), json, true);
    }else {
        ToastMsg::show(this, "", tr("Failed to get ROSE device IP"));//cheon01

    }
}

/**
 * @brief SettingDisplay::responseHttp : 로즈 장치 정보요청 결과 처리
 * @param p_jsonObject QJsonObject&
 */
void settingInoutSubsetting::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    //print_debug();//cheon01
    //qDebug() << "GET----Timer_p_id : " << p_id;//cheon01_src.

    switch(p_id){
    case HTTP_LANGUAGE_VALUE_GET :

        if(p_jsonObject.contains("timer")){//cheon01_src
            int tmp_timer = p_jsonObject["timer"].toInt();//cheon01_src

            //print_debug();//cheon01
            //qDebug() << " tmp_timer : " << tmp_timer ;//cheon01
            if(tmp_timer == 0){
                this->lb_language_res->setText(tr("       Current Language :       ") + QString(tr("English:0")));
            }
            else {
                this->lb_language_res->setText(tr("       Current Language :       ") + QString(tr("Korea:1")));
            }

            //print_debug();//cheon01
            //QJsonDocument doc(p_jsonObject);
            //QString strJson(doc.toJson(QJsonDocument::Compact));
            //qDebug() << "GET----Timer_p_jsonObject : " << strJson;//cheon01.
        }

        break;
    case HTTP_LANGUAGE_VALUE_SET :
        //print_debug();//cheon01
        //QJsonDocument doc(p_jsonObject);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "SET---Timer_p_jsonObject : " << strJson;//cheon01.
        break;
    }
   sender()->deleteLater();
}



