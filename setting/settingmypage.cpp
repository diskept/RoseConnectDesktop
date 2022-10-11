#include "setting/settingmypage.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/sqlitehelper.h"

#include "login/dialoglogin.h"
#include "widget/dialogconfirm.h"
#include "widget/dialogconfirm_3button.h"
#include "widget/framemyplaylist.h"
#include "widget/framerosetubeplaylist.h"
#include "widget/myphotoedit.h"

#include "widget/toastmsg.h"

#include <QScrollArea>
#include <QPainter>
#include <QScroller>
#include <QFileDialog>
#include <QDir>
#include <QHttpMultiPart>
#include <QDialogButtonBox>


const int HTTP_GET_MYINFO = 99;
const int HTTP_GET_MYINFO_AfterMemberImageUploaded = 101;
const int HTTP_GET_MYINFO_AfterProfileImageUploaded = 102;
const int HTTP_CHECK_DUPLICATE_NICKNAME = 98;
const int HTTP_PUT_DELETE_PHOTO = 97;
const int HTTP_PUT_UPLOAD_PHOTO = 110;
const int HTTP_SAVE_MYINFO = 96;
const int HTTP_MY_PLYALIST = 88;
const int HTTP_ROSETUBE_PLAYLIST = 77;
const int HTTP_SET_ROSE_USER_INFO = 66;
const int HTTP_USER_DELETE = 55;

const int PROFILEIMG_SIZE = 270;
const int PHOTOS_SIZE = 120;
/**
 * @brief SettingMyPage::SettingMyPage 설정 > 마이페이지 생성자
 * @param parent
 */
SettingMyPage::SettingMyPage(QWidget *parent) : QWidget(parent)
{

    linker = Linker::getInstance();
    connect(linker, SIGNAL(signal_profileImage_delete()), SLOT(slot_getMyInfo_profileImageDelete()));
    connect(linker, SIGNAL(signal_logined()), SLOT(slot_getMyInfo_loginAfter()));
    connect(linker, SIGNAL(signal_myPlaylistChanged()), SLOT(slot_myPlaylistChanged()));
    connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));

    this->setUIControl();
}


SettingMyPage::~SettingMyPage(){

    this->deleteLater();
}


/**
 * @brief SettingMyPage::setUIControl 기본 UI 세팅
 */
void SettingMyPage::setUIControl()
{

    widget_loginBefore = this->setUIControl_loginBefore();
    widget_loginAfter = this->setUIControl_loginAfter();

    // 전체
    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(widget_loginBefore);
    vl_total->addWidget(widget_loginAfter);

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
 * @brief SettingMyPage::setUIControl_noLogin 미로그인 상태의 화면
 * @return3
 */
QWidget* SettingMyPage::setUIControl_loginBefore(){

    QLabel *lb_text = new QLabel();
    lb_text->setText(tr("You can use various services through Login."));
    //lb_text->setText(tr("로그인하시면 다양한 서비스를 이용하실 수 있습니다."));
    lb_text->setStyleSheet("color:#CCCCCC;font-size:20px;");

    QPushButton *btn_login = new QPushButton();
    btn_login->setText(tr("Log-in"));
    //btn_login->setText(tr("로그인"));
    btn_login->setCursor(Qt::PointingHandCursor);
    btn_login->setStyleSheet("color:#CCCCCC;font-size:20px;border:3px solid #CCCCCC;border-radius:25;");
    btn_login->setFixedSize(230,55);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignCenter);
    vl_total->addWidget(lb_text);
    vl_total->addSpacing(30);
    vl_total->addWidget(btn_login,0,Qt::AlignCenter);

    QWidget *widget_loginBefore = new QWidget();
    widget_loginBefore->setObjectName("widget_loginBefore");
    widget_loginBefore->setFixedHeight(500);
    widget_loginBefore->setStyleSheet("#widget_loginBefore { background:qlineargradient(x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #222222, stop:1 #464646); }");
    widget_loginBefore->setLayout(vl_total);

    // 커넥션
    connect(btn_login, &QPushButton::clicked, this, &SettingMyPage::slot_showLoginPage);

    return widget_loginBefore;
}


/**
 * @brief SettingMyPage::setUIControl_Login 로그인된 상태의 화면 (마이페이지 View / 마이페이지 Edit)
 * @return
 */
QWidget* SettingMyPage::setUIControl_loginAfter(){

    this->stackWidget_loginAfter = new QStackedWidget();
    this->stackWidget_loginAfter->addWidget(setUIControl_loginAfter_myInfoView());  // View
    this->stackWidget_loginAfter->addWidget(setUIControl_loginAfter_myInfoEdit());  // Edit

    QVBoxLayout *vl_total_stackWidget = new QVBoxLayout();
    vl_total_stackWidget->setContentsMargins(0,0,0,0);
    vl_total_stackWidget->setSpacing(0);
    vl_total_stackWidget->setAlignment(Qt::AlignTop);
    vl_total_stackWidget->addWidget(this->stackWidget_loginAfter);

    QWidget *widget_loginAfter = new QWidget();
    widget_loginAfter->setObjectName("widget_loginAfter");
    widget_loginAfter->setLayout(vl_total_stackWidget);
    //widget_loginAfter->setStyleSheet("#widget_loginAfter { border:2px solid yellow }");
    return widget_loginAfter;
}


/**
 * @brief SettingMyPage::setUIControl_loginAfter_myInfoView 마이페이지 View 화면 구성
 * @return
 */
QWidget* SettingMyPage::setUIControl_loginAfter_myInfoView(){

    // 내 프로필
    lb_profile_grade = new QLabel();
    lb_profile = new QLabel();
    lb_profile_grade->setFixedSize(PROFILEIMG_SIZE,PROFILEIMG_SIZE);
    lb_profile->setFixedSize(PROFILEIMG_SIZE,PROFILEIMG_SIZE);
    lb_profile_grade->raise();

    lb_nickName = new QLabel();
    lb_nickName->setStyleSheet("color:#FFFFFF;font-size:36px;");
    lb_comment = new QLabel();
    lb_comment->setWordWrap(true);
    lb_comment->setStyleSheet("color:#CCCCCC;font-size:20px;");
    lb_nickName->setContentsMargins(25,0,0,0);
    lb_comment->setContentsMargins(25,0,0,0);

    lb_email = new QLabel();
    lb_email->setContentsMargins(25,0,0,0);
    lb_email->setAlignment(Qt::AlignCenter);
    lb_email->setStyleSheet("color:#FFFFFF;font-size:20px;");


    QPushButton *btn_edit = new QPushButton();
    btn_edit->setProperty("goPageIndex", 1);
    btn_edit->setText(tr("Edit"));
    //btn_edit->setText(tr("편집"));
    btn_edit->setFixedSize(100,40);
    btn_edit->setStyleSheet("color:#E6E6E6;font-size:18px;border:1px solid #666666;border-radius:20px;");
    btn_edit->setCursor(Qt::PointingHandCursor);
    QPushButton *btn_logout = new QPushButton();
    btn_logout->setText(tr("Log-out"));
    //btn_logout->setText(tr("로그아웃"));
    btn_logout->setFixedSize(100,40);
    btn_logout->setStyleSheet("color:#E6E6E6;font-size:18px;border:1px solid #666666;border-radius:20px;");
    btn_logout->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_btn = new QHBoxLayout();
    hl_btn->addWidget(btn_edit);
    hl_btn->addSpacing(25);
    hl_btn->addWidget(btn_logout);
    hl_btn->addSpacing(47);

    QGridLayout *gl_myInfo = new QGridLayout();
    gl_myInfo->setContentsMargins(0,0,0,0);
    gl_myInfo->setSpacing(0);
    gl_myInfo->addWidget(lb_profile, 0, 0, 4, 1);

    gl_myInfo->addWidget(lb_profile_grade, 0, 0, 4, 1);
    gl_myInfo->addWidget(lb_nickName, 1, 1);
    gl_myInfo->addWidget(lb_comment, 2, 1, 1, 2);
    gl_myInfo->addWidget(lb_email, 3, 1);

    gl_myInfo->addLayout(hl_btn, 0, 1, 1, 2, Qt::AlignRight);

    // 내 사진 리스트
    hl_photo = new QHBoxLayout();
    hl_photo->setContentsMargins(0,0,0,0);
    hl_photo->setSpacing(18);
    hl_photo->setAlignment(Qt::AlignLeft);

    QVBoxLayout *vl_myInfo = new QVBoxLayout();
    vl_myInfo->setContentsMargins(0,0,0,0);
    vl_myInfo->setSpacing(0);
    vl_myInfo->addLayout(gl_myInfo);
    vl_myInfo->addSpacing(37);
    vl_myInfo->addLayout(hl_photo);

    QWidget *widget_myInfoView = new QWidget();
    widget_myInfoView->setContentsMargins(40,30,30,40);
    widget_myInfoView->setObjectName("widget_myInfoView");
    widget_myInfoView->setStyleSheet("#widget_myInfoView { background:qlineargradient(x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #222222, stop:1 #464646); }");
    widget_myInfoView->setLayout(vl_myInfo);
    //widget_myInfoView->setFixedHeight(490);

    // 음악 플레이리스트
    QWidget *widget_playListMusic = this->setUIControl_playListMusic();

    // 로즈튜브 플레이리스트
    QWidget *widget_playListRoseTube = this->setUIControl_playListRoseTube();

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(widget_myInfoView);         // 내 정보
    vl_total->addSpacing(40);
    vl_total->addWidget(widget_playListMusic);      // 음악 플레이리스트
    vl_total->addSpacing(30);
    vl_total->addWidget(widget_playListRoseTube, 9);    // 로즈튜브 플레이리스트

    QWidget *widget = new QWidget();
    //widget->setStyleSheet("background-color:blue");
    widget->setLayout(vl_total);

    // 커넥션
    connect(btn_edit, &QPushButton::clicked, this, &SettingMyPage::slot_changePageEdit);
    connect(btn_logout, &QPushButton::clicked, this, &SettingMyPage::slot_logout);

    return widget;
}


