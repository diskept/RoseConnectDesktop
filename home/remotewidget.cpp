#include "home/remotewidget.h"

#include "home/remotebtnitem.h"
#include "home/sectionbottom.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/rosesettings.h"//c0121

#include "widget/toastmsg.h"

#include <QEvent>
#include <QJsonObject>
#include <QPainter>
#include <QSlider>
#include <QVBoxLayout>
#include <QPainterPath>

const int HTTP_REMOTE_CONTROL = 0;

const int HTTP_REMOTE_STANDBY = 1;
const int HTTP_REMOTE_REBOOT = 2;
const int HTTP_REMOTE_POWER = 3;

const int HTTP_REMOTE_BACK = 4;
const int HTTP_REMOTE_HOME = 5;
const int HTTP_REMOTE_LCD = 6;
const int HTTP_REMOTE_CLOCK = 7;

const int HTTP_REMOTE_PREV = 8;
const int HTTP_REMOTE_PLAYPAUSE = 9;
const int HTTP_REMOTE_NEXT = 10;

const int HTTP_REMOTE_EXTE = 11;
const int HTTP_REMOTE_VU = 12;
const int HTTP_REMOTE_TIMER = 13;
const int HTTP_REMOTE_HDMI = 14;
const int HTTP_REMOTE_DLNA = 15;
const int HTTP_REMOTE_AIRPLAY = 16;

const int HTTP_REMOTE_VOLUME_ADD = 17;
const int HTTP_REMOTE_VOLUME_SUB = 18;
const int HTTP_REMOTE_MUTE = 19;

const int HTTP_REMOTE_AMP = 20;


RemoteWidget::RemoteWidget(QWidget *parent) : QWidget(parent)
{

    linker = Linker::getInstance();
    connect(linker, SIGNAL(signal_connected()), SLOT(slot_connectedDevice()));

    this->widget_remote_onlyRose = new QWidget();
    this->widget_remote_onlyRose->setStyleSheet("background-color:transparent; border-radius:0px;");
    this->widget_remote_onlyRose->setFixedSize(953, 806);

    this->widget_remote_withAmp = new QWidget();
    this->widget_remote_withAmp->setStyleSheet("background-color:transparent; border-radius:0px;");
    this->widget_remote_withAmp->setFixedSize(953, 806);

    this->stackedWidget_remote = new QStackedWidget();
    this->stackedWidget_remote->addWidget(this->widget_remote_onlyRose);
    this->stackedWidget_remote->addWidget(this->widget_remote_withAmp);
    this->stackedWidget_remote->setCurrentIndex(0);

    this->scrollArea_main = new VerticalScrollArea();
    this->scrollArea_main->setWidget(this->stackedWidget_remote);
    this->scrollArea_main->setWidgetResizable(true);
    this->scrollArea_main->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->scrollArea_main->setStyleSheet("background-color:transparent; border-radius:0px;");
    this->scrollArea_main->setAlignment(Qt::AlignTop);

    this->vbox_remote_main = new QVBoxLayout();
    GSCommon::clearLayout(this->vbox_remote_main);
    this->vbox_remote_main->setContentsMargins(0, 0, 0, 0);
    this->vbox_remote_main->addWidget(this->scrollArea_main);

    this->setUIRose();
    this->setUIRemoteBtn();

    QWidget *widget_remote_base = new QWidget();
    widget_remote_base->setStyleSheet("background-color:#333333; border-top-left-radius:20px; border-bottom-left-radius:20px;");
    widget_remote_base->setLayout(this->vbox_remote_main);

    QVBoxLayout *vbox_remote_base = new QVBoxLayout();
    GSCommon::clearLayout(vbox_remote_base);
    vbox_remote_base->setContentsMargins(0, 0, 0, 0);
    vbox_remote_base->addWidget(widget_remote_base);

    this->setLayout(vbox_remote_base);
    this->hide();
}


RemoteWidget::~RemoteWidget(){

    this->deleteLater();
}


/**
 * @brief RemoteWidget::setInitData
 */
void RemoteWidget::setInitData(){

}


/**
 * @brief RemoteWidget::setUIRose 로즈 연결기기 관련 UI 세팅
 */
void RemoteWidget::setUIRose(){

    // ============================================================
    // 현재 연결 된 기기
    // ============================================================
    QLabel *lb_connect_version = new QLabel(this->widget_remote_onlyRose);
    lb_connect_version->setFixedSize(400, 40);
    lb_connect_version->setGeometry(30, 40, 0, 0);
    lb_connect_version->setStyleSheet("font-size:16px; color:#E6E6E6; padding-left:16px; background-color:#4D4D4D; border-radius:10px;");
    lb_connect_version->setText(QString(tr("Rose Connect Ver %1  (Build Date: %2)")).arg(rosesettings.APPLICATION_VERSION).arg(rosesettings.APPLICATION_BUILD_DATE));

    this->widget_curr_device = new QWidget(this->widget_remote_onlyRose);
    this->widget_curr_device->setFixedSize(400, 290);
    this->widget_curr_device->setGeometry(30, 105, 0, 0);
    this->widget_curr_device->setStyleSheet("background-color:#4D4D4D; border-radius:10px;");

    QLabel *lb_curr_title = new QLabel(this->widget_curr_device);
    lb_curr_title->setFixedSize(400, 50);
    lb_curr_title->setGeometry(0, 0, 0, 0);
    lb_curr_title->setStyleSheet("background-color:#4D4D4D; border-top-left-radius:10px; border-top-right-radius:10px; font-size:18px; color:#E6E6E6; padding-left:16px;");
    lb_curr_title->setText(tr("Connected device"));

    QWidget *widget_device_info = new QWidget(this->widget_curr_device);
    widget_device_info->setFixedSize(398, 237);
    widget_device_info->setGeometry(1, 51, 0, 0);
    widget_device_info->setStyleSheet("background:qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #000000,stop:1 #444444); border-top-right-radius:0px; border-top-left-radius:0px;");

    this->lb_curr_img = new QLabel(widget_device_info);
    this->lb_curr_img->setFixedSize(275, 115);
    this->lb_curr_img->setGeometry(62, 30, 0, 0);
    this->lb_curr_img->setStyleSheet("background-color:transparent;");

    this->lb_curr_name = new QLabel(widget_device_info);

    this->lb_curr_id = new QLabel(widget_device_info);

    this->lb_curr_ip = new QLabel(widget_device_info);

    // ============================================================
    // 연결 가능한 기기 UI
    // ============================================================
    this->widget_search = new SearchDeviceWidget(this->widget_remote_onlyRose);
    this->widget_search->setMouseTracking(true);
    this->widget_search->setGeometry(30, 420, 400, 350);

    connect(this->widget_search, SIGNAL(signal_selectedDevice(QString)), this, SIGNAL(connectNewDevice(QString)));
    connect(this->widget_search, SIGNAL(signal_searchWidget_adjust(bool)), this, SLOT(slot_searchWidget_adjust(bool)));
}


