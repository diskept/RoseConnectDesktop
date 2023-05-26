#include "settingmenu.h"//c230312
#include "common/global.h"
#include "widget/toastmsg.h"

#include <QScrollArea>
#include <common/gscommon.h>
#include <common/networkhttp.h>
#include <widget/dialogconfirm.h>
#include <common/sqlitehelper.h>
#include "bugs/BugsSettings.h"//c230329
#include "tidal/TidalSettings.h"//c230329
#include "qobuz/QobuzSettings.h"//c230329

const int HTTP_MENU_SET = 99;
const int HTTP_MENU_GET = 98;

const QString QUERY_CREATE_TABLE = "CREATE TABLE IF NOT EXISTS Leftmenu (idx int, menu int)";
const QString QUERY_DROP_TABLE = "DROP TABLE Leftmenu";
const QString QUERY_DEL = "DELETE FROM Leftmenu";
const QString QUERY_SEL = "SELECT * FROM Leftmenu";


/**
 * @brief SettingMenu::SettingMenu 생성자
 * @param parent
 */
SettingMenu::SettingMenu(QWidget *parent) : QWidget(parent)//c230312
{
    this->setUIControl();
}

/**
 * @brief SettingMenu::setUIControl UI 세팅
 */
void SettingMenu::setUIControl()
{
    print_debug();
    this->linker = Linker::getInstance();
    // 전체
    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("Menu Service Option")));
    vl_total->addWidget(this->getUIControlOptionTidal(tr("TIDAL Service Use")));
    vl_total->addWidget(this->getUIControlOptionBugs(tr("Bugs Service Use")));
    vl_total->addWidget(this->getUIControlOptionQobuz(tr("Qobuz Service Use")));

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
print_debug();
    this->setLayout(vl_scroll);
    selectTableLeftmenu();//c230329
}


/**
 * @brief SettingMenu::getUIControlOption VLC 플레이어 사용 on/off
 * @param p_title
 * @return
 */
QWidget* SettingMenu::getUIControlOptionTidal(QString p_title){//c230518
print_debug();
    QLabel *lb_img = new QLabel();
    QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/main_icon_tidal.png");
    lb_img->setPixmap(*pixmap_icon);                                   //버퍼에 있는 이미지를 QLabel에 출력
    lb_img->resize(pixmap_icon->width(), pixmap_icon->height());       //QLabel의 크기를 이미지 사이즈에 맞추어 조절한다.
    QLabel *lb_title = new QLabel(p_title);
    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;");

    QHBoxLayout *hl_title = new QHBoxLayout();
    hl_title->setContentsMargins(0,0,0,0);
    hl_title->setSpacing(0);
    hl_title->addWidget(lb_img, 0, Qt::AlignLeft);
    hl_title->addWidget(lb_title, 0, Qt::AlignLeft);

    QWidget *widget_title = new QWidget();
    widget_title->setFixedSize(190,70);
    widget_title->setLayout(hl_title);

    onOff_Tidal = new OnOffWidget;
    //onOff->setValue(false);
    onOff_Tidal->setCursor(Qt::PointingHandCursor);



    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,15,47,15);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(widget_title);
    hl_lb->addWidget(onOff_Tidal, 0, Qt::AlignRight);

    QPushButton *btn_onOffHover_Tidal = new QPushButton(onOff_Tidal);
    btn_onOffHover_Tidal->setFixedSize(80,40);
    btn_onOffHover_Tidal->setStyleSheet("background-color:transparent;border:0px;");


    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);
