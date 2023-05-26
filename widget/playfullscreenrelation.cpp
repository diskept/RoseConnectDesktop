#include "widget/playfullscreenrelation.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"
#include "common/rosesettings.h"
#include "common/sqlitehelper.h"
#include "common/commonGetRedirectUrl.h"    //j220903 twitter

#include "data/datapopup.h"

#include "widget/toastmsg.h"
#include "widget/optionpopup.h"

// for Tidal
#include "tidal/ProcTidal_forOuter.h"
#include "tidal/Dialog_ChoosePlaylist_forTidal.h"

// for Qobuz Added diskept 15/12/2020
#include "qobuz/ProcQobuz_forOuter.h"
#include "qobuz/Dialog_ChoosePlaylist_forQobuz.h"

// for Bugs
#include "bugs/ProcBugs_forOuter.h"


#include <QSettings>
#include <QLabel>
#include <QVBoxLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>
#include <QPainterPath>
#include <QJsonDocument>
#include <QScrollArea>
#include <QMetaEnum>


// 현재 재생중인 곡의 타입
const QString PLAYTYPE_MUSIC = "MUSIC";                 ///< 음악
const QString PLAYTYPE_CD = "CD";                       ///< CD
const QString PLAYTYPE_VIDEO = "VIDEO";                 ///< 비디오
const QString PLAYTYPE_FM_TUNER = "FM_TUNER";           ///< 비디오
const QString PLAYTYPE_RADIO = "RADIO";                 ///< 라디오
const QString PLAYTYPE_ROSE_RADIO = "ROSE_RADIO";       ///< 라디오
const QString PLAYTYPE_ROSETUBE = "YOUTUBE";            ///< 로즈튜브
const QString PLAYTYPE_POD = "POD";                     ///< 팟캐스트
const QString PLAYTYPE_TIDAL = "TIDAL";                 ///< 타이달 음악 (비디오와 구분됨)
const QString PLAYTYPE_TIDAL_VIDEO = "TIDAL_VIDEO";     ///< 타이달 비디오
const QString PLAYTYPE_BUGS = "BUGS";                   ///< 벅스 음악 (비디오와 구분됨)
const QString PLAYTYPE_BUGS_MV = "BUGS_MV";             ///< 벅스 비디오
const QString PLAYTYPE_QOBUZ = "QOBUZ";                 ///< 코부즈 음악 (비디오와 구분됨)
const QString PLAYTYPE_QOBUZ_VIDEO = "QOBUZ_VIDEO";     ///< 코부즈 비디오
const QString PLAYTYPE_APPLE = "APPLE_MUSIC";           ///< 애플뮤직 음악 (비디오와 구분됨)
const QString PLAYTYPE_SPOTIFY = "SPOTIFY";
const QString PLAYTYPE_ROON = "ROON";
const QString PLAYTYPE_DLNA = "DLNA";
const QString PLAYTYPE_AIR = "AIRPLAY";


// 현재 재생중인 곡의 앨범 이미지 사이즈
const int PAGE_W_MIN = 945;
const int IMG_W_MAX = 640;              //550;
const int IMG_W_MIN = 350;
/// 연관 컨텐츠의 앨범이미지 사이즈
const int ALBUMIMG_SIZE = 60;

const int HTTP_GET_VIDEO_SETTING_DISPLAY = 1;
const int HTTP_SET_VIDEO_SETTING_DISPLAY = 2;
const int HTTP_GET_ROSETUBE_SUBSCRIBE = 3;
const int HTTP_SET_ROSETUBE_SUBSCRIBE = 4;

const int HTTP_NETWORK_PLAY = 99;
const int HTTP_NETWORK_TRACK_HEART_SET = 98;
const int HTTP_NETWORK_TRACK_HEART_GET = 97;
const int HTTP_NETWORK_PLAY_MUSIC = 96;
const int HTTP_NETWORK_FAVORITE_GET = 95;
const int HTTP_NETWORK_FAVORITE_ADD = 94;
const int HTTP_NETWORK_FAVORITE_DELETE = 93;
const int HTTP_NETWORK_ROSETUBE_LIST = 92;
const int HTTP_HDMI_ON_OFF = 89;

const int HTTP_GET_STAR_MUSIC = 12;
const int HTTP_SET_STAR_MUSIC = 13;
const int HTTP_GET_STAR_ROSETUBE_TRACK = 71;
const int HTTP_SET_STAR_ROSETUBE_TRACK = 72;
const int HTTP_GET_HEART_ROSETUBE = 73;
const int HTTP_SET_HEART_ROSETUBE = 74;
const int HTTP_GET_HEART_MUSIC = 80;
const int HTTP_SET_HEART_MUSIC_ADD = 81;
const int HTTP_SET_HEART_MUSIC_DELETE = 82;
const int HTTP_GET_HEART_RADIO = 55;
const int HTTP_SET_HEART_RADIO = 56;


const QString youtube_playUrl_post = "https://www.youtube.com/watch?v=";


// constants about Tidal & Bugs & Qobuz
const int SECTION_FOR_MORE_POPUP___TIDAL_track = 1;
const int SECTION_FOR_MORE_POPUP___TIDAL_video = 2;
const int SECTION_FOR_MORE_POPUP___BUGS_track = 3;
const int SECTION_FOR_MORE_POPUP___BUGS_video = 4;
const int SECTION_FOR_MORE_POPUP___QOBUZ_track = 5;
const int SECTION_FOR_MORE_POPUP___QOBUZ_video = 6;
const int SECTION_FOR_MORE_POPUP___APPLE_track = 7;


/**
 * @brief OptionPopupVideoDisplay::OptionPopupVideoDisplay 생성자
 * @param parent
 */
OptionPopupVideoDisplay::OptionPopupVideoDisplay(QWidget *parent) : QDialog(parent){


    this->setWindowFlags(Qt::Popup| Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->hide();

    this->menu = new QMenu();
    this->menu->setFixedWidth(180);
    this->menu->setContentsMargins(0,0,0,0);
    this->menu->setCursor(Qt::PointingHandCursor);
    connect(menu, SIGNAL(aboutToHide()), this, SLOT(hide()));

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(menu);
    vl_total->setSizeConstraint(QLayout::SetFixedSize); // 사이즈

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setStyleSheet("#widget_total { background-color:#333333;border-radius:5px;border:1px solid #4D4D4D; } QMenu { font-size:14px;color:#CCCCCC;background-color:transparent; } QMenu::item { padding-left:25px;padding-top:12px;padding-bottom:12px;background-color:transparent; } QMenu::item:selected { background-color:#4D4D4D; } QMenu::indicator { color:red; background-color:yellow } ");
    widget_total->setLayout(vl_total);

    QVBoxLayout *vboxlayout = new QVBoxLayout();
    vboxlayout->setContentsMargins(0,0,0,0);
    vboxlayout->setSpacing(0);
    vboxlayout->addWidget(widget_total);
    vboxlayout->setSizeConstraint(QLayout::SetFixedSize); // 사이즈
    this->setLayout(vboxlayout);

    // Action 생성
    act_bestfit = new QAction(tr("Best fit"));
    act_16_9 = new QAction(tr("16:9"));
    act_4_3 = new QAction(tr("4:3"));
    act_center = new QAction(tr("Center"));
    act_bestfit->setProperty("displayIndex", 0);
    act_16_9->setProperty("displayIndex", 1);
    act_4_3->setProperty("displayIndex", 2);
    act_center->setProperty("displayIndex", 3);
    this->menu->addAction(act_bestfit);
    this->menu->addAction(act_16_9);
    this->menu->addAction(act_4_3);
    this->menu->addAction(act_center);

    // 커넥션
    connect(act_bestfit, &QAction::triggered, this, &OptionPopupVideoDisplay::slot_setSettingValue);
    connect(act_16_9, &QAction::triggered, this, &OptionPopupVideoDisplay::slot_setSettingValue);
    connect(act_4_3, &QAction::triggered, this, &OptionPopupVideoDisplay::slot_setSettingValue);
    connect(act_center, &QAction::triggered, this, &OptionPopupVideoDisplay::slot_setSettingValue);
}


OptionPopupVideoDisplay::~OptionPopupVideoDisplay(){

    this->deleteLater();
}


/**
 * @brief OptionPopupVideoDisplay::setCurrDisplayIndex 해당 인덱스의 디스플레이 설정값으로 세팅한다.
 * @param p_index
 */
void OptionPopupVideoDisplay::setCurrDisplayIndex(int p_index){

    this->curDisplayIndex = p_index;
    // 어플과 화면이 다르게 생겨서 현재 인덱스 세팅 그리지 않음
}


/**
 * @brief OptionPopupVideoDisplay::slot_setSettingValue [SLOT] 해당 인덱스로 설정값 변경
 */
void OptionPopupVideoDisplay::slot_setSettingValue(){

    int tmp_displayIndex = sender()->property("displayIndex").toInt();

    if(global.device.getDeviceIP() != ""){
        QJsonObject tmp_json;
        tmp_json.insert("curDisplaySize", tmp_displayIndex);
        tmp_json.insert("settingType", "VIDEO_DISPLAY");

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
        network->request(HTTP_SET_VIDEO_SETTING_DISPLAY, QString("http://%1:%2/set_video_setting_value").arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true, true);
    }
}





/**
 * @brief PlayFullScreenRelation::PlayFullScreenRelation 생성자
 * @param parent
 */
PlayFullScreenRelation::PlayFullScreenRelation(QWidget *parent) : QWidget(parent)
{
    setInit();
    setUIControl();
}


PlayFullScreenRelation::~PlayFullScreenRelation(){

    this->filedownloader->deleteLater();
    this->deleteLater();
}


/**
 * @brief PlayFullScreenRelation::setInit : 초기 세팅
 */
void PlayFullScreenRelation::setInit(){

    this->linker = Linker::getInstance();

    // [비디오] 디스플레이 설정 팝업
    this->optionPopupVideoDisplay = new OptionPopupVideoDisplay();

    // 음악 연관컨텐츠 delegate
    delegateMusic = new MusicRelationDelegate(this);
    connect(delegateMusic, SIGNAL(signal_clickedBtn(int,int)), this, SLOT(slot_delegateClicked_music(int,int)));

    // RoseTube 연관컨텐츠 delegate
    delegate = new RoseTubeRelationDelegate(this);
    connect(delegate, SIGNAL(signal_clickedBtn(int,int)), this, SLOT(slot_delegateClicked_roseTube(int,int)));

    // 파일다운로더
    this->filedownloader = new FileDownloader(this);
    connect(this->filedownloader, SIGNAL (downloaded()), SLOT (slot_loadImage()));

    // 앨범 이미지 라벨
    this->pixmap_albumImg = new QPixmap();

    list_dataPopup = new QList<DataPopup*>();

    // 로즈튜브 즐겨찾기 담기 Dlg
    proc_addPlaylist_RoseTube = new Proc_AddRosetubeInPlayList(this);
    connect(proc_addPlaylist_RoseTube, SIGNAL(signal_playlistClicked(int)), this, SLOT(slot_playlistClicked_Rosetube(int)));
    connect(proc_addPlaylist_RoseTube, SIGNAL(signal_newPlaylistNameEdited(QString)), this, SLOT(slot_newPlaylistNameEdited_Rosetube(QString)));
}

/**
 * @brief PlayFullScreenRelation::setUIControl UI 세팅
 */
void PlayFullScreenRelation::setUIControl(){

    lb_fileInfo = new QLabel(this);
    lb_artistName = new QLabel(this);
    lb_albumThumb = new QLabel(this);
    lb_fileInfo->setStyleSheet("font-size:12px;color:#FFFFFF;");
    lb_artistName->setStyleSheet("font-size:20px;color:#999999;");

    lb_title = new TextLongAnimation(IMG_W_MAX, IMG_W_MIN, 32);
    lb_title->setStyleSheet("font-size:26px;color:#FFFFFF;");

    lb_albumThumb->setMinimumSize(IMG_W_MIN, IMG_W_MIN);

    // TV hdmi 버튼
    this->btn_hdmi = GSCommon::getUIBtnImg("btn_hdmi", ":/images/playfullscreen/hdmi_off.png", this->lb_albumThumb);
    this->btn_hdmi->setCursor(Qt::PointingHandCursor);
    this->btn_hdmi->setFixedSize(60,60);
    this->btn_hdmi->setVisible(false);

    QHBoxLayout *hl_star = new QHBoxLayout();
    hl_star->setContentsMargins(0,0,0,0);
    hl_star->setSpacing(0);
    hl_star->setAlignment(Qt::AlignLeft);

    this->list_lb_star = new QList<ClickableLabel*>();

    /*for(int i = 0; i < 5; i++){
        this->list_lb_star->append(new ClickableLabel());
        this->list_lb_star->at(i)->setCursor(Qt::PointingHandCursor);
        hl_star->addWidget(this->list_lb_star->at(i));
        connect(this->list_lb_star->at(i), SIGNAL(signal_clicked()), this, SLOT(slot_clickedStar()));
    }*/

    this->widget_starOnly = new QWidget();
    this->widget_starOnly->setLayout(hl_star);

    // 비디오 디스플레이 사이즈 설정
    btn_icon_video_prop = GSCommon::getUIBtnImg("btn_icon_video_prop", ":/images/playfullscreen/icon_video_prop.png");
    btn_icon_video_prop->setFixedSize(60,60);
    btn_icon_video_prop->setCursor(Qt::PointingHandCursor);

    // 구독 (로즈튜브,팟캐스트)
    btn_play_sub = GSCommon::getUIBtnImg("btn_play_sub", ":/images/playfullscreen/play_sub_off.png");
    btn_play_sub->setFixedSize(60,60);
    btn_play_sub->setCursor(Qt::PointingHandCursor);

    // 플레이리스트 담기
    btn_play_list_add_icon = GSCommon::getUIBtnImg("btn_play_list_add_icon", ":/images/play_list_add_icon.png");
    btn_play_list_add_icon->setFixedSize(60,60);
    btn_play_list_add_icon->setCursor(Qt::PointingHandCursor);

    // 하트
    btn_list_fav_icon = GSCommon::getUIBtnImg("btn_list_fav_icon", ":/images/list_fav_icon.png");
    btn_list_fav_icon->setFixedSize(60,60);
    btn_list_fav_icon->setCursor(Qt::PointingHandCursor);

    // 더보기
    btn_icon_menu = GSCommon::getUIBtnImg("btn_icon_menu", ":/images/icon_menu.png");
    btn_icon_menu->setFixedSize(60,60);
    btn_icon_menu->setCursor(Qt::PointingHandCursor);


    QHBoxLayout *hl_btns = new QHBoxLayout();
    hl_btns->setContentsMargins(0,0,0,0);
    hl_btns->setSpacing(0);
    hl_btns->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    hl_btns->addWidget(this->widget_starOnly);
    hl_btns->addStretch(5);
    hl_btns->addWidget(btn_icon_video_prop, 0, Qt::AlignRight);
    hl_btns->addWidget(btn_play_sub, 0, Qt::AlignRight);
    hl_btns->addWidget(btn_play_list_add_icon, 0, Qt::AlignRight);
    hl_btns->addWidget(btn_list_fav_icon, 0, Qt::AlignRight);
    hl_btns->addWidget(btn_icon_menu, 0, Qt::AlignRight);

    QWidget *widget_btns = new QWidget();
    widget_btns->setLayout(hl_btns);
    widget_btns->setMaximumWidth(IMG_W_MAX);

    QVBoxLayout *vl_fileInfo = new QVBoxLayout();
    vl_fileInfo->setContentsMargins(0,0,0,0);
    vl_fileInfo->setSpacing(0);
    vl_fileInfo->addWidget(lb_fileInfo, 0, Qt::AlignCenter);
    vl_fileInfo->addSpacing(10);
    vl_fileInfo->addWidget(lb_title, 0, Qt::AlignCenter);
    vl_fileInfo->addSpacing(10);
    vl_fileInfo->addWidget(lb_artistName, 0, Qt::AlignCenter);

    QWidget *widget_fileInfo = new QWidget();
    widget_fileInfo->setLayout(vl_fileInfo);
    //widget_fileInfo->setMaximumWidth(IMG_W_MAX);

    this->lb_albumThumbText = new QLabel(this->lb_albumThumb);
    this->lb_albumThumbText->setContentsMargins(30,0,30,0);
    this->lb_albumThumbText->setWordWrap(true);
    this->lb_albumThumbText->setAlignment(Qt::AlignCenter);
    this->lb_albumThumbText->setFixedSize(IMG_W_MAX, IMG_W_MAX);
    this->lb_albumThumbText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->lb_albumThumbText->setStyleSheet("font:bold;font-size:75px;color:#FFFFFF;background-color:transparent;");
    this->lb_albumThumbText->hide();

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignVCenter);   // Qt::AlignCenter 하면 안된다.. lb_albumImg의 resize와 연관
    vl_total->addWidget(widget_fileInfo);
    vl_total->addSpacing(20);
    vl_total->addWidget(this->lb_albumThumb,0, Qt::AlignHCenter);     // Qt::AlignCenter 하면 안된다.. lb_albumImg의 resize와 연관
    vl_total->addSpacing(5);
    vl_total->addWidget(widget_btns);

    widget_playInfo = new QWidget();
    widget_playInfo->setContentsMargins(0,0,0,0);
    widget_playInfo->setLayout(vl_total);

    QLabel *lb_relationTitle = new QLabel();
    lb_relationTitle->setText(tr("Associated content"));
    //lb_relationTitle->setText(tr("연관 컨텐츠"));
    lb_relationTitle->setStyleSheet("font-size:26px;color:#FFFFFF;");
    listWidget_relation = new QListWidget();
    listWidget_relation->setItemDelegate(delegate);
    listWidget_relation->setCursor(Qt::PointingHandCursor);
    listWidget_relation->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget_relation->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget_relation->setStyleSheet("border:none;");

    QVBoxLayout *vl_relationList = new QVBoxLayout();
    vl_relationList->setContentsMargins(0,20,20,0);
    vl_relationList->setSpacing(0);
    vl_relationList->addWidget(lb_relationTitle);
    vl_relationList->addSpacing(15);
    vl_relationList->addWidget(listWidget_relation,10);

    this->widget_relationList = new QWidget();
    this->widget_relationList->setLayout(vl_relationList);


    QHBoxLayout *hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(0,0,0,0);
    hl_total->setSpacing(0);
    hl_total->addStretch(2);
    hl_total->addWidget(widget_playInfo,15);
    hl_total->addStretch(2);
    hl_total->addWidget(this->widget_relationList, 14);
    hl_total->addStretch(1);
    hl_total->setSizeConstraint(QLayout::QLayout::SetMaximumSize);
    //hl_total->setAlignment(this->widget_relationList, Qt::AlignCenter); // 하면 사이즈 줄어들어버린다. 하면 안됨. 재시도 안하기위해 일부러 주석 안지움.

    QWidget *widget_scrollArea = new QWidget();
    widget_scrollArea->setContentsMargins(0,0,0,0);
    widget_scrollArea->setLayout(hl_total);

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
    connect(btn_hdmi, &QPushButton::clicked, this, &PlayFullScreenRelation::slot_clickedHDMI);
    connect(btn_icon_video_prop, &QPushButton::clicked, this, &PlayFullScreenRelation::slot_clickedVideoProp);
    connect(btn_play_sub, &QPushButton::clicked, this, &PlayFullScreenRelation::slot_clickedBtn_sub);
    connect(btn_play_list_add_icon, &QPushButton::clicked, this, &PlayFullScreenRelation::slot_clickedBtn_addCollection);
    connect(btn_list_fav_icon, &QPushButton::clicked, this, &PlayFullScreenRelation::slot_clickedBtn_fav);
    connect(btn_icon_menu, &QPushButton::clicked, this, &PlayFullScreenRelation::slot_clickedBtn_more);
}


