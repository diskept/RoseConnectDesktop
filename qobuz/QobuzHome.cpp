#include "qobuz/QobuzHome.h"

#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ProcCommon_forQobuz.h"
#include "qobuz/ProcRosePlay_withQobuz.h"

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


namespace qobuz {

    // QOBUZ subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX_SUBTITLE_newReleases = 0;
    const int BTN_IDX_SUBTITLE_idealDiscography = 1;
    const int BTN_IDX_SUBTITLE_qobuzIssime = 2;
    const int BTN_IDX_SUBTITLE_editorPiscks = 3;

    // QOBUZ section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___new_releases = 0;
    const int SECTION_FOR_MORE_POPUP___idealdiscography = 1;
    const int SECTION_FOR_MORE_POPUP___qobuzissime = 2;
    const int SECTION_FOR_MORE_POPUP___editor_picks = 3;

    const int GET_ITEM_SIZE___ONCE_FOR_MAIN = 15;


    // QOBUZ API 관련
    QString QOBUZ_API_RELEASE_PATH = "album/getFeatured";
    QString QOBUZ_API_RELEASE_TYPE = "new-releases";
    QString QOBUZ_API_IDEAL_TYPE = "ideal-discography";
    QString QOBUZ_API_QOBUZ_TYPE = "qobuzissims";

    QString QOBUZ_API_PLAYLIST_PATH = "playlist/getFeatured";
    QString QOBUZ_API_PLAYLIST_TYPE = "editor-picks";


    // ROSE subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX_SUBTITLE_recentlyPlay = 4;
    const int BTN_IDX_SUBTITLE_recentlyTrack = 5;
    const int BTN_IDX_SUBTITLE_myPlaylists = 6;
    const int BTN_IDX_SUBTITLE_userPlaylists = 7;

    // ROSE section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___recentlyAlbum = 4;
    const int SECTION_FOR_MORE_POPUP___recentlyPlaylist = 5;
    const int SECTION_FOR_MORE_POPUP___recentlyTrack = 6;
    const int SECTION_FOR_MORE_POPUP___myPlaylists = 7;
    const int SECTION_FOR_MORE_POPUP___userPlaylists = 8;


    // ROSE API 관련
    QString ROSE_API_ALBUM_PATH = "member/album/recent";
    QString ROSE_API_PLAYLIST_PATH = "member/playlist/recent";
    QString ROSE_API_TRACK_PATH = "member/track/recent";
    QString ROSE_API_MYPLAYLIST_PATH = "member/playlist";
    QString ROSE_API_USERPLAYLIST_PATH = "member/playlist/all";

    QString ALBTAB_STEP_ALBUM = "album";
    QString ALBTAB_STEP_PLAYLIST = "playlist";