print_debug();
    // 커넥션
    //connect(onOff, SIGNAL(signal_changed(bool)), this, SLOT(slot_modeOnOff(bool)));
    connect(btn_onOffHover_Tidal, SIGNAL(clicked()), this, SLOT(slot_modeOnOff_Tidal()));
    return widget_total;
}
QWidget* SettingMenu::getUIControlOptionBugs(QString p_title){//c230518
print_debug();
    QLabel *lb_img = new QLabel();
    QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/main_icon_bugs.png");
    lb_img->setPixmap(*pixmap_icon);                                   //버퍼에 있는 이미지를 QLabel에 출력
    lb_img->resize(pixmap_icon->width(), pixmap_icon->height());
    QLabel *lb_title = new QLabel(p_title);
    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;");

    QHBoxLayout *hl_title = new QHBoxLayout();
    hl_title->setContentsMargins(0,0,0,0);
    hl_title->setSpacing(0);
    hl_title->addWidget(lb_img, 0, Qt::AlignLeft);
    hl_title->addWidget(lb_title, 0, Qt::AlignLeft);

    QWidget *widget_title = new QWidget();
    widget_title->setFixedSize(190,70);
    widget_title->setLayout(hl_title);

    onOff_Bugs = new OnOffWidget;
    //onOff->setValue(false);
    onOff_Bugs->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,15,47,15);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(widget_title);
    hl_lb->addWidget(onOff_Bugs, 0, Qt::AlignRight);

    QPushButton *btn_onOffHover_Bugs = new QPushButton(onOff_Bugs);
    btn_onOffHover_Bugs->setFixedSize(80,40);
    btn_onOffHover_Bugs->setStyleSheet("background-color:transparent;border:0px;");

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);
print_debug();
    // 커넥션
    //connect(onOff, SIGNAL(signal_changed(bool)), this, SLOT(slot_modeOnOff(bool)));
    connect(btn_onOffHover_Bugs, SIGNAL(clicked()), this, SLOT(slot_modeOnOff_Bugs()));
    return widget_total;
}
QWidget* SettingMenu::getUIControlOptionQobuz(QString p_title){//c230518
print_debug();
    QLabel *lb_img = new QLabel();
    QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/main_icon_qobuz.png");
    lb_img->setPixmap(*pixmap_icon);                                   //버퍼에 있는 이미지를 QLabel에 출력
    lb_img->resize(pixmap_icon->width(), pixmap_icon->height());
    QLabel *lb_title = new QLabel(p_title);
    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;");

    QHBoxLayout *hl_title = new QHBoxLayout();
    hl_title->setContentsMargins(0,0,0,0);
    hl_title->setSpacing(0);
    hl_title->addWidget(lb_img, 0, Qt::AlignLeft);
    hl_title->addWidget(lb_title, 0, Qt::AlignLeft);

    QWidget *widget_title = new QWidget();
    widget_title->setFixedSize(200,70);
    widget_title->setLayout(hl_title);

    onOff_Qobuz = new OnOffWidget;
    //onOff->setValue(false);
    onOff_Qobuz->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,15,47,15);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(widget_title);
    hl_lb->addWidget(onOff_Qobuz, 0, Qt::AlignRight);

    QPushButton *btn_onOffHover_Qobuz = new QPushButton(onOff_Qobuz);
    btn_onOffHover_Qobuz->setFixedSize(80,40);
    btn_onOffHover_Qobuz->setStyleSheet("background-color:transparent;border:0px;");

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);
print_debug();
    // 커넥션
    //connect(onOff, SIGNAL(signal_changed(bool)), this, SLOT(slot_modeOnOff(bool)));
    connect(btn_onOffHover_Qobuz, SIGNAL(clicked()), this, SLOT(slot_modeOnOff_Qobuz()));
    return widget_total;
}

/**
 * @brief SettingMenu::requestVideoSetData 현재 VLC 플레이어 사용 유무 가져오기
 */
void SettingMenu::requestVideoSetData(){
/*
    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_MENU_GET, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("get_video_setting_value"), tmp_json, true);
    */
}


void SettingMenu::changedIndexLeftmenu(int index, int menu){

    print_debug();
    qDebug() << "function input---index=" << index << " , "<< "menu=" << menu;
    SqliteHelper *sqliteHelper = new SqliteHelper(this);
    QSqlError err = sqliteHelper->addConnectionLocal();

    QString query = QString("UPDATE Leftmenu SET menu = %1 WHERE idx = %2").arg(menu).arg(index);
    sqliteHelper->exec(query);

    // SELECT 처리
    QVariantList *list = new QVariantList();
    sqliteHelper->exec(QUERY_SEL, *list);
    sqliteHelper->close();
    if(err.type() == QSqlError::NoError){
        print_debug();
        foreach(QVariant val, *list){
            QMap<QString, QVariant> map = val.toMap();
            int idx = map["idx"].toInt();
            int sel = map["menu"].toInt();
            qDebug() << "idx = " << idx << " ,menu=" << sel;
        }
    }

}

void SettingMenu::selectTableLeftmenu(){//c230329

    print_debug();
    SqliteHelper *sqliteHelper = new SqliteHelper(this);
    QSqlError err = sqliteHelper->addConnectionLocal();

    // SELECT 처리
    QVariantList *list = new QVariantList();
    sqliteHelper->exec(QUERY_SEL, *list);
    sqliteHelper->close();
    if(err.type() == QSqlError::NoError){
        print_debug();
        foreach(QVariant val, *list){
            QMap<QString, QVariant> map = val.toMap();
            int idx = map["idx"].toInt();
            int sel = map["menu"].toInt();
            qDebug() << "idx = " << idx << " ,menu=" << sel;
            if(idx == 1){
                this->onOff_Tidal->setValue(bool(sel));
            }else if(idx == 2){
                this->onOff_Qobuz->setValue(bool(sel));
            }else if(idx == 3){
                this->onOff_Bugs->setValue(bool(sel));
            }else {

            }
        }
    }

}


/**
 * @brief SettingMenu::slot_modeOnOff [SLOT] VU 모드 ON/OFF 변경 API 호출
 */