/**
 * @brief PlayFullScreenRelation::loadImage [SLOT]
 */
void PlayFullScreenRelation::slot_loadImage()
{
    int tmp_w = this->lb_albumThumb->size().width();
    int tmp_h = this->lb_albumThumb->size().height();
    int tmp_size = 0;
    if(tmp_w <= tmp_h){
       tmp_size = tmp_w;
    }
    else{
       tmp_size = tmp_h;
    }

    //if(tmp_w > this->lb_albumThumb->sizeHint().width()){ // 가로가 커지는 경우...
    //    tmp_size = tmp_w;
    //}

    //if(tmp_size < IMG_W_MIN){ tmp_size = IMG_W_MIN; }
    //if(tmp_size > IMG_W_MAX){ tmp_size = IMG_W_MAX; }

    //bool flagLoad = this->pixmap_albumImg->loadFromData(this->filedownloader->downloadedData());
    //QImage image;
    //bool flagLoad = image.loadFromData(this->filedownloader->downloadedData());
    bool flagLoad = this->image_buf.loadFromData(this->filedownloader->downloadedData());

    if(!flagLoad){
        this->image_buf.load(":/images/def_mus_550.png");
    }
    else{
        this->pixmap_albumImg->loadFromData(this->filedownloader->downloadedData());
    }

    QPixmap pixmapIMG = QPixmap(QSize(tmp_size, tmp_size));
    pixmapIMG.fill(Qt::transparent);

    QPixmap tmp_pixmap;
    tmp_pixmap = tmp_pixmap.fromImage(this->image_buf);
    tmp_pixmap = tmp_pixmap.scaled(tmp_size, tmp_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPainter painter (&pixmapIMG);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPainterPath path;
    path.addRoundedRect(0, 0, tmp_size, tmp_size, 8, 8);

    int leftValue = (tmp_size - tmp_pixmap.width()) / 2;
    int topValue = (tmp_size - tmp_pixmap.height()) / 2;

    if(leftValue > 0 || topValue > 0){
        pixmapIMG.fill(Qt::black);
    }

    painter.setClipPath(path);
    painter.drawPixmap(leftValue, topValue, tmp_pixmap);
    painter.end();



    this->lb_albumThumb->setPixmap(pixmapIMG);

    /*if(flagLoad){
        QPixmap pixmapIMG = QPixmap(QSize(tmp_size, tmp_size));
        pixmapIMG.fill(Qt::transparent);

        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(this->image_buf);
        tmp_pixmap = tmp_pixmap.scaled(tmp_size, tmp_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QPainter painter (&pixmapIMG);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path;
        path.addRoundedRect(0, 0, tmp_size, tmp_size, 8, 8);

        int leftValue = (tmp_size - tmp_pixmap.width()) / 2;
        int topValue = (tmp_size - tmp_pixmap.height()) / 2;

        if(leftValue > 0 || topValue > 0){
            pixmapIMG.fill(Qt::black);
        }

        painter.setClipPath(path);
        painter.drawPixmap(leftValue, topValue, tmp_pixmap);
        painter.end();

        this->lb_albumThumb->setPixmap(pixmapIMG);
    }
    else{
        this->image_buf.load(":/images/def_mus_550.png");

        QPixmap pixmapIMG = QPixmap(QSize(tmp_size, tmp_size));
        pixmapIMG.fill(Qt::transparent);

        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(this->image_buf);
        tmp_pixmap = tmp_pixmap.scaled(tmp_size, tmp_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QPainter painter (&pixmapIMG);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path;
        path.addRoundedRect(0, 0, tmp_size, tmp_size, 8, 8);

        int leftValue = (tmp_size - tmp_pixmap.width()) / 2;
        int topValue = (tmp_size - tmp_pixmap.height()) / 2;

        if(leftValue > 0 || topValue > 0){
            pixmapIMG.fill(Qt::black);
        }

        painter.setClipPath(path);
        painter.drawPixmap(leftValue, topValue, tmp_pixmap);
        painter.end();

        this->lb_albumThumb->setPixmap(pixmapIMG);
    }*/

    this->btn_hdmi->setGeometry(tmp_size-btn_hdmi->width(), -1, this->btn_hdmi->width(), this->btn_hdmi->height());

    // 배경
    this->setBackgroundAlbumImg();
}

/**
 * @brief PlayFullScreenRelation::setDataJson JSON 데이터를 멤버변수에 세팅한다.
 * @param p_jsonData
 */
void PlayFullScreenRelation::setDataJson(const QJsonObject &p_jsonData){

//    QJsonDocument doc(p_jsonData);
//    QString strJson(doc.toJson(QJsonDocument::Compact));
//    qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << strJson;

    // 초기화
    this->path = "";
    this->playType = "";
    this->albumName = "";
    this->titleName = "";
    this->artistName = "";
    this->duration = 0;
    this->curPosition = 0;
    this->isFavorite = false;
    this->isHdmiOn = false;
    this->isPlaying = false;
    this->isServer = false;
    this->isShareFile = false;
    this->repeatMode = 0;
    this->shuffleMode = 0;
    this->thumbnail = "";
    this->album_id = 0;
    this->audio_id = "";
    this->artist_id = "";
    this->cntStar = 0;
    this->isSelect_radio = false;
    this->radio_url = "";
    this->isSubscribe = false;

    if(p_jsonData.contains("data")){
        QJsonObject tmp_data = p_jsonData["data"].toObject();
        if(tmp_data.contains("playType")){
            this->playType = tmp_data["playType"].toString();
            //print_debug();
            //qDebug() << "this->playType=" << this->playType;
        }
        if(tmp_data.contains("albumName")){
            this->albumName = tmp_data["albumName"].toString();
            // ------------------------------------------------------
            //  YOUTUBE 의 경우 albumName=비디오 아이디
            // ------------------------------------------------------
            if(playType==PLAYTYPE_ROSETUBE){
                rosetubeVideoId = tmp_data["albumName"].toString();
            }
        }
        if(tmp_data.contains("titleName")){
            this->titleName = tmp_data["titleName"].toString();
        }
        if(tmp_data.contains("artistName")){
            // ------------------------------------------------------
            //  YOUTUBE 의 경우 artistName=채널아이디
            // ------------------------------------------------------
            if(playType==PLAYTYPE_ROSETUBE){
                this->rosetueChannelId = tmp_data["artistName"].toString();
            }else{
                 this->artistName = tmp_data["artistName"].toString();
            }
        }
        if(tmp_data.contains("path")){
            this->path = tmp_data["path"].toString();
        }
        if(tmp_data.contains("duration")){
            this->duration = tmp_data["duration"].toString().toInt();
        }
        if(tmp_data.contains("curPosition")){
            this->curPosition = tmp_data["curPosition"].toString().toInt();
        }
        if(tmp_data.contains("isFavorite")){
            //this->isFavorite = tmp_data["isFavorite"].toBool(); // 의미가없네.. 항상 false로 넘어옴..
            // 음악 : 항상 false로 들어옴.. true여야하는데 false로 들어옴.
            // 라디오 : false인데 true로 들어오는경우도 있음..
            this->isFavorite = false; // 기본세팅 false 로 강제 하고 API 호출해서 GET 하여 재세팅하는 구조로 되어있음.

            // by sunnyfish - ref - (Job-Tidal-Video-1)
            if(this->playType == PLAYTYPE_TIDAL_VIDEO){
                // Tidal > Video :: 따로 가져올 수 있는 방법 자체가 없는 구조로 되어 있음. (현재 반환값 기준으로는. Tidal Api 사용 불가)
                //      이 필드로 처리하는 구조임
                this->isFavorite = ProcJsonEasy::getBool(tmp_data, "isFavorite");
            }
            else if(this->playType == PLAYTYPE_QOBUZ_VIDEO)     // TODO :: Added diskept 15/12/2020
            {
                this->isFavorite = ProcJsonEasy::getBool(tmp_data, "isFavorite");
            }
        }
        if(tmp_data.contains("isFile")){
        }
        if(tmp_data.contains("isHdmiOn")){
            this->isHdmiOn = tmp_data["isHdmiOn"].toBool();
        }
        if(tmp_data.contains("isPlaying")){
            this->isPlaying = tmp_data["isPlaying"].toBool();
        }
        if(tmp_data.contains("isServer")){
            this->isServer = tmp_data["isServer"].toBool();
        }
        if(tmp_data.contains("repeatMode")){
            this->repeatMode = tmp_data["repeatMode"].toInt();
        }
        if(tmp_data.contains("shuffleMode")){
            this->shuffleMode = tmp_data["shuffleMode"].toInt();
        }
        if(tmp_data.contains("trackInfo")){
        }
        if(tmp_data.contains("subAppCurrentData")){
            QString tmp = tmp_data["subAppCurrentData"].toString();
            QJsonDocument doc = QJsonDocument::fromJson(tmp.toUtf8());
            QJsonObject tmp_data_subAppCurrentData = doc.object();

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
                //if(playType==PLAYTYPE_ROSE_RADIO){
                //    QString temp = tmp_array.first().toString().replace("//","/");
                //    this->thumbnail = temp.replace("https:/","https://");;
                //}else{
                    this->thumbnail = tmp_array.first().toString();
                //}
                qDebug() << "this->thumbnail=" << this->thumbnail;

            }
            if(tmp_array.count()>=3){
                QString tmp_isShareFile = tmp_array[2].toString();
                if(tmp_isShareFile.startsWith("isShareFile:")){
                    QString str_isShareFile = tmp_isShareFile.replace("isShareFile:", "");
                    this->isShareFile = str_isShareFile=="true" ? true : false;
                }
            }
            if(tmp_array.count()>=5){
                QString tmp_audioId = tmp_array[4].toString();
                if(tmp_audioId.startsWith("audioId:")){
                    this->audio_id = tmp_audioId.replace("audioId:", "");
                }
            }
        }
        if(tmp_data.contains("trackInfo")){
            this->lb_fileInfo->setText(tmp_data["trackInfo"].toString());
        }

    }
}

/**
 * @brief PlayFullScreenRelation::setUIfromData UI에 데이터를 세팅한다.
 */