/**
 * @brief SettingMyPage::setUIControl_playListMusic 마이페이지 View 화면의 음악 플레이리스트 UI 구성
 * @return
 */
QWidget* SettingMyPage::setUIControl_playListMusic(){

    QLabel *labelSubTitle_1 = new QLabel;
    labelSubTitle_1->setText(tr("Music Playlist"));
    //labelSubTitle_1->setText(tr("음악 플레이리스트"));
    labelSubTitle_1->setStyleSheet("font-size:20px;color:#FFFFFF;margin-left:10px;");

    QPushButton *btnViewAll_1 = new QPushButton;
    btnViewAll_1->setText(tr("View All"));
    btnViewAll_1->setStyleSheet("QPushButton{background:transparent;color:#CCCCCC;font-size:20px;}QPushButton:hover{color:#B18658;}");
    btnViewAll_1->setCursor(Qt::PointingHandCursor);
    btnViewAll_1->setProperty(KEY_VIEWALL_MODE.toStdString().c_str(), PAGECODE_VA_MYPLAYLIST);
    connect(btnViewAll_1, SIGNAL(clicked()), this, SLOT(slot_clickedViewAll_music()));

    QHBoxLayout *boxTop = new QHBoxLayout;
    boxTop->setSpacing(0);
    boxTop->setContentsMargins(0,0,28,0);
    boxTop->setAlignment(Qt::AlignTop);
    boxTop->addWidget(labelSubTitle_1);
    boxTop->addStretch();
    boxTop->addWidget(btnViewAll_1);

    QWidget *widget_top = new QWidget;
    widget_top->setLayout(boxTop);

    boxMyPlayList_music = new QHBoxLayout;
    boxMyPlayList_music->setSpacing(20);
    boxMyPlayList_music->setContentsMargins(10,10,0,0);
    boxMyPlayList_music->setSizeConstraint(QLayout::SetFixedSize);

    QWidget *widgetAlbum = new QWidget;
    widgetAlbum->setLayout(boxMyPlayList_music);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(widgetAlbum);
    scrollArea->setWidgetResizable(false);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("margin-bottom:0px;");

    QScroller::grabGesture(scrollArea, QScroller::LeftMouseButtonGesture);

    lb_emptyMyPlayList_music = new QLabel;
    lb_emptyMyPlayList_music->setText(tr("There is no Music Playlist."));
    //lb_emptyMyPlayList_music->setText(tr("음악 플레이 리스트가 없습니다."));
    lb_emptyMyPlayList_music->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lb_emptyMyPlayList_music->setAlignment(Qt::AlignCenter);
    lb_emptyMyPlayList_music->setStyleSheet("font-size:18px;color:#FFFFFF;");
    lb_emptyMyPlayList_music->setFixedHeight(286);

    stackMyPlayList_music = new QStackedWidget;
    //stackMyPlayList->addWidget(wg_login);
    stackMyPlayList_music->addWidget(lb_emptyMyPlayList_music);
    stackMyPlayList_music->addWidget(scrollArea);

    QVBoxLayout *boxTotal = new QVBoxLayout;
    boxTotal->setContentsMargins(30,0,0,0);
    boxTotal->setSpacing(0);
    boxTotal->addWidget(widget_top);
    boxTotal->addWidget(stackMyPlayList_music);

    QWidget *wg_playList = new QWidget;
    wg_playList->setLayout(boxTotal);
    wg_playList->setStyleSheet("border:0px;");

    return wg_playList;
}


/**
 * @brief SettingMyPage::setUIControl_playListRoseTube 마이페이지 View 화면의 로즈튜브 플레이리스트 UI 구성
 * @return
 */
QWidget* SettingMyPage::setUIControl_playListRoseTube(){

    QLabel *labelSubTitle_1 = new QLabel;
    labelSubTitle_1->setText(tr("RoseTube Playlist"));
    //labelSubTitle_1->setText(tr("로즈튜브 플레이리스트"));
    labelSubTitle_1->setStyleSheet("font-size:20px;color:#FFFFFF;margin-left:10px;");

    QPushButton *btnViewAll_1 = new QPushButton;
    btnViewAll_1->setText(tr("View All"));
    btnViewAll_1->setStyleSheet("QPushButton{background:transparent;color:#CCCCCC;font-size:20px;}QPushButton:hover{color:#B18658;}");
    btnViewAll_1->setCursor(Qt::PointingHandCursor);
    btnViewAll_1->setProperty("index",0);
    btnViewAll_1->setProperty("type","myPlayList");
    btnViewAll_1->setProperty("title", tr("RoseTube Playlist"));
    //btnViewAll_1->setProperty("title", tr("로즈튜브 플레이리스트"));
    btnViewAll_1->setProperty(KEY_VIEWALL_MODE.toStdString().c_str(), PAGECODE_VA_HOME);

    connect(btnViewAll_1, SIGNAL(clicked()), this, SLOT(slot_clickedViewAll_rosetube()));

    QHBoxLayout *boxTop = new QHBoxLayout;
    boxTop->setSpacing(0);
    boxTop->setContentsMargins(0,0,28,0);
    boxTop->setAlignment(Qt::AlignTop);
    boxTop->addWidget(labelSubTitle_1);
    boxTop->addStretch();
    boxTop->addWidget(btnViewAll_1);

    QWidget *widget_top = new QWidget;
    widget_top->setLayout(boxTop);

    boxMyPlay_rosetube = new QHBoxLayout;
    boxMyPlay_rosetube->setSpacing(0);
    boxMyPlay_rosetube->setContentsMargins(0,0,0,0);
    boxMyPlay_rosetube->setSizeConstraint(QLayout::SetFixedSize);

    QWidget *widgetAlbum = new QWidget;
    widgetAlbum->setLayout(boxMyPlay_rosetube);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(widgetAlbum);
    scrollArea->setWidgetResizable(false);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("margin-bottom:0px;");

    QScroller::grabGesture(scrollArea, QScroller::LeftMouseButtonGesture);

    lb_emptyMyPlay_rosetube = new QLabel;
    lb_emptyMyPlay_rosetube->setText(tr("There is no RoseTube Playlist."));
    //lb_emptyMyPlay_rosetube->setText(tr("로즈튜브 플레이리스트가 없습니다."));
    lb_emptyMyPlay_rosetube->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lb_emptyMyPlay_rosetube->setAlignment(Qt::AlignCenter);
    lb_emptyMyPlay_rosetube->setStyleSheet("font-size:18px;color:#FFFFFF;");
    lb_emptyMyPlay_rosetube->setFixedHeight(263);

    stackMyPlay_rosetube = new QStackedWidget;
    //stackMyPlay->addWidget(wg_login);
    stackMyPlay_rosetube->addWidget(lb_emptyMyPlay_rosetube);
    stackMyPlay_rosetube->addWidget(scrollArea);

    QVBoxLayout *boxTotal = new QVBoxLayout;
    boxTotal->setContentsMargins(30,0,0,0);
    boxTotal->setSpacing(0);
    boxTotal->addWidget(widget_top);
    boxTotal->addWidget(stackMyPlay_rosetube);

    QWidget *wg_myPlay = new QWidget;
    wg_myPlay->setLayout(boxTotal);
    wg_myPlay->setStyleSheet("border:0px;");

    return wg_myPlay;
}


/**
 * @brief SettingMyPage::setUIControl_loginAfter_myInfoEdit 마이페이지 Edit 화면 구성
 * @return
 */
