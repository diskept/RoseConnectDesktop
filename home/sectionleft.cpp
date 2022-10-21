#include "common/gscommon.h"
#include "common/global.h"
#include "menuitem.h"
#include "sectionleft.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QPainter>
#include <QScrollArea>
#include <QVBoxLayout>

#include <common/filedownloader.h>
#include <common/networkhttp.h>

#include "widget/toastmsg.h"
#include "home/homemain.h"

const int MENU_W_MAX = 240;
const int MENU_W_MIN = 80;
const int HTTP_MYINFO_GET = 99;
const int HTTP_FRIEND_FETCH = 98;

/**
 * @brief SectionLeft::SectionLeft 생성자
 * @param parent
 */
SectionLeft::SectionLeft(QWidget *parent) : QWidget(parent)
{

    //this->setFixedWidth(MENU_W_MAX);
    this->setMaximumWidth(MENU_W_MAX);
    this->setMinimumWidth(MENU_W_MIN);

    this->list_menuItem.clear();
    this->list_profile_my.clear();
    this->list_profile_friend.clear();
    this->setUIControl();
}


SectionLeft::~SectionLeft(){

    this->deleteLater();
}


/**
 * @brief SectionLeft::setUIControl 기본 UI 세팅
 */
void SectionLeft::setUIControl(){

    linker = Linker::getInstance();//cheon211008

    this->vl_logo = new QVBoxLayout();
    this->vl_logo->setContentsMargins(0,0,0,0);
    this->vl_logo->setSpacing(0);

    this->vl_mainMenu = new QVBoxLayout();
    this->vl_mainMenu->setContentsMargins(0,0,0,0);
    this->vl_mainMenu->setSpacing(0);

    this->vl_playList_my = new QVBoxLayout();
    this->vl_playList_my->setContentsMargins(0,0,0,0);
    this->vl_playList_my->setSpacing(0);

    this->vl_playList_friend = new QVBoxLayout();
    this->vl_playList_friend->setContentsMargins(0,0,0,0);
    this->vl_playList_friend->setSpacing(0);

    QWidget *widget_logo = new QWidget();
    widget_logo->setFixedHeight(140);
    widget_logo->setContentsMargins(0,0,0,0);
    widget_logo->setStyleSheet("background-color:#000000; border-left:1px solid #303030;");

    this->lb_logo = GSCommon::getUILabelImg(":/images/rosehome/logo_img.png", widget_logo);
    this->lb_logo->setStyleSheet("background-color:tranparent; border:0px");
    this->lb_logo->setGeometry(18, 45, 125, 50);

    this->vl_logo->addWidget(widget_logo);

    // 메인메뉴명 UI
    this->appendMenuItem(tr("ROSE Home"), QString(GSCommon::MainMenuCode::RoseHome), ":/images/main_icon_home.png");
    this->appendMenuItem(tr("Music"), QString(GSCommon::MainMenuCode::Music), ":/images/main_icon_mus.png");
    //this->appendMenuItem(tr("Music (beauty)"), QString(GSCommon::MainMenuCode::Music_2), ":/images/main_icon_mus.png");
    this->appendMenuItem(tr("Video"), QString(GSCommon::MainMenuCode::Video) ,":/images/main_icon_vide.png");
    this->appendMenuItem(tr("Radio"), QString(GSCommon::MainMenuCode::Radio),":/images/main_icon_rad.png");
    this->appendMenuItem(tr("Rose Radio"), QString(GSCommon::MainMenuCode::RoseRadio),":/images/main_icon_roserad.png");
    //this->appendMenuItem(tr("RoseFM"), QString(GSCommon::MainMenuCode::RoseFM),":/images/main_icon_fm.png");
    this->appendMenuItem(tr("RoseTube"), QString(GSCommon::MainMenuCode::RoseTube),":/images/main_icon_yt.png");
    this->appendMenuItem(tr("Podcast"), QString(GSCommon::MainMenuCode::PodCast),":/images/main_icon_pot.png");
    this->appendMenuItem(tr("CD"), QString(GSCommon::MainMenuCode::CDplay),":/images/main_icon_cd.png");//c220721
    this->appendMenuItem(tr("TIDAL"), QString(GSCommon::MainMenuCode::Tidal),":/images/main_icon_tidal.png");
    this->appendMenuItem(tr("Bugs"), QString(GSCommon::MainMenuCode::Bugs),":/images/main_icon_bugs.png");
    this->appendMenuItem(tr("Qobuz"), QString(GSCommon::MainMenuCode::Qobuz) ,":/images/main_icon_qobuz.png");
    //this->appendMenuItem(tr("Apple Music"), QString(GSCommon::MainMenuCode::AppleMusic) ,":/images/main_icon_apple.png");
    //this->appendMenuItem(tr("Vibe"), QString(GSCommon::MainMenuCode::NaverVibe) ,":/images/main_icon_qobuz.png");
    //this->appendMenuItem(tr("Amazon Music"), QString(GSCommon::MainMenuCode::AmazonMusic) ,":/images/main_icon_qobuz.png");
    //this->appendMenuItem(tr("Spotify"), QString(GSCommon::MainMenuCode::Spotify),":/images/main_icon_spotify.png");
    this->appendMenuItem(tr("Setting"), QString(GSCommon::MainMenuCode::Setting),":/images/main_icon_set.png");

    // 플레이리스트 UI
    this->widget_bar_my = GSCommon::getHorizontalBar("#151515", 2);
    this->widget_bar_my->setVisible(false);

    this->widget_bar_friend = GSCommon::getHorizontalBar("#151515", 2);
    this->widget_bar_friend->setVisible(false);

    this->lb_title_playList_my = new QLabel(tr("My Playlist"));
    this->lb_title_playList_my->setContentsMargins(25,10,0,10);
    this->lb_title_playList_my->setStyleSheet("font-size:15px;color:#888888;");
    this->lb_title_playList_my->setVisible(false);

    this->lb_title_playList_friend = new QLabel(tr("Friend playlist"));
    this->lb_title_playList_friend->setContentsMargins(25,10,0,10);
    this->lb_title_playList_friend->setStyleSheet("font-size:15px;color:#888888;");
    this->lb_title_playList_friend->setVisible(false);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    //vl_total->addSpacing(45);
    //vl_total->addLayout(this->vl_logo);
    //vl_total->addSpacing(50);
    vl_total->addLayout(this->vl_mainMenu);
    vl_total->addSpacing(30);
    vl_total->addWidget(widget_bar_my);
    vl_total->addWidget(lb_title_playList_my);
    vl_total->addLayout(this->vl_playList_my);
    vl_total->addWidget(widget_bar_friend);
    vl_total->addWidget(lb_title_playList_friend);
    vl_total->addLayout(this->vl_playList_friend);
    vl_total->addSpacing(50);

    QWidget *widget_sectionLeft = new QWidget();
    widget_sectionLeft->setObjectName("widget_sectionLeft");
    widget_sectionLeft->setLayout(vl_total);
    widget_sectionLeft->setStyleSheet("#widget_sectionLeft:enabled { background-color:#000000; }");

    QVBoxLayout *vl_totalReal = new QVBoxLayout();
    vl_totalReal->setContentsMargins(0,0,0,0);
    vl_totalReal->setSpacing(0);
    vl_totalReal->addWidget(widget_sectionLeft);

    // 스크롤
    QWidget *widget_scrollArea = new QWidget();
    widget_scrollArea->setContentsMargins(0,0,0,0);
    widget_scrollArea->setObjectName("widget_scrollArea");
    widget_scrollArea->setLayout(vl_totalReal);
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
    vl_scroll->setAlignment(Qt::AlignTop);
    vl_scroll->addLayout(this->vl_logo);
    vl_scroll->addWidget(scrollArea);

    this->setLayout(vl_scroll);

    connect(linker, SIGNAL(signal_leftmenuSearch(QString)), this, SLOT(slot_setSelectMenu(QString)));//cheon211008
}


