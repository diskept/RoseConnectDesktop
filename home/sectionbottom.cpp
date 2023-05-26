#include "home/sectionbottom.h"

#include "home/abstractmaincontent.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/filedownloader.h"
#include "common/rosesettings.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"
#include "common/commonGetRedirectUrl.h"    //j220903 twitter

#include "widget/clickablelabel.h"
#include "widget/toastmsg.h"

#include <QLabel>
#include <QWidget>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QJsonObject>
#include <QJsonArray>
#include <QPushButton>
#include <QTimer>
#include <QPainter>
#include <QMovie>
#include <QSettings>
#include <QScreen>
#include <QGuiApplication>
#include <QFileInfo>
#include <QPainterPath>

// for Tidal
#include "tidal/TidalSettings.h"
#include "tidal/ProcTidal_forOuter.h"
#include "tidal/ProcCommon.h"

// for Bugs
#include "bugs/ProcBugs_forOuter.h"

// for Qobuz
#include "qobuz/ProcQobuz_forOuter.h"
#include "qobuz/ProcCommon_forQobuz.h"//c230422

#include "ReplyTimeout.h"
#include "common/httpServer.h"//c230315_1


const int IMG_W_MAX = 79;
const int PROGRESS_H = 3;   // 5
const int BG_H = 95;

const int HTTP_CURRENT_PLAY_STATE = 77;
const int HTTP_DEVICE_GET_CURRENT_STATE_PLAY = 88;

const int HTTP_REMOTE_PLAYPAUSE = 17;
const int HTTP_REMOTE_PLAYNEXT = 18;
const int HTTP_REMOTE_PLAYBACK = 19;
const int HTTP_REMOTE_REP = 20;
const int HTTP_REMOTE_SHU = 21;
const int HTTP_REMOTE_MUTE = 22;
const int HTTP_REMOTE_VOLUME = 23;
const int HTTP_NETWORK_PLAY = 24;

int term_position = 6000;

// MusicMain에 있는 상수과 동일한 값
//const QString PAGECODE_ETC_PLAYFULLSCREEN = "playfullscreen";

namespace playIconPath  {

QString play_rep_icon_off = ":/images/SectionBottom/play_rep_icon_off.png";             ///< [rep] disabled
QString play_rep1_icon = ":/images/SectionBottom/play_rep1_icon.png";                   ///< [rep] all track rep (onetimes)
QString play_rep2_icon = ":/images/SectionBottom/play_rep2_icon.png";                   ///< [rep] all track rep (forever)
QString play_rep3_icon = ":/images/SectionBottom/play_rep3_icon.png";                   ///< [rep] only one track rep
QString play_shu_icon_off = ":/images/SectionBottom/play_shu_icon_off.png";             ///< [shuf] disabled
QString play_shu1_icon = ":/images/SectionBottom/play_shu1_icon.png";                   ///< [shuf] shuffle on
QString play_shu2_icon = ":/images/SectionBottom/play_shu2_icon.png";                   ///< [shuf] shuffle off
QString play_list_icon = ":/images/SectionBottom/play_list_icon.png";                   ///< [shuf] shuffle on
QString play_list_icon_on = ":/images/SectionBottom/play_list_icon_on.png";             ///< [shuf] shuffle off
QString play_sound_s_icon_off = ":/images/SectionBottom/play_sound_s_icon.png";         ///< [sound] off
QString play_sound_s_icon_on = ":/images/SectionBottom/play_sound_s_icon_on.png";       ///< [sound] on
QString play_mute_icon = ":/images/SectionBottom/play_mute_icon.png";                   ///< [mute] off
QString play_mute_icon_on = ":/images/SectionBottom/play_mute_icon_on.png";             ///< [mute] on
}


namespace playTypes{

QString Music = "MUSIC";
QString VIDEO = "VIDEO";
QString RADIO = "RADIO";
QString ROSE_RADIO = "ROSE_RADIO";
QString FM_TUNER = "FM_TUNER";
QString YOUTUBE = "YOUTUBE";
QString POD = "POD";
QString TIDAL = "TIDAL";
QString TIDAL_VIDEO = "TIDAL_VIDEO";
QString BUGS = "BUGS";
QString BUGS_MV = "BUGS_MV";
QString QOBUZ = "QOBUZ";                            ///< Added diskept 31/12/2020
QString APPLE = "APPLE_MUSIC";
QString SPOTIFY = "SPOTIFY";
QString ROON = "ROON";
QString DLNA = "DLNA";
QString AIRPLAY = "AIRPLAY";
QString BT = "BT";
QString CD = "CD";                                  ///< CD재생시
}