void RemoteWidget::slot_searchWidget_adjust(bool flag){

    if(flag == true){
        this->widget_curr_device->hide();
        this->widget_search->setGeometry(30, 100, 400, 670);
    }
    else{
        this->widget_search->setGeometry(30, 420, 400, 350);
        this->widget_curr_device->show();
    }
}


/**
 * @brief RemoteWidget::setUIRemoteBtn 리모콘 버튼 관련 UI 세팅
 */
void RemoteWidget::setUIRemoteBtn(){

    // ============================================================
    // Standby Mode, Reboot, Power Off UI
    // ============================================================

    QPushButton *btn_standby = new QPushButton(this->widget_remote_onlyRose);
    btn_standby->setObjectName("btn_standby");
    btn_standby->setProperty("type", HTTP_REMOTE_STANDBY);
    btn_standby->setCursor(Qt::PointingHandCursor);
    btn_standby->setFixedSize(100, 90);
    btn_standby->setGeometry(450, 40, 0, 0);
    btn_standby->setStyleSheet("#btn_standby{background-color:#4D4D4D; border-radius:10px;} #btn_standby:hover{background-color:#7D6144; border-radius:10px;}");

    QLabel *lb_standby_img = GSCommon::getUILabelImg(":/images/remote_standby.png", btn_standby);
    lb_standby_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    QLabel *lb_standby_name = new QLabel(btn_standby);
    lb_standby_name->setAlignment(Qt::AlignCenter);
    lb_standby_name->setWordWrap(true);
    lb_standby_name->setText(tr("Standby Mode"));

    if(global.lang == 0){
        lb_standby_img->setFixedSize(52, 40);
        lb_standby_img->setGeometry(24, 8, 0, 0);

        lb_standby_name->setStyleSheet("background-color:transparent; border-radius:0px; font-size:13px; color:#E4E4E3;");
        lb_standby_name->setFixedSize(68, 26);
        lb_standby_name->setGeometry(16, 53, 0, 0);

    }
    else if(global.lang == 1){
        lb_standby_img->setFixedSize(52, 40);
        lb_standby_img->setGeometry(24, 13, 0, 0);

        lb_standby_name->setStyleSheet("background-color:transparent; border-radius:0px; font-size:16px; color:#E4E4E3;");
        lb_standby_name->setFixedSize(68, 18);
        lb_standby_name->setGeometry(16, 58, 0, 0);
    }

    QPushButton *btn_reboot = new QPushButton(this->widget_remote_onlyRose);
    btn_reboot->setObjectName("btn_reboot");
    btn_reboot->setProperty("type", HTTP_REMOTE_REBOOT);
    btn_reboot->setCursor(Qt::PointingHandCursor);
    btn_reboot->setFixedSize(100, 90);
    btn_reboot->setGeometry(450, 145, 0, 0);
    btn_reboot->setStyleSheet("#btn_reboot{background-color:#4D4D4D; border-radius:10px;} #btn_reboot:hover{background-color:#7D6144; border-radius:10px;}");

    QLabel *lb_reboot_img = GSCommon::getUILabelImg(":/images/remote_boot.png", btn_reboot);
    lb_reboot_img->setFixedSize(52, 40);
    lb_reboot_img->setGeometry(24, 13, 0, 0);
    lb_reboot_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    QLabel *lb_reboot_name = new QLabel(btn_reboot);
    lb_reboot_name->setFixedSize(68, 18);
    lb_reboot_name->setGeometry(16, 58, 0, 0);
    lb_reboot_name->setAlignment(Qt::AlignCenter);
    lb_reboot_name->setStyleSheet("background-color:transparent; border-radius:0px; font-size:16px; color:#E4E4E3;");
    lb_reboot_name->setWordWrap(true);
    lb_reboot_name->setText(tr("Reboot"));

    QPushButton *btn_power = new QPushButton(this->widget_remote_onlyRose);
    btn_power->setObjectName("btn_power");
    btn_power->setProperty("type", HTTP_REMOTE_POWER);
    btn_power->setCursor(Qt::PointingHandCursor);
    btn_power->setFixedSize(100, 90);
    btn_power->setGeometry(450, 250, 0, 0);
    btn_power->setStyleSheet("#btn_power{background-color:#4D4D4D; border-radius:10px;} #btn_power:hover{background-color:#7D6144; border-radius:10px;}");

    QLabel *lb_power_img = GSCommon::getUILabelImg(":/images/remote_pow.png", btn_power);
    lb_power_img->setFixedSize(52, 40);
    lb_power_img->setGeometry(24, 13, 0, 0);
    lb_power_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    QLabel *lb_power_name = new QLabel(btn_power);
    lb_power_name->setFixedSize(100, 18);
    lb_power_name->setGeometry(0, 58, 0, 0);
    lb_power_name->setAlignment(Qt::AlignCenter);
    lb_power_name->setStyleSheet("background-color:transparent; border-radius:0px; font-size:16px; color:#E4E4E3;");
    lb_power_name->setWordWrap(true);
    lb_power_name->setText(tr("Power Off"));


    // ============================================================
    // Previous, Rose hone, LDC On/Off, Clock UI
    // ============================================================

    QWidget *widget_display = new QWidget(this->widget_remote_onlyRose);
    widget_display->setFixedSize(252, 300);
    widget_display->setGeometry(566, 40, 0, 0);
    widget_display->setStyleSheet("background-color:#4D4D4D; border-radius:10px;");

    QPushButton *btn_back = new QPushButton(widget_display);
    btn_back->setObjectName("btn_back");
    btn_back->setProperty("type", HTTP_REMOTE_BACK);
    btn_back->setCursor(Qt::PointingHandCursor);
    btn_back->setFixedSize(96, 96);
    btn_back->setGeometry(20, 35, 0, 0);
    btn_back->setStyleSheet("#btn_back{background-color:#777777; border-radius:48px;} #btn_back:hover{background-color:#7D6144; border-radius:48px;}");

    QLabel *lb_back_img = GSCommon::getUILabelImg(":/images/remote_back.png", btn_back);
    lb_back_img->setFixedSize(52, 40);
    lb_back_img->setGeometry(22, 17, 0, 0);
    lb_back_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    QLabel *lb_back_name = new QLabel(btn_back);
    lb_back_name->setFixedSize(64, 18);
    lb_back_name->setGeometry(16, 62, 0, 0);
    lb_back_name->setAlignment(Qt::AlignCenter);
    lb_back_name->setStyleSheet(global.lang == 0 ? "background-color:transparent; border-radius:0px; font-size:14px; color:#E4E4E3;" :
                                                   "background-color:transparent; border-radius:0px; font-size:16px; color:#E4E4E3;");
    lb_back_name->setText(tr("Previous"));

    QPushButton *btn_home = new QPushButton(widget_display);
    btn_home->setObjectName("btn_home");
    btn_home->setProperty("type", HTTP_REMOTE_HOME);
    btn_home->setCursor(Qt::PointingHandCursor);
    btn_home->setFixedSize(96, 96);
    btn_home->setGeometry(136, 35, 0, 0);
    btn_home->setStyleSheet("#btn_home{background-color:#777777; border-radius:48px;} #btn_home:hover{background-color:#7D6144; border-radius:48px;}");

    QLabel *lb_rosehome_img = GSCommon::getUILabelImg(":/images/remote_home.png", btn_home);
    lb_rosehome_img->setFixedSize(52, 40);
    lb_rosehome_img->setGeometry(22, 17, 0, 0);
    lb_rosehome_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    QLabel *lb_rosehome_name = new QLabel(btn_home);
    lb_rosehome_name->setAlignment(Qt::AlignCenter);
    lb_rosehome_name->setWordWrap(true);
    lb_rosehome_name->setStyleSheet(global.lang == 0 ? "background-color:transparent; border-radius:0px; font-size:14px; color:#E4E4E3;" :
                                                   "background-color:transparent; border-radius:0px; font-size:16px; color:#E4E4E3;");

    lb_rosehome_name->setText(tr("Rose Home"));

    if(global.lang == 0){
        lb_rosehome_name->setFixedSize(50, 30);
        lb_rosehome_name->setGeometry(23, 57, 0, 0);
    }
    else{
        lb_rosehome_name->setFixedSize(50, 30);
        lb_rosehome_name->setGeometry(23, 57, 0, 0);
    }

    QPushButton *btn_lcd = new QPushButton(widget_display);
    btn_lcd->setObjectName("btn_lcd");
    btn_lcd->setProperty("type", HTTP_REMOTE_LCD);
    btn_lcd->setCursor(Qt::PointingHandCursor);
    btn_lcd->setFixedSize(96, 96);
    btn_lcd->setGeometry(20, 169, 0, 0);
    btn_lcd->setStyleSheet("#btn_lcd{background-color:#777777; border-radius:48px;} #btn_lcd:hover{background-color:#7D6144; border-radius:48px;}");

    QLabel *btn_lcd_img = GSCommon::getUILabelImg(":/images/remote_lcd.png", btn_lcd);
    btn_lcd_img->setFixedSize(52, 40);
    btn_lcd_img->setGeometry(22, 17, 0, 0);
    btn_lcd_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    QLabel *btn_lcd_name = new QLabel(btn_lcd);
    btn_lcd_name->setAlignment(Qt::AlignCenter);
    btn_lcd_name->setWordWrap(true);
    btn_lcd_name->setStyleSheet("background-color:transparent; border-radius:0px; font-size:14px; color:#E4E4E3;");
    btn_lcd_name->setText("LCD On/Off");
    btn_lcd_name->setFixedSize(50, 30);
    btn_lcd_name->setGeometry(23, 57, 0, 0);

    QPushButton *btn_clock = new QPushButton(widget_display);
    btn_clock->setObjectName("btn_clock");
    btn_clock->setProperty("type", HTTP_REMOTE_CLOCK);
    btn_clock->setCursor(Qt::PointingHandCursor);
    btn_clock->setFixedSize(96, 96);
    btn_clock->setGeometry(136, 169, 0, 0);
    btn_clock->setStyleSheet("#btn_clock{background-color:#777777; border-radius:48px;} #btn_clock:hover{background-color:#7D6144; border-radius:48px;}");

    QLabel *lb_clockc_img = GSCommon::getUILabelImg(":/images/remote_clock.png", btn_clock);
    lb_clockc_img->setFixedSize(52, 40);
    lb_clockc_img->setGeometry(22, 17, 0, 0);
    lb_clockc_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    QLabel *lb_clockc_name = new QLabel(btn_clock);
    lb_clockc_name->setFixedSize(64, 18);
    lb_clockc_name->setGeometry(16, 62, 0, 0);
    lb_clockc_name->setAlignment(Qt::AlignCenter);
    lb_clockc_name->setStyleSheet("background-color:transparent; border-radius:0px; font-size:16px; color:#E4E4E3;");
    lb_clockc_name->setText(tr("Clock"));


    // ============================================================
    // Music Play UI
    // ============================================================

    QWidget *widget_play_control = new QWidget(this->widget_remote_onlyRose);
    widget_play_control->setFixedSize(372, 136);
    widget_play_control->setGeometry(450, 355, 0, 0);
    widget_play_control->setStyleSheet("background-color:#4D4D4D; border-radius:10px;");

    QPushButton *btn_previous = new QPushButton(widget_play_control);
    btn_previous->setObjectName("btn_previous");
    btn_previous->setProperty("type", HTTP_REMOTE_PREV);
    btn_previous->setCursor(Qt::PointingHandCursor);
    btn_previous->setFixedSize(96, 96);
    btn_previous->setGeometry(20, 20, 0, 0);
    btn_previous->setStyleSheet("#btn_previous{background-color:#777777; border-radius:48px;} #btn_previous:hover{background-color:#7D6144; border-radius:48px;}");

    QLabel *lb_previous_img = GSCommon::getUILabelImg(":/images/remote_prev.png", btn_previous);
    lb_previous_img->setFixedSize(52, 40);
    lb_previous_img->setGeometry(22, 28, 0, 0);
    lb_previous_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    QPushButton *btn_play = new QPushButton(widget_play_control);
    btn_play->setObjectName("btn_play");
    btn_play->setProperty("type", HTTP_REMOTE_PLAYPAUSE);
    btn_play->setCursor(Qt::PointingHandCursor);
    btn_play->setFixedSize(96, 96);
    btn_play->setGeometry(138, 20, 0, 0);
    btn_play->setStyleSheet("#btn_play{background-color:#777777; border-radius:48px;} #btn_play:hover{background-color:#7D6144; border-radius:48px;}");

    QLabel *lb_play_img = GSCommon::getUILabelImg(":/images/remote_play.png", btn_play);
    lb_play_img->setFixedSize(52, 40);
    lb_play_img->setGeometry(22, 28, 0, 0);
    lb_play_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    QPushButton *btn_next = new QPushButton(widget_play_control);
    btn_next->setObjectName("btn_next");
    btn_next->setProperty("type", HTTP_REMOTE_NEXT);
    btn_next->setCursor(Qt::PointingHandCursor);
    btn_next->setFixedSize(96, 96);
    btn_next->setGeometry(256, 20, 0, 0);
    btn_next->setStyleSheet("#btn_next{background-color:#777777; border-radius:48px;} #btn_next:hover{background-color:#7D6144; border-radius:48px;}");

    QLabel *lb_following_img = GSCommon::getUILabelImg(":/images/remote_next.png", btn_next);
    lb_following_img->setFixedSize(52, 40);
    lb_following_img->setGeometry(22, 28, 0, 0);
    lb_following_img->setStyleSheet("background-color:transparent; border-radius:0px;");


    // ============================================================
    // and so on UI
    // ============================================================

    QPushButton *btn_extIO = new QPushButton(this->widget_remote_onlyRose);
    btn_extIO->setObjectName("btn_extIO");
    btn_extIO->setProperty("type", HTTP_REMOTE_EXTE);
    btn_extIO->setCursor(Qt::PointingHandCursor);
    btn_extIO->setFixedSize(114, 90);
    btn_extIO->setGeometry(450, 506, 0, 0);
    btn_extIO->setStyleSheet("#btn_extIO{background-color:#4D4D4D; border-radius:10px;} #btn_extIO:hover{background-color:#7D6144; border-radius:10px;}");

    QLabel *lb_extIO_img = GSCommon::getUILabelImg(":/images/remote_exte.png", btn_extIO);
    lb_extIO_img->setFixedSize(52, 40);
    lb_extIO_img->setGeometry(31, 13, 0, 0);
    lb_extIO_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    QLabel *lb_extIO_name = new QLabel(btn_extIO);
    lb_extIO_name->setFixedSize(100, 18);
    lb_extIO_name->setGeometry(7, 59, 0, 0);
    lb_extIO_name->setAlignment(Qt::AlignCenter);
    lb_extIO_name->setStyleSheet("background-color:transparent; border-radius:0px; font-size:16px; color:#E4E4E3;");
    lb_extIO_name->setText(tr("In/Out setting"));

    QPushButton *btn_vu = new QPushButton(this->widget_remote_onlyRose);
    btn_vu->setObjectName("btn_vu");
    btn_vu->setProperty("type", HTTP_REMOTE_VU);
    btn_vu->setCursor(Qt::PointingHandCursor);
    btn_vu->setFixedSize(114, 90);
    btn_vu->setGeometry(579, 506, 0, 0);
    btn_vu->setStyleSheet("#btn_vu{background-color:#4D4D4D; border-radius:10px;} #btn_vu:hover{background-color:#7D6144; border-radius:10px;}");

    QLabel *lb_vu_img = GSCommon::getUILabelImg(":/images/remote_uv.png", btn_vu);
    lb_vu_img->setFixedSize(52, 40);
    lb_vu_img->setGeometry(31, 13, 0, 0);
    lb_vu_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    QLabel *lb_vu_name = new QLabel(btn_vu);
    lb_vu_name->setFixedSize(100, 18);
    lb_vu_name->setGeometry(7, 59, 0, 0);
    lb_vu_name->setAlignment(Qt::AlignCenter);
    lb_vu_name->setStyleSheet("background-color:transparent; border-radius:0px; font-size:16px; color:#E4E4E3;");
    lb_vu_name->setText(tr("VU Mode"));

    QPushButton *btn_timer = new QPushButton(this->widget_remote_onlyRose);
    btn_timer->setObjectName("btn_timer");
    btn_timer->setProperty("type", HTTP_REMOTE_TIMER);
    btn_timer->setCursor(Qt::PointingHandCursor);
    btn_timer->setFixedSize(114, 90);
    btn_timer->setGeometry(708, 506, 0, 0);
    btn_timer->setStyleSheet("#btn_timer{background-color:#4D4D4D; border-radius:10px;} #btn_timer:hover{background-color:#7D6144; border-radius:10px;}");

    QLabel *lb_timer_img = GSCommon::getUILabelImg(":/images/remote_time.png", btn_timer);
    lb_timer_img->setFixedSize(52, 40);
    lb_timer_img->setGeometry(31, 13, 0, 0);
    lb_timer_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    this->lb_timer = new QLabel(btn_timer);
    this->lb_timer->setFixedSize(114, 18);
    this->lb_timer->setGeometry(0, 59, 0, 0);
    this->lb_timer->setAlignment(Qt::AlignCenter);
    this->lb_timer->setStyleSheet("background-color:transparent; border-radius:0px; font-size:16px; color:#E4E4E3;");
    this->lb_timer->setText(tr("Timer"));

    QPushButton *btn_hdmi = new QPushButton(this->widget_remote_onlyRose);
    btn_hdmi->setObjectName("btn_hdmi");
    btn_hdmi->setProperty("type", HTTP_REMOTE_HDMI);
    btn_hdmi->setCursor(Qt::PointingHandCursor);
    btn_hdmi->setFixedSize(142, 96);
    btn_hdmi->setGeometry(450, 611, 0, 0);
    btn_hdmi->setStyleSheet("#btn_hdmi{background-color:#4D4D4D; border-radius:10px;} #btn_hdmi:hover{background-color:#7D6144; border-radius:10px;}");

    QLabel *lb_hdmi_img = GSCommon::getUILabelImg(":/images/remote_hdmi.png", btn_hdmi);
    lb_hdmi_img->setFixedSize(76, 40);
    lb_hdmi_img->setGeometry(33, 17, 0, 0);
    lb_hdmi_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    this->lb_hdmi = new QLabel(btn_hdmi);
    this->lb_hdmi->setFixedSize(142, 18);
    this->lb_hdmi->setGeometry(0, 61, 0, 0);
    this->lb_hdmi->setAlignment(Qt::AlignCenter);
    this->lb_hdmi->setStyleSheet("background-color:transparent; border-radius:0px; font-size:16px; color:#E4E4E3;");
    this->lb_hdmi->setText(tr("HDMI"));

    QPushButton *btn_dlna = new QPushButton(this->widget_remote_onlyRose);
    btn_dlna->setObjectName("btn_dlna");
    btn_dlna->setProperty("type", HTTP_REMOTE_DLNA);
    btn_dlna->setCursor(Qt::PointingHandCursor);
    btn_dlna->setFixedSize(100, 96);
    btn_dlna->setGeometry(607, 611, 0, 0);
    btn_dlna->setStyleSheet("#btn_dlna{background-color:#4D4D4D; border-radius:10px;} #btn_dlna:hover{background-color:#7D6144; border-radius:10px;}");

    QLabel *lb_dlna_img = GSCommon::getUILabelImg(":/images/remote_dlna.png", btn_dlna);
    lb_dlna_img->setFixedSize(52, 40);
    lb_dlna_img->setGeometry(24, 17, 0, 0);
    lb_dlna_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    QLabel *lb_dlna_name = new QLabel(btn_dlna);
    lb_dlna_name->setFixedSize(100, 18);
    lb_dlna_name->setGeometry(0, 61, 0, 0);
    lb_dlna_name->setAlignment(Qt::AlignCenter);
    lb_dlna_name->setStyleSheet("background-color:transparent; border-radius:0px; font-size:16px; color:#E4E4E3;");
    lb_dlna_name->setText(tr("DLNA"));

    QPushButton *btn_airplay = new QPushButton(this->widget_remote_onlyRose);
    btn_airplay->setObjectName("btn_airplay");
    btn_airplay->setProperty("type", HTTP_REMOTE_AIRPLAY);
    btn_airplay->setCursor(Qt::PointingHandCursor);
    btn_airplay->setFixedSize(100, 96);
    btn_airplay->setGeometry(722, 611, 0, 0);
    btn_airplay->setStyleSheet("#btn_airplay{background-color:#4D4D4D; border-radius:10px;} #btn_airplay:hover{background-color:#7D6144; border-radius:10px;}");

    QLabel *lb_airplay_img = GSCommon::getUILabelImg(":/images/remote_airplay.png", btn_airplay);
    lb_airplay_img->setFixedSize(52, 40);
    lb_airplay_img->setGeometry(24, 17, 0, 0);
    lb_airplay_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    QLabel *lb_airplay_name = new QLabel(btn_airplay);
    lb_airplay_name->setFixedSize(100, 18);
    lb_airplay_name->setGeometry(0, 61, 0, 0);
    lb_airplay_name->setAlignment(Qt::AlignCenter);
    lb_airplay_name->setStyleSheet("background-color:transparent; border-radius:0px; font-size:16px; color:#E4E4E3;");
    lb_airplay_name->setText(tr("Airplay"));


    // ============================================================
    // Volume UI
    // ============================================================

    QWidget *widget_volume = new QWidget(this->widget_remote_onlyRose);
    widget_volume->setFixedSize(100, 556);
    widget_volume->setGeometry(833, 40, 0, 0);
    widget_volume->setStyleSheet("background-color:#4D4D4D; border-radius:10px;");

    QLabel *lb_vol = new QLabel(widget_volume);
    lb_vol->setFixedSize(100, 18);
    lb_vol->setGeometry(0, 35, 0, 0);
    lb_vol->setAlignment(Qt::AlignCenter);
    lb_vol->setStyleSheet("background-color:transparent; font-size:16px; color:#FFFFFF;");
    lb_vol->setText(tr("Volume"));

    this->lb_volume = new QLabel(widget_volume);
    this->lb_volume->setFixedSize(24, 22);
    this->lb_volume->setGeometry(38, 63, 0, 0);
    this->lb_volume->setAlignment(Qt::AlignCenter);
    this->lb_volume->setStyleSheet("background-color:transparent; font-size:20px; color:#FFFFFF;");
    this->lb_volume->setText("0");

    QPushButton *btn_vol_add = new QPushButton(widget_volume);
    btn_vol_add->setObjectName("btn_vol_add");
    btn_vol_add->setProperty("type", HTTP_REMOTE_VOLUME_ADD);
    btn_vol_add->setCursor(Qt::PointingHandCursor);
    btn_vol_add->setFixedSize(46, 46);
    btn_vol_add->setGeometry(27, 106, 0, 0);
    btn_vol_add->setStyleSheet("#btn_vol_add{background-color:#777777; border-radius:23px;} #btn_vol_add:hover{background-color:#7D6144; border-radius:23px;}");

    QLabel *lb_vol_add = new QLabel(btn_vol_add);
    lb_vol_add->setFixedSize(46, 46);
    lb_vol_add->setGeometry(0, 0, 0, 0);
    lb_vol_add->setStyleSheet("background-color:transparent");

    QImage image_add;
    image_add.load(":/images/vol_add_remote.png");

    QPixmap pixmapIMG_add = QPixmap(QSize(90, 90));
    pixmapIMG_add.fill(Qt::transparent);

    QPixmap tmp_pixmap_add;
    tmp_pixmap_add = tmp_pixmap_add.fromImage(image_add);
    tmp_pixmap_add = tmp_pixmap_add.scaled(90, 90, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    QPainter painter_add (&pixmapIMG_add);
    painter_add.setRenderHint(QPainter::Antialiasing, true);
    painter_add.setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter_add.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPainterPath path_add = QPainterPath();
    path_add.addRoundedRect(0, 0, 90, 90, 0, 0);

    painter_add.setClipPath(path_add);
    painter_add.drawPixmap(-22, 0, tmp_pixmap_add);
    painter_add.end();

    lb_vol_add->setPixmap(pixmapIMG_add);

    this->slider_volume = new QSlider(widget_volume);
    this->slider_volume->setObjectName("slider_volume");
    this->slider_volume->setPageStep(1);
    this->slider_volume->setMaximum(99);
    this->slider_volume->setFixedSize(16, 268);
    this->slider_volume->setGeometry(42, 178, 0, 0);
    this->slider_volume->setCursor(Qt::PointingHandCursor);
    this->slider_volume->setStyleSheet("QSlider::groove:vertical { background:#333333;width:4px; } QSlider::handle:vertical { background:white;height:16px;border-radius:8px;margin:0 -6px; } QSlider::add-page:vertical { background:#CCCCCC; } QSlider::sub-page:vertical { background:#333333; } ");

    QPushButton *btn_vol_sub = new QPushButton(widget_volume);
    btn_vol_sub->setObjectName("btn_vol_sub");
    btn_vol_sub->setProperty("type", HTTP_REMOTE_VOLUME_SUB);
    btn_vol_sub->setCursor(Qt::PointingHandCursor);
    btn_vol_sub->setFixedSize(46, 46);
    btn_vol_sub->setGeometry(27, 470, 0, 0);
    btn_vol_sub->setStyleSheet("#btn_vol_sub{background-color:#777777; border-radius:23px;} #btn_vol_sub:hover{background-color:#7D6144; border-radius:23px;}");

    QLabel *lb_vol_sub = new QLabel(btn_vol_sub);
    lb_vol_sub->setFixedSize(46, 46);
    lb_vol_sub->setGeometry(0, 0, 0, 0);
    lb_vol_sub->setStyleSheet("background-color:transparent");

    QImage image_sub;
    image_sub.load(":/images/vol_sub_remote.png");

    QPixmap pixmapIMG_sub = QPixmap(QSize(90, 90));
    pixmapIMG_sub.fill(Qt::transparent);

    QPixmap tmp_pixmap_sub;
    tmp_pixmap_sub = tmp_pixmap_sub.fromImage(image_sub);
    tmp_pixmap_sub = tmp_pixmap_sub.scaled(90, 90, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    QPainter painter_sub (&pixmapIMG_sub);
    painter_sub.setRenderHint(QPainter::Antialiasing, true);
    painter_sub.setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter_sub.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPainterPath path_sub = QPainterPath();
    path_sub.addRoundedRect(0, 0, 90, 90, 0, 0);

    painter_sub.setClipPath(path_sub);
    painter_sub.drawPixmap(-22, 0, tmp_pixmap_sub);
    painter_sub.end();

    lb_vol_sub->setPixmap(pixmapIMG_sub);

    this->btn_mute = new QPushButton(this->widget_remote_onlyRose);
    this->btn_mute->setObjectName("btn_mute");
    this->btn_mute->setProperty("type", HTTP_REMOTE_MUTE);
    this->btn_mute->setCursor(Qt::PointingHandCursor);
    this->btn_mute->setFixedSize(100, 96);
    this->btn_mute->setGeometry(833, 611, 0, 0);
    this->btn_mute->setStyleSheet("#btn_mute{background-color:#4D4D4D; border-radius:10px;} #btn_mute:hover{background-color:#7D6144; border-radius:10px;}");

    QLabel *lb_mute_img = GSCommon::getUILabelImg(":/images/remote_mute.png", this->btn_mute);
    lb_mute_img->setFixedSize(52, 40);
    lb_mute_img->setGeometry(24, 16, 0, 0);
    lb_mute_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    QLabel *lb_mute_name = new QLabel(this->btn_mute);
    lb_mute_name->setFixedSize(80, 18);
    lb_mute_name->setGeometry(10, 61, 0, 0);
    lb_mute_name->setAlignment(Qt::AlignCenter);
    lb_mute_name->setStyleSheet("background-color:transparent; border-radius:0px; font-size:16px; color:#E4E4E3;");
    lb_mute_name->setText(tr("Mute"));

    // ============================================================
    // AMP Set UI
    // ============================================================

    QPushButton *btn_amp = new QPushButton(this->widget_remote_onlyRose);
    btn_amp->setObjectName("btn_amp");
    btn_amp->setProperty("type", HTTP_REMOTE_AMP);
    btn_amp->setCursor(Qt::PointingHandCursor);
    btn_amp->setFixedSize(483, 50);
    btn_amp->setGeometry(450, 722, 0, 0);
    btn_amp->setStyleSheet("#btn_amp{background-color:#4D4D4D; border-radius:10px;} #btn_amp:hover{background-color:#7D6144; border-radius:10px;}");

    QLabel *lb_amp_img = GSCommon::getUILabelImg(":/images/remote_standby_set.png", btn_amp);
    lb_amp_img->setFixedSize(52, 40);
    lb_amp_img->setGeometry(21, 5, 0, 0);
    lb_amp_img->setStyleSheet("background-color:transparent; border-radius:0px;");

    QLabel *lb_amp_name = new QLabel(btn_amp);
    lb_amp_name->setFixedSize(370, 20);
    lb_amp_name->setGeometry(93, 15, 0, 0);
    lb_amp_name->setAlignment(Qt::AlignLeft);
    lb_amp_name->setStyleSheet("background-color:transparent; border-radius:0px; font-size:16px; color:#E4E4E3;");
    lb_amp_name->setText(tr("Amp Connection Management"));


    // ============================================================
    // 커넥트
    // ============================================================

    connect(btn_standby, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));
    connect(btn_reboot, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));
    connect(btn_power, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));

    connect(btn_back, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));
    connect(btn_home, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));
    connect(btn_lcd, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));
    connect(btn_clock, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));

    connect(btn_previous, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));
    connect(btn_play, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));
    connect(btn_next, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));

    connect(btn_extIO, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));
    connect(btn_vu, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));
    connect(btn_timer, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));
    connect(btn_hdmi, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));
    connect(btn_dlna, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));
    connect(btn_airplay, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));

    connect(btn_vol_add, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));
    connect(btn_vol_sub, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));
    connect(this->slider_volume, SIGNAL(valueChanged(int)) ,this, SLOT(slot_volume_set(int)));
    connect(this->btn_mute, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));

    connect(btn_amp, SIGNAL(clicked()), this, SLOT(slot_clicked_remoteItem()));
}