QWidget* SettingMyPage::setUIControl_loginAfter_myInfoEdit(){

    // 내정보 편집 관련 된것들 --------------------------------------------------------------------
    QLabel *lb_title = new QLabel(tr("Edit My Info"));
    //QLabel *lb_title = new QLabel(tr("내정보 편집"));
    lb_title->setStyleSheet("color:#FFFFFF;font-size:20px;font:bold;");

    lb_profileEdit = new QLabel();
    lb_profileEdit->setFixedSize(PROFILEIMG_SIZE, PROFILEIMG_SIZE);
    lb_profileEdit->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/profile_def.png"));
    lb_profileEdit->setStyleSheet("border-radius:125px;border:0px;");
    QPushButton *btn_editImg = new QPushButton(tr("Edit"));
    //QPushButton *btn_editImg = new QPushButton(tr("편집"));
    btn_editImg->setFixedSize(PROFILEIMG_SIZE, PROFILEIMG_SIZE);
    btn_editImg->setStyleSheet("color:#FFFFFF;font-size:20px;text-align:bottom;padding-bottom:15px;");
    btn_editImg->setCursor(Qt::PointingHandCursor);
    QRect rect(0,0,PROFILEIMG_SIZE,PROFILEIMG_SIZE);
    QRegion region(rect, QRegion::Ellipse);
    btn_editImg->setMask(region);               // QPushButton 클릭 영역을 원으로 세팅

    btn_editImg->setVisible(true);     // TEST : 기능 구현 안되어있어서 hide 함

    lb_email = new QLabel();
    lb_email->setContentsMargins(0,17,0,0);
    lb_email->setAlignment(Qt::AlignCenter);
    lb_email->setStyleSheet("color:#FFFFFF;font-size:20px;");

    QVBoxLayout *vl_le_email = new QVBoxLayout();
    vl_le_email->setContentsMargins(47,0,0,0);
    vl_le_email->setSpacing(10);
    vl_le_email->addWidget(lb_email);


    QLabel *lb_title_nickName = new QLabel(tr("Change your nickname (Upto 10 Characters)"));
    //QLabel *lb_title_nickName = new QLabel(tr("닉네임 변경 (최대10자)"));
    lb_title_nickName->setStyleSheet("color:#CCCCCC;font-size:18px;");
    QLabel *lb_title_comment = new QLabel(tr("Change Introduction (Upto 60 Characters)"));
    //QLabel *lb_title_comment = new QLabel(tr("소개글 변경 (최대60자)"));
    lb_title_comment->setStyleSheet("color:#CCCCCC;font-size:18px;");
    le_nickName = new QLineEdit();
    le_nickName->setTextMargins(15,15,15,15);
    le_nickName->setFixedHeight(50);
    le_nickName->setStyleSheet("background-color:#212121;color:#CCCCCC;font-size:18px;border:0px;");
    le_nickName->setMaxLength(10);
    le_comment = new QLineEdit();
    le_comment->setTextMargins(15,15,15,15);
    le_comment->setStyleSheet("background-color:#212121;color:#CCCCCC;font-size:18px;border:0px;");
    le_comment->setFixedHeight(95);
    le_comment->setAlignment(Qt::AlignTop);
    le_comment->setMaxLength(60);

    QPushButton *btn_duplicateNickName = new QPushButton(tr("Check duplicates"));
    //QPushButton *btn_duplicateNickName = new QPushButton(tr("중복체크"));
    btn_duplicateNickName->setStyleSheet("color:#B18658;font-size:18px;padding-bottom:42px;padding-right:20px;");
    btn_duplicateNickName->setCursor(Qt::PointingHandCursor);


    QVBoxLayout *vl_le_nickName = new QVBoxLayout();
    vl_le_nickName->setContentsMargins(47,0,0,0);
    vl_le_nickName->setSpacing(10);
    vl_le_nickName->addWidget(lb_title_nickName);
    vl_le_nickName->addWidget(le_nickName);
    vl_le_nickName->addSpacing(30);

    QVBoxLayout *vl_le_comment = new QVBoxLayout();
    vl_le_comment->setContentsMargins(47,0,0,0);
    vl_le_comment->setSpacing(10);
    vl_le_comment->addWidget(lb_title_comment);
    vl_le_comment->addWidget(le_comment);


    QPushButton *btn_cancel = new QPushButton();
    btn_cancel->setProperty("goPageIndex", 0);
    btn_cancel->setText(tr("Cancel"));
    //btn_cancel->setText(tr("취소"));
    btn_cancel->setFixedSize(100,40);
    btn_cancel->setStyleSheet("color:#E6E6E6;font-size:18px;border:1px solid #666666;border-radius:20px;");
    btn_cancel->setCursor(Qt::PointingHandCursor);
    QPushButton *btn_save = new QPushButton();
    btn_save->setText(tr("Confirm"));
    //btn_save->setText(tr("확인"));
    btn_save->setFixedSize(100,40);
    btn_save->setStyleSheet("color:#E6E6E6;font-size:18px;border:1px solid #666666;border-radius:20px;");
    btn_save->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_btn = new QHBoxLayout();
    hl_btn->addWidget(btn_cancel);
    hl_btn->addSpacing(25);
    hl_btn->addWidget(btn_save);

    QGridLayout *gl_myInfo = new QGridLayout();
    gl_myInfo->setContentsMargins(0,0,0,0);
    gl_myInfo->setSpacing(0);
    gl_myInfo->addWidget(lb_title, 0, 0, Qt::AlignTop);
    gl_myInfo->addWidget(lb_profileEdit, 1, 0, 2, 1);
    gl_myInfo->addWidget(btn_editImg, 1, 0, 2, 1, Qt::AlignCenter | Qt::AlignBottom);
    gl_myInfo->addLayout(vl_le_email, 3, 1, Qt::AlignLeft);
    gl_myInfo->addLayout(vl_le_nickName, 1, 1, Qt::AlignBottom);
    gl_myInfo->addWidget(btn_duplicateNickName, 1, 1, Qt::AlignRight | Qt::AlignBottom);
    gl_myInfo->addLayout(vl_le_comment, 2, 1, Qt::AlignTop);
    gl_myInfo->addLayout(hl_btn, 0, 1, Qt::AlignRight);


    QVBoxLayout *vl_myInfo = new QVBoxLayout();
    vl_myInfo->setContentsMargins(0,0,0,0);
    vl_myInfo->setSpacing(0);
    vl_myInfo->addLayout(gl_myInfo);

    QWidget *widget_myInfoEdit = new QWidget();
    //widget_myInfoEdit->setContentsMargins(73,30,77,100);
    widget_myInfoEdit->setContentsMargins(73,30,77,40);
    widget_myInfoEdit->setObjectName("widget_myInfoEdit");
    widget_myInfoEdit->setStyleSheet("#widget_myInfoEdit { background:qlineargradient(x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #222222, stop:1 #464646); }");
    widget_myInfoEdit->setLayout(vl_myInfo);


    // 내정보 편집 외의 것들 --------------------------------------------------------------------
    lb_title_regPhoto = new QLabel(tr("Photo registration (You can register up to 10 photos.)"));
    //lb_title_regPhoto = new QLabel(tr("사진등록 (10개까지 등록하실 수 있습니다.)"));
    lb_title_regPhoto->setStyleSheet("font-size:18px;color:#CCCCCC");
    QLabel *lb_title_changePw = new QLabel(tr("Change Password"));
    //QLabel *lb_title_changePw = new QLabel(tr("비밀번호 변경"));
    lb_title_changePw->setStyleSheet("font-size:18px;color:#CCCCCC");
    le_pw = new QLineEdit();
    le_pw->setPlaceholderText(tr("Please enter a new password."));
    //le_pw->setPlaceholderText(tr("새 비밀번호를 입력해주세요."));
    le_pw->setTextMargins(15,15,15,15);
    le_pw->setFixedHeight(50);
    le_pw->setStyleSheet("background-color:#333333;color:#919191;font-size:20px;border:0px;");
    le_pw->setMaxLength(30);
    le_pw->setEchoMode(QLineEdit::Password);

    QLabel *lb_title_myInfo = new QLabel(tr("Personal information management (cancellation or recovery of deleted accounts is not possible.)"));
    //QLabel *lb_title_myInfo = new QLabel(tr("개인정보 관리 (탈퇴한 계정은 취소 혹은 복구가 불가능합니다.)"));
    lb_title_myInfo->setStyleSheet("font-size:18px;color:#CCCCCC");
    QPushButton *btn_delete = new QPushButton(tr("Delete your account"));
    //QPushButton *btn_delete = new QPushButton(tr("회원탈퇴"));
    btn_delete->setCursor(Qt::PointingHandCursor);
    btn_delete->setStyleSheet("color:#B18658;font-size:18px;font:bold;border-radius:25px;border:1px solid #B18658;");
    btn_delete->setFixedSize(500,50);

    flowlayout_photos = new FlowLayout();
    flowlayout_photos->setContentsMargins(0,15,0,36);
    flowlayout_photos->setSpacingHV(22, 22);
    widget_flow = new QWidget();
    widget_flow->setLayout(flowlayout_photos);

    vl_total_withoutMyInfo = new QVBoxLayout();
    vl_total_withoutMyInfo->setContentsMargins(43,0,43,0);
    vl_total_withoutMyInfo->setSpacing(0);
    vl_total_withoutMyInfo->setAlignment(Qt::AlignTop);
    vl_total_withoutMyInfo->addSpacing(36);
    vl_total_withoutMyInfo->addWidget(lb_title_regPhoto);
    vl_total_withoutMyInfo->addWidget(widget_flow);
    vl_total_withoutMyInfo->addWidget(lb_title_changePw);
    vl_total_withoutMyInfo->addSpacing(15);
    vl_total_withoutMyInfo->addWidget(le_pw);
    vl_total_withoutMyInfo->addSpacing(50);
    vl_total_withoutMyInfo->addWidget(lb_title_myInfo);
    vl_total_withoutMyInfo->addSpacing(15);
    vl_total_withoutMyInfo->addWidget(btn_delete);
    vl_total_withoutMyInfo->addSpacing(50);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(widget_myInfoEdit, 0, Qt::AlignTop);    // 내 정보 편집
    vl_total->addLayout(vl_total_withoutMyInfo);                // 내 정보 편집 외 (사진등록/비번/탈퇴)

    QWidget *widget = new QWidget();
    widget->setLayout(vl_total);


    // 커넥션
    connect(btn_cancel, &QPushButton::clicked, this, &SettingMyPage::slot_changePageEdit);
    connect(btn_save, &QPushButton::clicked, this, &SettingMyPage::slot_saveMyInfo);
    connect(btn_editImg, &QPushButton::clicked, this, &SettingMyPage::slot_editImage);
    connect(btn_delete, &QPushButton::clicked, this, &SettingMyPage::slot_deleteMyInfo);
    connect(btn_duplicateNickName, &QPushButton::clicked, this, &SettingMyPage::slot_duplicateNickName);

    return widget;
}


