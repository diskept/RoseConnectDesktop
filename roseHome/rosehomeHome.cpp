#include "roseHome/rosehomeHome.h"

#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"
#include "widget/toastmsg.h"
#include "widget/VerticalScrollArea.h"

#include <QDebug>
#include <QScroller>


namespace roseHome {

    // subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX_SUBTITLE_Recommends = 0;
    const int BTN_IDX_SUBTITLE_RecentlyPlay = 1;
    const int BTN_IDX_SUBTITLE_RecentlyRosetube = 2;
    const int BTN_IDX_SUBTITLE_RecentlyTrack = 3;
    const int BTN_IDX_SUBTITLE_MyPlaylists = 4;
    const int BTN_IDX_SUBTITLE_NewPlaylists = 5;
    const int BTN_IDX_SUBTITLE_PopPlaylists = 6;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___Recommends = 0;
    const int SECTION_FOR_MORE_POPUP___RecentlyAlbum = 1;
    const int SECTION_FOR_MORE_POPUP___RecentlyPlaylist = 2;
    const int SECTION_FOR_MORE_POPUP___RecentlyRosetube = 3;
    const int SECTION_FOR_MORE_POPUP___RecentlyTrack = 4;
    const int SECTION_FOR_MORE_POPUP___MyPlaylists = 5;
    const int SECTION_FOR_MORE_POPUP___NewPlaylists = 6;
    const int SECTION_FOR_MORE_POPUP___PopPlaylists = 7;

    const int HTTP_RECOMMENDS = 0;
    const int HTTP_RECENTLY_ALBUM = 1;
    const int HTTP_RECENTLY_PLAYLIST = 2;
    const int HTTP_RECENTLY_TRACK = 3;
    const int HTTP_RECENTLY_ROSETUBE = 4;
    const int HTTP_MY_PLAYLIST = 5;
    const int HTTP_NEW_PLAYLIST = 6;
    const int HTTP_POP_PLAYLIST = 7;

    const int HTTP_ROSETUBE_CACHE = 10;

    // ROSE API 관련
    QString ROSE_API_RECOMM_PATH = "member/recommend";
    QString ROSE_API_ALBUM_PATH = "member/album/recent";
    QString ROSE_API_PLAYLIST_PATH = "member/playlist/recent";
    QString ROSE_API_ROSETUBE_PATH = "member/track/recent";
    QString ROSE_API_TRACK_PATH = "member/track/recent";
    QString ROSE_API_MYPLAYLIST_PATH = "member/playlist";
    QString ROSE_API_NEWPLAYLIST_PATH = "member/playlist/all";
    QString ROSE_API_POPPLAYLIST_PATH = "member/playlist/all";

    QString ALBTAB_STEP_ALBUM = "album";
    QString ALBTAB_STEP_PLAYLIST = "playlist";


