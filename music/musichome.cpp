#include "music/musichome.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/networkhttp.h"
#include "common/global.h"
#include "common/gscommon.h"
#include "common/rosesettings.h"
#include "common/sqlitehelper.h"

#include "widget/dialogconfirm.h"
#include "widget/NoData_Widget.h"
#include "widget/optionpopup.h"

#include "widget/toastmsg.h"//c220729
#include "common/networkhttp.h"//c220729


#include <QSettings>
#include <QScroller>


/**
 * @brief MusicHome::MusicHome 음악 > 홈
 * @param parent
 */
namespace music {

    // MUSIC subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX_SUBTITLE_recentlyAdds = 0;
    const int BTN_IDX_SUBTITLE_recommandArtist = 1;

    // MUSIC section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___recentlyAdds = 0;
    const int SECTION_FOR_MORE_POPUP___recommandArtist = 1;


    // ROSE subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX_SUBTITLE_recentlyPlay = 10;
    const int BTN_IDX_SUBTITLE_recentlyTrack = 11;
    const int BTN_IDX_SUBTITLE_History = 12;
    const int BTN_IDX_SUBTITLE_myPlaylists = 13;
    const int BTN_IDX_SUBTITLE_userPlaylists = 14;

    // ROSE section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___recentlyAlbum = 10;
    const int SECTION_FOR_MORE_POPUP___recentlyPlaylist = 11;
    const int SECTION_FOR_MORE_POPUP___recentlyTrack = 12;
    const int SECTION_FOR_MORE_POPUP___History = 13;
    const int SECTION_FOR_MORE_POPUP___myPlaylists = 14;
    const int SECTION_FOR_MORE_POPUP___userPlaylists = 15;


    // ROSE API 관련
    QString ROSE_API_ALBUM_PATH = "member/album/recent";
    QString ROSE_API_PLAYLIST_PATH = "member/playlist/recent";
    QString ROSE_API_TRACK_PATH = "member/track/recent";
    QString ROSE_API_HISTORY_PATH = "member/track/history";
    QString ROSE_API_MYPLAYLIST_PATH = "member/playlist";
    QString ROSE_API_USERPLAYLIST_PATH = "member/playlist/all";

    QString ALBTAB_STEP_ALBUM = "album";
    QString ALBTAB_STEP_PLAYLIST = "playlist";


    MusicHome::MusicHome(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_rosefilter, parent) {

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_completeDownloadDB()), SLOT(slot_topLabelChanged()));
        //connect(linker, SIGNAL(signal_musicRecentChanged()), SLOT(slot_musicRecentChanged()));
        connect(linker, SIGNAL(signal_logined()), SLOT(slot_getMyInfo_loginAfter()));
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));

        //connect(linker, SIGNAL(signal_dragEnterEvent_hide_show(bool)), SLOT(slot_dragEnterEvent_hide_show(bool)));//c220930
        //connect(linker, SIGNAL(signal_dropEvent_hide_show(bool)), SLOT(slot_dropEvent_hide_show(bool)));//c220930

        this->widget_parent = parent;

        setTopUIControl(parent);
        requestDataCount();

        //data
        this->list_recentlyAlbum = new QList<roseHome::AlbumItemData>();
        this->list_recentlyPlaylist = new QList<roseHome::PlaylistItemData>();
        this->list_recentlytrack = new QList<roseHome::TrackItemData>();
        this->list_Historylist = new QList<roseHome::HistoryItemData>;
        this->list_myPlaylist = new QList<roseHome::PlaylistItemData>();
        this->list_userPlaylist = new QList<roseHome::PlaylistItemData>();

        this->list_RecentAdds = new QList<roseHome::AlbumItemData>();
        this->list_RecommandArtist = new QList<roseHome::ArtistItemData>();

#if defined(Q_OS_WINDOWS)
#elif defined(Q_OS_MAC)
        /*//cheon210831-network
        QString ipPrefix;//cheon210525
        QStringList list_ipNumber = QString(findIP->getMyIP()).split(".");//cheon210525

        if(list_ipNumber.size()==4){//cheon210525
            ipPrefix = QString("%1.%2.%3.").arg(list_ipNumber.at(0)).arg(list_ipNumber.at(1)).arg(list_ipNumber.at(2));
            QString currIp(ipPrefix + QString::number(1));//cheon210525
            QString parameter1 = "-c 1";
            QString parameter2 = "-W 0.002";
            QString parameter3 = "-i 0.10";
        // QString currIp = "192.168.1.1";
            int exitCode = QProcess::execute("ping", QStringList() << parameter1  << parameter2<< parameter3 << currIp);
        }
        */