/**
 * @brief SectionLeft::appendMenuItem 메뉴아이템 추가
 * @param p_menuName
 * @param p_menuCode
 * @param p_iconPath
 */
void SectionLeft::appendMenuItem(const QString p_menuName, const QString p_menuCode, const QString p_iconPath){

    MenuItem *menuItem = new MenuItem(p_menuName, p_menuCode, p_iconPath, this);
    connect(menuItem, SIGNAL(clicked(QString)), this, SLOT(clickedMenu(QString)));

    this->list_menuItem.append(menuItem);

    this->vl_mainMenu->addWidget(menuItem);
    this->vl_mainMenu->addSpacing(5);
}


/**
 * @brief SectionLeft::clickedMenu [SLOT] 메뉴 클릭
 * @param p_menuCode
 */
void SectionLeft::clickedMenu(QString p_menuCode){
print_debug();
    //if(global.enable_section_left == false){
   if(global.abs_ani_dialog_wait->isHidden() == true){
        global.searchMenuFlag = false;//cheon211008
        global.enable_section_left = true;

        for(int i = 0; i < this->list_menuItem.count(); i++){
            this->list_menuItem[i]->setUnSelectedMenu();      // 메뉴 스타일 초기화
            if(this->list_menuItem[i]->getMenuCode()==p_menuCode){
                this->list_menuItem[i]->setSelectedMenu();
                //global.now_Section = i;    //c220321
            }
        }
print_debug();
qDebug() << "p_menuCode= " << p_menuCode;
        emit changedMenu(p_menuCode);
    }
}


