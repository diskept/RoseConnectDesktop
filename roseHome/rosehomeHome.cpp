#include "roseHome/rosehomeHome.h"

#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/bugs_struct.h"
#include "bugs/ProcBugsAPI.h"

#include "qobuz/ProcCommon_forQobuz.h"

#include "tidal/ProcCommon.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "login/dialog_playlist_onRose.h"

#include "widget/dialogconfirm.h"
#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"

#include <QDebug>
#include <QScroller>


namespace roseHome {

    // subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX_SUBTITLE_Recommends = 0;
    const int BTN_IDX_SUBTITLE_RecentlyPlay = 1;
    const int BTN_IDX_SUBTITLE_RecentlyRosetube = 2;
    const int BTN_IDX_SUBTITLE_RecentlyTrack = 3;
    const int BTN_IDX_SUBTITLE_RecentlyArtist = 4;
    const int BTN_IDX_SUBTITLE_History = 5;
    const int BTN_IDX_SUBTITLE_MyPlaylists = 6;
    const int BTN_IDX_SUBTITLE_NewPlaylists = 7;
    const int BTN_IDX_SUBTITLE_PopPlaylists = 8;
    const int BTN_IDX_SUBTITLE_DuduoPlaylists = 9;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___Recommends = 0;
    const int SECTION_FOR_MORE_POPUP___RecentlyAlbum = 1;
    const int SECTION_FOR_MORE_POPUP___RecentlyPlaylist = 2;
    const int SECTION_FOR_MORE_POPUP___RecentlyRosetube = 3;
    const int SECTION_FOR_MORE_POPUP___RecentlyTrack = 4;
    const int SECTION_FOR_MORE_POPUP___RecentlyArtist = 5;
    const int SECTION_FOR_MORE_POPUP___History = 6;
    const int SECTION_FOR_MORE_POPUP___MyPlaylists = 7;
    const int SECTION_FOR_MORE_POPUP___NewPlaylists = 8;
    const int SECTION_FOR_MORE_POPUP___PopPlaylists = 9;
    const int SECTION_FOR_MORE_POPUP___DuduoPlaylists = 10;

    const int HTTP_RECOMMENDS = 0;
    const int HTTP_RECENTLY_ALBUM = 1;
    const int HTTP_RECENTLY_PLAYLIST = 2;
    const int HTTP_RECENTLY_TRACK = 3;
    const int HTTP_RECENTLY_ROSETUBE = 4;
    const int HTTP_HISTORY = 8;
    const int HTTP_RECENTLY_ARTIST = 10;
    const int HTTP_MY_PLAYLIST = 5;
    const int HTTP_NEW_PLAYLIST = 6;
    const int HTTP_POP_PLAYLIST = 7;
    const int HTTP_DUDUO_PLAYLIST = 9;

    const int HTTP_ROSETUBE_CACHE = 10;

    // ROSE API 관련
    QString ROSE_API_RECOMM_PATH = "member/recommend";
    QString ROSE_API_ALBUM_PATH = "member/album/recent";
    QString ROSE_API_PLAYLIST_PATH = "member/playlist/recent";
    QString ROSE_API_ROSETUBE_PATH = "member/track/recent";
    QString ROSE_API_TRACK_PATH = "member/track/recent";
    QString ROSE_API_ARTIST_PATH = "member/artist/recent";
    QString ROSE_API_HISTORY_PATH = "member/track/history";
    QString ROSE_API_MYPLAYLIST_PATH = "member/playlist";
    QString ROSE_API_NEWPLAYLIST_PATH = "member/playlist/all";
    QString ROSE_API_POPPLAYLIST_PATH = "member/playlist/all";
    QString ROSE_API_DUDUOPLAYLIST_PATH = "member/home/viewmore";

    QString ALBTAB_STEP_ALBUM = "album";
    QString ALBTAB_STEP_PLAYLIST = "playlist";