#endif
    }


    MusicHome::~MusicHome(){

        this->deleteLater();
    }


    void MusicHome::show_topLabelControl(){

        int left = 0;
        int width = 0;
        int spacing = 0;

        if(this->width() <= 1150){
            this->widget_db_info->hide();
        }
        else{
            this->widget_db_info->hide();

            this->label_album->setGeometry(left, 0, this->label_album->sizeHint().width(), this->widget_db_info->height());

            left = this->label_album->geometry().width();
            this->lb_album_cnt->setGeometry(left, 0, this->lb_album_cnt->sizeHint().width(), this->widget_db_info->height());

            left = this->lb_album_cnt->geometry().left() + this->lb_album_cnt->geometry().width() + 30;
            this->label_track->setGeometry(left, 0, this->label_track->sizeHint().width(), this->widget_db_info->height());

            left = this->label_track->geometry().left() + this->label_track->geometry().width();
            this->lb_track_cnt->setGeometry(left, 0, this->lb_track_cnt->sizeHint().width(), this->widget_db_info->height());

            left = this->lb_track_cnt->geometry().left() + this->lb_track_cnt->geometry().width() + 30;
            this->label_artist->setGeometry(left, 0, this->label_artist->sizeHint().width(), this->widget_db_info->height());

            left = this->label_artist->geometry().left() + this->label_artist->geometry().width();
            this->lb_artist_cnt->setGeometry(left, 0, this->lb_artist_cnt->sizeHint().width(), this->widget_db_info->height());

            width = this->label_album->geometry().width() + this->lb_album_cnt->geometry().width() + 30 + this->label_track->geometry().width() + this->lb_track_cnt->geometry().width() + 30
                    + this->label_artist->geometry().width() + this->lb_artist_cnt->geometry().width() + 100;
            this->widget_db_info->setGeometry(1150, 0, width, this->widget_parent->height());

            if(this->width() > (1150 + width) && this->width() <= (1150 + width + 100)){
                spacing = (this->width() - 1150 - width) / 2;

                left = 0;
                this->label_album->setGeometry(left, 0, this->label_album->sizeHint().width(), this->widget_db_info->height());

                left = this->label_album->geometry().width();
                this->lb_album_cnt->setGeometry(left, 0, this->lb_album_cnt->sizeHint().width(), this->widget_db_info->height());

                left = this->lb_album_cnt->geometry().left() + this->lb_album_cnt->geometry().width() + 30 + spacing;
                this->label_track->setGeometry(left, 0, this->label_track->sizeHint().width(), this->widget_db_info->height());

                left = this->label_track->geometry().left() + this->label_track->geometry().width();
                this->lb_track_cnt->setGeometry(left, 0, this->lb_track_cnt->sizeHint().width(), this->widget_db_info->height());

                left = this->lb_track_cnt->geometry().left() + this->lb_track_cnt->geometry().width() + 30 + spacing;
                this->label_artist->setGeometry(left, 0, this->label_artist->sizeHint().width(), this->widget_db_info->height());

                left = this->label_artist->geometry().left() + this->label_artist->geometry().width();
                this->lb_artist_cnt->setGeometry(left, 0, this->lb_artist_cnt->sizeHint().width(), this->widget_db_info->height());

                width = this->label_album->geometry().width() + this->lb_album_cnt->geometry().width() + 30 + spacing + this->label_track->geometry().width() + this->lb_track_cnt->geometry().width() + 30 + spacing
                        + this->label_artist->geometry().width() + this->lb_artist_cnt->geometry().width() + 100;
                this->widget_db_info->setGeometry(1150, 0, width, this->widget_parent->height());

                //qDebug() << "[Debug]" << __FUNCTION__ << " line::" << __LINE__ << this->width() << width << spacing;
            }
            else if(this->width() > (1150 + width + 100)){
                spacing = 50;

                left = 0;
                this->label_album->setGeometry(left, 0, this->label_album->sizeHint().width(), this->widget_db_info->height());

                left = this->label_album->geometry().width();
                this->lb_album_cnt->setGeometry(left, 0, this->lb_album_cnt->sizeHint().width(), this->widget_db_info->height());

                left = this->lb_album_cnt->geometry().left() + this->lb_album_cnt->geometry().width() + 30 + spacing;
                this->label_track->setGeometry(left, 0, this->label_track->sizeHint().width(), this->widget_db_info->height());

                left = this->label_track->geometry().left() + this->label_track->geometry().width();
                this->lb_track_cnt->setGeometry(left, 0, this->lb_track_cnt->sizeHint().width(), this->widget_db_info->height());

                left = this->lb_track_cnt->geometry().left() + this->lb_track_cnt->geometry().width() + 30 + spacing;
                this->label_artist->setGeometry(left, 0, this->label_artist->sizeHint().width(), this->widget_db_info->height());

                left = this->label_artist->geometry().left() + this->label_artist->geometry().width();
                this->lb_artist_cnt->setGeometry(left, 0, this->lb_artist_cnt->sizeHint().width(), this->widget_db_info->height());

                width = this->label_album->geometry().width() + this->lb_album_cnt->geometry().width() + 30 + spacing + this->label_track->geometry().width() + this->lb_track_cnt->geometry().width() + 30 + spacing
                        + this->label_artist->geometry().width() + this->lb_artist_cnt->geometry().width() + 100;

                left = this->width() - width;
                this->widget_db_info->setGeometry(left, 0, width, this->widget_parent->height());

                //qDebug() << "[Debug]" << __FUNCTION__ << " line::" << __LINE__ << this->width() << width << spacing;
            }

            this->widget_db_info->show();
        }


//        this->widget_album->show();
//        this->widget_track->show();
//        this->widget_artist->show();
    }


    void MusicHome::hide_topLabelControl(){

        this->widget_db_info->hide();

//        this->widget_album->hide();
//        this->widget_track->hide();
//        this->widget_artist->hide();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void MusicHome::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->flagNeedReload = true;

            this->page = pageCode;

            // init data
            this->list_recentlyAlbum->clear();
            this->list_recentlyPlaylist->clear();
            this->list_recentlytrack->clear();
            this->list_Historylist->clear();
            this->list_myPlaylist->clear();
            this->list_userPlaylist->clear();

            this->list_RecentAdds->clear();
            this->list_RecommandArtist->clear();

            this->flag_album[0] = false;
            this->flag_playlist[0] = false;
            this->flag_track[0] = false;
            flag_historylist[0] = false;
            this->flag_myPlaylist[0] = false;
            this->flag_userPlaylist[0] = false;

            this->flag_album[1] = false;
            this->flag_playlist[1] = false;
            this->flag_track[1] = false;
            flag_historylist[1] = false;
            this->flag_myPlaylist[1] = false;
            this->flag_userPlaylist[1] = false;

            this->jsonArr_tracks_toPlay = QJsonArray();
            this->jsonArr_artists = QJsonArray();

            print_debug();ContentLoadingwaitingMsgHide();//c230322_3
        }
        else{
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            global.enable_message_flag = true;

            this->flag_recentAlbum_check[0] = false;
            this->flag_recentPlaylist_check[0] = false;
            this->flag_recentTrack_check[0] = false;
            //this->flag_recentArtist_check[0] = false;
            this->flag_myPlaylist_check[0] = false;

            this->flag_recentAlbum_check[1] = false;
            this->flag_recentPlaylist_check[1] = false;
            this->flag_recentTrack_check[1] = false;
            //this->flag_recentArtist_check[1] = false;
            this->flag_myPlaylist_check[1] = false;

            // request HTTP API for Recently Album
            roseHome::ProcCommon *proc_recentlyAlbum = new roseHome::ProcCommon(this);
            connect(proc_recentlyAlbum, &roseHome::ProcCommon::completeReq_list_albums, this, &MusicHome::slot_applyResult_recentlyAlbumCheck);
            proc_recentlyAlbum->request_rose_getList_recentlyAlbums(ROSE_API_ALBUM_PATH , "MUSIC", 0, 15);

            // request HTTP API for Recently Playlist
            roseHome::ProcCommon *proc_recentlyPlaylist = new roseHome::ProcCommon(this);
            connect(proc_recentlyPlaylist, &roseHome::ProcCommon::completeReq_list_playlists, this, &MusicHome::slot_applyResult_recentlyPlaylistCheck);
            proc_recentlyPlaylist->request_rose_getList_recentlyPlaylists(ROSE_API_PLAYLIST_PATH , "MUSIC", 0, 15);

            // request HTTP API for Recently Track
            roseHome::ProcCommon *proc_recentlyTrack = new roseHome::ProcCommon(this);
            connect(proc_recentlyTrack, &roseHome::ProcCommon::completeReq_list_tracks, this, &MusicHome::slot_applyResult_recentlyTrackCheck);
            proc_recentlyTrack->request_rose_getList_recentlyTracks(ROSE_API_TRACK_PATH , "MUSIC", 0, 5);

            // request HTTP API for Recently Playlist
            roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
            connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &MusicHome::slot_applyResult_myPlaylistCheck);
            proc_myPlaylist->request_rose_getList_myPlaylists(ROSE_API_MYPLAYLIST_PATH , "MUSIC", 0, 15);

            print_debug();ContentLoadingwaitingMsgHide();//c230322_3
        }
    }


    void MusicHome::setActivePage(){

        if(this->flagNeedReload){
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // 항상 부모클래스의 함수 먼저 호출
            AbstractCommonSubWidget::setActivePage();

            this->box_contents->removeWidget(this->widget_rose_contents);
            this->box_contents->removeWidget(this->widget_login_contents);
            GSCommon::clearLayout(this->box_contents);

            this->box_rose_contents = new QVBoxLayout();
            this->box_rose_contents->setSpacing(0);
            this->box_rose_contents->setContentsMargins(0, 10, 0, 0);

            this->widget_rose_contents = new QWidget();
            this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_rose_contents->setLayout(this->box_rose_contents);

            this->widget_login = setUIControl_LoginBefore();

            this->box_login_contents = new QVBoxLayout();
            this->box_login_contents->setSpacing(0);
            this->box_login_contents->setContentsMargins(0, 0, 0, 0);
            this->box_login_contents->addWidget(this->widget_login, 0, Qt::AlignTop);

            this->widget_login_contents = new QWidget();
            this->widget_login_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_login_contents->setLayout(this->box_login_contents);
            this->widget_login_contents->hide();

            this->stackedWidget_Contents = new QStackedWidget();
            this->stackedWidget_Contents->addWidget(this->widget_rose_contents);
            this->stackedWidget_Contents->addWidget(this->widget_login_contents);

            //---------------------//c220729  start

            /*this->box_total_home_contents = new QVBoxLayout();
            this->box_total_home_contents->setSpacing(0);
            this->box_total_home_contents->setContentsMargins(0, 0, 0, 0);

            this->box_total_home_contents->addWidget(this->widget_rose_contents, 0, Qt::AlignTop);
            this->box_total_home_contents->addWidget(this->widget_login_contents, 0, Qt::AlignTop);

            this->widget_total_home_contents = new QWidget();
            this->widget_total_home_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_total_home_contents->setLayout(this->box_total_home_contents);

            this->box_contents->addWidget(this->widget_total_home_contents);*/

            this->box_contents->addWidget(this->stackedWidget_Contents);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            //--------------------------------------------//c220729  end

            // sub Title UI
            for(int i = 0; i < 20; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            if(global.updateCheckFlag || global.user.getAccess_token().isEmpty()){

                // init UI
                this->vBox_recentlyAdds = new QVBoxLayout();
                this->vBox_recommandArtist = new QVBoxLayout();

                this->hBox_recentlyAdds = new QHBoxLayout();
                this->hBox_recommandArtist = new QHBoxLayout();

                GSCommon::clearLayout(this->vBox_recentlyAdds);
                GSCommon::clearLayout(this->vBox_recommandArtist);

                GSCommon::clearLayout(this->hBox_recentlyAdds);
                GSCommon::clearLayout(this->hBox_recommandArtist);

                // sub Title UI
                for(int i = 0; i < 15; i++){
                    this->home_recently_Add[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___recentlyAdds, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                }

                for(int i = 0; i < 15; i++){
                    this->home_recommand_Artist[i] = new roseHome::ItemArtist_rosehome(i, SECTION_FOR_MORE_POPUP___recommandArtist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                }

                global.enable_message_count = 0;
                global.enable_message_flag = true;
                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223

                this->setUIControl_requestMusic();

                this->widget_rose_contents->show();
                this->widget_login_contents->hide();
                this->stackedWidget_Contents->setCurrentIndex(0);
            }
            else if(!global.user.getAccess_token().isEmpty()){

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
                this->vBox_recentlyPlay = new QVBoxLayout();
                this->vBox_recentlyTrack = new QVBoxLayout();
                this->vBox_historylist = new QVBoxLayout();
                this->vBox_myPlaylist = new QVBoxLayout();
                this->vBox_userPlaylist = new QVBoxLayout();

                this->hBox_recentlyAlbum = new QHBoxLayout();
                this->hBox_recentlyPlaylist = new QHBoxLayout();
                this->hBox_historylist = new QHBoxLayout();
                this->hBox_myPlaylist = new QHBoxLayout();
                this->hBox_userPlaylist = new QHBoxLayout();

                GSCommon::clearLayout(this->vBox_recentlyPlay);
                GSCommon::clearLayout(this->vBox_recentlyTrack);
                GSCommon::clearLayout(this->vBox_historylist);
                GSCommon::clearLayout(this->vBox_myPlaylist);
                GSCommon::clearLayout(this->vBox_userPlaylist);

                GSCommon::clearLayout(this->hBox_recentlyAlbum);
                GSCommon::clearLayout(this->hBox_recentlyPlaylist);
                GSCommon::clearLayout(this->hBox_historylist);
                GSCommon::clearLayout(this->hBox_myPlaylist);
                GSCommon::clearLayout(this->hBox_userPlaylist);

                for(int i = 0; i < 15; i++){
                    this->home_recently_album[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___recentlyAlbum, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                }

                for(int i = 0; i < 15; i++){
                    this->home_recently_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___recentlyPlaylist, tidal::AbstractItem::ImageSizeMode::Square_200x200, 2, true);
                }

                for (int i = 0; i < 5; i++) {
                    this->home_recently_track[i] = new PlaylistTrackDetailInfo_RHV;
                    this->home_recently_track[i]->setProperty("index", i);
                    this->home_recently_track[i]->setObjectName("recently tracks");
                }

                for(int i = 0; i < 15; i++){
                    this->home_historylist[i] = new roseHome::ItemHistory_rosehome(i, SECTION_FOR_MORE_POPUP___History, tidal::AbstractItem::ImageSizeMode::Square_200x200, false);
                }

                for(int i = 0; i < 15; i++){
                    this->home_myPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___myPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                }

                for(int i = 0; i < 15; i++){
                    this->home_userPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___userPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);
                }

                this->vBox_recentlyAdds = new QVBoxLayout();
                this->vBox_recommandArtist = new QVBoxLayout();

                GSCommon::clearLayout(this->vBox_recentlyAdds);
                GSCommon::clearLayout(this->vBox_recommandArtist);

                this->hBox_recentlyAdds = new QHBoxLayout();
                this->hBox_recommandArtist = new QHBoxLayout();

                GSCommon::clearLayout(this->hBox_recentlyAdds);
                GSCommon::clearLayout(this->hBox_recommandArtist);

                for(int i = 0; i < 15; i++){
                    this->home_recently_Add[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___recentlyAdds, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                }

                for(int i = 0; i < 15; i++){
                    this->home_recommand_Artist[i] = new roseHome::ItemArtist_rosehome(i, SECTION_FOR_MORE_POPUP___recommandArtist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                }

                global.enable_message_count = 0;
                global.enable_message_flag = true;
                //print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223

                this->setUIControl_requestRose();

                this->widget_rose_contents->show();
                this->widget_login_contents->hide();
                this->stackedWidget_Contents->setCurrentIndex(0);
            }
            print_debug();ContentLoadingwaitingMsgHide();//c230322_3
        }
    }


    QWidget* MusicHome::setUIControl_LoginBefore(){

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
        connect(btn_login, &QPushButton::clicked, this, &MusicHome::slot_showLoginPage);

        return widget_loginBefore;
    }


    void MusicHome::slot_hide_msg(){

        if(global.enable_message_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;
        }

        // j230331 left click check start
        if(global.enable_section_left == true){
            global.enable_section_left = false;
        }
        // j230331 left click check finish

        //ContentLoadingwaitingMsgHide();
    }


    void MusicHome::slot_time_out(){

        if(global.enable_message_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;
        }

        // j230331 left click check start
        if(global.enable_section_left == true){
            global.enable_section_left = false;
        }
        // j230331 left click check finish

        this->setUIControl_requestRose();
    }


    void MusicHome::slot_showLoginPage(){

        this->dlg_rose_login = new Dialog::DialogLogin(this);
        this->dlg_rose_login->exec();
    }


    void MusicHome::slot_getMyInfo_loginAfter(){

        if(global.user.isValid() == true && this->stackedWidget_Contents->currentIndex() == 1){

            this->page = "";
            QJsonObject tmpCode = QJsonObject();
            tmpCode.insert("pageCode", "home");

            this->setJsonObject_forData(tmpCode);
            this->setActivePage();

        }
    }


    void MusicHome::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_ROSE)){

            global.user.setDeviceChange(false);

            this->page = "";
            QJsonObject tmpCode = QJsonObject();
            tmpCode.insert("pageCode", "home");

            this->setJsonObject_forData(tmpCode);
            this->setActivePage();
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------


    void MusicHome::setUIControl_requestRose(){
        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223

        // request HTTP API for Recently Album
        roseHome::ProcCommon *proc_recentlyAlbum = new roseHome::ProcCommon(this);
        connect(proc_recentlyAlbum, &roseHome::ProcCommon::completeReq_list_albums, this, &MusicHome::slot_applyResult_recentlyAlbum);
        proc_recentlyAlbum->request_rose_getList_recentlyAlbums(ROSE_API_ALBUM_PATH , "MUSIC", 0, 15);

        // request HTTP API for Recently Playlist
        roseHome::ProcCommon *proc_recentlyPlaylist = new roseHome::ProcCommon(this);
        connect(proc_recentlyPlaylist, &roseHome::ProcCommon::completeReq_list_playlists, this, &MusicHome::slot_applyResult_recentlyPlaylist);
        proc_recentlyPlaylist->request_rose_getList_recentlyPlaylists(ROSE_API_PLAYLIST_PATH , "MUSIC", 0, 15);

        // request HTTP API for Recently Track
        roseHome::ProcCommon *proc_recentlyTrack = new roseHome::ProcCommon(this);
        connect(proc_recentlyTrack, &roseHome::ProcCommon::completeReq_list_tracks, this, &MusicHome::slot_applyResult_recentlyTrack);
        proc_recentlyTrack->request_rose_getList_recentlyTracks(ROSE_API_TRACK_PATH , "MUSIC", 0, 5);

        QDate todayData = QDate::currentDate();
        QString strToday = todayData.toString("yyyyMM");

        roseHome::ProcCommon *proc_historylist = new roseHome::ProcCommon(this);
        connect(proc_historylist, &roseHome::ProcCommon::completeReq_list_history, this, &MusicHome::slot_applyResult_historylist);
        proc_historylist->request_rose_getList_hisotrylist(ROSE_API_HISTORY_PATH, strToday, "MUSIC");

        // request HTTP API for Recently Playlist
        roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
        connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &MusicHome::slot_applyResult_myPlaylist);
        proc_myPlaylist->request_rose_getList_myPlaylists(ROSE_API_MYPLAYLIST_PATH , "MUSIC", 0, 15);

        roseHome::ProcCommon *proc_userPlaylist = new roseHome::ProcCommon(this);
        connect(proc_userPlaylist, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &MusicHome::slot_applyResult_userPlaylist);
        proc_userPlaylist->request_rose_getList_usersPlaylists(ROSE_API_USERPLAYLIST_PATH , "PLAYLIST_RECENT", "MUSIC", 0, 15);
        print_debug();ContentLoadingwaitingMsgHide();//c230322_3
    }


    void MusicHome::setUIControl_requestMusic(){

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223

        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){

            QString strQuery = "";
            strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art ";
            strQuery += " FROM album_info AS AI ";
            strQuery += " LEFT OUTER JOIN album_art AS AA ";
            strQuery += " ON AI._id=AA.album_id ";
            strQuery += " ORDER BY AI.date_added desc ";
            strQuery += " LIMIT 15";

            QVariantList data;
            sqlite->exec(strQuery, data);

            if(data.size() > 0){
                for(int i = 0 ; i < data.size(); i++){
                    QMap<QString, QVariant> map = data.at(i).toMap();

                    QString tmpMime = map["mime_type"].toString();
                    int tmpbitDepth = map["bitdepth"].toInt();
                    int tmpsampleRate = map["samplerate"].toInt();

                    if(!tmpMime.isEmpty()){
                        QString tmp_data = tmpMime;
                        QStringList splitToken = tmp_data.split("/");

                        if(splitToken.size() > 1){
                            QString tmp_type = splitToken.at(1);
                            if(tmp_type == "x-wav" || tmp_type == "wav"){
                                tmpMime = "WAV";
                            }
                            else if (tmp_type == "wv") {
                                tmpMime = "WV";
                            }
                            else if(tmp_type == "flac"){
                                tmpMime = "FLAC";
                            }
                            else if(tmp_type == "dff" || tmp_type == "dsf"){
                                tmpMime = "DSD";
                            }
                            else if(tmp_type == "mp4"){
                                tmpMime = "ALAC";
                            }
                            else if(tmp_type == "mpeg"){
                                tmpMime = "MP3";
                            }
                            else if(tmp_type == "amr"){
                                tmpMime = "AMR";
                            }
                            else if(tmp_type == "amr-wb"){
                                tmpMime = "AWB";
                            }
                            else if(tmp_type == "x-ms-wma"){
                                tmpMime = "WMA";
                            }
                            else if(tmp_type == "ape"){
                                tmpMime = "APE";
                            }
                            else if(tmp_type == "ogg"){
                                tmpMime = "OGG";
                            }
                            else if(tmp_type == "aac" || tmp_type == "aac-adts"){
                                tmpMime = "AAC";
                            }
                            else if(tmp_type == "x-matroska"){
                                tmpMime = "MKA";
                            }
                            else if(tmp_type == "midi"){
                                tmpMime = "MIDI";
                            }
                            else if(tmp_type == "sp-midi"){
                                tmpMime = "SMF";
                            }
                            else if(tmp_type == "imelody"){
                                tmpMime = "IMY";
                            }
                            else if(tmp_type == "x-mpegurl"){
                                tmpMime = "M3U8";
                            }
                            else if(tmp_type == "x-scpls"){
                                tmpMime = "PLS";
                            }
                            else if(tmp_type == "aiff"){
                                tmpMime = "AIFF";
                            }
                        }
                    }

                    QString mimeType = "";
                    if(tmpMime == "MP3" || tmpMime == "AMR" || tmpMime == "AWB" || tmpMime == "WMA" || tmpMime == "APE" || tmpMime == "OGG" || tmpMime == "AAC" || tmpMime == "MKA"
                            || tmpMime == "MIDI" || tmpMime == "SMF" || tmpMime == "IMY" || tmpMime == "M3U8" || tmpMime == "PLS" || tmpMime == "AIFF" || tmpMime == "WV"){

                        mimeType = tmpMime;
                    }
                    else if(tmpMime == "DSD"){
                        if(tmpsampleRate == 88200 || tmpsampleRate == 2822400){
                            mimeType = tmpMime + " 64";
                        }
                        else if(tmpsampleRate == 176400 || tmpsampleRate == 5644800){
                            mimeType = tmpMime + " 128";
                        }
                        else if(tmpsampleRate == 352800 || tmpsampleRate == 11289600){
                            mimeType = tmpMime + " 256";
                        }
                        else if(tmpsampleRate == 705600 || tmpsampleRate == 22579200){
                            mimeType = tmpMime + " 512";
                        }
                        else if(tmpsampleRate == 1411200 || tmpsampleRate == 45158400){
                            mimeType = tmpMime + " 1024";
                        }
                        else{
                            mimeType = tmpMime;
                        }
                    }
                    else if(tmpMime == "WAV" || tmpMime == "FLAC" || tmpMime == "ALAC"){
                        if(tmpbitDepth == 0 || tmpbitDepth == 16){
                            mimeType = tmpMime;
                        }
                        else if(tmpbitDepth > 16){
                            mimeType = tmpMime + QString(" %1").arg(tmpbitDepth);
                        }
                    }

                    QString tmpImg = map["album_art"].toString();
                    if(!tmpImg.isEmpty()){
                        tmpImg = "http://" + global.device.getDeviceIP() + ":" + global.port_img + map["album_art"].toString();
                    }

                    roseHome::AlbumItemData data_output;
                    data_output.id = map["album_id"].toInt();
                    //data_output.totalCount = ProcJsonEasy::getInt(jsonObj, "totalCount");
                    data_output.trackCount = map["num_of_song"].toInt();

                    data_output.type = "MUSIC";
                    data_output.title = map["album"].toString();
                    data_output.artist = map["artist"].toString();
                    data_output.playTime = map["album_art"].toString();
                    data_output.thumbnail = tmpImg;
                    data_output.mime = mimeType;

                    this->list_RecentAdds->append(data_output);
                }
            }
        }

        if(err.type() == QSqlError::NoError){

            QString strQuery = "";
            strQuery = " SELECT AI._id AS id, AI.artist, AI.number_of_albums, AI.number_of_tracks, ";
            strQuery += " (SELECT _data FROM album_art WHERE album_id=M.album_id) AS album_art";
            strQuery += " FROM artist_info AS AI INNER JOIN artists_albums_map AS M ON AI._id=M.artist_id ";
            strQuery += " GROUP BY AI._id ";
            strQuery += " ORDER BY random() LIMIT 100";

            QVariantList data;
            sqlite->exec(strQuery, data);

            if(data.size() > 0){
                for(int i = 0 ; i < data.size(); i++){
                    this->jsonArr_artists.append(data.at(i).toJsonObject());

                    QMap<QString, QVariant> map = data.at(i).toMap();

                    if(i < 15){
                        QString tmpImg = map["album_art"].toString();
                        if(!tmpImg.isEmpty()){
                            tmpImg = "http://" + global.device.getDeviceIP() + ":" + global.port_img + map["album_art"].toString();
                        }

                        roseHome::ArtistItemData data_output;
                        data_output.id = map["id"].toInt();
                        data_output.imageUrl = tmpImg;
                        data_output.name = map["artist"].toString();
                        data_output.albums_count = map["number_of_albums"].toInt();
                        data_output.tracks_count = map["number_of_tracks"].toInt();
                        data_output.artist_total_cnt = data.size();

                        this->list_RecommandArtist->append(data_output);
                    }
                }
            }
        }

        sqlite->close();
        delete sqlite;

        print_debug();ContentLoadingwaitingMsgHide();//c230322_3

        this->setUIControl_appendWidget();
    }


    void MusicHome::setUIControl_appendWidget_rose(){

        if(this->flag_album[0] == true && this->flag_playlist[0] == true && this->flag_track[0] == true && this->flag_historylist[0] == true && this->flag_myPlaylist[0] == true && this->flag_userPlaylist[0] == true)
        {
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//c230322_3

            if(this->flag_album[0] == true){
                this->flag_album[0] = false;

                this->widget_recentPlay = this->setUIControl_subTitle_withSubMenu(tr("Recently played"), "View All", BTN_IDX_SUBTITLE_recentlyPlay, this->vBox_recentlyPlay);

                this->vBox_recentlyPlay->addSpacing(10);
                this->vBox_recentlyPlay->addWidget(this->stackRecent, 1, Qt::AlignTop);

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
                this->stackRecent->addWidget(this->album_scrollArea);

                if(this->flag_album[1] == true){
                    this->flag_album[1] = false;

                    int maxCount = 0;
                    if(this->list_recentlyAlbum->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_recentlyAlbum->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_album[i]->setData(this->list_recentlyAlbum->at(i));
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_recently_album[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &MusicHome::slot_clickedItemAlbum);
                        this->hBox_recentlyAlbum->addWidget(this->home_recently_album[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                    noData_widget->setFixedSize(1500, 311);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_recentlyAlbum->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_recentlyPlay);
                this->box_rose_contents->addSpacing(45);
            }

            if(this->flag_playlist[0] == true){

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
                this->stackRecent->addWidget(this->playlist_scrollArea);

                if(this->flag_playlist[1] == true){
                    this->flag_playlist[1] = false;

                    int maxCount = 0;
                    if(this->list_recentlyPlaylist->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_recentlyPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_playlist[i]->setData(this->list_recentlyPlaylist->at(i));
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_recently_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &MusicHome::slot_clickedItemPlaylist);
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

            if(this->flag_track[0] == true){
                this->flag_track[0] = false;

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

                if(this->flag_track[1] == true){
                    this->flag_track[1] = false;

                    int maxCount = 0;
                    if(this->list_recentlytrack->size() > 5){
                        maxCount = 5;
                    }
                    else{
                        maxCount = this->list_recentlytrack->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_track[i]->setDataTrackInfo_RoseMain(this->list_recentlytrack->at(i));
                        this->home_recently_track[i]->setFavoritesIds(this->list_recentlytrack->at(i).favorite, this->list_recentlytrack->at(i).star);
                        this->home_recently_track[i]->resize(1550, 70);
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_recently_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &MusicHome::slot_clickedItemTrack_inList);
                        this->vBox_recentlyTrack_info->addWidget(this->home_recently_track[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
                    noData_widget->setFixedSize(1500, 300);
                    noData_widget->setObjectName("Nodata");

                    this->vBox_recentlyTrack_info->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_recentlyTrack);
                this->box_rose_contents->addSpacing(65);
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
                        connect(this->home_historylist[i], &roseHome::ItemHistory_rosehome::signal_clicked, this, &MusicHome::slot_clickedItemAlbum);
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
                this->flag_myPlaylist[0] = false;

                this->widget_myPlaylist = new QWidget();
                if(this->list_myPlaylist->size() > 0){
                    this->widget_myPlaylist = this->setUIControl_subTitle_withSideBtn(tr("My playlists") + QString(" (%1)").arg(this->list_myPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_myPlaylists, this->vBox_myPlaylist);
                }
                else{
                    this->widget_myPlaylist = this->setUIControl_subTitle_withSideBtn(tr("My playlists") + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_myPlaylists, this->vBox_myPlaylist);
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

                    int maxCount = 0;
                    if(this->list_myPlaylist->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_myPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_myPlaylist[i]->setData(this->list_myPlaylist->at(i));
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_myPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &MusicHome::slot_clickedItemPlaylist);
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
                this->box_rose_contents->addSpacing(50);
            }

            if(this->flag_userPlaylist[0] == true){
                this->flag_userPlaylist[0] = false;

                /*this->widget_userPlaylist = new QWidget();
                if(this->list_userPlaylist->size() > 0){
                    this->widget_userPlaylist = this->setUIControl_subTitle_withSideBtn(tr("Music Playlist On Rose") + QString(" (%1)").arg(this->list_userPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_userPlaylists, this->vBox_userPlaylist);
                }
                else{
                    this->widget_userPlaylist = this->setUIControl_subTitle_withSideBtn(tr("Music Playlist On Rose") + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_userPlaylists, this->vBox_userPlaylist);
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
                userPlaylist_scrollArea->setFixedHeight(305);

                QScroller::grabGesture(userPlaylist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_userPlaylist->addWidget(userPlaylist_scrollArea);

                if(this->flag_userPlaylist[1] == true){
                    this->flag_userPlaylist[1] = false;

                    int maxCount = 0;
                    if(this->list_userPlaylist->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_userPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_userPlaylist[i]->setData(this->list_userPlaylist->at(i));
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_userPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &MusicHome::slot_clickedItemPlaylist);
                        this->hBox_userPlaylist->addWidget(this->home_userPlaylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 305);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_userPlaylist->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_userPlaylist);
                this->box_rose_contents->addSpacing(50);*/

                this->setUIControl_requestMusic();
            }
            print_debug();ContentLoadingwaitingMsgHide();//c230322_3
        }
    }


    void MusicHome::setUIControl_checkWidget_rose(){

        if(this->flag_recentAlbum_check[0] == true && this->flag_recentPlaylist_check[0] == true && this->flag_recentTrack_check[0] == true
                && this->flag_myPlaylist_check[0] == true){  // && this->flag_recentArtist_check[0] == true){
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//c230322_3

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

                int maxCount = 0;
                if(this->list_recentlyAlbum->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_recentlyAlbum->size();
                }

                if(maxCount > 0){
                    for(int i = 0; i < 15; i++){
                        this->home_recently_album[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___recentlyAlbum, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_album[i]->setData(this->list_recentlyAlbum->at(i));
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_recently_album[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &MusicHome::slot_clickedItemAlbum);
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

                int maxCount = 0;
                if(this->list_recentlyPlaylist->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_recentlyPlaylist->size();
                }

                if(maxCount > 0){
                    for(int i = 0; i < 15; i++){
                        this->home_recently_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___recentlyPlaylist, tidal::AbstractItem::ImageSizeMode::Square_200x200, 2, true);
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_playlist[i]->setData(this->list_recentlyPlaylist->at(i));
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_recently_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &MusicHome::slot_clickedItemPlaylist);
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
                if(this->list_recentlytrack->size() > 0){
                    this->lb_subTitle[BTN_IDX_SUBTITLE_recentlyTrack]->setText(subTitle + QString(" (%1)").arg(this->list_recentlytrack->at(0).totalCount));
                }
                else{
                    this->lb_subTitle[BTN_IDX_SUBTITLE_recentlyTrack]->setText(subTitle + QString(" (0)"));
                }

                int maxCount = 0;
                if(this->list_recentlytrack->size() > 5){
                    maxCount = 5;
                }
                else{
                    maxCount = this->list_recentlytrack->size();
                }

                if(maxCount > 0){
                    for (int i = 0; i < maxCount; i++) {
                        this->home_recently_track[i] = new PlaylistTrackDetailInfo_RHV();
                        this->home_recently_track[i]->setProperty("index", i);
                        this->home_recently_track[i]->setObjectName("recently track");
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_track[i]->setDataTrackInfo_RoseMain(this->list_recentlytrack->at(i));
                        this->home_recently_track[i]->setFavoritesIds(this->list_recentlytrack->at(i).favorite, this->list_recentlytrack->at(i).star);
                        this->home_recently_track[i]->resize(1550, 70);
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_recently_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &MusicHome::slot_clickedItemTrack_inList);
                        this->vBox_recentlyTrack_info->addWidget(this->home_recently_track[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
                    noData_widget->setFixedSize(1500, 350);
                    noData_widget->setObjectName("Nodata");

                    vBox_recentlyTrack_info->addWidget(noData_widget);
                }
            }

            /*if(this->flag_recentArtist_check[1] == true){
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
                if(this->list_recentlyArtist->size() > 0){
                    this->lb_subTitle[BTN_IDX_SUBTITLE_recentlyArtist]->setText(subTitle + QString(" (%1)").arg(this->list_recentlyArtist->at(0).totalCount));
                }
                else{
                    this->lb_subTitle[BTN_IDX_SUBTITLE_recentlyArtist]->setText(subTitle + QString(" (0)"));
                }

                int maxCount = 0;
                if(this->list_RecentArtist->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_RecentArtist->size();
                }

                if(maxCount > 0){
                    for(int i = 0; i < 15; i++){
                        this->home_recently_artist[i] = new roseHome::ItemArtist_rosehome(i, SECTION_FOR_MORE_POPUP___RecentlyArtist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                        connect(this->home_recently_artist[i], &roseHome::ItemArtist_rosehome::signal_clicked, this, &MusicHome::slot_clickedItemArtist);
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently_artist[i]->setData(this->list_RecentArtist->at(i));
                        this->hBox_recentlyArtist->addWidget(this->home_recently_artist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Artist_NoData);
                    noData_widget->setFixedSize(1500, 260);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_recentlyArtist->addWidget(noData_widget);
                }
            }*/

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
                if(this->list_myPlaylist->size() > 0){
                    this->lb_subTitle[BTN_IDX_SUBTITLE_myPlaylists]->setText(subTitle + QString(" (%1)").arg(this->list_myPlaylist->at(0).totalCount));
                }
                else{
                    this->lb_subTitle[BTN_IDX_SUBTITLE_myPlaylists]->setText(subTitle + QString(" (0)"));
                }

                int maxCount = 0;
                if(this->list_myPlaylist->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_myPlaylist->size();
                }

                if(maxCount > 0){
                    for(int i = 0; i < maxCount; i++){
                        this->home_myPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, BTN_IDX_SUBTITLE_myPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_myPlaylist[i]->setData(this->list_myPlaylist->at(i));
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->home_myPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &MusicHome::slot_clickedItemPlaylist);
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


    void MusicHome::setUIControl_appendWidget(){

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//c230322_3

        if(this->list_RecentAdds->size() > 0){
            this->widget_recentlyAdds = new QWidget();
            this->widget_recentlyAdds = this->setUIControl_subTitle_withSideBtn(tr("Recently Added Albums"), "View All", BTN_IDX_SUBTITLE_recentlyAdds, this->vBox_recentlyAdds);

            this->vBox_recentlyAdds->addSpacing(10);

            this->hBox_recentlyAdds = this->setUIControl_hBoxLayout_forAlbum(this->vBox_recentlyAdds);

            for(int i = 0; i < this->list_RecentAdds->size(); i++){
                this->home_recently_Add[i]->setData(this->list_RecentAdds->at(i));
                QCoreApplication::processEvents();
            }

            for(int i = 0; i < this->list_RecentAdds->size(); i++){
                connect(this->home_recently_Add[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &MusicHome::slot_clickedItemAlbum);
                this->hBox_recentlyAdds->addWidget(this->home_recently_Add[i]);
            }

            this->box_rose_contents->addLayout(this->vBox_recentlyAdds);
            this->box_rose_contents->addSpacing(45);
        }
        else{
            this->widget_recentlyAdds = new QWidget();
            this->widget_recentlyAdds = this->setUIControl_subTitle_withSideBtn(QString("Recently Added Albums"), "View All", BTN_IDX_SUBTITLE_recentlyAdds, this->vBox_recentlyAdds);

            this->vBox_recentlyAdds->addSpacing(10);

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
            noData_widget->setFixedSize(1500, 285);

            this->vBox_recentlyAdds->addWidget(noData_widget);

            this->box_rose_contents->addLayout(this->vBox_recentlyAdds);
            this->box_rose_contents->addSpacing(45);
        }

        if(this->list_RecommandArtist->size() > 0){
            this->widget_recommandArtist = new QWidget();
            this->widget_recommandArtist = this->setUIControl_subTitle_withSideBtn(tr("Recommand Artists"), "View All", BTN_IDX_SUBTITLE_recommandArtist, this->vBox_recommandArtist);

            this->vBox_recommandArtist->addSpacing(10);

            this->hBox_recommandArtist = this->setUIControl_hBoxLayout_forArtist(this->vBox_recommandArtist);

            for(int i = 0; i < this->list_RecommandArtist->size(); i++){
                this->home_recommand_Artist[i]->setData(this->list_RecommandArtist->at(i));
                QCoreApplication::processEvents();
            }

            for(int i = 0; i < this->list_RecommandArtist->size(); i++){
                connect(this->home_recommand_Artist[i], &roseHome::ItemArtist_rosehome::signal_clicked, this, &MusicHome::slot_clickedItemArtist);
                this->hBox_recommandArtist->addWidget(this->home_recommand_Artist[i]);
            }

            this->box_rose_contents->addLayout(this->vBox_recommandArtist);
            this->box_rose_contents->addSpacing(45);
        }
        else{
            this->widget_recommandArtist = new QWidget();
            this->widget_recommandArtist = this->setUIControl_subTitle_withSideBtn(QString("Recommand Artists"), "View All", BTN_IDX_SUBTITLE_recommandArtist, this->vBox_recommandArtist);

            this->vBox_recommandArtist->addSpacing(10);

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Artist_NoData);
            noData_widget->setFixedSize(1500, 285);

            this->vBox_recommandArtist->addWidget(noData_widget);

            this->box_rose_contents->addLayout(this->vBox_recommandArtist);
            this->box_rose_contents->addSpacing(45);
        }

        this->slot_hide_msg();
        ContentLoadingwaitingMsgHide();
    }


    QWidget* MusicHome::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QWidget* MusicHome::setUIControl_subTitle_withSubMenu(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QHBoxLayout* MusicHome::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

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


    QHBoxLayout* MusicHome::setUIControl_hBoxLayout_forArtist(QLayout *p_layout){
        //----------------------------------------------------------------------------------------------------  BODY : START
        QHBoxLayout *hBox_album = new QHBoxLayout();
        hBox_album->setSpacing(0);
        hBox_album->setContentsMargins(0,0,0,0);
        hBox_album->setAlignment(Qt::AlignTop);
        hBox_album->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_content = new QWidget();
        widget_content->setLayout(hBox_album);
        widget_content->setContentsMargins(0,0,0,0);

        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_content);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(285);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        QHBoxLayout *hBox_total = new QHBoxLayout();
        hBox_total->setSpacing(0);
        hBox_total->setContentsMargins(0,0,0,0);
        hBox_total->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        hBox_total->setSizeConstraint(QLayout::SetFixedSize);
        hBox_total->addWidget(tmp_scrollArea);

        // return
        return hBox_album;
    }


    /**
     * @brief Playlist 전용으로, 1 Row 형태의 가로스크롤 배치하는 Layout 구조 생성 후 반환
     * @return
     */
    QHBoxLayout* MusicHome::setUIControl_hBoxLayout_forPlaylists(QLayout *p_layout){

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

    void MusicHome::slot_applyResult_recentlyAlbum(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void MusicHome::slot_applyResult_recentlyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void MusicHome::slot_applyResult_recentlyTrack(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void MusicHome::slot_applyResult_historylist(const QList<roseHome::HistoryItemData> &list_data, const QJsonArray &jsonArr_dataToPlay){

        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.length() > 0)
        {
            this->list_Historylist->append(list_data);
            this->flag_historylist[1] = true;
        }

        this->flag_historylist[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void MusicHome::slot_applyResult_myPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void MusicHome::slot_applyResult_userPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(list_data);
        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        /*if(list_data.length() > 0)
        {
            this->list_userPlaylist->append(list_data);
            this->flag_userPlaylist[1] = true;
        }*/

        this->flag_userPlaylist[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void MusicHome::slot_applyResult_recentlyAlbumCheck(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_recentlyAlbum->count() == 0){
                this->list_recentlyAlbum->clear();
                this->list_recentlyAlbum->append(list_data);

                this->flag_recentAlbum_check[1] = true;
            }
            else{
                int maxCount = (list_data.count() > this->list_recentlyAlbum->count()) ? this->list_recentlyAlbum->count() : list_data.count();

                for(int i = 0; i < maxCount; i++){
                    if(list_data.at(i).id != this->list_recentlyAlbum->at(i).id || list_data.at(i).title != this->list_recentlyAlbum->at(i).title || list_data.at(i).star != this->list_recentlyAlbum->at(i).star){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_recentlyAlbum->at(0).totalCount) || (change_flag > 0)){
                    this->list_recentlyAlbum->clear();
                    this->list_recentlyAlbum->append(list_data);

                    this->flag_recentAlbum_check[1] = true;
                }
            }
        }
        else if(list_data.size() == 0 && this->list_recentlyAlbum->count() != 0){
            this->list_recentlyAlbum->clear();

            this->flag_recentAlbum_check[1] = true;
        }

        this->flag_recentAlbum_check[0] = true;
        this->setUIControl_checkWidget_rose();
    }


    void MusicHome::slot_applyResult_recentlyPlaylistCheck(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_recentlyPlaylist->count() == 0){
                this->list_recentlyPlaylist->clear();
                this->list_recentlyPlaylist->append(list_data);

                this->flag_recentPlaylist_check[1] = true;
            }
            else{
                int maxCount = (list_data.count() > this->list_recentlyPlaylist->count()) ? this->list_recentlyPlaylist->count() : list_data.count();

                for(int i = 0; i < maxCount; i++){
                    if(list_data.at(i).id != this->list_recentlyPlaylist->at(i).id || list_data.at(i).title != this->list_recentlyPlaylist->at(i).title || list_data.at(i).star != this->list_recentlyPlaylist->at(i).star){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_recentlyPlaylist->at(0).totalCount) || (change_flag > 0)){
                    this->list_recentlyPlaylist->clear();
                    this->list_recentlyPlaylist->append(list_data);

                    this->flag_recentPlaylist_check[1] = true;
                }
            }
        }
        else if(list_data.size() == 0 && this->list_recentlyPlaylist->count() != 0){
            this->list_recentlyPlaylist->clear();

            this->flag_recentPlaylist_check[1] = true;
        }

        this->flag_recentPlaylist_check[0] = true;
        this->setUIControl_checkWidget_rose();
    }


    void MusicHome::slot_applyResult_recentlyTrackCheck(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_recentlytrack->count() == 0){
                this->list_recentlytrack->clear();
                this->jsonArr_tracks_toPlay = QJsonArray();

                this->list_recentlytrack->append(list_data);
                this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;

                this->flag_recentTrack_check[1] = true;
            }
            else{
                int maxCount = (list_data.count() > this->list_recentlytrack->count()) ? this->list_recentlytrack->count() : list_data.count();

                for(int i = 0; i < maxCount; i++){
                    if(list_data.at(i).id != this->list_recentlytrack->at(i).id || list_data.at(i).title != this->list_recentlytrack->at(i).title || list_data.at(i).star != this->list_recentlytrack->at(i).star){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_recentlytrack->at(0).totalCount) || (change_flag > 0)){
                    this->list_recentlytrack->clear();
                    this->jsonArr_tracks_toPlay = QJsonArray();

                    this->list_recentlytrack->append(list_data);
                    this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;

                    this->flag_recentTrack_check[1] = true;
                }
            }
        }
        else if(list_data.size() == 0 && this->list_recentlytrack->count() != 0){
            this->list_recentlytrack->clear();
            this->jsonArr_tracks_toPlay = QJsonArray();

            this->flag_recentTrack_check[1] = true;
        }

        this->flag_recentTrack_check[0] = true;
        this->setUIControl_checkWidget_rose();
    }


    void MusicHome::slot_applyResult_myPlaylistCheck(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_myPlaylist->count() == 0){
                this->list_myPlaylist->clear();
                this->list_myPlaylist->append(list_data);

                this->flag_myPlaylist_check[1] = true;
            }
            else{
                int maxCount = (list_data.count() > this->list_myPlaylist->count()) ? this->list_myPlaylist->count() : list_data.count();

                for(int i = 0; i < maxCount; i++){
                    if((list_data.at(i).id != this->list_myPlaylist->at(i).id) || (list_data.at(i).title != this->list_myPlaylist->at(i).title) || (list_data.at(i).star != this->list_myPlaylist->at(i).star)){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_myPlaylist->at(0).totalCount) || (change_flag > 0)){
                    this->list_myPlaylist->clear();
                    this->list_myPlaylist->append(list_data);

                    this->flag_myPlaylist_check[1] = true;
                }
            }
        }
        else if(list_data.size() == 0 && this->list_myPlaylist->size() != 0){
            this->list_myPlaylist->clear();

            this->flag_myPlaylist_check[1] = true;
        }

        this->flag_myPlaylist_check[0] = true;
        this->setUIControl_checkWidget_rose();
    }


    void MusicHome::slot_applyResult_myPlaylistDelete(const QJsonObject &jsonObj){

        if(jsonObj.value("flagOk").toBool() == true && jsonObj.value("message").toString() == "정상"){
            this->flag_recentAlbum_check[0] = true;
            this->flag_recentPlaylist_check[0] = true;
            this->flag_recentTrack_check[0] = true;
            //this->flag_recentArtist_check[0] = true;
            this->flag_myPlaylist_check[0] = true;

            this->flag_recentAlbum_check[1] = false;
            this->flag_recentPlaylist_check[1] = false;
            this->flag_recentTrack_check[1] = false;
            //this->flag_recentArtist_check[1] = false;
            this->flag_myPlaylist_check[1] = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
            connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &MusicHome::slot_applyResult_myPlaylistCheck);
            proc_myPlaylist->request_rose_getList_myPlaylists(ROSE_API_MYPLAYLIST_PATH , "MUSIC", 0, 15);
        }
    }


    void MusicHome::slot_applyResult_getRating_track(const QJsonArray &contents){

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
    void MusicHome::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_recentlyPlay){
            int list_count = 0;
            if(this->contentStep == ALBTAB_STEP_ALBUM){

                if(this->list_recentlyAlbum->count() > 0){
                    list_count = this->list_recentlyAlbum->at(0).totalCount;
                }

                QJsonObject jsonObj;
                jsonObj.insert("pathTitle", this->lb_subTitle[btnId]->text() + tr(" Album ") + QString("(%1)").arg(list_count));
                jsonObj.insert("api_subPath", "member/album/recent");
                jsonObj.insert("type", "RecentAlbum");
                jsonObj.insert(KEY_PAGE_CODE, PAGECODE_VA_PLAYALBUMALL);

                emit linker->signal_clickedMovePage(jsonObj);
            }
            else if(this->contentStep == ALBTAB_STEP_PLAYLIST){

                if(this->list_recentlyPlaylist->count() > 0){
                    list_count = this->list_recentlyPlaylist->at(0).totalCount;
                }

                QJsonObject jsonObj;
                jsonObj.insert("pathTitle", this->lb_subTitle[btnId]->text() + tr(" Playlist ") + QString("(%1)").arg(list_count));
                jsonObj.insert("api_subPath", "member/playlist/recent");
                jsonObj.insert("type", "RecentPlaylist");
                jsonObj.insert(KEY_PAGE_CODE, PAGECODE_VA_PLAYPLAYLISTALL);

                emit linker->signal_clickedMovePage(jsonObj);
            }
        }
        else if(btnId == BTN_IDX_SUBTITLE_recentlyTrack){

            QJsonObject jsonObj;
            jsonObj.insert("pathTitle", this->lb_subTitle[btnId]->text());
            jsonObj.insert("api_subPath", "member/track/recent");
            jsonObj.insert("type", "RecentTrack");
            jsonObj.insert(KEY_PAGE_CODE, PAGECODE_VA_PLAYTRACKALL);

            emit linker->signal_clickedMovePage(jsonObj);
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
            jsonObj.insert("filter_type", "MUSIC");
            jsonObj.insert("histories", histories);
            jsonObj.insert(KEY_PAGE_CODE, PAGECODE_VA_HISTORY_LIST_VIEW);

            emit linker->signal_clickedMovePage(jsonObj);
        }
        else if(btnId == BTN_IDX_SUBTITLE_myPlaylists){

            QJsonObject jsonObj;
            jsonObj.insert("pathTitle", tr("My Playlists ") + QString("(%1)").arg(this->list_myPlaylist->at(0).totalCount));
            jsonObj.insert("api_subPath", "member/playlist");
            jsonObj.insert("type", "MyPlaylist");
            jsonObj.insert(KEY_PAGE_CODE, PAGECODE_VA_MYPLAYLIST);

            emit linker->signal_clickedMovePage(jsonObj);
            //emit this->signal_clickedViewAll(PAGECODE_VA_MYPLAYLIST);
        }
        else if(btnId == BTN_IDX_SUBTITLE_userPlaylists){

            QJsonObject jsonObj;
            jsonObj.insert("pathTitle", tr("Music Playlist On Rose ") + QString("(%1)").arg(this->list_userPlaylist->at(0).totalCount));
            jsonObj.insert("api_subPath", "member/playlist/all");
            jsonObj.insert("type", "UserPlaylist");
            jsonObj.insert(KEY_PAGE_CODE, PAGECODE_VA_USERPLAYLIST);

            emit linker->signal_clickedMovePage(jsonObj);
            //emit this->signal_clickedViewAll(PAGECODE_VA_MYPLAYLIST);
        }
        else if(btnId == BTN_IDX_SUBTITLE_recentlyAdds){

            emit this->signal_clickedViewAll(PAGECODE_VA_ADDEDALBUM);
        }
        else if(btnId == BTN_IDX_SUBTITLE_recommandArtist){

            QJsonObject jsonObj;
            jsonObj.insert("title", "Recommand Artists");
            jsonObj.insert("data", this->jsonArr_artists);
            jsonObj.insert(KEY_PAGE_CODE, PAGECODE_VA_RECOMMANDARTIST);

            emit linker->signal_clickedMovePage(jsonObj);
        }
    }


    void MusicHome::changedOnlyTabUI_notSendSignal(QString p_step){

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


    void MusicHome::slot_changedSubTabUI(const QJsonObject &p_data){

        QString tmp_step = p_data[KEY_PAGE_CODE].toString();

        // 페이지 변경 및 데이터 세팅
        this->changedOnlyTabUI_notSendSignal(tmp_step);
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
    void MusicHome::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리 - Album 파트가 여러개임

        if(section == SECTION_FOR_MORE_POPUP___recentlyAdds){

            roseHome::AlbumItemData data_album = this->list_RecentAdds->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Music_Album, roseHome::ConvertData::getConvertOptHeaderData(data_album), index, SECTION_FOR_MORE_POPUP___recentlyAdds, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){

                QJsonArray jsonArr_tracks = this->getDataForPlayMusic(data_album.id);
                // Album 바로 재생
                QJsonObject tmp_json;
                tmp_json.insert("musicPlayType", 15);
                tmp_json.insert("music", jsonArr_tracks);
                tmp_json.insert("currentPosition", 0);
                tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

                /*qDebug() << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song");
                QJsonDocument doc(tmp_json);
                QString strJson(doc.toJson(QJsonDocument::Compact));
                qDebug() << "jsonObj = " << strJson;*/

                if(global.Queue_track_count != 0) {
                    emit linker->signal_checkQueue(27, "");

                    return;
                }
                emit linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(0, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                // Album Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert(KEY_OP_TITLE_MAIN, data_album.title);
                jsonData.insert(KEY_OP_TITLE_SUB, "");
                jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Album);
                jsonData.insert(KEY_OP_albumImg, data_album.thumbnail);
                jsonData.insert(KEY_OP_MIME, data_album.mime);
                jsonData.insert(KEY_OP_ARTIST, data_album.artist);
                jsonData.insert(KEY_OP_cntStar, 0);
                jsonData.insert(KEY_OP_DATA, this->getDataForPlayMusic(data_album.id));
                jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);
                jsonData.insert("isAlbum", true);

                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ALBUMINFO);
                emit linker->signal_clickedMovePage(jsonData);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___recentlyAlbum){
            roseHome::AlbumItemData data_album = this->list_recentlyAlbum->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, roseHome::ConvertData::getConvertOptHeaderData(data_album), index, SECTION_FOR_MORE_POPUP___recentlyAlbum, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Album 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byAlbumID(data_album.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                // Album Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert("id", data_album.id);
                jsonData.insert("star", data_album.star);
                jsonData.insert("type", data_album.type);
                jsonData.insert("title", data_album.title);
                jsonData.insert("thumbnail", data_album.thumbnail);
                jsonData.insert("playTime", data_album.playTime);
                jsonData.insert("clientKey", data_album.clientKey);
                jsonData.insert("trackId", data_album.trackId);
                jsonData.insert("macAddress", data_album.macAddress);
                jsonData.insert("favorite", data_album.favorite);

                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_RECENTALBUMINFO);

                emit linker->signal_clickedMovePage(jsonData);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___History){

            roseHome::HistoryItemData tmpData;
            tmpData.yearMonth = this->list_Historylist->at(index).yearMonth;
            tmpData.visible = this->list_Historylist->at(index).visible;
            tmpData.filter_type = "MUSIC";

            //this->list_Historylist->replace(index, tmpData);

            if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // History 페이지 진입
                QJsonObject jsonObj_move = roseHome::ConvertData::getObjectJson_historyData(tmpData);
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_VA_HISTORY_DETAIL);

                emit linker->signal_clickedMovePage(jsonObj_move);
            }
        }
    }


    QJsonArray MusicHome::getDataForPlayMusic(const int album_id){

        QJsonArray jsonArray;

        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();
        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._display_name AS orderName, A._data AS data, A.title, A.artist, A.duration, A.bookmark, A.track, A.mime_type, A.samplerate, A.bitdepth, ART._data AS album_art ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            strQuery += " WHERE A.album_id=%1 ORDER BY A.bookmark ASC, A.track ASC, orderName ASC ";

            QVariantList dataDB;
            sqlite->exec(strQuery.arg(album_id), dataDB);
            if(dataDB.size() > 0){
                for(int i = 0 ; i < dataDB.size(); i++){
                    /*QMap<QString, QVariant> map = dataDB.at(i).toMap();

                    QString album = map["album"].toString();
                    QString album_art = map["album_art"].toString();
                    int album_ids = map["album_id"].toInt();
                    QString album_key = map["album_key"].toString();

                    QString artist = map["artist"].toString();
                    int artist_id = map["artist_id"].toInt();
                    QString artist_key = map["artist_key"].toString();

                    int bitdepth = map["bitdepth"].toInt();
                    int bookmark = map["bookmark"].toInt();
                    QString data = map["data"].toString();
                    int duration = map["duration"].toInt();
                    int id = map["id"].toInt();
                    QString mime_type = map["mime_type"].toString();
                    QString album_mime = map["album_mime"].toString();
                    int samplerate = map["samplerate"].toInt();
                    QString title = map["title"].toString();
                    int track = map["track"].toInt();

                    QJsonObject tmpData;
                    tmpData.insert("album", album);
                    tmpData.insert("album_art", album_art);
                    tmpData.insert("album_id", album_ids);
                    tmpData.insert("album_key", album_key);

                    tmpData.insert("artist", artist);
                    tmpData.insert("artist_id", artist_id);
                    tmpData.insert("artist_key", artist_key);

                    tmpData.insert("bitdepth", bitdepth);
                    tmpData.insert("bookmark", bookmark);
                    tmpData.insert("data", data);
                    tmpData.insert("duration", duration);
                    tmpData.insert("id", id);
                    tmpData.insert("mime_type", mime_type);
                    tmpData.insert("album_mime", album_mime);
                    tmpData.insert("samplerate", samplerate);
                    tmpData.insert("title", title);
                    tmpData.insert("track", track);

                    jsonArray.append(tmpData);*/

                    DataPopup *tmp_dataPopup = new DataPopup(this);
                    tmp_dataPopup->setJsonData(dataDB.at(i).toJsonObject());

                    jsonArray.append(tmp_dataPopup->getJsonData());
                }
            }
        }

        sqlite->close();
        delete sqlite;

        return jsonArray;
    }


    void MusicHome::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리 - Album 파트가 여러개임

        if(section == SECTION_FOR_MORE_POPUP___recommandArtist){

            roseHome::ArtistItemData data_artist = this->list_RecommandArtist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                SqliteHelper *sqlite = new SqliteHelper(this);
                QSqlError err = sqlite->addConnectionRose();

                QList<DataPopup*> *dataPopupList = new QList<DataPopup*>();
                if(err.type() == QSqlError::NoError){
                    QString strQuery = "";
                    strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
                    strQuery += " FROM audio A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
                    strQuery += " WHERE A.artist_id="+QString::number(data_artist.id);

                    QVariantList data;
                    sqlite->exec(strQuery, data);

                    for(int i = 0; i < data.size(); i++){
                        DataPopup *tmp_dataPopup = new DataPopup(this);
                        tmp_dataPopup->setJsonData(data.at(i).toJsonObject());
                        dataPopupList->append(tmp_dataPopup);
                    }
                }

                sqlite->close();
                delete sqlite;

                QJsonArray jsonArrAlbum;
                for(int i = 0; i < dataPopupList->size(); i++){
                    jsonArrAlbum.append(dataPopupList->at(i)->getJsonData());
                }

                // Artist Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert(KEY_OP_TITLE_MAIN, data_artist.name);
                jsonData.insert(KEY_OP_TITLE_SUB, "");
                jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Home_Artist);
                jsonData.insert(KEY_OP_albumImg, data_artist.imageUrl);
                jsonData.insert(KEY_OP_cntStar, 0);
                jsonData.insert(KEY_OP_DATA, jsonArrAlbum);
                jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);

                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ARTISTINFO);

                emit linker->signal_clickedMovePage(jsonData);
            }
        }
    }


    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void MusicHome::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___recentlyPlaylist){
            roseHome::PlaylistItemData data_playlist = this->list_recentlyPlaylist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data_playlist), index, SECTION_FOR_MORE_POPUP___recentlyPlaylist, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                // Playlist Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert("id", data_playlist.id);
                jsonData.insert("ownerId", data_playlist.ownerId);
                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_RECENTPLAYLISTINFO);

                emit linker->signal_clickedMovePage(jsonData);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___myPlaylists){
            roseHome::PlaylistItemData data_playlist = this->list_myPlaylist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data_playlist), index, SECTION_FOR_MORE_POPUP___myPlaylists, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                // Playlist Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert("id", data_playlist.id);
                jsonData.insert("ownerId", data_playlist.ownerId);
                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_RECENTPLAYLISTINFO);

                emit linker->signal_clickedMovePage(jsonData);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___userPlaylists){
            roseHome::PlaylistItemData data_playlist = this->list_userPlaylist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data_playlist), index, SECTION_FOR_MORE_POPUP___userPlaylists, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                // Playlist Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert("id", data_playlist.id);
                jsonData.insert("ownerId", data_playlist.ownerId);
                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_RECENTPLAYLISTINFO);

                emit linker->signal_clickedMovePage(jsonData);
            }
        }
    }


    void MusicHome::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

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
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeReq_rating_track, this, &MusicHome::slot_applyResult_getRating_track);
                proc_favCheck_track->request_rose_setRating_Track(jsonObj, this->flag_track_fav, this->track_star_fav);

                this->flag_check_track = true;
            }
        }
        else{
            // ClickMode 별로 처리
            roseHome::TrackItemData data = list_recentlytrack->at(idx);

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

                /*QJsonArray send_track_json = QJsonArray();
                send_track_json.append(this->jsonArr_tracks_toPlay.at(idx).toObject());

                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data, send_track_json, idx, OptMorePopup::ClickMode::Play_RightNow_withReorder);*/
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                // 플레이리스트 담기 - Track
                //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){
                // OptMorePopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Track, roseHome::ConvertData::getConvertOptHeaderData(data), idx, section);
            }
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void MusicHome::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___recentlyAdds){

            roseHome::AlbumItemData data_album = this->list_RecentAdds->at(index);

            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                int playType = 0;

                if(clickMode == OptMorePopup::ClickMode::Play_RightNow){
                    playType = 15;
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last){
                    playType = 13;
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty){
                    playType = 12;
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext){
                    playType = 16;
                }
                else{
                    playType = 15;
                }

                // Album 바로 재생
                QJsonArray jsonArr_tracks = this->getDataForPlayMusic(data_album.id);

                // Album 바로 재생
                QJsonObject tmp_json;
                tmp_json.insert("musicPlayType", playType);
                tmp_json.insert("music", jsonArr_tracks);
                tmp_json.insert("currentPosition", 0);
                tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

                /*qDebug() << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song");
                QJsonDocument doc(tmp_json);
                QString strJson(doc.toJson(QJsonDocument::Compact));
                qDebug() << "jsonObj = " << strJson;*/

                if(global.Queue_track_count != 0) {
                    print_debug();
                    emit linker->signal_checkQueue(27, "");

                    return;
                }
                print_debug();
                emit linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(0, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
            }
            else if(clickMode == OptMorePopup::ClickMode::Go_Artist){
                // artist Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Etc_ArtistDetailTrack);
                jsonData.insert(KEY_OP_albumImg, data_album.thumbnail);
                jsonData.insert(KEY_OP_DATA, this->getDataForPlayMusic(data_album.id));

                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ARTISTINFO);

                emit this->linker->signal_clickedMovePage(jsonData);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___recentlyAlbum){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_recentlyAlbum, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___recentlyPlaylist){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_recentlyPlaylist, index, SECTION_FOR_MORE_POPUP___recentlyPlaylist, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___recentlyTrack){
            this->proc_clicked_optMorePopup_fromTrack(this->list_recentlytrack, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___myPlaylists){
            if(clickMode == OptMorePopup::ClickMode::Edit){
                QString view_type = "edit";

                QJsonObject data;
                data.insert("view_type", view_type);
                data.insert("playlist_id", this->list_myPlaylist->at(index).id);
                data.insert("type", "ROSE");

                QJsonObject jsonObj_move;
                jsonObj_move.insert("data", data);

                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_M_ADDPLAYLIST);

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
                    connect(proc_myPlaylistDel, &roseHome::ProcCommon::completeReq_delete_playlist, this, &MusicHome::slot_applyResult_myPlaylistDelete);
                    proc_myPlaylistDel->request_rose_delete_myPlaylist(this->list_myPlaylist->at(index).id);
                }

                delete dlg;
            }
            else{
                this->proc_clicked_optMorePopup_fromPlaylist(this->list_myPlaylist, index, SECTION_FOR_MORE_POPUP___myPlaylists, clickMode);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___userPlaylists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_userPlaylist, index, SECTION_FOR_MORE_POPUP___userPlaylists, clickMode);
        }
    }


    void MusicHome::slot_musicRecentChanged(){
        /*GSCommon::clearLayout(this->boxRecentAlbum);
        requestRecenAlbum();*/
    }


    void MusicHome::setTopUIControl(QWidget *parent){

        int left = 0;
        int width = 0;

        // DB track, album, artist UI total;
        this->widget_db_info = new QWidget(parent);
        this->widget_db_info->setStyleSheet("background-color:transparent; border: 0px solid #FFFFFF");

        QPixmap *pixmap_album = GSCommon::getUIPixmapImg(":/images/music/home_album_ico.png");
        this->label_album = new QLabel(this->widget_db_info);
        this->label_album->setPixmap(*pixmap_album);
        this->label_album->setStyleSheet("background-color:transparent;");
        this->label_album->setGeometry(left, 0, pixmap_album->width(), this->widget_db_info->height());

        left = this->label_album->geometry().width();
        this->lb_album_cnt = new QLabel(this->widget_db_info);
        this->lb_album_cnt->setStyleSheet("background-color:transparent; font-size:18px; font-weight: normal;font-style: normal;line-height: 1.25;text-align: left; color:#AAAAAA;");
        this->lb_album_cnt->setText("");
        this->lb_album_cnt->setGeometry(left, 0, this->lb_album_cnt->sizeHint().width(), this->widget_db_info->height());

        left = this->lb_album_cnt->geometry().left() + this->lb_album_cnt->geometry().width() + 30;
        QPixmap *pixmap_track = GSCommon::getUIPixmapImg(":/images/music/home_song_ico.png");
        this->label_track = new QLabel(this->widget_db_info);
        this->label_track->setPixmap(*pixmap_track);
        this->label_track->setStyleSheet("background-color:transparent;");
        this->label_track->setGeometry(left, 0, pixmap_track->width(), this->widget_db_info->height());

        left = this->label_track->geometry().left() + this->label_track->geometry().width();
        this->lb_track_cnt = new QLabel(this->widget_db_info);
        this->lb_track_cnt->setStyleSheet("background-color:transparent; font-size:18px; font-weight: normal;font-style: normal;line-height: 1.25;text-align: left; color:#AAAAAA;");
        this->lb_track_cnt->setText("");
        this->lb_track_cnt->setGeometry(left, 0, this->lb_track_cnt->sizeHint().width(), this->widget_db_info->height());

        left = this->lb_track_cnt->geometry().left() + this->lb_track_cnt->geometry().width() + 30;
        QPixmap *pixmap_artist = GSCommon::getUIPixmapImg(":/images/music/home_artist_ico.png");
        this->label_artist = new QLabel(this->widget_db_info);
        this->label_artist->setPixmap(*pixmap_artist);
        this->label_artist->setStyleSheet("background-color:transparent;");
        this->label_artist->setGeometry(left, 0, pixmap_artist->width(), this->widget_db_info->height());

        left = this->label_artist->geometry().left() + this->label_artist->geometry().width();
        this->lb_artist_cnt = new QLabel(this->widget_db_info);
        this->lb_artist_cnt->setStyleSheet("background-color:transparent; font-size:18px; font-weight: normal;font-style: normal;line-height: 1.25;text-align: left; color:#AAAAAA;");
        this->lb_artist_cnt->setText("");
        this->lb_artist_cnt->setGeometry(left, 0, this->lb_artist_cnt->sizeHint().width(), this->widget_db_info->height());

        width = this->label_album->geometry().width() + this->lb_album_cnt->geometry().width() + 30 + this->label_track->geometry().left() + this->lb_track_cnt->geometry().width() + 30
                + this->label_artist->geometry().width() + this->lb_artist_cnt->geometry().width() + 100;
        this->widget_db_info->setGeometry(global.menu_width, 0, width, parent->height());//c230409
        this->widget_db_info->hide();



        // track, album, artist UI add
        /*QPixmap *pixmap_album = GSCommon::getUIPixmapImg(":/images/music/home_album_ico.png");
        QLabel *label_album = new QLabel;
        label_album->setPixmap(*pixmap_album);
        label_album->setStyleSheet("background-color:transparent;");
        label_album->resize(pixmap_album->width(), pixmap_album->height());

        this->lb_album_cnt = new QLabel;
        this->lb_album_cnt->setText("");
        this->lb_album_cnt->setStyleSheet("font-size:18px;color:#aaaaaa;background-color:transparent;");
        this->lb_album_cnt->setFixedWidth(120);

        QHBoxLayout *hl_album = new QHBoxLayout();
        hl_album->setContentsMargins(0,0,0,0);
        hl_album->setSpacing(0);
        hl_album->setAlignment(Qt::AlignVCenter);
        hl_album->addWidget(label_album);
        hl_album->addWidget(lb_album_cnt);

        this->widget_album = new QWidget(this->widget_db_info);
        this->widget_album->setContentsMargins(0,15,15,0);
        this->widget_album->setObjectName("widget_album");
        this->widget_album->setLayout(hl_album);
        this->widget_album->setStyleSheet("#widget_album{background-color:transparent;}");

        QPixmap *pixmap_track = GSCommon::getUIPixmapImg(":/images/music/home_song_ico.png");
        QLabel *label_track = new QLabel;
        label_track->setPixmap(*pixmap_track);
        label_track->setStyleSheet("background-color:transparent;");
        label_track->resize(pixmap_track->width(), pixmap_track->height());
        this->lb_track_cnt = new QLabel;
        this->lb_track_cnt->setText("");
        this->lb_track_cnt->setStyleSheet("font-size:18px;color:#aaaaaa;background-color:transparent;");
        this->lb_track_cnt->setFixedWidth(80);//c221213_1 //this->lb_track_cnt->setFixedWidth(120);

        QHBoxLayout *hl_track = new QHBoxLayout();
        hl_track->setContentsMargins(0,0,0,0);
        hl_track->setSpacing(0);
        hl_track->setAlignment(Qt::AlignVCenter);
        hl_track->addWidget(label_track);
        hl_track->addWidget(lb_track_cnt);

        this->widget_track = new QWidget(this->widget_db_info);
        this->widget_track->setContentsMargins(0,15,15,0);
        this->widget_track->setObjectName("widget_track");
        this->widget_track->setLayout(hl_track);
        this->widget_track->setStyleSheet("#widget_track{background-color:transparent;}");

        QPixmap *pixmap_artist = GSCommon::getUIPixmapImg(":/images/music/home_artist_ico.png");
        QLabel *label_artist = new QLabel;
        label_artist->setPixmap(*pixmap_artist);
        label_artist->setStyleSheet("background-color:transparent;");
        label_artist->resize(pixmap_artist->width(), pixmap_artist->height());
        this->lb_artist_cnt = new QLabel;
        this->lb_artist_cnt->setText("");
        this->lb_artist_cnt->setStyleSheet("font-size:18px;color:#aaaaaa;background-color:transparent;");
        this->lb_artist_cnt->setFixedWidth(20);//c221213_1 //this->lb_artist_cnt->setFixedWidth(120);

        QHBoxLayout *hl_artist = new QHBoxLayout();
        hl_artist->setContentsMargins(0,0,0,0);
        hl_artist->setSpacing(0);
        hl_artist->setAlignment(Qt::AlignVCenter);
        hl_artist->addWidget(label_artist);
        hl_artist->addWidget(lb_artist_cnt);

        this->widget_artist = new QWidget(this->widget_db_info);
        this->widget_artist->setContentsMargins(0,15,15,0);
        this->widget_artist->setObjectName("widget_artist");
        this->widget_artist->setLayout(hl_artist);
        this->widget_artist->setStyleSheet("#widget_artist{background-color:transparent;}");*/
    }


    void MusicHome::requestDataCount(){

        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            strQuery = "SELECT sum(numalbums),sum(numsongs),sum(numartists) FROM scans";

            QVariantList *list = new QVariantList;
            sqlite->exec(strQuery, *list);
            if(list->size() > 0){
                foreach(QVariant val, *list){
                    QMap<QString, QVariant> map = val.toMap();
                    this->lb_album_cnt->setText(map["sum(numalbums)"].toString());
                    this->lb_track_cnt->setText(map["sum(numsongs)"].toString());
                    this->lb_artist_cnt->setText(map["sum(numartists)"].toString());
                    break;
                }
            }
            else{
                this->lb_album_cnt->setText("-");
                this->lb_track_cnt->setText("-");
                this->lb_artist_cnt->setText("-");

                //ToastMsg::show(this, "", tr("Fialed to get DB Information."));
            }
        }

        sqlite->close();
        delete sqlite;

        this->show_topLabelControl();
    }


    void MusicHome::slot_topLabelChanged(){

        this->requestDataCount();
    }


    void MusicHome::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

        Q_UNUSED(p_id);
        Q_UNUSED(p_jsonObject);