/**
 * @brief RemoteWidget::clickedRemoteItem [SLOT]
 */
void RemoteWidget::slot_clicked_remoteItem(){

    int type = sender()->property("type").toInt();

    int value = 0;
    QString url = "";

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject json;

    if(this->flag_button_enable == false){
        this->flag_button_enable = true;

        switch(type) {
            case HTTP_REMOTE_STANDBY :
                json = QJsonObject();
                json.insert("barControl", "remote_bar_order_sleep_on_off");
                json.insert("value", "-1");
                json.insert("roseToken", global.device.getDeviceRoseToken());

                url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/remote_bar_order").arg(global.port);

                network->request(HTTP_REMOTE_STANDBY, url, json, true);
                break;

            case HTTP_REMOTE_REBOOT :
                this->dialog_comfirm(network, HTTP_REMOTE_REBOOT);
                break;

            case HTTP_REMOTE_POWER :
                this->dialog_comfirm(network, HTTP_REMOTE_POWER);
                break;

            case HTTP_REMOTE_BACK :
                json = QJsonObject();
                json.insert("barControl", "remote_bar_order.back");
                json.insert("value", "-1");
                json.insert("roseToken", global.device.getDeviceRoseToken());

                url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/remote_bar_order").arg(global.port);

                network->request(HTTP_REMOTE_HOME, url, json, true);
                break;

            case HTTP_REMOTE_HOME :
                json = QJsonObject();
                json.insert("barControl", "remote_bar_order.home");
                json.insert("value", "-1");
                json.insert("roseToken", global.device.getDeviceRoseToken());

                url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/remote_bar_order").arg(global.port);

                network->request(HTTP_REMOTE_HOME, url, json, true);
                break;

            case HTTP_REMOTE_LCD :
                json = QJsonObject();
                json.insert("barControl", "remote_bar_order.screen_onoff");
                json.insert("value", "-1");
                json.insert("roseToken", global.device.getDeviceRoseToken());

                url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/remote_bar_order").arg(global.port);

                network->request(HTTP_REMOTE_HOME, url, json, true);
                break;

            case HTTP_REMOTE_CLOCK :
                json = QJsonObject();
                json.insert("barControl", "remote_bar_order.clock");
                json.insert("value", "-1");
                json.insert("roseToken", global.device.getDeviceRoseToken());

                url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/remote_bar_order").arg(global.port);

                network->request(HTTP_REMOTE_HOME, url, json, true);
                break;

            case HTTP_REMOTE_PREV :
                ToastMsg::show(this, "", tr("choose previous song"));

                json = QJsonObject();
                json.insert("currentPlayState", "19");
                json.insert("roseToken", global.device.getDeviceRoseToken());

                url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/current_play_state").arg(global.port);

                network->request(HTTP_REMOTE_PREV, url, json, true);
                break;

            case HTTP_REMOTE_PLAYPAUSE :
                ToastMsg::show(this, "", tr("choose Play/Pause"));

                json = QJsonObject();
                json.insert("currentPlayState", "17");
                json.insert("roseToken", global.device.getDeviceRoseToken());

                url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/current_play_state").arg(global.port);

                network->request(HTTP_REMOTE_PLAYPAUSE, url, json, true);
                break;

            case HTTP_REMOTE_NEXT :
                ToastMsg::show(this, "", tr("choose Next song"));

                json = QJsonObject();
                json.insert("currentPlayState", "18");
                json.insert("roseToken", global.device.getDeviceRoseToken());

                url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/current_play_state").arg(global.port);

                network->request(HTTP_REMOTE_NEXT, url, json, true);
                break;

            case HTTP_REMOTE_EXTE : // 외부입력
                this->hide();
                emit clickedRemoteEXTE();
                break;

            case HTTP_REMOTE_VU :
                if(global.device.CurrPlayType == "VIDEO" || global.device.CurrPlayType == "YOUTUBE" || global.device.CurrPlayType == "TIDAL_VIDEO" || global.device.CurrPlayType == "BUGS_MV"){
                    ToastMsg::show(this, "", tr("Not supported in current mode."));
                }
                else{
                    this->hide();
                    emit clickedRemoteVU();
                }
                break;

            case HTTP_REMOTE_TIMER :
                value = 0;

                if(this->timer_set > 0){
                    value = this->timer_set / 10;
                }

                json = QJsonObject();
                json.insert("barControl","remote_bar_order.timer");
                json.insert("value", value);
                json.insert("roseToken",global.device.getDeviceRoseToken());

                url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/remote_bar_order").arg(global.port);

                network->request(HTTP_REMOTE_TIMER, url, json, true);
                break;

            case HTTP_REMOTE_HDMI :
                this->hide();
                emit clickedRemoteEXTH();
                break;

            case HTTP_REMOTE_DLNA :
                this->dialog_comfirm(network, HTTP_REMOTE_DLNA);
                break;

            case HTTP_REMOTE_AIRPLAY :
                this->dialog_comfirm(network, HTTP_REMOTE_AIRPLAY);
                break;

            case HTTP_REMOTE_VOLUME_ADD :
                value = this->slider_volume->value();
                if(value < 100){
                    value += 1;

                    if(this->flag_mute == true){
                        this->flag_mute = false;
                        this->btn_mute->setStyleSheet("#btn_mute{background-color:#4D4D4D; border-radius:10px;} #btn_mute:hover{background-color:#7D6144; border-radius:10px;}");
                    }

                    this->slider_volume->setValue(value);
                }
                break;

            case HTTP_REMOTE_VOLUME_SUB :
                value = this->slider_volume->value();
                if(value > 0){
                    value -= 1;

                    if(this->flag_mute == true){
                        this->flag_mute = false;
                        this->btn_mute->setStyleSheet("#btn_mute{background-color:#4D4D4D; border-radius:10px;} #btn_mute:hover{background-color:#7D6144; border-radius:10px;}");
                    }

                    this->slider_volume->setValue(value);
                }
                break;

            case HTTP_REMOTE_MUTE :
                json = QJsonObject();
                json.insert("barControl", "remote_bar_order.mute");
                json.insert("value", "-1");
                json.insert("roseToken", global.device.getDeviceRoseToken());

                url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/remote_bar_order").arg(global.port);

                network->request(HTTP_REMOTE_MUTE, url, json, true);

                if(this->flag_mute == true){
                    this->flag_mute = false;
                    this->btn_mute->setStyleSheet("#btn_mute{background-color:#4D4D4D; border-radius:10px;} #btn_mute:hover{background-color:#7D6144; border-radius:10px;}");
                }
                else if(this->flag_mute == false){
                    this->flag_mute = true;
                    this->btn_mute->setStyleSheet("#btn_mute{background-color:#B18658; border-radius:10px;} #btn_mute:hover{background-color:#7D6144; border-radius:10px;}");

                }
                break;

            case HTTP_REMOTE_AMP :
                this->dialog_comfirm(network, HTTP_REMOTE_AMP);
                break;
        }
    }
}