    /**
     * @brief "RoseHome > 홈" 화면을 위한 생성자. @see PAGECODE_RH_HOME
     * @param parent
     */
    RoseHome::RoseHome(QWidget *parent) : AbstractRoseHomeSubWidget(VerticalScroll_rosefilter, parent) {

        global.isDrawingMainContent = false;
        //dis_conn = nullptr;
        //ContentLoadingwaitingMsgHide();
        //this->widget_login_contents = nullptr;//c230525
        this->widget_login_contents_flag = false;//c230525

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_logined()), SLOT(slot_getMyInfo_loginAfter()));
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));
        connect(linker, SIGNAL(signal_message_show_timeout()), SLOT(slot_time_out()));//c230525

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
        this->list_RecentTrack = new QList<roseHome::TrackItemData>();
        this->list_RecentArtist = new QList<ArtistItemData>();
        this->list_Historylist = new QList<roseHome::HistoryItemData>();
        this->list_MyPlaylist = new QList<PlaylistItemData>();
        this->list_NewPlaylist = new QList<PlaylistItemData>();
        this->list_PopPlaylist = new QList<PlaylistItemData>();
        this->list_DuduoPlaylist = new QList<PlaylistItemData>();

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

        global.media.clear();////c230516
        global.media << "" << "UNITED" << "MUSIC" << "YOUTUBE" << "TIDAL" << "BUGS" << "QOBUZ" << "APPLE_MUSIC";////c230516
        print_debug();
        qDebug() << "global.media=" << global.media;//c230516

        if(this->flagNeedReload == false){
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//c230322_3

            if(global.user.isValid() == true){
                // 항상 부모클래스의 함수 먼저 호출
                AbstractRoseHomeSubWidget::setActivePage();
                this->flagNeedReload = true;

                if(this->flagSet_media == false){
                    if(global.user.flag_favMusic_order == false){
                        global.user.flag_favorite_filter = true;
                        for(int i = 0; i < 30; i++){
                            //global.user.enable_home_filter[i] = 0;
                            this->flag_filter[i] = false;
                        }
                    }
                    print_debug();
                    this->setUIControl_chooseFilter_fromData();
                }

                this->recom_path = "";

                if(this->flag_page_draw == true){
                    this->flag_page_draw = false;
                    this->box_contents->removeWidget(this->stackedWidget_Contents);

                    delete this->stackedWidget_Contents;
                }

                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                this->box_rose_contents = new QVBoxLayout();
                this->box_rose_contents->setContentsMargins(0, 30, 0, 0);
                this->box_rose_contents->setAlignment(Qt::AlignTop);

                this->widget_rose_contents = new QWidget();
                this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_rose_contents->setLayout(this->box_rose_contents);
                this->widget_rose_contents->hide();

                this->widget_login_contents = this->setUIControl_LoginBefore();
                this->widget_login_contents->hide();

                this->stackedWidget_Contents = new QStackedWidget();
                this->stackedWidget_Contents->addWidget(this->widget_rose_contents);
                this->stackedWidget_Contents->addWidget(this->widget_login_contents);

                this->box_contents->addWidget(this->stackedWidget_Contents);

                if(global.user.getAccess_token().isEmpty()){

                    if(global.enable_section_left == true){
                        global.enable_section_left = false;
                    }

                    this->widget_rose_contents->hide();
                    this->widget_login_contents->show();
                    this->stackedWidget_Contents->setCurrentIndex(1);
                }
                else{

                    global.enable_message_count = 0;
                    global.enable_message_flag = true;
                    print_debug();
                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//c230303
/*//c230303
                    if(!global.window_activate_flag){//c230130
                        print_debug();
                        global.window_activate_flag = true;
                        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                        global.window_activate_flag = false;
                    }else{
                        print_debug();
                        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                    }
*/

                    // init data
                    this->list_Recommends->clear();
                    this->list_RecPlaylist->clear();
                    this->list_RecentAlbum->clear();
                    this->list_RecentPlaylist->clear();
                    this->list_RecentTrack->clear();
                    this->list_RecentArtist->clear();
                    this->list_Historylist->clear();
                    this->list_MyPlaylist->clear();
                    this->list_NewPlaylist->clear();
                    this->list_PopPlaylist->clear();
                    this->list_DuduoPlaylist->clear();

                    flag_recommends[0] = false;
                    flag_recentAlbum[0] = false;
                    flag_recentPlaylist[0] = false;
                    flag_recentRosetube[0] = false;
                    flag_recentTrack[0] = false;
                    flag_recentArtist[0] = false;
                    flag_historylist[0] = false;
                    flag_myPlaylist[0] = false;
                    flag_newPlaylist[0] = false;
                    flag_popPlaylist[0] = false;
                    flag_duduoPlaylist[0] = false;

                    flag_recommends[1] = false;
                    flag_recentAlbum[1] = false;
                    flag_recentPlaylist[1] = false;
                    flag_recentRosetube[1] = false;
                    flag_recentTrack[1] = false;
                    flag_recentArtist[1] = false;
                    flag_historylist[1] = false;
                    flag_myPlaylist[1] = false;
                    flag_newPlaylist[1] = false;
                    flag_popPlaylist[1] = false;
                    flag_duduoPlaylist[1] = false;

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
                    this->vBox_recommends->setContentsMargins(0, 0, 0, 0);
                    this->vBox_recentlyPlay = new QVBoxLayout();
                    this->vBox_recentlyPlay->setContentsMargins(0, 0, 0, 0);
                    this->vBox_recentlyRoseTube = new QVBoxLayout();
                    this->vBox_recentlyRoseTube->setContentsMargins(0, 0, 0, 0);
                    this->vBox_recentlyTrack = new QVBoxLayout();
                    this->vBox_recentlyTrack->setContentsMargins(0, 0, 0, 0);
                    this->vBox_recentlyArtist = new QVBoxLayout();
                    this->vBox_recentlyArtist->setContentsMargins(0, 0, 0, 0);
                    this->vBox_historylist = new QVBoxLayout();
                    this->vBox_historylist->setContentsMargins(0, 0, 0, 0);
                    this->vBox_myPlaylist = new QVBoxLayout();
                    this->vBox_myPlaylist->setContentsMargins(0, 0, 0, 0);
                    this->vBox_newPlaylist = new QVBoxLayout();
                    this->vBox_newPlaylist->setContentsMargins(0, 0, 0, 0);
                    this->vBox_popPlaylist = new QVBoxLayout();
                    this->vBox_popPlaylist->setContentsMargins(0, 0, 0, 0);
                    this->vBox_duduoPlaylist = new QVBoxLayout();
                    this->vBox_duduoPlaylist->setContentsMargins(0, 0, 0, 0);

                    this->hBox_recommends = new QHBoxLayout();
                    this->vBox_recommends->setContentsMargins(0, 0, 0, 0);
                    this->hBox_recentlyAlbum = new QHBoxLayout();
                    this->hBox_recentlyAlbum->setContentsMargins(0, 0, 0, 0);
                    this->hBox_recentlyPlaylist = new QHBoxLayout();
                    this->hBox_recentlyPlaylist->setContentsMargins(0, 0, 0, 0);
                    this->hBox_recentlyRoseTube = new QHBoxLayout();
                    this->hBox_recentlyRoseTube->setContentsMargins(0, 0, 0, 0);
                    this->hBox_recentlyArtist = new QHBoxLayout();
                    this->hBox_recentlyArtist->setContentsMargins(0, 0, 0, 0);
                    this->hBox_historylist = new QHBoxLayout();
                    this->hBox_historylist->setContentsMargins(0, 0, 0, 0);
                    this->hBox_myPlaylist = new QHBoxLayout();
                    this->hBox_myPlaylist->setContentsMargins(0, 0, 0, 0);
                    this->hBox_newPlaylist = new QHBoxLayout();
                    this->hBox_newPlaylist->setContentsMargins(0, 0, 0, 0);
                    this->hBox_popPlaylist = new QHBoxLayout();
                    this->hBox_popPlaylist->setContentsMargins(0, 0, 0, 0);
                    this->hBox_duduoPlaylist = new QHBoxLayout();
                    this->hBox_duduoPlaylist->setContentsMargins(0, 0, 0, 0);

                    GSCommon::clearLayout(this->vBox_recommends);
                    GSCommon::clearLayout(this->vBox_recentlyPlay);
                    GSCommon::clearLayout(this->vBox_recentlyRoseTube);
                    GSCommon::clearLayout(this->vBox_recentlyTrack);
                    GSCommon::clearLayout(this->vBox_recentlyArtist);
                    GSCommon::clearLayout(this->vBox_historylist);
                    GSCommon::clearLayout(this->vBox_myPlaylist);
                    GSCommon::clearLayout(this->vBox_newPlaylist);
                    GSCommon::clearLayout(this->vBox_popPlaylist);
                    GSCommon::clearLayout(this->vBox_duduoPlaylist);

                    GSCommon::clearLayout(this->hBox_recommends);
                    GSCommon::clearLayout(this->hBox_recentlyAlbum);
                    GSCommon::clearLayout(this->hBox_recentlyPlaylist);
                    GSCommon::clearLayout(this->hBox_recentlyRoseTube);
                    GSCommon::clearLayout(this->hBox_recentlyArtist);
                    GSCommon::clearLayout(this->hBox_historylist);
                    GSCommon::clearLayout(this->hBox_myPlaylist);
                    GSCommon::clearLayout(this->hBox_newPlaylist);
                    GSCommon::clearLayout(this->hBox_popPlaylist);
                    GSCommon::clearLayout(this->hBox_duduoPlaylist);

                    // sub Title UI
                    for(int i = 0; i < 10; i++){
                        this->lb_subTitle[i] = new QLabel();
                        this->btnView_all[i] = new QPushButton();
                    }

                    for(int i = 0; i < 15; i++){
                        this->home_recommends[i] = new roseHome::ItemRecommends_rosehome(i, SECTION_FOR_MORE_POPUP___Recommends, tidal::AbstractItem::ImageSizeMode::Square_284x284);
                    }

                    for(int i = 0; i < 15; i++){
                        this->home_recently_album[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___RecentlyAlbum, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                    }

                    for(int i = 0; i < 15; i++){
                        this->home_recently_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___RecentlyPlaylist, tidal::AbstractItem::ImageSizeMode::Square_200x200, HTTP_RECENTLY_PLAYLIST, true);
                    }

                    for(int i = 0; i < 15; i++){
                        this->home_recently_rosetube[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___RecentlyRosetube, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                    }

                    for (int i = 0; i < 5; i++) {
                        this->home_recently_track[i] = new PlaylistTrackDetailInfo_RHV();
                        this->home_recently_track[i]->setProperty("index", i);
                        this->home_recently_track[i]->setObjectName("recently track");
                    }

                    for(int i = 0; i < 15; i++){
                        this->home_recently_artist[i] = new roseHome::ItemArtist_rosehome(i, SECTION_FOR_MORE_POPUP___RecentlyArtist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    }

                    for(int i = 0; i < 15; i++){
                        this->home_historylist[i] = new roseHome::ItemHistory_rosehome(i, SECTION_FOR_MORE_POPUP___History, tidal::AbstractItem::ImageSizeMode::Square_200x200, false);
                    }

                    for(int i = 0; i < 15; i++){
                        this->home_myPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___MyPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, HTTP_MY_PLAYLIST, true);
                    }

                    for(int i = 0; i < 15; i++){
                        this->home_newPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___NewPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, HTTP_NEW_PLAYLIST, true);
                    }

                    for(int i = 0; i < 15; i++){
                        this->home_popPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___PopPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, HTTP_POP_PLAYLIST, true);
                    }

                    for(int i = 0; i < 15; i++){
                        this->home_duduoPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___DuduoPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, HTTP_DUDUO_PLAYLIST, true);
                    }

                    this->setUIControl_requestRose();

                    this->widget_rose_contents->show();
                    this->widget_login_contents->hide();
                    this->stackedWidget_Contents->setCurrentIndex(0);

                    this->flag_page_draw = true;
                }

                print_debug();ContentLoadingwaitingMsgHide();//c230322_3
            }
            else{
                print_debug();
                this->flagNeedReload = true;

                this->box_contents->removeWidget(this->stackedWidget_Contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                this->box_rose_contents = new QVBoxLayout();
                this->box_rose_contents->setContentsMargins(0, 15, 0, 0);
                this->box_rose_contents->setAlignment(Qt::AlignTop);

                this->widget_rose_contents = new QWidget();
                this->widget_rose_contents->setStyleSheet("background:#212121; border:none;");
                this->widget_rose_contents->setLayout(this->box_rose_contents);

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//c230303
/*
                if(!global.window_activate_flag){//c230127
                    global.window_activate_flag = true;
                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                    global.window_activate_flag = false;
                }
                else{
                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                }
*/
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

                this->flag_page_draw = true;
            }
            print_debug();ContentLoadingwaitingMsgHide();//c230322_3
        }
        else{
            this->flag_recentAlbum_check[0] = false;
            this->flag_recentPlaylist_check[0] = false;
            this->flag_recentRosetube_check[0] = false;
            this->flag_recentTrack_check[0] = false;
            this->flag_recentArtist_check[0] = false;
            this->flag_myPlaylist_check[0] = false;

            this->flag_recentAlbum_check[1] = false;
            this->flag_recentPlaylist_check[1] = false;
            this->flag_recentRosetube_check[1] = false;
            this->flag_recentTrack_check[1] = false;
            this->flag_recentArtist_check[1] = false;
            this->flag_myPlaylist_check[1] = false;

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

            global.enable_message_flag = true;
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//c230303
/*//c230303
            if(!global.window_activate_flag){//c230130
                print_debug();
                global.window_activate_flag = true;
                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                global.window_activate_flag = false;
            }
            else{
                print_debug();
                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }
*/

            if(global.user.enable_home_filter[0] == 1){
                this->recom_path = ROSE_API_RECOMM_PATH;
            }
            else{
                this->recom_path = ROSE_API_RECOMM_PATH + QString("?mediaTypes=%1").arg(this->str_media_type);
            }

            roseHome::ProcCommon *proc_recent_album = new roseHome::ProcCommon(this);
            connect(proc_recent_album, &roseHome::ProcCommon::completeReq_list_albums, this, &RoseHome::slot_applyResult_recentlyAlbumCheck);
            proc_recent_album->request_rose_getList_recentlyAlbums(ROSE_API_ALBUM_PATH, this->str_media_type, 0, 15);

            roseHome::ProcCommon *proc_recent_playlist = new roseHome::ProcCommon(this);
            connect(proc_recent_playlist, &roseHome::ProcCommon::completeReq_list_playlists, this, &RoseHome::slot_applyResult_recentlyPlaylistCheck);
            proc_recent_playlist->request_rose_getList_recentlyPlaylists(ROSE_API_PLAYLIST_PATH, this->str_media_type, 0, 15);

            roseHome::ProcCommon *proc_recent_rosetube = new roseHome::ProcCommon(this);
            connect(proc_recent_rosetube, &roseHome::ProcCommon::completeReq_list_rosetubes, this, &RoseHome::slot_applyResult_recentlyRosetubeCheck);
            proc_recent_rosetube->request_rose_getList_recentlyRosetube(ROSE_API_ROSETUBE_PATH, "YOUTUBE", 0, 15);

            roseHome::ProcCommon *proc_recent_track = new roseHome::ProcCommon(this);
            connect(proc_recent_track, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseHome::slot_applyResult_recentlyTrackCheck);

            if(global.user.enable_home_filter[0] == 1){
                proc_recent_track->request_rose_getList_recentlyTracks(ROSE_API_TRACK_PATH, "YOUTUBE", 0, 5);
            }
            else if(this->str_media_type == "YOUTUBE"){
                proc_recent_track->request_rose_getList_recentlyTracks(ROSE_API_TRACK_PATH, "ROSETUBE", 0, 5);
            }
            else{
                proc_recent_track->request_rose_getList_recentlyTracks(ROSE_API_TRACK_PATH, this->str_media_type, 0, 5);
            }

            roseHome::ProcCommon *proc_recent_artist = new roseHome::ProcCommon(this);
            connect(proc_recent_artist, &roseHome::ProcCommon::completeReq_list_artists, this, &RoseHome::slot_applyResult_recentlyArtistCheck);
            proc_recent_artist->request_rose_getList_recentlyArtists(ROSE_API_ARTIST_PATH, this->str_media_type, 0, 15);

            roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
            connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &RoseHome::slot_applyResult_MyPlaylistCheck);
            proc_myPlaylist->request_rose_getList_myPlaylists(ROSE_API_MYPLAYLIST_PATH, this->str_media_type, 0, 15);

            print_debug();ContentLoadingwaitingMsgHide();//c230322_3
        }

        this->widget_login_contents_flag = true;//c230525
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

        print_debug();
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

        const int widget_width = this->width() - 180;

        QWidget *widget_loginBefore = new QWidget();
        widget_loginBefore->setObjectName("widget_loginBefore");
        widget_loginBefore->setFixedSize(widget_width, 500);
        //widget_loginBefore->setFixedHeight(500);
        widget_loginBefore->setStyleSheet("#widget_loginBefore { background:qlineargradient(x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #222222, stop:1 #464646); }");

        int left, top = 0;
        this->lb_loginBefore = new QLabel(widget_loginBefore);
        this->lb_loginBefore->setText(tr("You can use various services through Login."));
        this->lb_loginBefore->setStyleSheet("background-color: transparent; color: #CCCCCC; font-size: 30px;");

        left = (widget_loginBefore->sizeHint().width() - this->lb_loginBefore->sizeHint().width()) / 2;
        top = (500 - 55 - 30 - this->lb_loginBefore->sizeHint().height()) / 2;
        this->lb_loginBefore->setGeometry(left, top, this->lb_loginBefore->sizeHint().width(), this->lb_loginBefore->sizeHint().height());

        this->btn_loginBefore = new QPushButton(widget_loginBefore);
        this->btn_loginBefore->setText(tr("Log-in"));
        this->btn_loginBefore->setCursor(Qt::PointingHandCursor);
        this->btn_loginBefore->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:20px;border:3px solid #CCCCCC;border-radius:25;");
        this->btn_loginBefore->setFixedSize(230, 55);

        left = (widget_loginBefore->sizeHint().width() - 230) / 2;
        this->btn_loginBefore->setGeometry(left, top + this->lb_loginBefore->sizeHint().height() + 30, 0, 0);

        // 커넥션
        connect(this->btn_loginBefore, &QPushButton::clicked, this, &RoseHome::slot_showLoginPage);

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

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//c230303
/*//c230303
        if(!global.window_activate_flag){//c230130
            print_debug();
            global.window_activate_flag = true;
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            global.window_activate_flag = false;
        }else{
            print_debug();
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
        }
*/

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

        roseHome::ProcCommon *proc_recent_artist = new roseHome::ProcCommon(this);
        connect(proc_recent_artist, &roseHome::ProcCommon::completeReq_list_artists, this, &RoseHome::slot_applyResult_recentlyArtist);
        proc_recent_artist->request_rose_getList_recentlyArtists(ROSE_API_ARTIST_PATH, this->str_media_type, 0, 15);

        QDate todayData = QDate::currentDate();
        QString strToday = todayData.toString("yyyyMM");

        roseHome::ProcCommon *proc_historylist = new roseHome::ProcCommon(this);
        connect(proc_historylist, &roseHome::ProcCommon::completeReq_list_history, this, &RoseHome::slot_applyResult_historylist);
        proc_historylist->request_rose_getList_hisotrylist(ROSE_API_HISTORY_PATH, strToday, this->str_media_type);

        roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
        connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &RoseHome::slot_applyResult_MyPlaylist);
        proc_myPlaylist->request_rose_getList_myPlaylists(ROSE_API_MYPLAYLIST_PATH, this->str_media_type, 0, 15);

        roseHome::ProcCommon *proc_newPlaylist = new roseHome::ProcCommon(this);
        connect(proc_newPlaylist, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &RoseHome::slot_applyResult_NewPlaylist);
        proc_newPlaylist->request_rose_getList_usersPlaylists(ROSE_API_NEWPLAYLIST_PATH, "PLAYLIST_RECENT", this->str_media_type, 0, 15);

        roseHome::ProcCommon *proc_popPlaylist = new roseHome::ProcCommon(this);
        connect(proc_popPlaylist, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &RoseHome::slot_applyResult_PopPlaylist);
        proc_popPlaylist->request_rose_getList_usersPlaylists(ROSE_API_POPPLAYLIST_PATH, "PLAYLIST_THUMBUP", this->str_media_type, 0, 15);

        roseHome::ProcCommon *proc_duduoPlaylist = new roseHome::ProcCommon(this);
        connect(proc_duduoPlaylist, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &RoseHome::slot_applyResult_duduoPlaylist);
        proc_duduoPlaylist->request_rose_getList_duduoPlaylist(ROSE_API_DUDUOPLAYLIST_PATH, "", 0, 15);

        print_debug();ContentLoadingwaitingMsgHide();//c230322_3
    }


    void RoseHome::setUIControl_appendWidget_rose(){//c230304

        if(this->flag_recommends[0] == true && this->flag_recentAlbum[0] == true && this->flag_recentPlaylist[0] == true && this->flag_recentRosetube[0] == true && this->flag_recentTrack[0] == true &&
                this->flag_recentArtist[0] == true && this->flag_historylist[0] == true && this->flag_myPlaylist[0] == true && this->flag_newPlaylist[0] == true && this->flag_popPlaylist[0] == true &&
                this->flag_duduoPlaylist[0] == true){

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//c230303

            int maxCount = 0;

            if(this->flag_recommends[0] == true){
                this->flag_recommends[0] = false;

                if(this->flag_recommends[1] == true){
                    this->flag_recommends[1] = false;

                    this->widget_recommends = new QWidget();
                    QString subTitle = tr("Rose Recommend Playlist");
                    if(this->list_Recommends->size() > 0){
                        this->widget_recommends = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->list_Recommends->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_Recommends, this->vBox_recommends);
                    }
                    else{
                        this->widget_recommends = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_Recommends, this->vBox_recommends);
                    }

                    this->vBox_recommends->addSpacing(10);

                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->hBox_recommends->setSpacing(0);
                    this->hBox_recommends->setContentsMargins(0, 0, 0, 0);
                    this->hBox_recommends->setAlignment(Qt::AlignTop);
                    this->hBox_recommends->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_recommends);
                    widget_content->setContentsMargins(0, 0, 0, 0);

                    QScrollArea *recommeds_scrollArea = new QScrollArea();
                    recommeds_scrollArea->setWidget(widget_content);
                    recommeds_scrollArea->setWidgetResizable(false);
                    recommeds_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    recommeds_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    recommeds_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                    recommeds_scrollArea->setContentsMargins(0, 0, 0, 0);
                    recommeds_scrollArea->setFixedHeight(410);

                    QScroller::grabGesture(recommeds_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_recommends->addWidget(recommeds_scrollArea);

                    this->flag_recommends[1] = false;

                    if(this->list_Recommends->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_Recommends->size();
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_recommends[i]->setData(this->list_Recommends->at(i));
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_recommends[i], &roseHome::ItemRecommends_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);
                        this->hBox_recommends->addWidget(this->home_recommends[i]);
                    }

                    //
                    QWidget *widget_tmp = new QWidget();
                    widget_tmp->setContentsMargins(0, 0, 0, 0);
                    widget_tmp->setLayout(this->vBox_recommends);

                    this->box_rose_contents->addWidget(widget_tmp, 0, Qt::AlignTop);
                    this->box_rose_contents->addSpacing(30);
                }
            }

            if(this->flag_recentAlbum[0] == true){
                this->flag_recentAlbum[0] = false;

                this->widget_recentlyPlay = this->setUIControl_subTitle_withSubMenu(tr("Recently played"), "View All", BTN_IDX_SUBTITLE_RecentlyPlay, this->vBox_recentlyPlay);

                this->vBox_recentlyPlay->addSpacing(0);
                this->vBox_recentlyPlay->addWidget(this->stackRecent);

                //----------------------------------------------------------------------------------------------------  BODY : START
                this->hBox_recentlyAlbum->setSpacing(0);
                this->hBox_recentlyAlbum->setContentsMargins(0, 0, 0, 0);
                this->hBox_recentlyAlbum->setAlignment(Qt::AlignTop);
                this->hBox_recentlyAlbum->setSizeConstraint(QLayout::SetFixedSize);

                QWidget *widget_content = new QWidget;
                widget_content->setLayout(this->hBox_recentlyAlbum);
                widget_content->setContentsMargins(0, 0, 0, 0);

                this->album_scrollArea = new QScrollArea();
                this->album_scrollArea->setWidget(widget_content);
                this->album_scrollArea->setWidgetResizable(false);
                this->album_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                this->album_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                this->album_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                this->album_scrollArea->setContentsMargins(0, 0, 0, 0);
                this->album_scrollArea->setFixedHeight(311);

                QScroller::grabGesture(this->album_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->stackRecent->insertWidget(0, this->album_scrollArea);

                if(this->flag_recentAlbum[1] == true){
                    this->flag_recentAlbum[1] = false;

                    if(this->list_RecentAlbum->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_RecentAlbum->size();
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_recently_album[i]->setData(this->list_RecentAlbum->at(i));
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_recently_album[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemAlbum);
                        this->hBox_recentlyAlbum->addWidget(this->home_recently_album[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                    noData_widget->setFixedSize(1500, 311);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_recentlyAlbum->addWidget(noData_widget);
                }

                //
                QWidget *widget_tmp = new QWidget();
                widget_tmp->setContentsMargins(0, 0, 0, 0);
                widget_tmp->setLayout(this->vBox_recentlyPlay);

                this->box_rose_contents->addWidget(widget_tmp, 0, Qt::AlignTop);
                this->box_rose_contents->addSpacing(30);
            }

            if(this->flag_recentPlaylist[0] == true){
                this->flag_recentPlaylist[0] = false;

                //----------------------------------------------------------------------------------------------------  BODY : START
                this->hBox_recentlyPlaylist->setSpacing(0);
                this->hBox_recentlyPlaylist->setContentsMargins(0, 0, 0, 0);
                this->hBox_recentlyPlaylist->setAlignment(Qt::AlignTop);
                this->hBox_recentlyPlaylist->setSizeConstraint(QLayout::SetFixedSize);

                QWidget *widget_content = new QWidget;
                widget_content->setLayout(this->hBox_recentlyPlaylist);
                widget_content->setContentsMargins(0, 0, 0, 0);

                this->playlist_scrollArea = new QScrollArea();
                this->playlist_scrollArea->setWidget(widget_content);
                this->playlist_scrollArea->setWidgetResizable(false);
                this->playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                this->playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                this->playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                this->playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                this->playlist_scrollArea->setFixedHeight(311);

                QScroller::grabGesture(this->playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->stackRecent->insertWidget(1, this->playlist_scrollArea);

                if(this->flag_recentPlaylist[1] == true){
                    if(this->list_RecentPlaylist->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_RecentPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_recently_playlist[i]->setData(this->list_RecentPlaylist->at(i));
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_recently_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);
                        this->hBox_recentlyPlaylist->addWidget(this->home_recently_playlist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 311);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_recentlyPlaylist->addWidget(noData_widget);
                }

                this->contentStep = ALBTAB_STEP_ALBUM;
                this->menubar->setSelectedSubMenuNoSignal(this->contentStep);
            }

            if(this->flag_recentRosetube[0] == true){
                this->flag_recentRosetube[0] = false;

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

                QScrollArea *rosetube_scrollArea = new QScrollArea();
                rosetube_scrollArea->setWidget(widget_content);
                rosetube_scrollArea->setWidgetResizable(false);
                rosetube_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                rosetube_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                rosetube_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                rosetube_scrollArea->setContentsMargins(0, 0, 0, 0);
                rosetube_scrollArea->setFixedHeight(308);

                QScroller::grabGesture(rosetube_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_recentlyRoseTube->addWidget(rosetube_scrollArea);

                if(this->flag_recentRosetube[1] == true){
                    this->flag_recentRosetube[1] = false;

                    if(this->jsonArr_rosetube.count() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->jsonArr_rosetube.count();
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_recently_rosetube[i]->setData(this->jsonArr_rosetube.at(i).toObject());
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_recently_rosetube[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);
                        this->hBox_recentlyRoseTube->addWidget(this->home_recently_rosetube[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                    noData_widget->setFixedSize(1500, 308);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_recentlyRoseTube->addWidget(noData_widget);
                }

                //
                QWidget *widget_tmp = new QWidget();
                widget_tmp->setContentsMargins(0, 0, 0, 0);
                widget_tmp->setLayout(this->vBox_recentlyRoseTube);

                this->box_rose_contents->addWidget(widget_tmp, 0, Qt::AlignTop);
                this->box_rose_contents->addSpacing(35);
            }

            if(this->flag_recentTrack[0] == true){
                this->flag_recentTrack[0] = false;

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

                this->vBox_recentlyTrack_info = new QVBoxLayout();
                this->vBox_recentlyTrack_info->setSpacing(0);
                this->vBox_recentlyTrack_info->setContentsMargins(40, 0, 54, 0);
                this->vBox_recentlyTrack_info->setAlignment(Qt::AlignTop);

                QWidget *widget_recentlyTrack_info = new QWidget();
                widget_recentlyTrack_info->setStyleSheet("background-color:#0d0d0d; border:0px;");
                widget_recentlyTrack_info->setContentsMargins(0, 0, 0, 0);
                widget_recentlyTrack_info->setFixedHeight(411);
                widget_recentlyTrack_info->setLayout(this->vBox_recentlyTrack_info);

                this->vBox_recentlyTrack->addWidget(widget_recentlyTrack_info);

                if(this->flag_recentTrack[1] == true){
                    this->flag_recentTrack[1] = false;

                    if(this->list_RecentTrack->size() > 5){
                        maxCount = 5;
                    }
                    else{
                        maxCount = this->list_RecentTrack->size();
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_recently_track[i]->setDataTrackInfo_RoseMain(this->list_RecentTrack->at(i));
                        this->home_recently_track[i]->setFavoritesIds(this->list_RecentTrack->at(i).favorite, this->list_RecentTrack->at(i).star);
                        this->home_recently_track[i]->resize(1550, 70);
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_recently_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &RoseHome::slot_clickedItemTrack_inList);
                        this->vBox_recentlyTrack_info->addWidget(this->home_recently_track[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
                    noData_widget->setFixedSize(1500, 350);
                    noData_widget->setObjectName("Nodata");

                    this->vBox_recentlyTrack_info->addWidget(noData_widget);
                }

                //
                QWidget *widget_tmp = new QWidget();
                widget_tmp->setStyleSheet("background-color:#0d0d0d; border:0px;");
                widget_tmp->setContentsMargins(0, 0, 0, 0);
                widget_tmp->setLayout(this->vBox_recentlyTrack);

                this->box_rose_contents->addWidget(widget_tmp, 0, Qt::AlignTop);
                this->box_rose_contents->addSpacing(30);
            }

            if(this->flag_recentArtist[0] == true){
                this->flag_recentArtist[0] = false;

                this->widget_recentlyArtist = new QWidget();
                QString subTitle = tr("Recently Played Artists");
                if(this->list_RecentArtist->size() > 0){
                    this->widget_recentlyArtist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->list_RecentArtist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_RecentlyArtist, this->vBox_recentlyArtist);
                }
                else{
                    this->widget_recentlyArtist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_RecentlyArtist, this->vBox_recentlyArtist);
                }

                this->vBox_recentlyArtist->addSpacing(10);

                //----------------------------------------------------------------------------------------------------  BODY : START
                this->hBox_recentlyArtist->setSpacing(0);
                this->hBox_recentlyArtist->setContentsMargins(0, 0, 0, 0);
                this->hBox_recentlyArtist->setAlignment(Qt::AlignTop);
                this->hBox_recentlyArtist->setSizeConstraint(QLayout::SetFixedSize);

                QWidget *widget_content = new QWidget;
                widget_content->setLayout(this->hBox_recentlyArtist);
                widget_content->setContentsMargins(0, 0, 0, 0);

                QScrollArea *artist_scrollArea = new QScrollArea();
                artist_scrollArea->setWidget(widget_content);
                artist_scrollArea->setWidgetResizable(false);
                artist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                artist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                artist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                artist_scrollArea->setContentsMargins(0, 0, 0, 0);
                artist_scrollArea->setFixedHeight(261);

                QScroller::grabGesture(artist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_recentlyArtist->addWidget(artist_scrollArea);

                if(this->flag_recentArtist[1] == true){
                    this->flag_recentArtist[1] = false;

                    if(this->list_RecentArtist->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_RecentArtist->size();
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_recently_artist[i]->setData(this->list_RecentArtist->at(i));
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_recently_artist[i], &roseHome::ItemArtist_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemArtist);
                        this->hBox_recentlyArtist->addWidget(this->home_recently_artist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Artist_NoData);
                    noData_widget->setFixedSize(1500, 261);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_recentlyArtist->addWidget(noData_widget);
                }

                //
                QWidget *widget_tmp = new QWidget();
                widget_tmp->setContentsMargins(0, 0, 0, 0);
                widget_tmp->setLayout(this->vBox_recentlyArtist);

                this->box_rose_contents->addWidget(widget_tmp, 0, Qt::AlignTop);
                this->box_rose_contents->addSpacing(30);
            }

            if(this->flag_historylist[0] == true){
                this->flag_historylist[0] = false;

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
                    this->flag_historylist[1] = false;

                    if(this->list_Historylist->size() > 13){
                        maxCount = 13;
                    }
                    else{
                        maxCount = this->list_Historylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_historylist[i]->setData(this->list_Historylist->at(i));
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_historylist[i], &roseHome::ItemHistory_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemAlbum);
                        this->hBox_historylist->addWidget(this->home_historylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                    noData_widget->setFixedSize(1500, 234);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_historylist->addWidget(noData_widget);
                }

                //
                QWidget *widget_tmp = new QWidget();
                widget_tmp->setContentsMargins(0, 0, 0, 0);
                widget_tmp->setLayout(this->vBox_historylist);

                this->box_rose_contents->addWidget(widget_tmp, 0, Qt::AlignTop);
                this->box_rose_contents->addSpacing(30);
            }

            if(this->flag_myPlaylist[0] == true){
                this->flag_myPlaylist[0] = false;

                this->widget_myPlaylist = new QWidget();
                QString subTitle = tr("My playlists");
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

                QScrollArea *myPlaylist_scrollArea = new QScrollArea();
                myPlaylist_scrollArea->setWidget(widget_content);
                myPlaylist_scrollArea->setWidgetResizable(false);
                myPlaylist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                myPlaylist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                myPlaylist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                myPlaylist_scrollArea->setContentsMargins(0, 0, 0, 0);
                myPlaylist_scrollArea->setFixedHeight(311);

                QScroller::grabGesture(myPlaylist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_myPlaylist->addWidget(myPlaylist_scrollArea);

                if(this->flag_myPlaylist[1] == true){
                    this->flag_myPlaylist[1] = false;

                    if(this->list_MyPlaylist->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_MyPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_myPlaylist[i]->setData(this->list_MyPlaylist->at(i));
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_myPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);
                        this->hBox_myPlaylist->addWidget(this->home_myPlaylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 311);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_myPlaylist->addWidget(noData_widget);
                }

                //
                QWidget *widget_tmp = new QWidget();
                widget_tmp->setContentsMargins(0, 0, 0, 0);
                widget_tmp->setLayout(this->vBox_myPlaylist);

                this->box_rose_contents->addWidget(widget_tmp, 0, Qt::AlignTop);
                this->box_rose_contents->addSpacing(30);
            }

            if(this->flag_newPlaylist[0] == true){
                this->flag_newPlaylist[0] = false;

                this->widget_newPlaylist = new QWidget();
                QString subTitle = tr("New playlists");
                if(this->list_NewPlaylist->size() > 0){
                    this->widget_newPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->list_NewPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_NewPlaylists, this->vBox_newPlaylist);
                }
                else{
                    this->widget_newPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_NewPlaylists, this->vBox_newPlaylist);
                }

                this->vBox_newPlaylist->addSpacing(10);

                //----------------------------------------------------------------------------------------------------  BODY : START
                this->hBox_newPlaylist->setSpacing(0);
                this->hBox_newPlaylist->setContentsMargins(0, 0, 0, 0);
                this->hBox_newPlaylist->setAlignment(Qt::AlignTop);
                this->hBox_newPlaylist->setSizeConstraint(QLayout::SetFixedSize);

                QWidget *widget_content = new QWidget;
                widget_content->setLayout(this->hBox_newPlaylist);
                widget_content->setContentsMargins(0, 0, 0, 0);

                QScrollArea *newPlaylist_scrollArea = new QScrollArea();
                newPlaylist_scrollArea->setWidget(widget_content);
                newPlaylist_scrollArea->setWidgetResizable(false);
                newPlaylist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                newPlaylist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                newPlaylist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                newPlaylist_scrollArea->setContentsMargins(0, 0, 0, 0);
                newPlaylist_scrollArea->setFixedHeight(338);

                QScroller::grabGesture(newPlaylist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_newPlaylist->addWidget(newPlaylist_scrollArea);

                if(this->flag_newPlaylist[1] == true){
                    this->flag_newPlaylist[1] = false;

                    if(this->list_NewPlaylist->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_NewPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_newPlaylist[i]->setData(this->list_NewPlaylist->at(i));
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_newPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);
                        this->hBox_newPlaylist->addWidget(this->home_newPlaylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 338);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_newPlaylist->addWidget(noData_widget);
                }

                //
                QWidget *widget_tmp = new QWidget();
                widget_tmp->setContentsMargins(0, 0, 0, 0);
                widget_tmp->setLayout(this->vBox_newPlaylist);

                this->box_rose_contents->addWidget(widget_tmp, 0, Qt::AlignTop);
                this->box_rose_contents->addSpacing(30);
            }

            if(this->flag_popPlaylist[0] == true){
                this->flag_popPlaylist[0] = false;

                this->widget_popPlaylist = new QWidget();
                QString subTitle = tr("Popular playlists");
                if(this->list_PopPlaylist->size() > 0){
                    this->widget_popPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->list_PopPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_PopPlaylists, this->vBox_popPlaylist);
                }
                else{
                    this->widget_popPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_PopPlaylists, this->vBox_popPlaylist);
                }

                this->vBox_popPlaylist->addSpacing(10);

                //----------------------------------------------------------------------------------------------------  BODY : START
                this->hBox_popPlaylist->setSpacing(0);
                this->hBox_popPlaylist->setContentsMargins(0, 0, 0, 0);
                this->hBox_popPlaylist->setAlignment(Qt::AlignTop);
                this->hBox_popPlaylist->setSizeConstraint(QLayout::SetFixedSize);

                QWidget *widget_content = new QWidget;
                widget_content->setLayout(this->hBox_popPlaylist);
                widget_content->setContentsMargins(0, 0, 0, 0);

                QScrollArea *popPlaylist_scrollArea = new QScrollArea();
                popPlaylist_scrollArea->setWidget(widget_content);
                popPlaylist_scrollArea->setWidgetResizable(false);
                popPlaylist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                popPlaylist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                popPlaylist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                popPlaylist_scrollArea->setContentsMargins(0, 0, 0, 0);
                popPlaylist_scrollArea->setFixedHeight(338);

                QScroller::grabGesture(popPlaylist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_popPlaylist->addWidget(popPlaylist_scrollArea);

                if(this->flag_popPlaylist[1] == true){
                    this->flag_popPlaylist[1] = false;

                    if(this->list_PopPlaylist->size() > 10){
                        maxCount = 10;
                    }
                    else{
                        maxCount = this->list_PopPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_popPlaylist[i]->setData(this->list_PopPlaylist->at(i));
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_popPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);
                        this->hBox_popPlaylist->addWidget(this->home_popPlaylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 338);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_popPlaylist->addWidget(noData_widget);
                }

                //
                QWidget *widget_tmp = new QWidget();
                widget_tmp->setContentsMargins(0, 0, 0, 0);
                widget_tmp->setLayout(this->vBox_popPlaylist);

                this->box_rose_contents->addWidget(widget_tmp, 0, Qt::AlignTop);
                this->box_rose_contents->addSpacing(30);
            }

            if(this->flag_duduoPlaylist[0] == true){
                this->flag_duduoPlaylist[0] = false;

                this->widget_duduoPlaylist = new QWidget();
                QString subTitle = tr("Duduo References playlist");
                if(this->list_DuduoPlaylist->size() > 0){
                    this->widget_duduoPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->list_DuduoPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_DuduoPlaylists, this->vBox_duduoPlaylist);
                }
                else{
                    this->widget_duduoPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_DuduoPlaylists, this->vBox_duduoPlaylist);
                }

                this->vBox_duduoPlaylist->addSpacing(10);

                //----------------------------------------------------------------------------------------------------  BODY : START
                this->hBox_duduoPlaylist->setSpacing(0);
                this->hBox_duduoPlaylist->setContentsMargins(0, 0, 0, 0);
                this->hBox_duduoPlaylist->setAlignment(Qt::AlignTop);
                this->hBox_duduoPlaylist->setSizeConstraint(QLayout::SetFixedSize);

                QWidget *widget_content = new QWidget;
                widget_content->setLayout(this->hBox_duduoPlaylist);
                widget_content->setContentsMargins(0, 0, 0, 0);

                QScrollArea *duduoPlaylist_scrollArea = new QScrollArea();
                duduoPlaylist_scrollArea->setWidget(widget_content);
                duduoPlaylist_scrollArea->setWidgetResizable(false);
                duduoPlaylist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                duduoPlaylist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                duduoPlaylist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                duduoPlaylist_scrollArea->setContentsMargins(0, 0, 0, 0);
                duduoPlaylist_scrollArea->setFixedHeight(338);

                QScroller::grabGesture(duduoPlaylist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_duduoPlaylist->addWidget(duduoPlaylist_scrollArea);

                if(this->flag_duduoPlaylist[1] == true){
                    this->flag_duduoPlaylist[1] = false;

                    if(this->list_DuduoPlaylist->size() > 10){
                        maxCount = 10;
                    }
                    else{
                        maxCount = this->list_DuduoPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_duduoPlaylist[i]->setData(this->list_DuduoPlaylist->at(i));
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_duduoPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);
                        this->hBox_duduoPlaylist->addWidget(this->home_duduoPlaylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 338);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_duduoPlaylist->addWidget(noData_widget);
                }

                //
                QWidget *widget_tmp = new QWidget();
                widget_tmp->setContentsMargins(0, 0, 0, 0);
                widget_tmp->setLayout(this->vBox_duduoPlaylist);

                this->box_rose_contents->addWidget(widget_tmp, 0, Qt::AlignTop);
                this->box_rose_contents->addSpacing(30);
            }

            this->slot_hide_msg();
            ContentLoadingwaitingMsgHide();
            global.isDrawingMainContent = true;
        }
    }


    void RoseHome::setUIControl_checkWidget_rose(){

        if(this->flag_recentAlbum_check[0] == true && this->flag_recentPlaylist_check[0] == true && this->flag_recentRosetube_check[0] == true
                && this->flag_recentTrack_check[0] == true && this->flag_recentArtist_check[0] == true && this->flag_myPlaylist_check[0] == true){

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//c230303

            int maxCount = 0;
            if(this->flag_recentAlbum_check[1] == true){
                this->flag_recentAlbum_check[0] = false;
                this->flag_recentAlbum_check[1] = false;

                QWidget *tmpWidget = this->hBox_recentlyAlbum->itemAt(0)->widget();
                //qDebug() << this->hBox_recentlyAlbum->count() << tmpWidget->objectName();

                if(tmpWidget->objectName() != "NoData"){
                    for(int i = 0; i < this->hBox_recentlyAlbum->count(); i++){
                        this->home_recently_album[i]->hide();
                        this->home_recently_album[i]->disconnect();
                        this->hBox_recentlyAlbum->removeWidget(this->home_recently_album[i]);
                    }
                }
                GSCommon::clearLayout(this->hBox_recentlyAlbum);

                maxCount = 0;
                if(this->list_RecentAlbum->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_RecentAlbum->size();
                }

                if(maxCount > 0){
                    for(int i = 0; i < maxCount; i++){//c230213
                        this->home_recently_album[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___RecentlyAlbum, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                        QCoreApplication::processEvents();//c230213
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_recently_album[i]->setData(this->list_RecentAlbum->at(i));
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_recently_album[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemAlbum);//c230213
                        this->hBox_recentlyAlbum->addWidget(this->home_recently_album[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                    noData_widget->setFixedSize(1500, 311);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_recentlyAlbum->addWidget(noData_widget);
                }

                this->changedOnlyTabUI_notSendSignal(this->contentStep);
            }

            if(this->flag_recentPlaylist_check[1] == true){
                this->flag_recentPlaylist_check[0] = false;
                this->flag_recentPlaylist_check[1] = false;

                QWidget *tmpWidget = this->hBox_recentlyPlaylist->itemAt(0)->widget();
                //qDebug() << this->hBox_recentlyPlaylist->count() << tmpWidget->objectName();

                if(tmpWidget->objectName() != "NoData"){
                    for(int i = 0; i < this->hBox_recentlyPlaylist->count(); i++){
                        this->home_recently_playlist[i]->hide();
                        this->home_recently_playlist[i]->disconnect();
                        this->hBox_recentlyPlaylist->removeWidget(this->home_recently_playlist[i]);
                    }
                }
                GSCommon::clearLayout(this->hBox_recentlyPlaylist);

                maxCount = 0;
                if(this->list_RecentPlaylist->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_RecentPlaylist->size();
                }

                if(maxCount > 0){
                    for(int i = 0; i < maxCount; i++){//c230213
                        this->home_recently_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___RecentlyPlaylist, tidal::AbstractItem::ImageSizeMode::Square_200x200, HTTP_RECENTLY_PLAYLIST, true);
                        QCoreApplication::processEvents();//c230213
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_recently_playlist[i]->setData(this->list_RecentPlaylist->at(i));
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_recently_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);//c230213
                        this->hBox_recentlyPlaylist->addWidget(this->home_recently_playlist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 311);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_recentlyPlaylist->addWidget(noData_widget);
                }

                this->changedOnlyTabUI_notSendSignal(this->contentStep);
            }

            if(this->flag_recentRosetube_check[1] == true){
                this->flag_recentRosetube_check[0] = false;
                this->flag_recentRosetube_check[1] = false;

                QWidget *tmpWidget = this->hBox_recentlyRoseTube->itemAt(0)->widget();
                //qDebug() << this->hBox_recentlyPlaylist->count() << tmpWidget->objectName();

                if(tmpWidget->objectName() != "NoData"){
                    for(int i = 0; i < this->hBox_recentlyPlaylist->count(); i++){
                        this->home_recently_rosetube[i]->hide();
                        this->home_recently_rosetube[i]->disconnect();
                        this->hBox_recentlyRoseTube->removeWidget(this->home_recently_rosetube[i]);
                    }
                }
                GSCommon::clearLayout(this->hBox_recentlyRoseTube);

                QString subTitle = tr("Recently Played Rosetube");
                if(this->list_MyPlaylist->size() > 0){
                    this->lb_subTitle[BTN_IDX_SUBTITLE_RecentlyRosetube]->setText(subTitle + QString(" (%1)").arg(this->recentRoseTube_cnt));
                }
                else{
                    this->lb_subTitle[BTN_IDX_SUBTITLE_RecentlyRosetube]->setText(subTitle + QString(" (0)"));
                }

                maxCount = 0;
                if(this->jsonArr_rosetube.count() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->jsonArr_rosetube.count();
                }

                if(maxCount > 0){
                    for(int i = 0; i < maxCount; i++){//c230213
                        this->home_recently_rosetube[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___RecentlyRosetube, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                        QCoreApplication::processEvents();//c230213
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_recently_rosetube[i]->setData(this->jsonArr_rosetube.at(i).toObject());
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_recently_rosetube[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);
                        this->hBox_recentlyRoseTube->addWidget(this->home_recently_rosetube[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                    noData_widget->setFixedSize(1500, 308);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_recentlyRoseTube->addWidget(noData_widget);
                }
            }

            if(this->flag_recentTrack_check[1] == true){
                this->flag_recentTrack_check[0] = false;
                this->flag_recentTrack_check[1] = false;

                QWidget *tmpWidget = this->vBox_recentlyTrack_info->itemAt(0)->widget();
                //qDebug() << this->vBox_recentlyTrack_info->count() << tmpWidget->objectName();

                if(tmpWidget->objectName() != "NoData"){
                    for(int i = 0; i < this->vBox_recentlyTrack_info->count(); i++){
                        this->home_recently_track[i]->hide();
                        this->home_recently_track[i]->disconnect();
                        this->vBox_recentlyTrack_info->removeWidget(this->home_recently_track[i]);
                    }
                }
                GSCommon::clearLayout(this->vBox_recentlyTrack_info);

                QString subTitle = tr("Recently played tracks");
                if(this->list_RecentTrack->size() > 0){
                    this->lb_subTitle[BTN_IDX_SUBTITLE_RecentlyTrack]->setText(subTitle + QString(" (%1)").arg(this->list_RecentTrack->at(0).totalCount));
                }
                else{
                    this->lb_subTitle[BTN_IDX_SUBTITLE_RecentlyTrack]->setText(subTitle + QString(" (0)"));
                }

                maxCount = 0;
                if(this->list_RecentTrack->size() > 5){
                    maxCount = 5;
                }
                else{
                    maxCount = this->list_RecentTrack->size();
                }

                if(maxCount > 0){
                    for (int i = 0; i < maxCount; i++) {
                        this->home_recently_track[i] = new PlaylistTrackDetailInfo_RHV();
                        this->home_recently_track[i]->setProperty("index", i);
                        this->home_recently_track[i]->setObjectName("recently track");
                        QCoreApplication::processEvents();//c230213
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_recently_track[i]->setDataTrackInfo_RoseMain(this->list_RecentTrack->at(i));
                        this->home_recently_track[i]->setFavoritesIds(this->list_RecentTrack->at(i).favorite, this->list_RecentTrack->at(i).star);
                        this->home_recently_track[i]->resize(1550, 70);
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_recently_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &RoseHome::slot_clickedItemTrack_inList);
                        this->vBox_recentlyTrack_info->addWidget(this->home_recently_track[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
                    noData_widget->setFixedSize(1500, 350);
                    noData_widget->setObjectName("Nodata");

                    this->vBox_recentlyTrack_info->addWidget(noData_widget);
                }
            }

            if(this->flag_recentArtist_check[1] == true){
                this->flag_recentArtist_check[0] = false;
                this->flag_recentArtist_check[1] = false;

                QWidget *tmpWidget = this->hBox_recentlyArtist->itemAt(0)->widget();
                //qDebug() << this->hBox_recentlyArtist->count() << tmpWidget->objectName();

                if(tmpWidget->objectName() != "NoData"){
                    for(int i = 0; i < this->hBox_recentlyArtist->count(); i++){
                        this->home_recently_artist[i]->hide();
                        this->home_recently_artist[i]->disconnect();
                        this->hBox_recentlyArtist->removeWidget(this->home_recently_artist[i]);
                    }
                }
                GSCommon::clearLayout(this->hBox_recentlyArtist);

                QString subTitle = tr("Recently Played Artists");
                if(this->list_RecentArtist->size() > 0){
                    this->lb_subTitle[BTN_IDX_SUBTITLE_RecentlyArtist]->setText(subTitle + QString(" (%1)").arg(this->list_RecentArtist->at(0).totalCount));
                }
                else{
                    this->lb_subTitle[BTN_IDX_SUBTITLE_RecentlyArtist]->setText(subTitle + QString(" (0)"));
                }

                maxCount = 0;
                if(this->list_RecentArtist->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_RecentArtist->size();
                }

                if(maxCount > 0){
                    for(int i = 0; i < maxCount; i++){//c230213
                        this->home_recently_artist[i] = new roseHome::ItemArtist_rosehome(i, SECTION_FOR_MORE_POPUP___RecentlyArtist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                        QCoreApplication::processEvents();//c230213
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_recently_artist[i]->setData(this->list_RecentArtist->at(i));
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_recently_artist[i], &roseHome::ItemArtist_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemArtist);//c230213
                        this->hBox_recentlyArtist->addWidget(this->home_recently_artist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Artist_NoData);
                    noData_widget->setFixedSize(1500, 261);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_recentlyArtist->addWidget(noData_widget);
                }
            }

            if(this->flag_myPlaylist_check[1] == true){
                this->flag_myPlaylist_check[0] = false;
                this->flag_myPlaylist_check[1] = false;

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

                QString subTitle = tr("My playlists");
                if(this->list_MyPlaylist->size() > 0){
                    this->lb_subTitle[BTN_IDX_SUBTITLE_MyPlaylists]->setText(subTitle + QString(" (%1)").arg(this->list_MyPlaylist->at(0).totalCount));
                }
                else{
                    this->lb_subTitle[BTN_IDX_SUBTITLE_MyPlaylists]->setText(subTitle + QString(" (0)"));
                }

                maxCount = 0;
                if(this->list_MyPlaylist->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_MyPlaylist->size();
                }

                if(maxCount > 0){
                    for(int i = 0; i < maxCount; i++){//c230213
                        this->home_myPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___MyPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, HTTP_MY_PLAYLIST, true);
                        QCoreApplication::processEvents();//c230213
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        this->home_myPlaylist[i]->setData(this->list_MyPlaylist->at(i));
                    }

                    for(int i = 0; i < maxCount; i++){//c230210
                        connect(this->home_myPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseHome::slot_clickedItemPlaylist);//c230213
                        this->hBox_myPlaylist->addWidget(this->home_myPlaylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 311);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_myPlaylist->addWidget(noData_widget);
                }
            }

            this->slot_hide_msg();
            ContentLoadingwaitingMsgHide();
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


    void RoseHome::slot_applyResult_recentlyArtist(const QList<roseHome::ArtistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_RecentArtist->append(list_data);
            this->flag_recentArtist[1] = true;
        }

        this->flag_recentArtist[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseHome::slot_applyResult_historylist(const QList<roseHome::HistoryItemData> &list_data, const QJsonArray &jsonArr_dataToPlay){

        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.length() > 0)
        {
            this->list_Historylist->append(list_data);
            this->flag_historylist[1] = true;
        }

        this->flag_historylist[0] = true;
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
    }


    void RoseHome::slot_applyResult_duduoPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_DuduoPlaylist->append(list_data);
            this->flag_duduoPlaylist[1] = true;
        }

        this->flag_duduoPlaylist[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseHome::slot_applyResult_recentlyAlbumCheck(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_RecentAlbum->count() == 0){
                this->list_RecentAlbum->clear();
                this->list_RecentAlbum->append(list_data);

                this->flag_recentAlbum_check[1] = true;
            }
            else{
                int maxCount = (list_data.count() > this->list_RecentAlbum->count()) ? this->list_RecentAlbum->count() : list_data.count();

                for(int i = 0; i < maxCount; i++){
                    if(list_data.at(i).id != this->list_RecentAlbum->at(i).id || list_data.at(i).title != this->list_RecentAlbum->at(i).title || list_data.at(i).star != this->list_RecentAlbum->at(i).star){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_RecentAlbum->at(0).totalCount) || (change_flag > 0)){
                    this->list_RecentAlbum->clear();
                    this->list_RecentAlbum->append(list_data);

                    this->flag_recentAlbum_check[1] = true;
                }
            }
        }
        else if(list_data.size() == 0 && this->list_RecentAlbum->size() != 0){
            this->list_RecentAlbum->clear();

            this->flag_recentAlbum_check[1] = true;
        }

        this->flag_recentAlbum_check[0] = true;
        this->setUIControl_checkWidget_rose();
    }


    void RoseHome::slot_applyResult_recentlyPlaylistCheck(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    if(list_data.at(i).id != this->list_RecentPlaylist->at(i).id || list_data.at(i).title != this->list_RecentPlaylist->at(i).title || list_data.at(i).star != this->list_RecentPlaylist->at(i).star){
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
        else if(list_data.size() == 0 && this->list_RecentPlaylist->size() != 0){
            this->list_RecentPlaylist->clear();

            this->flag_recentPlaylist_check[1] = true;
        }

        this->flag_recentPlaylist_check[0] = true;
        this->setUIControl_checkWidget_rose();
    }


    void RoseHome::slot_applyResult_recentlyRosetubeCheck(const QJsonArray &jsonArr, const int &totalCnt, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(jsonArr.size() > 0){
            if(this->jsonArr_rosetube.count() == 0){
                this->jsonArr_rosetube = jsonArr;
                this->recentRoseTube_cnt = totalCnt;

                this->flag_recentRosetube_check[1] = true;
            }
            else{
                int maxCount = (jsonArr.count() > this->jsonArr_rosetube.count()) ? this->jsonArr_rosetube.count() : jsonArr.count();

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
        else if(jsonArr.size() == 0 && this->jsonArr_rosetube.size() != 0){
            this->jsonArr_rosetube = QJsonArray();
            this->recentRoseTube_cnt = totalCnt;

            this->flag_recentRosetube_check[1] = true;
        }

        this->flag_recentRosetube_check[0] = true;
        this->setUIControl_checkWidget_rose();
    }


    void RoseHome::slot_applyResult_recentlyTrackCheck(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_RecentTrack->count() == 0){
                this->list_RecentTrack->clear();
                this->jsonArr_tracks_toPlay = QJsonArray();

                this->list_RecentTrack->append(list_data);
                this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;

                this->flag_recentTrack_check[1] = true;
            }
            else{
                int maxCount = (list_data.count() > this->list_RecentTrack->count()) ? this->list_RecentTrack->count() : list_data.count();

                for(int i = 0; i < maxCount; i++){
                    if(list_data.at(i).id != this->list_RecentTrack->at(i).id || list_data.at(i).title != this->list_RecentTrack->at(i).title || list_data.at(i).star != this->list_RecentTrack->at(i).star){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_RecentTrack->at(0).totalCount) || (change_flag > 0)){
                    this->list_RecentTrack->clear();
                    this->jsonArr_tracks_toPlay = QJsonArray();

                    this->list_RecentTrack->append(list_data);
                    this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;

                    this->flag_recentTrack_check[1] = true;
                }
            }
        }
        else if(list_data.size() == 0 && this->list_RecentTrack->size() != 0){
            this->list_RecentTrack->clear();
            this->jsonArr_tracks_toPlay = QJsonArray();

            this->flag_recentTrack_check[1] = true;
        }

        this->flag_recentTrack_check[0] = true;
        this->setUIControl_checkWidget_rose();
    }


    void RoseHome::slot_applyResult_recentlyArtistCheck(const QList<roseHome::ArtistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_RecentArtist->count() == 0){
                this->list_RecentArtist->clear();
                this->list_RecentArtist->append(list_data);

                this->flag_recentArtist_check[1] = true;
            }
            else{
                int maxCount = (list_data.count() > this->list_RecentArtist->count()) ? this->list_RecentArtist->count() : list_data.count();

                for(int i = 0; i < maxCount; i++){
                    if(list_data.at(i).id != this->list_RecentArtist->at(i).id || list_data.at(i).name != this->list_RecentArtist->at(i).name || list_data.at(i).star != this->list_RecentArtist->at(i).star){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_RecentArtist->at(0).totalCount) || (change_flag > 0)){
                    this->list_RecentArtist->clear();
                    this->list_RecentArtist->append(list_data);

                    this->flag_recentArtist_check[1] = true;
                }
            }
        }
        else if(list_data.size() == 0 && this->list_RecentArtist->size() != 0){
            this->list_RecentArtist->clear();

            this->flag_recentArtist_check[1] = true;
        }

        this->flag_recentArtist_check[0] = true;
        this->setUIControl_checkWidget_rose();
    }


    void RoseHome::slot_applyResult_MyPlaylistCheck(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
            this->list_RecentTrack = new QList<roseHome::TrackItemData>();
            this->list_RecentArtist = new QList<ArtistItemData>();
            this->list_Historylist = new QList<roseHome::HistoryItemData>();
            this->list_MyPlaylist = new QList<PlaylistItemData>();
            this->list_NewPlaylist = new QList<PlaylistItemData>();
            this->list_PopPlaylist = new QList<PlaylistItemData>();
            this->list_DuduoPlaylist = new QList<PlaylistItemData>();

            this->jsonArr_rosetube = QJsonArray();
            this->jsonArr_tracks_toPlay = QJsonArray();

            this->flagNeedReload = false;
            this->setActivePage();

            QResizeEvent fakeEvent(this->size(), this->size());  // 현재 크기를 이용한 가짜 이벤트 생성
            this->resizeEvent(&fakeEvent);
        }
    }


    void RoseHome::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_ROSE)){

            global.user.setDeviceChange(false);

            this->list_Recommends = new QList<RecommendsItemData>();
            this->list_RecPlaylist = new QList<PlaylistItemData>();
            this->list_RecentAlbum = new QList<AlbumItemData>();
            this->list_RecentPlaylist = new QList<PlaylistItemData>();
            this->list_RecentTrack = new QList<roseHome::TrackItemData>();
            this->list_RecentArtist = new QList<ArtistItemData>();
            this->list_Historylist = new QList<roseHome::HistoryItemData>();
            this->list_MyPlaylist = new QList<PlaylistItemData>();
            this->list_NewPlaylist = new QList<PlaylistItemData>();
            this->list_PopPlaylist = new QList<PlaylistItemData>();
            this->list_DuduoPlaylist = new QList<PlaylistItemData>();

            this->jsonArr_rosetube = QJsonArray();
            this->jsonArr_tracks_toPlay = QJsonArray();

            this->flagNeedReload = false;
            this->setActivePage();

            QResizeEvent fakeEvent(this->size(), this->size());  // 현재 크기를 이용한 가짜 이벤트 생성
            this->resizeEvent(&fakeEvent);
        }
    }


    void RoseHome::changedOnlyTabUI_notSendSignal(QString p_step){

        this->contentStep = p_step;

        // Tab 타이틀 변경
        this->menubar->setSelectedSubMenuNoSignal(contentStep);

        // DB 가져와서 데이터 세팅
        if(this->contentStep == ALBTAB_STEP_ALBUM){
            this->stackRecent->setCurrentIndex(0);
        }
        else if(this->contentStep == ALBTAB_STEP_PLAYLIST){
            this->stackRecent->setCurrentIndex(1);
        }
    }


    void RoseHome::slot_changedSubTabUI(const QJsonObject &p_data){

        QString tmp_step = p_data[KEY_PAGE_CODE].toString();

        // 페이지 변경 및 데이터 세팅
        this->changedOnlyTabUI_notSendSignal(tmp_step);
    }


    void RoseHome::slot_hide_msg(){

        if(global.enable_message_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;
        }

        if(global.enable_section_left == true){
            global.enable_section_left = false;
        }

        //ContentLoadingwaitingMsgHide();//c220625
    }


    void RoseHome::slot_time_out(){
        print_debug();
        if(global.enable_message_flag == true && this->widget_login_contents_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;

            if(global.user.getAccess_token().isEmpty()){
                this->widget_rose_contents->hide();
                this->widget_login_contents->show();
                this->stackedWidget_Contents->setCurrentIndex(1);
            }else{
                this->widget_rose_contents->show();
                this->widget_login_contents->hide();
                this->stackedWidget_Contents->setCurrentIndex(0);
            }


        }

        if(global.enable_section_left == true){
            global.enable_section_left = false;
        }
        print_debug();ContentLoadingwaitingMsgHide();
    }


    void RoseHome::slot_clickBtn_Filter(){

        for(int i = 0 ; i < this->btn_filter_cnt; i++){//c230516
            if(!menuTableCheck_ForFilter(this->str_media_name[i])){//c230516
                print_debug();qDebug() << "i=" << i; qDebug() << this->str_media_name[i];
                this->btn_filter[i]->hide();
                this->flowLayout_media->removeWidget(this->btn_filter[i]);
            }else{
                print_debug();qDebug() << "i=" << i; qDebug() << this->str_media_name[i];
                this->flowLayout_media->addWidget(this->btn_filter[i]);
                this->btn_filter[i]->show();
            }
        }

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
            if(!menuTableCheck_ForFilter(this->str_media_name[idx])){//c230516
                print_debug();qDebug() << "idx=" << idx; qDebug() << this->str_media_name[idx];
                this->btn_filter[idx]->hide();
                this->flowLayout_media->removeWidget(this->btn_filter[idx]);
            }else{
                print_debug();qDebug() << "idx=" << idx; qDebug() << this->str_media_name[idx];
                this->flowLayout_media->addWidget(this->btn_filter[idx]);
                this->btn_filter[idx]->show();
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

            QResizeEvent fakeEvent(this->size(), this->size());  // 현재 크기를 이용한 가짜 이벤트 생성
            this->resizeEvent(&fakeEvent);
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
                data_page.pathTitle = this->lb_subTitle[btnId]->text() + tr(" Playlists ") + QString("(%1)").arg(list_count);
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
        else if(btnId == BTN_IDX_SUBTITLE_RecentlyArtist){
            PageInfo_ArtistAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = ROSE_API_ARTIST_PATH;
            data_page.type = "RecentArtist";
            data_page.type_id = HTTP_RECENTLY_ARTIST;
            data_page.filter_type = this->str_media_type;

            this->movePage_artist_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_History){
            PageInfo_HistoryAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.type = "History";
            data_page.type_id = HTTP_HISTORY;
            data_page.filter_type = this->str_media_type;
            data_page.list_history.append(*this->list_Historylist);

            this->movePage_history_allView(data_page);
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
        else if(btnId == BTN_IDX_SUBTITLE_DuduoPlaylists){
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = ROSE_API_DUDUOPLAYLIST_PATH;
            data_page.type = "DuduoPlaylist";
            data_page.type_id = HTTP_DUDUO_PLAYLIST;
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

        QCoreApplication::processEvents();

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

        QCoreApplication::processEvents();

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
        tmp_scrollArea->setFixedHeight(285);

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
        else if(section == SECTION_FOR_MORE_POPUP___History){

            for(int i = 0; i < this->list_Historylist->count(); i++){
                roseHome::HistoryItemData tmpData;
                tmpData.yearMonth = this->list_Historylist->at(i).yearMonth;
                tmpData.visible = this->list_Historylist->at(i).visible;
                tmpData.filter_type = this->str_media_type;

                this->list_Historylist->replace(i, tmpData);
            }

            this->proc_clicked_itemHistory(this->list_Historylist, clickMode, index, section);
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
        else if(section == SECTION_FOR_MORE_POPUP___DuduoPlaylists){
            this->proc_clicked_itemPlaylist_rose(this->list_DuduoPlaylist, clickMode, index, section);
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

                this->track_idx_fav = index;
                this->track_star_fav = star_fav;
                this->track_type_fav = SECTION_FOR_MORE_POPUP___RecentlyRosetube;

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
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseHome::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, flag_fav, star_fav);

                this->flag_check_track = true;

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
                    ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
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
                    data_header.type = ProcJsonEasy::getString(tmpObj, "type");      //j220906 share link
                    data_header.flagProcStar = false;
                    data_header.isShare = true;      //j220906 share link

                    // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Rosetube_Track, data_header, index, section);
                }
            }
        }
    }


    void RoseHome::slot_add_rosePlaylist_withRosetube(const int &idx, const QJsonObject &dataObj){

        QString view_type = "";
        if(idx < 0){
            view_type = "create";
        }
        else{
            view_type = "add";
        }

        QJsonObject data = dataObj;
        data.insert("view_type", view_type);
        data.insert("type", "ROSETUBE");

        QJsonObject jsonObj_move;
        jsonObj_move.insert("data", data);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RH_ADD_PLAYLIST);

        emit linker->signal_clicked_movePage(jsonObj_move);
    }


    void RoseHome::slot_applyResult_myPlaylistDelete(const QJsonObject &jsonObj){

        if(jsonObj.value("flagOk").toBool() == true && jsonObj.value("message").toString() == "정상"){
            this->flag_recentAlbum_check[0] = true;
            this->flag_recentPlaylist_check[0] = true;
            this->flag_recentRosetube_check[0] = true;
            this->flag_recentTrack_check[0] = true;
            this->flag_recentArtist_check[0] = true;
            this->flag_myPlaylist_check[0] = true;

            this->flag_recentAlbum_check[1] = false;
            this->flag_recentPlaylist_check[1] = false;
            this->flag_recentRosetube_check[1] = false;
            this->flag_recentTrack_check[1] = false;
            this->flag_recentArtist_check[1] = false;
            this->flag_myPlaylist_check[1] = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
            connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &RoseHome::slot_applyResult_MyPlaylistCheck);
            proc_myPlaylist->request_rose_getList_myPlaylists(ROSE_API_MYPLAYLIST_PATH , this->str_media_type, 0, 15);
        }
    }


    void RoseHome::slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고. => OptMorePopup 에서 하라고, 가려줌
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
            bool status  = ProcJsonEasy::getBool(p_jsonObj, "status");

            // Qobuz favorite toggle check
            if(this->flag_send_track == true){
                if((status == true && this->flag_track_fav == false) || (status == false && this->flag_track_fav == true)){
                    // Tidal Favorite toggle
                    //ProcCommon *proc = new ProcCommon(this);
                    //connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &BugsRecentlyTrackAll::slot_bugs_completeReq_listAll_myFavoritesIds);
                    //proc->request_tidal_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                }
                //this->flag_send_track = false;
            }
        }
    }


    void RoseHome::slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고.
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            // Tidal favorite check
            if(this->flag_send_track == true){
                QVariantList arr_myFavoriteIds = ProcJsonEasy::getJsonArray(p_jsonObj, "TRACK").toVariantList();
                bool status = arr_myFavoriteIds.contains(this->track_id_fav);

                if(status == true && this->flag_track_fav == false){
                    // Tidal track Favorite del
                    tidal::ProcCommon *proc = new tidal::ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &RoseHome::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                else if(status == false && this->flag_track_fav == true){
                    // Tidal track Favorite add
                    tidal::ProcCommon *proc = new tidal::ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &RoseHome::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                this->flag_send_track = false;
            }
            else{

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

                    if(this->track_type_fav == SECTION_FOR_MORE_POPUP___RecentlyTrack){
                        this->home_recently_track[i]->setFavoritesIds(flag, star);
                    }
                    else if(this->track_type_fav == SECTION_FOR_MORE_POPUP___RecentlyRosetube){
                        this->home_recently_rosetube[i]->setFavorite_btnHeart(flag, star);
                    }
                }
            }
            else if(track_info.contains("message") && (track_info["message"].toString() == "정상")){
                if(this->track_type_fav == SECTION_FOR_MORE_POPUP___RecentlyTrack){
                    this->home_recently_track[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
                }
                else if(this->track_type_fav == SECTION_FOR_MORE_POPUP___RecentlyRosetube){
                    this->home_recently_rosetube[this->track_idx_fav]->setFavorite_btnHeart(this->flag_track_fav, this->track_star_fav);
                }
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }

        ContentLoadingwaitingMsgHide();
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

                QString type = this->list_RecentTrack->at(idx).type;

                if(this->track_star_fav == 3){
                    this->track_star_fav = 0;
                    this->flag_track_fav = false;
                }
                else if(this->track_star_fav >= 0 && this->track_star_fav < 3){
                    this->track_star_fav++;
                    this->flag_track_fav = true;
                }
                if(this->track_star_fav == 0 || this->track_star_fav == 1){
                    if(type == "BUGS"){
                        // Bugs Favorite toggle
                        bugs::ItemPositionData itemPosData;
                        itemPosData.section = section;
                        itemPosData.index = idx;
                        itemPosData.data_id = QString("%1").arg(this->list_RecentTrack->at(idx).id);

                        bugs::ProcBugsAPI *proc = new bugs::ProcBugsAPI(this);
                        connect(proc, &bugs::ProcBugsAPI::completeReq_favarite_track, this, &RoseHome::slot_bugs_completeReq_listAll_myFavoritesIds);

                        if(this->track_star_fav == 0){
                            itemPosData.likes_yn = false;

                            proc->request_bugs_deleteFavorite_track(this->list_RecentTrack->at(idx).clientKey.toInt(), bugs::ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                        }
                        else if(this->track_star_fav == 1){
                            itemPosData.likes_yn = true;

                            proc->request_bugs_addFavorite_track(this->list_RecentTrack->at(idx).clientKey.toInt(), bugs::ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                        }

                        this->flag_send_track = true;
                    }
                    else if(type == "QOBUZ"){
                        this->track_id_fav = this->list_RecentTrack->at(idx).clientKey.toInt();

                        qobuz::ProcCommon *proc = new qobuz::ProcCommon(this);
                        connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &RoseHome::slot_bugs_completeReq_listAll_myFavoritesIds);
                        proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                        this->flag_send_track = true;
                    }
                    else if(type == "TIDAL"){
                        this->track_id_fav = this->list_RecentTrack->at(idx).clientKey.toInt();

                        if(this->track_star_fav == 1){
                            // Tidal Track Favorite add
                            tidal::ProcCommon *proc = new tidal::ProcCommon(this);
                            proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                        }
                        else if(this->track_star_fav == 0){
                            // Tidal Track Favorite del
                            tidal::ProcCommon *proc = new tidal::ProcCommon(this);
                            proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                        }

                        // favorite 정보 가져오기
                        tidal::ProcCommon *proc_fav = new tidal::ProcCommon(this);
                        connect(proc_fav, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &RoseHome::slot_tidal_completeReq_listAll_myFavoritesIds);
                        proc_fav->request_tidal_getAll_favorites();
                        this->flag_send_track = true;
                    }
                }

                this->track_idx_fav = idx;
                this->track_type_fav = SECTION_FOR_MORE_POPUP___RecentlyTrack;

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

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_RecentTrack, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }


    void  RoseHome::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리 - Artist 파트가 여러개임
        if(section == SECTION_FOR_MORE_POPUP___RecentlyArtist){

            if(this->list_RecentArtist->at(index).type == "APPLE_MUSIC"){
                DialogConfirm *dlgConfirmMSG = new DialogConfirm(this);
                dlgConfirmMSG->setTitle(tr("Notice"));
                dlgConfirmMSG->setFixedSize(350, 400);
                dlgConfirmMSG->setAlignment(Qt::AlignCenter);
                dlgConfirmMSG->setTextHeight(150);

                dlgConfirmMSG->setText(tr("Service is being prepared."));

                dlgConfirmMSG->exec();

                delete dlgConfirmMSG;
            }
            else{
                this->proc_clicked_itemArtist(this->list_RecentArtist, clickMode, index, section);
            }
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
            else if(clickMode == OptMorePopup::ClickMode::Add_MyCollection){
                QJsonArray tracks = QJsonArray();
                tracks.append(this->jsonArr_rosetube.at(index).toObject());

                QJsonObject json = QJsonObject();
                json.insert("tracks", tracks);

                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::ROSETUBE, json, this);
                dialog_playlist->request_playlist_fetch();
                connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &RoseHome::slot_add_rosePlaylist_withRosetube);
                int result = dialog_playlist->exec();

                if(result == QDialog::Accepted){
                    delete dialog_playlist;
                }
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
        else if(section == SECTION_FOR_MORE_POPUP___RecentlyArtist){
            this->proc_clicked_optMorePopup_fromArtist(this->list_RecentArtist, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___MyPlaylists){
            if(clickMode == OptMorePopup::ClickMode::Delete){

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
                    connect(proc_myPlaylistDel, &roseHome::ProcCommon::completeReq_delete_playlist, this, &RoseHome::slot_applyResult_myPlaylistDelete);
                    proc_myPlaylistDel->request_rose_delete_myPlaylist(this->list_MyPlaylist->at(index).id);
                }

                delete dlg;
            }
            else{
                this->proc_clicked_optMorePopup_fromPlaylist(this->list_MyPlaylist, index, SECTION_FOR_MORE_POPUP___MyPlaylists, clickMode);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___NewPlaylists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_NewPlaylist, index, SECTION_FOR_MORE_POPUP___NewPlaylists, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___PopPlaylists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_PopPlaylist, index, SECTION_FOR_MORE_POPUP___PopPlaylists, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___DuduoPlaylists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_DuduoPlaylist, index, SECTION_FOR_MORE_POPUP___DuduoPlaylists, clickMode);
        }
    }


    /**
     * @brief RoseHome::resizeEvent
     * @param event
     */
    void RoseHome::resizeEvent(QResizeEvent *event){//c230223

        Q_UNUSED(event);

        int width = this->width() - 180;
        qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << this->width() << width - 180;

        int left = 0;
        int top = 0;

        this->widget_login_contents->setFixedSize(width, 500);

        left = (width - this->lb_loginBefore->sizeHint().width()) / 2;
        top = (500 - 55 - 30 - this->lb_loginBefore->sizeHint().height()) / 2;
        this->lb_loginBefore->setGeometry(left, top, this->lb_loginBefore->sizeHint().width(), this->lb_loginBefore->sizeHint().height());

        //qDebug() << "[Debug] RoseHome::resizeEvent " << width << left << top;

        left = (width - 230) / 2;
        this->btn_loginBefore->setGeometry(left, top + this->lb_loginBefore->sizeHint().height() + 30, 0, 0);

        //qDebug() << "[Debug] RoseHome::resizeEvent " << width << left << top + this->lb_loginBefore->sizeHint().height();
    }
}
