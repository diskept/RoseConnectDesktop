#include "tidal/TidalHome.h"

#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"
#include "tidal/ProcRosePlay_withTidal.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/ProcJsonEasy.h"
#include "common/rosesettings.h"

#include "widget/NoData_Widget.h"

#include "widget/toastmsg.h"//c220729
#include "common/networkhttp.h"//c220729


#include <QDebug>
#include <QScroller>
#include <QSettings>


namespace tidal {

    // subTitle의 사이드 버튼의 btnId 로 사용
    //const int BTN_IDX_NONE= 0;
    const int BTN_IDX_MIXES_TRACK = 1;
    const int BTN_IDX_MIXES_VIDEO = 2;
    const int BTN_IDX_SUGGESTED_NEW_ALBUM = 3;
    const int BTN_IDX_SUGGESTED_TOP_ALBUM = 4;
    const int BTN_IDX_MASTER_PLAYLIST = 5;
    const int BTN_IDX_MASTER_ALBUM = 6;
    const int BTN_IDX_MOODS_FOCUS = 7;
    const int BTN_IDX_RISING_NEW = 8;
    const int BTN_IDX_VISUAL_ALBUM = 9;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___mixes_track = 1;
    const int SECTION_FOR_MORE_POPUP___mixes_video = 2;
    const int SECTION_FOR_MORE_POPUP___suggested_albums = 3;
    const int SECTION_FOR_MORE_POPUP___suggested_top = 4;
    const int SECTION_FOR_MORE_POPUP___master_playlists = 5;
    const int SECTION_FOR_MORE_POPUP___master_albums = 6;
    const int SECTION_FOR_MORE_POPUP___focus_playlists = 7;
    const int SECTION_FOR_MORE_POPUP___rising_playlists = 8;
    const int SECTION_FOR_MORE_POPUP___visual_album = 9;

    const int GET_ITEM_SIZE___ONCE_FOR_MAIN = 15;


    // TIDAL API 관련
    QString TIDAL_API_MIXES_TRACK = "mixes/daily/track";
    QString TIDAL_API_MIXES_VIDEO = "mixes/daily/video";
    QString TIDAL_API_SUGGESTED_ALBUM = "featured/new/albums";
    QString TIDAL_API_SUGGESTED_TOP = "featured/top/albums";
    QString TIDAL_API_MASTER_PLAYLIST = "master/recommended/playlists";
    QString TIDAL_API_MASTER_ALBUM = "master/recommended/albums";
    QString TIDAL_API_MOODS_FOCUS = "moods/concentrate/playlists";
    QString TIDAL_API_RISING_NEW = "rising/new/playlists";
    QString TIDAL_API_VISUAL_ALBUM= "visual album";