    /**
     * @brief "QOBUZ > 홈" 화면을 위한 생성자. @see PAGECODE_T_HOME
     * @param parent
     */
    QobuzHome::QobuzHome(QWidget *parent) : AbstractQobuzSubWidget(VerticalScroll_filter, parent) {

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));
        connect(linker, SIGNAL(signal_message_show_timeout()), SLOT(slot_time_out()));



        if(global.user_forQobuz.isLogined() == true){

            // 기본 UI 세팅
            this->btnStyle_normal = "QPushButton{ ";
            this->btnStyle_normal += "background:transparent;color:#FFFFFF;font-size:15px;";
            this->btnStyle_normal += "border:1px solid #707070; border-radius:20px;}";
            this->btnStyle_normal += "QPushButton:hover{background-color:#BE905F;}";

            this->btnStyle_selected = "QPushButton{ ";
            this->btnStyle_selected += "background:#BE905F;color:#FFFFFF;font-size:15px;";
            this->btnStyle_selected += "border:1px solid #BE905F; border-radius:20px;}";

            this->setUIControl_chooseGenres();

            // data
            this->list_newReleases = new QList<qobuz::AlbumItemData>();
            this->list_editorPicks = new QList<qobuz::PlaylistItemData>();
            this->list_idealDiscography = new QList<qobuz::AlbumItemData>();
            this->list_qobuz_issime = new QList<qobuz::AlbumItemData>();

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
    QobuzHome::~QobuzHome(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzHome::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            if(this->flag_release[1] == true || this->flag_editor[1] == true || this->flag_ideal[1] == true || this->flag_qobuz[1] == true){
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

            this->list_newReleases->clear();
            this->list_editorPicks->clear();
            this->list_idealDiscography->clear();
            this->list_qobuz_issime->clear();

            this->flag_release[0] = false;
            this->flag_editor[0] = false;
            this->flag_ideal[0] = false;
            this->flag_qobuz[0] = false;

            this->flag_release[1] = false;
            this->flag_editor[1] = false;
            this->flag_ideal[1] = false;
            this->flag_qobuz[1] = false;
        }
    }



    /**
     * @brief QobuzHome::setActivePage
     */
    void QobuzHome::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

            // 데이터로 UI 적용 : 세팅이 안되어 있는 경우에만
            if(this->flagSet_genre == false){
                if(global.user_forQobuz.flag_main_filter == false){
                    global.user_forQobuz.flag_main_filter = true;
                    for(int i = 0; i < 30; i++){
                        global.user_forQobuz.enable_main_filter[i] = 0;
                        this->flag_filter[i] = false;
                    }
                }
                this->applyUIControl_chooseGenres_fromlistData();
            }

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

            this->box_contents->addWidget(this->widget_rose_contents);

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
           // this->make_CustomLineEdit();//c220729

            /*this->box_total_home_contents = new QVBoxLayout();
            this->box_total_home_contents->setSpacing(0);
            this->box_total_home_contents->setContentsMargins(0, 0, 0, 0);

            this->box_total_home_contents->addWidget(this->widget_rose_contents, 0, Qt::AlignTop);
            this->box_total_home_contents->addWidget(this->widget_home_contents, 1, Qt::AlignTop);

            this->widget_total_home_contents = new QWidget();
            this->widget_total_home_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_total_home_contents->setLayout(this->box_total_home_contents);

            this->box_contents->addWidget(this->widget_total_home_contents);
            //this->box_contents->addWidget(this->le_search_back);*/
            //--------------------------------------------//c220729  end

            this->box_contents->addWidget(this->widget_home_contents);

            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // init UI
            this->vBox_recentlyPlay = new QVBoxLayout();
            this->vBox_recentlyTrack = new QVBoxLayout();
            this->vBox_myPlaylist = new QVBoxLayout();
            this->vBox_userPlaylist = new QVBoxLayout();

            this->hBox_recentlyAlbum = new QHBoxLayout();
            this->hBox_recentlyPlaylist = new QHBoxLayout();
            this->hBox_myPlaylist = new QHBoxLayout();
            this->hBox_userPlaylist = new QHBoxLayout();

            this->vBox_newReleases = new QVBoxLayout();
            this->vBox_editorPicks = new QVBoxLayout();
            this->vBox_idealDiscography = new QVBoxLayout();
            this->vBox_qobuzIssime = new QVBoxLayout();

            this->hBox_newReleases = new QHBoxLayout();
            this->hBox_editorPicks = new QHBoxLayout();
            this->hBox_idealDiscography = new QHBoxLayout();
            this->hBox_qobuzIssime = new QHBoxLayout();

            // clear UI
            GSCommon::clearLayout(this->vBox_recentlyPlay);
            GSCommon::clearLayout(this->vBox_recentlyTrack);
            GSCommon::clearLayout(this->vBox_myPlaylist);
            GSCommon::clearLayout(this->vBox_userPlaylist);

            GSCommon::clearLayout(this->hBox_recentlyAlbum);
            GSCommon::clearLayout(this->hBox_recentlyPlaylist);
            GSCommon::clearLayout(this->hBox_myPlaylist);
            GSCommon::clearLayout(this->hBox_userPlaylist);

            GSCommon::clearLayout(this->vBox_newReleases);
            GSCommon::clearLayout(this->vBox_editorPicks);
            GSCommon::clearLayout(this->vBox_idealDiscography);
            GSCommon::clearLayout(this->vBox_qobuzIssime);

            GSCommon::clearLayout(this->hBox_newReleases);
            GSCommon::clearLayout(this->hBox_editorPicks);
            GSCommon::clearLayout(this->hBox_idealDiscography);
            GSCommon::clearLayout(this->hBox_qobuzIssime);

            // sub Title UI
            for(int i = 0; i < 10; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            for(int i = 0; i < 15; i++){
                this->home_recently_album[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___recentlyAlbum, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                connect(this->home_recently_album[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &QobuzHome::slot_clickedItemAlbum);
            }

            for(int i = 0; i < 15; i++){
                this->home_recently_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___recentlyPlaylist, tidal::AbstractItem::ImageSizeMode::Square_200x200, 2, true);        //Ractangle_384x186
                connect(this->home_recently_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &QobuzHome::slot_clickedItemPlaylist);
            }

            for (int i = 0; i < 5; i++) {
                this->home_recently_track[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->home_recently_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzHome::slot_clickedItemTrack_inList);
                this->home_recently_track[i]->setProperty("index", i);
                this->home_recently_track[i]->setObjectName("recently tracks");
            }

            for(int i = 0; i < 15; i++){
                this->home_myPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___myPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                connect(this->home_myPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &QobuzHome::slot_clickedItemPlaylist);
            }

            for(int i = 0; i < 15; i++){
                this->home_userPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___userPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 8, true);
                connect(this->home_userPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &QobuzHome::slot_clickedItemPlaylist);
            }


            for(int i = 0; i < 15; i++){
                this->home_newReleases[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___new_releases, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_newReleases[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzHome::slot_clickedItemAlbum);
            }

            for(int i = 0; i < 15; i++){
                this->home_editorPicks[i] = new qobuz::ItemPlaylist_qobuz(i, SECTION_FOR_MORE_POPUP___editor_picks, tidal::AbstractItem::ImageSizeMode::Ractangle_360x176);
                connect(this->home_editorPicks[i], &qobuz::ItemPlaylist_qobuz::signal_clicked, this, &QobuzHome::slot_clickedItemPlaylist);
            }

            for(int i = 0; i < 15; i++){
                this->home_idealDiscography[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___idealdiscography, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_idealDiscography[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzHome::slot_clickedItemAlbum);
            }

            for(int i = 0; i < 15; i++){
                this->home_qobuzIssime[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___qobuzissime, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_qobuzIssime[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzHome::slot_clickedItemAlbum);
            }

            if(global.updateCheckFlag){

                //ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223
                //QTimer::singleShot(10000, this, SLOT(slot_hide_msg()));

                if(global.enable_section_left == true){
                    global.enable_section_left = false;
                }

                // page move
                if(global.user_forQobuz.isLogined() == true){
                    AbstractQobuzSubWidget::slot_acceptedDialogLogin();
                }
                else if(global.user_forQobuz.isLogined() == false){
                    AbstractQobuzSubWidget::slot_acceptedDialogLogout();
                }

                QJsonObject tmp_data;
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_SETTINGS);

                // 부모에게 페이지 변경하라고 시그널 보냄
                emit this->signal_clickedMovePage(tmp_data);
            }
            else{

                global.enable_message_count = 0;
                global.enable_message_flag = true;
                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223

                this->setUIControl_requestRose();
                this->setUIControl_requestGenres();
            }
        }
    }


    void QobuzHome::slot_hide_msg(){

        ContentLoadingwaitingMsgHide();

        if(global.enable_message_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;
        }

        for(int i = 0 ; i < this->btn_filter_cnt ; i++){

            this->btn_filter[i]->setEnabled(true);
        }
        this->btn_filter_clear->setEnabled(true);
    }


    void QobuzHome::slot_time_out(){

        ContentLoadingwaitingMsgHide();

        if(global.enable_message_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;

            // page move
            if(global.user_forQobuz.isLogined() == true){
                AbstractQobuzSubWidget::slot_acceptedDialogLogin();
            }
            else if(global.user_forQobuz.isLogined() == false){
                AbstractQobuzSubWidget::slot_acceptedDialogLogout();
            }

            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_SETTINGS);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedMovePage(tmp_data);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 장르 선택하는 부분. 최상단.
     */
    void QobuzHome::setUIControl_chooseGenres(){

        // 상단 필터
        /*this->btn_filter_ico = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
        this->btn_filter_ico->setFixedSize(80, 70);
        this->btn_filter_ico->setCursor(Qt::PointingHandCursor);

        this->box_genre_filter = new QHBoxLayout();
        this->box_genre_filter->setContentsMargins(0,0,0,0);
        this->box_genre_filter->setSpacing(0);
        this->box_genre_filter->setAlignment(Qt::AlignLeft);
        this->box_genre_filter->addWidget(this->btn_filter_ico);

        this->btn_filter_cnt = ProcCommon::getCount_listGenreInfo();

        for(int i = 0; i < this->btn_filter_cnt; i++){
            this->lb_filter[i] = new QLabel();
            this->lb_filter[i]->setContentsMargins(20,0,20,0);
            this->lb_filter[i]->setFixedHeight(30);
            this->lb_filter[i]->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->lb_filter[i]->setAlignment(Qt::AlignCenter);
        }

        this->flow_genre_filter = new FlowLayout(0, 5, 10, 0);
        this->flow_genre_filter->setContentsMargins(0, 0, 0, 0);
        this->flow_genre_filter->setSizeConstraint(QLayout::SetMinimumSize);

        this->widget_genre_filter = new QWidget();
        this->widget_genre_filter->setFixedSize(1500, 70);
        this->widget_genre_filter->setStyleSheet("background:transparent;");
        this->widget_genre_filter->setLayout(this->flow_genre_filter);

        this->hbox_genre_filter = new QHBoxLayout();
        this->hbox_genre_filter->setContentsMargins(0,0,0,0);
        this->hbox_genre_filter->setSpacing(0);
        this->hbox_genre_filter->addWidget(this->widget_genre_filter, Qt::AlignCenter);

        QHBoxLayout *hbox_btnFilter = new QHBoxLayout();
        hbox_btnFilter->setContentsMargins(0,0,0,0);
        hbox_btnFilter->setSpacing(0);
        hbox_btnFilter->addLayout(this->box_genre_filter);
        hbox_btnFilter->addSpacing(10);
        hbox_btnFilter->addLayout(this->hbox_genre_filter);

        QWidget *widget_btnFilter = new QWidget();
        widget_btnFilter->setObjectName("widget_btnFilter");
        widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color:#171717; }");
        widget_btnFilter->setFixedSize(1680, 70);
        widget_btnFilter->setLayout(hbox_btnFilter);

        this->box_filter->addWidget(widget_btnFilter);*/

        // 상단 필터
        this->widget_btnFilter = new QWidget();
        this->widget_btnFilter->setObjectName("widget_btnFilter");
        this->widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color:#171717; }");
        this->widget_btnFilter->setFixedSize(global.LmtCnt, 75);//c220805

        this->btn_filter_ico = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png", this->widget_btnFilter);
        this->btn_filter_ico->setFixedSize(80, 70);
        this->btn_filter_ico->setCursor(Qt::PointingHandCursor);
        this->btn_filter_ico->setGeometry(3, 0, 0, 0);

        connect(this->btn_filter_ico, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter()));

        this->flow_genre_filter = new FlowLayout(0, 0, 10, 5);
        this->flow_genre_filter->setContentsMargins(0, 0, 0, 0);
        this->flow_genre_filter->setSizeConstraint(QLayout::SetMinimumSize);

        this->widget_genre_filter = new QWidget(this->widget_btnFilter);
        this->widget_genre_filter->setFixedSize(global.LmtCnt - 180, 75);
        this->widget_genre_filter->setStyleSheet("background:transparent;");
        this->widget_genre_filter->setLayout(this->flow_genre_filter);
        this->widget_genre_filter->setGeometry(90, 5, 0, 0);

        this->box_filter->addWidget(widget_btnFilter);

        this->btn_filter_cnt = ProcCommon::getCount_listGenreInfo();

        for(int i = 0; i < this->btn_filter_cnt; i++){
            this->lb_filter[i] = new QLabel();
            this->lb_filter[i]->setContentsMargins(20, 0, 20, 0);
            this->lb_filter[i]->setFixedHeight(30);
            this->lb_filter[i]->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->lb_filter[i]->setAlignment(Qt::AlignCenter);
        }
    }


    void QobuzHome::setUIControl_requestGenres(){

        for(int i = 0; i < 10; i++){
            this->lb_subTitle[i] = new QLabel();
            this->btnView_all[i] = new QPushButton();
        }

        this->list_newReleases->clear();
        this->list_editorPicks->clear();
        this->list_idealDiscography->clear();
        this->list_qobuz_issime->clear();

        QString genre_ids = "";

        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(global.user_forQobuz.enable_main_filter[0] == 1){
                genre_ids = QString("%1").arg(ProcCommon::getGenreInfo(0).id);
                break;
            }
            else if(global.user_forQobuz.enable_main_filter[i] == 1){
                if(genre_ids.isEmpty()){
                    genre_ids = QString("%1").arg(ProcCommon::getGenreInfo(i).id);
                }
                else{
                    genre_ids += QString(", %1").arg(ProcCommon::getGenreInfo(i).id);
                }
            }
        }

        if(genre_ids.isEmpty()){

            genre_ids = QString("%1").arg(ProcCommon::getGenreInfo(0).id);
        }

        // request HTTP API for New Relsease
        qobuz::ProcCommon *proc_newRelease = new qobuz::ProcCommon(this);
        connect(proc_newRelease, &ProcCommon::completeReq_list_albums, this, &QobuzHome::slot_applyResult_newReleases);
        proc_newRelease->request_qobuz_getList_albums(QOBUZ_API_RELEASE_PATH, QOBUZ_API_RELEASE_TYPE, genre_ids, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

        // request HTTP API for Editor's Pick
        qobuz::ProcCommon *proc_editorPicks = new qobuz::ProcCommon(this);
        connect(proc_editorPicks, &ProcCommon::completeReq_list_playlists, this, &QobuzHome::slot_applyResult_editorPicks);
        proc_editorPicks->request_qobuz_getList_playlists(QOBUZ_API_PLAYLIST_PATH, QOBUZ_API_PLAYLIST_TYPE, genre_ids, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

        // request HTTP API for Ideal Discography
        qobuz::ProcCommon *proc_idealDiscography = new qobuz::ProcCommon(this);
        connect(proc_idealDiscography, &ProcCommon::completeReq_list_albums, this, &QobuzHome::slot_applyResult_idealDiscography);
        proc_idealDiscography->request_qobuz_getList_albums(QOBUZ_API_RELEASE_PATH, QOBUZ_API_IDEAL_TYPE, genre_ids, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

        // request HTTP API for Qobuzissime
        qobuz::ProcCommon *proc_qobuzissime = new qobuz::ProcCommon(this);
        connect(proc_qobuzissime, &ProcCommon::completeReq_list_albums, this, &QobuzHome::slot_applyResult_qobuzIssime);
        proc_qobuzissime->request_qobuz_getList_albums(QOBUZ_API_RELEASE_PATH, QOBUZ_API_QOBUZ_TYPE, genre_ids, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

    }


    void QobuzHome::setUIControl_requestRose(){

        if(!global.user.getAccess_token().isEmpty()){

            // request HTTP API for Recently Album
            roseHome::ProcCommon *proc_recentlyAlbum = new roseHome::ProcCommon(this);
            connect(proc_recentlyAlbum, &roseHome::ProcCommon::completeReq_list_albums, this, &QobuzHome::slot_applyResult_recentlyAlbum);
            proc_recentlyAlbum->request_rose_getList_recentlyAlbums(ROSE_API_ALBUM_PATH , "QOBUZ", 0, GET_ITEM_SIZE___ONCE_FOR_MAIN);

            // request HTTP API for Recently Playlist
            roseHome::ProcCommon *proc_recentlyPlaylist = new roseHome::ProcCommon(this);
            connect(proc_recentlyPlaylist, &roseHome::ProcCommon::completeReq_list_playlists, this, &QobuzHome::slot_applyResult_recentlyPlaylist);
            proc_recentlyPlaylist->request_rose_getList_recentlyPlaylists(ROSE_API_PLAYLIST_PATH , "QOBUZ", 0, GET_ITEM_SIZE___ONCE_FOR_MAIN);

            // request HTTP API for Recently Track
            roseHome::ProcCommon *proc_recentlyTrack = new roseHome::ProcCommon(this);
            connect(proc_recentlyTrack, &roseHome::ProcCommon::completeReq_list_tracks, this, &QobuzHome::slot_applyResult_recentlyTrack);
            proc_recentlyTrack->request_rose_getList_recentlyTracks(ROSE_API_TRACK_PATH , "QOBUZ", 0, 5);

            // request HTTP API for Recently Playlist
            roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
            connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &QobuzHome::slot_applyResult_myPlaylist);
            proc_myPlaylist->request_rose_getList_myPlaylists(ROSE_API_MYPLAYLIST_PATH , "QOBUZ", 0, GET_ITEM_SIZE___ONCE_FOR_MAIN);

            // request HTTP API for User Playlist
            roseHome::ProcCommon *proc_userPlaylist = new roseHome::ProcCommon(this);
            connect(proc_userPlaylist, &roseHome::ProcCommon::completeReq_list_usersplaylists, this, &QobuzHome::slot_applyResult_userPlaylist);
            proc_userPlaylist->request_rose_getList_usersPlaylists(ROSE_API_USERPLAYLIST_PATH , "PLAYLIST_RECENT", "QOBUZ", 0, GET_ITEM_SIZE___ONCE_FOR_MAIN);
        }
    }


    /**
     * @brief 장르 종류 UI 적용하기 (데이터를 활용)
     */
    void QobuzHome::applyUIControl_chooseGenres_fromlistData(){

        QWidget *widget_header = new QWidget();
        widget_header->setFixedSize(735, 50);
        widget_header->setStyleSheet("background:transparent; border:1px solid #4D4D4D;");

        QPushButton *btn_close_genre = new QPushButton(widget_header);
        btn_close_genre->setText(tr("Select one or more genres"));
        btn_close_genre->setFixedSize(735, 50);
        btn_close_genre->setStyleSheet("color:#CCCCCC;font-size:16px;background-color:transparent;");
        btn_close_genre->setGeometry(0, 0, 0, 0);
        btn_close_genre->setCursor(Qt::PointingHandCursor);

        connect(btn_close_genre, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter_close()));

        QString btnStyle_clear = "QPushButton{ ";
        btnStyle_clear += "background:transparent;color:#FFFFFF;font-size:15px;";
        btnStyle_clear += "border:1px solid #707070; border-radius:15px;}";
        btnStyle_clear += "QPushButton:hover{background-color:#BE905F;}";

        this->btn_filter_clear = new QPushButton(btn_close_genre);
        this->btn_filter_clear->setGeometry(635, 8, 80, 30);
        this->btn_filter_clear->setText(tr("Clear"));
        this->btn_filter_clear->setStyleSheet(btnStyle_clear);
        this->btn_filter_clear->setCursor(Qt::PointingHandCursor);

        connect(this->btn_filter_clear, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter_clear()));

        QHBoxLayout *box_header = new QHBoxLayout;
        box_header->setContentsMargins(0, 0, 0, 0);
        box_header->setSpacing(0);
        box_header->addWidget(widget_header);

        this->flowLayout_genre = new FlowLayout(0, 0, 20, 20);
        this->flowLayout_genre->setContentsMargins(10, 20, 10, 20);
        this->flowLayout_genre->setSizeConstraint(QLayout::SetMinimumSize);

        QWidget *widget_body = new QWidget();
        widget_body->setFixedSize(735, 210);

        widget_body->setStyleSheet("background:transparent;");
        widget_body->setLayout(this->flowLayout_genre);

        QHBoxLayout *box_body = new QHBoxLayout;
        box_body->setContentsMargins(0, 0, 0, 0);
        box_body->setSpacing(0);
        box_body->addWidget(widget_body);

        if(this->btn_filter_cnt > 0){
            this->flagSet_genre = true;
        }

        int select_count = 0;
        for(int i = 0 ; i < this->btn_filter_cnt ; i++){

            if(ProcCommon::getGenreInfo(i).name.contains("&")){
                this->str_genre_name[i] = ProcCommon::getGenreInfo(i).name.replace("&", "&&");
            }
            else{
                this->str_genre_name[i] = ProcCommon::getGenreInfo(i).name;
            }

            this->enable_genre_id[i] = 0;

            this->btn_filter[i] = new QPushButton();
            this->btn_filter[i]->setText(this->str_genre_name[i]);
            this->btn_filter[i]->setFixedSize(QSize(125, 40));
            if(global.user_forQobuz.enable_main_filter[i] == 1){
                //this->btn_filter[i]->setStyleSheet(global.user_forQobuz.enable_main_filter[i] == 1 ? this->btnStyle_selected : this->btnStyle_normal);
                this->btn_filter[i]->setStyleSheet(this->btnStyle_selected);
                select_count++;
            }
            else{
                this->btn_filter[i]->setStyleSheet(this->btnStyle_normal);
            }
            this->btn_filter[i]->setProperty("idx", i);
            this->btn_filter[i]->setCursor(Qt::PointingHandCursor);

            /*if(global.user_forQobuz.enable_main_filter[i] == 1){
                this->lb_filter[i]->setText(this->str_genre_name[i]);
                this->box_genre_filter->addWidget(this->lb_filter[i]);
                this->flag_filter[i] = true;
            }*/

            connect(this->btn_filter[i], SIGNAL(clicked()), this, SLOT(slot_clickBtn_chooseDetailGenre()));

            this->flowLayout_genre->addWidget(this->btn_filter[i]);
        }

        /*if(this->box_genre_filter->count() == 1){
            this->lb_filter[0]->setText(this->str_genre_name[0]);
            this->box_genre_filter->addWidget(this->lb_filter[0]);
            this->flag_filter[0] = true;
        }*/

        if(select_count == 0 || select_count == this->btn_filter_cnt){
            this->lb_filter[0]->setText(this->str_genre_name[0]);
            //this->box_genre_filter->addWidget(this->lb_filter[0]);
            this->flow_genre_filter->addWidget(this->lb_filter[0]);
            this->flag_filter[0] = true;

            this->widget_genre_filter->setGeometry(90, 23, 0, 0);
        }
        else{
           int label_size = 0;
            for(int i = 0; i < this->btn_filter_cnt; i++){
                if(global.user_forQobuz.enable_main_filter[i] == 1){
                    this->lb_filter[i]->setText(this->str_genre_name[i]);
                    //this->box_genre_filter->addWidget(this->lb_filter[i]);
                    this->flow_genre_filter->addWidget(this->lb_filter[i]);
                    this->flag_filter[i] = true;

                    label_size += (this->lb_filter[i]->sizeHint().width() + 10);
                }
            }

            /*if(global.LmtCnt > 1680){//c220805
                widget_btnFilter->setFixedSize(global.LmtCnt, 75);
            }else{
                if(label_size < global.LmtCnt){
                    widget_btnFilter->setFixedSize(global.LmtCnt, 75);
                }else{
                    widget_btnFilter->setFixedSize(global.LmtCnt, 100);
                }

            }

            print_debug();
            qDebug() << "this->btn_filter_cnt=" << this->btn_filter_cnt;

            if(global.LmtCnt > 1500){//c220805
                widget_genre_filter->setFixedSize(global.LmtCnt, 75);
            }else{
                if(label_size < global.LmtCnt){
                    widget_genre_filter->setFixedSize(global.LmtCnt, 75);
                }else{
                    widget_genre_filter->setFixedSize(global.LmtCnt, 100);
                }

            }*/
            if(label_size > 1500){
                this->widget_genre_filter->setGeometry(90, 5, 0, 0);
            }
            else{
                this->widget_genre_filter->setGeometry(90, 23, 0, 0);
            }
        }

        QVBoxLayout *box_total = new QVBoxLayout;
        box_total->setContentsMargins(0, 0, 0, 0);
        box_total->setSpacing(0);
        box_total->setAlignment(Qt::AlignTop);
        box_total->addLayout(box_header);
        box_total->addLayout(box_body);

        this->widget_filter = new QWidget(this);
        this->widget_filter->setObjectName("widget_filter");
        this->widget_filter->setStyleSheet("#widget_filter { background-color:#333333;border:1px solid #4D4D4D; border-radius:3px; }");
        this->widget_filter->setFixedSize(735, 260);
        this->widget_filter->move(70, 10);
        this->widget_filter->setContentsMargins(1, 1, 1, 1);

        QPushButton *btn_total = new QPushButton(this->widget_filter);
        btn_total->setStyleSheet("background:transparent; border:0px");
        btn_total->setCursor(Qt::PointingHandCursor);
        btn_total->setFixedSize(735, 260);
        btn_total->setGeometry(0, 0, 0, 0);
        btn_total->setLayout(box_total);

        connect(btn_total, SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_close()));

        this->widget_filter->hide();
    }


    void QobuzHome::setUIControl_appendWidget_rose(){

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
                this->box_rose_contents->addSpacing(80);
            }

            if(this->flag_userPlaylist[0] == true){

                this->widget_userPlaylist = new QWidget();
                if(this->list_userPlaylist->size() > 0){
                    this->widget_userPlaylist = this->setUIControl_subTitle_withSideBtn(tr("Qobuz Playlist On Rose") + QString(" (%1)").arg(this->list_userPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_userPlaylists, this->vBox_userPlaylist);
                }
                else{
                    this->widget_userPlaylist = this->setUIControl_subTitle_withSideBtn(tr("Qobuz Playlist On Rose") + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_userPlaylists, this->vBox_userPlaylist);
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


    void QobuzHome::setUIControl_appendWidget(){

        if(this->flag_release[0] == true && this->flag_editor[0] == true && this->flag_ideal[0] == true && this->flag_qobuz[0] == true){

            if(this->flag_release[1] == true){
                this->widget_newReleases = new QWidget();
                this->widget_newReleases = this->setUIControl_subTitle_withSideBtn(QString("New releases (%1)").arg(this->list_newReleases->at(0).album_total_cnt), "View All", BTN_IDX_SUBTITLE_newReleases, this->vBox_newReleases);

                this->vBox_newReleases->addSpacing(10);

                this->hBox_newReleases = this->setUIControl_hBoxLayout_forAlbum(this->vBox_newReleases);

                for(int i = 0; i < this->list_newReleases->size(); i++){
                    this->home_newReleases[i]->setData(this->list_newReleases->at(i));
                    this->hBox_newReleases->addWidget(this->home_newReleases[i]);
                }

                this->box_home_contents->addLayout(this->vBox_newReleases);
                this->box_home_contents->addSpacing(45);
            }

            if(this->flag_editor[1] == true){
                this->widget_editorPicks = new QWidget();
                this->widget_editorPicks = this->setUIControl_subTitle_withSideBtn(QString("Qobuz playlists (%1)").arg(this->list_editorPicks->at(0).album_total_cnt), "View All", BTN_IDX_SUBTITLE_editorPiscks, this->vBox_editorPicks);

                this->vBox_editorPicks->addSpacing(10);

                this->hBox_editorPicks = this->setUIControl_hBoxLayout_forPlaylists(this->vBox_editorPicks);

                for(int i = 0; i < this->list_editorPicks->size(); i++){
                    this->home_editorPicks[i]->setData(this->list_editorPicks->at(i));
                    this->hBox_editorPicks->addWidget(this->home_editorPicks[i]);
                }

                this->box_home_contents->addLayout(this->vBox_editorPicks);
                this->box_home_contents->addSpacing(45);
            }

            if(this->flag_ideal[1] == true){
                this->widget_idealDiscography = new QWidget();
                this->widget_idealDiscography = this->setUIControl_subTitle_withSideBtn(QString("Ideal discography (%1)").arg(this->list_idealDiscography->at(0).album_total_cnt), "View All", BTN_IDX_SUBTITLE_idealDiscography, this->vBox_idealDiscography);

                this->vBox_idealDiscography->addSpacing(10);

                this->hBox_idealDiscography = this->setUIControl_hBoxLayout_forAlbum(this->vBox_idealDiscography);

                for(int i = 0; i < this->list_idealDiscography->size(); i++){
                    this->home_idealDiscography[i]->setData(this->list_idealDiscography->at(i));
                    this->hBox_idealDiscography->addWidget(this->home_idealDiscography[i]);
                }

                this->box_home_contents->addLayout(this->vBox_idealDiscography);
                this->box_home_contents->addSpacing(45);
            }

            if(this->flag_qobuz[1] == true){
                this->widget_qobuzIssime = new QWidget();
                this->widget_qobuzIssime = this->setUIControl_subTitle_withSideBtn(QString("Qoubzissme (%1)").arg(this->list_qobuz_issime->at(0).album_total_cnt), "View All", BTN_IDX_SUBTITLE_qobuzIssime, this->vBox_qobuzIssime);

                this->vBox_qobuzIssime->addSpacing(10);

                this->hBox_qobuzIssime = this->setUIControl_hBoxLayout_forAlbum(this->vBox_qobuzIssime);

                for(int i = 0; i < this->list_qobuz_issime->size(); i++){
                    this->home_qobuzIssime[i]->setData(this->list_qobuz_issime->at(i));
                    this->hBox_qobuzIssime->addWidget(this->home_qobuzIssime[i]);
                }

                this->box_home_contents->addLayout(this->vBox_qobuzIssime);
                this->box_home_contents->addSpacing(60);
            }
        }
    }


    void QobuzHome::setUIControl_initialized(){

        if(this->flag_release[1] == true || this->flag_editor[1] == true || this->flag_ideal[1] == true || this->flag_qobuz[1] == true){
            this->widget_home_contents->hide();
        }

        // init data
        this->list_newReleases->clear();
        this->list_editorPicks->clear();
        this->list_idealDiscography->clear();
        this->list_qobuz_issime->clear();

        this->flag_release[0] = false;
        this->flag_editor[0] = false;
        this->flag_ideal[0] = false;
        this->flag_qobuz[0] = false;

        this->flag_release[1] = false;
        this->flag_editor[1] = false;
        this->flag_ideal[1] = false;
        this->flag_qobuz[1] = false;

        // clear UI
        GSCommon::clearLayout(this->hBox_newReleases);
        GSCommon::clearLayout(this->hBox_editorPicks);
        GSCommon::clearLayout(this->hBox_idealDiscography);
        GSCommon::clearLayout(this->hBox_qobuzIssime);

        GSCommon::clearLayout(this->vBox_newReleases);
        GSCommon::clearLayout(this->vBox_editorPicks);
        GSCommon::clearLayout(this->vBox_idealDiscography);
        GSCommon::clearLayout(this->vBox_qobuzIssime);

        GSCommon::clearLayout(this->box_home_contents);
        this->box_contents->removeWidget(this->widget_home_contents);

        this->box_home_contents = new QVBoxLayout();
        this->box_home_contents->setSpacing(0);
        this->box_home_contents->setContentsMargins(0, 0, 0, 0);

        this->widget_home_contents = new QWidget();
        this->widget_home_contents->setStyleSheet("background:#212121; border:0px;");
        this->widget_home_contents->setLayout(this->box_home_contents);

        this->box_contents->addWidget(this->widget_home_contents, 0, Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // init UI
        this->vBox_newReleases = new QVBoxLayout();
        this->vBox_editorPicks = new QVBoxLayout();
        this->vBox_idealDiscography = new QVBoxLayout();
        this->vBox_qobuzIssime = new QVBoxLayout();

        this->hBox_newReleases = new QHBoxLayout();
        this->hBox_editorPicks = new QHBoxLayout();
        this->hBox_idealDiscography = new QHBoxLayout();
        this->hBox_qobuzIssime = new QHBoxLayout();

        // sub Title UI
        for(int i = 0; i < 10; i++){
            this->lb_subTitle[i] = new QLabel();
            this->btnView_all[i] = new QPushButton();
        }

        for(int i = 0; i < 15; i++){
            this->home_newReleases[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___new_releases, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->home_newReleases[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzHome::slot_clickedItemAlbum);
        }

        for(int i = 0; i < 15; i++){
            this->home_editorPicks[i] = new qobuz::ItemPlaylist_qobuz(i, SECTION_FOR_MORE_POPUP___editor_picks, tidal::AbstractItem::ImageSizeMode::Ractangle_360x176);
            connect(this->home_editorPicks[i], &qobuz::ItemPlaylist_qobuz::signal_clicked, this, &QobuzHome::slot_clickedItemPlaylist);
        }

        for(int i = 0; i < 15; i++){
            this->home_idealDiscography[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___idealdiscography, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->home_idealDiscography[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzHome::slot_clickedItemAlbum);
        }

        for(int i = 0; i < 15; i++){
            this->home_qobuzIssime[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___qobuzissime, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->home_qobuzIssime[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzHome::slot_clickedItemAlbum);
        }
    }


    QWidget* QobuzHome::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QWidget* QobuzHome::setUIControl_subTitle_withSubMenu(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QHBoxLayout* QobuzHome::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

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
    QHBoxLayout* QobuzHome::setUIControl_hBoxLayout_forPlaylists(QLayout *p_layout){

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
        tmp_scrollArea->setFixedHeight(293);

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
     * @brief [slot] Album 데이터를 받아서 처리함 (Suggested Album)
     * @param list_data
     */
    void QobuzHome::slot_applyResult_newReleases(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_newReleases->append(list_data);
            this->flag_release[1] = true;
        }

        this->flag_release[0] = true;
        this->setUIControl_appendWidget();
    }



    /**
     * @brief [slot] playlist 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzHome::slot_applyResult_editorPicks(const QList<qobuz::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_editorPicks->append(list_data);
            this->flag_editor[1] = true;
        }

        this->flag_editor[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzHome::slot_applyResult_idealDiscography(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_idealDiscography->append(list_data);
            this->flag_ideal[1] = true;
        }

        this->flag_ideal[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzHome::slot_applyResult_qobuzIssime(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_qobuz_issime->append(list_data);
            this->flag_qobuz[1] = true;
        }

        this->flag_qobuz[0] = true;
        this->setUIControl_appendWidget();
        this->slot_hide_msg();
    }


    void QobuzHome::slot_applyResult_recentlyAlbum(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void QobuzHome::slot_applyResult_recentlyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void QobuzHome::slot_applyResult_recentlyTrack(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void QobuzHome::slot_applyResult_myPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void QobuzHome::slot_applyResult_userPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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


    void QobuzHome::slot_applyResult_getShareLink(const QString &link){//c220826_2

        this->shareLink = link;
    }


    void QobuzHome::slot_applyResult_getRating_track(const QJsonArray &contents){

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
    void QobuzHome::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        QString genre_ids = "";

        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(global.user_forQobuz.enable_main_filter[i] == 1){
                if(genre_ids.isEmpty()){
                    genre_ids = QString("%1").arg(ProcCommon::getGenreInfo(i).id);
                }
                else{
                    genre_ids += QString(", %1").arg(ProcCommon::getGenreInfo(i).id);
                }
            }
        }

        if(btnId == BTN_IDX_SUBTITLE_newReleases){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();         //"New Release";
            data_page.api_subPath = QOBUZ_API_RELEASE_PATH;
            data_page.type = QOBUZ_API_RELEASE_TYPE;
            data_page.genre_ids = genre_ids;

            this->movePage_album_allView(data_page);                        // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_editorPiscks){
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();         //"Qobuz playlists";
            data_page.api_subPath = QOBUZ_API_PLAYLIST_PATH;
            data_page.type = QOBUZ_API_PLAYLIST_TYPE;
            data_page.genre_ids = genre_ids;

            this->movePage_playlist_allView(data_page);                     // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_idealDiscography){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();         //"Ideal discography";
            data_page.api_subPath = QOBUZ_API_RELEASE_PATH;
            data_page.type = QOBUZ_API_IDEAL_TYPE;
            data_page.genre_ids = genre_ids;

            this->movePage_album_allView(data_page);                        // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_qobuzIssime){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();         //"Qobuzissime";
            data_page.api_subPath = QOBUZ_API_RELEASE_PATH;
            data_page.type = QOBUZ_API_QOBUZ_TYPE;
            data_page.genre_ids = genre_ids;

            this->movePage_album_allView(data_page);                        // 페이지 이동 signal
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
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_MY_RECENTLY_ALBUM_ALL_LIST);
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
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_MY_RECENTLY_PLAYLIST_ALL_LIST);
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
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_MY_RECENTLY_TRACK_ALL_LIST);
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
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_MY_ROSE_PLAYLIST_ALL_LIST);
            emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_userPlaylists){
            QString title = tr("Qobuz Playlist On Rose");

            QJsonObject tmp_data;
            if(this->list_userPlaylist->size() > 0){
                tmp_data.insert("title", QString(title + "(%1)").arg(this->list_userPlaylist->at(0).totalCount));
            }
            else{
                tmp_data.insert("title", QString(title + " (0)"));
            }
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_USER_ROSE_PLAYLIST_ALL_LIST);
            emit this->signal_clickedMovePage(tmp_data);                // 페이지 이동 signal
        }
    }


    void QobuzHome::slot_clickBtn_Filter(){

        this->widget_filter->setVisible(!widget_filter->isVisible());
    }


    void QobuzHome::slot_clickBtn_Filter_close(){

        this->widget_filter->setVisible(false);
    }


    void QobuzHome::slot_clickBtn_Filter_clear(){

        int filter_select = 0;
        int filter_total = 0;
        int filter_before = 0;

        for(int sel_idx = 0; sel_idx < this->btn_filter_cnt; sel_idx++){

            if(global.user_forQobuz.enable_main_filter[sel_idx] == 1){
                filter_select++;
            }

            this->btn_filter[sel_idx]->setStyleSheet(this->btnStyle_normal);
            global.user_forQobuz.enable_main_filter[sel_idx] = 0;
        }

        for(int i = 0; i < filter_select; i++){
            if(this->flag_filter[i] == true){
                this->lb_filter[i]->setText("");
                this->lb_filter[i]->setStyleSheet("");

                //this->box_genre_filter->removeWidget(this->lb_filter[i]);
                this->flow_genre_filter->removeWidget(this->lb_filter[i]);
                this->flag_filter[i] = false;
            }
        }

        //this->box_genre_filter->removeWidget(this->btn_filter_ico);
        //this->box_genre_filter->removeWidget(this->widget_genre_filter);
        //GSCommon::clearLayout(this->box_genre_filter);
        GSCommon::clearLayout(this->box_filter);
        this->setUIControl_chooseGenres();

        filter_select = 0;
        filter_total = 0;

        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(global.user_forQobuz.enable_main_filter[i] == 1){
                filter_select += i;
            }
            filter_total += i;
        }

        if((filter_select == 0) || (filter_select == filter_total)){
            this->lb_filter[0]->setText(ProcCommon::getGenreInfo(0).name);
            this->lb_filter[0]->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");

            //this->box_genre_filter->addWidget(this->lb_filter[0]);
            this->flow_genre_filter->addWidget(this->lb_filter[0]);
            this->flag_filter[0] = true;

            this->widget_genre_filter->setGeometry(90, 23, 0, 0);
        }
        else{
            int label_size = 0;
            for(int i = 0; i < this->btn_filter_cnt; i++){
                if(global.user_forQobuz.enable_main_filter[i] == 1){
                    this->lb_filter[i]->setText(this->str_genre_name[i]);
                    this->lb_filter[i]->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");

                    //this->box_genre_filter->addWidget(this->lb_filter[i]);
                    this->flow_genre_filter->addWidget(this->lb_filter[i]);
                    this->flag_filter[i] = true;
                    label_size += (this->lb_filter[i]->sizeHint().width() + 10);
                    filter_select += i;
                }
                filter_total += i;
            }

            if(label_size > 1500){
                this->widget_genre_filter->setGeometry(90, 5, 0, 0);
            }
            else{
                this->widget_genre_filter->setGeometry(90, 23, 0, 0);
            }
        }


        filter_select = 0;
        filter_total = 0;
        filter_before = 0;

        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(global.user_forQobuz.enable_main_filter[i] == 1){
                filter_select += i;
            }
            filter_total += i;
            filter_before += this->enable_genre_id[i];
        }

        if((global.user_forQobuz.enable_main_filter[0] == 0) && (filter_select == filter_total)){
            this->btn_filter[0]->setStyleSheet(this->btnStyle_selected);
            global.user_forQobuz.enable_main_filter[0] = 1;
        }

        bool reload_frame = false;
        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(this->enable_genre_id[i] != global.user_forQobuz.enable_main_filter[i]){
                if((filter_before == 0) && (filter_select == filter_total)){
                    reload_frame = false;
                }
                else if((filter_before == this->btn_filter_cnt) && (filter_select == 0)){
                    reload_frame = false;
                }
                else{
                    reload_frame = true;
                }
                break;
            }
        }

        if(reload_frame == true){

            for(int i = 0 ; i < this->btn_filter_cnt ; i++){

                this->btn_filter[i]->setEnabled(false);
            }
            this->btn_filter_clear->setEnabled(false);

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->setUIControl_initialized();

            this->setUIControl_requestGenres();

            for(int i = 0; i < this->btn_filter_cnt; i++){
                this->enable_genre_id[i] = global.user_forQobuz.enable_main_filter[i];
            }
        }
        else{
            for(int i = 0; i < this->btn_filter_cnt; i++){
                this->enable_genre_id[i] = global.user_forQobuz.enable_main_filter[i];
            }
        }

        //widget_btnFilter->setFixedSize(global.LmtCnt, 75);//c220805
        //widget_genre_filter->setFixedSize(global.LmtCnt, 75);//c220805
    }


    /**
     * @brief 장르선택 뷰에서 "장르" 1개 선택한 경우임. 버튼 click과 연결되어 있는 slot
     */
    void QobuzHome::slot_clickBtn_chooseDetailGenre(){

        int filter_select = 0;
        int filter_total = 0;
        int filter_before = 0;

        int idx = sender()->property("idx").toInt();

        if(idx == 0){
            if(global.user_forQobuz.enable_main_filter[idx] == 0){
                for(int sel_idx = 0; sel_idx < this->btn_filter_cnt; sel_idx++){
                    this->btn_filter[sel_idx]->setStyleSheet(this->btnStyle_selected);
                    global.user_forQobuz.enable_main_filter[sel_idx] = 1;
                }
            }
            else if(global.user_forQobuz.enable_main_filter[idx] == 1){
                for(int sel_idx = 0; sel_idx < this->btn_filter_cnt; sel_idx++){
                    this->btn_filter[sel_idx]->setStyleSheet(this->btnStyle_normal);
                    global.user_forQobuz.enable_main_filter[sel_idx] = 0;
                }
            }
        }
        else{
            if(global.user_forQobuz.enable_main_filter[0] == 1){
                this->btn_filter[0]->setStyleSheet(this->btnStyle_normal);
                global.user_forQobuz.enable_main_filter[0] = 0;
            }

            if(global.user_forQobuz.enable_main_filter[idx] == 1){
                this->btn_filter[idx]->setStyleSheet(this->btnStyle_normal);
                global.user_forQobuz.enable_main_filter[idx] = 0;
            }
            else if(global.user_forQobuz.enable_main_filter[idx] == 0){
                this->btn_filter[idx]->setStyleSheet(this->btnStyle_selected);
                global.user_forQobuz.enable_main_filter[idx] = 1;
            }

        }

        //for(int i = 0; i <= this->btn_filter_cnt; i++){
            if(this->flag_filter[idx] == true){
                this->lb_filter[idx]->setText("");
                this->lb_filter[idx]->setStyleSheet("");

                //this->box_genre_filter->removeWidget(this->lb_filter[idx]);
                this->flow_genre_filter->removeWidget(this->lb_filter[0]);
                this->flag_filter[idx] = false;
            }
        //}

        //this->box_genre_filter->removeWidget(this->btn_filter_ico);
        //this->box_genre_filter->removeWidget(this->widget_genre_filter);
        //GSCommon::clearLayout(this->box_genre_filter);
        GSCommon::clearLayout(this->box_filter);
        this->setUIControl_chooseGenres();
        //this->slot_clickBtn_Filter_close();

        filter_select = 0;
        filter_total = 0;

        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(global.user_forQobuz.enable_main_filter[i] == 1){
                filter_select += i;
            }
            filter_total += i;
        }

        if((filter_select == 0) || (filter_select == filter_total)){
            this->lb_filter[0]->setText(ProcCommon::getGenreInfo(0).name);
            this->lb_filter[0]->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");

            //this->box_genre_filter->addWidget(this->lb_filter[0]);
            this->flow_genre_filter->addWidget(this->lb_filter[0]);
            this->flag_filter[0] = true;

            this->widget_genre_filter->setGeometry(90, 23, 0, 0);
        }
        else{
            int label_size = 0;//c220805
            for(int i = 0; i < this->btn_filter_cnt; i++){
                if(global.user_forQobuz.enable_main_filter[i] == 1){
                    this->lb_filter[i]->setText(this->str_genre_name[i]);
                    this->lb_filter[i]->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");

                    //this->box_genre_filter->addWidget(this->lb_filter[i]);
                    this->flow_genre_filter->addWidget(this->lb_filter[i]);
                    this->flag_filter[i] = true;
                    label_size += (this->lb_filter[i]->sizeHint().width() + 10);
                    filter_select += i;
                }
                filter_total += i;
            }

            //c220805
            /*if(global.LmtCnt > 1680){//c220805
                widget_btnFilter->setFixedSize(global.LmtCnt, 75);
            }else{
                if(label_size < global.LmtCnt){
                    widget_btnFilter->setFixedSize(global.LmtCnt, 75);
                }else{
                    widget_btnFilter->setFixedSize(global.LmtCnt, 100);
                }

            }

            print_debug();
            qDebug() << "this->btn_filter_cnt=" << this->btn_filter_cnt;

            if(global.LmtCnt > 1500){//c220805
                widget_genre_filter->setFixedSize(global.LmtCnt, 75);
            }else{
                if(label_size < global.LmtCnt){
                    widget_genre_filter->setFixedSize(global.LmtCnt, 75);
                }else{
                    widget_genre_filter->setFixedSize(global.LmtCnt, 100);
                }

            }*/

            if(label_size > 1500){
                this->widget_genre_filter->setGeometry(90, 5, 0, 0);
            }
            else{
                this->widget_genre_filter->setGeometry(90, 23, 0, 0);
            }
        }

        filter_select = 0;
        filter_total = 0;
        filter_before = 0;

        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(global.user_forQobuz.enable_main_filter[i] == 1){
                filter_select += i;
            }
            filter_total += i;
            filter_before += this->enable_genre_id[i];
        }

        if((global.user_forQobuz.enable_main_filter[0] == 0) && (filter_select == filter_total)){
            this->btn_filter[0]->setStyleSheet(this->btnStyle_selected);
            global.user_forQobuz.enable_main_filter[0] = 1;
        }

        bool reload_frame = false;
        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(this->enable_genre_id[i] != global.user_forQobuz.enable_main_filter[i]){
                if((filter_before == 0) && (filter_select == filter_total)){
                    reload_frame = false;
                }
                else if((filter_before == this->btn_filter_cnt) && (filter_select == 0)){
                    reload_frame = false;
                }
                else{
                    reload_frame = true;
                }
                break;
            }
        }

        if(reload_frame == true){

            for(int i = 0 ; i < this->btn_filter_cnt ; i++){

                this->btn_filter[i]->setEnabled(false);
            }
            this->btn_filter_clear->setEnabled(false);

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->setUIControl_initialized();

            this->setUIControl_requestGenres();

            for(int i = 0; i < this->btn_filter_cnt; i++){
                this->enable_genre_id[i] = global.user_forQobuz.enable_main_filter[i];
            }
        }
        else{
            for(int i = 0; i < this->btn_filter_cnt; i++){
                this->enable_genre_id[i] = global.user_forQobuz.enable_main_filter[i];
            }
        }
    }


    void QobuzHome::changedOnlyTabUI_notSendSignal(QString p_step){

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


    void QobuzHome::slot_changedSubTabUI(const QJsonObject &p_data){

        QString tmp_step = p_data[KEY_PAGE_CODE].toString();

        // 페이지 변경 및 데이터 세팅
        this->changedOnlyTabUI_notSendSignal(tmp_step);
    }


    void QobuzHome::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_QOBUZ)){

            global.user.setDeviceChange(false);

            if(global.user_forQobuz.isLogined() == true){
                AbstractQobuzSubWidget::slot_acceptedDialogLogin();
            }
            else if(global.user_forQobuz.isLogined() == false){
                AbstractQobuzSubWidget::slot_acceptedDialogLogout();
            }

            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_SETTINGS);

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
    void QobuzHome::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리 - Album 파트가 여러개임
        if(section == SECTION_FOR_MORE_POPUP___new_releases){
            this->proc_clicked_itemAlbum(this->list_newReleases, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___idealdiscography){
            this->proc_clicked_itemAlbum(this->list_idealDiscography, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___qobuzissime){
            this->proc_clicked_itemAlbum(this->list_qobuz_issime, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___recentlyAlbum){
            roseHome::AlbumItemData data = this->list_recentlyAlbum->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Album 바로 재생
                qobuz::ProcRosePlay_withQobuz *procRosePlay = new qobuz::ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byAlbumID(data.clientKey, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Album Detail 페이지 진입
                QJsonObject jsonObj_move = QJsonObject();
                jsonObj_move.insert("id", data.clientKey);
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ALBUM_DETAIL);

                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                print_debug();

                // OptionPopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
            }
        }
    }


    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzHome::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___editor_picks){
            this->proc_clicked_itemPlaylist(this->list_editorPicks, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___recentlyPlaylist){
            roseHome::PlaylistItemData data = this->list_recentlyPlaylist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                if(data.type == "QOBUZ" && data.isRose == false){
                    qobuz::PlaylistItemData tmpPlaylist;
                    tmpPlaylist.id = data.clientKey;
                    tmpPlaylist.tracks_count = 100;
                    qobuz::ProcRosePlay_withQobuz *procRosePlay = new qobuz::ProcRosePlay_withQobuz(this);
                    procRosePlay->requestPlayRose_byPlaylistID(tmpPlaylist, OptMorePopup::ClickMode::Play_RightNow);
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
                        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_MY_ROSE_PLAYLIST_DETAIL);

                        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                    }
                    else{
                        QJsonObject jsonObj_move = QJsonObject();
                        jsonObj_move.insert("id", data.clientKey);
                        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_PLAYLIST_DETAIL);

                        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                    }
                }
                else if(data.isRose == true){
                    QJsonObject jsonObj_move = QJsonObject();
                    jsonObj_move.insert("id", data.id);
                    jsonObj_move.insert("ownerId", data.ownerId);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_MY_ROSE_PLAYLIST_DETAIL);

                    emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                }
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
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
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_MY_ROSE_PLAYLIST_DETAIL);

                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
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
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_MY_ROSE_PLAYLIST_DETAIL);

                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                print_debug();

                // OptionPopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data), index, section);
            }
        }
    }


    void QobuzHome::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

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
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzHome::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

                this->flag_check_track = true;
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
    void QobuzHome::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

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
                qobuz::ProcRosePlay_withQobuz *procRosePlay = new qobuz::ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byAlbumID(this->list_recentlyAlbum->at(index).clientKey, OptMorePopup::ClickMode::Play_RightNow);
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
                if(this->list_recentlyPlaylist->at(index).type == "QOBUZ" && this->list_recentlyPlaylist->at(index).isRose == false){
                    qobuz::PlaylistItemData tmpPlaylist;
                    tmpPlaylist.id = this->list_recentlyPlaylist->at(index).clientKey;
                    tmpPlaylist.tracks_count = 100;
                    qobuz::ProcRosePlay_withQobuz *procRosePlay = new qobuz::ProcRosePlay_withQobuz(this);
                    procRosePlay->requestPlayRose_byPlaylistID(tmpPlaylist, OptMorePopup::ClickMode::Play_RightNow);
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
        else if(section == SECTION_FOR_MORE_POPUP___new_releases){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_newReleases, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___editor_picks){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_editorPicks, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___idealdiscography){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_idealDiscography, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___qobuzissime){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_qobuz_issime, index, clickMode);
        }
    }


    void QobuzHome::resizeEvent(QResizeEvent *event){
        Q_UNUSED(event);

        this->widget_btnFilter->setFixedSize(global.LmtCnt, 75);
        this->widget_genre_filter->setFixedSize(global.LmtCnt - 180, 75);
    }
}
