#include "bugs/BugsHome.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"
#include "common/rosesettings.h"

#include "widget/NoData_Widget.h"

#include "widget/toastmsg.h"
#include "common/networkhttp.h"//c220729


#include <delegate/BugsTrackDelegate.h>
#include "tidal/AbstractItem.h"

#include <QDebug>
#include <QScroller>
#include <QSettings>


namespace bugs {

    // subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX___music_recently = 1;
    const int BTN_IDX___music_bugsChart = 2;

    const int SECTION_FOR_MORE_POPUP___music_recently = 1;
    const int SECTION_FOR_MORE_POPUP___music_bugsChart = 2;

    const QString BUGS_API_SUBPAHT___music_recently = "genre/track/total/all";
    const QString BUGS_API_SUBPAHT___music_bugsChart = "chart/track/realtime";

    // Bugs subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX___track_recently = 1;
    const int BTN_IDX___album_recently = 2;
    const int BTN_IDX___video_recently = 3;
    const int BTN_IDX___track_bugsChart = 4;
    const int BTN_IDX___album_bugsChart = 5;
    const int BTN_IDX___video_bugsChart = 6;

    const int SECTION_FOR_MORE_POPUP___track_recently = 1;
    const int SECTION_FOR_MORE_POPUP___album_recently = 2;
    const int SECTION_FOR_MORE_POPUP___video_recently = 3;
    const int SECTION_FOR_MORE_POPUP___track_bugsChart = 4;
    const int SECTION_FOR_MORE_POPUP___album_bugsChart = 5;
    const int SECTION_FOR_MORE_POPUP___video_bugsChart = 6;

    QString BUGS_API_SUBPAHT___track_recently = "genre/track/total/all";
    QString BUGS_API_SUBPAHT___album_recently = "genre/album/total/all";
    QString BUGS_API_SUBPAHT___video_recently = "genre/mv/total/all";
    QString BUGS_API_SUBPAHT___track_bugsChart = "chart/track/realtime";
    QString BUGS_API_SUBPAHT___album_bugsChart = "chart/album/day/20151";
    QString BUGS_API_SUBPAHT___video_bugsChart = "chart/mv/day/20151";


    // ROSE subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX_SUBTITLE_recentlyPlay = 10;
    const int BTN_IDX_SUBTITLE_recentlyTrack = 12;
    const int BTN_IDX_SUBTITLE_myPlaylists = 13;
    const int BTN_IDX_SUBTITLE_userPlaylists = 14;

    // ROSE section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___recentlyAlbum = 10;
    const int SECTION_FOR_MORE_POPUP___recentlyPlaylist = 11;
    const int SECTION_FOR_MORE_POPUP___recentlyTrack = 12;
    const int SECTION_FOR_MORE_POPUP___myPlaylists = 13;
    const int SECTION_FOR_MORE_POPUP___userPlaylists = 14;

    // ROSE API 관련
    QString ROSE_API_ALBUM_PATH = "member/album/recent";
    QString ROSE_API_PLAYLIST_PATH = "member/playlist/recent";
    QString ROSE_API_TRACK_PATH = "member/track/recent";
    QString ROSE_API_MYPLAYLIST_PATH = "member/playlist";
    QString ROSE_API_USERPLAYLIST_PATH = "member/playlist/all";

    QString ALBTAB_STEP_ALBUM = "album";
    QString ALBTAB_STEP_PLAYLIST = "playlist";