/**
 * @brief SettingMyPage::setDataMyPage 첫번째 Stack 으로 이동
 * @note 외부에서 페이지 처음 보일때 사용되는 함수
 */
void SettingMyPage::setDataMyPage(){

    //if(!global.user.getEmail().isEmpty()){
    if(global.user.isValid()==true){

        // 로그인 후 화면 구성
        widget_loginBefore->setVisible(false);
        widget_loginAfter->setVisible(true);
        this->slot_getMyInfo_loginAfter();
    }else{

        widget_loginBefore->setVisible(true);
        widget_loginAfter->setVisible(false);
    }
    this->stackWidget_loginAfter->setCurrentIndex(0);
}


/**
 * @brief SettingMyPage::slot_showLoginPage [SLOT] 로그인 버튼 클릭시
 */
void SettingMyPage::slot_showLoginPage(){

    Dialog::DialogLogin *dlg = new Dialog::DialogLogin(this);
    dlg->exec();


}


/**
 * @brief SettingMyPage::slot_getMyInfo_loginAfter [SLOT] 내 정보/음악 플레이리스트/로즈튜브 플레이리스트 를 가져온다
 */
void SettingMyPage::slot_getMyInfo_loginAfter(){

    if(global.user.isValid()==true){

        // 내 정보 가져오기
        // 내 정보 가져오기
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + "/member/member/" + global.user.getSub();
        QJsonObject json;

        network->request(HTTP_GET_MYINFO
                         ,url
                         , json
                         , false
                         , true);

        requestMusicMyPlaylist();     
        requestRosetubeMyPlaylist();
    }
}

void SettingMyPage::slot_getMyInfo_AfterMemberImageUploaded(){

    if(global.user.isValid()==true){

        // 내 정보 가져오기
        // 내 정보 가져오기
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + "/member/member/" + global.user.getSub();
        QJsonObject json;

        network->request(HTTP_GET_MYINFO_AfterMemberImageUploaded
                         ,url
                         , json
                         , false
                         , true);


    }
}

/**
 * @brief SettingMyPage::slot_myPlaylistChanged : [슬롯] 마이 플레이리스트 변경
 */
void SettingMyPage::slot_myPlaylistChanged(){

    requestMusicMyPlaylist();
    requestRosetubeMyPlaylist();
}


void SettingMyPage::slot_change_device_state(const QString &category){

    if(global.user.isChanged() && (category == SIGNAL_CATEGORY_ROSE)){

        global.user.setDeviceChange(false);

        // 내 정보 가져오기
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + "/member/member/%2" + global.user.getSub();
        QJsonObject json;

        network->request(HTTP_GET_MYINFO
                         ,url
                         , json
                         , false
                         , true);
    }
}


/**
 * @brief SettingMyPage::requestMusicMyPlaylist : 음악 마이플레이리스트 GET
 */
void SettingMyPage::requestMusicMyPlaylist(){

    // 음악 플레이리스트 가져오기
    GSCommon::clearLayout(boxMyPlayList_music);
    //stackMyPlayList->setCurrentIndex(1);
    stackMyPlayList_music->setCurrentIndex(0);
    lb_emptyMyPlayList_music->setText(tr("Gathering Information"));
    //lb_emptyMyPlayList_music->setText(tr("정보를 가져오는 중입니다."));

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    //QString url = global.legacy_v1.
    QJsonObject json;
    json.insert("local_ip", global.device.getDeviceIP());
    json.insert("mac_address", global.device.getDeviceID());
    network->request(HTTP_MY_PLYALIST, QString("%1/playlist?page=0&size=10")
                     .arg(global.legacy_v3_api)
                     , json, true,true);
}


/**
 * @brief SettingMyPage::requestRosetubeMyPlaylist : 로즈튜브 마이플레이리스트 GET
 */
void SettingMyPage::requestRosetubeMyPlaylist(){

    // 로즈튜브 플레이리스트 가져오기
    GSCommon::clearLayout(boxMyPlay_rosetube);
    //stackMyPlay->setCurrentIndex(1);
    stackMyPlay_rosetube->setCurrentIndex(0);
    lb_emptyMyPlay_rosetube->setText(tr("Gathering Information"));
    //lb_emptyMyPlay_rosetube->setText(tr("정보를 가져오는 중입니다."));

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("type","youtube");
    params.addQueryItem("username",global.user.getUsername());
    //params.addQueryItem("range_start",QString::number(0));
    //params.addQueryItem("range_end",QString::number(15));
    network->request(HTTP_ROSETUBE_PLAYLIST, QString("%1/playlist/fetch?range_start=0&range_end=10")
                     .arg(global.legacy_mod_api), params, true,true);
}


/**
 * @brief SettingMyPage::slot_responseHttp Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void SettingMyPage::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    print_debug();
    //QJsonDocument doc(p_jsonObject);QString strJson(doc.toJson(QJsonDocument::Compact));qDebug() << "SettingMyPage::slot_responseHttp---- " << strJson << "\n";

    if(p_id == HTTP_GET_MYINFO){
        this->setResponseProfile(p_jsonObject);
    }else if(p_id == HTTP_CHECK_DUPLICATE_NICKNAME){
        this->setResponseNickNameCheck(p_jsonObject);
    }else if(p_id == HTTP_GET_MYINFO_AfterMemberImageUploaded){
        print_debug();
        this->setResponseProfile_memberImageUploaded(p_jsonObject);
    }else if(p_id == HTTP_PUT_DELETE_PHOTO){
        this->setResponseProfile(p_jsonObject);
        //this->setResponseDeletePhoto(p_jsonObject);
    }else if(p_id == HTTP_SAVE_MYINFO){
        if(ProcJsonEasy::get_flagOk(p_jsonObject) == true)
        {
            QString email;
            QString pw;

            SqliteHelper *sqlite = new SqliteHelper(this);
            QSqlError err = sqlite->addConnectionLocal();

            if(err.type() == QSqlError::NoError){
                QVariantList data;
                sqlite->exec("SELECT * FROM User", data);

                if(data.size() > 0){
                    QMap<QString, QVariant> map = data.at(0).toMap();
                    if(map.contains("email")){
                        email = map["email"].toString();
                    }
                    if(map.contains("pw")){
                        pw = map["pw"].toString();
                    }
                }
            }

            sqlite->close();
            delete sqlite;

            // 1) global 변수 재세팅 및 저장
            global.user.setUserName(this->le_nickName->text());
            global.user.setUserBIO(this->le_comment->text());

            if(!this->tmp_password.isEmpty() && this->tmp_password != pw){
                SqliteHelper *sqlite = new SqliteHelper(this);
                QSqlError err = sqlite->addConnectionLocal();

                if(err.type() == QSqlError::NoError){
                    QString strQuery_create = "CREATE TABLE IF NOT EXISTS User (";
                    strQuery_create += "email VARCHAR(50), pw TEXT)";
                    sqlite->exec(strQuery_create);

                    QString strQuery_delete = "DELETE FROM User";
                    sqlite->exec(strQuery_delete);

                    QString strQuery_insert = "INSERT INTO User (email, pw) ";
                    strQuery_insert += " VALUES ";
                    strQuery_insert += " ('"+global.user.getEmail()+"','"+this->tmp_password+"'";
                    strQuery_insert += "  )";
                    sqlite->exec(strQuery_insert);
                }

                sqlite->close();
                delete sqlite;
            }

            // 2) 로즈기기에 사용자 정보 재세팅
            this->requestSetRoseUserInfo(global.user.getUsername(), global.user.getEmail(), global.user.getAccess_token());

            // 3) 페이지 리로드
            this->setDataMyPage();

            // 4) 시그널 발생
            emit signal_changeMyInfoData();
        }
        else{

        }
        /*QString tmp_status = "";
        if(p_jsonObject.contains("status")){
            tmp_status = p_jsonObject["status"].toString();
        }
        if(tmp_status=="OK"){

            // 1) global 변수 재세팅
            global.user.setUserName(this->le_nickName->text());

            // 2) 로즈기기에 사용자 정보 재세팅
            this->requestSetRoseUserInfo(this->le_nickName->text(), global.user.getEmail(), global.user.getAccess_token());

            // 3) 페이지 리로드
            this->setDataMyPage();

            // 4) 시그널 발생
            emit signal_changeMyInfoData();
        }*/
    }else if(p_id == HTTP_MY_PLYALIST){
        this->setResultOfPlayListMusic(p_jsonObject);

    }else if(p_id == HTTP_ROSETUBE_PLAYLIST){
        this->setResultOfPlayListRosetube(p_jsonObject);

    }else if(p_id == HTTP_SET_ROSE_USER_INFO){

    }else if(p_id == HTTP_USER_DELETE){

        bool flagOk = false;
        if(p_jsonObject.contains("flagOk")){
            flagOk = p_jsonObject["flagOk"].toBool();
        }

        if(flagOk == true){
            this->setLogout();     // 회원 탈퇴 성공시 로그아웃 처리
        }
        else{
            ToastMsg::show(this, "", tr("Failed to delete account. Please try again."));
        }
    }

    sender()->deleteLater();
}


