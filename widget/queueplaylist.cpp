#include "queueplaylist.h"
#include "trackinfo.h"
#include "common/global.h"

#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QJsonObject>
#include <QScrollArea>
#include <qpainter.h>
#include <QPixmapCache>
#include <QCheckBox>
#include <QDateTime>

#include <QFileDialog>//c220609
#include <QMediaPlayer>//c220609

#include <QtAlgorithms>
#include <common/gscommon.h>
#include <common/networkhttp.h>
#include <common/sqlitehelper.h>
#include "widget/framewidget.h"
#include "widget/toastmsg.h"
//#include "widget/listwidget.h"
#include "common/rosesettings.h"
//#include "setting/setting_struct.h"
#include "home/sectionleft.h"
#include <QWheelEvent>//cheon211114-01

// for Tidal
#include "tidal/tidal_struct.h"
#include "tidal/ConvertData.h"
#include "tidal/ProcRosePlay_withTidal.h"

// for Bugs
#include "bugs/bugs_struct.h"
#include "bugs/ConvertData_forBugs.h"
#include "bugs/ProcRoseAPI_withBugs.h"

// for Qobuz
#include "qobuz/qobuz_struct.h"
#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ProcRosePlay_withQobuz.h"
#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";

//binarycreator.exe -c config\config.xml -p packages Rose_Connect_setup_4.1.8.2.exe

/*
 https://dev.api.roseaudio.kr/docs/member/v1/#mediaType%EB%B3%84userpick%EC%B9%9C%EA%B5%AC%EB%AA%A9%EB%A1%9D

 * */

//const bool NODELFLAG = false;
//const bool DELFLAG = true;
int ProgressCont = 0;
int MAX_THUMB_SET = 20;
//const int THUMB_MUSIC_W = 60;
//const int THUMB_MUSIC_H = 60;
//const int THUMB_ROSETUBE_W = 60;
//const int THUMB_ROSETUBE_H = 33;

const int QUEUEPLAY_W = 550;
//const int QUEUEPLAY_H = 840;          // 사용하지 않는 값으로 주석처리함 by sunnyfish
const int Q_MAIN_ICON_W = 30;
const int Q_MAIN_ICON_H = 60;

const int HTTP_PLAY_CURRENT_MUSIC = 41;
const int HTTP_PLAY_CURRENT_ROSETUBE = 42;
//const int HTTP_PLAY_CURRENT_TIDAL = 43;
//const int HTTP_PLAY_CURRENT_BUGS = 44;
//const int HTTP_PLAY_CURRENT_QOBUZ = 45;
const int HTTP_CURR_QUEUE_GET_MUSIC = 46;//cheon211104-01

const int HTTP_CURR_TOTAL_QUEUE_GET = 51;
const int HTTP_CURR_TOTAL_QUEUE_POSITION_GET = 511;



const int HTTP_CURR_QUEUE_GET_ROSETUBE = 52;
const int HTTP_CURR_QUEUE_GET_TIDAL = 53;
const int HTTP_CURR_QUEUE_GET_BUGS = 54;
//const int HTTP_CURR_QUEUE_GET_BUGSMV = 55;
const int HTTP_CURR_QUEUE_GET_QOBUZ = 56;
const int HTTP_CURR_QUEUE_GET_TOTAL = 57;
const int HTTP_CURR_QUEUE_GET_APPLEMUSIC = 158;//cheon210916-total

//start---------cheon211114-01
const int HTTP_CURR_QUEUE_GET_RECOMM_MUSIC = 88;//cheon211104-01
//const int HTTP_CURR_TOTAL_QUEUE_GET_RECOMM = 81;
const int HTTP_CURR_QUEUE_GET_RECOMM_ROSETUBE = 82;
const int HTTP_CURR_QUEUE_GET_RECOMM_TIDAL = 83;
const int HTTP_CURR_QUEUE_GET_RECOMM_BUGS = 84;
const int HTTP_CURR_QUEUE_GET_RECOMM_BUGSMV = 85;
const int HTTP_CURR_QUEUE_GET_RECOMM_QOBUZ = 86;
const int HTTP_CURR_QUEUE_GET_RECOMM_TOTAL = 87;               //cheon210916-total
const int HTTP_CURR_QUEUE_GET_RECOMM_APPLEMUSIC = 89;
//end-----------------------------

//const int HTTP_CURR_QUEUE_GET_CHECKSUM_QOBUZ = 61;      //cheon210916-total
//const int HTTP_CURR_QUEUE_GET_CHECKSUM_TIDAL = 62;      //cheon210916-total
//const int HTTP_CURR_QUEUE_GET_CHECKSUM_BUGS = 63;       //cheon210916-total
//const int HTTP_CURR_QUEUE_GET_CHECKSUM_ROSETUBE = 64;   //cheon210916-total
//const int HTTP_CURR_QUEUE_GET_CHECKSUM_MUSIC = 65;      //cheon210916-total
const int HTTP_CURR_QUEUE_GET_CHECKSUM_TOTAL = 66;      //cheon210916-total



const QString CURRMENU_MOVE = "clb_move";
const QString CURRMENU_DEL = "clb_del";

const QString CURRMENU_MUSIC = "clb_music";
const QString CURRMENU_VIDEO = "clb_video";
const QString CURRMENU_FMTUNER = "clb_fmtuner";
const QString CURRMENU_RADIO = "clb_radio";
const QString CURRMENU_ROSETUBE = "clb_roseTube";
const QString CURRMENU_POTCAST = "clb_potcast";
const QString CURRMENU_TIDAL = "clb_tidal";
const QString CURRMENU_BUGS = "clb_bugs";
const QString CURRMENU_QOBUZ = "clb_qobuz";
const QString CURRMENU_APPLE = "clb_apple";
const QString CURRMENU_TOTAL = "clb_total";

const int HTTP_CURR_TOTAL_QUEUE_CURRPLAYINDEX_GET = 73;
const int HTTP_CURR_TOTAL_QUEUE_CURRPLAYINDEX_GET_CHECK = 77;
//-------------cheon210916-total
const QString REMOTE_TOTAL_QUEUE_GET_CHECKSUM = "total_queue_get_checksum";
const QString TOTAL_QUEUE_GET_FILE = "total_queue_get_file";
const QString REMOTE_TOTAL_QUEUE_GET = "total_queue_get";
const QString REMOTE_TOTAL_QUEUE_SET = "total_queue_set";

const QString REMOTE_TOTAL_QUEUE_RECOMM_GET = "total_recommand_get";//cheon211114-01
const int HTTP_CURR_TOTAL_QUEUE__RECOMM_GET = 58;               //cheon211114-01
const int CURR_QUEUE_POST_PLAY_RELATE_STATE = 59;//cheon211122-01


const QString REMOTE_TOTAL_QUEUE_DELETE = "total_queue_delete";
const QString REMOTE_TOTAL_QUEUE_MODIFY = "total_queue_modify";
const QString REMOTE_TOTAL_PLAY_LIST_ADD = "total_queue_play_list_add";
//--------------

/** ******************************************************************************************************
 * ******************************************************************************************************
 *
 *  QueuePlayList 클래스
 *
 * ******************************************************************************************************
 * ****************************************************************************************************** */
/**
 * @brief QueuePlayList::QueuePlayList 생성자
 * @param parent
 */
QueuePlayList::QueuePlayList(QWidget *parent) : QWidget(parent)
{
    //print_debug();
    //this->list_queueItem = QList<QueueItem*>();
    pixmapDefault.load(":images/def_mus_60.png");
    pixmapCheckOff.load(":images/playlist/list_check_off.png");
    pixmapCheckOn.load(":images/playlist/list_check_on.png");
    pixmapDel.load(":images/playlist/fol_del_ico.png");

    pixmapMore.load(":images/icon_menu.png");
    pixmapNet.load(":images/playBar/net_play_ico.png");
    pixmapShare.load(":images/playBar/mus_play_ico_share.png");
    pixmapPlaying.load(":images/playing_icon.png");
    pixmapFlac.load(":images/flac_mini.png");
    pixmapChange_list.load(":images/change_list_ico_b.png");

    this->nextPlayNo = -1;
    //delegate = new QueueItemDelegate(this);
    //connect(delegate, SIGNAL(signal_clickedBtn(int,int)), this, SLOT(slot_delegateClicked(int,int)));
    // 상단 title 및 버튼들
    //QSlider::sub-page:horizontal { background:#B18658; }
    //queProgressBar->setStyleSheet("QProgressBar::groove:horizontal { background:red; } QProgressBar::handle:horizontal { width:15px;height:30px;background:white;border-radius:0px;margin:0px 0px; } QProgressBar::add-page:horizontal { background:#3D3D3E; } QProgressBar::sub-page:horizontal { background:#B18658; } ");

    queProgressBar->setStyleSheet("QProgressBar {background-color:#888888;border:1px solid #888888;border-radius:15px; padding-right:10px;} QProgressBar::handle:horizontal { width:15px;height:30px;background:white;border-radius:0px;margin:0px 0px; } QProgressBar::chunk {background-color: #010327; } QProgressBar::sub-page:horizontal { background:#010327; }");//cheon05_progress_bar
    queProgressBar->setFixedHeight(5);//cheon05_progress_bar // QProgressBar
    queProgressBar->setRange(0, 100);
    queProgressBar->setValue(0);
    queProgressBar->setTextVisible(false);
    //queProgressBar->setFixedWidth(250);//cheon05_progress_bar // QProgressBar

    //---------------------------------------------------------
    QPushButton *btn_question;
    btn_question =  GSCommon::getUIBtnImg("question", ":images/setting/201/tip_icon.png");
    btn_question->setCursor(Qt::PointingHandCursor);
    btn_question->setFixedSize(QSize(30,30));

    dlgConfirmQueue = new DialogConfirm(this);

    btn_edit = new QPushButton();
    //btn_edit->setText(tr("Del"));
    //btn_edit->setText(tr("편집"));
    btn_edit->setIcon(QIcon(":/images/q_del_ico.png"));
    btn_edit->setIconSize(QSize(50, 50));//70,70
    //btn_edit->setFixedSize(60,33);
    btn_edit->setCursor(Qt::PointingHandCursor);
    btn_edit->setObjectName("btn_edit");
    //btn_edit->setStyleSheet("#btn_edit {font-size:16px; color:#ececec; background-color:#333333; border-radius:13px;  } ");
    btn_edit->setStyleSheet("QPushButton {font-size:16px; color:#ececec;  background-color:transparent;  border-radius:13px;} QPushButton:hover {background-color: #505050; color: white;} QPushButton:pressed{background-color: #F79862;} QPushButton:checked{background-color: #F79862;border:none;} QToolTip{ color: #404040; }");


    btn_mov = new QPushButton();
    //btn_mov->setText(tr("Move"));
    btn_mov->setIcon(QIcon(":/images/q_move_ico.png"));
    btn_mov->setIconSize(QSize(50, 50));//70,70
    //btn_mov->setFixedSize(60,33);
    btn_mov->setCursor(Qt::PointingHandCursor);
    btn_mov->setObjectName("btn_del");
    //btn_mov->setStyleSheet("#btn_del {font-size:16px; color:#ececec; background-color:#333333; border-radius:13px;  } ");
    btn_mov->setStyleSheet("QPushButton {font-size:16px; color:#ececec;  background-color:transparent;  border-radius:13px;} QPushButton:hover {background-color: #505050; color: white;} QPushButton:pressed{background-color: #F79862;} QPushButton:checked{background-color: #F79862;border:none;} QToolTip{ color: #404040; }");


    btn_play = new QPushButton();
    btn_play->setText(tr("Play"));

    btn_play->setFixedSize(60,33);
    btn_play->setCursor(Qt::PointingHandCursor);
    btn_play->setObjectName("btn_del");
    //btn_play->setStyleSheet("#btn_del {font-size:16px; color:#ececec;background-color:#333333; border-radius:13px;  } ");

    menu_click_init();

    btn_PlayPosition = new QPushButton(tr("Refresh"));//c220718
    btn_PlayPosition->setObjectName("btn_PlayPosition");
    btn_PlayPosition->setCursor(Qt::PointingHandCursor);
    btn_PlayPosition->setFixedSize(80,30);            //45, 50
    //btn_PlayPosition->setStyleSheet("#btn_PlayPosition { font-size:16px; color:#ececec;  background-color:transparent;  border:2px solid #CCCCCC;border-radius:13px;  } ");
    btn_PlayPosition->setStyleSheet("QPushButton {font-size:16px; color:#ececec;  background-color:transparent;  border:2px solid #CCCCCC;border-radius:13px;} QPushButton:hover {background-color: #505050; color: white;} QPushButton:pressed{background-color: #F79862;} QPushButton:checked{background-color: #F79862;border:none;} QToolTip{ color: #404040; }");


    // 편집모드일때 보이는 버튼
    checkbox_allSelected = new QCheckBox();
    checkbox_allSelected->setObjectName("checkbox_allSelected");
    checkbox_allSelected->setText(tr("Select All"));
    //checkbox_allSelected->setText(tr("전체선택"));
    checkbox_allSelected->setStyleSheet(
                "#checkbox_allSelected { background-color:transparent;font-size:16px;color:#CCCCCC;spacing:0px;font:bold;}"
                "QCheckBox::indicator:unchecked { image: url(:images/playlist/list_check_off.png);}"
                "QCheckBox::indicator:checked { image: url(:images/playlist/list_check_on.png);}"
                );
    checkbox_allSelected->setCursor(Qt::PointingHandCursor);
    checkbox_allSelected->setFixedHeight(33);

    btn_delok = new QPushButton(tr("Complete"));
    btn_delok->setObjectName("btn_delok");
    btn_delok->setCursor(Qt::PointingHandCursor);
    btn_delok->setFixedSize(80,26);            //45, 50
    btn_delok->setStyleSheet("#btn_delok {font-weight:200; font-size:14px; color:#B18658;  background-color:transparent; border:1px solid #B18658;border-radius:13px;  } ");
    btn_delok->setStyleSheet("QPushButton {font-size:14px; color:#ececec;  background-color:transparent;  border:2px solid #CCCCCC;border-radius:13px;} QPushButton:hover {background-color: #505050; color: white;} QPushButton:pressed{background-color: #F79862;} QPushButton:checked{background-color: #F79862;border:none;} QToolTip{ color: #404040; }");


    btn_del = new QPushButton(tr("Delete"));
    btn_del->setObjectName("btn_delok");
    btn_del->setCursor(Qt::PointingHandCursor);
    btn_del->setFixedSize(70,26);            //45, 50
    btn_del->setStyleSheet("#btn_delok { font-size:14px; color:#ececec;  background-color:transparent;  border-radius:13px;  } ");

    QHBoxLayout *hl_editMode = new QHBoxLayout();
    hl_editMode->setContentsMargins(0,0,10,0);
    hl_editMode->setSpacing(10);
    hl_editMode->addWidget(checkbox_allSelected);
    hl_editMode->addStretch(1);
    hl_editMode->addWidget(btn_del);
    hl_editMode->addWidget(btn_question);

    widget_editMode = new QWidget();
    widget_editMode->setObjectName("widget_editMode");
    widget_editMode->setFixedHeight(33);

    widget_editMode->setLayout(hl_editMode);
    widget_editMode->setStyleSheet("#widget_editMode { background-color:#494949; border-bottom:1px solid #494949; }");//#494949

    //-----------------------------------------------------------


    btn_moveok = new QPushButton(tr("Complete"));
    btn_moveok->setObjectName("btn_moveok");
    btn_moveok->setCursor(Qt::PointingHandCursor);
    btn_moveok->setFixedSize(80,26);            //45, 50
    //btn_moveok->setStyleSheet("color:#CCCCCC;font-size:16px;font:bold;background-color:transparent;");
    // btn_moveok->setStyleSheet("#btn_moveok { font-weight:200;font-size:14px; color:#B18658; background-color:transparent;border:1px solid #B18658; border-radius:13px;  } ");
    btn_moveok->setStyleSheet("QPushButton {font-size:14px; color:#ececec;  background-color:transparent;  border:2px solid #CCCCCC;border-radius:13px;} QPushButton:hover {background-color: #505050; color: white;} QPushButton:pressed{background-color: #F79862;} QPushButton:checked{background-color: #F79862;border:none;} QToolTip{ color: #404040; }");


    QHBoxLayout *hl_moveMode = new QHBoxLayout();
    hl_moveMode->setContentsMargins(0,0,10,0);
    hl_moveMode->setSpacing(10);
    hl_moveMode->addStretch(1);
    hl_moveMode->addWidget(btn_moveok);
    hl_moveMode->addWidget(btn_question);

    widget_moveMode = new QWidget();
    widget_moveMode->setObjectName("widget_moveMode");
    widget_moveMode->setFixedHeight(33);
    widget_moveMode->setLayout(hl_moveMode);
    widget_moveMode->setStyleSheet("#widget_moveMode { background-color:#494949; border-bottom:1px solid #494949; }");

    //----
    lb_title = new QLabel();
    lb_title->setText(tr("Queue List"));
    lb_title->setFixedHeight(30);
    //lb_title->setText(tr("재생목록"));
    lb_title->setStyleSheet("font-weight:200;font-size:22px;font:bold;color:#FFFFFF;");

    lb_title_cnt = new QLabel();
    lb_title_cnt->setText(tr("0/3,000"));

    lb_title_cnt->setFixedHeight(20);
    lb_title_cnt->setStyleSheet("font-weight:200;font-size:18px;color:#bbbbbb;");
    //---------------
    lb_position_label = new QLabel();
    lb_position_label->setText(tr("Currently Playing : "));
    lb_position_label->setFixedSize(135,20);
    lb_position_label->setStyleSheet("font-weight:200;font-size:16px;color:#FFFFFF;");

    lb_position_site = new QLabel();
    lb_position_site->setText(tr("Tidal"));
    lb_position_site->setFixedSize(110,20);
    lb_position_site->setStyleSheet("font-weight:200;font-size:16px;color:#B18658;");

    lb_position_title_name = new QLabel();
    lb_position_title_name->setText(tr("Title : "));
    lb_position_title_name->setFixedSize(50,20);
    lb_position_title_name->setStyleSheet("font-weight:200;font-size:16px;color:#FFFFFF; ");

    lb_position_title = new QLabel();
    lb_position_title->setText(tr("song"));
    lb_position_title->setFixedHeight(20);
    lb_position_title->setStyleSheet("font-weight:200;font-size:16px;color:#B18658; ");
    //--------------
    QHBoxLayout *vl_lb_position_msg = new QHBoxLayout();
    vl_lb_position_msg->setContentsMargins(0,0,0,0);
    vl_lb_position_msg->setSpacing(0);
    vl_lb_position_msg->setAlignment(Qt::AlignCenter);
    vl_lb_position_msg->addWidget(lb_position_label);
    vl_lb_position_msg->addWidget(lb_position_site, 0, Qt::AlignLeft);
    vl_lb_position_msg->addSpacing(10);
    vl_lb_position_msg->addWidget(lb_position_title_name);
    vl_lb_position_msg->addWidget(lb_position_title, 0, Qt::AlignLeft);

    widget_position_msg = new QWidget();
    widget_position_msg->setStyleSheet("background-color:#555555; ");
    widget_position_msg->setLayout(vl_lb_position_msg);
    //------------------

    QVBoxLayout *vl_lb_title = new QVBoxLayout();
    vl_lb_title->setContentsMargins(0,10,0,10);
    vl_lb_title->setSpacing(0);
    vl_lb_title->setAlignment(Qt::AlignTop);
    vl_lb_title->addWidget(lb_title);
    vl_lb_title->addWidget(lb_title_cnt);

    widget_title = new QWidget();
    widget_title->setLayout(vl_lb_title);

    lb_title_move = new QLabel();
    lb_title_move->setText(tr("OrderEdit"));
    lb_title_move->setFixedHeight(70);
    //lb_title_move->setText(tr("재생목록"));
    lb_title_move->setStyleSheet("font-weight:200;font-size:22px;color:#FFFFFF;");

    lb_title_del = new QLabel();
    lb_title_del->setText(tr("0 songs selected"));
    lb_title_del->setFixedHeight(70);
    lb_title_del->setStyleSheet("font-weight:200;font-size:22px;color:#FFFFFF;");

    QHBoxLayout *hl_top = new QHBoxLayout();
    //hl_top->setContentsMargins(12,15,12,18);
    hl_top->setContentsMargins(10,0,10,0);
    hl_top->setSpacing(0);
    hl_top->addWidget(widget_title);
    hl_top->addSpacing(10);
    hl_top->addWidget(lb_title_move);
    hl_top->addWidget(lb_title_del);

    hl_top->addStretch(1);
    hl_top->addWidget(btn_PlayPosition);
    hl_top->addSpacing(10);//c220720
    hl_top->addWidget(btn_edit);
    hl_top->addWidget(btn_mov);
    hl_top->addWidget(btn_moveok);
    hl_top->addWidget(btn_delok);

    QWidget *widget_top = new QWidget();
    widget_top->setObjectName("widget_top");
    widget_top->setStyleSheet("#widget_top { background-color:#333333; border-bottom:1px solid #5A5A5A; }");
    widget_top->setLayout(hl_top);
    //----------------------------------------------------------

    pixmapPlaying = pixmapPlaying.scaled(50,50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    pixmapDefault = pixmapDefault.scaled(50,50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    pixmapChange_list = pixmapChange_list.scaled(50,50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    linker = Linker::getInstance();

    //---------------------------------------------------------
    for(int i = 0; i < 10; i++){
        listWidget_total[i] = setListWidget();//-------------------------
    }

    global.now_Section = 0;

    box_contents = new QVBoxLayout();
    box_contents->setSpacing(0);
    box_contents->setContentsMargins(0,0,3,0);
    box_contents->addWidget(listWidget_total[0]);
    box_contents->addWidget(listWidget_total[1]);
    box_contents->addWidget(listWidget_total[2]);
    box_contents->addWidget(listWidget_total[3]);
    box_contents->addWidget(listWidget_total[4]);
    box_contents->addWidget(listWidget_total[5]);
    box_contents->addWidget(listWidget_total[6]);
    box_contents->addWidget(listWidget_total[7]);
    box_contents->addWidget(listWidget_total[8]);
    box_contents->addWidget(listWidget_total[9]);

    listWidget_total_setShow(global.now_Section);

    //this->stackWidget_queue = new QStackedWidget();//c220625

    temp_menuWidget =  new QWidget();
    temp_menuWidget->setStyleSheet("background-color:#111111;");
    temp_menuWidget->setLayout(box_contents);


    QVBoxLayout *vl_topAndTracks = new QVBoxLayout();
    //vl_topAndTracks->setContentsMargins(0,0,0,30);
    vl_topAndTracks->setContentsMargins(0,0,0,0);
    vl_topAndTracks->setSpacing(0);
    vl_topAndTracks->setAlignment(Qt::AlignTop);
    vl_topAndTracks->addWidget(widget_top);
    //vl_topAndTracks->addWidget(queProgressBar);
    vl_topAndTracks->addWidget(widget_position_msg);
    //vl_topAndTracks->addWidget(this->scrollArea);//cheon211120-2
    vl_topAndTracks->addWidget(widget_moveMode);
    vl_topAndTracks->addWidget(widget_editMode);
    vl_topAndTracks->addWidget(temp_menuWidget);//c220625

    //-------------------------------------------------------------------------
    QVBoxLayout *vl_menuIcon = new QVBoxLayout();
    vl_menuIcon->setContentsMargins(0,30,0,0);
    vl_menuIcon->setSpacing(10);
    vl_menuIcon->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    vl_menuIcon->addWidget(clb_total);//cheon210916-total
    vl_menuIcon->addWidget(clb_music);
    //vl_menuIcon->addWidget(clb_video);
    //vl_menuIcon->addWidget(clb_radio);
    //vl_menuIcon->addWidget(clb_fmtuner);
    vl_menuIcon->addWidget(clb_roseTube);
    //vl_menuIcon->addWidget(clb_potcast);
    vl_menuIcon->addWidget(clb_tidal);
    vl_menuIcon->addWidget(clb_bugs);
    vl_menuIcon->addWidget(clb_qobuz);
    vl_menuIcon->addWidget(clb_apple);


    pixmap_init();


    QWidget *widget_menuIcon = new QWidget();
    widget_menuIcon->setFixedWidth(50);
    widget_menuIcon->setObjectName("widget_menuIcon");
    widget_menuIcon->setStyleSheet("#widget_menuIcon { background-color:#666666;border-top-left-radius:15px;border-bottom-left-radius:15px; }");
    widget_menuIcon->setLayout(vl_menuIcon);
    QVBoxLayout *vl_menuIconTotal = new QVBoxLayout();
    vl_menuIconTotal->setContentsMargins(0,0,0,0);
    vl_menuIconTotal->setSpacing(0);
    vl_menuIconTotal->addWidget(widget_menuIcon);


    // 스택에 담기
    //this->stackedWidget = new QStackedWidget();

    QHBoxLayout *hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(0,0,0,0);
    hl_total->setSpacing(0);
    hl_total->addLayout(vl_menuIconTotal);
    hl_total->addLayout(vl_topAndTracks);

    QWidget *widget_total = new QWidget();
    widget_total->setFixedWidth(QUEUEPLAY_W);
    widget_total->setObjectName("widget_total");
    widget_total->setStyleSheet("#widget_total {border-top-left-radius:15px;border-bottom-left-radius:15px;  }");
    widget_total->setLayout(hl_total);
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);
    vlayout->addWidget(widget_total);

    this->setLayout(vlayout);
    //this->setFixedSize(QUEUEPLAY_W,QUEUEPLAY_H);
    this->setFixedWidth(QUEUEPLAY_W);
    this->hide();

    this->widget_position_msg->hide();

    // 현재 선택된 메뉴 default
    this->currMenu = CURRMENU_TOTAL;//cheon211120-02

    connect(btn_question, SIGNAL(clicked()), this, SLOT(slot_queue_question_popup()));
    // 커넥션
    connect(btn_edit,   &QPushButton::clicked, this, &QueuePlayList::slot_clickedEdit);//1126
    //connect(btn_edit,   SIGNAL(clicked()), this, SLOT(slot_clickedEdit()));//1126
    //connect(btn_ok,     &QPushButton::clicked, this, &QueuePlayList::slot_clickedEdit);
    connect(btn_del, &QPushButton::clicked, this, &QueuePlayList::slot_clickedDelete);


    connect(btn_delok, &QPushButton::clicked, this, &QueuePlayList::slot_clickedDeleteOk);

    connect(btn_mov,   &QPushButton::clicked, this, &QueuePlayList::slot_clickedMove);
    //connect(btn_mov,   SIGNAL(signal_clicked()), this, SLOT(slot_changedQueueSubMenu()));//1126
    connect(btn_moveok,     &QPushButton::clicked, this, &QueuePlayList::slot_dragAndDropEdit);
    //connect(btn_mov, &QPushButton::clicked, this, &QueuePlayList::slot_clickedDelete);

    //connect(btn_play,   &QPushButton::clicked, this, &QueuePlayList::slot_clickedPlay);

    connect(checkbox_allSelected, &QCheckBox::clicked, this, &QueuePlayList::slot_clickedAllSelected);

    connect(clb_total, SIGNAL(signal_clicked()), this, SLOT(slot_changedQueueMenu()));//cheon210916-total
    connect(clb_music, SIGNAL(signal_clicked()), this, SLOT(slot_changedQueueMenu()));
    connect(clb_video, SIGNAL(signal_clicked()), this, SLOT(slot_changedQueueMenu()));
    connect(clb_fmtuner, SIGNAL(signal_clicked()), this, SLOT(slot_changedQueueMenu()));
    connect(clb_radio, SIGNAL(signal_clicked()), this, SLOT(slot_changedQueueMenu()));
    connect(clb_roseTube, SIGNAL(signal_clicked()), this, SLOT(slot_changedQueueMenu()));
    connect(clb_potcast, SIGNAL(signal_clicked()), this, SLOT(slot_changedQueueMenu()));
    connect(clb_tidal, SIGNAL(signal_clicked()), this, SLOT(slot_changedQueueMenu()));
    connect(clb_bugs, SIGNAL(signal_clicked()), this, SLOT(slot_changedQueueMenu()));
    connect(clb_qobuz, SIGNAL(signal_clicked()), this, SLOT(slot_changedQueueMenu()));
    connect(clb_apple, SIGNAL(signal_clicked()), this, SLOT(slot_changedQueueMenu()));

    //connect(listWidget->verticalScrollBar(), SIGNAL(sliderMoved(int)), this, SLOT(slot_sliderMoved_notifySliderMove(int)));//cheon211120-3
    connect(this, SIGNAL(signal_assignImg()), this, SLOT(slot_assignImg()));//cheon211122-02
    connect(this, SIGNAL(signal_assignImg0()), this, SLOT(slot_assignImg0()));//cheon211122-02


    //connect(this, SIGNAL(signal_editmode(QListWidgetItem *)), this, SLOT(slot_editModeClicked(QListWidgetItem *)));


    for(int i = 0 ; i < 10; i++){
        //listWidget_total[i]->verticalScrollBar()->setRange(0,100);
        //connect(listWidget_total[i]->verticalScrollBar(), SIGNAL(sliderReleased()), this, SLOT(slot_sliderReleased()));//c221004_1

        connect(listWidget_total[i]->verticalScrollBar(), SIGNAL(sliderReleased()), this, SLOT(slot_sliderReleased()));//c221004_1
        connect(listWidget_total[i], SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slot_listwidgetItemClicked(QListWidgetItem *)));
        connect(listWidget_total[i]->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(slot_listwidgetcurrentRowChanged(int)));//c221004_1
        //connect(listWidget_total[i]->verticalScrollBar(), SIGNAL(rangeChanged(int, int)), this, SLOT(slot_listwidgetcurrentRowChanged(int, int)));//c221004_1
        connect(listWidget_total[i], SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(slot_listwidgetItemPressed(QListWidgetItem *)));
    }
    //------------------------------------------------------------------------------------------
    //connect(listWidget_total, SIGNAL(itemEntered(QListWidgetItem *)), this, SLOT(slot_listwidgetItemEntered(QListWidgetItem *)));
    // connect(listWidget_total, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(slot_listwidgetcurrentItemChanged(QListWidgetItem *, QListWidgetItem *)));
    // connect(listWidget_total, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(slot_listwidgetItemActivated(QListWidgetItem *)));
    //connect(listWidget_total, SIGNAL(currentRowChanged(int)), this, SLOT(slot_listwidgetcurrentRowChanged(int)));
    //   connect(listWidget_total[global.now_Section]->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(myFunction(QModelIndex,int,int,QModelIndex,int)));
    //onScrollBarValueChanged
    //connect(linker, SIGNAL(signal_dragEnter()), this, SLOT(slot_dragAndDropEdit()));

    //signal_queuelist_reload
    //connect(linker, SIGNAL(signal_queuelist_reload()), this, SLOT(slot_queuelistCurrPlayIndexGet_singleshot()));//c220603
    //signal_queuelist_reload
    //connect(linker, SIGNAL(signal_queuelist_reload()), this, SLOT(slot_queuelistCurrPlayIndexGet_singleshot()));//c220603
    //connect(this, SIGNAL(signal_music_start()), this, SLOT(slot_queuelistPositionGet_check()));

    //connect(btn_PlayPosition, &QPushButton::clicked, this, &QueuePlayList::slot_queuelistCurrPlayIndexGet);
    //connect(this, SIGNAL(signal_valueChanged(int)), this, SLOT(slot_listwidgetcurrentRowChanged_start(int)));//c220620

    //slot_queuelistPositionGet();
    //connect(this, SIGNAL(signal_queuelist_GetcurrPlayIndex()), this, SLOT(slot_queuelistCurrPlayIndexGet()));
    //scrollArea =new QScrollBar();

    //linker = Linker::getInstance();

    //slot_modeOnOff(true);
    //------------------------------------------------
    connect(linker, SIGNAL(signal_dragDrop()), this, SLOT(slot_dragAndDropLoop()));

    print_debug();
    connect(this, SIGNAL(signal_playCurr_recomm()), this, SLOT(slot_playCurr_recomm()));

    connect(linker, SIGNAL(signal_queue_recent_track_add()), this, SLOT(slot_queue_recent_track_add_http_signal()));//cheon211206
    connect(linker, SIGNAL(signal_queuelist_musicStart()), this, SLOT(slot_queuelistPositionGet_http_signal()));
    //connect(linker, SIGNAL(signal_queuelist_reload()), this, SLOT(slot_queuelistPositionGet_http_signal()));

    connect(this, SIGNAL(signal_for_view_std_listWidget_one_lineItemUI()), this, SLOT(slot_for_view_std_listWidget_one_lineItemUI()));

    connect(btn_PlayPosition, SIGNAL(clicked()), this, SLOT(slot_queuelistCurrPlayIndexGet()));

    print_debug();

    only_replace_flag = false;;

    this->changeEditMode(true, false, false);
    reGet_checkSum();
    timer_reGetCeckSum = new QTimer(this);
    timer_reGetCeckSum->setInterval(1000); // 1초
    connect(timer_reGetCeckSum, SIGNAL(timeout()), SLOT(slot_reGet_checkSum()));
    //this->timer_reGetCeckSum->start();

}

void QueuePlayList::menu_click_init()
{
    // 메인메뉴 버튼
    clb_total = new ClickableLabel();//cheon210916-total
    clb_total->setImageResource(":/images/ico_filter.png");//cheon210916-total
    //clb_total->setImageResource(":/images/q_main_home_icon.png");//cheon210916-total
    clb_total->setImageResource(":/images/q_main_all_icon.png");//cheon210916-total
    //clb_total->setStyleSheet("background-color:transparent;border-radius:15px;");//cheon210916-total
    clb_total->setStyleSheet("ClickableLabel { background-color:transparent;  border-radius:15px;} ClickableLabel:hover {background-color: #505050; color: white;} ClickableLabel:pressed{background-color: #B18658;} ClickableLabel:checked{background-color: #B18658;border:none;} QToolTip{ color: #404040; }");

    clb_total->setFixedSize(Q_MAIN_ICON_W,Q_MAIN_ICON_H);//cheon210916-total
    clb_music = new ClickableLabel();
    clb_music->setImageResource(":/images/q_main_music_icon.png");
    //clb_music->setStyleSheet("background-color:transparent;border-radius:15px;");
    clb_music->setStyleSheet("ClickableLabel { background-color:transparent;  border-radius:15px;} ClickableLabel:hover {background-color: #505050; color: white;} ClickableLabel:pressed{background-color: #B18658;} ClickableLabel:checked{background-color: #B18658;border:none;} QToolTip{ color: #404040; }");

    clb_music->setFixedSize(Q_MAIN_ICON_W,Q_MAIN_ICON_H);
    clb_video = new ClickableLabel();
    clb_video->setImageResource(":/images/q_main_video_icon.png");
    //clb_video->setStyleSheet("background-color:transparent;border-radius:15px;");
    clb_video->setStyleSheet("ClickableLabel { background-color:transparent;  border-radius:15px;} ClickableLabel:hover {background-color: #505050; color: white;} ClickableLabel:pressed{background-color: #B18658;} ClickableLabel:checked{background-color: #B18658;border:none;} QToolTip{ color: #404040; }");

    clb_video->setFixedSize(Q_MAIN_ICON_W,Q_MAIN_ICON_H);
    clb_radio = new ClickableLabel();
    clb_radio->setImageResource(":/images/q_main_radio_icon.png");
    //clb_radio->setStyleSheet("background-color:transparent;border-radius:15px;");
    clb_radio->setStyleSheet("ClickableLabel { background-color:transparent;  border-radius:15px;} ClickableLabel:hover {background-color: #505050; color: white;} ClickableLabel:pressed{background-color: #B18658;} ClickableLabel:checked{background-color: #B18658;border:none;} QToolTip{ color: #404040; }");

    clb_radio->setFixedSize(Q_MAIN_ICON_W,Q_MAIN_ICON_H);
    clb_fmtuner = new ClickableLabel();
    clb_fmtuner->setImageResource(":/images/q_main_fm_icon.png");
    clb_fmtuner->setStyleSheet("background-color:transparent;border-radius:15px;");
    clb_music->setStyleSheet("ClickableLabel { background-color:transparent;  border-radius:15px;} ClickableLabel:hover {background-color: #505050; color: white;} ClickableLabel:pressed{background-color: #B18658;} ClickableLabel:checked{background-color: #B18658;border:none;} QToolTip{ color: #404040; }");

    clb_fmtuner->setFixedSize(Q_MAIN_ICON_W,Q_MAIN_ICON_H);
    clb_roseTube = new ClickableLabel();
    clb_roseTube->setImageResource(":/images/q_main_yube_icon.png");
    //clb_roseTube->setStyleSheet("background-color:transparent;border-radius:15px;");
    clb_roseTube->setStyleSheet("ClickableLabel { background-color:transparent;  border-radius:15px;} ClickableLabel:hover {background-color: #505050; color: white;} ClickableLabel:pressed{background-color: #B18658;} ClickableLabel:checked{background-color: #B18658;border:none;} QToolTip{ color: #404040; }");

    clb_roseTube->setFixedSize(Q_MAIN_ICON_W,Q_MAIN_ICON_H);
    clb_potcast = new ClickableLabel();
    clb_potcast->setImageResource(":/images/q_main_cast_icon.png");
    //clb_potcast->setStyleSheet("background-color:transparent;border-radius:15px;");
    clb_potcast->setStyleSheet("ClickableLabel { background-color:transparent;  border-radius:15px;} ClickableLabel:hover {background-color: #505050; color: white;} ClickableLabel:pressed{background-color: #B18658;} ClickableLabel:checked{background-color: #B18658;border:none;} QToolTip{ color: #404040; }");

    clb_potcast->setFixedSize(Q_MAIN_ICON_W,Q_MAIN_ICON_H);
    clb_tidal = new ClickableLabel();
    clb_tidal->setImageResource(":/images/q_tidal_icon.png");
    //clb_tidal->setStyleSheet("background-color:transparent;border-radius:15px;");
    clb_tidal->setStyleSheet("ClickableLabel { background-color:transparent;  border-radius:15px;} ClickableLabel:hover {background-color: #505050; color: white;} ClickableLabel:pressed{background-color: #B18658;} ClickableLabel:checked{background-color: #B18658;border:none;} QToolTip{ color: #404040; }");

    clb_tidal->setFixedSize(Q_MAIN_ICON_W,Q_MAIN_ICON_H);
    clb_bugs = new ClickableLabel();
    clb_bugs->setImageResource(":/images/q_bugs_icon.png");
    //clb_bugs->setStyleSheet("background-color:transparent;border-radius:15px;");
    clb_bugs->setStyleSheet("ClickableLabel { background-color:transparent;  border-radius:15px;} ClickableLabel:hover {background-color: #505050; color: white;} ClickableLabel:pressed{background-color: #B18658;} ClickableLabel:checked{background-color: #B18658;border:none;} QToolTip{ color: #404040; }");

    clb_bugs->setFixedSize(Q_MAIN_ICON_W,Q_MAIN_ICON_H);
    clb_qobuz = new ClickableLabel();
    clb_qobuz->setImageResource(":/images/q_qobuz_icon.png");
    //clb_qobuz->setStyleSheet("background-color:transparent;border-radius:15px;");
    clb_qobuz->setStyleSheet("ClickableLabel { background-color:transparent;  border-radius:15px;} ClickableLabel:hover {background-color: #505050; color: white;} ClickableLabel:pressed{background-color: #B18658;} ClickableLabel:checked{background-color: #B18658;border:none;} QToolTip{ color: #404040; }");

    clb_qobuz->setFixedSize(Q_MAIN_ICON_W,Q_MAIN_ICON_H);
    clb_apple = new ClickableLabel();
    clb_apple->setImageResource(":/images/apple/main_icon_apple.png");
    //clb_apple->setStyleSheet("background-color:transparent;border-radius:15px;");
    clb_apple->setStyleSheet("ClickableLabel { background-color:transparent;  border-radius:15px;} ClickableLabel:hover {background-color: #505050; color: white;} ClickableLabel:pressed{background-color: #B18658;} ClickableLabel:checked{background-color: #B18658;border:none;} QToolTip{ color: #404040; }");

    clb_apple->setFixedSize(Q_MAIN_ICON_W,Q_MAIN_ICON_H);


    clb_total->setProperty("currMenuCode", CURRMENU_TOTAL);//cheon210916-total
    clb_music->setProperty("currMenuCode", CURRMENU_MUSIC);
    clb_video->setProperty("currMenuCode", CURRMENU_VIDEO);
    clb_radio->setProperty("currMenuCode", CURRMENU_RADIO);
    clb_fmtuner->setProperty("currMenuCode", CURRMENU_FMTUNER);
    clb_roseTube->setProperty("currMenuCode", CURRMENU_ROSETUBE);
    clb_potcast->setProperty("currMenuCode", CURRMENU_POTCAST);
    clb_tidal->setProperty("currMenuCode", CURRMENU_TIDAL);
    clb_bugs->setProperty("currMenuCode", CURRMENU_BUGS);
    clb_qobuz->setProperty("currMenuCode", CURRMENU_QOBUZ);
    clb_apple->setProperty("currMenuCode", CURRMENU_APPLE);

}

void  QueuePlayList::pixmap_init(){
    pixmap_type_music = GSCommon::getUIPixmapImg(":/images/queue/q_mus_play_ico.png");
    pixmap_type_video = GSCommon::getUIPixmapImg(":/images/queue/q_video_play_ico.png");
    pixmap_type_rosetube = GSCommon::getUIPixmapImg(":/images/queue/q_tube_play_ico.png");
    pixmap_type_podcast = GSCommon::getUIPixmapImg(":/images/queue/q_pod_play_ico.png");
    pixmap_type_tidal = GSCommon::getUIPixmapImg(":/images/queue/q_tidal_play_ico.png");
    pixmap_type_bugs = GSCommon::getUIPixmapImg(":/images/queue/q_bugs_play_ico.png");
    pixmap_type_qobuz = GSCommon::getUIPixmapImg(":/images/queue/q_qobuz_play_ico.png");
    pixmap_type_apple = GSCommon::getUIPixmapImg(":/images/queue/q_apple_play_ico.png");
    pixmap_type_cloud = GSCommon::getUIPixmapImg(":/images/queue/q_cloud_play_ico.png");
    pixmap_type_network = GSCommon::getUIPixmapImg(":/images/queue/q_net_play_ico.png");
    pixmap_type_local = GSCommon::getUIPixmapImg(":/images/queue/q_local_play_ico.png");
    pixmap_type_audioQuality = GSCommon::getUIPixmapImg(":/images/qobuz/hires_ico.png");
    pixmap_type_mqa = GSCommon::getUIPixmapImg(":/images/tidal/mqa_ico.png");
    pixmap_type_flac = GSCommon::getUIPixmapImg(":/images/flac_mini.png");

}

void QueuePlayList::slot_queue_recent_track_add_http_signal(){
    print_debug();
    if(listWidget_total[global.now_Section]->count() > current_queue_cnt){//c220629
        int p_id = currMenutoPid();
        QJsonObject tmp_json2;//cheon211114-01
        NetworkHttp *network_get2 = new NetworkHttp;//cheon211114-01

        connect(network_get2, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_recomm(int, QJsonObject)));

        if(p_id == HTTP_CURR_QUEUE_GET_TOTAL){//cheon211114-01
            print_debug();//HTTP_CURR_QUEUE_GET_RECOMM_TOTAL
            network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_TOTAL, QString("http://%1:%2/%3")//cheon211104-01
                                  .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01

        }else if(p_id == HTTP_CURR_QUEUE_GET_MUSIC ){
            print_debug();
            network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_MUSIC, QString("http://%1:%2/%3")//cheon211104-01
                                  .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01

        }else if(p_id==HTTP_CURR_QUEUE_GET_ROSETUBE ){
            print_debug();
            network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_ROSETUBE, QString("http://%1:%2/%3")//cheon211104-01
                                  .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01

        }else if(p_id==HTTP_CURR_QUEUE_GET_TIDAL){
            print_debug();
            network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_TIDAL, QString("http://%1:%2/%3")//cheon211104-01
                                  .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01
            print_debug();
        }else if(p_id==HTTP_CURR_QUEUE_GET_BUGS){
            print_debug();
            network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_BUGS, QString("http://%1:%2/%3")//cheon211104-01
                                  .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01


        }else if(p_id==HTTP_CURR_QUEUE_GET_QOBUZ ){//REMOTE_TOTAL_QUEUE_GET
            print_debug();
            network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_QOBUZ, QString("http://%1:%2/%3")//cheon211104-01
                                  .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01


        }else if(p_id==HTTP_CURR_QUEUE_GET_APPLEMUSIC ){//REMOTE_TOTAL_QUEUE_GET
            print_debug();
            network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_APPLEMUSIC, QString("http://%1:%2/%3")//cheon211104-01
                                  .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01


        }
        print_debug();
    }
    print_debug();
}
void QueuePlayList::listWidget_total_setShow(const int &no){
    print_debug();
    for(int i = 0;i < 10;i++){
        if(i == no){
            listWidget_total[i]->show();
        }else{
            listWidget_total[i]->hide();
        }
    }
}

ListWidget *QueuePlayList::setListWidget(){
    ListWidget *listWidget = new ListWidget(this);
    //listWidget->setItemDelegate(delegate);
    listWidget->setCursor(Qt::PointingHandCursor);

    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setStyleSheet("background-color:#222222;");

    listWidget->verticalScrollBar()->

            setStyleSheet(QString::fromUtf8("QScrollBar:vertical {"
                                            "   width: 8px;"
                                            "   background: rgba (0,0,0,0%);"
                                            "   margin: 0px, 0px, 0px, 0px;"
                                            "   padding-top: 9px;"//set aside 9px for the arrows above and below
                                            "   padding-bottom: 9px;"
                                            "}"
                                            "QScrollBar :: handle: vertical"
                                            "{"
                                            "   width:8px;"
                                            "   background: rgba (0,0,0,25%);"
                                            "   border-radius: 4px;"//Both ends of the scroll bar become ellipses
                                            "   min-height: 20 px;"
                                            "}"
                                            " QScrollBar :: handle: vertical: hover"
                                            " {"
                                            "     width: 8px;"
                                            "     background: rgba (0,0,0,50%);"//When the mouse is placed on the scroll bar, the color becomes darker
                                            "     border-radius: 4px;"
                                            "     min-height: 20 px;"
                                            " }"));

    //this->listWidget->verticalScrollBar()->setPageStep(1000);//c220620
    //this->listWidget->verticalScrollBar()->setSingleStep(1000);//c220620
    listWidget->setMouseTracking(true);//c220620
    listWidget->setStyleSheet("QListWidget::item:hover {background-color:#111111;}");//c220620
    return listWidget;
}

void QueuePlayList::slot_sliderReleased(){
    /*
    print_debug();
    qDebug() << "Released----------------------------------------------";
    qDebug() << "this->scrollArea->verticalScrollBar()->sliderPosition()=" << listWidget_total[global.now_Section]->verticalScrollBar()->sliderPosition();
    this->CurrViewRow =  listWidget_total[global.now_Section]->verticalScrollBar()->sliderPosition();
    for(int i = 0; i < global.LmtCnt_H/70*4+3; i++){
        if(this->CurrViewRow+i < current_queue_cnt){
            QLayout* layout = widget[global.now_Section].at(this->CurrViewRow+i)->layout();
            if (layout == 0){
                downloadThumbImageMore_playbarChanged(this->CurrViewRow+i);
                this->std_listWidget_one_lineItemUI(this->CurrViewRow+i);
            }
        }
    }
    print_debug();
    mouse_all_clear();
   // mouse_clickable_state_print();
   */
    slot_listwidgetcurrentRowChanged(this->listWidget_total[global.now_Section]->verticalScrollBar()->sliderPosition());  //c221004_1

}

void QueuePlayList::listwidgetcurrentRow_append(const int &p_index){
    QObject::disconnect(disConSelectId[p_index]);

    //widget[global.now_Section].append(new QWidget());
    widget[global.now_Section].insert(p_index, new QWidget());

    //lb_queueListImg[global.now_Section].append(new QLabel());
    lb_queueListImg[global.now_Section].insert(p_index, new QLabel());

    queue_setListWidget(p_index);

    //checkbox_Selected[global.now_Section].append(new QCheckBox());
    checkbox_Selected[global.now_Section].insert(p_index, new QCheckBox());
    checkbox_Selected[global.now_Section].at(p_index)->setProperty("check_index", p_index);
    checkbox_Selected[global.now_Section].at(p_index)->setStyleSheet(
                "#checkbox_Selected { background-color:transparent;spacing:0px;}"
                "QCheckBox::indicator:unchecked { image: url(:images/playlist/list_check_off.png);}"
                "QCheckBox::indicator:checked { image: url(:images/playlist/list_check_on.png);}"
                );
    //checkbox_Selected->setCursor(Qt::PointingHandCursor);
    checkbox_Selected[global.now_Section].at(p_index)->setFixedSize(40,33);
    disConSelectId[p_index] = connect(checkbox_Selected[global.now_Section].at(p_index), SIGNAL(stateChanged(int)), this, SLOT(slot_clickedSelected(int)));

}



void QueuePlayList::slot_listwidgetcurrentRowChanged(int row){  //c220620
    print_debug();

    qDebug() << "row=" << row;

    //emit linker->signal_checkQueue(17, "");
    //qDebug() << "current_queue_cnt--------==" << current_queue_cnt;
    //qDebug() << "this->list_dataTrackForDelegate[global.now_Section].count()--------==" << this->list_dataTrackForDelegate[global.now_Section].count();

    //qDebug() << "this->CurrViewRow--------==" << this->CurrViewRow;
    //qDebug() << "this->PreViewRow--------==" << this->PreViewRow;
    //qDebug() << "this->indexCurrPlay[global.now_Section]--------==" << this->indexCurrPlay[global.now_Section];

    //int index = get_fromABSNextIndexNo_toSUBIndex(this->indexCurrPlay[global.now_Section]);
    //if(index < 0){
    //    index = 0;
    //}
    //qDebug() << "index--------==" << index;
    int sliderPosition = this->listWidget_total[global.now_Section]->verticalScrollBar()->sliderPosition();
    curr_sliderPosition = sliderPosition;
    qDebug() << "################this->listWidget_total[global.now_Section]->verticalScrollBar()->sliderPosition()=" << this->listWidget_total[global.now_Section]->verticalScrollBar()->sliderPosition();
    int rrow = row;//c220624
#if defined(Q_OS_WINDOWS)

#endif
#if defined(Q_OS_MAC)
    rrow = rrow/70;//c220624
#endif
    if(rrow<0||rrow>=current_queue_cnt) rrow = 0;//c220627

    this->CurrViewRow = rrow;
    //int compare = this->CurrViewRow - this->PreViewRow;
    int compare = curr_sliderPosition - pre_sliderPosition;
    qDebug() << "#################################-curr_sliderPosition= " << curr_sliderPosition;
    qDebug() << "#################################-pre_sliderPosition= " << pre_sliderPosition;
    qDebug() << "#################################-compare= " << compare;

    if(listWidget_total[global.now_Section]->count() > 0 && widget[global.now_Section].count() > 0){

        if(compare >=0 ){
            if(compare > global.LmtCnt_H/70*4+3) {
                //this->PreViewRow = rrow;
                print_debug();
                mouse_all_clear();
                pre_sliderPosition = sliderPosition;
                return;
            }
            print_debug();
            if(listWidget_total[global.now_Section]->count()> global.LmtCnt_H/70*2){
                for(int i = 0; i < global.LmtCnt_H/70*4+3; i++){
                    if(widget[global.now_Section].count() > this->CurrViewRow+i && 0 <= this->CurrViewRow+i){
                        //listwidgetcurrentRow_append(this->CurrViewRow+i);
                        if(this->CurrViewRow+i >= 0 && this->CurrViewRow+i < current_queue_cnt){
                            //qDebug() << "i=" << i;
                            //qDebug() << "this->CurrViewRow+i=" << this->CurrViewRow+i;
                            QLayout* layout = widget[global.now_Section].at(this->CurrViewRow+i)->layout();
                            //print_debug();
                            if (layout == 0){
                                //listwidgetcurrentRow_append(i);
                                downloadThumbImageMore_playbarChanged(this->CurrViewRow+i);
                                this->std_listWidget_one_lineItemUI(this->CurrViewRow+i);
                            }else{
                                // print_debug();
                            }
                        }
                    }
                }
            }else{
                for(int i = 0; i < global.LmtCnt_H/70*2+3; i++){
                    if(widget[global.now_Section].count() > this->CurrViewRow+i && 0 <= this->CurrViewRow+i){
                        //listwidgetcurrentRow_append(this->CurrViewRow+i);
                        if(this->CurrViewRow+i >= 0 && this->CurrViewRow+i < current_queue_cnt){
                            //qDebug() << "this->CurrViewRow+i=" << this->CurrViewRow+i;
                            QLayout* layout = widget[global.now_Section].at(this->CurrViewRow+i)->layout();
                            if (layout == 0){
                                //listwidgetcurrentRow_append(i);
                                downloadThumbImageMore_playbarChanged(this->CurrViewRow+i);
                                this->std_listWidget_one_lineItemUI(this->CurrViewRow+i);
                            }else{
                                // print_debug();
                            }
                        }
                    }
                }
            }
        }else{
            //c1111//widget[global.now_Section].count() > this->PreViewRow-i &&
            print_debug();
            if(listWidget_total[global.now_Section]->count()> global.LmtCnt_H/70*2){
                for(int i = 0; i < global.LmtCnt_H/70*4+3; i++){
                    if(widget[global.now_Section].count() > this->PreViewRow-i &&0 <= this->PreViewRow-i){
                        if(this->PreViewRow-i >= 0 && this->PreViewRow-i < current_queue_cnt){
                            //listwidgetcurrentRow_append(this->PreViewRow-i);
                            QLayout* layout = widget[global.now_Section].at(this->PreViewRow-i)->layout();
                            if (layout == 0){

                                downloadThumbImageMore_playbarChanged(this->PreViewRow-i);
                                this->std_listWidget_one_lineItemUI(this->PreViewRow-i);
                            }else{
                                // print_debug();
                            }
                        }
                    }
                }
            }else{
                for(int i = 0; i < global.LmtCnt_H/70*2+3; i++){
                    if(widget[global.now_Section].count() > this->PreViewRow-i &&0 <= this->PreViewRow-i){
                        if(this->PreViewRow-i >= 0 && this->PreViewRow-i < current_queue_cnt){
                            //listwidgetcurrentRow_append(this->PreViewRow-i);
                            QLayout* layout = widget[global.now_Section].at(this->PreViewRow-i)->layout();
                            if (layout == 0){

                                downloadThumbImageMore_playbarChanged(this->PreViewRow-i);
                                this->std_listWidget_one_lineItemUI(this->PreViewRow-i);
                            }else{
                                // print_debug();
                            }
                        }
                    }
                }
            }


        }
        this->PreViewRow = rrow;
        print_debug();
        mouse_all_clear();
        //mouse_clickable_state_print();

    }

    pre_sliderPosition = sliderPosition;

}

void QueuePlayList::slot_listwidgetcurrentRowChanged_start(int row){  //c220620
    print_debug();

    qDebug() << "current_queue_cnt--------==" << current_queue_cnt;
    //qDebug() << "this->list_dataTrackForDelegate[global.now_Section].count()--------==" << this->list_dataTrackForDelegate[global.now_Section].count();

    //qDebug() << "this->CurrViewRow--------==" << this->CurrViewRow;
    //qDebug() << "this->PreViewRow--------==" << this->PreViewRow;
    qDebug() << "this->indexCurrPlay[global.now_Section]--------==" << this->indexCurrPlay[global.now_Section];
    //int index = get_fromABSNextIndexNo_toSUBIndex(row);
    int index = row;
    if(index < 0){
        index = 0;
    }
    //index = index;
    qDebug() << "----------------------------------------------------------------------index--------==" << index;
    qDebug() << "global.LmtCnt_H/70--------==" << global.LmtCnt_H/70;
    qDebug() << "global.now_Section--------==" << global.now_Section;

    if(listWidget_total[global.now_Section]->count() > 0 && widget[global.now_Section].count() > 0){

        qDebug() << "this->CurrViewRow--------==" << this->CurrViewRow;
        int temp = index-global.LmtCnt_H/70+1;
        if(temp < 0) temp = 0;
        this->CurrViewRow = temp;
        qDebug() << "--------------------------------------------------- temp = " << temp;
        for(int i = temp; i < temp+global.LmtCnt_H/70*2+2; i++){
            //listwidgetcurrentRow_append(i);
            //print_debug();qDebug() << "----------------------------------- i = " << i;
            if(i < current_queue_cnt && widget[global.now_Section].count() > i){
                QLayout* layout = widget[global.now_Section].at(i)->layout();
                if (layout == 0){

                    downloadThumbImageMore_playbarChanged(i);
                    this->std_listWidget_one_lineItemUI(i);
                }else{
                    // print_debug();
                }
            }
        }


        this->PreViewRow = this->CurrViewRow;

        //listWidget_total[global.now_Section]->setCurrentRow(index);

    }
    print_debug();
    queuelist_change_replace(false, current_queue_cnt);

}

void QueuePlayList::slot_queuelistCurrPlayIndexGet_singleshot(){//c220603
    // QTimer::singleShot(3000, this, SLOT(slot_queuelistCurrPlayIndexGet()));
}

void QueuePlayList::slot_reGet_checkSum(){
    //print_debug();
    QJsonObject tmp_json4;
    NetworkHttp *network4 = new NetworkHttp;
    connect(network4, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_checksum(int, QJsonObject)));
    tmp_json4.insert("roseToken", global.device.getDeviceRoseToken());
    network4->request(HTTP_CURR_QUEUE_GET_CHECKSUM_TOTAL, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_GET_CHECKSUM), tmp_json4, true, true);

}

void QueuePlayList::slot_checking_currPlay(){
    print_debug();
    if(this->indexPrePlay[tabSectionlist[global.now_Section]] != this->indexCurrPlay[tabSectionlist[global.now_Section]]){
        print_debug();
        equelRecieveJsonFlag[global.now_Section] = true;
        if(this->currMenu == CURRMENU_TOTAL){
            current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[0]].count();
        }else if(this->currMenu == CURRMENU_QOBUZ){
            current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[5]].count();
        }else if(this->currMenu == CURRMENU_BUGS){
            current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[4]].count();
        }else if(this->currMenu == CURRMENU_TIDAL){
            current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[3]].count();
        }else if(this->currMenu == CURRMENU_ROSETUBE){
            current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[2]].count();
        }else if(this->currMenu == CURRMENU_MUSIC){
            current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[1]].count();
        }else if(this->currMenu == CURRMENU_APPLE){
            current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[6]].count();
        }

        QLocale aEnglish;
        lb_title_cnt->setText(tr("%1 / 3,000").arg(aEnglish.toString(this->current_queue_cnt)));
        print_debug();
        queuelist_change_replace(false, current_queue_cnt);

        emit linker->signal_checkQueue(6, "");
    }else{
        queueReflash_checking(currMenutoPid());
        emit linker->signal_checkQueue(6, "");
        print_debug();
        mouse_all_clear();
    }
}

void QueuePlayList::slot_queuelistPositionGet_checking_currPlay(){
    //
    print_debug();
    //qDebug() << "checking_curr_play_index = " << checking_curr_play_index;
    if(global.queueTimerFlag && listWidget_total[global.now_Section]->count() > 0 && mouse_trigger_menu_flag ){
        print_debug();
        reloadList();
    }
}

void QueuePlayList::slot_editModeClicked(QListWidgetItem *item){  //1126


    print_debug();
    qDebug() << "this->list_dataTrackForDelegate = " << this->list_dataTrackForDelegate[global.now_Section].count();

    qDebug() << "= " << this->currMenu;

    int tmp_index = listWidget_total[global.now_Section]->row(item);
    //if(this->currMenu == CURRMENU_TOTAL){
    qDebug() << "QueuePlayList::slot_editModeClicked---tmp_index" << tmp_index;
    DataTrackforDelegate delchecked_item = this->list_dataTrackForDelegate[global.now_Section].at(tmp_index);
    if(delchecked_item.isDelChecked == false ){

        DataTrackforDelegate delchecked_item = this->list_dataTrackForDelegate[global.now_Section].at(tmp_index);
        delchecked_item.isDelChecked = true;
        this->list_dataTrackForDelegate[global.now_Section].replace(tmp_index,delchecked_item);




    }else if(delchecked_item.isDelChecked == true){
        DataTrackforDelegate delchecked_item = this->list_dataTrackForDelegate[global.now_Section].at(tmp_index);
        delchecked_item.isDelChecked = false;

        this->list_dataTrackForDelegate[global.now_Section].replace(tmp_index,delchecked_item);


    }else{
        print_debug();
    }
    print_debug();
    qDebug() << "= " << this->currMenu;
    print_debug();

}

void QueuePlayList::slot_setWidgetColor(int i){
    print_debug();
    widget[global.now_Section].at(i)->setStyleSheet(widget[global.now_Section].at(i)->styleSheet().replace("#111111", "#222222"));
    print_debug();
}
void QueuePlayList::slot_setWidgetColor_recomm(int i){
    print_debug();
    widget_recomm[global.now_Section].at(i)->setStyleSheet(widget_recomm[global.now_Section].at(i)->styleSheet().replace("#111111", "#222222"));
    print_debug();
}

void QueuePlayList::itemPressesBackground(int i){
    print_debug();
    qDebug() << "i=" << i;
    qDebug() << " widget[global.now_Section].count()=" <<  widget[global.now_Section].count();
    widget[global.now_Section].at(i)->setStyleSheet(widget[global.now_Section].at(i)->styleSheet().replace("#222222", "#111111"));
    print_debug();
    QTimer::singleShot(100, this, SLOT(slot_setWidgetColor(i)));
}

void QueuePlayList::itemPressesBackground_recomm(int i){
    print_debug();
    widget_recomm[global.now_Section].at(i)->setStyleSheet(widget_recomm[global.now_Section].at(i)->styleSheet().replace("#222222", "#111111"));
    print_debug();
    QTimer::singleShot(100, this, SLOT(slot_setWidgetColor_recomm(i)));
}


void QueuePlayList::slot_listwidgetItemPressed(QListWidgetItem *item){  //cheon210916-total
    print_debug();
    emit linker->signal_clickListBtn();
    qDebug() << "this->list_dataTrackForDelegate[global.now_Section].count()= "<< this->list_dataTrackForDelegate[global.now_Section].count();
    qDebug() << "current_queue_cnt= "<< current_queue_cnt;
    qDebug() << "global.now_Section= "<< global.now_Section;
    int tmp_index = listWidget_total[global.now_Section]->row(item);
    qDebug() << "tmp_index=" << tmp_index;
    if(0 <= tmp_index && tmp_index < this->list_dataTrackForDelegate[global.now_Section].count()){
        print_debug();
        if(this->mouse_trigger_flag == false){
            emit linker->signal_checkQueue(7, "");
            //ContentLoadingwaitingMsgShow(tr("Please wait... Downloading Queuelist."), 1);
            print_debug();
            return;
        }

        if(!isEditMode && !isMoveMode && isPlayMode ){

            print_debug();

            itemPressesBackground(tmp_index);
            print_debug();

        }
    }
    else if(tmp_index > this->list_dataTrackForDelegate[global.now_Section].count()){
        print_debug();
        if(this->mouse_trigger_menu_flag == false){
            emit linker->signal_checkQueue(7, "");
            //ContentLoadingwaitingMsgShow(tr("Please wait... Downloading Queuelist."), 1);
            print_debug();
            return;
        }
        if(!isEditMode && !isMoveMode && isPlayMode ){

            print_debug();
            qDebug() << "tmp_index - (current_queue_cnt + 1) = " << tmp_index - (current_queue_cnt + 1);
            itemPressesBackground_recomm(tmp_index - (current_queue_cnt + 1) );
            print_debug();

        }
    }

}

/**
 * @brief QueuePlayList::slot_delegateClicked : [슬롯] 트렉정보 delegateClick
 * @details QlistWidget item click 처리<br>
 * @param p_index
 * @param p_btnType ItemClicked
 */
void QueuePlayList::slot_listwidgetItemClicked(QListWidgetItem *item){  //cheon210916-total
    print_debug();
    qDebug() << "this->mouse_trigger_flag = " << this->mouse_trigger_flag;
    qDebug() << "this->mouse_trigger_menu_flag = " << this->mouse_trigger_menu_flag;
    qDebug() << "this->isEditMode = " << this->isEditMode;
    qDebug() << "this->isMoveMode = " << this->isMoveMode;
    qDebug() << "this->list_dataTrackForDelegate[global.now_Section].count() = " << this->list_dataTrackForDelegate[global.now_Section].count();
    qDebug() << "this->widget[global.now_Section].count() = " << this->widget[global.now_Section].count();


    print_debug();
    if(this->isEditMode == true){
        print_debug();
        //emit signal_editmode(item);
        return;
    }else if(this->isMoveMode == true){
        print_debug();
        return;
    }
    int tmp_index = listWidget_total[global.now_Section]->row(item);
    qDebug() << "tmp_index" << tmp_index;
    //if(p_btnType == QueueItemDelegate::BtnType::etc){


    if(!this->mouse_trigger_flag){
        print_debug();
        emit linker->signal_checkQueue(7, "");
        return;
    }
    if(tmp_index < this->list_dataTrackForDelegate[global.now_Section].count()){
        print_debug();
        if(this->mouse_trigger_flag == false){
            emit linker->signal_checkQueue(7, "");
            //ContentLoadingwaitingMsgShow(tr("Please wait... Downloading Queuelist."), 1);
            print_debug();
            return;
        }

        this->playCurr(tmp_index);

    }
    else if(tmp_index == this->list_dataTrackForDelegate[global.now_Section].count()){
        print_debug();

    }
    else if(tmp_index > this->list_dataTrackForDelegate[global.now_Section].count()){
        print_debug();
        if(this->mouse_trigger_menu_flag == false){
            emit linker->signal_checkQueue(7, "");
            //ContentLoadingwaitingMsgShow(tr("Please wait... Downloading Queuelist."), 1);
            print_debug();
            return;
        }
        this->playCurr_recomm(tmp_index - (this->list_dataTrackForDelegate[global.now_Section].count()+1));
        /*
        if(total_queue_list_count ==3000){
            print_debug();
            widget_recomm[global.now_Section].at(tmp_index)->setStyleSheet("background-color:#9c9c9c;");
print_debug();
            print_debug();
            emit linker->signal_checkQueue(2, "");
            print_debug();
            playAfterDeleteIndex = tmp_index - (this->list_dataTrackForDelegate[global.now_Section].count()+1);
            print_debug();
            oneTrackDelete(0);
            print_debug();
        }else{
            print_debug();
            this->playCurr_recomm(tmp_index - (this->list_dataTrackForDelegate[global.now_Section].count()+1));
        }
*/

    }


}
void QueuePlayList::slot_clickedSelected(int ss){//1126
    //print_debug();
    //qDebug() << "ss = " << ss;
    QCheckBox *tmp_btn = dynamic_cast<QCheckBox *>(sender());
    //tmp_btn->setStyleSheet(tmp_btn->styleSheet().replace("transparent", "#B18658"));
    int check_index = tmp_btn->property("check_index").toInt();
    //qDebug() << "check_index = " << check_index;

    if(isEditMode == true && !isMoveMode && !isPlayMode ){
        if(checkbox_Selected[global.now_Section].at(check_index)->isChecked()){

            //print_debug();
            jsonArr_deletePositon.append(check_index);
            widget[global.now_Section].at(check_index)->setStyleSheet(widget[global.now_Section].at(check_index)->styleSheet().replace("#333333", "#222222"));

        }else{
            bool ff = false;
            for (int i = 0; i < jsonArr_deletePositon.size(); ++i){

                int vv = jsonArr_deletePositon[i].toInt();
                if(vv == check_index){
                    jsonArr_deletePositon.removeAt(i);
                    ff = true;
                }
            }
            if(ff){
                widget[global.now_Section].at(check_index)->setStyleSheet(widget[global.now_Section].at(check_index)->styleSheet().replace("#222222", "#333333"));

            }
        }
        lb_title_del->setText(QString(tr("%1 songs selected")).arg(jsonArr_deletePositon.count()));

    }
    mouse_all_clear();
    //slot_clickedDelete();
    /*
    this->isAllSelected = !this->isAllSelected;
    jsonArr_deletePositon = QJsonArray();
    if(this->isAllSelected==true){
        this->isAllSelected = false;
        // print_debug();
        for(int i=0; i<this->list_dataTrackForDelegate[global.now_Section].count(); i++){
            jsonArr_deletePositon.append(i);
            widget[global.now_Section].at(i)->setStyleSheet(widget[global.now_Section].at(i)->styleSheet().replace("#333333", "#222222"));
            widget[global.now_Section].at(i)->setStyleSheet(widget[global.now_Section].at(i)->styleSheet().replace("#e6e1a5", "#9c9c9d"));

        }
    }else{
        // print_debug();
        for(int i=0; i<this->list_dataTrackForDelegate[global.now_Section].count(); i++){
            widget[global.now_Section].at(i)->setStyleSheet(widget[global.now_Section].at(i)->styleSheet().replace("#222222", "#333333"));
            widget[global.now_Section].at(i)->setStyleSheet(widget[global.now_Section].at(i)->styleSheet().replace("#9c9c9d", "#e6e1a5"));
        }
    }
    //this->changeEditMode(false, false, true);
    */
}


/**
 * @brief QueuePlayList::slot_clickedAllSelected [SLOT] 전체선택 체크박스 클릭시
 */
void QueuePlayList::slot_clickedAllSelected(){//1126
    print_debug();
    qDebug() << "this->list_dataTrackForDelegate[global.now_Section].count() = " << this->list_dataTrackForDelegate[global.now_Section].count();

    this->isAllSelected = !this->isAllSelected;

    if(this->isAllSelected==true){

        jsonArr_deletePositon = QJsonArray();
        print_debug();
        for(int i=0; i<this->list_dataTrackForDelegate[global.now_Section].count(); i++){
            //qDebug() << i;
            checkbox_Selected[global.now_Section].at(i)->setChecked(true);
            QLayout* layout = widget[global.now_Section].at(i)->layout();
            if (layout != 0){
                //checkbox_Selected[global.now_Section].at(i)->setChecked(true);
                //jsonArr_deletePositon.append(i);
                widget[global.now_Section].at(i)->setStyleSheet(widget[global.now_Section].at(i)->styleSheet().replace("#333333", "#222222"));
                //widget[global.now_Section].at(i)->setStyleSheet(widget[global.now_Section].at(i)->styleSheet().replace("#e6e1a5", "#9c9c9d"));

            }else{
                print_debug();
            }

        }
    }else{
        jsonArr_deletePositon = QJsonArray();
        print_debug();
        for(int i=0; i<this->list_dataTrackForDelegate[global.now_Section].count(); i++){
            //qDebug() << i;
            QLayout* layout = widget[global.now_Section].at(i)->layout();
            if (layout != 0){
                checkbox_Selected[global.now_Section].at(i)->setChecked(false);
                //jsonArr_deletePositon.removeAt(i);
                widget[global.now_Section].at(i)->setStyleSheet(widget[global.now_Section].at(i)->styleSheet().replace("#222222", "#333333"));
                //widget[global.now_Section].at(i)->setStyleSheet(widget[global.now_Section].at(i)->styleSheet().replace("#9c9c9d", "#e6e1a5"));

            }else{
                print_debug();
            }


        }
    }
    lb_title_del->setText(QString(tr("%1 songs selected")).arg(jsonArr_deletePositon.count()));
    //lb_title_del->setText(QString(tr("%1 songs selected")).arg(this->list_dataTrackForDelegate[global.now_Section].count()));
    mouse_all_clear();
    //this->changeEditMode(false, false, true);
}

void QueuePlayList::slot_queue_question_popup(){
    dlgOutputQueueQuestion();
    QTimer::singleShot(500, this, SLOT(slot_showDlgOfQueueNotice()));

}

void QueuePlayList::dlgOutputQueueQuestion(){
    dlgConfirmQueue->setTitle(tr("QueuePlayList Notice"));
    dlgConfirmQueue->setText(tr("Multi-Move: Click one list item and drag-move it by clicking the desired block range item with the shift key again."));
    //dlgConfirmQueue->setStyleSheet("Text-align:left;");
}


void QueuePlayList::slot_showDlgOfQueueNotice(){

    dlgConfirmQueue->setGeometry(100+480,350+100, 500,500);
    dlgConfirmQueue->setAlertMode();
    dlgConfirmQueue->setProperty("flagShown",false);
    if(dlgConfirmQueue->property("flagShown").toBool()==false){
        dlgConfirmQueue->setProperty("flagShown",true);

        int result = dlgConfirmQueue->exec();

        if(result == QDialog::Accepted){
            //showRemote(true);

        }
    }
}


void QueuePlayList::slot_dragAndDropEnter(){
    print_debug();
    listWidget_total[global.now_Section]->setStyleSheet("QListWidget::item {border-bottom: 1px solid #707070;background-color:#222222; }");//transperant


}

void QueuePlayList::slot_dragAndDropLoop(){
    print_debug();
    this->drop_cnt_flag++;

}

void QueuePlayList::slot_dragAndDropEdit(){
    print_debug();
    if(this->drop_cnt_flag > 0 && !isEditMode && !isPlayMode && isMoveMode){
        this->drop_cnt_flag = 0;
        //this->btn_moveok->setStyleSheet(btn_moveok->styleSheet().replace("#707070", "#B18658"));
        //qDebug() << "QueuePlayList::slot_dragAndDropEdit---so_index = " << so_index;
        //qDebug() << "QueuePlayList::slot_dragAndDropEdit---tg_index = " << tg_index;
        jsonArr_editPositon  = QJsonArray();
        QJsonObject jsonObj_send;
        int p_id;
        if(this->currMenu == CURRMENU_TOTAL){
            p_id = HTTP_CURR_QUEUE_GET_TOTAL;
            jsonObj_send.insert("playType", "TOTAL");
        }else if(this->currMenu == CURRMENU_QOBUZ){
            p_id = HTTP_CURR_QUEUE_GET_QOBUZ;
            jsonObj_send.insert("playType", "QOBUZ");
        }else if(this->currMenu == CURRMENU_BUGS){
            p_id = HTTP_CURR_QUEUE_GET_BUGS;
            jsonObj_send.insert("playType", "BUGS");
        }else if(this->currMenu == CURRMENU_TIDAL){
            p_id = HTTP_CURR_QUEUE_GET_TIDAL;
            jsonObj_send.insert("playType", "TIDAL");
        }else if(this->currMenu == CURRMENU_ROSETUBE){
            p_id = HTTP_CURR_QUEUE_GET_ROSETUBE;
            jsonObj_send.insert("playType", "YOUTUBE");
        }else if(this->currMenu == CURRMENU_MUSIC){
            p_id = HTTP_CURR_QUEUE_GET_MUSIC;
            jsonObj_send.insert("playType", "MUSIC");
        }else if(this->currMenu == CURRMENU_APPLE){
            p_id = HTTP_CURR_QUEUE_GET_APPLEMUSIC;
            jsonObj_send.insert("playType", "APPLE_MUSIC");
        }

        if(this->queueList_draDropEditDialogConfirm()){
            print_debug();
            //delete listWidget_total[global.now_Section]->takeItem(so_index);

            //reloadList();
            for(int i = 0 ; i < this->list_dataTrackForDelegate[global.now_Section].count(); i++){
                //QStringList list = listWidget_total[global.now_Section]->item(i)->text().split(";
                //print_debug();
                //qDebug() << listWidget_total[global.now_Section]->item(i)->text().toInt();

                jsonArr_editPositon.append(listWidget_total[global.now_Section]->item(i)->text().toInt());
            }
            //jsonArr_editPositon.insert(tg_index, so_index);

            if(jsonArr_editPositon.count() > 0){
                print_debug();
                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_motify(int, QJsonObject)));

                // 모두 string 으로 보내야 함


                jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
                //qDebug() << "QueuePlayList::slot_dragAndDropEdit----global.checkSum = " << global.checkSum;
                jsonObj_send.insert("checkSum", global.checkSum);

                //
                jsonObj_send.insert("indexArr", jsonArr_editPositon);
                //jsonObj_send.insert("deleteArr", jsonArr_deleteArr);
                QJsonDocument doc(jsonObj_send);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::slot_dragAndDropEdit---" << strJson;
                qDebug() << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_modify");
                network->request(p_id,
                                 QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_modify"),
                                 jsonObj_send,
                                 true,
                                 true);

            }

        }else{
            print_debug();
            this->changeEditMode(true, false, false);
            emit linker->signal_checkQueue(0, "");

            force_reflash(p_id);
        }
    }else{
        this->changeEditMode(true, false, false);
        emit linker->signal_checkQueue(13, "");
        slot_clickedPlay();
    }
    //slot_clickedPlay();


}

void QueuePlayList::slot_listwidgetcurrentRowChanged(){  //cheon210916-total
    print_debug();
    qDebug() << "slot_listwidgetcurrentRowChanged ";
    //int tmp_index = listWidget_total[global.now_Section]->row(item);
    // qDebug() << "row=" << row;


}


void QueuePlayList::slot_listwidgetItemActivated(QListWidgetItem *item){  //cheon210916-total
    print_debug();
    qDebug() << "slot_listwidgetItemActivated ";
    int tmp_index = listWidget_total[global.now_Section]->row(item);
    qDebug() << "tmp_index=" << tmp_index;


}


void QueuePlayList::slot_listwidgetcurrentItemChanged(QListWidgetItem *curritem, QListWidgetItem *preitem){  //cheon210916-total
    print_debug();
    qDebug() << "slot_listwidgetcurrentItemChanged ";
    int currtmp_index = listWidget_total[global.now_Section]->row(curritem);
    qDebug() << "currtmp_index=" << currtmp_index;
    int pretmp_index = listWidget_total[global.now_Section]->row(preitem);
    qDebug() << "pretmp_index =" << pretmp_index;


}
void QueuePlayList::slot_listwidgetItemEntered(QListWidgetItem *item){  //cheon210916-total
    print_debug();
    qDebug() << "slot_listwidgetItemEntered ";
    int tmp_index = listWidget_total[global.now_Section]->row(item);
    qDebug() << "tmp_index=" << tmp_index;


}
void QueuePlayList::myFunction(QModelIndex Sindex,int S,int E,QModelIndex Dindex ,int Drow){
    qDebug() << "S = " << S;
    qDebug() << "E = " << E;
    qDebug() << "Drow = " << Drow;

}


void QueuePlayList::slot_httpservermsg(const QJsonObject &p_jsonObject){//c211213
    print_debug();
    QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"QueuePlayList::slot_httpservermsg---" << strJson;
    //QJsonObject tmp_json;
    qDebug() << this->islistOpen;
    qDebug() << "global.queueTimerFlag = " << global.queueTimerFlag;
    /*
    if(global.queueTimerFlag){
    //if(this->islistOpen == true){//c0114
        if(p_jsonObject.contains("dataObj")){//c211213
            QJsonObject tmp_Json = p_jsonObject["dataObj"].toObject();
            QString message = tmp_Json["message"].toString();//cheon211102
            QString messageType = tmp_Json["messageType"].toString();//cheon211102
            QString pop_type = tmp_Json["pop_type"].toString();//cheon211102
            print_debug();
            qDebug() << "temp_message= " << message;
            //if(message == "Requested quality is not allowed in user's subscription"){//c1217
            //if(pop_type == "1")
              //  emit linker->signal_checkQueue(9, message);
            //}


            if(message == "AVERROR_HTTP_NOT_FOUND" || message == "AVERROR_HTTP_REQUEST_TIMEOUT"
                    || message == "AVERROR_HTTP_SERVICE_UNAVAILABLE" || pop_type == "0"){
                print_debug();
                //emit linker->signal_checkQueue(9, message);
                qDebug() << "global.current_position = " << global.current_position;
                qDebug() << "global.current_duration = " << global.current_duration;

                int index_old = -1;
                DataTrackforDelegate tmp_dataTrack;
                for( int i = 0 ; i < this->list_dataTrackForDelegate[global.now_Section].count(); i++){
                    tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(i);
                    if(tmp_dataTrack.isPlay == true){
                        index_old = i;
                        break;
                    }
                }

                int p_index = index_old;
                //print_debug();
                qDebug() << "QueuePlayList::playCurr--index_old: " << index_old;
                qDebug() << "QueuePlayList::playCurr--p_index: " << p_index;
                qDebug() << "QueuePlayList::playCurr--this->list_dataTrackForDelegate[global.now_Section].count(): " << this->list_dataTrackForDelegate[global.now_Section].count();

                if(p_index>=0 && p_index<this->list_dataTrackForDelegate[global.now_Section].count()){
                    print_debug();
                    only_replace_flag = true;
                    view_listWidget_setItemWidget_replace(p_index, index_old);//old:배그라운드 원상복구, p_index: 황색으로
                }
                global.queuelist_mouse_trigger_flag = true;
                this->mouse_trigger_flag = true;
            }else{
                if( pop_type == "1"){
                    print_debug();

                }
            }

        }else if(p_jsonObject.contains("messageType")){
            QString message = p_jsonObject["messageType"].toString();
            //if(message)
            //emit linker->signal_checkQueue(9, message);

        }
    }

*/


}



void QueuePlayList::slot_httpservermsg_queue_play(const QJsonObject &p_jsonObject){//1126
    //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"HomeMain::slot_httpservermsg---" << strJson;
    //QJsonObject tmp_json;

    if(p_jsonObject.contains("messageType")){//cheon210812-iso
        print_debug();
        QString messageType = p_jsonObject["messageType"].toString();

        QString playerType = p_jsonObject["playerType"].toString();
        QString str="";
        if(messageType == "queue_play"){
            //queuelistMotify();//1126
            /*
            if(playerType == "BUGS"){
                str = "BUG_login_stat changed";
                //qDebug() << "BUG_login_stat changed";
            }
            else if(playerType == "TIDAL"){
                str = "TIDAL_login_stat changed";
                //qDebug() << "TIDAL_login_stat changed";
            }
            else if(playerType == "QOBUZ"){
                str = "QOBUZ_login_stat changed";
                //qDebug() << "QOBUZ_login_stat changed";
            }
            ToastMsg::show(this,"",str);
            */
        }

        //else if(messageType == "show_toast_noti"  ||messageType == "music_start"  || messageType == "queue_play"){//cheon211107-01
        //else if(messageType == "show_toast_noti" ){//cheon211107-01
        //    ToastMsg::show(this,"",messageType);
        //}

    }
}

QWidget* QueuePlayList::getUIControlOption(QString p_title){//cheon211122-02

    onOff = new OnOffWidget;
    onOff->setValue(true);
    onOff->setCursor(Qt::PointingHandCursor);
    QPushButton *btn_onOffHover = new QPushButton(onOff);
    btn_onOffHover->setProperty("btnNo", 4);
    btn_onOffHover->setFixedSize(80,40);
    btn_onOffHover->setStyleSheet("background-color:transparent;border:0px;");

    QLabel *label = new QLabel();
    label->setText(p_title);
    label->setStyleSheet("font-size: 24px; color: white; background-color:#5A5A5A;");

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(label);

    QWidget *widget = new QWidget();
    widget->setLayout(layout);

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,0,30,0);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(widget);
    //hl_lb->addWidget(btn_question);
    hl_lb->addWidget(onOff, 0, Qt::AlignRight);

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    //widget_total->setStyleSheet("#widget_total {background-color:#5A5A5A; border-bottom:1px solid #5A5A5A; } ");
    widget_total->setStyleSheet("background-color:#5A5A5A;  ");
    widget_total->setLayout(hl_lb);

    connect(btn_onOffHover, SIGNAL(clicked(bool)), this, SLOT(slot_modeOnOff(bool)));
    return widget_total;
}

void QueuePlayList::slot_modeOnOff(bool flagOn){//1126
    print_debug();
    if(flagOn==false){

    }else{

    }
    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_playRelateState(int, QJsonObject)));
    tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
    network->request(CURR_QUEUE_POST_PLAY_RELATE_STATE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_play_relate_state"), tmp_json, false, true);
}

void QueuePlayList::slot_responseHttp_playRelateState(const int &p_id, const QJsonObject &p_jsonObject){//cheon211122-01
    print_debug();
    QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::slot_responseHttp_playRelateState---" << strJson;
    switch(p_id){
    case CURR_QUEUE_POST_PLAY_RELATE_STATE://cheon211107-01
        print_debug();
        if(p_jsonObject.contains("flagOk")){
            if(p_jsonObject["flagOk"].toBool()==true){
                bool ispass = this->onOff->getCurrentValue();
                global.onOffModeFlag = ispass;
                if(ispass) {
                    print_debug();
                    this->onOff->setValue(false);
                }else{
                    print_debug();
                    this->onOff->setValue(true);
                }
            }
        }else{
            ToastMsg::show(this, "", tr("Failed to change recommanded play relate status."));
        }
        break;
    }
    sender()->deleteLater();
}

void QueuePlayList::slot_sliderMoved_notifySliderMove(int index){//cheon211120-3
    //print_debug();
    QScrollBar *vb1 = listWidget_total[global.now_Section]->verticalScrollBar();//cheon211114-01
    qDebug() << "QueuePlayList::slot_sliderMoved_notifySliderMove---index= " << index;
    qDebug() << "QueuePlayList::slot_sliderMoved_notifySliderMove--vb1->maximum()= " << vb1->maximum();

    //if(index+listWidget_total[global.now_Section]->height()/70 >= vb1->maximum()||index+(listWidget_total[global.now_Section]->height()+listWidget_recomm->height()+widget_top_recomm->height())/70 >= vb1->maximum()){

    if(index+listWidget_total[global.now_Section]->height()/70 >= vb1->maximum()){

    }
    sender()->deleteLater();

}

void QueuePlayList::slot_valueChanged_notifySliderMove(int index){//1126
    //print_debug();
    QScrollBar *vb1 = listWidget_total[global.now_Section]->verticalScrollBar();//cheon211114-01
    int currentRow = listWidget_total[global.now_Section]->currentRow();
    //qDebug() << "listWidget_height= " << listWidget_height;
    qDebug() << "QueuePlayList::slot_valueChanged_notifySliderMove---index= " << index;
    qDebug() << "QueuePlayList::slot_valueChanged_notifySliderMove--currentRow= " << currentRow;

    //if(index+listWidget_total[global.now_Section]->height()/70 >= vb1->maximum()||index+(listWidget_total[global.now_Section]->height()+listWidget_recomm->height()+widget_top_recomm->height())/70 >= vb1->maximum()){
    local_index = index;
    local_max = vb1->maximum();
    //if(this->currMenu == CURRMENU_TOTAL){//cheon211122-02
    //   if(index >= vb1->maximum()){
    //       print_debug();
    //   }else{
    //   }
    //}else{

    //}
    //sender()->deleteLater();
}

QJsonDocument QueuePlayList::loadJson(QString fileName) {
    QFile jsonFile(fileName);
    jsonFile.open(QFile::ReadOnly);
    return QJsonDocument().fromJson(jsonFile.readAll());
}

void QueuePlayList::saveJson(QJsonDocument document, QString fileName) {
    QFile jsonFile(fileName);
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(document.toJson());
}

void QueuePlayList::setTotalQueueList(int p_id, const QJsonObject &p_jsonObject){  //c220609
    print_debug();
    //mtx.lock();

    QJsonDocument doc(p_jsonObject); QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() << "QueuePlayList::setTotalQueueList---- " << strJson << "\n";


    /*
    QString jsonFilePath = qApp->applicationDirPath()+"/translations/prejson.json";
    saveJson(doc,jsonFilePath);
    //QJsonDocument predoc = loadJson(jsonFilePath);
*/

    qDebug() << "equelRecieveJsonFlag[global.now_Section] = " << equelRecieveJsonFlag[global.now_Section];

    //this->indexCurrPlay[global.now_Section] = -1;

    qDebug() << "pre----QueuePlayList::setTotalQueueList----this->indexCurrPlay[global.now_Section]= " << this->indexCurrPlay[global.now_Section];
    //this->indexCurrPlay[global.now_Section] = this->nextPlayNo;//assignindexCurrPlay[global.now_Section]
    this->noTotal_currntPlay = this->nextPlayNo;//assignindexCurrPlay[global.now_Section]
    print_debug();
    qDebug() << "next----QueuePlayList::setTotalQueueList----this->indexCurrPlay[global.now_Section]=  " << this->indexCurrPlay[global.now_Section];
    //lb_title->setText(tr("%1/3000").arg(total_queue_list_count));//cheon211114-01

    if(p_jsonObject.contains("flagOk")){
        if(p_jsonObject["flagOk"].toBool()==false){
            this->listWidget_total[global.now_Section]->clear();
            this->jsonArray_backup_total[global.now_Section] = QJsonArray();
            this->list_dataTrackForDelegate[global.now_Section].clear();
            return;
        }
    }

    bar_pos = this->indexCurrPlay[global.now_Section];
    if(p_jsonObject.contains("array")){
        QJsonArray tmp_arr = p_jsonObject["array"].toArray();
        qDebug() << "tmp_arr.count()=" << tmp_arr.count();
        if(this->jsonArray_backup_total[global.now_Section].count()>0 && tmp_arr==this->jsonArray_backup_total[global.now_Section]){

        }
        else{
            print_debug();

            this->jsonArray_backup_total[global.now_Section] = QJsonArray();
            this->jsonArray_backup_tidal = QJsonArray();
            this->jsonArray_backup_qobuz = QJsonArray();
            this->jsonArray_backup_bugs = QJsonArray();
            this->jsonArray_backup_music = QJsonArray();
            this->jsonArray_backup_youtube = QJsonArray();
            this->jsonArray_backup_apple = QJsonArray();
            if(mouse_trigger_menu_flag){
                //lb_queueListImg[global.now_Section].clear();//c220410
            }

            //this->listWidget_total[global.now_Section]->clear();//total
            //this->widget[global.now_Section].clear();
            //GSCommon::clearLayout(this->box_contents);

            for(int i =0 ; i < 7; i++){
                //  this->list_dataTrackForDelegate[tabSectionlist[i]].clear();
                //  this->list_thumbPath[tabSectionlist[i]].clear();
            }
            this->list_dataTrackForDelegate[global.now_Section].clear();
            this->list_thumbPath[global.now_Section].clear();
            this->list_dataTrackForDelegate[0].clear();
            this->list_thumbPath[0].clear();


            //this->lb_queueListImg_recomm[global.now_Section].clear();
            int k = 0;//cheon210916-total

            bool jsonprintFlag_Ap = true;
            bool jsonprintFlag_QO = true;
            bool jsonprintFlag_TI = true;
            bool jsonprintFlag_Bmvs = true;
            bool jsonprintFlag_Btrack = true;
            bool jsonprintFlag_YO = true;
            bool jsonprintFlag_MU = true;
            qint64 addtimeResult = 0;
            for(int i=0; i<tmp_arr.count(); i++){
                QJsonObject tmp_json1 = tmp_arr.at(i).toObject();
                //if(i==0){
                //    QJsonDocument doc(tmp_json1); QString strJson(doc.toJson(QJsonDocument::Compact));
                //    qDebug() << "QueuePlayList::setQobuzQueueList----tmp_json1== " << strJson << "\n";
                //}

                QString tmpStr1;
                if(tmp_json1.contains("PLAY_TAG")){
                    //print_debug();qDebug() << i;
                    tmpStr1 = tmp_json1["PLAY_TAG"].toString(); // 현재 진행중인 곡 스타일 설정
                }

                if(tmp_json1.contains("addTime")){
                    qint64 addtime = tmp_json1["addTime"].toDouble()/1000; // 현재 진행중인 곡 스타일 설정
                    qint64 currentSecsSinceEpoch = QDateTime::currentSecsSinceEpoch();//
                    //qint64 queueAddtimeSec = currentSecsSinceEpoch - addtime;
                    QDateTime t_local;
                    QDateTime t_localcurr = QDateTime::fromSecsSinceEpoch(currentSecsSinceEpoch);
                    //qint64  sect_local = t_local.secsTo(t_localcurr);
                    QDateTime time2 = QDateTime::fromSecsSinceEpoch(addtime);
                    addtimeResult = time2.secsTo(t_localcurr);
                    if(addtimeResult <= 0) {
                        addtimeResult = 1;
                    }

                }
                if(tmpStr1 ==  "APPLE_MUSIC"  && (p_id == HTTP_CURR_QUEUE_GET_APPLEMUSIC || p_id == HTTP_CURR_QUEUE_GET_TOTAL)){

                    if(tmp_json1.contains("track")){

                        //print_debug();
                        //qDebug() << "jsonprintFlag_Ap = " << jsonprintFlag_Ap;
                        if(jsonprintFlag_Ap==false){
                            QString tmp_string11 = tmp_json1["track"].toString();
                            //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_string : " << tmp_string11 << "\n";
                            //QJsonObject tmp_json = tmp_json1["track"].toObject();
                            //QJsonDocument doc(tmp_json); QString strJson(doc.toJson(QJsonDocument::Compact));
                            //qDebug() << "QueuePlayList::setQobuzQueueList--APPLE-tmp_json : " << strJson << "\n";
                            jsonprintFlag_Ap = false;

                        }
                        QString tmp_string = tmp_json1["track"].toString();
                        //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_string : " << tmp_string << "\n";
                        //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                        QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                        QJsonObject tmp_track = json.object(); // since your string is an JSON object

                        //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                        //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_track : " << strJson << "\n";
                        this->jsonArray_backup_apple.append(tmp_track);//cheon210916-total
                        this->jsonArray_backup_total[global.now_Section].append(tmp_track);//cheon210916-total
                        qobuz::TrackItemData_apple data_track = qobuz::ConvertData::make_trackData_fromAppleJsonObj(tmp_track);
                        //print_debug();
                        //qDebug() << "data_track.title = " << data_track.title;
                        // 1> 데이터 담기
                        DataTrackforDelegate tmp_dataTrack;
                        tmp_dataTrack.type = DataTrackforDelegate::Types_Data::AppleMusic;
                        tmp_dataTrack.addtimeResult = addtimeResult;
                        if(data_track.work.size() > 0){
                            tmp_dataTrack.title = data_track.title + " - " + data_track.work;
                        }else{
                            tmp_dataTrack.title = data_track.title;
                        }

                        tmp_dataTrack.artist = data_track.list_artist_name.join(",");
                        tmp_dataTrack.duration = data_track.duration;
                        if(data_track.hires){
                            tmp_dataTrack.audioQuality = "HI_RES";
                        }
                        //tmp_dataTrack.audioQuality = data_track.hires;
                        if(tmp_dataTrack.artist == ""){
                            tmp_dataTrack.artist = data_track.performer_name;
                        }
                        //qDebug() << "data_track.album_image_thumbnail = " << data_track.album_image_thumbnail;
                        //data_track.album_image_thumbnail.replace("{w}x{h}bb.jpg", QString("%1").arg(data_track.image_width)+"x"+QString("%1").arg(data_track.image_height)+"bb.jpg");
                        //data_track.album_image_thumbnail.replace("{w}x{h}bb.jpg", QString("%1").arg(100)+"x"+QString("%1").arg(100)+"bb.jpg");
                        data_track.album_image_thumbnail.replace("{w}x{h}", "60x60");

                        //data_track.album_image_thumbnail.replace("/{w}x{h}bb.jpg", "");
                        //qDebug() << "after-data_track.album_image_thumbnail = " << data_track.album_image_thumbnail;
                        tmp_dataTrack.thumbnail = data_track.album_image_thumbnail;
                        tmp_dataTrack.no = i;


                        if(i==this->indexCurrPlay[tabSectionlist[6]]){//cheon211104-01
                            print_debug();
                            //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                            //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_track : " << strJson << "\n";
                            qDebug() << "tmp_dataTrack.title= " << tmp_dataTrack.title;
                            //this->currMenu = "clb_qobuz";
                            this->noTotal_currntPlay = k;

                            tmp_dataTrack.isPlay = true;


                        }

                        QString tmp_imgPath = tmp_dataTrack.thumbnail;
                        if(tmp_imgPath.startsWith("http")==false){
                            tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                        }


                        this->list_thumbPath[tabSectionlist[6]].append(tmp_imgPath);
                        this->list_dataTrackForDelegate[tabSectionlist[6]].append(tmp_dataTrack);

                        // 2> UI에 데이터 세팅
                        this->appendTrackUI(k);//cheon210916-total
                        k++;//cheon210916-total

                    }
                }
                if(tmpStr1 ==  "APPLE_MUSIC"){

                    if(tmp_json1.contains("track")){

                        //print_debug();
                        //qDebug() << "jsonprintFlag_Ap = " << jsonprintFlag_Ap;
                        if(jsonprintFlag_Ap==false){
                            QString tmp_string11 = tmp_json1["track"].toString();
                            qDebug() << "QueuePlayList::setQobuzQueueList---tmp_string : " << tmp_string11 << "\n";
                            QJsonObject tmp_json = tmp_json1["track"].toObject();
                            QJsonDocument doc(tmp_json); QString strJson(doc.toJson(QJsonDocument::Compact));
                            qDebug() << "QueuePlayList::setQobuzQueueList--APPLE-tmp_json : " << strJson << "\n";
                            jsonprintFlag_Ap = false;

                        }
                        QString tmp_string = tmp_json1["track"].toString();
                        //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_string : " << tmp_string << "\n";
                        //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                        QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                        QJsonObject tmp_track = json.object(); // since your string is an JSON object

                        //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                        //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_track : " << strJson << "\n";
                        qobuz::TrackItemData_apple data_track = qobuz::ConvertData::make_trackData_fromAppleJsonObj(tmp_track);

                        // 1> 데이터 담기
                        DataTrackforDelegate tmp_dataTrack;
                        tmp_dataTrack.type = DataTrackforDelegate::Types_Data::AppleMusic;
                        tmp_dataTrack.addtimeResult = addtimeResult;
                        if(data_track.work.size() > 0){
                            tmp_dataTrack.title = data_track.title + " - " + data_track.work;
                        }else{
                            tmp_dataTrack.title = data_track.title;
                        }
                        tmp_dataTrack.artist = data_track.list_artist_name.join(",");
                        tmp_dataTrack.duration = data_track.duration;
                        if(data_track.hires){
                            tmp_dataTrack.audioQuality = "HI_RES";
                        }
                        //tmp_dataTrack.audioQuality = data_track.hires;
                        if(tmp_dataTrack.artist == ""){
                            tmp_dataTrack.artist = data_track.performer_name;
                        }

                        //data_track.album_image_thumbnail.replace("{w}x{h}bb.jpg", QString("%1").arg(data_track.image_width)+"x"+QString("%1").arg(data_track.image_height)+"bb.jpg");
                        //data_track.album_image_thumbnail.replace("{w}x{h}bb.jpg", QString("%1").arg(100)+"x"+QString("%1").arg(100)+"bb.jpg");
                        data_track.album_image_thumbnail.replace("{w}x{h}", "60x60");

                        //data_track.album_image_thumbnail.replace("/{w}x{h}bb.jpg", "");

                        tmp_dataTrack.thumbnail = data_track.album_image_thumbnail;
                        tmp_dataTrack.no = i;


                        if(i==this->indexCurrPlay[tabSectionlist[0]]){//cheon211104-01
                            print_debug();
                            //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                            //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_track : " << strJson << "\n";
                            qDebug() << "tmp_dataTrack.title= " << tmp_dataTrack.title;
                            //this->currMenu = "clb_qobuz";
                            this->noTotal_currntPlay = k;

                            tmp_dataTrack.isPlay = true;


                        }

                        QString tmp_imgPath = tmp_dataTrack.thumbnail;
                        if(tmp_imgPath.startsWith("http")==false){
                            tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                        }

                        this->list_thumbPath[tabSectionlist[0]].append(tmp_imgPath);
                        this->list_dataTrackForDelegate[tabSectionlist[0]].append(tmp_dataTrack);
                    }

                }


                if(tmpStr1 ==  "QOBUZ"  && (p_id == HTTP_CURR_QUEUE_GET_QOBUZ || p_id == HTTP_CURR_QUEUE_GET_TOTAL)){

                    if(tmp_json1.contains("track")){

                        //print_debug();
                        if(jsonprintFlag_QO==false){

                            QJsonObject tmp_json = tmp_json1["track"].toObject();
                            QJsonDocument doc(tmp_json); QString strJson(doc.toJson(QJsonDocument::Compact));
                            qDebug() << "QueuePlayList::setQobuzQueueList--QOBUZ-tmp_json : " << strJson << "\n";
                            jsonprintFlag_QO = false;

                        }
                        QString tmp_string = tmp_json1["track"].toString();
                        //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_string : " << tmp_string << "\n";
                        //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                        QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                        QJsonObject tmp_track = json.object(); // since your string is an JSON object

                        //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                        //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_track : " << strJson << "\n";
                        this->jsonArray_backup_qobuz.append(tmp_track);//cheon210916-total
                        this->jsonArray_backup_total[global.now_Section].append(tmp_track);//cheon210916-total
                        qobuz::TrackItemData data_track = qobuz::ConvertData::make_trackData_fromQobuzJsonObj(tmp_track);

                        // 1> 데이터 담기
                        DataTrackforDelegate tmp_dataTrack;
                        tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Qobuz;
                        tmp_dataTrack.addtimeResult = addtimeResult;
                        if(data_track.work.size() > 0){
                            tmp_dataTrack.title = data_track.title + " - " + data_track.work;
                        }else{
                            tmp_dataTrack.title = data_track.title;
                        }
                        tmp_dataTrack.artist = data_track.list_artist_name.join(",");
                        tmp_dataTrack.duration = data_track.duration;
                        if(data_track.hires){
                            tmp_dataTrack.audioQuality = "HI_RES";
                        }
                        //tmp_dataTrack.audioQuality = data_track.hires;
                        if(tmp_dataTrack.artist == ""){
                            tmp_dataTrack.artist = data_track.performer_name;
                        }

                        tmp_dataTrack.thumbnail = data_track.album_image_small;
                        //tmp_dataTrack.thumbnail = data_track.album_image_thumbnail;
                        tmp_dataTrack.no = i;


                        if(i==this->indexCurrPlay[tabSectionlist[5]]){//cheon211104-01
                            print_debug();
                            //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                            //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_track : " << strJson << "\n";
                            qDebug() << "tmp_dataTrack.title= " << tmp_dataTrack.title;
                            //this->currMenu = "clb_qobuz";
                            this->noTotal_currntPlay = k;

                            tmp_dataTrack.isPlay = true;

                        }

                        QString tmp_imgPath = tmp_dataTrack.thumbnail;
                        if(tmp_imgPath.startsWith("http")==false){
                            tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                        }

                        this->list_thumbPath[tabSectionlist[5]].append(tmp_imgPath);
                        this->list_dataTrackForDelegate[tabSectionlist[5]].append(tmp_dataTrack);

                        // 2> UI에 데이터 세팅
                        this->appendTrackUI(k);//cheon210916-total
                        k++;//cheon210916-total

                    }
                }
                if(tmpStr1 ==  "QOBUZ"){

                    if(tmp_json1.contains("track")){

                        //print_debug();
                        if(jsonprintFlag_QO==false){
                            QJsonObject tmp_json = tmp_json1["track"].toObject();
                            QJsonDocument doc(tmp_json); QString strJson(doc.toJson(QJsonDocument::Compact));
                            qDebug() << "QueuePlayList::setQobuzQueueList--QOBUZ-tmp_json : " << strJson << "\n";
                            jsonprintFlag_QO = false;

                        }
                        QString tmp_string = tmp_json1["track"].toString();
                        //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_string : " << tmp_string << "\n";
                        //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                        QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                        QJsonObject tmp_track = json.object(); // since your string is an JSON object

                        //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                        //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_track : " << strJson << "\n";
                        qobuz::TrackItemData data_track = qobuz::ConvertData::make_trackData_fromQobuzJsonObj(tmp_track);

                        // 1> 데이터 담기
                        DataTrackforDelegate tmp_dataTrack;
                        tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Qobuz;
                        tmp_dataTrack.addtimeResult = addtimeResult;
                        if(data_track.work.size() > 0){
                            tmp_dataTrack.title = data_track.title + " - " + data_track.work;
                        }else{
                            tmp_dataTrack.title = data_track.title;
                        }
                        tmp_dataTrack.artist = data_track.list_artist_name.join(",");
                        tmp_dataTrack.duration = data_track.duration;
                        if(data_track.hires){
                            tmp_dataTrack.audioQuality = "HI_RES";
                        }
                        //tmp_dataTrack.audioQuality = data_track.hires;
                        if(tmp_dataTrack.artist == ""){
                            tmp_dataTrack.artist = data_track.performer_name;
                        }

                        tmp_dataTrack.thumbnail = data_track.album_image_small;
                        //tmp_dataTrack.thumbnail = data_track.album_image_thumbnail;
                        tmp_dataTrack.no = i;


                        if(i==this->indexCurrPlay[tabSectionlist[0]]){//cheon211104-01
                            print_debug();
                            //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                            //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_track : " << strJson << "\n";
                            qDebug() << "tmp_dataTrack.title= " << tmp_dataTrack.title;
                            //this->currMenu = "clb_qobuz";
                            this->noTotal_currntPlay = k;

                            tmp_dataTrack.isPlay = true;

                        }

                        QString tmp_imgPath = tmp_dataTrack.thumbnail;
                        if(tmp_imgPath.startsWith("http")==false){
                            tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                        }

                        this->list_thumbPath[tabSectionlist[0]].append(tmp_imgPath);
                        this->list_dataTrackForDelegate[tabSectionlist[0]].append(tmp_dataTrack);
                    }

                }



                if(tmpStr1 ==  "TIDAL" && (p_id == HTTP_CURR_QUEUE_GET_TIDAL || p_id == HTTP_CURR_QUEUE_GET_TOTAL)){

                    if(tmp_json1.contains("track")){
                        if(jsonprintFlag_TI==false){
                            QJsonObject tmp_json = tmp_json1["track"].toObject();
                            QJsonDocument doc(tmp_json); QString strJson(doc.toJson(QJsonDocument::Compact));
                            qDebug() << "QueuePlayList::setQobuzQueueList--TIDAL-tmp_json : " << strJson << "\n";
                            jsonprintFlag_TI = false;

                        }
                        //print_debug();
                        QString tmp_string = tmp_json1["track"].toString();
                        //qDebug() << "QueuePlayList::setTidalQueueList---tmp_string : " << tmp_string << "\n";
                        //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                        QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                        QJsonObject tmp_track = json.object(); // since your string is an JSON object

                        //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                        //qDebug() << "QueuePlayList::setTidalQueueList---tmp_track : " << strJson << "\n";
                        this->jsonArray_backup_tidal.append(tmp_track);//cheon210916-total
                        this->jsonArray_backup_total[global.now_Section].append(tmp_track);//cheon210916-total
                        tidal::TrackItemData data_track = tidal::ConvertData::make_trackData_fromTidalJsonObj(tmp_track);

                        // 1> 데이터 담기
                        DataTrackforDelegate tmp_dataTrack;
                        tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Tidal;
                        tmp_dataTrack.addtimeResult = addtimeResult;
                        tmp_dataTrack.title = data_track.title;
                        tmp_dataTrack.artist = data_track.list_artist_name.join(",");
                        tmp_dataTrack.thumbnail = data_track.image;
                        tmp_dataTrack.duration = data_track.duration;
                        //qDebug() << tmp_dataTrack.duration;
                        if(data_track.audioQuality == "HI_RES"){
                            tmp_dataTrack.audioQuality = "MQA";
                        }
                        tmp_dataTrack.no = i;


                        if(i==this->indexCurrPlay[tabSectionlist[3]]){//cheon211104-01
                            //this->bar_pos = k;
                            //this->currMenu = "clb_tidal";
                            tmp_dataTrack.isPlay = true;
                            this->noTotal_currntPlay = k;


                        }

                        QString tmp_imgPath = tmp_dataTrack.thumbnail;
                        if(tmp_imgPath.startsWith("http")==false){
                            tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                        }

                        this->list_thumbPath[tabSectionlist[3]].append(tmp_imgPath);
                        this->list_dataTrackForDelegate[tabSectionlist[3]].append(tmp_dataTrack);

                        // 2> UI에 데이터 세팅
                        this->appendTrackUI(k);//cheon210916-total

                    }
                    k++;//cheon210916-total

                }
                if(tmpStr1 ==  "TIDAL"){

                    if(tmp_json1.contains("track")){
                        if(jsonprintFlag_TI==false){
                            QJsonObject tmp_json = tmp_json1["track"].toObject();
                            QJsonDocument doc(tmp_json); QString strJson(doc.toJson(QJsonDocument::Compact));
                            qDebug() << "QueuePlayList::setQobuzQueueList--TIDAL-tmp_json : " << strJson << "\n";
                            jsonprintFlag_TI = false;

                        }
                        //print_debug();
                        QString tmp_string = tmp_json1["track"].toString();
                        //qDebug() << "QueuePlayList::setTidalQueueList---tmp_string : " << tmp_string << "\n";
                        //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                        QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                        QJsonObject tmp_track = json.object(); // since your string is an JSON object

                        //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                        //qDebug() << "QueuePlayList::setTidalQueueList---tmp_track : " << strJson << "\n";
                        tidal::TrackItemData data_track = tidal::ConvertData::make_trackData_fromTidalJsonObj(tmp_track);

                        // 1> 데이터 담기
                        DataTrackforDelegate tmp_dataTrack;
                        tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Tidal;
                        tmp_dataTrack.addtimeResult = addtimeResult;
                        tmp_dataTrack.title = data_track.title;
                        tmp_dataTrack.artist = data_track.list_artist_name.join(",");
                        tmp_dataTrack.thumbnail = data_track.image;
                        tmp_dataTrack.duration = data_track.duration;
                        //qDebug() << tmp_dataTrack.duration;
                        if(data_track.audioQuality == "HI_RES"){
                            tmp_dataTrack.audioQuality = "MQA";
                        }
                        tmp_dataTrack.no = i;


                        if(i==this->indexCurrPlay[tabSectionlist[0]]){//cheon211104-01
                            //this->bar_pos = k;
                            //this->currMenu = "clb_tidal";
                            tmp_dataTrack.isPlay = true;
                            this->noTotal_currntPlay = k;
                        }

                        QString tmp_imgPath = tmp_dataTrack.thumbnail;
                        if(tmp_imgPath.startsWith("http")==false){
                            tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                        }

                        this->list_thumbPath[tabSectionlist[0]].append(tmp_imgPath);
                        this->list_dataTrackForDelegate[tabSectionlist[0]].append(tmp_dataTrack);
                    }

                }
                if(tmpStr1 ==  "BUGS" && (p_id == HTTP_CURR_QUEUE_GET_BUGS || p_id == HTTP_CURR_QUEUE_GET_TOTAL)){
                    //QJsonDocument doc(tmp_json1); QString strJson(doc.toJson(QJsonDocument::Compact));
                    //qDebug() << "QueuePlayList::setQobuzQueueList--BUGS-tmp_json1 : " << strJson << "\n";


                    if(tmp_json1.contains("track")){
                        //print_debug();
                        if(jsonprintFlag_Btrack==false){
                            QJsonObject tmp_json = tmp_json1["track"].toObject();
                            QJsonDocument doc(tmp_json); QString strJson(doc.toJson(QJsonDocument::Compact));
                            qDebug() << "QueuePlayList::setQobuzQueueList--BUGS-tmp_json : " << strJson << "\n";
                            jsonprintFlag_Btrack = false;

                        }
                        //print_debug();
                        QString tmp_string = tmp_json1["track"].toString();
                        //qDebug() << "QueuePlayList::setBugsQueueList---tmp_string : " << tmp_string << "\n";
                        //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                        QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                        QJsonObject tmp_track = json.object(); // since your string is an JSON object

                        QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                        //qDebug() << "QueuePlayList::setTidalQueueList---tmp_track : " << strJson << "\n";
                        this->jsonArray_backup_bugs.append(tmp_track);//cheon210916-total
                        this->jsonArray_backup_total[global.now_Section].append(tmp_track);//cheon210916-total
                        DataTrackforDelegate tmp_dataTrack;
                        //QJsonObject tmp_trackOBJ = tmp_arr.at(i).toObject();
                        if(tmp_track.contains("mv_title")){
                            //print_debug();
                            bugs::VideoItemData data_video = bugs::ConvertData_forBugs::make_videoData_fromBugsJsonObj(tmp_track);


                            // 1> 데이터 담기

                            tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Bugs;
                            tmp_dataTrack.addtimeResult = addtimeResult;
                            tmp_dataTrack.title = data_video.mv_title;
                            tmp_dataTrack.artist = data_video.list_artist_nm.join(",");
                            if(data_video.bitrates.contains("fullhd") == true){
                                tmp_dataTrack.audioQuality = "FULL HD";
                            }
                            else if(data_video.bitrates.contains("hd") == true){
                                tmp_dataTrack.audioQuality = "HD";
                            }
                            else if(data_video.bitrates.contains("sd") == true){
                                tmp_dataTrack.audioQuality = "SD";
                            }
                            //tmp_dataTrack.audioQuality = data_video.bitrates.join(",");
                            //qDebug() << "audioQuality=" << tmp_dataTrack.audioQuality;
                            tmp_dataTrack.len = data_video.len;
                            //qDebug() << "data_video.len=" << data_video.len;
                            tmp_dataTrack.thumbnail = data_video.image; //data_track.album_image;
                        }else{
                            //print_debug();
                            bugs::TrackItemData data_track = bugs::ConvertData_forBugs::make_trackData_fromBugsJsonObj(tmp_track);

                            // 1> 데이터 담기

                            tmp_dataTrack.addtimeResult = addtimeResult;
                            tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Bugs;
                            tmp_dataTrack.title = data_track.track_title;

                            tmp_dataTrack.artist = data_track.list_artist_nm.join(",");
                            tmp_dataTrack.audioQuality = data_track.bitrates;
                            tmp_dataTrack.len = data_track.len;
                            tmp_dataTrack.thumbnail = data_track.album_image;
                        }
                        tmp_dataTrack.no = i;

                        if(i==this->indexCurrPlay[tabSectionlist[4]]){//cheon211104-01
                            tmp_dataTrack.isPlay = true;
                            //this->currMenu = "clb_bugs";
                            //this->bar_pos = k;
                            this->noTotal_currntPlay = k;


                        }

                        QString tmp_imgPath = tmp_dataTrack.thumbnail;
                        if(tmp_imgPath.startsWith("http")==false){
                            tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                        }

                        this->list_thumbPath[tabSectionlist[4]].append(tmp_imgPath);
                        this->list_dataTrackForDelegate[tabSectionlist[4]].append(tmp_dataTrack);

                        // 2> UI에 데이터 세팅
                        this->appendTrackUI(k);//cheon210916-total

                    }else if(tmp_json1.contains("mvs")){//cheon211006
                        //print_debug();
                        if(jsonprintFlag_Bmvs==false){
                            QJsonObject tmp_json = tmp_json1["track"].toObject();
                            QJsonDocument doc(tmp_json); QString strJson(doc.toJson(QJsonDocument::Compact));
                            qDebug() << "QueuePlayList::setQobuzQueueList--BUGSmvs-tmp_json : " << strJson << "\n";
                            jsonprintFlag_Bmvs = false;

                        }
                        //print_debug();
                        QString tmp_string = tmp_json1["mvs"].toString();
                        //qDebug() << "QueuePlayList::setBugsQueueList--mvs-tmp_string : " << tmp_string << "\n";
                        //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                        QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                        QJsonObject tmp_track = json.object(); // since your string is an JSON object

                        //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                        //qDebug() << "QueuePlayList::setBugsQueueList---tmp_track : " << strJson << "\n";
                        this->jsonArray_backup_bugs.append(tmp_track);//cheon210916-total
                        this->jsonArray_backup_total[global.now_Section].append(tmp_track);
                        bugs::VideoItemData data_video = bugs::ConvertData_forBugs::make_videoData_fromBugsJsonObj(tmp_arr.at(i).toObject());


                        // 1> 데이터 담기
                        DataTrackforDelegate tmp_dataTrack;
                        tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Bugs;
                        tmp_dataTrack.addtimeResult = addtimeResult;
                        tmp_dataTrack.title = data_video.mv_title;
                        tmp_dataTrack.len = data_video.len;
                        tmp_dataTrack.artist = data_video.list_artist_nm.join(",");
                        if(data_video.bitrates.contains("fullhd") == true){
                            tmp_dataTrack.audioQuality = "FULL HD";
                        }
                        else if(data_video.bitrates.contains("hd") == true){
                            tmp_dataTrack.audioQuality = "HD";
                        }
                        else if(data_video.bitrates.contains("sd") == true){
                            tmp_dataTrack.audioQuality = "SD";
                        }

                        tmp_dataTrack.thumbnail = data_video.image; //data_track.album_image;
                        tmp_dataTrack.no = i;//cheon211107-01

                        if(i==this->indexCurrPlay[tabSectionlist[4]]){//cheon211104-01
                            this->bar_pos = k;
                            //this->currMenu = "clb_bugs";
                            tmp_dataTrack.isPlay = true;
                            this->noTotal_currntPlay = k;



                            //lb_title->setText(tr("Queue List  (%1/%2)").arg(k+1).arg(this->bugs_queue_list_count));//cheon211114-01
                        }

                        QString tmp_imgPath = tmp_dataTrack.thumbnail;
                        if(tmp_imgPath.startsWith("http")==false){
                            tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                        }

                        this->list_thumbPath[tabSectionlist[4]].append(tmp_imgPath);
                        this->list_dataTrackForDelegate[tabSectionlist[4]].append(tmp_dataTrack);

                        // 2> UI에 데이터 세팅
                        this->appendTrackUI(k);//cheon210916-total

                    }
                    k++;//cheon210916-total
                }
                if(tmpStr1 ==  "BUGS"){

                    if(tmp_json1.contains("track")){
                        //print_debug();
                        if(jsonprintFlag_Btrack==false){
                            QJsonObject tmp_json = tmp_json1["track"].toObject();
                            QJsonDocument doc(tmp_json); QString strJson(doc.toJson(QJsonDocument::Compact));
                            qDebug() << "QueuePlayList::setQobuzQueueList--BUGS-tmp_json : " << strJson << "\n";
                            jsonprintFlag_Btrack = false;

                        }
                        //print_debug();
                        QString tmp_string = tmp_json1["track"].toString();
                        //qDebug() << "QueuePlayList::setBugsQueueList---tmp_string : " << tmp_string << "\n";
                        //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                        QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                        QJsonObject tmp_track = json.object(); // since your string is an JSON object

                        QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                        //qDebug() << "QueuePlayList::setTidalQueueList---tmp_track : " << strJson << "\n";
                        DataTrackforDelegate tmp_dataTrack;
                        //QJsonObject tmp_trackOBJ = tmp_arr.at(i).toObject();
                        if(tmp_track.contains("mv_title")){
                            //print_debug();
                            bugs::VideoItemData data_video = bugs::ConvertData_forBugs::make_videoData_fromBugsJsonObj(tmp_track);


                            // 1> 데이터 담기

                            tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Bugs;
                            tmp_dataTrack.addtimeResult = addtimeResult;
                            tmp_dataTrack.title = data_video.mv_title;
                            tmp_dataTrack.len = data_video.len;//c220713
                            tmp_dataTrack.artist = data_video.list_artist_nm.join(",");
                            if(data_video.bitrates.contains("fullhd") == true){
                                tmp_dataTrack.audioQuality = "FULL HD";
                            }
                            else if(data_video.bitrates.contains("hd") == true){
                                tmp_dataTrack.audioQuality = "HD";
                            }
                            else if(data_video.bitrates.contains("sd") == true){
                                tmp_dataTrack.audioQuality = "SD";
                            }

                            tmp_dataTrack.thumbnail = data_video.image; //data_track.album_image;
                        }else{
                            //print_debug();
                            bugs::TrackItemData data_track = bugs::ConvertData_forBugs::make_trackData_fromBugsJsonObj(tmp_track);

                            // 1> 데이터 담기

                            tmp_dataTrack.addtimeResult = addtimeResult;
                            tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Bugs;
                            tmp_dataTrack.title = data_track.track_title;

                            tmp_dataTrack.artist = data_track.list_artist_nm.join(",");
                            tmp_dataTrack.audioQuality = data_track.bitrates;
                            tmp_dataTrack.len = data_track.len;
                            tmp_dataTrack.thumbnail = data_track.album_image;
                        }
                        tmp_dataTrack.no = i;

                        if(i==this->indexCurrPlay[tabSectionlist[0]]){//cheon211104-01
                            tmp_dataTrack.isPlay = true;
                            //this->currMenu = "clb_bugs";
                            //this->bar_pos = k;
                            this->noTotal_currntPlay = k;


                        }

                        QString tmp_imgPath = tmp_dataTrack.thumbnail;
                        if(tmp_imgPath.startsWith("http")==false){
                            tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                        }

                        this->list_thumbPath[tabSectionlist[0]].append(tmp_imgPath);
                        this->list_dataTrackForDelegate[tabSectionlist[0]].append(tmp_dataTrack);

                    }else if(tmp_json1.contains("mvs")){//cheon211006
                        //print_debug();
                        if(jsonprintFlag_Bmvs==false){
                            QJsonObject tmp_json = tmp_json1["track"].toObject();
                            QJsonDocument doc(tmp_json); QString strJson(doc.toJson(QJsonDocument::Compact));
                            qDebug() << "QueuePlayList::setQobuzQueueList--BUGSmvs-tmp_json : " << strJson << "\n";
                            jsonprintFlag_Bmvs = false;

                        }
                        //print_debug();
                        QString tmp_string = tmp_json1["mvs"].toString();
                        //qDebug() << "QueuePlayList::setBugsQueueList--mvs-tmp_string : " << tmp_string << "\n";
                        //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                        QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                        QJsonObject tmp_track = json.object(); // since your string is an JSON object

                        //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                        //qDebug() << "QueuePlayList::setBugsQueueList---tmp_track : " << strJson << "\n";
                        bugs::VideoItemData data_video = bugs::ConvertData_forBugs::make_videoData_fromBugsJsonObj(tmp_arr.at(i).toObject());


                        // 1> 데이터 담기
                        DataTrackforDelegate tmp_dataTrack;
                        tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Bugs;
                        tmp_dataTrack.addtimeResult = addtimeResult;
                        tmp_dataTrack.title = data_video.mv_title;
                        tmp_dataTrack.len = data_video.len;
                        tmp_dataTrack.artist = data_video.list_artist_nm.join(",");
                        if(data_video.bitrates.contains("fullhd") == true){
                            tmp_dataTrack.audioQuality = "FULL HD";
                        }
                        else if(data_video.bitrates.contains("hd") == true){
                            tmp_dataTrack.audioQuality = "HD";
                        }
                        else if(data_video.bitrates.contains("sd") == true){
                            tmp_dataTrack.audioQuality = "SD";
                        }

                        tmp_dataTrack.thumbnail = data_video.image; //data_track.album_image;
                        tmp_dataTrack.no = i;//cheon211107-01

                        if(i==this->indexCurrPlay[tabSectionlist[0]]){//cheon211104-01
                            this->bar_pos = k;
                            //this->currMenu = "clb_bugs";
                            tmp_dataTrack.isPlay = true;
                            this->noTotal_currntPlay = k;
                            //lb_title->setText(tr("Queue List  (%1/%2)").arg(k+1).arg(this->bugs_queue_list_count));//cheon211114-01
                        }

                        QString tmp_imgPath = tmp_dataTrack.thumbnail;
                        if(tmp_imgPath.startsWith("http")==false){
                            tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                        }

                        this->list_thumbPath[tabSectionlist[0]].append(tmp_imgPath);
                        this->list_dataTrackForDelegate[tabSectionlist[0]].append(tmp_dataTrack);
                    }

                }
                if(tmpStr1 ==  "YOUTUBE" && (p_id == HTTP_CURR_QUEUE_GET_ROSETUBE || p_id == HTTP_CURR_QUEUE_GET_TOTAL)){

                    if(tmp_json1.contains("track")){
                        if(jsonprintFlag_YO == false){
                            QJsonObject tmp_json = tmp_json1["track"].toObject();
                            QJsonDocument doc(tmp_json1); QString strJson(doc.toJson(QJsonDocument::Compact));
                            //qDebug() << "QueuePlayList::setQobuzQueueList--YOUTUBE-tmp_json : " << strJson << "\n";
                            jsonprintFlag_YO = false;

                        }
                        //if(tmp_arr.count() > 0){//cheon211104-01
                        //    this->indexCurrPlay[global.now_Section] = this->jsonArray_backup_youtube.count();//cheon210916-total
                        //}
                        //QJsonArray tmp_arr1 = p_jsonObject["track"].toArray();//cheon210916-total


                        //print_debug();//c220521
                        QString tmp_string = tmp_json1["track"].toString();
                        //qDebug() << "QueuePlayList::setRoseTubeQueueList---tmp_string : " << tmp_string << "\n";//c220521
                        //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                        QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                        QJsonObject tmp_jsonObject = json.object(); // since your string is an JSON object

                        QJsonDocument doc(tmp_jsonObject); QString strJson(doc.toJson(QJsonDocument::Compact));
                        //qDebug() << "QueuePlayList::setRoseTubeQueueList---tmp_jsonObject : " << strJson << "\n";
                        this->jsonArray_backup_youtube.append(tmp_jsonObject);//cheon210916-total
                        this->jsonArray_backup_total[global.now_Section].append(tmp_jsonObject);//cheon210916-total

                        //tidal::TrackItemData data_track = tidal::ConvertData::make_trackData_fromTidalJsonObj(tmp_track);
                        //QJsonObject tmp_jsonObject = tmp_arr[i].toObject();
                        // 1> 데이터 담기tmp_dataTrack.audioQuality = data_video.bitrates;
                        DataTrackforDelegate tmp_dataTrack;
                        tmp_dataTrack.type = DataTrackforDelegate::Types_Data::RoseTube;
                        tmp_dataTrack.addtimeResult = addtimeResult;
                        if(tmp_jsonObject.contains("title")){
                            tmp_dataTrack.title = tmp_jsonObject["title"].toString();
                        }
                        if(tmp_jsonObject.contains("duration")){
                            //tmp_dataTrack.duration = data_track.duration;
                            tmp_dataTrack.len = tmp_jsonObject["duration"].toString();//c220521
                            //tmp_dataTrack.duration = tmp.toInt();//c220521
                            //qDebug() << "tmp_dataTrack.len = " << tmp_dataTrack.len;
                        }
                        if(tmp_jsonObject.contains("channelName")){
                            tmp_dataTrack.artist = tmp_jsonObject["channelName"].toString();
                        }
                        if(tmp_jsonObject.contains("thumbnailUrl")){
                            tmp_dataTrack.thumbnail = tmp_jsonObject["thumbnailUrl"].toString();
                        }

                        if(i==this->indexCurrPlay[tabSectionlist[2]]){//cheon211104-01
                            tmp_dataTrack.isPlay = true;
                            //this->currMenu = "clb_roseTube";
                            this->bar_pos = k;
                            this->noTotal_currntPlay = k;


                        }
                        tmp_dataTrack.no = i;//cheon210916-total


                        QString tmp_imgPath = tmp_dataTrack.thumbnail;
                        if(tmp_imgPath.startsWith("http")==false){
                            tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                        }

                        this->list_thumbPath[tabSectionlist[2]].append(tmp_imgPath);
                        this->list_dataTrackForDelegate[tabSectionlist[2]].append(tmp_dataTrack);
                        //print_debug();
                        //qDebug() << "this->jsonArray_backup_youtube.count()=" << this->jsonArray_backup_youtube.count();//cheon210916-total
                        // 2> UI에 데이터 세팅
                        this->appendTrackUI(k++);//cheon210916-total
                        //print_debug();

                    }//if(tmp_json1.contains("track"))
                }//if(tmpStr1 ==
                if(tmpStr1 ==  "YOUTUBE"){
                    //print_debug();
                    //QJsonDocument doc(tmp_json1); QString strJson(doc.toJson(QJsonDocument::Compact));
                    //qDebug() << "QueuePlayList::setQobuzQueueList--YOUTUBE-tmp_json1 : " << strJson << "\n";


                    if(tmp_json1.contains("track")){
                        if(jsonprintFlag_YO == false){
                            QJsonObject tmp_json = tmp_json1["track"].toObject();
                            QJsonDocument doc(tmp_json1); QString strJson(doc.toJson(QJsonDocument::Compact));
                            //qDebug() << "QueuePlayList::setQobuzQueueList--YOUTUBE-tmp_json : " << strJson << "\n";
                            jsonprintFlag_YO = false;

                        }
                        //if(tmp_arr.count() > 0){//cheon211104-01
                        //    this->indexCurrPlay[global.now_Section] = this->jsonArray_backup_youtube.count();//cheon210916-total
                        //}
                        //QJsonArray tmp_arr1 = p_jsonObject["track"].toArray();//cheon210916-total


                        //print_debug();
                        QString tmp_string = tmp_json1["track"].toString();
                        //qDebug() << "QueuePlayList::setRoseTubeQueueList---tmp_string : " << tmp_string << "\n";
                        //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                        QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                        QJsonObject tmp_jsonObject = json.object(); // since your string is an JSON object

                        QJsonDocument doc(tmp_jsonObject); QString strJson(doc.toJson(QJsonDocument::Compact));
                        //qDebug() << "QueuePlayList::setRoseTubeQueueList---tmp_jsonObject : " << strJson << "\n";

                        //tidal::TrackItemData data_track = tidal::ConvertData::make_trackData_fromTidalJsonObj(tmp_track);
                        //QJsonObject tmp_jsonObject = tmp_arr[i].toObject();
                        // 1> 데이터 담기tmp_dataTrack.audioQuality = data_video.bitrates;
                        DataTrackforDelegate tmp_dataTrack;
                        tmp_dataTrack.type = DataTrackforDelegate::Types_Data::RoseTube;
                        tmp_dataTrack.addtimeResult = addtimeResult;
                        if(tmp_jsonObject.contains("title")){
                            tmp_dataTrack.title = tmp_jsonObject["title"].toString();

                        }
                        if(tmp_jsonObject.contains("duration")){
                            QString tmp = tmp_jsonObject["duration"].toString();//c220521
                            //tmp_dataTrack.duration = tmp.toInt();//c220521
                            tmp_dataTrack.len = tmp;//c220601
                        }
                        if(tmp_jsonObject.contains("channelName")){
                            tmp_dataTrack.artist = tmp_jsonObject["channelName"].toString();
                        }
                        if(tmp_jsonObject.contains("thumbnailUrl")){
                            tmp_dataTrack.thumbnail = tmp_jsonObject["thumbnailUrl"].toString();
                        }

                        if(i==this->indexCurrPlay[tabSectionlist[0]]){//cheon211104-01
                            tmp_dataTrack.isPlay = true;
                            //this->currMenu = "clb_roseTube";
                            this->bar_pos = k;
                            this->noTotal_currntPlay = k;


                        }
                        tmp_dataTrack.no = i;//cheon210916-total


                        QString tmp_imgPath = tmp_dataTrack.thumbnail;
                        if(tmp_imgPath.startsWith("http")==false){
                            tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                        }

                        this->list_thumbPath[tabSectionlist[0]].append(tmp_imgPath);
                        this->list_dataTrackForDelegate[tabSectionlist[0]].append(tmp_dataTrack);
                    }

                }
                if(tmpStr1 ==  "MUSIC" && (p_id == HTTP_CURR_QUEUE_GET_MUSIC || p_id == HTTP_CURR_QUEUE_GET_TOTAL)){
                    //print_debug();
                    //qDebug() << "**************i=" <<i << ", this->indexCurrPlay[global.now_Section]=" << this->indexCurrPlay[global.now_Section];
                    //QJsonDocument doc(tmp_json1);  QString strJson1(doc.toJson(QJsonDocument::Compact));  qDebug() << "tmp_json1---" << strJson1;


                    if(tmp_json1.contains("track")){
                        if(jsonprintFlag_MU == true){
                            //QJsonObject tmp_json = mediaState_jsonObject["track"].toObject();
                            // QJsonDocument doc(tmp_json); QString strJson(doc.toJson(QJsonDocument::Compact));
                            // qDebug() << "QueuePlayList::setQobuzQueueList--MUSIC-tmp_json : " << strJson << "\n";
                            // jsonprintFlag_MU = true;

                        }
                        //if(tmp_arr.count() > 0){//cheon211104-01
                        //    this->indexCurrPlay[global.now_Section] = this->jsonArray_backup_music.count();//cheon210916-total
                        //}
                        //QJsonArray tmp_arr1 = p_jsonObject["track"].toArray();//cheon210916-total


                        //print_debug();
                        //QJsonObject tmp_json = mediaState_jsonObject["track"].toObject();
                        //QJsonDocument doc(tmp_json); QString strJson(doc.toJson(QJsonDocument::Compact));
                        //QJsonDocument json = QJsonDocument::fromJson(strJson.toUtf8());
                        //QJsonObject tmp_jsonObject = json.object();
                        QString tmp_string = tmp_json1["track"].toString();
                        //qDebug() << "QueuePlayList::setMusicQueueList---tmp_string : " << tmp_string << "\n";
                        //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                        QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                        QJsonObject tmp_jsonObject = json.object(); // since your string is an JSON object
                        //QJsonDocument doc1(tmp_jsonObject); QString strJson1(doc1.toJson(QJsonDocument::Compact));//cheon211114-01
                        //print_debug();
                        //qDebug() << "QueuePlayList::setMusicQueueList---tmp_jsonObject : " << strJson1 << "\n";

                        this->jsonArray_backup_music.append(tmp_jsonObject);//cheon210916-total
                        this->jsonArray_backup_total[global.now_Section].append(tmp_jsonObject);//cheon210916-total

                        //tidal::TrackItemData data_track = tidal::ConvertData::make_trackData_fromTidalJsonObj(tmp_track);
                        //QJsonObject tmp_jsonObject = tmp_arr[i].toObject();
                        // 1> 데이터 담기
                        DataTrackforDelegate tmp_dataTrack;
                        //QJsonObject tmp_jsonObject = tmp_arr[i].toObject();
                        bool isDirect = false;                  // 음악 폴더에서 재생한건지 여부
                        if(tmp_jsonObject.contains("isDirect")){
                            isDirect = tmp_jsonObject["isDirect"].toBool();
                        }
                        if(isDirect==true){
                            if(tmp_jsonObject.contains("mDirectFileInfo")){
                                //print_debug();
                                QJsonObject tmp_data = tmp_jsonObject["mDirectFileInfo"].toObject();
                                QString tmp_ip = "";

                                // 1> 데이터 담기
                                tmp_dataTrack.addtimeResult = addtimeResult;
                                tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Music;
                                tmp_dataTrack.isDirect = true;
                                if(tmp_data.contains("ip")){
                                    tmp_ip = tmp_data["ip"].toString().replace("?", "");
                                }
                                if(tmp_data.contains("smbParamPath")){
                                    if(tmp_data["smbParamPath"].toString()!=""){
                                        // 네트워크 노래
                                    }else{
                                        // 친구 음원 (공유음원) - 나의 맥주소와 다른경우
                                        if(tmp_ip!="" && (tmp_ip!=global.device.getDeviceID())){
                                            tmp_dataTrack.isShare = true;
                                        }
                                    }
                                }
                                if(tmp_data.contains("name")){
                                    tmp_dataTrack.title = tmp_data["name"].toString();
                                }
                                if(tmp_json1.contains("mediaState")){
                                    QJsonObject mediaState_jsonObject = tmp_json1["mediaState"].toObject();
                                    //QJsonDocument doc1(mediaState_jsonObject); QString strJson1(doc1.toJson(QJsonDocument::Compact));//cheon211114-01
                                    //print_debug();
                                    //qDebug() << "QueuePlayList::setMusicQueueList---mediaState_jsonObject : " << strJson1 << "\n";


                                    if(mediaState_jsonObject.contains("duration")){
                                        //print_debug();
                                        //qDebug() << "mediaState_jsonObject-duration = " << tmp_data["duration"].toString().toInt();
                                        tmp_dataTrack.duration = mediaState_jsonObject["duration"].toString().toInt();

                                    }else{
                                        //print_debug();
                                        if(tmp_data.contains("duration")){
                                            //qDebug() << "tmp_data-duration = " << tmp_data["duration"].toString().toInt();
                                            tmp_dataTrack.duration = tmp_data["duration"].toString().toInt();
                                        }
                                    }
                                }else{
                                    //print_debug();
                                    if(tmp_data.contains("duration")){
                                        //qDebug() << "tmp_data-duration = " << tmp_data["duration"].toString().toInt();
                                        tmp_dataTrack.duration = tmp_data["duration"].toString().toInt();
                                    }
                                }

                                tmp_dataTrack.artist = "";
                                if(tmp_data.contains("artist")){
                                    tmp_dataTrack.artist = tmp_data["artist"].toString();
                                }
                                if(tmp_data.contains("thumbnail")){
                                    tmp_dataTrack.thumbnail = tmp_data["thumbnail"].toString();

                                }

                                if(i==this->indexCurrPlay[tabSectionlist[1]]){//cheon211104-01
                                    print_debug();
                                    tmp_dataTrack.isPlay = true;
                                    //this->currMenu = "clb_music";
                                    this->bar_pos = k;
                                    this->noTotal_currntPlay = k;


                                }
                            }
                        }else{
                            if(tmp_jsonObject.contains("mMusicSongInfo")){
                                //print_debug();
                                //qDebug() << "i=" <<i << ", this->indexCurrPlay[global.now_Section]=" << this->indexCurrPlay[global.now_Section];
                                QJsonObject tmp_data = tmp_jsonObject["mMusicSongInfo"].toObject();

                                // 1> 데이터 담기
                                tmp_dataTrack.addtimeResult = addtimeResult;
                                tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Music;
                                tmp_dataTrack.isDirect = false;
                                if(tmp_data.contains("title")){
                                    tmp_dataTrack.title = tmp_data["title"].toString();
                                }
                                if(tmp_data.contains("artist")){
                                    tmp_dataTrack.artist = tmp_data["artist"].toString();
                                }
                                if(tmp_jsonObject.contains("mediaState")){
                                    QJsonObject mediaState_jsonObject = tmp_json1["mediaState"].toObject();
                                    if(mediaState_jsonObject.contains("duration")){
                                        //qDebug() << "else-duration = " << tmp_data["duration"].toString().toInt();
                                        tmp_dataTrack.duration = mediaState_jsonObject["duration"].toString().toInt();
                                    }
                                }else{
                                    if(tmp_data.contains("duration")){
                                        //qDebug() << "else-duration = " << tmp_data["duration"].toString().toInt();
                                        tmp_dataTrack.duration = tmp_data["duration"].toString().toInt();
                                    }
                                }
                                int tmp_albumId = 0;
                                if(tmp_data.contains("album_id")){
                                    tmp_albumId = tmp_data["album_id"].toInt();
                                }

                                if(i==this->indexCurrPlay[tabSectionlist[1]]){//cheon211104-01
                                    print_debug();
                                    //this->currMenu = "clb_music";
                                    tmp_dataTrack.isPlay = true;
                                    this->bar_pos = k;
                                    this->noTotal_currntPlay = k;


                                }
                                QString album_art = "";
                                if(tmp_data.contains("album_art")){
                                    album_art = tmp_data["album_art"].toString();
                                }
                                if(album_art!=""){
                                    SqliteHelper *sqlite = new SqliteHelper(this);
                                    QSqlError err = sqlite->addConnectionRose();
                                    if(err.type() == QSqlError::NoError){
                                        QString strQuery = "SELECT album_art FROM album_art WHERE album_id=%1 ";
                                        strQuery = strQuery.arg(tmp_albumId);
                                        QVariantList data;
                                        sqlite->exec(strQuery, data);
                                        if(data.size() > 0){
                                            QJsonObject tmp_data = data.first().toJsonObject();
                                            if(tmp_data.contains("album_art")){
                                                album_art = tmp_data["album_art"].toString();
                                            }
                                        }
                                    }
                                    sqlite->close();
                                    delete sqlite;
                                }
                                tmp_dataTrack.thumbnail = album_art;
                            }
                        }
                        tmp_dataTrack.no = i;//cheon210916-total
                        QString tmp_imgPath = tmp_dataTrack.thumbnail;
                        if(tmp_imgPath.startsWith("http")==false){
                            tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                        }

                        this->list_thumbPath[tabSectionlist[1]].append(tmp_imgPath);
                        this->list_dataTrackForDelegate[tabSectionlist[1]].append(tmp_dataTrack);
                        // 2> UI에 데이터 세팅
                        this->appendTrackUI(k++);//cheon210916-total
                    }//if(tmp_json1.contains("track"))

                }//if(tmpStr1 ==  "MUSIC")
                if(tmpStr1 ==  "MUSIC"){
                    //print_debug();
                    //qDebug() << "**************i=" <<i << ", this->indexCurrPlay[global.now_Section]=" << this->indexCurrPlay[global.now_Section];
                    //QJsonDocument doc(tmp_json1);  QString strJson1(doc.toJson(QJsonDocument::Compact));  qDebug() << "tmp_json1---" << strJson1;



                    if(tmp_json1.contains("track")){
                        if(jsonprintFlag_MU == true){
                            //QJsonObject tmp_json = tmp_json1["track"].toObject();
                            //QJsonDocument doc(tmp_json); QString strJson(doc.toJson(QJsonDocument::Compact));
                            //qDebug() << "QueuePlayList::setQobuzQueueList--YOUTUBE-tmp_json : " << strJson << "\n";
                            //jsonprintFlag_MU = false;

                        }
                        //if(tmp_arr.count() > 0){//cheon211104-01
                        //    this->indexCurrPlay[global.now_Section] = this->jsonArray_backup_music.count();//cheon210916-total
                        //}
                        //QJsonArray tmp_arr1 = p_jsonObject["track"].toArray();//cheon210916-total


                        //print_debug();
                        QString tmp_string = tmp_json1["track"].toString();
                        //qDebug() << "QueuePlayList::setMusicQueueList---tmp_string : " << tmp_string << "\n";
                        //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                        QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                        QJsonObject tmp_jsonObject = json.object(); // since your string is an JSON object

                        //QJsonDocument doc(tmp_jsonObject); QString strJson(doc.toJson(QJsonDocument::Compact));//cheon211114-01
                        //qDebug() << "QueuePlayList::setMusicQueueList---tmp_jsonObject : " << strJson << "\n";

                        //tidal::TrackItemData data_track = tidal::ConvertData::make_trackData_fromTidalJsonObj(tmp_track);
                        //QJsonObject tmp_jsonObject = tmp_arr[i].toObject();
                        // 1> 데이터 담기
                        DataTrackforDelegate tmp_dataTrack;
                        //QJsonObject tmp_jsonObject = tmp_arr[i].toObject();
                        bool isDirect = false;                  // 음악 폴더에서 재생한건지 여부
                        if(tmp_jsonObject.contains("isDirect")){
                            isDirect = tmp_jsonObject["isDirect"].toBool();
                        }
                        if(isDirect==true){
                            if(tmp_jsonObject.contains("mDirectFileInfo")){
                                //print_debug();
                                QJsonObject tmp_data = tmp_jsonObject["mDirectFileInfo"].toObject();
                                QString tmp_ip = "";

                                // 1> 데이터 담기
                                tmp_dataTrack.addtimeResult = addtimeResult;
                                tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Music;
                                tmp_dataTrack.isDirect = true;
                                if(tmp_data.contains("ip")){
                                    tmp_ip = tmp_data["ip"].toString().replace("?", "");
                                }
                                if(tmp_data.contains("smbParamPath")){
                                    if(tmp_data["smbParamPath"].toString()!=""){
                                        // 네트워크 노래
                                    }else{
                                        // 친구 음원 (공유음원) - 나의 맥주소와 다른경우
                                        if(tmp_ip!="" && (tmp_ip!=global.device.getDeviceID())){
                                            tmp_dataTrack.isShare = true;
                                        }
                                    }
                                }
                                if(tmp_data.contains("name")){
                                    tmp_dataTrack.title = tmp_data["name"].toString();
                                }
                                if(tmp_jsonObject.contains("mediaState")){
                                    QJsonObject mediaState_jsonObject = tmp_json1["mediaState"].toObject();
                                    if(mediaState_jsonObject.contains("duration")){
                                        // qDebug() << "duration = " << tmp_data["duration"].toString().toInt();
                                        tmp_dataTrack.duration = mediaState_jsonObject["duration"].toString().toInt();
                                    }
                                }else{
                                    if(tmp_data.contains("duration")){
                                        // qDebug() << "duration = " << tmp_data["duration"].toString().toInt();
                                        tmp_dataTrack.duration = tmp_data["duration"].toString().toInt();
                                    }
                                }
                                tmp_dataTrack.artist = "";
                                if(tmp_data.contains("artist")){
                                    tmp_dataTrack.artist = tmp_data["artist"].toString();
                                }
                                if(tmp_data.contains("thumbnail")){
                                    tmp_dataTrack.thumbnail = tmp_data["thumbnail"].toString();

                                }

                                if(i==this->indexCurrPlay[tabSectionlist[0]]){//cheon211104-01
                                    print_debug();
                                    tmp_dataTrack.isPlay = true;
                                    //this->currMenu = "clb_music";
                                    this->bar_pos = k;
                                    this->noTotal_currntPlay = k;


                                }
                            }
                        }else{
                            if(tmp_jsonObject.contains("mMusicSongInfo")){
                                //print_debug();
                                //qDebug() << "i=" <<i << ", this->indexCurrPlay[global.now_Section]=" << this->indexCurrPlay[global.now_Section];
                                QJsonObject tmp_data = tmp_jsonObject["mMusicSongInfo"].toObject();

                                // 1> 데이터 담기
                                tmp_dataTrack.addtimeResult = addtimeResult;
                                tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Music;
                                tmp_dataTrack.isDirect = false;
                                if(tmp_data.contains("title")){
                                    tmp_dataTrack.title = tmp_data["title"].toString();
                                }
                                if(tmp_data.contains("artist")){
                                    tmp_dataTrack.artist = tmp_data["artist"].toString();
                                }
                                if(tmp_jsonObject.contains("mediaState")){
                                    QJsonObject mediaState_jsonObject = tmp_json1["mediaState"].toObject();
                                    if(mediaState_jsonObject.contains("duration")){
                                        //qDebug() << "duration = " << tmp_data["duration"].toString().toInt();
                                        tmp_dataTrack.duration = mediaState_jsonObject["duration"].toString().toInt();
                                    }
                                }else{
                                    if(tmp_data.contains("duration")){
                                        //qDebug() << "else-duration = " << tmp_data["duration"].toString().toInt();
                                        tmp_dataTrack.duration = tmp_data["duration"].toString().toInt();
                                    }
                                }

                                int tmp_albumId = 0;
                                if(tmp_data.contains("album_id")){
                                    tmp_albumId = tmp_data["album_id"].toInt();
                                }

                                if(i==this->indexCurrPlay[tabSectionlist[0]]){//cheon211104-01
                                    print_debug();
                                    //this->currMenu = "clb_music";
                                    tmp_dataTrack.isPlay = true;
                                    this->bar_pos = k;
                                    this->noTotal_currntPlay = k;


                                }
                                QString album_art = "";
                                if(tmp_data.contains("album_art")){
                                    album_art = tmp_data["album_art"].toString();
                                }
                                if(album_art!=""){
                                    SqliteHelper *sqlite = new SqliteHelper(this);
                                    QSqlError err = sqlite->addConnectionRose();
                                    if(err.type() == QSqlError::NoError){
                                        QString strQuery = "SELECT album_art FROM album_art WHERE album_id=%1 ";
                                        strQuery = strQuery.arg(tmp_albumId);
                                        QVariantList data;
                                        sqlite->exec(strQuery, data);
                                        if(data.size() > 0){
                                            QJsonObject tmp_data = data.first().toJsonObject();
                                            if(tmp_data.contains("album_art")){
                                                album_art = tmp_data["album_art"].toString();
                                            }
                                        }
                                    }
                                    sqlite->close();
                                    delete sqlite;
                                }
                                tmp_dataTrack.thumbnail = album_art;
                            }
                        }
                        tmp_dataTrack.no = i;//cheon210916-total
                        QString tmp_imgPath = tmp_dataTrack.thumbnail;
                        if(tmp_imgPath.startsWith("http")==false){
                            tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                        }

                        this->list_thumbPath[tabSectionlist[0]].append(tmp_imgPath);
                        this->list_dataTrackForDelegate[tabSectionlist[0]].append(tmp_dataTrack);
                    }

                }
#if defined(Q_OS_WINDOWS)
                // QCoreApplication::processEvents();
#endif
#if defined(Q_OS_MAC)
#endif

            }

            qDebug() << "this->jsonArray_backup_total[global.now_Section].count()=" << this->jsonArray_backup_total[global.now_Section].count();
            print_debug();
        }//else  //cheon211102
    }
    // mtx.unlock();

}

void QueuePlayList::mythreadFunc(){
    forever{

        // 여기서 오래걸리는 작업을 수행


        if ( QThread::currentThread()->isInterruptionRequested() ) {
            return;
        }
    }
}

//int kk = 0;


void QueuePlayList::slot_responseHttp_playbar(const int& p_id, const QJsonObject& p_jsonObj){
    print_debug();
}

void QueuePlayList::slot_thumbnailDownloaded(){//1126
    //print_debug();

    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    int tmp_index = fileDownloader->property("filedown_index").toInt();


    //if(this->list_thumbPath[global.now_Section].count()){
    if(tmp_index>=0 && tmp_index < list_thumbPath_cnt2){

        list_thumbPath_cnt--;
        //qDebug() << "list_thumbPath_cnt = " << list_thumbPath_cnt;
        QImage image;
        bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());





        if(flagLoaded){
            //print_debug();
            //qDebug() << "tmp_index = " << tmp_index;
            image = image.scaled( 50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            QPixmap tmp_pixmap = QPixmap::fromImage(image,  Qt::AutoColor);

            tmp_pixmap = tmp_pixmap.scaled(50,50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            //mutex.lock();
            if(lb_queueListImg[global.now_Section].count() > 0 ){

                if(this->currMenu == CURRMENU_TOTAL){
                    // backupPixmapArrayTotal.replace(tmp_index, tmp_pixmap);
                    //backupPixmapArrayTotal.append(tmp_pixmap);
                }

                //print_debug();
                //lb_queueListImg[global.now_Section].at(tmp_index)->setPixmap(tmp_pixmap);
                //print_debug();
            }
        }else{

        }
        /*
        if(this->indexCurrPlay[global.now_Section] == tmp_index){
            print_debug();
            pixmapPlaying = pixmapPlaying.scaled(40,40, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            lb_queueListImg[global.now_Section].at(tmp_index)->setPixmap(QPixmap(pixmapPlaying));
            print_debug();
        }
        */

    }


    if(list_thumbPath_cnt==0){
        print_debug();
        //qDebug() << "*backupPixmapArrayTotal.count() = " << backupPixmapArrayTotal.count();
        emit signal_assignImg();
    }
    //if(tmp_index == this->list_thumbPath[global.now_Section].count()-1){


    fileDownloader->deleteLater();
}

void QueuePlayList::slot_thumbnailDownloaded_playbarChanged(){
    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    int tmp_index = fileDownloader->property("filedown_index").toInt();
    //print_debug();
    if(tmp_index>=0 && tmp_index < list_thumbPath[global.now_Section].count()){

        QImage image;
        bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());
        QPixmap tmp_pixmap;

        if(flagLoaded){
            QPixmap tmp_pixmap = tmp_pixmap.fromImage(image);

            tmp_pixmap = tmp_pixmap.scaled(50,50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            if(lb_queueListImg[global.now_Section].count() > 0 ){

                if(tmp_index != this->indexCurrPlay[global.now_Section]){
                    this->lb_queueListImg[global.now_Section].at(tmp_index)->setPixmap(tmp_pixmap);
                }else{
                    this->lb_queueListImg[global.now_Section].at(tmp_index)->setPixmap(pixmapPlaying);
                }
            }
        }else{
            this->lb_queueListImg[global.now_Section].at(tmp_index)->setPixmap(QPixmap(pixmapDefault));
        }
        //emit signal_assignImg0();
    }


    fileDownloader->deleteLater();

}

void QueuePlayList::slot_thumbnailDownloaded_playbarChanged_section(){
    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    int tmp_index = fileDownloader->property("filedown_index").toInt();

    int section = tmp_index/10000;
    int index = tmp_index%10000;
    if(index>=0 && index < list_thumbPath[section].count()){

        QImage image;
        bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());
        QPixmap tmp_pixmap;

        if(flagLoaded){
            QPixmap tmp_pixmap = tmp_pixmap.fromImage(image);

            tmp_pixmap = tmp_pixmap.scaled(50,50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            if(lb_queueListImg[section].count() > 0 ){

                //if(this->currMenu == CURRMENU_TOTAL){
                //    backupPixmapArrayTotal.replace(tmp_index, tmp_pixmap);

                //}
                //print_debug();qDebug() << "section=" << section;qDebug() << "tmp_index=" << tmp_index;
                lb_queueListImg[section].at(index)->setPixmap(tmp_pixmap);
            }
        }else{
            this->lb_queueListImg[section].at(index)->setPixmap(QPixmap(pixmapDefault));
        }
        //emit signal_assignImg0();
    }


    fileDownloader->deleteLater();

}




void QueuePlayList::downloadThumbImageMore_playbarChanged(int i){
    //print_debug();

    FileDownloader *fileDownloader = new FileDownloader(this);
    fileDownloader->setProperty("filedown_index", i);
    fileDownloader->setImageURL(this->list_thumbPath[global.now_Section].at(i));

    //pixmapDefault = pixmapDefault.scaled(50,50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    //qDebug() << "this->list_thumbPath[global.now_Section].at(i) = " << "i: " << i << " " << this->list_thumbPath[global.now_Section].at(i);

    disConId[i] = connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded_playbarChanged()));

    //QCoreApplication::processEvents();//c221004_1

    //c1111


}

void QueuePlayList::downloadThumbImageMore_playbarChanged(int section, int i){
    //print_debug();

    FileDownloader *fileDownloader = new FileDownloader(this);
    fileDownloader->setProperty("filedown_index", section*10000+i);
    fileDownloader->setImageURL(this->list_thumbPath[section].at(i));

    //pixmapDefault = pixmapDefault.scaled(50,50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    //qDebug() << "this->list_thumbPath[global.now_Section].at(i) = " << "i: " << i << " " << this->list_thumbPath[global.now_Section].at(i);

    disConId[i] = connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded_playbarChanged_section()));

    //QCoreApplication::processEvents();//c221004_1

    //c1111


}



void QueuePlayList::slot_assignImg(){
    print_debug();


}
void QueuePlayList::slot_assignImg0(){

}

void QueuePlayList::slot_downloadThumbImage(){

    // downloadThumbImageMore_new();
}

void QueuePlayList::downloadThumbImageMore(){//1126


    print_debug();
    mouse_trigger_menu_flag = false;
    qDebug() << "******************mouse_trigger_menu_flag = " << mouse_trigger_menu_flag;

    global.queuelist_mouse_trigger_menu_flag = false;
    qDebug() << "this->currMenu = " << this->currMenu;
    qDebug() << "downloadThumbImage_ing_flag = " << downloadThumbImage_ing_flag;

    qDebug() << "global.current_position = " << global.current_position;
    qDebug() << "global.current_duration = " << global.current_duration;

    qDebug() << "global.current_duration*0.3 = " << global.current_duration*0.3;
    qDebug() << "(global.current_duration - global.current_duration*0.3) = " << (global.current_duration - global.current_duration*0.3);

    qDebug() << "this->currMenu = "<< this->currMenu;
    qDebug() << "this->list_thumbPath[global.now_Section].count() = "<< this->list_thumbPath[global.now_Section].count();
    //if(this->currMenu == CURRMENU_TOTAL || 3000  <  backupPixmapArrayTotal.count() + this->list_thumbPath[global.now_Section].count()){

    print_debug();
    if(this->currMenu==CURRMENU_TOTAL){
        print_debug();
        // backupPixmapArrayTotal.clear();
        //if(backupPixmapArrayTotal.count() > 0){
        //   for(int i = 0; i < this->list_thumbPath[global.now_Section].count(); i++){
        //      QObject::disconnect(disConId[i]);
        //  }
        //}
    }

    //}

    print_debug();qDebug()<<"*************************************************************";
    linker->signal_checkQueue(6, "");
    global.queuelist_mouse_trigger_flag = true;
    mouse_trigger_flag = true;
    print_debug();
    //for(int i = 0; i < disConId_cnt; i++){
    //    QObject::disconnect(disConId[i]);
    //}
    disConId_cnt = this->list_thumbPath[global.now_Section].count();
    pixmapDefault = pixmapDefault.scaled(50,50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    //if(global.current_duration==60000){
    //    emit linker->signal_checkQueue(10, "");
    //}
    if(this->list_thumbPath[global.now_Section].count() == 0){
        emit linker->signal_checkQueue(10, "");
        mouse_trigger_menu_flag = true;
        qDebug() << "******************mouse_trigger_menu_menu_flag = " << mouse_trigger_menu_flag;


        global.queuelist_mouse_trigger_menu_flag = true;
        return;
    }
    qDebug() << "this->indexCurrPlay[global.now_Section] = "<< this->indexCurrPlay[global.now_Section];
    qDebug() << "this->list_thumbPath[global.now_Section].count() = "<< this->list_thumbPath[global.now_Section].count();
#if defined(Q_OS_WINDOWS)

#endif


    //cal_indexStartSetThumb_forSpeedLoop();




    this->indexCurrPlay[global.now_Section] = save_indexCurrPlay[global.now_Section];
    qDebug() << "this->indexCurrPlay[global.now_Section] = "<< this->indexCurrPlay[global.now_Section];

#if defined(Q_OS_WINDOWS)

    if(list_thumbPath_cnt2 > 0){

        QTimer::singleShot(100, this, SLOT(slot_downloadThumbImage()));
        //downloadThumbImageMore_new();
    }else{
        if(current_queue_cnt == 0){
            queProgressBar->setValue(queProgressBar->maximum());
        }
    }

#endif
#if defined(Q_OS_MAC)
    if(list_thumbPath_cnt2 > 0){
        QTimer::singleShot(100, this, SLOT(slot_downloadThumbImage()));
        //downloadThumbImageMore_new();
    }else{

        if(current_queue_cnt == 0){
            queProgressBar->setValue(queProgressBar->maximum());
        }
    }
#endif
    print_debug();
    //global.isPlaying_flag = false;
    //-----------------------------------------------------
    /*

    */
    //-----------------------------------------------------
    print_debug();
    qDebug() << "global.current_position = " << global.current_position;
    qDebug() << "global.current_duration = " << global.current_duration;
    qDebug() << "global.isPlaying = " << global.isPlaying;
    qDebug() << "global.current_duration*0.3 = " << global.current_duration*0.3;
    qDebug() << "(global.current_duration - global.current_duration*0.3) = " << (global.current_duration - (global.current_duration - 4000));


}


/**
 * @brief QueuePlayList:: 이미지 따로 Download Start
 */
void QueuePlayList::downloadThumbImageMore_recomm(){
    print_debug();
    //qDebug() << "this->indexStartSetThumb_recomm= " << this->indexStartSetThumb_recomm;
    qDebug() << "this->list_thumbPath_recomm[global.now_Section].count() --> " << this->list_thumbPath_recomm[global.now_Section].count();
    qDebug() << "lb_queueListImg_recomm[global.now_Section].count()=" << lb_queueListImg_recomm[global.now_Section].count();

    disConIdRecomm_cnt = this->list_thumbPath_recomm[global.now_Section].count();
    for(int i = 0; i<this->list_thumbPath_recomm[global.now_Section].count(); i++){
        //qDebug() << "i=" << i;
        //print_debug();
        //qDebug() << "this->list_thumbPath_recomm[global.now_Section].at(i)= " << this->list_thumbPath_recomm[global.now_Section].at(i);
        //if(this->list_thumbPath_recomm[global.now_Section].count()>0){
        FileDownloader *fileDownloader = new FileDownloader(this);
        fileDownloader->setProperty("filedown_index", i);

        fileDownloader->setImageURL(this->list_thumbPath_recomm[global.now_Section].at(i));
        disConId_recomm[i] = connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded_recomm()));



    }
    emit linker->signal_checkQueue(6, "");
    print_debug();
    mouse_all_clear();
    print_debug();

}

/**
 * @brief QueuePlayList::slot_thumbnailDownloaded : [슬롯] 썸네일 다운로드 완료
 */
void QueuePlayList::slot_thumbnailDownloaded_recomm(){

    // print_debug();
    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    int tmp_index = fileDownloader->property("filedown_index").toInt();
    //print_debug();
    //qDebug()  << "slot_thumbnailDownloaded_recomm----tmp_index = " << tmp_index;
    if(this->list_thumbPath_recomm[global.now_Section].count()>0){
        if(tmp_index >= 0 && tmp_index < this->list_thumbPath_recomm[global.now_Section].count()){

            QImage image;
            bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());
            //QPixmap tmp_pixmap;

            QPixmap tmp_pixmap = tmp_pixmap.fromImage(image);

            if(!tmp_pixmap.isNull()){
                tmp_pixmap = tmp_pixmap.scaled(50,50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            }else {
                tmp_pixmap = pixmapDefault;
            }

            if(flagLoaded){
                //print_debug();
                //qDebug() << "lb_queueListImg_recomm[global.now_Section].count()=" << lb_queueListImg_recomm[global.now_Section].count();

                lb_queueListImg_recomm[global.now_Section].at(tmp_index)->setPixmap(tmp_pixmap);
                //print_debug();
            }else{
                //print_debug();
                //  if(!lb_queueListImg_recomm[global.now_Section].isEmpty())
                lb_queueListImg_recomm[global.now_Section].at(tmp_index)->setPixmap(pixmapDefault);
            }

        }
    }

    fileDownloader->deleteLater();

}
/**
 * @brief QueuePlayList::appendTrackUI : Track UI추가
 * @param p_index int index
 */
void QueuePlayList::appendTrackUI_recomm(const int &p_index){
    //print_debug();


}

void QueuePlayList::TotalQueueListCount_afterGetFile_recomm(const int &p_id, const QJsonObject &p_jsonObject){
    print_debug();
    //recomm_totalQueue_backup = p_jsonObject;
    //this->current_queue_cnt_recomm = 0;
    this->tidal_queue_list_count_recomm = 0;
    this->qobuz_queue_list_count_recomm = 0;
    this->bugs_queue_list_count_recomm = 0;
    this->music_queue_list_count_recomm = 0;
    this->rosetube_queue_list_count_recomm = 0;
    this->applemusic_queue_list_count_recomm = 0;
    this->total_queue_list_count_recomm = 0;
    if(p_jsonObject.contains("flagOk")){
        if(p_jsonObject["flagOk"].toBool()==false){

            print_debug();
            return;

        }else{

            if(p_jsonObject.contains("arr")){
                QJsonArray tmp_arr = p_jsonObject["arr"].toArray();
                //qDebug() << "tmp_arr.count()=" << tmp_arr.count();
                //if(this->jsonArray_backup_tidal.count()>0 && tmp_arr==this->jsonArray_backup_tidal){

                //}
                //else{
                //this->total_queue_list_count_recomm = tmp_arr.count();
                for(int i=0; i<tmp_arr.count(); i++){
                    QJsonObject tmp_json1 = tmp_arr.at(i).toObject();
                    QJsonDocument doc(tmp_json1); QString strJson(doc.toJson(QJsonDocument::Compact));
                    //qDebug() << "QueuePlayList::setTidalQueueList----tmp_json1== " << strJson << "\n";
                    QString tmpStr1;
                    if(tmp_json1.contains("PLAY_TAG")){
                        tmpStr1 = tmp_json1["PLAY_TAG"].toString(); // 현재 진행중인 곡 스타일 설정
                    }
                    if(tmpStr1 ==  "TIDAL"){
                        if(tmp_json1.contains("track")){
                            this->tidal_queue_list_count_recomm++;
                            this->total_queue_list_count_recomm++;
                        }
                    }
                    if(tmpStr1 ==  "BUGS"){
                        if(tmp_json1.contains("track")){
                            this->bugs_queue_list_count_recomm++;
                            this->total_queue_list_count_recomm++;
                        }
                    }
                    if(tmpStr1 ==  "MUSIC"){
                        if(tmp_json1.contains("track")){
                            this->music_queue_list_count_recomm++;
                            this->total_queue_list_count_recomm++;
                        }
                    }
                    if(tmpStr1 ==  "QOBUZ"){
                        if(tmp_json1.contains("track")){
                            this->qobuz_queue_list_count_recomm++;
                            this->total_queue_list_count_recomm++;
                        }
                    }
                    if(tmpStr1 ==  "YOUTUBE"){
                        if(tmp_json1.contains("track")){
                            this->rosetube_queue_list_count_recomm++;
                            this->total_queue_list_count_recomm++;
                        }
                    }
                    if(tmpStr1 ==  "APPLEMUSIC"){
                        if(tmp_json1.contains("track")){
                            this->applemusic_queue_list_count_recomm++;
                            this->total_queue_list_count_recomm++;
                        }
                    }
                }
                //}
            }
        }
    }

    qDebug() << "tidal_queue_list_count_recomm = " << tidal_queue_list_count_recomm;
    qDebug() << "bugs_queue_list_count_recomm = " << bugs_queue_list_count_recomm;
    qDebug() << "music_queue_list_count_recomm = " << music_queue_list_count_recomm;
    qDebug() << "qobuz_queue_list_count_recomm = " << qobuz_queue_list_count_recomm;
    qDebug() << "rosetube_queue_list_count_recomm = " << rosetube_queue_list_count_recomm;
    qDebug() << "applemusic_queue_list_count_recomm = " << applemusic_queue_list_count_recomm;
    qDebug() << "total_queue_list_count_recomm = " << total_queue_list_count_recomm;

}

void QueuePlayList::slot_responseHttp_recomm_check(const int &p_id, const QJsonObject &p_jsonObject){//cheon210916-total

    print_debug();
    QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));
    QString preJsonStr_recomm_curr = strJson;
    if(preJsonStr_recomm_curr == preJsonStr_recomm) {
        recomm_equelRecieveJsonFlag = true;

    }else recomm_equelRecieveJsonFlag = false;

}

/**
 * @brief QueuePlayList::responseHttp : 로즈 장치 정보요청 결과 처리
 * @param p_jsonObject QJsonObject&
 */
void QueuePlayList::slot_responseHttp_recomm(const int &p_id, const QJsonObject &p_jsonObject){//cheon210916-total

    print_debug();
    emit linker->signal_clickListBtn();
    this->TotalQueueListCount_afterGetFile_recomm(p_id, p_jsonObject);

    QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));
    preJsonStr_recomm = strJson;


    //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() << "QueuePlayList::slot_responseHttp_recomm---" << strJson;//cheon211114-01
    qDebug() << "********** p_id== "<< p_id;

    //lb_title->setText(tr("Queue List  (0/%1)").arg(this->total_queue_list_count));//cheon211114-01

    switch(p_id){

    case HTTP_CURR_QUEUE_GET_RECOMM_TOTAL://cheon211114-01

        print_debug();
        this->setTotalQueueList_recomm(p_id, p_jsonObject);
        break;
    case HTTP_CURR_QUEUE_GET_RECOMM_TIDAL://cheon211114-01
        print_debug();
        this->setTotalQueueList_recomm(p_id, p_jsonObject);
        break;
    case HTTP_CURR_QUEUE_GET_RECOMM_BUGS://cheon211114-01
        print_debug();
        this->setTotalQueueList_recomm(p_id, p_jsonObject);
        break;
    case HTTP_CURR_QUEUE_GET_RECOMM_QOBUZ://cheon211114-01
        print_debug();
        this->setTotalQueueList_recomm(p_id, p_jsonObject);
        break;
    case HTTP_CURR_QUEUE_GET_RECOMM_ROSETUBE://cheon211114-01
        print_debug();
        this->setTotalQueueList_recomm(p_id, p_jsonObject);
        break;
    case HTTP_CURR_QUEUE_GET_RECOMM_MUSIC://cheon211114-01
        print_debug();
        this->setTotalQueueList_recomm(p_id, p_jsonObject);
        break;

    case HTTP_CURR_QUEUE_GET_RECOMM_APPLEMUSIC://cheon211114-01
        print_debug();
        this->setTotalQueueList_recomm(p_id, p_jsonObject);
        break;
    }
    print_debug();
    view_listWidget_setItemWidget_recomm(p_id);
    print_debug();

    qDebug() << "QueuePlayList::view_listWidget_setItemWidget_recomm->count()=======================" << this->list_dataTrackForDelegate_recomm[global.now_Section].count();

    qDebug() << "QueuePlayList::view_listWidget_setItemWidget->count()=======================" << this->list_dataTrackForDelegate[global.now_Section].count();
    print_debug();
    sender()->deleteLater();
}

/**
 * @brief QueuePlayList::responseHttp : 로즈 장치 정보요청 결과 처리
 * @param p_jsonObject QJsonObject&
 */
void QueuePlayList::slot_responseHttp_recomm_only(const int &p_id, const QJsonObject &p_jsonObject){//cheon210916-total

    print_debug();
    this->TotalQueueListCount_afterGetFile_recomm(p_id, p_jsonObject);

    QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));
    preJsonStr_recomm = strJson;


    //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() << "QueuePlayList::slot_responseHttp_recomm---" << strJson;//cheon211114-01
    qDebug() << "********** p_id== "<< p_id;

    //lb_title->setText(tr("Queue List  (0/%1)").arg(this->total_queue_list_count));//cheon211114-01

    switch(p_id){

    case HTTP_CURR_QUEUE_GET_RECOMM_TOTAL://cheon211114-01

        print_debug();
        this->setTotalQueueList_recomm(p_id, p_jsonObject);
        break;
    case HTTP_CURR_QUEUE_GET_RECOMM_TIDAL://cheon211114-01
        print_debug();
        this->setTotalQueueList_recomm(p_id, p_jsonObject);
        break;
    case HTTP_CURR_QUEUE_GET_RECOMM_BUGS://cheon211114-01
        print_debug();
        this->setTotalQueueList_recomm(p_id, p_jsonObject);
        break;
    case HTTP_CURR_QUEUE_GET_RECOMM_QOBUZ://cheon211114-01
        print_debug();
        this->setTotalQueueList_recomm(p_id, p_jsonObject);
        break;
    case HTTP_CURR_QUEUE_GET_RECOMM_ROSETUBE://cheon211114-01
        print_debug();
        this->setTotalQueueList_recomm(p_id, p_jsonObject);
        break;
    case HTTP_CURR_QUEUE_GET_RECOMM_MUSIC://cheon211114-01
        print_debug();
        this->setTotalQueueList_recomm(p_id, p_jsonObject);
        break;

    case HTTP_CURR_QUEUE_GET_RECOMM_APPLEMUSIC://cheon211114-01
        print_debug();
        this->setTotalQueueList_recomm(p_id, p_jsonObject);
        break;
    }
    print_debug();
    view_listWidget_setItemWidget_recomm_only(p_id);
    print_debug();
    sender()->deleteLater();
    print_debug();
}

int QueuePlayList::get_fromNextIndexNo_toCurrIndexPlay(){
    int nextIndex = -1;//c1217
    for(int i = 0; i < this->list_dataTrackForDelegate[global.now_Section].count(); i++){

        DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(i);
        if(tmp_dataTrack.no == nextPlayNo){
            print_debug();
            qDebug() << "tmp_dataTrack.no=" << tmp_dataTrack.no;
            qDebug() << "i=" << i;
            nextIndex = i;
            break;
        }


    }
    return nextIndex;
}

int QueuePlayList::get_IndexCurrPlayfromNextIndexNo(const int &p_index){
    //print_debug();
    //qDebug() << "this->list_dataTrackForDelegate[p_index].count()= "<< this->list_dataTrackForDelegate[p_index].count();
    int nextIndex = -1;//c1217
    for(int i = 0; i < this->list_dataTrackForDelegate[p_index].count(); i++){

        DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate[p_index].at(i);
        if(tmp_dataTrack.no == nextPlayNo){
            //print_debug();
            //qDebug() << "tmp_dataTrack.no=" << tmp_dataTrack.no;
            //qDebug() << "i=" << i;
            nextIndex = i;
            break;
        }


    }
    return nextIndex;
}


int QueuePlayList::get_fromABSNextIndexNo_toSUBIndex(const int &p_index){
    int nextIndex = -1;//c1217
    for(int i = 0; i < this->list_dataTrackForDelegate[global.now_Section].count(); i++){

        DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(i);
        if(tmp_dataTrack.no == p_index){
            print_debug();
            qDebug() << "tmp_dataTrack.no=" << tmp_dataTrack.no;
            qDebug() << "i=" << i;
            nextIndex = i;
            break;
        }


    }
    return nextIndex;
}




void QueuePlayList::setChangMenuStyling(QString curr_nemu_play){


    if(currMenutoPid() == HTTP_CURR_QUEUE_GET_TOTAL){
        global.now_Section = 0;
    }else if(curr_nemu_play == "Qobuz"){
        global.now_Section = 8;
    }else if(curr_nemu_play == "Bugs"){
        global.now_Section = 7;
    }else if(curr_nemu_play == "RoseTube"){
        global.now_Section = 4;
    }else if(curr_nemu_play == "Music"){
        global.now_Section = 1;
    }else if(curr_nemu_play == "Apple"){
        global.now_Section = 9;
    }else if(curr_nemu_play == "Tidal"){
        global.now_Section = 6;
    }


    // 모든 버튼 선택해제 스타일로 세팅
    this->clb_total->setStyleSheet(clb_total->styleSheet().replace("#B18658", "transparent"));//cheon210916-total
    this->clb_music->setStyleSheet(clb_music->styleSheet().replace("#B18658", "transparent"));
    this->clb_video->setStyleSheet(clb_video->styleSheet().replace("#B18658", "transparent"));
    this->clb_radio->setStyleSheet(clb_radio->styleSheet().replace("#B18658", "transparent"));
    this->clb_fmtuner->setStyleSheet(clb_fmtuner->styleSheet().replace("#B18658", "transparent"));
    this->clb_roseTube->setStyleSheet(clb_roseTube->styleSheet().replace("#B18658", "transparent"));
    this->clb_potcast->setStyleSheet(clb_potcast->styleSheet().replace("#B18658", "transparent"));
    this->clb_tidal->setStyleSheet(clb_tidal->styleSheet().replace("#B18658", "transparent"));
    this->clb_bugs->setStyleSheet(clb_bugs->styleSheet().replace("#B18658", "transparent"));
    this->clb_qobuz->setStyleSheet(clb_qobuz->styleSheet().replace("#B18658", "transparent"));
    this->clb_apple->setStyleSheet(clb_apple->styleSheet().replace("#B18658", "transparent"));
    qDebug() << "-global.now_Section = " << global.now_Section;
    QString tmp_currMenuNew;
    //global.now_Section = 0;//cheon211120-02



}



void QueuePlayList::slot_queuelistScrollbarMotify(const int &p_id){//c220629
    print_debug();
    qDebug() << "lb_queueListImg[global.now_Section].count() = " << lb_queueListImg[global.now_Section].count();
    qDebug() << "this->list_dataTrackForDelegate[global.now_Section].count() = " << this->list_dataTrackForDelegate[global.now_Section].count();
    qDebug() << "indexCurrPlay[global.now_Section] = " << indexCurrPlay[global.now_Section];
    qDebug() << "global.now_Section = " << global.now_Section;

    global.indexCurrPlay[global.now_Section] = this->indexCurrPlay[global.now_Section];
    if(isPlayMode && global.queueTimerFlag ){


        qDebug() << "this->listWidget_total[global.now_Section]->count() = " << this->listWidget_total[global.now_Section]->count();
        qDebug() << "current_queue_cnt = " << current_queue_cnt;

        if( this->indexCurrPlay[global.now_Section] >= 0){

            if(this->indexCurrPlay[global.now_Section] > global.LmtCnt_H/70){
                this->CurrViewRow = this->indexCurrPlay[global.now_Section] - global.LmtCnt_H/70;
            }else {
                this->CurrViewRow = this->indexCurrPlay[global.now_Section];
            }
#if defined(Q_OS_WINDOWS)
            QListWidgetItem* item = this->listWidget_total[global.now_Section]->item(this->indexCurrPlay[global.now_Section]);

            listWidget_total[global.now_Section]->scrollToItem(item, QAbstractItemView::EnsureVisible);//c220629

#endif
#if defined(Q_OS_MAC)
            listWidget_total[global.now_Section]->setCurrentRow(this->indexCurrPlay[global.now_Section]);//c220629
#endif
            print_debug();

        }else{

#if defined(Q_OS_WINDOWS)
            QListWidgetItem* item = this->listWidget_total[global.now_Section]->item(0);

            listWidget_total[global.now_Section]->scrollToItem(item, QAbstractItemView::EnsureVisible);//c220629

#endif
#if defined(Q_OS_MAC)
            listWidget_total[global.now_Section]->setCurrentRow(0);//c220629
#endif
            print_debug();

        }

        slot_reGet_checkSum();
        qDebug() << "QueuePlayList::slot_queuelistScrollbarMotify---this->indexCurrPlay[global.now_Section]= " << this->indexCurrPlay[global.now_Section];
    }

}


void QueuePlayList::slot_queuelistScrollbarMotify_menuChange(const int &p_id){//1126
    print_debug();
    qDebug() << "lb_queueListImg[global.now_Section].count() = " << lb_queueListImg[global.now_Section].count();
    qDebug() << "this->list_dataTrackForDelegate[global.now_Section].count() = " << this->list_dataTrackForDelegate[global.now_Section].count();
    qDebug() << "this->list_dataTrackForDelegate[global.now_Section].count() = " << this->list_dataTrackForDelegate_Total.count();
    qDebug() << "indexCurrPlay[global.now_Section] = " << indexCurrPlay[global.now_Section];
    qDebug() << "nextPlayNo = " << nextPlayNo;

    int currIndex = get_fromNextIndexNo_toCurrIndexPlay();
    qDebug() << "currIndex = " << currIndex;

    if(nextPlayNo < this->list_dataTrackForDelegate_Total.count() && this->list_dataTrackForDelegate_Total.count() > 0 && nextPlayNo >= 0){
        print_debug();
        DataTrackforDelegate tmp_dataTrack;
        tmp_dataTrack = this->list_dataTrackForDelegate_Total.at(nextPlayNo);
        print_debug();
        QString type = "None";
        int curr_menu_play = -1;
        if(list_dataTrackForDelegate_Total.at(this->nextPlayNo).type == DataTrackforDelegate::Types_Data::Music){
            type = "Music";
            curr_menu_play = HTTP_CURR_QUEUE_GET_MUSIC;
        }else if(list_dataTrackForDelegate_Total.at(this->nextPlayNo).type == DataTrackforDelegate::Types_Data::AppleMusic){
            type = "Apple";
            curr_menu_play = HTTP_CURR_QUEUE_GET_APPLEMUSIC;
        }else if(list_dataTrackForDelegate_Total.at(this->nextPlayNo).type == DataTrackforDelegate::Types_Data::Qobuz){
            type = "Qobuz";
            curr_menu_play = HTTP_CURR_QUEUE_GET_QOBUZ;
        }else if(list_dataTrackForDelegate_Total.at(this->nextPlayNo).type == DataTrackforDelegate::Types_Data::Tidal){
            type = "Tidal";
            curr_menu_play = HTTP_CURR_QUEUE_GET_TIDAL;
        }else if(list_dataTrackForDelegate_Total.at(this->nextPlayNo).type == DataTrackforDelegate::Types_Data::Bugs){
            type = "Bugs";
            curr_menu_play = HTTP_CURR_QUEUE_GET_BUGS;
        }else if(list_dataTrackForDelegate_Total.at(this->nextPlayNo).type == DataTrackforDelegate::Types_Data::RoseTube){
            type = "RoseTube";
            curr_menu_play = HTTP_CURR_QUEUE_GET_ROSETUBE;
        }
        print_debug();
        QString msg_str;
        msg_str = QString(tr(" Currently Playing : %1(%2), Title : %3")).arg(type).arg(this->nextPlayNo).arg(this->list_dataTrackForDelegate_Total.at(this->nextPlayNo).title);
        qDebug() << "msg_str = " << msg_str;

        //if((currMenutoPid()!= curr_menu_play) || currMenutoPid() == HTTP_PLAY_CURRENT_MUSIC){
        //    this->lb_position_site->setText(QString("%1: %2(%3)").arg(type).arg("All").arg(this->nextPlayNo));
        //    this->lb_position_title->setText(this->list_dataTrackForDelegate_Total.at(this->nextPlayNo).title);

        //this->widget_position_msg->show();
        //QTimer::singleShot(2000, this->widget_position_msg, SLOT(hide()));
        //    global.now_Section = 0;
        //    setChangMenuStyling(type);
        //   print_debug();
        //reloadList();

        // }else{
        this->lb_position_site->setText(QString("%1(%2)").arg(type).arg(currIndex));
        this->lb_position_title->setText(this->list_dataTrackForDelegate_Total.at(this->nextPlayNo).title);

        //this->widget_position_msg->show();
        //QTimer::singleShot(2000, this->widget_position_msg, SLOT(hide()));
        //}




    }



    if(this->listWidget_total[global.now_Section]->count() >= currIndex && currIndex >= 0){//c220629
#if defined(Q_OS_WINDOWS)
        QListWidgetItem* item = this->listWidget_total[global.now_Section]->item(currIndex);
        listWidget_total[global.now_Section]->scrollToItem(item, QAbstractItemView::EnsureVisible);
        print_debug();
#endif
#if defined(Q_OS_MAC)
        listWidget_total[global.now_Section]->setCurrentRow(currIndex);//c220629
#endif


    }

    //print_debug();
    qDebug() << "QueuePlayList::slot_queuelistScrollbarMotify---this->indexCurrPlay[global.now_Section]= " << this->indexCurrPlay[global.now_Section];


}


void QueuePlayList::slot_queuelistDelay(){

}



void QueuePlayList::requestRecom_check(const int &p_id){
    QJsonObject tmp_json2;//cheon211114-01
    NetworkHttp *network_get2 = new NetworkHttp;//cheon211114-01

    connect(network_get2, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_recomm_check(int, QJsonObject)));

    if(p_id == HTTP_CURR_QUEUE_GET_TOTAL){//cheon211114-01
        print_debug();//HTTP_CURR_QUEUE_GET_RECOMM_TOTAL
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_TOTAL, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01

    }else if(p_id == HTTP_CURR_QUEUE_GET_MUSIC ){
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_MUSIC, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01

    }else if(p_id==HTTP_CURR_QUEUE_GET_ROSETUBE ){
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_ROSETUBE, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01

    }else if(p_id==HTTP_CURR_QUEUE_GET_TIDAL){
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_TIDAL, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01
        print_debug();
    }else if(p_id==HTTP_CURR_QUEUE_GET_BUGS){
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_BUGS, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01


    }else if(p_id==HTTP_CURR_QUEUE_GET_QOBUZ ){//REMOTE_TOTAL_QUEUE_GET
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_QOBUZ, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01


    }else if(p_id==HTTP_CURR_QUEUE_GET_APPLEMUSIC ){//REMOTE_TOTAL_QUEUE_GET
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_APPLEMUSIC, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01


    }
    print_debug();

}

void QueuePlayList::slot_responseHttp_recomm_createlist_only(const int &p_id){//c1217

    //----------------------------recommand_title insert
    print_debug();


    for(int i = current_queue_cnt; i < current_queue_cnt+total_queue_list_count_recomm+1 ; i++)
    {
        //print_debug();
        listWidget_total[global.now_Section]->takeItem(this->list_dataTrackForDelegate[tabSectionlist[0]].count());


    }

    listWidget_recommTitledrawingUI();

    QJsonObject tmp_json2;//cheon211114-01
    NetworkHttp *network_get2 = new NetworkHttp;//cheon211114-01

    connect(network_get2, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_recomm(int, QJsonObject)));

    if(p_id == HTTP_CURR_QUEUE_GET_TOTAL){//cheon211114-01
        print_debug();//HTTP_CURR_QUEUE_GET_RECOMM_TOTAL
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_TOTAL, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01

    }else if(p_id == HTTP_CURR_QUEUE_GET_MUSIC ){
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_MUSIC, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01

    }else if(p_id==HTTP_CURR_QUEUE_GET_ROSETUBE ){
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_ROSETUBE, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01

    }else if(p_id==HTTP_CURR_QUEUE_GET_TIDAL){
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_TIDAL, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01
        print_debug();
    }else if(p_id==HTTP_CURR_QUEUE_GET_BUGS){
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_BUGS, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01


    }else if(p_id==HTTP_CURR_QUEUE_GET_QOBUZ ){//REMOTE_TOTAL_QUEUE_GET
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_QOBUZ, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01


    }else if(p_id==HTTP_CURR_QUEUE_GET_APPLEMUSIC ){//REMOTE_TOTAL_QUEUE_GET
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_APPLEMUSIC, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01


    }
    print_debug();


}


void QueuePlayList::slot_responseHttp_recomm_createlist(const int &p_id){//c1217

    //----------------------------recommand_title insert
    print_debug();


    for(int i = current_queue_cnt; i < current_queue_cnt+total_queue_list_count_recomm+1 ; i++)
    {
        //print_debug();
        listWidget_total[global.now_Section]->takeItem(this->list_dataTrackForDelegate[tabSectionlist[0]].count());


    }

    listWidget_recommTitledrawingUI();

    QJsonObject tmp_json2;//cheon211114-01
    NetworkHttp *network_get2 = new NetworkHttp;//cheon211114-01

    connect(network_get2, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_recomm(int, QJsonObject)));

    if(p_id == HTTP_CURR_QUEUE_GET_TOTAL){//cheon211114-01
        print_debug();//HTTP_CURR_QUEUE_GET_RECOMM_TOTAL
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_TOTAL, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01

    }else if(p_id == HTTP_CURR_QUEUE_GET_MUSIC ){
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_MUSIC, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01

    }else if(p_id==HTTP_CURR_QUEUE_GET_ROSETUBE ){
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_ROSETUBE, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01

    }else if(p_id==HTTP_CURR_QUEUE_GET_TIDAL){
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_TIDAL, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01
        print_debug();
    }else if(p_id==HTTP_CURR_QUEUE_GET_BUGS){
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_BUGS, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01


    }else if(p_id==HTTP_CURR_QUEUE_GET_QOBUZ ){//REMOTE_TOTAL_QUEUE_GET
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_QOBUZ, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01


    }else if(p_id==HTTP_CURR_QUEUE_GET_APPLEMUSIC ){//REMOTE_TOTAL_QUEUE_GET
        print_debug();
        network_get2->request(HTTP_CURR_QUEUE_GET_RECOMM_APPLEMUSIC, QString("http://%1:%2/%3")//cheon211104-01
                              .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_RECOMM_GET), tmp_json2, false, true);//cheon211114-01


    }

}



void QueuePlayList::setTotalQueueList_recomm(int p_id, const QJsonObject &p_jsonObject){  //c220609
    print_debug();
    qDebug() << "p_id = " << p_id;

    //QJsonDocument doc(p_jsonObject);QString strJson(doc.toJson(QJsonDocument::Compact));qDebug() << "QueuePlayList::setTotalQueueList_recomm---- " << strJson << "\n";

    //lb_title->setText(tr("Queue List  (%1/%2)").arg(this->indexCurrPlay[global.now_Section]).arg(this->total_queue_list_count));//cheon211114-01
    qDebug() << "QueuePlayList::setTotalQueueList_recomm---this->indexCurrPlay[global.now_Section]- " << this->indexCurrPlay[global.now_Section] << "\n";
    if(p_jsonObject.contains("flagOk")){
        if(p_jsonObject["flagOk"].toBool()==false){
            //this->listWidget_recomm->clear();
            this->jsonArray_backup_total_recomm[global.now_Section] = QJsonArray();
            this->list_dataTrackForDelegate_recomm[global.now_Section].clear();
            return;
        }else{
            if(p_jsonObject.contains("mode")){

                this->queMode = p_jsonObject["mode"].toBool(); // 현재 진행중인 곡 스타일 설정
                global.onOffModeFlag = this->queMode;
                this->onOff->setValue(this->queMode);
            }
        }
    }
    if(p_jsonObject.contains("arr")){
        QJsonArray tmp_arr = p_jsonObject["arr"].toArray();
        this->jsonArray_backup_total_recomm[global.now_Section] = QJsonArray();
        this->jsonArray_backup_tidal_recomm = QJsonArray();
        this->jsonArray_backup_qobuz_recomm = QJsonArray();
        this->jsonArray_backup_bugs_recomm = QJsonArray();
        this->jsonArray_backup_music_recomm = QJsonArray();
        this->jsonArray_backup_apple_recomm = QJsonArray();
        this->jsonArray_backup_youtube_recomm = QJsonArray();

        this->list_dataTrackForDelegate_recomm[global.now_Section].clear();
        //print_debug();
        this->list_thumbPath_recomm[global.now_Section].clear();
        this->indexStartSetThumb_recomm = 0;

        //this->lb_queueListImg_recomm[global.now_Section].clear();
        int k = 0;//cheon210916-total
        //print_debug();
        //bool indexCurrPlay[global.now_Section]Flag = false;//cheon211103
        //k = +qobuz_queue_list_count;
        for(int i=0; i<tmp_arr.count(); i++){
            QJsonObject tmp_json1 = tmp_arr.at(i).toObject();
            //0927QJsonDocument doc(tmp_json1); QString strJson(doc.toJson(QJsonDocument::Compact));
            //0927qDebug() << "QueuePlayList::setQobuzQueueList----tmp_json1== " << strJson << "\n";
            QString tmpStr1;
            if(tmp_json1.contains("PLAY_TAG")){

                tmpStr1 = tmp_json1["PLAY_TAG"].toString(); // 현재 진행중인 곡 스타일 설정
            }
            if(tmpStr1 ==  "APPLE_MUSIC"  && (p_id == HTTP_CURR_QUEUE_GET_RECOMM_APPLEMUSIC || p_id == HTTP_CURR_QUEUE_GET_RECOMM_TOTAL)){

                if(tmp_json1.contains("track")){

                    //print_debug();
                    //qDebug() << "jsonprintFlag_Ap = " << jsonprintFlag_Ap;
                    //if(jsonprintFlag_Ap==true){
                    //    QString tmp_string11 = tmp_json1["track"].toString();
                    //    qDebug() << "QueuePlayList::setQobuzQueueList---tmp_string : " << tmp_string11 << "\n";
                    //QJsonObject tmp_json = tmp_json1["track"].toObject();
                    //QJsonDocument doc(tmp_json); QString strJson(doc.toJson(QJsonDocument::Compact));
                    //qDebug() << "QueuePlayList::setQobuzQueueList--APPLE-tmp_json : " << strJson << "\n";
                    //    jsonprintFlag_Ap = false;

                    //}
                    QString tmp_string = tmp_json1["track"].toString();
                    //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_string : " << tmp_string << "\n";
                    //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                    QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                    QJsonObject tmp_track = json.object(); // since your string is an JSON object

                    //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                    //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_track : " << strJson << "\n";
                    this->jsonArray_backup_apple_recomm.append(tmp_track);//cheon210916-total
                    this->jsonArray_backup_total_recomm[global.now_Section].append(tmp_track);//cheon210916-total
                    qobuz::TrackItemData_apple data_track = qobuz::ConvertData::make_trackData_fromAppleJsonObj(tmp_track);
                    //print_debug();
                    //qDebug() << "data_track.title = " << data_track.title;
                    // 1> 데이터 담기
                    DataTrackforDelegate tmp_dataTrack;
                    tmp_dataTrack.type = DataTrackforDelegate::Types_Data::AppleMusic;
                    //tmp_dataTrack.addtimeResult = addtimeResult;
                    if(data_track.work.size() > 0){
                        tmp_dataTrack.title = data_track.title + " - " + data_track.work;
                    }else{
                        tmp_dataTrack.title = data_track.title;
                    }
                    tmp_dataTrack.artist = data_track.list_artist_name.join(",");
                    tmp_dataTrack.duration = data_track.duration;
                    if(data_track.hires){
                        tmp_dataTrack.audioQuality = "HI_RES";
                    }
                    //tmp_dataTrack.audioQuality = data_track.hires;
                    if(tmp_dataTrack.artist == ""){
                        tmp_dataTrack.artist = data_track.performer_name;
                    }
                    //data_track.album_image_thumbnail.replace("{w}x{h}bb.jpg", QString("%1").arg(data_track.image_width)+"x"+QString("%1").arg(data_track.image_height)+"bb.jpg");
                    data_track.album_image_thumbnail.replace("{w}x{h}", "60x60");
                    tmp_dataTrack.thumbnail = data_track.album_image_thumbnail;
                    tmp_dataTrack.no = i;


                    if(i==this->indexCurrPlay[global.now_Section]){//cheon211104-01
                        print_debug();
                        //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                        //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_track : " << strJson << "\n";
                        qDebug() << "tmp_dataTrack.title= " << tmp_dataTrack.title;
                        //this->currMenu = "clb_qobuz";
                        this->noTotal_currntPlay = k;

                        tmp_dataTrack.isPlay = true;
                    }

                    QString tmp_imgPath = tmp_dataTrack.thumbnail;
                    if(tmp_imgPath.startsWith("http")==false){
                        tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                    }

                    this->list_thumbPath_recomm[global.now_Section].append(tmp_imgPath);
                    this->list_dataTrackForDelegate_recomm[global.now_Section].append(tmp_dataTrack);

                    // 2> UI에 데이터 세팅
                    this->appendTrackUI_recomm(k);//cheon210916-total


                }
                k++;//cheon210916-total
            }else if(tmpStr1 ==  "QOBUZ"  && (p_id == HTTP_CURR_QUEUE_GET_RECOMM_QOBUZ  || p_id == HTTP_CURR_QUEUE_GET_RECOMM_TOTAL )){




                //this->jsonArray_backup_qobuz_recomm.append(tmp_json1);//cheon210916-total
                //this->jsonArray_backup_total_recomm[global.now_Section].append(tmp_json1);//cheon210916-total
                if(tmp_json1.contains("track")){

                    //print_debug();
                    QString tmp_string = tmp_json1["track"].toString();
                    //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_string : " << tmp_string << "\n";
                    //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                    QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                    QJsonObject tmp_track = json.object(); // since your string is an JSON object

                    //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                    //qDebug() << "QueuePlayList::setQobuzQueueList---tmp_track : " << strJson << "\n";
                    this->jsonArray_backup_qobuz_recomm.append(tmp_track);//cheon210916-total
                    this->jsonArray_backup_total_recomm[global.now_Section].append(tmp_track);//cheon210916-total
                    qobuz::TrackItemData data_track = qobuz::ConvertData::make_trackData_fromQobuzJsonObj(tmp_track);

                    // 1> 데이터 담기
                    DataTrackforDelegate tmp_dataTrack;
                    tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Qobuz;
                    tmp_dataTrack.title = data_track.title;
                    tmp_dataTrack.artist = data_track.list_artist_name.join(",");
                    tmp_dataTrack.duration = data_track.duration;
                    tmp_dataTrack.thumbnail = data_track.album_image_thumbnail;
                    if(data_track.hires){
                        tmp_dataTrack.audioQuality = "HI_RES";
                    }
                    tmp_dataTrack.no = i;
                    if(i==this->indexCurrPlay[global.now_Section]){//cheon211104-01
                        //print_debug();
                        qDebug() << "i = "<< i;

                        // tmp_dataTrack.isPlay = true;
                    }

                    QString tmp_imgPath = tmp_dataTrack.thumbnail;
                    if(tmp_imgPath.startsWith("http")==false){
                        tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                    }
                    //print_debug();
                    this->list_thumbPath_recomm[global.now_Section].append(tmp_imgPath);
                    this->list_dataTrackForDelegate_recomm[global.now_Section].append(tmp_dataTrack);

                    // 2> UI에 데이터 세팅
                    this->appendTrackUI_recomm(k);//cheon210916-total

                }
                k++;//cheon210916-total
            }else if(tmpStr1 ==  "TIDAL"  && (p_id == HTTP_CURR_QUEUE_GET_RECOMM_TIDAL  || p_id == HTTP_CURR_QUEUE_GET_RECOMM_TOTAL )){
                //this->jsonArray_backup_tidal_recomm.append(tmp_json1);//cheon210916-total
                //this->jsonArray_backup_total_recomm[global.now_Section].append(tmp_json1);//cheon210916-total
                if(tmp_json1.contains("track")){

                    //print_debug();
                    QString tmp_string = tmp_json1["track"].toString();
                    //qDebug() << "QueuePlayList::setTidalQueueList---tmp_string : " << tmp_string << "\n";
                    //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                    QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                    QJsonObject tmp_track = json.object(); // since your string is an JSON object

                    //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                    //qDebug() << "QueuePlayList::setTidalQueueList---tmp_track : " << strJson << "\n";
                    this->jsonArray_backup_tidal_recomm.append(tmp_track);//cheon210916-total
                    this->jsonArray_backup_total_recomm[global.now_Section].append(tmp_track);//cheon210916-total
                    tidal::TrackItemData data_track = tidal::ConvertData::make_trackData_fromTidalJsonObj(tmp_track);

                    // 1> 데이터 담기
                    DataTrackforDelegate tmp_dataTrack;
                    tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Tidal;
                    tmp_dataTrack.title = data_track.title;
                    tmp_dataTrack.artist = data_track.list_artist_name.join(",");
                    tmp_dataTrack.thumbnail = data_track.image;
                    tmp_dataTrack.duration = data_track.duration;
                    //qDebug() << tmp_dataTrack.duration;
                    if(data_track.audioQuality == "HI_RES"){
                        tmp_dataTrack.audioQuality = "MQA";
                    }
                    tmp_dataTrack.no = i;


                    if(i==this->indexCurrPlay[global.now_Section]){//cheon211104-01

                        //tmp_dataTrack.isPlay = true;
                    }

                    QString tmp_imgPath = tmp_dataTrack.thumbnail;
                    if(tmp_imgPath.startsWith("http")==false){
                        tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                    }

                    this->list_thumbPath_recomm[global.now_Section].append(tmp_imgPath);
                    this->list_dataTrackForDelegate_recomm[global.now_Section].append(tmp_dataTrack);
                    //print_debug();
                    // 2> UI에 데이터 세팅
                    this->appendTrackUI_recomm(k);//cheon210916-total

                }

                k++;//cheon210916-total

            }else if(tmpStr1 ==  "BUGS"  && (p_id == HTTP_CURR_QUEUE_GET_RECOMM_BUGS  || p_id == HTTP_CURR_QUEUE_GET_RECOMM_BUGSMV   || p_id == HTTP_CURR_QUEUE_GET_RECOMM_TOTAL )){
                //this->jsonArray_backup_bugs_recomm.append(tmp_json1);//cheon210916-total
                //this->jsonArray_backup_total_recomm[global.now_Section].append(tmp_json1);//cheon210916-total
                //print_debug();
                if(tmp_json1.contains("track")){
                    //print_debug();
                    //print_debug();
                    QString tmp_string = tmp_json1["track"].toString();
                    //qDebug() << "QueuePlayList::setTidalQueueList---tmp_string : " << tmp_string << "\n";
                    //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                    QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                    QJsonObject tmp_track = json.object(); // since your string is an JSON object

                    //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                    //qDebug() << "QueuePlayList::setTidalQueueList---tmp_track : " << strJson << "\n";
                    this->jsonArray_backup_bugs_recomm.append(tmp_track);//cheon210916-total
                    this->jsonArray_backup_total_recomm[global.now_Section].append(tmp_track);//cheon210916-total
                    bugs::TrackItemData data_track = bugs::ConvertData_forBugs::make_trackData_fromBugsJsonObj(tmp_track);

                    DataTrackforDelegate tmp_dataTrack;
                    if(tmp_track.contains("mv_title")){
                        //print_debug();
                        bugs::VideoItemData data_video = bugs::ConvertData_forBugs::make_videoData_fromBugsJsonObj(tmp_track);


                        // 1> 데이터 담기

                        tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Bugs;
                        //tmp_dataTrack.addtimeResult = addtimeResult;
                        tmp_dataTrack.title = data_video.mv_title;
                        tmp_dataTrack.artist = data_video.list_artist_nm.join(",");
                        tmp_dataTrack.len = data_video.len;
                        if(data_video.bitrates.contains("fullhd") == true){
                            tmp_dataTrack.audioQuality = "FULL HD";
                        }
                        else if(data_video.bitrates.contains("hd") == true){
                            tmp_dataTrack.audioQuality = "HD";
                        }
                        else if(data_video.bitrates.contains("sd") == true){
                            tmp_dataTrack.audioQuality = "SD";
                        }

                        tmp_dataTrack.thumbnail = data_video.image; //data_track.album_image;
                    }else{
                        //print_debug();
                        bugs::TrackItemData data_track = bugs::ConvertData_forBugs::make_trackData_fromBugsJsonObj(tmp_track);

                        // 1> 데이터 담기

                        //tmp_dataTrack.addtimeResult = addtimeResult;
                        tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Bugs;
                        tmp_dataTrack.title = data_track.track_title;

                        tmp_dataTrack.artist = data_track.list_artist_nm.join(",");
                        tmp_dataTrack.audioQuality = data_track.bitrates;
                        tmp_dataTrack.len = data_track.len;
                        tmp_dataTrack.thumbnail = data_track.album_image;
                    }
                    // 1> 데이터 담기

                    tmp_dataTrack.no = i;

                    if(i==this->indexCurrPlay[global.now_Section]){//cheon211104-01
                        // tmp_dataTrack.isPlay = true;
                    }

                    QString tmp_imgPath = tmp_dataTrack.thumbnail;
                    if(tmp_imgPath.startsWith("http")==false){
                        tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                    }

                    this->list_thumbPath_recomm[global.now_Section].append(tmp_imgPath);
                    this->list_dataTrackForDelegate_recomm[global.now_Section].append(tmp_dataTrack);

                    // 2> UI에 데이터 세팅
                    this->appendTrackUI_recomm(k);//cheon210916-total

                }else if(tmp_json1.contains("mvs")){//cheon211006


                    //print_debug();
                    QString tmp_string = tmp_json1["mvs"].toString();
                    qDebug() << "QueuePlayList::setBugsQueueList--mvs-tmp_string : " << tmp_string << "\n";
                    //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                    QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                    QJsonObject tmp_track = json.object(); // since your string is an JSON object

                    //QJsonDocument doc(tmp_track); QString strJson(doc.toJson(QJsonDocument::Compact));
                    //qDebug() << "QueuePlayList::setBugsQueueList---tmp_track : " << strJson << "\n";
                    this->jsonArray_backup_bugs_recomm.append(tmp_track);//cheon210916-total
                    this->jsonArray_backup_total_recomm[global.now_Section].append(tmp_track);//cheon210916-total
                    bugs::VideoItemData data_video = bugs::ConvertData_forBugs::make_videoData_fromBugsJsonObj(tmp_arr.at(i).toObject());


                    // 1> 데이터 담기
                    DataTrackforDelegate tmp_dataTrack;
                    tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Bugs;
                    tmp_dataTrack.title = data_video.mv_title;
                    tmp_dataTrack.artist = data_video.list_artist_nm.join(",");
                    tmp_dataTrack.len = data_video.len;
                    if(data_video.bitrates.contains("fullhd") == true){
                        tmp_dataTrack.audioQuality = "FULL HD";
                    }
                    else if(data_video.bitrates.contains("hd") == true){
                        tmp_dataTrack.audioQuality = "HD";
                    }
                    else if(data_video.bitrates.contains("sd") == true){
                        tmp_dataTrack.audioQuality = "SD";
                    }

                    tmp_dataTrack.thumbnail = data_video.image; //data_track.album_image;
                    tmp_dataTrack.no = i;//cheon211107-01

                    if(i==this->indexCurrPlay[global.now_Section]){//cheon211104-01
                        //tmp_dataTrack.isPlay = true;
                    }

                    QString tmp_imgPath = tmp_dataTrack.thumbnail;
                    if(tmp_imgPath.startsWith("http")==false){
                        tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                    }

                    this->list_thumbPath_recomm[global.now_Section].append(tmp_imgPath);
                    this->list_dataTrackForDelegate_recomm[global.now_Section].append(tmp_dataTrack);

                    // 2> UI에 데이터 세팅
                    this->appendTrackUI(k);//cheon210916-total

                }

                k++;//cheon210916-total

            }else if(tmpStr1 ==  "YOUTUBE"  && (p_id == HTTP_CURR_QUEUE_GET_RECOMM_ROSETUBE  || p_id == HTTP_CURR_QUEUE_GET_RECOMM_TOTAL )){
                //this->jsonArray_backup_youtube_recomm.append(tmp_json1);//cheon210916-total
                //this->jsonArray_backup_total_recomm[global.now_Section].append(tmp_json1);//cheon210916-total
                if(tmp_json1.contains("track")){
                    //if(tmp_arr.count() > 0){//cheon211104-01
                    //    this->indexCurrPlay[global.now_Section] = this->jsonArray_backup_youtube.count();//cheon210916-total
                    //}
                    //QJsonArray tmp_arr1 = p_jsonObject["track"].toArray();//cheon210916-total


                    //print_debug();
                    QString tmp_string = tmp_json1["track"].toString();
                    //qDebug() << "QueuePlayList::setRoseTubeQueueList---tmp_string : " << tmp_string << "\n";
                    //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                    QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                    QJsonObject tmp_jsonObject = json.object(); // since your string is an JSON object

                    QJsonDocument doc(tmp_jsonObject); QString strJson(doc.toJson(QJsonDocument::Compact));
                    //qDebug() << "QueuePlayList::setRoseTubeQueueList---tmp_jsonObject : " << strJson << "\n";
                    this->jsonArray_backup_youtube_recomm.append(tmp_jsonObject);//cheon210916-total
                    this->jsonArray_backup_total_recomm[global.now_Section].append(tmp_jsonObject);//cheon210916-total

                    //tidal::TrackItemData data_track = tidal::ConvertData::make_trackData_fromTidalJsonObj(tmp_track);
                    //QJsonObject tmp_jsonObject = tmp_arr[i].toObject();
                    // 1> 데이터 담기
                    DataTrackforDelegate tmp_dataTrack;
                    tmp_dataTrack.type = DataTrackforDelegate::Types_Data::RoseTube;
                    if(tmp_jsonObject.contains("title")){
                        tmp_dataTrack.title = tmp_jsonObject["title"].toString();
                    }
                    if(tmp_jsonObject.contains("channelName")){
                        tmp_dataTrack.artist = tmp_jsonObject["channelName"].toString();
                    }
                    if(tmp_jsonObject.contains("duration")){
                        //tmp_dataTrack.duration = data_track.duration;
                        tmp_dataTrack.len = tmp_jsonObject["duration"].toString();//c220521
                        //tmp_dataTrack.duration = tmp.toInt();//c220521
                    }
                    if(tmp_jsonObject.contains("thumbnailUrl")){
                        tmp_dataTrack.thumbnail = tmp_jsonObject["thumbnailUrl"].toString();
                    }

                    if(i==this->indexCurrPlay[global.now_Section]){//cheon211104-01
                        //tmp_dataTrack.isPlay = true;
                    }
                    tmp_dataTrack.no = i;//cheon210916-total


                    QString tmp_imgPath = tmp_dataTrack.thumbnail;
                    if(tmp_imgPath.startsWith("http")==false){
                        tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                    }

                    this->list_thumbPath_recomm[global.now_Section].append(tmp_imgPath);
                    this->list_dataTrackForDelegate_recomm[global.now_Section].append(tmp_dataTrack);
                    //print_debug();
                    //qDebug() << "this->jsonArray_backup_youtube.count()=" << this->jsonArray_backup_youtube.count();//cheon210916-total
                    // 2> UI에 데이터 세팅
                    this->appendTrackUI_recomm(k++);//cheon210916-total
                    //print_debug();

                }//if(tmp_json1.contains("track"))



            }//if(tmpStr1 ==
            else if(tmpStr1 ==  "MUSIC"  && (p_id == HTTP_CURR_QUEUE_GET_RECOMM_MUSIC  || p_id == HTTP_CURR_QUEUE_GET_RECOMM_TOTAL )){
                //this->jsonArray_backup_music_recomm.append(tmp_json1);//cheon210916-total
                //this->jsonArray_backup_total_recomm[global.now_Section].append(tmp_json1);//cheon210916-total
                //print_debug();
                //qDebug() << "**************i=" <<i << ", this->indexCurrPlay[global.now_Section]=" << this->indexCurrPlay[global.now_Section];
                //QJsonDocument doc(tmp_json1);  QString strJson1(doc.toJson(QJsonDocument::Compact));  qDebug() << "tmp_json1---" << strJson1;



                if(tmp_json1.contains("track")){


                    //print_debug();
                    QString tmp_string = tmp_json1["track"].toString();
                    //qDebug() << "QueuePlayList::setMusicQueueList---tmp_string : " << tmp_string << "\n";
                    //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                    QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                    QJsonObject tmp_jsonObject = json.object(); // since your string is an JSON object

                    //QJsonDocument doc(tmp_jsonObject); QString strJson(doc.toJson(QJsonDocument::Compact));//cheon211114-01
                    //qDebug() << "QueuePlayList::setMusicQueueList---tmp_jsonObject : " << strJson << "\n";
                    this->jsonArray_backup_music_recomm.append(tmp_jsonObject);//cheon210916-total
                    this->jsonArray_backup_total_recomm[global.now_Section].append(tmp_jsonObject);//cheon210916-total

                    //tidal::TrackItemData data_track = tidal::ConvertData::make_trackData_fromTidalJsonObj(tmp_track);
                    //QJsonObject tmp_jsonObject = tmp_arr[i].toObject();
                    // 1> 데이터 담기
                    DataTrackforDelegate tmp_dataTrack;
                    //QJsonObject tmp_jsonObject = tmp_arr[i].toObject();
                    bool isDirect = false;                  // 음악 폴더에서 재생한건지 여부
                    if(tmp_jsonObject.contains("isDirect")){
                        isDirect = tmp_jsonObject["isDirect"].toBool();
                    }
                    if(isDirect==true){

                        if(tmp_jsonObject.contains("mDirectFileInfo")){
                            print_debug();
                            QJsonObject tmp_data = tmp_jsonObject["mDirectFileInfo"].toObject();
                            QString tmp_ip = "";

                            // 1> 데이터 담기
                            tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Music;
                            tmp_dataTrack.isDirect = true;
                            if(tmp_data.contains("ip")){
                                tmp_ip = tmp_data["ip"].toString().replace("?", "");
                            }
                            if(tmp_data.contains("smbParamPath")){
                                if(tmp_data["smbParamPath"].toString()!=""){
                                    // 네트워크 노래
                                }else{
                                    // 친구 음원 (공유음원) - 나의 맥주소와 다른경우
                                    if(tmp_ip!="" && (tmp_ip!=global.device.getDeviceID())){
                                        tmp_dataTrack.isShare = true;
                                    }
                                }
                            }
                            if(tmp_data.contains("duration")){
                                //qDebug() << "duration = " << tmp_data["duration"].toString().toInt();
                                tmp_dataTrack.duration = tmp_data["duration"].toString().toInt();
                            }
                            if(tmp_data.contains("name")){
                                tmp_dataTrack.title = tmp_data["name"].toString();
                            }
                            tmp_dataTrack.artist = "";
                            if(tmp_data.contains("artist")){
                                tmp_dataTrack.artist = tmp_data["artist"].toString();
                            }

                            if(tmp_data.contains("thumbnail")){
                                tmp_dataTrack.thumbnail = tmp_data["thumbnail"].toString();

                            }

                            if(i==this->indexCurrPlay[global.now_Section]){//cheon211104-01
                                print_debug();
                                //tmp_dataTrack.isPlay = true;
                            }
                        }
                    }else{
                        if(tmp_jsonObject.contains("mMusicSongInfo")){
                            //print_debug();
                            //qDebug() << "i=" <<i << ", this->indexCurrPlay[global.now_Section]=" << this->indexCurrPlay[global.now_Section];
                            QJsonObject tmp_data = tmp_jsonObject["mMusicSongInfo"].toObject();

                            // 1> 데이터 담기
                            tmp_dataTrack.type = DataTrackforDelegate::Types_Data::Music;

                            tmp_dataTrack.isDirect = false;
                            if(tmp_data.contains("title")){
                                tmp_dataTrack.title = tmp_data["title"].toString();
                            }
                            if(tmp_data.contains("artist")){
                                tmp_dataTrack.artist = tmp_data["artist"].toString();
                            }
                            if(tmp_data.contains("duration")){
                                //qDebug() << "duration = " << tmp_data["duration"].toString().toInt();
                                tmp_dataTrack.duration = tmp_data["duration"].toString().toInt()/1000;
                            }
                            int tmp_albumId = 0;
                            if(tmp_data.contains("album_id")){
                                tmp_albumId = tmp_data["album_id"].toInt();
                            }

                            if(i==this->indexCurrPlay[global.now_Section]){//cheon211104-01
                                print_debug();
                                //tmp_dataTrack.isPlay = true;
                            }
                            QString album_art = "";
                            if(tmp_data.contains("album_art")){
                                album_art = tmp_data["album_art"].toString();
                            }
                            if(album_art!=""){
                                SqliteHelper *sqlite = new SqliteHelper(this);
                                QSqlError err = sqlite->addConnectionRose();
                                if(err.type() == QSqlError::NoError){
                                    QString strQuery = "SELECT album_art FROM album_art WHERE album_id=%1 ";
                                    strQuery = strQuery.arg(tmp_albumId);
                                    QVariantList data;
                                    sqlite->exec(strQuery, data);
                                    if(data.size() > 0){
                                        QJsonObject tmp_data = data.first().toJsonObject();
                                        if(tmp_data.contains("album_art")){
                                            album_art = tmp_data["album_art"].toString();
                                        }
                                    }
                                }
                                sqlite->close();
                                delete sqlite;
                            }
                            tmp_dataTrack.thumbnail = album_art;
                        }
                    }
                    tmp_dataTrack.no = i;//cheon210916-total
                    QString tmp_imgPath = tmp_dataTrack.thumbnail;
                    if(tmp_imgPath.startsWith("http")==false){
                        tmp_imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_imgPath);
                    }

                    this->list_thumbPath_recomm[global.now_Section].append(tmp_imgPath);
                    this->list_dataTrackForDelegate_recomm[global.now_Section].append(tmp_dataTrack);
                    //print_debug();
                    //qDebug() << "this->jsonArray_backup_music.count()=" << this->jsonArray_backup_music.count();//cheon210916-total
                    // 2> UI에 데이터 세팅
                    this->appendTrackUI_recomm(k++);//cheon210916-total
                    //print_debug();

                }//if(tmp_json1.contains("track"))
            }//if(tmpStr1 ==  "MUSIC")
        }
    }
}

/**
 * @brief QueuePlayList:: 이미지 따로 Download Start
 */

void QueuePlayList::slot_queueListImgCheckOff(){

}


QPixmap* QueuePlayList::getUIPixmapImg(QString p_url){

    //QImage *img = new QImage();
    QImage img;
    QPixmap *pixmap_icon = new QPixmap();
    if(img.load(p_url)){
        *pixmap_icon = QPixmap::fromImage(img);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_icon = pixmap_icon->scaled(5, 5, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);                //이미지 사이즈 조절
    }else{
        //QMessageBox::about(0, "Image load Error", "Image load Error[Common]...Retry.. (p_url :"  + p_url);
    }
    return pixmap_icon;
}




void QueuePlayList::view_listWidget_setItemWidget_replace(const int &p_index,const int &index_old){//1126
    print_debug();
    qDebug() << "lb_queueListImg[global.now_Section].count() = " << lb_queueListImg[global.now_Section].count();
    qDebug() << "p_index = " << p_index;
    qDebug() << "index_old = " << index_old;
    DataTrackforDelegate tmp_dataTrack;
    qDebug() << "this->list_dataTrackForDelegate[global.now_Section].count() = " << this->list_dataTrackForDelegate[global.now_Section].count();
    if(listWidget_total[global.now_Section]->count() > 0 && p_index == index_old){
        print_debug();
        return;
        //widget[global.now_Section].at(index_old)->setStyleSheet(widget[global.now_Section].at(index_old)->styleSheet().replace("#111111", "#333333"));
    }

    if(listWidget_total[global.now_Section]->count() > 0 && index_old >=0){
        //int subIndex = get_fromABSNextIndexNo_toSUBIndex(index_old);

        tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(index_old);
        qDebug() << "index_old - tmp_dataTrack.title = " << tmp_dataTrack.title;
        tmp_dataTrack.isPlay = false;
        this->list_dataTrackForDelegate[global.now_Section].replace(index_old, tmp_dataTrack);
        std_listWidget_one_line_replace(index_old);
    }
    if(listWidget_total[global.now_Section]->count() > 0 && p_index >=0){
        tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(p_index);
        qDebug() << "index_old - tmp_dataTrack.title = " << tmp_dataTrack.title;
        tmp_dataTrack.isPlay = true;
        this->list_dataTrackForDelegate[global.now_Section].replace(p_index, tmp_dataTrack);
        std_listWidget_one_line_replace(p_index);

    }

}

void QueuePlayList::view_listWidget_setItemWidget_replace_only(const int &p_index,const int &index_old){//1126
    print_debug();
    qDebug() << "lb_queueListImg[global.now_Section].count() = " << lb_queueListImg[global.now_Section].count();
    qDebug() << "p_index = " << p_index;
    qDebug() << "index_old = " << index_old;
    DataTrackforDelegate tmp_dataTrack;
    qDebug() << "this->list_dataTrackForDelegate[global.now_Section].count() = " << this->list_dataTrackForDelegate[global.now_Section].count();
    if(listWidget_total[global.now_Section]->count() > 0 && p_index == index_old){
        print_debug();
        return;
        //widget[global.now_Section].at(index_old)->setStyleSheet(widget[global.now_Section].at(index_old)->styleSheet().replace("#111111", "#333333"));
    }
    if(listWidget_total[global.now_Section]->count() > 0 && index_old >=0){
        //int subIndex = get_fromABSNextIndexNo_toSUBIndex(index_old);

        tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(index_old);
        qDebug() << "index_old - tmp_dataTrack.title = " << tmp_dataTrack.title;
        tmp_dataTrack.isPlay = false;
        this->list_dataTrackForDelegate[global.now_Section].replace(index_old, tmp_dataTrack);
        std_listWidget_one_line_replace_only(index_old);
    }
    if(listWidget_total[global.now_Section]->count() > 0 && p_index >=0){
        tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(p_index);
        qDebug() << "index_old - tmp_dataTrack.title = " << tmp_dataTrack.title;
        tmp_dataTrack.isPlay = true;
        this->list_dataTrackForDelegate[global.now_Section].replace(p_index, tmp_dataTrack);
        std_listWidget_one_line_replace_only(p_index);

    }



}

void QueuePlayList::listWidget_recommTitledrawingUI()//1126
{
    //RemoveLayout(widget[global.now_Section].at(p_index));
    //widget_top_recomm.clear();
    //listWidget_total[global.now_Section]->setStyleSheet("background-color:#333333; ");//transperant
    QListWidgetItem *item = new QListWidgetItem();
    print_debug();
    //listWidget_total[global.now_Section]->setStyleSheet("QListWidget::item {background-color:#707070; }");//transperant
    print_debug();
#if defined(Q_OS_WINDOWS)
    item->setSizeHint(QSize(90, 70));
#endif
#if defined(Q_OS_MAC)
    item->setSizeHint(QSize(90, 70));
#endif
    //print_debug();
    listWidget_total[global.now_Section]->addItem(item);
    //print_debug();

    lb_title_recomm = new QLabel();
    lb_title_recomm->setText(tr("Recommand List"));
    lb_title_recomm->setFixedHeight(50);
    lb_title_recomm->setStyleSheet("font-size:20px;font:bold;color:#FFFFFF;background-color:#5A5A5A;");

    QHBoxLayout *hl_top_recomm = new QHBoxLayout();
    hl_top_recomm->setContentsMargins(10,0,0,0);
    hl_top_recomm->setSpacing(0);
    hl_top_recomm->addWidget(lb_title_recomm);
    hl_top_recomm->addWidget(this->getUIControlOption(tr("AutoPlay")));

    QWidget *widget_top_recomm = new QWidget();

    widget_top_recomm->setObjectName("widget_top_recomm");
    //widget_top_recomm->setStyleSheet("#widget_top_recomm { background-color:#5A5A5A; border-top:1px solid #5A5A5A;border-bottom:1px solid #5A5A5A; }");
    widget_top_recomm->setStyleSheet("#widget_top_recomm { background-color:#5A5A5A; }");
    widget_top_recomm->setLayout(hl_top_recomm);
    //widget_top_recomm->setCursor(Qt::PointingHandCursor);

    listWidget_total[global.now_Section]->setItemWidget(item, widget_top_recomm);
    print_debug();
}




void QueuePlayList::view_listWidget_setItemWidget_recomm(const int &p_id){//c220620
    print_debug();
    this->changeEditMode(true, false, false);
    qDebug() << "QueuePlayList::view_listWidget_setItemWidget_recomm->count()=======================" << this->list_dataTrackForDelegate_recomm[global.now_Section].count();
    qDebug() << "p_id = " << p_id;
    qDebug() << "pre-listWidget_total[global.now_Section]->count() = " << listWidget_total[global.now_Section]->count();
    //widget[global.now_Section].clear();

    for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
        listWidget_total[global.now_Section]->takeItem(this->list_dataTrackForDelegate[global.now_Section].count()+1);
    }
    qDebug() << "next-listWidget_total[global.now_Section]->count() = " << listWidget_total[global.now_Section]->count();
    widget_recomm[global.now_Section].clear();
    lb_queueListImg_recomm[global.now_Section].clear();

    int k = 0;

    if(p_id == HTTP_CURR_QUEUE_GET_RECOMM_TOTAL  ){

        //queProgressBar->setValue(total_queue_list_count);
        print_debug();
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            // print_debug();
            //qDebug() << i;
            QListWidgetItem *item = new QListWidgetItem();
            if(i == this->list_dataTrackForDelegate_recomm[global.now_Section].count()-1){
                item->setSizeHint(QSize(90, 100));
            }else{
                item->setSizeHint(QSize(90, 70));
            }
            listWidget_total[global.now_Section]->addItem(item);


            lb_queueListImg_recomm[global.now_Section].append(new QLabel());
            lb_queueListImg_recomm[global.now_Section].at(i)->setFixedSize(50,50);
            widget_recomm[global.now_Section].append( new QWidget());
            // QListWidgetItem *item = listWidget_total[global.now_Section]->item(i);
            listWidget_total[global.now_Section]->setItemWidget(item,widget_recomm[global.now_Section].at(i));
        }
        qDebug() << "end-listWidget_total[global.now_Section]->count() = " << listWidget_total[global.now_Section]->count();

        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            this->std_listWidget_one_line_recomm(i);

        }
    }else if(p_id == HTTP_CURR_QUEUE_GET_RECOMM_APPLEMUSIC  ){
        print_debug();

        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::AppleMusic){
                QListWidgetItem *item = new QListWidgetItem();
                if(i == this->list_dataTrackForDelegate_recomm[global.now_Section].count()-1){
                    item->setSizeHint(QSize(90, 100));
                }else{
                    item->setSizeHint(QSize(90, 70));
                }
                listWidget_total[global.now_Section]->addItem(item);
                lb_queueListImg_recomm[global.now_Section].append(new QLabel());
                lb_queueListImg_recomm[global.now_Section].at(i)->setFixedSize(50,50);
                //listWidget_total[global.now_Section]->setStyleSheet("QListWidget::item {border-bottom: 1px solid #707070;background-color:#222222; }");//transperant
                widget_recomm[global.now_Section].append( new QWidget());
                listWidget_total[global.now_Section]->setItemWidget(item,widget_recomm[global.now_Section].at(k++));
            }

        }
        k = 0;
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::AppleMusic){
                this->std_listWidget_one_line_recomm(k++);
            }

        }

    }else if(p_id == HTTP_CURR_QUEUE_GET_RECOMM_TIDAL  ){
        print_debug();

        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Tidal){
                QListWidgetItem *item = new QListWidgetItem();
                if(i == this->list_dataTrackForDelegate_recomm[global.now_Section].count()-1){
                    item->setSizeHint(QSize(90, 100));
                }else{
                    item->setSizeHint(QSize(90, 70));
                }
                listWidget_total[global.now_Section]->addItem(item);
                lb_queueListImg_recomm[global.now_Section].append(new QLabel());
                lb_queueListImg_recomm[global.now_Section].at(i)->setFixedSize(50,50);
                //listWidget_total[global.now_Section]->setStyleSheet("QListWidget::item {border-bottom: 1px solid #707070;background-color:#222222; }");//transperant
                widget_recomm[global.now_Section].append( new QWidget());
                listWidget_total[global.now_Section]->setItemWidget(item,widget_recomm[global.now_Section].at(k++));
            }

        }
        k = 0;
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Tidal){
                this->std_listWidget_one_line_recomm(k++);
            }

        }

    }else if(p_id == HTTP_CURR_QUEUE_GET_RECOMM_BUGS ){
        print_debug();

        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Bugs){
                QListWidgetItem *item = new QListWidgetItem();

                if(i == this->list_dataTrackForDelegate_recomm[global.now_Section].count()-1){
                    item->setSizeHint(QSize(90, 100));
                }else{
                    item->setSizeHint(QSize(90, 70));
                }

                listWidget_total[global.now_Section]->addItem(item);
                lb_queueListImg_recomm[global.now_Section].append(new QLabel());
                lb_queueListImg_recomm[global.now_Section].at(i)->setFixedSize(50,50);

                //listWidget_total[global.now_Section]->setStyleSheet("QListWidget::item {border-bottom: 1px solid #707070;background-color:#222222; }");//transperant
                widget_recomm[global.now_Section].append( new QWidget());

                listWidget_total[global.now_Section]->setItemWidget(item,widget_recomm[global.now_Section].at(k++));

            }
        }
        print_debug();
        k = 0;
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Bugs){
                this->std_listWidget_one_line_recomm(k++);
            }

        }


    }else if(p_id == HTTP_CURR_QUEUE_GET_RECOMM_MUSIC ){
        print_debug();
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Music){
                QListWidgetItem *item = new QListWidgetItem();

                if(i == this->list_dataTrackForDelegate_recomm[global.now_Section].count()-1){
                    item->setSizeHint(QSize(90, 100));
                }else{
                    item->setSizeHint(QSize(90, 70));
                }
                listWidget_total[global.now_Section]->addItem(item);
                lb_queueListImg_recomm[global.now_Section].append(new QLabel());
                lb_queueListImg_recomm[global.now_Section].at(i)->setFixedSize(50,50);
                //listWidget_total[global.now_Section]->setStyleSheet("QListWidget::item {border-bottom: 1px solid #707070;background-color:#222222; }");//transperant
                widget_recomm[global.now_Section].append( new QWidget());
                listWidget_total[global.now_Section]->setItemWidget(item,widget_recomm[global.now_Section].at(k++));
            }
        }
        k = 0;
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Music){
                this->std_listWidget_one_line_recomm(k++);
            }

        }



    }else if(p_id == HTTP_CURR_QUEUE_GET_RECOMM_ROSETUBE  ){
        print_debug();
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::RoseTube){
                QListWidgetItem *item = new QListWidgetItem();


                if(i == this->list_dataTrackForDelegate_recomm[global.now_Section].count()-1){
                    item->setSizeHint(QSize(90, 100));
                }else{
                    item->setSizeHint(QSize(90, 70));
                }


                listWidget_total[global.now_Section]->addItem(item);
                lb_queueListImg_recomm[global.now_Section].append(new QLabel());
                lb_queueListImg_recomm[global.now_Section].at(i)->setFixedSize(50,50);
                //listWidget_total[global.now_Section]->setStyleSheet("QListWidget::item {border-bottom: 1px solid #707070;background-color:#222222; }");//transperant
                widget_recomm[global.now_Section].append( new QWidget());
                listWidget_total[global.now_Section]->setItemWidget(item,widget_recomm[global.now_Section].at(k++));
            }
        }
        k = 0;
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::RoseTube){
                this->std_listWidget_one_line_recomm(k++);
            }

        }


    }else if(p_id == HTTP_CURR_QUEUE_GET_RECOMM_QOBUZ  ){
        print_debug();
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Qobuz){
                QListWidgetItem *item = new QListWidgetItem();

                if(i == this->list_dataTrackForDelegate_recomm[global.now_Section].count()-1){
                    item->setSizeHint(QSize(90, 100));
                }else{
                    item->setSizeHint(QSize(90, 70));
                }
                listWidget_total[global.now_Section]->addItem(item);
                lb_queueListImg_recomm[global.now_Section].append(new QLabel());
                lb_queueListImg_recomm[global.now_Section].at(i)->setFixedSize(50,50);
                //listWidget_total[global.now_Section]->setStyleSheet("QListWidget::item {border-bottom: 1px solid #707070;background-color:#222222; }");//transperant
                widget_recomm[global.now_Section].append( new QWidget());
                listWidget_total[global.now_Section]->setItemWidget(item,widget_recomm[global.now_Section].at(k++));
            }
        }
        k = 0;
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Qobuz){
                this->std_listWidget_one_line_recomm(k++);
            }

        }


    }

    print_debug();
    qDebug() << "p_id = " << p_id;
    qDebug() << "list_dataTrackForDelegate_Total.count() = " << list_dataTrackForDelegate_Total.count();
    //qDebug() << "backupPixmapArrayTotal.count() = " << backupPixmapArrayTotal.count();
    qDebug() << "this->list_dataTrackForDelegate[tabSectionlist[0]].count() = " << this->list_dataTrackForDelegate[tabSectionlist[0]].count();
    qDebug() << "only_replace_flag = " << only_replace_flag;
    //qDebug() << "this->normal_reflash_call_flag = " << this->normal_reflash_call_flag;
    qDebug() << "global.queue_playFlag = " << global.queue_playFlag;
    qDebug() << "global.queue_recommPlayFlag = " << global.queue_recommPlayFlag;
    qDebug() << "global.repeatMode = " << global.repeatMode;
    qDebug() << "this->preSelectedMenu = " << this->preSelectedMenu;
    qDebug() << "this->currSelectedMenu = " << this->currSelectedMenu;
    //this->mouse_trigger_flag = true;
    //global.queuelist_mouse_trigger_flag = true;
    int curr_songOtion = global.device.getMusicPlayType();

    global.Queue_track_count = 0;//c220826_2
    global.autoRecommCount = 0;


    global.queue_queuePlayFlag = false;//c220526
    global.queue_recommPlayFlag = false;//c220526

    pre_current_queue_cnt = current_queue_cnt;
    this->preSelectedMenu = currMenutoPid();
    //print_debug();
    //qDebug() << "lb_queueListImg_recomm[global.now_Section].count()=" << lb_queueListImg_recomm[global.now_Section].count();

    downloadThumbImageMore_recomm();

    //print_debug();
    global.isPlaying_flag = false;
    //qDebug() << "after downloadThumbImageMore_recomm---6666666666666666666666666666666666666666666666666666666666666666666";
    //qDebug() << "global.isPlaying_flag = " << global.isPlaying_flag;

    print_debug();
    qDebug() << "this->indexCurrPlay[global.now_Section] = " << this->indexCurrPlay[global.now_Section];
    qDebug() << this->mouse_trigger_flag;



}


void QueuePlayList::view_listWidget_setItemWidget_recomm_only(const int &p_id){//1126
    print_debug();
    this->changeEditMode(true, false, false);
    qDebug() << "QueuePlayList::view_listWidget_setItemWidget_recomm=1=======================" << this->list_dataTrackForDelegate_recomm[global.now_Section].count();
    qDebug() << "p_id = " << p_id;
    //widget[global.now_Section].clear();
    widget_recomm[global.now_Section].clear();
    lb_queueListImg_recomm[global.now_Section].clear();
    print_debug();
    //listWidget_total[global.now_Section]->setStyleSheet("#QListWidget::item {border-bottom: 1px solid #707070;background-color:#333333; }");//transperant

    //listWidget_total[global.now_Section]->setStyleSheet("background-color:#333333; ");//transperant
    int k = 0;
    //margin: 10px; padding: 0px; border-bottom: 1px solid #707070;
    //listWidget_total[global.now_Section]->setStyleSheet("QListWidget::item {margin: 0px;padding: 2px; border-bottom: 1px solid #707070;background-color:#333333; }");//transperant
    print_debug();
    //emit linker->signal_checkQueue(7, "");
    if(p_id == HTTP_CURR_QUEUE_GET_RECOMM_TOTAL  ){

        //queProgressBar->setValue(total_queue_list_count);
        print_debug();
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            //print_debug();
            //qDebug() << i;
            QListWidgetItem *item = new QListWidgetItem();
#if defined(Q_OS_WINDOWS)
            item->setSizeHint(QSize(90, 70));
#endif
#if defined(Q_OS_MAC)
            item->setSizeHint(QSize(90, 70));
#endif
            listWidget_total[global.now_Section]->addItem(item);
            lb_queueListImg_recomm[global.now_Section].append(new QLabel());
            lb_queueListImg_recomm[global.now_Section].at(i)->setFixedSize(50,50);
            widget_recomm[global.now_Section].append( new QWidget());
            listWidget_total[global.now_Section]->setItemWidget(item,widget_recomm[global.now_Section].at(i));
        }
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            this->std_listWidget_one_line_recomm(i);
#if defined(Q_OS_WINDOWS)
            QCoreApplication::processEvents();
#endif
#if defined(Q_OS_MAC)
#endif
        }
    }else if(p_id == HTTP_CURR_QUEUE_GET_RECOMM_APPLEMUSIC  ){
        print_debug();

        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::AppleMusic){
                QListWidgetItem *item = new QListWidgetItem();
                if(i == this->list_dataTrackForDelegate_recomm[global.now_Section].count()-1){
                    item->setSizeHint(QSize(90, 100));
                }else{
                    item->setSizeHint(QSize(90, 70));
                }
                listWidget_total[global.now_Section]->addItem(item);
                lb_queueListImg_recomm[global.now_Section].append(new QLabel());
                lb_queueListImg_recomm[global.now_Section].at(i)->setFixedSize(50,50);
                //listWidget_total[global.now_Section]->setStyleSheet("QListWidget::item {border-bottom: 1px solid #707070;background-color:#222222; }");//transperant
                widget_recomm[global.now_Section].append( new QWidget());
                listWidget_total[global.now_Section]->setItemWidget(item,widget_recomm[global.now_Section].at(k++));
            }

        }
        k = 0;
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::AppleMusic){
                this->std_listWidget_one_line_recomm(k++);
            }
#if defined(Q_OS_WINDOWS)
            QCoreApplication::processEvents();
#endif
#if defined(Q_OS_MAC)
#endif
        }

    }else if(p_id == HTTP_CURR_QUEUE_GET_RECOMM_TIDAL  ){
        print_debug();

        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Tidal){
                QListWidgetItem *item = new QListWidgetItem();
                if(i == this->list_dataTrackForDelegate_recomm[global.now_Section].count()-1){
                    item->setSizeHint(QSize(90, 100));
                }else{
                    item->setSizeHint(QSize(90, 70));
                }
                listWidget_total[global.now_Section]->addItem(item);
                lb_queueListImg_recomm[global.now_Section].append(new QLabel());
                lb_queueListImg_recomm[global.now_Section].at(i)->setFixedSize(50,50);
                //listWidget_total[global.now_Section]->setStyleSheet("QListWidget::item {border-bottom: 1px solid #707070;background-color:#222222; }");//transperant
                widget_recomm[global.now_Section].append( new QWidget());
                listWidget_total[global.now_Section]->setItemWidget(item,widget_recomm[global.now_Section].at(k++));
            }

        }
        k = 0;
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Tidal){
                this->std_listWidget_one_line_recomm(k++);
            }
#if defined(Q_OS_WINDOWS)
            QCoreApplication::processEvents();
#endif
#if defined(Q_OS_MAC)
#endif
        }

    }else if(p_id == HTTP_CURR_QUEUE_GET_RECOMM_BUGS ){
        print_debug();

        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Bugs){
                QListWidgetItem *item = new QListWidgetItem();

                if(i == this->list_dataTrackForDelegate_recomm[global.now_Section].count()-1){
                    item->setSizeHint(QSize(90, 100));
                }else{
                    item->setSizeHint(QSize(90, 70));
                }

                listWidget_total[global.now_Section]->addItem(item);
                lb_queueListImg_recomm[global.now_Section].append(new QLabel());
                lb_queueListImg_recomm[global.now_Section].at(i)->setFixedSize(50,50);

                //listWidget_total[global.now_Section]->setStyleSheet("QListWidget::item {border-bottom: 1px solid #707070;background-color:#222222; }");//transperant
                widget_recomm[global.now_Section].append( new QWidget());

                listWidget_total[global.now_Section]->setItemWidget(item,widget_recomm[global.now_Section].at(k++));

            }
        }
        print_debug();
        k = 0;
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Bugs){
                this->std_listWidget_one_line_recomm(k++);
            }
#if defined(Q_OS_WINDOWS)
            QCoreApplication::processEvents();
#endif
#if defined(Q_OS_MAC)
#endif
        }


    }else if(p_id == HTTP_CURR_QUEUE_GET_RECOMM_MUSIC ){
        print_debug();
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Music){
                QListWidgetItem *item = new QListWidgetItem();

                if(i == this->list_dataTrackForDelegate_recomm[global.now_Section].count()-1){
                    item->setSizeHint(QSize(90, 100));
                }else{
                    item->setSizeHint(QSize(90, 70));
                }
                listWidget_total[global.now_Section]->addItem(item);
                lb_queueListImg_recomm[global.now_Section].append(new QLabel());
                lb_queueListImg_recomm[global.now_Section].at(i)->setFixedSize(50,50);
                //listWidget_total[global.now_Section]->setStyleSheet("QListWidget::item {border-bottom: 1px solid #707070;background-color:#222222; }");//transperant
                widget_recomm[global.now_Section].append( new QWidget());
                listWidget_total[global.now_Section]->setItemWidget(item,widget_recomm[global.now_Section].at(k++));
            }
        }
        k = 0;
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Music){
                this->std_listWidget_one_line_recomm(k++);
            }
#if defined(Q_OS_WINDOWS)
            QCoreApplication::processEvents();
#endif
#if defined(Q_OS_MAC)
#endif
        }



    }else if(p_id == HTTP_CURR_QUEUE_GET_RECOMM_ROSETUBE  ){
        print_debug();
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::RoseTube){
                QListWidgetItem *item = new QListWidgetItem();


                if(i == this->list_dataTrackForDelegate_recomm[global.now_Section].count()-1){
                    item->setSizeHint(QSize(90, 100));
                }else{
                    item->setSizeHint(QSize(90, 70));
                }


                listWidget_total[global.now_Section]->addItem(item);
                lb_queueListImg_recomm[global.now_Section].append(new QLabel());
                lb_queueListImg_recomm[global.now_Section].at(i)->setFixedSize(50,50);
                //listWidget_total[global.now_Section]->setStyleSheet("QListWidget::item {border-bottom: 1px solid #707070;background-color:#222222; }");//transperant
                widget_recomm[global.now_Section].append( new QWidget());
                listWidget_total[global.now_Section]->setItemWidget(item,widget_recomm[global.now_Section].at(k++));
            }
        }
        k = 0;
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::RoseTube){
                this->std_listWidget_one_line_recomm(k++);
            }
#if defined(Q_OS_WINDOWS)
            QCoreApplication::processEvents();
#endif
#if defined(Q_OS_MAC)
#endif
        }


    }else if(p_id == HTTP_CURR_QUEUE_GET_RECOMM_QOBUZ  ){
        print_debug();
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Qobuz){
                QListWidgetItem *item = new QListWidgetItem();

                if(i == this->list_dataTrackForDelegate_recomm[global.now_Section].count()-1){
                    item->setSizeHint(QSize(90, 100));
                }else{
                    item->setSizeHint(QSize(90, 70));
                }
                listWidget_total[global.now_Section]->addItem(item);
                lb_queueListImg_recomm[global.now_Section].append(new QLabel());
                lb_queueListImg_recomm[global.now_Section].at(i)->setFixedSize(50,50);
                //listWidget_total[global.now_Section]->setStyleSheet("QListWidget::item {border-bottom: 1px solid #707070;background-color:#222222; }");//transperant
                widget_recomm[global.now_Section].append( new QWidget());
                listWidget_total[global.now_Section]->setItemWidget(item,widget_recomm[global.now_Section].at(k++));
            }
        }
        k = 0;
        for(int i = 0 ;  i < this->list_dataTrackForDelegate_recomm[global.now_Section].count() ; i++){
            if(list_dataTrackForDelegate_recomm[global.now_Section].at(i).type == DataTrackforDelegate::Types_Data::Qobuz){
                this->std_listWidget_one_line_recomm(k++);
            }
#if defined(Q_OS_WINDOWS)
            QCoreApplication::processEvents();
#endif
#if defined(Q_OS_MAC)
#endif
        }


    }

    print_debug();
    downloadThumbImageMore_recomm();




}



void QueuePlayList::appendTrackUI(const int &p_index){


}

#include <QFont>


void QueuePlayList::queue_setListWidget(int i){
    QListWidgetItem *item = listWidget_total[global.now_Section]->item(i);

    listWidget_total[global.now_Section]->setItemWidget(item,widget[global.now_Section].at(i));
}


void QueuePlayList::view_listWidget_setItemWidget_forMenu(int queue_list_cnt){//c220826_2
    print_debug();
    qDebug() << "start-----########################################################################################################################################################################";


    if(listWidget_total[global.now_Section]->count() != 3000){
        for(int i = 0 ;  i < queue_list_cnt ; i++){

            QListWidgetItem *item = new QListWidgetItem();
            //item->setBackground(QBrush("#333333"));
            item->setText("   "+ QString::number(i));
            //DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(i);
            //item->setText("    "+ QString::number(i)+ " " + this->list_dataTrackForDelegate[global.now_Section].at(i).title);
            //item->setTextColor(QColor::fromRgb(33,33,33));
            //item->setFont(QFont("Arial", 8));
            item->setFlags(Qt::ItemIsEnabled| Qt::ItemIsSelectable| Qt::ItemIsDragEnabled);
#if defined(Q_OS_WINDOWS)
            //item->setSizeHint(QSize(item->sizeHint().width(), 70));
            item->setSizeHint(QSize(90, 70));
#endif
#if defined(Q_OS_MAC)
            item->setSizeHint(QSize(90, 70));
#endif

            listWidget_total[global.now_Section]->addItem(item);

            //queProgressBar->setValue(ProgressCont+i);
        }
    }

    for(int i = 0 ;  i < queue_list_cnt ; i++){


        QObject::disconnect(disConSelectId[i]);

        //widget[global.now_Section].append(new QWidget());
        widget[global.now_Section].insert(i, new QWidget());

        //lb_queueListImg[global.now_Section].append(new QLabel());
        lb_queueListImg[global.now_Section].insert(i, new QLabel());

        queue_setListWidget(i);

        //checkbox_Selected[global.now_Section].append(new QCheckBox());
        checkbox_Selected[global.now_Section].insert(i, new QCheckBox());
        checkbox_Selected[global.now_Section].at(i)->setProperty("check_index", i);
        checkbox_Selected[global.now_Section].at(i)->setStyleSheet(
                    "#checkbox_Selected { background-color:transparent;spacing:0px;}"
                    "QCheckBox::indicator:unchecked { image: url(:images/playlist/list_check_off.png);}"
                    "QCheckBox::indicator:checked { image: url(:images/playlist/list_check_on.png);}"
                    );
        //checkbox_Selected->setCursor(Qt::PointingHandCursor);
        checkbox_Selected[global.now_Section].at(i)->setFixedSize(40,33);
        disConSelectId[i] = connect(checkbox_Selected[global.now_Section].at(i), SIGNAL(stateChanged(int)), this, SLOT(slot_clickedSelected(int)));


    }
    print_debug();
    qDebug() << "end-----########################################################################################################################################################################";



}

void QueuePlayList::view_listWidget_setItemWidget(const int &p_id){//c220826_2
    print_debug();
    qDebug() << "QueuePlayList::view_listWidget_setItemWidget=======this->list_dataTrackForDelegate[global.now_Section]===" << this->list_dataTrackForDelegate[global.now_Section].count();
    qDebug() << "lb_queueListImg[global.now_Section].count() = " << lb_queueListImg[global.now_Section].count();

    if(listWidget_total[global.now_Section]->count() == 3000){
        //listWidget_total[global.now_Section]->clear();

    }else{
        listWidget_total[global.now_Section]->clear();

    }
    widget[global.now_Section].clear();
    lb_queueListImg[global.now_Section].clear();
    //backupPixmapArrayTotal.clear();
    checkbox_Selected[global.now_Section].clear();


    global.queuelist_mouse_trigger_flag = false;
    global.queuelist_mouse_trigger_menu_flag = false;
    mouse_trigger_flag = false;
    mouse_trigger_menu_flag = false;

    if(p_id == HTTP_CURR_QUEUE_GET_TOTAL ){
        view_listWidget_setItemWidget_forMenu(this->list_dataTrackForDelegate[tabSectionlist[0]].count());
    }else if(p_id == HTTP_CURR_QUEUE_GET_APPLEMUSIC ){
        view_listWidget_setItemWidget_forMenu(this->list_dataTrackForDelegate[tabSectionlist[6]].count());

    }else if(p_id == HTTP_CURR_QUEUE_GET_TIDAL ){
        view_listWidget_setItemWidget_forMenu(this->list_dataTrackForDelegate[tabSectionlist[3]].count());

    }else if(p_id == HTTP_CURR_QUEUE_GET_MUSIC ){
        view_listWidget_setItemWidget_forMenu(this->list_dataTrackForDelegate[tabSectionlist[1]].count());
    }else if(p_id == HTTP_CURR_QUEUE_GET_ROSETUBE  ){
        view_listWidget_setItemWidget_forMenu(this->list_dataTrackForDelegate[tabSectionlist[2]].count());
    }else if(p_id == HTTP_CURR_QUEUE_GET_QOBUZ  ){
        view_listWidget_setItemWidget_forMenu(this->list_dataTrackForDelegate[tabSectionlist[5]].count());
    }else if(p_id == HTTP_CURR_QUEUE_GET_BUGS  ){
        view_listWidget_setItemWidget_forMenu(this->list_dataTrackForDelegate[tabSectionlist[4]].count());
    }


    qDebug() << "******************mouse_trigger_menu_flag = " << mouse_trigger_menu_flag;



    //this->indexCurrPlay[global.now_Section] = -1;
    //listWidget_total[global.now_Section]->setStyleSheet("background-color:#333333; ");//transperant



    //this->indexCurrPlay[global.now_Section] = save_indexCurrPlay[global.now_Section];
    print_debug();
    qDebug() << this->mouse_trigger_flag;


}



void QueuePlayList::slot_for_view_std_listWidget_one_lineItemUI(){
    //qDebug() << "QueuePlayList::view_listWidget_setItemWidget========================" << this->list_dataTrackForDelegate[global.now_Section].count();
    print_debug();


    for(int i = this->indexStartSetThumb; i < this->list_thumbPath[global.now_Section].count(); i++){

        this->std_listWidget_one_lineItemUI(i);
        queProgressBar->setValue(ProgressCont+i);
#if defined(Q_OS_WINDOWS)
        QCoreApplication::processEvents();
#endif
#if defined(Q_OS_MAC)

#endif

    }

    for(int i = this->indexStartSetThumb2; i < this->indexCurrPlay[global.now_Section]; i++){
        this->std_listWidget_one_lineItemUI(i);
        queProgressBar->setValue(ProgressCont+i);
#if defined(Q_OS_WINDOWS)
        QCoreApplication::processEvents();
#endif
#if defined(Q_OS_MAC)

#endif

    }


    if(current_queue_cnt == 0){
        queProgressBar->setValue(queProgressBar->maximum());
    }
    if(this->isPlayMode){
        print_debug();
        std_listWidget_one_line_replace(this->indexCurrPlay[global.now_Section]);
    }
    mouse_trigger_flag = true;
    global.queuelist_mouse_trigger_flag = true;
    //emit linker->signal_checkQueue(6, "");//c220502
}
void QueuePlayList::for_view_std_listWidget_one_lineItemUI(const int &p_id){//c220620
    print_debug();
    qDebug() <<  "this->CurrViewRow=" <<  this->CurrViewRow;
    for(int i = this->CurrViewRow; i < 10; i++){
        //  this->std_listWidget_one_lineItemUI(i);
    }

    /*
    global.queuelist_mouse_trigger_menu_flag = false;

    cal_indexStartSetThumb_forSpeedLoop();

    qDebug() << "this->indexStartSetThumb = " << this->indexStartSetThumb;
    qDebug() << "this->list_thumbPath[global.now_Section].count() = " << this->list_thumbPath[global.now_Section].count();
    qDebug() << "this->indexStartSetThumb2 = " << this->indexStartSetThumb2;
    qDebug() << "indexCurrPlay[global.now_Section] = " << indexCurrPlay[global.now_Section];

    slot_for_view_std_listWidget_one_lineItemUI();
    //emit signal_for_view_std_listWidget_one_lineItemUI();
    */

}

inline void QueuePlayList::std_listWidget_one_lineItemUI(const int &p_index){
    //print_debug();

    lb_queueListImg[global.now_Section].at(p_index)->setFixedSize(50,50);

    hl_listl3layout_lineItem = new QHBoxLayout();

    widget_listl3layout_lineItem = new QWidget();
    widget_listl3layout_lineItem->setLayout(hl_listl3layout_lineItem);

    vl_list_titlelayout_lineItem = new QVBoxLayout();

    widget_title_lineItem = new QWidget();
    widget_title_lineItem->setLayout(vl_list_titlelayout_lineItem);


    vl_list_titlelayout2_lineItem = new QVBoxLayout();
    if(this->isEditMode == true){
        vl_list_titlelayout2_lineItem->setContentsMargins(0,0,0,0);
    }else{
        vl_list_titlelayout2_lineItem->setContentsMargins(10,5,10,0);
    }

    RemoveLayout(widget[global.now_Section].at(p_index));
    QLayout* layout = widget[global.now_Section].at(p_index)->layout();

    if (layout == 0){
        // print_debug();
        this->widget[global.now_Section].at(p_index)->setLayout(vl_list_titlelayout2_lineItem);
    }

    QLabel *lb_Change_list = new QLabel();
    lb_Change_list->setPixmap(QPixmap(pixmapChange_list));


    QLabel *lb_listBox_title = new QLabel();

    lb_listBox_title->setStyleSheet("font-weight:300;color:#FFFFFF;font-size:16px;");
    //lb_listBox_title->setStyleSheet("QWidget { background-color:#222222;  }"
    //                                                            "QLabel:hover {background-color: #505050; color: white;}"
    //                                                            "QLabel:pressed{background-color: #B18658;} QLabel:checked{background-color: #B18658;border:none;} QToolTip{ color: #404040; }");

    QLabel *lb_listBox_artist = new QLabel();
    lb_listBox_artist->setStyleSheet("font-weight:300;color:#B18658;font-size:14px;");



    //lb_listBox->setFixedHeight(70);
    QLabel *lb_listBox_type = new QLabel();
    lb_listBox_type->setStyleSheet("background-color:transparent;");

    lb_listBox_type->resize(5, 5);

    QLabel *lb_list_duration = new QLabel();
    lb_list_duration->setStyleSheet("color:#999999;font-size:14px;");

    QLabel *lb_list_addtimeResult = new QLabel();
    lb_list_addtimeResult->setStyleSheet("color:#999999;font-size:14px;");

    QLabel *lb_queueListImgPlaying = new QLabel();
    lb_queueListImgPlaying->setFixedSize(40,40);
    lb_queueListImgPlaying->setStyleSheet("background-color:#070707;");

    QLabel *lb_queueListImgDefault = new QLabel();
    lb_queueListImgDefault->setFixedSize(40,40);
    lb_queueListImgDefault->setStyleSheet("background-color:#070707;");

    //hl_listl3layout_lineItem = new QHBoxLayout();
    hl_listl3layout_lineItem->setContentsMargins(0,0,0,0);
    hl_listl3layout_lineItem->setSpacing(10);
    hl_listl3layout_lineItem->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    hl_listl3layout_lineItem->addWidget(lb_listBox_type);
    hl_listl3layout_lineItem->addWidget(lb_list_duration);
    hl_listl3layout_lineItem->addWidget(lb_list_addtimeResult);

    vl_list_titlelayout_lineItem->setContentsMargins(10,0,0,0);
    vl_list_titlelayout_lineItem->setSpacing(3);
    vl_list_titlelayout_lineItem->addWidget(lb_listBox_title);
    vl_list_titlelayout_lineItem->addWidget(lb_listBox_artist);
    //if(this->isPlayMode == true){
    vl_list_titlelayout_lineItem->addWidget(widget_listl3layout_lineItem);
    //}
    //print_debug();
    DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(p_index);
    //print_debug();
    //listWidget_total[global.now_Section]->item(p_index)->setText("   "+ QString::number(p_index)+ " " + this->list_dataTrackForDelegate[global.now_Section].at(p_index).title);

    lb_listBox_title->setText(this->list_dataTrackForDelegate[global.now_Section].at(p_index).title);
    lb_listBox_artist->setText(this->list_dataTrackForDelegate[global.now_Section].at(p_index).artist);
    //lb_listBox->setFixedHeight(70);
    //qDebug() << "list_dataTrackForDelegate[global.now_Section].at(p_index).type=" << list_dataTrackForDelegate[global.now_Section].at(p_index).type;
    if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Music){
        lb_listBox_type->setPixmap(*pixmap_type_music);
    }else if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::AppleMusic){
        lb_listBox_type->setPixmap(*pixmap_type_apple);
    }else if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Qobuz){
        lb_listBox_type->setPixmap(*pixmap_type_qobuz);
    }else if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Tidal){
        lb_listBox_type->setPixmap(*pixmap_type_tidal);
    }else if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Bugs){
        lb_listBox_type->setPixmap(*pixmap_type_bugs);
    }else if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::RoseTube){
        lb_listBox_type->setPixmap(*pixmap_type_rosetube);
    }

    QString duration = "";
    int tt;

    if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Bugs
            || list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::RoseTube){//

        QString tempstr = this->list_dataTrackForDelegate[global.now_Section].at(p_index).len;
        //print_debug();
        QString tmp_split = "";
        QStringList splitToken;

        tmp_split = tempstr;
        splitToken = tmp_split.split(":");
        //qDebug() << "tempstr = " << tempstr;
        //qDebug() << "splitToken.size() = " << splitToken.size();
        if(splitToken.size() > 0){
            int sum_num = 0;
            for(int i = 0; i < splitToken.size(); i++){
                int tmp_number = splitToken.at(i).toInt();
                if(splitToken.size()==3){
                    if(i == 0){
                        tmp_number = tmp_number*3600;
                    }
                    if(i == 1){
                        tmp_number = tmp_number*60;
                    }
                }
                if(splitToken.size()==2){
                    if(i == 0){
                        tmp_number = tmp_number*60;
                    }
                }

                sum_num += tmp_number;
                //qDebug() << "tmp_number = " << tmp_number;
                if(i == splitToken.size() - 1){
                    duration += QString::number(tmp_number).rightJustified(2, '0');
                }
                else{
                    duration += QString::number(tmp_number).rightJustified(2, '0') + ":";
                }
            }
            if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::RoseTube && sum_num <= 0){//c220715
                duration = QString(tr("LIVE"));
            }else {

                //tt = this->list_dataTrackForDelegate[global.now_Section].at(p_index).len.toInt();
                if(sum_num >= 3600){
                    duration = QDateTime::fromTime_t(sum_num).toUTC().toString("hh:mm:ss");
                }else{
                    duration = QDateTime::fromTime_t(sum_num).toUTC().toString("mm:ss");
                }

            }
            //this->label_duration->setText(duration);
        }

    }

    else{

        //print_debug();
        //qDebug() << "list_dataTrackForDelegate[global.now_Section].at(p_index).duration = "<< list_dataTrackForDelegate[global.now_Section].at(p_index).duration;
        //qDebug() << "list_dataTrackForDelegate[global.now_Section].at(p_index).title = "<< list_dataTrackForDelegate[global.now_Section].at(p_index).title;
        if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Music){
            tt = this->list_dataTrackForDelegate[global.now_Section].at(p_index).duration/1000;
            if(tt >= 3600){
                duration = QDateTime::fromTime_t(tt).toUTC().toString("hh:mm:ss");
            }else{
                duration = QDateTime::fromTime_t(tt).toUTC().toString("mm:ss");
            }

            //duration = QString("%1:%2").arg(temp).arg(temp2);
        }else{
            tt = this->list_dataTrackForDelegate[global.now_Section].at(p_index).duration;
            if(tt >= 3600){
                duration = QDateTime::fromTime_t(tt).toUTC().toString("hh:mm:ss");
            }else{
                duration = QDateTime::fromTime_t(tt).toUTC().toString("mm:ss");
            }
        }

    }

    QString addtimeResult = "";
    quint64 tt2 = this->list_dataTrackForDelegate[global.now_Section].at(p_index).addtimeResult;
    print_debug();
    qDebug() << "tt2=" << tt2;
    if(quint64(tt2/86400) > 0){
        addtimeResult = QString(tr("Added %1 days ago")).arg(tt2/86400);
        print_debug();
    }else if(quint64(tt2/3600) > 0){
        addtimeResult = QString(tr("Added %1 hours ago")).arg(tt2/3600);
        print_debug();
    }else if(quint64(tt2/60) > 0){
        addtimeResult = QString(tr("Added %1 minutes ago")).arg(tt2/60);
        print_debug();
    }else if(quint64(tt2%60) >= 0){
        addtimeResult = QString(tr("Added %1 seconds ago")).arg(tt2%60);
        print_debug();
    }
    if(tt2==0){
        addtimeResult = QString(tr("Added %1 seconds ago")).arg(0);
    }



    lb_list_duration->setText(QString("%1").arg(duration));

    lb_list_addtimeResult->setText(QString("%1").arg(addtimeResult));

    QLabel *lb_list_audioQuality = new QLabel();
    //lb_list_audioQuality->setFixedSize(70,40);
    lb_list_audioQuality->setStyleSheet("color:#777777;font-size:16px;");

    if(list_dataTrackForDelegate[global.now_Section].at(p_index).audioQuality == "HI_RES" ){
        lb_list_audioQuality->setPixmap(*pixmap_type_audioQuality);
    }
    else if(list_dataTrackForDelegate[global.now_Section].at(p_index).audioQuality == "MQA" ){
        lb_list_audioQuality->setPixmap(*pixmap_type_mqa);
    }
    else if(!list_dataTrackForDelegate[global.now_Section].at(p_index).audioQuality.isEmpty() ){
        QString resolution = list_dataTrackForDelegate[global.now_Section].at(p_index).audioQuality;
        QLabel *tmplabel = new QLabel();
        tmplabel->setStyleSheet("background-color:transparent;color:#888888;font-size:16px;font-weight:normal;");
        tmplabel->setText(resolution);

        int width = tmplabel->sizeHint().width() + 20;

        lb_list_audioQuality->setFixedSize(width, 30);
        lb_list_audioQuality->setStyleSheet("background-color:transparent; color:#888888; font-size:14px; font-weight:normal; border:1px solid #888888; border-radius:15px;");
        lb_list_audioQuality->setAlignment(Qt::AlignCenter);
        lb_list_audioQuality->setText(resolution);
    }
    /*else if(list_dataTrackForDelegate[global.now_Section].at(p_index).audioQuality == "FLAC" ){
        lb_list_audioQuality->setFixedSize(55,30);
        lb_list_audioQuality->setText("FLAC");
        lb_list_audioQuality->setStyleSheet("font-size:16px; color:#888888;  background-color:transparent;  border:1px solid #888888;border-radius:16px; ");
        //lb_list_audioQuality->setText("FLAC");
        //lb_list_audioQuality->setPixmap(QPixmap(pixmapFlac));
        //lb_list_audioQuality->setStyleSheet("background-color:#FFFFFF;color:white;font-size:13px;border-color:#FFFFFF;border-width: 1px, border-style: solid;");

        //lb_list_audioQuality->setPixmap(*pixmap_type_flac);
    }else if(list_dataTrackForDelegate[global.now_Section].at(p_index).audioQuality == "sd,hd,fullhd" ){
        lb_list_audioQuality->setFixedSize(55,30);
        lb_list_audioQuality->setText("fullhd");
        lb_list_audioQuality->setStyleSheet("font-size:16px; color:#888888;  background-color:transparent;  border:1px solid #888888;border-radius:16px; ");
    }//sd,hd,fullhd*/


    hl_listBoxlayout_lineItem = new QHBoxLayout();
    hl_listBoxlayout_lineItem->setContentsMargins(0,0,10,0);
    //print_debug()
    // this->lb_queueListImg[global.now_Section].at(p_index)->setPixmap(QPixmap(pixmapDefault));

    //print_debug();

    if(this->isEditMode == true){
        hl_listBoxlayout_lineItem->addWidget(checkbox_Selected[global.now_Section].at(p_index));//c0114
    }
    hl_listBoxlayout_lineItem->setSpacing(10);
    hl_listBoxlayout_lineItem->addWidget(lb_queueListImg[global.now_Section].at(p_index));
    hl_listBoxlayout_lineItem->addWidget(widget_title_lineItem);
    if(this->isPlayMode == true){
        hl_listBoxlayout_lineItem->addWidget(lb_list_audioQuality, 0, Qt::AlignVCenter|Qt::AlignRight);
    }
    if(this->isMoveMode == true){
        hl_listBoxlayout_lineItem->addWidget(lb_Change_list, 0, Qt::AlignVCenter|Qt::AlignRight);
    }

    QWidget *widget_title333 = new QWidget();
    //widget_title333->setFixedHeight(60);
    widget_title333->setLayout(hl_listBoxlayout_lineItem);


    QWidget *line_bottom_1 = new QWidget();
    line_bottom_1->setFixedHeight(1);
    line_bottom_1->setStyleSheet("background-color:#707070;");

    vl_list_titlelayout2_lineItem->addWidget(line_bottom_1);
    vl_list_titlelayout2_lineItem->addWidget(widget_title333);



    this->widget[global.now_Section].at(p_index)->setStyleSheet("background-color:#222222;");

    //this->widget[global.now_Section].at(p_index)->setStyleSheet("QWidget { background-color:#222222;  }"
    //                                                            "QWidget:hover {background-color: #505050; color: white;}"
    //                                                            "QWidget:pressed{background-color: #B18658;} QWidget:checked{background-color: #B18658;border:none;} QToolTip{ color: #404040; }");

    if(this->isPlayMode == true && tmp_dataTrack.isPlay == true){
        print_debug();
        //this->indexCurrPlay[global.now_Section] =  p_index;//assignindexCurrPlay[global.now_Section]
        if(this->isPlayMode == true){

            this->widget[global.now_Section].at(p_index)->setStyleSheet("background-color:#735639;");
        }else{
            this->lb_listBox_title->setStyleSheet(lb_listBox_title->styleSheet().replace("#FFFFFF", "#B18658"));

            //this->widget[global.now_Section].at(p_index)->setStyleSheet("QWidget { background-color:#222222;  border-radius:15px;} QWidget:hover {background-color: #505050; color: white;} QWidget:pressed{background-color: #B18658;} QWidget:checked{background-color: #B18658;border:none;} QToolTip{ color: #404040; }");

            this->widget[global.now_Section].at(p_index)->setStyleSheet("background-color:#222222;");
        }
        print_debug();

    }

    //print_debug();
}


void QueuePlayList::std_listWidget_one_line_replace(const int &p_index){
    print_debug();
    qDebug() << "p_index =  " << p_index;
    std_listWidget_one_line_replace_only(p_index);
    print_debug();
    if(p_index >= 0){
        DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(p_index);
        if(tmp_dataTrack.isPlay == true){
            slot_queuelistScrollbarMotify(0);

        }
    }


}

void QueuePlayList::std_listWidget_one_line_replace_only(const int &p_index){
    print_debug();
    qDebug() << "p_index =  " << p_index;
    if(p_index < 0) return;

    if(widget[global.now_Section].count() <= p_index){
        print_debug();

    }else{
        RemoveLayout(widget[global.now_Section].at(p_index));
        print_debug();
        DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(p_index);

        QLabel *lb_listBox_title = new QLabel();
        lb_listBox_title->setText(this->list_dataTrackForDelegate[global.now_Section].at(p_index).title);
        //lb_listBox_title->setStyleSheet(lb_listBox_title->styleSheet().replace("#FFFFFF", "#B18658"));

        lb_listBox_title->setStyleSheet("font-weight:300;color:#FFFFFF;font-size:16px;");
        QLabel *lb_listBox_artist = new QLabel();
        lb_listBox_artist->setText(this->list_dataTrackForDelegate[global.now_Section].at(p_index).artist);


        lb_listBox_artist->setStyleSheet("font-weight:300;color:#B18658;font-size:14px;");

        QLabel *lb_listBox_type = new QLabel();
        lb_listBox_type->setStyleSheet("background-color:transparent;");
        lb_listBox_type->resize(5, 5);
        if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Music){
            lb_listBox_type->setPixmap(*pixmap_type_music);
            //lb_listBox_type->setText(tr("Music"));
        }else if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Qobuz){
            lb_listBox_type->setPixmap(*pixmap_type_qobuz);
            //lb_listBox_type->setText(tr("Qobuz"));
        }else if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Tidal){
            lb_listBox_type->setPixmap(*pixmap_type_tidal);
            //lb_listBox_type->setText(tr("Tidal"));
        }else if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Bugs){
            lb_listBox_type->setPixmap(*pixmap_type_bugs);
            //lb_listBox_type->setText(tr("Bugs"));
        }else if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::RoseTube){
            lb_listBox_type->setPixmap(*pixmap_type_rosetube);
            //lb_listBox_type->setText(tr("RoseTube"));
        }else if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::AppleMusic){
            lb_listBox_type->setPixmap(*pixmap_type_apple);
        }//c220603

        lb_list_duration = new QLabel();
        lb_list_duration->setStyleSheet("color:#777777;font-size:13px;");

        lb_list_addtimeResult = new QLabel();
        lb_list_addtimeResult->setStyleSheet("color:#777777;font-size:13px;");

        QString duration = "";
        int tt;
        if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::RoseTube||list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Bugs){//

            QString tempstr = this->list_dataTrackForDelegate[global.now_Section].at(p_index).len;
            //print_debug();
            //QString duration = "";
            QString tmp_split = "";
            QStringList splitToken;

            tmp_split = tempstr;
            splitToken = tmp_split.split(":");

            //qDebug() << "splitToken.size() = " << splitToken.size();
            if(splitToken.size() > 0){
                int sum_num = 0;
                for(int i = 0; i < splitToken.size(); i++){
                    int tmp_number = splitToken.at(i).toInt();
                    if(splitToken.size()==3){
                        if(i == 0){
                            tmp_number = tmp_number*3600;
                        }
                        if(i == 1){
                            tmp_number = tmp_number*60;
                        }
                    }
                    if(splitToken.size()==2){
                        if(i == 0){
                            tmp_number = tmp_number*60;
                        }
                    }

                    sum_num += tmp_number;
                    //qDebug() << "tmp_number = " << tmp_number;
                    if(i == splitToken.size() - 1){
                        duration += QString::number(tmp_number).rightJustified(2, '0');
                    }
                    else{
                        duration += QString::number(tmp_number).rightJustified(2, '0') + ":";
                    }
                }
                if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::RoseTube && sum_num <= 0){
                    duration = QString(tr("LIVE"));
                }else {

                    //tt = this->list_dataTrackForDelegate[global.now_Section].at(p_index).len.toInt();
                    if(sum_num >= 3600){
                        duration = QDateTime::fromTime_t(sum_num).toUTC().toString("hh:mm:ss");
                    }else{
                        duration = QDateTime::fromTime_t(sum_num).toUTC().toString("mm:ss");
                    }

                }
                //this->label_duration->setText(duration);
            }

            //qDebug() << "p_index-bugs = " << p_index;
            //qDebug() << "duration-bugs = "<< duration;
            // qDebug() << "list_dataTrackForDelegate[global.now_Section].at(p_index).len = "<< list_dataTrackForDelegate[global.now_Section].at(p_index).len;
            // qDebug() << "list_dataTrackForDelegate[global.now_Section].at(p_index).title = "<< list_dataTrackForDelegate[global.now_Section].at(p_index).title;
        }

        else{

            //print_debug();
            // qDebug() << "list_dataTrackForDelegate[global.now_Section].at(p_index).duration = "<< list_dataTrackForDelegate[global.now_Section].at(p_index).duration;
            // qDebug() << "list_dataTrackForDelegate[global.now_Section].at(p_index).title = "<< list_dataTrackForDelegate[global.now_Section].at(p_index).title;

            if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Music){
                tt = this->list_dataTrackForDelegate[global.now_Section].at(p_index).duration/1000;
                if(tt >= 3600){
                    duration = QDateTime::fromTime_t(tt).toUTC().toString("hh:mm:ss");
                }else{
                    duration = QDateTime::fromTime_t(tt).toUTC().toString("mm:ss");
                }

                //duration = QString("%1:%2").arg(temp).arg(temp2);
            }else{
                tt = this->list_dataTrackForDelegate[global.now_Section].at(p_index).duration;
                if(tt >= 3600){
                    duration = QDateTime::fromTime_t(tt).toUTC().toString("hh:mm:ss");
                }else{
                    duration = QDateTime::fromTime_t(tt).toUTC().toString("mm:ss");
                }
            }
        }

        QString addtimeResult = "";
        quint64 tt2 = this->list_dataTrackForDelegate[global.now_Section].at(p_index).addtimeResult;
        if(quint64(tt2/86400) > 0){
            addtimeResult = QString(tr("Added %1 days ago")).arg(tt2/86400);
        }else if(quint64(tt2/3600) > 0){
            addtimeResult = QString(tr("Added %1 hours ago")).arg(tt2/3600);
        }else if(quint64(tt2/60) > 0){
            addtimeResult = QString(tr("Added %1 minutes ago")).arg(tt2/60);
        }else if(quint64(tt2%60) >= 0){
            addtimeResult = QString(tr("Added %1 seconds ago")).arg(tt2%60);
        }
        if(tt2==0){
            addtimeResult = QString(tr("Added %1 seconds ago")).arg(0);
        }
        lb_list_duration->setText(QString("%1").arg(duration));

        lb_list_addtimeResult->setText(QString("%1").arg(addtimeResult));
        qDebug() << "p_index = " << p_index;

        QLabel *lb_list_audioQuality = new QLabel();
        //lb_list_audioQuality->setFixedSize(70,40);
        lb_list_audioQuality->setStyleSheet("color:#777777;font-size:16px;");

        if(list_dataTrackForDelegate[global.now_Section].at(p_index).audioQuality == "HI_RES" ){
            lb_list_audioQuality->setPixmap(*pixmap_type_audioQuality);
        }
        else if(list_dataTrackForDelegate[global.now_Section].at(p_index).audioQuality == "MQA" ){
            lb_list_audioQuality->setPixmap(*pixmap_type_mqa);
        }
        else if(!list_dataTrackForDelegate[global.now_Section].at(p_index).audioQuality.isEmpty() ){
            QString resolution = list_dataTrackForDelegate[global.now_Section].at(p_index).audioQuality;
            QLabel *tmplabel = new QLabel();
            tmplabel->setStyleSheet("background-color:transparent;color:#888888;font-size:16px;font-weight:normal;");
            tmplabel->setText(resolution);

            int width = tmplabel->sizeHint().width() + 20;

            lb_list_audioQuality->setFixedSize(width, 30);
            lb_list_audioQuality->setStyleSheet("background-color:transparent; color:#888888; font-size:14px; font-weight:normal; border:1px solid #888888; border-radius:15px;");
            lb_list_audioQuality->setAlignment(Qt::AlignCenter);
            lb_list_audioQuality->setText(resolution);
        }
        /*else if(list_dataTrackForDelegate[global.now_Section].at(p_index).audioQuality == "MQA" ){
            lb_list_audioQuality->setPixmap(*pixmap_type_mqa);
        }
        else if(list_dataTrackForDelegate[global.now_Section].at(p_index).audioQuality == "sd,hd,fullhd" ){
            lb_list_audioQuality->setFixedSize(55,30);
            lb_list_audioQuality->setText("fullhd");
            lb_list_audioQuality->setStyleSheet("font-size:16px; color:#888888;  background-color:transparent;  border:1px solid #888888;border-radius:16px; ");
        }//sd,hd,fullhd*/

        //lb_list_audioQuality->setStyleSheet("color:#777777;font-size:16px;");
        QHBoxLayout *hl_listl3layout = new QHBoxLayout();
        hl_listl3layout->setContentsMargins(0,0,0,0);
        hl_listl3layout->setSpacing(10);
        hl_listl3layout->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
        hl_listl3layout->addWidget(lb_listBox_type);
        hl_listl3layout->addWidget(lb_list_duration);
        hl_listl3layout->addWidget(lb_list_addtimeResult);



        QLabel *lb_queueListImgPlaying = new QLabel();
        lb_queueListImgPlaying->setFixedSize(50,50);
        //pixmapPlaying = pixmapPlaying.scaled(50,50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        lb_queueListImgPlaying->setPixmap(QPixmap(pixmapPlaying));//pixmapDefault
        lb_queueListImgPlaying->setStyleSheet("background-color:#070707;");

        QLabel *lb_queueListImgDefault = new QLabel();
        lb_queueListImgDefault->setFixedSize(50,50);
        //pixmapDefault = pixmapDefault.scaled(50,50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        lb_queueListImgDefault->setPixmap(QPixmap(pixmapDefault));//pixmapDefault
        lb_queueListImgDefault->setStyleSheet("background-color:#070707;");

        if(tmp_dataTrack.isPlay == false){

            QListWidgetItem *item = this->listWidget_total[global.now_Section]->item(p_index);
            item->setSizeHint(QSize(90, 70));

            QVBoxLayout *vl_list_titlelayout = new QVBoxLayout();
            vl_list_titlelayout->setContentsMargins(10,5,0,0);
            vl_list_titlelayout->setSpacing(3);
            vl_list_titlelayout->addWidget(lb_listBox_title);
            vl_list_titlelayout->addWidget(lb_listBox_artist);
            vl_list_titlelayout->addLayout(hl_listl3layout);
            vl_list_titlelayout->setSpacing(3);
            QWidget *widget_title = new QWidget();
            widget_title->setFixedHeight(60);
            widget_title->setLayout(vl_list_titlelayout);

            QHBoxLayout *hl_listBoxlayout = new QHBoxLayout();
            hl_listBoxlayout->setContentsMargins(0,0,0,0);
            hl_listBoxlayout->setSpacing(10);
            hl_listBoxlayout->addWidget(lb_queueListImg[global.now_Section].at(p_index));
            //hl_listBoxlayout->addWidget(lb_queueListImgDefault);

            hl_listBoxlayout->addWidget(widget_title, 0, Qt::AlignVCenter|Qt::AlignLeft);
            hl_listBoxlayout->addWidget(lb_list_audioQuality, 0, Qt::AlignVCenter|Qt::AlignRight);

            widget[global.now_Section].at(p_index)->setStyleSheet("background-color:#222222;");

            QWidget *line_bottom_1 = new QWidget();
            line_bottom_1->setFixedSize(300,1);
            line_bottom_1->setStyleSheet("background-color:#707070;");
            QWidget *line_bottom_2 = new QWidget();
            line_bottom_2->setFixedHeight(1);
            line_bottom_2->setStyleSheet("background-color:#707070;");

            //GSCommon::clearLayout(vl_list_titlelayout2);
            QVBoxLayout *vl_list_titlelayout2 = new QVBoxLayout();
            vl_list_titlelayout2->setContentsMargins(10,0,10,0);

            vl_list_titlelayout2->addWidget(line_bottom_2);
            vl_list_titlelayout2->addLayout(hl_listBoxlayout);
            print_debug();
            QWidget *widget_title333 = new QWidget();
            //widget_title333->setFixedHeight(60);
            widget_title333->setLayout(vl_list_titlelayout2);
            vl_list_titlelayout2_lineItem = new QVBoxLayout();
            //GSCommon::clearLayout(vl_list_titlelayout2);
            //vl_list_titlelayout2 = new QVBoxLayout();
            vl_list_titlelayout2_lineItem->setContentsMargins(0,0,10,0);
            vl_list_titlelayout2_lineItem->setSpacing(0);
            if(this->isPlayMode == true){
                print_debug();
                //vl_list_titlelayout2_lineItem->addLayout(hl_title_current_playing);
                // vl_list_titlelayout2_lineItem->addWidget(widget_title_current_playing);
            }

            vl_list_titlelayout2_lineItem->addWidget(widget_title333);
            //vl_list_titlelayout2_lineItem->addLayout(vl_list_titlelayout2);
            widget[global.now_Section].at(p_index)->setLayout(vl_list_titlelayout2_lineItem);
            lb_queueListImg[global.now_Section].at(p_index)->setPixmap(QPixmap(pixmapDefault));
            downloadThumbImageMore_playbarChanged(p_index);//c220620
            print_debug();

        }else if(tmp_dataTrack.isPlay == true){
            qDebug() << " tmp_dataTrack.isPlay =true, " << p_index;
            //this->indexCurrPlay[global.now_Section] =  p_index;
            //hl_listBoxlayout->addWidget(lb_queueListImgPlaying);

            QListWidgetItem *item = this->listWidget_total[global.now_Section]->item(p_index);
            item->setSizeHint(QSize(90, 125));
            /*
        this->listWidget_total[global.now_Section]->takeItem(p_index);
        QListWidgetItem *item = new QListWidgetItem;
        item->setText("   "+ QString::number(p_index));
        item->setSizeHint(QSize(90, 70));
        widget[global.now_Section].removeAt(p_index);
        widget[global.now_Section].insert(p_index, new QWidget());

        listWidget_total[global.now_Section]->insertItem(p_index, item);

        listWidget_total[global.now_Section]->setItemWidget(item,widget[global.now_Section].at(p_index));
*/
            QWidget *line_bottom_1 = new QWidget();
            line_bottom_1->setFixedSize(300,1);
            line_bottom_1->setStyleSheet("background-color:#707070;");
            QWidget *line_bottom_2 = new QWidget();
            line_bottom_2->setFixedHeight(1);
            line_bottom_2->setStyleSheet("background-color:#b0b0b0;");
            //QFrame* hFrame = new QFrame;
            //hFrame->setFrameShape(QFrame::HLine);
            QLabel *lb_listBox_title_tmp = new QLabel();
            lb_listBox_title_tmp->setFixedHeight(30);
            lb_listBox_title_tmp->setText(tr("  Currently playing"));
            lb_listBox_title_tmp->setStyleSheet("color:#FFFFFF;font-size:22px;font:bold;");

            QHBoxLayout *hl_title_current_playing = new QHBoxLayout();
            hl_title_current_playing->setContentsMargins(0,0,0,0);
            hl_title_current_playing->setSpacing(0);
            //hl_title_current_playing->setAlignment(Qt::AlignLeft);
            hl_title_current_playing->addWidget(lb_listBox_title_tmp);
            hl_title_current_playing->addWidget(line_bottom_2);

            QWidget *widget_title_current_playing = new QWidget();
            widget_title_current_playing->setLayout(hl_title_current_playing);
            widget_title_current_playing->setStyleSheet("background-color:#222222;");

            QVBoxLayout *vl_list_titlelayout = new QVBoxLayout();
            vl_list_titlelayout->setContentsMargins(10,5,0,0);
            vl_list_titlelayout->setSpacing(3);
            vl_list_titlelayout->addWidget(lb_listBox_title);
            vl_list_titlelayout->addWidget(lb_listBox_artist);
            vl_list_titlelayout->addLayout(hl_listl3layout);
            vl_list_titlelayout->setSpacing(3);
            QWidget *widget_title = new QWidget();
            widget_title->setFixedHeight(60);
            widget_title->setLayout(vl_list_titlelayout);

            QHBoxLayout *hl_listBoxlayout = new QHBoxLayout();
            hl_listBoxlayout->setContentsMargins(10,0,10,0);//setContentsMargins(0,0,10,0);
            hl_listBoxlayout->setSpacing(10);
            //qDebug() << "lb_queueListImg[global.now_Section].count() = " << lb_queueListImg[global.now_Section].count();
            //qDebug() << "p_index = " << p_index;
            //lb_queueListImg[global.now_Section].at(p_index)->clear();
            //lb_queueListImg[global.now_Section].at(p_index)->setPixmap(QPixmap(pixmapPlaying));

            hl_listBoxlayout->addWidget(lb_queueListImg[global.now_Section].at(p_index));
            //hl_listBoxlayout->setSpacing(10);
            hl_listBoxlayout->addWidget(widget_title, 0, Qt::AlignVCenter|Qt::AlignLeft);
            //hl_listBoxlayout->addSpacing(10);
            hl_listBoxlayout->addWidget(lb_list_audioQuality, 0, Qt::AlignVCenter|Qt::AlignRight);
            lb_queueListImg[global.now_Section].at(p_index)->setPixmap(QPixmap(pixmapPlaying));


            vl_list_titlelayout2_lineItem = new QVBoxLayout();
            //GSCommon::clearLayout(vl_list_titlelayout2);
            //vl_list_titlelayout2 = new QVBoxLayout();
            vl_list_titlelayout2_lineItem->setContentsMargins(0,0,10,0);
            vl_list_titlelayout2_lineItem->setSpacing(0);
            QVBoxLayout *vl_list_titlelayout2 = new QVBoxLayout();
            vl_list_titlelayout2->setContentsMargins(0,0,0,0);

            if(this->isPlayMode == true){
                print_debug();
                vl_list_titlelayout2->addWidget(widget_title_current_playing);
            }else{
                vl_list_titlelayout2->addWidget(line_bottom_2);
            }
            vl_list_titlelayout2->addLayout(hl_listBoxlayout);

            QWidget *widget_title333 = new QWidget();
            //widget_title333->setFixedHeight(60);
            widget_title333->setLayout(vl_list_titlelayout2);

            vl_list_titlelayout2_lineItem->addWidget(widget_title333);

            //vl_list_titlelayout2_lineItem->addLayout(vl_list_titlelayout2);


            if(this->isPlayMode == true){
                print_debug();
                widget[global.now_Section].at(p_index)->setStyleSheet("background-color:#735639;");
            }else{
                lb_listBox_title->setStyleSheet(lb_listBox_title->styleSheet().replace("#FFFFFF", "#B18658"));

            }
            widget[global.now_Section].at(p_index)->setLayout(vl_list_titlelayout2_lineItem);

            //slot_queuelistScrollbarMotify(0);

        }
    }


}

void QueuePlayList::std_listWidget_one_line_replace_only(const int &section, const int &p_index){
    print_debug();
    qDebug() << "p_index =  " << p_index;
    if(p_index < 0) return;
    if(widget[section].count() <= p_index){
        print_debug();

    }else{
        print_debug();
        RemoveLayout(widget[section].at(p_index));
        print_debug();
        DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate[section].at(p_index);
        print_debug();
        QLabel *lb_listBox_title = new QLabel();
        lb_listBox_title->setText(this->list_dataTrackForDelegate[section].at(p_index).title);
        //lb_listBox_title->setStyleSheet(lb_listBox_title->styleSheet().replace("#FFFFFF", "#B18658"));

        lb_listBox_title->setStyleSheet("font-weight:300;color:#FFFFFF;font-size:16px;");
        QLabel *lb_listBox_artist = new QLabel();
        lb_listBox_artist->setText(this->list_dataTrackForDelegate[section].at(p_index).artist);


        lb_listBox_artist->setStyleSheet("font-weight:300;color:#B18658;font-size:14px;");

        QLabel *lb_listBox_type = new QLabel();
        lb_listBox_type->setStyleSheet("background-color:transparent;");
        lb_listBox_type->resize(5, 5);
        if(list_dataTrackForDelegate[section].at(p_index).type == DataTrackforDelegate::Types_Data::Music){
            lb_listBox_type->setPixmap(*pixmap_type_music);
            //lb_listBox_type->setText(tr("Music"));
        }else if(list_dataTrackForDelegate[section].at(p_index).type == DataTrackforDelegate::Types_Data::Qobuz){
            lb_listBox_type->setPixmap(*pixmap_type_qobuz);
            //lb_listBox_type->setText(tr("Qobuz"));
        }else if(list_dataTrackForDelegate[section].at(p_index).type == DataTrackforDelegate::Types_Data::Tidal){
            lb_listBox_type->setPixmap(*pixmap_type_tidal);
            //lb_listBox_type->setText(tr("Tidal"));
        }else if(list_dataTrackForDelegate[section].at(p_index).type == DataTrackforDelegate::Types_Data::Bugs){
            lb_listBox_type->setPixmap(*pixmap_type_bugs);
            //lb_listBox_type->setText(tr("Bugs"));
        }else if(list_dataTrackForDelegate[section].at(p_index).type == DataTrackforDelegate::Types_Data::RoseTube){
            lb_listBox_type->setPixmap(*pixmap_type_rosetube);
            //lb_listBox_type->setText(tr("RoseTube"));
        }else if(list_dataTrackForDelegate[section].at(p_index).type == DataTrackforDelegate::Types_Data::AppleMusic){
            lb_listBox_type->setPixmap(*pixmap_type_apple);
        }//c220603

        lb_list_duration = new QLabel();
        lb_list_duration->setStyleSheet("color:#777777;font-size:13px;");

        lb_list_addtimeResult = new QLabel();
        lb_list_addtimeResult->setStyleSheet("color:#777777;font-size:13px;");

        QString duration = "";
        int tt;
        if(list_dataTrackForDelegate[section].at(p_index).type == DataTrackforDelegate::Types_Data::RoseTube||list_dataTrackForDelegate[section].at(p_index).type == DataTrackforDelegate::Types_Data::Bugs){//

            QString tempstr = this->list_dataTrackForDelegate[section].at(p_index).len;
            //print_debug();
            //QString duration = "";
            QString tmp_split = "";
            QStringList splitToken;

            tmp_split = tempstr;
            splitToken = tmp_split.split(":");

            //qDebug() << "splitToken.size() = " << splitToken.size();
            if(splitToken.size() > 0){
                int sum_num = 0;
                for(int i = 0; i < splitToken.size(); i++){
                    int tmp_number = splitToken.at(i).toInt();
                    if(splitToken.size()==3){
                        if(i == 0){
                            tmp_number = tmp_number*3600;
                        }
                        if(i == 1){
                            tmp_number = tmp_number*60;
                        }
                    }
                    if(splitToken.size()==2){
                        if(i == 0){
                            tmp_number = tmp_number*60;
                        }
                    }

                    sum_num += tmp_number;
                    //qDebug() << "tmp_number = " << tmp_number;
                    if(i == splitToken.size() - 1){
                        duration += QString::number(tmp_number).rightJustified(2, '0');
                    }
                    else{
                        duration += QString::number(tmp_number).rightJustified(2, '0') + ":";
                    }
                }
                if(list_dataTrackForDelegate[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::RoseTube && sum_num < 0){
                    duration = QString(tr("LIVE"));
                }else {

                    //tt = this->list_dataTrackForDelegate[global.now_Section].at(p_index).len.toInt();
                    if(sum_num >= 3600){
                        duration = QDateTime::fromTime_t(sum_num).toUTC().toString("hh:mm:ss");
                    }else{
                        duration = QDateTime::fromTime_t(sum_num).toUTC().toString("mm:ss");
                    }

                }
                //this->label_duration->setText(duration);
            }

            //qDebug() << "p_index-bugs = " << p_index;
            //qDebug() << "duration-bugs = "<< duration;
            // qDebug() << "list_dataTrackForDelegate[section].at(p_index).len = "<< list_dataTrackForDelegate[section].at(p_index).len;
            // qDebug() << "list_dataTrackForDelegate[section].at(p_index).title = "<< list_dataTrackForDelegate[section].at(p_index).title;
        }

        else{

            //print_debug();
            // qDebug() << "list_dataTrackForDelegate[section].at(p_index).duration = "<< list_dataTrackForDelegate[section].at(p_index).duration;
            // qDebug() << "list_dataTrackForDelegate[section].at(p_index).title = "<< list_dataTrackForDelegate[section].at(p_index).title;

            if(list_dataTrackForDelegate[section].at(p_index).type == DataTrackforDelegate::Types_Data::Music){
                tt = this->list_dataTrackForDelegate[section].at(p_index).duration/1000;
                if(tt >= 3600){
                    duration = QDateTime::fromTime_t(tt).toUTC().toString("hh:mm:ss");
                }else{
                    duration = QDateTime::fromTime_t(tt).toUTC().toString("mm:ss");
                }

                //duration = QString("%1:%2").arg(temp).arg(temp2);
            }else{
                tt = this->list_dataTrackForDelegate[section].at(p_index).duration;
                if(tt >= 3600){
                    duration = QDateTime::fromTime_t(tt).toUTC().toString("hh:mm:ss");
                }else{
                    duration = QDateTime::fromTime_t(tt).toUTC().toString("mm:ss");
                }
            }
        }

        QString addtimeResult = "";
        quint64 tt2 = this->list_dataTrackForDelegate[section].at(p_index).addtimeResult;
        if(quint64(tt2/86400) > 0){
            addtimeResult = QString(tr("Added %1 days ago")).arg(tt2/86400);
        }else if(quint64(tt2/3600) > 0){
            addtimeResult = QString(tr("Added %1 hours ago")).arg(tt2/3600);
        }else if(quint64(tt2/60) > 0){
            addtimeResult = QString(tr("Added %1 minutes ago")).arg(tt2/60);
        }else if(quint64(tt2%60) > 0){
            addtimeResult = QString(tr("Added %1 seconds ago")).arg(tt2%60);
        }
        if(tt2==0){
            addtimeResult = QString(tr("Added %1 seconds ago")).arg(0);
        }
        lb_list_duration->setText(QString("%1").arg(duration));

        lb_list_addtimeResult->setText(QString("%1").arg(addtimeResult));
        qDebug() << "p_index = " << p_index;

        QLabel *lb_list_audioQuality = new QLabel();
        //lb_list_audioQuality->setFixedSize(70,40);
        lb_list_audioQuality->setStyleSheet("color:#777777;font-size:16px;");

        if(list_dataTrackForDelegate[section].at(p_index).audioQuality == "HI_RES" ){
            lb_list_audioQuality->setPixmap(*pixmap_type_audioQuality);
        }
        else if(list_dataTrackForDelegate[section].at(p_index).audioQuality == "MQA" ){
            lb_list_audioQuality->setPixmap(*pixmap_type_mqa);
        }
        else if(!list_dataTrackForDelegate[section].at(p_index).audioQuality.isEmpty() ){
            QString resolution = list_dataTrackForDelegate[section].at(p_index).audioQuality;
            QLabel *tmplabel = new QLabel();
            tmplabel->setStyleSheet("background-color:transparent;color:#888888;font-size:16px;font-weight:normal;");
            tmplabel->setText(resolution);

            int width = tmplabel->sizeHint().width() + 20;

            lb_list_audioQuality->setFixedSize(width, 30);
            lb_list_audioQuality->setStyleSheet("background-color:transparent; color:#888888; font-size:14px; font-weight:normal; border:1px solid #888888; border-radius:15px;");
            lb_list_audioQuality->setAlignment(Qt::AlignCenter);
            lb_list_audioQuality->setText(resolution);
        }
        /*else if(list_dataTrackForDelegate[section].at(p_index).audioQuality == "MQA" ){
            lb_list_audioQuality->setPixmap(*pixmap_type_mqa);
        }
        else if(list_dataTrackForDelegate[section].at(p_index).audioQuality == "sd,hd,fullhd" ){
            lb_list_audioQuality->setFixedSize(55,30);
            lb_list_audioQuality->setText("fullhd");
            lb_list_audioQuality->setStyleSheet("font-size:16px; color:#888888;  background-color:transparent;  border:1px solid #888888;border-radius:16px; ");
        }//sd,hd,fullhd*/

        QHBoxLayout *hl_listl3layout = new QHBoxLayout();
        hl_listl3layout->setContentsMargins(0,0,0,0);
        hl_listl3layout->setSpacing(10);
        hl_listl3layout->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
        hl_listl3layout->addWidget(lb_listBox_type);
        hl_listl3layout->addWidget(lb_list_duration);
        hl_listl3layout->addWidget(lb_list_addtimeResult);



        QLabel *lb_queueListImgPlaying = new QLabel();
        lb_queueListImgPlaying->setFixedSize(50,50);
        //pixmapPlaying = pixmapPlaying.scaled(50,50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        lb_queueListImgPlaying->setPixmap(QPixmap(pixmapPlaying));//pixmapDefault
        lb_queueListImgPlaying->setStyleSheet("background-color:#070707;");

        QLabel *lb_queueListImgDefault = new QLabel();
        lb_queueListImgDefault->setFixedSize(50,50);
        //pixmapDefault = pixmapDefault.scaled(50,50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        lb_queueListImgDefault->setPixmap(QPixmap(pixmapDefault));//pixmapDefault
        lb_queueListImgDefault->setStyleSheet("background-color:#070707;");

        if(tmp_dataTrack.isPlay == false){

            QListWidgetItem *item = this->listWidget_total[section]->item(p_index);
            item->setSizeHint(QSize(90, 70));

            QVBoxLayout *vl_list_titlelayout = new QVBoxLayout();
            vl_list_titlelayout->setContentsMargins(10,5,0,0);
            vl_list_titlelayout->setSpacing(3);
            vl_list_titlelayout->addWidget(lb_listBox_title);
            vl_list_titlelayout->addWidget(lb_listBox_artist);
            vl_list_titlelayout->addLayout(hl_listl3layout);
            vl_list_titlelayout->setSpacing(3);
            QWidget *widget_title = new QWidget();
            widget_title->setFixedHeight(60);
            widget_title->setLayout(vl_list_titlelayout);

            QHBoxLayout *hl_listBoxlayout = new QHBoxLayout();
            hl_listBoxlayout->setContentsMargins(0,0,0,0);
            hl_listBoxlayout->setSpacing(10);
            hl_listBoxlayout->addWidget(lb_queueListImg[section].at(p_index));
            //hl_listBoxlayout->addWidget(lb_queueListImgDefault);

            hl_listBoxlayout->addWidget(widget_title, 0, Qt::AlignVCenter|Qt::AlignLeft);
            hl_listBoxlayout->addWidget(lb_list_audioQuality, 0, Qt::AlignVCenter|Qt::AlignRight);

            widget[section].at(p_index)->setStyleSheet("background-color:#222222;");

            QWidget *line_bottom_1 = new QWidget();
            line_bottom_1->setFixedSize(300,1);
            line_bottom_1->setStyleSheet("background-color:#707070;");
            QWidget *line_bottom_2 = new QWidget();
            line_bottom_2->setFixedHeight(1);
            line_bottom_2->setStyleSheet("background-color:#707070;");

            //GSCommon::clearLayout(vl_list_titlelayout2);
            QVBoxLayout *vl_list_titlelayout2 = new QVBoxLayout();
            vl_list_titlelayout2->setContentsMargins(10,0,10,0);

            vl_list_titlelayout2->addWidget(line_bottom_2);
            vl_list_titlelayout2->addLayout(hl_listBoxlayout);
            print_debug();
            QWidget *widget_title333 = new QWidget();
            //widget_title333->setFixedHeight(60);
            widget_title333->setLayout(vl_list_titlelayout2);
            vl_list_titlelayout2_lineItem = new QVBoxLayout();
            //vl_list_titlelayout2 = new QVBoxLayout();
            vl_list_titlelayout2_lineItem->setContentsMargins(0,0,10,0);
            vl_list_titlelayout2_lineItem->setSpacing(0);
            if(this->isPlayMode == true){
                print_debug();
                //vl_list_titlelayout2_lineItem->addLayout(hl_title_current_playing);
                // vl_list_titlelayout2_lineItem->addWidget(widget_title_current_playing);
            }

            vl_list_titlelayout2_lineItem->addWidget(widget_title333);
            //vl_list_titlelayout2_lineItem->addLayout(vl_list_titlelayout2);
            widget[section].at(p_index)->setLayout(vl_list_titlelayout2_lineItem);
            lb_queueListImg[section].at(p_index)->setPixmap(QPixmap(pixmapDefault));
            downloadThumbImageMore_playbarChanged(section, p_index);//c220620
            print_debug();

        }else if(tmp_dataTrack.isPlay == true){
            qDebug() << " tmp_dataTrack.isPlay =true, " << p_index;
            //this->indexCurrPlay[section] =  p_index;
            //hl_listBoxlayout->addWidget(lb_queueListImgPlaying);

            QListWidgetItem *item = this->listWidget_total[section]->item(p_index);
            item->setSizeHint(QSize(90, 125));

            QWidget *line_bottom_1 = new QWidget();
            line_bottom_1->setFixedSize(300,1);
            line_bottom_1->setStyleSheet("background-color:#707070;");
            QWidget *line_bottom_2 = new QWidget();
            line_bottom_2->setFixedHeight(1);
            line_bottom_2->setStyleSheet("background-color:#b0b0b0;");
            //QFrame* hFrame = new QFrame;
            //hFrame->setFrameShape(QFrame::HLine);
            QLabel *lb_listBox_title_tmp = new QLabel();
            lb_listBox_title_tmp->setFixedHeight(30);
            lb_listBox_title_tmp->setText(tr("  Currently playing"));
            lb_listBox_title_tmp->setStyleSheet("color:#FFFFFF;font-size:22px;font:bold;");

            QHBoxLayout *hl_title_current_playing = new QHBoxLayout();
            hl_title_current_playing->setContentsMargins(0,0,0,0);
            hl_title_current_playing->setSpacing(0);
            //hl_title_current_playing->setAlignment(Qt::AlignLeft);
            hl_title_current_playing->addWidget(lb_listBox_title_tmp);
            hl_title_current_playing->addWidget(line_bottom_2);

            QWidget *widget_title_current_playing = new QWidget();
            widget_title_current_playing->setLayout(hl_title_current_playing);
            widget_title_current_playing->setStyleSheet("background-color:#222222;");

            QVBoxLayout *vl_list_titlelayout = new QVBoxLayout();
            vl_list_titlelayout->setContentsMargins(10,5,0,0);
            vl_list_titlelayout->setSpacing(3);
            vl_list_titlelayout->addWidget(lb_listBox_title);
            vl_list_titlelayout->addWidget(lb_listBox_artist);
            vl_list_titlelayout->addLayout(hl_listl3layout);
            vl_list_titlelayout->setSpacing(3);
            QWidget *widget_title = new QWidget();
            widget_title->setFixedHeight(60);
            widget_title->setLayout(vl_list_titlelayout);

            QHBoxLayout *hl_listBoxlayout = new QHBoxLayout();
            hl_listBoxlayout->setContentsMargins(10,0,10,0);//setContentsMargins(0,0,10,0);
            hl_listBoxlayout->setSpacing(10);
            //qDebug() << "lb_queueListImg[section].count() = " << lb_queueListImg[section].count();
            //qDebug() << "p_index = " << p_index;
            //lb_queueListImg[section].at(p_index)->clear();


            hl_listBoxlayout->addWidget(lb_queueListImg[section].at(p_index));
            //hl_listBoxlayout->setSpacing(10);
            hl_listBoxlayout->addWidget(widget_title, 0, Qt::AlignVCenter|Qt::AlignLeft);
            //hl_listBoxlayout->addSpacing(10);
            hl_listBoxlayout->addWidget(lb_list_audioQuality, 0, Qt::AlignVCenter|Qt::AlignRight);
            //lb_queueListImg[section].at(p_index)->setPixmap(QPixmap(pixmapPlaying));

            vl_list_titlelayout2_lineItem = new QVBoxLayout();
            //GSCommon::clearLayout(vl_list_titlelayout2);
            //vl_list_titlelayout2 = new QVBoxLayout();
            vl_list_titlelayout2_lineItem->setContentsMargins(0,0,10,0);
            vl_list_titlelayout2_lineItem->setSpacing(0);
            QVBoxLayout *vl_list_titlelayout2 = new QVBoxLayout();
            vl_list_titlelayout2->setContentsMargins(0,0,0,0);

            if(this->isPlayMode == true){
                print_debug();
                vl_list_titlelayout2->addWidget(widget_title_current_playing);
            }else{
                vl_list_titlelayout2->addWidget(line_bottom_2);
            }
            vl_list_titlelayout2->addLayout(hl_listBoxlayout);

            QWidget *widget_title333 = new QWidget();
            //widget_title333->setFixedHeight(60);
            widget_title333->setLayout(vl_list_titlelayout2);

            vl_list_titlelayout2_lineItem->addWidget(widget_title333);

            //vl_list_titlelayout2_lineItem->addLayout(vl_list_titlelayout2);


            if(this->isPlayMode == true){
                print_debug();

                widget[section].at(p_index)->setStyleSheet("background-color:#735639;");
            }else{
                lb_listBox_title->setStyleSheet(lb_listBox_title->styleSheet().replace("#FFFFFF", "#B18658"));

            }
            widget[section].at(p_index)->setLayout(vl_list_titlelayout2_lineItem);

            //slot_queuelistScrollbarMotify(0);

        }
    }


}


//const int &p_index, const int &i

void QueuePlayList::std_listWidget_one_line_recomm(const int &p_index){
    //print_debug();qDebug() << p_index;
    // GSCommon::clearLayout(widget_recomm[global.now_Section].at(p_index)->layout());
    //print_debug();


    DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate_recomm[global.now_Section].at(p_index);

    QLabel *lb_listBox_title = new QLabel();
    lb_listBox_title->setText(this->list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).title);
    lb_listBox_title->setStyleSheet("font-weight:300;color:#FFFFFF;font-size:16px;");
    QLabel *lb_listBox_artist = new QLabel();
    lb_listBox_artist->setText(this->list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).artist);

    lb_listBox_artist->setStyleSheet("font-weight:300;color:rgb(177,134,88);font-size:14px;");

    //lb_listBox->setFixedHeight(70);
    //print_debug();qDebug() << p_index;
    QLabel *lb_listBox_type = new QLabel();
    lb_listBox_type->setStyleSheet("background-color:transparent;");
    lb_listBox_type->resize(5, 5);

    if(list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Music){
        lb_listBox_type->setPixmap(*pixmap_type_music);
        //lb_listBox_type->setText(tr("Music"));
    }else if(list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Qobuz){
        lb_listBox_type->setPixmap(*pixmap_type_qobuz);
        //lb_listBox_type->setText(tr("Qobuz"));
    }else if(list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Tidal){
        lb_listBox_type->setPixmap(*pixmap_type_tidal);
        //lb_listBox_type->setText(tr("Tidal"));
    }else if(list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Bugs){
        lb_listBox_type->setPixmap(*pixmap_type_bugs);
        //lb_listBox_type->setText(tr("Bugs"));
    }else if(list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::RoseTube){
        lb_listBox_type->setPixmap(*pixmap_type_rosetube);
        //lb_listBox_type->setText(tr("RoseTube"));
    }else if(list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::AppleMusic){
        lb_listBox_type->setPixmap(*pixmap_type_apple);
        //lb_listBox_type->setText(tr("RoseTube"));
    }

    QString duration = "";
    int tt;
    if(list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::Bugs
            || list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::RoseTube){
        QString tempstr = this->list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).len;
        //print_debug();
        //QString duration = "";
        //qDebug() << "tempstr = " << tempstr;

        QString tmp_split = "";
        QStringList splitToken;

        tmp_split = tempstr;
        splitToken = tmp_split.split(":");

        //qDebug() << "splitToken.size() = " << splitToken.size();
        if(splitToken.size() > 0){
            int sum_num = 0;

            for(int i = 0; i < splitToken.size(); i++){
                int tmp_number = splitToken.at(i).toInt();
                if(splitToken.size()==3){
                    if(i == 0){
                        tmp_number = tmp_number*3600;
                    }
                    if(i == 1){
                        tmp_number = tmp_number*60;
                    }
                }
                if(splitToken.size()==2){
                    if(i == 0){
                        tmp_number = tmp_number*60;
                    }
                }

                sum_num += tmp_number;
                //qDebug() << "tmp_number = " << tmp_number;
                if(i == splitToken.size() - 1){
                    duration += QString::number(tmp_number).rightJustified(2, '0');
                }
                else{
                    duration += QString::number(tmp_number).rightJustified(2, '0') + ":";
                }
            }

            if(list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).type == DataTrackforDelegate::Types_Data::RoseTube && sum_num <= 0){
                duration = QString(tr("LIVE"));
            }else {

                //tt = this->list_dataTrackForDelegate[global.now_Section].at(p_index).len.toInt();
                if(sum_num >= 3600){
                    duration = QDateTime::fromTime_t(sum_num).toUTC().toString("hh:mm:ss");
                }else{
                    duration = QDateTime::fromTime_t(sum_num).toUTC().toString("mm:ss");
                }

            }

            //this->label_duration->setText(duration);
        }

        //qDebug() << "p_index-bugs = " << p_index;
        //qDebug() << "duration-bugs = "<< duration;
        //qDebug() << "list_dataTrackForDelegate[global.now_Section].at(p_index).len = "<< list_dataTrackForDelegate[global.now_Section].at(p_index).len;
        //qDebug() << "list_dataTrackForDelegate[global.now_Section].at(p_index).title = "<< list_dataTrackForDelegate[global.now_Section].at(p_index).title;
    }

    else{

        // print_debug();//c220716
        //qDebug() << "list_dataTrackForDelegate[global.now_Section].at(p_index).duration = "<< list_dataTrackForDelegate[global.now_Section].at(p_index).duration;
        //qDebug() << "list_dataTrackForDelegate[global.now_Section].at(p_index).title = "<< list_dataTrackForDelegate[global.now_Section].at(p_index).title;

        tt = this->list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).duration;
        char temp[10] ="", temp2[10]="";
        int i10 = tt/60;
        sprintf(temp, "%02d", i10);
        i10 = tt%60;
        sprintf(temp2, "%02d", i10);
        duration = QString("%1:%2").arg(temp).arg(temp2);
        //qDebug() << temp;
    }
    //print_debug();
    QLabel *lb_list_duration = new QLabel();
    lb_list_duration->setText(QString("%1").arg(duration));
    lb_list_duration->setStyleSheet("color:#777777;font-size:13px;");


    QLabel *lb_list_audioQuality = new QLabel();
    lb_list_audioQuality->setStyleSheet("color:#777777;font-size:16px;");

    if(list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).audioQuality == "HI_RES" ){
        lb_list_audioQuality->setPixmap(*pixmap_type_audioQuality);
    }
    else if(list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).audioQuality == "MQA" ){
        lb_list_audioQuality->setPixmap(*pixmap_type_mqa);
    }
    else if(!list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).audioQuality.isEmpty() ){
        QString resolution = list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).audioQuality;
        QLabel *tmplabel = new QLabel();
        tmplabel->setStyleSheet("background-color:transparent;color:#888888;font-size:16px;font-weight:normal;");
        tmplabel->setText(resolution);

        int width = tmplabel->sizeHint().width() + 20;

        lb_list_audioQuality->setFixedSize(width, 30);
        lb_list_audioQuality->setStyleSheet("background-color:transparent; color:#888888; font-size:14px; font-weight:normal; border:1px solid #888888; border-radius:15px;");
        lb_list_audioQuality->setAlignment(Qt::AlignCenter);
        lb_list_audioQuality->setText(resolution);
    }
    /*else if(list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).audioQuality == "MQA" ){
        lb_list_audioQuality->setPixmap(*pixmap_type_mqa);
    }
    else if(list_dataTrackForDelegate_recomm[global.now_Section].at(p_index).audioQuality == "sd,hd,fullhd" ){
        lb_list_audioQuality->setFixedSize(55,30);
        lb_list_audioQuality->setText("fullhd");
        lb_list_audioQuality->setStyleSheet("font-size:16px; color:#888888;  background-color:transparent;  border:1px solid #888888;border-radius:16px; ");
    }//sd,hd,fullhd*/

    QHBoxLayout *hl_listl3layout = new QHBoxLayout();
    hl_listl3layout->setContentsMargins(0,0,0,0);
    hl_listl3layout->setSpacing(10);
    hl_listl3layout->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    hl_listl3layout->addWidget(lb_listBox_type);
    hl_listl3layout->addWidget(lb_list_duration);

    QVBoxLayout *vl_list_titlelayout = new QVBoxLayout();
    vl_list_titlelayout->setContentsMargins(10,4,0,0);
    vl_list_titlelayout->setSpacing(5);
    vl_list_titlelayout->addWidget(lb_listBox_title);
    vl_list_titlelayout->addWidget(lb_listBox_artist);
    vl_list_titlelayout->addLayout(hl_listl3layout);

    QWidget *widget_title = new QWidget();
    widget_title->setLayout(vl_list_titlelayout);
    //print_debug();
    //lb_queueListImg_recomm[global.now_Section].append(new QLabel());
    //lb_queueListImg_recomm[global.now_Section].at(p_index)->setFixedSize(40,40);

    QHBoxLayout *hl_listBoxlayout = new QHBoxLayout();
    hl_listBoxlayout->setContentsMargins(10,0,10,0);
    hl_listBoxlayout->setSpacing(10);
    QLabel *lb_queueListImgPlaying = new QLabel();
    lb_queueListImgPlaying->setFixedSize(50,50);
    pixmapPlaying = pixmapPlaying.scaled(40,40, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    lb_queueListImgPlaying->setPixmap(QPixmap(pixmapPlaying));//pixmapDefault
    lb_queueListImgPlaying->setStyleSheet("background-color:#070707;");

    QLabel *lb_queueListImgDefault = new QLabel();
    lb_queueListImgDefault->setFixedSize(50,50);
    pixmapDefault = pixmapDefault.scaled(50,50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    lb_queueListImgDefault->setPixmap(QPixmap(pixmapDefault));//pixmapDefault
    lb_queueListImgDefault->setStyleSheet("background-color:#070707;");
    //lb_queueListImg_recomm[global.now_Section].at(p_index)->setPixmap(pixmapDefault);
    /*
    if(tmp_dataTrack.isPlay == true){
        qDebug() << " tmp_dataTrack.isPlay =true, " << p_index;
        hl_listBoxlayout->addWidget(lb_queueListImgPlaying);
        widget_recomm[global.now_Section].at(p_index)->setStyleSheet("background-color:rgb(227,184,138);");//rgb(227,184,138)rgb(177,134,88)

    }else if(tmp_dataTrack.isPlay == false){
        //print_debug();
        lb_queueListImg_recomm[global.now_Section].at(p_index)->setPixmap(pixmapDefault);
        hl_listBoxlayout->addWidget(lb_queueListImg_recomm[global.now_Section].at(p_index));
        //hl_listBoxlayout->addWidget(lb_queueListImgDefault);
        widget_recomm[global.now_Section].at(p_index)->setStyleSheet("widget {background-color:#333333;color:#FFFFFF;font-size:16px;}");//rgb(227,184,138)rgb(177,134,88)

    }
    */
    lb_queueListImg_recomm[global.now_Section].at(p_index)->setPixmap(pixmapDefault);
    hl_listBoxlayout->addWidget(lb_queueListImg_recomm[global.now_Section].at(p_index));
    //hl_listBoxlayout->addWidget(lb_queueListImgDefault);

    QWidget *line_bottom_1 = new QWidget();
    line_bottom_1->setFixedHeight(1);
    line_bottom_1->setStyleSheet("background-color:#707070;");

    QWidget *line_bottom_end = new QWidget();
    line_bottom_end->setFixedHeight(5);
    line_bottom_end->setStyleSheet("background-color:#707070;");


    //widget_recomm[global.now_Section].removeAt(p_index);
    //widget_recomm[global.now_Section].insert(p_index, new QWidget());

    // widget_recomm[global.now_Section].at(p_index)->setStyleSheet("background-color:#333333;");//rgb(227,184,138)rgb(177,134,88)

    hl_listBoxlayout->addWidget(widget_title, 0, Qt::AlignVCenter|Qt::AlignLeft);
    hl_listBoxlayout->addWidget(lb_list_audioQuality, 0, Qt::AlignVCenter|Qt::AlignRight);



    QLabel *lb_listBox_end = new QLabel();
    lb_listBox_end->setText("        ------- List End ------");
    lb_listBox_end->setStyleSheet("font-weight:300;color:#FFFFFF;font-size:16px;");

    QVBoxLayout *vl_list_titlelayout2 = new QVBoxLayout();

    if(p_index < this->list_dataTrackForDelegate_recomm[global.now_Section].count()-1){
        widget_recomm[global.now_Section].at(p_index)->setFixedHeight(70);
        vl_list_titlelayout2->setContentsMargins(10,0,10,0);
        vl_list_titlelayout2->addWidget(line_bottom_1);
        vl_list_titlelayout2->addLayout(hl_listBoxlayout);
    }else{
        widget_recomm[global.now_Section].at(p_index)->setFixedHeight(100);
        vl_list_titlelayout2->setContentsMargins(10,0,10,0);
        vl_list_titlelayout2->addWidget(line_bottom_1);
        vl_list_titlelayout2->addLayout(hl_listBoxlayout);
        vl_list_titlelayout2->addWidget(line_bottom_end);
        //vl_list_titlelayout->addSpacing(20);
        vl_list_titlelayout2->addWidget(lb_listBox_end);
    }
    RemoveLayout(widget_recomm[global.now_Section].at(p_index));
    QLayout* layout = widget_recomm[global.now_Section].at(p_index)->layout();

    if (layout == 0){

        this->widget_recomm[global.now_Section].at(p_index)->setLayout(vl_list_titlelayout2);
    }
    // widget_recomm[global.now_Section].at(p_index)->setLayout(vl_list_titlelayout2);
    widget_recomm[global.now_Section].at(p_index)->setStyleSheet("background-color:#222222;");
    // print_debug();

}



void QueuePlayList::responseQueueList_checksum(const QJsonObject &p_jsonObject){  //c220609
    if(p_jsonObject.contains("checkSum")){
        global.checkSum = p_jsonObject["checkSum"].toString();
        //qDebug() << "global.checkSum=" << global.checkSum;
    }
}

void QueuePlayList::slot_responseHttp_play(const int& p_id, const QJsonObject& p_jsonObj){//cheon210916-total
    //print_debug();
    Q_UNUSED(p_id);

    // test debug 28.01.2021 Added Jeon
    //print_tidal_func();
    //QJsonDocument doc(p_jsonObj);//cheon211114-01
    //QString strJson(doc.toJson(QJsonDocument::Compact));//cheon211114-01
    //qDebug() << "slot_responseHttp_rosetube---" << strJson;

    // 나중에 실패요인에 따라서 처리 보강
    if(ProcJsonEasy::get_flagOk(p_jsonObj)){

        QString code = ProcJsonEasy::getString(p_jsonObj, "code");
        if(code == "G0000"){

        }
        else if(code == "SLEEP"){
            print_debug();
            emit linker->signal_checkQueue(24, "");

        }
    }
    else{
        //            emit this->signal_cannotRosePlay(tr("I requested the playback with Rose device, but an unknown error occurred."));
        //            emit this->signal_cannotRosePlay(tr("Rose 기기로 재생요청을 하였으나, 알수없는 오류가 발생하였습니다."));
    }

    sender()->deleteLater();

}

/**
 * @brief QueuePlayList::playCurr 해당 index를 재생하라
 * @param p_index
 */
void QueuePlayList::playCurr_recomm(int p_index){//1126
    print_debug();
    //for(int i = 0 ; i < this->jsonArray_backup_tidal_recomm.count(); i++){
    //    QJsonDocument doc(this->jsonArray_backup_tidal_recomm.at(i).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));//cheon211114-01
    //    qDebug() << "i:"<< i << "QueuePlayList::playCurr_recomm---this->jsonArray_backup_tidal_recomm.at(p_index).toObject()--" << strJson;//cheon211114-01
    //}

    if(p_index>=0 && p_index<this->list_dataTrackForDelegate_recomm[global.now_Section].count()){
        print_debug();

        if(global.device.getDeviceIP()!=""){
            this->recomCurr_selIndex = p_index;
            global.queue_recommPlayFlag = true;
            // widget_recomm[global.now_Section].at(p_index)->setStyleSheet("background-color:#9c9c9c;");


            DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate_recomm[global.now_Section].at(p_index);
            qDebug() << "tmp_dataTrack.type--" << tmp_dataTrack.type;
            // 네트워크 API 호출
            //NetworkHttp *network = new NetworkHttp;//cheon211107-01
            //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));//cheon211107-01
            QJsonObject tmp_json;
            if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::Music){//cheon210916-total

                print_debug();
                //qDebug() << "**this->jsonArray_backup_music.count()=" << this->jsonArray_backup_music.count();
                //qDebug() << "p_index=" << p_index;
                //QJsonDocument doc(this->jsonArray_backup_music.at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::playCurr---this->jsonArray_backup_music.at(p_index).toObject()--" << strJson;//cheon211114-01
                QString tmp_string = this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toString();
                //qDebug() << "QueuePlayList::playCurr_recomm---tmp_string : " << tmp_string << "\n";
                QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                //QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";

                //QJsonDocument doc3(this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject());  QString strToJson(doc3.toJson(QJsonDocument::Compact));qDebug() << "QueuePlayList::playCurr---" << strToJson;////cheon211114-01

                QJsonObject jsonObj_send, tt;

                jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
                jsonObj_send.insert("position", tmp_dataTrack.no );//cheon210916-index


                //qDebug() << "QueuePlayList::playCurr----global.checkSum = " << global.checkSum;
                //QJsonDocument doc2(jsonObj_send);  QString strJson2(doc2.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::playCurr---" << strJson2;//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr----requst_str = " << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_set");
                //QString tmp_string = jsonObj_send["data"].toString();
                //print_debug();
                //qDebug() << "tmp_string -- : " << tmp_string;



                NetworkHttp *network = new NetworkHttp;
                print_debug();
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_play(int, QJsonObject)));
                network->request(HTTP_PLAY_CURRENT_MUSIC,
                                 QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_recommand_play"),
                                 jsonObj_send,
                                 true,
                                 true);

            }else if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::RoseTube){//cheon210916-total
                print_debug();

                //print_debug();
                //qDebug() << "**this->jsonArray_backup_youtube.count()=" << this->jsonArray_backup_youtube.count();
                //qDebug() << "p_index=" << p_index;
                //QJsonDocument doc(this->jsonArray_backup_youtube.at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::playCurr---this->jsonArray_backup_youtube.at(p_index).toObject()--" << strJson;//cheon211114-01
                QString tmp_string = this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toString();
                //qDebug() << "QueuePlayList::playCurr---tmp_string : " << tmp_string << "\n";
                QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                //QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";

                //QJsonDocument doc3(this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject());  QString strToJson(doc3.toJson(QJsonDocument::Compact));qDebug() << "QueuePlayList::playCurr---" << strToJson;//cheon211114-01

                QJsonObject jsonObj_send, tt;

                jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
                jsonObj_send.insert("position", tmp_dataTrack.no);//cheon210916-index


                //qDebug() << "QueuePlayList::playCurr----global.checkSum = " << global.checkSum;
                //QJsonDocument doc2(jsonObj_send);  QString strJson2(doc2.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::playCurr---" << strJson2;//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr----requst_str = " << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_set");
                //QString tmp_string = jsonObj_send["data"].toString();
                //print_debug();
                //qDebug() << "tmp_string -- : " << tmp_string;
                NetworkHttp *network = new NetworkHttp;
                print_debug();
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_play(int, QJsonObject)));
                network->request(HTTP_PLAY_CURRENT_ROSETUBE,

                                 //QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("qobuz_set_queue.single"),
                                 QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_recommand_play"),
                                 jsonObj_send,
                                 true,
                                 true);
                /*
                tmp_json.insert("youtube", this->jsonArray_backup_youtube); // 기가멕히네.. 필수값임..
                tmp_json.insert("currentPosition", p_index);
                tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
                network->request(HTTP_PLAY_CURRENT_ROSETUBE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("youtubePlay.playlist.delete"), tmp_json, true, true);
                */
            }else if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::Tidal){//cheon210916-total
                print_debug();
                //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));//cheon211107-01
                // Tidal - Queue에서 해당 index의 Track 재생 (Tidal-Track---Play)
                //print_debug();
                //qDebug() << "this->jsonArray_backup_tidal.count()=" << this->jsonArray_backup_tidal.count();
                QJsonDocument doc(this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));//cheon211114-01
                qDebug() << "QueuePlayList::playCurr_recomm---this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject()--" << strJson;//cheon211114-01
                //QString tmp_string = this->jsonArray_backup_tidal.at(p_index).toObject()["track"].toString();
                //qDebug() << "QueuePlayList::playCurr---strJson : " << strJson << "\n";
                QJsonDocument json = QJsonDocument::fromJson(strJson.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                //QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";
                tidal::ProcRosePlay_withTidal *procRose = new tidal::ProcRosePlay_withTidal(this);
                qDebug() << "QueuePlayList::playCurr--p_index: " << p_index;
                //qDebug() << "QueuePlayList::playCurr--music_queue_list_count: " << music_queue_list_count;
                //qDebug() << "QueuePlayList::playCurr--tidal_queue_list_count: " << tidal_queue_list_count;
                qDebug() << "QueuePlayList::playCurr--tmp_dataTrack.no: " << tmp_dataTrack.no;
                procRose->request_recommPlay_current_playlist(tmp_dataTrack.no, tmp_track);//cheon210916-index
                //tidal::ProcRosePlay_withTidal *procRose = new tidal::ProcRosePlay_withTidal(this);
                //procRose->request_tidalPlay_current_playlist(p_index+music_queue_list_count, tmp_track);//cheon210916-index
                //procRose->request_tidalPlay_current_playlist(p_index, this->jsonArray_backup_tidal.at(p_index).toObject());
            }else if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::Bugs){
                print_debug();
                //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));//cheon211107-01
                // Bugs - Queue에서 해당 index의 Track 재생 (Bugs-Track---Play)
                tidal::ProcRosePlay_withTidal *procRose = new tidal::ProcRosePlay_withTidal(this);

                QJsonDocument doc(this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));
                qDebug() << "QueuePlayList::playCurr---this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject()--" << strJson;
                //QString tmp_string = this->jsonArray_backup_bugs.at(p_index).toObject()["track"].toString();
                //qDebug() << "QueuePlayList::playCurr---strJson : " << strJson << "\n";
                //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                QJsonDocument json = QJsonDocument::fromJson(strJson.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";
                //qobuz::ProcRosePlay_withQobuz *procRose = new qobuz::ProcRosePlay_withQobuz(this);
                //qDebug() << "QueuePlayList::playCurr--p_index: " << p_index;
                //qDebug() << "QueuePlayList::playCurr--music_queue_list_count: " << music_queue_list_count;
                //qDebug() << "QueuePlayList::playCurr--tidal_queue_list_count: " << tidal_queue_list_count;
                //qDebug() << "QueuePlayList::playCurr--music_queue_list_count: " << qobuz_queue_list_count;
                //qDebug() << "QueuePlayList::playCurr--tidal_queue_list_count: " << rosetube_queue_list_count;
                procRose->request_recommPlay_current_playlist(tmp_dataTrack.no, tmp_track);//cheon210916-index

                //procRose->request_setQueue_single(p_index+music_queue_list_count+tidal_queue_list_count+qobuz_queue_list_count+rosetube_queue_list_count, tmp_track);//cheon210916-index
            }else if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::Qobuz){//cheon210916-total
                print_debug();
                //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));//cheon211107-01
                // Qobuz - Queue에서 해당 index의 Track 재생 (Qobuz-Track---Play)  // Added Jeon 15/12/2020
                //print_debug();
                //qDebug() << "this->jsonArray_backup_qobuz.count()=" << this->jsonArray_backup_qobuz.count();
                QJsonDocument doc(this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));
                qDebug() << "QueuePlayList::playCurr_recomm---this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject()--" << strJson;
                //QString tmp_string = this->jsonArray_backup_qobuz.at(p_index).toObject()["track"].toString();
                //qDebug() << "QueuePlayList::playCurr---strJson : " << strJson << "\n";
                //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                QJsonDocument json = QJsonDocument::fromJson(strJson.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";
                tidal::ProcRosePlay_withTidal *procRose = new tidal::ProcRosePlay_withTidal(this);
                qDebug() << "QueuePlayList::playCurr_recomm--p_index: " << p_index;
                qDebug() << "QueuePlayList::playCurr_recomm--tmp_dataTrack.no: " << tmp_dataTrack.no;
                //qDebug() << "QueuePlayList::playCurr--p_index+ music_queue_list_count+tidal_queue_list_count: " << p_index+ music_queue_list_count+tidal_queue_list_count;
                procRose->request_recommPlay_current_playlist(tmp_dataTrack.no, tmp_track);//cheon210916-index
                //procRose->request_qobuzPlay_current_playlist(p_index+ music_queue_list_count+tidal_queue_list_count, tmp_track);//cheon210916-index
                //procRose->request_qobuzPlay_current_playlist(p_index, this->jsonArray_backup_qobuz.at(p_index).toObject());
            }else if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::AppleMusic){//cheon210916-total
                print_debug();
                //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));//cheon211107-01
                // Qobuz - Queue에서 해당 index의 Track 재생 (Qobuz-Track---Play)  // Added Jeon 15/12/2020
                //print_debug();
                //qDebug() << "this->jsonArray_backup_qobuz.count()=" << this->jsonArray_backup_qobuz.count();
                QJsonDocument doc(this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));
                qDebug() << "QueuePlayList::playCurr_recomm---this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject()--" << strJson;
                //QString tmp_string = this->jsonArray_backup_qobuz.at(p_index).toObject()["track"].toString();
                //qDebug() << "QueuePlayList::playCurr---strJson : " << strJson << "\n";
                //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                QJsonDocument json = QJsonDocument::fromJson(strJson.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";
                tidal::ProcRosePlay_withTidal *procRose = new tidal::ProcRosePlay_withTidal(this);
                qDebug() << "QueuePlayList::playCurr_recomm--p_index: " << p_index;
                qDebug() << "QueuePlayList::playCurr_recomm--tmp_dataTrack.no: " << tmp_dataTrack.no;
                //qDebug() << "QueuePlayList::playCurr--p_index+ music_queue_list_count+tidal_queue_list_count: " << p_index+ music_queue_list_count+tidal_queue_list_count;
                procRose->request_recommPlay_current_playlist(tmp_dataTrack.no, tmp_track);//cheon210916-index
                //procRose->request_qobuzPlay_current_playlist(p_index+ music_queue_list_count+tidal_queue_list_count, tmp_track);//cheon210916-index
                //procRose->request_qobuzPlay_current_playlist(p_index, this->jsonArray_backup_qobuz.at(p_index).toObject());
            }
        }
    }
}

void QueuePlayList::slot_playCurr_recomm(){//1126
    print_debug();
    //for(int i = 0 ; i < this->jsonArray_backup_tidal_recomm.count(); i++){
    //    QJsonDocument doc(this->jsonArray_backup_tidal_recomm.at(i).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));//cheon211114-01
    //    qDebug() << "i:"<< i << "QueuePlayList::playCurr_recomm---this->jsonArray_backup_tidal_recomm.at(p_index).toObject()--" << strJson;//cheon211114-01
    //}
    if(playAfterDeleteIndex < 0){
        return;
    }
    int p_index = playAfterDeleteIndex;
    playAfterDeleteIndex = -1;
    if(p_index>=0 && p_index<this->list_dataTrackForDelegate_recomm[global.now_Section].count()){
        print_debug();

        if(global.device.getDeviceIP()!=""){
            this->recomCurr_selIndex = p_index;

            //widget_recomm[global.now_Section].at(p_index)->setStyleSheet("background-color:#9c9c9c;");

            //else{
            //   ToastMsg::show(this, "", tr("Failed to play the selected track. Queue check required"),2000);
            //   return;
            //}

            DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate_recomm[global.now_Section].at(p_index);
            qDebug() << "tmp_dataTrack.type--" << tmp_dataTrack.type;
            // 네트워크 API 호출
            //NetworkHttp *network = new NetworkHttp;//cheon211107-01
            //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));//cheon211107-01
            QJsonObject tmp_json;
            if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::Music){//cheon210916-total

                print_debug();
                //qDebug() << "**this->jsonArray_backup_music.count()=" << this->jsonArray_backup_music.count();
                //qDebug() << "p_index=" << p_index;
                //QJsonDocument doc(this->jsonArray_backup_music.at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::playCurr---this->jsonArray_backup_music.at(p_index).toObject()--" << strJson;//cheon211114-01
                QString tmp_string = this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toString();
                //qDebug() << "QueuePlayList::playCurr---tmp_string : " << tmp_string << "\n";
                QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                //QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";

                //QJsonDocument doc3(this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject());  QString strToJson(doc3.toJson(QJsonDocument::Compact));qDebug() << "QueuePlayList::playCurr---" << strToJson;////cheon211114-01

                QJsonObject jsonObj_send, tt;

                jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
                jsonObj_send.insert("position", tmp_dataTrack.no );//cheon210916-index


                //qDebug() << "QueuePlayList::playCurr----global.checkSum = " << global.checkSum;
                //QJsonDocument doc2(jsonObj_send);  QString strJson2(doc2.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::playCurr---" << strJson2;//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr----requst_str = " << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_set");
                //QString tmp_string = jsonObj_send["data"].toString();
                //print_debug();
                //qDebug() << "tmp_string -- : " << tmp_string;



                NetworkHttp *network = new NetworkHttp;
                print_debug();
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_play(int, QJsonObject)));
                network->request(HTTP_PLAY_CURRENT_MUSIC,
                                 QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_recommand_play"),
                                 jsonObj_send,
                                 true,
                                 true);

            }else if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::RoseTube){//cheon210916-total
                print_debug();

                //print_debug();
                //qDebug() << "**this->jsonArray_backup_youtube.count()=" << this->jsonArray_backup_youtube.count();
                //qDebug() << "p_index=" << p_index;
                //QJsonDocument doc(this->jsonArray_backup_youtube.at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::playCurr---this->jsonArray_backup_youtube.at(p_index).toObject()--" << strJson;//cheon211114-01
                QString tmp_string = this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toString();
                //qDebug() << "QueuePlayList::playCurr---tmp_string : " << tmp_string << "\n";
                QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                //QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";

                //QJsonDocument doc3(this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject());  QString strToJson(doc3.toJson(QJsonDocument::Compact));qDebug() << "QueuePlayList::playCurr---" << strToJson;//cheon211114-01

                QJsonObject jsonObj_send, tt;

                jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
                jsonObj_send.insert("position", tmp_dataTrack.no);//cheon210916-index


                //qDebug() << "QueuePlayList::playCurr----global.checkSum = " << global.checkSum;
                //QJsonDocument doc2(jsonObj_send);  QString strJson2(doc2.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::playCurr---" << strJson2;//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr----requst_str = " << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_set");
                //QString tmp_string = jsonObj_send["data"].toString();
                //print_debug();
                //qDebug() << "tmp_string -- : " << tmp_string;
                NetworkHttp *network = new NetworkHttp;
                print_debug();
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_play(int, QJsonObject)));
                network->request(HTTP_PLAY_CURRENT_ROSETUBE,

                                 //QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("qobuz_set_queue.single"),
                                 QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_recommand_play"),
                                 jsonObj_send,
                                 true,
                                 true);
                /*
                tmp_json.insert("youtube", this->jsonArray_backup_youtube); // 기가멕히네.. 필수값임..
                tmp_json.insert("currentPosition", p_index);
                tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
                network->request(HTTP_PLAY_CURRENT_ROSETUBE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("youtubePlay.playlist.delete"), tmp_json, true, true);
                */
            }else if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::Tidal){//cheon210916-total
                print_debug();
                //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));//cheon211107-01
                // Tidal - Queue에서 해당 index의 Track 재생 (Tidal-Track---Play)
                //print_debug();
                //qDebug() << "this->jsonArray_backup_tidal.count()=" << this->jsonArray_backup_tidal.count();
                QJsonDocument doc(this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));//cheon211114-01
                qDebug() << "QueuePlayList::playCurr_recomm---this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject()--" << strJson;//cheon211114-01
                //QString tmp_string = this->jsonArray_backup_tidal.at(p_index).toObject()["track"].toString();
                //qDebug() << "QueuePlayList::playCurr---strJson : " << strJson << "\n";
                QJsonDocument json = QJsonDocument::fromJson(strJson.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                //QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";
                tidal::ProcRosePlay_withTidal *procRose = new tidal::ProcRosePlay_withTidal(this);
                qDebug() << "QueuePlayList::playCurr--p_index: " << p_index;
                //qDebug() << "QueuePlayList::playCurr--music_queue_list_count: " << music_queue_list_count;
                //qDebug() << "QueuePlayList::playCurr--tidal_queue_list_count: " << tidal_queue_list_count;
                qDebug() << "QueuePlayList::playCurr--tmp_dataTrack.no: " << tmp_dataTrack.no;
                procRose->request_recommPlay_current_playlist(tmp_dataTrack.no, tmp_track);//cheon210916-index
                //tidal::ProcRosePlay_withTidal *procRose = new tidal::ProcRosePlay_withTidal(this);
                //procRose->request_tidalPlay_current_playlist(p_index+music_queue_list_count, tmp_track);//cheon210916-index
                //procRose->request_tidalPlay_current_playlist(p_index, this->jsonArray_backup_tidal.at(p_index).toObject());
            }else if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::Bugs){
                print_debug();
                //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));//cheon211107-01
                // Bugs - Queue에서 해당 index의 Track 재생 (Bugs-Track---Play)
                tidal::ProcRosePlay_withTidal *procRose = new tidal::ProcRosePlay_withTidal(this);

                QJsonDocument doc(this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));
                qDebug() << "QueuePlayList::playCurr---this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject()--" << strJson;
                //QString tmp_string = this->jsonArray_backup_bugs.at(p_index).toObject()["track"].toString();
                //qDebug() << "QueuePlayList::playCurr---strJson : " << strJson << "\n";
                //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                QJsonDocument json = QJsonDocument::fromJson(strJson.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";
                //qobuz::ProcRosePlay_withQobuz *procRose = new qobuz::ProcRosePlay_withQobuz(this);
                //qDebug() << "QueuePlayList::playCurr--p_index: " << p_index;
                //qDebug() << "QueuePlayList::playCurr--music_queue_list_count: " << music_queue_list_count;
                //qDebug() << "QueuePlayList::playCurr--tidal_queue_list_count: " << tidal_queue_list_count;
                //qDebug() << "QueuePlayList::playCurr--music_queue_list_count: " << qobuz_queue_list_count;
                //qDebug() << "QueuePlayList::playCurr--tidal_queue_list_count: " << rosetube_queue_list_count;
                procRose->request_recommPlay_current_playlist(tmp_dataTrack.no, tmp_track);//cheon210916-index

                //procRose->request_setQueue_single(p_index+music_queue_list_count+tidal_queue_list_count+qobuz_queue_list_count+rosetube_queue_list_count, tmp_track);//cheon210916-index
            }else if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::Qobuz){//cheon210916-total
                print_debug();
                //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));//cheon211107-01
                // Qobuz - Queue에서 해당 index의 Track 재생 (Qobuz-Track---Play)  // Added Jeon 15/12/2020
                //print_debug();
                //qDebug() << "this->jsonArray_backup_qobuz.count()=" << this->jsonArray_backup_qobuz.count();
                QJsonDocument doc(this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));
                qDebug() << "QueuePlayList::playCurr_slot_playCurr_recomm---this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject()--" << strJson;
                //QString tmp_string = this->jsonArray_backup_qobuz.at(p_index).toObject()["track"].toString();
                //qDebug() << "QueuePlayList::playCurr---strJson : " << strJson << "\n";
                //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                QJsonDocument json = QJsonDocument::fromJson(strJson.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";
                tidal::ProcRosePlay_withTidal *procRose = new tidal::ProcRosePlay_withTidal(this);
                qDebug() << "QueuePlayList::playCurr--p_index: " << p_index;
                qDebug() << "QueuePlayList::playCurr--tmp_dataTrack.no: " << tmp_dataTrack.no;
                //qDebug() << "QueuePlayList::playCurr--p_index+ music_queue_list_count+tidal_queue_list_count: " << p_index+ music_queue_list_count+tidal_queue_list_count;
                procRose->request_recommPlay_current_playlist(tmp_dataTrack.no, tmp_track);//cheon210916-index
                //procRose->request_qobuzPlay_current_playlist(p_index+ music_queue_list_count+tidal_queue_list_count, tmp_track);//cheon210916-index
                //procRose->request_qobuzPlay_current_playlist(p_index, this->jsonArray_backup_qobuz.at(p_index).toObject());
            }else if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::AppleMusic){//cheon210916-total
                print_debug();
                QJsonDocument doc(this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));
                qDebug() << "QueuePlayList::playCurr_slot_playCurr_recomm---this->jsonArray_backup_total_recomm[global.now_Section].at(p_index).toObject()--" << strJson;
                QJsonDocument json = QJsonDocument::fromJson(strJson.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";
                tidal::ProcRosePlay_withTidal *procRose = new tidal::ProcRosePlay_withTidal(this);
                qDebug() << "QueuePlayList::playCurr--p_index: " << p_index;
                qDebug() << "QueuePlayList::playCurr--tmp_dataTrack.no: " << tmp_dataTrack.no;
                //qDebug() << "QueuePlayList::playCurr--p_index+ music_queue_list_count+tidal_queue_list_count: " << p_index+ music_queue_list_count+tidal_queue_list_count;
                procRose->request_recommPlay_current_playlist(tmp_dataTrack.no, tmp_track);//cheon210916-index
            }
        }
    }
}

/**
 * @brief QueuePlayList::playCurr 해당 index를 재생하라
 * @param p_index
 */
void QueuePlayList::playCurr(int p_index){//1126
    print_debug();
    //int index_old = get_fromNextIndexNo_toCurrIndexPlay();



    //print_debug();
    //qDebug() << "QueuePlayList::playCurr--index_old: " << index_old;
    qDebug() << "QueuePlayList::playCurr--p_index: " << p_index;
    qDebug() << "QueuePlayList::playCurr--this->list_dataTrackForDelegate[global.now_Section].count(): " << this->list_dataTrackForDelegate[global.now_Section].count();

    if(p_index>=0 && p_index<this->list_dataTrackForDelegate[global.now_Section].count()){
        print_debug();


        if(global.device.getDeviceIP()!=""){
            //emit signal_music_start();
            //slot_queuelistPositionGet_check();
            print_debug();
            //emit linker->signal_checkQueue(17, "");
            // 네트워크 API 호출
            //this->indexCurrPlay[global.now_Section] = p_index;
            global.queue_playFlag = true;
            QJsonObject tmp_json;
            DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(p_index);
            qDebug() << "tmp_dataTrack.no = " << tmp_dataTrack.no;
            qDebug() << "tmp_dataTrack.title = " << tmp_dataTrack.title;
            if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::Music){//cheon210916-total

                print_debug();
                //qDebug() << "**this->jsonArray_backup_music.count()=" << this->jsonArray_backup_music.count();
                qDebug() << "p_index=" << p_index;
                QJsonDocument doc(this->jsonArray_backup_total[global.now_Section].at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact)); // qDebug() << "QueuePlayList::playCurr---this->jsonArray_backup_music.at(p_index).toObject()--" << strJson;//cheon211114-01
                QString tmp_string = this->jsonArray_backup_total[global.now_Section].at(p_index).toString();
                //qDebug() << "QueuePlayList::playCurr---tmp_string : " << tmp_string << "\n";
                QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                //QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";

                QJsonDocument doc3(this->jsonArray_backup_total[global.now_Section].at(p_index).toObject());  QString strToJson(doc3.toJson(QJsonDocument::Compact));//qDebug() << "QueuePlayList::playCurr---" << strToJson;////cheon211114-01
                qDebug() << "tmp_dataTrack.no  = " << tmp_dataTrack.no ;
                QJsonObject jsonObj_send, tt;
                //jsonObj_send.insert("data",tmp_track);
                jsonObj_send.insert("data",strToJson);
                jsonObj_send.insert("currentPlayer", "MUSIC");
                jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
                jsonObj_send.insert("remotePositionPlay", tmp_dataTrack.no );//cheon210916-index
                jsonObj_send.insert("queueMode", "TOTAL");//cheon211104-01
                //qDebug() << "QueuePlayList::playCurr----p_index+tidal_queue_list_count = " << p_index;
                jsonObj_send.insert("checkSum", global.checkSum);

                //qDebug() << "QueuePlayList::playCurr----global.checkSum = " << global.checkSum;
                //QJsonDocument doc2(jsonObj_send);  QString strJson2(doc2.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::playCurr---" << strJson2;//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr----requst_str = " << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_set");
                //QString tmp_string = jsonObj_send["data"].toString();
                //print_debug();
                //qDebug() << "tmp_string -- : " << tmp_string;
                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_music(int, QJsonObject)));
                network->request(HTTP_PLAY_CURRENT_MUSIC,
                                 QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_set"),
                                 jsonObj_send,
                                 true,
                                 true);

            }else if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::RoseTube){//cheon210916-total
                print_debug();

                //print_debug();
                //qDebug() << "**this->jsonArray_backup_youtube.count()=" << this->jsonArray_backup_youtube.count();
                qDebug() << "p_index=" << p_index;
                QJsonDocument doc(this->jsonArray_backup_total[global.now_Section].at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::playCurr---this->jsonArray_backup_youtube.at(p_index).toObject()--" << strJson;//cheon211114-01
                QString tmp_string = this->jsonArray_backup_total[global.now_Section].at(p_index).toString();
                //qDebug() << "QueuePlayList::playCurr---tmp_string : " << tmp_string << "\n";
                QJsonDocument json = QJsonDocument::fromJson(tmp_string.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                //QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";

                QJsonDocument doc3(this->jsonArray_backup_total[global.now_Section].at(p_index).toObject());  QString strToJson(doc3.toJson(QJsonDocument::Compact));qDebug() << "QueuePlayList::playCurr---" << strToJson;//cheon211114-01

                QJsonObject jsonObj_send, tt;
                jsonObj_send.insert("data",strToJson);
                jsonObj_send.insert("currentPlayer", "YOUTUBE");//cheon211104-01
                jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
                jsonObj_send.insert("remotePositionPlay", tmp_dataTrack.no);//cheon210916-index
                //jsonObj_send.insert("remotePositionPlay", p_index + music_queue_list_count+tidal_queue_list_count+qobuz_queue_list_count);//cheon210916-index
                jsonObj_send.insert("queueMode", "TOTAL");//cheon211104-01
                //qDebug() << "QueuePlayList::playCurr----p_index+tidal_queue_list_count = " << p_index+ music_queue_list_count+tidal_queue_list_count+qobuz_queue_list_count;
                jsonObj_send.insert("checkSum", global.checkSum);

                //qDebug() << "QueuePlayList::playCurr----global.checkSum = " << global.checkSum;
                //QJsonDocument doc2(jsonObj_send);  QString strJson2(doc2.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::playCurr---" << strJson2;//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr----requst_str = " << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_set");
                //QString tmp_string = jsonObj_send["data"].toString();
                //print_debug();
                //qDebug() << "tmp_string -- : " << tmp_string;
                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_music(int, QJsonObject)));
                network->request(HTTP_PLAY_CURRENT_ROSETUBE,

                                 //QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("qobuz_set_queue.single"),
                                 QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_set"),
                                 jsonObj_send,
                                 true,
                                 true);
            }else if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::Tidal){//cheon210916-total
                print_debug();
                if(global.user_forTidal.isLogined() == false){
                    print_debug();
                    widget[global.now_Section].at(p_index)->setStyleSheet(widget[global.now_Section].at(p_index)->styleSheet().replace("#111111", "#333333"));
                    //emit linker->signal_checkQueue(6, "");
                    ToastMsg::show(this,"", tr("The subscription has expired. Please log in again."), 3000, 0);
                    return;
                }
                //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));//cheon211107-01
                // Tidal - Queue에서 해당 index의 Track 재생 (Tidal-Track---Play)
                //print_debug();
                //qDebug() << "this->jsonArray_backup_tidal.count()=" << this->jsonArray_backup_tidal.count();
                QJsonDocument doc(this->jsonArray_backup_total[global.now_Section].at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr---this->jsonArray_backup_tidal.at(p_index).toObject()--" << strJson;//cheon211114-01
                //QString tmp_string = this->jsonArray_backup_tidal.at(p_index).toObject()["track"].toString();
                //qDebug() << "QueuePlayList::playCurr---strJson : " << strJson << "\n";
                QJsonDocument json = QJsonDocument::fromJson(strJson.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                //QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));//cheon211114-01
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";
                tidal::ProcRosePlay_withTidal *procRose = new tidal::ProcRosePlay_withTidal(this);
                qDebug() << "QueuePlayList::playCurr--p_index: " << p_index;
                //qDebug() << "QueuePlayList::playCurr--music_queue_list_count: " << music_queue_list_count;
                //qDebug() << "QueuePlayList::playCurr--tidal_queue_list_count: " << tidal_queue_list_count;
                qDebug() << "QueuePlayList::playCurr--tmp_dataTrack.no: " << tmp_dataTrack.no;
                procRose->request_tidalPlay_current_playlist(tmp_dataTrack.no, tmp_track);//cheon210916-index

            }else if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::Bugs){
                print_debug();
                if(global.user_forBugs.isLogined() == false){
                    print_debug();
                    widget[global.now_Section].at(p_index)->setStyleSheet(widget[global.now_Section].at(p_index)->styleSheet().replace("#111111", "#333333"));
                    //emit linker->signal_checkQueue(6, "");
                    //emit linker->signal_checkQueue(16, "");
                    ToastMsg::show(this,"", tr("The subscription has expired. Please log in again."), 3000, 0);
                    return;
                }
                //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));//cheon211107-01
                // Bugs - Queue에서 해당 index의 Track 재생 (Bugs-Track---Play)
                bugs::ProcRoseAPI_withBugs *procRose = new bugs::ProcRoseAPI_withBugs(this);

                QJsonDocument doc(this->jsonArray_backup_total[global.now_Section].at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact)); // qDebug() << "QueuePlayList::playCurr---this->jsonArray_backup_bugs.at(p_index).toObject()--" << strJson;
                //QString tmp_string = this->jsonArray_backup_bugs.at(p_index).toObject()["track"].toString();
                //qDebug() << "QueuePlayList::playCurr---strJson : " << strJson << "\n";

                QJsonDocument json = QJsonDocument::fromJson(strJson.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object

                QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));
                qDebug() << "QueuePlayList::playCurr-bugs--tmp_track : " << strJson1 << "\n";
                qDebug() << "tmp_dataTrack.no= " << tmp_dataTrack.no << "\n";

                //if(global.user_forBugs.isLogined()){//c220705
                procRose->request_setQueue_single(tmp_dataTrack.no, tmp_track);//cheon210916-index
                //}else{
                //   ToastMsg::show(this,"", tr("The subscription has expired. Please log in again."), 3000, 0);

                //}

            }else if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::Qobuz){//cheon210916-total
                print_debug();
                if(global.user_forQobuz.isLogined() == false){
                    print_debug();
                    widget[global.now_Section].at(p_index)->setStyleSheet(widget[global.now_Section].at(p_index)->styleSheet().replace("#111111", "#333333"));
                    //emit linker->signal_checkQueue(6, "");
                    ToastMsg::show(this,"", tr("The subscription has expired. Please log in again."), 3000, 0);
                    return;
                }
                //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));//cheon211107-01
                // Qobuz - Queue에서 해당 index의 Track 재생 (Qobuz-Track---Play)  // Added Jeon 15/12/2020
                //print_debug();
                //qDebug() << "this->jsonArray_backup_qobuz.count()=" << this->jsonArray_backup_qobuz.count();
                QJsonDocument doc(this->jsonArray_backup_total[global.now_Section].at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));
                //qDebug() << "QueuePlayList::playCurr---this->jsonArray_backup_qobuz.at(p_index).toObject()--" << strJson;
                //QString tmp_string = this->jsonArray_backup_qobuz.at(p_index).toObject()["track"].toString();
                //qDebug() << "QueuePlayList::playCurr---strJson : " << strJson << "\n";
                //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                QJsonDocument json = QJsonDocument::fromJson(strJson.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";
                qobuz::ProcRosePlay_withQobuz *procRose = new qobuz::ProcRosePlay_withQobuz(this);
                qDebug() << "QueuePlayList::playCurr--p_index: " << p_index;
                qDebug() << "QueuePlayList::playCurr--tmp_dataTrack.no: " << tmp_dataTrack.no;
                //qDebug() << "QueuePlayList::playCurr--p_index+ music_queue_list_count+tidal_queue_list_count: " << p_index+ music_queue_list_count+tidal_queue_list_count;
                procRose->request_qobuzPlay_current_playlist(tmp_dataTrack.no, tmp_track);//cheon210916-index

            }else if(tmp_dataTrack.type == DataTrackforDelegate::Types_Data::AppleMusic){//cheon210916-total
                print_debug();
                //emit linker->signal_checkQueue(20, "");


                if(global.user_forApple.isLogined() == false){
                    print_debug();
                    widget[global.now_Section].at(p_index)->setStyleSheet(widget[global.now_Section].at(p_index)->styleSheet().replace("#111111", "#333333"));
                    //emit linker->signal_checkQueue(6, "");
                    //    emit linker->signal_checkQueue(16, "");
                    ToastMsg::show(this,"", tr("The subscription has expired. Please log in again."), 3000, 0);
                    return;
                }
                //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));//cheon211107-01
                // Qobuz - Queue에서 해당 index의 Track 재생 (Qobuz-Track---Play)  // Added Jeon 15/12/2020
                //print_debug();
                //qDebug() << "this->jsonArray_backup_qobuz.count()=" << this->jsonArray_backup_qobuz.count();
                QJsonDocument doc(this->jsonArray_backup_total[global.now_Section].at(p_index).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));
                //qDebug() << "QueuePlayList::playCurr---this->jsonArray_backup_qobuz.at(p_index).toObject()--" << strJson;
                //QString tmp_string = this->jsonArray_backup_qobuz.at(p_index).toObject()["track"].toString();
                //qDebug() << "QueuePlayList::playCurr---strJson : " << strJson << "\n";
                //QString s(R"({"name":"Adam","body":"is fat","Eyes":"blue"})"); // raw string
                QJsonDocument json = QJsonDocument::fromJson(strJson.toUtf8());
                QJsonObject tmp_track = json.object(); // since your string is an JSON object
                QJsonDocument doc1(tmp_track); QString strJson1(doc1.toJson(QJsonDocument::Compact));
                //qDebug() << "QueuePlayList::playCurr---tmp_track : " << strJson1 << "\n";
                qobuz::ProcRosePlay_withQobuz *procRose = new qobuz::ProcRosePlay_withQobuz(this);
                qDebug() << "QueuePlayList::playCurr--p_index: " << p_index;
                qDebug() << "QueuePlayList::playCurr--tmp_dataTrack.no: " << tmp_dataTrack.no;
                //qDebug() << "QueuePlayList::playCurr--p_index+ music_queue_list_count+tidal_queue_list_count: " << p_index+ music_queue_list_count+tidal_queue_list_count;
                procRose->request_applePlay_current_playlist(tmp_dataTrack.no, tmp_track);//cheon210916-index

                /*
                QString file = QFileDialog::getOpenFileName(this, "file select", "D:\\", "Files(*.mp3)");//cheon210617-file
                qDebug() << "file----- : " << file;
                QMediaPlayer *player = new QMediaPlayer;
                // ...
                player->setMedia(QUrl::fromLocalFile("https://music.apple.com/kr/album/%EA%B7%B8%EA%B2%8C-%EB%82%98%EC%98%80%EB%84%A4/1625629976?i=1625629977"));//file));
                player->setVolume(50);
                player->play();
*/


            }
        }
        //mutex.unlock();
    }
}

void QueuePlayList::slot_responseHttp_music(const int& p_id, const QJsonObject& p_jsonObj){//cheon210916-total
    //print_debug();
    Q_UNUSED(p_id);

    // test debug 28.01.2021 Added Jeon
    //print_tidal_func();
    QJsonDocument doc(p_jsonObj);//cheon211114-01
    QString strJson(doc.toJson(QJsonDocument::Compact));//cheon211114-01
    qDebug() << "slot_responseHttp_music---" << strJson;
    qDebug() << "global.queuelist_mouse_trigger_flag---" << global.queuelist_mouse_trigger_flag;

    // 나중에 실패요인에 따라서 처리 보강
    if(ProcJsonEasy::get_flagOk(p_jsonObj)){
        bool checkQueue = ProcJsonEasy::getBool(p_jsonObj, "checkQueue");//c1209
        if(checkQueue == true){//c1209
            print_debug();
            //emit linker->signal_checkQueue(6, "");//c220411

            if(global.queuelist_mouse_trigger_flag){//c220503
                print_debug();

                //queuelist_change_replace(false, current_queue_cnt);
                //slot_queuelistScrollbarMotify(0);
                //emit signal_music_start();//c220411
            }

        }
        QString code = ProcJsonEasy::getString(p_jsonObj, "code");
        if(code == "G0000"){

        }
        else if(code == "SLEEP"){
            print_debug();
            emit linker->signal_checkQueue(24, "");

        }
    }
    else{
        //            emit this->signal_cannotRosePlay(tr("I requested the playback with Rose device, but an unknown error occurred."));
        //            emit this->signal_cannotRosePlay(tr("Rose 기기로 재생요청을 하였으나, 알수없는 오류가 발생하였습니다."));
    }

    sender()->deleteLater();

}
void QueuePlayList::slot_responseHttp_motify(const int& p_id, const QJsonObject& p_jsonObj){//cheon210916-total
    //print_debug();
    Q_UNUSED(p_id);
    // test debug 28.01.2021 Added Jeon
    //print_tidal_func();
    print_debug();
    QJsonDocument doc(p_jsonObj);//cheon211114-01
    QString strJson(doc.toJson(QJsonDocument::Compact));//cheon211114-01
    qDebug() << "slot_responseHttp_motify---" << strJson;

    force_newLoad_flag = false;
    this->changeEditMode(true, false, false);
    // 나중에 실패요인에 따라서 처리 보강
    if(ProcJsonEasy::get_flagOk(p_jsonObj)){

        emit linker->signal_checkQueue(11, "");
        force_newLoad_flag = true;
        listWidget_total[global.now_Section]->clear();

        this->after_move_queuelist_flag = true;

        slot_queuelistMotify();
        //reloadList();


        bool checkQueue = ProcJsonEasy::getBool(p_jsonObj, "checkQueue");//c1209
        if(checkQueue == false){//c1209

            //emit linker->signal_checkQueue(0, "");

        }
        QString code = ProcJsonEasy::getString(p_jsonObj, "code");
        if(code == "G0000"){

        }
    }
    else{
        emit linker->signal_checkQueue(12, "");

        force_reflash(p_id);

    }

    sender()->deleteLater();

}



void QueuePlayList::reGet_checkSum()
{
    print_debug();
    QJsonObject tmp_json4;
    NetworkHttp *network4 = new NetworkHttp;
    connect(network4, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_checksum(int, QJsonObject)));
    tmp_json4.insert("roseToken", global.device.getDeviceRoseToken());
    network4->request(HTTP_CURR_QUEUE_GET_CHECKSUM_TOTAL, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_GET_CHECKSUM), tmp_json4, true, true);

}

void QueuePlayList::showEvent_after_reGet_checkSum()
{
    print_debug();
    QJsonObject tmp_json4;
    NetworkHttp *network4 = new NetworkHttp;
    connect(network4, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_checksum_showEvent(int, QJsonObject)));
    tmp_json4.insert("roseToken", global.device.getDeviceRoseToken());
    network4->request(HTTP_CURR_QUEUE_GET_CHECKSUM_TOTAL, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_GET_CHECKSUM), tmp_json4, true, true);

}

int QueuePlayList::currMenutoPid(){
    int p_id = HTTP_CURR_QUEUE_GET_TOTAL;
    print_debug();
    qDebug() << "this->currMenu = " << this->currMenu;
    if(this->currMenu == CURRMENU_TOTAL){
        p_id = HTTP_CURR_QUEUE_GET_TOTAL;
    }else if(this->currMenu == CURRMENU_QOBUZ){
        p_id = HTTP_CURR_QUEUE_GET_QOBUZ;
    }else if(this->currMenu == CURRMENU_BUGS){
        p_id = HTTP_CURR_QUEUE_GET_BUGS;
    }else if(this->currMenu == CURRMENU_TIDAL){
        p_id = HTTP_CURR_QUEUE_GET_TIDAL;
    }else if(this->currMenu == CURRMENU_ROSETUBE){
        p_id = HTTP_CURR_QUEUE_GET_ROSETUBE;
    }else if(this->currMenu == CURRMENU_MUSIC){
        p_id = HTTP_CURR_QUEUE_GET_MUSIC;
    }else if(this->currMenu == CURRMENU_APPLE){
        p_id = HTTP_CURR_QUEUE_GET_APPLEMUSIC;
    }
    return p_id;
}

/**
 * @brief slot_clickedEdit [SLOT] 편집 버튼 클릭시
 */
void QueuePlayList::slot_clickedEdit(){
    print_debug();
    if(global.power_sleepMode_flag) {
        print_debug();
        emit linker->signal_checkQueue(24, "");
        return;
    }
    if(this->mouse_trigger_menu_flag == false){
        emit linker->signal_checkQueue(7, "");
        print_debug();
        return;
    }
    //mouse_all_lock();

    qDebug() << "this->preMenu = " << this->preMenu;
    qDebug() << "*this->currMenu = " << this->currMenu;
    if(isEditMode == true) return;
    //this->changeEditMode(false, false, true);//del

    if(drop_cnt_flag> 0) {
        //this->changeEditMode(false, false, true);
        slot_dragAndDropEdit();
    }



    jsonArr_deletePositon = QJsonArray();

    this->changeEditMode(false, false, true);
    print_debug();
    qDebug() << "this->indexCurrPlay[global.now_Section] = " << this->indexCurrPlay[global.now_Section];
    force_reflashexceptRecomm(currMenutoPid());

}

void QueuePlayList::slot_clickedMove(){
    print_debug();
    if(global.power_sleepMode_flag) {
        print_debug();
        emit linker->signal_checkQueue(24, "");
        return;
    }
    if(this->mouse_trigger_menu_flag == false){
        emit linker->signal_checkQueue(7, "");
        print_debug();
        return;
    }

    //mouse_all_lock();

    qDebug() << "this->preMenu = " << this->preMenu;
    qDebug() << "*this->currMenu = " << this->currMenu;


    if(jsonArr_deletePositon.count() > 0){
        slot_clickedDelete();
    }
    print_debug();
    emit linker->signal_checkQueue(17, "");

    this->changeEditMode(false, true, false);
    force_reflashexceptRecomm(currMenutoPid());

}

void QueuePlayList::slot_clickedPlay(){
    print_debug();
    if(this->mouse_trigger_menu_flag == false){
        emit linker->signal_checkQueue(7, "");
        print_debug();
        return;
    }
    emit linker->signal_checkQueue(17, "");
    click_slot_clickedPlay_flag = true;

    qDebug() << "this->preMenu = " << this->preMenu;
    qDebug() << "*this->currMenu = " << this->currMenu;






    qDebug() << "this->precurrisMode=" << this->precurrisMode;
    if(this->precurrisMode == 1){
        print_debug();
        this->changeEditMode(true, false, false);
        force_play_mode = true;//강제로 처음부터 리로딩 하도록 플레그 참
        this->islistOpen = true;
        this->pre_checkSum = "";
        showEvent(nullptr);
        //slot_queuelistCurrPlayIndexGet();
        //reloadList();
        //force_reflash(currMenutoPid());
        //force_reflashexceptRecomm(currMenutoPid());
    }else {
        print_debug();

        if(jsonArr_deletePositon.count() > 0){
            slot_clickedDelete();
        }
        if(drop_cnt_flag> 0) {
            slot_dragAndDropEdit();
        }
        this->changeEditMode(true, false, false);
        listWidget_total[global.now_Section]->clear();
        if(jsonArr_deletePositon.count() == 0 || drop_cnt_flag == 0){
            // delete_PixmapArray_flag = true;
            print_debug();
            slot_queuelistMotify();
            //reloadList();
        }else{
            print_debug();
            slot_queuelistMotify();

        }

    }


}


bool QueuePlayList::queueList_delDialogConfirm()
{
    DialogConfirm *dlg = new DialogConfirm(this);

    QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
    int latestWidth = settings->value(rosesettings.SETTINGS_W, 0).toInt();
    int latestHeight = settings->value(rosesettings.SETTINGS_H, 800).toInt();
    int left = 0;
    int top = 0;


    left = latestWidth - (dlg->width());
    top = (latestHeight/ 2) - (dlg->sizeHint().height() / 2);
    print_debug();
    qDebug() << "dlg->width() = " << dlg->width();
    qDebug() << "left = " << left;
    dlg->setGeometry(left-50, top, 0, 0);


    dlg->setTitle(tr("Queue List Delete Notice"));
    //dlg->setGeometry(this->width()-400,300,1000,200);
    //dlg->yellow_setStyleSheet();
    dlg->setText(tr("Are you sure you want to delete your selected Queue list?"));
    dlg->setTextHeight(100);

    int result = dlg->exec();

    if(result==QDialog::Accepted){
        return result;
    }
    return result;
    delete dlg;
}
bool QueuePlayList::queueList_draDropEditDialogConfirm()
{
    DialogConfirm *dlg = new DialogConfirm(this);
    QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
    int latestWidth = settings->value(rosesettings.SETTINGS_W, 0).toInt();
    int latestHeight = settings->value(rosesettings.SETTINGS_H, 800).toInt();
    int left = 0;
    int top = 0;


    left = latestWidth - (dlg->width());
    top = (latestHeight/ 2) - (dlg->sizeHint().height() / 2);
    print_debug();
    qDebug() << "dlg->width() = " << dlg->width();
    qDebug() << "left = " << left;
    dlg->setGeometry(left-50, top, 0, 0);
    dlg->setTextHeight(100);



    dlg->setTitle(tr("Queue List Edit Notice"));
    //dlg->setStyleSheet("#wg_total { background-color:rgb(227,184,138);border-radius:4px;border:1px solid #4D4D4D; }");//#333333

    //dlg->yellow_setStyleSheet();
    dlg->setText(tr("Are you sure you want to edit your selected Queue list?"));


    int result = dlg->exec();

    if(result==QDialog::Accepted){
        return result;
    }
    return result;
    delete dlg;
}

/**
 * @brief QueuePlayList::slot_clickedDelete [SLOT] 삭제 버튼 클릭시
 * @note 삭제 처리됨과 동시에 View 모드로 전환된다.
 */
void QueuePlayList::slot_clickedDeleteOk(){//1126
    print_debug();
    if(this->mouse_trigger_menu_flag == false){
        emit linker->signal_checkQueue(7, "");
        print_debug();
        return;
    }


    print_debug();

    if(isEditMode && !isPlayMode && !isMoveMode){

        this->changeEditMode(true, false, false);
        emit linker->signal_checkQueue(15, "");
        slot_clickedPlay();
    }


}


/**
 * @brief QueuePlayList::slot_clickedDelete [SLOT] 삭제 버튼 클릭시
 * @note 삭제 처리됨과 동시에 View 모드로 전환된다.
 */
void QueuePlayList::slot_clickedDelete(){//1126
    print_debug();
    if(this->mouse_trigger_menu_flag == false){
        emit linker->signal_checkQueue(7, "");
        print_debug();
        return;
    }
    //mouse_all_lock();

    print_debug();
    //delete_PixmapArray_flag = false;
    if(isEditMode && !isPlayMode && !isMoveMode){
        print_debug();

        if(jsonArr_deletePositon.count() > 0){

            print_debug();
            for(int i=0; i < jsonArr_deletePositon.count(); i++){


                qDebug() << "jsonArr_deletePositon.at(i).toInt()=" << jsonArr_deletePositon.at(i).toInt();

            }

            if(this->queueList_delDialogConfirm()==QDialog::Accepted){
                emit linker->signal_checkQueue(17, "");
                //delete_PixmapArray_flag = true;
                QList<int> values;

                for(int i=0; i < jsonArr_deletePositon.count(); i++){

                    values.append(jsonArr_deletePositon.at(i).toInt());

                }

                qSort(values);

                //print_debug();
                for(int i=values.count()-1; i >= 0; i--){

                    //double kk = jsonArr_deletePositon.at(i).toDouble();
                    //qDebug() << i << ",";
                    this->list_dataTrackForDelegate[global.now_Section].removeAt(values.at(i));
                    //this->backupPixmapArrayTotal.removeAt(values.at(i));
                }

                QJsonObject jsonObj_send;
                int p_id;
                if(this->currMenu == CURRMENU_TOTAL){
                    p_id = HTTP_CURR_QUEUE_GET_TOTAL;
                    jsonObj_send.insert("playType", "TOTAL");
                }else if(this->currMenu == CURRMENU_QOBUZ){
                    p_id = HTTP_CURR_QUEUE_GET_QOBUZ;
                    jsonObj_send.insert("playType", "QOBUZ");
                }else if(this->currMenu == CURRMENU_BUGS){
                    p_id = HTTP_CURR_QUEUE_GET_BUGS;
                    jsonObj_send.insert("playType", "BUGS");
                }else if(this->currMenu == CURRMENU_TIDAL){
                    p_id = HTTP_CURR_QUEUE_GET_TIDAL;
                    jsonObj_send.insert("playType", "TIDAL");
                }else if(this->currMenu == CURRMENU_ROSETUBE){
                    p_id = HTTP_CURR_QUEUE_GET_ROSETUBE;
                    jsonObj_send.insert("playType", "YOUTUBE");
                }else if(this->currMenu == CURRMENU_MUSIC){
                    p_id = HTTP_CURR_QUEUE_GET_MUSIC;
                    jsonObj_send.insert("playType", "MUSIC");
                }else if(this->currMenu == CURRMENU_APPLE){
                    p_id = HTTP_CURR_QUEUE_GET_APPLEMUSIC;
                    jsonObj_send.insert("playType", "APPLE_MUSIC");
                }
                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_delete(int, QJsonObject)));


                jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
                jsonObj_send.insert("checkSum", global.checkSum);
                jsonObj_send.insert("indexArr", jsonArr_deletePositon);
                print_debug();
                QJsonDocument doc(jsonObj_send);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::slot_clickedDelete()---" << strJson;
                qDebug() << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_delete");
                network->request(3002,
                                 QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_delete"),
                                 jsonObj_send,
                                 true,
                                 true);
                //this->request_totalPlay_current_playlist_delete(1,jsonArr_deletePositon, QJsonArray());//cheon210916-total
                jsonArr_deletePositon  = QJsonArray();


            }


        }else{
            // this->changeEditMode(false, false, true);
            emit linker->signal_checkQueue(15, "");
        }
        lb_title_del->setText(QString(tr("%1 songs selected")).arg(jsonArr_deletePositon.count()));

    }



}



void QueuePlayList::request_musicPlay_current_playlist_delete(const int totalCnt, const int deletePosition, const QJsonObject& jsonObj_deleteObj){
    QJsonArray jsonArr_deletePositon;
    jsonArr_deletePositon.append(deletePosition);
    //print_debug();
    QJsonArray jsonArr_deleteArr;
    jsonArr_deleteArr.append(jsonObj_deleteObj);

    this->request_musicPlay_current_playlist_delete(totalCnt, jsonArr_deletePositon, jsonArr_deleteArr);
}
void QueuePlayList::request_musicPlay_current_playlist_delete(const int totalCnt, const QJsonArray& jsonArr_deletePosition, const QJsonArray& jsonArr_deleteArr){//cheon210916-delete

    //print_debug();
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_delete(int, QJsonObject)));

    // 모두 string 으로 보내야 함
    QJsonObject jsonObj_send;

    jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
    //qDebug() << "QueuePlayList::request_musicPlay_current_playlist_delete----global.checkSum = " << global.checkSum;
    jsonObj_send.insert("checkSum", global.checkSum);
    jsonObj_send.insert("playType", "MUSIC");//cheon211104-01
    jsonObj_send.insert("indexArr", jsonArr_deletePosition);
    //jsonObj_send.insert("deleteArr", jsonArr_deleteArr);

    network->request(3002,
                     QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_delete"),
                     jsonObj_send,
                     true,
                     true);
}

void QueuePlayList::request_rosetubePlay_current_playlist_delete(const int totalCnt, const int deletePosition, const QJsonObject& jsonObj_deleteObj){
    QJsonArray jsonArr_deletePositon;
    jsonArr_deletePositon.append(deletePosition);
    //print_debug();
    QJsonArray jsonArr_deleteArr;
    jsonArr_deleteArr.append(jsonObj_deleteObj);

    this->request_rosetubePlay_current_playlist_delete(totalCnt, jsonArr_deletePositon, jsonArr_deleteArr);
}
void QueuePlayList::request_rosetubePlay_current_playlist_delete(const int totalCnt, const QJsonArray& jsonArr_deletePosition, const QJsonArray& jsonArr_deleteArr){//cheon210916-delete

    //print_debug();
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_delete(int, QJsonObject)));

    // 모두 string 으로 보내야 함
    QJsonObject jsonObj_send;

    jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
    //qDebug() << "ProcRosePlay_withTidal::request_rosetubePlay_current_playlist_delete----global.checkSum = " << global.checkSum;
    jsonObj_send.insert("checkSum", global.checkSum);
    jsonObj_send.insert("playType", "YOUTUBE");//cheon211104-01
    jsonObj_send.insert("indexArr", jsonArr_deletePosition);
    //jsonObj_send.insert("deleteArr", jsonArr_deleteArr);

    network->request(3001,
                     QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_delete"),
                     jsonObj_send,
                     true,
                     true);
}

void QueuePlayList::request_totalPlay_current_playlist_delete(const int totalCnt, const int deletePosition, const QJsonObject& jsonObj_deleteObj){
    QJsonArray jsonArr_deletePositon;
    jsonArr_deletePositon.append(deletePosition);
    //print_debug();
    QJsonArray jsonArr_deleteArr;
    jsonArr_deleteArr.append(jsonObj_deleteObj);

    this->request_totalPlay_current_playlist_delete(totalCnt, jsonArr_deletePositon, jsonArr_deleteArr);
}
void QueuePlayList::request_totalPlay_current_playlist_delete(const int totalCnt, const QJsonArray& jsonArr_deletePosition, const QJsonArray& jsonArr_deleteArr){//cheon210916-delete

    print_debug();
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_delete(int, QJsonObject)));

    // 모두 string 으로 보내야 함
    QJsonObject jsonObj_send;

    jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
    //qDebug() << "QueuePlayList::request_totalPlay_current_playlist_delete----global.checkSum = " << global.checkSum;
    jsonObj_send.insert("checkSum", global.checkSum);
    jsonObj_send.insert("playType", "TOTAL");//cheon211104-01
    jsonObj_send.insert("indexArr", jsonArr_deletePosition);
    //jsonObj_send.insert("deleteArr", jsonArr_deleteArr);
    QJsonDocument doc(jsonObj_send);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "ProcCommon::request_totalPlay_current_playlist_delete---" << strJson;
    qDebug() << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_delete");
    network->request(3002,
                     QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_delete"),
                     jsonObj_send,
                     true,
                     true);
}


void QueuePlayList::slot_responseHttp_delete(const int& p_id, const QJsonObject& p_jsonObj){
    print_debug();
    //Q_UNUSED(p_id);
    // test debug 28.01.2021 Added Jeon
    //print_tidal_func();
    QJsonDocument doc(p_jsonObj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug() << strJson;
    emit linker->signal_checkQueue(6, "");
    // 나중에 실패요인에 따라서 처리 보강
    if(ProcJsonEasy::get_flagOk(p_jsonObj)){
        //emit signal_playCurr_recomm();
        QString code = ProcJsonEasy::getString(p_jsonObj, "code");
        if(code == "G0000"){
            //reloadList();

        }
        // 2> View 모드 전환
        lb_title_del->setText(QString(tr("%1 songs selected")).arg(0));

        this->changeEditMode(false, false, true);


        // 선택된 것이 있는 경우에만 진행
        for(int i=0; i < jsonArr_deletePositon.count(); i++){
            //double kk = jsonArr_deletePositon.at(i).toDouble();
            //qDebug() << "kk = " << kk;
            //this->list_dataTrackForDelegate[global.now_Section].removeAt(kk);
            //this->jsonArray_backup_total[global.now_Section].removeAt(kk);
        }
        jsonArr_deletePositon = QJsonArray();//1126
        if(this->list_dataTrackForDelegate[global.now_Section].count() == 0){
            this->indexCurrPlay[global.now_Section] = -1;

        }else{
            qDebug();
        }
        //reloadList();
        force_reflashexceptRecomm(currMenutoPid());//c220719
        //slot_queuelistCurrPlayIndexGet();
        //reloadList();//c220630

    }
    else{
    }

    sender()->deleteLater();

}



/**
 * @brief QueuePlayList::oneTrackDelete 해당 트랙 1개 개별 삭제 처리
 * @note 여기는.. 해당 트랙만 바로 삭제 처리 삭제 처리되고 편집모드 그대로 유지된 상태...
 * @param p_index
 */
void QueuePlayList::oneTrackDelete(int p_index){

}





/**
 * @brief QueuePlayList::changeEditMode 편집모드로 전환/해제
 * @param p_isEditMode
 */


void QueuePlayList::changeEditMode(bool p_isPlayMode, bool p_isMoveMode, bool p_isEditMode){
    print_debug();qDebug() << "play : "<< p_isPlayMode << "move : "<< p_isMoveMode << "delete : "<< p_isEditMode;
    if(p_isPlayMode==true){
        //this->btn_mov->setStyleSheet(btn_mov->styleSheet().replace("#B18658", "#4c4c4c"));
        this->btn_play->setStyleSheet(btn_play->styleSheet().replace("#333333", "#4c4c4c"));
        this->btn_edit->setStyleSheet(btn_edit->styleSheet().replace("#4c4c4c", "#333333"));
        this->btn_mov->setStyleSheet(btn_mov->styleSheet().replace("#4c4c4c", "#333333"));

        this->precurrisMode = 1;
        print_debug();
        global.changeEditMode_isPlayMode = true;
        this->isPlayMode = true;
        this->isMoveMode = false;
        this->isEditMode = false;
        //this->btn_play->setVisible(true);

        this->btn_PlayPosition->setVisible(true);
        this->btn_mov->setVisible(true);
        this->btn_edit->setVisible(true);
        this->widget_title->setVisible(true);
        this->lb_title_move->setVisible(false);
        this->lb_title_del->setVisible(false);
        this->btn_moveok->setVisible(false);
        this->btn_delok->setVisible(false);

        this->widget_moveMode->setVisible(false);
        this->widget_editMode->setVisible(false);
        //print_debug();

    }
    /*else{
        print_debug();
        this->isPlayMode = false;
        this->btn_play->setVisible(true);
        this->btn_mov->setVisible(true);
        this->btn_edit->setVisible(true);
        this->btn_moveok->setVisible(false);
        this->btn_delok->setVisible(false);
        print_debug();

    }
    */
    else if(p_isMoveMode==true){
        this->btn_play->setStyleSheet(btn_play->styleSheet().replace("#4c4c4c", "#333333"));
        this->btn_edit->setStyleSheet(btn_edit->styleSheet().replace("#4c4c4c", "#333333"));
        this->btn_mov->setStyleSheet(btn_mov->styleSheet().replace("#333333", "#4c4c4c"));

        this->precurrisMode = 2;
        print_debug();
        this->isPlayMode = false;
        this->isMoveMode = true;
        this->isEditMode = false;
        global.changeEditMode_isPlayMode = false;
        this->btn_PlayPosition->setVisible(false);
        this->widget_title->setVisible(false);
        this->lb_title_move->setVisible(true);
        this->lb_title_del->setVisible(false);
        this->btn_mov->setVisible(false);
        this->btn_edit->setVisible(false);
        this->btn_moveok->setVisible(true);
        this->btn_delok->setVisible(false);
        //print_debug();
        this->widget_moveMode->setVisible(false);
        this->widget_editMode->setVisible(false);
        //print_debug();
    }
    /*else {
        print_debug();
        this->isMoveMode = false;
        this->btn_play->setVisible(true);
        this->btn_mov->setVisible(true);
        this->btn_edit->setVisible(true);
        this->btn_moveok->setVisible(true);
        this->btn_delok->setVisible(false);
        print_debug();
    }
    */
    else if(p_isEditMode==true){
        this->btn_play->setStyleSheet(btn_play->styleSheet().replace("#4c4c4c", "#333333"));
        this->btn_edit->setStyleSheet(btn_edit->styleSheet().replace("#333333", "#333333"));
        this->btn_mov->setStyleSheet(btn_mov->styleSheet().replace("#4c4c4c", "#333333"));

        this->isAllSelected = false;
        checkbox_allSelected->setChecked(false);
        print_debug();
        this->precurrisMode = 3;
        this->isPlayMode = false;
        this->isMoveMode = false;
        this->isEditMode = true;
        global.changeEditMode_isPlayMode = false;
        this->btn_PlayPosition->setVisible(false);
        //this->btn_play->setVisible(true);
        this->widget_title->setVisible(false);
        this->lb_title_move->setVisible(false);
        this->lb_title_del->setVisible(true);
        this->btn_mov->setVisible(false);
        this->btn_edit->setVisible(false);
        this->btn_moveok->setVisible(false);
        this->btn_delok->setVisible(true);
        //print_debug();
        this->widget_moveMode->setVisible(false);
        this->widget_editMode->setVisible(true);
        //print_debug();
    }else{
        this->btn_play->setStyleSheet(btn_play->styleSheet().replace("#4c4c4c", "#333333"));
        this->btn_edit->setStyleSheet(btn_edit->styleSheet().replace("#4c4c4c", "#333333"));
        this->btn_mov->setStyleSheet(btn_mov->styleSheet().replace("#4c4c4c", "#333333"));

        this->isAllSelected = false;
        this->precurrisMode = 0;
        this->isPlayMode = true;
        this->isMoveMode = false;
        this->isEditMode = false;
        global.changeEditMode_isPlayMode = true;
        //this->btn_play->setVisible(true);
        this->btn_PlayPosition->setVisible(false);
        this->widget_title->setVisible(false);
        this->btn_mov->setVisible(false);
        this->btn_edit->setVisible(false);
        this->btn_moveok->setVisible(false);
        this->btn_delok->setVisible(false);
        this->widget_moveMode->setVisible(false);
        this->widget_editMode->setVisible(false);
        print_debug();
    }
    /*else{
        print_debug();
        this->isEditMode = false;
        this->btn_play->setVisible(true);
        this->btn_mov->setVisible(true);
        this->btn_edit->setVisible(true);
        this->btn_moveok->setVisible(false);
        this->btn_delok->setVisible(false);
        print_debug();
    }
    */
    global.isMoveMode = this->isMoveMode;


}

void QueuePlayList::loadingGif(){
    print_debug();
    qDebug()<<"###########################################################################################";
    //QLabel *Movie = new QLabel(this);
    //pixmapCheckOff.load(":images/playlist/list_check_off.png");
    // QFile f("C:/Users/doulz/Rose_Connect-21-12-21-20/images/Spinner-1s-200px.gif");
    // QByteArray *b = new QByteArray;
    // if (f.open(QFile::ReadOnly)) {
    //     b = new QByteArray(f.readAll());
    //    f.close();
    // }
    // QBuffer *buffer = new QBuffer(b);
    QLabel *Movie = new QLabel(this);
    QMovie *mov = new QMovie("C:/Users/doulz/Rose_Connect-21-12-21-20/images/Spinner-1s-200px.gif");
    Movie->setMovie(mov);
    Movie->setGeometry(20, 20, 392, 225);
    mov->start();
}


void QueuePlayList::RemoveLayout(QWidget* widget)
{

    //print_debug();
    QLayout* layout = widget->layout();

    // GSCommon::clearLayout_new(layout);

    if (layout != 0)
    {
        //print_debug();
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != 0){
            layout->removeItem(item);
        }


        delete layout;


    }


}

void QueuePlayList::queuelist_change_replace_recovery(int section){
    print_debug();
    qDebug() << "section = " << section;
    int tmp_indexCurrPlay = get_IndexCurrPlayfromNextIndexNo(section);
    qDebug() << "this->list_dataTrackForDelegate[section].count() = " << this->list_dataTrackForDelegate[section].count();
    qDebug() << "this->indexCurrPlay[section] = " << this->indexCurrPlay[section];
    qDebug() << "this->indexCurrPlay[0] = " << this->indexCurrPlay[0];
    qDebug() << "tmp_indexCurrPlay = " << tmp_indexCurrPlay;


    DataTrackforDelegate tmp_dataTrack;//"background-color:#735639;"
    if(tmp_indexCurrPlay < 0 && listWidget_total[section]->count() > 0 ){
        for(int i= 0 ; i < this->list_dataTrackForDelegate[section].count(); i++){
            QListWidgetItem *tmp_item = listWidget_total[section]->item(i);
            QWidget* ww = listWidget_total[section]->itemWidget(tmp_item);

            if(ww != 0){
                QLayout* layout = widget[section].at(i)->layout();

                if (layout != 0){
                    // print_debug();
                    QString temp = widget[section].at(i)->styleSheet();
                    // qDebug() << "styleSheet-temp=" << temp;
                    bool ret = temp.contains("background-color:#735639", Qt::CaseInsensitive); // 대소문자 구분 없이
                    if( ret){
                        tmp_dataTrack = this->list_dataTrackForDelegate[section].at(i);
                        //  qDebug() << "pre_menu - tmp_dataTrack.title = " << tmp_dataTrack.title;
                        tmp_dataTrack.isPlay = false;
                        this->list_dataTrackForDelegate[section].replace(i, tmp_dataTrack);

                        std_listWidget_one_line_replace_only(section, i);
                    }
                }
            }

            //lb_queueListImg[m_index].at(this->indexCurrPlay[m_index])->setPixmap(QPixmap(pixmapPlaying));

        }
        print_debug();


    }
    print_debug();


}

void QueuePlayList::queuelist_change_replace(bool flag, int menu_count){

    print_debug();
    int preIndex = -1 ;//c1217
    print_debug();
    //
    if(!this->isPlayMode) return;//c220530

    //queuelist_change_replace_recovery(pre_now_Section);
    qDebug() << "current_queue_cnt =" << current_queue_cnt;
    qDebug() << "this->list_dataTrackForDelegate[global.now_Section].count() =" << this->list_dataTrackForDelegate[global.now_Section].count();
    qDebug() << "this->indexPrePlay[global.now_Section] =" << this->indexPrePlay[global.now_Section];
    qDebug() << "this->indexCurrPlay[global.now_Section] =" << this->indexCurrPlay[global.now_Section];
    qDebug() << "menu_count =" << menu_count;
    qDebug() << "global.now_Section =" << global.now_Section;
    qDebug() << "currMenutoPid() =" << currMenutoPid();
    if(menu_count == 0){
        this->indexCurrPlay[global.now_Section] = -1;
        this->indexPrePlay[global.now_Section] = -1;
        return;
    }
    /*
     //for(int i = 0 ; i < 7; i++){
         if(tabSectionlist[i] != 0){
            this->indexCurrPlay[tabSectionlist[i]] = get_IndexCurrPlayfromNextIndexNo(tabSectionlist[i]);
         }
         //qDebug() << tabSectionlist[i] << ":" << this->indexCurrPlay[tabSectionlist[i]] << ", ";
     //}
     */

    this->indexCurrPlay[global.now_Section] = get_IndexCurrPlayfromNextIndexNo(global.now_Section);



    print_debug();
    qDebug() << "this->indexPrePlay[global.now_Section] =" << this->indexPrePlay[global.now_Section];
    qDebug() << "this->indexCurrPlay[global.now_Section] =" << this->indexCurrPlay[global.now_Section];

    for(int i = 0; i < menu_count; i++){
        if(menu_count == this->list_dataTrackForDelegate[global.now_Section].count()){
            DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(i);

            if(tmp_dataTrack.no == this->indexPrePlay[global.now_Section]){
                print_debug();
                qDebug() << "pre-tmp_dataTrack.no=" << tmp_dataTrack.no;
                qDebug() << "pre-tmp_dataTrack.title=" << tmp_dataTrack.title;
                qDebug() << "i=" << i;
                preIndex = i;
                break;
            }

        }

    }

    print_debug();
    qDebug() << "this->indexCurrPlay[global.now_Section]=" << this->indexCurrPlay[global.now_Section];

    qDebug() << "preIndex=" << preIndex;

    if(this->indexPrePlay[global.now_Section] != this->indexCurrPlay[global.now_Section]){
        DataTrackforDelegate tmp_dataTrack;
        if(this->indexPrePlay[global.now_Section] >= 0 && this->indexPrePlay[global.now_Section] < this->list_dataTrackForDelegate[global.now_Section].count() ){
            print_debug();
            /*
                tmp_dataTrack = this->list_dataTrackForDelegate[tabSectionlist[0]].at(this->indexPrePlay[global.now_Section]);
                qDebug() << "indexCurrPlay[global.now_Section] - tmp_dataTrack.title = " << tmp_dataTrack.title;
                tmp_dataTrack.isPlay = false;
                this->list_dataTrackForDelegate[tabSectionlist[0]].replace(this->indexPrePlay[global.now_Section], tmp_dataTrack);

*/
            tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(this->indexPrePlay[global.now_Section]);
            qDebug() << "indexCurrPlay[global.now_Section] - tmp_dataTrack.title = " << tmp_dataTrack.title;
            tmp_dataTrack.isPlay = false;
            this->list_dataTrackForDelegate[global.now_Section].replace(this->indexPrePlay[global.now_Section], tmp_dataTrack);


        }
        if(this->indexCurrPlay[global.now_Section] >= 0 && this->indexCurrPlay[global.now_Section] < this->list_dataTrackForDelegate[global.now_Section].count()){
            DataTrackforDelegate tmp_dataTrack;
            /*
                tmp_dataTrack = this->list_dataTrackForDelegate[tabSectionlist[0]].at(this->indexCurrPlay[global.now_Section]);
                qDebug() << "indexCurrPlay[global.now_Section] - tmp_dataTrack.title = " << tmp_dataTrack.title;
                tmp_dataTrack.isPlay = true;
                this->list_dataTrackForDelegate[tabSectionlist[0]].replace(this->indexCurrPlay[global.now_Section], tmp_dataTrack);

*/
            tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(this->indexCurrPlay[global.now_Section]);
            qDebug() << "indexCurrPlay[global.now_Section] - tmp_dataTrack.title = " << tmp_dataTrack.title;
            tmp_dataTrack.isPlay = true;
            this->list_dataTrackForDelegate[global.now_Section].replace(this->indexCurrPlay[global.now_Section], tmp_dataTrack);

        }
    }
    //
    if(preIndex >=0 && this->indexCurrPlay[global.now_Section] >=0 && this->indexCurrPlay[global.now_Section] != preIndex){
        print_debug();
        //view_listWidget_setItemWidget_replace(this->indexCurrPlay[global.now_Section],preIndex);

        view_listWidget_setItemWidget_replace_only(this->indexCurrPlay[global.now_Section],preIndex);



    }else if(this->indexCurrPlay[global.now_Section] >=0 && preIndex < 0){

        print_debug();

        DataTrackforDelegate tmp_dataTrack;
        tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(this->indexCurrPlay[global.now_Section]);
        qDebug() << "index_old - tmp_dataTrack.title = " << tmp_dataTrack.title;
        tmp_dataTrack.isPlay = true;
        this->list_dataTrackForDelegate[global.now_Section].replace(this->indexCurrPlay[global.now_Section], tmp_dataTrack);
        std_listWidget_one_line_replace_only(this->indexCurrPlay[global.now_Section]);

    }else if(preIndex >=0 && this->indexCurrPlay[global.now_Section] < 0){
        DataTrackforDelegate tmp_dataTrack;
        tmp_dataTrack = this->list_dataTrackForDelegate[global.now_Section].at(preIndex);
        qDebug() << "index_old - tmp_dataTrack.title = " << tmp_dataTrack.title;
        tmp_dataTrack.isPlay = false;
        this->list_dataTrackForDelegate[global.now_Section].replace(preIndex, tmp_dataTrack);
        std_listWidget_one_line_replace_only(preIndex);

    }
    print_debug();
    this->indexPrePlay[global.now_Section] = this->indexCurrPlay[global.now_Section];

    qDebug() << "this->indexCurrPlay[global.now_Section] =" << this->indexCurrPlay[global.now_Section];
    qDebug() << "this->indexPrePlay[global.now_Section] =" << this->indexPrePlay[global.now_Section];


    mouse_all_clear();


    slot_queuelistScrollbarMotify(0);


}

/*
    for(int i = 0 ; i < 7; i++){
        this->indexPrePlay[tabSectionlist[i]] = this->indexCurrPlay[tabSectionlist[i]];
        //qDebug() << i << ":" << this->indexCurrPlay[tabSectionlist[i]] << ", ";
    }
    */

bool QueuePlayList::check_func(const int &p_id, const int &menu_count){
    print_debug();

    //lb_title->setText(tr("Queue List  (%1/3000)").arg(total_queue_list_count));//cheon211114-01
    qDebug() << "QueuePlayList::check_func----menu_count= " << menu_count;
    qDebug() << "QueuePlayList::check_func----global.queue_playFlag= " << global.queue_playFlag;
    qDebug() << "QueuePlayList::check_func----mouse_trigger_menu_flag= " << mouse_trigger_menu_flag;
    qDebug() << "lb_queueListImg[global.now_Section].count() = " << lb_queueListImg[global.now_Section].count();

    return true;
}

void QueuePlayList::force_reflash(int p_id){
    print_debug();
    emit linker->signal_checkQueue(17, "");
    qDebug() << "p_id = " << p_id;
    only_replace_flag = false;
    this->view_listWidget_setItemWidget(p_id);
    //if(this->isPlayMode){
    if(this->indexCurrPlay[global.now_Section] < 0) {
        slot_listwidgetcurrentRowChanged_start(0);
        //emit signal_valueChanged(0);
    }else{
        slot_listwidgetcurrentRowChanged_start(this->indexCurrPlay[global.now_Section]);
        // emit signal_valueChanged(this->indexCurrPlay[global.now_Section]);
    }
    //}
    //print_debug();
    if(this->isPlayMode){

        slot_responseHttp_recomm_createlist(p_id);

    }else{
        print_debug();

    }
#if defined(Q_OS_WINDOWS)
    if(this->indexCurrPlay[global.now_Section] < 0){
        QListWidgetItem* item = this->listWidget_total[global.now_Section]->item(0);

        listWidget_total[global.now_Section]->scrollToItem(item, QAbstractItemView::EnsureVisible);//c220629
    }else{
        QListWidgetItem* item = this->listWidget_total[global.now_Section]->item(this->indexCurrPlay[global.now_Section]);

        listWidget_total[global.now_Section]->scrollToItem(item, QAbstractItemView::EnsureVisible);//c220629
    }


#endif
#if defined(Q_OS_MAC)
    if(this->indexCurrPlay[global.now_Section] < 0){
        listWidget_total[global.now_Section]->setCurrentRow(0);//c220629
    }else{
        listWidget_total[global.now_Section]->setCurrentRow(this->indexCurrPlay[global.now_Section]);//c220629
    }

#endif
    print_debug();



    slot_reGet_checkSum();

}

void QueuePlayList::force_reflashexceptRecomm(int p_id){

    print_debug();
    if(!this->isPlayMode){
        emit linker->signal_checkQueue(17, "");
        qDebug() << "p_id = " << p_id;
        qDebug() << "only_replace_flag = " << only_replace_flag;
        only_replace_flag = false;
        this->view_listWidget_setItemWidget(p_id);
        qDebug() << "--this->indexCurrPlay[global.now_Section] =" << this->indexCurrPlay[global.now_Section];
        qDebug() << "--listWidget_total[global.now_Section]->currentRow() =" << listWidget_total[global.now_Section]->verticalScrollBar()->sliderPosition();
        //listWidget_total[global.now_Section]->setCurrentRow(this->indexCurrPlay[global.now_Section]);
        slot_listwidgetcurrentRowChanged_start(listWidget_total[global.now_Section]->verticalScrollBar()->sliderPosition());

        emit linker->signal_checkQueue(6, "");
        print_debug();
        mouse_all_clear();

    }
    //print_debug();
    // slot_responseHttp_recomm_createlist(p_id);

}

void QueuePlayList::mouse_all_clear(){
    print_debug();
    mouse_trigger_menu_flag = true;
    global.queuelist_mouse_trigger_flag = true;
    global.queuelist_mouse_trigger_menu_flag = true;
    this->mouse_trigger_flag = true;
    mouse_clickable_state_print();

}

void QueuePlayList::mouse_all_lock(){
    print_debug();
    mouse_trigger_menu_flag = false;
    global.queuelist_mouse_trigger_flag = false;
    global.queuelist_mouse_trigger_menu_flag = false;
    this->mouse_trigger_flag = false;
    mouse_clickable_state_print();

}
void QueuePlayList::mouse_all_semiLock(){
    print_debug();
    mouse_trigger_menu_flag = false;
    global.queuelist_mouse_trigger_flag = true;
    global.queuelist_mouse_trigger_menu_flag = false;
    this->mouse_trigger_flag = true;
    mouse_clickable_state_print();

}

void QueuePlayList::mouse_clickable_state_print(){
    print_debug();
    qDebug() << "******************mouse_trigger_flag = " << mouse_trigger_flag;
    qDebug() << "******************mouse_trigger_menu_flag = " << mouse_trigger_menu_flag;
    qDebug() << "******************global.queuelist_mouse_trigger_flag = " << global.queuelist_mouse_trigger_flag;
    qDebug() << "******************global.queuelist_mouse_trigger_menu_flag = " << global.queuelist_mouse_trigger_menu_flag;

}


void QueuePlayList::append_widget(){
    for(int i = 0 ;  i < current_queue_cnt ; i++){


        //for(int i = 0; i < this->list_thumbPath[global.now_Section].count(); i++){
        QObject::disconnect(disConSelectId[i]);
        //}

        widget[global.now_Section].append(new QWidget());

        lb_queueListImg[global.now_Section].append(new QLabel());


        queue_setListWidget(i);



        checkbox_Selected[global.now_Section].append(new QCheckBox());
        checkbox_Selected[global.now_Section].at(i)->setProperty("check_index", i);
        checkbox_Selected[global.now_Section].at(i)->setStyleSheet(
                    "#checkbox_Selected { background-color:transparent;spacing:0px;}"
                    "QCheckBox::indicator:unchecked { image: url(:images/playlist/list_check_off.png);}"
                    "QCheckBox::indicator:checked { image: url(:images/playlist/list_check_on.png);}"
                    );
        //checkbox_Selected->setCursor(Qt::PointingHandCursor);
        checkbox_Selected[global.now_Section].at(i)->setFixedSize(40,33);
        disConSelectId[i] = connect(checkbox_Selected[global.now_Section].at(i), SIGNAL(stateChanged(int)), this, SLOT(slot_clickedSelected(int)));
    }


}

bool QueuePlayList::queueReflash_checking(int p_id){//1126
    print_debug();
    /*
    if(force_newLoad_flag == true){
        force_newLoad_flag = false;
        print_debug();
        return true;
    }
    force_newLoad_flag = false;
    if(click_slot_clickedPlay_flag) {
        print_debug();
        click_slot_clickedPlay_flag =  false;
        return true;
    }
*/

    switch(p_id){
    case HTTP_CURR_QUEUE_GET_TOTAL://cheon21114-01
        print_debug();
        current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[0]].count();
        break;
    case HTTP_CURR_QUEUE_GET_TIDAL://cheon210916-total
        print_debug();
        current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[3]].count();
        break;
    case HTTP_CURR_QUEUE_GET_BUGS://cheon210916-total
        print_debug();
        current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[4]].count();
        break;
    case HTTP_CURR_QUEUE_GET_QOBUZ://cheon210916-total
        print_debug();
        current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[5]].count();
        break;
    case HTTP_CURR_QUEUE_GET_MUSIC ://cheon210916-total
        print_debug();
        current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[1]].count();
        break;
    case HTTP_CURR_QUEUE_GET_ROSETUBE ://cheon210916-total
        print_debug();
        current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[2]].count();
        break;
    case HTTP_CURR_QUEUE_GET_APPLEMUSIC ://cheon210916-total
        print_debug();
        current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[6]].count();
        break;
    }
    QLocale aEnglish;
    lb_title_cnt->setText(tr("%1 / 3,000").arg(aEnglish.toString(this->current_queue_cnt)));
    qDebug() << "this->current_queue_cnt=" << this->current_queue_cnt;
    /*
    if(this->pre_checkSum != global.checkSum){
       print_debug();
       equelRecieveJsonFlag[global.now_Section] = false;
       this->pre_checkSum = global.checkSum;
       return true;
    }else if(listWidget_total[global.now_Section]->count() > 0 && listWidget_total[global.now_Section]->count() >= indexCurrPlay[global.now_Section]){
        print_debug();
        equelRecieveJsonFlag[global.now_Section] = true;
        if(this->currMenu == CURRMENU_TOTAL){
            current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[0]].count();
        }else if(this->currMenu == CURRMENU_QOBUZ){
            current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[5]].count();
        }else if(this->currMenu == CURRMENU_BUGS){
            current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[4]].count();
        }else if(this->currMenu == CURRMENU_TIDAL){
            current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[3]].count();
        }else if(this->currMenu == CURRMENU_ROSETUBE){
            current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[2]].count();
        }else if(this->currMenu == CURRMENU_MUSIC){
            current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[1]].count();
        }else if(this->currMenu == CURRMENU_APPLE){
            current_queue_cnt = this->list_dataTrackForDelegate[tabSectionlist[6]].count();
        }

        QLocale aEnglish;
        lb_title_cnt->setText(tr("%1 / 3,000").arg(aEnglish.toString(this->current_queue_cnt)));

        //slot_listwidgetcurrentRowChanged_start(indexCurrPlay[global.now_Section]);
        //slot_listwidgetcurrentRowChanged_start(nextPlayNo);
        //queuelist_change_replace(false, current_queue_cnt);

        print_debug();

        //slot_queuelistPositionGet_http_signal();
        queuelist_change_replace(false, current_queue_cnt);

        emit linker->signal_checkQueue(6, "");
        return false;

    }else{
        //reloadList();
        //return false;
    }
    //slot_queuelistCurrPlayIndexGet();//c220625
*/
    return true;
}


bool QueuePlayList::TotalQueueListCount_afterGetFile(const int &p_id, const QJsonObject &p_jsonObject){
    print_debug();

    return true;
}
/**
 * @brief QueuePlayList::responseHttp : 로즈 장치 정보요청 결과 처리
 * @param p_jsonObject QJsonObject&
 */
void QueuePlayList::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){//1126
    print_debug();

    emit linker->signal_checkQueue(17, "");

    this->currSelectedMenu = p_id;
    print_debug();
    qDebug() << "********** mouse_trigger_menu_flag== "<< mouse_trigger_menu_flag;
    qDebug() << "********** global.queue_recent_track_addFlag== "<< global.queue_recent_track_addFlag;

    //int old_index = indexCurrPlay[global.now_Section];
    qDebug() << "this->nextPlayNo; = " << this->nextPlayNo;;


    emit linker->signal_clickListBtn();
    this->setTotalQueueList(p_id, p_jsonObject);//------------------------------------------

    for(int i = 0 ; i < 7; i++){
        this->indexCurrPlay[tabSectionlist[i]] = get_IndexCurrPlayfromNextIndexNo(tabSectionlist[i]);
        qDebug() << i << ":" << this->indexCurrPlay[tabSectionlist[i]] << ", ";
    }
    print_debug();
    for(int i = 0 ; i < 7; i++){
        qDebug() << "this->list_dataTrackForDelegate[tabSectionlist[i]].count() = " << this->list_dataTrackForDelegate[tabSectionlist[i]].count();
    }

    if(!queueReflash_checking(p_id)){//c220526

        print_debug();
        mouse_all_clear();
        //slot_queuelistCurrPlayIndexGet();//c220625

        emit linker->signal_checkQueue(6, "");
        sender()->deleteLater();//c220603
        print_debug();
        qDebug()<<"==================queueReflash_checking---return;";
        return;
    }

    qDebug() << "this->list_thumbPath[global.now_Section].count() = " << this->list_thumbPath[global.now_Section].count();
    QJsonDocument doc(p_jsonObject); QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() << "QueuePlayList::setTotalQueueList---- " << strJson << "\n";

    //-------------------------------------------------------------------------------------------
    emit linker->signal_clickListBtn();
    this->view_listWidget_setItemWidget(p_id);

    print_debug();
    qDebug() << "this->indexCurrPlay[global.now_Section] = " << this->indexCurrPlay[global.now_Section];

    emit linker->signal_clickListBtn();
    if(this->indexCurrPlay[global.now_Section] < 0) {

        slot_listwidgetcurrentRowChanged_start(0);
        //emit signal_valueChanged(0);
    }else{
        slot_listwidgetcurrentRowChanged_start(this->indexCurrPlay[global.now_Section]);
        // emit signal_valueChanged(this->indexCurrPlay[global.now_Section]);
    }
    //print_debug();
    if(this->isPlayMode){

        slot_responseHttp_recomm_createlist(p_id);

    }else{
        mouse_all_clear();
        print_debug();

    }
    emit linker->signal_checkQueue(6, "");
    sender()->deleteLater();
    print_debug();

}



void QueuePlayList::slot_responseHttp_checksum(const int &p_id, const QJsonObject &p_jsonObject){//cheon210916-total

    //print_debug();
    //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::slot_responseHttp_checksum---" << strJson;

    switch(p_id){
    case HTTP_CURR_QUEUE_GET_CHECKSUM_TOTAL://cheon211107-01
        //print_debug();
        this->responseQueueList_checksum(p_jsonObject);
        break;

    }

    sender()->deleteLater();
}

void QueuePlayList::slot_responseHttp_checksum_showEvent(const int &p_id, const QJsonObject &p_jsonObject){//cheon210916-total

    //print_debug();
    //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "QueuePlayList::slot_responseHttp_checksum---" << strJson;

    switch(p_id){
    case HTTP_CURR_QUEUE_GET_CHECKSUM_TOTAL://cheon211107-01
        //print_debug();
        this->responseQueueList_checksum(p_jsonObject);
        showEvent(nullptr);
        break;

    }

    sender()->deleteLater();
}
void QueuePlayList::responseHttp_flagOkFalseMsg(){//c221004_1
    print_debug();
    DialogConfirm *dlgConfirmOutput = new DialogConfirm(this);
    dlgConfirmOutput->setAlignment(Qt::AlignLeft);
    dlgConfirmOutput->setTitle(tr("Notice"));
    dlgConfirmOutput->setText(tr("Failed to get cue list information. Would you like to try again?"));
    dlgConfirmOutput->setGeometry((this->pos().rx() + 80), (350 + 100), 350, 500);
    dlgConfirmOutput->setAlertMode();
    //dlgConfirmOutput->setProperty("flagShown",false);

    if(dlgConfirmOutput->property("flagShown").toBool()==false){
        //dlgConfirmOutput->setProperty("flagShown",true);

        int result = dlgConfirmOutput->exec();

        if(result == QDialog::Accepted){
            print_debug();
            slot_queuelistCurrPlayIndexGet();
        }
    }
}

void QueuePlayList::slot_responseHttp_get(const int &p_id, const QJsonObject &p_jsonObject){//cheon211104-01

    print_debug();
    qDebug() << "lb_queueListImg[global.now_Section].count() = " << lb_queueListImg[global.now_Section].count();
    qDebug()<< "p_id = " << p_id;
    QJsonDocument doc(p_jsonObject);QString strJson(doc.toJson(QJsonDocument::Compact));qDebug() << "QueuePlayList::slot_responseHttp_get--HTTP_CURR_TOTAL_QUEUE_GET-- " << strJson << "\n";
    switch(p_id){
    case HTTP_CURR_TOTAL_QUEUE_GET:
        print_debug();
        if(p_jsonObject.contains("flagOk")){
            if(p_jsonObject["flagOk"].toBool()==true){// && p_jsonObject["code"].toString()!="G0000"){
                if(p_jsonObject.contains("position")){

                    print_debug();

                    this->nextPlayNo = p_jsonObject["position"].toInt();

                    //this->indexCurrPlay[global.now_Section] = this->nextPlayNo;

                    qDebug() << "QueuePlayList::slot_responseHttp_get--this->nextPlayNo-" << this->nextPlayNo;
                    qDebug() << "this->indexCurrPlay[global.now_Section] = " << this->indexCurrPlay[global.now_Section];

                    reloadList();


                }
            }else{//c221004_1
                print_debug();
                qDebug() << "flagOK is false";
                responseHttp_flagOkFalseMsg();
            }

        }else{
            print_debug();
            qDebug() << "flagOK is false";
            responseHttp_flagOkFalseMsg();//c221004_1

        }
        break;

    case HTTP_CURR_TOTAL_QUEUE_POSITION_GET://cheon211206
        print_debug();
        music_start_positionGet_flag = false;

        if(p_jsonObject.contains("flagOk")){
            if(p_jsonObject["flagOk"].toBool()==true){
                if(p_jsonObject.contains("position")){
                    print_debug();

                    this->nextPlayNo = p_jsonObject["position"].toInt();




                    qDebug() << "HTTP_CURR_TOTAL_QUEUE_POSITION_GET---this->indexCurrPlay[global.now_Section] = " << this->indexCurrPlay[global.now_Section];
                    qDebug() << "HTTP_CURR_TOTAL_QUEUE_POSITION_GET---this->nextPlayNo = " << this->nextPlayNo;
                    //qDebug() << "HTTP_CURR_TOTAL_QUEUE_POSITION_GET---nextIndex = " << nextIndex;
                    qDebug() << "HTTP_CURR_TOTAL_QUEUE_POSITION_GET---current_queue_cnt = " << current_queue_cnt;
                    qDebug() << "global.repeatMode = " << global.repeatMode;
                    qDebug() << "this->currMenu = " << this->currMenu;
                    qDebug() << "HTTP_CURR_TOTAL_QUEUE_POSITION_GET---global.queue_recent_track_addFlag = " << global.queue_recent_track_addFlag;
                    print_debug();
                    //qDebug() << "HTTP_CURR_TOTAL_QUEUE_POSITION_GET---QueuePlayList::slot_responseHttp_get--this->nextPlayNo-" << this->nextPlayNo;
                    if(this->currMenu == CURRMENU_TOTAL){
                        global.now_Section = 0;
                    }else if(this->currMenu == CURRMENU_MUSIC){
                        global.now_Section = 1;
                    }else if(this->currMenu == CURRMENU_ROSETUBE){
                        global.now_Section = 4;
                    }else if(this->currMenu == CURRMENU_TIDAL){
                        global.now_Section = 6;
                    }else if(this->currMenu == CURRMENU_BUGS){
                        global.now_Section = 7;
                    }else if(this->currMenu == CURRMENU_QOBUZ){
                        global.now_Section = 8;
                    }else if(this->currMenu == CURRMENU_APPLE){
                        global.now_Section = 9;
                    }
                    qDebug() << "global.now_Section = " << global.now_Section;
                    int tmp_indexCurrPlay = get_IndexCurrPlayfromNextIndexNo(global.now_Section);
                    qDebug() << "tmp_indexCurrPlay = " << tmp_indexCurrPlay;
                    qDebug() << "this->indexCurrPlay[global.now_Section] = " << this->indexCurrPlay[global.now_Section];
                    if(tmp_indexCurrPlay == this->indexCurrPlay[global.now_Section] && this->indexCurrPlay[global.now_Section] != 2999){//c220826_2

                        //return;//c220826_2
                    }else{

                    }
                    print_debug();
                    this->reloadList();

                    print_debug();
                    //global.queue_recent_track_addFlag = false;
                    qDebug() << "*music_start_positionGet_flag= " << music_start_positionGet_flag;
                    qDebug() << "*current_queue_cnt= " << current_queue_cnt;

                    qDebug() << "*this->isEditMode= " << this->isEditMode;
                    qDebug() << "*this->mouse_trigger_flag= " << this->mouse_trigger_flag;
                    qDebug() << "*global.queue_recent_track_addFlag= " << global.queue_recent_track_addFlag;
                    if(this->http_signal_flag) this->http_signal_flag = false;

                }
            }

        }
        break;

    case HTTP_CURR_TOTAL_QUEUE_CURRPLAYINDEX_GET_CHECK://timer exec

        print_debug();
        qDebug() << "*global.queueTimerFlag= " << global.queueTimerFlag;

        if(p_jsonObject.contains("flagOk")){
            if(p_jsonObject["flagOk"].toBool()==true){
                if(p_jsonObject.contains("position")){
                    print_debug();
                    //mouse_trigger_menu_flag =  true;
                    //global.queuelist_mouse_trigger_flag =  true;
                    //global.queuelist_mouse_trigger_flag =  true;
                    //global.queuelist_mouse_trigger_menu_flag =  true;
                    this->nextPlayNo = p_jsonObject["position"].toInt();
                    slot_checking_currPlay();
                }

            }
        }
        break;

    case HTTP_CURR_TOTAL_QUEUE_CURRPLAYINDEX_GET:

        print_debug();
        qDebug() << "*global.queueTimerFlag= " << global.queueTimerFlag;
        if(global.queueTimerFlag == false){
            // timer->stop();
        }
        print_debug();
        if(p_jsonObject.contains("flagOk")){
            if(p_jsonObject["flagOk"].toBool()==true){
                if(p_jsonObject.contains("position")){

                    this->nextPlayNo = p_jsonObject["position"].toInt();

                    //mouse_all_clear();

                    if(this->indexCurrPlay[global.now_Section] >= 0 ){
                        //  queuelist_change_replace(true, current_queue_cnt);//c220625
                    }

                    //slot_queuelistScrollbarMotify(0);


                }
            }
        }

        break;
    case HTTP_CURR_TOTAL_QUEUE__RECOMM_GET://cheon211114-01
        print_debug();

        //QJsonDocument doc1(p_jsonObject);
        //QString strJson1(doc1.toJson(QJsonDocument::Compact));
        //qDebug() << "QueuePlayList::slot_responseHttp_get_position--HTTP_CURR_TOTAL_QUEUE__RECOMM_GET-- " << strJson1 << "\n";

        //global.checkSum = "";
        if(p_jsonObject.contains("flagOk")){
            if(p_jsonObject["flagOk"].toBool()==true){
            }
        }

        break;
    }
    sender()->deleteLater();
}

/**
 * @brief QueuePlayList::slot_changedQueueMenu [SLOT] 메뉴 아이콘 클릭시
 */
void QueuePlayList::slot_changedQueueSubMenu(){
    print_debug();
    if(this->mouse_trigger_menu_flag == false){
        emit linker->signal_checkQueue(7, "");
        print_debug();
        return;
    }

    qDebug() << "this->mouse_trigger_flag = " << this->mouse_trigger_flag;
    qDebug() << "this->preMenu = " << this->preMenu;
    qDebug() << "*this->currMenu = " << this->currMenu;


    // 모든 버튼 선택해제 스타일로 세팅
    this->btn_mov->setStyleSheet(btn_mov->styleSheet().replace("#B18658", "transparent"));//cheon210916-total
    this->btn_edit->setStyleSheet(btn_edit->styleSheet().replace("#B18658", "transparent"));

    // 선택버튼만 스타일 세팅
    ClickableLabel *tmp_btn = dynamic_cast<ClickableLabel*>(sender());
    tmp_btn->setStyleSheet(tmp_btn->styleSheet().replace("transparent", "#B18658"));
    QString tmp_currMenuNew = tmp_btn->property("currMenuCode").toString();

    qDebug() << "*tmp_currMenuNew = " << tmp_currMenuNew;

    global.queuelist_mouse_trigger_flag = false;
    this->mouse_trigger_flag = false;



}



/**
 * @brief QueuePlayList::slot_changedQueueMenu [SLOT] 메뉴 아이콘 클릭시
 */
void QueuePlayList::slot_changedQueueMenu(){
    print_debug();
    if(global.power_sleepMode_flag) {
        print_debug();
        emit linker->signal_checkQueue(24, "");
        return;
    }
    if(this->mouse_trigger_menu_flag == false){
        emit linker->signal_checkQueue(7, "");
        print_debug();
        return;
    }


    emit linker->signal_clickListBtn();
    pre_now_Section = global.now_Section;

    for(int i = 0; i < 7; i++){
        if(global.now_Section != tabSectionlist[i]){
            queuelist_change_replace_recovery(pre_now_Section);
        }

    }

    qDebug() << "this->mouse_trigger_flag = " << this->mouse_trigger_flag;
    qDebug() << "this->preMenu = " << this->preMenu;
    qDebug() << "*this->currMenu = " << this->currMenu;


    // 모든 버튼 선택해제 스타일로 세팅
    this->clb_total->setStyleSheet(clb_total->styleSheet().replace("#B18658", "transparent"));//cheon210916-total
    this->clb_music->setStyleSheet(clb_music->styleSheet().replace("#B18658", "transparent"));
    this->clb_video->setStyleSheet(clb_video->styleSheet().replace("#B18658", "transparent"));
    this->clb_radio->setStyleSheet(clb_radio->styleSheet().replace("#B18658", "transparent"));
    this->clb_fmtuner->setStyleSheet(clb_fmtuner->styleSheet().replace("#B18658", "transparent"));
    this->clb_roseTube->setStyleSheet(clb_roseTube->styleSheet().replace("#B18658", "transparent"));
    this->clb_potcast->setStyleSheet(clb_potcast->styleSheet().replace("#B18658", "transparent"));
    this->clb_tidal->setStyleSheet(clb_tidal->styleSheet().replace("#B18658", "transparent"));
    this->clb_bugs->setStyleSheet(clb_bugs->styleSheet().replace("#B18658", "transparent"));
    this->clb_qobuz->setStyleSheet(clb_qobuz->styleSheet().replace("#B18658", "transparent"));
    this->clb_apple->setStyleSheet(clb_apple->styleSheet().replace("#B18658", "transparent"));


    // 선택버튼만 스타일 세팅
    ClickableLabel *tmp_btn = dynamic_cast<ClickableLabel*>(sender());
    tmp_btn->setStyleSheet(tmp_btn->styleSheet().replace("transparent", "#B18658"));
    QString tmp_currMenuNew = tmp_btn->property("currMenuCode").toString();
    qDebug() << "slot_changedQueueMenu--tmp_currMenuNew = " << tmp_currMenuNew;
    if(this->currMenu!=tmp_currMenuNew){
        // 초기화
        //this->listWidget_total[global.now_Section]->clear();
        //this->jsonArray_backup_total[global.now_Section] = QJsonArray();
        this->jsonArray_backup_music = QJsonArray();
        this->jsonArray_backup_youtube = QJsonArray();
        this->jsonArray_backup_tidal = QJsonArray();
        this->jsonArray_backup_bugs = QJsonArray();
        this->jsonArray_backup_qobuz = QJsonArray();
        this->jsonArray_backup_apple = QJsonArray();
    }
    this->currMenu = tmp_currMenuNew;
    if(this->currMenu == CURRMENU_TOTAL){
        global.now_Section = 0;
    }else if(this->currMenu == CURRMENU_MUSIC){
        global.now_Section = 1;
    }else if(this->currMenu == CURRMENU_ROSETUBE){
        global.now_Section = 4;
    }else if(this->currMenu == CURRMENU_TIDAL){
        global.now_Section = 6;
    }else if(this->currMenu == CURRMENU_BUGS){
        global.now_Section = 7;
    }else if(this->currMenu == CURRMENU_QOBUZ){
        global.now_Section = 8;
    }else if(this->currMenu == CURRMENU_APPLE){
        emit linker->signal_checkQueue(29, "");
        global.now_Section = 9;
    }
    this->islistOpen = false;

    listWidget_total_setShow(global.now_Section);

    selected_menuSave = this->currMenu;
    if( this->preMenu == this->currMenu){
        print_debug();
        mouse_trigger_flag = true;
        global.queuelist_mouse_trigger_flag = true;

        return;
    }
    this->mouse_trigger_flag = false;
    global.queuelist_mouse_trigger_flag = false;

    queuelist_change_replace_recovery(global.now_Section);
    // 선택한 메뉴 Queue List 가져온다.

    showEvent_after_reGet_checkSum();


    //this->preMenu = this->currMenu;

}

/**
 * @brief QueuePlayList::mousePressEvent [override] 삭제하면 안됨.. 최상위 부모의 mousePressEvent 버블링 막기 위해 오버라이딩만 하였음. 최상위 부모가 큐목록을 hide 하기 위함.
 * @param event
 * @note 삭제하면 안됨...
 */
void QueuePlayList::mousePressEvent(QMouseEvent *event){
    Q_UNUSED(event);

}

/**
 * @brief QueuePlayList::showEvent [override] 현재 Queue List 가져오기
 * @param event
 */
void QueuePlayList::showEvent(QShowEvent *event){
    Q_UNUSED(event);
    print_debug();
    qDebug() << "pre******************mouse_trigger_flag = " << mouse_trigger_flag;
    qDebug() << "******************mouse_trigger_menu_flag = " << mouse_trigger_menu_flag;
    qDebug() << "******************global.queuelist_mouse_trigger_flag = " << global.queuelist_mouse_trigger_flag;
    qDebug() << "******************global.queuelist_mouse_trigger_menu_flag = " << global.queuelist_mouse_trigger_menu_flag;

    qDebug() << "global.signal_selectedDevice_flag = " << global.signal_selectedDevice_flag;

    qDebug() << "this->islistOpen = " << this->islistOpen;
    emit linker->signal_checkQueue(17, "");

    if(global.signal_selectedDevice_flag){

        this->islistOpen = true;

    }
    //if(!mouse_trigger_menu_flag) {
    //    print_debug();

    //    emit linker->signal_checkQueue(6, "");
    //    return;
    //}
    //global.Queue_track_count = 0;//c220826_2
    qDebug() << "this->islistOpen = " << this->islistOpen;
    if(this->pre_checkSum != global.checkSum){
        equelRecieveJsonFlag[global.now_Section] = false;
        this->pre_checkSum = global.checkSum;
    }else{
        equelRecieveJsonFlag[global.now_Section] = true;
    }
    if(this->islistOpen == true)// && queue_init_flag == false)
    {
        print_debug();
        global.islistOpenFlag = this->islistOpen;
        //linker = Linker::getInstance();

        //if(global.signal_selectedDevice_flag){//backupPixmapArrayTotal
        global.now_Section = 0;

        //if(this->preMenu != this->currMenu)
        for(int i = 0 ; i < 7; i++){
            this->jsonArray_backup_total[tabSectionlist[i]] = QJsonArray();
            this->jsonArray_backup_total_recomm[tabSectionlist[i]] = QJsonArray();
            this->lb_queueListImg[tabSectionlist[i]].clear();
            this->indexCurrPlay[tabSectionlist[i]] = -1;
            this->indexPrePlay[tabSectionlist[i]] = -1;
            this->listWidget_total[tabSectionlist[i]]->clear();
            this->widget[tabSectionlist[i]].clear();
            this->list_dataTrackForDelegate[tabSectionlist[i]].clear();
        }

        menu_init();


        this->islistOpen = false;
        print_debug();
    }
    this->changeEditMode(true, false, false);
    print_debug();
    qDebug() << "---global.now_Section = " << global.now_Section;
    this->PreViewRow = 0; this->CurrViewRow = 0;


    qDebug() << "---equelRecieveJsonFlag[global.now_Section] = " << equelRecieveJsonFlag[global.now_Section];
    emit linker->signal_clickListBtn();
    qDebug() << "this->preMenu =" << this->preMenu;
    qDebug() << "this->currMenu =" << this->currMenu;
    if( force_play_mode || listWidget_total[global.now_Section]->count() == 0 || !equelRecieveJsonFlag[global.now_Section]){
        print_debug();

        force_play_mode = false;

        emit linker->signal_checkQueue(6, "");
        global.queuelist_mouse_trigger_menu_flag = false;//c221004_2
        QJsonObject tmp_json;//cheon211114-01
        NetworkHttp *network_get = new NetworkHttp;//cheon211114-01
        connect(network_get, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_get(int, QJsonObject)));//cheon211114-01
        network_get->request(HTTP_CURR_TOTAL_QUEUE_GET, QString("http://%1:%2/%3")//cheon211104-01
                             .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_GET), tmp_json, false, true);//cheon211114-01

    }else if(listWidget_total[global.now_Section]->count() != 0 && equelRecieveJsonFlag[global.now_Section]){
        print_debug();
        emit linker->signal_checkQueue(6, "");
        global.queuelist_mouse_trigger_menu_flag = false;//c221004_2
        QJsonObject tmp_json;//cheon211114-01
        NetworkHttp *network_get = new NetworkHttp;//cheon211114-01
        connect(network_get, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_get(int, QJsonObject)));//cheon211114-01
        network_get->request(HTTP_CURR_TOTAL_QUEUE_CURRPLAYINDEX_GET_CHECK, QString("http://%1:%2/%3")//cheon211104-01
                             .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_GET), tmp_json, false, true);//cheon211114-01


    }else{
        queueReflash_checking(currMenutoPid());
        emit linker->signal_checkQueue(6, "");
        print_debug();
        mouse_all_clear();
    }
    this->preMenu = this->currMenu;

    print_debug();
}

void QueuePlayList::menu_init(){//c220629

    // backupPixmapArrayTotal.clear();//c220527-1
    qDebug() << "***************this->mouse_trigger_flag = " << this->mouse_trigger_menu_flag;
    //global.queuelist_mouse_trigger_flag = true;
    //global.queuelist_mouse_trigger_menu_flag = true;
    global.signal_selectedDevice_flag = false;
    listWidget_total_setShow(global.now_Section);
    //}

    // 모든 버튼 선택해제 스타일로 세팅
    this->clb_total->setStyleSheet(clb_total->styleSheet().replace("#B18658", "transparent"));//cheon210916-total
    this->clb_music->setStyleSheet(clb_music->styleSheet().replace("#B18658", "transparent"));
    this->clb_video->setStyleSheet(clb_video->styleSheet().replace("#B18658", "transparent"));
    this->clb_radio->setStyleSheet(clb_radio->styleSheet().replace("#B18658", "transparent"));
    this->clb_fmtuner->setStyleSheet(clb_fmtuner->styleSheet().replace("#B18658", "transparent"));
    this->clb_roseTube->setStyleSheet(clb_roseTube->styleSheet().replace("#B18658", "transparent"));
    this->clb_potcast->setStyleSheet(clb_potcast->styleSheet().replace("#B18658", "transparent"));
    this->clb_tidal->setStyleSheet(clb_tidal->styleSheet().replace("#B18658", "transparent"));
    this->clb_bugs->setStyleSheet(clb_bugs->styleSheet().replace("#B18658", "transparent"));
    this->clb_qobuz->setStyleSheet(clb_qobuz->styleSheet().replace("#B18658", "transparent"));
    this->clb_apple->setStyleSheet(clb_apple->styleSheet().replace("#B18658", "transparent"));
    qDebug() << "-global.now_Section = " << global.now_Section;
    QString tmp_currMenuNew;
    //global.now_Section = 0;//cheon211120-02

    switch(global.now_Section)
    {
    case 0://cheon210916-total
        this->currMenu = CURRMENU_TOTAL;

        this->clb_total->setStyleSheet(this->clb_total->styleSheet().replace("transparent", "#B18658"));
        tmp_currMenuNew = this->clb_total->property("currMenuCode").toString();
        break;
    case 1:
        this->currMenu = CURRMENU_MUSIC;
        this->clb_music->setStyleSheet(this->clb_music->styleSheet().replace("transparent", "#B18658"));
        tmp_currMenuNew = this->clb_music->property("currMenuCode").toString();
        break;

    case 4:
        this->currMenu = CURRMENU_ROSETUBE;
        this->clb_roseTube->setStyleSheet(this->clb_roseTube->styleSheet().replace("transparent", "#B18658"));
        tmp_currMenuNew = this->clb_roseTube->property("currMenuCode").toString();
        break;

    case 6:
        this->currMenu = CURRMENU_TIDAL;
        this->clb_tidal->setStyleSheet(this->clb_tidal->styleSheet().replace("transparent", "#B18658"));
        tmp_currMenuNew = this->clb_tidal->property("currMenuCode").toString();
        break;
    case 7:
        this->currMenu = CURRMENU_BUGS;
        this->clb_bugs->setStyleSheet(this->clb_bugs->styleSheet().replace("transparent", "#B18658"));
        tmp_currMenuNew = this->clb_bugs->property("currMenuCode").toString();
        break;
    case 8:
        this->currMenu = CURRMENU_QOBUZ;
        this->clb_qobuz->setStyleSheet(this->clb_qobuz->styleSheet().replace("transparent", "#B18658"));
        tmp_currMenuNew = this->clb_qobuz->property("currMenuCode").toString();
        break;
    case 9:

        this->currMenu = CURRMENU_APPLE;
        this->clb_apple->setStyleSheet(this->clb_apple->styleSheet().replace("transparent", "#B18658"));
        tmp_currMenuNew = this->clb_apple->property("currMenuCode").toString();
        break;
    default:
        this->currMenu = CURRMENU_TOTAL;
        this->clb_total->setStyleSheet(this->clb_total->styleSheet().replace("transparent", "#B18658"));
        tmp_currMenuNew = this->clb_total->property("currMenuCode").toString();
        break;
    }
    /*
    for(int i = 0; i < this->list_thumbPath[global.now_Section].count(); i++){
        QObject::disconnect(disConId[i]);
    }
    for(int i = 0; i < this->list_thumbPath_recomm[global.now_Section].count(); i++){
        QObject::disconnect(disConId_recomm[i]);
    }
    */
    // 선택버튼만 스타일 세팅
    if(this->currMenu!=tmp_currMenuNew){
        // 초기화
        //this->jsonArray_backup_total[global.now_Section] = QJsonArray();
        this->jsonArray_backup_music = QJsonArray();
        this->jsonArray_backup_youtube = QJsonArray();
        this->jsonArray_backup_tidal = QJsonArray();
        this->jsonArray_backup_bugs = QJsonArray();
        this->jsonArray_backup_qobuz = QJsonArray();
        this->jsonArray_backup_apple = QJsonArray();

        //this->jsonArray_backup_total_recomm[global.now_Section] = QJsonArray();
        this->jsonArray_backup_music_recomm = QJsonArray();
        this->jsonArray_backup_youtube_recomm = QJsonArray();
        this->jsonArray_backup_tidal_recomm = QJsonArray();
        this->jsonArray_backup_bugs_recomm = QJsonArray();
        this->jsonArray_backup_qobuz_recomm = QJsonArray();
        this->jsonArray_backup_apple_recomm = QJsonArray();
    }

}

void QueuePlayList::wheelEvent(QWheelEvent *event){//c221004_1
    print_debug();

    //qDebug() << "event->angleDelta()" << event->angleDelta();

    //QScrollBar *vb1 = listWidget_total[global.now_Section]->verticalScrollBar();//cheon211114-01

    //int i = vb1->value();//cheon211114-01
    //qDebug() << "------++++++++" << listWidget_total[global.now_Section]->currentRow();

    qDebug() << "this->listWidget_total[global.now_Section]->verticalScrollBar()->sliderPosition()= " << this->listWidget_total[global.now_Section]->verticalScrollBar()->sliderPosition();

#if defined(Q_OS_WINDOWS)

    //slot_listwidgetcurrentRowChanged(this->listWidget_total[global.now_Section]->verticalScrollBar()->sliderPosition());  //c221004_1

#endif
#if defined(Q_OS_MAC)

#endif
}

void QueuePlayList::slot_queuelistPositionGet_http_signal_singhot(){
    qDebug() << "mouse_trigger_menu_flag = " << mouse_trigger_menu_flag;

    this->http_signal_flag = true;
    if(mouse_trigger_menu_flag && isPlayMode && global.queueTimerFlag ){
        if(!global.device.getDeviceIP().isEmpty()){

            print_debug();
            /*
            global.queue_playFlag = false;
            global.queue_recommPlayFlag = false;
            global.queue_music_menu_startFlag = false;
            global.Queue_track_count = 0;
            //mouse_trigger_menu_flag = false;
            */

            qDebug() << "this->nextPlayNo = " << this->nextPlayNo;
            qDebug() << "this->indexCurrPlay[global.now_Section] = " << this->indexCurrPlay[global.now_Section];
            //emit linker->signal_checkQueue(8, "");
            //this->currMenu = selected_menuSave;

            // this->showEvent(nullptr);

            QJsonObject tmp_json;
            NetworkHttp *network_get = new NetworkHttp;

            connect(network_get, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_get(int, QJsonObject)));//cheon211114-01

            network_get->request(HTTP_CURR_TOTAL_QUEUE_POSITION_GET, QString("http://%1:%2/%3")//cheon211104-01
                                 .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_GET), tmp_json, false, true);//cheon211114-01


        }
    }

}

void QueuePlayList::slot_queuelistPositionGet_http_signal(){//1126//music start call
    print_debug();
    QTimer::singleShot(1000, this, SLOT(slot_queuelistPositionGet_http_signal_singhot()));
}

void QueuePlayList::slot_queuelistPositionGet_check(){//1126//music start call

    QTimer::singleShot(100, this, SLOT(slot_checking_currPlay()));

}

void QueuePlayList::slot_queuelistPositionGet(){//1126//music start call
    print_debug();
    if(mouse_trigger_flag && isPlayMode && global.queueTimerFlag ){
        if(!global.device.getDeviceIP().isEmpty()){

            print_debug();
            qDebug() << "this->nextPlayNo = " << this->nextPlayNo;
            qDebug() << "this->indexCurrPlay[global.now_Section] = " << this->indexCurrPlay[global.now_Section];
            //emit linker->signal_checkQueue(8, "");
            //this->currMenu = selected_menuSave;
            QJsonObject tmp_json;
            NetworkHttp *network_get = new NetworkHttp;

            connect(network_get, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_get(int, QJsonObject)));//cheon211114-01

            network_get->request(HTTP_CURR_TOTAL_QUEUE_CURRPLAYINDEX_GET, QString("http://%1:%2/%3")//cheon211104-01
                                 .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_GET), tmp_json, false, true);//cheon211114-01

        }
    }
}


void QueuePlayList::slot_queuelistCurrPlayIndexGet(){//c220629
    print_debug();//if(global.queueTimerFlag
    if(!global.power_sleepMode_flag && isPlayMode && global.queueTimerFlag){
        emit linker->signal_checkQueue(6, "");
        emit linker->signal_checkQueue(17, "");
        force_reflash(currMenutoPid());
        emit linker->signal_checkQueue(6, "");
    }
    /*
    if(!global.power_sleepMode_flag && isPlayMode && global.queueTimerFlag){
        qDebug() << "mouse_trigger_flag = " << mouse_trigger_flag;
        qDebug() << "mouse_trigger_menu_flag = " << mouse_trigger_menu_flag;
        qDebug() << "global.queuelist_mouse_trigger_menu_flag= " << global.queuelist_mouse_trigger_menu_flag;
        if(mouse_trigger_flag ){
            slot_queuelistScrollbarMotify(0);

        }
    }
*/

}

void QueuePlayList::slot_queuelistMotify(){//1126

    if(!global.power_sleepMode_flag && isPlayMode && global.queueTimerFlag){
        if(!global.device.getDeviceIP().isEmpty()){
            print_debug();


            QJsonObject tmp_json;//cheon211114-01
            NetworkHttp *network_get = new NetworkHttp;//cheon211114-01
            connect(network_get, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_get(int, QJsonObject)));//cheon211114-01
            network_get->request(HTTP_CURR_TOTAL_QUEUE_GET, QString("http://%1:%2/%3")//cheon211104-01
                                 .arg(global.device.getDeviceIP()).arg(global.port).arg(REMOTE_TOTAL_QUEUE_GET), tmp_json, false, true);//cheon211114-01


        }
    }else{
        print_debug();
    }
}

/**
 * @brief QueuePlayList::reloadList 재생목록 리로드
 */
void QueuePlayList::reloadList(){//cheon211114-01
    print_debug();
    emit linker->signal_checkQueue(17, "");

    //if(!global.power_sleepMode_flag && isPlayMode && global.queueTimerFlag){
    if(!global.power_sleepMode_flag  && global.queueTimerFlag){
        if(!global.device.getDeviceIP().isEmpty() ){


            if(!mouse_trigger_menu_flag) {
                print_debug();

                //emit linker->signal_checkQueue(6, "");
                return;
            }
            //mouse_all_lock();

            qDebug() << "global.now_Section = " << global.now_Section;


            switch(global.now_Section)
            {
            case 0://cheon210916-total
                this->currMenu = CURRMENU_TOTAL;
                this->clb_total->setStyleSheet(this->clb_total->styleSheet().replace("transparent", "#B18658"));
                break;
            case 1:
                this->currMenu = CURRMENU_MUSIC;
                this->clb_music->setStyleSheet(this->clb_music->styleSheet().replace("transparent", "#B18658"));
                break;

            case 4:
                this->currMenu = CURRMENU_ROSETUBE;
                this->clb_roseTube->setStyleSheet(this->clb_roseTube->styleSheet().replace("transparent", "#B18658"));
                break;

            case 6:
                this->currMenu = CURRMENU_TIDAL;
                this->clb_tidal->setStyleSheet(this->clb_tidal->styleSheet().replace("transparent", "#B18658"));
                break;
            case 7:
                this->currMenu = CURRMENU_BUGS;
                this->clb_bugs->setStyleSheet(this->clb_bugs->styleSheet().replace("transparent", "#B18658"));
                break;
            case 8:
                this->currMenu = CURRMENU_QOBUZ;
                this->clb_qobuz->setStyleSheet(this->clb_qobuz->styleSheet().replace("transparent", "#B18658"));
                break;
            case 9:
                this->currMenu = CURRMENU_APPLE;
                this->clb_apple->setStyleSheet(this->clb_apple->styleSheet().replace("transparent", "#B18658"));
                break;
            default:
                this->currMenu = CURRMENU_MUSIC;
                this->clb_music->setStyleSheet(this->clb_music->styleSheet().replace("transparent", "#B18658"));
                break;
            }

            //selected_menuSave = this->currMenu;
            /*
        for(int i = 0; i < this->list_thumbPath[global.now_Section].count(); i++){
            QObject::disconnect(disConId[i]);
        }
        for(int i = 0; i < this->list_thumbPath_recomm[global.now_Section].count(); i++){
            QObject::disconnect(disConId_recomm[i]);
        }
        */


            print_debug();
            qDebug() << "********** mouse_trigger_menu_flag== "<< mouse_trigger_menu_flag;
            //if(this->isEditMode == true){
            //this->changeEditMode(this->isEditMode);
            //    return;
            //}
            global.queuelist_mouse_trigger_menu_flag = false;//c221004_2
            NetworkHttp *network = new NetworkHttp;//cheon211114-01
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));//cheon211114-01
            QJsonObject tmp_json;//cheon211114-01
            // 현재 선택한 메뉴만 Queue List 가져온다.
            if(this->currMenu == CURRMENU_TOTAL){//cheon211114-01
                //requestRecom_check(HTTP_CURR_QUEUE_GET_TOTAL);
                print_debug();
                //QPixmapCache::clear();
                network->request(HTTP_CURR_QUEUE_GET_TOTAL, QString("http://%1:%2/%3")//cheon211104-01
                                 .arg(global.device.getDeviceIP()).arg(9285).arg(TOTAL_QUEUE_GET_FILE), tmp_json, false, true);//cheon211114-01

            }else if(this->currMenu == CURRMENU_MUSIC){
                //requestRecom_check(HTTP_CURR_QUEUE_GET_MUSIC);
                print_debug();
                network->request(HTTP_CURR_QUEUE_GET_MUSIC, QString("http://%1:%2/%3")//cheon210916-total
                                 .arg(global.device.getDeviceIP()).arg(9285).arg(TOTAL_QUEUE_GET_FILE), tmp_json, false, true);

            }else if(this->currMenu==CURRMENU_ROSETUBE){
                //requestRecom_check(HTTP_CURR_QUEUE_GET_ROSETUBE);
                print_debug();
                network->request(HTTP_CURR_QUEUE_GET_ROSETUBE, QString("http://%1:%2/%3")//cheon210916-total
                                 .arg(global.device.getDeviceIP()).arg(9285).arg(TOTAL_QUEUE_GET_FILE), tmp_json, false, true);

            }else if(this->currMenu==CURRMENU_TIDAL){
                //requestRecom_check(HTTP_CURR_QUEUE_GET_TIDAL);
                print_debug();
                qDebug()<< "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
                network->request(HTTP_CURR_QUEUE_GET_TIDAL, QString("http://%1:%2/%3")//cheon210916-total
                                 .arg(global.device.getDeviceIP()).arg(9285).arg(TOTAL_QUEUE_GET_FILE), tmp_json, false, true);

            }else if(this->currMenu==CURRMENU_BUGS){
                //requestRecom_check(HTTP_CURR_QUEUE_GET_BUGS);
                print_debug();
                network->request(HTTP_CURR_QUEUE_GET_BUGS, QString("http://%1:%2/%3")//cheon210916-total
                                 .arg(global.device.getDeviceIP()).arg(9285).arg(TOTAL_QUEUE_GET_FILE), tmp_json, false, true);


            }else if(this->currMenu==CURRMENU_QOBUZ){//REMOTE_TOTAL_QUEUE_GET
                //requestRecom_check(HTTP_CURR_QUEUE_GET_QOBUZ);
                print_debug();
                network->request(HTTP_CURR_QUEUE_GET_QOBUZ, QString("http://%1:%2/%3")//cheon210916-total
                                 .arg(global.device.getDeviceIP()).arg(9285).arg(TOTAL_QUEUE_GET_FILE), tmp_json, false, true);


            }else if(this->currMenu==CURRMENU_APPLE){//REMOTE_TOTAL_QUEUE_GET
                //requestRecom_check(HTTP_CURR_QUEUE_GET_APPLEMUSIC);
                print_debug();
                network->request(HTTP_CURR_QUEUE_GET_APPLEMUSIC, QString("http://%1:%2/%3")//cheon210916-total
                                 .arg(global.device.getDeviceIP()).arg(9285).arg(TOTAL_QUEUE_GET_FILE), tmp_json, false, true);


            }

        }
    }else{
        print_debug();
    }
    this->preMenu = this->currMenu;
}