//void SettingMenu::slot_modeOnOff(bool flagOn){
void SettingMenu::slot_modeOnOff_Tidal(){//c230329
    print_debug();
    DialogConfirm *dlg = new DialogConfirm(this);
    //dlg->move(this->pos().x()-dlg->width(), global.mouse_py);
    dlg->setTitle(tr("TIDAL Menu Service Change"));
    //dlg->setTitle(tr("비디오 플레이어 변경"));
    if(onOff_Tidal->getCurrentValue() == 0){
        dlg->setText(tr("Do you want the TIDAL menu to appear in the left menu services?"));
    }else if(onOff_Tidal->getCurrentValue() == 1){
        dlg->setText(tr("Do you want to remove the TIDAL menu in the left menu services?"));
    }
    int result = dlg->exec();
    if(result==QDialog::Accepted){

       bool isVLC = !this->onOff_Tidal->getCurrentValue();
       this->onOff_Tidal->setValue(isVLC);
       //this->list_menuItem[i]->getMenuCode()

       emit linker->signal_leftmenuOnOff(QString(GSCommon::MainMenuCode::Tidal), this->onOff_Tidal->getCurrentValue());
       changedIndexLeftmenu(1,this->onOff_Tidal->getCurrentValue());

//       if(!this->onOff_Tidal->getCurrentValue()){
//           tidal::TidalSettings *tidal = new tidal::TidalSettings(this);
//           tidal->slot_clickBtnLogout();
//       }

/*
       QJsonObject tmp_json;
       tmp_json.insert("settingType", "MENU_SERVICE_USE");
       tmp_json.insert("isMENU_SERVICE", isVLC);
       NetworkHttp *network = new NetworkHttp;
       connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
       network->request(HTTP_MENU_SET, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("set_video_setting_value"), tmp_json, true);
       */
    }
}
void SettingMenu::slot_modeOnOff_Bugs(){//c230329
print_debug();
    DialogConfirm *dlg = new DialogConfirm(this);

    //qDebug() << "dlg->width()=" << dlg->width();
    //dlg->move(this->pos().x()-dlg->width(), global.mouse_py);
    dlg->setTitle(tr("Bugs Menu Service Change"));
    //dlg->setTitle(tr("비디오 플레이어 변경"));
    if(onOff_Bugs->getCurrentValue() == 0){
        dlg->setText(tr("Do you want the Bugs menu to appear in the left menu services?"));
    }else if(onOff_Bugs->getCurrentValue() == 1){
        dlg->setText(tr("Do you want to remove the Bugs menu in the left menu services?"));
    }
    int result = dlg->exec();
    if(result==QDialog::Accepted){

       bool isVLC = !this->onOff_Bugs->getCurrentValue();
       this->onOff_Bugs->setValue(isVLC);
       emit linker->signal_leftmenuOnOff(QString(GSCommon::MainMenuCode::Bugs), this->onOff_Bugs->getCurrentValue());
       changedIndexLeftmenu(3,this->onOff_Bugs->getCurrentValue());

//       if(!this->onOff_Bugs->getCurrentValue()){
//           bugs::BugsSettings *bugs = new bugs::BugsSettings(this);
//           bugs->slot_clickBtnLogout();
//       }


    }
}
void SettingMenu::slot_modeOnOff_Qobuz(){//c230329
print_debug();
    DialogConfirm *dlg = new DialogConfirm(this);
    //dlg->move(this->pos().x()-dlg->width(), global.mouse_py);
    dlg->setTitle(tr("Qobuz Menu Service Change"));
    //dlg->setTitle(tr("비디오 플레이어 변경"));
    if(onOff_Qobuz->getCurrentValue() == 0){
        dlg->setText(tr("Do you want the Qobuz menu to appear in the left menu services?"));
    }else if(onOff_Qobuz->getCurrentValue() == 1){
        dlg->setText(tr("Do you want to remove the Qobuz menu in the left menu services?"));
    }
    int result = dlg->exec();
    if(result==QDialog::Accepted){

       bool isVLC = !this->onOff_Qobuz->getCurrentValue();
       this->onOff_Qobuz->setValue(isVLC);
       emit linker->signal_leftmenuOnOff(QString(GSCommon::MainMenuCode::Qobuz), this->onOff_Qobuz->getCurrentValue());
       changedIndexLeftmenu(2,this->onOff_Qobuz->getCurrentValue());

//       if(!this->onOff_Qobuz->getCurrentValue()){
//           qobuz::QobuzSettings *qobuz = new qobuz::QobuzSettings(this);
//           qobuz->slot_clickBtnLogout();
//       }

    }
}

/**
 * @brief SettingMenu::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void SettingMenu::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    /*
    switch(p_id){
    case HTTP_MENU_SET :
        break;
    case HTTP_MENU_GET :
        bool isInstallVlc = false;
        if(p_jsonObject.contains("isInstallMenuServoce")){
            isInstallVlc = p_jsonObject["isInstallVlc"].toBool();
        }
        if(isInstallVlc==false){
            ToastMsg::show(this, "", tr("Menu Service is not installed. You can use it after installing it in the market"));
            this->onOff->setValue(false);
            this->onOff->setEnabled(false);
        }else{
            this->onOff->setEnabled(true);
            if(p_jsonObject.contains("isVLC")){
                bool isVLC = p_jsonObject["isMenuService"].toBool();
                this->onOff->setValue(isVLC);
            }
        }
        break;
    }

    sender()->deleteLater();
    */
}