void PlayFullScreenRelation::setUIfromData(){

    // 초기화
    this->listWidget_relation->clear();
    if(playType==PLAYTYPE_ROSETUBE){
        listWidget_relation->setItemDelegate(delegate);
    }
    else{
        listWidget_relation->setItemDelegate(delegateMusic);
    }

    if(this->isHdmiOn == true){
        this->btn_hdmi->setStyleSheet(this->btn_hdmi->styleSheet().replace("hdmi_off.png", "hdmi_on.png"));
    }
    else{
        this->btn_hdmi->setStyleSheet(this->btn_hdmi->styleSheet().replace("hdmi_on.png", "hdmi_off.png"));
    }

    this->lb_title->setText(this->titleName);
    this->lb_artistName->setText(this->artistName);
    QString tmp_imgURL = "";

    // 버튼 show/hide
    this->btn_hdmi->setVisible(false);
    this->btn_icon_video_prop->setVisible(false);
    this->widget_starOnly->setVisible(false);
    this->btn_play_sub->setVisible(false);
    this->btn_play_list_add_icon->setVisible(false);
    this->btn_list_fav_icon->setVisible(false);
    this->btn_icon_menu->setVisible(false);
    this->widget_relationList->setVisible(false);   // 연관 컨텐츠 hide

    if(this->playType==PLAYTYPE_MUSIC){
        if(this->thumbnail.startsWith("http")==false){
            tmp_imgURL = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(this->thumbnail);
        }else{
            tmp_imgURL = this->thumbnail;
        }
        this->widget_starOnly->setVisible(true);
        this->btn_list_fav_icon->setVisible(true);
        this->btn_icon_menu->setVisible(true);
        if(this->isShareFile==true){
            this->btn_icon_menu->setVisible(false);
        }

        // 연관 컨텐츠 가져오기
        this->widget_relationList->setVisible(true);
        this->setDataRelationFromDB_music();

    }else if(this->playType==PLAYTYPE_CD){
        if(this->thumbnail.startsWith("http")==false){
            tmp_imgURL = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(this->thumbnail);
        }
        else{
            tmp_imgURL = this->thumbnail;
        }
        this->widget_starOnly->setVisible(false);
        this->btn_list_fav_icon->setVisible(false);
        this->btn_icon_menu->setVisible(false);
        if(this->isShareFile==true){
            this->btn_icon_menu->setVisible(false);
        }

        // 연관 컨텐츠 가져오기
        //this->widget_relationList->setVisible(true);
        //this->setDataRelationFromDB_music();
    }else if(this->playType==PLAYTYPE_VIDEO){
        if(this->thumbnail.contains("jpg") || this->thumbnail.contains("jpeg")){
            if(this->thumbnail.startsWith("http")==false){
                tmp_imgURL = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(this->thumbnail);
            }else{
                tmp_imgURL = this->thumbnail;
            }
        }
        else{
            tmp_imgURL = "";
        }

        this->widget_starOnly->setVisible(true);
        this->btn_hdmi->setVisible(true);
    }else if(this->playType==PLAYTYPE_FM_TUNER){
        tmp_imgURL = QString("%1").arg(this->thumbnail);
        this->btn_hdmi->setVisible(false);

    }else if(this->playType==PLAYTYPE_RADIO || this->playType==PLAYTYPE_ROSE_RADIO){
        tmp_imgURL = QString("%1").arg(this->thumbnail);

    }else if(this->playType==PLAYTYPE_ROSETUBE){

        tmp_imgURL = QString("%1").arg(this->thumbnail);
        /*QStringList tmp_strListPath = tmp_imgURL.split(".jpg");
        QString tmp_imgType = tmp_strListPath.at(0).split("/").last();
        tmp_imgURL.replace(tmp_imgType+".jpg","sddefault.jpg");*/

        this->btn_hdmi->setVisible(true);
        this->widget_starOnly->setVisible(true);
        this->btn_play_sub->setVisible(true);  // 구독 아이콘 show
        this->btn_play_list_add_icon->setVisible(true);
        this->btn_list_fav_icon->setVisible(true);
        this->widget_relationList->setVisible(true);
        this->setDataRelationFromDB_youtube();

    }else if(this->playType==PLAYTYPE_POD){
        tmp_imgURL = QString("%1").arg(this->thumbnail);
        this->btn_play_sub->setVisible(false);

    }else if(this->playType==PLAYTYPE_TIDAL){
        this->btn_list_fav_icon->setVisible(true);
        tmp_imgURL = QString("%1").arg(this->thumbnail);
        this->btn_play_list_add_icon->setVisible(true);
        this->btn_icon_menu->setVisible(true);

    }else if(this->playType==PLAYTYPE_TIDAL_VIDEO){
        tmp_imgURL = QString("%1").arg(this->thumbnail);
        this->btn_hdmi->setVisible(true);

    }else if(this->playType==PLAYTYPE_BUGS){
        tmp_imgURL = QString("%1").arg(this->thumbnail);
        this->btn_icon_menu->setVisible(true);

    }else if(this->playType==PLAYTYPE_BUGS_MV){
        tmp_imgURL = QString("%1").arg(this->thumbnail);
        this->btn_hdmi->setVisible(true);
        this->btn_icon_menu->setVisible(true);

    }else if(this->playType==PLAYTYPE_QOBUZ){
        this->btn_list_fav_icon->setVisible(true);
        tmp_imgURL = QString("%1").arg(this->thumbnail);
        this->btn_play_list_add_icon->setVisible(true);
        this->btn_icon_menu->setVisible(true);

    }else if(this->playType==PLAYTYPE_QOBUZ_VIDEO){
        tmp_imgURL = QString("%1").arg(this->thumbnail);
        this->btn_hdmi->setVisible(true);
    }
    else if(this->playType==PLAYTYPE_APPLE){
        this->btn_list_fav_icon->setVisible(true);
        tmp_imgURL = QString("%1").arg(this->thumbnail);
        this->btn_play_list_add_icon->setVisible(true);
        this->btn_icon_menu->setVisible(true);

    }
    else if(this->playType==PLAYTYPE_SPOTIFY || this->playType==PLAYTYPE_ROON || this->playType==PLAYTYPE_DLNA || this->playType==PLAYTYPE_AIR){
        QString temp = QString("%1").arg(this->thumbnail);

        if(temp.contains("http://")){
           tmp_imgURL = temp;
        }
        else{
            tmp_imgURL = "http://" + global.device.getDeviceIP() + ":" + global.port_img + temp;
        }

        this->widget_starOnly->setVisible(false);
        this->btn_list_fav_icon->setVisible(false);
        this->btn_icon_menu->setVisible(false);
        if(this->isShareFile==true){
            this->btn_icon_menu->setVisible(false);
        }
    }


    if(this->titleName==""){
        this->lb_title->setText(tr("The selected playlist is empty."));
        //this->lb_title->setText(tr("선택한 재생 목록이 비어 있습니다."));
        this->lb_artistName->setText(this->artistName);
    }
    if(this->artistName==""){
        this->lb_artistName->setVisible(false);
    }else{
        this->lb_artistName->setVisible(true);
    }

    this->lb_albumThumbText->hide();
    if(!this->thumbnail.isEmpty()){
        qDebug() << "tmp_imgURL=" << tmp_imgURL;
        if(this->playType==PLAYTYPE_CD){
            if(this->thumbnail.contains(global.device.getDeviceIP())){
                this->filedownloader->setImageURL(QUrl(tmp_imgURL));
            }
            else{
                CommonGetRedirectUrl *redirectUrl = new CommonGetRedirectUrl("CD", tmp_imgURL);
                connect(redirectUrl, &CommonGetRedirectUrl::signal_redirectUrl, this, &PlayFullScreenRelation::slot_redirectUrl);   //j220903 twitter

                this->pixmap_albumImg = GSCommon::getUIPixmapImg(":/images/cd/cd_thum_1500.png");
            }
        }
        else{
            this->filedownloader->setImageURL(QUrl(tmp_imgURL));
        }
    }
    else{
        if(this->playType==PLAYTYPE_CD){
            this->pixmap_albumImg = GSCommon::getUIPixmapImg(":/images/cd/cd_thum_1500.png");
        }
        else if(this->playType==PLAYTYPE_RADIO){
            this->pixmap_albumImg = GSCommon::getUIPixmapImg(":/images/radio_thumd_play.png");

            QString radio_name = this->titleName;
            radio_name = radio_name.replace("_"," ");
            this->lb_albumThumbText->setText(radio_name);
            this->lb_albumThumbText->show();
        }
        else{
            this->pixmap_albumImg = GSCommon::getUIPixmapImg(":/images/def_mus_550.png");
        }

        if(this->playType == "INOUT"){//bj230518

            int tmp_page_w = this->width();
            int tmp_size = IMG_W_MIN * tmp_page_w / PAGE_W_MIN;

            if(tmp_size > IMG_W_MAX){
                tmp_size = IMG_W_MAX;
            }
            else if(tmp_size < IMG_W_MIN){
                tmp_size = IMG_W_MIN;
            }

            // 640x640 크기의 QPixmap 생성
            this->pixmap_albumImg = new QPixmap(tmp_size,tmp_size);
            pixmap_albumImg->fill(Qt::black); // 투명색으로 채우기

            // QPainter를 사용하여 그라데이션 효과 추가
            QPainter painter(pixmap_albumImg);
            QLinearGradient gradient;
            gradient.setStart(0, pixmap_albumImg->height());         // 그라데이션 시작점 (왼쪽 아래)
            gradient.setFinalStop(pixmap_albumImg->width(), 0);      // 그라데이션 끝점 (오른쪽 위)
            gradient.setColorAt(0, QColor(0, 0, 0));                    // 그라데이션 시작 색상 (검정색)
            gradient.setColorAt(1, QColor(38, 38, 46, 145));            // 그라데이션 중간 색상 (회색, 알파 145)
            gradient.setColorAt(2, QColor(0, 212, 255));                // 그라데이션 끝 색상 (파란색)
            painter.setBrush(gradient);
            painter.drawRect(0, 0, pixmap_albumImg->width(), pixmap_albumImg->height());

            QPixmap roundedPixmap(pixmap_albumImg->size());
            roundedPixmap.fill(Qt::transparent);  // 투명색으로 채우기

            painter.setRenderHint(QPainter::Antialiasing, true);  // 안티앨리어싱 활성화

            QPen borderPen(Qt::gray);
            borderPen.setWidth(1);  // 테두리의 두께 설정

            QPainterPath roundedRectPath;
            int cornerRadius = 10;  // 굴곡의 반지름 설정
            roundedRectPath.addRoundedRect(
                pixmap_albumImg->rect().adjusted(1, 1, -1, -1),
                cornerRadius, cornerRadius
            );

            painter.setPen(borderPen);
            painter.fillPath(roundedRectPath, *pixmap_albumImg);
            painter.drawPath(roundedRectPath);

            // pixmap_albumImg에 굴곡과 테두리가 있는 이미지 적용
            *pixmap_albumImg = roundedPixmap;

            if(this->titleName == "LINE IN"){

                QImage centerImage(":/images/inout_line_in_x3.png");  // 중앙에 그릴 이미지 로드

                int x = (pixmap_albumImg->width() - centerImage.width()) / 2;
                int y = (pixmap_albumImg->height() - centerImage.height()) / 2;
                painter.drawImage(x, y, centerImage);

            }
            else if(this->titleName == "COAXIAL IN"){
                QImage centerImage(":/images/inout_coax_x3.png");  // 중앙에 그릴 이미지 로드

                int x = (pixmap_albumImg->width() - centerImage.width()) / 2;
                int y = (pixmap_albumImg->height() - centerImage.height()) / 2;
                painter.drawImage(x, y, centerImage);
            }
            else if(this->titleName == "OPTICAL IN"){
                QImage centerImage(":/images/inout_opt_x3.png");  // 중앙에 그릴 이미지 로드

                int x = (pixmap_albumImg->width() - centerImage.width()) / 2;
                int y = (pixmap_albumImg->height() - centerImage.height()) / 2;
                painter.drawImage(x, y, centerImage);
            }
            else if(this->titleName == "USB IN"){
                QImage centerImage(":/images/inout_USB_IN_x3.png");  // 중앙에 그릴 이미지 로드

                int x = (pixmap_albumImg->width() - centerImage.width()) / 2;
                int y = (pixmap_albumImg->height() - centerImage.height()) / 2;
                painter.drawImage(x, y, centerImage);
            }
            else if(this->titleName == "ARC IN" || this->titleName == "eARC IN"){
                QImage centerImage(":/images/inout_HDMI_ARC_IN_x3.png");  // 중앙에 그릴 이미지 로드

                int x = (pixmap_albumImg->width() - centerImage.width()) / 2;
                int y = (pixmap_albumImg->height() - centerImage.height()) / 2;
                painter.drawImage(x, y, centerImage);
            }
            else if(this->titleName == "AES/EBU IN"){
                QImage centerImage(":/images/inout_balanced_x2.png");  // 중앙에 그릴 이미지 로드

                int height = centerImage.height();
                height -= 50;
                int x = (pixmap_albumImg->width() - centerImage.width()) / 2;
                int y = (pixmap_albumImg->height() - height) / 2;
                painter.drawImage(x, y, centerImage);
            }
        }

        // 앨범 이미지
        this->lb_albumThumb->setPixmap(*this->pixmap_albumImg);

        // 배경
        this->setBackgroundAlbumImg();
    }

    this->repaintStarUI();
    this->repaintHearUI();
    this->repaintSubscribeUI();
}

/**
 * @brief PlayFullScreenRelation::setShowHideBtnVideoSettingUI 비디오 디스플레이 설정 관련 UI 세팅
 * @param p_jsonObject
 * @note 설정 아이콘 show/hide 여부 세팅한다.
 */
void PlayFullScreenRelation::setShowHideBtnVideoSettingUI(QJsonObject p_jsonObject){

    bool isVLC = false;
    int curDisplaySize = 0;
    if(p_jsonObject.contains("curDisplaySize")){
        curDisplaySize = p_jsonObject["curDisplaySize"].toInt();
    }
    if(p_jsonObject.contains("isVLC")){
        isVLC = p_jsonObject["isVLC"].toBool();
    }

    if(isVLC==true){
        this->optionPopupVideoDisplay->setCurrDisplayIndex(curDisplaySize);
        this->btn_icon_video_prop->setVisible(true);
    }else{
        this->btn_icon_video_prop->setVisible(false);
    }
}

/**
 * @brief PlayFullScreenRelation::setStarUI 별 UI 갱신
 */
void PlayFullScreenRelation::repaintStarUI(){

    for(int i=0; i<this->cntStar; i++){
        if(i < list_lb_star->size()){
            QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/star_icon_on.png");
            this->list_lb_star->at(i)->setPixmap(*pixmap_icon);
        }
    }
    for(int i=this->cntStar; i<5; i++){
        if(i>=0 && i < list_lb_star->size()){
            QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/star_icon_off.png");
            this->list_lb_star->at(i)->setPixmap(*pixmap_icon);
        }
    }
}

/**
 * @brief PlayFullScreenRelation::repaintHearUI 하트 UI 갱신
 */
void PlayFullScreenRelation::repaintHearUI(){

    if(this->isFavorite==true){
        btn_list_fav_icon->setStyleSheet(btn_list_fav_icon->styleSheet().replace("list_fav_icon.png", "list_fav_icon_on.png"));
    }else{
        btn_list_fav_icon->setStyleSheet(btn_list_fav_icon->styleSheet().replace("list_fav_icon_on.png", "list_fav_icon.png"));
    }
}

/**
 * @brief PlayFullScreenRelation::repaintSubscribeUI 구독 UI 갱신
 */
void PlayFullScreenRelation::repaintSubscribeUI(){
    if(this->isSubscribe==true){
        btn_play_sub->setStyleSheet(btn_play_sub->styleSheet().replace("play_sub_off.png", "play_sub_on.png"));
    }else{
        btn_play_sub->setStyleSheet(btn_play_sub->styleSheet().replace("play_sub_on.png", "play_sub_off.png"));
    }
}

/**
 * @brief PlayFullScreenRelation::setDataPlayFullScreen 기본정보 데이터 세팅
 * @param p_data
 */