    // ROSE subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX_SUBTITLE_recentlyPlay = 10;
    const int BTN_IDX_SUBTITLE_recentlyTrack = 11;
    const int BTN_IDX_SUBTITLE_myPlaylists = 12;
    const int BTN_IDX_SUBTITLE_userPlaylists = 13;

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
     * @brief "TIDAL > 홈" 화면을 위한 생성자. @see PAGECODE_T_HOME
     * @param parent
     */
    TidalHome::TidalHome(QWidget *parent) : AbstractTidalSubWidget(VerticalScroll_filter, parent) {

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));
        connect(linker, SIGNAL(signal_message_show_timeout()), SLOT(slot_time_out()));


        if(global.user_forTidal.isLogined() == true)
        {
            // data
            this->list_mixesTrack = new QList<tidal::PlaylistItemData>();
            this->list_mixesVideo = new QList<tidal::PlaylistItemData>();
            this->list_albumSuggested = new QList<tidal::AlbumItemData>();
            this->list_albumSuggested_Top = new QList<tidal::AlbumItemData>();
            this->list_playlistMaster = new QList<tidal::PlaylistItemData>();
            this->list_albumMaster = new QList<tidal::AlbumItemData>();
            this->list_playlistFocus = new QList<tidal::PlaylistItemData>();
            this->list_playlistRising = new QList<tidal::PlaylistItemData>();
            this->list_visualAlbum = new QList<tidal::AlbumItemData>();
            this->list_visualAlbum_viewall = new QList<tidal::AlbumItemData>();

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
    }


    /**
     * @brief 소멸자.
     */
    TidalHome::~TidalHome(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalHome::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            if(this->flag_mixesTrack[1] == true || this->flag_mixesVideo[1] == true || this->flag_suggestedAlbum[1] == true || this->flag_suggestedTOP[1] == true ||
                this->flag_masterPlaylist[1] == true || flag_masterAlbum[1] == true || flag_focusplaylist[1] == true || flag_risingplaylist[1] == true || flag_visualAlbum[1] == true){
                this->widget_rose_contents->hide();
                this->widget_home_contents->hide();
            }

            this->page = pageCode;

            // init data
            this->list_recentlyAlbum->clear();
            this->list_recentlyPlaylist->clear();
            this->list_recentlytrack->clear();
            this->list_myPlaylist->clear();
            this->list_userPlaylist->clear();

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

            // init data
            this->list_mixesTrack->clear();
            this->list_mixesVideo->clear();
            this->list_albumSuggested->clear();
            this->list_albumSuggested_Top->clear();
            this->list_playlistMaster->clear();
            this->list_albumMaster->clear();
            this->list_playlistFocus->clear();
            this->list_playlistRising->clear();
            this->list_visualAlbum->clear();

            this->flag_mixesTrack[0] = false;
            this->flag_mixesVideo[0] = false;
            this->flag_suggestedAlbum[0] = false;
            this->flag_suggestedTOP[0] = false;
            this->flag_masterPlaylist[0] = false;
            this->flag_masterAlbum[0] = false;
            this->flag_focusplaylist[0] = false;
            this->flag_risingplaylist[0] = false;
            this->flag_visualAlbum[0] = false;

            this->flag_mixesTrack[1] = false;
            this->flag_mixesVideo[1] = false;
            this->flag_suggestedAlbum[1] = false;
            this->flag_suggestedTOP[1] = false;
            this->flag_masterPlaylist[1] = false;
            this->flag_masterAlbum[1] = false;
            this->flag_focusplaylist[1] = false;
            this->flag_risingplaylist[1] = false;
            this->flag_visualAlbum[1] = false;
        }
    }


    /**
     * @brief TidalHome::setActivePage
     */

    void TidalHome::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            this->box_contents->removeWidget(this->widget_rose_contents);
            this->box_contents->removeWidget(this->widget_home_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->box_rose_contents = new QVBoxLayout();
            this->box_rose_contents->setSpacing(0);
            this->box_rose_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_rose_contents = new QWidget();
            this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_rose_contents->setLayout(this->box_rose_contents);

            this->box_contents->addWidget(this->widget_rose_contents, 0, Qt::AlignTop);

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

            //this->box_contents->addSpacing(100);

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

            this->vBox_mixesTrack = new QVBoxLayout();
            this->vBox_mixesVideo = new QVBoxLayout();
            this->vBox_suggestedAlbum = new QVBoxLayout();
            this->vBox_suggestedTOP = new QVBoxLayout();
            this->vBox_masterPlaylist = new QVBoxLayout();
            this->vBox_masterAlbum = new QVBoxLayout();
            this->vBox_focusplaylist = new QVBoxLayout();
            this->vBox_risingplaylist = new QVBoxLayout();
            this->vBox_visualAlbum = new QVBoxLayout();

            this->hBox_mixesTrack = new QHBoxLayout();
            this->hBox_mixesVideo = new QHBoxLayout();
            this->hBox_suggestedAlbum = new QHBoxLayout();
            this->hBox_suggestedTOP = new QHBoxLayout();
            this->hBox_masterPlaylist = new QHBoxLayout();
            this->hBox_masterAlbum = new QHBoxLayout();
            this->hBox_focusplaylist = new QHBoxLayout();
            this->hBox_risingplaylist = new QHBoxLayout();
            this->hBox_visualAlbum = new QHBoxLayout();

            // init UI
            GSCommon::clearLayout(this->vBox_recentlyPlay);
            GSCommon::clearLayout(this->vBox_recentlyTrack);
            GSCommon::clearLayout(this->vBox_myPlaylist);
            GSCommon::clearLayout(this->vBox_userPlaylist);

            GSCommon::clearLayout(this->hBox_recentlyAlbum);
            GSCommon::clearLayout(this->hBox_recentlyPlaylist);
            GSCommon::clearLayout(this->hBox_myPlaylist);
            GSCommon::clearLayout(this->hBox_userPlaylist);

            GSCommon::clearLayout(this->vBox_mixesTrack);
            GSCommon::clearLayout(this->vBox_mixesVideo);
            GSCommon::clearLayout(this->vBox_suggestedAlbum);
            GSCommon::clearLayout(this->vBox_suggestedTOP);
            GSCommon::clearLayout(this->vBox_masterPlaylist);
            GSCommon::clearLayout(this->vBox_masterAlbum);
            GSCommon::clearLayout(this->vBox_focusplaylist);
            GSCommon::clearLayout(this->vBox_risingplaylist);
            GSCommon::clearLayout(this->vBox_visualAlbum);

            GSCommon::clearLayout(this->hBox_mixesTrack);
            GSCommon::clearLayout(this->hBox_mixesVideo);
            GSCommon::clearLayout(this->hBox_suggestedAlbum);
            GSCommon::clearLayout(this->hBox_suggestedTOP);
            GSCommon::clearLayout(this->hBox_masterPlaylist);
            GSCommon::clearLayout(this->hBox_masterAlbum);
            GSCommon::clearLayout(this->hBox_focusplaylist);
            GSCommon::clearLayout(this->hBox_risingplaylist);
            GSCommon::clearLayout(this->hBox_visualAlbum);

            // sub Title UI
            for(int i = 0; i < 20; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            for(int i = 0; i < 15; i++){
                this->home_recently_album[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___recentlyAlbum, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                connect(this->home_recently_album[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &TidalHome::slot_clickedItemAlbum);
            }

            for(int i = 0; i < 15; i++){
                this->home_recently_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___recentlyPlaylist, tidal::AbstractItem::ImageSizeMode::Square_200x200, 2, true);
                connect(this->home_recently_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &TidalHome::slot_clickedItemPlaylist);
            }

            for (int i = 0; i < 5; i++) {
                this->home_recently_track[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->home_recently_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TidalHome::slot_clickedItemTrack_inList);
                this->home_recently_track[i]->setProperty("index", i);
                this->home_recently_track[i]->setObjectName("recently tracks");
            }

            for(int i = 0; i < 15; i++){
                this->home_myPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___myPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                connect(this->home_myPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &TidalHome::slot_clickedItemPlaylist);
            }

            for(int i = 0; i < 15; i++){
                this->home_userPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___userPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);
                connect(this->home_userPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &TidalHome::slot_clickedItemPlaylist);
            }


            for(int i = 0; i < 15; i++){
                this->home_mixesTrack[i] = new tidal::ItemPlaylist(i, SECTION_FOR_MORE_POPUP___mixes_track, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_mixesTrack[i], &tidal::ItemPlaylist::signal_clicked, this, &TidalHome::slot_clickedItemPlaylist);
            }

            for(int i = 0; i < 15; i++){
                this->home_mixesVideo[i] = new tidal::ItemPlaylist(i, SECTION_FOR_MORE_POPUP___mixes_video, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_mixesVideo[i], &tidal::ItemPlaylist::signal_clicked, this, &TidalHome::slot_clickedItemPlaylist);
            }

            for(int i = 0; i < 15; i++){
                this->home_suggestedAlbum[i] = new tidal::ItemAlbum(i, SECTION_FOR_MORE_POPUP___suggested_albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_suggestedAlbum[i], &tidal::ItemAlbum::signal_clicked, this, &TidalHome::slot_clickedItemAlbum);
            }

            for(int i = 0; i < 15; i++){
                this->home_suggestedTop[i] = new tidal::ItemAlbum(i, SECTION_FOR_MORE_POPUP___suggested_top, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_suggestedTop[i], &tidal::ItemAlbum::signal_clicked, this, &TidalHome::slot_clickedItemAlbum);
            }

            for(int i = 0; i < 15; i++){
                this->home_albumMaster[i] = new tidal::ItemAlbum(i, SECTION_FOR_MORE_POPUP___master_albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_albumMaster[i], &tidal::ItemAlbum::signal_clicked, this, &TidalHome::slot_clickedItemAlbum);
            }

            for(int i = 0; i < 15; i++){
                this->home_playlistMaster[i] = new tidal::ItemPlaylist(i, SECTION_FOR_MORE_POPUP___master_playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_playlistMaster[i], &tidal::ItemPlaylist::signal_clicked, this, &TidalHome::slot_clickedItemPlaylist);
            }

            for(int i = 0; i < 15; i++){
                this->home_playlistFocus[i] = new tidal::ItemPlaylist(i, SECTION_FOR_MORE_POPUP___focus_playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_playlistFocus[i], &tidal::ItemPlaylist::signal_clicked, this, &TidalHome::slot_clickedItemPlaylist);
            }

            for(int i = 0; i < 15; i++){
                this->home_playlistRising[i] = new tidal::ItemPlaylist(i, SECTION_FOR_MORE_POPUP___rising_playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_playlistRising[i], &tidal::ItemPlaylist::signal_clicked, this, &TidalHome::slot_clickedItemPlaylist);
            }

            for(int i = 0; i < 15; i++){
                this->home_visualAlbum[i] = new tidal::ItemAlbum(i, SECTION_FOR_MORE_POPUP___visual_album, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_visualAlbum[i], &tidal::ItemAlbum::signal_clicked, this, &TidalHome::slot_clickedItemAlbum);
            }

            if(global.updateCheckFlag){

                //ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223
                //QTimer::singleShot(10000, this, SLOT(slot_hide_msg()));

                if(global.enable_section_left == true){
                    global.enable_section_left = false;
                }
            }
            else{

                global.enable_message_count = 0;
                global.enable_message_flag = true;
                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223

                this->setUIControl_requestRose();
                this->setUIControl_requestTidal();
            }
        }
    }


    void TidalHome::slot_hide_msg(){

        ContentLoadingwaitingMsgHide();

        if(global.enable_message_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;
        }
    }


    void TidalHome::slot_time_out(){

        ContentLoadingwaitingMsgHide();

        if(global.enable_message_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;

            // page move
            if(global.user_forTidal.isLogined() == true){
                AbstractTidalSubWidget::slot_acceptedDialogLogin();
            }
            else if(global.user_forTidal.isLogined() == false){
                AbstractTidalSubWidget::slot_acceptedDialogLogout();
            }

            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_SETTINGS);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedMovePage(tmp_data);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------


    void TidalHome::setUIControl_requestRose(){

        if(!global.user.getAccess_token().isEmpty()){

            // request HTTP API for Recently Album
            roseHome::ProcCommon *proc_recentlyAlbum = new roseHome::ProcCommon(this);
            connect(proc_recentlyAlbum, &roseHome::ProcCommon::completeReq_list_albums, this, &TidalHome::slot_applyResult_recentlyAlbum);
            proc_recentlyAlbum->request_rose_getList_recentlyAlbums(ROSE_API_ALBUM_PATH , "TIDAL", 0, GET_ITEM_SIZE___ONCE_FOR_MAIN);

            // request HTTP API for Recently Playlist
            roseHome::ProcCommon *proc_recentlyPlaylist = new roseHome::ProcCommon(this);
            connect(proc_recentlyPlaylist, &roseHome::ProcCommon::completeReq_list_playlists, this, &TidalHome::slot_applyResult_recentlyPlaylist);
            proc_recentlyPlaylist->request_rose_getList_recentlyPlaylists(ROSE_API_PLAYLIST_PATH , "TIDAL", 0, GET_ITEM_SIZE___ONCE_FOR_MAIN);

            // request HTTP API for Recently Track
            roseHome::ProcCommon *proc_recentlyTrack = new roseHome::ProcCommon(this);
            connect(proc_recentlyTrack, &roseHome::ProcCommon::completeReq_list_tracks, this, &TidalHome::slot_applyResult_recentlyTrack);
            proc_recentlyTrack->request_rose_getList_recentlyTracks(ROSE_API_TRACK_PATH , "TIDAL", 0, 5);

            // request HTTP API for Recently Playlist
            roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
            connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &TidalHome::slot_applyResult_myPlaylist);
            proc_myPlaylist->request_rose_getList_myPlaylists(ROSE_API_MYPLAYLIST_PATH , "TIDAL", 0, GET_ITEM_SIZE___ONCE_FOR_MAIN);

            // request HTTP API for Recently Playlist
            roseHome::ProcCommon *proc_userPlaylist = new roseHome::ProcCommon(this);
            connect(proc_userPlaylist, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &TidalHome::slot_applyResult_userPlaylist);
            proc_userPlaylist->request_rose_getList_usersPlaylists(ROSE_API_USERPLAYLIST_PATH , "PLAYLIST_RECENT", "TIDAL", 0, GET_ITEM_SIZE___ONCE_FOR_MAIN);
        }
    }


    void TidalHome::setUIControl_requestTidal(){

        // request HTTP API for Mixes Track
        ProcCommon *proc_mixes = new ProcCommon(this);
        connect(proc_mixes, &ProcCommon::completeReq_list_mixes, this, &TidalHome::slot_applyResult_mixes_track);
        proc_mixes->request_tidal_getList_mixes(TIDAL_API_MIXES_TRACK, 0, 0);

        // request HTTP API for Mixes Track
        ProcCommon *proc_video = new ProcCommon(this);
        connect(proc_video, &ProcCommon::completeReq_list_mixes, this, &TidalHome::slot_applyResult_mixes_video);
        proc_video->request_tidal_getList_mixes(TIDAL_API_MIXES_VIDEO, 0, 0);

        // request HTTP API for Suggested New
        ProcCommon *proc_suggested = new ProcCommon(this);
        connect(proc_suggested, &ProcCommon::completeReq_list_albums, this, &TidalHome::slot_applyResult_albums_suggested);
        proc_suggested->request_tidal_getList_albums(TIDAL_API_SUGGESTED_ALBUM, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

        // request HTTP API for Suggested Top
        ProcCommon *proc_suggested_top = new ProcCommon(this);
        connect(proc_suggested_top, &ProcCommon::completeReq_list_albums, this, &TidalHome::slot_applyResult_albums_suggested_top);
        proc_suggested_top->request_tidal_getList_albums(TIDAL_API_SUGGESTED_TOP, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

        // request HTTP API for Master
        ProcCommon *proc_master_album = new ProcCommon(this);
        connect(proc_master_album, &ProcCommon::completeReq_list_albums, this, &TidalHome::slot_applyResult_albums_master);
        proc_master_album->request_tidal_getList_albums(TIDAL_API_MASTER_ALBUM, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

        ProcCommon *proc_master_playlist = new ProcCommon(this);
        connect(proc_master_playlist, &ProcCommon::completeReq_list_playlists, this, &TidalHome::slot_applyResult_playlists_master);
        proc_master_playlist->request_tidal_getList_playlists(TIDAL_API_MASTER_PLAYLIST, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

        // request HTTP API for Focus
        ProcCommon *proc_focus = new ProcCommon(this);
        connect(proc_focus, &ProcCommon::completeReq_list_playlists, this, &TidalHome::slot_applyResult_playlists_focus);
        proc_focus->request_tidal_getList_playlists(TIDAL_API_MOODS_FOCUS, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

        // request HTTP API for Rising
        ProcCommon *proc_rising = new ProcCommon(this);
        connect(proc_rising, &ProcCommon::completeReq_list_playlists, this, &TidalHome::slot_applyResult_playlists_rising);
        proc_rising->request_tidal_getList_playlists(TIDAL_API_RISING_NEW, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

        // request HTTP API for Suggested Top
        ProcCommon *proc_visual = new ProcCommon(this);
        connect(proc_visual, &ProcCommon::completeReq_list_visuals, this, &TidalHome::slot_applyResult_albums_visual);
        proc_visual->request_tidal_getList_visual(TIDAL_API_VISUAL_ALBUM, 50, 0);
    }


    void TidalHome::setUIControl_appendWidget_rose(){

        if(this->flag_album[0] == true && this->flag_playlist[0] == true && this->flag_track[0] == true && this->flag_myPlaylist[0] == true && this->flag_userPlaylist[0] == true)
        {
            if(this->flag_album[0] == true){

                this->widget_recentPlay = this->setUIControl_subTitle_withSubMenu(tr("Recently played"), "View All", BTN_IDX_SUBTITLE_recentlyPlay, this->vBox_recentlyPlay);

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
                    this->widget_myPlaylist = this->setUIControl_subTitle_withSideBtn(tr("My playlists") + QString(" (%1)").arg(this->list_myPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_myPlaylists, this->vBox_myPlaylist);
                }
                else{
                    this->widget_myPlaylist = this->setUIControl_subTitle_withSideBtn(tr("My playlists") + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_myPlaylists, this->vBox_myPlaylist);
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
                this->box_rose_contents->addSpacing(50);
            }

            if(this->flag_userPlaylist[0] == true){

                this->widget_userPlaylist = new QWidget();
                if(this->list_userPlaylist->size() > 0){
                    this->widget_userPlaylist = this->setUIControl_subTitle_withSideBtn(tr("TIDAL Playlist On Rose") + QString(" (%1)").arg(this->list_userPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_userPlaylists, this->vBox_userPlaylist);
                }
                else{
                    this->widget_userPlaylist = this->setUIControl_subTitle_withSideBtn(tr("TIDAL Playlist On Rose") + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_userPlaylists, this->vBox_userPlaylist);
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


    void TidalHome::setUIControl_appendWidget(){

        if(this->flag_mixesTrack[0] == true && this->flag_mixesVideo[0] == true && this->flag_suggestedAlbum[0] == true && this->flag_suggestedTOP[0] == true &&
            this->flag_masterPlaylist[0] == true && flag_masterAlbum[0] == true && flag_focusplaylist[0] == true && flag_risingplaylist[0] == true && flag_visualAlbum[0] == true){

            if(this->flag_mixesTrack[1] == true){
                this->widget_mixesTrack = new QWidget();
                this->widget_mixesTrack = this->setUIControl_subTitle_withSideBtn(QString("Mixes For You (%1)").arg(this->list_mixesTrack->at(0).totalCount), "", BTN_IDX_MIXES_TRACK, this->vBox_mixesTrack);

                this->vBox_mixesTrack->addSpacing(10);

                this->hBox_mixesTrack = this->setUIControl_hBoxLayout_forPlaylists(this->vBox_mixesTrack);

                for(int i = 0; i < this->list_mixesTrack->size(); i++){
                    this->home_mixesTrack[i]->setData(this->list_mixesTrack->at(i));
                    this->hBox_mixesTrack->addWidget(this->home_mixesTrack[i]);
                }

                this->box_home_contents->addLayout(this->vBox_mixesTrack);
                this->box_home_contents->addSpacing(45);
            }

            if(this->flag_mixesVideo[1] == true){
                this->widget_mixesVideo = new QWidget();
                this->widget_mixesVideo = this->setUIControl_subTitle_withSideBtn(QString("Mixes For You [Video] (%1)").arg(this->list_mixesVideo->at(0).totalCount), "", BTN_IDX_MIXES_VIDEO, this->vBox_mixesVideo);

                this->vBox_mixesVideo->addSpacing(10);

                this->hBox_mixesVideo = this->setUIControl_hBoxLayout_forPlaylists(this->vBox_mixesVideo);

                for(int i = 0; i < this->list_mixesVideo->size(); i++){
                    this->home_mixesVideo[i]->setData(this->list_mixesVideo->at(i));
                    this->hBox_mixesVideo->addWidget(this->home_mixesVideo[i]);
                }

                this->box_home_contents->addLayout(this->vBox_mixesVideo);
                this->box_home_contents->addSpacing(45);
            }

            if(this->flag_suggestedAlbum[1] == true){
                this->widget_suggestedAlbum = new QWidget();
                this->widget_suggestedAlbum = this->setUIControl_subTitle_withSideBtn(QString("Suggested New Albums (%1)").arg(this->list_albumSuggested->at(0).totalCount), "View All", BTN_IDX_SUGGESTED_NEW_ALBUM, this->vBox_suggestedAlbum);

                this->vBox_suggestedAlbum->addSpacing(10);

                this->hBox_suggestedAlbum = this->setUIControl_hBoxLayout_forAlbum(this->vBox_suggestedAlbum);

                for(int i = 0; i < this->list_albumSuggested->size(); i++){
                    this->home_suggestedAlbum[i]->setData(this->list_albumSuggested->at(i));
                    this->hBox_suggestedAlbum->addWidget(this->home_suggestedAlbum[i]);
                }

                this->box_home_contents->addLayout(this->vBox_suggestedAlbum);
                this->box_home_contents->addSpacing(45);
            }

            if(this->flag_suggestedTOP[1] == true){
                this->widget_suggestedTOP = new QWidget();
                this->widget_suggestedTOP = this->setUIControl_subTitle_withSideBtn(QString("Popular Albums (%1)").arg(this->list_albumSuggested_Top->at(0).totalCount), "View All", BTN_IDX_SUGGESTED_TOP_ALBUM, this->vBox_suggestedTOP);

                this->vBox_suggestedTOP->addSpacing(10);

                this->hBox_suggestedTOP = this->setUIControl_hBoxLayout_forAlbum(this->vBox_suggestedTOP);

                for(int i = 0; i < this->list_albumSuggested_Top->size(); i++){
                    this->home_suggestedTop[i]->setData(this->list_albumSuggested_Top->at(i));
                    this->hBox_suggestedTOP->addWidget(this->home_suggestedTop[i]);
                }

                this->box_home_contents->addLayout(this->vBox_suggestedTOP);
                this->box_home_contents->addSpacing(45);
            }

            if(this->flag_masterPlaylist[1] == true){
                this->widget_masterPlaylist = new QWidget();
                this->widget_masterPlaylist = this->setUIControl_subTitle_withSideBtn(QString("Masters Playlists (%1)").arg(this->list_playlistMaster->at(0).totalCount), "View All", BTN_IDX_MASTER_PLAYLIST, this->vBox_masterPlaylist);

                this->vBox_masterPlaylist->addSpacing(10);

                this->hBox_masterPlaylist = this->setUIControl_hBoxLayout_forPlaylists(this->vBox_masterPlaylist);

                for(int i = 0; i < this->list_playlistMaster->size(); i++){
                    this->home_playlistMaster[i]->setData(this->list_playlistMaster->at(i));
                    this->hBox_masterPlaylist->addWidget(this->home_playlistMaster[i]);
                }

                this->box_home_contents->addLayout(this->vBox_masterPlaylist);
                this->box_home_contents->addSpacing(45);
            }

            if(this->flag_masterAlbum[1] == true){
                this->widget_masterAlbum = new QWidget();
                this->widget_masterAlbum = this->setUIControl_subTitle_withSideBtn(QString("Masters Albums (%1)").arg(this->list_albumMaster->at(0).totalCount), "View All", BTN_IDX_MASTER_ALBUM, this->vBox_masterAlbum);

                this->vBox_masterAlbum->addSpacing(10);

                this->hBox_masterAlbum = this->setUIControl_hBoxLayout_forAlbum(this->vBox_masterAlbum);

                for(int i = 0; i < this->list_albumMaster->size(); i++){
                    this->home_albumMaster[i]->setData(this->list_albumMaster->at(i));
                    this->hBox_masterAlbum->addWidget(this->home_albumMaster[i]);
                }

                this->box_home_contents->addLayout(this->vBox_masterAlbum);
                this->box_home_contents->addSpacing(45);
            }

            if(this->flag_focusplaylist[1] == true){
                this->widget_focusplaylist = new QWidget();
                this->widget_focusplaylist = this->setUIControl_subTitle_withSideBtn(QString("Focus (%1)").arg(this->list_playlistFocus->at(0).totalCount), "View All", BTN_IDX_MOODS_FOCUS, this->vBox_focusplaylist);

                this->vBox_focusplaylist->addSpacing(10);

                this->hBox_focusplaylist = this->setUIControl_hBoxLayout_forPlaylists(this->vBox_focusplaylist);

                for(int i = 0; i < this->list_playlistFocus->size(); i++){
                    this->home_playlistFocus[i]->setData(this->list_playlistFocus->at(i));
                    this->hBox_focusplaylist->addWidget(this->home_playlistFocus[i]);
                }

                this->box_home_contents->addLayout(this->vBox_focusplaylist);
                this->box_home_contents->addSpacing(45);
            }

            if(this->flag_risingplaylist[1] == true){
                this->widget_risingplaylist = new QWidget();
                this->widget_risingplaylist = this->setUIControl_subTitle_withSideBtn(QString("Tidal Rising (%1)").arg(this->list_playlistRising->at(0).totalCount), "View All", BTN_IDX_RISING_NEW, this->vBox_risingplaylist);

                this->vBox_risingplaylist->addSpacing(10);

                this->hBox_risingplaylist = this->setUIControl_hBoxLayout_forPlaylists(this->vBox_risingplaylist);

                for(int i = 0; i < this->list_playlistRising->size(); i++){
                    this->home_playlistRising[i]->setData(this->list_playlistRising->at(i));
                    this->hBox_risingplaylist->addWidget(this->home_playlistRising[i]);
                }

                this->box_home_contents->addLayout(this->vBox_risingplaylist);
                this->box_home_contents->addSpacing(45);
            }

            if(this->flag_visualAlbum[1] == true){
                this->widget_visualAlbum = new QWidget();
                this->widget_visualAlbum = this->setUIControl_subTitle_withSideBtn(QString("Visual Album (%1)").arg(this->list_visualAlbum->at(0).totalCount), "View All", BTN_IDX_VISUAL_ALBUM, this->vBox_visualAlbum);

                this->vBox_visualAlbum->addSpacing(10);

                this->hBox_visualAlbum = this->setUIControl_hBoxLayout_forAlbum(this->vBox_visualAlbum);

                for(int i = 0; i < this->list_visualAlbum->size(); i++){
                    this->home_visualAlbum[i]->setData(this->list_visualAlbum->at(i));
                    this->hBox_visualAlbum->addWidget(this->home_visualAlbum[i]);
                }

                this->box_home_contents->addLayout(this->vBox_visualAlbum);
                this->box_home_contents->addSpacing(60);
            }
        }
    }


    QWidget* TidalHome::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QWidget* TidalHome::setUIControl_subTitle_withSubMenu(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QHBoxLayout* TidalHome::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

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
    QHBoxLayout* TidalHome::setUIControl_hBoxLayout_forPlaylists(QLayout *p_layout){

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
        tmp_scrollArea->setContentsMargins(0, 0, 0 ,0);
        tmp_scrollArea->setFixedHeight(305);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_playlist;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] playlist 데이터를 받아서 처리함
     * @param list_data
     */
    void TidalHome::slot_applyResult_mixes_track(const QList<tidal::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_mixesTrack->append(list_data);
            this->flag_mixesTrack[1] = true;
        }

        this->flag_mixesTrack[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] playlist 데이터를 받아서 처리함
     * @param list_data
     */
    void TidalHome::slot_applyResult_mixes_video(const QList<tidal::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_mixesVideo->append(list_data);
            this->flag_mixesVideo[1] = true;
        }

        this->flag_mixesVideo[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함 (Suggested New Albums)
     * @param list_data
     */
    void TidalHome::slot_applyResult_albums_suggested(const QList<tidal::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_albumSuggested->append(list_data);
            this->flag_suggestedAlbum[1] = true;
        }

        this->flag_suggestedAlbum[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함 (Popular Albums)
     * @param list_data
     */
    void TidalHome::slot_applyResult_albums_suggested_top(const QList<tidal::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_albumSuggested_Top->append(list_data);
            this->flag_suggestedTOP[1] = true;
        }

        this->flag_suggestedTOP[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] playlist 데이터를 받아서 처리함
     * @param list_data
     */
    void TidalHome::slot_applyResult_playlists_master(const QList<tidal::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_playlistMaster->append(list_data);
            this->flag_masterPlaylist[1] = true;
        }

        this->flag_masterPlaylist[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함 (Master Album)
     * @param list_data
     */
    void TidalHome::slot_applyResult_albums_master(const QList<tidal::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_albumMaster->append(list_data);
            this->flag_masterAlbum[1] = true;
        }

        this->flag_masterAlbum[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함 (Focus Album)
     * @param list_data
     */
    void TidalHome::slot_applyResult_playlists_focus(const QList<tidal::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_playlistFocus->append(list_data);
            this->flag_focusplaylist[1] = true;
        }

        this->flag_focusplaylist[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함 (Rising Album)
     * @param list_data
     */
    void TidalHome::slot_applyResult_playlists_rising(const QList<tidal::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_playlistRising->append(list_data);
            this->flag_risingplaylist[1] = true;
        }

        this->flag_risingplaylist[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함 (Master Album)
     * @param list_data
     */
    void TidalHome::slot_applyResult_albums_visual(const QList<tidal::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            for(int i = 0; i < 15; i++){
              this->list_visualAlbum->append(list_data.at(i));
            }
            this->list_visualAlbum_viewall->clear();
            this->list_visualAlbum_viewall->append(list_data);
            this->flag_visualAlbum[1] = true;
        }

        this->flag_visualAlbum[0] = true;
        this->setUIControl_appendWidget();
        this->slot_hide_msg();
    }


    void TidalHome::slot_applyResult_recentlyAlbum(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void TidalHome::slot_applyResult_recentlyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void TidalHome::slot_applyResult_recentlyTrack(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void TidalHome::slot_applyResult_myPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void TidalHome::slot_applyResult_userPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void TidalHome::slot_applyResult_getRating_track(const QJsonArray &contents){

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

    // 서브 타이틀별 우측 View All ---------------------------------------------------------------------------------------------------

    /**
     * @brief 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void TidalHome::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUGGESTED_NEW_ALBUM){
            // Album All View 화면으로 이동
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = TIDAL_API_SUGGESTED_ALBUM;
            this->movePage_album_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUGGESTED_TOP_ALBUM){
            // Album All View 화면으로 이동
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = TIDAL_API_SUGGESTED_TOP;
            this->movePage_album_allView(data_page);
        }
        else if(btnId == BTN_IDX_MASTER_PLAYLIST){
            // Playlist All View 화면으로 이동
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = TIDAL_API_MASTER_PLAYLIST;
            this->movePage_playlist_allView(data_page);
        }
        else if(btnId == BTN_IDX_MASTER_ALBUM){
            // Album All View 화면으로 이동
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = TIDAL_API_MASTER_ALBUM;
            this->movePage_album_allView(data_page);
        }
        else if(btnId == BTN_IDX_MOODS_FOCUS){
            // Album All View 화면으로 이동
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = TIDAL_API_MOODS_FOCUS;
            this->movePage_playlist_allView(data_page);
        }
        else if(btnId == BTN_IDX_RISING_NEW){
            // Album All View 화면으로 이동
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = TIDAL_API_RISING_NEW;
            this->movePage_playlist_allView(data_page);
        }
        else if(btnId == BTN_IDX_VISUAL_ALBUM){
            // Album All View 화면으로 이동
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = TIDAL_API_VISUAL_ALBUM;
            global.user_forTidal.list_visualData.clear();
            global.user_forTidal.list_visualData.append(*this->list_visualAlbum_viewall);
            this->movePage_album_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_recentlyPlay){
            if(this->contentStep == ALBTAB_STEP_ALBUM){
                QString title = tr("Recently played albums");

                QJsonObject tmp_data;
                if(this->list_recentlyAlbum->size() > 0){
                    tmp_data.insert("title", QString(title + " (%1)").arg(this->list_recentlyAlbum->at(0).totalCount));
                }
                else{
                    tmp_data.insert("title", QString(title + " (0)"));
                }
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_MY_RECENTLY_ALBUM_ALL_LIST);
                emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
            }
            else if(this->contentStep == ALBTAB_STEP_PLAYLIST){
                QString title = tr("Recently played playlists");

                QJsonObject tmp_data;
                if(this->list_recentlyPlaylist->size() > 0){
                    tmp_data.insert("title", QString(title + " (%1)").arg(this->list_recentlyPlaylist->at(0).totalCount));
                }
                else{
                    tmp_data.insert("title", QString(title + " (0)"));
                }
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_MY_RECENTLY_PLAYLIST_ALL_LIST);
                emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
            }
        }
        else if(btnId == BTN_IDX_SUBTITLE_recentlyTrack){
            QString title = tr("Recently played tracks");

            QJsonObject tmp_data;
            if(this->list_recentlytrack->size() > 0){
                tmp_data.insert("title", QString(title + " (%1)").arg(this->list_recentlytrack->at(0).totalCount));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_MY_RECENTLY_TRACK_ALL_LIST);
            emit this->signal_clickedMovePage(tmp_data);                    // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_myPlaylists){
            QString title = tr("My playlists");

            QJsonObject tmp_data;
            if(this->list_myPlaylist->size() > 0){
                tmp_data.insert("title", QString(title + "(%1)").arg(this->list_myPlaylist->at(0).totalCount));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_MY_ROSE_PLAYLIST_ALL_LIST);
            emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_userPlaylists){
            QString title = tr("TIDAL Playlist On Rose");

            QJsonObject tmp_data;
            if(this->list_userPlaylist->size() > 0){
                tmp_data.insert("title", QString(title + "(%1)").arg(this->list_userPlaylist->at(0).totalCount));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_USER_ROSE_PLAYLIST_ALL_LIST);
            emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
        }
    }


    void TidalHome::changedOnlyTabUI_notSendSignal(QString p_step){

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


    void TidalHome::slot_changedSubTabUI(const QJsonObject &p_data){

        QString tmp_step = p_data[KEY_PAGE_CODE].toString();

        // 페이지 변경 및 데이터 세팅
        this->changedOnlyTabUI_notSendSignal(tmp_step);
    }


    void TidalHome::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_TIDAL)){

            global.user.setDeviceChange(false);

            if(global.user_forTidal.isLogined() == true){
                AbstractTidalSubWidget::slot_acceptedDialogLogin();
            }
            else if(global.user_forTidal.isLogined() == false){
                AbstractTidalSubWidget::slot_acceptedDialogLogout();
            }

            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_SETTINGS);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedMovePage(tmp_data);
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
    void TidalHome::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리 - Album 파트가 여러개임
        if(section == SECTION_FOR_MORE_POPUP___suggested_albums){
            this->proc_clicked_itemAlbum(this->list_albumSuggested, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___suggested_top){
            this->proc_clicked_itemAlbum(this->list_albumSuggested_Top, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___master_albums){
            this->proc_clicked_itemAlbum(this->list_albumMaster, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___visual_album){
            if(clickMode == AbstractItem::ClickMode::PlayBtn){
                ProcCommon *proc = new ProcCommon(this);
                connect(proc, &ProcCommon::completeReq_list_items_of_visual, this, &TidalHome::slot_applyResult_tracks);
                proc->request_tidal_getList_items_of_visual(this->list_visualAlbum->at(index).id, this->list_visualAlbum->at(index).numberOfTracks);
            }
            else{
                this->proc_clicked_itemAlbum(this->list_visualAlbum, clickMode, index, section);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___recentlyAlbum){
            roseHome::AlbumItemData data = this->list_recentlyAlbum->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Album 바로 재생
                tidal::ProcRosePlay_withTidal *procRosePlay = new tidal::ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byAlbumID(data.clientKey.toInt(), OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Album Detail 페이지 진입
                QJsonObject jsonObj_move = QJsonObject();
                jsonObj_move.insert("id", data.clientKey.toInt());
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_ALBUM_DETAIL);

                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){//c220826_2
                print_debug();

                // OptionPopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
            }
        }
    }


    void TidalHome::slot_applyResult_tracks(const QList<tidal::TrackItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(flag_lastPage);
        Q_UNUSED(list_data);

        ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
        procRosePlay->requestPlayRose_videolist(jsonArr_dataToPlay, OptMorePopup::ClickMode::Play_RightNow);
    }


    void TidalHome::slot_applyResult_getShareLink(const QString &link){//c220826_2

        this->shareLink = link;
    }


    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalHome::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___mixes_track){
            this->proc_clicked_itemPlaylist(this->list_mixesTrack, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___mixes_video){
            this->proc_clicked_itemPlaylist(this->list_mixesVideo, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___master_playlists){
            this->proc_clicked_itemPlaylist(this->list_playlistMaster, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___focus_playlists){
            this->proc_clicked_itemPlaylist(this->list_playlistFocus, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___rising_playlists){
            this->proc_clicked_itemPlaylist(this->list_playlistRising, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___recentlyPlaylist){
            roseHome::PlaylistItemData data = this->list_recentlyPlaylist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                if(data.type == "TIDAL" && data.isRose == false){
                    tidal::PlaylistItemData tmpPlaylist;
                    tmpPlaylist.uuid = data.clientKey;

                    if(data.title.contains("Mix")){
                        tmpPlaylist.description = "mixes";
                    }

                    tidal::ProcRosePlay_withTidal *procRosePlay = new tidal::ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_byPlaylistUUID(tmpPlaylist, OptMorePopup::ClickMode::Play_RightNow);
                }
                else{
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byPlaylistID(this->list_recentlyPlaylist->at(index).id, OptMorePopup::ClickMode::Play_RightNow);
                }
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입
                if(data.isRose == false){
                    if(data.ownerName == "tidal" && data.title.contains("Mix")){
                        QJsonObject jsonObj_move = QJsonObject();
                        jsonObj_move.insert("uuid", data.clientKey);
                        jsonObj_move.insert("title", data.title);
                        jsonObj_move.insert("image", data.thumbnail);
                        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MIXES_DETAIL);

                        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                    }
                    else{
                        if(data.clientKey.isEmpty()){
                            QJsonObject jsonObj_move = QJsonObject();
                            jsonObj_move.insert("id", data.id);
                            jsonObj_move.insert("ownerId", data.ownerId);
                            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_ROSE_PLAYLIST_DETAIL);

                            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                        }
                        else{
                            QJsonObject jsonObj_move = QJsonObject();
                            jsonObj_move.insert("uuid", data.clientKey);
                            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_PLAYLIST_DETAIL);

                            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                        }
                    }
                }
                else{
                    QJsonObject jsonObj_move = QJsonObject();
                    jsonObj_move.insert("id", data.id);
                    jsonObj_move.insert("ownerId", data.ownerId);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_ROSE_PLAYLIST_DETAIL);

                    emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                }
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){//c220826_2
                print_debug();

                // OptionPopup 띄우기 필요
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
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_ROSE_PLAYLIST_DETAIL);

                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){//c220826_2
                print_debug();

                // OptionPopup 띄우기 필요
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
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_ROSE_PLAYLIST_DETAIL);

                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){//c220826_2
                print_debug();

                // OptionPopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
            }
        }
    }


    void TidalHome::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___recentlyTrack;

        // ClickMode 별로 처리
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
                ratingInfo.insert("type", this->list_recentlytrack->at(idx).type);

                QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalHome::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

                this->flag_check_track = true;;
            }
        }
        else{
            // ClickMode 별로 처리
            //this->proc_clicked_itemTrack_inList(this->list_recentlytrack, this->jsonArr_tracks_toPlay, clickMode, idx, section);
            roseHome::TrackItemData data = list_recentlytrack->at(idx);

            if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AllBox){
                // Track 바로 재생 - clicked 항목만 재생
                /*QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
                int curr_clickMode = settings->value(rosesettings.SETTINGS_TRACK_OPTION).toInt();

                OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                switch(curr_clickMode)
                {
                    case 0:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                        break;
                    case 1:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                        break;
                    case 2:
                        playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                        break;
                    case 3:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                        break;
                    case 4:
                        playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                        break;
                    case 5:
                        playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                        break;
                    case 6:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                        break;
                }*/

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

                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data, this->jsonArr_tracks_toPlay, idx, playType);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                // 플레이리스트 담기 - Track
                //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){
                // OptMorePopup 띄우기 필요
                print_debug();
                        this->makeObj_optMorePopup(OptMorePopup::Rosehome_Track, roseHome::ConvertData::getConvertOptHeaderData(data), idx, section);
            }
        }
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
    void TidalHome::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

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
                tidal::ProcRosePlay_withTidal *procRosePlay = new tidal::ProcRosePlay_withTidal(this);
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
                if(this->list_recentlyPlaylist->at(index).type == "TIDAL" && this->list_recentlyPlaylist->at(index).isRose == false){
                    tidal::PlaylistItemData tmpPlaylist;
                    tmpPlaylist.uuid = this->list_recentlyPlaylist->at(index).clientKey;

                    if(this->list_recentlyPlaylist->at(index).title.contains("Mix")){
                        tmpPlaylist.description = "mixes";
                    }

                    tidal::ProcRosePlay_withTidal *procRosePlay = new tidal::ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_byPlaylistUUID(tmpPlaylist, clickMode);
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
        else if(section == SECTION_FOR_MORE_POPUP___mixes_track){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_mixesTrack, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___mixes_video){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_mixesVideo, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___suggested_albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_albumSuggested, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___suggested_top){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_albumSuggested_Top, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___master_playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlistMaster, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___master_albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_albumMaster, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___focus_playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlistFocus, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___rising_playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlistRising, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___visual_album){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_visualAlbum, index, clickMode);
        }
    }
}