void SectionLeft::slot_setSelectMenu(QString p_menuCode)//cheon211008
{

    for(int i=0; i<this->list_menuItem.count(); i++){
        this->list_menuItem[i]->setUnSelectedMenu();      // 메뉴 스타일 초기화
        if(this->list_menuItem[i]->getMenuCode()==p_menuCode){
            this->list_menuItem[i]->setSelectedMenu();
            //global.now_Section = i; //c220321
        }
    }
}

void SectionLeft::setSmallSize(){

    this->lb_logo->setGeometry(18, 45, 45, 50);
    this->setFixedWidth(MENU_W_MIN);
    //this->setMaximumWidth(MENU_W_MIN);
    lb_title_playList_my->setText(tr("My"));
    lb_title_playList_friend->setText(tr("friend"));
    /*
     *     lb_title_playList_my->setText(tr("마이"));
    lb_title_playList_friend->setText(tr("친구"));
    */
    for(int i = 0; i < this->list_menuItem.count(); i++){
        this->list_menuItem[i]->hideMenuName();
    }
    for(int i = 0; i < this->list_profile_my.count(); i++){
        this->list_profile_my[i]->setMinWidthSize();
    }
    for(int i = 0; i < this->list_profile_friend.count(); i++){
        this->list_profile_friend[i]->setMinWidthSize();
    }
}

void SectionLeft::setBigSize(){

    this->lb_logo->setGeometry(18, 45, 125, 50);

    this->setFixedWidth(MENU_W_MAX);
    //this->setMaximumWidth(MENU_W_MAX);
    lb_title_playList_my->setText(tr("My Playlist"));
    //    lb_title_playList_my->setText(tr("마이 플레이리스트"));
    lb_title_playList_friend->setText(tr("Friend Playlist"));
    //lb_title_playList_friend->setText(tr("친구 플레이리스트"));
    for(int i = 0; i < this->list_menuItem.count(); i++){
        this->list_menuItem[i]->showMenuName();
    }
    for(int i = 0; i < this->list_profile_my.count(); i++){
        this->list_profile_my[i]->setMaxWidthSize();
    }
    for(int i = 0; i < this->list_profile_friend.count(); i++){
        this->list_profile_friend[i]->setMaxWidthSize();
    }
}


/**
 * @brief SectionLeft::requestPlayList 플레이리스트 마이/친구 API 호출 요청
 * @note 로그인 정보 세팅 완료 후 외부에서 호출된다.
 */
void SectionLeft::requestPlayList(){

    if(global.user.isValid()==false){

        // ----------------------------------
        // 로그아웃 된 상태임
        // ----------------------------------
        this->lb_title_playList_my->setVisible(false);
        this->lb_title_playList_friend->setVisible(false);
        this->widget_bar_my->setVisible(false);
        this->widget_bar_friend->setVisible(false);
        this->list_profile_my.clear();
        this->list_profile_friend.clear();

        GSCommon::clearLayout(this->vl_playList_my);
        GSCommon::clearLayout(this->vl_playList_friend);
    }else{
        // ----------------------------------
        // 로그인 된 상태임
        // ----------------------------------

        // 동시에 마이 플레이, 친구 플레이 API 호출 한다.
        //if(true){
        QString userKey = "";
        userKey = global.user.getSub();

        if(!userKey.isEmpty()){
            NetworkHttp *network = new NetworkHttp();
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            QJsonObject json;
            network->request(HTTP_MYINFO_GET, QString("%1/member/member/%2")
                             .arg(global.legacy_v1).arg(userKey), json, false, true);
            /*NetworkHttp *network = new NetworkHttp();
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            QUrlQuery params;
            params.addQueryItem("username",global.user.getUsername());
            network->request(HTTP_MYINFO_GET, QString("%1/user/get")
                             .arg(global.legacy_mod_api), params, true,true);*/
        }
        /*if(true){
            NetworkHttp *network = new NetworkHttp();
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            QUrlQuery params;
            params.addQueryItem("username", global.user.getUsername());
            network->request(HTTP_FRIEND_FETCH, QString("%1/user/friend/fetch")
                             .arg(global.legacy_mod_api), params, true,true);
        }*/
    }
}