void PlayFullScreenRelation::setDataPlayFullScreen(const QJsonObject &p_jsonData){

    try {
        // 1> JSON 풀어서 멤버변수에 데이터 세팅
        this->setDataJson(p_jsonData);

        // 2> UI에 데이터 세팅한다.
        this->setUIfromData();

        // 3> [API 호출] 비디오 설정정보 GET
        this->requestVideoSetting();

        // 4> [API 호출] 별 정보 GET
        this->requestGetStarInfo();

        // 5> [API 호출] 하트 정보 GET
        this->requestGetHartInfo();

        // 6> [API 호출] 구독 정보 GET
        this->requestGetSubInfo();
    }
    catch (...) {
    }
}


void PlayFullScreenRelation::setDataHDMIStateChange(const bool flag){

    try {
        this->isHdmiOn = flag;
        if(this->isHdmiOn == true){
            this->btn_hdmi->setStyleSheet(this->btn_hdmi->styleSheet().replace("hdmi_off.png", "hdmi_on.png"));
        }
        else{
            this->btn_hdmi->setStyleSheet(this->btn_hdmi->styleSheet().replace("hdmi_on.png", "hdmi_off.png"));
        }
    }
    catch (...) {
    }
}

/**
 * @brief PlayFullScreenRelation::getJsonObject_TrackDataCurr 현재 재생중인 음원의 정보 Json 반환 (for 하트 SET API)
 * @note 하트 SET 할때 사용함
 */
QJsonObject PlayFullScreenRelation::getJsonObject_TrackDataCurr(){

    QJsonObject jsonTrack;
    // 앨범
    jsonTrack.insert("album", this->albumName);

    // 아티스트
    QJsonObject jsonArtist;
    jsonArtist.insert("name", this->artistName);
    QJsonArray jsonArrArtists;
    jsonArrArtists.append(jsonArtist);
    jsonTrack.insert("artists", jsonArrArtists);

    // 클라이언트 아이디 = 오디오 아이디
    jsonTrack.insert("client_id", this->audio_id);

    // 썸네일
    QJsonObject jsonThumb;
    jsonThumb.insert("url", this->thumbnail);

    QJsonArray jsonArrThumbs;
    jsonArrThumbs.append(jsonThumb);
    jsonTrack.insert("thumbnails", jsonArrThumbs);

    // 타이틀
    jsonTrack.insert("title", this->titleName);

    // 재생 URL
    jsonTrack.insert("url", this->path);

    jsonTrack.insert("isFavorite", false);
    jsonTrack.insert("isSelected", false);
    jsonTrack.insert("mac_address", global.device.getDeviceID());
    jsonTrack.insert("type","music");

    return jsonTrack;
}


/**
 * @brief PlayFullScreenRelation::requestVideoSetting 비디오 설정 정보 GET 가져오는 API 호출한다.
 * @note 비디오 에서만 사용함
 */
void PlayFullScreenRelation::requestVideoSetting(){

    if(this->playType==PLAYTYPE_VIDEO){
        if(global.device.getDeviceIP()!=""){
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
            QJsonObject tmp_json;
            network->request(HTTP_GET_VIDEO_SETTING_DISPLAY, QString("http://%1:%2/get_video_setting_value").arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true, true);
        }
    }
}

/**
 * @brief PlayFullScreenRelation::requestGetSubInfo 구독 정보 GET 가져오는 API 호출한다.
 * @note 로즈튜브, 팟캐스트 에서만 사용함
 */
void PlayFullScreenRelation::requestGetSubInfo(){

    if(this->playType==PLAYTYPE_ROSETUBE){
        if(this->rosetueChannelId!="" && global.user.getUsername()!=""){
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
            QUrlQuery params;
            params.addQueryItem("channel_id",  this->rosetueChannelId);
            params.addQueryItem("username",global.user.getUsername());
            network->request(HTTP_GET_ROSETUBE_SUBSCRIBE, QString("%1/user/subscribe/get").arg(global.legacy_mod_api), params, true,true);
        }
    }else if(this->playType==PLAYTYPE_POD){



    }
}

/**
 * @brief PlayFullScreenRelation::requestGetStarInfo 별 정보 GET 하는 API 호출한다.
 * @note [중요] 음악과 로즈튜브에만 별 정보 존재한다.
 */
void PlayFullScreenRelation::requestGetStarInfo(){

    if(this->playType==PLAYTYPE_MUSIC){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
        int tmp_idInt = this->audio_id.toInt();
        QJsonObject tmp_json;
        tmp_json.insert("id", tmp_idInt);                       // 오디오 PK 또는 앨범 PK
        tmp_json.insert("isAlbum", false);                      // 오디오냐 앨범이냐

        network->request(HTTP_GET_STAR_MUSIC, QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port).arg(global.api_music_rate_get), tmp_json, true, true);

    }else if(this->playType==PLAYTYPE_ROSETUBE){

        QString playurl = youtube_playUrl_post + this->rosetubeVideoId;
        if(global.user.getUsername()!=""){
            if(this->rosetubeVideoId!=""){
                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));

                QUrlQuery params;
                params.addQueryItem(KEY_OP_ROSETUBE_PLAYURL, playurl);
                params.addQueryItem("username", global.user.getUsername());
                network->request(HTTP_GET_STAR_ROSETUBE_TRACK, QString("%1/playlist/track/get")
                                 .arg(global.legacy_mod_api), params, true,true);
            }
        }
    }
}

/**
 * @brief PlayFullScreenRelation::slot_clickedStar [SLOT] 별 점수 클릭시 처리
 * @note 별 정보 GET/SET 은....  음악(트랙/앨점)과 로즈튜브에서만 별 정보 사용한다.
 */
void PlayFullScreenRelation::slot_clickedStar(){

    int tmp_index = this->list_lb_star->indexOf(dynamic_cast<ClickableLabel*>(this->sender()));
    if(this->playType==PLAYTYPE_MUSIC){

        this->cntStar = tmp_index+1;    // 유효할때만 세팅
        this->repaintStarUI();
        int tmp_idInt = this->audio_id.toInt();

        QJsonObject tmp_json;
        tmp_json.insert("starRate", this->cntStar);
        tmp_json.insert("id", tmp_idInt);                       // 오디오 PK 또는 앨범 PK
        tmp_json.insert("isAlbum", false);                      // 오디오냐 앨범이냐
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
        network->request(HTTP_SET_STAR_MUSIC, QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port).arg(global.api_music_rate_set), tmp_json, true, true);

    }else if(this->playType==PLAYTYPE_ROSETUBE){

        QString playurl = youtube_playUrl_post + this->rosetubeVideoId;
        if(this->rosetubeVideoId!=""){
            this->cntStar = tmp_index+1;    // 유효할때만 세팅
            this->repaintStarUI();

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));

            QUrlQuery params;
            params.addQueryItem("star", QString::number(this->cntStar));
            params.addQueryItem(KEY_OP_ROSETUBE_PLAYURL, playurl);
            network->request(HTTP_SET_STAR_ROSETUBE_TRACK, QString("%1/playlist/track/star")
                             .arg(global.legacy_mod_api), params, true,true);
        }
    }
}


/**
 * @brief PlayFullScreenRelation::requestGetHartInfo 하트 정보 GET 하는 API 호출한다.
 * @note [중요] 음악, 라디오, 로즈튜브, 타이달, 벅스에만 하트 정보 존재한다.
 */
void PlayFullScreenRelation::requestGetHartInfo(){

    if(this->playType==PLAYTYPE_MUSIC){
        QJsonObject tmp_json;
        tmp_json.insert("local_ip",global.device.getDeviceIP());
        tmp_json.insert("mac_address",global.device.getDeviceID());
        tmp_json.insert("url", this->path);
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
        network->request(HTTP_GET_HEART_MUSIC, QString("%1%2").arg(global.legacy_v3_api).arg(global.api_track_favorite_get), tmp_json, true, true);

    }else if(this->playType==PLAYTYPE_RADIO){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
        QJsonObject json;
        json.insert("filter", "fav");
        network->request(HTTP_GET_HEART_RADIO, QString("http://%1:%2/get.radio.user.channels.item").arg(global.device.getDeviceIP()).arg(global.port), json, true);

    }else if(this->playType==PLAYTYPE_ROSETUBE){
        if(this->rosetubeVideoId!="" && global.user.getUsername()!=""){
            NetworkHttp *network = new NetworkHttp(this);
            connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
            QString playurl = youtube_playUrl_post + this->rosetubeVideoId;
            QUrlQuery params;
            params.addQueryItem("playurl", playurl);
            params.addQueryItem("username", global.user.getUsername());
            network->request(HTTP_GET_HEART_ROSETUBE, QString("%1/playlist/track/get").arg(global.legacy_mod_api), params, true, true);
        }
    }else if(this->playType==PLAYTYPE_TIDAL){
        // TIDAL > Track : flagFav 정보 요청 by sunnyfish (Job-Tidal-Track-1) - END
        tidal::ProcTidal_forOuter *proc = new tidal::ProcTidal_forOuter(this);
        connect(proc, &tidal::ProcTidal_forOuter::completeReq_flagFav_onTrack, this, &PlayFullScreenRelation::slot_completeReq_flagFav_onTrack);
        proc->request_flagFav_ofTrack(this->jsonObj_subAppCurrentData);

    }else if(this->playType==PLAYTYPE_TIDAL_VIDEO){

        this->repaintHearUI();

    }else if(this->playType==PLAYTYPE_BUGS){


    }else if(this->playType==PLAYTYPE_BUGS_MV){


    }else if(this->playType==PLAYTYPE_QOBUZ){
        // QOBUZ > Track : flagFav 정보 요청    Added diskept 15/12/2020
        qobuz::ProcQobuz_forOuter *proc = new qobuz::ProcQobuz_forOuter(this);
        connect(proc, &qobuz::ProcQobuz_forOuter::completeReq_flagFav_onTrack, this, &PlayFullScreenRelation::slot_completeReq_flagFav_onTrack);
        proc->request_flagFav_ofTrack(this->jsonObj_subAppCurrentData);

    }else if(this->playType==PLAYTYPE_QOBUZ_VIDEO){

        this->repaintHearUI();

    }else if(this->playType==PLAYTYPE_APPLE){
        // APPLE MUSIC FAVORITE GET
        /*qobuz::ProcQobuz_forOuter *proc = new qobuz::ProcQobuz_forOuter(this);
        connect(proc, &qobuz::ProcQobuz_forOuter::completeReq_flagFav_onTrack, this, &PlayFullScreenRelation::slot_completeReq_flagFav_onTrack);
        proc->request_flagFav_ofTrack(this->jsonObj_subAppCurrentData);*/

    }
}

/**
 * @brief PlayFullScreenRelation::slot_clickedBtn_fav [SLOT] 현재 재생중인 음원의 하트 버튼 클릭시
 * @note [중요] 음악, 라디오, 로즈튜브, 타이달, 벅스 에서만 보인다.
 */
void PlayFullScreenRelation::slot_clickedBtn_fav(){

    if(this->playType==PLAYTYPE_MUSIC){
        if(!global.device.getDeviceIP().isEmpty()){
            QJsonArray tmp_jsonArray;
            QJsonObject tmp_data = getJsonObject_TrackDataCurr();
            tmp_data.insert("type", "music");
            tmp_data.insert("url",this->path);
            tmp_data.insert("mac_address", global.device.getDeviceID());
            tmp_data.insert("isFavorite", !this->isFavorite);
            tmp_jsonArray.append(tmp_data);

            QJsonObject tmp_json;
            tmp_json.insert("local_ip",global.device.getDeviceIP());
            tmp_json.insert("mac_address",global.device.getDeviceID());
            tmp_json.insert("url",this->path);
            tmp_json.insert("favorite", !this->isFavorite);
            tmp_json.insert("tracks",tmp_jsonArray);
            if(this->isFavorite==true){
                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
                network->request(HTTP_SET_HEART_MUSIC_DELETE, QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_delete), tmp_json, true, true);

            }else{
                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
                network->request(HTTP_SET_HEART_MUSIC_ADD, QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_add), tmp_json, true, true);
            }
        }

    }else if(this->playType==PLAYTYPE_RADIO){

        // 선 UI 세팅
        this->isFavorite = !this->isFavorite;
        this->repaintHearUI();

        QJsonObject tmp_radio;
        tmp_radio.insert("favorite", this->isFavorite==true ? 1 : 0);
        tmp_radio.insert("isSelect", this->isSelect_radio);
        tmp_radio.insert("radio_nm", this->titleName);
        tmp_radio.insert("radio_url", this->radio_url);
        QJsonObject tmp_json;
        tmp_json.insert("fav", tmp_radio);
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
        network->request(HTTP_SET_HEART_RADIO, QString("http://%1:%2/set.radio.fav.url").arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true, true);

        // 시그널 보내야한다고 함.. 라디오메뉴의 즐겨찾기에 목록 동기화하기위함..
        emit linker->signal_changedRadioFavorChannel(!this->isFavorite);

    }else if(this->playType==PLAYTYPE_ROSETUBE){
        if(this->rosetubeVideoId!=""){

            // 선 UI 세팅
            this->isFavorite = !this->isFavorite;
            this->repaintHearUI();

            NetworkHttp *network = new NetworkHttp(this);
            connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
            QString playurl = youtube_playUrl_post + this->rosetubeVideoId;
            QUrlQuery params;
            params.addQueryItem("playurl", playurl);
            network->request(HTTP_SET_HEART_ROSETUBE, QString("%1/playlist/track/favorites").arg(global.legacy_mod_api), params, true, true);
        }

    }
    else if(this->playType==PLAYTYPE_TIDAL){

        tidal::ProcTidal_forOuter *proc = new tidal::ProcTidal_forOuter(this);
        proc->request_procFav_ofTrack(this->isFavorite, this->jsonObj_subAppCurrentData);

        this->isFavorite = !this->isFavorite;
        this->repaintHearUI();
    }
    else if(this->playType==PLAYTYPE_TIDAL_VIDEO){

    }
    else if(this->playType==PLAYTYPE_BUGS){

    }
    else if(this->playType==PLAYTYPE_BUGS_MV){

    }
    else if(this->playType==PLAYTYPE_QOBUZ){

        qobuz::ProcQobuz_forOuter *proc = new qobuz::ProcQobuz_forOuter(this);
        proc->request_procFav_ofTrack(this->isFavorite, this->jsonObj_subAppCurrentData);

        this->isFavorite = !this->isFavorite;
        this->repaintHearUI();
    }
    else if(this->playType==PLAYTYPE_QOBUZ_VIDEO){

    }
    else if(this->playType==PLAYTYPE_APPLE){

        /*qobuz::ProcQobuz_forOuter *proc = new qobuz::ProcQobuz_forOuter(this);
        proc->request_procFav_ofTrack(this->isFavorite, this->jsonObj_subAppCurrentData);

        this->isFavorite = !this->isFavorite;
        this->repaintHearUI();*/
    }
}

/**
 * @brief PlayFullScreenRelation::slot_clickedVideoProp [SLOT] 현재 재생중인 비디오의 디스플레이 사이즈 설정 클릭시
 */
void PlayFullScreenRelation::slot_clickedVideoProp(){

    QPoint pos = QCursor::pos();
    int tmp_posX = pos.x();
    int tmp_posY = pos.y();
    this->optionPopupVideoDisplay->move(tmp_posX,tmp_posY);
    this->optionPopupVideoDisplay->show();
}