void RemoteWidget::dialog_comfirm(NetworkHttp *network, int type){

    DialogConfirm *dlgConfirmMSG = new DialogConfirm(this);
    dlgConfirmMSG->setTitle(tr("Notice"));
    dlgConfirmMSG->setFixedSize(350, 400);
    dlgConfirmMSG->setAlignment(Qt::AlignCenter);
    dlgConfirmMSG->setTextHeight(150);

    switch(type) {
        case HTTP_REMOTE_REBOOT :
            dlgConfirmMSG->setText(tr("Do you want to restart?"));
            break;

        case HTTP_REMOTE_POWER :
            dlgConfirmMSG->setText(tr("Do you want to turn it off?"));
            break;

        case HTTP_REMOTE_DLNA :
            dlgConfirmMSG->setText(tr("Do you want to restart the DLNA service?"));
            break;

        case HTTP_REMOTE_AIRPLAY :
            dlgConfirmMSG->setText(tr("Do you want to restart the AirPlay service?"));
            break;

        case HTTP_REMOTE_AMP :
            dlgConfirmMSG->setText(tr("Service is being prepared."));
            break;
    }

    int result = dlgConfirmMSG->exec();

    if(result == QDialog::Accepted){

        QJsonObject json;

        switch(type) {
            case HTTP_REMOTE_REBOOT :
                json = QJsonObject();
                json.insert("barControl", "remote_bar_order.reboot");
                json.insert("value", "-1");
                json.insert("roseToken", global.device.getDeviceRoseToken());
                break;

            case HTTP_REMOTE_POWER :
                json = QJsonObject();
                json.insert("barControl", "remote_bar_order.power_onoff");
                json.insert("value", "-1");
                json.insert("roseToken", global.device.getDeviceRoseToken());
                break;

            case HTTP_REMOTE_DLNA :
                json = QJsonObject();
                //json.insert("barControl", "remote_bar_order.reboot");
                json.insert("barControl", "remote_bar_order_dlna_restart");
                json.insert("value", "-1");
                json.insert("roseToken", global.device.getDeviceRoseToken());
                break;

            case HTTP_REMOTE_AIRPLAY :
                json = QJsonObject();
                json.insert("barControl", "remote_bar_order_airplay_restart");
                json.insert("value", "-1");
                json.insert("roseToken", global.device.getDeviceRoseToken());
                break;

            case HTTP_REMOTE_AMP :
                break;
        }

        if(type != HTTP_REMOTE_AMP){
            QString url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/remote_bar_order").arg(global.port);
            network->request(type, url, json, true);
        }
    }

    delete dlgConfirmMSG;

    if(this->flag_button_enable == true){
        this->flag_button_enable = false;
    }
}