/**
 * @brief SectionLeft::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void SectionLeft::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    switch(p_id){
    case HTTP_MYINFO_GET :
        this->list_profile_my.clear();
        GSCommon::clearLayout(this->vl_playList_my);
        if(p_jsonObject.isEmpty()==false){

            QJsonObject userInfo = ProcJsonEasy::getJsonObject(p_jsonObject, "member");

            if(userInfo.contains("id")){
                if(userInfo["id"].toInt() > 0){
                    this->widget_bar_my->setVisible(true);
                    this->lb_title_playList_my->setVisible(true);
                    ProfilePlayListWidget *profileBtn = new ProfilePlayListWidget(ProfilePlayListWidget::ProfileWhoType::My);
                    connect(profileBtn, SIGNAL(signal_clickedPlayListTitle(QJsonObject)), this, SIGNAL(signal_goPagePlayListDetail(QJsonObject)));
                    this->list_profile_my.append(profileBtn);
                    //connect(profileBtn, &ProfileRowBtn::clicked, this, &SectionLeft::slot_clickedProfileRow);
                    profileBtn->setDataProfileInfo(userInfo);
                    this->vl_playList_my->addWidget(profileBtn);
                }
            }
            /*if(p_jsonObject.contains("no")){
                if(p_jsonObject["no"].toInt()>0){
                    this->widget_bar_my->setVisible(true);
                    this->lb_title_playList_my->setVisible(true);
                    ProfilePlayListWidget *profileBtn = new ProfilePlayListWidget(ProfilePlayListWidget::ProfileWhoType::My);
                    connect(profileBtn, SIGNAL(signal_clickedPlayListTitle(QJsonObject)), this, SIGNAL(signal_goPagePlayListDetail(QJsonObject)));
                    this->list_profile_my.append(profileBtn);
                    //connect(profileBtn, &ProfileRowBtn::clicked, this, &SectionLeft::slot_clickedProfileRow);
                    profileBtn->setDataProfileInfo(p_jsonObject);
                    this->vl_playList_my->addWidget(profileBtn);
                }
            }
            else if(p_jsonObject.contains("totalCount")){
                if(p_jsonObject["totalCount"].toInt()>0){
                    this->widget_bar_my->setVisible(true);
                    this->lb_title_playList_my->setVisible(true);
                    ProfilePlayListWidget *profileBtn = new ProfilePlayListWidget(ProfilePlayListWidget::ProfileWhoType::My);
                    connect(profileBtn, SIGNAL(signal_clickedPlayListTitle(QJsonObject)), this, SIGNAL(signal_goPagePlayListDetail(QJsonObject)));
                    this->list_profile_my.append(profileBtn);
                    //connect(profileBtn, &ProfileRowBtn::clicked, this, &SectionLeft::slot_clickedProfileRow);
                    profileBtn->setDataProfileInfo(p_jsonObject);
                    this->vl_playList_my->addWidget(profileBtn);
                }
            }*/
        }
        break;
    case HTTP_FRIEND_FETCH :
        this->list_profile_friend.clear();
        GSCommon::clearLayout(this->vl_playList_friend);
        if(p_jsonObject.contains("array")){
            QJsonArray tmp_array = p_jsonObject["array"].toArray();
            if(tmp_array.count()>0){
                this->widget_bar_friend->setVisible(true);
                this->lb_title_playList_friend->setVisible(true);
            }else{
                this->widget_bar_friend->setVisible(false);
                this->lb_title_playList_friend->setVisible(false);
            }
            for(int i=0; i<tmp_array.count(); i++){
                ProfilePlayListWidget *profileBtn = new ProfilePlayListWidget(ProfilePlayListWidget::ProfileWhoType::Friend);
                this->list_profile_friend.append(profileBtn);
                //connect(profileBtn, &ProfilePlayListWidget::signal_goPagePlayListDetail, this, &SectionLeft::slot_clickedProfileRow);
                connect(profileBtn, SIGNAL(signal_clickedPlayListTitle(QJsonObject)), this, SIGNAL(signal_goPagePlayListDetail(QJsonObject)));
                profileBtn->setDataProfileInfo(tmp_array[i].toObject());
                this->vl_playList_friend->addWidget(profileBtn);
                this->vl_playList_friend->addWidget(GSCommon::getHorizontalBar("#151515", 2));
            }
        }
        break;
    }
    sender()->deleteLater();
}

/**
 * @brief SectionLeft::slot_clickedProfileRow [SLOT] 플레이리스트 Row 클릭시
 */
void SectionLeft::slot_clickedProfileRow(){

    // nothing

}