/**
 * @brief SettingMyPage::getUIAddPhotoWidget
 * @return
 */
QPushButton* SettingMyPage::getUIAddPhotoWidget(){

    QPushButton *btn_addPhoto = GSCommon::getUIBtnImg("btn_addPhoto",":/images/setting/addPhoto.png");
    btn_addPhoto->setFixedSize(200,200);
    btn_addPhoto->setCursor(Qt::PointingHandCursor);
    btn_addPhoto->setStyleSheet(btn_addPhoto->styleSheet().replace("}", "border:3px solid #707070; }"));
    connect(btn_addPhoto, &QPushButton::clicked, this, &SettingMyPage::slot_addPhoto);
    return btn_addPhoto;
}


/**
 * @brief SettingMyPage::setResponseProfile 해당 json으로 프로필 정보 세팅
 * @param p_jsonObject
 * @note View화면과 Edit화면 둘다 UI에 데이터 세팅한다.
 */
void SettingMyPage::setResponseProfile(QJsonObject p_jsonObject){

    //QJsonDocument doc(p_jsonObject);
    //QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() << "ROSE USER PROFILE RCV : " << strJson;

    bool flagOk = false;
    if(p_jsonObject.contains("flagOk")){
        flagOk = p_jsonObject["flagOk"].toBool();
    }

    if(flagOk == false){
        // 다른 기기 로그인 하였거나 다른 문제로 인해 .. 다시 로그인 시도하게 해야한다...
        widget_loginBefore->setVisible(true);
        widget_loginAfter->setVisible(false);
        //ToastMsg::show(this, "", tr("Account Information has been changed."));
        //ToastMsg::show(this, "", tr("계정정보가 변경되었습니다."));        
    }else{
        global.user.setUserInfo(p_jsonObject);

        widget_loginBefore->setVisible(false);
        widget_loginAfter->setVisible(true);

        // 뷰어화면 초기화
        this->lb_comment->clear();
        this->lb_profile->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/LV_VIP_image.png"));
        this->lb_profile_grade->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/LV_VIP_outline.png"));

        // 편집화면 초기화
        this->lb_profileEdit->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/profile_def.png"));

        GSCommon::clearLayout(this->hl_photo);
        GSCommon::clearLayout(this->flowlayout_photos);

        this->le_pw->setText("");
        this->le_comment->clear();
        //if(global.debugCHK > 3){
        flowlayout_photos->addWidget(getUIAddPhotoWidget()); // TEST : 기능 구현 안되어있어서 hide 함
        //}

        this->list_myPhotoEdit.clear();        
        this->lb_title_regPhoto->setVisible(true);
        this->widget_flow->setVisible(true);

        QString tmp_imgURL_main = "";

        if(p_jsonObject.contains("member")){
            QJsonObject tmp_jsonObject_member = p_jsonObject["member"].toObject();

            // 기본 5등급부터 시작.. output에 등급 key 없음..
            // 그래서, 현재 항상 무조건 5등급의 기본이미지 세팅
            this->lb_profile_grade->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/LV_VIP_outline.png"));

            if(tmp_jsonObject_member.contains("name")){
                this->lb_nickName->setText(tmp_jsonObject_member["name"].toString());
                this->le_nickName->setText(tmp_jsonObject_member["name"].toString());
                this->data_profileInfo.username = tmp_jsonObject_member["name"].toString();
            }
            if(tmp_jsonObject_member.contains("id")){

                this->member_id = tmp_jsonObject_member["id"].toInt();
            }

            if(tmp_jsonObject_member.contains("comment")){
                this->lb_comment->setText(tmp_jsonObject_member["comment"].toString());
                this->le_comment->setText(tmp_jsonObject_member["comment"].toString());
                this->data_profileInfo.bio = tmp_jsonObject_member["comment"].toString();
            }
            else{
                this->lb_comment->setText(tr("There is no introduction."));
                //this->lb_comment->setText(tr("소개글이 없습니다."));

                this->le_comment->setText(tr("There is no introduction."));
                //this->le_comment->setText(tr("소개글이 없습니다."));
            }

            if(tmp_jsonObject_member.contains("email")){
                this->lb_email->setText(tmp_jsonObject_member["email"].toString());
                this->data_profileInfo.email = tmp_jsonObject_member["email"].toString();
                print_debug();
                qDebug() << "this->data_profileInfo.email = " << this->data_profileInfo.email;
            }

            if(tmp_jsonObject_member.contains("profileImage")){
                tmp_imgURL_main = tmp_jsonObject_member["profileImage"].toString();
                del_imgURL_main = tmp_imgURL_main;
                if(tmp_imgURL_main!=""){

                    FileDownloader *tmp_fileDownLoader = new FileDownloader(this);
                    connect(tmp_fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImageMainProfile()));
                    tmp_fileDownLoader->setImageURL(QUrl(tmp_imgURL_main));
                }
            }

            if(tmp_jsonObject_member.contains("images")){
                QJsonArray tmp_arr = tmp_jsonObject_member["images"].toArray();
                for(int i=0; i<tmp_arr.count(); i++){

                    // 뷰어화면의 사진목록
                    QLabel *lb_photo = new QLabel();
                    lb_photo->setFixedSize(PHOTOS_SIZE, PHOTOS_SIZE);
                    this->hl_photo->addWidget(lb_photo);

                    // 편집화면의 사진 목록
                    MyPhotoEdit *myPhotoEdit = new MyPhotoEdit();
                    myPhotoEdit->setDataMyPhotoEdit(tmp_arr[i].toString());
                    this->list_myPhotoEdit.append(myPhotoEdit);
                    connect(myPhotoEdit, &MyPhotoEdit::signal_deletePhoto, this, &SettingMyPage::slot_deletePhoto);
                    this->flowlayout_photos->addWidget(myPhotoEdit);

                    FileDownloader *tmp_fileDownLoader = new FileDownloader(this);
                    tmp_fileDownLoader->setProperty("index", i);
                    connect(tmp_fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImagePhotos()));
                    tmp_fileDownLoader->setImageURL(QUrl(tmp_arr[i].toString()));
                }

                // TEST : 기능 구현 안되어있어서 hide/show 함
                if(tmp_arr.count() > 0){
                    this->lb_title_regPhoto->setVisible(true);
                    this->widget_flow->setVisible(true);
                }
            }
        }
    }
}

void SettingMyPage::setResponseProfile_memberImageUploaded(QJsonObject p_jsonObject){

    //QJsonDocument doc(p_jsonObject);
    //QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() << "ROSE USER PROFILE RCV : " << strJson;

    print_debug();
    bool flagOk = false;
    if(p_jsonObject.contains("flagOk")){
        flagOk = p_jsonObject["flagOk"].toBool();
    }

    if(flagOk == false){
        print_debug();
        // 다른 기기 로그인 하였거나 다른 문제로 인해 .. 다시 로그인 시도하게 해야한다...
        widget_loginBefore->setVisible(true);
        widget_loginAfter->setVisible(false);
        //ToastMsg::show(this, "", tr("Account Information has been changed."));
        //ToastMsg::show(this, "", tr("계정정보가 변경되었습니다."));
    }else{

        print_debug();
        GSCommon::clearLayout(this->hl_photo);
        GSCommon::clearLayout(this->flowlayout_photos);

        //if(global.debugCHK > 3){
            flowlayout_photos->addWidget(getUIAddPhotoWidget()); // TEST : 기능 구현 안되어있어서 hide 함
        //}

        QString tmp_imgURL_main = "";

        if(p_jsonObject.contains("member")){
            QJsonObject tmp_jsonObject_member = p_jsonObject["member"].toObject();

            if(tmp_jsonObject_member.contains("profileImage")){
                tmp_imgURL_main = tmp_jsonObject_member["profileImage"].toString();
                del_imgURL_main = tmp_imgURL_main;
                if(tmp_imgURL_main!=""){

                    FileDownloader *tmp_fileDownLoader = new FileDownloader(this);
                    connect(tmp_fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImageMainProfile()));
                    tmp_fileDownLoader->setImageURL(QUrl(tmp_imgURL_main));
                }
            }


            if(tmp_jsonObject_member.contains("images")){
                QJsonArray tmp_arr = tmp_jsonObject_member["images"].toArray();
                for(int i=0; i<tmp_arr.count(); i++){
                    print_debug();
                    // 뷰어화면의 사진목록
                    QLabel *lb_photo = new QLabel();
                    lb_photo->setFixedSize(PHOTOS_SIZE, PHOTOS_SIZE);
                    this->hl_photo->addWidget(lb_photo);

                    // 편집화면의 사진 목록
                    MyPhotoEdit *myPhotoEdit = new MyPhotoEdit();
                    myPhotoEdit->setDataMyPhotoEdit(tmp_arr[i].toString());
                    this->list_myPhotoEdit.append(myPhotoEdit);
                    connect(myPhotoEdit, &MyPhotoEdit::signal_deletePhoto, this, &SettingMyPage::slot_deletePhoto);
                    this->flowlayout_photos->addWidget(myPhotoEdit);

                    FileDownloader *tmp_fileDownLoader = new FileDownloader(this);
                    tmp_fileDownLoader->setProperty("index", i);
                    connect(tmp_fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImagePhotos()));
                    tmp_fileDownLoader->setImageURL(QUrl(tmp_arr[i].toString()));
                }

            }
        }
    }
}