/**
 * @brief PlayFullScreenRelation::slot_clickedHDMI [SLOT] TV HDMI 버튼 클릭시
 */
void PlayFullScreenRelation::slot_clickedHDMI(){

    if(global.device.getDeviceIP()!= ""){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
        QJsonObject tmp_json;
        tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
        network->request(HTTP_HDMI_ON_OFF, QString("http://%1:%2/hdmi_on_off").arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true, false);
    }

//    this->isHdmiOn = !this->isHdmiOn;
//    if(this->isHdmiOn == true){
//        this->btn_hdmi->setStyleSheet(this->btn_hdmi->styleSheet().replace("hdmi_off.png", "hdmi_on.png"));
//    }
//    else{
//        this->btn_hdmi->setStyleSheet(this->btn_hdmi->styleSheet().replace("hdmi_on.png", "hdmi_off.png"));
//    }
}

/**
 * @brief PlayFullScreenRelation::slot_clickedBtn_sub [SLOT] 현재 재생중인 음원의 구독 버튼 클릭시
 * @note 로즈튜브,팟케스트에서만 존재하는 구독 버튼이다.
 */
void PlayFullScreenRelation::slot_clickedBtn_sub(){
    if(this->playType==PLAYTYPE_ROSETUBE){
        if(this->rosetueChannelId!="" && global.user.getUsername()!=""){

            // 선 UI 세팅
            this->isSubscribe = !this->isSubscribe;
            this->repaintSubscribeUI();

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
            QUrlQuery params;
            params.addQueryItem("channel_id",  this->rosetueChannelId);
            params.addQueryItem("username", global.user.getUsername());
            QString tmp_url;
            if(!this->isSubscribe){
                tmp_url = QString("%1/%2").arg(global.legacy_mod_api).arg("user/subscribe/cancel");
            }else{
                tmp_url = QString("%1/%2").arg(global.legacy_mod_api).arg("user/subscribe/new");
            }
            network->request(HTTP_SET_ROSETUBE_SUBSCRIBE, tmp_url, params, true,true);
        }
    }else if(this->playType==PLAYTYPE_POD){

        ToastMsg::show(this, "", "Click Subscribe... Podcast... ");
        //ToastMsg::show(this, "", "구독 클릭... 팟캐스트... ");
    }
}

/**
 * @brief PlayFullScreenRelation::slot_clickedBtn_addCollection [SLOT] 현재 재생중인 음원의 플레이리스트 담기
 * @note [중요] 음악, 로즈튜브, 타이달, 벅스 에서만 보인다.
 */
void PlayFullScreenRelation::slot_clickedBtn_addCollection(){

    if(this->playType==PLAYTYPE_MUSIC){
        // nothing
    }else if(this->playType==PLAYTYPE_ROSETUBE){
        if(!rosetubeVideoId.isEmpty()){
            QString playurl = youtube_playUrl_post + this->rosetubeVideoId;
            proc_addPlaylist_RoseTube->setProperty("playurl", playurl);
            proc_addPlaylist_RoseTube->showDlgOfPlayList();
        }

    }else if(this->playType==PLAYTYPE_TIDAL){
        this->showDialog_toAddMyCollection();

    }else if(this->playType==PLAYTYPE_TIDAL_VIDEO){

    }else if(this->playType==PLAYTYPE_BUGS){

    }else if(this->playType==PLAYTYPE_BUGS_MV){

    }else if(this->playType==PLAYTYPE_QOBUZ){
        this->showDialog_toAddMyCollection();

    }else if(this->playType==PLAYTYPE_QOBUZ_VIDEO){

    }else if(this->playType==PLAYTYPE_APPLE){
        this->showDialog_toAddMyCollection();

    }
}

/**
 * @brief PlayFullScreenRelation::slot_clickedBtn_more [SLOT] 현재 재생중인 음원의 더보기 버튼 클릭시
 * @note 음악,타이달,벅스 에서 보임.
 */
void PlayFullScreenRelation::slot_clickedBtn_more(){

    if(this->playType==PLAYTYPE_MUSIC){

        QJsonArray jsonArr;
        DataPopup *dataPopup = new DataPopup();
        dataPopup->setAlbum(this->albumName);     // API에서 앨범명이 empty로 들어오네???
        dataPopup->setAlbum_id(this->album_id);
        dataPopup->setArtist(this->artistName);
        dataPopup->setTitle(this->titleName);
        dataPopup->setId(this->audio_id);
        jsonArr.append(dataPopup->getJsonData());

        QJsonObject jsonData;
        jsonData.insert(KEY_OP_TITLE_MAIN, "");
        jsonData.insert(KEY_OP_TITLE_SUB, "");
        jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Common_Etc_PlayFullScreenCurrTrack);
        jsonData.insert(KEY_OP_albumImg, this->thumbnail);
        jsonData.insert(KEY_OP_cntStar, 0);
        jsonData.insert(KEY_OP_DATA, jsonArr);
        jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);
        emit linker->signal_clickedHoverItem(HOVER_CODE_MORE, jsonData);

    }
    else if(this->playType==PLAYTYPE_TIDAL){
        this->makeObj_optMorePopup(OptMorePopup::Tidal_Track_inFullScreen, tidal::ProcTidal_forOuter::getConvertOptHeaderData_track(this->jsonObj_subAppCurrentData), SECTION_FOR_MORE_POPUP___TIDAL_track);
    }
    else if(this->playType==PLAYTYPE_TIDAL_VIDEO){

    }
    else if(this->playType==PLAYTYPE_BUGS){
        bool flagTrack_hasMV = bugs::ProcBugs_forOuter::extract_track_has_musicVideo(this->jsonObj_subAppCurrentData);
        OptMorePopup::PopupMenuMode tmp_popupMenuMode = flagTrack_hasMV ? OptMorePopup::Bugs_Track_inFullScreen_withVideo : OptMorePopup::Bugs_Track_inFullScreen;
        this->makeObj_optMorePopup(tmp_popupMenuMode, bugs::ProcBugs_forOuter::getConvertOptHeaderData_of_track(this->jsonObj_subAppCurrentData), SECTION_FOR_MORE_POPUP___BUGS_track);
    }
    else if(this->playType==PLAYTYPE_BUGS_MV){
        this->makeObj_optMorePopup(OptMorePopup::Bugs_Video_inFullScreen, bugs::ProcBugs_forOuter::getConvertOptHeaderData_of_video(this->jsonObj_subAppCurrentData), SECTION_FOR_MORE_POPUP___BUGS_video);
    }else if(this->playType==PLAYTYPE_QOBUZ){
        this->makeObj_optMorePopup(OptMorePopup::Qobuz_Track_inFullScreen, qobuz::ProcQobuz_forOuter::getConvertOptHeaderData_track(this->jsonObj_subAppCurrentData), SECTION_FOR_MORE_POPUP___QOBUZ_track);
    }
    else if(this->playType==PLAYTYPE_QOBUZ_VIDEO){

    }else if(this->playType==PLAYTYPE_APPLE){
        //this->makeObj_optMorePopup(OptMorePopup::Qobuz_Track_inFullScreen, qobuz::ProcQobuz_forOuter::getConvertOptHeaderData_track(this->jsonObj_subAppCurrentData), SECTION_FOR_MORE_POPUP___APPLE_track);
    }
}



/**
 * @brief PlayFullScreenRelation::slot_delegateClicked_music : [슬롯] 음악 트렉정보 delegateClick
 * @details QlistWidget item click 처리<br>
 * @param p_index
 * @param p_btnType
 */
void PlayFullScreenRelation::slot_delegateClicked_music(const int &p_index, const int &p_btnType){

    if(p_btnType == RoseTubeRelationDelegate::BtnType::etc){
        // 재생 클릭 (전체영역 클릭)
        requestPlayMusic(p_index);
    }
    else if(p_btnType == RoseTubeRelationDelegate::BtnType::trackHeart){
        // 하트 클릭
        changeTrackFavoritesUI(p_index);
        requestSetMusicTrackFavorites(p_index);
    }
    else if(p_btnType == RoseTubeRelationDelegate::BtnType::more){
        QJsonArray jsonArr;
        DataPopup *dataPopup = list_dataPopup->at(p_index);
        jsonArr.append(dataPopup->getJsonData());

        QJsonObject jsonData;
        jsonData.insert(KEY_OP_TITLE_MAIN, "");
        jsonData.insert(KEY_OP_TITLE_SUB, "");
        jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Etc_RelationMusic);
        jsonData.insert(KEY_OP_albumImg,  dataPopup->getAlbum_art());
        jsonData.insert(KEY_OP_cntStar, 0);
        jsonData.insert(KEY_OP_DATA, jsonArr);
        jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);
        jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_PLAYLISTINFO);

        emit linker->signal_clickedHoverItem(HOVER_CODE_MORE, jsonData);
    }
}


/**
 * @brief PlayFullScreenRelation::getJsonObject_TrackData : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index
 * @return QJsonObject
 */
QJsonObject PlayFullScreenRelation::getJsonObject_TrackData(const int &p_index){

    QJsonObject jsonTrack;

    if(p_index >=0 && p_index < list_dataPopup->size()){
        DataPopup *dataPopup = list_dataPopup->at(p_index);

        // 앨범
        jsonTrack.insert("album", dataPopup->getAlbum());

        // 아티스트
        QJsonObject jsonArtist;
        jsonArtist.insert("name", dataPopup->getArtist());
        QJsonArray jsonArrArtists;
        jsonArrArtists.append(jsonArtist);
        jsonTrack.insert("artists", jsonArrArtists);

        // 클라이언트 아이디 = 오디오 아이디
        jsonTrack.insert("client_id", dataPopup->getId());

        // 썸네일
        QJsonObject jsonThumb;
        jsonThumb.insert("url", dataPopup->getAlbum_art());

        QJsonArray jsonArrThumbs;
        jsonArrThumbs.append(jsonThumb);
        jsonTrack.insert("thumbnails", jsonArrThumbs);

        // 타이틀
        jsonTrack.insert("title", dataPopup->getTitle());

        // 재생 URL
        jsonTrack.insert("url", dataPopup->getData());

        jsonTrack.insert("isFavorite", false);
        jsonTrack.insert("isSelected", false);
        jsonTrack.insert("mac_address", global.device.getDeviceID());
        jsonTrack.insert("type","music");
    }


    return jsonTrack;
}

/**
 * @brief PlayFullScreenRelation::requestPlayMusic : 음악 재생
 * @param p_index
 */
void PlayFullScreenRelation::requestPlayMusic(const int &p_index){

    QJsonArray array_music;
    array_music.append(list_dataPopup->at(p_index)->getJsonData());


    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));

    QJsonObject tmp_json;
    tmp_json.insert("music", array_music);
    tmp_json.insert("musicPlayType", 15);
    tmp_json.insert("currentPosition", 0);
    tmp_json.insert("shuffle", 0);
    tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

    if(global.Queue_track_count != 0) {
        print_debug();emit linker->signal_checkQueue(27, "");

        return;
    }
    print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
    global.Queue_track_count += array_music.count();     // 220419 queue count

    network->request(HTTP_NETWORK_PLAY_MUSIC, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
}


/**
 * @brief PlayFullScreenRelation::setResultOfGetFavorite : 트랙 하트정보 요청결과 처리
 * @param p_jsonData QJsonObject response
 */
void PlayFullScreenRelation::setResultOfGetMusicFavorite(const int &p_index, const QJsonObject &p_jsonData){

    const QString jsonKey_favorite = "favorite";

    if(p_jsonData.contains(jsonKey_favorite)){
        bool tmp_favorite = p_jsonData[jsonKey_favorite].toBool();
        setTrackFavoritesUI(p_index, tmp_favorite);
    }
}

/**
 * @brief PlayFullScreenRelation::setResultOfSetMusicFavorite : 트랙 하트정보 SET 요청결과 처리
 * @param p_index int index
 * @param p_jsonData QJsonObject response
 */
void PlayFullScreenRelation::setResultOfSetMusicFavorite(const int &p_index, const QJsonObject &p_jsonData){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";

    bool tmp_flagOk = false;

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_result) && p_jsonData[jsonKey_result].toString().toLower()=="success"){
            tmp_flagOk = true;
        }
    }

    if(!tmp_flagOk){
        changeTrackFavoritesUI(p_index);
        ToastMsg::show(this,"",tr("Unable to change the Favorites."));
        //ToastMsg::show(this,"",tr("즐겨찾기를 변경할 수 없습니다."));
    }
}


/**
 * @brief PlayFullScreenRelation::setDataRelationFromDB 연관 컨텐츠 데이터 DB 쿼리 (음악용)
 */
void PlayFullScreenRelation::setDataRelationFromDB_music(){

    QList<QJsonObject> *list_jsonData = new QList<QJsonObject>();

    // 초기화
    //this->list_data.clear();
    listWidget_relation->clear();
    list_dataPopup->clear();

    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";

        // 아티스트 관련 컨텐츠
        strQuery += "SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
        strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";

        if(audio_id.isEmpty()){
            strQuery += " WHERE A.artist='%1' ";
        }else{
            strQuery += " WHERE A._id!=" + this->audio_id + " AND A.artist='%1' ";
        }
        strQuery += " ORDER BY random() ";
        strQuery += " LIMIT %2";
        strQuery = strQuery.arg(this->artistName).arg(10);
        QVariantList data;
        sqlite->exec(strQuery, data);
        if(data.size() > 0){
            for(int i = 0 ; i < data.size(); i++){
                QJsonObject tmp_jsonData = data.at(i).toJsonObject();

                DataPopup *tmp_data = new DataPopup(this);
                tmp_data->setJsonData(tmp_jsonData);

                list_dataPopup->append(tmp_data);
                list_jsonData->append(tmp_jsonData);
            }
        }

        if(this->list_dataPopup->count()<=0 || this->list_dataPopup->count()<10){

            // 없으면 : 장르 관련 컨텐츠
            strQuery = "SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            if(audio_id.isEmpty()){
                strQuery += " WHERE A.genre_id=(SELECT genre_id FROM audio_genres_map WHERE audio_id=%1) ";
            }else{
                strQuery += " WHERE A._id!=%1 AND A.genre_id=(SELECT genre_id FROM audio_genres_map WHERE audio_id=%1) ";
            }

            strQuery += " ORDER BY random() ";
            strQuery += " LIMIT %2";
            strQuery = strQuery.arg(this->audio_id).arg(10-this->list_dataPopup->count());
            QVariantList data;
            sqlite->exec(strQuery, data);
            if(data.size() > 0){
                for(int i = 0 ; i < data.size(); i++){
                    QJsonObject tmp_jsonData = data.at(i).toJsonObject();

                    DataPopup *tmp_data = new DataPopup(this);
                    tmp_data->setJsonData(tmp_jsonData);

                    list_dataPopup->append(tmp_data);
                    list_jsonData->append(tmp_jsonData);
                }
            }
        }

        if(this->list_dataPopup->count()<10){

            // 없으면 : 랜덤
            strQuery = "SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            if(audio_id.isEmpty()){
                // nothing
            }else{
                strQuery += " WHERE A._id!=" + this->audio_id;
            }
            strQuery += " ORDER BY random() ";
            strQuery += " LIMIT %1";
            strQuery = strQuery.arg(10-this->list_dataPopup->count());
            QVariantList data;
            sqlite->exec(strQuery, data);
            if(data.size() > 0){
                for(int i = 0 ; i < data.size(); i++){
                    QJsonObject tmp_jsonData = data.at(i).toJsonObject();

                    DataPopup *tmp_data = new DataPopup(this);
                    tmp_data->setJsonData(tmp_jsonData);

                    list_dataPopup->append(tmp_data);
                    list_jsonData->append(tmp_jsonData);
                }
            }
        }
    }
    sqlite->close();
    delete sqlite;

    for(int i = 0 ; i < list_jsonData->size(); i++){
        appendMusicTrack(list_jsonData->at(i));
    }
}

