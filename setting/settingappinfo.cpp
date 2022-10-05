#include "settingappinfo.h"
#include "common/global.h"

#include <QScrollArea>
#include <QFile>
#include <common/gscommon.h>
#include <common/networkhttp.h>
#include <common/rosesettings.h>

/**
 * @brief SettingAppInfo::SettingAppInfo 생성자
 * @param parent
 */
SettingAppInfo::SettingAppInfo(QWidget *parent) : QWidget(parent)
{
    this->setUIControl();
    this->readFileLicense();
}
/**
 * @brief SettingAppInfo::setUIControl UI 세팅
 */
void SettingAppInfo::setUIControl(){

    lb_licese = new QLabel();
    lb_licese->setContentsMargins(33,26,33,40);
    lb_licese->setAlignment(Qt::AlignTop);
    lb_licese->setWordWrap(true);
    lb_licese->setStyleSheet("font-size:14px;color:#919191;background-color:#000000;");

    QString ver = QString(" (%1)").arg(QDate::currentDate().toString("yyMMdd").replace(" ", ""));

    // 전체
    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("Rose Connect app Info")));
    vl_total->addWidget(this->getUIControlTitleSub(tr("Rose Connect app version"), rosesettings.APPLICATION_VERSION + ver));
    vl_total->addWidget(this->getUIControlTitleSub(global.device.getDeviceName(), global.device.getDeviceVersion()));
    vl_total->addWidget(this->getUIControlTitleSubClick(tr("Open Source License")));
    /*
     *     vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("로즈커넥트 앱 정보")));
    vl_total->addWidget(this->getUIControlTitleSub(tr("로즈 커넥트 앱 버전"), rosesettings.APPLICATION_VERSION));
    vl_total->addWidget(this->getUIControlTitleSub(global.device.getDeviceName(), global.device.getDeviceVersion()));
    vl_total->addWidget(this->getUIControlTitleSubClick(tr("오픈소스 라이센스")));
    */
    vl_total->addWidget(lb_licese);
    vl_total->addSpacing(50);

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

    // 커넥션
    connect(btn_license, SIGNAL(clicked()), this, SLOT(slot_showHideLicese()));
    this->lb_licese->setVisible(false);
}




/**
 * @brief SettingAppInfo::getUIControlTitle 타이틀 및 버전명
 * @param p_title
 * @param p_version
 * @return
 */
QWidget* SettingAppInfo::getUIControlTitleSub(QString p_title, QString p_version){

    // Text
    QLabel *lb_title = new QLabel(p_title);
    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;");

    QLabel *lb_version = new QLabel(p_version);
    lb_version->setStyleSheet("color:#B18658;font-size:18px;font:bold;");

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,15,74,15);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_title);
    hl_lb->addWidget(lb_version, 0, Qt::AlignRight);

    QWidget *widget_bar = new QWidget();
    widget_bar->setObjectName("widget_bar");
    widget_bar->setFixedHeight(1);
    widget_bar->setStyleSheet("#widget_bar { background-color:#060606 } ");
    QWidget *widget_bar2 = new QWidget();
    widget_bar2->setObjectName("widget_bar2");
    widget_bar2->setFixedHeight(1);
    widget_bar2->setStyleSheet("#widget_bar2 { background-color:#303030 } ");

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);
    vlayout->addLayout(hl_lb);
    vlayout->addWidget(widget_bar);
    vlayout->addWidget(widget_bar2);

    QWidget *widget_title = new QWidget();
    widget_title->setObjectName("widget_titleSub");
    widget_title->setContentsMargins(0,0,0,0);
    widget_title->setLayout(vlayout);
    widget_title->setStyleSheet("#widget_titleSub { border-bottom:1px solid #303030;border-left:1px solid #303030; }");
    widget_title->setFixedHeight(70);

    return widget_title;
}

/**
 * @brief SettingAppInfo::getUIControlTitleSubClick
 * @param p_title
 * @return
 */
QWidget* SettingAppInfo::getUIControlTitleSubClick(QString p_title){

    // Text
    QLabel *lb_title = new QLabel(p_title);
    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;");

    pixmap_icon_show = GSCommon::getUIPixmapImg(":/images/icon_open_w.png");
    pixmap_icon_hide = GSCommon::getUIPixmapImg(":/images/icon_hide_w.png");

    lb_icon_arrow = new QLabel();
    lb_icon_arrow->setPixmap(*pixmap_icon_show);
    lb_icon_arrow->resize(pixmap_icon_show->width(), pixmap_icon_show->height());

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,15,47,15);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_title);
    hl_lb->addWidget(lb_icon_arrow, 0, Qt::AlignRight);

    QWidget *widget_bar = new QWidget();
    widget_bar->setObjectName("widget_bar");
    widget_bar->setFixedHeight(1);
    widget_bar->setStyleSheet("#widget_bar { background-color:#060606 } ");
    QWidget *widget_bar2 = new QWidget();
    widget_bar2->setObjectName("widget_bar2");
    widget_bar2->setFixedHeight(1);
    widget_bar2->setStyleSheet("#widget_bar2 { background-color:#303030 } ");

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);
    vlayout->addLayout(hl_lb);
    vlayout->addWidget(widget_bar);
    vlayout->addWidget(widget_bar2);

    btn_license = new QPushButton();
    btn_license->setObjectName("widget_titleSub");
    btn_license->setContentsMargins(0,0,0,0);
    btn_license->setLayout(vlayout);
    btn_license->setStyleSheet("#widget_titleSub { border:0px;border-left:1px solid #303030; }");
    btn_license->setFixedHeight(70);
    btn_license->setCursor(Qt::PointingHandCursor);

    return btn_license;
}

/**
 * @brief SettingAppInfo::slot_showHideLicese [SLOT]
 */
void SettingAppInfo::slot_showHideLicese(){

    if(this->lb_licese->isVisible()==false){
        this->lb_licese->setVisible(true);
        lb_icon_arrow->setPixmap(*pixmap_icon_hide);
        lb_icon_arrow->resize(pixmap_icon_hide->width(), pixmap_icon_hide->height());

    }else{
        this->lb_licese->setVisible(false);
        lb_icon_arrow->setPixmap(*pixmap_icon_show);
        lb_icon_arrow->resize(pixmap_icon_show->width(), pixmap_icon_show->height());
    }
}

/**
 * @brief SettingAppInfo::readFileLicense 오픈소스 라이센스 txt 파일 읽어서 세팅한다.
 */
void SettingAppInfo::readFileLicense(){

    QString filename = ":/opensource_license.txt";
    QFile file(filename);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        return;
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");           // for Korean
    QString myText = in.readAll();
    this->lb_licese->setText(myText);
    file.close();

}