    /**
     * @brief "RoseHome > 홈" 화면을 위한 생성자. @see PAGECODE_RH_HOME
     * @param parent
     */
    RoseHome::RoseHome(QWidget *parent) : AbstractRoseHomeSubWidget(VerticalScroll_rosefilter, parent) {

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_logined()), SLOT(slot_getMyInfo_loginAfter()));
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));
        connect(linker, SIGNAL(signal_message_show_timeout()), SLOT(slot_time_out()));

        // 기본 UI 세팅
        this->btnStyle_normal = "QPushButton{ ";
        this->btnStyle_normal += "background:transparent;color:#FFFFFF;font-size:15px;";
        this->btnStyle_normal += "border:1px solid #707070; border-radius:20px;}";
        this->btnStyle_normal += "QPushButton:hover{background-color:#BE905F;}";

        this->btnStyle_selected = "QPushButton{ ";
        this->btnStyle_selected += "background:#BE905F;color:#FFFFFF;font-size:15px;";
        this->btnStyle_selected += "border:1px solid #BE905F; border-radius:20px;}";

        this->mediaTypeList << tr("All Music") << tr("Mix") << tr("Music") << tr("RoseTube") << tr("Tidal") << tr("Bugs") << tr("Qobuz") << tr("Apple Music");      // << "VIDEO" << "SPOTIFY";

        this->setUIControl_chooseFilter();

        //data
        this->list_Recommends = new QList<RecommendsItemData>();
        this->list_RecPlaylist = new QList<PlaylistItemData>();
        this->list_RecentAlbum = new QList<AlbumItemData>();
        this->list_RecentPlaylist = new QList<PlaylistItemData>();
        this->list_RecentTrack = new QList<roseHome::TrackItemData>;
        this->list_MyPlaylist = new QList<PlaylistItemData>();
        this->list_NewPlaylist = new QList<PlaylistItemData>();
        this->list_PopPlaylist = new QList<PlaylistItemData>();

        this->jsonArr_rosetube = QJsonArray();
        this->jsonArr_tracks_toPlay = QJsonArray();
    }


    /**
     * @brief 소멸자.
     */
    RoseHome::~RoseHome(){

        this->deleteLater();
    }


    /**
     * @brief RoseHome::setActivePage
     */
    void RoseHome::setActivePage(){

        if(this->flagNeedReload == false){

            if(global.user.isValid() == true){

                // 항상 부모클래스의 함수 먼저 호출
                AbstractRoseHomeSubWidget::setActivePage();
                this->flagNeedReload = true;

                if(this->flagSet_media == false){
                    if(global.user.flag_favMusic_order == false){
                        global.user.flag_favorite_filter = true;
                        for(int i = 0; i < 30; i++){
                            global.user.enable_home_filter[i] = 0;
                            this->flag_filter[i] = false;
                        }
                    }
                    this->setUIControl_chooseFilter_fromData();
                }

                this->recom_path = "";

                // init data
                this->list_Recommends->clear();
                this->list_RecPlaylist->clear();
                this->list_RecentAlbum->clear();
                this->list_RecentPlaylist->clear();
                this->list_RecentTrack->clear();
                this->list_MyPlaylist->clear();
                this->list_NewPlaylist->clear();
                this->list_PopPlaylist->clear();

                flag_recommends[0] = false;
                flag_recentAlbum[0] = false;
                flag_recentPlaylist[0] = false;
                flag_recentRosetube[0] = false;
                flag_recentTrack[0] = false;
                flag_myPlaylist[0] = false;
                flag_newPlaylist[0] = false;
                flag_popPlaylist[0] = false;

                flag_recommends[1] = false;
                flag_recentAlbum[1] = false;
                flag_recentPlaylist[1] = false;
                flag_recentRosetube[1] = false;
                flag_recentTrack[1] = false;
                flag_myPlaylist[1] = false;
                flag_newPlaylist[1] = false;
                flag_popPlaylist[1] = false;

                this->box_contents->removeWidget(this->widget_rose_contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->box_rose_contents = new QVBoxLayout();
                this->box_rose_contents->setSpacing(0);
                this->box_rose_contents->setContentsMargins(0, 30, 0, 0);

                this->widget_rose_contents = new QWidget();
                this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_rose_contents->setLayout(this->box_rose_contents);
                this->widget_rose_contents->hide();

                this->widget_login_contents = setUIControl_LoginBefore();
                this->widget_login_contents->hide();

                this->stackedWidget_Contents = new QStackedWidget();
                this->stackedWidget_Contents->addWidget(this->widget_rose_contents);
                this->stackedWidget_Contents->addWidget(this->widget_login_contents);


                this->box_contents->addWidget(this->stackedWidget_Contents);
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                QJsonArray *p_jsonArray_titlSub = new QJsonArray();
                QJsonObject sub1 { {"name", tr("Album")}, {"code", ALBTAB_STEP_ALBUM} };
                QJsonObject sub2 { {"name", tr("Playlist")}, {"code", ALBTAB_STEP_PLAYLIST} };

                p_jsonArray_titlSub->push_back(sub1);
                p_jsonArray_titlSub->push_back(sub2);

                this->menubar = new TopMenuBar(TopMenuBar::ShowMenuStyle::ListSubMenuBar);
                this->menubar->setDataTopMenuBar(QJsonObject(), p_jsonArray_titlSub);

                connect(this->menubar, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(slot_changedSubTabUI(QJsonObject)));

                this->contentStep = "album";
                this->menubar->setSelectedSubMenuNoSignal(this->contentStep);
                this->menubar->hide();

                this->stackRecent = new QStackedWidget();
                this->stackRecent->setFixedHeight(305);

                // init UI
                this->vBox_recommends = new QVBoxLayout();
                this->vBox_recentlyPlay = new QVBoxLayout();
                this->vBox_recentlyRoseTube = new QVBoxLayout();
                this->vBox_recentlyTrack = new QVBoxLayout();
                this->vBox_myPlaylist = new QVBoxLayout();
                this->vBox_newPlaylist = new QVBoxLayout();
                this->vBox_popPlaylist = new QVBoxLayout();

                this->hBox_recommends = new QHBoxLayout();
                this->hBox_recentlyAlbum = new QHBoxLayout();
                this->hBox_recentlyPlaylist = new QHBoxLayout();
                this->hBox_recentlyRoseTube = new QHBoxLayout();
                this->hBox_myPlaylist = new QHBoxLayout();
                this->hBox_newPlaylist = new QHBoxLayout();
                this->hBox_popPlaylist = new QHBoxLayout();

                GSCommon::clearLayout(this->vBox_recommends);
                GSCommon::clearLayout(this->vBox_recentlyPlay);
                GSCommon::clearLayout(this->vBox_recentlyRoseTube);
                GSCommon::clearLayout(this->vBox_recentlyTrack);
                GSCommon::clearLayout(this->vBox_myPlaylist);
                GSCommon::clearLayout(this->vBox_newPlaylist);
                GSCommon::clearLayout(this->vBox_popPlaylist);

                GSCommon::clearLayout(this->hBox_recommends);
                GSCommon::clearLayout(this->hBox_recentlyAlbum);
                GSCommon::clearLayout(this->hBox_recentlyPlaylist);
                GSCommon::clearLayout(this->hBox_recentlyRoseTube);
                GSCommon::clearLayout(this->hBox_myPlaylist);
                GSCommon::clearLayout(this->hBox_newPlaylist);
                GSCommon::clearLayout(this->hBox_popPlaylist);

                // sub Title UI
                for(int i = 0; i < 10; i++){
                    this->lb_subTitle[i] = new QLabel();
                    this->btnView_all[i] = new QPushButton();
                }

                for(int i = 0; i < 15; i++){
                    this->home_recommends[i] = new roseHome::ItemRecommends_rosehome(i, SECTION_FOR_MORE_POPUP___Recommends, tidal::AbstractItem::ImageSizeMode::Square_284x284);
                    connect(this->home_recommends[i], &roseHome::ItemRecommends_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);
                }

                for(int i = 0; i < 15; i++){
                    this->home_recently_album[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___RecentlyAlbum, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                    connect(this->home_recently_album[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemAlbum);
                }

                for(int i = 0; i < 15; i++){
                    this->home_recently_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___RecentlyPlaylist, tidal::AbstractItem::ImageSizeMode::Square_200x200, HTTP_RECENTLY_PLAYLIST, true);
                    connect(this->home_recently_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);
                }

                for(int i = 0; i < 15; i++){
                    this->home_recently_rosetube[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___RecentlyRosetube, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                    connect(this->home_recently_rosetube[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);
                }

                for (int i = 0; i < 5; i++) {
                    this->home_recently_track[i] = new PlaylistTrackDetailInfo_RHV;
                    connect(this->home_recently_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &RoseHome::slot_clickedItemTrack_inList);
                    this->home_recently_track[i]->setProperty("index", i);
                    this->home_recently_track[i]->setObjectName("recently track");
                }

                for(int i = 0; i < 15; i++){
                    this->home_myPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___MyPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, HTTP_MY_PLAYLIST, true);
                    connect(this->home_myPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);
                }

                for(int i = 0; i < 15; i++){
                    this->home_newPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___NewPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, HTTP_NEW_PLAYLIST, true);
                    connect(this->home_newPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);
                }

                for(int i = 0; i < 15; i++){
                    this->home_popPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PopPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, HTTP_POP_PLAYLIST, true);
                    connect(this->home_popPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);
                }

                if(global.user.getAccess_token().isEmpty()){

                    this->widget_rose_contents->hide();
                    this->widget_login_contents->show();
                    this->stackedWidget_Contents->setCurrentIndex(1);

                    if(global.enable_section_left == true){
                        global.enable_section_left = false;
                    }
                }
                else {

                    global.enable_message_count = 0;
                    global.enable_message_flag = true;
                    ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223

                    this->setUIControl_requestRose();

                    this->widget_rose_contents->show();
                    this->widget_login_contents->hide();
                    this->stackedWidget_Contents->setCurrentIndex(0);
                }
            }
            else{
                this->flagNeedReload = true;

                // init data
                this->list_Recommends->clear();
                this->list_RecPlaylist->clear();
                this->list_RecentAlbum->clear();
                this->list_RecentPlaylist->clear();
                this->list_RecentTrack->clear();
                this->list_MyPlaylist->clear();
                this->list_NewPlaylist->clear();
                this->list_PopPlaylist->clear();

                flag_recommends[0] = false;
                flag_recentAlbum[0] = false;
                flag_recentPlaylist[0] = false;
                flag_recentRosetube[0] = false;
                flag_recentTrack[0] = false;
                flag_myPlaylist[0] = false;
                flag_newPlaylist[0] = false;
                flag_popPlaylist[0] = false;

                flag_recommends[1] = false;
                flag_recentAlbum[1] = false;
                flag_recentPlaylist[1] = false;
                flag_recentRosetube[1] = false;
                flag_recentTrack[1] = false;
                flag_myPlaylist[1] = false;
                flag_newPlaylist[1] = false;
                flag_popPlaylist[1] = false;


                this->box_contents->removeWidget(this->widget_rose_contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->box_rose_contents = new QVBoxLayout();
                this->box_rose_contents->setSpacing(0);
                this->box_rose_contents->setContentsMargins(0, 0, 0, 0);

                this->widget_rose_contents = new QWidget();
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


    /**
     * @brief Filter 선택하는 부분. 최상단.
     */
    void RoseHome::setUIControl_chooseFilter(){

        // 상단 필터
        this->btn_filter_ico = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
        this->btn_filter_ico->setFixedSize(80, 70);
        this->btn_filter_ico->setCursor(Qt::PointingHandCursor);

        connect(this->btn_filter_ico, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter()));

        this->box_media_filter  = new QHBoxLayout();
        this->box_media_filter->setContentsMargins(0, 0, 0, 0);
        this->box_media_filter->setSpacing(0);
        this->box_media_filter->setAlignment(Qt::AlignLeft);
        this->box_media_filter->addWidget(this->btn_filter_ico);
        this->box_media_filter->setSpacing(10);

        QWidget *widget_btnFilter = new QWidget();
        widget_btnFilter->setObjectName("widget_btnFilter");
        widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color:#212121; }");
        widget_btnFilter->setLayout(this->box_media_filter);

        this->box_filter->addWidget(widget_btnFilter, 0, Qt::AlignLeft);

        this->btn_filter_cnt = this->mediaTypeList.size();

        for(int i = 0; i < this->btn_filter_cnt; i++){
            this->lb_media[i] = new QLabel();
            this->lb_media[i]->setContentsMargins(20,0,20,0);
            this->lb_media[i]->setFixedHeight(30);
            this->lb_media[i]->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->lb_media[i]->setAlignment(Qt::AlignCenter);
        }
    }


    void RoseHome::setUIControl_chooseFilter_fromData(){

        QPushButton *btn_close_media = new QPushButton();
        btn_close_media->setText(tr("Select Media Type"));
        btn_close_media->setFixedSize(470, 50);
        btn_close_media->setStyleSheet("color:#CCCCCC;font-size:16px;background-color:transparent;");
        btn_close_media->setContentsMargins(0, 0, 0, 0);
        btn_close_media->setCursor(Qt::PointingHandCursor);

        connect(btn_close_media, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter_close()));

        QHBoxLayout *box_header = new QHBoxLayout;
        box_header->setContentsMargins(0, 0, 0, 0);
        box_header->setSpacing(0);
        box_header->addWidget(btn_close_media);

        QFrame *line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFixedHeight(1);
        line->setLineWidth(1);
        line->setStyleSheet("background-color:#4D4D4D;border:none;");

        this->flowLayout_media = new FlowLayout(0, 20, 20);
        this->flowLayout_media->setContentsMargins(10, 20, 10, 20);
        this->flowLayout_media->setSizeConstraint(QLayout::SetMinimumSize);

        if(this->btn_filter_cnt > 0){
            this->flagSet_media = true;
        }

        int check_btn = 0;
        for(int i = 0 ; i < this->btn_filter_cnt; i++){

            this->str_media_name[i] = this->mediaTypeList.at(i);

            this->enable_media_id[i] = 0;

            this->btn_filter[i] = new QPushButton();
            this->btn_filter[i]->setText(this->str_media_name[i]);
            this->btn_filter[i]->setFixedSize(QSize(135, 40));
            this->btn_filter[i]->setStyleSheet(global.user.enable_home_filter[i] == 1 ? this->btnStyle_selected : this->btnStyle_normal);
            this->btn_filter[i]->setObjectName("media");
            this->btn_filter[i]->setProperty("idx", i);
            this->btn_filter[i]->setCursor(Qt::PointingHandCursor);

            if(global.user.enable_home_filter[i] == 1){
                this->lb_media[i]->setText(this->str_media_name[i]);
                this->box_media_filter->addWidget(this->lb_media[i]);
                this->flag_filter[i] = true;
                check_btn++;
            }

            connect(this->btn_filter[i], SIGNAL(clicked()), this, SLOT(slot_clickBtn_chooseMedia()));

            this->flowLayout_media->addWidget(this->btn_filter[i]);
        }

        if(check_btn == 0){
            this->btn_filter[0]->setStyleSheet(this->btnStyle_selected);
            this->lb_media[0]->setText(this->str_media_name[0]);
            this->box_media_filter->addWidget(this->lb_media[0]);
            this->flag_filter[0] = true;
            global.user.enable_home_filter[0] = 1;
        }

        QPushButton *btn_apply_media = new QPushButton();
        btn_apply_media->setObjectName("btn_apply_media");
        btn_apply_media->setFixedSize(170, 40);
        btn_apply_media->setStyleSheet("#btn_apply_media{background:#B18658;color:#FFFFFF;font-size:20px;border:1px solid #B18658;border-radius:4px;} #btn_apply_media:hover{background:#7D6144;color:#FFFFFF;font-size:20px;border:1px solid #7D6144;border-radius:4px;}");
        btn_apply_media->setText(tr("Apply"));
        btn_apply_media->setCursor(Qt::PointingHandCursor);

        connect(btn_apply_media, SIGNAL(clicked()), this, SLOT(slot_clickBtn_applyMedia()));

        QPushButton *btn_cancel_media = new QPushButton();
        btn_cancel_media->setObjectName("btn_cancel_media");
        btn_cancel_media->setFixedSize(170, 40);
        btn_cancel_media->setStyleSheet("#btn_cancel_media{background:#777777;color:#FFFFFF;font-size:20px;border:1px solid #777777;border-radius:4px;} #btn_cancel_media:hover{background:#4D4D4D;color:#FFFFFF;font-size:20px;border:1px solid #4D4D4D;border-radius:4px;}");
        btn_cancel_media->setText(tr("Cancel"));
        btn_cancel_media->setCursor(Qt::PointingHandCursor);

        connect(btn_cancel_media, SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_close()));

        QHBoxLayout *box_apply = new QHBoxLayout;
        box_apply->setContentsMargins(0, 0, 0, 0);
        box_apply->setSpacing(30);
        box_apply->setAlignment(Qt::AlignCenter);
        box_apply->addWidget(btn_apply_media, 0);
        box_apply->addWidget(btn_cancel_media, 1);


        QVBoxLayout *box_total = new QVBoxLayout;
        box_total->setContentsMargins(0, 0, 0, 0);
        box_total->setSpacing(0);
        box_total->setAlignment(Qt::AlignTop);
        box_total->addLayout(box_header);
        box_total->addWidget(line);
        box_total->addLayout(this->flowLayout_media, 0);
        box_total->addLayout(box_apply, 1);

        this->widget_filter = new QWidget(this);
        this->widget_filter->setObjectName("widget_filter");
        this->widget_filter->setStyleSheet("#widget_filter { background-color:#333333;border:1px solid #4D4D4D; border-radius:3px; }");
        this->widget_filter->setFixedSize(470, 320);
        this->widget_filter->move(70, 10);
        this->widget_filter->setContentsMargins(1, 1, 1, 1);

        QPushButton *btn_total = new QPushButton(this->widget_filter);
        btn_total->setStyleSheet("background:transparent; border:0px");
        btn_total->setCursor(Qt::PointingHandCursor);
        btn_total->setLayout(box_total);
        btn_total->setFixedSize(470, 320);
        btn_total->setGeometry(0, 0, 0, 0);

        connect(btn_total, SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_close()));

        this->widget_filter->hide();
    }


    QWidget* RoseHome::setUIControl_LoginBefore(){

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
        btn_login->setFixedSize(230,55);
        btn_login->setGeometry(635, top + lb_text->sizeHint().height() + 30, 230, 55);


        // 커넥션
        connect(btn_login, &QPushButton::clicked, this, &RoseHome::slot_showLoginPage);

        return widget_loginBefore;
    }


    void RoseHome::setUIControl_requestRose(){

        QStringList media;
        media << "" << "UNITED" << "MUSIC" << "YOUTUBE" << "TIDAL" << "BUGS" << "QOBUZ" << "APPLE_MUSIC";

        this->str_media_type = "";

        if(global.user.enable_home_filter[0] == 1){

        }
        else{
            for(int i = 1; i < this->btn_filter_cnt; i++){
                if(global.user.enable_home_filter[i] == 1){
                    if(this->str_media_type.isEmpty()){
                        this->str_media_type = media[i];
                    }
                    else{
                        this->str_media_type += "," + media[i];
                    }
                }
            }
        }

        ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

        QJsonObject json;

        if(global.user.enable_home_filter[0] == 1){
            this->recom_path = ROSE_API_RECOMM_PATH;
        }
        else{
            this->recom_path = ROSE_API_RECOMM_PATH + QString("?mediaTypes=%1").arg(this->str_media_type);
        }

        roseHome::ProcCommon *proc_recomm = new roseHome::ProcCommon(this);
        connect(proc_recomm, &roseHome::ProcCommon::completeReq_list_recommeds, this, &RoseHome::slot_applyResult_Recommends);
        proc_recomm->request_rose_getList_recommends(recom_path, 0, 15);

        roseHome::ProcCommon *proc_recent_album = new roseHome::ProcCommon(this);
        connect(proc_recent_album, &roseHome::ProcCommon::completeReq_list_albums, this, &RoseHome::slot_applyResult_recentlyAlbum);
        proc_recent_album->request_rose_getList_recentlyAlbums(ROSE_API_ALBUM_PATH, this->str_media_type, 0, 15);

        roseHome::ProcCommon *proc_recent_playlist = new roseHome::ProcCommon(this);
        connect(proc_recent_playlist, &roseHome::ProcCommon::completeReq_list_playlists, this, &RoseHome::slot_applyResult_recentlyPlaylist);
        proc_recent_playlist->request_rose_getList_recentlyPlaylists(ROSE_API_PLAYLIST_PATH, this->str_media_type, 0, 15);

        roseHome::ProcCommon *proc_recent_rosetube = new roseHome::ProcCommon(this);
        connect(proc_recent_rosetube, &roseHome::ProcCommon::completeReq_list_rosetubes, this, &RoseHome::slot_applyResult_recentlyRosetube);
        proc_recent_rosetube->request_rose_getList_recentlyRosetube(ROSE_API_ROSETUBE_PATH, "YOUTUBE", 0, 15);

        roseHome::ProcCommon *proc_recent_track = new roseHome::ProcCommon(this);
        connect(proc_recent_track, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseHome::slot_applyResult_recentlyTrack);

        if(global.user.enable_home_filter[0] == 1){
            proc_recent_track->request_rose_getList_recentlyTracks(ROSE_API_TRACK_PATH, "YOUTUBE", 0, 5);
        }
        else if(this->str_media_type == "YOUTUBE"){
            proc_recent_track->request_rose_getList_recentlyTracks(ROSE_API_TRACK_PATH, "ROSETUBE", 0, 5);
        }
        else{
            proc_recent_track->request_rose_getList_recentlyTracks(ROSE_API_TRACK_PATH, this->str_media_type, 0, 5);
        }

        roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
        connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &RoseHome::slot_applyResult_MyPlaylist);
        proc_myPlaylist->request_rose_getList_myPlaylists(ROSE_API_MYPLAYLIST_PATH , this->str_media_type, 0, 15);

        roseHome::ProcCommon *proc_newPlaylist = new roseHome::ProcCommon(this);
        connect(proc_newPlaylist, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &RoseHome::slot_applyResult_NewPlaylist);
        proc_newPlaylist->request_rose_getList_usersPlaylists(ROSE_API_NEWPLAYLIST_PATH , "PLAYLIST_RECENT", this->str_media_type, 0, 15);

        roseHome::ProcCommon *proc_popPlaylist = new roseHome::ProcCommon(this);
        connect(proc_popPlaylist, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &RoseHome::slot_applyResult_PopPlaylist);
        proc_popPlaylist->request_rose_getList_usersPlaylists(ROSE_API_POPPLAYLIST_PATH , "PLAYLIST_THUMBUP", this->str_media_type, 0, 15);
    }


    void RoseHome::setUIControl_appendWidget_rose(){

        if(this->flag_recommends[0] == true && this->flag_recentAlbum[0] == true && this->flag_recentPlaylist[0] == true && this->flag_recentRosetube[0] == true &&
                this->flag_recentTrack[0] == true && this->flag_myPlaylist[0] == true && this->flag_newPlaylist[0] == true && this->flag_popPlaylist[0] == true){

            if(this->flag_recommends[0] == true){

                this->widget_recommends = new QWidget();
                QString subTitle = tr("Rose Recommend Playlist");
                if(this->list_Recommends->size() > 0){
                    this->widget_recommends = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->list_Recommends->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_Recommends, this->vBox_recommends);
                }
                else{
                    this->widget_recommends = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_Recommends, this->vBox_recommends);
                }

                this->vBox_recommends->addSpacing(10);

                if(this->flag_recommends[1] == true){

                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->hBox_recommends->setSpacing(0);
                    this->hBox_recommends->setContentsMargins(0, 0, 0, 0);
                    this->hBox_recommends->setAlignment(Qt::AlignTop);
                    this->hBox_recommends->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_recommends);
                    widget_content->setContentsMargins(0, 0, 0, 0);

                    QScrollArea *playlist_scrollArea = new QScrollArea();
                    playlist_scrollArea->setWidget(widget_content);
                    playlist_scrollArea->setWidgetResizable(false);
                    playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                    playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                    playlist_scrollArea->setFixedHeight(425);

                    QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_recommends->addWidget(playlist_scrollArea);

                    int maxCount = 0;
                    if(this->list_Recommends->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_Recommends->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recommends[i]->setData(this->list_Recommends->at(i));
                        this->hBox_recommends->addWidget(this->home_recommends[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 425);

                    this->vBox_recommends->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_recommends);
                this->box_rose_contents->addSpacing(80);
            }

            if(this->flag_recentAlbum[0] == true){

                this->widget_recentlyPlay = this->setUIControl_subTitle_withSubMenu(tr("Recently played"), "View All", BTN_IDX_SUBTITLE_RecentlyPlay, this->vBox_recentlyPlay);

                this->vBox_recentlyPlay->addSpacing(0);
                this->vBox_recentlyPlay->addWidget(this->stackRecent, 1, Qt::AlignTop);

                if(this->flag_recentAlbum[1] == true){

                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->hBox_recentlyAlbum->setSpacing(0);
                    this->hBox_recentlyAlbum->setContentsMargins(0, 0, 0, 0);
                    this->hBox_recentlyAlbum->setAlignment(Qt::AlignTop);
                    this->hBox_recentlyAlbum->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_recentlyAlbum);
                    widget_content->setContentsMargins(0, 0, 0, 0);

                    QScrollArea *album_scrollArea = new QScrollArea();
                    album_scrollArea->setWidget(widget_content);
                    album_scrollArea->setWidgetResizable(false);
                    album_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    album_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    album_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                    album_scrollArea->setContentsMargins(0, 0, 0, 0);
                    album_scrollArea->setFixedHeight(305);

                    QScroller::grabGesture(album_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->stackRecent->addWidget(album_scrollArea);

                    int maxCount = 0;
                    if(this->list_RecentAlbum->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_RecentAlbum->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_album[i]->setData(this->list_RecentAlbum->at(i));
                        this->hBox_recentlyAlbum->addWidget(this->home_recently_album[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                    noData_widget->setFixedSize(1500, 305);

                    this->stackRecent->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_recentlyPlay);
                this->box_rose_contents->addSpacing(60);
            }

            if(this->flag_recentPlaylist[1] == true){

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
                playlist_scrollArea->setFixedHeight(305);

                QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->stackRecent->addWidget(playlist_scrollArea);

                int maxCount = 0;
                if(this->list_RecentPlaylist->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_RecentPlaylist->size();
                }

                for(int i = 0; i < maxCount; i++){
                    this->home_recently_playlist[i]->setData(this->list_RecentPlaylist->at(i));
                    this->hBox_recentlyPlaylist->addWidget(this->home_recently_playlist[i]);
                }

                this->contentStep = ALBTAB_STEP_ALBUM;
                this->menubar->setSelectedSubMenuNoSignal(this->contentStep);
            }
            else{
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedSize(1500, 305);

                this->stackRecent->addWidget(noData_widget);

                this->contentStep = ALBTAB_STEP_ALBUM;
                this->menubar->setSelectedSubMenuNoSignal(this->contentStep);
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

                if(this->flag_recentRosetube[1] == true){
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
                    playlist_scrollArea->setFixedHeight(300);

                    QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_recentlyRoseTube->addWidget(playlist_scrollArea);

                    int maxCount = 0;
                    if(this->jsonArr_rosetube.count() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->jsonArr_rosetube.count();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_rosetube[i]->setData(this->jsonArr_rosetube.at(i).toObject());

                        this->hBox_recentlyRoseTube->addWidget(this->home_recently_rosetube[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                    noData_widget->setFixedSize(1500, 300);

                    this->vBox_recentlyRoseTube->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_recentlyRoseTube);
                this->box_rose_contents->addSpacing(80);
            }

            if(this->flag_recentTrack[0] == true){

                // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.
                QHBoxLayout *tmp_hBox = new QHBoxLayout();
                tmp_hBox->setSpacing(0);
                tmp_hBox->setContentsMargins(0, 0, 0, 0);
                tmp_hBox->setAlignment(Qt::AlignTop);

                this->widget_recentlyTrack = new QWidget();
                this->widget_recentlyTrack->setStyleSheet("background-color:#0d0d0d; border:0px;");
                this->widget_recentlyTrack->setContentsMargins(40, 50, 47, 35);
                this->widget_recentlyTrack->setLayout(tmp_hBox);

                QString subTitle = tr("Recently played tracks");
                if(this->list_RecentTrack->size() > 0){
                    this->lb_subTitle[BTN_IDX_SUBTITLE_RecentlyTrack]->setText(subTitle + QString(" (%1)").arg(this->list_RecentTrack->at(0).totalCount));
                }
                else{
                    this->lb_subTitle[BTN_IDX_SUBTITLE_RecentlyTrack]->setText(subTitle + QString(" (0)"));
                }
                this->lb_subTitle[BTN_IDX_SUBTITLE_RecentlyTrack]->setStyleSheet("font-size:24px;font-weight:bold;color:#FFFFFF;");
                tmp_hBox->addWidget(this->lb_subTitle[BTN_IDX_SUBTITLE_RecentlyTrack], 0, Qt::AlignVCenter);

                // side button - it works when the button text is not empty
                this->btnView_all[BTN_IDX_SUBTITLE_RecentlyTrack]->setText("View All");
                this->btnView_all[BTN_IDX_SUBTITLE_RecentlyTrack]->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
                this->btnView_all[BTN_IDX_SUBTITLE_RecentlyTrack]->setProperty("idx", BTN_IDX_SUBTITLE_RecentlyTrack);
                this->btnView_all[BTN_IDX_SUBTITLE_RecentlyTrack]->setCursor(Qt::PointingHandCursor);
                connect(this->btnView_all[BTN_IDX_SUBTITLE_RecentlyTrack], SIGNAL(clicked()), this, SLOT(slot_clickBtn_subTitle_viewAll()));

                tmp_hBox->addWidget(this->btnView_all[BTN_IDX_SUBTITLE_RecentlyTrack], 1, Qt::AlignVCenter | Qt::AlignRight);

                this->vBox_recentlyTrack->addWidget(this->widget_recentlyTrack);

                QVBoxLayout *vBox_recentlyTrack_info = new QVBoxLayout();
                vBox_recentlyTrack_info->setSpacing(0);
                vBox_recentlyTrack_info->setContentsMargins(40, 0, 54, 0);
                vBox_recentlyTrack_info->setAlignment(Qt::AlignTop);

                QWidget *widget_recentlyTrack_info = new QWidget();
                widget_recentlyTrack_info->setStyleSheet("background-color:#0d0d0d; border:0px;");
                widget_recentlyTrack_info->setContentsMargins(0, 0, 0, 0);
                widget_recentlyTrack_info->setFixedHeight(411);
                widget_recentlyTrack_info->setLayout(vBox_recentlyTrack_info);

                this->vBox_recentlyTrack->addWidget(widget_recentlyTrack_info);

                if(this->flag_recentTrack[1] == true){
                    int maxCount = 0;
                    if(this->list_RecentTrack->at(0).totalCount > 5){
                        maxCount = 5;
                    }
                    else{
                        maxCount = this->list_RecentTrack->at(0).totalCount;
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_track[i]->setDataTrackInfo_RoseMain(this->list_RecentTrack->at(i));
                        this->home_recently_track[i]->setFavoritesIds(this->list_RecentTrack->at(i).favorite, this->list_RecentTrack->at(i).star);
                        this->home_recently_track[i]->resize(1550, 70);

                        vBox_recentlyTrack_info->addWidget(this->home_recently_track[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
                    noData_widget->setFixedSize(1500, 350);

                    vBox_recentlyTrack_info->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_recentlyTrack);
                this->box_rose_contents->addSpacing(100);
            }

            if(this->flag_myPlaylist[0] == true){

                this->widget_myPlaylist = new QWidget();
                QString subTitle = tr("My playlists");
                if(this->list_MyPlaylist->size() > 0){
                    this->widget_myPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->list_MyPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_MyPlaylists, this->vBox_myPlaylist);
                }
                else{
                    this->widget_myPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_MyPlaylists, this->vBox_myPlaylist);
                }

                this->vBox_myPlaylist->addSpacing(10);

                if(this->flag_myPlaylist[1] == true){
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
                    playlist_scrollArea->setFixedHeight(305);

                    QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_myPlaylist->addWidget(playlist_scrollArea);

                    int maxCount = 0;
                    if(this->list_MyPlaylist->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_MyPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_myPlaylist[i]->setData(this->list_MyPlaylist->at(i));
                        this->hBox_myPlaylist->addWidget(this->home_myPlaylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 305);

                    this->vBox_myPlaylist->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_myPlaylist);
                this->box_rose_contents->addSpacing(80);
            }

            if(this->flag_newPlaylist[0] == true){

                this->widget_newPlaylist = new QWidget();
                QString subTitle = tr("New playlists");
                if(this->list_NewPlaylist->size() > 0){
                    this->widget_newPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->list_NewPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_NewPlaylists, this->vBox_newPlaylist);
                }
                else{
                    this->widget_newPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_NewPlaylists, this->vBox_newPlaylist);
                }

                this->vBox_newPlaylist->addSpacing(10);

                if(this->flag_newPlaylist[1] == true){
                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->hBox_newPlaylist->setSpacing(0);
                    this->hBox_newPlaylist->setContentsMargins(0, 0, 0, 0);
                    this->hBox_newPlaylist->setAlignment(Qt::AlignTop);
                    this->hBox_newPlaylist->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_newPlaylist);
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
                    this->vBox_newPlaylist->addWidget(playlist_scrollArea);

                    int maxCount = 0;
                    if(this->list_NewPlaylist->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_NewPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_newPlaylist[i]->setData(this->list_NewPlaylist->at(i));
                        this->hBox_newPlaylist->addWidget(this->home_newPlaylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 335);

                    this->vBox_newPlaylist->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_newPlaylist);
                this->box_rose_contents->addSpacing(80);
            }

            if(this->flag_popPlaylist[0] == true){

                this->widget_popPlaylist = new QWidget();
                QString subTitle = tr("Popular playlists");
                if(this->list_PopPlaylist->size() > 0){
                    this->widget_popPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->list_PopPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_PopPlaylists, this->vBox_popPlaylist);
                }
                else{
                    this->widget_popPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_PopPlaylists, this->vBox_popPlaylist);
                }

                this->vBox_popPlaylist->addSpacing(10);

                if(this->flag_popPlaylist[1] == true){
                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->hBox_popPlaylist->setSpacing(0);
                    this->hBox_popPlaylist->setContentsMargins(0, 0, 0, 0);
                    this->hBox_popPlaylist->setAlignment(Qt::AlignTop);
                    this->hBox_popPlaylist->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_popPlaylist);
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
                    this->vBox_popPlaylist->addWidget(playlist_scrollArea);

                    int maxCount = 0;
                    if(this->list_PopPlaylist->size() > 10){
                        maxCount = 10;
                    }
                    else{
                        maxCount = this->list_PopPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_popPlaylist[i]->setData(this->list_PopPlaylist->at(i));
                        this->hBox_popPlaylist->addWidget(this->home_popPlaylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 335);

                    this->vBox_popPlaylist->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_popPlaylist);
                this->box_rose_contents->addSpacing(60);
            }
        }
    }


    void RoseHome::slot_applyResult_Recommends(const QList<roseHome::RecommendsItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0){

            this->list_Recommends->append(list_data);
            this->flag_recommends[1] = true;

            for(int i = 0; i < list_data.count(); i++){
                PlaylistItemData tmp_data;

                tmp_data.id = list_data.at(i).id;
                tmp_data.star = list_data.at(i).star;
                tmp_data.ownerId = list_data.at(i).ownerId;
                tmp_data.thumbupCount = list_data.at(i).thumbupCount;
                tmp_data.trackCount = list_data.at(i).trackCount;
                tmp_data.sort = list_data.at(i).sort;

                tmp_data.type = list_data.at(i).type;
                tmp_data.title = list_data.at(i).title;
                tmp_data.share = list_data.at(i).share;
                tmp_data.ownerName = list_data.at(i).ownerName;
                tmp_data.thumbnail = list_data.at(i).thumbnail;
                tmp_data.registDateTime = list_data.at(i).registDateTime;
                tmp_data.clientKey = list_data.at(i).clientKey;

                tmp_data.favoriteTrack = list_data.at(i).favoriteTrack;
                tmp_data.favorite = list_data.at(i).favorite;
                tmp_data.thumbup = list_data.at(i).thumbup;
                tmp_data.isRose = list_data.at(i).isRose;

                for(int j = 0; j < list_data.at(i).tags.count(); j++){
                    tmp_data.tags.append(list_data.at(i).tags.at(j));
                }

                this->list_RecPlaylist->append(tmp_data);
            }
        }

        this->flag_recommends[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseHome::slot_applyResult_recentlyAlbum(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);
        //ContentLoadingwaitingMsgHide();//c220616
        if(list_data.length() > 0)
        {
            this->list_RecentAlbum->append(list_data);
            this->flag_recentAlbum[1] = true;
        }

        this->flag_recentAlbum[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseHome::slot_applyResult_recentlyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void RoseHome::slot_applyResult_recentlyRosetube(const QJsonArray &jsonArr, const int &totalCnt, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(jsonArr.count() > 0){
            this->flag_recentRosetube[1] = true;
            this->jsonArr_rosetube = jsonArr;
            this->recentRoseTube_cnt = totalCnt;
        }

        this->flag_recentRosetube[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseHome::slot_applyResult_recentlyTrack(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_RecentTrack->append(list_data);
            this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;
            this->flag_recentTrack[1] = true;
        }

        this->flag_recentTrack[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseHome::slot_applyResult_MyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void RoseHome::slot_applyResult_NewPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_NewPlaylist->append(list_data);
            this->flag_newPlaylist[1] = true;
        }

        this->flag_newPlaylist[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseHome::slot_applyResult_PopPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_PopPlaylist->append(list_data);
            this->flag_popPlaylist[1] = true;
        }

        this->flag_popPlaylist[0] = true;
        this->setUIControl_appendWidget_rose();
        this->slot_hide_msg();
    }


    /**
         * @brief RoseHome::slot_showLoginPage [SLOT] 로그인 버튼 클릭시
         */
    void RoseHome::slot_showLoginPage(){

        this->dlg_rose_login = new Dialog::DialogLogin(this);
        this->dlg_rose_login->exec();
    }


    /**
         * @brief RoseHome::slot_getMyInfo_loginAfter [SLOT] Rosehome Contents Display
         */
    void RoseHome::slot_getMyInfo_loginAfter(){

        if(global.user.isValid() == true && this->stackedWidget_Contents->currentIndex() == 1){

            this->list_Recommends = new QList<RecommendsItemData>();
            this->list_RecPlaylist = new QList<PlaylistItemData>();
            this->list_RecentAlbum = new QList<AlbumItemData>();
            this->list_RecentPlaylist = new QList<PlaylistItemData>();
            this->list_RecentTrack = new QList<roseHome::TrackItemData>;
            this->list_MyPlaylist = new QList<PlaylistItemData>();
            this->list_NewPlaylist = new QList<PlaylistItemData>();
            this->list_PopPlaylist = new QList<PlaylistItemData>();

            this->jsonArr_rosetube = QJsonArray();
            this->jsonArr_tracks_toPlay = QJsonArray();

            this->flagNeedReload = false;
            this->setActivePage();
        }
    }


    void RoseHome::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_ROSE)){

            global.user.setDeviceChange(false);

            this->list_Recommends = new QList<RecommendsItemData>();
            this->list_RecPlaylist = new QList<PlaylistItemData>();
            this->list_RecentAlbum = new QList<AlbumItemData>();
            this->list_RecentPlaylist = new QList<PlaylistItemData>();
            this->list_RecentTrack = new QList<roseHome::TrackItemData>;
            this->list_MyPlaylist = new QList<PlaylistItemData>();
            this->list_NewPlaylist = new QList<PlaylistItemData>();
            this->list_PopPlaylist = new QList<PlaylistItemData>();

            this->jsonArr_rosetube = QJsonArray();
            this->jsonArr_tracks_toPlay = QJsonArray();

            this->flagNeedReload = false;
            this->setActivePage();
        }
    }


    void RoseHome::changedOnlyTabUI_notSendSignal(QString p_step){

        this->contentStep = p_step;

        // Tab 타이틀 변경
        this->menubar->setSelectedSubMenuNoSignal(contentStep);

        // DB 가져와서 데이터 세팅
        if(this->contentStep == ALBTAB_STEP_ALBUM){
            this->stackRecent->setCurrentIndex(0);
        }else if(this->contentStep == ALBTAB_STEP_PLAYLIST){
            this->stackRecent->setCurrentIndex(1);
        }
    }


    void RoseHome::slot_changedSubTabUI(const QJsonObject &p_data){

        QString tmp_step = p_data[KEY_PAGE_CODE].toString();

        // 페이지 변경 및 데이터 세팅
        this->changedOnlyTabUI_notSendSignal(tmp_step);
    }


    void RoseHome::slot_hide_msg(){

        ContentLoadingwaitingMsgHide();//c220625

        if(global.enable_message_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;
        }
    }


    void RoseHome::slot_time_out(){

        ContentLoadingwaitingMsgHide();

        if(global.enable_message_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;

            this->box_contents->removeWidget(this->widget_rose_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);


            this->box_rose_contents = new QVBoxLayout();
            this->box_rose_contents->setSpacing(0);
            this->box_rose_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_rose_contents = new QWidget();
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
    }


    void RoseHome::slot_clickBtn_Filter(){

        this->widget_filter->setVisible(!widget_filter->isVisible());
    }


    void RoseHome::slot_clickBtn_Filter_close(){

        for(int idx = 0; idx < this->btn_filter_cnt; idx++){
            this->btn_filter[idx]->setStyleSheet(this->btnStyle_normal);
            this->flag_filter[idx] = false;

            if(global.user.enable_home_filter[idx] == 1){
                this->btn_filter[idx]->setStyleSheet(this->btnStyle_selected);
                this->flag_filter[idx] = true;
            }
        }

        this->filter_change = false;

        this->widget_filter->setVisible(false);
    }


    void RoseHome::slot_clickBtn_chooseMedia(){

        QString type = sender()->objectName();
        int idx = sender()->property("idx").toInt();
        int media_idx = 0;
        int change_cnt = 0;

        if(type == "media"){

            if(this->filter_change == false){
                for(int i = 0; i < this->btn_filter_cnt; i++){
                    this->enable_media_id[i] = global.user.enable_home_filter[i];
                }
            }

            if(idx == 0){
                for(int sel_idx = 0; sel_idx < this->btn_filter_cnt; sel_idx++){
                    this->btn_filter[sel_idx]->setStyleSheet(this->btnStyle_normal);
                    this->flag_filter[sel_idx] = false;
                    //global.user.enable_home_filter[sel_idx] = 0;
                    this->enable_media_id[sel_idx] = 0;
                }
            }
            else{
                //if(global.user.enable_home_filter[0] == 1){
                if(this->enable_media_id[0] == 1){
                    this->btn_filter[0]->setStyleSheet(this->btnStyle_normal);
                    this->flag_filter[0] = false;
                    //global.user.enable_home_filter[0] = 0;
                    this->enable_media_id[0] = 0;
                }
            }

            if(idx > 0 && this->enable_media_id[idx] == 1){
                this->btn_filter[idx]->setStyleSheet(this->btnStyle_normal);
                this->flag_filter[idx] = false;
                //global.user.enable_home_filter[idx] = 0;
                this->enable_media_id[idx] = 0;
            }
            else{
                this->btn_filter[idx]->setStyleSheet(this->btnStyle_selected);
                this->flag_filter[idx] = true;
                //global.user.enable_home_filter[idx] = 1;
                this->enable_media_id[idx] = 1;
            }

            for(int idx = 1; idx < this->btn_filter_cnt; idx++){
                //media_idx += global.user.enable_home_filter[idx] == 1 ? 1 : 0;
                media_idx += this->enable_media_id[idx] == 1 ? 1 : 0;

                change_cnt += this->enable_media_id[idx] == global.user.enable_home_filter[idx] ? 0 : 1;

                //global.user.enable_home_filter[idx] = this->enable_media_id[idx];
            }

            if(change_cnt > 0){
               this->filter_change = true;
            }

            if(media_idx == 7 || media_idx == 0){
                for(int sel_idx = 0; sel_idx < this->btn_filter_cnt; sel_idx++){
                    this->btn_filter[sel_idx]->setStyleSheet(this->btnStyle_normal);
                    this->flag_filter[sel_idx] = false;

                    //global.user.enable_home_filter[sel_idx] = 0;
                    this->enable_media_id[sel_idx] = 0;
                }

                this->btn_filter[0]->setStyleSheet(this->btnStyle_selected);
                this->flag_filter[0] = true;
                //global.user.enable_home_filter[idx] = 1;
                this->enable_media_id[idx] = 1;
            }
        }
    }


    void RoseHome::slot_clickBtn_applyMedia(){

        if(this->filter_change == true){

            for(int i = 0; i < this->btn_filter_cnt; i++){

                global.user.enable_home_filter[i] = this->enable_media_id[i];

                this->lb_media[i]->setText("");
                this->lb_media[i]->setStyleSheet("");

                if(this->flag_filter[i] == true){
                    this->box_media_filter->removeWidget(this->lb_media[i]);
                }

                this->flag_filter[i] = false;
            }

            GSCommon::clearLayout(this->box_media_filter);
            GSCommon::clearLayout(this->box_filter);
            this->setUIControl_chooseFilter();

            for(int i = 0; i < this->btn_filter_cnt; i++){

                if(global.user.enable_home_filter[i] == 1){
                    this->lb_media[i]->setText(this->str_media_name[i]);
                    this->box_media_filter->addWidget(this->lb_media[i]);

                    this->flag_filter[i] = true;
                }
            }

            this->flagNeedReload = false;
            this->setActivePage();
        }

        this->widget_filter->setVisible(false);

        this->filter_change = false;
    }


    void RoseHome::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();
        int list_count = 0;

        if(btnId == BTN_IDX_SUBTITLE_Recommends){
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = this->recom_path;       //ROSE_API_RECOMM_PATH;
            data_page.type = "Recommends";
            data_page.type_id = HTTP_RECOMMENDS;

            this->movePage_playlist_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_RecentlyPlay){
            if(this->contentStep == ALBTAB_STEP_ALBUM){

                if(this->list_RecentAlbum->count() > 0){
                    list_count = this->list_RecentAlbum->at(0).totalCount;
                }
                PageInfo_AlbumAllView data_page;
                data_page.pathTitle = this->lb_subTitle[btnId]->text() + tr(" Album ") + QString("(%1)").arg(list_count);
                data_page.api_subPath = ROSE_API_ALBUM_PATH;
                data_page.type = "RecentAlbum";
                data_page.type_id = HTTP_RECENTLY_ALBUM;
                data_page.filter_type = this->str_media_type;

                this->movePage_album_allView(data_page);
            }
            else if(this->contentStep == ALBTAB_STEP_PLAYLIST){

                if(this->list_RecentAlbum->count() > 0){
                    list_count = this->list_RecentPlaylist->at(0).totalCount;
                }
                PageInfo_PlaylistAllView data_page;
                data_page.pathTitle = this->lb_subTitle[btnId]->text() + tr(" Playlist ") + QString("(%1)").arg(list_count);
                data_page.api_subPath = ROSE_API_PLAYLIST_PATH;
                data_page.type = "RecentPlaylist";
                data_page.type_id = HTTP_RECENTLY_PLAYLIST;
                data_page.filter_type = this->str_media_type;

                this->movePage_playlist_allView(data_page);
            }
        }
        else if(btnId == BTN_IDX_SUBTITLE_RecentlyRosetube){
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = "/member/track/recent";
            data_page.type = "RecentRosetube";
            data_page.type_id = HTTP_RECENTLY_ROSETUBE;

            this->movePage_rosetube_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_RecentlyTrack){
            PageInfo_TrackAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = ROSE_API_TRACK_PATH;
            data_page.type = "RecentTrack";
            data_page.type_id = HTTP_RECENTLY_TRACK;
            if(global.user.enable_home_filter[0] == 1){
                data_page.filter_type = "YOUTUBE";
            }
            else if(this->str_media_type == "YOUTUBE"){
                data_page.filter_type = "ROSETUBE";
            }
            else{
                data_page.filter_type = this->str_media_type;
            }

            this->movePage_track_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_MyPlaylists){
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = ROSE_API_MYPLAYLIST_PATH;
            data_page.type = "MyPlaylist";
            data_page.type_id = HTTP_MY_PLAYLIST;
            data_page.filter_type = this->str_media_type;

            this->movePage_playlist_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_NewPlaylists){
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = ROSE_API_NEWPLAYLIST_PATH;
            data_page.type = "NewPlaylist";
            data_page.type_id = HTTP_NEW_PLAYLIST;
            data_page.filter_type = this->str_media_type;

            this->movePage_playlist_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_PopPlaylists){
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = ROSE_API_POPPLAYLIST_PATH;
            data_page.type = "PopPlaylist";
            data_page.type_id = HTTP_POP_PLAYLIST;
            data_page.filter_type = this->str_media_type;

            this->movePage_playlist_allView(data_page);
        }

    }

    QWidget* RoseHome::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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
            this->btnView_all[btnId]->setCursor(Qt::PointingHandCursor);
            connect(this->btnView_all[btnId], SIGNAL(clicked()), this, SLOT(slot_clickBtn_subTitle_viewAll()));

            tmp_hBox->addWidget(this->btnView_all[btnId], 1, Qt::AlignVCenter | Qt::AlignRight);
        }

        // Apply Main Layout with spacing
        p_layout->addWidget(widget_box_subTitle);

        return widget_box_subTitle;
    }


    QWidget* RoseHome::setUIControl_subTitle_withSubMenu(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

        // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.
        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        tmp_hBox->setSpacing(0);
        tmp_hBox->setContentsMargins(0,0,0,0);
        tmp_hBox->setAlignment(Qt::AlignTop);

        QWidget *widget_box_subTitle = new QWidget();
        widget_box_subTitle->setContentsMargins(0, 0, 0, 0);
        widget_box_subTitle->setLayout(tmp_hBox);

        this->lb_subTitle[btnId]->setText(subTitle);
        this->lb_subTitle[btnId]->setStyleSheet("font-size:24px;font-weight:bold;color:#FFFFFF;");
        tmp_hBox->addWidget(this->lb_subTitle[btnId], 0, Qt::AlignVCenter);
        tmp_hBox->addSpacing(10);
        tmp_hBox->addWidget(this->menubar, 1, Qt::AlignTop);
        this->menubar->show();

        // side button - it works when the button text is not empty
        if(btnText.isEmpty() == false){
            this->btnView_all[btnId]->setText(btnText);
            this->btnView_all[btnId]->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
            this->btnView_all[btnId]->setProperty("idx", btnId);
            this->btnView_all[btnId]->setCursor(Qt::PointingHandCursor);
            connect(this->btnView_all[btnId], SIGNAL(clicked()), this, SLOT(slot_clickBtn_subTitle_viewAll()));

            tmp_hBox->addWidget(this->btnView_all[btnId], 2, Qt::AlignVCenter | Qt::AlignRight);
        }

        // Apply Main Layout with spacing
        p_layout->addWidget(widget_box_subTitle);

        return widget_box_subTitle;
    }


    QHBoxLayout* RoseHome::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QHBoxLayout *hBox_album = new QHBoxLayout();
        hBox_album->setSpacing(0);
        hBox_album->setContentsMargins(0, 0, 0, 0);
        hBox_album->setAlignment(Qt::AlignTop);
        hBox_album->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_content = new QWidget;
        widget_content->setLayout(hBox_album);
        widget_content->setContentsMargins(0, 0, 0, 0);

        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_content);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0, 0, 0, 0);
        tmp_scrollArea->setFixedHeight(275);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_album;
    }


    /**
         * @brief Playlist 전용으로, 1 Row 형태의 가로스크롤 배치하는 Layout 구조 생성 후 반환
         * @return
         */
    QHBoxLayout* RoseHome::setUIControl_hBoxLayout_forPlaylists(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------
        QHBoxLayout *hBox_playlist = new QHBoxLayout();
        hBox_playlist->setSpacing(0);
        hBox_playlist->setContentsMargins(0, 0, 0, 0);
        hBox_playlist->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_contentSub = new QWidget();
        widget_contentSub->setLayout(hBox_playlist);
        widget_contentSub->setContentsMargins(0, 0, 0, 0);


        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_contentSub);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0, 0,0 ,0);
        tmp_scrollArea->setFixedHeight(295);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_playlist;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    void RoseHome::slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj){

        if(p_id == HTTP_ROSETUBE_CACHE){
            if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
            }
        }

    }


    /**
         * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
         * @param clickMode
         */
    void RoseHome::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리 - Album 파트가 여러개임
        if(section == SECTION_FOR_MORE_POPUP___RecentlyAlbum){
            this->proc_clicked_itemAlbum(this->list_RecentAlbum, clickMode, index, section);
        }
    }


    /**
         * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
         * @param clickMode
         */
    void RoseHome::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___Recommends){

            this->proc_clicked_itemPlaylist(this->list_RecPlaylist, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___RecentlyPlaylist){
            this->proc_clicked_itemPlaylist(this->list_RecentPlaylist, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___MyPlaylists){
            this->proc_clicked_itemPlaylist_rose(this->list_MyPlaylist, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___NewPlaylists){
            this->proc_clicked_itemPlaylist_rose(this->list_NewPlaylist, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___PopPlaylists){
            this->proc_clicked_itemPlaylist_rose(this->list_PopPlaylist, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___RecentlyRosetube){

            if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add){

                /*if(this->flag_check_track == false){
                        this->track_star_fav = this->home_recently_track[idx]->getFavoritesStars();
                        this->flag_track_fav = false;

                        if(this->track_star_fav == 3){
                            this->track_star_fav = 0;
                            this->flag_track_fav = false;
                        }
                        else if(this->track_star_fav >= 0 && this->track_star_fav < 3){
                            this->track_star_fav++;
                            this->flag_track_fav = true;
                        }

                        this->track_idx_fav = idx;

                        QJsonObject ratingInfo;
                        ratingInfo.insert("favorite", this->flag_track_fav);
                        ratingInfo.insert("star", this->track_star_fav);
                        ratingInfo.insert("thumbup", false);
                        ratingInfo.insert("type", );

                        QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

                        QJsonObject json;
                        json.insert("ratingInfo", ratingInfo);
                        json.insert("track", track);

                        // request HTTP API - get favorite for Rose Server
                        roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                        connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseHome::slot_applyResult_getRating_track);
                        proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

                        this->flag_check_track = true;
                    }*/
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
                    ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byTracks(data_playlist, this->jsonArr_rosetube, index, playType);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
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
                    data_header.type = ProcJsonEasy::getString(tmpObj, "type");      //j220906 share link
                    data_header.flagProcStar = false;
                    data_header.isShare = true;      //j220906 share link

                    // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Rosetube_Track, data_header, index, section);
                }
            }
        }
    }


    void RoseHome::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->home_recently_track[i]->setFavoritesIds(flag, star);
                }
            }
            else if(track_info.contains("message") && (track_info["message"].toString() == "정상")){
                this->home_recently_track[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    /**
         * @brief [Slot] PlaylistTrackDetailInfo_RHV signal에 대하여 처리
         * @param clickMode
         */
    void RoseHome::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___RecentlyTrack;

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->home_recently_track[idx]->getFavoritesStars();
                this->flag_track_fav = false;

                if(this->track_star_fav == 3){
                    this->track_star_fav = 0;
                    this->flag_track_fav = false;
                }
                else if(this->track_star_fav >= 0 && this->track_star_fav < 3){
                    this->track_star_fav++;
                    this->flag_track_fav = true;
                }

                this->track_idx_fav = idx;

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_track_fav);
                ratingInfo.insert("star", this->track_star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", this->list_RecentTrack->at(idx).type);

                QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseHome::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

                this->flag_check_track = true;
            }
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_RecentTrack, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }


    /**
         * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
         * @param clickMode
         * @param index
         * @param section
         */
    void RoseHome::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___Recommends){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_RecPlaylist, index, SECTION_FOR_MORE_POPUP___Recommends, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___RecentlyAlbum){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_RecentAlbum, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___RecentlyPlaylist){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_RecentPlaylist, index, SECTION_FOR_MORE_POPUP___RecentlyPlaylist, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___RecentlyRosetube){
            roseHome::TrackItemData data_track;
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                setUIShare();
            }
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
                ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data_track, this->jsonArr_rosetube, index, clickMode);
            }
            else if(clickMode == OptMorePopup::ClickMode::Caching_ahead){
                QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(index).toObject();

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
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_download_item";
                network->request(HTTP_ROSETUBE_CACHE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___RecentlyTrack){
            this->proc_clicked_optMorePopup_fromTrack(this->list_RecentTrack, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___MyPlaylists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_MyPlaylist, index, SECTION_FOR_MORE_POPUP___MyPlaylists, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___NewPlaylists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_NewPlaylist, index, SECTION_FOR_MORE_POPUP___NewPlaylists, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___PopPlaylists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_PopPlaylist, index, SECTION_FOR_MORE_POPUP___PopPlaylists, clickMode);
        }
    }
}