/**
 * @brief PlayFullScreenRelation::setDataRelationFromDB_youtube 연관 컨텐츠 데이터 쿼리 (로즈튜브용)
 */
void PlayFullScreenRelation::setDataRelationFromDB_youtube(){

    if(!rosetubeVideoId.isEmpty()){
        requestRelationVideo_youtube(rosetubeVideoId);
    }
}


/**
 * @brief PlayFullScreenRelation::requestRelationVideo_youtube
 * @param p_videoId
 */
void PlayFullScreenRelation::requestRelationVideo_youtube(const QString &p_videoId){
    if(!p_videoId.isEmpty() && (p_videoId!=rosetubeRelationVideoId
            || listWidget_relation->count() < 10)){

        rosetubeRelationVideoId = p_videoId;

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));

        QJsonObject jsonParams;
        jsonParams.insert("roseToken", global.device.getDeviceRoseToken());

        network->request(HTTP_NETWORK_ROSETUBE_LIST, QString("http://%1:%2/youtube.current.play.relate.list")
                         .arg(global.device.getDeviceIP())
                         .arg(global.port)
                         , jsonParams, false);
    }
}


/**
 * @brief PlayFullScreenRelation::appendMusicTrack : 음악 연관컨텐츠 트랙 정보 추가
 * @param p_jsonData
 */
void PlayFullScreenRelation::appendMusicTrack(const QJsonObject &p_jsonData){
    int tmp_cntItem = listWidget_relation->count();

    DataPopup *tmp_dataPopup = new DataPopup();
    tmp_dataPopup->setJsonData(p_jsonData);

    QString tmp_albumImg = "";
    if(p_jsonData.contains("album_art")){
        tmp_albumImg = p_jsonData["album_art"].toString();
    }

    QJsonObject json;
    json.insert("index", tmp_cntItem);
    json.insert("fileName", tmp_dataPopup->getTitle());
    json.insert("duration", "");
    json.insert("author", tmp_dataPopup->getArtist());
    json.insert("albumTitle", tmp_dataPopup->getAlbum());
    json.insert("title", tmp_dataPopup->getTitle());
    json.insert("isFile", true);
    json.insert("album_art", tmp_albumImg);

    QListWidgetItem *item = new QListWidgetItem;
    item->setData(Qt::UserRole, json);
    item->setData(Qt::DecorationRole, QPixmap(":images/def_mus_60.png"));
    listWidget_relation->addItem(item);

    FileDownloader *fileDownloader = new FileDownloader(this);
    fileDownloader->setProperty("index", tmp_cntItem);
    fileDownloader->setProperty("playType", PLAYTYPE_MUSIC);
    fileDownloader->setImageURL( QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_albumImg));
    connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));

    requestGetMusicTrackFavorites(tmp_cntItem);
}

/**
 * @brief PlayFullScreenRelation::setBackgroundAlbumImg
 */
void PlayFullScreenRelation::setBackgroundAlbumImg(){

    if(pixmap_albumImg!=nullptr){

        emit linker->signal_showPlayScreenBG(*pixmap_albumImg);
    }
}

/**
 * @brief PlayFullScreenRelation::resizeEvent
 * @param event
 */
void PlayFullScreenRelation::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    int tmp_page_w = this->width();
    int tmp_size = IMG_W_MIN * tmp_page_w / PAGE_W_MIN;

    if(tmp_size > IMG_W_MAX){
        tmp_size = IMG_W_MAX;
    }
    else if(tmp_size < IMG_W_MIN){
        tmp_size = IMG_W_MIN;
    }

    if(this->playType == "INOUT"){//bj230518
        // 640x640 크기의 QPixmap 생성
        this->pixmap_albumImg = new QPixmap(tmp_size,tmp_size);
        pixmap_albumImg->fill(Qt::black); // 투명색으로 채우기

        // QPainter를 사용하여 그라데이션 효과 추가
        QPainter painter(pixmap_albumImg);
        QLinearGradient gradient;
        gradient.setStart(0, pixmap_albumImg->height());         // 그라데이션 시작점 (왼쪽 아래)
        gradient.setFinalStop(pixmap_albumImg->width(), 0);      // 그라데이션 끝점 (오른쪽 위)
        gradient.setColorAt(0, QColor(0, 0, 0));                    // 그라데이션 시작 색상 (검정색)
        gradient.setColorAt(1, QColor(38, 38, 46, 145));            // 그라데이션 중간 색상 (회색, 알파 145)
        gradient.setColorAt(2, QColor(0, 212, 255));                // 그라데이션 끝 색상 (파란색)
        painter.setBrush(gradient);
        painter.drawRect(0, 0, pixmap_albumImg->width(), pixmap_albumImg->height());

        QPixmap roundedPixmap(pixmap_albumImg->size());
        roundedPixmap.fill(Qt::transparent);  // 투명색으로 채우기

        painter.setRenderHint(QPainter::Antialiasing, true);  // 안티앨리어싱 활성화

        QPen borderPen(Qt::gray);
        borderPen.setWidth(1);  // 테두리의 두께 설정

        QPainterPath roundedRectPath;
        int cornerRadius = 10;  // 굴곡의 반지름 설정
        roundedRectPath.addRoundedRect(
            pixmap_albumImg->rect().adjusted(1, 1, -1, -1),
            cornerRadius, cornerRadius
        );

        painter.setPen(borderPen);
        painter.fillPath(roundedRectPath, *pixmap_albumImg);
        painter.drawPath(roundedRectPath);

        // pixmap_albumImg에 굴곡과 테두리가 있는 이미지 적용
        *pixmap_albumImg = roundedPixmap;

        if(this->titleName == "LINE IN"){

            QImage centerImage(":/images/inout_line_in_x3.png");  // 중앙에 그릴 이미지 로드

            int x = (pixmap_albumImg->width() - centerImage.width()) / 2;
            int y = (pixmap_albumImg->height() - centerImage.height()) / 2;
            painter.drawImage(x, y, centerImage);

        }
        else if(this->titleName == "COAXIAL IN"){
            QImage centerImage(":/images/inout_coax_x3.png");  // 중앙에 그릴 이미지 로드

            int x = (pixmap_albumImg->width() - centerImage.width()) / 2;
            int y = (pixmap_albumImg->height() - centerImage.height()) / 2;
            painter.drawImage(x, y, centerImage);
        }
        else if(this->titleName == "OPTICAL IN"){
            QImage centerImage(":/images/inout_opt_x3.png");  // 중앙에 그릴 이미지 로드

            int x = (pixmap_albumImg->width() - centerImage.width()) / 2;
            int y = (pixmap_albumImg->height() - centerImage.height()) / 2;
            painter.drawImage(x, y, centerImage);
        }
        else if(this->titleName == "USB IN"){
            QImage centerImage(":/images/inout_USB_IN_x3.png");  // 중앙에 그릴 이미지 로드

            int x = (pixmap_albumImg->width() - centerImage.width()) / 2;
            int y = (pixmap_albumImg->height() - centerImage.height()) / 2;
            painter.drawImage(x, y, centerImage);
        }
        else if(this->titleName == "ARC IN" || this->titleName == "eARC IN"){
            QImage centerImage(":/images/inout_HDMI_ARC_IN_x3.png");  // 중앙에 그릴 이미지 로드

            int x = (pixmap_albumImg->width() - centerImage.width()) / 2;
            int y = (pixmap_albumImg->height() - centerImage.height()) / 2;
            painter.drawImage(x, y, centerImage);
        }
        else if(this->titleName == "AES/EBU IN"){
            QImage centerImage(":/images/inout_balanced_x2.png");  // 중앙에 그릴 이미지 로드

            int height = centerImage.height();
            height -= 50;
            int x = (pixmap_albumImg->width() - centerImage.width()) / 2;
            int y = (pixmap_albumImg->height() - height) / 2;
            painter.drawImage(x, y, centerImage);
        }

        this->lb_albumThumb->setPixmap(*pixmap_albumImg);
    }
    else{
        // 빈 Pixmap
        QPixmap pixmap_painter = QPixmap(QSize(tmp_size,tmp_size));
        pixmap_painter.fill(Qt::transparent);

        QPixmap tmp_pixmap;
        if(this->image_buf.isNull() == false){
            tmp_pixmap = tmp_pixmap.fromImage(this->image_buf);
            tmp_pixmap = tmp_pixmap.scaled(tmp_size, tmp_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        else{
            QImage img;
            if(img.load(":/images/def_mus_550.png")){
                tmp_pixmap = QPixmap::fromImage(img);                                        //이미지를 버퍼에 옮긴다
                tmp_pixmap = tmp_pixmap.scaled(tmp_size, tmp_size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            }
        }

        QPainter painter (&pixmap_painter);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path = QPainterPath();
        path.addRoundedRect(0, 0, tmp_size, tmp_size, 8, 8);

        int leftValue = (tmp_size - tmp_pixmap.width()) / 2;
        int topValue = (tmp_size - tmp_pixmap.height()) / 2;

        if(leftValue > 0 || topValue > 0){
            pixmap_painter.fill(Qt::black);
        }

        painter.setClipPath(path);
        painter.drawPixmap(leftValue, topValue, tmp_pixmap);
        painter.end();


        this->lb_albumThumb->setPixmap(pixmap_painter);
    }
    this->widget_playInfo->setFixedWidth(tmp_size);

    this->btn_hdmi->setGeometry(tmp_size-btn_hdmi->width(), -1, this->btn_hdmi->width(), this->btn_hdmi->height());

    // 화면 사이즈에 맞게 배경이미지 재세팅
    this->setBackgroundAlbumImg();
}

/**
 * @brief PlayFullScreenRelation::paintEvent
 * @param event
 */
void PlayFullScreenRelation::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  로즈튜브 연관 컨텐츠 관련
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * @brief PlayFullScreenRelation::getVideoIdFromThumbnail : 로즈튜브 썸네일에서 비디오 아이디 추출
 * @return
 * @note 로즈튜브 연관 비디오 목록 요청시 비디오 아이디값 필요.
 */
QString PlayFullScreenRelation::getVideoIdFromThumbnail(){
    QString tmp_id;
    if(!thumbnail.isEmpty()){
        QString tmp_filePath = thumbnail.split("?")[0];
        QStringList tmp_strThumbList = tmp_filePath.split("/");

        if(tmp_strThumbList.size()>1){
            tmp_id = tmp_strThumbList.at(tmp_strThumbList.size()-2);
        }
    }

    return tmp_id;
}

/**
 * @brief PlayFullScreenRelation::getVideoIdFromUrl : 로즈튜브 재생 url에서 비디오 아이디 추출
 * @param p_playUrl
 * @return
 */
QString PlayFullScreenRelation::getVideoIdFromUrl(QString p_playUrl){
    QString tmp_id;
    if(!p_playUrl.isEmpty()){
        QStringList tmp_strUrlList = p_playUrl.split("v=");
        tmp_id = p_playUrl.split("v=").last();
    }

    return  tmp_id;
}


/**
 * @brief PlayFullScreenRelation::appendRosetubeTrack : 로즈튜브 연관컨텐츠 트랙 추가
 * @param p_jsonData QjsonObject 트랙 정보
 */
void PlayFullScreenRelation::appendRosetubeTrack(const QJsonObject &p_jsonData){

    int tmp_cntItem = listWidget_relation->count();

    QString tmp_url = p_jsonData["url"].toString();
    tmp_url = tmp_url.replace("https://youtube", "https://www.youtube");

    QJsonObject json;
    json.insert("index", tmp_cntItem);
    json.insert("duration", p_jsonData["time"].toString());
    json.insert("title", p_jsonData["title"].toString());
    json.insert("favorites", "0");
    json.insert("artist", p_jsonData["channel"].toString());
    json.insert("viewCount", p_jsonData["hit"].toString());
    json.insert("url", tmp_url);
    json.insert("rose_img", p_jsonData["img"].toString());
    json.insert("rose_channel_id", "");                         // 연관 컨텐츠의 채널아이디 확인 불가

    QListWidgetItem *item = new QListWidgetItem;
    item->setData(Qt::UserRole, json);
    item->setData(Qt::DecorationRole, QPixmap(":images/no_image_video.png"));
    listWidget_relation->addItem(item);

    QPixmap pixmap;
    QString tmp_imgPath = p_jsonData["img"].toString();

    FileDownloader *fileDownloader = new FileDownloader(this);
    fileDownloader->setProperty("index", tmp_cntItem);
    fileDownloader->setProperty("playType",PLAYTYPE_ROSETUBE);
    fileDownloader->setImageURL(tmp_imgPath);
    connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));

    requestGetRosetubeTrackFavorite(tmp_cntItem, tmp_url);
}

/**
 * @brief PlayFullScreenRelation::appendRosetubeTrack_NEW : 로즈튜브 연관컨텐츠 트랙 추가
 * @param p_jsonData QjsonObject 트랙 정보
 * @note CITECH SEARCH API 사용
 */
void PlayFullScreenRelation::appendRosetubeTrack_NEW(const QJsonObject &p_jsonData){
    int tmp_cntItem = listWidget_relation->count();

    QString tmp_url = youtube_playUrl_post+p_jsonData["id"].toString();
    QString tmp_thumbnail = p_jsonData["thumbnailUrl"].toString();
    QStringList tmp_strListPath = tmp_thumbnail.split(".jpg");
    QString tmp_imgType = tmp_strListPath.at(0).split("/").last();
    //tmp_thumbnail.replace(tmp_imgType+".jpg","hqdefault.jpg");


    if(!tmp_imgType.endsWith("_live")){

        QJsonObject json;
        json.insert("index", tmp_cntItem);
        json.insert("duration", p_jsonData["duration"].toString());
        json.insert("title", p_jsonData["title"].toString());
        json.insert("favorites", "0");
        json.insert("artist", p_jsonData["channelName"].toString());

        int tmp_viewCount = p_jsonData["viewsCount"].toString().toInt();

        if(tmp_viewCount > 0){
            json.insert("viewCount", tr("Views")+" "+QLocale(QLocale::English).toString(tmp_viewCount)+tr("Count"));
            //json.insert("viewCount", tr("조회수")+" "+QLocale(QLocale::English).toString(tmp_viewCount)+tr("회"));

        }else{
            json.insert("viewCount", "");
        }
        json.insert("url", tmp_url);
        json.insert("rose_img", tmp_thumbnail);
        json.insert("rose_channel_id", "");



        QListWidgetItem *item = new QListWidgetItem;
        item->setData(Qt::UserRole, json);
        item->setData(Qt::DecorationRole, QPixmap(":/images/no_image_video.png"));
        listWidget_relation->addItem(item);

        QPixmap pixmap;
        //QString tmp_imgPath = p_jsonData["thumbnailUrl"].toString();

        FileDownloader *fileDownloader = new FileDownloader(this);
        fileDownloader->setProperty("index", tmp_cntItem);
        fileDownloader->setProperty("playType",PLAYTYPE_ROSETUBE);
        fileDownloader->setImageURL(tmp_thumbnail);
        connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));

        requestGetRosetubeTrackFavorite(tmp_cntItem, tmp_url);
    }
}