/**
 * @brief RemoteWidget::responseHttp : 로즈 장치 정보요청 결과 처리
 * @param p_jsonObject QJsonObject&
 */
void RemoteWidget::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    switch(p_id){
        case HTTP_REMOTE_CONTROL:
            if(p_jsonObject.contains("displayInfo")){
                this->lb_hdmi->setText(ProcJsonEasy::getString(p_jsonObject, "displayInfo"));
            }

            if(p_jsonObject.contains("volumeValue")){
                int volume = ProcJsonEasy::getInt(p_jsonObject, "volumeValue");
                this->lb_volume->setText(QString("%1").number(volume));
                this->slider_volume->setValue(volume);
            }

            if(p_jsonObject.contains("airplayInfo")){
                this->flag_airplay = ProcJsonEasy::getBool(p_jsonObject, "airplayInfo");
            }

            if(p_jsonObject.contains("dlnaInfo")){
                this->flag_dlna = ProcJsonEasy::getBool(p_jsonObject, "dlnaInfo");
            }

            if(p_jsonObject.contains("spotifyInfo")){
                this->flag_spotify = ProcJsonEasy::getBool(p_jsonObject, "spotifyInfo");
            }
            break;

        case HTTP_REMOTE_POWER :
            emit linker->signal_devicePowerChanged();
            break;
        case HTTP_REMOTE_STANDBY :
            break;
        case HTTP_REMOTE_REBOOT :
            break;

        case HTTP_REMOTE_BACK :
            break;
        case HTTP_REMOTE_HOME :
            break;
        case HTTP_REMOTE_LCD :
            break;
        case HTTP_REMOTE_CLOCK :
            break;

        case HTTP_REMOTE_PREV :
            break;
        case HTTP_REMOTE_PLAYPAUSE :
            break;
        case HTTP_REMOTE_NEXT :
            break;

        case HTTP_REMOTE_EXTE :
            break;
        case HTTP_REMOTE_VU :
            break;
        case HTTP_REMOTE_TIMER :
            if(p_jsonObject.contains("timer")){
                this->timer_set = ProcJsonEasy::getInt(p_jsonObject, "timer");

                if(this->timer_set > 0){
                    this->lb_timer->setText(tr("Timer") + QString(":%1").arg(this->timer_set) + tr("Min"));
                }
                else{
                    this->lb_timer->setText(tr("Timer"));
                }
            }
            else{
                QJsonObject json;
                QString url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/sleep.time.get").arg(global.port);

                NetworkHttp *network_timer = new NetworkHttp(this);
                connect(network_timer, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network_timer->request(HTTP_REMOTE_TIMER, url, json, true);
            }
            break;

        case HTTP_REMOTE_HDMI :
            break;
        case HTTP_REMOTE_DLNA :
            break;
        case HTTP_REMOTE_AIRPLAY :
            break;

        case HTTP_REMOTE_VOLUME_ADD :
            break;
        case HTTP_REMOTE_VOLUME_SUB :
            break;
        case HTTP_REMOTE_MUTE :
            if(p_jsonObject.contains("mute")){
                int mute = ProcJsonEasy::getInt(p_jsonObject, "mute");
                this->flag_mute = (mute == 1) ? true : false;
                this->slot_volume_mute();
            }
            break;
    }

    if(this->flag_button_enable == true){
        this->flag_button_enable = false;
    }

    sender()->deleteLater();
}