/*
        if(p_id == 3344){//c220727
            if(ProcJsonEasy::get_flagOk(p_jsonObject)){
                print_debug();
                ToastMsg::show(this,"", tr("Play the selected YouTube content."), 2000, 0);

            }else{
                print_debug();
                ToastMsg::show(this,"", tr("Failed to perform playback of the selected YouTube content."), 2000, 0);

            }
            slot_overrideSigalSearch(false);

        }
        else if(p_id == 5678){//c220727
            if(ProcJsonEasy::get_flagOk(p_jsonObject)){
                qDebug() << "test";
                print_debug();
                this->le_search_back->clear();
                bool p_flagShuffle = false;
                QJsonArray jsonArrYoutube;
                //QJsonObject tmp_json;
                QJsonObject jsonTrackData;
                jsonTrackData.insert("playurl",global.dragDrop_pathStr);
                qDebug()<<"global.dragDrop_pathStr="<<global.dragDrop_pathStr;

                QString videoID = jsonTrackData["playurl"].toString().split("?v=").last();
                qDebug()<<"videoID="<<videoID;
                QJsonObject jsonYoutube;
                jsonYoutube.insert("channelId", p_jsonObject["channelId"].toString());
                jsonYoutube.insert("channelName", p_jsonObject["uploaderName"].toString());
                jsonYoutube.insert("duration", QString("%1").arg(p_jsonObject["duration"].toInt()));
                jsonYoutube.insert("id", videoID);
                jsonYoutube.insert("isFavorite", p_jsonObject["favorites"].toString()=="1" ? true : false);
                jsonYoutube.insert("isLiveStream",p_jsonObject["streamType"].toString()=="LIVE_STREAM" ? true : false);
                jsonYoutube.insert("thumbnailUrl", p_jsonObject["thumbnailUrl"].toString());
                jsonYoutube.insert("thumbsUpPercentage",0);
                jsonYoutube.insert("title", p_jsonObject["name"].toString());
                jsonYoutube.insert("viewsCount", QString("%1").arg(p_jsonObject["viewCount"].toInt()));

                jsonArrYoutube.append(jsonYoutube);
                QJsonObject jsonParam;
                jsonParam.insert("roseToken", global.device.getDeviceRoseToken());
                jsonParam.insert("shuffle", p_flagShuffle ? 1 : 0);
                jsonParam.insert("youtubePlayType", 15);
                jsonParam.insert("youtube", jsonArrYoutube);

                if(global.Queue_track_count != 0) {
                    print_debug();emit linker->signal_checkQueue(27, "");

                    return;
                }
                print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArrYoutube.count();     // 220419 queue count

                NetworkHttp *network = new NetworkHttp(this);
                connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));

                network->request(3344, QString("http://%1:%2/youtubePlay.playlist.add")
                                 .arg(global.device.getDeviceIP()).arg(global.port)
                                 , jsonParam, true);
            }else{
                print_debug();

            }
            slot_overrideSigalSearch(false);
        }
        */
        /*QJsonDocument doc(p_jsonObject);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << "jsonObj = " << strJson;*/
    }


    void MusicHome::resizeEvent(QResizeEvent *event){//c230409

        Q_UNUSED(event);

        //qDebug() << "[Debug]" << __FUNCTION__ << " line::" << __LINE__ << this->width();

        int left = 0;
        int width = 0;
        int spacing = 0;
        int w1150 = global.menu_width;
        print_debug();
        qDebug() << "w1150=" << w1150;
        qDebug() << "this->widget_db_info->width()=" << this->widget_db_info->width();
        if(this->width() <= w1150+this->widget_db_info->width()){
            this->widget_db_info->hide();
        }
        else{
            this->widget_db_info->hide();

            this->label_album->setGeometry(left, 0, this->label_album->sizeHint().width(), this->widget_db_info->height());

            left = this->label_album->geometry().width();
            this->lb_album_cnt->setGeometry(left, 0, this->lb_album_cnt->sizeHint().width(), this->widget_db_info->height());

            left = this->lb_album_cnt->geometry().left() + this->lb_album_cnt->geometry().width() + 30;
            this->label_track->setGeometry(left, 0, this->label_track->sizeHint().width(), this->widget_db_info->height());

            left = this->label_track->geometry().left() + this->label_track->geometry().width();
            this->lb_track_cnt->setGeometry(left, 0, this->lb_track_cnt->sizeHint().width(), this->widget_db_info->height());

            left = this->lb_track_cnt->geometry().left() + this->lb_track_cnt->geometry().width() + 30;
            this->label_artist->setGeometry(left, 0, this->label_artist->sizeHint().width(), this->widget_db_info->height());

            left = this->label_artist->geometry().left() + this->label_artist->geometry().width();
            this->lb_artist_cnt->setGeometry(left, 0, this->lb_artist_cnt->sizeHint().width(), this->widget_db_info->height());

            width = this->label_album->geometry().width() + this->lb_album_cnt->geometry().width() + 30 + this->label_track->geometry().width() + this->lb_track_cnt->geometry().width() + 30
                    + this->label_artist->geometry().width() + this->lb_artist_cnt->geometry().width() + 100;
            this->widget_db_info->setGeometry(w1150, 0, width, this->widget_parent->height());

            if(this->width() > (w1150 + width) && this->width() <= (w1150 + width + 100)){
                spacing = (this->width() - w1150 - width) / 2;

                left = 0;
                this->label_album->setGeometry(left, 0, this->label_album->sizeHint().width(), this->widget_db_info->height());

                left = this->label_album->geometry().width();
                this->lb_album_cnt->setGeometry(left, 0, this->lb_album_cnt->sizeHint().width(), this->widget_db_info->height());

                left = this->lb_album_cnt->geometry().left() + this->lb_album_cnt->geometry().width() + 30 + spacing;
                this->label_track->setGeometry(left, 0, this->label_track->sizeHint().width(), this->widget_db_info->height());

                left = this->label_track->geometry().left() + this->label_track->geometry().width();
                this->lb_track_cnt->setGeometry(left, 0, this->lb_track_cnt->sizeHint().width(), this->widget_db_info->height());

                left = this->lb_track_cnt->geometry().left() + this->lb_track_cnt->geometry().width() + 30 + spacing;
                this->label_artist->setGeometry(left, 0, this->label_artist->sizeHint().width(), this->widget_db_info->height());

                left = this->label_artist->geometry().left() + this->label_artist->geometry().width();
                this->lb_artist_cnt->setGeometry(left, 0, this->lb_artist_cnt->sizeHint().width(), this->widget_db_info->height());

                width = this->label_album->geometry().width() + this->lb_album_cnt->geometry().width() + 30 + spacing + this->label_track->geometry().width() + this->lb_track_cnt->geometry().width() + 30 + spacing
                        + this->label_artist->geometry().width() + this->lb_artist_cnt->geometry().width() + 100;
                this->widget_db_info->setGeometry(w1150, 0, width, this->widget_parent->height());

                //qDebug() << "[Debug]" << __FUNCTION__ << " line::" << __LINE__ << this->width() << width << spacing;/// /k
            }
            else if(this->width() > (w1150 + width + 100)){
                spacing = 50;

                left = 0;
                this->label_album->setGeometry(left, 0, this->label_album->sizeHint().width(), this->widget_db_info->height());

                left = this->label_album->geometry().width();
                this->lb_album_cnt->setGeometry(left, 0, this->lb_album_cnt->sizeHint().width(), this->widget_db_info->height());

                left = this->lb_album_cnt->geometry().left() + this->lb_album_cnt->geometry().width() + 30 + spacing;
                this->label_track->setGeometry(left, 0, this->label_track->sizeHint().width(), this->widget_db_info->height());

                left = this->label_track->geometry().left() + this->label_track->geometry().width();
                this->lb_track_cnt->setGeometry(left, 0, this->lb_track_cnt->sizeHint().width(), this->widget_db_info->height());

                left = this->lb_track_cnt->geometry().left() + this->lb_track_cnt->geometry().width() + 30 + spacing;
                this->label_artist->setGeometry(left, 0, this->label_artist->sizeHint().width(), this->widget_db_info->height());

                left = this->label_artist->geometry().left() + this->label_artist->geometry().width();
                this->lb_artist_cnt->setGeometry(left, 0, this->lb_artist_cnt->sizeHint().width(), this->widget_db_info->height());

                width = this->label_album->geometry().width() + this->lb_album_cnt->geometry().width() + 30 + spacing + this->label_track->geometry().width() + this->lb_track_cnt->geometry().width() + 30 + spacing
                        + this->label_artist->geometry().width() + this->lb_artist_cnt->geometry().width() + 100;

                left = this->width() - width;
                this->widget_db_info->setGeometry(left, 0, width, this->widget_parent->height());

                //qDebug() << "[Debug]" << __FUNCTION__ << " line::" << __LINE__ << this->width() << width << spacing;
            }

            this->widget_db_info->show();
        }


//        print_debug();
//        qDebug() << "this->lb_album_cnt->text().length()=" << this->lb_album_cnt->text().length();
//        qDebug() << "this->lb_album_cnt->text()=" << this->lb_album_cnt->text();
//        qDebug() << "this->width()=" << this->width();
//        this->lb_album_cnt->setFixedWidth(this->lb_album_cnt->text().length()*10);//c221213_1
//        this->lb_track_cnt->setFixedWidth(this->lb_track_cnt->text().length()*10);//c221213_1
//        this->lb_artist_cnt->setFixedWidth(this->lb_artist_cnt->text().length()*10);//c221213_1

//        int album_left = this->width() - this->widget_album->sizeHint().width() - this->widget_track->sizeHint().width() - this->widget_artist->sizeHint().width() - 50;
//        int track_left = album_left + this->widget_album->sizeHint().width();
//        int artist_left = track_left + this->widget_track->sizeHint().width();

//        /*if(this->width() < 1149)
//        {
//            album_left = 1200 - this->widget_album->sizeHint().width() - this->widget_track->sizeHint().width() - this->widget_artist->sizeHint().width() - 50;
//            track_left = album_left + this->widget_album->sizeHint().width();
//            artist_left = track_left + this->widget_track->sizeHint().width();
//        }*/

//        if(this->width() < 1299)
//        {
//            album_left = 1350 - this->widget_album->sizeHint().width() - this->widget_track->sizeHint().width() - this->widget_artist->sizeHint().width() - 50;
//            track_left = album_left + this->widget_album->sizeHint().width()-70;
//            artist_left = track_left + this->widget_track->sizeHint().width()-70;

//            this->widget_album->hide();
//            this->widget_track->hide();
//            this->widget_artist->hide();
//        }
//        else{

//            this->widget_album->show();
//            this->widget_track->show();
//            this->widget_artist->show();
//            this->widget_album->setGeometry(album_left, 0,  this->widget_album->sizeHint().width(), this->widget_album->sizeHint().height());
//            this->widget_track->setGeometry(track_left, 0,  this->widget_track->sizeHint().width(), this->widget_track->sizeHint().height());
//            this->widget_artist->setGeometry(artist_left, 0,  this->widget_artist->sizeHint().width(), this->widget_artist->sizeHint().height());

//        }

//        //this->widget_album->setGeometry(album_left, 0,  this->widget_album->sizeHint().width(), this->widget_album->sizeHint().height());
//        //this->widget_track->setGeometry(track_left, 0,  this->widget_track->sizeHint().width(), this->widget_track->sizeHint().height());
//        //this->widget_artist->setGeometry(artist_left, 0,  this->widget_artist->sizeHint().width(), this->widget_artist->sizeHint().height());
    }
}