/**
 * @brief PlayFullScreenRelation::setTrackFavoritesUI : 트랙 즐겨찾기 정보 세팅
 * @param p_index
 * @param p_flagFavor
 */
void PlayFullScreenRelation::setTrackFavoritesUI(const int &p_index, const bool &p_flagFavor){
    if(p_index>=0 && p_index<listWidget_relation->count()){
        QListWidgetItem *item = listWidget_relation->item(p_index);
        QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
        map["favorites"] = p_flagFavor ? "1" : "0";
        item->setData(Qt::UserRole, map);
    }
}

/**
 * @brief PlayFullScreenRelation::changeTrackFavoritesUI : 트랙 즐겨찾기 정보 변경
 * @param p_index
 * @param p_currFavorite
 */
void PlayFullScreenRelation::changeTrackFavoritesUI(const int &p_index){
    if(p_index>=0 && p_index<listWidget_relation->count()){
        QListWidgetItem *item = listWidget_relation->item(p_index);
        QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
        map["favorites"] = map["favorites"].toString()=="1" ? "0" : "1";
        item->setData(Qt::UserRole, map);
    }
}

/**
 * @brief PlayFullScreenRelation::requestGetTrackFavorites : 트랙 하트 정보 요청
 * @param p_index
 */
void PlayFullScreenRelation::requestGetMusicTrackFavorites(const int &p_index){
    if(p_index>=0 && p_index<list_dataPopup->count()){
        if(!global.user.getAccess_token().isEmpty()){
            QJsonObject tmp_json;
            tmp_json.insert("local_ip",global.device.getDeviceIP());
            tmp_json.insert("mac_address",global.device.getDeviceID());
            tmp_json.insert("url",list_dataPopup->at(p_index)->getData());

            NetworkHttp *network = new NetworkHttp;
            network->setProperty("index", p_index);
            connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
            network->request(HTTP_NETWORK_FAVORITE_GET, QString("%1%2").arg(global.legacy_v3_api).arg(global.api_track_favorite_get), tmp_json, true, true);
        }
    }
}


/**
 * @brief PlayFullScreenRelation::requestSetTrackFavorites : 음악-트랙 즐겨찾기 세팅
 * @param p_index
 */
void PlayFullScreenRelation::requestSetMusicTrackFavorites(const int &p_index){

    if(p_index>=0 && p_index<listWidget_relation->count()){

        if(!global.user.getAccess_token().isEmpty()){

            QListWidgetItem *item = listWidget_relation->item(p_index);
            QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
            bool tmp_isFavor = map["favorites"].toBool();

            QJsonArray tmp_jsonArrTrack;
            QJsonObject tmp_jsonTrack = getJsonObject_TrackData(p_index);
            tmp_jsonTrack.insert("mac_address", global.device.getDeviceID());
            tmp_jsonTrack.insert("type", "music");
            tmp_jsonTrack.insert("url",list_dataPopup->at(p_index)->getData());
            tmp_jsonTrack.insert("isFavorite", tmp_isFavor);
            tmp_jsonArrTrack.append(tmp_jsonTrack);

            QJsonObject tmp_json;
            tmp_json.insert("local_ip",global.device.getDeviceIP());
            tmp_json.insert("mac_address",global.device.getDeviceID());
            tmp_json.insert("url",list_dataPopup->at(p_index)->getData());
            tmp_json.insert("favorite", tmp_isFavor);
            tmp_json.insert("tracks", tmp_jsonArrTrack);

            if(tmp_isFavor){
                NetworkHttp *network = new NetworkHttp;
                network->setProperty("index", p_index);
                connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
                network->request(HTTP_NETWORK_FAVORITE_ADD,  QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_add), tmp_json, true, true);
            }else{
                NetworkHttp *network = new NetworkHttp;
                network->setProperty("index", p_index);
                connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
                network->request(HTTP_NETWORK_FAVORITE_DELETE, QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_delete), tmp_json, true, true);
            }
        }
    }
}


/**
 * @brief PlayFullScreenRelation::slot_thumbnailDownloaded : [슬롯] 썸네일 다운로드 완료
 */
void PlayFullScreenRelation::slot_thumbnailDownloaded(){

    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    int tmp_index = fileDownloader->property("index").toInt();

    if(tmp_index>=0 && tmp_index<listWidget_relation->count()){
        QImage image;
        bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());
        if(flagLoaded){
            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            QString tmp_type = sender()->property("playType").toString();

            if(tmp_type==playType){
                if(tmp_type==PLAYTYPE_ROSETUBE){
                    // YOUTUBE
                    tmp_pixmap = tmp_pixmap.scaled(165,122, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }else{
                    // MUSIC
                    tmp_pixmap = tmp_pixmap.scaled(60,60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }
            }
            listWidget_relation->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
        }
    }

    fileDownloader->deleteLater();
}

/**
 * @brief PlayFullScreenRelation::slot_delegateClicked_roseTube : [슬롯] 로즈튜브 트렉정보 delegateClick
 * @details QlistWidget item click 처리<br>
 * @param p_index
 * @param p_btnType
 */
void PlayFullScreenRelation::slot_delegateClicked_roseTube(const int &p_index, const int &p_btnType){

    QListWidgetItem *item = listWidget_relation->item(p_index);

    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    int tmp_index = map["index"].toInt();
    QString tmp_favorites = map["favorites"].toString();
    QString tmp_url = map["url"].toString();
    QString tmp_channel = map["artist"].toString();
    QString tmp_imgPath = map["rose_img"].toString();
    QString tmp_channelID = map["rose_channel_id"].toString();
    QString tmp_title = map["title"].toString();
    QString tmp_videoID = getVideoIdFromUrl(tmp_url);
    QString tmp_duration = map["duration"].toString();


    if(p_btnType == RoseTubeRelationDelegate::BtnType::etc){

        // 버튼 제외 기타 여백 클릭 => 비디오 제생
        QJsonArray jsonArr;
        QJsonObject jsonTracks;
        jsonTracks.insert("id", tmp_videoID);
        jsonTracks.insert("channelId", tmp_channelID);
        jsonTracks.insert("channelName", tmp_channel);
        jsonTracks.insert("thumbnailUrl", tmp_imgPath);
        jsonTracks.insert("title", tmp_title);
        jsonTracks.insert("duration", tmp_duration);
        jsonArr.append(jsonTracks);

        // 로즈튜브 재생
        requestPlayRosetube(jsonArr);
    }
    else if(p_btnType == RoseTubeRelationDelegate::BtnType::trackHeart){

        // 하트 클릭
        changeTrackFavoritesUI(tmp_index);
        requestSetRosetubeTrackFavorite(tmp_index, tmp_url);
    }
    else if(p_btnType == RoseTubeRelationDelegate::BtnType::more){

        QJsonArray jsonArr;

        DataPopup *dataPopup = new DataPopup(this);
        dataPopup->setId(tmp_videoID);
        dataPopup->setChannelId(tmp_channelID);
        dataPopup->setTitle(tmp_title);
        dataPopup->setArtist(tmp_channel);
        dataPopup->setChannelName(tmp_channel);
        dataPopup->setData(tmp_url);
        dataPopup->setThumbnailUrl(tmp_imgPath);
        dataPopup->setstrDuration(tmp_duration);

        jsonArr.append(dataPopup->getJsonData());

        QJsonObject jsonData;
        jsonData.insert(KEY_OP_TITLE_MAIN, tmp_title);
        jsonData.insert(KEY_OP_TITLE_SUB, tmp_channel);
        jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::RoseTube_Etc_RelationVideo);
        jsonData.insert(KEY_OP_albumImg,  tmp_imgPath);
        jsonData.insert(KEY_OP_cntStar, 0);
        jsonData.insert(KEY_OP_DATA, jsonArr);
        jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::RoseTubeList);
        jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_PLAYLISTINFO);

        QJsonObject jsonData_sub;
        jsonData_sub.insert("channel_id", tmp_channelID);
        jsonData_sub.insert("title", tmp_title);
        jsonData.insert(KEY_DATA, jsonData_sub);                // for 구독정보

        emit linker->signal_clickedHoverItem(HOVER_CODE_MORE, jsonData);
    }
}

/**
 * @brief PlayFullScreenRelation::requestPlayRosetube : 로즈튜브 연관컨텐츠 재생
 * @param jsonPlayList
 */
void PlayFullScreenRelation::requestPlayRosetube(QJsonArray jsonPlayList){

    QJsonObject jsonData;
    jsonData.insert("youtubePlayType", 15);
    //jsonData.insert("youtubePlayType", playType);
    jsonData.insert("shuffle", 0);
    jsonData.insert("roseToken", global.device.getDeviceRoseToken());
    jsonData.insert("youtube", jsonPlayList);

    if(global.Queue_track_count != 0) {
        emit linker->signal_checkQueue(27, "");

        return;
    }
    emit linker->signal_queuelist_mouse_trigger_menu_flag();
    global.Queue_track_count += jsonPlayList.count();     // 220419 queue count

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
    network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3")
                     .arg(global.device.getDeviceIP())
                     .arg(global.port).arg("youtubePlay.playlist.add"), jsonData, true);

    /*NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
    network->request(HTTP_NETWORK_PLAY
                     , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("youtubePlay")
                     , jsonData
                     , true
                     , true);*/
}

/**
 * @brief PlayFullScreenRelation::requestTrackFavorite : 트랙 즐겨찾기(하트) GET
 * @param p_index
 * @param p_playUrl
 * @note 하트 정보 GET API 없음. 트랙 정보 요청하여 처리
 */
void PlayFullScreenRelation::requestGetRosetubeTrackFavorite(const int &p_index, const QString &p_playUrl){

    if(!global.user.getUsername().isEmpty()){
        //bj230515

//        NetworkHttp *network = new NetworkHttp;
//        network->setProperty("index", p_index);
//        connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));

//        QUrlQuery params;
//        params.addQueryItem("playurl", p_playUrl);
//        params.addQueryItem("username", global.user.getUsername());
//        network->request(HTTP_NETWORK_TRACK_HEART_GET, QString("%1/playlist/track/get")
//                         .arg(global.legacy_mod_api), params, true,true);
    }else{
        ToastMsg::show(this,"",tr("Loin is required."));
        //ToastMsg::show(this,"",tr("로그인이 필요합니다."));
    }
}

/**
 * @brief PlayFullScreenRelation::setResultOfGetTrackFavorites : 트랙 즐겨찾기(하트) GET 요청결과 처리
 * @param p_index int
 * @param p_jsonData QJsonObject response
 * @note 하트 GET API 없음. TRACK 정보 요청 API에서 하트 정보 GET
 */
void PlayFullScreenRelation::setResultOfGetRosetubeTrackFavorite(const int &p_index, const QJsonObject &p_jsonData){

    if(p_index>=0 && p_index<listWidget_relation->count()){
        const QString jsonKey_flagOk = "flagOk";
        const QString jsonKey_favorites = "favorites";

        if(p_jsonData.contains(jsonKey_flagOk)&&p_jsonData[jsonKey_flagOk].toBool()){
            if(p_jsonData.contains(jsonKey_favorites)){
                QString tmp_favorite = p_jsonData[jsonKey_favorites].toString();

                QListWidgetItem *item = listWidget_relation->item(p_index);
                QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
                map["favorites"] = tmp_favorite;

                item->setData(Qt::UserRole, map);
            }
        }
    }
}


/**
 * @brief PlayFullScreenRelation::requestUpdateFavorites : 트랙 즐겨찾기(하트) 업데이트
 * @param p_index
 * @param p_playUrl
 */
void PlayFullScreenRelation::requestSetRosetubeTrackFavorite(const int &p_index, const QString &p_playUrl){


    NetworkHttp *network = new NetworkHttp(this);
    network->setProperty("index", p_index);
    connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("playurl", p_playUrl);

    network->request(HTTP_NETWORK_TRACK_HEART_SET, QString("%1/playlist/track/favorites")
                     .arg(global.legacy_mod_api), params, true,true);
}

/**
 * @brief PlayFullScreenRelation::setResultOfUpdateTrackFavorites : 트랙 하트 업데이트 요청결과처리
 * @param p_jsonObject
 */
void PlayFullScreenRelation::setResultOfSetRosetubeTrackFavorites(const int &p_index, const QJsonObject &p_jsonObject){


    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";

    bool flagOk = false;

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_result) && p_jsonObject[jsonKey_result].toString().toLower()=="success"){
            flagOk = true;
        }
    }

    if(!flagOk){
        changeTrackFavoritesUI(p_index);
        ToastMsg::show(this,"",tr("Unable to change the Favorites setting."));
        //ToastMsg::show(this,"",tr("즐겨찾기 설정을 변경할 수 없습니다."));
    }

}





/**
 * @brief PlayFullScreenRelation::setResultOfGetRelationVideoList : 로즈튜브 연관컨텐츠 요청결과 처리
 * @param p_jsonData QJsonObject
 */
void PlayFullScreenRelation::setResultOfGetRelationVideoList(const QJsonObject &p_jsonData){


    listWidget_relation->clear();

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_info = "info";
    const QString jsonKey_currentPlayRelate = "currentPlayRelate";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_currentPlayRelate)){
            QJsonArray jsonArr = p_jsonData[jsonKey_currentPlayRelate].toArray();

            for(int i = 0 ; i < jsonArr.size(); i++){
                appendRosetubeTrack_NEW(jsonArr.at(i).toObject());
            }
        }
    }

}




/**
 * @brief PlayFullScreenRelation::slot_responseHttp : [슬롯] Http 요청
 * @param p_id
 * @param p_jsonData
 */
