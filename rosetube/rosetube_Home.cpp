#include "rosetube/rosetube_Home.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "login/dialoglogin.h"
#include "login/dialog_playlist_onRose.h"

#include "widget/myqwidget.h"//c220727
#include "widget/dialogconfirm.h"
#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"

#include <QDebug>
#include <QScroller>
#include <QScrollBar>


namespace rosetube {

    const int HTTP_RECOMMEND = 0;
    const int HTTP_CACHE = 1;

    // subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX_SUBTITLE_RecentlyPlaylist = 20;
    const int BTN_IDX_SUBTITLE_RecentlyRosetube = 21;
    const int BTN_IDX_SUBTITLE_History = 22;
    const int BTN_IDX_SUBTITLE_MyPlaylists = 23;
    const int BTN_IDX_SUBTITLE_UserPlaylists = 24;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___RecentlyPlaylist = 20;
    const int SECTION_FOR_MORE_POPUP___RecentlyRosetube = 21;
    const int SECTION_FOR_MORE_POPUP___History = 22;
    const int SECTION_FOR_MORE_POPUP___MyPlaylists = 23;
    const int SECTION_FOR_MORE_POPUP___UserPlaylists = 24;

    // ROSE API 관련
    QString ROSE_API_PLAYLIST_PATH = "member/playlist/recent";
    QString ROSE_API_ROSETUBE_PATH = "member/track/recent";
    QString ROSE_API_HISTORY_PATH = "member/track/history";
    QString ROSE_API_MYPLAYLIST_PATH = "member/playlist";
    QString ROSE_API_USERPLAYLIST_PATH = "member/playlist/all";