SectionBottom::SectionBottom(QWidget *parent) : QWidget(parent)
{
    global.pr = parent;//c230415
    this->linker = Linker::getInstance();

    m_updater_tmp = new Updater();

    //connect(this->linker, SIGNAL(signal_window_hide()), this, SLOT(slot_checkWindow_hide()));//c220401
    //connect(this->linker, SIGNAL(signal_playMusic_desktop(QJsonArray, bool)), this, SLOT(slot_playMusic_desktop(QJsonArray, bool)));//c220704
    //connect(this->linker, SIGNAL(signal_sliderclick(int)), this, SLOT(slot_sliderClick(int)));//c221007_1

    connect(this->linker, SIGNAL(signal_checkQueue(int,QString)), this, SLOT(slot_checkQueueMsg(int, QString)));//c1209
    connect(this->linker, SIGNAL(signal_deviceStart()), this, SLOT(slot_deviceModeStart()));//c220609
    connect(this->linker, SIGNAL(signal_localPlayMusicInfo(QJsonObject, QPixmap)), this, SLOT(slot_responseHttp_localDriverPlayer(QJsonObject, QPixmap)));//c220609
    //connect(this->linker, SIGNAL(siganl_desktopModeStart()), this, SLOT(slot_desktopModeStart()));//c220704

    connect(this->linker, SIGNAL(signal_volume_change(int&)), this, SLOT(slot_volume_change(int&)));
    connect(this->linker, SIGNAL(signal_volume_muteState(int&)), this, SLOT(slot_muteState_change(int&)));
    connect(this->linker, SIGNAL(signal_volume_widget_hide()), this, SLOT(slot_volume_widget_hide()));


    this->dialog_wait = new QDialog(this) ;

    this->ContentLoadingwaitingAni_init();//c230303_2
    this->ContentLoadingwaitingMsg_init();//c230303_2

    this->dlgWakeUp = new DialogWakeUp(this);
    this->dlgWakeUp->setAlertMode();
    this->dlgWakeUp->setTitle(tr("Power Standby"));
    this->dlgWakeUp->setText(tr("ROSE is standby mode.\nTurn on the device."));
    this->dlgWakeUp->setbText(tr("Connected Device"));

    int left = global.left_mainwindow + ((global.width_mainwindow - 600) / 2);
    int top = global.top_mainwindow + ((global.height_mainwindow - 600) / 2);

    this->dlgWakeUp->setGeometry(left, top, 0, 0);

    if(!global.device.getDeviceName().isEmpty()){
        this->dlgWakeUp->setDevice(global.device.getDeviceName());
    }
    else{
        this->dlgWakeUp->setDevice(tr("NONE"));
    }

    // 파일다운로더
    this->filedownloader = new FileDownloader(this);
    connect(this->filedownloader, SIGNAL (downloaded()), SLOT (slot_loadImage()));

    this->pixmap_albumImg = new QPixmap();

    this->widget_total = new QWidget();
    this->widget_total->setObjectName("sectionBottom");
    //this->widget_total->setStyleSheet("#sectionBottom { background-color:white; }");    // 큐 재생목록 위젯을 raise 하는 것때문에 배경색 있어야한다. 지우면 안됨.
    this->widget_total->setStyleSheet("#sectionBottom { background-color: transparent; }");    // 큐 재생목록 위젯을 raise 하는 것때문에 배경색 있어야한다. 지우면 안됨.
    this->widget_total->setFixedHeight(95);

    // ===============================================
    // 레이아웃에 넣지 않고 show... 위치 지정
    // ===============================================
    // 베경 > 앨범 이미지
    QGraphicsBlurEffect *effect = new QGraphicsBlurEffect();
    effect->setBlurRadius(40);      // old value 50

    this->lb_albumbg = new QLabel(this->widget_total);
    this->lb_albumbg->setGraphicsEffect(effect);
    this->lb_albumbg->show();

    // 배경 > opacity
    this->lb_bgOpacity = new ClickableLabel(this->widget_total);
    this->lb_bgOpacity->setStyleSheet("background-color: rgba(0,0,0,180);");
    this->lb_bgOpacity->show();
    // ===============================================

    // 앨범 info
    this->lb_albumThumb = new ClickableLabel(this->widget_total);
    this->lb_albumThumb->setStyleSheet("background-color:transparent;");
    this->lb_albumThumb->setGeometry(8, 8, IMG_W_MAX, IMG_W_MAX);

    this->lb_albumThumbText = new QLabel(this->lb_albumThumb);
    this->lb_albumThumbText->setContentsMargins(5,0,5,0);
    this->lb_albumThumbText->setWordWrap(true);
    this->lb_albumThumbText->setAlignment(Qt::AlignCenter);
    this->lb_albumThumbText->setFixedSize(IMG_W_MAX, IMG_W_MAX);
    this->lb_albumThumbText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->lb_albumThumbText->setStyleSheet("font:bold; font-size:12px; color:#FFFFFF; background-color:transparent;");
    this->lb_albumThumbText->hide();

    this->lb_icon_playType = new ClickableLabel(this->widget_total);
    this->lb_icon_playType->setStyleSheet("background-color: transparent;");
    this->lb_icon_playType->setGeometry(107, 15, 30, 30);

    this->lb_title = new ClickableLabel(this->widget_total);
    this->lb_title->setStyleSheet("background-color: transparent; font-size: 16px; font-weight: normal; line-height: 1.44; text-align: left; color: #FFFFFF; ");
    this->lb_title->setGeometry(143, 17, 235, 24);

    this->lb_artistName = new ClickableLabel(this->widget_total);
    this->lb_artistName->setStyleSheet("background-color: transparent; font-size: 12px; font-weight: 300; line-height: 1.42; text-align: left; color: #CCCCCC; ");
    this->lb_artistName->setGeometry(143, 42, 235, 18);

    this->lb_trackinfo = new ClickableLabel(this->widget_total);
    this->lb_trackinfo->setStyleSheet("background-color: transparent; font-size: 12px; font-weight: 300; line-height: 1.42; text-align: left; color: #CCCCCC; ");
    this->lb_trackinfo->setGeometry(143, 60, 235, 18);


    this->btn_sound = new QPushButton(this->widget_total);
    this->btn_sound->setProperty("type", "sound");
    this->btn_sound->setStyleSheet("background-color: transparent;");
    this->btn_sound->setCursor(Qt::PointingHandCursor);
    this->btn_sound->setGeometry(global.width_mainwindow - 70, 0, 70, 95);

    QImage img_sound;
    QPixmap *pixmap_icon_sound = new QPixmap();
    if(img_sound.load(playIconPath::play_sound_s_icon_off)){
        *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    this->lb_sound_img = new QLabel(this->btn_sound);
    this->lb_sound_img->setStyleSheet("background-color: transparent;");
    this->lb_sound_img->setPixmap(*pixmap_icon_sound);
    this->lb_sound_img->setGeometry(0, 0, 70, 95);

    this->lb_sound_text = new QLabel(this->btn_sound);
    this->lb_sound_text->setAlignment(Qt::AlignCenter);
    this->lb_sound_text->setStyleSheet("background-color: transparent; font-size: 14px; font-weight: normal; line-height: 1.21; text-align: center; color: #FFFFFF; ");
    this->lb_sound_text->setGeometry(27, 62, 16, 20);
    this->lb_sound_text->setText("");

    this->btn_list = new QPushButton(this->widget_total);
    this->btn_list->setProperty("type", "list");
    this->btn_list->setStyleSheet("background-color: transparent;");
    this->btn_list->setCursor(Qt::PointingHandCursor);
    this->btn_list->setGeometry(this->btn_sound->geometry().left() - 70, 0, 70, 95);

    QImage img_list;
    QPixmap *pixmap_icon_list = new QPixmap();
    if(img_list.load(playIconPath::play_list_icon)){
        *pixmap_icon_list = QPixmap::fromImage(img_list);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_icon_list = pixmap_icon_list->scaled(img_list.width(), img_list.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    this->lb_list_img = new QLabel(this->btn_list);
    this->lb_list_img->setStyleSheet("background-color: transparent;");
    this->lb_list_img->setPixmap(*pixmap_icon_list);
    this->lb_list_img->setGeometry(0, 0, 70, 95);

    this->btn_shu = new QPushButton(this->widget_total);
    this->btn_shu->setProperty("type", "shu");
    this->btn_shu->setStyleSheet("background-color: transparent;");
    this->btn_shu->setCursor(Qt::PointingHandCursor);
    this->btn_shu->setGeometry(this->btn_list->geometry().left() - 70, 0, 70, 95);

    QImage img_shu;
    QPixmap *pixmap_icon_shu = new QPixmap();
    if(img_shu.load(playIconPath::play_shu_icon_off)){
        *pixmap_icon_shu = QPixmap::fromImage(img_shu);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_icon_shu = pixmap_icon_shu->scaled(img_shu.width(), img_shu.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    this->lb_shu_img = new QLabel(this->btn_shu);
    this->lb_shu_img->setStyleSheet("background-color: transparent;");
    this->lb_shu_img->setPixmap(*pixmap_icon_shu);
    this->lb_shu_img->setGeometry(7, 24, 56, 48);

    this->btn_next = new QPushButton(this->widget_total);
    this->btn_next->setProperty("type", "next");
    this->btn_next->setStyleSheet("background-color: transparent;");
    this->btn_next->setCursor(Qt::PointingHandCursor);
    this->btn_next->setGeometry(this->btn_shu->geometry().left() - 70, 0, 70, 95);

    QImage img_next;
    QPixmap *pixmap_icon_next = new QPixmap();
    if(img_next.load(":/images/SectionBottom/play_next_icon.png")){
        *pixmap_icon_next = QPixmap::fromImage(img_next);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_icon_next = pixmap_icon_next->scaled(img_next.width(), img_next.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    this->lb_next_img = new QLabel(this->btn_next);
    this->lb_next_img->setStyleSheet("background-color: transparent;");
    this->lb_next_img->setPixmap(*pixmap_icon_next);
    this->lb_next_img->setGeometry(7, 24, 56, 48);

    this->btn_pause = new QPushButton(this->widget_total);
    this->btn_pause->setProperty("type", "pause");
    this->btn_pause->setStyleSheet("background-color: transparent;");
    this->btn_pause->setCursor(Qt::PointingHandCursor);
    this->btn_pause->setGeometry(this->btn_next->geometry().left() - 70, 0, 70, 95);

    QImage img_pause;
    QPixmap *pixmap_icon_pause = new QPixmap();
    if(img_pause.load(":/images/SectionBottom/play_pause_icon.png")){
        *pixmap_icon_pause = QPixmap::fromImage(img_pause);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_icon_pause = pixmap_icon_pause->scaled(img_pause.width(), img_pause.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    this->lb_pause_img = new QLabel(this->btn_pause);
    this->lb_pause_img->setStyleSheet("background-color: transparent;");
    this->lb_pause_img->setPixmap(*pixmap_icon_pause);
    this->lb_pause_img->setGeometry(7, 24, 56, 48);

    this->btn_back = new QPushButton(this->widget_total);
    this->btn_back->setProperty("type", "back");
    this->btn_back->setStyleSheet("background-color: transparent;");
    this->btn_back->setCursor(Qt::PointingHandCursor);
    this->btn_back->setGeometry(this->btn_pause->geometry().left() - 70, 0, 70, 95);

    QImage img_back;
    QPixmap *pixmap_icon_back = new QPixmap();
    if(img_back.load(":/images/SectionBottom/play_back_icon.png")){
        *pixmap_icon_back = QPixmap::fromImage(img_back);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_icon_back = pixmap_icon_back->scaled(img_back.width(), img_back.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    this->lb_back_img = new QLabel(this->btn_back);
    this->lb_back_img->setStyleSheet("background-color: transparent;");
    this->lb_back_img->setPixmap(*pixmap_icon_back);
    this->lb_back_img->setGeometry(7, 24, 56, 48);

    this->btn_rep = new QPushButton(this->widget_total);
    this->btn_rep->setProperty("type", "rep");
    this->btn_rep->setStyleSheet("background-color: transparent;");
    this->btn_rep->setCursor(Qt::PointingHandCursor);
    this->btn_rep->setGeometry(this->btn_back->geometry().left() - 70, 0, 70, 95);

    QImage img_rep;
    QPixmap *pixmap_icon_rep = new QPixmap();
    if(img_rep.load(playIconPath::play_rep_icon_off)){
        *pixmap_icon_rep = QPixmap::fromImage(img_rep);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_icon_rep = pixmap_icon_rep->scaled(img_rep.width(), img_rep.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    this->lb_rep_img = new QLabel(this->btn_rep);
    this->lb_rep_img->setStyleSheet("background-color: transparent;");
    this->lb_rep_img->setPixmap(*pixmap_icon_rep);
    this->lb_rep_img->setGeometry(7, 24, 56, 48);


    this->slider_playbar = new sectionbottomSlider(Qt::Horizontal, this->widget_total);
    this->slider_playbar->setCursor(Qt::PointingHandCursor);
    this->slider_playbar->setStyleSheet("QSlider::groove:horizontal { background: transparent; height: 5px; margin: 2px;}"
                                        "QSlider::handle:horizontal { background: #FFFFFF; width: 3px; height: 14px; margin: -4px 6px -4px 6px;}"
                                        "QSlider::add-page:horizontal { background: #212121; }"
                                        "QSlider::sub-page:horizontal { background: #B18658; }");
    this->slider_playbar->setGeometry(455, 63, 342, 14);
    this->slider_playbar->hide();

    this->lb_playbar_cur = new QLabel(this->widget_total);
    this->lb_playbar_cur->setStyleSheet("background-color: transparent; font-size: 14px; font-weight: normal; line-height: 1.64;  text-align: left; color: #FFFFFF; ");
    this->lb_playbar_cur->setText("0:00");
    left = this->slider_playbar->geometry().left() - this->lb_playbar_cur->sizeHint().width() - 20;
    this->lb_playbar_cur->setGeometry(left, 60, this->lb_playbar_cur->sizeHint().width(), 20);
    this->lb_playbar_cur->hide();

    this->lb_playbar_duration = new QLabel(this->widget_total);
    this->lb_playbar_duration->setStyleSheet("background-color: transparent; font-size: 14px; font-weight: normal; line-height: 1.64;  text-align: left; color: #FFFFFF; ");
    this->lb_playbar_duration->setText("0:00");
    left = this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width() + 20;
    this->lb_playbar_duration->setGeometry(left, 60, this->lb_playbar_duration->sizeHint().width(), 20);
    this->lb_playbar_duration->hide();


    /*QGridLayout *glayout_playTitle = new QGridLayout();
    glayout_playTitle->setContentsMargins(0,0,0,0);
    glayout_playTitle->setSpacing(5);
    glayout_playTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    glayout_playTitle->addWidget(this->lb_icon_playType, 0, 0, Qt::AlignLeft);
    glayout_playTitle->addWidget(this->lb_title, 0, 1, Qt::AlignLeft);
    glayout_playTitle->addWidget(this->lb_artistName, 1, 1, Qt::AlignLeft);

    this->btn_rep = getUIBtn(playIconPath::play_rep_icon_off, "rep");
    this->btn_back = getUIBtn(":/images/play_back_icon.png", "back");
    this->btn_pause = getUIBtn(":/images/play_pause_icon.png", "pause");
    this->btn_next = getUIBtn(":/images/play_next_icon.png", "next");
    this->btn_shu = getUIBtn(playIconPath::play_shu_icon_off, "shu");
    this->btn_list = getUIBtn(":/images/play_list_icon.png", "list");
    this->btn_sound = getUIBtn(playIconPath::play_sound_s_icon_off, "sound");

    QHBoxLayout *hlayout_icon = new QHBoxLayout();
    hlayout_icon->setContentsMargins(0,0,0,0);
    hlayout_icon->setSpacing(0);
    hlayout_icon->addWidget(this->btn_rep);
    hlayout_icon->addWidget(this->btn_back);
    hlayout_icon->addWidget(this->btn_pause);
    hlayout_icon->addWidget(this->btn_next);
    hlayout_icon->addWidget(this->btn_shu);
    hlayout_icon->addWidget(this->btn_list);
    hlayout_icon->addWidget(this->btn_sound);

    this->lb_albumThumb = new ClickableLabel();
    this->lb_albumThumb->setFixedSize(IMG_W_MAX, IMG_W_MAX);

    this->lb_albumThumbText = new QLabel(this->lb_albumThumb);
    this->lb_albumThumbText->setContentsMargins(5,0,5,0);
    this->lb_albumThumbText->setWordWrap(true);
    this->lb_albumThumbText->setAlignment(Qt::AlignCenter);
    this->lb_albumThumbText->setFixedSize(IMG_W_MAX, IMG_W_MAX);
    this->lb_albumThumbText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->lb_albumThumbText->setStyleSheet("font:bold;font-size:12px;color:#FFFFFF;background-color:transparent;");
    this->lb_albumThumbText->hide();



    //this->slider_playbar = new QSlider(Qt::Horizontal);
    this->slider_playbar = new sectionbottomSlider(Qt::Horizontal);
    //this->slider_playbar->setMinimum(-1);
    this->slider_playbar->setSingleStep(1000);     // 1초 = 1000    // arrow-key step-size
    this->slider_playbar->setPageStep(1000);                // mouse-wheel/page-key step-size//margin:0 -6px;
    //this->slider_playbar->setFixedHeight(PROGRESS_H);

    //this->slider_playbar->setStyleSheet("QSlider::groove:horizontal { background:red; height: 4px} QSlider::handle:horizontal { width:5px;height:10px;background:white;border-radius:0px;margin:-6px -6px; } QSlider::add-page:horizontal { background:#aaaaaa; } QSlider::sub-page:horizontal { background:#B18658; } ");
    //this->slider_playbar->setStyleSheet("QSlider::groove:horizontal { background:red; } QSlider::handle:horizontal { width:5px;height:5px;background:white;border-radius:0px;margin:0px 0px; } QSlider::add-page:horizontal { background:#3D3D3E; } QSlider::sub-page:horizontal { background:#B18658; } ");
    //this->slider_playbar->setStyleSheet("QSlider::groove:horizontal { background-color: black; border: 0px solid #424242; height: 3px; border-radius: 4px; } QSlider::handle:horizontal { background-color: white;border: 2px solid white; width: 6px; height: 5px; line-height: 20px; margin-top: -5px; margin-bottom: -5px; border-radius: 10px; } QSlider::add-page:horizontal { background:#3D3D3E; } QSlider::sub-page:horizontal { background:#B18658; } ");
    this->slider_playbar->setStyleSheet("QSlider::groove:horizontal { background-color: black; border: 0px solid #424242; height: 3px; border-radius: 4px; } QSlider::handle:horizontal { background-color: white;border: 1px solid white; width: 1px; height: 1px; line-height: 5px; margin-top: -1px; margin-bottom: -1px; border-radius: 10px; } QSlider::add-page:horizontal { background:#3D3D3E; } QSlider::sub-page:horizontal { background:#B18658; } ");



    this->lb_playbar_cur = new QLabel(tr(""));
    this->lb_playbar_cur->setStyleSheet("color:#E5E5E4");
    this->lb_playbar_cur->setFixedWidth(45);//c220526
    this->lb_playbar_cur->setFixedHeight(15);//c220526
    this->lb_playbar_cur->setText(tr("00:00"));
    this->lb_playbar_cur->setStyleSheet("Text-align:right;font-size:16px;color:#E5E5E4; ");

    this->lb_playbar_duration = new QLabel(tr(""));
    this->lb_playbar_duration->setStyleSheet("color:#E5E5E4");
    this->lb_playbar_duration->setFixedWidth(60);//c220526
    this->lb_playbar_duration->setFixedHeight(15);//c220526
    this->lb_playbar_duration->setText(tr("00:00"));
    this->lb_playbar_duration->setStyleSheet("Text-align:left;font-size:16px;color:#E5E5E4; ");


    QHBoxLayout *hlayout_bar = new QHBoxLayout();
    hlayout_bar->setContentsMargins(0,0,0,0);//c220603
    hlayout_bar->setSpacing(5);
    hlayout_bar->addWidget(this->lb_playbar_cur);//, 0,Qt::AlignLeft);
    hlayout_bar->addWidget(this->slider_playbar);//, 0,Qt::AlignCenter);
    hlayout_bar->addSpacing(5);//c220603
    hlayout_bar->addWidget(this->lb_playbar_duration);//, 0,Qt::AlignRight);

    wg_playbar_cur = new QWidget();
    wg_playbar_cur->setFixedHeight(20);
    //wg_playbar_cur->setFixedWidth(200);
    wg_playbar_cur->setLayout(hlayout_bar);
    wg_playbar_cur->setStyleSheet("min-width:150px;max-width:950px;font-size:20px;color:#FFFFFF;");

    QWidget *line_bottom_1 = new QWidget();
    line_bottom_1->setFixedHeight(5);
    line_bottom_1->setStyleSheet("background-color:#333333;");

    QWidget *line_bottom_2 = new QWidget();
    //line_bottom_2->setLayout(glayout_playTitle);

    QHBoxLayout *hlayout = new QHBoxLayout(this->widget_total);
    hlayout->setContentsMargins(0,0,0,0);
    hlayout->setSpacing(0);
    hlayout->addSpacing(8);
    //hlayout->addWidget(this->lb_albumThumb);
    //hlayout->addSpacing(12);
    //hlayout->addLayout(glayout_playTitle,2);
    //hlayout->addWidget(line_bottom_2);
    //hlayout->addSpacing(30);
    //hlayout->addWidget(wg_playbar_cur);
    //hlayout->addLayout(hlayout_icon);*/

    QVBoxLayout *total = new QVBoxLayout();//c220526
    total->setContentsMargins(0,0,0,0);
    total->setSpacing(0);
    //total->addWidget(line_bottom_1);
    //total->addLayout(hlayout_bar);           // play progress bar
    //total->addWidget(wg_playbar_cur);
    total->addWidget(this->widget_total);

    this->setLayout(total);


    connect(this->lb_icon_playType, SIGNAL(signal_clicked()), this, SLOT(slot_showPlayFullScreen()));
    connect(this->lb_albumThumb, SIGNAL(signal_clicked()), this, SLOT(slot_showPlayFullScreen()));
    connect(this->lb_title, SIGNAL(signal_clicked()), this, SLOT(slot_showPlayFullScreen()));
    connect(this->lb_artistName, SIGNAL(signal_clicked()), this, SLOT(slot_showPlayFullScreen()));

    connect(this->lb_bgOpacity, SIGNAL(signal_clicked()), this, SLOT(slot_showPlayFullScreen()));

    connect(this->btn_back, SIGNAL(clicked()), this, SLOT(clickedBtn()));
    connect(this->btn_pause, SIGNAL(clicked()), this, SLOT(clickedBtn()));
    connect(this->btn_next, SIGNAL(clicked()), this, SLOT(clickedBtn()));
    connect(this->btn_list, SIGNAL(clicked()), this, SLOT(clickedBtn()));
    connect(this->btn_sound, SIGNAL(clicked()), this, SLOT(clickedBtn()));

    connect(this->slider_playbar, SIGNAL(signal_sliderclick(int)), this, SLOT(slot_sliderClick(int)));//c221007_1
    //connect(this->slider_playbar, SIGNAL(sliderReleased()) ,this, SLOT(changedSliderBar()));//c221013_3

    // default 데이터 세팅
    this->setDataSectionBottom(QJsonObject());


    QJsonObject json;
    QString Url = "";

    NetworkHttp *network_mute = new NetworkHttp(this);
    connect(network_mute, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    Url = QString("http://%1:%2/mute.state.get").arg(global.device.getDeviceIP(), global.port);
    network_mute->request(HTTP_REMOTE_MUTE
                          , Url
                          , json
                          , true);

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    json.insert("volumeType", "volume_get");
    Url = QString("http://%1:%2/volume").arg(global.device.getDeviceIP(), global.port);
    network->request(HTTP_REMOTE_VOLUME
                     , Url
                     , json
                     , true);


    this->timer = new QTimer(this);
    this->timer->setInterval(1000); // 1초 1000 -> 500
    connect(this->timer, SIGNAL(timeout()), SLOT(slot_deviceModetimePlayValue()));
    this->timer->start();


    Dt_thread = new DesktopMediaPlayerThread("", this);
    connect(Dt_thread, &DesktopMediaPlayerThread::finished, Dt_thread, &QObject::deleteLater);
    connect(Dt_thread, SIGNAL(signal_stopThread()), this, SLOT(slot_player_valuestop()));

#if 0
    //DESKTOP_MODE  //c220705
    Dt_thread->start();
#endif

    this->repeatMode = 0;//c220828_2
    this->shuffleMode = 0;//c220828_2

    m_updater_tmp->checkForUpdates_tmp();
    //slot_Update_call();//c230315_1

    //QTimer::singleShot(8000, this, SLOT(slot_Update_call()));//c230315_1
}


SectionBottom::~SectionBottom(){//c220705

    //Dt_thread->quit();//c220609
    //Dt_thread->deleteLater();//c220609
    //delete m_updater_tmp;
    this->deleteLater();
}


void SectionBottom::slot_Update_call(){//c221013_1

    //m_updater_tmp = new Updater();
    m_updater_tmp->checkForUpdates_tmp();
}


void SectionBottom::slot_player_valuestop(){//c220705

    if(QMediaPlayer::PlayingState == global.music_player->state()){
        //global.music_player->pause();
        if(global.repeatMode == 0){
            nextSongPlay();
        }
    }
    //Dt_thread->stop();-------
}


void SectionBottom::nextSongPlay(){//c220705

    print_debug();
}


void SectionBottom::slot_desktopModeStart(){//c220704

    if(global.desktopPlayerMode){
        this->timer->stop();
    }

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    if(this->device_playingState && global.desktopPlayerMode){
        print_debug();
        global.isPlaying = false;
        QJsonObject tmp_json = QJsonObject();
        tmp_json.insert("currentPlayState","17");
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
        network->request(HTTP_REMOTE_PLAYPAUSE, QString("http://%1:%2/current_play_state")
                         .arg(global.device.getDeviceIP(),global.port), tmp_json, true);
    }
}


void SectionBottom::slot_deviceModeStart(){//c220609

    global.desktopPlayerMode =  false;
    this->timer->start();
    global.music_player->pause();
}


/*void SectionBottom::check_device_stateInfo(){
    if(!global.desktopPlayerMode && !global.device.getDeviceIP().isEmpty()){
        print_debug();

        NetworkHttp *network_playBar = new NetworkHttp;
        connect(network_playBar, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_state(int, QJsonObject)));
        QJsonObject tmp_json;
        tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
        tmp_json.insert("currentPlayState", this->playType);
        network_playBar->request(HTTP_DEVICE_GET_CURRENT_STATE_PLAY, QString("http://%1:%2/get_current_state")
                                 .arg(global.device.getDeviceIP(),global.port), tmp_json, true);

    }
}*/


/**
 * @brief SectionBottom::slot_timePlayValue
 */
void SectionBottom::slot_deviceModetimePlayValue(){//c220609

    if(this->isActiveWindow()) {//c221001_1
        //print_debug();//c220909_1
        global.window_activate_flag = true;
    }else{
       // print_debug();//c220909_1
        global.window_activate_flag = false;
    }

    //&& !global.user.getAccess_token().isEmpty()
    // 로즈기기의 현재 재생 컨트롤러 정보 가져오기
    //print_debug();
    //qDebug() << "global.device.getDeviceIP()=" << global.device.getDeviceIP();
    if(global.Queue_track_count == 0 && global.abs_ani_dialog_wait->isHidden() == true && !global.device.getDeviceIP().isEmpty() ){
        //print_debug();

        NetworkHttp *network_playBar = new NetworkHttp;
        connect(network_playBar, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        QJsonObject tmp_json;
        tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
        tmp_json.insert("currentPlayState", this->playType);
        network_playBar->request(HTTP_DEVICE_GET_CURRENT_STATE_PLAY, QString("http://%1:%2/get_current_state")
                                 .arg(global.device.getDeviceIP(),global.port), tmp_json, true);

    }

    if(global.Queue_track_count == 1 ){//c230522
        qDebug() << "SectionBottom::slot_deviceModetimePlayValue() 664 global.Queue_track_count == 1";
        long tmp_currTimeSec_error10 = QDateTime::currentSecsSinceEpoch();
        if(latestCheckTimeSecond_connect10sec == 0){//
            latestCheckTimeSecond_connect10sec = QDateTime::currentSecsSinceEpoch();
        }

        if( (tmp_currTimeSec_error10 > (latestCheckTimeSecond_connect10sec + 10) )){

            //  print_debug();
            global.Queue_track_count = 0;

            latestCheckTimeSecond_connect10sec = QDateTime::currentSecsSinceEpoch();

        }//
    }

    if(global.enable_message_flag == true){

        if(global.enable_message_count < 10){
            global.enable_message_count++;
        }
        else{
            emit this->linker->signal_message_show_timeout();
        }
    }
}


void SectionBottom::slot_checkWindow_hide(){

    print_debug();
}


void SectionBottom::slot_checkWindow_show(){

    print_debug();
}


/**
 * @brief SectionBottom::changedSliderBar [SLOT] 재생바 변경
 * @param p_value
 */
void SectionBottom::slot_sliderClick(int p_value){

    //Q_UNUSED(p_value);
    // nothing
    print_debug();
    this->thumbnail_realHttp = "";//c220727
    slot_sliderClick_flag = true;
    qDebug() << "p_value = " << p_value;
    qDebug() << "this->isIsoOpen = " << this->isIsoOpen;
    qDebug() << "this->curPosition = " << this->curPosition;
    qDebug() << "this->duration = " << this->duration;
    qDebug() << "global.device.CurrPlayType = " << global.device.CurrPlayType;
    qDebug() << "global.desktopPlayerMode = " << global.desktopPlayerMode;
    //this->timer->stop();

    if(this->curPosition < 0 || this->curPosition > this->duration){
        print_debug();
    }//this->playType!=playTypes::CD &&global.device.CurrExtType

    //if(!global.desktopPlayerMode && (global.device.CurrPlayType != "iso" && this->playType!=playTypes::ROSE_RADIO && this->playType!=playTypes::RADIO)){//c221014_1
    if(!global.desktopPlayerMode && (this->playType!=playTypes::CD && this->playType!=playTypes::ROSE_RADIO && this->playType!=playTypes::RADIO && this->playType!=playTypes::FM_TUNER)){//c221014_1

        QJsonObject json;
        json.insert("currentPlaySeekto",p_value);
        json.insert("currentPlayState",22);                             // 항상 22 (라디오 제외 모든 메인메뉴 음원 재생시 모두 22 동일함-어플에서-)
        json.insert("roseToken",global.device.getDeviceRoseToken());

        NetworkHttp *network = new NetworkHttp(this);
        network->request(HTTP_DEVICE_GET_CURRENT_STATE_PLAY
                         , QString("http://%1:%2/current_play_state").arg(global.device.getDeviceIP(), global.port)
                         , json
                         , true);

        //}else if(this->playType==playTypes::CD && global.currMainMenuCode == QString(GSCommon::MainMenuCode::CDplay)){
    }
    else if(this->playType==playTypes::CD && this->isIsoOpen == false){

        QJsonObject json;
        json.insert("currentPlaySeekto",p_value);
        json.insert("currentPlayState",22);                             // 항상 22 (라디오 제외 모든 메인메뉴 음원 재생시 모두 22 동일함-어플에서-)
        json.insert("roseToken",global.device.getDeviceRoseToken());

        NetworkHttp *network = new NetworkHttp(this);
        network->request(HTTP_DEVICE_GET_CURRENT_STATE_PLAY
                         , QString("http://%1:%2/current_play_state").arg(global.device.getDeviceIP(), global.port)
                         , json
                         , true);
    }
    else{
        //print_debug();
        global.window_activate_flag = true;
        ToastMsg::show(this, "", tr("You cannot move your play position in this playtype."), 2000);//

        qDebug() << "global.desktopPlayerMode=" << global.desktopPlayerMode;

        global.window_activate_flag = false;
        if(global.desktopPlayerMode){
            global.music_player->setPosition(p_value);
        }
    }
}


void SectionBottom::ContentLoadingwaitingAni_init(){ ////c230303_2

    //----------------------------------------------------

    print_debug();
    // QLabel *lb_msg = new QLabel();
    // lb_msg->setText(msg);
    // lb_msg->setAlignment(Qt::AlignCenter);

    QLabel *Movie = new QLabel(this);
    Movie->setStyleSheet("background-color:transparent;");
    //QLabel *Movie = new QLabel(this);
    //Movie->setStyleSheet("background-color:#333333;");
    //QMovie *mov = new QMovie("C:/Users/doulz/Rose_Connect-21-12-21-20/images/Spinner-4.2s-200px.gif");
    //mov = new QMovie(":/images/loading.gif");//":/images/Spinner-4.2s-200px.gif"
    QMovie *mov = new QMovie(":/images/Spinner-4.2s-200px.gif");//":/images/Spinner-4.2s-200px.gif"//c220616
    mov->setScaledSize(QSize(120,120));

    //Movie->setGeometry(20, 20, 392, 225);
    mov->setScaledSize(QSize(120,120));
    //mov->setBackgroundColor("transparent");
    mov->setBackgroundColor("#333333");

    Movie->setMovie(mov);

    QHBoxLayout *hl_msgBox = new QHBoxLayout();
    hl_msgBox->setContentsMargins(0,0,0,0);
    hl_msgBox->setSpacing(0);
    hl_msgBox->setContentsMargins(35,20,35,15);
    //hl_msgBox->addStretch(1);
    //hl_msgBox->addWidget(lb_msg);
    hl_msgBox->addWidget(Movie);
    hl_msgBox->setAlignment(Qt::AlignCenter);
    //hl_msgBox->addStretch(1);


    if(this->dialog_wait->isHidden() != true){
        this->dialog_wait->hide();
        return;
    }

    if(this->dialog_wait->layout()==0){//c220616
        this->dialog_wait->setModal(true);
        this->dialog_wait->setWindowFlags( Qt::Tool | Qt::FramelessWindowHint );
        this->dialog_wait->setAttribute(Qt::WA_TranslucentBackground);
        this->dialog_wait->setStyleSheet("background-color:#333333;");



        this->dialog_wait->setFixedSize(240,240);
        this->dialog_wait->setLayout(hl_msgBox);

    }

    mov->start();

    this->dialog_wait->hide();
}


void SectionBottom::ContentLoadingwaitingAniShow(QString msg, int flag){ //c230303_2

    //----------------------------------------------------
    //  global.dialog_wait->hide(); //cheontidal
    //  global.dialog_wait->deleteLater();
    if(!global.window_activate_flag) return;//c220804
    if(this->showHideCheckFlag == true) return;
    this->showHideCheckFlag = true;

    if(this->dialog_wait->isHidden() != true){
        this->dialog_wait->hide();
        return;
    }

    int left = global.left_mainwindow + global.width_mainwindow - 200 - (dialog_wait->sizeHint().width());//c221115
    if(flag == 2){//c221115
        left = global.left_mainwindow + global.width_mainwindow / 2;//
    }
    else{

    }

    int top = global.top_mainwindow + global.height_mainwindow/2 - (dialog_wait->sizeHint().height());//c220804
    dialog_wait->move(left, top);//c220804

    this->dialog_wait->show();
    this->dialog_wait->raise();

    if(flag == 2){//c221115
        QTimer::singleShot(2000, this->dialog_wait, SLOT(hide()));
    }
}


void SectionBottom::ContentLoadingwaitingMsg_init(){ //c230303_2
    print_debug();
    qDebug() << "global.powerDialogShowFlag=" << global.powerDialogShowFlag;
    qDebug() << "global.window_activate_flag=" << global.window_activate_flag;
    qDebug() << "tmp_window_activate_flag=" << tmp_window_activate_flag;

    print_debug();
    lb_msg_show = new QLabel();
    lb_msg_show->setText("");
    lb_msg_show->setAlignment(Qt::AlignCenter);

    QHBoxLayout *hl_msgBox = new QHBoxLayout();
    hl_msgBox->setContentsMargins(0,0,0,0);
    hl_msgBox->setSpacing(0);
    hl_msgBox->setContentsMargins(35,20,35,15);
    hl_msgBox->addStretch(1);
    hl_msgBox->addWidget(lb_msg_show);
    hl_msgBox->addStretch(1);
    QWidget *widget_msgBox = new QWidget();
    widget_msgBox->setStyleSheet("background-color:#B18658;color:#FFFFFF;font-size:20px;border-radius:5px;");
    widget_msgBox->setLayout(hl_msgBox);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget_msgBox, 0, Qt::AlignCenter);
    //vl_total->addWidget(queProgressBar, 0, Qt::AlignCenter);

    msg_dialog_wait = new QDialog(this) ;
    //msg_dialog_wait->setModal(false);
    msg_dialog_wait->setWindowFlags(msg_dialog_wait->windowFlags()| Qt::FramelessWindowHint );
    //msg_dialog_wait->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint );

    msg_dialog_wait->setAttribute(Qt::WA_TranslucentBackground);

    msg_dialog_wait->setStyleSheet("background-color:#55FF0000;border-radius:20px;");
    int msgboxWiddth = 1;

    msg_dialog_wait->setFixedSize(msgboxWiddth, 200);//630
    msg_dialog_wait->setLayout(vl_total);
    msg_dialog_wait->hide();
    print_debug();

}

void SectionBottom::ContentLoadingwaitingMsgShow(QString msg, int flag){ //c230303_2
    print_debug();
    qDebug() << "-----------------------------------------------------------------------------------------------------" << msg;
    qDebug() << "global.powerDialogShowFlag=" << global.powerDialogShowFlag;
    qDebug() << "global.window_activate_flag=" << global.window_activate_flag;
    qDebug() << "tmp_window_activate_flag=" << tmp_window_activate_flag;
    //if(global.pretoastmsg == msg) return;//c230422
    global.pretoastmsg = msg;
    if(global.powerDialogShowFlag) return;//c220724

    if(!global.window_activate_flag){
        return;//c221013_3
    }

    print_debug();
    tmp_window_activate_flag = false;

    lb_msg_show->setText(msg);

    int msgboxWiddth = 0;
    if(global.lang == 0){//eng
        msgboxWiddth = msg.size()*20;
    }else{//kor
        msgboxWiddth = msg.size()*40;
    }
    msg_dialog_wait->setFixedSize(msgboxWiddth, 200);//630


    int left = global.left_mainwindow+global.width_mainwindow/2- (msg_dialog_wait->sizeHint().width() / 2);//c220804
    //int top = global.top_mainwindow+global.height_mainwindow + (msg_dialog_wait->sizeHint().height() / 2);//c220804
    int top = global.top_mainwindow+global.height_mainwindow - (msg_dialog_wait->sizeHint().height()*2);

    msg_dialog_wait->move(left, top);//c220804

    if(flag == 0){
        //msg_dialog_wait->raise();
        //dialog_wait->activateWindow();
        //dialog_wait->move((this->pos().x()+60), (this->pos().y()+200));
        // msg_dialog_wait->show();
    }
    if(flag == 1){  //queue msg
        msg_dialog_wait->show();
        //msg_dialog_wait->raise();
        //msg_dialog_wait->setGeometry(this->width()-msgboxWiddth,300,1000,200);//c220804
        //dialog_wait->activateWindow();
        // dialog_wait->move((global.queulistLeftPosion+20), (this->pos().y()+200));
        QTimer::singleShot(1000, msg_dialog_wait, SLOT(hide()));

    }
    else if(flag == 2){  //not queue msg
        msg_dialog_wait->show();
        //msg_dialog_wait->raise();
        //dialog_wait->activateWindow();
        //msg_dialog_wait->setGeometry(this->width()-msgboxWiddth,200,1000,200);//c220804

        // dialog_wait->move((global.queulistLeftPosion+20), (this->pos().y()+200));
        QTimer::singleShot(2000, msg_dialog_wait, SLOT(hide()));//c220802
    }
    else if(flag == 3){  //not queue msg
        msg_dialog_wait->show();
        //msg_dialog_wait->raise();
        //dialog_wait->activateWindow();
        //msg_dialog_wait->setGeometry(this->width()-msgboxWiddth, 300, 1000, 200);//c220804

        // dialog_wait->move((global.queulistLeftPosion+20), (this->pos().y()+200));
        QTimer::singleShot(3000, msg_dialog_wait, SLOT(hide()));
    }
    else if(flag == 4){  //c221115
        int top = global.top_mainwindow+global.height_mainwindow/2;
        msg_dialog_wait->move(left, top);
        msg_dialog_wait->show();
        //msg_dialog_wait->raise();
        //dialog_wait->activateWindow();
        //msg_dialog_wait->setGeometry(this->width()-msgboxWiddth, 300, 1000, 200);//c220804

        // dialog_wait->move((global.queulistLeftPosion+20), (this->pos().y()+200));
        QTimer::singleShot(3000, msg_dialog_wait, SLOT(hide()));
    }
    QCoreApplication::processEvents();//c230302_2
    print_debug();
}


void SectionBottom::slot_redirectUrl(const QString url){  //j220903 twitter

    if(url != this->cd_img_change){
        this->cd_img_change = url;
        this->filedownloader->setImageURL(QUrl(this->cd_img_change));
    }
}


void SectionBottom::slot_checkQueueMsg(int id, QString str){        //c211213

    //print_debug();
    //qDebug()<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    if(id == 0){
        print_debug();
    }else if(id == 1){
        print_debug();
        if(this->showHideCheckFlag){
            if(this->dialog_wait->isHidden() != true){
                this->dialog_wait->hide();
                ToastMsg::delay(this,"", tr("delay"), 500);//c230311
            }
            this->showHideCheckFlag = false;
        }
        else{
            ContentLoadingwaitingMsgShow( tr("Failed to play the selected track. Queue check required."),1);
        }
    }
    else if(id == 2){
        print_debug();
        //dialog_wait->hide();
        ContentLoadingwaitingMsgShow( tr("It is added to the playlist from the recommended list and played."),1);
    }
    else if(id == 3){
        print_debug();
        //dialog_wait->hide();
        ContentLoadingwaitingMsgShow( tr("You have already selected a queue playlist."),1);
    }
    else if(id == 4){
        print_debug();
        //dialog_wait->hide();
        ContentLoadingwaitingMsgShow( tr("There are no modifications in the queuelist."),1);
    }
    else if(id == 5){
        print_debug();
        ContentLoadingwaitingMsgShow(tr("This menu has already been selected."),1);
    }
    else if(id == 6){
        print_debug();
        qDebug() << "this->showHideCheckFlag = " << this->showHideCheckFlag;
        if(this->showHideCheckFlag){
            print_debug();
            if(this->dialog_wait->isHidden() != true){
                print_debug();
                this->dialog_wait->hide();
                ToastMsg::delay(this,"", tr("delay"), 500);//c230311
            }
            this->showHideCheckFlag = false;

        }
        else{

        }
    }
    else if(id == 61){
        print_debug();
        qDebug() << "global.queue_position_startFlag = " << global.queue_position_startFlag;
        if(this->showHideCheckFlag){
            print_debug();
            if(this->dialog_wait->isHidden() != true){
                this->dialog_wait->hide();
                ToastMsg::delay(this,"", tr("delay"), 500);//c230311
            }
            print_debug();
            this->showHideCheckFlag = false;
        }else{

        }
        //section_bottom_mouse_trigger_menu_flag = true;
    }
    else if(id == 7){
        print_debug();
        ContentLoadingwaitingMsgShow(tr("Please wait. Downloading Queuelist.\n Only track songs are clickable"),2);
    }
    else if(id == 17){
        print_debug();

        //qDebug()<< "7777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777";
        //if(dialog_wait->isHidden()){
        ContentLoadingwaitingAniShow(tr("Please wait. Downloading Queuelist."),1);
        //}
    }
    else if(id == 8){
        print_debug();
        //dialog_wait->hide();
        ContentLoadingwaitingMsgShow(tr("A song has been added to the recommendation list."),1);
    }
    else if(id == 9){
        print_debug();
        if(global.queueTimerFlag == true){
            ContentLoadingwaitingMsgShow(str, 1);
        }
    }
    else if(id == 10){
        print_debug();
        //section_bottom_mouse_trigger_menu_flag = true;
        ContentLoadingwaitingMsgShow(tr("Download of queuelist thumbnails is complete."),1);
        //emit this->linker->signal_checkQueue(21, "");
    }
    else if(id == 11){
        print_debug();
        //dialog_wait->hide();
        ContentLoadingwaitingMsgShow( tr("Queuelist update was successful."),1);
    }
    else if(id == 12){
        print_debug();
        //dialog_wait->hide();
        ContentLoadingwaitingMsgShow( tr("Failed to update queuelist."),1);
    }
    else if(id == 13){
        print_debug();
        //dialog_wait->hide();
        ContentLoadingwaitingMsgShow( tr("There was no drag and drop/delete item."),1);
    }
    else if(id == 14){
        print_debug();
        //ContentLoadingwaitingAniShow(tr("Please wait. Downloading Queuelist."),2);//c221115
        if(playType == "CD"){
            this->dialog_comfirmCD();
        }
    }
    else if(id == 15){
        print_debug();
        //dialog_wait->hide();
        ContentLoadingwaitingMsgShow( tr("There is no edit list selected."),1);
    }
    else if(id == 16){
        print_debug();
        //dialog_wait->hide();
        ContentLoadingwaitingMsgShow( tr("Log in to the menu is required."),1);
    }
    else if(id == 18){
        print_debug();
        //dialog_wait->hide();
        ContentLoadingwaitingMsgShow( tr("checking for latest version updates."),2);
    }
    else if(id == 19){
        print_debug();
        //dialog_wait->hide();
        global.updateCheckFlag = true;
        ContentLoadingwaitingMsgShow( tr("Failed to connect to server."),2);
    }
    else if(id == 20){
        print_debug();
        //dialog_wait->hide();
        ContentLoadingwaitingMsgShow( tr("Failed to play the selected track. service in preparation..."),2);
    }
    else if(id == 22){
        print_debug();
        ContentLoadingwaitingMsgShow(tr("Please wait. Downloading Queuelist."),1);
    }
    else if(id == 21){
        print_debug();
    }
    else if(id == 23){
        print_debug();

        ContentLoadingwaitingMsgShow(str, 3);
    }
    else if(id == 24){
        print_debug();
        if(this->showHideCheckFlag){
            if(this->dialog_wait->isHidden() != true){
                print_debug();
                this->dialog_wait->hide();
                ToastMsg::delay(this,"", tr("delay"), 500);//c230311
            }
            this->showHideCheckFlag = false;
        }
        else{

        }
        ContentLoadingwaitingMsgShow( tr("ROSE is standby mode.\nTurn on the device."),2);
    }
    else if(id == 25){
        print_debug();
        ContentLoadingwaitingMsgShow(tr("There is no song."),3);
    }
    else if(id == 26){//c220414
        print_debug();

    }
    else if(id == 27){
        ToastMsg::delay(this,"", tr("delay"), 2000);//c230319
        qDebug() << "global.Queue_track_count = " << global.Queue_track_count;
        global.Queue_track_count = 0;
        print_debug();

        ContentLoadingwaitingMsgShow(tr("Duplicate selected."),1);
    }
    else if(id == 227){//c230422
        ToastMsg::delay(this,"", tr("delay"), 2000);//c230319
        qDebug() << "global.Queue_track_count = " << global.Queue_track_count;
        global.Queue_track_count = 0;
        print_debug();

        ContentLoadingwaitingMsgShow(tr("Requested quality is not allowed in user's subscription.\n"),1);
    }
    else if(id == 28){
        print_debug();
        //dialog_wait->hide();
        ContentLoadingwaitingMsgShow( tr("Please check the connection status of the Rose device."),3);
    }
    else if(id == 29){
        print_debug();
        //dialog_wait->hide();
        //ToastMsg::show(this,"", tr("Currently, to use Apple Music in Queue, you can only log in from your device."), 2000, -60);
        ContentLoadingwaitingMsgShow( tr("Currently, to use Apple Music in Queue, you can only log in from your device."),3);
    }
    else if(id == 30){//c220729
        print_debug();
        //tmp_window_activate_flag = true;
        //ContentLoadingwaitingMsgShow(tr("The selected drag-and-drop content is played."),3);
        if(!global.window_activate_flag){//c230127
            global.window_activate_flag = true;
            ToastMsg::show(this, "", tr("The selected drag-and-drop content is played."), 2000);//c220824_4
            global.window_activate_flag = false;
        }
        else{
            ToastMsg::show(this, "", tr("The selected drag-and-drop content is played."), 2000);//c220824_4
        }
    }
    else if(id == 31){
        //print_debug();
        //ContentLoadingwaitingMsgShow( tr("There is no response for the requested content from the server."),2);
        if(!global.window_activate_flag){//c230127
            global.window_activate_flag = true;
            ToastMsg::show(this, "", tr("There is no response for the requested content from the server."), 1000);
            global.window_activate_flag = false;
        }else{
            ToastMsg::show(this, "", tr("There is no response for the requested content from the server."), 1000);
        }
    }
    else if(id == 32){//c220802
        print_debug();
        if(!global.window_activate_flag){//c230127
            tmp_window_activate_flag = true;
            ContentLoadingwaitingMsgShow(str,2);
            tmp_window_activate_flag = false;//c220804
        }
        else{
            ContentLoadingwaitingMsgShow(str,2);
        }
    }
    else if(id == 33){//c220803
        print_debug();
        slot_clickedYoutubeQuestion();//c220803
    }
    else if(id == 34){//c220823
        print_debug();
        if(!global.window_activate_flag){//c230127
            global.window_activate_flag = true;
            ToastMsg::show(this, "", tr("This function is being prepared for service."), 2000);
            global.window_activate_flag = false;
        }
        else{
            ToastMsg::show(this, "", tr("This function is being prepared for service."), 2000);
        }
    }
    else if(id == 35){
        print_debug();
        ToastMsg::show(this,"", str, 3000, 0, 2);//c221118
    }
}


void SectionBottom::slot_clickedYoutubeQuestion(){//c220803

    print_debug();
    DialogConfirm *dlgConfirmOutput = new DialogConfirm(this);
    dlgConfirmOutput->setAlignment(Qt::AlignLeft);
    dlgConfirmOutput->setTitle(tr("Youtube drag&drop play noticee"));
    dlgConfirmOutput->setText(tr("RoseConnect does not support Mix contents as information differs for each user. \nClick and \ndrag individual tracks"));

    dlgConfirmOutput->setGeometry(global.width_mainwindow*0.3, (global.height_mainwindow/2), 350, 500);
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


/**
 * @brief SectionBottom::emitSinalCurrPlayDataInfo 현재 음원정보를 시그널로 알려준다.
 */
void SectionBottom::emitSinalCurrPlayDataInfo(){

    // 음원이 변경되었다는 시그널을 보내서 데이터 다시 세팅할 수 있도록 시그널 보낸다.
    emit signal_changedCurrPlayTrack(this->data_jsonObject);
}


void SectionBottom::dialog_comfirmCD(){      //c220711
    //print_debug();

    DialogConfirm *dlgConfirmConnect_CDNotice = new DialogConfirm(this);//c230326

    //print_debug();

    dlgConfirmConnect_CDNotice->setAlignment(Qt::AlignLeft);
    dlgConfirmConnect_CDNotice->setTitle(tr("CD PLAY Notice!"));
    dlgConfirmConnect_CDNotice->setText(QString(tr("Stops the currently playing CDPLAY immediately. \nPlease select(click) a song again.")));

    dlgConfirmConnect_CDNotice->setTextHeight(150);

    int left = global.left_mainwindow + ((global.width_mainwindow - 350) / 2);
    int top = global.top_mainwindow + ((global.height_mainwindow - 250) / 2);

    dlgConfirmConnect_CDNotice->setGeometry(left, top, 350, 250);
    dlgConfirmConnect_CDNotice->setAlertMode();
    dlgConfirmConnect_CDNotice->setProperty("flagShown",false);
    if(dlgConfirmConnect_CDNotice->property("flagShown").toBool()==false){
        dlgConfirmConnect_CDNotice->setProperty("flagShown",true);

        int result = dlgConfirmConnect_CDNotice->exec();
        dlgConfirmConnect_CDNotice->setProperty("flagShown",false);
        //print_debug();
        if(result == QDialog::Accepted){
            dlgConfirmConnect_CDNotice->hide();
            //print_debug();
            //print_debug();
            QJsonObject tmp_json;
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("cd.play.stop"), tmp_json, true, true);
        }

    }

}


/**
 * @brief SectionBottom::setDataSectionBottom 외부로부터 데이터 세팅한다.
 * @param p_jsonObject
 */
void SectionBottom::setDataSectionBottom(QJsonObject p_jsonObject){

    // if(flag_response_proc == false){//c220709
    //flag_response_proc = true;
    // 1> Json 풀어서 멤버변수에 세팅
     //print_debug();
     //QJsonDocument doc(p_jsonObject);    QString strJson(doc.toJson(QJsonDocument::Compact));    qDebug() << "SectionBottom::setJsonData-strJson = " << strJson;
    /*
        if(p_jsonObject.contains("data")){

            QJsonObject tmp_data = p_jsonObject["data"].toObject();
            if(tmp_data["titleName"].toString() == "" || tmp_data["playType"].toString() == ""){
              //  flag_response_proc = false;
                print_debug();

                this->setJsonData(p_jsonObject);
                return;//c220709
            }else{
                print_debug();
               this->setJsonData(p_jsonObject);
            }
        }else{

            print_debug();
            this->setJsonData(p_jsonObject);
           // flag_response_proc = false;
            return;
        }
        print_debug();
        QJsonDocument doc2(p_jsonObject);    QString strJson1(doc.toJson(QJsonDocument::Compact));    qDebug() << "SectionBottom::setJsonData-strJson1 = " << strJson1;
*/
    // 1> Json 풀어서 멤버변수에 세팅
    this->setJsonData(p_jsonObject);

    // 2> UI에 데이터를 세팅
    this->setUIDataBasicInfo();

    // 3> 재생 관련 아이콘 세팅
    this->setUIDataPlayIcon();
    //  flag_response_proc = false;
    //}
}


/**
 * @brief SectionBottom::setJsonData 데이터 풀어서 멤버변수에 세팅
 * @param p_jsonObject
 */
void SectionBottom::setJsonData(QJsonObject p_jsonObject){

    // 재생중 트랙 변경 확인
    bool flagChangedCurrrPlayTrack = false;
    this->data_jsonObject = p_jsonObject;
    //this->audio_id = ""; // [중요] 초기화 안함.. 이전 데이터와 비교하기 위함
    //this->playType = ""; // [중요] 초기화 안함.. 이전 데이터와 비교하기 위함
    //this->titleName = ""; // [중요] 초기화 안함.. 이전 데이터와 비교하기 위함 (음악/비디오/라디오)
    //this->albumName = ""; // [중요] 초기화 안함.. 이전 데이터와 비교하기 위함 (음악/비디오/라디오)
    this->artistName = "";
    //this->duration = 0;//3333
    //this->curPosition = 0;3333
    //this->isHdmiOn = false;  // [중요] 초기화 안함.. 이전 데이터와 비교하기 위함
    this->isPlaying = false;
    this->isServer = false;
    this->isShareFile = false;
    //this->repeatMode = 0;//c220828_2
    //this->shuffleMode = 0;//c220828_2
    this->thumbnail = "";
    this->album_id = 0;
    this->artist_id = "";

    //print_debug();
    QJsonDocument doc(p_jsonObject);    QString strJson(doc.toJson(QJsonDocument::Compact));    //qDebug() << "SectionBottom::setJsonData-strJson = " << strJson;
    //global.sectionBottomTop = this->pos().y();//c221021_1
    //qDebug() <<"global.top_mainwindow="<<global.top_mainwindow;
    //qDebug() <<"global.height_mainwindow="<<global.height_mainwindow;
    if(this->isActiveWindow() || global.pr->isActiveWindow()) {//c220907_1
        //print_debug();//c220909_1
        global.window_activate_flag = true;
    }else{
        //print_debug();//c220909_1
        global.window_activate_flag = false;
    }
    //print_debug();
    //c220525
    //c220521   start
    long tmp_currTimeSec_error = QDateTime::currentSecsSinceEpoch();
    if(latestCheckTimeSecond_connect == 0){
        latestCheckTimeSecond_connect = QDateTime::currentSecsSinceEpoch();
    }
    if(latestCheckTimeSecond_connect5sec == 0){//c230315_1
        latestCheckTimeSecond_connect5sec = QDateTime::currentSecsSinceEpoch();
    }
    if(latestCheckTimeSecond_connect60sec == 0){//c230315_1
        latestCheckTimeSecond_connect60sec = QDateTime::currentSecsSinceEpoch();
    }
    if(latestCheckTimeSecond_connectloginsec == 0){//c230422
        latestCheckTimeSecond_connectloginsec = QDateTime::currentSecsSinceEpoch();
    }

    global.power_sleepMode_flag = false;
    if(ProcJsonEasy::getString(p_jsonObject, "code") == "SLEEP"){
        //print_debug();
        global.power_sleepMode_flag = true;
        long tmp_currTimeSec = QDateTime::currentSecsSinceEpoch();

        if(!global.powerDialogShowFlag && !dlgWakeUp->isVisible() && tmp_currTimeSec > (latestCheckTimeSecond + 13)){//c221004_1

            // dlg wake-up 위치 재조정
            int left = global.left_mainwindow + ((global.width_mainwindow - 600) / 2);
            int top = global.top_mainwindow + ((global.height_mainwindow - 600) / 2);

            qDebug() << global.left_mainwindow << global.width_mainwindow << this->dlgWakeUp->sizeHint().width() << ((global.width_mainwindow - this->dlgWakeUp->sizeHint().width()) / 2);

            this->dlgWakeUp->setGeometry(left, top, 0, 0);

            if(!global.device.getDeviceName().isEmpty()){
                this->dlgWakeUp->setDevice(global.device.getDeviceName());
            }
            else{
                this->dlgWakeUp->setDevice(tr("NONE"));
            }
            this->dlgWakeUp->exec();

            latestCheckTimeSecond = QDateTime::currentSecsSinceEpoch();
        }

        if(this->dlgWakeUp->isHidden()){
            // this->m_updater_tmp->checkForUpdates_tmp();//c220521
        }
    }
    else{
        if(p_jsonObject.contains("flagOk") && !p_jsonObject["flagOk"].toBool()){

            if( (tmp_currTimeSec_error > (latestCheckTimeSecond_connect + 15) )){//c220623

                // print_debug();
                emit this->linker->signal_device_connect_check_error();

                latestCheckTimeSecond_connect = QDateTime::currentSecsSinceEpoch();
            }//c220521   end
        }
        else{
            //emit this->linker->signal_device_connect_check();//c221004_2
            //print_debug();
            if( (tmp_currTimeSec_error > (latestCheckTimeSecond_connect5sec + 5) )){//c221004_2

                //  print_debug();
                emit this->linker->signal_device_connect_check();

                latestCheckTimeSecond_connect5sec = QDateTime::currentSecsSinceEpoch();
            }//c220521   end
            if( (tmp_currTimeSec_error > (latestCheckTimeSecond_connect60sec + 3600) )){//c230410

                //  print_debug();
                slot_Update_call();//c230315_1

                latestCheckTimeSecond_connect60sec = QDateTime::currentSecsSinceEpoch();
            }
            if( (tmp_currTimeSec_error > (latestCheckTimeSecond_connect60sec + 3600) )){//c230422

                //  print_debug();
                slot_Update_call();//c230315_1

                latestCheckTimeSecond_connect60sec = QDateTime::currentSecsSinceEpoch();
            }
            if( (tmp_currTimeSec_error > (latestCheckTimeSecond_connectloginsec + 3600) )){//c230422

                QJsonObject tmp_data = p_jsonObject["data"].toObject();
                print_debug();
                if(p_jsonObject.contains("data")){
                    //print_debug();
                    QJsonObject tmp_data = p_jsonObject["data"].toObject();
                    if(tmp_data.contains("playType")){
                        if((tmp_data["playType"].toString() == playTypes::BUGS) || (tmp_data["playType"].toString() == playTypes::BUGS_MV)){
                            QDateTime e_date = QDateTime::fromString(global.user_forBugs.getProduct_display_end_dt(),"yyyy.MM.dd HH:mm");
                            //qDebug() << "e_date=" << e_date;
                            QDateTime c_date = QDateTime::currentDateTime();
                            //qDebug() << "c_date=" << c_date;
                            if(e_date.isNull() || e_date < c_date){//c230426
                                print_debug();

                                global.user_forBugs.set_logoutState();
                            }else{
                                print_debug();
                            }


                        }
                        if((tmp_data["playType"].toString() == playTypes::QOBUZ)){
                            //--qobuz---------------------------------------------
                            if(!global.user_forQobuz.getUsername().isEmpty()){
                                global.user_forQobuz.flag_login_wait = true;
                                qobuz::ProcCommon *procQobuz = new qobuz::ProcCommon(this);
                                qobuz::UserLoginInfo qobuz_userLoginInfo = procQobuz->getLoginInfo_qobuzDB();
                                //connect(procQobuz, &qobuz::ProcCommon::successLogin, this, &HomeMain::slot_completeReq_getqLoginInfo);
                                procQobuz->request_qobuzLogin_termCall(qobuz_userLoginInfo);
                            }
                            //-------------------------------------------------
                        }
                        if((tmp_data["playType"].toString() == playTypes::TIDAL)|| (tmp_data["playType"].toString() == playTypes::TIDAL_VIDEO)){
                            //--tidal-------------------------------
                            tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);//
                            procTidal->request_tidal_getRefreshTokenAndSave();//

                        }
                    }
                }

                latestCheckTimeSecond_connectloginsec = QDateTime::currentSecsSinceEpoch();
            }
        }
    }

    if(this->dlgWakeUp->isVisible() == true && ProcJsonEasy::getString(p_jsonObject, "code") == "G0000"){
        print_debug();
        global.power_sleepMode_flag = false;
        this->dlgWakeUp->hide();

    }
    else{

    }

    if(p_jsonObject.contains("data")){
        //print_debug();
        QJsonObject tmp_data = p_jsonObject["data"].toObject();
        if(tmp_data.contains("playType")){
            if(tmp_data["playType"].toString() == ""){
                // bottomInfo_flag = true;
                //print_debug();
                // return;//c220828_2
            }

            if(this->playType!="" && tmp_data["playType"].toString()!=this->playType){
                // 이전에 세팅되어있던 음원 PK와 다른경우.. 음원 변경되었다는 시그널 호출
                flagChangedCurrrPlayTrack = true;
                emit signal_changedCurrPlayTrack(p_jsonObject);//bj230517
                print_debug();
                qDebug() << "flagChangedCurrrPlay=" << flagChangedCurrrPlayTrack;
            }
            this->playType = tmp_data["playType"].toString();
            //qDebug() << "this->playType=" << this->playType;
            global.device.CurrPlayType = this->playType;
        }else{
            this->playType = "";
        }
        if(tmp_data.contains("albumName")){
            if(playType==playTypes::YOUTUBE){
                if(this->albumName!="" && tmp_data["albumName"].toString()!=this->albumName){
                    // 이전에 세팅되어있던 음원 PK와 다른경우.. 음원 변경되었다는 시그널 호출
                    flagChangedCurrrPlayTrack = true;
                    emit signal_changedCurrPlayTrack(p_jsonObject);//bj230517
                    //print_debug();
                    //qDebug() << "flagChangedCurrrPlay=" << flagChangedCurrrPlayTrack;
                }
                this->albumName = tmp_data["albumName"].toString();

            }
            else if(playType==playTypes::ROSE_RADIO){
                this->albumName = tmp_data["stream_title"].toString();
                if(this->albumName!="" && tmp_data["albumName"].toString()!=this->albumName){
                    emit signal_changedCurrPlayTrack(p_jsonObject);//bj230517
                    // 이전에 세팅되어있던 음원 PK와 다른경우.. 음원 변경되었다는 시그널 호출
                    //flagChangedCurrrPlayTrack = true;
                }
            }
        }
        else{
            this->albumName = "";
        }

        if(tmp_data.contains("titleName")){
            if(tmp_data["titleName"].toString() == ""){
                //  bottomInfo_flag = true;
                if(playType!=playTypes::YOUTUBE){
                    // return;//c220709
                }
            }
            if(this->titleName!="" && tmp_data["titleName"].toString()!=this->titleName){
                // 이전에 세팅되어있던 음원 PK와 다른경우.. 음원 변경되었다는 시그널 호출
                // print_debug();
                flagChangedCurrrPlayTrack = true;
                emit signal_changedCurrPlayTrack(p_jsonObject);//bj230517
                //qDebug() << "flagChangedCurrrPlay=" << flagChangedCurrrPlayTrack;
            }
            this->titleName = tmp_data["titleName"].toString();

            global.device.titleName = this->titleName;
            //qDebug() << "this->titleName=" << this->titleName;

        }
        else{
            this->titleName = "";
            //global.device.TitleName = this->titleName;
            global.device.titleName = this->titleName;

        }
        if(tmp_data.contains("artistName")){
            if(playType==playTypes::ROSE_RADIO){//c220923_1
                this->artistName = "";

                if(tmp_data.contains("tempArr")){
                    QStringList stream_title_list;
                    if(!tmp_data["tempArr"].toArray().empty()){
                        QJsonArray tmp_array = tmp_data["tempArr"].toArray();

                        if(tmp_array.count()>0){
                            for(int i = 0 ; i < tmp_array.count(); i++){
                                QString temp = tmp_array.at(i).toString();
                                stream_title_list.append(temp.split(":").last());
                            }

                            if(stream_title_list.last() != "false"){
                                this->artistName = stream_title_list.last();
                            }
                        }
                    }
                }
            }
            else{
                this->artistName = tmp_data["artistName"].toString();
            }
        }
        if(tmp_data.contains("trackInfo")){
            this->trackinfo = tmp_data["trackInfo"].toString();
        }
        if(tmp_data.contains("ui_state")){//c230327
            this->ui_state = tmp_data["ui_state"].toInt();
        }
        //qDebug() << "this->titleName = " << this->titleName;
        //qDebug() << "this->artistName = " << this->artistName;
        if(tmp_data.contains("duration")){
            this->duration = tmp_data["duration"].toString().toInt();
            //qDebug()<< "SectionBottom::setJsonData----this->duration" << this->duration;
            global.current_duration = this->duration;
        }
        if(tmp_data.contains("isPlaying")){
            this->isPlaying = tmp_data["isPlaying"].toBool();
            //print_debug();
            //qDebug() << "this->isPlaying=" << this->isPlaying;
            if(!global.desktopPlayerMode){
                global.device_playingState = this->isPlaying;//c220704
            }
            global.isPlaying = this->isPlaying;
        }
        if(tmp_data.contains("curPosition")){//cheon211206
            this->curPosition = tmp_data["curPosition"].toString().toInt();
            // qDebug()<< "SectionBottom::setJsonData----this->curPosition" << this->curPosition;
            //if(this->curPosition < 0) this->curPosition = 0;
            global.current_position = this->curPosition;


            if(global.isPlaying){
                // print_debug();
                global.isPlaying_flag = true;
            }else{
                //print_debug();
                global.isPlaying_flag = false;
            }// global.queueTimerFlag &&
            //qDebug() << "next-global.isPlaying_flag = " << global.isPlaying_flag;

            //qDebug() << "this->shuffleMode = " << this->shuffleMode;
            if(!global.queueTimerFlag){
                QString tmp_style = this->btn_list->styleSheet();
                this->flagSelectedQueue = false;
                tmp_style.replace(":/images/play_list_icon_on.png", ":/images/play_list_icon.png");
                this->btn_list->setStyleSheet(tmp_style);
            }
        }
        if(tmp_data.contains("isFavorite")){
        }
        if(tmp_data.contains("isFile")){
        }
        if(tmp_data.contains("isHdmiOn")){
            if(tmp_data["isHdmiOn"].toBool()!= this->isHdmiOn){
                flagChangedCurrrPlayTrack = true;//c220709
                //print_debug();
                //qDebug() << "flagChangedCurrrPlay=" << flagChangedCurrrPlayTrack;
                emit this->signal_changedCurrHDMI(tmp_data["isHdmiOn"].toBool());
            }
            this->isHdmiOn = tmp_data["isHdmiOn"].toBool();
        }
        if(tmp_data.contains("isServer")){
            this->isServer = tmp_data["isServer"].toBool();
        }
        if(tmp_data.contains("repeatMode")){
            this->repeatMode = tmp_data["repeatMode"].toInt();
            global.repeatMode = this->repeatMode;
            //qDebug() << "global.repeatMode=" << global.repeatMode;

            if(global.repeatMode == 2){
                global.music_playlist->setPlaybackMode(QMediaPlaylist::Loop);//c220609

            }else if(global.repeatMode == 0){
                global.music_playlist->setPlaybackMode(QMediaPlaylist::Sequential);//c220609

            }else if(global.repeatMode == 1){
                global.music_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);//c220609

            }


        }
        if(tmp_data.contains("shuffleMode")){
            this->shuffleMode = tmp_data["shuffleMode"].toInt();
            global.shuffleMode = this->shuffleMode;
        }
        if(tmp_data.contains("subAppCurrentData")){
            QString tmp = tmp_data["subAppCurrentData"].toString();
            QJsonDocument doc = QJsonDocument::fromJson(tmp.toUtf8());
            QJsonObject tmp_data_subAppCurrentData = doc.object();

            //print_debug();
            //QJsonDocument doc1(tmp_data_subAppCurrentData);    QString strJson(doc1.toJson(QJsonDocument::Compact));    qDebug() << "SectionBottom::setJsonData-strJson = " << strJson;

            if(tmp_data_subAppCurrentData.contains("duration")){
                //this->duration


                if(playType==playTypes::YOUTUBE){
                    QString tempstr = tmp_data_subAppCurrentData["duration"].toString();
                    QString tmp_split = "";
                    QStringList splitToken;
                    tmp_split = tempstr;
                    splitToken = tmp_split.split(":");
                    //qDebug() << "tempstr = " << tempstr;
                    //qDebug() << "splitToken.size() = " << splitToken.size();
                    if(splitToken.size() > 1){

                    }else{
                        int tmp_duration = tmp_data_subAppCurrentData["duration"].toString().toInt();
                        //qDebug()<< "SectionBottom::setJsonData----tmp_duration=" << tmp_duration;
                        if(tmp_duration > 0){
                            // print_debug();//c220716
                            this->duration = tmp_duration *1000;
                            global.current_duration = this->duration;
                            global.current_position = this->curPosition;
                        }else{
                            this->duration = -1;
                            this->curPosition = 0;
                            global.current_duration = this->duration;
                            global.current_position = this->curPosition;
                        }
                    }
                }


            }
            //print_debug();
            //qDebug() << "this->duration = " << this->duration;

            int old_track_id = -1;
            int curr_track_id = -1;
            if(playType==playTypes::TIDAL){
                old_track_id = tidal::ProcTidal_forOuter::extract_track_id_onTrackData(this->jsonObj_subAppCurrentData);
                curr_track_id = tidal::ProcTidal_forOuter::extract_track_id_onTrackData(tmp_data_subAppCurrentData);
            }else if(playType==playTypes::TIDAL_VIDEO){
                old_track_id = tidal::ProcTidal_forOuter::extract_video_id_onVideoData(this->jsonObj_subAppCurrentData);
                curr_track_id = tidal::ProcTidal_forOuter::extract_video_id_onVideoData(tmp_data_subAppCurrentData);
            }else if(playType==playTypes::BUGS){
                old_track_id = bugs::ProcBugs_forOuter::extract_track_id_onTrackData(this->jsonObj_subAppCurrentData);
                curr_track_id = bugs::ProcBugs_forOuter::extract_track_id_onTrackData(tmp_data_subAppCurrentData);
            }else if(playType==playTypes::BUGS_MV){
                old_track_id = bugs::ProcBugs_forOuter::extract_video_id_onVideoData(this->jsonObj_subAppCurrentData);
                curr_track_id = bugs::ProcBugs_forOuter::extract_video_id_onVideoData(tmp_data_subAppCurrentData);
            }else if(playType==playTypes::QOBUZ){
                old_track_id = qobuz::ProcQobuz_forOuter::extract_track_id_onTrackData(this->jsonObj_subAppCurrentData);
                curr_track_id = qobuz::ProcQobuz_forOuter::extract_track_id_onTrackData(tmp_data_subAppCurrentData);
            }else if(playType==playTypes::APPLE){
                old_track_id = qobuz::ProcQobuz_forOuter::extract_track_id_onTrackData_Apple(this->jsonObj_subAppCurrentData);
                curr_track_id = qobuz::ProcQobuz_forOuter::extract_track_id_onTrackData_Apple(tmp_data_subAppCurrentData);
            }


            if(curr_track_id != old_track_id){
                // 이전에 세팅되어있던 음원 PK와 다른경우.. 음원 변경되었다는 시그널 호출
                flagChangedCurrrPlayTrack = true;
                // print_debug();
                // qDebug() << "flagChangedCurrrPlay=" << flagChangedCurrrPlayTrack;
            }
            else if(curr_track_id == -1){
                flagChangedCurrrPlayTrack = false;
            }
            // Tidal, Bugs, Qobuz는 이 정보가 필요함. 멤버 변수로 저장함 by sunnyfish ------------------------ TIDAL, BUGS, QOBUZ
            this->jsonObj_subAppCurrentData = tmp_data_subAppCurrentData;

            if(tmp_data_subAppCurrentData.contains("mMusicSongInfo")){
                QJsonObject tmp_data_mMusicSongInfo = tmp_data_subAppCurrentData["mMusicSongInfo"].toObject();
                if(tmp_data_mMusicSongInfo.contains("album_id")){
                    this->album_id = tmp_data_mMusicSongInfo["album_id"].toInt();
                }
                if(tmp_data_mMusicSongInfo.contains("artist_id")){
                    this->artist_id = tmp_data_mMusicSongInfo["artist_id"].toString();
                }
                if(tmp_data_mMusicSongInfo.contains("id")){
                    //this->audio_id = tmp_data_mMusicSongInfo["id"].toString();
                }
            }
            if(tmp_data_subAppCurrentData.contains("mDirectFileInfo")){
            }
        }

        if(tmp_data.contains("thumbnail")){
            QJsonArray tmp_array = tmp_data["thumbnail"].toArray();
            if(tmp_array.count() > 0){
                if(playType == playTypes::ROSE_RADIO){
                    QString temp = tmp_array.first().toString().replace("//","/");
                    this->thumbnail = temp.replace("https:/","https://");
                }
                else if(playType == playTypes::SPOTIFY || playType == playTypes::ROON || playType == playTypes::DLNA || playType == playTypes::AIRPLAY){
                    QString temp = tmp_array.first().toString();

                    if(temp.contains("http")){
                        this->thumbnail = temp;
                    }
                    else{
                        this->thumbnail = "http://" + global.device.getDeviceIP() + ":" + global.port_img + temp;
                    }
                }
                else if(playType==playTypes::CD){
                    QString temp = tmp_array.first().toString();

                    if(temp.contains("http")){
                        this->thumbnail = temp;
                    }
                    else{
                        this->thumbnail = "http://" + global.device.getDeviceIP() + ":" + global.port_img + temp;
                    }

                    QString isIsoOpen_str = tmp_array.last().toString();
                    QString isIsoOpen_bool = isIsoOpen_str.split(":").last();
                    //qDebug() << "isIsoOpen_bool=" << isIsoOpen_bool;
                    if(isIsoOpen_bool == "true"){
                        this->isIsoOpen = true;
                    }else{
                        this->isIsoOpen = false;
                    }
                }
                else if(playType == playTypes::VIDEO){
                    QString temp = tmp_array.first().toString();
                    if(temp.contains("jpg") || temp.contains("jpeg")){
                        this->thumbnail = temp;
                    }
                    else{
                        this->thumbnail = "";
                    }
                }
                else{
                    this->thumbnail = tmp_array.first().toString();
                }
            }

            // 하단 현재재생플레이바의 array[2] 인덱스 접근시, count 확인하는 if 문 추가..
            // 경우따라 array갯수가 다르게 들어오는경우가 잇는 모양인가보옴. API가 그렇게 생겨서 어쩔수 없음.. if문 걸어놓고 하는 수밖에
            if(tmp_array.count() >= 3){
                QString tmp_isShareFile = tmp_array[2].toString();
                if(tmp_isShareFile.startsWith("isShareFile") && tmp_isShareFile.endsWith("true")){
                    this->isShareFile = true;
                }
            }

            if(tmp_array.count() >= 5){
                QString tmp_audioId = tmp_array[4].toString();
                if(tmp_audioId.startsWith("audioId:")){
                    QString tmp_audio_id = tmp_audioId.replace("audioId:", "");
                    if(this->audio_id>0 && tmp_audio_id!="" && tmp_audio_id!=this->audio_id){
                        // 이전에 세팅되어있던 음원 PK와 다른경우.. 음원 변경되었다는 시그널 호출
                        flagChangedCurrrPlayTrack = true;
                        // print_debug();
                        //qDebug() << "flagChangedCurrrPlay=" << flagChangedCurrrPlayTrack;
                    }
                    this->audio_id = tmp_audio_id;
                }
            }
            //qDebug() << "this->thumbnail = " << this->thumbnail;
        }
        // print_debug();
        // bottomInfo_flag = false;
    }else{
        // print_debug();//c221115
        //return 0;
        // bottomInfo_flag = true;
    }


    // ---------------------------------------------------------------
    //  재생중인 트랙변경 시그널 호출
    // ---------------------------------------------------------------
    if(global.device.getDeviceIP()!="" && global.user.getAccess_token()!=""){//c220618
        if(flagChangedCurrrPlayTrack){
            print_debug();
            if(global.queueTimerFlag){
                // emit this->linker->signal_queuelist_reload();//c220618
                emit this->linker->signal_queuelist_musicStart();

                global.isPlaying_stop_flag = false;
                print_debug();
            }
            if(global.desktopPlayerMode){//c220618
                emit this->linker->signal_playlist_reflesh();//c220618
            }

            emit signal_changedCurrPlayTrack(p_jsonObject);//c220615
            //emit this->linker->signal_rosefmChannelChanged(p_jsonObject);
        }
    }
    //return 1;
    // print_debug();
}

/**
 * @brief SectionBottom::setUIDataBasicInfo 기본정보 세팅
 */
void SectionBottom::setUIDataBasicInfo(){

    /*this->lb_title->setText(this->titleName);
    this->lb_artistName->setText(this->artistName);
    this->lb_trackinfo->setText(this->trackinfo);*/

    this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->titleName, this->lb_title->geometry().width(), this->lb_title->font()));
    //QString tmpCompare = (this->lb_title->text() == this->titleName) ? "is same" : "is not same";
    //qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << this->lb_title->text() << this->titleName << tmpCompare;
    if(this->lb_title->text().contains("…")){
        //this->lb_title->setToolTip(this->titleName);
        QString html = QString("<span style='background-color: transparent; font-size: 16px; font-weight: normal; line-height: 1.44; text-align: left; color: #000000;'>%1</span>").arg(this->titleName);
        this->lb_title->setToolTip(html);
        this->lb_title->setToolTipDuration(2000);
    }
    else{
        this->lb_title->setToolTip("");
    }

    this->lb_artistName->setText(GSCommon::getTextCutFromLabelWidth(this->artistName, this->lb_artistName->geometry().width(), this->lb_artistName->font()));
    if(this->lb_artistName->text().contains("…")){
        this->lb_artistName->setToolTip(this->artistName);
        this->lb_artistName->setToolTipDuration(2000);
    }
    this->lb_trackinfo->setText(GSCommon::getTextCutFromLabelWidth(this->trackinfo, this->lb_trackinfo->geometry().width(), this->lb_trackinfo->font()));
    if(this->lb_trackinfo->text().contains("…")){
        this->lb_trackinfo->setToolTip(this->trackinfo);
        this->lb_trackinfo->setToolTipDuration(2000);
    }

    if(this->duration < 0){
        this->slider_playbar->setMaximum(100);
    }
    else{
        this->slider_playbar->setMaximum(this->duration);
    }

    this->slider_playbar->setCursor(Qt::PointingHandCursor);
    //this->slider_playbar->setStyleSheet(this->slider_playbar->styleSheet().replace("QSlider::handle:horizontal { width:0px;", "QSlider::handle:horizontal { width:15px;"));//cheon211203
    //print_debug();
    //if(slot_sliderClick_flag)
    //this->slider_playbar->disconnect();//5555
    //connect(this->slider_playbar, SIGNAL(signal_sliderclick(int)), this, SLOT(slot_sliderClick(int)));//c1209
    //connect(this->slider_playbar, SIGNAL(valueChanged(int)) ,this, SLOT(changedSliderBar()));
    //connect(this->slider_playbar, SIGNAL(sliderReleased()) ,this, SLOT(changedSliderBar()));
    //connect(this->slider_playbar, SIGNAL(valueChanged(int)) ,this, SLOT(slot_sliderClick(int)));
    //connect(this->slider_playbar, SIGNAL(sliderMoved()) ,this, SLOT(changedSliderBar()));
    //qDebug() <<"______________________________________________________________"<<this->playType;


    QString tmp_iconPath;
    QString tmp_imgURL = "";
    // 어떤 value로 실제 들어오는지 값들 미확인함.... MUSIC/VIDEO/RADIO/YOUTUBE/POD 만 확인완료
    if(this->playType==playTypes::Music){
        tmp_iconPath = ":/images/playBar/mus_play_ico.png";   // 일반 음악파일
        if(this->isShareFile==true){
            tmp_iconPath = ":/images/playBar/mus_play_ico_share.png";   // 공유 모양 아이콘
        }else if(this->isServer==true){
            tmp_iconPath = ":/images/playBar/net_play_ico.png"; // 네트워크 파일(마이/친구플레이리스트에있는것도 포함)
        }
        if(this->thumbnail.startsWith("http")==false){
            //print_debug();
            QString roseURL = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(this->thumbnail);
            //tmp_imgURL = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(this->thumbnail);
            tmp_imgURL =roseURL;
        }else{
            //print_debug();
            tmp_imgURL = this->thumbnail;
        }
    }
    else if(this->playType==playTypes::VIDEO){
        tmp_iconPath = ":/images/playBar/vide_play_ico.png";

        if(this->thumbnail.startsWith("http") == false){
            //print_debug();
            QString roseURL = QString("http://%1:%2%3").arg(global.device.getDeviceIP(), global.port_img, this->thumbnail);
            //tmp_imgURL = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(this->thumbnail);
            tmp_imgURL = roseURL;
        }
        else{
            //print_debug();
            tmp_imgURL = this->thumbnail;
        }
    }
    else if(this->playType==playTypes::RADIO){
        tmp_iconPath = ":/images/playBar/radio_play_ico.png";
        this->slider_playbar->setCursor(Qt::ArrowCursor);
        //this->slider_playbar->setStyleSheet(this->slider_playbar->styleSheet().replace("QSlider::handle:horizontal { width:5px;", "QSlider::handle:horizontal { width:0px;"));    // handle 0px로 변경
        //print_debug();
        //this->slider_playbar->disconnect();
    }
    else if(this->playType==playTypes::ROSE_RADIO){
        tmp_iconPath = ":/images/playBar/roseradio_play_ico.png";
        tmp_imgURL = QString("%1").arg(this->thumbnail);
        this->slider_playbar->setCursor(Qt::ArrowCursor);
        //this->slider_playbar->setStyleSheet(this->slider_playbar->styleSheet().replace("QSlider::handle:horizontal { width:5px;", "QSlider::handle:horizontal { width:0px;"));    // handle 0px로 변경
        //print_debug();
        //this->slider_playbar->disconnect();
    }
    else if(this->playType==playTypes::FM_TUNER){
        tmp_iconPath = ":/images/playBar/fm_play_ico.png";
        tmp_imgURL = QString("%1").arg(this->thumbnail);
        this->slider_playbar->setCursor(Qt::ArrowCursor);
        //this->slider_playbar->setStyleSheet(this->slider_playbar->styleSheet().replace("QSlider::handle:horizontal { width:5px;", "QSlider::handle:horizontal { width:0px;"));    // handle 0px로 변경
        //print_debug();
        //if(slot_sliderClick_flag)
        //this->slider_playbar->disconnect();
    }
    else if(this->playType==playTypes::YOUTUBE){
        tmp_iconPath = ":/images/playBar/tube_play_ico.png";//c220620
        tmp_imgURL = QString("%1").arg(this->thumbnail);
    }
    else if(this->playType==playTypes::POD){
        tmp_iconPath = ":/images/playBar/pod_play_ico.png";
        tmp_imgURL = QString("%1").arg(this->thumbnail);
    }
    else if(this->playType==playTypes::TIDAL || this->playType==playTypes::TIDAL_VIDEO){
        tmp_iconPath = ":/images/playBar/tidal_play_ico.png";

        tmp_imgURL = QString("%1").arg(this->thumbnail);
        //print_debug();
        //qDebug() << "tmp_imgURL=" << tmp_imgURL;
    }
    else if(this->playType==playTypes::BUGS || this->playType==playTypes::BUGS_MV){
        tmp_iconPath = ":/images/playBar/bugs_play_ico.png";
        tmp_imgURL = QString("%1").arg(this->thumbnail);
    }
    else if(this->playType==playTypes::QOBUZ){
        tmp_iconPath = ":/images/playBar/qobuz_play_ico.png";
        tmp_imgURL = QString("%1").arg(this->thumbnail);
    }
    else if(this->playType==playTypes::APPLE){
        tmp_iconPath = ":/images/apple/main_icon_apple.png";
        tmp_imgURL = QString("%1").arg(this->thumbnail);
    }
    else if(this->playType==playTypes::ROON){
        tmp_iconPath = ":/images/playBar/roon_play_ico.png";
        tmp_imgURL = QString("%1").arg(this->thumbnail);
    }
    else if(this->playType==playTypes::SPOTIFY){
        tmp_iconPath = ":/images/playBar/spotify_play_ico.png";
        tmp_imgURL = QString("%1").arg(this->thumbnail);
    }
    else if(this->playType==playTypes::DLNA){
        tmp_iconPath = ":/images/playBar/dlna_play_ico.png";
        tmp_imgURL = QString("%1").arg(this->thumbnail);
    }
    else if(this->playType==playTypes::AIRPLAY){
        tmp_iconPath = ":/images/playBar/airplay_play_ico.png";
        tmp_imgURL = QString("%1").arg(this->thumbnail);
    }
    else if(this->playType==playTypes::BT){
        tmp_iconPath = ":/images/playBar/bt_play_ico.png";
        tmp_imgURL = QString("%1").arg(this->thumbnail);
    }
    else if(this->playType==playTypes::CD){
        tmp_iconPath = ":/images/playBar/cd_play_ico.png";
        tmp_imgURL = QString("%1").arg(this->thumbnail);

        if(this->thumbnail != this->cd_img_before){
            this->cd_img_change = "";

            CommonGetRedirectUrl *redirectUrl = new CommonGetRedirectUrl("CD", this->thumbnail);                        //j220903 twitter
            connect(redirectUrl, &CommonGetRedirectUrl::signal_redirectUrl, this, &SectionBottom::slot_redirectUrl);    //j220903 twitter
        }
    }   

    if(!tmp_iconPath.isEmpty() && iconPath != tmp_iconPath){
        this->iconPath = tmp_iconPath;
        QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(tmp_iconPath);
        this->lb_icon_playType->setPixmap(*pixmap_icon);
        this->lb_icon_playType->resize(pixmap_icon->width(), pixmap_icon->height());
    }

    if(this->titleName==""){//c220824
        if(this->playType!=playTypes::YOUTUBE ){
            this->lb_icon_playType->clear();
            //this->lb_title->setText(tr("The selected playlist is empty."));
            this->titleName = tr("The selected playlist is empty.");
            this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->titleName, this->lb_title->geometry().width(), this->lb_title->font()));
        }
        else{
            //this->lb_title->setText(tr("The selected playlist is YouTube Music"));
            this->titleName = tr("The selected playlist is YouTube Music");
            this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->titleName, this->lb_title->geometry().width(), this->lb_title->font()));
        }
    }

    if(this->artistName==""){
        this->lb_artistName->setText(GSCommon::getTextCutFromLabelWidth(this->artistName, this->lb_artistName->geometry().width(), this->lb_artistName->font()));
        if(this->lb_artistName->text().contains("…")){
            this->lb_artistName->setToolTip(this->artistName);
            this->lb_artistName->setToolTipDuration(2000);
        }
        this->lb_artistName->setVisible(false);
    }
    else{
        this->lb_artistName->setText(GSCommon::getTextCutFromLabelWidth(this->artistName, this->lb_artistName->geometry().width(), this->lb_artistName->font()));
        if(this->lb_artistName->text().contains("…")){
            this->lb_artistName->setToolTip(this->artistName);
            this->lb_artistName->setToolTipDuration(2000);
        }
        this->lb_artistName->setVisible(true);
    }

    this->lb_albumThumbText->hide();
    if(this->thumbnail != ""){
        this->cd_img_before = tmp_imgURL;

        // 앨범이미지 download
        if(this->playType==playTypes::CD){     //j220903 twitter
            if(this->cd_img_change.isEmpty()){
                QImage image;
                image.load(":/images/cd/cd_thum_60x2.png");

                QPixmap pixmapIMG = QPixmap(QSize(IMG_W_MAX, IMG_W_MAX));
                pixmapIMG.fill(Qt::transparent);

                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                tmp_pixmap = tmp_pixmap.scaled(IMG_W_MAX, IMG_W_MAX, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

                QPainter painter (&pixmapIMG);
                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
                painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

                QPainterPath path = QPainterPath();
                path.addRoundedRect(0, 0, IMG_W_MAX, IMG_W_MAX, 4, 4);

                int leftValue = (IMG_W_MAX - tmp_pixmap.width()) / 2;
                int topValue = (IMG_W_MAX - tmp_pixmap.height()) / 2;

                painter.setClipPath(path);
                painter.drawPixmap(leftValue, topValue, tmp_pixmap);
                painter.end();

                this->lb_albumThumb->setPixmap(pixmapIMG);
            }
            else{
                this->filedownloader->setImageURL(QUrl(this->cd_img_change));
            }
        }
        else{
            this->filedownloader->setImageURL(QUrl(tmp_imgURL));
        }      //j220903 twitter
    }
    else{

        if(this->playType == playTypes::VIDEO){
            QImage image;
            image.load(":/images/def_video_200.png");

            QPixmap pixmapIMG = QPixmap(QSize(IMG_W_MAX, IMG_W_MAX));
            pixmapIMG.fill(Qt::black);

            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap.scaledToHeight(IMG_W_MAX, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, IMG_W_MAX, IMG_W_MAX, 4, 4);

            int leftValue = (IMG_W_MAX - tmp_pixmap.width()) / 2;
            int topValue = (IMG_W_MAX - tmp_pixmap.height()) / 2;

            painter.setClipPath(path);
            painter.drawPixmap(leftValue, topValue, tmp_pixmap);
            painter.end();

            this->lb_albumThumb->setPixmap(pixmapIMG);

            this->pixmap_albumImg = GSCommon::getUIPixmapImg(":/images/def_video_200.png");
        }
        else if(this->playType == playTypes::RADIO){
            QImage image;
            image.load(":/images/radio_thumd_play_75.png");

            QPixmap pixmapIMG = QPixmap(QSize(IMG_W_MAX, IMG_W_MAX));
            pixmapIMG.fill(Qt::transparent);

            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(IMG_W_MAX, IMG_W_MAX, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, IMG_W_MAX, IMG_W_MAX, 4, 4);

            int leftValue = (IMG_W_MAX - tmp_pixmap.width()) / 2;
            int topValue = (IMG_W_MAX - tmp_pixmap.height()) / 2;

            painter.setClipPath(path);
            painter.drawPixmap(leftValue, topValue, tmp_pixmap);
            painter.end();

            this->lb_albumThumb->setPixmap(pixmapIMG);

            this->pixmap_albumImg = GSCommon::getUIPixmapImg(":/images/radio_thumd_play_75.png");

            // QT wrap 문제로 인한 문자열 교체
            QString radio_name = this->titleName;
            radio_name = radio_name.replace("_"," ");
            this->lb_albumThumbText->setText(radio_name);
            this->lb_albumThumbText->show();
        }
        else if(this->playType == playTypes::CD){
            QImage image;
            image.load(":/images/cd/cd_thum_60x2.png");

            QPixmap pixmapIMG = QPixmap(QSize(IMG_W_MAX, IMG_W_MAX));
            pixmapIMG.fill(Qt::transparent);

            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(IMG_W_MAX, IMG_W_MAX, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, IMG_W_MAX, IMG_W_MAX, 4, 4);

            int leftValue = (IMG_W_MAX - tmp_pixmap.width()) / 2;
            int topValue = (IMG_W_MAX - tmp_pixmap.height()) / 2;

            painter.setClipPath(path);
            painter.drawPixmap(leftValue, topValue, tmp_pixmap);
            painter.end();

            // 앨범 이미지
            this->lb_albumThumb->setPixmap(pixmapIMG);       //j220903 twitter

            this->pixmap_albumImg = GSCommon::getUIPixmapImg(":/images/cd/cd_thum_60x2.png");
        }
        else{
            QImage image;
            image.load(":/images/def_mus_75.png");

            QPixmap pixmapIMG = QPixmap(QSize(IMG_W_MAX, IMG_W_MAX));
            pixmapIMG.fill(Qt::transparent);

            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(IMG_W_MAX, IMG_W_MAX, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, IMG_W_MAX, IMG_W_MAX, 4, 4);

            int leftValue = (IMG_W_MAX - tmp_pixmap.width()) / 2;
            int topValue = (IMG_W_MAX - tmp_pixmap.height()) / 2;

            painter.setClipPath(path);
            painter.drawPixmap(leftValue, topValue, tmp_pixmap);
            painter.end();

            // 앨범 이미지
            this->lb_albumThumb->setPixmap(pixmapIMG);

            this->pixmap_albumImg = GSCommon::getUIPixmapImg(":/images/def_mus_75.png");
        }

        if(this->playType == "INOUT"){ //bj230518
            QPixmap pixmap_icon(10, 10);
            pixmap_icon.fill(Qt::transparent);
            this->lb_icon_playType->setPixmap(pixmap_icon);

            this->pixmap_albumImg = new QPixmap(IMG_W_MAX,IMG_W_MAX);
            pixmap_albumImg->fill(Qt::transparent); // 투명색으로 채우기

            // QPainter를 사용하여 그라데이션 효과 추가
            QPainter painter(pixmap_albumImg);
            QLinearGradient gradient;
            gradient.setStart(0, pixmap_albumImg->height());         // 그라데이션 시작점 (왼쪽 아래)
            gradient.setFinalStop(pixmap_albumImg->width(), 0);      // 그라데이션 끝점 (오른쪽 위)
            gradient.setColorAt(0, QColor(0, 0, 0));                    // 그라데이션 시작 색상 (검정색)
            gradient.setColorAt(1, QColor(38, 38, 46, 145));            // 그라데이션 중간 색상 (회색, 알파 145)
            painter.setBrush(gradient);
            painter.drawRect(0, 0, pixmap_albumImg->width(), pixmap_albumImg->height());


            if(this->titleName == "LINE IN"){

                QImage centerImage(":/images/inout_line_in_x3.png");  // 중앙에 그릴 이미지 로드
                QImage scaledImage = centerImage.scaled(45, 45, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                int x = (pixmap_albumImg->width() - scaledImage.width()) / 2;
                int y = (pixmap_albumImg->height() - scaledImage.height()) / 2;

                painter.drawImage(x, y, scaledImage);

            }
            else if(this->titleName == "COAXIAL IN"){
                QImage centerImage(":/images/inout_coax_x3.png");  // 중앙에 그릴 이미지 로드
                QImage scaledImage = centerImage.scaled(45, 45, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                int x = (pixmap_albumImg->width() - scaledImage.width()) / 2;
                int y = (pixmap_albumImg->height() - scaledImage.height()) / 2;

                painter.drawImage(x, y, scaledImage);
            }
            else if(this->titleName == "OPTICAL IN"){
                QImage centerImage(":/images/inout_opt_x3.png");  // 중앙에 그릴 이미지 로드
                QImage scaledImage = centerImage.scaled(45, 45, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                int x = (pixmap_albumImg->width() - scaledImage.width()) / 2;
                int y = (pixmap_albumImg->height() - scaledImage.height()) / 2;

                painter.drawImage(x, y, scaledImage);
            }
            else if(this->titleName == "USB IN"){
                QImage centerImage(":/images/inout_USB_IN_x3.png");  // 중앙에 그릴 이미지 로드
                QImage scaledImage = centerImage.scaled(45, 45, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                int x = (pixmap_albumImg->width() - scaledImage.width()) / 2;
                int y = (pixmap_albumImg->height() - scaledImage.height()) / 2;

                painter.drawImage(x, y, scaledImage);
            }
            else if(this->titleName == "ARC IN" || this->titleName == "eARC IN"){
                QImage centerImage(":/images/inout_HDMI_ARC_IN_x3.png");  // 중앙에 그릴 이미지 로드
                QImage scaledImage = centerImage.scaled(45, 45, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                int x = (pixmap_albumImg->width() - scaledImage.width()) / 2;
                int y = (pixmap_albumImg->height() - scaledImage.height()) / 2;

                painter.drawImage(x, y, scaledImage);
            }
            else if(this->titleName == "AES/EBU IN"){
                QImage centerImage(":/images/inout_balanced_x2.png");  // 중앙에 그릴 이미지 로드
                QImage scaledImage = centerImage.scaled(45, 45, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                int height = scaledImage.height();
                height -= 5;

                int x = (pixmap_albumImg->width() - scaledImage.width()) / 2;
                int y = (pixmap_albumImg->height() - height) / 2;

                painter.drawImage(x, y, scaledImage);
            }


            this->lb_albumThumb->setPixmap(*pixmap_albumImg);
        }


        // 배경
        this->setBackgroundAlbumImg();
    }
}


/**
 * @brief SectionBottom::setUIDataPlayIcon 재생 관련 아이콘 세팅
 */
void SectionBottom::setUIDataPlayIcon(){

    // if(bottomInfo_flag ) return;//c220709

    this->setCurrPlayValue(this->curPosition);

    if(this->isPlaying == true){
        QImage img_pause;
        QPixmap *pixmap_icon_pause = new QPixmap();
        if(img_pause.load(":/images/SectionBottom/play_pause_icon.png")){
            *pixmap_icon_pause = QPixmap::fromImage(img_pause);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_pause = pixmap_icon_pause->scaled(img_pause.width(), img_pause.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->lb_pause_img->setPixmap(*pixmap_icon_pause);
    }
    else{
        QImage img_pause;
        QPixmap *pixmap_icon_pause = new QPixmap();
        if(img_pause.load(":/images/SectionBottom/play_icon.png")){
            *pixmap_icon_pause = QPixmap::fromImage(img_pause);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_pause = pixmap_icon_pause->scaled(img_pause.width(), img_pause.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->lb_pause_img->setPixmap(*pixmap_icon_pause);
    }

    if(this->repeatMode == 0){
        QImage img_rep;
        QPixmap *pixmap_icon_rep = new QPixmap();
        if(img_rep.load(playIconPath::play_rep1_icon)){
            *pixmap_icon_rep = QPixmap::fromImage(img_rep);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_rep = pixmap_icon_rep->scaled(img_rep.width(), img_rep.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->lb_rep_img->setPixmap(*pixmap_icon_rep);
    }
    else if(this->repeatMode == 1){
        QImage img_rep;
        QPixmap *pixmap_icon_rep = new QPixmap();
        if(img_rep.load(playIconPath::play_rep3_icon)){
            *pixmap_icon_rep = QPixmap::fromImage(img_rep);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_rep = pixmap_icon_rep->scaled(img_rep.width(), img_rep.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->lb_rep_img->setPixmap(*pixmap_icon_rep);
    }
    else if(this->repeatMode == 2){
        QImage img_rep;
        QPixmap *pixmap_icon_rep = new QPixmap();
        if(img_rep.load(playIconPath::play_rep2_icon)){
            *pixmap_icon_rep = QPixmap::fromImage(img_rep);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_rep = pixmap_icon_rep->scaled(img_rep.width(), img_rep.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->lb_rep_img->setPixmap(*pixmap_icon_rep);
    }

    if(this->shuffleMode == 0){
        QImage img_shu;
        QPixmap *pixmap_icon_shu = new QPixmap();
        if(img_shu.load(playIconPath::play_shu2_icon)){
            *pixmap_icon_shu = QPixmap::fromImage(img_shu);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_shu = pixmap_icon_shu->scaled(img_shu.width(), img_shu.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->lb_shu_img->setPixmap(*pixmap_icon_shu);
    }
    else if(this->shuffleMode == 1){
        QImage img_shu;
        QPixmap *pixmap_icon_shu = new QPixmap();
        if(img_shu.load(playIconPath::play_shu1_icon)){
            *pixmap_icon_shu = QPixmap::fromImage(img_shu);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_shu = pixmap_icon_shu->scaled(img_shu.width(), img_shu.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->lb_shu_img->setPixmap(*pixmap_icon_shu);
    }

    // 항상 disconnect 한 다음 연결 해주기
    this->btn_rep->setCursor(Qt::ArrowCursor);
    this->btn_shu->setCursor(Qt::ArrowCursor);

    //if(slot_sliderClick_flag){
    disconnect(btn_rep, SIGNAL(clicked()), this, SLOT(clickedBtn()));
    disconnect(btn_shu, SIGNAL(clicked()), this, SLOT(clickedBtn()));
    //}


    if(this->playType==playTypes::Music){
        this->btn_rep->setCursor(Qt::PointingHandCursor);
        this->btn_shu->setCursor(Qt::PointingHandCursor);
        connect(btn_rep, SIGNAL(clicked()), this, SLOT(clickedBtn()));
        connect(btn_shu, SIGNAL(clicked()), this, SLOT(clickedBtn()));
    }
    else if(this->playType==playTypes::VIDEO){
        QImage img_rep;
        QPixmap *pixmap_icon_rep = new QPixmap();
        if(img_rep.load(playIconPath::play_rep_icon_off)){
            *pixmap_icon_rep = QPixmap::fromImage(img_rep);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_rep = pixmap_icon_rep->scaled(img_rep.width(), img_rep.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->lb_rep_img->setPixmap(*pixmap_icon_rep);

        QImage img_shu;
        QPixmap *pixmap_icon_shu = new QPixmap();
        if(img_shu.load(playIconPath::play_shu_icon_off)){
            *pixmap_icon_shu = QPixmap::fromImage(img_shu);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_shu = pixmap_icon_shu->scaled(img_shu.width(), img_shu.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->lb_shu_img->setPixmap(*pixmap_icon_shu);
    }
    else if(this->playType==playTypes::RADIO || this->playType==playTypes::ROSE_RADIO){
        QImage img_rep;
        QPixmap *pixmap_icon_rep = new QPixmap();
        if(img_rep.load(playIconPath::play_rep_icon_off)){
            *pixmap_icon_rep = QPixmap::fromImage(img_rep);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_rep = pixmap_icon_rep->scaled(img_rep.width(), img_rep.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->lb_rep_img->setPixmap(*pixmap_icon_rep);

        QImage img_shu;
        QPixmap *pixmap_icon_shu = new QPixmap();
        if(img_shu.load(playIconPath::play_shu_icon_off)){
            *pixmap_icon_shu = QPixmap::fromImage(img_shu);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_shu = pixmap_icon_shu->scaled(img_shu.width(), img_shu.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->lb_shu_img->setPixmap(*pixmap_icon_shu);
    }
    else if(this->playType==playTypes::YOUTUBE){
        this->btn_rep->setCursor(Qt::PointingHandCursor);
        this->btn_shu->setCursor(Qt::PointingHandCursor);
        connect(btn_rep, SIGNAL(clicked()), this, SLOT(clickedBtn()));
        connect(btn_shu, SIGNAL(clicked()), this, SLOT(clickedBtn()));
    }
    else if(this->playType==playTypes::POD){
        QImage img_rep;
        QPixmap *pixmap_icon_rep = new QPixmap();
        if(img_rep.load(playIconPath::play_rep_icon_off)){
            *pixmap_icon_rep = QPixmap::fromImage(img_rep);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_rep = pixmap_icon_rep->scaled(img_rep.width(), img_rep.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->lb_rep_img->setPixmap(*pixmap_icon_rep);

        QImage img_shu;
        QPixmap *pixmap_icon_shu = new QPixmap();
        if(img_shu.load(playIconPath::play_shu_icon_off)){
            *pixmap_icon_shu = QPixmap::fromImage(img_shu);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_shu = pixmap_icon_shu->scaled(img_shu.width(), img_shu.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->lb_shu_img->setPixmap(*pixmap_icon_shu);
    }
    else if(this->playType==playTypes::TIDAL || this->playType==playTypes::TIDAL_VIDEO){
        this->btn_rep->setCursor(Qt::PointingHandCursor);
        this->btn_shu->setCursor(Qt::PointingHandCursor);
        connect(btn_rep, SIGNAL(clicked()), this, SLOT(clickedBtn()));
        connect(btn_shu, SIGNAL(clicked()), this, SLOT(clickedBtn()));
    }
    else if(this->playType==playTypes::BUGS || this->playType==playTypes::BUGS_MV){
        this->btn_rep->setCursor(Qt::PointingHandCursor);
        this->btn_shu->setCursor(Qt::PointingHandCursor);
        connect(btn_rep, SIGNAL(clicked()), this, SLOT(clickedBtn()));
        connect(btn_shu, SIGNAL(clicked()), this, SLOT(clickedBtn()));
    }
    else if(this->playType==playTypes::QOBUZ){
        this->btn_rep->setCursor(Qt::PointingHandCursor);
        this->btn_shu->setCursor(Qt::PointingHandCursor);
        connect(btn_rep, SIGNAL(clicked()), this, SLOT(clickedBtn()));
        connect(btn_shu, SIGNAL(clicked()), this, SLOT(clickedBtn()));
    }
    else if(this->playType==playTypes::APPLE){
        this->btn_rep->setCursor(Qt::PointingHandCursor);
        this->btn_shu->setCursor(Qt::PointingHandCursor);
        connect(btn_rep, SIGNAL(clicked()), this, SLOT(clickedBtn()));
        connect(btn_shu, SIGNAL(clicked()), this, SLOT(clickedBtn()));
    }
}


/**
 * @brief getData 데이터 반환
 * @return
 */
QJsonObject SectionBottom::getData(){

    QJsonObject tmp_data;
    tmp_data["playType"] = this->playType;
    tmp_data["titleName"] = this->titleName;
    tmp_data["artistName"] = this->artistName;
    tmp_data["duration"] = this->duration;
    tmp_data["curPosition"] = this->curPosition;
    tmp_data["isPlaying"] = this->isPlaying;
    tmp_data["repeatMode"] = this->repeatMode;
    tmp_data["shuffleMode"] = this->shuffleMode;

    if(this->playType==playTypes::CD){  //j220903 twitter
        if( this->cd_img_change.isEmpty()){
            tmp_data["thumbnail"] = this->thumbnail;
        }
        else{
            qDebug() << this->cd_img_change;
            tmp_data["thumbnail"] = this->cd_img_change;
        }
    }
    else{
        tmp_data["thumbnail"] = this->thumbnail;
    }

    return this->data_jsonObject;
}


/**
 * @brief SectionBottom::getUIBtn
 * @param p_url
 * @param p_property
 * @return
 */
QPushButton* SectionBottom::getUIBtn(QString p_url, QString p_property){

    QPushButton *btn_icon = new QPushButton();
    btn_icon->setObjectName("bottom_btn");
    btn_icon->setStyleSheet("#bottom_btn { background-color:rgba(0,0,0,0);background-image:url('" + p_url + "');background-repeat:no-repeat;background-position:center center; } ");
    btn_icon->setFixedSize(70,95);
    btn_icon->setCursor(Qt::PointingHandCursor);
    btn_icon->setProperty("type", p_property);
    //btn_icon->setFixedSize(40,40);
    return btn_icon;
}


/**
 * @brief SectionBottom::loadImage [SLOT] 썸네일 이미지 세팅
 */
void SectionBottom::slot_loadImage(){

    //bool flagLoad = this->pixmap_albumImg->loadFromData(this->filedownloader->downloadedData());

    QImage image;
    bool flagLoad = image.loadFromData(this->filedownloader->downloadedData());

    if(flagLoad){

        /*this->pixmap_albumImg->fill(Qt::transparent);

        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(image);
        tmp_pixmap = tmp_pixmap.scaled(IMG_W_MAX, IMG_W_MAX, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QPainter painter (this->pixmap_albumImg);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path = QPainterPath();
        path.addRoundedRect(0, 0, IMG_W_MAX, IMG_W_MAX, 4, 4);

        int leftValue = (IMG_W_MAX - tmp_pixmap.width()) / 2;
        int topValue = (IMG_W_MAX - tmp_pixmap.height()) / 2;

        if(leftValue < 0 || topValue < 0){
            this->pixmap_albumImg->fill(Qt::black);
        }

        painter.setClipPath(path);
        painter.drawPixmap(leftValue, topValue, tmp_pixmap);
        painter.end();

        this->lb_albumThumb->setPixmap(*this->pixmap_albumImg);*/

        QPixmap pixmapIMG = QPixmap(QSize(IMG_W_MAX, IMG_W_MAX));

        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(image);
        if(tmp_pixmap.width() > tmp_pixmap.height()){
            tmp_pixmap = tmp_pixmap.scaledToWidth(IMG_W_MAX, Qt::SmoothTransformation);
            pixmapIMG.fill(Qt::black);
        }
        else if(tmp_pixmap.height() > tmp_pixmap.width()){
            tmp_pixmap = tmp_pixmap.scaledToHeight(IMG_W_MAX, Qt::SmoothTransformation);
            pixmapIMG.fill(Qt::black);
        }
        else{
            tmp_pixmap = tmp_pixmap.scaled(IMG_W_MAX, IMG_W_MAX, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            pixmapIMG.fill(Qt::transparent);
        }

        QPainter painter (&pixmapIMG);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path = QPainterPath();
        path.addRoundedRect(0, 0, IMG_W_MAX, IMG_W_MAX, 4, 4);

        int leftValue = (IMG_W_MAX - tmp_pixmap.width()) / 2;
        int topValue = (IMG_W_MAX - tmp_pixmap.height()) / 2;

        painter.setClipPath(path);
        painter.drawPixmap(leftValue, topValue, tmp_pixmap);
        painter.end();

        this->lb_albumThumb->setPixmap(pixmapIMG);

        this->pixmap_albumImg->fill(Qt::transparent);

        QPainter painterBig (this->pixmap_albumImg);
        painterBig.setRenderHint(QPainter::Antialiasing, true);
        painterBig.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painterBig.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath pathBig = QPainterPath();
        pathBig.addRoundedRect(0, 0, IMG_W_MAX, IMG_W_MAX, 4, 4);

        if(leftValue < 0 || topValue < 0){
            this->pixmap_albumImg->fill(Qt::black);
        }

        painterBig.setClipPath(path);
        painterBig.drawPixmap(leftValue, topValue, tmp_pixmap);
        painterBig.end();
    }
    else{

        // 앨범 이미지
        this->thumbnail_realHttp = "";
        if(this->playType == playTypes::CD){

            QImage image;
            image.load(":/images/cd/cd_thum_60x2.png");

            /*this->pixmap_albumImg->fill(Qt::transparent);

            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(IMG_W_MAX, IMG_W_MAX, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

            QPainter painter (this->pixmap_albumImg);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, IMG_W_MAX, IMG_W_MAX, 4, 4);

            int leftValue = (IMG_W_MAX - tmp_pixmap.width()) / 2;
            int topValue = (IMG_W_MAX - tmp_pixmap.height()) / 2;

            painter.setClipPath(path);
            painter.drawPixmap(leftValue, topValue, tmp_pixmap);
            painter.end();

            this->lb_albumThumb->setPixmap(*this->pixmap_albumImg);*/

            QPixmap pixmapIMG = QPixmap(QSize(IMG_W_MAX, IMG_W_MAX));
            pixmapIMG.fill(Qt::transparent);

            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(IMG_W_MAX, IMG_W_MAX, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, IMG_W_MAX, IMG_W_MAX, 4, 4);

            int leftValue = (IMG_W_MAX - tmp_pixmap.width()) / 2;
            int topValue = (IMG_W_MAX - tmp_pixmap.height()) / 2;

            painter.setClipPath(path);
            painter.drawPixmap(leftValue, topValue, tmp_pixmap);
            painter.end();

            this->lb_albumThumb->setPixmap(pixmapIMG);

            this->pixmap_albumImg->fill(Qt::transparent);

            QPainter painterBig (this->pixmap_albumImg);
            painterBig.setRenderHint(QPainter::Antialiasing, true);
            painterBig.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painterBig.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath pathBig = QPainterPath();
            pathBig.addRoundedRect(0, 0, IMG_W_MAX, IMG_W_MAX, 4, 4);

            if(leftValue < 0 || topValue < 0){
                this->pixmap_albumImg->fill(Qt::black);
            }

            painterBig.setClipPath(path);
            painterBig.drawPixmap(leftValue, topValue, tmp_pixmap);
            painterBig.end();
        }
        else{

            QImage image;
            image.load(":/images/def_mus_75.png");

            QPixmap pixmapIMG = QPixmap(QSize(IMG_W_MAX, IMG_W_MAX));
            pixmapIMG.fill(Qt::transparent);

            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(IMG_W_MAX, IMG_W_MAX, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, IMG_W_MAX, IMG_W_MAX, 4, 4);

            int leftValue = (IMG_W_MAX - tmp_pixmap.width()) / 2;
            int topValue = (IMG_W_MAX - tmp_pixmap.height()) / 2;

            painter.setClipPath(path);
            painter.drawPixmap(leftValue, topValue, tmp_pixmap);
            painter.end();

            this->lb_albumThumb->setPixmap(pixmapIMG);

            this->pixmap_albumImg = GSCommon::getUIPixmapImg(":/images/def_mus_75.png");
            //this->lb_albumThumb->setPixmap(*this->pixmap_albumImg);
        }
    }

    // 배경
    this->setBackgroundAlbumImg();
}


/**
 * @brief SectionBottom::slot_showPlayFullScreen [SLOT] 재생화면 FullScreen
 */
void SectionBottom::slot_showPlayFullScreen(){

    QJsonObject tmp_data = getData();
    tmp_data.insert(KEY_PAGE_CODE, PAGECODE_ETC_PLAYFULLSCREEN);
    emit signal_showPlayFullScreen(tmp_data);
}


/**
 * @brief SectionBottom::clickedBtn [SLOT]
 */
void SectionBottom::clickedBtn(){
    print_debug();
    if(global.power_sleepMode_flag) {
        print_debug();
        emit this->linker->signal_checkQueue(24, "");
        return;
    }
    else{
        ToastMsg::delay(this,"", tr("delay"), 500);//c230311
    }
    qDebug() << "global.islistOpenFlag = " << global.islistOpenFlag;
    qDebug() << "global.queueTimerFlag = " << global.queueTimerFlag;
    //qDebug() << "section_bottom_mouse_trigger_menu_flag = " << section_bottom_mouse_trigger_menu_flag;
    qDebug() << "this->curPosition = " << this->curPosition;
    qDebug() << " global.current_position = " <<  global.current_position;
    qDebug() << " global.current_duration  -term_position = " <<  global.current_duration  -term_position;
    qDebug() << "-------------------------------------------------------------";

    qDebug() << "global.desktopPlayerMode= " << global.desktopPlayerMode;

    QString tmp_type = sender()->property("type").toString();

    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    if(tmp_type=="rep"){
        /*
                global.music_playlist->setPlaybackMode(QMediaPlaylist::Loop);//c220609
                global.music_playlist->next();
                global.music_player->play();
            }else if (global.repeatMode == 0 && state == QMediaPlayer::State::StoppedState){
                global.music_playlist->setPlaybackMode(QMediaPlaylist::Sequential);//c220609
                global.music_playlist->next();
                global.music_player->play();
            }else if (global.repeatMode == 1 && state == QMediaPlayer::State::StoppedState){
                global.music_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);//c220609
 * */
        if(!global.desktopPlayerMode){
            tmp_json = QJsonObject();
            tmp_json.insert("currentPlayState","24");
            tmp_json.insert("roseToken",global.device.getDeviceRoseToken());

            QString Url = QString("http://%1:%2/current_play_state").arg(global.device.getDeviceIP(), global.port);
            network->request(HTTP_REMOTE_REP
                             , Url
                             , tmp_json
                             , true);
        }
        else{
            if(global.repeatMode == 1){
                global.repeatMode = 2;
                global.music_playlist->setPlaybackMode(QMediaPlaylist::Loop);//c220609
            }
            else if(global.repeatMode == 2){
                global.repeatMode = 0;
                global.music_playlist->setPlaybackMode(QMediaPlaylist::Sequential);//c220609
            }
            else if(global.repeatMode == 0){
                global.repeatMode = 1;
                global.music_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);//c220609
            }
        }
    }
    else if(tmp_type=="back"){
        /*
        if(global.islistOpenFlag && global.queueTimerFlag ){
            print_debug();
            emit this->linker->signal_checkQueue(7, "");
            return;
        }
        */
        if(!global.desktopPlayerMode){
            if(this->playType==playTypes::RADIO || this->playType == playTypes::FM_TUNER || this->playType==playTypes::ROSE_RADIO){
                print_debug();
            }
            else{
                print_debug();
                tmp_json = QJsonObject();
                tmp_json.insert("currentPlayState","19");
                tmp_json.insert("roseToken",global.device.getDeviceRoseToken());

                QString Url = QString("http://%1:%2/current_play_state").arg(global.device.getDeviceIP(),global.port);
                network->request(HTTP_REMOTE_PLAYBACK
                                 , Url
                                 , tmp_json
                                 , true);

                //network->request(HTTP_DEVICE_GET_CURRENT_STATE_PLAY, QString("http://%1:%2/current_play_state")
                //                 .arg(global.device.getDeviceIP(),global.port), tmp_json, true);
            }
        }
        else{
            global.music_playlist->previous();
            global.music_player->play();
        }
    }
    else if(tmp_type=="pause"){//HTTP_REMOTE_PLAYNEXT
        print_debug();qDebug() << "this->curPosition = " << this->curPosition;
        /*
        if(global.islistOpenFlag && global.queueTimerFlag ){
            if(global.isPlaying_stop_flag){

                print_debug();
               // emit this->linker->signal_fileInfo_filePath_stop();
                emit this->linker->signal_checkQueue(7, "");
                return;
            }
        }
        */
        /*
        //if(this->curPosition < 0 || global.current_position >= (global.current_duration  -term_position)){
        if(this->curPosition < -1 ||  (global.current_position >= (global.current_duration  -term_position)&& global.current_duration > 0)){
            print_debug();
            tmp_json = QJsonObject();
            tmp_json.insert("currentPlayState","18");
            tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
            network->request(HTTP_REMOTE_PLAYNEXT, QString("http://%1:%2/current_play_state")
                             .arg(global.device.getDeviceIP(),global.port), tmp_json, true);

        }else{//HTTP_REMOTE_PLAYPAUSE
            print_debug();
            if(global.desktopPlayerMode && QMediaPlayer::PlayingState == global.music_player->state()){
                global.music_player->pause();
            }else if(global.desktopPlayerMode){//}) && QMediaPlayer::PausedState == global.music_player->state()){
                global.music_player->play();
            }
            if(!global.desktopPlayerMode){
                tmp_json = QJsonObject();
                tmp_json.insert("currentPlayState","17");
                tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
                network->request(HTTP_REMOTE_PLAYPAUSE, QString("http://%1:%2/current_play_state")
                                 .arg(global.device.getDeviceIP(),global.port), tmp_json, true);
            }


        }*/

        if(global.desktopPlayerMode && QMediaPlayer::PlayingState == global.music_player->state()){
            global.music_player->pause();
        }
        else if(global.desktopPlayerMode){//}) && QMediaPlayer::PausedState == global.music_player->state()){
            global.music_player->play();
        }
        if(this->ui_state == -1){//c230327
            print_debug();
            qDebug()<<"this->ui_state = " << this->ui_state;
            emit linker->signal_connect_reset_notice();
            return;
        }
        if(!global.desktopPlayerMode){
            print_debug();
            tmp_json = QJsonObject();
            tmp_json.insert("currentPlayState","17");
            tmp_json.insert("roseToken",global.device.getDeviceRoseToken());

            QString Url = QString("http://%1:%2/current_play_state").arg(global.device.getDeviceIP(),global.port);
            network->request(HTTP_REMOTE_PLAYPAUSE
                             , Url
                             , tmp_json
                             , true);
        }

    }
    else if(tmp_type=="next"){
        if(!global.desktopPlayerMode){
            if(this->playType==playTypes::RADIO || this->playType == playTypes::FM_TUNER || this->playType==playTypes::ROSE_RADIO){
                print_debug();
            }
            else{
                tmp_json = QJsonObject();
                tmp_json.insert("currentPlayState","18");
                tmp_json.insert("roseToken",global.device.getDeviceRoseToken());

                QString Url = QString("http://%1:%2/current_play_state").arg(global.device.getDeviceIP(), global.port);
                network->request(HTTP_REMOTE_PLAYBACK
                                 , Url
                                 , tmp_json
                                 , true);
            }
        }
        else{
            global.music_playlist->next();
            global.music_player->play();
        }
    }
    else if(tmp_type=="shu"){
        if(!global.desktopPlayerMode){
            tmp_json = QJsonObject();
            tmp_json.insert("currentPlayState", "25");
            tmp_json.insert("roseToken",global.device.getDeviceRoseToken());

            QString Url = QString("http://%1:%2/current_play_state").arg(global.device.getDeviceIP(), global.port);
            network->request(HTTP_REMOTE_SHU
                             , Url
                             , tmp_json
                             , true);
        }
        else{
            if(global.shuffleMode == 0){
                global.shuffleMode = 1;
                global.music_playlist->setPlaybackMode(QMediaPlaylist::Random);//c220609
            }
            else if(global.shuffleMode == 1){
                global.shuffleMode = 0;
                if(global.repeatMode == 1){
                    // global.repeatMode = 2;
                    global.music_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);//c220609
                }
                else if(global.repeatMode == 2){
                    // global.repeatMode = 0;
                    global.music_playlist->setPlaybackMode(QMediaPlaylist::Loop);//c220609
                }
                else if(global.repeatMode == 0){
                    //global.repeatMode = 1;
                    global.music_playlist->setPlaybackMode(QMediaPlaylist::Sequential);//c220609
                }
            }
        }
    }
    else if(tmp_type=="list"){
        if(global.queuelist_mouse_trigger_flag ){//c220411    && global.queuelist_mouse_trigger_flag
            QImage img_list;
            QPixmap *pixmap_icon_list = new QPixmap();

            if(this->flagSelectedQueue==true){
                this->flagSelectedQueue = false;

                if(img_list.load(playIconPath::play_list_icon)){
                    *pixmap_icon_list = QPixmap::fromImage(img_list);                                        //이미지를 버퍼에 옮긴다.
                    *pixmap_icon_list = pixmap_icon_list->scaled(img_list.width(), img_list.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }
            }
            else{
                this->flagSelectedQueue = true;

                if(img_list.load(playIconPath::play_list_icon_on)){
                    *pixmap_icon_list = QPixmap::fromImage(img_list);                                        //이미지를 버퍼에 옮긴다.
                    *pixmap_icon_list = pixmap_icon_list->scaled(img_list.width(), img_list.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }
            }
            this->lb_list_img->setPixmap(*pixmap_icon_list);

            emit clickedQueuePlayList(this->flagSelectedQueue);
        }
        else{
            emit this->linker->signal_checkQueue(22, "");
        }
    }
    else if(tmp_type=="sound"){
        QImage img_sound;
        QPixmap *pixmap_icon_sound = new QPixmap();
        QString tmp_style = this->lb_sound_text->styleSheet();

        if(this->flagMuteState == true){
            if(this->flagSelectedVolumn == true){
                this->flagSelectedVolumn = false;

                if(img_sound.load(playIconPath::play_mute_icon)){
                    *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
                    *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }

                tmp_style.replace("color: #B18658;", "color: #FFFFFF;");
            }
            else{
                this->flagSelectedVolumn = true;

                if(img_sound.load(playIconPath::play_mute_icon_on)){
                    *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
                    *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }

                tmp_style.replace("color: #FFFFFF;", "color: #B18658;");
            }
        }
        else if(this->flagMuteState == false){
            if(this->flagSelectedVolumn == true){
                this->flagSelectedVolumn = false;

                if(img_sound.load(playIconPath::play_sound_s_icon_off)){
                    *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
                    *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }

                tmp_style.replace("color: #B18658;", "color: #FFFFFF;");
            }
            else{
                this->flagSelectedVolumn = true;

                if(img_sound.load(playIconPath::play_sound_s_icon_on)){
                    *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
                    *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }

                tmp_style.replace("color: #FFFFFF;", "color: #B18658;");
            }
        }

        this->lb_sound_img->setPixmap(*pixmap_icon_sound);
        this->lb_sound_text->setStyleSheet(tmp_style);

        emit clickedVolumn(this->flagSelectedVolumn);
        emit this->linker->signal_searchBarFocusChanged(false);//c220804
    }
}


void SectionBottom::slot_responseHttp_localDriverPlayer(const QJsonObject &p_jsonObject, const QPixmap pixmap){

    //print_debug();
    //global.desktopPlayerMode =  true;

    //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nSectionBottom::slot_responseHttp_localDriverPlayer--p_jsonObject=" << strJson  << "\n";

    //if(p_id==HTTP_DEVICE_GET_CURRENT_STATE_PLAY){
    this->setDataSectionBottom(p_jsonObject);
    *this->pixmap_albumImg = pixmap;
    this->lb_albumThumb->setPixmap(*this->pixmap_albumImg);
    // 배경
    this->setBackgroundAlbumImg();
    //}
    sender()->deleteLater();
}


void SectionBottom::slot_responseHttp_state(const int &p_id, const QJsonObject &p_jsonObject){

    //print_debug();
    //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nSectionBottom::slot_responseHttp--p_jsonObject=" << strJson  << "\n";

    if(p_id==HTTP_DEVICE_GET_CURRENT_STATE_PLAY){

    }else if(p_id==HTTP_REMOTE_PLAYPAUSE){
        //ToastMsg::show(this, "", tr("Play/Pause icon image needs to be changed"));

    }else if(p_id==HTTP_REMOTE_PLAYNEXT){
        //ToastMsg::show(this, "", tr("Need to change to the next song data"));

    }else if(p_id==HTTP_REMOTE_PLAYBACK){
        //ToastMsg::show(this, "", tr("Need to change to the previous song data"));

    }else if(p_id==HTTP_REMOTE_REP){
        //ToastMsg::show(this, "", tr("REP icon image needs to be changed"));

    }else if(p_id==HTTP_REMOTE_SHU){
        //ToastMsg::show(this, "", tr("SHUPPLE icon image needs to be changed"));
        /*}else if(p_id==556){
        print_debug();
        if(p_jsonObject.contains("data")){
            QJsonObject tmp_data = p_jsonObject["data"].toObject();
            if(tmp_data.contains("isPlaying")){
                this->isPlaying = tmp_data["isPlaying"].toBool();
                global.isPlaying = this->isPlaying;
                if(this->isPlaying){
                    print_debug();

                    section_bottom_mouse_trigger_menu_flag = false;
                }
            }
        }
*/
        //ToastMsg::show(this, "", tr("SHUPPLE icon image needs to be changed"));
    }
    //this->timer->start();
    sender()->deleteLater();
}


/**
 * @brief SectionBottom::responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void SectionBottom::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nSectionBottom::slot_responseHttp--p_jsonObject=" << strJson  << "\n";

    if(p_id==HTTP_DEVICE_GET_CURRENT_STATE_PLAY){

        this->setDataSectionBottom(p_jsonObject);
    }
    else if(p_id==HTTP_REMOTE_PLAYPAUSE){

    }
    else if(p_id==HTTP_REMOTE_PLAYNEXT){

    }
    else if(p_id==HTTP_REMOTE_PLAYBACK){

    }
    else if(p_id==HTTP_REMOTE_REP){

    }
    else if(p_id==HTTP_REMOTE_SHU){

    }
    else if(p_id==HTTP_REMOTE_MUTE){
        int mute = ProcJsonEasy::getInt(p_jsonObject, "mute");

        this->flagMuteState = (mute == 1) ? true : false;
        global.setMuteState = this->flagMuteState;

        QImage img_sound;
        QPixmap *pixmap_icon_sound = new QPixmap();

        QString tmp_style = this->lb_sound_text->styleSheet();

        if(this->flagMuteState == true){
            if(this->flagSelectedVolumn == true){
                if(img_sound.load(playIconPath::play_mute_icon_on)){
                    *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
                    *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }

                tmp_style.replace("color: #FFFFFF;", "color: #B18658;");
            }
            else{
                if(img_sound.load(playIconPath::play_mute_icon)){
                    *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
                    *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }

                tmp_style.replace("color: #B18658;", "color: #FFFFFF;");
            }
        }
        else if(this->flagMuteState == false){
            if(this->flagSelectedVolumn == true){
                if(img_sound.load(playIconPath::play_sound_s_icon_on)){
                    *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
                    *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }

                tmp_style.replace("color: #FFFFFF;", "color: #B18658;");
            }
            else{
                if(img_sound.load(playIconPath::play_sound_s_icon_off)){
                    *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
                    *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }

                tmp_style.replace("color: #B18658;", "color: #FFFFFF;");
            }
        }

        this->lb_sound_img->setPixmap(*pixmap_icon_sound);
        this->lb_sound_text->setStyleSheet(tmp_style);
    }
    else if(p_id==HTTP_REMOTE_VOLUME){
        if(p_jsonObject.contains("volumeValue")){
            int change_val = p_jsonObject["volumeValue"].toInt();

            global.setVolumeSync = change_val;
            this->slot_volume_change(change_val);
        }
    }
    else if(p_id==HTTP_NETWORK_PLAY){
        print_debug();
        if(p_jsonObject.contains("code")){//cheon210812-iso
            QString tmp_code = p_jsonObject["code"].toString();
            bool tmp_codeOk = p_jsonObject["flagOk"].toBool();
            if(tmp_code == "CDPLAY" && tmp_codeOk){
                //qDebug() << "tmp_code-CDPLAYING---";
                //emit signal_isoplay(true);
                //global.isoflag = true;
            }
            else if(tmp_code == "G0000" && tmp_codeOk){
                print_debug();
                //emit signal_isoplay(false);
                //QTimer::singleShot(2000, this, SLOT(slot_iso_play()));
                //global.isoflag = false;
                ToastMsg::show(this, "", tr("CD PLAY has stoped. \nPlaese Select a song again to play CD PLAY."),3000);//cheon210812-iso

            }
        }
        global.Queue_track_count = 0;
    }

    sender()->deleteLater();
}



/**
 * @brief SectionBottom::setInitUIQueueIcon 재생목록 Queue 아이콘 UI 초기화
 */
void SectionBottom::setInitUIQueueIcon(){

    this->flagSelectedQueue = false;

    QImage img_list;
    QPixmap *pixmap_icon_list = new QPixmap();
    if(img_list.load(playIconPath::play_list_icon)){
        *pixmap_icon_list = QPixmap::fromImage(img_list);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_icon_list = pixmap_icon_list->scaled(img_list.width(), img_list.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    this->lb_list_img->setPixmap(*pixmap_icon_list);
}


/**
 * @brief SectionBottom::setInitUIVolumnIcon 볼륨 아이콘 UI 초기화
 */
void SectionBottom::setInitUIVolumnIcon(){

    this->flagSelectedVolumn = false;

    QImage img_sound;
    QPixmap *pixmap_icon_sound = new QPixmap();

    QString tmp_style = this->lb_sound_text->styleSheet();

    if(this->flagMuteState == true){
        if(img_sound.load(playIconPath::play_mute_icon)){
            *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        tmp_style.replace("color: #B18658;", "color: #FFFFFF;");
    }
    else if(this->flagMuteState == false){
        if(img_sound.load(playIconPath::play_sound_s_icon_off)){
            *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        tmp_style.replace("color: #B18658;", "color: #FFFFFF;");
    }

    this->lb_sound_img->setPixmap(*pixmap_icon_sound);
    this->lb_sound_text->setStyleSheet(tmp_style);
}




/**
 * @brief SectionBottom::setCurrPlayValue 현재 Play 재생바 세팅
 * @param p_curr_playValue
 */
void SectionBottom::setCurrPlayValue(int p_curr_playValue){//c220603

    //if(p_curr_playValue > 0 && p_curr_playValue <= 2000){//cheon211203
    //   emit this->linker->signal_queuelist_reload();
    //   global.queue_position_startFlag = true;
    //}else{
    //    global.queue_position_startFlag = false;
    //}
    //qDebug() << "p_curr_playValue= " << p_curr_playValue;
    this->curPosition = p_curr_playValue;

    char tempSec[10]="", tempMin[10]="", temp3600[10]="";
    int tt2 = this->curPosition/1000;
    int i3600=tt2/3600;
    int i3600_mod=tt2%3600;
    int i12, i23;
    if(i3600>0){
        i23 = i3600_mod/60;
        sprintf(tempMin, "%02d", i23);
        i12 = i3600_mod%60;
        sprintf(tempSec, "%02d", i12);
    }else{
        i23 = tt2/60;
        sprintf(tempMin, "%02d", i23);
        i12 = tt2%60;
        sprintf(tempSec, "%02d", i12);
    }

    QString playbar_cur;
    sprintf(temp3600, "%02d", i3600);
    if(i3600>0){
        //lb_playbar_cur->setFixedWidth(70);
        playbar_cur = QString("%1:%2:%3").arg(temp3600).arg(tempMin).arg(tempSec);
    }
    else{
        //lb_playbar_cur->setFixedWidth(50);
        playbar_cur = QString("%1:%2").arg(tempMin).arg(tempSec);
    }

    tt2 = this->duration/1000;
    //char temp[10] ="", temp2[10]="";
    i3600=tt2/3600;
    i3600_mod=tt2%3600;

    if(i3600>0){
        i23 = i3600_mod/60;
        sprintf(tempMin, "%02d", i23);
        i12 = i3600_mod%60;
        sprintf(tempSec, "%02d", i12);
    }else{
        i23 = tt2/60;
        sprintf(tempMin, "%02d", i23);
        i12 = tt2%60;
        sprintf(tempSec, "%02d", i12);
    }

    //this->lb_duration->setText(tmp_duration.toString("mm:ss"));
    QString playbar_duration;
    sprintf(temp3600, "%02d", i3600);
    if(i3600 > 0){
        //lb_playbar_duration->setFixedWidth(70);
        playbar_duration = QString("%1:%2:%3").arg(temp3600).arg(tempMin).arg(tempSec);
    }
    else{
        //lb_playbar_duration->setFixedWidth(50);

        playbar_duration = QString("%1:%2").arg(tempMin).arg(tempSec);
        //qDebug() << "playbar_duration=" << playbar_duration;
    }


    //this->slider_playbar->disconnect();
    //this->slider_playbar->setValue(this->curPosition);
    //----if(this->playType!=playTypes::RADIO){
    //if(slot_sliderClick_flag)
    //this->slider_playbar->disconnect();//c220526
    //print_debug();

    //qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << this->duration << this->curPosition;

    if(this->duration < 0 ){
        //if(this->titleName == ""){
        //    return;//c220709
        //}
        // print_debug();
        // qDebug() << "this->curPosition= " << this->curPosition;
        //qDebug() << "this->duration= " << this->duration;
        this->slider_playbar->setStyleSheet("QSlider::groove:horizontal { background: transparent; height: 5px; margin: 2px;}"
                                             "QSlider::handle:horizontal { background: #FFFFFF; width: 3px; height: 14px; margin: -4px 6px -4px 6px;}"
                                             "QSlider::add-page:horizontal { background: transparent; }"
                                             "QSlider::sub-page:horizontal { background: #B18658; }");
        this->slider_playbar->setMaximum(1);
        this->lb_playbar_cur->setText(playbar_cur);
        this->lb_playbar_cur->setGeometry(this->lb_playbar_cur->geometry().left(), this->lb_playbar_cur->geometry().top(), this->lb_playbar_cur->sizeHint().width(), 20);
        this->lb_playbar_duration->setText(tr("LIVE"));
        this->lb_playbar_duration->setGeometry(this->lb_playbar_duration->geometry().left(), this->lb_playbar_duration->geometry().top(), this->lb_playbar_duration->sizeHint().width(), 20);
        this->slider_playbar->setValue(this->slider_playbar->maximum());
    }
    else{
        if(this->playType == playTypes::RADIO || this->playType == playTypes::FM_TUNER || this->playType == playTypes::ROSE_RADIO){
            this->slider_playbar->setStyleSheet("QSlider::groove:horizontal { background: transparent; height: 5px; margin: 2px;}"
                                                 "QSlider::handle:horizontal { background: #FFFFFF; width: 3px; height: 14px; margin: -4px 6px -4px 6px;}"
                                                 "QSlider::add-page:horizontal { background: transparent; }"
                                                 "QSlider::sub-page:horizontal { background: #B18658; }");
            this->slider_playbar->setMaximum(1);
            this->lb_playbar_cur->setText(playbar_cur);
            this->lb_playbar_cur->setGeometry(this->lb_playbar_cur->geometry().left(), this->lb_playbar_cur->geometry().top(), this->lb_playbar_cur->sizeHint().width(), 20);
            this->lb_playbar_duration->setText(tr("LIVE"));
            this->lb_playbar_duration->setGeometry(this->lb_playbar_duration->geometry().left(), this->lb_playbar_duration->geometry().top(), this->lb_playbar_duration->sizeHint().width(), 20);
            this->slider_playbar->setValue(this->slider_playbar->maximum());
        }
        else{
            this->lb_playbar_cur->setText(playbar_cur);
            this->lb_playbar_cur->setGeometry(this->lb_playbar_cur->geometry().left(), this->lb_playbar_cur->geometry().top(), this->lb_playbar_cur->sizeHint().width(), 20);
            this->lb_playbar_duration->setText(playbar_duration);
            this->lb_playbar_duration->setGeometry(this->lb_playbar_duration->geometry().left(), this->lb_playbar_duration->geometry().top(), this->lb_playbar_duration->sizeHint().width(), 20);

            if(this->curPosition == 0){
                this->slider_playbar->setStyleSheet("QSlider::groove:horizontal { background: transparent; height: 5px; margin: 2px;}"
                                                     "QSlider::handle:horizontal { background: #FFFFFF; width: 3px; height: 14px; margin: -4px 6px -4px 6px;}"
                                                     "QSlider::add-page:horizontal { background: #212121; }"
                                                     "QSlider::sub-page:horizontal { background: transparent; }");
            }
            else{
                this->slider_playbar->setStyleSheet("QSlider::groove:horizontal { background: transparent; height: 5px; margin: 2px;}"
                                                     "QSlider::handle:horizontal { background: #FFFFFF; width: 3px; height: 14px; margin: -4px 6px -4px 6px;}"
                                                     "QSlider::add-page:horizontal { background: #212121; }"
                                                     "QSlider::sub-page:horizontal { background: #B18658; }");
            }

            this->slider_playbar->setValue(this->curPosition);
        }
    }

    //c220521 connect(this->slider_playbar, SIGNAL(signal_sliderclick(int)), this, SLOT(slot_sliderClick(int)));//c1209
    //connect(this->slider_playbar, SIGNAL(valueChanged(int)) ,this, SLOT(changedSliderBar(int)));
    //connect(this->slider_playbar, SIGNAL(sliderReleased()) ,this, SLOT(changedSliderBar()));//c220511
    //connect(this->slider_playbar, SIGNAL(sliderPressed()) ,this, SLOT(changedSliderBar()));
    //}
}


/**
 * @brief SectionBottom::changedSliderBar [SLOT] 재생바 변경
 */
void SectionBottom::changedSliderBar(){

    print_debug();
    //this->timer->stop();
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject json;
    json.insert("currentPlaySeekto",this->slider_playbar->value());
    json.insert("currentPlayState",22);                             // 항상 22 (라디오 제외 모든 메인메뉴 음원 재생시 모두 22 동일함-어플에서-)
    json.insert("roseToken",global.device.getDeviceRoseToken());
    network->request(HTTP_DEVICE_GET_CURRENT_STATE_PLAY, QString("http://%1:%2/current_play_state").arg(global.device.getDeviceIP()).arg(global.port), json, true);
    //network->request(HTTP_CURRENT_PLAY_STATE, QString("http://%1:%2/current_play_state").arg(global.device.getDeviceIP()).arg(global.port), json, true);

}


/**
 * @brief SectionBottom::changedSliderBar [SLOT] 재생바 변경
 * @param p_value
 */
void SectionBottom::changedSliderBar(int p_value){

    //Q_UNUSED(p_value);
    // nothing
    print_debug();
    qDebug() << "p_value = " << p_value;

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject json;
    json.insert("currentPlaySeekto",p_value);
    json.insert("currentPlayState",22);                             // 항상 22 (라디오 제외 모든 메인메뉴 음원 재생시 모두 22 동일함-어플에서-)
    json.insert("roseToken",global.device.getDeviceRoseToken());
    network->request(HTTP_CURRENT_PLAY_STATE, QString("http://%1:%2/current_play_state").arg(global.device.getDeviceIP()).arg(global.port), json, true);

}


/**
 * @brief SectionBottom::setBackgroundAlbumImg 앨범이미지를 배경이미지로 background 세팅//KeepAspectRatioByExpanding
 */
void SectionBottom::setBackgroundAlbumImg(){

    if(this->pixmap_albumImg->isNull() == false){
        //this->lb_albumbg->setPixmap(this->pixmap_albumImg->scaled(this->width(), 1000, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        this->lb_albumbg->setPixmap(this->pixmap_albumImg->scaled(this->width(), 1000, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        this->lb_albumbg->setGeometry(0, 0, this->width(), BG_H);
        this->lb_bgOpacity->setGeometry(0, 0, this->width(), BG_H);
    }
}


void SectionBottom::slot_volume_change(int &change_val){

    this->lb_sound_text->setText(QString::number(change_val));
}


void SectionBottom::slot_muteState_change(int &change_val){

    this->flagMuteState = (change_val == 1) ? true : false;
    global.setMuteState = this->flagMuteState;

    QImage img_sound;
    QPixmap *pixmap_icon_sound = new QPixmap();

    QString tmp_style = this->lb_sound_text->styleSheet();

    if(this->flagMuteState == true){
        if(this->flagSelectedVolumn == true){
            if(img_sound.load(playIconPath::play_mute_icon_on)){
                *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
                *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

            tmp_style.replace("color: #FFFFFF;", "color: #B18658;");
        }
        else{
            if(img_sound.load(playIconPath::play_mute_icon)){
                *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
                *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

            tmp_style.replace("color: #B18658;", "color: #FFFFFF;");
        }
    }
    else if(this->flagMuteState == false){
        if(this->flagSelectedVolumn == true){
            if(img_sound.load(playIconPath::play_sound_s_icon_on)){
                *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
                *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

            tmp_style.replace("color: #FFFFFF;", "color: #B18658;");
        }
        else{
            if(img_sound.load(playIconPath::play_sound_s_icon_off)){
                *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
                *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

            tmp_style.replace("color: #B18658;", "color: #FFFFFF;");
        }
    }

    this->lb_sound_img->setPixmap(*pixmap_icon_sound);
    this->lb_sound_text->setStyleSheet(tmp_style);
}


void SectionBottom::slot_volume_widget_hide(){

    QImage img_sound;
    QPixmap *pixmap_icon_sound = new QPixmap();
    QString tmp_style = this->lb_sound_text->styleSheet();

    if(this->flagMuteState == true){
        if(img_sound.load(playIconPath::play_mute_icon)){
            *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        tmp_style.replace("color: #B18658;", "color: #FFFFFF;");
    }
    else if(this->flagMuteState == false){
        if(img_sound.load(playIconPath::play_sound_s_icon_off)){
            *pixmap_icon_sound = QPixmap::fromImage(img_sound);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_icon_sound = pixmap_icon_sound->scaled(img_sound.width(), img_sound.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        tmp_style.replace("color: #B18658;", "color: #FFFFFF;");
    }

    this->lb_sound_img->setPixmap(*pixmap_icon_sound);
    this->lb_sound_text->setStyleSheet(tmp_style);
}


void SectionBottom::resizeEvent(QResizeEvent* event){

    Q_UNUSED(event);

    int left = 0;

    this->btn_sound->setGeometry(this->width() - 70, 0, 70, 95);
    this->btn_list->setGeometry(this->btn_sound->geometry().left() - 70, 0, 70, 95);

    if(this->width() <= 1024){
        this->slider_playbar->setGeometry(455, 63, 342, 14);

        left = this->slider_playbar->geometry().left() - this->lb_playbar_cur->sizeHint().width() - 20;
        this->lb_playbar_cur->setGeometry(left, 60, this->lb_playbar_cur->sizeHint().width(), 20);

        left = this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width() + 20;
        this->lb_playbar_duration->setGeometry(left, 60, this->lb_playbar_duration->sizeHint().width(), 20);

        this->lb_shu_img->setGeometry(7, 0, 56, 48);
        this->lb_next_img->setGeometry(7, 0, 56, 48);
        this->lb_pause_img->setGeometry(7, 0, 56, 48);
        this->lb_back_img->setGeometry(7, 0, 56, 48);
        this->lb_rep_img->setGeometry(7, 0, 56, 48);

        this->btn_shu->setGeometry((this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width()) - 73, 10, 56, 48);
        this->btn_next->setGeometry(this->btn_shu->geometry().left() - 70, 10, 56, 48);
        this->btn_pause->setGeometry(this->btn_next->geometry().left() - 70, 10, 56, 48);
        this->btn_back->setGeometry(this->btn_pause->geometry().left() - 70, 10, 56, 48);
        this->btn_rep->setGeometry(this->btn_back->geometry().left() - 70, 10, 56, 48);

        this->lb_icon_playType->setGeometry(94, 15, 30, 30);
        this->lb_title->setGeometry(130, 17, 235, 24);
        this->lb_artistName->setGeometry(130, 42, 235, 18);
        this->lb_trackinfo->setGeometry(130, 60, 235, 18);
    }
    else if(this->width() > 1024 && this->width() <= 1920){
        int right_area = (this->width() - 1024);

        if(right_area > 70){
            int icon_cnt = (right_area / 70);

            switch (icon_cnt) {
            case 1:
                this->slider_playbar->setGeometry(455, 63, 342, 14);

                left = this->slider_playbar->geometry().left() - this->lb_playbar_cur->sizeHint().width() - 20;
                this->lb_playbar_cur->setGeometry(left, 60, this->lb_playbar_cur->sizeHint().width(), 20);

                left = this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width() + 20;
                this->lb_playbar_duration->setGeometry(left, 60, this->lb_playbar_duration->sizeHint().width(), 20);

                this->lb_shu_img->setGeometry(7, 24, 56, 48);
                this->lb_next_img->setGeometry(7, 0, 56, 48);
                this->lb_pause_img->setGeometry(7, 0, 56, 48);
                this->lb_back_img->setGeometry(7, 0, 56, 48);
                this->lb_rep_img->setGeometry(7, 0, 56, 48);

                this->btn_shu->setGeometry(this->btn_list->geometry().left() - 70, 0, 70, 95);
                this->btn_next->setGeometry((this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width()) - 73, 10, 56, 48);
                this->btn_pause->setGeometry(this->btn_next->geometry().left() - 70, 10, 56, 48);
                this->btn_back->setGeometry(this->btn_pause->geometry().left() - 70, 10, 56, 48);
                this->btn_rep->setGeometry(this->btn_back->geometry().left() - 70, 10, 56, 48);

                this->lb_icon_playType->setGeometry(94, 15, 30, 30);
                this->lb_title->setGeometry(130, 17, 235, 24);
                this->lb_artistName->setGeometry(130, 42, 235, 18);
                this->lb_trackinfo->setGeometry(130, 60, 235, 18);
                break;
            case 2:
                this->slider_playbar->setGeometry(455, 63, 342, 14);

                left = this->slider_playbar->geometry().left() - this->lb_playbar_cur->sizeHint().width() - 20;
                this->lb_playbar_cur->setGeometry(left, 60, this->lb_playbar_cur->sizeHint().width(), 20);

                left = this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width() + 20;
                this->lb_playbar_duration->setGeometry(left, 60, this->lb_playbar_duration->sizeHint().width(), 20);

                this->lb_shu_img->setGeometry(7, 24, 56, 48);
                this->lb_next_img->setGeometry(7, 24, 56, 48);
                this->lb_pause_img->setGeometry(7, 0, 56, 48);
                this->lb_back_img->setGeometry(7, 0, 56, 48);
                this->lb_rep_img->setGeometry(7, 0, 56, 48);

                this->btn_shu->setGeometry(this->btn_list->geometry().left() - 70, 0, 70, 95);
                this->btn_next->setGeometry(this->btn_shu->geometry().left() - 70, 0, 70, 95);
                this->btn_pause->setGeometry((this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width()) - 73, 10, 56, 48);
                this->btn_back->setGeometry(this->btn_pause->geometry().left() - 70, 10, 56, 48);
                this->btn_rep->setGeometry(this->btn_back->geometry().left() - 70, 10, 56, 48);

                this->lb_icon_playType->setGeometry(94, 15, 30, 30);
                this->lb_title->setGeometry(130, 17, 235, 24);
                this->lb_artistName->setGeometry(130, 42, 235, 18);
                this->lb_trackinfo->setGeometry(130, 60, 235, 18);
                break;
            case 3:
                this->slider_playbar->setGeometry(455, 63, 342, 14);

                left = this->slider_playbar->geometry().left() - this->lb_playbar_cur->sizeHint().width() - 20;
                this->lb_playbar_cur->setGeometry(left, 60, this->lb_playbar_cur->sizeHint().width(), 20);

                left = this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width() + 20;
                this->lb_playbar_duration->setGeometry(left, 60, this->lb_playbar_duration->sizeHint().width(), 20);

                this->lb_shu_img->setGeometry(7, 24, 56, 48);
                this->lb_next_img->setGeometry(7, 24, 56, 48);
                this->lb_pause_img->setGeometry(7, 24, 56, 48);
                this->lb_back_img->setGeometry(7, 0, 56, 48);
                this->lb_rep_img->setGeometry(7, 0, 56, 48);

                this->btn_shu->setGeometry(this->btn_list->geometry().left() - 70, 0, 70, 95);
                this->btn_next->setGeometry(this->btn_shu->geometry().left() - 70, 0, 70, 95);
                this->btn_pause->setGeometry(this->btn_next->geometry().left() - 70, 0, 70, 95);
                this->btn_back->setGeometry((this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width()) - 73, 10, 56, 48);
                this->btn_rep->setGeometry(this->btn_back->geometry().left() - 70, 10, 56, 48);

                this->lb_icon_playType->setGeometry(94, 15, 30, 30);
                this->lb_title->setGeometry(130, 17, 235, 24);
                this->lb_artistName->setGeometry(130, 42, 235, 18);
                this->lb_trackinfo->setGeometry(130, 60, 235, 18);
                break;
            case 4:
                this->slider_playbar->setGeometry(455, 63, 342, 14);

                left = this->slider_playbar->geometry().left() - this->lb_playbar_cur->sizeHint().width() - 20;
                this->lb_playbar_cur->setGeometry(left, 60, this->lb_playbar_cur->sizeHint().width(), 20);

                left = this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width() + 20;
                this->lb_playbar_duration->setGeometry(left, 60, this->lb_playbar_duration->sizeHint().width(), 20);

                this->lb_shu_img->setGeometry(7, 24, 56, 48);
                this->lb_next_img->setGeometry(7, 24, 56, 48);
                this->lb_pause_img->setGeometry(7, 24, 56, 48);
                this->lb_back_img->setGeometry(7, 24, 56, 48);
                this->lb_rep_img->setGeometry(7, 0, 56, 48);

                this->btn_shu->setGeometry(this->btn_list->geometry().left() - 70, 0, 70, 95);
                this->btn_next->setGeometry(this->btn_shu->geometry().left() - 70, 0, 70, 95);
                this->btn_pause->setGeometry(this->btn_next->geometry().left() - 70, 0, 70, 95);
                this->btn_back->setGeometry(this->btn_pause->geometry().left() - 70, 0, 70, 95);
                this->btn_rep->setGeometry((this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width()) - 73, 10, 56, 48);

                this->lb_icon_playType->setGeometry(94, 15, 30, 30);
                this->lb_title->setGeometry(130, 17, 235, 24);
                this->lb_artistName->setGeometry(130, 42, 235, 18);
                this->lb_trackinfo->setGeometry(130, 60, 235, 18);
                break;
            default:
                this->lb_shu_img->setGeometry(7, 24, 56, 48);
                this->lb_next_img->setGeometry(7, 24, 56, 48);
                this->lb_pause_img->setGeometry(7, 24, 56, 48);
                this->lb_back_img->setGeometry(7, 24, 56, 48);
                this->lb_rep_img->setGeometry(7, 24, 56, 48);

                this->btn_shu->setGeometry(this->btn_list->geometry().left() - 70, 0, 70, 95);
                this->btn_next->setGeometry(this->btn_shu->geometry().left() - 70, 0, 70, 95);
                this->btn_pause->setGeometry(this->btn_next->geometry().left() - 70, 0, 70, 95);
                this->btn_back->setGeometry(this->btn_pause->geometry().left() - 70, 0, 70, 95);
                this->btn_rep->setGeometry(this->btn_back->geometry().left() - 70, 0, 70, 95);

                if((this->width() - 1420) <= 0){
                    this->slider_playbar->setGeometry(455, 42, 342, 14);

                    left = this->slider_playbar->geometry().left() - this->lb_playbar_cur->sizeHint().width() - 20;
                    this->lb_playbar_cur->setGeometry(left, 38, this->lb_playbar_cur->sizeHint().width(), 20);

                    left = this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width() + 20;
                    this->lb_playbar_duration->setGeometry(left, 38, this->lb_playbar_duration->sizeHint().width(), 20);

                    this->lb_icon_playType->setGeometry(94, 15, 30, 30);
                    this->lb_title->setGeometry(130, 17, 235, 24);
                    this->lb_artistName->setGeometry(130, 42, 235, 18);
                    this->lb_trackinfo->setGeometry(130, 60, 235, 18);
                }
                else{
                    if((this->width() - 1420) - 30 <= 0){
                        int slider_interval = ((this->width() - 1420) / 3);
                        this->slider_playbar->setGeometry(455 + slider_interval, 42, 342, 14);

                        left = this->slider_playbar->geometry().left() - this->lb_playbar_cur->sizeHint().width() - (20 + slider_interval);
                        this->lb_playbar_cur->setGeometry(left, 38, this->lb_playbar_cur->sizeHint().width(), 20);

                        left = this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width() + (20 + slider_interval);
                        this->lb_playbar_duration->setGeometry(left, 38, this->lb_playbar_duration->sizeHint().width(), 20);

                        this->lb_icon_playType->setGeometry(94, 15, 30, 30);
                        this->lb_title->setGeometry(130, 17, 235, 24);
                        this->lb_artistName->setGeometry(130, 42, 235, 18);
                        this->lb_trackinfo->setGeometry(130, 60, 235, 18);
                    }
                    else if((this->width() - 1450) - 258 <= 0){
                        int slider_width = this->width() - 1450;
                        int slider_left = (slider_width > 214) ? 465 + (this->width() - 1450) - 215 : 465;
                        this->slider_playbar->setGeometry(slider_left, 42, 342 + slider_width, 14);

                        left = this->slider_playbar->geometry().left() - this->lb_playbar_cur->sizeHint().width() - 30;
                        this->lb_playbar_cur->setGeometry(left, 38, this->lb_playbar_cur->sizeHint().width(), 20);

                        left = this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width() + 30;
                        this->lb_playbar_duration->setGeometry(left, 38, this->lb_playbar_duration->sizeHint().width(), 20);

                        this->lb_icon_playType->setGeometry(94, 15, 30, 30);
                        this->lb_title->setGeometry(130, 17, 235, 24);
                        this->lb_artistName->setGeometry(130, 42, 235, 18);
                        this->lb_trackinfo->setGeometry(130, 60, 235, 18);
                    }
                    else if((this->width() - 1708) - 215 <= 0){
                        int slider_left = ((this->width() - 1708) < 215) ? 508 + (this->width() - 1708) : 723;
                        this->slider_playbar->setGeometry(slider_left, 42, 600, 14);

                        left = this->slider_playbar->geometry().left() - this->lb_playbar_cur->sizeHint().width() - 30;
                        this->lb_playbar_cur->setGeometry(left, 38, this->lb_playbar_cur->sizeHint().width(), 20);

                        left = this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width() + 30;
                        this->lb_playbar_duration->setGeometry(left, 38, this->lb_playbar_duration->sizeHint().width(), 20);

                        if(this->width() >= 1728 && (this->width() - 1728) <= 13){
                            int lb_left = (this->width() - 1728);

                            this->lb_icon_playType->setGeometry(94 + lb_left, 15, 30, 30);
                            this->lb_title->setGeometry(130 + lb_left, 17, 235, 24);
                            this->lb_artistName->setGeometry(130 + lb_left, 42, 235, 18);
                            this->lb_trackinfo->setGeometry(130 + lb_left, 60, 235, 18);
                        }
                        else if(this->width() >= 1728 && (this->width() - 1728) > 13){
                            int lb_width = ((this->width() - 1728) < 185) ? 235 + (this->width() - 1728) : 420;
                            this->lb_icon_playType->setGeometry(107, 15, 30, 30);
                            this->lb_title->setGeometry(143, 17, lb_width, 24);
                            this->lb_artistName->setGeometry(143, 42, lb_width, 18);
                            this->lb_trackinfo->setGeometry(143, 60, lb_width, 18);
                        }
                    }
                }
                break;
            }
        }
        else{
            this->slider_playbar->setGeometry(455, 63, 342, 14);

            left = this->slider_playbar->geometry().left() - this->lb_playbar_cur->sizeHint().width() - 20;
            this->lb_playbar_cur->setGeometry(left, 60, this->lb_playbar_cur->sizeHint().width(), 20);

            left = this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width() + 20;
            this->lb_playbar_duration->setGeometry(left, 60, this->lb_playbar_duration->sizeHint().width(), 20);

            this->lb_shu_img->setGeometry(7, 0, 56, 48);
            this->lb_next_img->setGeometry(7, 0, 56, 48);
            this->lb_pause_img->setGeometry(7, 0, 56, 48);
            this->lb_back_img->setGeometry(7, 0, 56, 48);
            this->lb_rep_img->setGeometry(7, 0, 56, 48);

            this->btn_shu->setGeometry((this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width()) - 73, 10, 56, 48);
            this->btn_next->setGeometry(this->btn_shu->geometry().left() - 70, 10, 56, 48);
            this->btn_pause->setGeometry(this->btn_next->geometry().left() - 70, 10, 56, 48);
            this->btn_back->setGeometry(this->btn_pause->geometry().left() - 70, 10, 56, 48);
            this->btn_rep->setGeometry(this->btn_back->geometry().left() - 70, 10, 56, 48);

            this->lb_icon_playType->setGeometry(94, 15, 30, 30);
            this->lb_title->setGeometry(130, 17, 235, 24);
            this->lb_artistName->setGeometry(130, 42, 235, 18);
            this->lb_trackinfo->setGeometry(130, 60, 235, 18);
        }
    }
    else{
        this->lb_shu_img->setGeometry(7, 24, 56, 48);
        this->lb_next_img->setGeometry(7, 24, 56, 48);
        this->lb_pause_img->setGeometry(7, 24, 56, 48);
        this->lb_back_img->setGeometry(7, 24, 56, 48);
        this->lb_rep_img->setGeometry(7, 24, 56, 48);

        this->btn_shu->setGeometry(this->btn_list->geometry().left() - 70, 0, 70, 95);
        this->btn_next->setGeometry(this->btn_shu->geometry().left() - 70, 0, 70, 95);
        this->btn_pause->setGeometry(this->btn_next->geometry().left() - 70, 0, 70, 95);
        this->btn_back->setGeometry(this->btn_pause->geometry().left() - 70, 0, 70, 95);
        this->btn_rep->setGeometry(this->btn_back->geometry().left() - 70, 0, 70, 95);

        if(this->width() > 1920 && this->width() <= 2592){
            int slider_width = ((this->width() - 1920) < 672) ? 600 + (this->width() - 1920) : 1272;
            this->slider_playbar->setGeometry(707, 42, slider_width, 14);

            left = this->slider_playbar->geometry().left() - this->lb_playbar_cur->sizeHint().width() - 30;
            this->lb_playbar_cur->setGeometry(left, 38, this->lb_playbar_cur->sizeHint().width(), 20);

            left = this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width() + 30;
            this->lb_playbar_duration->setGeometry(left, 38, this->lb_playbar_duration->sizeHint().width(), 20);
        }
        else if(this->width() > 2592 && this->width() <= 3155){
            int slider_left = ((this->width() - 2592) < 562) ? 707 + (this->width() - 2592) : 1285;
            this->slider_playbar->setGeometry(slider_left, 42, 1272, 14);

            left = this->slider_playbar->geometry().left() - this->lb_playbar_cur->sizeHint().width() - 30;
            this->lb_playbar_cur->setGeometry(left, 38, this->lb_playbar_cur->sizeHint().width(), 20);

            left = this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width() + 30;
            this->lb_playbar_duration->setGeometry(left, 38, this->lb_playbar_duration->sizeHint().width(), 20);
        }
        else{
            this->slider_playbar->setGeometry(1285, 42, 1272, 14);

            left = this->slider_playbar->geometry().left() - this->lb_playbar_cur->sizeHint().width() - 30;
            this->lb_playbar_cur->setGeometry(left, 38, this->lb_playbar_cur->sizeHint().width(), 20);

            left = this->slider_playbar->geometry().left() + this->slider_playbar->geometry().width() + 30;
            this->lb_playbar_duration->setGeometry(left, 38, this->lb_playbar_duration->sizeHint().width(), 20);
        }

        if(this->width() >= 2925){
            int lb_width = ((this->width() - 2925) < 230) ? 420 + (this->width() - 2925) : 650;
            this->lb_icon_playType->setGeometry(107, 15, 30, 30);
            this->lb_title->setGeometry(143, 17, lb_width, 24);
            this->lb_artistName->setGeometry(143, 42, lb_width, 18);
            this->lb_trackinfo->setGeometry(143, 60, lb_width, 18);
        }
        else{
            this->lb_icon_playType->setGeometry(107, 15, 30, 30);
            this->lb_title->setGeometry(143, 17, 420, 24);
            this->lb_artistName->setGeometry(143, 42, 420, 18);
            this->lb_trackinfo->setGeometry(143, 60, 420, 18);
        }

        //qDebug() << "[Debug] left" << this->slider_playbar->geometry() <<  this->width();
    }

    this->slider_playbar->show();
    this->lb_playbar_cur->show();
    this->lb_playbar_duration->show();

    // 화면 사이즈에 맞게 text 생략할지 말지 재세팅
    this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->titleName, this->lb_title->geometry().width(), this->lb_title->font()));
    this->lb_artistName->setText(GSCommon::getTextCutFromLabelWidth(this->artistName, this->lb_artistName->geometry().width(), this->lb_artistName->font()));
    this->lb_trackinfo->setText(GSCommon::getTextCutFromLabelWidth(this->trackinfo, this->lb_trackinfo->geometry().width(), this->lb_trackinfo->font()));

    // 화면 사이즈에 맞게 배경이미지 재세팅
    this->setBackgroundAlbumImg();

    // 화면 사이즈에 맞게 play bar 재세팅
    this->setCurrPlayValue(this->curPosition);

    //QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    //print_debug();

    //qDebug() << this->lb_title->width();
    //this->wg_playbar_cur->resize(this->width() - 900, 20);
    //qDebug() << this->wg_playbar_cur->width();
}