    /**
         * @brief 생성자
         * @param parent
         */
    BugsHome::BugsHome(QWidget *parent) : AbstractBugsSubWidget(VerticalScroll_filter, parent) {

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));
        connect(linker, SIGNAL(signal_message_show_timeout()), SLOT(slot_time_out()));
        connect(linker, SIGNAL(signal_dialog_share_link(QString)), SLOT(slot_dialog_share_link(QString)));//c220902_1


        if(global.user_forBugs.isLogined() == true){

            //data
            this->list_trackRecently = new QList<bugs::TrackItemData>();
            this->list_albumRecently = new QList<bugs::AlbumItemData>();
            this->list_videoRecently = new QList<bugs::VideoItemData>();
            this->list_trackBugsChart = new QList<bugs::TrackItemData>();
            this->list_albumBugsChart = new QList<bugs::AlbumItemData>();
            this->list_videoBugsChart = new QList<bugs::VideoItemData>();

            this->list_recentlyAlbum = new QList<roseHome::AlbumItemData>();
            this->list_recentlyPlaylist = new QList<roseHome::PlaylistItemData>();
            this->list_recentlytrack = new QList<roseHome::TrackItemData>();
            this->list_myPlaylist = new QList<roseHome::PlaylistItemData>();
            this->list_userPlaylist = new QList<roseHome::PlaylistItemData>();
        }
        else{
            if(global.enable_section_left == true){
                global.enable_section_left = false;
            }
        }

        // Data
        this->list_track_musicRecently = new QList<bugs::TrackItemData>();
        this->list_track_bugsChart = new QList<bugs::TrackItemData>();

        this->flagReq_track_bugsChart = false;
        this->flagReq_track_musicRecently = false;
    }


    BugsHome::~BugsHome(){

        this->deleteLater();
    }




    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsHome::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            this->page = pageCode;

            // init data for Rose
            this->list_recentlyAlbum->clear();
            this->list_recentlyPlaylist->clear();
            this->list_recentlytrack->clear();
            this->list_myPlaylist->clear();
            this->list_userPlaylist->clear();

            this->jsonArr_tracks_toPlay = QJsonArray();

            this->flag_album[0] = false;
            this->flag_playlist[0] = false;
            this->flag_track[0] = false;
            this->flag_myPlaylist[0] = false;
            this->flag_userPlaylist[0] = false;

            this->flag_album[1] = false;
            this->flag_playlist[1] = false;
            this->flag_track[1] = false;
            this->flag_myPlaylist[1] = false;
            this->flag_userPlaylist[1] = false;

            // init data for Bugs
            this->list_trackRecently->clear();
            this->list_albumRecently->clear();
            this->list_videoRecently->clear();
            this->list_trackBugsChart->clear();
            this->list_albumBugsChart->clear();
            this->list_videoBugsChart->clear();

            this->jsonArr_recently_toPlay = QJsonArray();
            this->jsonArr_bugsChart_toPlay = QJsonArray();

            this->video_recently_toPlay = QJsonArray();
            this->video_bugsChart_toPlay = QJsonArray();

            flag_trackRecently[0] = false;
            flag_albumRecently[0] = false;
            flag_videoRecently[0] = false;
            flag_trackBugsChart[0] = false;
            flag_albumBugsChart[0] = false;
            flag_videoBugsChart[0] = false;

            flag_trackRecently[1] = false;
            flag_albumRecently[1] = false;
            flag_videoRecently[1] = false;
            flag_trackBugsChart[1] = false;
            flag_albumBugsChart[1] = false;
            flag_videoBugsChart[1] = false;
        }
    }


    /**
     * @brief TidalHome::setActivePage
     */
    void BugsHome::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            this->box_contents->removeWidget(this->widget_rose_contents);
            this->box_contents->removeWidget(this->widget_home_contents);
            GSCommon::clearLayout(this->box_contents);

            this->box_rose_contents = new QVBoxLayout();
            this->box_rose_contents->setSpacing(0);
            this->box_rose_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_rose_contents = new QWidget();
            this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_rose_contents->setLayout(this->box_rose_contents);

            this->box_contents->addWidget(this->widget_rose_contents, 0, Qt::AlignTop);

            QJsonArray *p_jsonArray_titlSub = new QJsonArray();
            QJsonObject sub1 { {"name", "앨범"}, {"code", ALBTAB_STEP_ALBUM} };
            QJsonObject sub2 { {"name", "플레이리스트"}, {"code", ALBTAB_STEP_PLAYLIST} };

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

            this->box_home_contents = new QVBoxLayout();
            this->box_home_contents->setSpacing(0);
            this->box_home_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_home_contents = new QWidget();
            this->widget_home_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_home_contents->setLayout(this->box_home_contents);

            //---------------------//c220729  start


            this->box_total_home_contents = new QVBoxLayout();
            this->box_total_home_contents->setSpacing(0);
            this->box_total_home_contents->setContentsMargins(0, 0, 0, 0);

            this->box_total_home_contents->addWidget(this->widget_rose_contents, 0, Qt::AlignTop);
            this->box_total_home_contents->addWidget(this->widget_home_contents, 0, Qt::AlignTop);

            this->widget_total_home_contents = new QWidget();
            this->widget_total_home_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_total_home_contents->setLayout(this->box_total_home_contents);

            this->box_contents->addWidget(this->widget_total_home_contents);

            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);


            //--------------------------------------------//c220729  end
            // init UI
            this->vBox_recentlyPlay = new QVBoxLayout();
            this->vBox_recentlyTrack = new QVBoxLayout();
            this->vBox_myPlaylist = new QVBoxLayout();
            this->vBox_userPlaylist = new QVBoxLayout();

            this->hBox_recentlyAlbum = new QHBoxLayout();
            this->hBox_recentlyPlaylist = new QHBoxLayout();
            this->hBox_myPlaylist = new QHBoxLayout();
            this->hBox_userPlaylist = new QHBoxLayout();

            this->vBox_trackRecently = new QVBoxLayout();
            this->vBox_albumRecently = new QVBoxLayout();
            this->vBox_videoRecently = new QVBoxLayout();
            this->vBox_trackBugsChart = new QVBoxLayout();
            this->vBox_albumBugsChart = new QVBoxLayout();
            this->vBox_videoBugsChart = new QVBoxLayout();

            this->hBox_trackRecently = new QHBoxLayout();
            this->hBox_albumRecently = new QHBoxLayout();
            this->hBox_videoRecently = new QHBoxLayout();
            this->hBox_trackBugsChart = new QHBoxLayout();
            this->hBox_albumBugsChart = new QHBoxLayout();
            this->hBox_videoBugsChart = new QHBoxLayout();

            // init UI
            GSCommon::clearLayout(this->vBox_recentlyPlay);
            GSCommon::clearLayout(this->vBox_recentlyTrack);
            GSCommon::clearLayout(this->vBox_myPlaylist);
            GSCommon::clearLayout(this->vBox_userPlaylist);

            GSCommon::clearLayout(this->hBox_recentlyAlbum);
            GSCommon::clearLayout(this->hBox_recentlyPlaylist);
            GSCommon::clearLayout(this->hBox_myPlaylist);
            GSCommon::clearLayout(this->hBox_userPlaylist);

            GSCommon::clearLayout(this->vBox_trackRecently);
            GSCommon::clearLayout(this->vBox_albumRecently);
            GSCommon::clearLayout(this->vBox_videoRecently);
            GSCommon::clearLayout(this->vBox_trackBugsChart);
            GSCommon::clearLayout(this->vBox_albumBugsChart);
            GSCommon::clearLayout(this->vBox_videoBugsChart);

            GSCommon::clearLayout(this->hBox_trackRecently);
            GSCommon::clearLayout(this->hBox_albumRecently);
            GSCommon::clearLayout(this->hBox_videoRecently);
            GSCommon::clearLayout(this->hBox_trackBugsChart);
            GSCommon::clearLayout(this->hBox_albumBugsChart);
            GSCommon::clearLayout(this->hBox_videoBugsChart);

            // sub Title UI
            for(int i = 0; i < 20; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            for(int i = 0; i < 15; i++){
                this->home_recently_album[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___recentlyAlbum, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                connect(this->home_recently_album[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &BugsHome::slot_clickedItemAlbum);
            }

            for(int i = 0; i < 15; i++){
                this->home_recently_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___recentlyPlaylist, tidal::AbstractItem::ImageSizeMode::Square_200x200, 2, true);
                connect(this->home_recently_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &BugsHome::slot_clickedItemPlaylist);
            }

            for (int i = 0; i < 5; i++) {
                this->home_recently_track[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->home_recently_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &BugsHome::slot_clickedItemTrack_inList);
                this->home_recently_track[i]->setProperty("index", i);
                this->home_recently_track[i]->setObjectName("recently_tracks");
            }

            for(int i = 0; i < 15; i++){
                this->home_myPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___myPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                connect(this->home_myPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &BugsHome::slot_clickedItemPlaylist);
            }

            for(int i = 0; i < 15; i++){
                this->home_userPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___userPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);
                connect(this->home_userPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &BugsHome::slot_clickedItemPlaylist);
            }


            for(int i = 0; i < 5; i++){
                this->home_track_recently[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->home_track_recently[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &BugsHome::slot_clickedItemTrack_inList);
                this->home_track_recently[i]->setProperty("index", i);
                this->home_track_recently[i]->setObjectName("track_recently");
            }

            for(int i = 0; i < 15; i++){
                this->home_album_recently[i] = new bugs::ItemAlbum_bugs(i, SECTION_FOR_MORE_POPUP___album_recently, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_album_recently[i], &bugs::ItemAlbum_bugs::signal_clicked, this, &BugsHome::slot_clickedItemAlbum);
            }

            for(int i = 0; i < 15; i++){
                this->home_video_recently[i] = new bugs::ItemVideo_bugs(i, SECTION_FOR_MORE_POPUP___video_recently, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200);
                connect(this->home_video_recently[i], &bugs::ItemVideo_bugs::signal_clicked, this, &BugsHome::slot_clickedItemVideo);
            }

            for(int i = 0; i < 5; i++){
                this->home_track_bugsChart[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->home_track_bugsChart[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &BugsHome::slot_clickedItemTrack_inList);
                this->home_track_bugsChart[i]->setProperty("index", i);
                this->home_track_bugsChart[i]->setObjectName("track_bugsChart");
            }

            for(int i = 0; i < 15; i++){
                this->home_album_bugsChart[i] = new bugs::ItemAlbum_bugs(i, SECTION_FOR_MORE_POPUP___album_bugsChart, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_album_bugsChart[i], &bugs::ItemAlbum_bugs::signal_clicked, this, &BugsHome::slot_clickedItemAlbum);
            }

            for(int i = 0; i < 15; i++){
                this->home_video_bugsChart[i] = new bugs::ItemVideo_bugs(i, SECTION_FOR_MORE_POPUP___video_bugsChart, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200);
                connect(this->home_video_bugsChart[i], &bugs::ItemVideo_bugs::signal_clicked, this, &BugsHome::slot_clickedItemVideo);
            }

            if(global.updateCheckFlag){

                //ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223
                //QTimer::singleShot(10000, this, SLOT(slot_hide_msg()));

                this->page = "";
                this->flagNeedReload = false;

                if(global.enable_section_left == true){
                    global.enable_section_left = false;
                }

                QJsonObject tmp_data;
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS___SETTING);

                // 부모에게 페이지 변경하라고 시그널 보냄
                emit this->signal_clickedMovePage(tmp_data);
            }
            else{

                global.enable_message_count = 0;
                global.enable_message_flag = true;
                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223

                this->setUIControl_requestRose();
                this->setUIControl_requestBugs();
            }
        }
    }


    /**
     * @brief BugsHome::applyLoginSuccessState
     */
    void BugsHome::applyLoginSuccessState(){

        /*if(this->is_bugs_logined()){

            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;
                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                this->setUIControl_logined();
            }

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));      //cheontidal

            QTimer::singleShot(13000, this, SLOT(slot_hide_msg()));

            // 서브메뉴 광클 방지
            if(this->flagReq_track_musicRecently == false){
                this->flagReq_track_musicRecently = true;

                this->list_track_musicRecently->clear();
                GSCommon::clearLayout(this->hBox_musicRecently);

                // request HTTP API
                ProcBugsAPI *proc_recently = new ProcBugsAPI(this);
                proc_recently->setProperty("section", SECTION_FOR_MORE_POPUP___music_recently);
                connect(proc_recently, &ProcBugsAPI::completeReq_list_tracks, this, &BugsHome::slot_applyResult_tracks);
                proc_recently->request_bugs_getList_tracks(BUGS_API_SUBPAHT___music_recently, 10, 1);
            }

            if(this->flagReq_track_bugsChart == false){
                this->flagReq_track_bugsChart = true;

                this->list_track_bugsChart->clear();

                this->listWidget_music_bugsChart->clear();
                this->listWidget_music_bugsChart->setFixedHeight(70*5);

                ProcBugsAPI *proc_chart = new ProcBugsAPI(this);
                proc_chart->setProperty("section", SECTION_FOR_MORE_POPUP___music_bugsChart);
                connect(proc_chart, &ProcBugsAPI::completeReq_list_tracks, this, &BugsHome::slot_applyResult_tracks);
                proc_chart->request_bugs_getList_tracks(BUGS_API_SUBPAHT___music_bugsChart, 5, 1);
            }

        }
        else{
            if(global.enable_section_left == true){
                global.enable_section_left = false;
            }

            if(this->m_flagCurrLogined){
                // 로그인 ---> 로그아웃
                this->m_flagCurrLogined = false;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->setUIControl_notLogin();
            }
        }*/
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------


    void BugsHome::setUIControl_requestRose(){

        if(!global.user.getAccess_token().isEmpty()){

            // request HTTP API for Recently Album
            roseHome::ProcCommon *proc_recentlyAlbum = new roseHome::ProcCommon(this);
            connect(proc_recentlyAlbum, &roseHome::ProcCommon::completeReq_list_albums, this, &BugsHome::slot_applyResult_recentlyAlbum);
            proc_recentlyAlbum->request_rose_getList_recentlyAlbums(ROSE_API_ALBUM_PATH , "BUGS", 0, 15);

            // request HTTP API for Recently Playlist
            roseHome::ProcCommon *proc_recentlyPlaylist = new roseHome::ProcCommon(this);
            connect(proc_recentlyPlaylist, &roseHome::ProcCommon::completeReq_list_playlists, this, &BugsHome::slot_applyResult_recentlyPlaylist);
            proc_recentlyPlaylist->request_rose_getList_recentlyPlaylists(ROSE_API_PLAYLIST_PATH , "BUGS", 0, 15);

            // request HTTP API for Recently Track
            roseHome::ProcCommon *proc_recentlyTrack = new roseHome::ProcCommon(this);
            connect(proc_recentlyTrack, &roseHome::ProcCommon::completeReq_list_tracks, this, &BugsHome::slot_applyResult_recentlyTrack);
            proc_recentlyTrack->request_rose_getList_recentlyTracks(ROSE_API_TRACK_PATH , "BUGS", 0, 5);

            // request HTTP API for Recently Playlist
            roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
            connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &BugsHome::slot_applyResult_myPlaylist);
            proc_myPlaylist->request_rose_getList_myPlaylists(ROSE_API_MYPLAYLIST_PATH , "BUGS", 0, 15);

            // request HTTP API for User Playlist
            roseHome::ProcCommon *proc_userPlaylist = new roseHome::ProcCommon(this);
            connect(proc_userPlaylist, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &BugsHome::slot_applyResult_userPlaylist);
            proc_userPlaylist->request_rose_getList_usersPlaylists(ROSE_API_USERPLAYLIST_PATH , "PLAYLIST_RECENT", "BUGS", 0, 15);
        }
        else{

            this->widget_rose_contents->hide();
        }
    }


    void BugsHome::setUIControl_requestBugs(){

        // request HTTP API for Recently Track
        ProcBugsAPI *proc_track_recently = new ProcBugsAPI(this);
        connect(proc_track_recently, &ProcBugsAPI::completeReq_list_tracks, this, &BugsHome::slot_applyResult_trackRecently);
        proc_track_recently->request_bugs_getList_tracks(BUGS_API_SUBPAHT___track_recently, 5, 1);

        // request HTTP API for Recently Album
        ProcBugsAPI *proc_album_recently = new ProcBugsAPI(this);
        connect(proc_album_recently, &ProcBugsAPI::completeReq_list_albums, this, &BugsHome::slot_applyResult_albumRecently);
        proc_album_recently->request_bugs_getList_albums(BUGS_API_SUBPAHT___album_recently, 15, 1);

        // request HTTP API for Recently Album
        ProcBugsAPI *proc_video_recently = new ProcBugsAPI(this);
        connect(proc_video_recently, &ProcBugsAPI::completeReq_list_videos, this, &BugsHome::slot_applyResult_videoRecently);
        proc_video_recently->request_bugs_getList_videos(BUGS_API_SUBPAHT___video_recently, 15, 1);

        // request HTTP API for Bugs track chart
        ProcBugsAPI *proc_track_bugsChart = new ProcBugsAPI(this);
        connect(proc_track_bugsChart, &ProcBugsAPI::completeReq_list_tracks, this, &BugsHome::slot_applyResult_trackBugsChart);
        proc_track_bugsChart->request_bugs_getList_tracks(BUGS_API_SUBPAHT___track_bugsChart, 5, 1);

        // request HTTP API for Bugs Album chart
        ProcBugsAPI *proc_album_bugsChart = new ProcBugsAPI(this);
        connect(proc_album_bugsChart, &ProcBugsAPI::completeReq_list_albums, this, &BugsHome::slot_applyResult_albumBugsChart);
        proc_album_bugsChart->request_bugs_getList_albums(BUGS_API_SUBPAHT___album_bugsChart, 15, 1);

        // request HTTP API for Recently Album
        ProcBugsAPI *proc_video_bugsChart = new ProcBugsAPI(this);
        connect(proc_video_bugsChart, &ProcBugsAPI::completeReq_list_videos, this, &BugsHome::slot_applyResult_videoBugsChart);
        proc_video_bugsChart->request_bugs_getList_videos(BUGS_API_SUBPAHT___video_bugsChart, 15, 1);
    }


    void BugsHome::setUIControl_appendWidget_rose(){

        if(this->flag_album[0] == true && this->flag_playlist[0] == true && this->flag_track[0] == true && this->flag_myPlaylist[0] == true && this->flag_userPlaylist[0] == true)
        {
            if(this->flag_album[0] == true){

                this->widget_recentPlay = this->setUIControl_subTitle_withSubMenu("최근 재생", "View All", BTN_IDX_SUBTITLE_recentlyPlay, this->vBox_recentlyPlay);

                this->vBox_recentlyPlay->addSpacing(0);
                this->vBox_recentlyPlay->addWidget(this->stackRecent, 1, Qt::AlignTop);

                if(this->flag_album[1] == true){
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
                    if(this->list_recentlyAlbum->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_recentlyAlbum->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_album[i]->setData(this->list_recentlyAlbum->at(i));
                        this->hBox_recentlyAlbum->addWidget(this->home_recently_album[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                    noData_widget->setFixedSize(1500, 305);

                    this->stackRecent->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_recentlyPlay);
                this->box_rose_contents->addSpacing(45);
            }

            if(this->flag_playlist[1] == true){

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
                if(this->list_recentlyPlaylist->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_recentlyPlaylist->size();
                }

                for(int i = 0; i < maxCount; i++){
                    this->home_recently_playlist[i]->setData(this->list_recentlyPlaylist->at(i));
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

            if(this->flag_track[0] == true){

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
                if(this->list_recentlytrack->size() > 0){
                    this->lb_subTitle[BTN_IDX_SUBTITLE_recentlyTrack]->setText(subTitle + QString(" (%1)").arg(this->list_recentlytrack->at(0).totalCount));
                }
                else{
                    this->lb_subTitle[BTN_IDX_SUBTITLE_recentlyTrack]->setText(subTitle + QString(" (0)"));
                }
                this->lb_subTitle[BTN_IDX_SUBTITLE_recentlyTrack]->setStyleSheet("font-size:24px;font-weight:bold;color:#FFFFFF;");
                tmp_hBox->addWidget(this->lb_subTitle[BTN_IDX_SUBTITLE_recentlyTrack], 0, Qt::AlignVCenter);

                // side button - it works when the button text is not empty
                this->btnView_all[BTN_IDX_SUBTITLE_recentlyTrack]->setText("View All");
                this->btnView_all[BTN_IDX_SUBTITLE_recentlyTrack]->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
                this->btnView_all[BTN_IDX_SUBTITLE_recentlyTrack]->setProperty("idx", BTN_IDX_SUBTITLE_recentlyTrack);
                this->btnView_all[BTN_IDX_SUBTITLE_recentlyTrack]->setCursor(Qt::PointingHandCursor);
                connect(this->btnView_all[BTN_IDX_SUBTITLE_recentlyTrack], SIGNAL(clicked()), this, SLOT(slot_clickBtn_subTitle_viewAll()));

                tmp_hBox->addWidget(this->btnView_all[BTN_IDX_SUBTITLE_recentlyTrack], 1, Qt::AlignVCenter | Qt::AlignRight);

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

                if(this->flag_track[1] == true){
                    int maxCount = 0;
                    if(this->list_recentlytrack->at(0).totalCount > 5){
                        maxCount = 5;
                    }
                    else{
                        maxCount = this->list_recentlytrack->at(0).totalCount;
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_track[i]->setDataTrackInfo_RoseMain(this->list_recentlytrack->at(i));
                        this->home_recently_track[i]->setFavoritesIds(this->list_recentlytrack->at(i).favorite, this->list_recentlytrack->at(i).star);
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
                this->box_rose_contents->addSpacing(65);
            }

            if(this->flag_myPlaylist[0] == true){

                this->widget_myPlaylist = new QWidget();
                if(this->list_myPlaylist->size() > 0){
                    this->widget_myPlaylist = this->setUIControl_subTitle_withSideBtn("My 플레이리스트" + QString(" (%1)").arg(this->list_myPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_myPlaylists, this->vBox_myPlaylist);
                }
                else{
                    this->widget_myPlaylist = this->setUIControl_subTitle_withSideBtn("My 플레이리스트" + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_myPlaylists, this->vBox_myPlaylist);
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
                    if(this->list_myPlaylist->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_myPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_myPlaylist[i]->setData(this->list_myPlaylist->at(i));
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

            if(this->flag_userPlaylist[0] == true){

                this->widget_userPlaylist = new QWidget();
                if(this->list_userPlaylist->size() > 0){
                    this->widget_userPlaylist = this->setUIControl_subTitle_withSideBtn(tr("BUGS Playlist On Rose") + QString(" (%1)").arg(this->list_userPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_userPlaylists, this->vBox_userPlaylist);
                }
                else{
                    this->widget_userPlaylist = this->setUIControl_subTitle_withSideBtn(tr("BUGS Playlist On Rose") + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_userPlaylists, this->vBox_userPlaylist);
                }

                this->vBox_userPlaylist->addSpacing(10);

                if(this->flag_userPlaylist[1] == true){
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
                    userPlaylist_scrollArea->setFixedHeight(305);

                    QScroller::grabGesture(userPlaylist_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_userPlaylist->addWidget(userPlaylist_scrollArea);

                    int maxCount = 0;
                    if(this->list_userPlaylist->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_userPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_userPlaylist[i]->setData(this->list_userPlaylist->at(i));
                        this->hBox_userPlaylist->addWidget(this->home_userPlaylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 305);

                    this->vBox_userPlaylist->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_userPlaylist);
                this->box_rose_contents->addSpacing(50);
            }
        }
    }


    void BugsHome::setUIControl_appendWidget(){

        if(this->flag_trackRecently[0] == true && this->flag_albumRecently[0] == true && this->flag_videoRecently[0] == true
                && this->flag_trackBugsChart[0] == true && this->flag_albumBugsChart[0] == true && this->flag_videoBugsChart[0] == true){

            QString title = "";
            if(this->flag_trackRecently[1] == true){
                title = "최신 트랙";
                this->widget_trackRecently = new QWidget();
                this->widget_trackRecently = this->setUIControl_subTitle_withSideBtn(title + QString(" (%1)").arg(this->list_trackRecently->at(0).total_count), "View All", BTN_IDX___track_recently, this->vBox_trackRecently);

                this->vBox_trackRecently->addSpacing(10);

                for(int i = 0; i < this->list_trackRecently->size(); i++){
                    this->home_track_recently[i]->setDataTrackInfo_Bugs(this->list_trackRecently->at(i), "Recently");
                    this->home_track_recently[i]->resize(1550, 70);

                    this->vBox_trackRecently->addWidget(this->home_track_recently[i]);
                }

                this->box_home_contents->addLayout(this->vBox_trackRecently);
                this->box_home_contents->addSpacing(45);

                /*if(this->flag_album_fav_chk == false){
                    this->album_idx_fav = 0;

                    // request HTTP API - get favorite for Rose Server
                    roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                    connect(proc_favCheck_track, &roseHome::ProcCommon::completeReq_rating_album, this, &QobuzExplore::slot_applyResult_getRating_album);
                    proc_favCheck_track->request_rose_getRating_Album("QOBUZ", QString("%1").arg(this->list_trackRecently->at(this->album_idx_fav).id));
                }*/
            }

            if(this->flag_albumRecently[1] == true){
                title = "최신 앨범";
                this->widget_albumRecently = new QWidget();
                this->widget_albumRecently = this->setUIControl_subTitle_withSideBtn(title + QString(" (%1)").arg(this->list_albumRecently->at(0).total_count), "View All", BTN_IDX___album_recently, this->vBox_albumRecently);

                this->vBox_albumRecently->addSpacing(10);

                this->hBox_albumRecently = this->setUIControl_hBoxLayout_forAlbum(this->vBox_albumRecently);

                for(int i = 0; i < this->list_albumRecently->size(); i++){
                    this->home_album_recently[i]->setData(this->list_albumRecently->at(i));
                    this->hBox_albumRecently->addWidget(this->home_album_recently[i]);
                }

                this->box_home_contents->addLayout(this->vBox_albumRecently);
                this->box_home_contents->addSpacing(45);
            }

            if(this->flag_videoRecently[1] == true){
                title = "최신 영상";
                this->widget_videoRecently = new QWidget();
                this->widget_videoRecently = this->setUIControl_subTitle_withSideBtn(title + QString(" (%1)").arg(this->list_videoRecently->at(0).total_count), "View All", BTN_IDX___video_recently, this->vBox_videoRecently);

                this->vBox_videoRecently->addSpacing(10);

                this->hBox_videoRecently = this->setUIControl_hBoxLayout_forVideo(this->vBox_videoRecently);

                for(int i = 0; i < this->list_videoRecently->size(); i++){
                    this->home_video_recently[i]->setData(this->list_videoRecently->at(i));
                    this->hBox_videoRecently->addWidget(this->home_video_recently[i]);
                }

                this->box_home_contents->addLayout(this->vBox_videoRecently);
                this->box_home_contents->addSpacing(45);
            }

            if(this->flag_trackBugsChart[1] == true){
                title = "벅스 트랙 차트";
                this->widget_trackBugsChart = new QWidget();
                this->widget_trackBugsChart = this->setUIControl_subTitle_withSideBtn(title + QString(" (%1)").arg(this->list_trackBugsChart->at(0).total_count), "View All", BTN_IDX___track_bugsChart, this->vBox_trackBugsChart);

                this->vBox_trackBugsChart->addSpacing(10);

                for(int i = 0; i < this->list_trackBugsChart->size(); i++){
                    this->home_track_bugsChart[i]->setDataTrackInfo_Bugs(this->list_trackBugsChart->at(i), "Chart");
                    this->home_track_bugsChart[i]->resize(1550, 70);

                    this->vBox_trackBugsChart->addWidget(this->home_track_bugsChart[i]);
                }

                this->box_home_contents->addLayout(this->vBox_trackBugsChart);
                this->box_home_contents->addSpacing(45);

                /*if(this->flag_album_fav_chk == false){
                    this->album_idx_fav = 0;

                    // request HTTP API - get favorite for Rose Server
                    roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                    connect(proc_favCheck_track, &roseHome::ProcCommon::completeReq_rating_album, this, &QobuzExplore::slot_applyResult_getRating_album);
                    proc_favCheck_track->request_rose_getRating_Album("QOBUZ", QString("%1").arg(this->list_trackBugsChart->at(this->album_idx_fav).id));
                }*/
            }

            if(this->flag_albumBugsChart[1] == true){
                title = "벅스 앨범 차트";
                this->widget_albumBugsChart = new QWidget();
                this->widget_albumBugsChart = this->setUIControl_subTitle_withSideBtn(title + QString(" (%1)").arg(this->list_albumBugsChart->at(0).total_count), "View All", BTN_IDX___album_bugsChart, this->vBox_albumBugsChart);

                this->vBox_albumBugsChart->addSpacing(10);

                this->hBox_albumBugsChart = this->setUIControl_hBoxLayout_forAlbum(this->vBox_albumBugsChart);

                for(int i = 0; i < this->list_albumBugsChart->size(); i++){
                    this->home_album_bugsChart[i]->setData(this->list_albumBugsChart->at(i));
                    this->hBox_albumBugsChart->addWidget(this->home_album_bugsChart[i]);
                }

                this->box_home_contents->addLayout(this->vBox_albumBugsChart);
                this->box_home_contents->addSpacing(45);
            }

            if(this->flag_videoBugsChart[1] == true){
                title = "벅스 영상 차트";
                this->widget_videoBugsChart = new QWidget();
                this->widget_videoBugsChart = this->setUIControl_subTitle_withSideBtn(title + QString(" (%1)").arg(this->list_videoBugsChart->at(0).total_count), "View All", BTN_IDX___video_bugsChart, this->vBox_videoBugsChart);

                this->vBox_videoBugsChart->addSpacing(10);

                this->hBox_videoBugsChart = this->setUIControl_hBoxLayout_forVideo(this->vBox_videoBugsChart);

                for(int i = 0; i < this->list_videoBugsChart->size(); i++){
                    this->home_video_bugsChart[i]->setData(this->list_videoBugsChart->at(i));
                    this->hBox_videoBugsChart->addWidget(this->home_video_bugsChart[i]);
                }

                this->box_home_contents->addLayout(this->vBox_videoBugsChart);
                this->box_home_contents->addSpacing(60);
            }
        }
    }


    QWidget* BugsHome::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QWidget* BugsHome::setUIControl_subTitle_withSubMenu(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QHBoxLayout* BugsHome::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

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
     * @brief Video 전용으로, 1 Row 형태의 가로스크롤 배치하는 Layout 구조 생성 후 반환
     * @return
     */
    QHBoxLayout* BugsHome::setUIControl_hBoxLayout_forVideo(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------
        QHBoxLayout *hBox_video = new QHBoxLayout();
        hBox_video->setSpacing(0);
        hBox_video->setContentsMargins(0, 0, 0, 0);
        hBox_video->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_contentSub = new QWidget();
        widget_contentSub->setLayout(hBox_video);
        widget_contentSub->setContentsMargins(0, 0, 0, 0);


        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_contentSub);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0, 0, 0 ,0);
        tmp_scrollArea->setFixedHeight(275);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_video;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void BugsHome::slot_applyResult_trackRecently(const QList<bugs::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_trackRecently->append(list_data);
            this->jsonArr_recently_toPlay = jsonArr_dataToPlay;
            this->flag_trackRecently[1] = true;
        }

        this->flag_trackRecently[0] = true;
        this->setUIControl_appendWidget();
    }


    void BugsHome::slot_applyResult_albumRecently(const QList<bugs::AlbumItemData> &list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_albumRecently->append(list_data);
            this->flag_albumRecently[1] = true;
        }

        this->flag_albumRecently[0] = true;
        this->setUIControl_appendWidget();
    }


    void BugsHome::slot_applyResult_videoRecently(const QList<bugs::VideoItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_videoRecently->append(list_data);
            this->video_recently_toPlay = jsonArr_dataToPlay;
            this->flag_videoRecently[1] = true;
        }

        this->flag_videoRecently[0] = true;
        this->setUIControl_appendWidget();
    }


    void BugsHome::slot_applyResult_trackBugsChart(const QList<bugs::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_trackBugsChart->append(list_data);
            this->jsonArr_bugsChart_toPlay = jsonArr_dataToPlay;
            this->flag_trackBugsChart[1] = true;
        }

        this->flag_trackBugsChart[0] = true;
        this->setUIControl_appendWidget();
    }


    void BugsHome::slot_applyResult_albumBugsChart(const QList<bugs::AlbumItemData> &list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_albumBugsChart->append(list_data);
            this->flag_albumBugsChart[1] = true;
        }

        this->flag_albumBugsChart[0] = true;
        this->setUIControl_appendWidget();
    }


    void BugsHome::slot_applyResult_videoBugsChart(const QList<bugs::VideoItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_videoBugsChart->append(list_data);
            this->video_bugsChart_toPlay = jsonArr_dataToPlay;
            this->flag_videoBugsChart[1] = true;
        }

        this->flag_videoBugsChart[0] = true;
        this->setUIControl_appendWidget();
        this->slot_hide_msg();
    }


    void BugsHome::slot_applyResult_recentlyAlbum(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_recentlyAlbum->append(list_data);
            this->flag_album[1] = true;
        }

        this->flag_album[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void BugsHome::slot_applyResult_recentlyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_recentlyPlaylist->append(list_data);
            this->flag_playlist[1] = true;
        }

        this->flag_playlist[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void BugsHome::slot_applyResult_recentlyTrack(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_recentlytrack->append(list_data);
            this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;
            this->flag_track[1] = true;
        }

        this->flag_track[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void BugsHome::slot_applyResult_myPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_myPlaylist->append(list_data);
            this->flag_myPlaylist[1] = true;
        }

        this->flag_myPlaylist[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void BugsHome::slot_applyResult_userPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_userPlaylist->append(list_data);
            this->flag_userPlaylist[1] = true;
        }

        this->flag_userPlaylist[0] = true;
        this->setUIControl_appendWidget_rose();
        this->slot_hide_msg();
    }


    void BugsHome::slot_applyResult_getRating_track(const QJsonArray &contents){

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


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Functions
    //
    //-----------------------------------------------------------------------------------------------------------------------

    // 서브 타이틀별 우측 View All -------------------------------------------------------------------------------------------

    /**
     * @brief 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void BugsHome::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_recentlyPlay){
            if(this->contentStep == ALBTAB_STEP_ALBUM){
                QString title = "최근 재생 앨범";

                QJsonObject tmp_data;
                if(this->list_recentlyAlbum->size() > 0){
                    tmp_data.insert("title", QString(title + " (%1)").arg(this->list_recentlyAlbum->at(0).totalCount));
                }
                else{
                    tmp_data.insert("title", QString(title + " (0)"));
                }
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_RECENTLY_ALBUM_ALL_LIST);
                emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
            }
            else if(this->contentStep == ALBTAB_STEP_PLAYLIST){
                QString title = "최근 재생 플레이리스트";

                QJsonObject tmp_data;
                if(this->list_recentlyPlaylist->size() > 0){
                    tmp_data.insert("title", QString(title + " (%1)").arg(this->list_recentlyPlaylist->at(0).totalCount));
                }
                else{
                    tmp_data.insert("title", QString(title + " (0)"));
                }
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_RECENTLY_PLAYLIST_ALL_LIST);
                emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
            }
        }
        else if(btnId == BTN_IDX_SUBTITLE_recentlyTrack){
            QString title = "최근 재생 트랙";

            QJsonObject tmp_data;
            if(this->list_recentlytrack->size() > 0){
                tmp_data.insert("title", QString(title + " (%1)").arg(this->list_recentlytrack->at(0).totalCount));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_RECENTLY_TRACK_ALL_LIST);
            emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_myPlaylists){
            QString title = "My 플레이리스트";

            QJsonObject tmp_data;
            if(this->list_myPlaylist->size() > 0){
                tmp_data.insert("title", QString(title + " (%1)").arg(this->list_myPlaylist->at(0).totalCount));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_ROSE_PLAYLIST_ALL_LIST);
            emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_userPlaylists){
            QString title = "BUGS Playlist On Rose";

            QJsonObject tmp_data;
            if(this->list_userPlaylist->size() > 0){
                tmp_data.insert("title", QString(title + " (%1)").arg(this->list_userPlaylist->at(0).totalCount));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS__USER_ROSE_PLAYLIST_ALL_LIST);
            emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX___track_recently){
            QString title = "최신 트랙";

            QJsonObject tmp_data;
            if(this->list_trackRecently->size() > 0){
                tmp_data.insert("title", QString(title + " (%1)").arg(this->list_trackRecently->at(0).total_count));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS___HOME_TRACK_RECENTLY);
            emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX___album_recently){
            QString title = "최신 앨범";

            QJsonObject tmp_data;
            if(this->list_albumRecently->size() > 0){
                tmp_data.insert("title", QString(title + " (%1)").arg(this->list_albumRecently->at(0).total_count));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS___HOME_ALBUM_RECENTLY);
            emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX___video_recently){
            QString title = "최신 영상";

            QJsonObject tmp_data;
            if(this->list_videoRecently->size() > 0){
                tmp_data.insert("title", QString(title + " (%1)").arg(this->list_videoRecently->at(0).total_count));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS___HOME_VIDEO_RECENTLY);
            emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX___track_bugsChart){
            QString title = "벅스 트랙 차트";

            QJsonObject tmp_data;
            if(this->list_trackBugsChart->size() > 0){
                tmp_data.insert("title", QString(title + " (%1)").arg(this->list_trackBugsChart->at(0).total_count));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS___HOME_TRACK_CHART);
            emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX___album_bugsChart){
            QString title = "벅스 앨범 차트";

            QJsonObject tmp_data;
            if(this->list_albumBugsChart->size() > 0){
                tmp_data.insert("title", QString(title + " (%1)").arg(this->list_albumBugsChart->at(0).total_count));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS___HOME_ALBUM_CHART);
            emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX___video_bugsChart){
            QString title = "벅스 영상 차트";

            QJsonObject tmp_data;
            if(this->list_videoBugsChart->size() > 0){
                tmp_data.insert("title", QString(title + " (%1)").arg(this->list_videoBugsChart->at(0).total_count));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS___HOME_VIDEO_CHART);
            emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
        }
    }


    void BugsHome::changedOnlyTabUI_notSendSignal(QString p_step){

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


    void BugsHome::slot_changedSubTabUI(const QJsonObject &p_data){

        QString tmp_step = p_data[KEY_PAGE_CODE].toString();

        // 페이지 변경 및 데이터 세팅
        this->changedOnlyTabUI_notSendSignal(tmp_step);
    }


    void BugsHome::slot_hide_msg(){

        ContentLoadingwaitingMsgHide();

        if(global.enable_message_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;
        }
    }


    void BugsHome::slot_time_out(){

        ContentLoadingwaitingMsgHide();

        if(global.enable_message_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;

            this->page = "";
            this->flagNeedReload = false;

            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS___SETTING);

            global.user_forBugs.set_logoutState();

            ProcRoseAPI_withBugs *procRose = new ProcRoseAPI_withBugs(this);

            bugs::RoseSessionInfo_forBugs sessionInfo;
            sessionInfo.BUGS_ProductDisplayEndDt = "";
            sessionInfo.BUGS_StreamingYn = false;
            sessionInfo.BUGS_ExpiresIn = "";
            sessionInfo.BUGS_ProductName = "";
            sessionInfo.BUGS_Nickname = "";

            sessionInfo.BUGS_RefreshToken = "";
            sessionInfo.BUGS_TokenType = "";
            sessionInfo.BUGS_AccessToken = "";
            sessionInfo.BUGS_FlacPremiumYn = false;
            sessionInfo.BUGS_AdultYn = false;

            sessionInfo.BUGS_Scope = "";

            procRose->request_set_session_info(sessionInfo);
            ToastMsg::show(this,"", tr("The subscription has expired. Please log in again."), 3000, 0);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedMovePage(tmp_data);
        }
    }


    void BugsHome::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_BUGS)){

            global.user.setDeviceChange(false);

            //this->applyLoginSuccessState();
            if(this->is_bugs_logined()){

                this->page = "";
                this->flagNeedReload = false;

                QJsonObject tmpPage = QJsonObject();
                tmpPage.insert("pageCode", "BUGS__HOME");
                this->setJsonObject_forData(tmpPage);
                this->setActivePage();
            }
            else{

                this->page = "";
                this->flagNeedReload = false;

                if(global.enable_section_left == true){
                    global.enable_section_left = false;
                }

                QJsonObject tmp_data;
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS___SETTING);

                // 부모에게 페이지 변경하라고 시그널 보냄
                emit this->signal_clickedMovePage(tmp_data);
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void BugsHome::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        if(section == SECTION_FOR_MORE_POPUP___recentlyAlbum){
            roseHome::AlbumItemData data = this->list_recentlyAlbum->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Album 바로 재생
                ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byAlbumID(data.clientKey.toUInt(), OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Album Detail 페이지 진입
                QJsonObject jsonObj_move = QJsonObject();
                jsonObj_move.insert("album_id", data.clientKey.toInt());
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ALBUM_DETAIL);

                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptionPopup 띄우기 필요
                print_debug();

                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___album_recently){

            // ClickMode 별로 처리
            this->proc_clicked_itemAlbum(this->list_albumRecently, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___album_bugsChart){

            // ClickMode 별로 처리
            this->proc_clicked_itemAlbum(this->list_albumBugsChart, clickMode, index, section);
        }
    }


    void BugsHome::slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        if(section == SECTION_FOR_MORE_POPUP___video_recently){

            // ClickMode 별로 처리
            this->proc_clicked_itemVideo(this->list_videoRecently, this->video_recently_toPlay, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___video_bugsChart){

            // ClickMode 별로 처리
            this->proc_clicked_itemVideo(this->list_videoBugsChart, this->video_bugsChart_toPlay, clickMode, index, section);
        }
    }


    void BugsHome::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        if(section == SECTION_FOR_MORE_POPUP___recentlyPlaylist){
            roseHome::PlaylistItemData data = this->list_recentlyPlaylist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                if(this->list_recentlyPlaylist->at(index).type == "BUGS" && this->list_recentlyPlaylist->at(index).isRose == false){
                    // PD Album 바로 재생
                    bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
                    procRosePlay->requestPlayRose_byPD_AlbumID(this->list_recentlyPlaylist->at(index).clientKey.toInt(), OptMorePopup::ClickMode::Play_RightNow);
                }
                else{
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byPlaylistID(this->list_recentlyPlaylist->at(index).id, OptMorePopup::ClickMode::Play_RightNow);
                }
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입
                if(data.isRose == false){
                    if(data.clientKey.isEmpty()){
                        QJsonObject jsonObj_move = QJsonObject();
                        jsonObj_move.insert("id", data.id);
                        jsonObj_move.insert("ownerId", data.ownerId);
                        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_ROSE_PLAYLIST_DETAIL);

                        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                    }
                    else{
                        QJsonObject jsonObj_move = QJsonObject();
                        jsonObj_move.insert("es_album_id", data.clientKey.toInt());
                        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___PD_ALBUM_DETAIL);

                        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                    }
                }
                else if(data.isRose == true){
                    QJsonObject jsonObj_move = QJsonObject();
                    jsonObj_move.insert("id", data.id);
                    jsonObj_move.insert("ownerId", data.ownerId);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_ROSE_PLAYLIST_DETAIL);

                    emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                }
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptionPopup 띄우기 필요
                print_debug();


                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___myPlaylists){
            roseHome::PlaylistItemData data = this->list_myPlaylist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입
                QJsonObject jsonObj_move = QJsonObject();
                jsonObj_move.insert("id", data.id);
                jsonObj_move.insert("ownerId", data.ownerId);
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_ROSE_PLAYLIST_DETAIL);

                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptionPopup 띄우기 필요
                print_debug();

                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___userPlaylists){
            roseHome::PlaylistItemData data = this->list_userPlaylist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입
                QJsonObject jsonObj_move = QJsonObject();
                jsonObj_move.insert("id", data.id);
                jsonObj_move.insert("ownerId", data.ownerId);
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_ROSE_PLAYLIST_DETAIL);

                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptionPopup 띄우기 필요
                print_debug();

                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
            }
        }
    }


    void BugsHome::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        QString obj_name = sender()->objectName();

        if(obj_name == "recently_tracks"){
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

                    if(this->track_star_fav == 0 || this->track_star_fav == 1){
                        // Bugs Favorite toggle
                        //int id = this->list_recentlytrack->at(idx).id;

                        //ProcCommon *proc = new ProcCommon(this);
                        //connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &BugsRecentlyTrackAll::slot_qobuz_completeReq_listAll_myFavoritesIds);
                        //proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                        //this->flag_send_track = true;
                    }

                    this->track_idx_fav = idx;

                    QJsonObject ratingInfo = QJsonObject();
                    ratingInfo.insert("favorite", this->flag_track_fav);
                    ratingInfo.insert("star", this->track_star_fav);
                    ratingInfo.insert("thumbup", false);
                    ratingInfo.insert("type", this->list_recentlytrack->at(idx).type);

                    QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

                    QJsonObject jsonObj = QJsonObject();
                    jsonObj.insert("ratingInfo", ratingInfo);
                    jsonObj.insert("track", track);

                    // request HTTP API - get favorite for Rose Server
                    roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                    connect(proc_favCheck_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsHome::slot_applyResult_getRating_track);
                    proc_favCheck_track->request_rose_setRating_Track(jsonObj, this->flag_track_fav, this->track_star_fav);

                    this->flag_check_track = true;
                }
            }
            else{
                // ClickMode 별로 처리
                roseHome::TrackItemData data = this->list_recentlytrack->at(idx);

                if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AllBox){
                    // Track 바로 재생 - clicked 항목만 재생
                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::Play_RightNow_withReorder;

                    if(global.device.getMusicPlayType() == 15){
                        playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    }
                    else if(global.device.getMusicPlayType() == 13){
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                    }
                    else if(global.device.getMusicPlayType() == 12){
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                    }
                    else if(global.device.getMusicPlayType() == 16){
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                    }
                    else if(global.device.getMusicPlayType() == 17){
                        playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                    }
                    else if(global.device.getMusicPlayType() == 18){
                        playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                    }
                    else if(global.device.getMusicPlayType() == 19){
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                    }

                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byTracks(this->list_recentlytrack->at(idx), this->jsonArr_tracks_toPlay, idx, playType);
                }
                else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
                }
                else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){

                    //---------------------------------//c220825_1
                    print_debug();
                    QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();
                    qDebug() << "track==" << track;

                    QJsonObject track_data = ProcJsonEasy::getJsonObject(track, "data");
                    QJsonObject track_artist = ProcJsonEasy::getJsonObject(track, "artists");
                    QString artist_nm = ProcJsonEasy::getString(track_artist, "artist_nm");
                    //qDebug() << "\ntrack_album==" << track_album;
                    QString trackId = ProcJsonEasy::getString(track_data, "track_id");

                    QString title = ProcJsonEasy::getString(track_data, "track_title");
                    QString thumbnailUrl = ProcJsonEasy::getString(track, "thumbnailUrl");
                    QString artist = ProcJsonEasy::getString(track, "ownerName");
                    qDebug() << "\n trackId==" << trackId;
                    qDebug() << "thumbnailUrl==" << thumbnailUrl;
                    qDebug() << "artist==" << artist_nm;
                    qDebug() << "title==" << title;


                    // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Rosehome_Track, roseHome::ConvertData::getConvertOptHeaderData(data), idx, SECTION_FOR_MORE_POPUP___recentlyTrack);
                }
            }
        }
        else if(obj_name == "track_recently"){
            if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            }
            else{
                this->proc_clicked_itemTrack_inList(this->list_trackRecently, this->jsonArr_recently_toPlay, clickMode, idx, SECTION_FOR_MORE_POPUP___track_recently);
            }
        }
        else if(obj_name == "track_bugsChart"){
            if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            }
            else{
                this->proc_clicked_itemTrack_inList(this->list_trackBugsChart, this->jsonArr_bugsChart_toPlay, clickMode, idx, SECTION_FOR_MORE_POPUP___track_bugsChart);
            }
        }
    }


    void BugsHome::slot_applyResult_getShareLink(const QString &link){//c220825_1

        this->shareLink = link;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] OptMorePopup (더보기팝업에서 발생한 signal 처리)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void BugsHome::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___recentlyAlbum){
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                // Rose Play 요청
                bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byAlbumID(this->list_recentlyAlbum->at(index).clientKey.toInt(), clickMode);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___recentlyPlaylist){
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                if(this->list_recentlyPlaylist->at(index).type == "BUGS" && this->list_recentlyPlaylist->at(index).isRose == false){
                    bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
                    procRosePlay->requestPlayRose_byPD_AlbumID(this->list_recentlyPlaylist->at(index).clientKey.toInt(), clickMode);
                }
                else{
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byPlaylistID(this->list_recentlyPlaylist->at(index).id, clickMode);
                }
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___myPlaylists){
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(this->list_myPlaylist->at(index).id, clickMode);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___userPlaylists){
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();

                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(this->list_userPlaylist->at(index).id, clickMode);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___recentlyTrack){
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

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
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(this->list_recentlytrack->at(index), this->jsonArr_tracks_toPlay, index, clickMode);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___track_recently){
            this->proc_clicked_optMorePopup_fromTrack(this->list_trackRecently, this->jsonArr_recently_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___album_recently){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_albumRecently, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___video_recently){
            this->proc_clicked_optMorePopup_fromVideo(this->list_videoRecently, this->video_recently_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___track_bugsChart){
            this->proc_clicked_optMorePopup_fromTrack(this->list_trackBugsChart, this->jsonArr_bugsChart_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___album_bugsChart){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_albumBugsChart, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___video_bugsChart){
            this->proc_clicked_optMorePopup_fromVideo(this->list_videoBugsChart, this->video_bugsChart_toPlay, index, clickMode);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void BugsHome::setUIControl_logined(){

        /*// 최신 음악
        this->addUIControl_subTitle_withSideBtn_common(tr("New Music"), tr("View All"), BTN_IDX___music_recently);
        this->hBox_musicRecently = this->get_addUIControl_hBoxLayout_forMusic();

        // 벅스 차트
        this->addUIControl_subTitle_withSideBtn_common(tr("Bugs Chart"), tr("View All"), BTN_IDX___music_bugsChart);
        this->delegate = new BugsTrackDelegate(SECTION_FOR_MORE_POPUP___music_bugsChart, BugsTrackDelegate::Bugs_Track_withRank, this);
        connect(delegate, &BugsTrackDelegate::clicked, this, &BugsHome::slot_clickedItemTrack_inList);
        this->listWidget_music_bugsChart = this->get_addUIControl_ListWidget(this->delegate);*/
    }






    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
         * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의
         * @param btnId
         */
    void BugsHome::slot_clickBtn_subTitle_side(const int btnId){

        if(btnId == BTN_IDX___music_recently){
            // 최신 음악 - All View 이동
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___HOME_MUSIC_RECENTLY);
            emit this->signal_clickedMovePage(jsonObj_move);                    // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX___music_bugsChart){
            // 벅스 차트 - All View 이동
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___HOME_MUSIC_CHART);
            emit this->signal_clickedMovePage(jsonObj_move);                    // 페이지 이동 signal
        }

    }

    /**
         * @brief BugsHome::slot_applyResult_tracks
         * @param list_data
         * @param jsonObj_track
         */
    void BugsHome::slot_applyResult_tracks(const QList<bugs::TrackItemData>& list_data, const QJsonArray& jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        int section = sender()->property("section").toInt();
        if(section == SECTION_FOR_MORE_POPUP___music_recently){
            this->flagReq_track_musicRecently = false;
            this->jsonArr_tracks_toPlay_musicRecently = jsonArr_dataToPlay;
            this->list_track_musicRecently->append(list_data);
            this->createList_itemTrackSqure_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_musicRecently, SECTION_FOR_MORE_POPUP___music_recently);
        }
        else if(section == SECTION_FOR_MORE_POPUP___music_bugsChart){
            this->flagReq_track_bugsChart = false;
            this->jsonArr_tracks_toPlay_bugsChart = jsonArr_dataToPlay;
            this->list_track_bugsChart->append(list_data);
            this->createList_itemTrackDelegate_applyingWithData_showImage(list_data, this->listWidget_music_bugsChart, 0, section, true);
        }

        if(global.enable_section_left == true){
            global.enable_section_left = false;
        }
    }


    void BugsHome::slot_applyResult_tracks2(const QList<bugs::TrackItemData>& list_data, const QJsonArray& jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        int section = sender()->property("section").toInt();
        print_debug();
        qDebug() << "global.searchMenuFlag=" << global.searchMenuFlag;
        //if(!global.searchMenuFlag){//cheon211008
        if(section == SECTION_FOR_MORE_POPUP___music_recently){
            this->flagReq_track_musicRecently = false;
            this->jsonArr_tracks_toPlay_musicRecently = jsonArr_dataToPlay;
            this->list_track_musicRecently->append(list_data);
            this->createList_itemTrackSqure_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_musicRecently, SECTION_FOR_MORE_POPUP___music_recently);
        }
        else if(section == SECTION_FOR_MORE_POPUP___music_bugsChart){
            this->flagReq_track_bugsChart = false;
            this->jsonArr_tracks_toPlay_bugsChart = jsonArr_dataToPlay;
            this->list_track_bugsChart->append(list_data);
            this->createList_itemTrackDelegate_applyingWithData_showImage(list_data, this->listWidget_music_bugsChart, 0, section, true);
        }
        //}else{//cheon211008

        //}
        //if(global.searchMenuFlag){//cheon211115-01
        //    print_debug();
        //   emit linker->signal_menumoveBugs();
        //}
    }

    /**
         * @brief [Slot] ListWidgetItem 관련 - 썸네일 다운로드 완료
         */
    void BugsHome::slot_thumbnailDownloaded_fotListWidget_delegate(){

        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());
        int section = fileDownloader_sender->property("section").toInt();
        if(section == SECTION_FOR_MORE_POPUP___music_bugsChart){
            this->applyImageData_itemTrackDelegate_withFileDownload(fileDownloader_sender, this->listWidget_music_bugsChart, this->list_track_bugsChart);
        }
    }


    /**
         * @brief BugsHome::slot_clickedItemTrackSqure
         */
    void BugsHome::slot_clickedItemTrackSqure(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___music_recently){
            this->proc_clicked_itemTrackSquare(this->list_track_musicRecently, this->jsonArr_tracks_toPlay_musicRecently, clickMode, index, section);
        }

    }

    /**
         * @brief [slot] TrackDelegate로부터 signal 받은
         * @param index
         * @param clickMode
         */
    /*void BugsHome::slot_clickedItemTrack_inList(const int idx, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode){

        int section = ((tidal::AbstractTidalTrackDelegate*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___music_bugsChart){
            this->proc_clicked_itemTrack_inList(this->list_track_bugsChart, this->jsonArr_tracks_toPlay_bugsChart, clickMode, idx, section);
        }
    }*/


    /**
         * @brief Track에 대해서 Favorite Add/Delete의 성공 처리 - Track에 대해서 Fav 처리 결과를 반영함 (Data & UI)
         * @param jsonObj_opt
         */
    void BugsHome::slot_completedReg_favorite_track(const QJsonObject& jsonObj_opt){

        int section = ProcJsonEasy::getInt(jsonObj_opt, "section");
        int index = ProcJsonEasy::getInt(jsonObj_opt, "index");

        if(section == SECTION_FOR_MORE_POPUP___music_recently){
            // Update Data
            bugs::TrackItemData data_track = this->list_track_musicRecently->at(index);
            data_track.likes_yn = ProcJsonEasy::getBool(jsonObj_opt, "likes_yn");
            this->list_track_musicRecently->replace(index, data_track);
        }
        else if(section == SECTION_FOR_MORE_POPUP___music_bugsChart){
            // Update Data
            bugs::TrackItemData data_track = this->list_track_bugsChart->at(index);
            data_track.likes_yn = ProcJsonEasy::getBool(jsonObj_opt, "likes_yn");
            this->list_track_bugsChart->replace(index, data_track);
            // Update ListWidget
            this->listWidget_music_bugsChart->item(index)->setData(Qt::UserRole, ConvertData_forBugs::getObjectJson_trackData(data_track));
        }
    }


    /**
         * @brief BugsHome::proc_preUpdate_favorite_track
         * @param data_itemPos
         */
    void BugsHome::proc_preUpdate_favorite_track(const ItemPositionData& data_itemPos){

        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___music_recently){
            // Update Data
            bugs::TrackItemData data_track = this->list_track_musicRecently->at(data_itemPos.index);
            data_track.likes_yn = data_itemPos.likes_yn;
            this->list_track_musicRecently->replace(data_itemPos.index, data_track);
        }
        else if(data_itemPos.section == SECTION_FOR_MORE_POPUP___music_bugsChart){
            // Update Data
            bugs::TrackItemData data_track = this->list_track_bugsChart->at(data_itemPos.index);
            data_track.likes_yn = data_itemPos.likes_yn;
            this->list_track_bugsChart->replace(data_itemPos.index, data_track);

            // Update ListWidget
            this->listWidget_music_bugsChart->item(data_itemPos.index)->setData(Qt::UserRole, ConvertData_forBugs::getObjectJson_trackData(data_track));
        }

    }

    void BugsHome::slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj){//c220729

        //print_debug();
        //QJsonDocument doc(p_jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"SearchRemoteBar::slot_responseHttp---" << strJson;


    }

    void BugsHome::slot_dialog_share_link(QString link){//c220902_1
        this->shareLink = link;
        setUIShare();
        qDebug() << "this->shareLink="<<this->shareLink;
    }

    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] OptMorePopup (더보기팝업에서 발생한 signal 처리)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
         * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
         * @param clickMode
         * @param index
         * @param section
         */
    /*void BugsHome::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___music_recently){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track_musicRecently, this->jsonArr_tracks_toPlay_musicRecently, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___music_bugsChart){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track_bugsChart, this->jsonArr_tracks_toPlay_bugsChart, index, clickMode);
        }
    }*/

}

