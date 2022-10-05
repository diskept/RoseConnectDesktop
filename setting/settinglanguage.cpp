#include "settinglib.h"//cheon01
#include "data/data_media_library.h"//cheon01
#include "settinglanguage.h"//cheon01
#include "widget/toastmsg.h"//cheon01
#include <common/gscommon.h>
#include "common/rosesettings.h"
#include <QJsonObject>//cheon01
#include <QScrollArea>
#include <QSettings>
#include <QComboBox>

#include "common/global.h"//cheon01
#include <common/networkhttp.h>//cheon01
#include <QDebug>
#include <QProcess>//cheon07_language
#include <QApplication>//cheon07_language
#include <QFileDialog>
#include <QStandardPaths>
#include <common/sqlitehelper.h>
#include "mainwindow.h"

const int HTTP_LANGUAGE_VALUE_GET = 103;
const int HTTP_LANGUAGE_VALUE_SET = 104;

const QString QUERY_CREATE_TABLE = "CREATE TABLE IF NOT EXISTS Language (idx int, lang int)";
const QString QUERY_DROP_TABLE = "DROP TABLE Language";
const QString QUERY_DEL = "DELETE FROM Language";
const QString QUERY_SEL = "SELECT * FROM Language";

/**
 * @brief SettingTimer::SettingTimer 생성자
 * @param parent
 */
SettingLanguage::SettingLanguage(QWidget *parent) : QWidget(parent)
{
    this->setUIControl();
}

/**
 * @brief SettingTimer::setUIControl 기본 UI 세팅
 */
void SettingLanguage::setUIControl()
{
    //print_debug(); //cheon01
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
    //print_debug(); //cheon01
}


/**
 * @brief SettingTimer::getUIControlOption 콤보박스 포함된 ROW 생성한다
 * @param p_title
 * @return
 */
QWidget* SettingLanguage::getUIControlOption(QString p_title){

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
    comboBox_language->setCurrentIndex(global.lang);
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
 *
 *
 */
/*
    QString stdPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString srclangFilePath = qApp->applicationDirPath()+"/myapp_ko.qm";
    QString srclangFilePath3 = qApp->applicationDirPath()+"/albumdetail.obj";
    QString tgtlangFilePath1 = QString("%1%2").arg(stdPath).arg("/translations/myapp_ko.qm");
    QString tgtlangFilePath2 = qApp->applicationDirPath()+"/translations";

    QString selectedLanguage_str = comboBox_language->itemText(index);

    qDebug() << " index: " << index;
    qDebug() << " srclangFilePath: " << srclangFilePath;
    qDebug() << " tgtlangFilePath1: " << tgtlangFilePath1;
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
                    print_debug(); qDebug() << " cmd: " << QString("del %1").arg(tgtlangFilePath1);  //cheon06-mypage//cheon06-mypage
            }


        }else{
          //  ToastMsg::show(this, "", tr("이미 영어 모드를 선택하였습니다."));//cheon01
            ToastMsg::show(this, "", tr("You have already selected the English mode."));//cheon01
        }

    }else if(index == 1){
        if(QFile::exists(srclangFilePath)){
                qDebug() << "srclangFilePath is exists";
        }else{
                qDebug() << "srclangFilePath is not exists";
        }


        // proc->start(QString("copy %1 %2").arg(srclangFilePath).arg(tgtlangFilePath2));
        if(QFile::exists(srclangFilePath)&&!QFile::exists(tgtlangFilePath1)){
            qDebug() << "if(!QFile::exists(tgtlangFilePath1))--tgtlangFilePath1" << tgtlangFilePath1;
         if(QFile::copy(srclangFilePath, tgtlangFilePath2)){
           //  ToastMsg::show(this, "", srclangFilePath);
             ToastMsg::show(this, "", tr("Run the program again to change to the selected korean language mode."));
           //  ToastMsg::show(this, "", tr("선택한 한국어 언어 모드로 변경을 위해서는  프로그램을 다시 실행하세요."));
             print_debug(); qDebug() << " cmd: " << QString("copy %1 %2").arg(srclangFilePath).arg(tgtlangFilePath1);  //cheon06-mypage//cheon06-mypage

         }
        }else{
            qDebug()<< "You have already selected the korean language mode.";
            //  ToastMsg::show(this, "", tr("이미 한국어 모드를 선택하였습니다."));//cheon01
              ToastMsg::show(this, "", tr("You have already selected the korean language mode."));//cheon01
        }
    }else{
         ToastMsg::show(this, "", tr("There is no  language selected."));//cheon01
    }
*/

void SettingLanguage::slot_changedIndexLanguage(int index){

    int idx = 0;
        int sel = index;

        DialogConfirm *dlg = new DialogConfirm(this);
        dlg->setTitle(tr("Changing Language Notice"));
        QString lang = tr("English");
        if(sel == 1){
            lang = tr("Korea");
        }
        dlg->setText(QString(tr("Are you sure you want to change to %1 ? If accepted, Rose_Connect will be re-launched. ")).arg(lang));//c220511
        print_debug();
        int result;
        if(global.lang  != sel){
            result = dlg->exec();
            if(result==QDialog::Accepted){
                SqliteHelper *sqliteHelper = new SqliteHelper(this);
                //QSqlError err = sqlite->addConnectionRose();      // Rose에서 받는 DB 파일 수정 변경 불가
                QSqlError err = sqliteHelper->addConnectionLocal();

                QString strQuery_insert = "INSERT INTO Language (idx, lang) VALUES";
                strQuery_insert.append( QString("(%1, %2)").arg(idx).arg(sel));

                qDebug() << strQuery_insert;

                // 항상 테이블 생성을 체크하고, 삭제처리 한다. for 초기화 (항상 레코드 최대 1개 유지)
                sqliteHelper->exec(QUERY_CREATE_TABLE);
                sqliteHelper->exec(QUERY_DEL);
                sqliteHelper->exec(strQuery_insert);

                // SELECT 처리
                QVariantList *list = new QVariantList();
                sqliteHelper->exec(QUERY_SEL, *list);
                sqliteHelper->close();
                if(err.type() == QSqlError::NoError){
                    print_debug();
                    foreach(QVariant val, *list){
                        QMap<QString, QVariant> map = val.toMap();
                        idx = map["idx"].toInt();
                        sel = map["lang"].toInt();
                        global.lang = sel;

                        qApp->exit( MainWindow::EXIT_CODE_REBOOT );
                        break;

                    }
                }

                //sqliteHelper->close();
            }else{
                comboBox_language->setCurrentIndex(global.lang);
                dlg->close();
            }
        }

        //delete sqliteHelper;

        qDebug() << "select data -> idx : " << idx << ", sel : " << sel;

}


void SettingLanguage::requestGetValueLanguage(){
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
void SettingLanguage::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

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