void SettingMyPage::setResponseProfile_profileImageUploaded(QJsonObject p_jsonObject){

    //QJsonDocument doc(p_jsonObject);
    //QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() << "ROSE USER PROFILE RCV : " << strJson;

    print_debug();
    bool flagOk = false;
    if(p_jsonObject.contains("flagOk")){
        flagOk = p_jsonObject["flagOk"].toBool();
    }

    if(flagOk == false){
        print_debug();
        // 다른 기기 로그인 하였거나 다른 문제로 인해 .. 다시 로그인 시도하게 해야한다...
        widget_loginBefore->setVisible(true);
        widget_loginAfter->setVisible(false);
        //ToastMsg::show(this, "", tr("Account Information has been changed."));
        //ToastMsg::show(this, "", tr("계정정보가 변경되었습니다."));
    }else{

        print_debug();
        GSCommon::clearLayout(this->hl_photo);
        GSCommon::clearLayout(this->flowlayout_photos);

        //if(global.debugCHK > 3){
            flowlayout_photos->addWidget(getUIAddPhotoWidget()); // TEST : 기능 구현 안되어있어서 hide 함
        //}

        QString tmp_imgURL_main = "";

        if(p_jsonObject.contains("member")){
            QJsonObject tmp_jsonObject_member = p_jsonObject["member"].toObject();
            if(tmp_jsonObject_member.contains("profileImage")){
                tmp_imgURL_main = tmp_jsonObject_member["profileImage"].toString();
                del_imgURL_main = tmp_imgURL_main;
                if(tmp_imgURL_main!=""){

                    FileDownloader *tmp_fileDownLoader = new FileDownloader(this);
                    connect(tmp_fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImageMainProfile()));
                    tmp_fileDownLoader->setImageURL(QUrl(tmp_imgURL_main));
                }
            }


        }
    }
}

/**
 * @brief SettingMyPage::setResponseNickNameCheck 해당 json으로 nickName 중복체크 체크
 * @param p_jsonObject
 */
void SettingMyPage::setResponseNickNameCheck(QJsonObject p_jsonObject){

    bool flagOk = false;
    if(p_jsonObject.contains("flagOk")){
        flagOk = p_jsonObject["flagOk"].toBool();
    }

    if(flagOk == false){
        QString message = p_jsonObject["message"].toString();

        if(message == "이름 중복"){
            ToastMsg::show(this, "", tr("This nickanme is already using."));
            //ToastMsg::show(this, "", tr("이미 사용중인 닉네임입니다."));
        }
        else{
            ToastMsg::show(this, "", tr("Please try again.(SMCN)"));
            //ToastMsg::show(this, "", tr("다시 시도해주세요.(SMCN)"));
        }
    }
    else{
        ToastMsg::show(this, "", tr("Availiable nickname."));
        //ToastMsg::show(this, "", tr("사용 가능한 닉네임입니다."));
    }
}


/**
 * @brief SettingMyPage::setResponseDeletePhoto 사진 삭제 후 결과 세팅
 * @param p_jsonObject
 */
void SettingMyPage::setResponseDeletePhoto(QJsonObject p_jsonObject){

    bool flagOk = false;
    if(p_jsonObject.contains("flagOk")){
        flagOk = p_jsonObject["flagOk"].toBool();
        slot_getMyInfo_AfterMemberImageUploaded();

    }
    if(flagOk==false){
        ToastMsg::show(this, "", tr("Please try again.(SMDP)"));
        //ToastMsg::show(this, "", tr("다시 시도해주세요.(SMDP)"));

    }else{
        QString tmp_status = "";
        if(p_jsonObject.contains("status")){
            tmp_status = p_jsonObject["status"].toString();
        }
        if(tmp_status=="OK"){
            // 미리 이미지 UI 제거하였음.
        }
    }
}


/**
 * @brief SettingMyPage::setResultOfMyPlayList : 음악 플레이리스트 요청 결과 처리
 * @param p_jsonObject QJsonObject
 */
void SettingMyPage::setResultOfPlayListMusic(const QJsonObject &p_jsonObject){
print_debug();
    bool flagOk = false;
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_playLists = "playlists";
    const QString jsonKey_totalcount = "totalcount";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_totalcount)){

            flagOk = true;
            int tmp_cntPlayList = p_jsonObject[jsonKey_totalcount].toInt(0);

            if(tmp_cntPlayList){
                if(p_jsonObject.contains(jsonKey_playLists)){
                    QJsonArray jsonArr = p_jsonObject[jsonKey_playLists].toArray();

                    for(int i = 0; i < jsonArr.size(); i++){
                        QJsonObject jsonPlayList = jsonArr.at(i).toObject();
                        appendPlayListMusic(jsonPlayList);
                    }
                    //stackMyPlayList->setCurrentIndex(2);
                    stackMyPlayList_music->setCurrentIndex(1);
                }
            }else{
                lb_emptyMyPlayList_music->setText(tr("There is no music playlist."));
                //lb_emptyMyPlayList_music->setText(tr("음악 플레이 리스트가 없습니다."));
            }
        }
    }

    if(!flagOk){
        lb_emptyMyPlayList_music->setText(tr("The music playlist information could not be verified."));
        //lb_emptyMyPlayList_music->setText(tr("음악 플레이 리스트 정보를 확인할 수 없습니다."));
    }
}


/**
 * @brief SettingMyPage::setResultOfPlayListRosetube : 로즈튜브 플레이리스트 요청 결과 처리
 * @param p_jsonObject QJsonObject
 */
void SettingMyPage::setResultOfPlayListRosetube(const QJsonObject &p_jsonObject){
print_debug();
    bool flagOk = false;

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";
    const QString jsonKey_playlists = "playlists";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_playlists)){
            flagOk = true;
            QJsonArray jsonArr = p_jsonObject[jsonKey_playlists].toArray();

            if(jsonArr.size() > 0){
                for(int i = 0; i < jsonArr.size(); i++){
                    appendPlayListRosetube(jsonArr.at(i).toObject());
                }
                //stackMyPlay->setCurrentIndex(2);
                stackMyPlay_rosetube->setCurrentIndex(1);
                //wg_myPlay->show();

            }else{
                lb_emptyMyPlay_rosetube->setText(tr("There is no RoseTube Playlist."));
                //lb_emptyMyPlay_rosetube->setText(tr("로즈튜브 플레이리스트가 없습니다."));
            }
        }

    }

    if(!flagOk){
        lb_emptyMyPlay_rosetube->setText(tr("Unable to verify the RoseTube playlist information."));
        //lb_emptyMyPlay_rosetube->setText(tr("로즈튜브 플레이리스트 정보를 확인할 수 없습니다."));
    }
}


/**
 * @brief SettingMyPage::appendMyPlayList : 음악 플레이리스트 UI 추가
 * @param p_json QJsonObject 플레이리스트 정보
 */
void SettingMyPage::appendPlayListMusic(const QJsonObject &p_json){

    FrameMyPlayList *tmp_widget = new FrameMyPlayList;
    tmp_widget->setData(p_json);
    tmp_widget->setHover();
    boxMyPlayList_music->addWidget(tmp_widget);
}


/**
 * @brief SettingMyPage::appendPlayListRosetube : 로즈튜브 플레이리스트 UI 추가
 * @param p_json QJsonObject 플레이리스트 정보
 */
void SettingMyPage::appendPlayListRosetube(const QJsonObject &p_json){

    const QString jsonKey_type = "type";
    if(p_json.contains(jsonKey_type) && p_json[jsonKey_type].toString()=="youtube"){
        FrameRoseTubePlayList *tmp_widget = new FrameRoseTubePlayList(OptionPopup::TypeMenu::RoseTube_Home_PlayList_My);
        tmp_widget->setData(p_json);
        tmp_widget->setHover();
        boxMyPlay_rosetube->addWidget(tmp_widget);
    }
}


/**
 * @brief SettingMyPage::slot_loadImageMainProfile [SLOT] 프로필 메인 이미지 세팅
 */
void SettingMyPage::slot_loadImageMainProfile(){

    FileDownloader *tmp_fileDownloader = qobject_cast<FileDownloader*>(sender());
    QPixmap *pixmap_albumImg = new QPixmap();
    bool flagLoad = pixmap_albumImg->loadFromData(tmp_fileDownloader->downloadedData());
    if(flagLoad){
        QPixmap tmp_pixmap = pixmap_albumImg->scaled(PROFILEIMG_SIZE, PROFILEIMG_SIZE, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation); // KeepAspectRatio

        // 빈 Pixmap
        QPixmap pixmap_painter = QPixmap(QSize(PROFILEIMG_SIZE,PROFILEIMG_SIZE));
        pixmap_painter.fill(Qt::transparent);

        QPainter painter (&pixmap_painter);
        painter.setRenderHint(QPainter::Antialiasing, true);
        QBrush brush = QBrush(tmp_pixmap);
        painter.setBrush(brush);
        painter.drawRoundedRect(0, 0, PROFILEIMG_SIZE, PROFILEIMG_SIZE, PROFILEIMG_SIZE/2, PROFILEIMG_SIZE/2);    // border-radius:4px
        this->lb_profile->setPixmap(pixmap_painter);
        this->lb_profileEdit->setPixmap(pixmap_painter);
    }

    tmp_fileDownloader->deleteLater();
}