void PlayFullScreenRelation::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData){

//    QJsonDocument doc(p_jsonData);
//    QString strJson(doc.toJson(QJsonDocument::Compact));
//    qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << strJson;

    try{
        if(p_id == HTTP_GET_VIDEO_SETTING_DISPLAY){
            this->setShowHideBtnVideoSettingUI(p_jsonData);
        }
        else if(p_id == HTTP_GET_ROSETUBE_SUBSCRIBE){
            if(p_jsonData.contains("array")){
                QJsonArray jsonArray = p_jsonData["array"].toArray();
                if(jsonArray.size() > 0){
                    this->isSubscribe = true;   // 구독 정보 있음
                    this->repaintSubscribeUI();
                }
            }
        }
        else if(p_id == HTTP_SET_HEART_MUSIC_ADD){
            if(p_jsonData.contains("status")){
                if(p_jsonData["status"]=="OK"){
                    this->isFavorite = true;
                    this->repaintHearUI();
                }
            }
        }
        else if(p_id == HTTP_SET_HEART_MUSIC_DELETE){
            if(p_jsonData.contains("status")){
                if(p_jsonData["status"]=="OK"){
                    this->isFavorite = false;
                    this->repaintHearUI();
                }
            }
        }
        else if(p_id == HTTP_GET_HEART_MUSIC){
            if(p_jsonData.contains("favorite")){
                this->isFavorite = p_jsonData["favorite"].toBool();
                this->repaintHearUI();
            }
        }
        else if(p_id == HTTP_GET_STAR_MUSIC){
            if(p_jsonData.contains("starRate")){
                this->cntStar = p_jsonData["starRate"].toInt();
                this->repaintStarUI();
            }
        }
        else if(p_id == HTTP_GET_HEART_RADIO){
            if(p_jsonData.contains("arr")){
                QJsonArray tmp_arr = p_jsonData["arr"].toArray();
                for(int i=0; i<tmp_arr.count(); i++){
                    QJsonObject tmp_data = tmp_arr.at(i).toObject();
                    if(tmp_data.contains("radio_nm")){
                        if(this->titleName == tmp_data["radio_nm"].toString()){
                            this->isFavorite = true;
                            this->repaintHearUI();
                        }
                    }
                    if(tmp_data.contains("isSelect")){
                        this->isSelect_radio = tmp_data["isSelect"].toBool();
                    }
                    if(tmp_data.contains("radio_url")){
                        this->radio_url = tmp_data["radio_url"].toString();
                    }
                }
            }
        }
        else if(p_id == HTTP_GET_HEART_ROSETUBE){
            if(p_jsonData.contains("favorites")){
                QString tmp_favorites = p_jsonData["favorites"].toString();
                this->isFavorite = tmp_favorites=="1" ? true : false;
                this->repaintHearUI();
            }
        }
        else if(p_id == HTTP_GET_STAR_ROSETUBE_TRACK){
            if(p_jsonData.contains("star")){
                this->cntStar = p_jsonData["star"].toInt();
                this->repaintStarUI();
            }
        }
        else if(p_id == HTTP_NETWORK_TRACK_HEART_SET){
            setResultOfSetRosetubeTrackFavorites(sender()->property("index").toInt(), p_jsonData);
        }
        else if(p_id == HTTP_NETWORK_TRACK_HEART_GET){
            setResultOfGetRosetubeTrackFavorite(sender()->property("index").toInt(), p_jsonData);
        }
        else if(p_id == HTTP_NETWORK_FAVORITE_GET){
            setResultOfGetMusicFavorite(sender()->property("index").toInt(), p_jsonData);
        }
        else if(p_id == HTTP_NETWORK_FAVORITE_ADD || p_id == HTTP_NETWORK_FAVORITE_DELETE){
            setResultOfSetMusicFavorite(sender()->property("index").toInt(), p_jsonData);
        }
        else if(p_id == HTTP_NETWORK_ROSETUBE_LIST){
            setResultOfGetRelationVideoList(p_jsonData);
        }
        else if(p_id == HTTP_HDMI_ON_OFF){
//            QJsonDocument doc(p_jsonData);
//            QString strJson(doc.toJson(QJsonDocument::Compact));
//            qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << strJson;
        }

        sender()->deleteLater();
    }
    catch (...) {
        sender()->deleteLater();
    }
}








//------------------------------------------------------------------------------------------------------------------------
//
// MARK : for Tidal, Bugs, Qobuz
//
//------------------------------------------------------------------------------------------------------------------------

// Tidal - Favorite  ------------------------------------------------------------------------------------------------------

/**
 * @brief Tidal > Track : Fav 기본값 세팅함
 * @details @see requestGetHartInfo 함수에서 호출되는 ProcTidal_forOuter 클래스의 signal 연결된 Slot 함수임.
 * @param track_id
 * @param flagFavOn
 */
void PlayFullScreenRelation::slot_completeReq_flagFav_onTrack(const int track_id, const bool flagFavOn){

    // Tidal Track 인지, track_id 가 맞는지 체크 필요함. signal-slot 처리가 async이므로, 처리 도중에 컨텐츠가 바뀌었을 수 있음.
    if(this->playType==PLAYTYPE_TIDAL){
        int curr_track_id = tidal::ProcTidal_forOuter::extract_track_id_onTrackData(this->jsonObj_subAppCurrentData);
        if(curr_track_id == track_id){
            // 맞는 경우에만 flagFav 처리함
            this->isFavorite = flagFavOn;
            this->repaintHearUI();
        }
    }
    // Qobuz Track 인지, track_id 가 맞는지 체크 필요함. signal-slot 처리가 async이므로, 처리 도중에 컨텐츠가 바뀌었을 수 있음.  Added diskept 15/12/2020
    else if(this->playType==PLAYTYPE_QOBUZ){
        int curr_track_id = qobuz::ProcQobuz_forOuter::extract_track_id_onTrackData(this->jsonObj_subAppCurrentData);
        if(curr_track_id == track_id){
            // 맞는 경우에만 flagFav 처리함
            this->isFavorite = flagFavOn;
            this->repaintHearUI();
        }
    }
}

/**
 * @brief Tidal > Video : Fav 기본값 세팅함
 * @details @see requestGetHartInfo 함수에서 호출되는 ProcTidal_forOuter 클래스의 signal 연결된 Slot 함수임.
 * @param video_id
 * @param flagFavOn
 */
void PlayFullScreenRelation::slot_completeReq_flagFav_onVideo(const int video_id, const bool flagFavOn){
    if(this->playType==PLAYTYPE_TIDAL_VIDEO){
        int curr_video_id = tidal::ProcTidal_forOuter::extract_video_id_onVideoData(this->jsonObj_subAppCurrentData);
        if(curr_video_id == video_id){
            // 맞는 경우에만 flagFav 처리함
            this->isFavorite = flagFavOn;
            this->repaintHearUI();
        }
    }
    else if(this->playType==PLAYTYPE_QOBUZ_VIDEO){
        int curr_video_id = qobuz::ProcQobuz_forOuter::extract_video_id_onVideoData(this->jsonObj_subAppCurrentData);
        if(curr_video_id == video_id){
            // 맞는 경우에만 flagFav 처리함
            this->isFavorite = flagFavOn;
            this->repaintHearUI();
        }
    }
}




// Tidal - Add Collection  ------------------------------------------------------------------------------------------------------

/**
 * @brief 타이달 > 플레이리스트 담기 다이얼로그 띄우기
 * @detail @see AbstractTidalSubWidget 클래스의 @see showDialog_toAddMyCollection 함수 참고.
 * 함수명은 똑같이 구성하였지만, parameter와 함수 body 가 상이함. 외부 래스에서 사용하는 것이니, 영향도를 최소화 하기 위함.
 * @param content_id
 * @param content_type
 */
void PlayFullScreenRelation::showDialog_toAddMyCollection(){
    int curr_track_id = tidal::ProcTidal_forOuter::extract_track_id_onTrackData(this->jsonObj_subAppCurrentData);

    // 플레이리스트 담기 다이얼로그 띄우기 - 여기에선 Track에 대한 것만 처리하면 됨
    tidal::Dialog_ChoosePlaylist_forTidal *dialog_chooslPlaylist = new tidal::Dialog_ChoosePlaylist_forTidal(this);
    dialog_chooslPlaylist->set_contentData(QVariant(curr_track_id), tidal::Dialog_ChoosePlaylist_forTidal::ItemType_forAddCollection::Track);
    connect(dialog_chooslPlaylist, &tidal::Dialog_ChoosePlaylist_forTidal::signal_choosePlaylist_forTrack, this, &PlayFullScreenRelation::slot_add_choosePlaylist_forTrack);
    dialog_chooslPlaylist->exec();

    // TODO :: Qobuz contents Additional Request  ... 15/12/2020
}

/**
 * @brief Tidal Track 에 대하여 플레이리스트 담기 처리 요청. 참고로 응답 없음. (Tidal API가 따로 처리결과가 없음. Posting 하고 끝)
 * @param playlist_uuid_target - 사용자가 선택한 user created playlist 의 KEY
 * @param track_id - 현재 재생중인 Track 의 KEY
 */
void PlayFullScreenRelation::slot_add_choosePlaylist_forTrack(QString playlist_uuid_target, int track_id){
    // 선택한 사용자 플레이리스트에 담기 처리 요청
    tidal::ProcTidal_forOuter *proc = new tidal::ProcTidal_forOuter(this);
    connect(proc, &tidal::ProcTidal_forOuter::completeReq_add_track_in_myPlaylist, this, &PlayFullScreenRelation::slot_done_addPlaylist);
    proc->request_tidal_add_track_in_myPlaylist(playlist_uuid_target, track_id);

    // TODO :: Qobuz contents Additional Request  ... 15/12/2020
}

/**
 * @brief 타이달 > 플레이리스트 담기 처리 완료 후
 */
void PlayFullScreenRelation::slot_done_addPlaylist(){
    ToastMsg::show(this, "", tr("Added to Playlist."));       // Rose 앱과 메시지 통일
    //ToastMsg::show(this, "", tr("플레이리스트에 추가되었습니다."));       // Rose 앱과 메시지 통일
}




// Tidal & Bugs - OptMorePopup  ------------------------------------------------------------------------------------------------------

/**
 * @brief OptMorePopup을 화면에 보인다.
 * @param menuMode
 * @param data_header
 * @param section
 */
void PlayFullScreenRelation::makeObj_optMorePopup(const OptMorePopup::PopupMenuMode menuMode, const OptMorePopup::HeaderData &data_header, const int section){

    // OptMorePopup 을 띄운다.
    OptMorePopup *optPopup = new OptMorePopup(this);
    connect(optPopup, &OptMorePopup::signal_menuClicked, this, &PlayFullScreenRelation::slot_optMorePopup_menuClicked);
    optPopup->setMenuMode(menuMode, data_header, 0, section);
    optPopup->showPopup();
}

/**
 * @brief [Slot] OptMorePopup에서 발생한 이벤트(signal)에 대한 처리 (slot)
 * @param clickMode
 * @param index
 * @param section
 */
void PlayFullScreenRelation::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

    Q_UNUSED(index);

    if(section == SECTION_FOR_MORE_POPUP___TIDAL_track){
        if(clickMode == OptMorePopup::ClickMode::Go_Artist){
            // Artist Detail 페이지 진입 - AbstractMainContent slot 연결 (우선 첫번째 artist로 바로 이동)
            QJsonObject jsonObj_move = tidal::ProcTidal_forOuter::makeJsonObj_toMovePage_artistDetail_fromTrack(this->jsonObj_subAppCurrentData);
            if(jsonObj_move.count() > 0){
                emit this->signal_mustChangeMainMenuAndMovePage(GSCommon::MainMenuCode::Tidal, jsonObj_move);
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Album){
            // Album Detail 페이지 진입 - AbstractMainContent slot 연결
            QJsonObject jsonObj_move = tidal::ProcTidal_forOuter::makeJsonObj_toMovePage_albumDetail_fromTrack(this->jsonObj_subAppCurrentData);
            if(jsonObj_move.count() > 0){
                emit this->signal_mustChangeMainMenuAndMovePage(GSCommon::MainMenuCode::Tidal, jsonObj_move);
            }
        }
    }
    else if(section == SECTION_FOR_MORE_POPUP___TIDAL_video){
        // do nothing (현재는 없음)

    }
    else if(section == SECTION_FOR_MORE_POPUP___BUGS_track){
        if(clickMode == OptMorePopup::ClickMode::Go_Artist){
            // Artist Detail 페이지 진입 - AbstractMainContent slot 연결 (우선 첫번째 artist로 바로 이동)
            QJsonObject jsonObj_move = bugs::ProcBugs_forOuter::makeJsonObj_toMovePage_artistDetail_fromTrack(this->jsonObj_subAppCurrentData);
            if(jsonObj_move.count() > 0){
                emit this->signal_mustChangeMainMenuAndMovePage(GSCommon::MainMenuCode::Bugs, jsonObj_move);
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Album){
            // Album Detail 페이지 진입 - AbstractMainContent slot 연결
            QJsonObject jsonObj_move = bugs::ProcBugs_forOuter::makeJsonObj_toMovePage_albumDetail_fromTrack(this->jsonObj_subAppCurrentData);
            if(jsonObj_move.count() > 0){
                emit this->signal_mustChangeMainMenuAndMovePage(GSCommon::MainMenuCode::Bugs, jsonObj_move);
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Ref_Videos){
            // mvs 값이 없어서, 이런 경우가 안생긴다. Rose에서 mvs 정보를 전달해주면, 그때 구현 가능
        }
    }
    else if(section == SECTION_FOR_MORE_POPUP___BUGS_video){
        if(clickMode == OptMorePopup::ClickMode::Go_Artist){
            // Artist Detail 페이지 진입 - AbstractMainContent slot 연결 (우선 첫번째 artist로 바로 이동)
            QJsonObject jsonObj_move = bugs::ProcBugs_forOuter::makeJsonObj_toMovePage_artistDetail_fromVideo(this->jsonObj_subAppCurrentData);
            if(jsonObj_move.count() > 0){
                emit this->signal_mustChangeMainMenuAndMovePage(GSCommon::MainMenuCode::Bugs, jsonObj_move);
            }
        }
    }
    if(section == SECTION_FOR_MORE_POPUP___QOBUZ_track){
        if(clickMode == OptMorePopup::ClickMode::Go_Artist){
            // Artist Detail 페이지 진입 - AbstractMainContent slot 연결 (우선 첫번째 artist로 바로 이동)
            /*QJsonDocument doc(jsonObj_subAppCurrentData);
            QString strJson(doc.toJson(QJsonDocument::Compact));
            qDebug() << strJson;*/

            QJsonObject jsonObj_move = qobuz::ProcQobuz_forOuter::makeJsonObj_toMovePage_artistDetail_fromTrack(this->jsonObj_subAppCurrentData);
            if(jsonObj_move.count() > 0){
                emit this->signal_mustChangeMainMenuAndMovePage(GSCommon::MainMenuCode::Qobuz, jsonObj_move);
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Album){
            // Album Detail 페이지 진입 - AbstractMainContent slot 연결
            QJsonObject jsonObj_move = qobuz::ProcQobuz_forOuter::makeJsonObj_toMovePage_albumDetail_fromTrack(this->jsonObj_subAppCurrentData);
            if(jsonObj_move.count() > 0){
                emit this->signal_mustChangeMainMenuAndMovePage(GSCommon::MainMenuCode::Qobuz, jsonObj_move);
            }
        }
    }
    else if(section == SECTION_FOR_MORE_POPUP___QOBUZ_video){
        // do nothing (현재는 없음)

    }
}


/**
 * @brief PlayFullScreenRelation::slot_playlistClicked : [슬롯] 플레이리스트 선택 다이얼로그-플레이리스트 클릭
 * @param p_playlistno
 */
void PlayFullScreenRelation::slot_playlistClicked_Rosetube(const int &p_playlistno){

    if(p_playlistno >0){
        proc_addPlaylist_RoseTube->requestAddVideoInPlaylist(p_playlistno, sender()->property("playurl").toString());
    }else{
        proc_addPlaylist_RoseTube->showDlgOfNewPlayList();
    }
}

/**
 * @brief PlayFullScreenRelation::slot_newPlaylistNameEdited : [슬롯] 신규 플레이리스트 명 입력 완료
 * @param p_name
 * @note 신규 플레이리스트 생성
 */
void PlayFullScreenRelation::slot_newPlaylistNameEdited_Rosetube(QString p_name){

    proc_addPlaylist_RoseTube->requestNewVideoInPlaylist(p_name, sender()->property("playurl").toString());

}


void PlayFullScreenRelation::slot_redirectUrl(const QString url){  //j220903 twitter

    this->filedownloader->setImageURL(QUrl(url));
}