    RoseTubeHome::RoseTubeHome(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_filter, parent) {

        global.isDrawingMainContent = false;

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_logined()), SLOT(slot_getMyInfo_loginAfter()));
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));
        connect(linker, SIGNAL(signal_message_show_timeout()), SLOT(slot_time_out()));
        connect(linker, SIGNAL(signal_optMorePopup_menuClicked22(int)), SLOT(slot_optMorePopup_menuClicked22(int)));

        //data
        this->list_RecentPlaylist = new QList<roseHome::PlaylistItemData>();
        this->list_Historylist = new QList<roseHome::HistoryItemData>();
        this->list_MyPlaylist = new QList<roseHome::PlaylistItemData>();
        this->list_UserPlaylist = new QList<roseHome::PlaylistItemData>();

        this->jsonArr_rosetube = QJsonArray();

    }


    RoseTubeHome::~RoseTubeHome(){

        this->deleteLater();
    }


    void RoseTubeHome::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            this->page = pageCode;

            // init data
            this->list_RecentPlaylist->clear();
            this->list_Historylist->clear();
            this->list_MyPlaylist->clear();
            this->list_UserPlaylist->clear();

            this->rosetubeCategory = QJsonArray();

            for(int i = 0; i < 10; i++){
                this->jsonArr_rosetubeTrack[i] = QJsonArray();
            }

            for(int i = 0; i < 5; i++){
                this->jsonArr_rosetubePlaylist[i] = QJsonArray();
            }

            this->flag_recentPlaylist[0] = false;
            this->flag_recentRosetube[0] = false;
            this->flag_historylist[0] = false;
            this->flag_myPlaylist[0] = false;
            this->flag_userPlaylist[0] = false;
            this->flag_Rosetube[0] = false;

            this->flag_recentPlaylist[1] = false;
            this->flag_recentRosetube[1] = false;
            this->flag_historylist[1] = false;
            this->flag_myPlaylist[1] = false;
            this->flag_userPlaylist[1] = false;
            this->flag_Rosetube[1] = false;

            for(int i = 0; i < 10; i++){
                this->flag_track[i] = false;
            }

            for(int i = 0; i < 5; i++){
                this->flag_playlist[i] = false;
            }
        }
        else{
            this->flag_recentPlaylist_check[0] = false;
            this->flag_recentRosetube_check[0] = false;
            this->flag_myPlaylist_check[0] = false;

            this->flag_recentPlaylist_check[1] = false;
            this->flag_recentRosetube_check[1] = false;
            this->flag_myPlaylist_check[1] = false;

            global.enable_message_flag = true;
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            roseHome::ProcCommon *proc_recent_playlist = new roseHome::ProcCommon(this);
            connect(proc_recent_playlist, &roseHome::ProcCommon::completeReq_list_playlists, this, &RoseTubeHome::slot_applyResult_recentlyPlaylistCheck);
            proc_recent_playlist->request_rose_getList_recentlyPlaylists(ROSE_API_PLAYLIST_PATH, "YOUTUBE", 0, 15);

            roseHome::ProcCommon *proc_recent_rosetube = new roseHome::ProcCommon(this);
            connect(proc_recent_rosetube, &roseHome::ProcCommon::completeReq_list_rosetubes, this, &RoseTubeHome::slot_applyResult_recentlyRosetubeCheck);
            proc_recent_rosetube->request_rose_getList_recentlyRosetube(ROSE_API_ROSETUBE_PATH, "YOUTUBE", 0, 15);

            roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
            connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &RoseTubeHome::slot_applyResult_MyPlaylistCheck);
            proc_myPlaylist->request_rose_getList_myPlaylists(ROSE_API_MYPLAYLIST_PATH , "YOUTUBE", 0, 15);
        }
    }


    void RoseTubeHome::setActivePage(){

        if(this->flagNeedReload){
            ContentLoadingwaitingMsgShow("RoseTubeHome::setActivePage");//c230308_5
            //emit linker->clickedQueuePlayListHide(true);//c230428
            if(global.user.isValid() == true){

                // 항상 부모클래스의 함수 먼저 호출
                AbstractRoseHomeSubWidget::setActivePage();

                this->box_contents->removeWidget(this->stackedWidget_Contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->box_rose_contents = new QVBoxLayout();
                this->box_rose_contents->setSpacing(0);
                this->box_rose_contents->setContentsMargins(0, 10, 0, 0);

                this->widget_rose_contents = new QWidget();//c220727
                this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_rose_contents->setLayout(this->box_rose_contents);

                this->widget_login_contents = setUIControl_LoginBefore();
                this->widget_login_contents->hide();

                this->stackedWidget_Contents = new QStackedWidget();
                this->stackedWidget_Contents->addWidget(this->widget_rose_contents);
                this->stackedWidget_Contents->addWidget(this->widget_login_contents);

                this->box_contents->addWidget(this->stackedWidget_Contents);
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                // init UI
                this->vBox_recentlyPlaylist = new QVBoxLayout();
                this->vBox_recentlyRoseTube = new QVBoxLayout();
                this->vBox_historylist = new QVBoxLayout();
                this->vBox_myPlaylist = new QVBoxLayout();
                this->vBox_userPlaylist = new QVBoxLayout();

                this->hBox_recentlyPlaylist = new QHBoxLayout();
                this->hBox_recentlyRoseTube = new QHBoxLayout();
                this->hBox_historylist = new QHBoxLayout();
                this->hBox_myPlaylist = new QHBoxLayout();
                this->hBox_userPlaylist = new QHBoxLayout();

                GSCommon::clearLayout(this->hBox_recentlyPlaylist);
                GSCommon::clearLayout(this->vBox_recentlyRoseTube);
                GSCommon::clearLayout(this->vBox_historylist);
                GSCommon::clearLayout(this->vBox_myPlaylist);
                GSCommon::clearLayout(this->vBox_userPlaylist);

                GSCommon::clearLayout(this->hBox_recentlyRoseTube);
                GSCommon::clearLayout(this->hBox_recentlyRoseTube);
                GSCommon::clearLayout(this->hBox_historylist);
                GSCommon::clearLayout(this->hBox_myPlaylist);
                GSCommon::clearLayout(this->hBox_userPlaylist);


                if(global.updateCheckFlag || global.user.getAccess_token().isEmpty()){

                    //print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223
                    //QTimer::singleShot(10000, this, SLOT(slot_hide_msg()));

                    this->widget_rose_contents->hide();
                    this->widget_login_contents->show();
                    this->stackedWidget_Contents->setCurrentIndex(1);

                    if(global.enable_section_left == true){
                        global.enable_section_left = false;
                    }
                }
                else if(!global.user.getAccess_token().isEmpty()){

                    global.enable_message_count = 0;
                    global.enable_message_flag = true;
                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223

                    // sub Title UI
                    for(int i = 0; i < 30; i++){
                        this->lb_subTitle[i] = new QLabel();
                        this->btnView_all[i] = new QPushButton();
                    }

                    for(int i = 0; i < 15; i++){
                        this->home_recently_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___RecentlyPlaylist, tidal::AbstractItem::ImageSizeMode::Square_200x200, 2, true);
                    }

                    for(int i = 0; i < 15; i++){
                        this->home_recently_rosetube[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___RecentlyRosetube, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                    }

                    for(int i = 0; i < 15; i++){
                        this->home_historylist[i] = new roseHome::ItemHistory_rosehome(i, SECTION_FOR_MORE_POPUP___History, tidal::AbstractItem::ImageSizeMode::Square_200x200, false);
                    }

                    for(int i = 0; i < 15; i++){
                        this->home_myPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___MyPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                    }

                    for(int i = 0; i < 15; i++){
                        this->home_userPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___UserPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);
                    }

                    for(int i = 0; i < 10; i++){
                        this->vBox_Track[i] = new QVBoxLayout();
                        GSCommon::clearLayout(this->vBox_Track[i]);

                        this->hBox_Track[i] = new QHBoxLayout();
                        GSCommon::clearLayout(this->hBox_Track[i]);
                    }

                    for(int i = 0; i < 10; i++){
                        for(int j = 0; j < 10; j++){
                            this->home_rosetube_track[i][j] = new rosetube::ItemTrack_rosetube(j, i, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                        }
                    }

                    for(int i = 0; i < 5; i++){
                        this->vBox_Playlist[i] = new QVBoxLayout();
                        GSCommon::clearLayout(this->vBox_Playlist[i]);

                        this->hBox_Playlist[i] = new QHBoxLayout();
                        GSCommon::clearLayout(this->hBox_Playlist[i]);
                    }

                    for(int i = 0; i < 5; i++){
                        for(int j = 0; j < 10; j++){
                            this->home_rosetube_playlist[i][j] = new rosetube::ItemPlaylist_rosetube(j, i + 10, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                        }
                    }

                    this->setUIControl_requestRose();
                    this->setUIControl_requestRosetube();

                    this->widget_rose_contents->show();
                    this->widget_login_contents->hide();
                    this->stackedWidget_Contents->setCurrentIndex(0);
                }
                print_debug(); ContentLoadingwaitingMsgHide();//c230308_5
            }
            else{

                this->box_contents->removeWidget(this->stackedWidget_Contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->box_rose_contents = new QVBoxLayout();
                this->box_rose_contents->setSpacing(0);
                this->box_rose_contents->setContentsMargins(0, 0, 0, 0);


                this->widget_rose_contents = new MyQWidget();
                this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_rose_contents->setLayout(this->box_rose_contents);

                this->widget_login_contents = setUIControl_LoginBefore();
                this->widget_login_contents->hide();

                this->stackedWidget_Contents = new QStackedWidget();
                this->stackedWidget_Contents->addWidget(this->widget_rose_contents);
                this->stackedWidget_Contents->addWidget(this->widget_login_contents);

                this->box_contents->addWidget(this->stackedWidget_Contents);


                this->widget_rose_contents->hide();
                this->widget_login_contents->show();
                this->stackedWidget_Contents->setCurrentIndex(1);

                if(global.enable_section_left == true){
                    global.enable_section_left = false;
                }
            }
        }
    }


    QWidget* RoseTubeHome::setUIControl_LoginBefore(){

        QWidget *widget_loginBefore = new QWidget();
        widget_loginBefore->setObjectName("widget_loginBefore");
        widget_loginBefore->setFixedHeight(500);
        widget_loginBefore->setStyleSheet("#widget_loginBefore { background:qlineargradient(x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #222222, stop:1 #464646); }");

        int left, top = 0;
        QLabel *lb_text = new QLabel(widget_loginBefore);
        lb_text->setText(tr("You can use various services through Login."));
        lb_text->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:30px;");

        left = (1500 - lb_text->sizeHint().width()) / 2;
        top = (500 - 55 - 30 - lb_text->sizeHint().height()) / 2;
        lb_text->setGeometry(left, top, lb_text->sizeHint().width(), lb_text->sizeHint().height());

        QPushButton *btn_login = new QPushButton(widget_loginBefore);
        btn_login->setText(tr("Log-in"));
        btn_login->setCursor(Qt::PointingHandCursor);
        btn_login->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:20px;border:3px solid #CCCCCC;border-radius:25;");
        btn_login->setFixedSize(230, 55);
        btn_login->setGeometry(635, top + lb_text->sizeHint().height() + 30, 230, 55);

        // 커넥션
        connect(btn_login, &QPushButton::clicked, this, &RoseTubeHome::slot_showLoginPage);

        return widget_loginBefore;
    }


    void RoseTubeHome::setUIControl_requestRose(){

        ContentLoadingwaitingMsgShow("RoseTubeHome::setUIControl_requestRose");//c230308_5

        roseHome::ProcCommon *proc_recent_playlist = new roseHome::ProcCommon(this);
        connect(proc_recent_playlist, &roseHome::ProcCommon::completeReq_list_playlists, this, &RoseTubeHome::slot_applyResult_recentlyPlaylist);
        proc_recent_playlist->request_rose_getList_recentlyPlaylists(ROSE_API_PLAYLIST_PATH, "YOUTUBE", 0, 15);

        roseHome::ProcCommon *proc_recent_rosetube = new roseHome::ProcCommon(this);
        connect(proc_recent_rosetube, &roseHome::ProcCommon::completeReq_list_rosetubes, this, &RoseTubeHome::slot_applyResult_recentlyRosetube);
        proc_recent_rosetube->request_rose_getList_recentlyRosetube(ROSE_API_ROSETUBE_PATH, "YOUTUBE", 0, 15);

        QDate todayData = QDate::currentDate();
        QString strToday = todayData.toString("yyyyMM");

        roseHome::ProcCommon *proc_historylist = new roseHome::ProcCommon(this);
        connect(proc_historylist, &roseHome::ProcCommon::completeReq_list_history, this, &RoseTubeHome::slot_applyResult_historylist);
        proc_historylist->request_rose_getList_hisotrylist(ROSE_API_HISTORY_PATH, strToday, "YOUTUBE");

        roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
        connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &RoseTubeHome::slot_applyResult_MyPlaylist);
        proc_myPlaylist->request_rose_getList_myPlaylists(ROSE_API_MYPLAYLIST_PATH , "YOUTUBE", 0, 15);

        roseHome::ProcCommon *proc_userPlaylist = new roseHome::ProcCommon(this);
        connect(proc_userPlaylist, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &RoseTubeHome::slot_applyResult_UserPlaylist);
        proc_userPlaylist->request_rose_getList_usersPlaylists(ROSE_API_USERPLAYLIST_PATH , "PLAYLIST_RECENT", "YOUTUBE", 0, 15);

        print_debug();ContentLoadingwaitingMsgHide();//c230322_3
    }


    void RoseTubeHome::setUIControl_requestRosetube(){

        ContentLoadingwaitingMsgShow("RoseTubeHome::setUIControl_requestRose");//c230308_5

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));

        QString url = global.legacy_v1 + "/member/youtube/home?";
        QJsonObject json;

        if(global.lang == 0){
            url += "languageType=en&page=0&size=10";
        }
        else if(global.lang == 1){
            url += "languageType=ko&page=0&size=10";
        }

        network->request(HTTP_RECOMMEND
                         , url
                         , json
                         , false
                         , true);

        print_debug();ContentLoadingwaitingMsgHide();//c230322_3
    }


    void RoseTubeHome::setUIControl_appendWidget_rose(){

        if(this->flag_recentPlaylist[0] == true &&this->flag_recentRosetube[0] == true && this->flag_historylist[0] == true && this->flag_myPlaylist[0] == true && this->flag_Rosetube[0] == true && this->flag_userPlaylist[0] == true){

            ContentLoadingwaitingMsgShow("RoseTubeHome::setUIControl_requestRose");//c230308_5

            if(this->flag_recentPlaylist[0] == true){

                this->widget_recentlyPlaylist = new QWidget();
                QString subTitle = tr("Recently played Playlist");
                if(this->list_RecentPlaylist->size() > 0){
                    this->widget_recentlyPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->list_RecentPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_RecentlyPlaylist, this->vBox_recentlyPlaylist);
                }
                else{
                    this->widget_recentlyPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_RecentlyPlaylist, this->vBox_recentlyPlaylist);
                }

                this->vBox_recentlyPlaylist->addSpacing(10);

                //----------------------------------------------------------------------------------------------------  BODY : START
                this->hBox_recentlyPlaylist->setSpacing(0);
                this->hBox_recentlyPlaylist->setContentsMargins(0, 0, 0, 0);
                this->hBox_recentlyPlaylist->setAlignment(Qt::AlignTop);
                this->hBox_recentlyPlaylist->setSizeConstraint(QLayout::SetFixedSize);

                QWidget *widget_content = new QWidget;
                widget_content->setLayout(this->hBox_recentlyPlaylist);
                widget_content->setContentsMargins(0, 0, 0, 0);

                QScrollArea *playlist_scrollArea = new QScrollArea();
                playlist_scrollArea->setWidget(widget_content);
                playlist_scrollArea->setWidgetResizable(false);
                playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                playlist_scrollArea->setFixedHeight(311);

                QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_recentlyPlaylist->addWidget(playlist_scrollArea);

                if(this->flag_recentPlaylist[1] == true){

                    int maxCount = 0;
                    if(this->list_RecentPlaylist->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_RecentPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_playlist[i]->setData(this->list_RecentPlaylist->at(i));
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_recently_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseTubeHome::slot_clickedItemPlaylist);
                        this->hBox_recentlyPlaylist->addWidget(this->home_recently_playlist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 311);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_recentlyPlaylist->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_recentlyPlaylist);
                this->box_rose_contents->addSpacing(30);
            }

            if(this->flag_recentRosetube[0] == true){

                this->widget_recentlyRoseTube = new QWidget();
                QString subTitle = tr("Recently Played Rosetube");
                if(this->jsonArr_rosetube.count() > 0){
                    this->widget_recentlyRoseTube = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->recentRoseTube_cnt), "View All", BTN_IDX_SUBTITLE_RecentlyRosetube, this->vBox_recentlyRoseTube);
                }
                else{
                    this->widget_recentlyRoseTube = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_RecentlyRosetube, this->vBox_recentlyRoseTube);
                }

                this->vBox_recentlyRoseTube->addSpacing(10);

                //----------------------------------------------------------------------------------------------------  BODY : START
                this->hBox_recentlyRoseTube->setSpacing(0);
                this->hBox_recentlyRoseTube->setContentsMargins(0, 0, 0, 0);
                this->hBox_recentlyRoseTube->setAlignment(Qt::AlignTop);
                this->hBox_recentlyRoseTube->setSizeConstraint(QLayout::SetFixedSize);

                QWidget *widget_content = new QWidget;
                widget_content->setLayout(this->hBox_recentlyRoseTube);
                widget_content->setContentsMargins(0, 0, 0, 0);

                QScrollArea *playlist_scrollArea = new QScrollArea();
                playlist_scrollArea->setWidget(widget_content);
                playlist_scrollArea->setWidgetResizable(false);
                playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                playlist_scrollArea->setFixedHeight(308);

                QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_recentlyRoseTube->addWidget(playlist_scrollArea, 0, Qt::AlignTop);

                if(this->flag_recentRosetube[1] == true){

                    int maxCount = 0;
                    if(this->jsonArr_rosetube.count() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->jsonArr_rosetube.count();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_rosetube[i]->setData(this->jsonArr_rosetube.at(i).toObject());
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_recently_rosetube[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTubeHome::slot_clickedItemPlaylist);
                        this->hBox_recentlyRoseTube->addWidget(this->home_recently_rosetube[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                    noData_widget->setFixedSize(1500, 308);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_recentlyRoseTube->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_recentlyRoseTube);
                this->box_rose_contents->addSpacing(30);
            }

            if(this->flag_historylist[0] == true){

                this->widget_historylist = new QWidget();
                QString subTitle = tr("My History");
                this->widget_historylist = this->setUIControl_subTitle_withSideBtn(subTitle, "View All", BTN_IDX_SUBTITLE_History, this->vBox_historylist);

                this->vBox_historylist->addSpacing(10);

                //----------------------------------------------------------------------------------------------------  BODY : START
                this->hBox_historylist->setSpacing(0);
                this->hBox_historylist->setContentsMargins(0, 0, 0, 0);
                this->hBox_historylist->setAlignment(Qt::AlignTop);
                this->hBox_historylist->setSizeConstraint(QLayout::SetFixedSize);

                QWidget *widget_content = new QWidget;
                widget_content->setLayout(this->hBox_historylist);
                widget_content->setContentsMargins(0, 0, 0, 0);

                QScrollArea *history_scrollArea = new QScrollArea();
                history_scrollArea->setWidget(widget_content);
                history_scrollArea->setWidgetResizable(false);
                history_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                history_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                history_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                history_scrollArea->setContentsMargins(0, 0, 0, 0);
                history_scrollArea->setFixedHeight(234);

                QScroller::grabGesture(history_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_historylist->addWidget(history_scrollArea);

                if(this->flag_historylist[1] == true){

                    int maxCount = 0;
                    if(this->list_Historylist->size() > 13){
                        maxCount = 13;
                    }
                    else{
                        maxCount = this->list_Historylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_historylist[i]->setData(this->list_Historylist->at(i));
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_historylist[i], &roseHome::ItemHistory_rosehome::signal_clicked, this, &RoseTubeHome::slot_clickedItemPlaylist);
                        this->hBox_historylist->addWidget(this->home_historylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                    noData_widget->setFixedSize(1500, 234);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_historylist->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_historylist);
                this->box_rose_contents->addSpacing(60);
            }

            if(this->flag_myPlaylist[0] == true){

                this->widget_myPlaylist = new QWidget();
                QString subTitle = tr("My Playlists");
                if(this->list_MyPlaylist->size() > 0){
                    this->widget_myPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->list_MyPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_MyPlaylists, this->vBox_myPlaylist);
                }
                else{
                    this->widget_myPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_MyPlaylists, this->vBox_myPlaylist);
                }

                this->vBox_myPlaylist->addSpacing(10);

                //----------------------------------------------------------------------------------------------------  BODY : START
                this->hBox_myPlaylist->setSpacing(0);
                this->hBox_myPlaylist->setContentsMargins(0, 0, 0, 0);
                this->hBox_myPlaylist->setAlignment(Qt::AlignTop);
                this->hBox_myPlaylist->setSizeConstraint(QLayout::SetFixedSize);

                QWidget *widget_content = new QWidget;
                widget_content->setLayout(this->hBox_myPlaylist);
                widget_content->setContentsMargins(0, 0, 0, 0);

                QScrollArea *playlist_scrollArea = new QScrollArea();
                playlist_scrollArea->setWidget(widget_content);
                playlist_scrollArea->setWidgetResizable(false);
                playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                playlist_scrollArea->setFixedHeight(311);

                QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_myPlaylist->addWidget(playlist_scrollArea);

                if(this->flag_myPlaylist[1] == true){

                    int maxCount = 0;
                    if(this->list_MyPlaylist->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_MyPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_myPlaylist[i]->setData(this->list_MyPlaylist->at(i));
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_myPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseTubeHome::slot_clickedItemPlaylist);
                        this->hBox_myPlaylist->addWidget(this->home_myPlaylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 311);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_myPlaylist->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_myPlaylist);
                this->box_rose_contents->addSpacing(30);
            }

            if(this->flag_userPlaylist[0] == true){

                this->widget_userPlaylist = new QWidget();
                if(this->list_UserPlaylist->size() > 0){
                    this->widget_userPlaylist = this->setUIControl_subTitle_withSideBtn(tr("RoseTube Playlist On Rose") + QString(" (%1)").arg(this->list_UserPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_UserPlaylists, this->vBox_userPlaylist);
                }
                else{
                    this->widget_userPlaylist = this->setUIControl_subTitle_withSideBtn(tr("RoseTube Playlist On Rose") + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_UserPlaylists, this->vBox_userPlaylist);
                }

                this->vBox_userPlaylist->addSpacing(10);

                //----------------------------------------------------------------------------------------------------  BODY : START
                this->hBox_userPlaylist->setSpacing(0);
                this->hBox_userPlaylist->setContentsMargins(0, 0, 0, 0);
                this->hBox_userPlaylist->setAlignment(Qt::AlignTop);
                this->hBox_userPlaylist->setSizeConstraint(QLayout::SetFixedSize);

                QWidget *widget_userRoseplaylist = new QWidget;
                widget_userRoseplaylist->setLayout(this->hBox_userPlaylist);
                widget_userRoseplaylist->setContentsMargins(0, 0, 0, 0);

                QScrollArea *userPlaylist_scrollArea = new QScrollArea();
                userPlaylist_scrollArea->setWidget(widget_userRoseplaylist);
                userPlaylist_scrollArea->setWidgetResizable(false);
                userPlaylist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                userPlaylist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                userPlaylist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                userPlaylist_scrollArea->setContentsMargins(0, 0, 0, 0);
                userPlaylist_scrollArea->setFixedHeight(311);

                QScroller::grabGesture(userPlaylist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_userPlaylist->addWidget(userPlaylist_scrollArea);

                if(this->flag_userPlaylist[1] == true){

                    int maxCount = 0;
                    if(this->list_UserPlaylist->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_UserPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_userPlaylist[i]->setData(this->list_UserPlaylist->at(i));
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_userPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseTubeHome::slot_clickedItemPlaylist);
                        this->hBox_userPlaylist->addWidget(this->home_userPlaylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 311);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_userPlaylist->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_userPlaylist);
                this->box_rose_contents->addSpacing(50);
            }

            if(this->flag_Rosetube[1] == true){
                this->flag_track_idx = 0;
                this->flag_playlist_idx = 0;
                this->slot_applyResult_subCategories(this->rosetubeCategory);
            }

            this->slot_hide_msg();
            global.isDrawingMainContent = true;
            ContentLoadingwaitingMsgHide();
        }
    }


    void RoseTubeHome::setUIControl_checkWidget_rose(){

        if(this->flag_recentPlaylist_check[0] == true && this->flag_recentRosetube_check[0] == true && this->flag_myPlaylist_check[0] == true){

            ContentLoadingwaitingMsgShow("RoseTubeHome::setUIControl_requestRose");//c230308_5

            if(this->flag_recentPlaylist_check[1] == true){
                this->flag_recentPlaylist_check[0] = false;
                this->flag_recentPlaylist_check[1] = false;

                QString subTitle = tr("Recently played Playlist");
                if(this->list_RecentPlaylist->size() > 0){
                    this->lb_subTitle[BTN_IDX_SUBTITLE_RecentlyPlaylist]->setText(subTitle + QString(" (%1)").arg(this->list_RecentPlaylist->at(0).totalCount));
                }
                else{
                    this->lb_subTitle[BTN_IDX_SUBTITLE_RecentlyPlaylist]->setText(subTitle + QString(" (0)"));
                }

                QWidget *tmpWidget = this->hBox_myPlaylist->itemAt(0)->widget();
                //qDebug() << this->hBox_myPlaylist->count() << tmpWidget->objectName();

                if(tmpWidget->objectName() != "NoData"){
                    for(int i = 0; i < this->hBox_recentlyPlaylist->count(); i++){
                        this->home_recently_playlist[i]->hide();
                        this->home_recently_playlist[i]->disconnect();
                        this->hBox_recentlyPlaylist->removeWidget(this->home_recently_playlist[i]);
                    }
                }
                GSCommon::clearLayout(this->hBox_recentlyPlaylist);

                int maxCount = 0;
                if(this->list_RecentPlaylist->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_RecentPlaylist->size();
                }

                if(maxCount > 0){
                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___RecentlyPlaylist, tidal::AbstractItem::ImageSizeMode::Square_200x200, 2, true);
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_playlist[i]->setData(this->list_RecentPlaylist->at(i));
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_recently_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseTubeHome::slot_clickedItemPlaylist);
                        this->hBox_recentlyPlaylist->addWidget(this->home_recently_playlist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 311);

                    this->hBox_recentlyPlaylist->addWidget(noData_widget);
                }
            }

            if(this->flag_recentRosetube_check[1] == true){
                this->flag_recentRosetube_check[0] = false;
                this->flag_recentRosetube_check[1] = false;

                QString subTitle = tr("Recently played Rosetube");
                if(this->recentRoseTube_cnt > 0){
                    this->lb_subTitle[BTN_IDX_SUBTITLE_RecentlyRosetube]->setText(subTitle + QString(" (%1)").arg(this->recentRoseTube_cnt));
                }
                else{
                    this->lb_subTitle[BTN_IDX_SUBTITLE_RecentlyRosetube]->setText(subTitle + QString(" (0)"));
                }

                QWidget *tmpWidget = this->hBox_myPlaylist->itemAt(0)->widget();
                //qDebug() << this->hBox_myPlaylist->count() << tmpWidget->objectName();

                if(tmpWidget->objectName() != "NoData"){
                    for(int i = 0; i < this->hBox_recentlyRoseTube->count(); i++){
                        this->home_recently_rosetube[i]->hide();
                        this->home_recently_rosetube[i]->disconnect();
                        this->hBox_recentlyRoseTube->removeWidget(this->home_recently_rosetube[i]);
                    }
                }
                GSCommon::clearLayout(this->hBox_recentlyRoseTube);

                int maxCount = 0;
                if(this->jsonArr_rosetube.count() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->jsonArr_rosetube.count();
                }

                if(maxCount > 0){
                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_rosetube[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___RecentlyRosetube, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_rosetube[i]->setData(this->jsonArr_rosetube.at(i).toObject());
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_recently_rosetube[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTubeHome::slot_clickedItemPlaylist);
                        this->hBox_recentlyRoseTube->addWidget(this->home_recently_rosetube[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                    noData_widget->setFixedSize(1500, 308);

                    this->hBox_recentlyRoseTube->addWidget(noData_widget);
                }
            }

            if(this->flag_myPlaylist_check[1] == true){
                this->flag_myPlaylist_check[0] = false;
                this->flag_myPlaylist_check[1] = false;

                if(this->flag_recentPlaylist_check[0] == true){
                    this->flag_recentPlaylist_check[0] = false;
                }

                if(this->flag_recentRosetube_check[0] == true){
                    this->flag_recentRosetube_check[0] = false;
                }

                QString subTitle = tr("My Playlists");
                if(this->list_RecentPlaylist->size() > 0){
                    this->lb_subTitle[BTN_IDX_SUBTITLE_MyPlaylists]->setText(subTitle + QString(" (%1)").arg(this->list_MyPlaylist->at(0).totalCount));
                }
                else{
                    this->lb_subTitle[BTN_IDX_SUBTITLE_MyPlaylists]->setText(subTitle + QString(" (0)"));
                }

                QWidget *tmpWidget = this->hBox_myPlaylist->itemAt(0)->widget();
                //qDebug() << this->hBox_myPlaylist->count() << tmpWidget->objectName();

                if(tmpWidget->objectName() != "NoData"){
                    for(int i = 0; i < this->hBox_myPlaylist->count(); i++){
                        this->home_myPlaylist[i]->hide();
                        this->home_myPlaylist[i]->disconnect();
                        this->hBox_myPlaylist->removeWidget(this->home_myPlaylist[i]);
                    }
                }
                GSCommon::clearLayout(this->hBox_myPlaylist);

                int maxCount = 0;
                if(this->list_MyPlaylist->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_MyPlaylist->size();
                }

                if(maxCount > 0){
                    for(int i = 0; i < maxCount; i++){
                        this->home_myPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___MyPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_myPlaylist[i]->setData(this->list_MyPlaylist->at(i));
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_myPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseTubeHome::slot_clickedItemPlaylist);
                        this->hBox_myPlaylist->addWidget(this->home_myPlaylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                    noData_widget->setFixedSize(1500, 311);

                    this->hBox_myPlaylist->addWidget(noData_widget);
                }
            }

            this->slot_hide_msg();
            ContentLoadingwaitingMsgHide();
        }
    }


    void RoseTubeHome::slot_applyResult_recentlyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_RecentPlaylist->append(list_data);
            this->flag_recentPlaylist[1] = true;
        }

        this->flag_recentPlaylist[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseTubeHome::slot_applyResult_recentlyRosetube(const QJsonArray &jsonArr, const int &totalCnt, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        ContentLoadingwaitingMsgHide();//c220616

        if(jsonArr.count() > 0){
            this->flag_recentRosetube[1] = true;
            this->jsonArr_rosetube = jsonArr;
            this->recentRoseTube_cnt = totalCnt;
        }

        this->flag_recentRosetube[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseTubeHome::slot_applyResult_historylist(const QList<roseHome::HistoryItemData> &list_data, const QJsonArray &jsonArr_dataToPlay){

        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.length() > 0)
        {
            this->list_Historylist->append(list_data);
            this->flag_historylist[1] = true;
        }

        this->flag_historylist[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseTubeHome::slot_applyResult_MyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_MyPlaylist->append(list_data);
            this->flag_myPlaylist[1] = true;
        }

        this->flag_myPlaylist[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseTubeHome::slot_applyResult_UserPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_UserPlaylist->append(list_data);
            this->flag_userPlaylist[1] = true;
        }

        this->flag_userPlaylist[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseTubeHome::slot_applyResult_recentlyPlaylistCheck(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_RecentPlaylist->count() == 0){
                this->list_RecentPlaylist->clear();
                this->list_RecentPlaylist->append(list_data);

                this->flag_recentPlaylist_check[1] = true;
            }
            else{
                int maxCount = (list_data.count() > this->list_RecentPlaylist->count()) ? this->list_RecentPlaylist->count() : list_data.count();

                for(int i = 0; i < maxCount; i++){
                    if(list_data.at(i).title != this->list_RecentPlaylist->at(i).title){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_RecentPlaylist->at(0).totalCount) || (change_flag > 0)){
                    this->list_RecentPlaylist->clear();
                    this->list_RecentPlaylist->append(list_data);

                    this->flag_recentPlaylist_check[1] = true;
                }
            }
        }
        else if(list_data.size() == 0 && this->list_RecentPlaylist->count() != 0){
            this->list_RecentPlaylist->clear();

            this->flag_recentPlaylist_check[1] = true;
        }

        this->flag_recentPlaylist_check[0] = true;
        this->setUIControl_checkWidget_rose();
    }


    void RoseTubeHome::slot_applyResult_recentlyRosetubeCheck(const QJsonArray &jsonArr, const int &totalCnt, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(jsonArr.size() > 0){
            if(this->jsonArr_rosetube.count() == 0){
                this->jsonArr_rosetube = jsonArr;
                this->recentRoseTube_cnt = totalCnt;

                this->flag_recentRosetube_check[1] = true;
            }
            else{
                int maxCount = (jsonArr.count() > recentRoseTube_cnt) ? recentRoseTube_cnt : jsonArr.count();

                for(int i = 0; i < maxCount; i++){
                    if(jsonArr.at(i).toObject() != this->jsonArr_rosetube.at(i).toObject()){
                        change_flag++;
                    }
                }

                if((totalCnt != this->recentRoseTube_cnt) || (change_flag > 0)){
                    this->jsonArr_rosetube = jsonArr;
                    this->recentRoseTube_cnt = totalCnt;

                    this->flag_recentRosetube_check[1] = true;
                }
            }
        }
        else if(jsonArr.size() == 0 && this->jsonArr_rosetube.count() != 0){
            this->jsonArr_rosetube = QJsonArray();
            this->recentRoseTube_cnt = totalCnt;

            this->flag_recentRosetube_check[1] = true;
        }

        this->flag_recentRosetube_check[0] = true;
        this->setUIControl_checkWidget_rose();
    }


    void RoseTubeHome::slot_applyResult_MyPlaylistCheck(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_MyPlaylist->count() == 0){
                this->list_MyPlaylist->clear();
                this->list_MyPlaylist->append(list_data);

                this->flag_myPlaylist_check[1] = true;
            }
            else{
                int maxCount = (list_data.count() > this->list_MyPlaylist->count()) ? this->list_MyPlaylist->count() : list_data.count();

                for(int i = 0; i < maxCount; i++){
                    if((list_data.at(i).id != this->list_MyPlaylist->at(i).id) || (list_data.at(i).title != this->list_MyPlaylist->at(i).title) || (list_data.at(i).star != this->list_MyPlaylist->at(i).star)){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_MyPlaylist->at(0).totalCount) || (change_flag > 0)){
                    this->list_MyPlaylist->clear();
                    this->list_MyPlaylist->append(list_data);

                    this->flag_myPlaylist_check[1] = true;
                }
            }
        }
        else if(list_data.size() == 0 && this->list_MyPlaylist->size() != 0){
            this->list_MyPlaylist->clear();

            this->flag_myPlaylist_check[1] = true;
        }

        this->flag_myPlaylist_check[0] = true;
        this->setUIControl_checkWidget_rose();
    }


    void RoseTubeHome::slot_applyResult_subCategories(const QJsonArray &jsonArr){

        if(jsonArr.size() > 0){
            ContentLoadingwaitingMsgShow("RoseTubeHome::slot_applyResult_subCategories");//c230308_5
            for(int i = 0; i < jsonArr.size(); i++){
                QJsonObject jsonObj = jsonArr.at(i).toObject();

                if(jsonObj["listType"].toString() == "TRACK"){
                    global.ROSETUBE_API_TRACK[this->flag_track_idx] = ProcJsonEasy::getInt(jsonObj, "id");
                    QJsonObject trackObj = ProcJsonEasy::getJsonObject(jsonObj, "playlist");

                    QJsonArray trackArr = ProcJsonEasy::getJsonArray(trackObj, "tracks");
                    this->jsonArr_rosetubeTrack[this->flag_track_idx] = trackArr;

                    int maxCount = 0;
                    if(trackArr.size() > 10){
                        maxCount = 10;
                    }
                    else{
                        maxCount = trackArr.size();
                    }

                    this->widget_roseTube_Track[this->flag_track_idx] = new QWidget();
                    QString subTitle = ProcJsonEasy::getString(jsonObj, "name");
                    if(trackArr.size() > 0){
                        this->widget_roseTube_Track[this->flag_track_idx] = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(ProcJsonEasy::getInt(trackObj, "trackCount")), "View All", this->flag_track_idx, this->vBox_Track[this->flag_track_idx]);
                    }
                    else{
                        this->widget_roseTube_Track[this->flag_track_idx] = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", this->flag_track_idx, this->vBox_Track[this->flag_track_idx]);
                    }

                    this->vBox_Track[this->flag_track_idx]->addSpacing(10);

                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->hBox_Track[this->flag_track_idx]->setSpacing(0);
                    this->hBox_Track[this->flag_track_idx]->setContentsMargins(0, 0, 0, 0);
                    this->hBox_Track[this->flag_track_idx]->setAlignment(Qt::AlignTop);
                    this->hBox_Track[this->flag_track_idx]->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_Track[this->flag_track_idx]);
                    widget_content->setContentsMargins(0, 0, 0, 0);

                    QScrollArea *playlist_scrollArea = new QScrollArea();
                    playlist_scrollArea->setWidget(widget_content);
                    playlist_scrollArea->setWidgetResizable(false);
                    playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                    playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                    playlist_scrollArea->setFixedHeight(308);

                    QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_Track[this->flag_track_idx]->addWidget(playlist_scrollArea);

                    if(maxCount == 0){
                        NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                        noData_widget->setFixedSize(1500, 308);
                        noData_widget->setObjectName("Nodata");

                        this->hBox_Track[this->flag_track_idx]->addWidget(noData_widget);
                    }
                    else{
                        for(int j = 0; j < maxCount; j++){
                            this->home_rosetube_track[this->flag_track_idx][j]->setData(trackArr.at(j).toObject());
                            QCoreApplication::processEvents();
                        }

                        for(int j = 0; j < maxCount; j++){
                            connect(this->home_rosetube_track[i][j], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTubeHome::slot_clickedItemPlaylist);
                            this->hBox_Track[this->flag_track_idx]->addWidget(this->home_rosetube_track[this->flag_track_idx][j]);
                        }
                    }

                    this->box_rose_contents->addLayout(this->vBox_Track[this->flag_track_idx]);
                    this->box_rose_contents->addSpacing(30);

                    this->flag_track_idx++;
                }
                else if(jsonObj["listType"].toString() == "PLAYLIST"){
                    global.ROSETUBE_API_PLAYLIST[this->flag_playlist_idx] = ProcJsonEasy::getInt(jsonObj, "id");
                    QJsonArray palylistArr = ProcJsonEasy::getJsonArray(jsonObj, "playlists");
                    this->jsonArr_rosetubePlaylist[this->flag_playlist_idx] = palylistArr;

                    int maxCount = 0;
                    if(palylistArr.size() > 10){
                        maxCount = 10;
                    }
                    else{
                        maxCount = palylistArr.size();
                    }

                    this->widget_roseTube_Playlist[this->flag_playlist_idx] = new QWidget();
                    QString subTitle = ProcJsonEasy::getString(jsonObj, "name");

                    this->widget_roseTube_Playlist[this->flag_playlist_idx] = this->setUIControl_subTitle_withSideBtn(subTitle , "View All", this->flag_playlist_idx + 10, this->vBox_Playlist[this->flag_playlist_idx]);

                    this->vBox_Playlist[this->flag_playlist_idx]->addSpacing(10);

                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->hBox_Playlist[this->flag_playlist_idx]->setSpacing(0);
                    this->hBox_Playlist[this->flag_playlist_idx]->setContentsMargins(0, 0, 0, 0);
                    this->hBox_Playlist[this->flag_playlist_idx]->setAlignment(Qt::AlignTop);
                    this->hBox_Playlist[this->flag_playlist_idx]->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_Playlist[this->flag_playlist_idx]);
                    widget_content->setContentsMargins(0, 0, 0, 0);

                    QScrollArea *playlist_scrollArea = new QScrollArea();
                    playlist_scrollArea->setWidget(widget_content);
                    playlist_scrollArea->setWidgetResizable(false);
                    playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                    playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                    playlist_scrollArea->setFixedHeight(335);

                    QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_Playlist[this->flag_playlist_idx]->addWidget(playlist_scrollArea);

                    if(maxCount == 0){
                        NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                        noData_widget->setFixedSize(1500, 335);
                        noData_widget->setObjectName("Nodata");

                        this->hBox_Playlist[this->flag_playlist_idx]->addWidget(noData_widget);
                    }
                    else{
                        for(int j = 0; j < maxCount; j++){
                            this->home_rosetube_playlist[this->flag_playlist_idx][j]->setData(palylistArr.at(j).toObject());
                            QCoreApplication::processEvents();
                        }

                        for(int j = 0; j < maxCount; j++){
                            connect(this->home_rosetube_playlist[i][j], &rosetube::ItemPlaylist_rosetube::signal_clicked, this, &RoseTubeHome::slot_clickedItemPlaylist);
                            this->hBox_Playlist[this->flag_playlist_idx]->addWidget(this->home_rosetube_playlist[this->flag_playlist_idx][j]);
                        }

                        this->box_rose_contents->addLayout(this->vBox_Playlist[this->flag_playlist_idx]);
                    }

                    this->box_rose_contents->addSpacing(30);

                    this->flag_playlist_idx++;
                }
            }
            print_debug(); ContentLoadingwaitingMsgHide();//c230308_5
        }
    }


    void RoseTubeHome::slot_hide_msg(){

        if(global.enable_message_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;
        }

        if(global.enable_section_left == true){
            global.enable_section_left = false;
        }

        //ContentLoadingwaitingMsgHide();
    }

    void RoseTubeHome::slot_time_out(){//c220727

        if(global.enable_message_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;


            this->box_contents->removeWidget(this->widget_rose_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->box_rose_contents = new QVBoxLayout();
            this->box_rose_contents->setSpacing(0);
            this->box_rose_contents->setContentsMargins(0, 0, 0, 0);


            this->widget_rose_contents = new MyQWidget();
            this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_rose_contents->setLayout(this->box_rose_contents);

            this->widget_login_contents = setUIControl_LoginBefore();
            this->widget_login_contents->hide();

            this->stackedWidget_Contents = new QStackedWidget();
            this->stackedWidget_Contents->addWidget(this->widget_rose_contents);
            this->stackedWidget_Contents->addWidget(this->widget_login_contents);


            this->box_contents->addWidget(this->stackedWidget_Contents);


            this->widget_rose_contents->hide();
            this->widget_login_contents->show();
            this->stackedWidget_Contents->setCurrentIndex(1);
        }

        if(global.enable_section_left == true){
            global.enable_section_left = false;
        }

        ContentLoadingwaitingMsgHide();
    }


    void RoseTubeHome::slot_showLoginPage(){

        this->dlg_rose_login = new Dialog::DialogLogin(this);
        this->dlg_rose_login->exec();
    }


    void RoseTubeHome::slot_getMyInfo_loginAfter(){

        if(global.user.isValid() == true && this->stackedWidget_Contents->currentIndex() == 1){

            this->list_MyPlaylist = new QList<roseHome::PlaylistItemData>();

            this->jsonArr_rosetube = QJsonArray();

            this->flagNeedReload = false;
            this->setActivePage();
        }
    }


    void RoseTubeHome::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_ROSE)){

            global.user.setDeviceChange(false);

            this->page = "";
            QJsonObject tmpCode = QJsonObject();
            tmpCode.insert("pageCode", "home");

            this->setJsonObject_forData(tmpCode);
            this->setActivePage();
        }
    }


    void RoseTubeHome::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_RecentlyPlaylist){

            QString title = tr("Recently Played Playlist");

            QJsonObject tmp_data;
            if(this->list_RecentPlaylist->size() > 0){
                tmp_data.insert("title", QString(title + " (%1)").arg(this->list_RecentPlaylist->at(0).totalCount));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_PLAYLIST);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedViewAll(tmp_data);
        }
        else if(btnId == BTN_IDX_SUBTITLE_RecentlyRosetube){

            QString title = tr("Recently Played Rosetube");

            QJsonObject tmp_data;
            if(this->list_MyPlaylist->size() > 0){
                tmp_data.insert("title", QString(title + " (%1)").arg(this->recentRoseTube_cnt));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_ROSE);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedViewAll(tmp_data);
        }
        else if(btnId == BTN_IDX_SUBTITLE_History){

            QJsonArray histories;
            for(int i = 0; i < this->list_Historylist->count(); i++){
                QJsonObject tmphistory;
                tmphistory.insert("yearMonth", this->list_Historylist->at(i).yearMonth);
                tmphistory.insert("visible", this->list_Historylist->at(i).visible);

                histories.append(tmphistory);
            }

            QJsonObject jsonObj;
            jsonObj.insert("pathTitle", this->lb_subTitle[btnId]->text());
            jsonObj.insert("type", "History");
            jsonObj.insert("type_id", 8);
            jsonObj.insert("filter_type", "YOUTUBE");
            jsonObj.insert("histories", histories);
            jsonObj.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_HISTORY_LIST_VIEW);

            emit this->signal_clickedViewAll(jsonObj);
        }
        else if(btnId == BTN_IDX_SUBTITLE_MyPlaylists){

            QString title = tr("My Playlists");

            QJsonObject tmp_data;
            if(this->list_MyPlaylist->size() > 0){
                tmp_data.insert("title", QString(title + " (%1)").arg(this->list_MyPlaylist->at(0).totalCount));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_MYROSE);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedViewAll(tmp_data);
        }
        else if(btnId == BTN_IDX_SUBTITLE_UserPlaylists){

            QString title = tr("Rosetube Playlist On Rose");

            QJsonObject tmp_data;
            if(this->list_UserPlaylist->size() > 0){
                tmp_data.insert("title", QString(title + " (%1)").arg(this->list_UserPlaylist->at(0).totalCount));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_USERROSE);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedViewAll(tmp_data);
        }
        else{

            QJsonObject tmp_data;
            tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME);
            tmp_data.insert("title", sender()->property("title").toString());

            if(btnId < 10){
                tmp_data.insert("subCategoryId", global.ROSETUBE_API_TRACK[btnId]);
                tmp_data.insert("type", "track");
            }
            else{
                tmp_data.insert("subCategoryId", global.ROSETUBE_API_PLAYLIST[btnId - 10]);
                tmp_data.insert("type", "playlist");
            }

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedViewAll(tmp_data);
        }
    }


    QWidget* RoseTubeHome::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

        // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.
        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        tmp_hBox->setSpacing(0);
        tmp_hBox->setContentsMargins(0, 0, 0, 0);
        tmp_hBox->setAlignment(Qt::AlignTop);

        QWidget *widget_box_subTitle = new QWidget();
        widget_box_subTitle->setContentsMargins(0, 0, 0, 0);
        widget_box_subTitle->setLayout(tmp_hBox);

        this->lb_subTitle[btnId]->setText(subTitle);
        this->lb_subTitle[btnId]->setStyleSheet("font-size:24px;font-weight:bold;color:#FFFFFF;");
        tmp_hBox->addWidget(this->lb_subTitle[btnId], 0, Qt::AlignVCenter);

        // side button - it works when the button text is not empty
        if(btnText.isEmpty() == false){
            this->btnView_all[btnId]->setText(btnText);
            this->btnView_all[btnId]->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
            this->btnView_all[btnId]->setProperty("idx", btnId);
            this->btnView_all[btnId]->setProperty("title", subTitle);
            this->btnView_all[btnId]->setCursor(Qt::PointingHandCursor);
            connect(this->btnView_all[btnId], SIGNAL(clicked()), this, SLOT(slot_clickBtn_subTitle_viewAll()));

            tmp_hBox->addWidget(this->btnView_all[btnId], 1, Qt::AlignVCenter | Qt::AlignRight);
        }

        // Apply Main Layout with spacing
        p_layout->addWidget(widget_box_subTitle);

        return widget_box_subTitle;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RoseTubeHome::slot_applyResult_getShareLink(const QString &link){//c220818 share link

        this->shareLink = link;
    }


    void RoseTubeHome::slot_add_rosePlaylist_withRosetube(const int &idx, const QJsonObject &dataObj){

        QString view_type = "";
        if(idx < 0){
            view_type = "create";
        }
        else{
            view_type = "add";
        }

        QJsonObject jsonObj_move = dataObj;
        jsonObj_move.insert("view_type", view_type);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RT_ADDPLAYLIST);

        emit this->signal_clickedPagemove(jsonObj_move);
    }


    void RoseTubeHome::slot_applyResult_myPlaylistDelete(const QJsonObject &jsonObj){

        if(jsonObj.value("flagOk").toBool() == true && jsonObj.value("message").toString() == "정상"){
            this->flag_recentPlaylist_check[0] = true;
            this->flag_recentRosetube_check[0] = true;
            this->flag_myPlaylist_check[0] = false;

            this->flag_recentPlaylist_check[1] = false;
            this->flag_recentRosetube_check[1] = false;
            this->flag_myPlaylist_check[1] = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
            connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &RoseTubeHome::slot_applyResult_MyPlaylistCheck);
            proc_myPlaylist->request_rose_getList_myPlaylists(ROSE_API_MYPLAYLIST_PATH , "YOUTUBE", 0, 15);
        }
    }


    void RoseTubeHome::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){

            QJsonObject tmpObj = contents.at(0).toObject();

            if(tmpObj.value("flagOk").toBool() == true && tmpObj.value("message").toString() == "정상"){
                if(this->flag_fav_type == SECTION_FOR_MORE_POPUP___RecentlyRosetube){
                    this->home_recently_rosetube[this->flag_fav_idx]->setFavorite_btnHeart(this->flag_fav_star == 0 ? false : true, this->flag_fav_star);
                }
                else{
                    this->home_rosetube_track[this->flag_fav_type][this->flag_fav_idx]->setFavorite_btnHeart(this->flag_fav_star == 0 ? false : true, this->flag_fav_star);
                }
            }
        }

        ContentLoadingwaitingMsgHide();
    }


    void RoseTubeHome::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___RecentlyPlaylist){
            roseHome::PlaylistItemData data_playlist = this->list_RecentPlaylist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data_playlist), index, SECTION_FOR_MORE_POPUP___RecentlyPlaylist, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입
                QJsonObject tmp_data;
                tmp_data.insert("id", data_playlist.id);
                tmp_data.insert("ownerId", data_playlist.ownerId);
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYDETAIL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_MYDETAIL);
                // 부모에게 페이지 변경하라고 시그널 보냄
                emit this->signal_clickedViewAll(tmp_data);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___History){
            roseHome::HistoryItemData tmpData;
            tmpData.yearMonth = this->list_Historylist->at(index).yearMonth;
            tmpData.visible = this->list_Historylist->at(index).visible;
            tmpData.filter_type = "YOUTUBE";

            if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // History 페이지 진입
                QJsonObject tmp_data = roseHome::ConvertData::getObjectJson_historyData(tmpData);
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_HISTORY_DETAIL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_HISTORY_DETAIL);

                emit this->signal_clickedViewAll(tmp_data);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___MyPlaylists){
            roseHome::PlaylistItemData data_playlist = this->list_MyPlaylist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data_playlist), index, SECTION_FOR_MORE_POPUP___MyPlaylists, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입
                QJsonObject tmp_data;
                tmp_data.insert("id", data_playlist.id);
                tmp_data.insert("ownerId", data_playlist.ownerId);
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYDETAIL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_MYDETAIL);
                // 부모에게 페이지 변경하라고 시그널 보냄
                emit this->signal_clickedViewAll(tmp_data);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___UserPlaylists){
            roseHome::PlaylistItemData data_playlist = this->list_UserPlaylist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data_playlist), index, SECTION_FOR_MORE_POPUP___UserPlaylists, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입
                QJsonObject tmp_data;
                tmp_data.insert("id", data_playlist.id);
                tmp_data.insert("ownerId", data_playlist.ownerId);
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYDETAIL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_MYDETAIL);
                // 부모에게 페이지 변경하라고 시그널 보냄
                emit this->signal_clickedViewAll(tmp_data);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___RecentlyRosetube){

            if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx2
                    || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx3 || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){

                bool flag_fav = false;
                int star_fav = 0;

                if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add){
                    flag_fav = true;
                    star_fav = 1;
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx2){
                    flag_fav = true;
                    star_fav = 2;
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx3){
                    flag_fav = true;
                    star_fav = 3;
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){
                    flag_fav = true;
                    star_fav = 0;
                }

                this->flag_fav_idx = index;
                this->flag_fav_star = star_fav;
                this->flag_fav_type = SECTION_FOR_MORE_POPUP___RecentlyRosetube;

                QJsonObject track = this->jsonArr_rosetube.at(index).toObject();

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", flag_fav);
                ratingInfo.insert("star", star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", "YOUTUBE");

                QJsonObject json;
                json.insert("track", track);
                json.insert("ratingInfo", ratingInfo);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseTubeHome::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, flag_fav, star_fav);

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }
            else{

                if(clickMode == tidal::AbstractItem::ClickMode::AllBox || clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                    // Track 바로 재생 - clicked 항목만 재생
                    int curr_clickMode = global.device.getMusicPlayType();

                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    switch(curr_clickMode)
                    {
                        case 13:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                            break;
                        case 12:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                            break;
                        case 15:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                            break;
                        case 16:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                            break;
                        case 17:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                            break;
                        case 18:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                            break;
                        case 19:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                            break;
                    }

                    roseHome::TrackItemData data_playlist;
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byTracks(data_playlist, this->jsonArr_rosetube, index, playType);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    QJsonObject tmpObj = this->jsonArr_rosetube.at(index).toObject();

                    QJsonObject data = ProcJsonEasy::getJsonObject(tmpObj, "data");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(data, "snippet");

                    OptMorePopup::HeaderData data_header;
                    data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                    data_header.sub_title = ProcJsonEasy::getString(snippet, "channelTitle");
                    data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                    data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(tmpObj, "id"));
                    data_header.type = "YOUTUBE";      //j220906 share link
                    data_header.flagProcStar = false;
                    data_header.isShare = true;      //j220906 share link

                    // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Rosetube_Video, data_header, index, section);
                }
            }
        }
        else{

            if(section < 10){
                if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx2
                        || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx3 || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){

                    bool flag_fav = false;
                    int star_fav = 0;

                    if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add){
                        flag_fav = true;
                        star_fav = 1;
                    }
                    else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx2){
                        flag_fav = true;
                        star_fav = 2;
                    }
                    else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx3){
                        flag_fav = true;
                        star_fav = 3;
                    }
                    else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){
                        flag_fav = true;
                        star_fav = 0;
                    }

                    this->flag_fav_idx = index;
                    this->flag_fav_star = star_fav;
                    this->flag_fav_type = section;

                    QJsonObject track = this->jsonArr_rosetubeTrack[section].at(index).toObject();

                    QJsonObject ratingInfo;
                    ratingInfo.insert("favorite", flag_fav);
                    ratingInfo.insert("star", star_fav);
                    ratingInfo.insert("thumbup", false);
                    ratingInfo.insert("type", "YOUTUBE");

                    QJsonObject json;
                    json.insert("track", track);
                    json.insert("ratingInfo", ratingInfo);

                    // request HTTP API - get favorite for Rose Server
                    roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                    connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseTubeHome::slot_applyResult_getRating_track);
                    proc_fav_track->request_rose_setRating_Track(json, flag_fav, star_fav);

                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                }
                else{

                    if(clickMode == tidal::AbstractItem::ClickMode::AllBox || clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                        // Track 바로 재생 - clicked 항목만 재생
                        int curr_clickMode = global.device.getMusicPlayType();

                        OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                        switch(curr_clickMode)
                        {
                            case 13:
                                playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                                break;
                            case 12:
                                playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                                break;
                            case 15:
                                playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                                break;
                            case 16:
                                playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                                break;
                            case 17:
                                playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                                break;
                            case 18:
                                playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                                break;
                            case 19:
                                playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                                break;
                        }

                        roseHome::TrackItemData data_playlist;
                        roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                        procRosePlay->requestPlayRose_byTracks(data_playlist, this->jsonArr_rosetubeTrack[section], index, playType);
                    }
                    else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                        QJsonObject tmpObj = this->jsonArr_rosetubeTrack[section].at(index).toObject();

                        QJsonObject data = ProcJsonEasy::getJsonObject(tmpObj, "data");
                        QJsonObject snippet = ProcJsonEasy::getJsonObject(data, "snippet");

                        OptMorePopup::HeaderData data_header;
                        data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                        data_header.sub_title = ProcJsonEasy::getString(snippet, "channelTitle");
                        data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                        data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(tmpObj, "id"));
                        data_header.type = "YOUTUBE";      //j220906 share link
                        data_header.flagProcStar = false;
                        data_header.isShare = true;      //j220906 share link

                        // OptMorePopup 띄우기 필요
                        this->makeObj_optMorePopup(OptMorePopup::Rosetube_Video, data_header, index, section);
                    }
                }
            }
            else{

                if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                    // Track 바로 재생 - clicked 항목만 재생
                    int curr_clickMode = global.device.getMusicPlayType();

                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    switch(curr_clickMode)
                    {
                        case 13:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last;
                            break;
                        case 12:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty;
                            break;
                        case 15:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_RightNow;
                            break;
                        case 16:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext;
                            break;
                        default:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_RightNow;
                            break;
                    }

                    QJsonObject tmpObj = this->jsonArr_rosetubePlaylist[section - 10].at(index).toObject();
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byPlaylistID(ProcJsonEasy::getInt(tmpObj, "id"), playType);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                    QJsonObject tmpObj = this->jsonArr_rosetubePlaylist[section - 10].at(index).toObject();
                    // Playlist Detail 페이지 진입
                    QJsonObject tmp_data;
                    tmp_data.insert("id", ProcJsonEasy::getInt(tmpObj, "id"));
                    tmp_data.insert("ownerId", ProcJsonEasy::getInt(tmpObj, "ownerId"));
                    tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYDETAIL);
                    tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_MYDETAIL);
                    // 부모에게 페이지 변경하라고 시그널 보냄
                    emit this->signal_clickedViewAll(tmp_data);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    QJsonObject tmpObj = this->jsonArr_rosetubePlaylist[section - 10].at(index).toObject();

                    QJsonObject data = ProcJsonEasy::getJsonObject(tmpObj, "data");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(data, "snippet");

                    OptMorePopup::HeaderData data_header;
                    data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                    data_header.sub_title = ProcJsonEasy::getString(tmpObj, "ownerName").toUpper();
                    data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnail");
                    data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(tmpObj, "id"));
                    data_header.type = "YOUTUBE";      //j220906 share link
                    data_header.flagProcStar = false;
                    data_header.isShare = true;      //j220906 share link

                    // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Rosetube_Playlist, data_header, index, section);
                }
            }
        }
    }


    void RoseTubeHome::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___RecentlyPlaylist){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_RecentPlaylist, index, SECTION_FOR_MORE_POPUP___RecentlyPlaylist, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___MyPlaylists){
            if(clickMode == OptMorePopup::ClickMode::Edit){
                QString view_type = "edit";

                QJsonObject data;
                data.insert("view_type", view_type);
                data.insert("playlist_id", this->list_MyPlaylist->at(index).id);
                data.insert("type", "ROSE");

                QJsonObject jsonObj_move;
                jsonObj_move.insert("data", data);

                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RT_ADDPLAYLIST);

                emit linker->signal_clickedMovePage(jsonObj_move);
            }
            else if(clickMode == OptMorePopup::ClickMode::Delete){

                DialogConfirm *dlg = new DialogConfirm(this);
                dlg->setFixedHeight(560);
                dlg->setTitle(tr("PlayList Delete Notice"));
                dlg->setText(tr("Are you sure you want to delete your selected playlist's track?"));

                int latestWidth = global.width_mainwindow;
                int latestHeight = global.height_mainwindow;
                int left = global.left_mainwindow + ((latestWidth - dlg->sizeHint().width()) / 2);
                int top = global.top_mainwindow + ((latestHeight - dlg->sizeHint().height()) / 2);

                dlg->setGeometry(left, top, 0, 0);

                int result = dlg->exec();

                if(result == QDialog::Accepted){
                    roseHome::ProcCommon *proc_myPlaylistDel = new roseHome::ProcCommon(this);
                    connect(proc_myPlaylistDel, &roseHome::ProcCommon::completeReq_delete_playlist, this, &RoseTubeHome::slot_applyResult_myPlaylistDelete);
                    proc_myPlaylistDel->request_rose_delete_myPlaylist(this->list_MyPlaylist->at(index).id);
                }

                delete dlg;
            }
            else{
                this->proc_clicked_optMorePopup_fromPlaylist(this->list_MyPlaylist, index, SECTION_FOR_MORE_POPUP___MyPlaylists, clickMode);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___UserPlaylists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_UserPlaylist, index, SECTION_FOR_MORE_POPUP___UserPlaylists, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___RecentlyRosetube){
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last

            )
            {
                // Rose Play 요청
                roseHome::TrackItemData data_track;
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data_track, this->jsonArr_rosetube, index, clickMode);
            }
            else if(clickMode == OptMorePopup::ClickMode::Add_MyCollection){
                QJsonArray tracks = QJsonArray();
                tracks.append(this->jsonArr_rosetube.at(index).toObject());

                QJsonObject json = QJsonObject();
                json.insert("tracks", tracks);

                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::ROSETUBE, json, this);
                dialog_playlist->request_playlist_fetch();
                connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &RoseTubeHome::slot_add_rosePlaylist_withRosetube);
                int result = dialog_playlist->exec();

                if(result == QDialog::Accepted){
                    delete dialog_playlist;
                }
            }
            else if(clickMode == OptMorePopup::ClickMode::SubscribInfo){
                QJsonObject tmpObj = this->jsonArr_rosetube.at(index).toObject();
                QJsonObject dataObj = ProcJsonEasy::getJsonObject(tmpObj, "data");
                QJsonObject snippet = ProcJsonEasy::getJsonObject(dataObj, "snippet");
                QString channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                QString channelId = ProcJsonEasy::getString(snippet, "channelId");

                QJsonObject tmp_data;
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert("title", channelName);
                tmp_data.insert("channel_id", channelId);

                emit this->signal_clickedViewAll(tmp_data);
            }
            else if(clickMode == OptMorePopup::ClickMode::Share){
                setUIShare();//c220818
            }
        }
        else{
            if(section < 10){
                if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                        || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
                        || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
                        || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
                        || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
                        || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                        || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                        || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last

                )
                {
                    // Rose Play 요청
                    roseHome::TrackItemData data_track;
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byTracks(data_track, this->jsonArr_rosetubeTrack[section], index, clickMode);
                }
                else if(clickMode == OptMorePopup::ClickMode::Add_MyCollection){
                    QJsonArray tracks = QJsonArray();
                    tracks.append(this->jsonArr_rosetubeTrack[section].at(index).toObject());

                    QJsonObject json = QJsonObject();
                    json.insert("tracks", tracks);

                    Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::ROSETUBE, json, this);
                    dialog_playlist->request_playlist_fetch();
                    connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &RoseTubeHome::slot_add_rosePlaylist_withRosetube);
                    int result = dialog_playlist->exec();

                    if(result == QDialog::Accepted){
                        delete dialog_playlist;
                    }
                }
                else if(clickMode == OptMorePopup::ClickMode::SubscribInfo){
                    QJsonObject tmpObj = this->jsonArr_rosetubeTrack[section].at(index).toObject();
                    QJsonObject dataObj = ProcJsonEasy::getJsonObject(tmpObj, "data");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(dataObj, "snippet");
                    QString channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                    QString channelId = ProcJsonEasy::getString(snippet, "channelId");

                    QJsonObject tmp_data;
                    tmp_data.insert(KEY_PAGE_CODE, PAGECODE_SUB_CHANNEL);
                    tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_SUB_CHANNEL);
                    tmp_data.insert("title", channelName);
                    tmp_data.insert("channel_id", channelId);

                    emit this->signal_clickedViewAll(tmp_data);
                }
                else if(clickMode == OptMorePopup::ClickMode::Share){
                    setUIShare();//c220818
                }
                else if(clickMode == OptMorePopup::ClickMode::Caching_ahead){
                    QJsonObject trackInfo = this->jsonArr_rosetubeTrack[section].at(index).toObject();

                    QString channelId = "";
                    QString channelName = "";
                    QString duration = "";
                    QString id = "";
                    QString thumbnailUrl = "";
                    QString title = "";

                    if(trackInfo.contains("data")){
                        QJsonObject trackData = ProcJsonEasy::getJsonObject(trackInfo, "data");
                        QJsonObject detailContents = ProcJsonEasy::getJsonObject(trackData, "detailContents");
                        QJsonObject snippet = ProcJsonEasy::getJsonObject(trackData, "snippet");
                        QJsonObject trackStatistic = ProcJsonEasy::getJsonObject(trackData, "detailContents");

                        QString tmpDuration = ProcJsonEasy::getString(detailContents, "duration");

                        QString time = tmpDuration;
                        QString hour;
                        QString minute;
                        QString second;

                        if(tmpDuration.indexOf("PT") >= 0){
                            time = tmpDuration.right(tmpDuration.size() - 2);
                        }
                        if(time.indexOf("H") >= 0){
                            hour = time.section("H", 0, -2);
                            time = time.mid(time.indexOf("H") + 1);
                        }
                        if(time.indexOf("M") >= 0){
                            minute = time.section("M", 0, -2);
                            time = time.mid(time.indexOf("M") + 1);
                        }
                        if(time.indexOf("S") >= 0){
                            second = time.section("S", 0, -2);
                        }

                        if(hour.isEmpty()){
                            if(minute.isEmpty()){
                                duration = "0:" + second.rightJustified(2, '0');
                            }
                            else{
                                duration = minute + ":" + second.rightJustified(2, '0');
                            }
                        }
                        else{
                            duration = hour + ":" + minute.rightJustified(2, '0') + ":" + second.rightJustified(2, '0');
                        }

                        channelId = ProcJsonEasy::getString(snippet, "channelId");
                        channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                        id = ProcJsonEasy::getString(trackInfo, "clientKey");
                        thumbnailUrl = ProcJsonEasy::getString(trackInfo, "thumbnailUrl");
                        title = ProcJsonEasy::getString(trackInfo, "title");
                    }
                    else{
                        if(trackInfo.contains("channelTitle")){
                            channelName = ProcJsonEasy::getString(trackInfo, "channelTitle");
                        }
                        else if(trackInfo.contains("channelName")){
                            channelName = ProcJsonEasy::getString(trackInfo, "channelName");
                        }

                        int time = ProcJsonEasy::getInt(trackInfo, "duration");
                        if(time == 0){
                            QString tmpDur = ProcJsonEasy::getString(trackInfo, "duration");

                            if(tmpDur.isEmpty()){
                                duration = "";
                            }
                            else{
                                duration = tmpDur;
                            }
                        }
                        else if(time >= 3600){
                             duration = QDateTime::fromTime_t(time).toUTC().toString("hh:mm:ss");
                        }
                        else{
                            duration = QDateTime::fromTime_t(time).toUTC().toString("mm:ss");
                        }

                        channelId = ProcJsonEasy::getString(trackInfo, "channelId");
                        id = ProcJsonEasy::getString(trackInfo, "id");
                        thumbnailUrl = ProcJsonEasy::getString(trackInfo, "thumbnailUrl");
                        title = ProcJsonEasy::getString(trackInfo, "title");
                    }

                    QJsonObject data;
                    data.insert("channelId", channelId);
                    data.insert("channelName", channelName);
                    data.insert("duration", duration);
                    data.insert("id", id);
                    data.insert("isFavorite", false);
                    data.insert("isLiveStream", false);
                    data.insert("thumbnailUrl", thumbnailUrl);
                    data.insert("thumbsUpPercentage", 0);
                    data.insert("title", title);
                    data.insert("viewsCount", "-1");

                    QJsonObject json;
                    json.insert("data", data);
                    json.insert("isDelete", false);

                    NetworkHttp *network = new NetworkHttp;
                    connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));

                    QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_download_item";
                    network->request(HTTP_CACHE,
                                     Url,
                                     json,
                                     true,
                                     true);
                }
            }
            else{
                // Track 바로 재생 - clicked 항목만 재생
                int curr_clickMode = global.device.getMusicPlayType();

                OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                switch(curr_clickMode)
                {
                    case 13:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last;
                        break;
                    case 12:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty;
                        break;
                    case 15:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_RightNow;
                        break;
                    case 16:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext;
                        break;
                    default:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_RightNow;
                        break;
                }

                QJsonObject tmpObj = this->jsonArr_rosetubePlaylist[section - 10].at(index).toObject();
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(ProcJsonEasy::getInt(tmpObj, "id"), playType);//c220818-trackplayid
            }
        }
    }

    void RoseTubeHome::slot_optMorePopup_menuClicked22(int id){//c220818
        print_debug();
        qDebug() << "id=" << id;
        OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
        roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
        procRosePlay->requestPlayRose_byPlaylistID(id, playType);//c220818-trackplayid
    }

    void RoseTubeHome::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << "p_jsonObj = " << strJson;*/

        print_debug();
        qDebug() << "p_id=" << p_id;
        //QJsonDocument doc(p_jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"SearchRemoteBar::slot_responseHttp---" << strJson;

        if(p_id==HTTP_RECOMMEND){

            if(p_jsonObj.contains("roseTubeCategory")){
                QJsonObject tmpObj = ProcJsonEasy::getJsonObject(p_jsonObj, "roseTubeCategory");
                this->rosetubeCategory = ProcJsonEasy::getJsonArray(tmpObj, "roseTubeSubCategories");

                this->flag_Rosetube[1] = true;
            }

            this->flag_Rosetube[0] = true;
            this->setUIControl_appendWidget_rose();
        }
        else if(p_id == HTTP_CACHE){
            if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
            }
        }

        sender()->deleteLater();
    }

}