/**
 * @brief SettingMyPage::slot_loadImagePhotos [SLOT] 프로필 여러 사진 세팅
 */
void SettingMyPage::slot_loadImagePhotos(){

    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    int tmp_index = fileDownloader->property("index").toInt();

    if(tmp_index>=0 && tmp_index<hl_photo->count()){
        QImage image;
        bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());

        if(flagLoaded){
            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(PHOTOS_SIZE,PHOTOS_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            dynamic_cast<QLabel*>(this->hl_photo->itemAt(tmp_index)->widget())->setPixmap(tmp_pixmap);
            //listWidget->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
            //QPixmapCache::insert(dataTracks->at(tmp_index)->getThumbnail(), tmp_pixmap);
        }
    }

    fileDownloader->deleteLater();
}



void SettingMyPage::uploadphotoImgToFile(const QString &tmp_savedfileName){


    qint64 tmp_time = QDateTime::currentMSecsSinceEpoch();

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->setBoundary("6o2knFse3p53ty9dmcQvWAIx1zInP11uCfbm");



    fileUpload = new QFile(tmp_savedfileName);
    fileUpload->open(QIODevice::ReadOnly);
    print_debug();
    qDebug() << "fileUpload->fileName() = " << fileUpload->fileName();

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader,  QVariant("image/jpeg"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"file\"; filename=\""+QString::number(tmp_time)+".txt\""));

    imagePart.setBodyDevice(fileUpload);
    fileUpload->setParent(multiPart);


    multiPart->append(imagePart);

    QUrl url(QString("%1/member/member/file/MEMBER_IMAGE").arg(global.legacy_dev_v1));

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("multipart/form-data; charset=utf-8;boundary=\"6o2knFse3p53ty9dmcQvWAIx1zInP11uCfbm\""));
    //request.setHeader(QNetworkRequest::ContentDispositionHeader,
    //                    QVariant("form-data; name=\"file\"; filename=\""+QString(tmp_savedfileName)+"\""));
     //                 QVariant("form-data; name=\"file\"; filename=\""+QString::number(tmp_time)+".txt\""));

    QString header0 ="ko";
    request.setRawHeader("Accept-Language", header0.toUtf8());
    QString header = global.user.getToken_type()+" "+global.user.getAccess_token();
    request.setRawHeader("Authorization", header.toUtf8());

    print_debug();
    qDebug() << QString("form-data; name=\"file\"; filename=\""+QString(QFileInfo(tmp_savedfileName).fileName())+"\"");
    qDebug() << QString("%1/member/member/file/MEMBER_IMAGE").arg(global.legacy_dev_v1);
    qDebug() << header;

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply *reply = manager->post(request, multiPart);
    reply->setProperty("filename", QVariant(tmp_savedfileName));
    multiPart->setParent(reply);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_networkManagerFinished()));
    connect(reply, SIGNAL(finished()), this, SLOT(slot_replyFinished()));
}


void SettingMyPage::uploadphotoImgToFile_member_Profile(const QString &tmp_savedfileName){


    qint64 tmp_time = QDateTime::currentMSecsSinceEpoch();

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->setBoundary("6o2knFse3p53ty9dmcQvWAIx1zInP11uCfbm");



    fileUpload = new QFile(tmp_savedfileName);
    fileUpload->open(QIODevice::ReadOnly);
    print_debug();
    qDebug() << "fileUpload->fileName() = " << fileUpload->fileName();

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader,  QVariant("image/jpeg"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"file\"; filename=\""+QString::number(tmp_time)+".txt\""));

    imagePart.setBodyDevice(fileUpload);
    fileUpload->setParent(multiPart);


    multiPart->append(imagePart);

    QUrl url(QString("%1/member/member/file/MEMBER_PROFILE").arg(global.legacy_dev_v1));

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("multipart/form-data; charset=utf-8;boundary=\"6o2knFse3p53ty9dmcQvWAIx1zInP11uCfbm\""));
    //request.setHeader(QNetworkRequest::ContentDispositionHeader,
    //                    QVariant("form-data; name=\"file\"; filename=\""+QString(tmp_savedfileName)+"\""));
     //                 QVariant("form-data; name=\"file\"; filename=\""+QString::number(tmp_time)+".txt\""));

    QString header0 ="ko";
    request.setRawHeader("Accept-Language", header0.toUtf8());
    QString header = global.user.getToken_type()+" "+global.user.getAccess_token();
    request.setRawHeader("Authorization", header.toUtf8());

    print_debug();
    qDebug() << QString("form-data; name=\"file\"; filename=\""+QString(QFileInfo(tmp_savedfileName).fileName())+"\"");
    qDebug() << QString("%1/member/member/file/MEMBER_IMAGE").arg(global.legacy_dev_v1);
    qDebug() << header;

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply *reply = manager->post(request, multiPart);
    reply->setProperty("filename", QVariant(tmp_savedfileName));
    multiPart->setParent(reply);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_networkManagerFinished()));
    connect(reply, SIGNAL(finished()), this, SLOT(slot_replyFinished()));
}

void SettingMyPage::slot_replyFinished(){


}

/**
 * @brief Proc_UploadThumbnail::slot_networkManagerFinished : [슬롯]QNetworkAccessManager finished
 */
void SettingMyPage::slot_networkManagerFinished(){

    print_debug();
    slot_getMyInfo_AfterMemberImageUploaded();
    emit signal_uploadedMemberImage();
}

/**
 * @brief SettingMyPage::slot_addPhoto 사진 추가 버튼 클릭시
 */
void SettingMyPage::slot_addPhoto(){

    QStringList files = QFileDialog::getOpenFileNames(this,
                                                      tr("Open files"),
                                                      QString(),
                                                      tr("jpg Files (*.jpg)"));
    QString uploadFile;

    foreach (QString filePath, files) {
        //qDebug() << "QDir(filePath).dirName() = " << QDir(filePath).dirName();
        qDebug() << "QFileInfo(filePath).fileName() = " <<  QFileInfo(filePath).fileName();
        qDebug() << "filePath = " << filePath;
        uploadFile = filePath;
        //QUrl(filePath);
    }
    uploadphotoImgToFile(uploadFile);

}
void SettingMyPage::slot_addPhoto_member_Profile(){

    DialogConfirm_3button *dlg = new DialogConfirm_3button(this);


    dlg->setTitle(tr("Notification"));
    dlg->setText(tr("Are you sure you want to motify your profile image(photo)?"));
    int result = dlg->exec();

    if(result == QDialog::Accepted){

        QStringList files = QFileDialog::getOpenFileNames(this,
                                                          tr("Open files"),
                                                          QString(),
                                                          tr("jpg Files (*.jpg)"));
        QString uploadFile;

        foreach (QString filePath, files) {
            //qDebug() << "QDir(filePath).dirName() = " << QDir(filePath).dirName();
            qDebug() << "QFileInfo(filePath).fileName() = " <<  QFileInfo(filePath).fileName();
            qDebug() << "filePath = " << filePath;
            uploadFile = filePath;
            //QUrl(filePath);
        }
        uploadphotoImgToFile_member_Profile(uploadFile);
    }

}


/**
 * @brief SettingMyPage::slot_changePageEdit [SLOT] 편집/취소 버튼 클릭시
 * @note View모드->Edit모드로 전환. 또는. Edit모드->View모드로 전환.
 */
void SettingMyPage::slot_changePageEdit(){

    this->slot_getMyInfo_loginAfter();

    int tmp_index = sender()->property("goPageIndex").toInt();
    this->stackWidget_loginAfter->setCurrentIndex(tmp_index);
}


/**
 * @brief SettingMyPage::slot_logout [SLOT] 로그아웃 버튼 클릭시
 */
void SettingMyPage::slot_logout(){

    DialogConfirm *dlg = new DialogConfirm(this);
    dlg->setTitle(tr("Log-out"));
    dlg->setText(tr("Are you sure you want to log out?"));
    /*
     *     dlg->setTitle(tr("로그아웃"));
    dlg->setText(tr("로그아웃 하시겠습니까?"));
    */
    int result = dlg->exec();
    if(result == QDialog::Accepted){

        this->setLogout();
    }
}


/**
 * @brief SettingMyPage::setLogout 로그아웃 처리
 */
void SettingMyPage::setLogout(){

    // 1) global 변수 초기화
    global.user.setDataUser(QJsonObject());

    // 2) DB 초기화
    this->logoutDBClear();

    // 3) 로즈기기에 사용자 정보 로그아웃 처리
    this->requestSetRoseUserInfo("", "", "");

    // 4) 페이지 리로드
    this->setDataMyPage();

    // 5) 시그널 발생
    emit signal_changeMyInfoData();
}