/**
 * @brief RemoteWidget::slot_volume_set [SLOT]
 * @param p_value
 */
void RemoteWidget::slot_volume_set(int p_value){

    this->lb_volume->setText(QString::number(p_value));

    if(this->flag_mute == false){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        // API 볼륨 조절
        QJsonObject json;
        json.insert("volumeType", "volume_set");
        json.insert("volumeValue", p_value);

        QString url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/volume").arg(global.port);
        network->request(HTTP_REMOTE_VOLUME_ADD, url, json, true);
    }
}


/**
 * @brief RemoteWidget::slot_volume_mute [SLOT]
 */
void RemoteWidget::slot_volume_mute(){

    if(this->flag_mute == true){
        this->btn_mute->setStyleSheet("#btn_mute{background-color:#B18658; border-radius:10px;} #btn_mute:hover{background-color:#7D6144; border-radius:10px;}");
    }
    else{
        this->btn_mute->setStyleSheet("#btn_mute{background-color:#4D4D4D; border-radius:10px;} #btn_mute:hover{background-color:#7D6144; border-radius:10px;}");
    }

    QJsonObject json;
    QString url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/get_control_info").arg(global.port);
    NetworkHttp *network_control = new NetworkHttp(this);
    connect(network_control, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network_control->request(HTTP_REMOTE_CONTROL, url, json, true);
}


/**
 * @brief RemoteWidget::slot_connectedDevice : [슬롯] 기기 연결
 * @details 리모콘에 현재 연결된 기기 정보 세팅
 */
void RemoteWidget::slot_connectedDevice(){

    QPixmap *pixmap_icon;
    if(global.device.getDeviceType()=="RS150"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rs150_on.png");
    }
    else if(global.device.getDeviceType()=="RS150B"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rs150_on.png");
    }
    else if(global.device.getDeviceType()=="RS250"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rs250_on.png");
    }
    else if(global.device.getDeviceType()=="RS250A"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rs250_on.png");
    }
    else if(global.device.getDeviceType()=="RS201"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rs201_on.png");
    }
    else if(global.device.getDeviceType()=="RS301"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rs301_on.png");
    }
    else if(global.device.getDeviceType()=="RS350"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rs350_on.png");
    }
    else if(global.device.getDeviceType()=="RS520"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rs520_on.png");
    }
    else{
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rs350_on.png");
    }

    this->lb_curr_img->setPixmap(*pixmap_icon);                                   //버퍼에 있는 이미지를 QLabel에 출력
    this->lb_curr_img->resize(pixmap_icon->width(), pixmap_icon->height());

    int left = 0;
    QLabel *lb_tmp = new QLabel();

    lb_tmp->setStyleSheet("background-color:transparent; font-size:20px; color:#FFFFFF;");
    lb_tmp->setText(global.device.getDeviceName());

    left = (398 - lb_tmp->sizeHint().width()) / 2;
    this->lb_curr_name->setText(global.device.getDeviceName());
    this->lb_curr_name->setStyleSheet("background-color:transparent; font-size:20px; color:#FFFFFF;");
    this->lb_curr_name->setGeometry(left, 146, lb_tmp->sizeHint().width(), 22);

    lb_tmp->setStyleSheet("background-color:transparent; font-size:16px; color:#FFFFFF;");
    lb_tmp->setText(global.device.getDeviceType() + " (Rose OS " + global.device.getDeviceVersion() + ")");

    left = (398 - lb_tmp->sizeHint().width()) / 2;
    this->lb_curr_id->setText(global.device.getDeviceType() + " (Rose OS " + global.device.getDeviceVersion() + ")");
    this->lb_curr_id->setStyleSheet("background-color:transparent; font-size:16px; color:#FFFFFF;");
    this->lb_curr_id->setGeometry(left, 173, lb_tmp->sizeHint().width(), 18);

    lb_tmp->setStyleSheet("background-color:transparent; font-size:16px; color:#CCCCCC;");
    lb_tmp->setText(global.device.getDeviceIP());

    left = (398 - lb_tmp->sizeHint().width()) / 2;
    this->lb_curr_ip->setText(global.device.getDeviceIP());
    this->lb_curr_ip->setStyleSheet("background-color:transparent; font-size:16px; color:#CCCCCC;");
    this->lb_curr_ip->setGeometry(left, 201, lb_tmp->sizeHint().width(), 18);
}


/**
 * @brief RemoteWidget::showEvent [override] 볼륨정보 가져와서 세팅한다.
 * @param event
 */
void RemoteWidget::showEvent(QShowEvent *event){

    Q_UNUSED(event);

    if(global.device.getDeviceIP().isEmpty() == false){

        QJsonObject json;
        QString url = "";

        url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/mute.state.get").arg(global.port);

        NetworkHttp *network_mute = new NetworkHttp(this);
        connect(network_mute, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network_mute->request(HTTP_REMOTE_MUTE, url, json, true);

        url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1/sleep.time.get").arg(global.port);

        NetworkHttp *network_timer = new NetworkHttp(this);
        connect(network_timer, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network_timer->request(HTTP_REMOTE_TIMER, url, json, true);

        /*NetworkHttp *network_control = new NetworkHttp(this);
        connect(network_control, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network_control->request(HTTP_REMOTE_CONTROL, QString("http://%1:%2/get_control_info").arg(global.device.getDeviceIP()).arg(global.port), json, true);*/
    }
}


bool RemoteWidget::event(QEvent *event){

    if(event->type() == QEvent::Show){
        if(!this->widget_search->getFlagHasDeviceList()){
            this->widget_search->searchDevice("");
        }
    }

    return QWidget::event(event);
}


/**
 * @brief RemoteWidget::mousePressEvent [override] 삭제하면 안됨.. 최상위 부모의 mousePressEvent 버블링 막기 위해 오버라이딩만 하였음. 최상위 부모가 큐목록을 hide 하기 위함.
 * @param event
 */
void RemoteWidget::mousePressEvent(QMouseEvent *event){

    Q_UNUSED(event);
    // Nothing.. 버블링 막기 위해서.. override 하고 부모 호출 안함..
}