/**
 * @brief SettingMyPage::requestSetRoseUserInfo 로즈기기에 사용자 정보 로그아웃 처리
 * @param p_userName
 * @param p_email
 * @param p_accesskey
 */
void SettingMyPage::requestSetRoseUserInfo(QString p_userName, QString p_email, QString p_accesskey){

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject json;
    json.insert("accesskey", p_accesskey);
    json.insert("email", p_email);
    json.insert("userName", p_userName);

    network->request(HTTP_SET_ROSE_USER_INFO
                     , QString("http://%1:%2/set_rose_user_info").arg(global.device.getDeviceIP()).arg(global.port)
                     , json
                     , true
                     , true);
}


/**
 * @brief SettingMyPage::slot_saveMyInfo [SLOT] 확인 버튼 클릭시
 */
void SettingMyPage::slot_saveMyInfo(){

    bool flagOK = true;

    QString tmp_pw = QString("%1").arg(QString(QCryptographicHash::hash(le_pw->text().trimmed().toUtf8(),QCryptographicHash::Sha1).toHex()));

    QJsonObject json_member;
    if(global.user.getUsername() != this->le_nickName->text()){
        json_member.insert("name", this->le_nickName->text());
    }
    else{
        json_member.insert("name", global.user.getUsername());
    }
    json_member.insert("email", QString("%1").arg(global.user.getEmail()));//this->data_profileInfo.email );

    if(!le_pw->text().isEmpty()){
        if(le_pw->text().length() < 5)
        {
            flagOK = false;
            ToastMsg::show(this, "", tr("Please set the password to 5 digits or more"));
        }
        else{
            json_member.insert("password", tmp_pw);
            this->tmp_password = tmp_pw;
        }
    }
    json_member.insert("id", this->member_id);

    if(global.user.getUserPrivacy() == 0) {
        json_member.insert("shareType", "ALL");
    }
    else{
        json_member.insert("shareType", "PRIVATE");
    }

    if(global.user.getUserBIO() != this->le_comment->text()){
        json_member.insert("comment", this->le_comment->text());
    }
    else{
        json_member.insert("comment", global.user.getUserBIO());
    }

    QJsonObject json;
    json.insert("member", json_member);

    print_debug();
    QJsonDocument doc(json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "SettingMyPage::slot_saveMyInfo()---" << strJson;

    if(flagOK == true){
        // 회원정보 수정 PUT
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->requestPut(HTTP_SAVE_MYINFO, QString("%1/member/member").arg(global.legacy_v1), json, true);
    }

    /*
    QUrlQuery params;
    params.addQueryItem("id", QString("%1").arg(global.user.getUserNo()));
    //params.addQueryItem("accesskey", global.user.getAccess_token());

    if(global.user.getUsername() != this->le_nickName->text()){
        params.addQueryItem("name", this->le_nickName->text());
    }
    params.addQueryItem("email", QString("%1").arg(global.user.getEmail()));
    if(!le_pw->text().isEmpty()){
        params.addQueryItem("password", tmp_pw);
        this->tmp_password = tmp_pw;
    }
    if(global.user.getUserBIO() != this->le_comment->text()){
        params.addQueryItem("comment", this->le_comment->text());
    }
    params.addQueryItem("shareType", QString("%1").arg(global.user.getUserPrivacy()));
*/
   // network->request(HTTP_SAVE_MYINFO, QString("%1/member/member").arg(global.legacy_dev_v1), json_member, true, true);

}


/**
 * @brief SettingMyPage::slot_editImage [SLOT] 이미지 편집 버튼 클릭시
 */
void SettingMyPage::slot_editImage(){
//lb_profileEdit->setPixmap

    slot_addPhoto_member_Profile();
    //ToastMsg::show(this, "", "Preparing to adit image...");
    //ToastMsg::show(this, "", "이미지 편집.. 준비중..");
}


/**
 * @brief SettingMyPage::slot_deleteMyInfo [SLOT] 회원탈퇴 버튼 클릭시  global.legacy_v1
 */
void SettingMyPage::slot_deleteMyInfo(){

    DialogConfirm *dlg = new DialogConfirm(this);
    dlg->setTitle(tr("Notification"));
    //dlg->setTitle(tr("알림"));
    dlg->setText(tr("Are you sure you want to delete your account?"));
    //dlg->setText(tr("정말 탈퇴 하시겠습니까?"));
    int result = dlg->exec();

    if(result == QDialog::Accepted){

        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + "/member/member";
        QJsonObject json;

        network->requestDelete(HTTP_USER_DELETE
                         , url
                         , json
                         , true);
    }
}


/**
 * @brief SettingMyPage::slot_duplicateNickName [SOT] 중복체크 버튼 클릭시
 */
void SettingMyPage::slot_duplicateNickName(){

    QString tmp_nickName = this->le_nickName->text();

    if(this->data_profileInfo.username == tmp_nickName){
        ToastMsg::show(this, "", tr("This nickname is already in use."));
        //ToastMsg::show(this, "", tr("이미 사용중인 닉네임입니다."));
    }else{
            NetworkHttp *network = new NetworkHttp(this);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QString url = global.legacy_v1 + "/member/member/check/name?name=" + tmp_nickName;
            QJsonObject json;
            network->request(HTTP_CHECK_DUPLICATE_NICKNAME
                             , url
                             , json
                             , false
                             , true);
    }
}

void SettingMyPage::slot_getMyInfo_profileImageDelete(){
    print_debug();
    // 삭제 UI 먼저 처리
    //this->flowlayout_photos->removeWidget(tmp_sender);
    //this->flowlayout_photos->update();

    // 삭제 처리 호출 PUT
    DialogConfirm *dlg = new DialogConfirm(this);
    dlg->setTitle(tr("Notification"));
    //dlg->setTitle(tr("알림"));
    dlg->setText(tr("Are you sure you want to delete it?"));
    //dlg->setText(tr("삭제하시겠습니까?"));
    print_debug();
    int result = dlg->exec();
    if(result==QDialog::Accepted){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonArray json_array;
        json_array.append(del_imgURL_main);
        QJsonObject json_member;
        json_member.insert("profileimgs", json_array);
        QJsonObject json;
        network->requestDelete(HTTP_PUT_DELETE_PHOTO,QString("%1/member/member/file/MEMBER_PROFILE?image=%2").arg(global.legacy_v1).arg(del_imgURL_main)
                               , json, true);
    }

}

/**
 * @brief SettingMyPage::slot_deletePhoto [SLOT] 사진 삭제 버튼 클릭시
 * @param p_imgPath
 */
void SettingMyPage::slot_deletePhoto(QString p_imgPath){

    MyPhotoEdit *tmp_sender = dynamic_cast<MyPhotoEdit*>(sender());

    DialogConfirm *dlg = new DialogConfirm(this);
    dlg->setTitle(tr("Notification"));
    //dlg->setTitle(tr("알림"));
    dlg->setText(tr("Are you sure you want to delete it?"));
    //dlg->setText(tr("삭제하시겠습니까?"));
    print_debug();
    qDebug() << "p_imgPath = " << p_imgPath;
    int result = dlg->exec();
    if(result==QDialog::Accepted){

        // 삭제 UI 먼저 처리
        this->flowlayout_photos->removeWidget(tmp_sender);
        //this->flowlayout_photos->update();

        // 삭제 처리 호출 PUT
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonArray json_array;
        json_array.append(p_imgPath);
        QJsonObject json_member;
        json_member.insert("memberimage", json_array);
        QJsonObject json;
        network->requestDelete(HTTP_PUT_DELETE_PHOTO,QString("%1/member/member/file/MEMBER_IMAGE?image=%2").arg(global.legacy_v1).arg(p_imgPath)
                         , json, true);
    }
}


/**
 * @brief SettingMyPage::logoutDBClear 로그아웃 시, DB 초기화
 */
void SettingMyPage::logoutDBClear(){

    SqliteHelper *sqliteHelper = new SqliteHelper(this);
    QSqlError err = sqliteHelper->addConnectionLocal();
    if(err.type() == QSqlError::NoError){

       QString strQuery_delete = "DELETE FROM User";
       sqliteHelper->exec(strQuery_delete);
    }
}


/**
 * @brief SettingMyPage::slot_clickedViewAll_music [SLOT] View All 버튼 클릭시
 */
void SettingMyPage::slot_clickedViewAll_music(){

    // 부모에게 페이지 변경하라고 시그널 보냄
    emit this->signal_clickedViewAll_music(sender()->property(KEY_VIEWALL_MODE.toStdString().c_str()).toString());
}


/**
 * @brief SettingMyPage::slot_clickedViewAll_rosetube [SLOT] View All 버튼 클릭시
 */
void SettingMyPage::slot_clickedViewAll_rosetube(){

    QJsonObject tmp_data;
    tmp_data.insert(KEY_VIEWALL_MODE, sender()->property(KEY_VIEWALL_MODE.toStdString().c_str()).toString());
    tmp_data.insert("index", sender()->property("index").toInt());
    tmp_data.insert("type", sender()->property("type").toString());
    tmp_data.insert("title", sender()->property("title").toString());

    // 부모에게 페이지 변경하라고 시그널 보냄
    emit this->signal_clickedViewAll_rosetube(tmp_data);
}
