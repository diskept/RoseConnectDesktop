#include "tidal/TidalMyCollection.h"

#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"
#include "tidal/ProcRosePlay_withTidal.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"

#include <QScroller>


namespace tidal {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int BTN_IDX_SUBTITLE_playlists_tidal = 1;
    const int BTN_IDX_SUBTITLE_playlists_created = 2;
    const int BTN_IDX_SUBTITLE_tracks = 3;
    const int BTN_IDX_SUBTITLE_albums = 4;
    const int BTN_IDX_SUBTITLE_artists = 5;
    const int BTN_IDX_SUBTITLE_videos = 6;


    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___playlists_tidal = 1;
    const int SECTION_FOR_MORE_POPUP___playlists_created = 2;
    const int SECTION_FOR_MORE_POPUP___tracks = 3;
    const int SECTION_FOR_MORE_POPUP___albums = 4;
    const int SECTION_FOR_MORE_POPUP___artists = 5;
    const int SECTION_FOR_MORE_POPUP___videos = 6;

    const int GET_ITEM_SIZE___ONCE_FOR_MAIN = 15;

    const QString TIDAL_API_PATH = "users";          // 뒤에 userId 추가로 붙여야 함
    const QString TIDAL_MAIN_TITLE = "My Playlist";


    /**
     * @brief "TIDAL > My Collection" 관련 화면
     * @param parent
     */
    TidalMycollection::TidalMycollection(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_filter, parent) {

        // Data
        this->list_playlist_created = new QList<tidal::PlaylistItemData>();
        this->list_playlist_tidal = new QList<tidal::PlaylistItemData>();
        this->list_album = new QList<tidal::AlbumItemData>();
        this->list_track = new QList<tidal::TrackItemData>();
        this->list_artist = new QList<tidal::ArtistItemData>();
        this->list_video = new QList<tidal::VideoItemData>();

        // 타이달 로그인 여부에 따라서
        /*if(this->is_tidal_logined()){
            this->setUIControl_logined();            
        }
        else{
            this->setUIControl_notLogin();
        }*/
    }


    /**
     * @brief 소멸자.
     */
    TidalMycollection::~TidalMycollection(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalMycollection::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            this->page = pageCode;

            // init data
            this->list_playlist_tidal->clear();
            this->list_playlist_created->clear();
            this->list_album->clear();
            this->list_track->clear();
            this->list_artist->clear();
            this->list_video->clear();

            this->flag_playlist_tidal[0] = false;
            this->flag_playlist_created[0] = false;
            this->flag_album[0] = false;
            this->flag_track[0] = false;
            this->flag_artist[0] = false;
            this->flag_video[0] = false;

            this->flag_playlist_tidal[1] = false;
            this->flag_playlist_created[1] = false;
            this->flag_album[1] = false;
            this->flag_track[1] = false;
            this->flag_artist[1] = false;
            this->flag_video[1] = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);

            // playlist 는 created 다음으로 favorite 가 보여야 하므로, slot(created)에서 요청하도록 한다.
            connect(proc, &ProcCommon::completeReq_list_myFavoritePlaylists, this, &TidalMycollection::slot_applyResult_myFavoritePlaylists);
            proc->request_tidal_getList_myFavoritePlaylists(ProcCommon::DataOrderOption::DATE, ProcCommon::DataOrderDirection::DESC, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            connect(proc, &ProcCommon::completeReq_list_myCreatedPlaylists, this, &TidalMycollection::slot_applyResult_myCreatedPlaylists);
            proc->request_tidal_getList_myCreatedPlaylists(ProcCommon::DataOrderOption::DATE, ProcCommon::DataOrderDirection::DESC, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            connect(proc, &ProcCommon::completeReq_list_myFavoriteAlbums, this, &TidalMycollection::slot_applyResult_myFavoriteAlbums);
            proc->request_tidal_getList_myFavoriteAlbums(ProcCommon::DataOrderOption::DATE, ProcCommon::DataOrderDirection::DESC, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            connect(proc, &ProcCommon::completeReq_list_myFavoriteTracks, this, &TidalMycollection::slot_applyResult_myFavoriteTracks);
            proc->request_tidal_getList_myFavoriteTracks(ProcCommon::DataOrderOption::DATE, ProcCommon::DataOrderDirection::DESC, 5, 0);

            connect(proc, &ProcCommon::completeReq_list_myFavoriteArtists, this, &TidalMycollection::slot_applyResult_myFavoriteArtists);
            proc->request_tidal_getList_myFavoriteArtists(ProcCommon::DataOrderOption::DATE, ProcCommon::DataOrderDirection::DESC, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            connect(proc, &ProcCommon::completeReq_list_myFavoriteVideos, this, &TidalMycollection::slot_applyResult_myFavoriteVideos);
            proc->request_tidal_getList_myFavoriteVideos(ProcCommon::DataOrderOption::DATE, ProcCommon::DataOrderDirection::DESC, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 화면 활성화
     */
    void TidalMycollection::setActivePage(){

        if(this->flagNeedReload == true){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            this->box_contents->removeWidget(this->widget_main_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // init UI
            this->vBox_playlistTIDAL = new QVBoxLayout();
            this->vBox_playlistCreate = new QVBoxLayout();
            this->vBox_artist = new QVBoxLayout();
            this->vBox_album = new QVBoxLayout();
            this->vBox_track = new QVBoxLayout();
            this->vBox_video = new QVBoxLayout();

            this->hBox_playlistTIDAL = new QHBoxLayout();
            this->hBox_playlistCreate = new QHBoxLayout();
            this->hBox_artist = new QHBoxLayout();
            this->hBox_album = new QHBoxLayout();
            this->hBox_video = new QHBoxLayout();

            // clear UI
            GSCommon::clearLayout(this->vBox_playlistTIDAL);
            GSCommon::clearLayout(this->vBox_playlistCreate);
            GSCommon::clearLayout(this->vBox_artist);
            GSCommon::clearLayout(this->vBox_album);
            GSCommon::clearLayout(this->vBox_track);
            GSCommon::clearLayout(this->vBox_video);

            GSCommon::clearLayout(this->hBox_playlistTIDAL);
            GSCommon::clearLayout(this->hBox_playlistCreate);
            GSCommon::clearLayout(this->hBox_artist);
            GSCommon::clearLayout(this->hBox_album);
            GSCommon::clearLayout(this->hBox_video);

            // sub Title UI
            for(int i = 0; i < 10; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            for(int i = 0; i < 15; i++){
                this->collection_playlist_tidal[i] = new tidal::ItemPlaylist(i, SECTION_FOR_MORE_POPUP___playlists_tidal, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->collection_playlist_tidal[i], &tidal::ItemPlaylist::signal_clicked, this, &TidalMycollection::slot_clickedItemPlaylist);
            }

            for(int i = 0; i < 15; i++){
                this->collection_playlist_create[i] = new tidal::ItemPlaylist(i, SECTION_FOR_MORE_POPUP___playlists_created, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->collection_playlist_create[i], &tidal::ItemPlaylist::signal_clicked, this, &TidalMycollection::slot_clickedItemPlaylist);
            }

            for(int i = 0; i < 15; i++){
                this->collection_album[i] = new tidal::ItemAlbum(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->collection_album[i], &tidal::ItemAlbum::signal_clicked, this, &TidalMycollection::slot_clickedItemAlbum);
            }

            for (int i = 0; i < 5; i++) {
                this->collection_track[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->collection_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TidalMycollection::slot_clickedItemTrack_inList);
                this->collection_track[i]->setProperty("index", i);
                this->collection_track[i]->setObjectName("my_tracks");
            }

            for(int i = 0; i < 15; i++){
                this->collection_artist[i] = new tidal::ItemArtist(i, SECTION_FOR_MORE_POPUP___artists, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->collection_artist[i], &tidal::ItemArtist::signal_clicked, this, &TidalMycollection::slot_clickedItemArtist);
            }

            for(int i = 0; i < 15; i++){
                this->collection_video[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___videos, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200);
                connect(this->collection_video[i], &tidal::ItemVideo::signal_clicked, this, &TidalMycollection::slot_clickedItemVideo);
            }
        }
    }


    void TidalMycollection::setUIControl_appendWidget(){

        if(this->flag_playlist_tidal[0] == true && this->flag_playlist_created[0] == true && this->flag_album[0] == true
                && this->flag_track[0] == true && this->flag_artist[0] == true && this->flag_video[0] == true){

            ContentLoadingwaitingMsgHide();

            if(this->flag_playlist_tidal[1] == true){
                this->widget_playlistTIDAL = new QWidget();
                this->widget_playlistTIDAL = this->setUIControl_subTitle_withSideBtn(QString("TIDAL Playlists (%1)").arg(this->list_playlist_tidal->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_playlists_tidal, this->vBox_playlistTIDAL);

                this->vBox_playlistTIDAL->addSpacing(10);

                this->hBox_playlistTIDAL = this->setUIControl_hBoxLayout_forPlaylist(this->vBox_playlistTIDAL);

                for(int i = 0; i < this->list_playlist_tidal->size(); i++){
                    this->collection_playlist_tidal[i]->setData(this->list_playlist_tidal->at(i));
                    this->hBox_playlistTIDAL->addWidget(this->collection_playlist_tidal[i]);
                }

                this->box_main_contents->addLayout(this->vBox_playlistTIDAL);
                this->box_main_contents->addSpacing(30);
            }
            else{
                this->widget_playlistTIDAL = new QWidget();
                this->widget_playlistTIDAL = this->setUIControl_subTitle_withSideBtn(QString("TIDAL Playlists (0)"), "View All", BTN_IDX_SUBTITLE_playlists_tidal, this->vBox_playlistTIDAL);

                this->vBox_playlistTIDAL->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedHeight(305);
                this->vBox_playlistTIDAL->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_playlistTIDAL);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_playlist_created[1] == true){
                this->widget_playlistCreate = new QWidget();
                this->widget_playlistCreate = this->setUIControl_subTitle_withSideBtn(QString("My Playlists (%1)").arg(this->list_playlist_created->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_playlists_created, this->vBox_playlistCreate);

                this->vBox_playlistCreate->addSpacing(10);

                this->hBox_playlistCreate = this->setUIControl_hBoxLayout_forPlaylist(this->vBox_playlistCreate);

                for(int i = 0; i < this->list_playlist_created->size(); i++){
                    this->collection_playlist_create[i]->setData(this->list_playlist_created->at(i));
                    this->hBox_playlistCreate->addWidget(this->collection_playlist_create[i]);
                }

                this->box_main_contents->addLayout(this->vBox_playlistCreate);
                this->box_main_contents->addSpacing(30);
            }
            else{
                this->widget_playlistCreate = new QWidget();
                this->widget_playlistCreate = this->setUIControl_subTitle_withSideBtn(QString("My Playlists (0)"), "View All", BTN_IDX_SUBTITLE_playlists_created, this->vBox_playlistTIDAL);

                this->vBox_playlistCreate->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::MyPlaylist_NoData);
                noData_widget->setFixedHeight(305);
                this->vBox_playlistCreate->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->hBox_playlistCreate);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_album[1] == true){
                this->widget_album = new QWidget();
                this->widget_album = this->setUIControl_subTitle_withSideBtn(QString("My Albums (%1)").arg(this->list_album->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_albums, this->vBox_album);

                this->vBox_album->addSpacing(10);

                this->hBox_album = this->setUIControl_hBoxLayout_forAlbum(this->vBox_album);

                for(int i = 0; i < this->list_album->size(); i++){
                    this->collection_album[i]->setData(this->list_album->at(i));
                    this->hBox_album->addWidget(this->collection_album[i]);
                }

                this->box_main_contents->addLayout(this->vBox_album);
                this->box_main_contents->addSpacing(30);
            }
            else{
                this->widget_album = new QWidget();
                this->widget_album = this->setUIControl_subTitle_withSideBtn(QString("My Albums (0)"), "View All", BTN_IDX_SUBTITLE_albums, this->vBox_album);

                this->vBox_album->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedHeight(285);
                this->vBox_album->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_album);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_track[1] == true){
                this->widget_track = new QWidget();
                this->widget_track = this->setUIControl_subTitle_withSideBtn(QString("My Tracks (%1)").arg(this->list_track->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_tracks, this->vBox_track);

                this->vBox_track->addSpacing(10);

                QList<QString> tmp_clientkey;
                int maxCount = this->list_track->size();
                if(this->list_track->size() > 5){
                    maxCount = 5;
                }

                for(int i = 0; i < maxCount; i++){
                    this->collection_track[i]->setDataTrackInfo_Tidal(this->list_track->at(i));
                    this->collection_track[i]->resize(1550, 70);

                    this->vBox_track->addWidget(this->collection_track[i]);

                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));
                }

                this->box_main_contents->addLayout(this->vBox_track);
                this->box_main_contents->addSpacing(30);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalMycollection::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("TIDAL", tmp_clientkey);
            }
            else{
                this->widget_track = new QWidget();
                this->widget_track = this->setUIControl_subTitle_withSideBtn(QString("My Tracks (0)"), "View All", BTN_IDX_SUBTITLE_tracks, this->vBox_track);

                this->vBox_track->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
                noData_widget->setFixedHeight(275);
                this->vBox_track->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_track);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_artist[1] == true){
                this->widget_artist = new QWidget();
                this->widget_artist = this->setUIControl_subTitle_withSideBtn(QString("My Artists (%1)").arg(this->list_artist->at(0).artist_total_cnt), "View All", BTN_IDX_SUBTITLE_artists, this->vBox_artist);

                this->vBox_artist->addSpacing(10);

                this->hBox_artist = this->setUIControl_hBoxLayout_forArtist(this->vBox_artist);

                for(int i = 0; i < this->list_artist->size(); i++){
                    this->collection_artist[i]->setData(this->list_artist->at(i));
                    this->hBox_artist->addWidget(this->collection_artist[i]);
                }

                this->box_main_contents->addLayout(this->vBox_artist);
                this->box_main_contents->addSpacing(30);
            }
            else{
                this->widget_artist = new QWidget();
                this->widget_artist = this->setUIControl_subTitle_withSideBtn(QString("My Artists (0)"), "View All", BTN_IDX_SUBTITLE_artists, this->vBox_artist);

                this->vBox_artist->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Artist_NoData);
                noData_widget->setFixedHeight(261);
                this->vBox_artist->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_artist);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_video[1] == true){
                this->widget_video = new QWidget();
                this->widget_video = this->setUIControl_subTitle_withSideBtn(QString("My Music Videos (%1)").arg(this->list_video->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_videos, this->vBox_video);

                this->vBox_video->addSpacing(10);

                this->hBox_video = this->setUIControl_hBoxLayout_forVideo(this->vBox_video);

                for(int i = 0; i < this->list_video->size(); i++){
                    this->collection_video[i]->setData(this->list_video->at(i));
                    this->hBox_video->addWidget(this->collection_video[i]);
                }

                this->box_main_contents->addLayout(this->vBox_video);
                this->box_main_contents->addSpacing(30);
            }
            else{
                this->widget_video = new QWidget();
                this->widget_video = this->setUIControl_subTitle_withSideBtn(QString("My Music Videos (0)"), "View All", BTN_IDX_SUBTITLE_videos, this->vBox_video);

                this->vBox_video->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Video_NoData);
                noData_widget->setFixedHeight(285);
                this->vBox_video->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_video);
                this->box_main_contents->addSpacing(30);
            }
        }

    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------


    QWidget* TidalMycollection::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

        // box_main_contents 에 담을 widget, layout 생성.  box_main_contents에 먼저 담는다.
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


    QHBoxLayout* TidalMycollection::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QHBoxLayout *hBox_album = new QHBoxLayout();
        hBox_album->setSpacing(0);
        hBox_album->setContentsMargins(0,0,0,0);
        hBox_album->setAlignment(Qt::AlignTop);
        hBox_album->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_content = new QWidget;
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

        // return
        return hBox_album;
    }


    QHBoxLayout* TidalMycollection::setUIControl_hBoxLayout_forArtist(QLayout *p_layout){

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
        tmp_scrollArea->setFixedHeight(261);

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


    QHBoxLayout* TidalMycollection::setUIControl_hBoxLayout_forPlaylist(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------
        QHBoxLayout *hBox_playlist = new QHBoxLayout();
        hBox_playlist->setSpacing(0);
        hBox_playlist->setContentsMargins(0,0,0,0);
        hBox_playlist->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_contentSub = new QWidget();
        widget_contentSub->setLayout(hBox_playlist);
        widget_contentSub->setContentsMargins(0,0,0,0);


        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_contentSub);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(305);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------

        QHBoxLayout *hBox_background = new QHBoxLayout();
        hBox_background->addWidget(tmp_scrollArea);

        QWidget *widget_background = new QWidget();
        widget_background->setLayout(hBox_background);
        widget_background->setContentsMargins(0,0,0,0);
        widget_background->setStyleSheet("background-color:#212121; border:0px;");

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_playlist;
    }


    QHBoxLayout* TidalMycollection::setUIControl_hBoxLayout_forVideo(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------
        QHBoxLayout *hBox_playlist = new QHBoxLayout();
        hBox_playlist->setSpacing(0);
        hBox_playlist->setContentsMargins(0,0,0,0);
        hBox_playlist->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_contentSub = new QWidget();
        widget_contentSub->setLayout(hBox_playlist);
        widget_contentSub->setContentsMargins(0,0,0,0);


        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_contentSub);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(285);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------

        QHBoxLayout *hBox_background = new QHBoxLayout();
        hBox_background->addWidget(tmp_scrollArea);

        QWidget *widget_background = new QWidget();
        widget_background->setLayout(hBox_background);
        widget_background->setContentsMargins(0,0,0,0);
        widget_background->setStyleSheet("background-color:#212121; border:0px;");

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_playlist;
    }


    /**
     * @brief  사용자가 타이달 로그인했을 경우의 화면을 위한 Widget
     */
    /*void TidalMycollection::setUIControl_logined(){

        // 기본 UI 세팅
        this->addUIControl_subTitle_withSideBtn_common(tr("Playlist By TIDAL"), tr("View All"), BTN_IDX_SUBTITLE_playlists_playlists_created_tidal);
        this->hBox_playlist_tidal = this->get_addUIControl_hBoxLayout_forPlaylist();

        this->addUIControl_subTitle_withSideBtn_common(tr("My Playlists"), tr("View All"), BTN_IDX_SUBTITLE_playlists_playlists_created);
        this->hBox_playlist = this->get_addUIControl_hBoxLayout_forPlaylist();

        this->addUIControl_subTitle_withSideBtn_common(tr("My Albums"), tr("View All"), BTN_IDX_SUBTITLE_albums);
        this->hBox_album = this->get_addUIControl_hBoxLayout_forAlbum();

        this->addUIControl_subTitle_withSideBtn_common(tr("My Tracks"), tr("View All"), BTN_IDX_SUBTITLE_tracks);
        this->delegate = new TidalTrackDelegate(SECTION_FOR_MORE_POPUP___tracks, TidalTrackDelegate::Tidal_Track_General, this);
        connect(delegate, &TidalTrackDelegate::clicked, this, &TidalMycollection::slot_clickedItemTrack_inList);
        this->listWidget_track = this->get_addUIControl_ListWidget(this->delegate);
        this->listWidget_track->setFixedHeight(70*5);

        this->addUIControl_subTitle_withSideBtn_common(tr("My Artists"), tr("View All"), BTN_IDX_SUBTITLE_artists);
        this->hBox_artist = this->get_addUIControl_hBoxLayout_forArtist();

        this->addUIControl_subTitle_withSideBtn_common(tr("My Videos"), tr("View All"), BTN_IDX_SUBTITLE_videos);
        this->hBox_video = this->get_addUIControl_hBoxLayout_forVideo();
    }*/


    /**
     * @brief TidalMycollection::setUIControl_notLogin
     */
    /*void TidalMycollection::setUIControl_notLogin(){

        this->widget_notLogin = this->get_createUIControl_notLoginView();
        this->box_contents->addWidget(this->widget_notLogin, 1, Qt::AlignCenter | Qt::AlignVCenter);
    }*/


    /**
     * @brief My created playlist 삭제 처리에 대해서 UI갱신 처리
     * @param data_itemPos
     */
    /*void TidalMycollection::proc_preUpdate_delete_myCreatePlaylist(const ItemPositionData& data_itemPos){

        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___playlists_created){
            // index : 실제 데이터 QList 인덱스가 아님. 생성 시의 index임
            this->hide_itemWidget_fromLayout(this->hBox_playlist, data_itemPos.index, data_itemPos.section);

            // 참고로 list_playlist 데이터에서는 삭제하지 않음. :: 화면 refreshing 하기 전까지, index 데이터를 유지하기 위함
        }
    }*/


    // MARK : TidalPlaylistAll HTTP API ---------------------------------------------------------------------------------

    /**
     * @brief Tidal API 요청 - 사용자가 생성한 playlist
     */
    /*void TidalMycollection::request_playlistData_tidal(){

        // request HTTP API
        ProcCommon *proc = new ProcCommon(this);
        connect(proc, &ProcCommon::completeReq_list_myFavoritePlaylists, this, &TidalMycollection::slot_applyResult_myFavoritePlaylists);
        proc->request_tidal_getList_myFavoritePlaylists(ProcCommon::DataOrderOption::NAME, ProcCommon::DataOrderDirection::ASC, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

    }*/


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief 타이달 다이얼로그에서 로그인 성공 이벤트 발생에 대한 처리
     * @details 토스트 메시지를 띄우고, 로그인한 화면으로 전환
     */
    /*void TidalMycollection::slot_acceptedDialogLogin(){
        // 부모 함수 호출
        AbstractTidalSubWidget::slot_acceptedDialogLogin();

        // 로그인한 UI로 변경함
        this->setActivePage();
    }*/


    /**
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    //void TidalMycollection::slot_clickBtn_subTitle_side(const int btnId){
    void TidalMycollection::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_playlists_tidal){
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_COLLECTION_TIDALPALYLIST_ALL_LIST);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_playlists_created){
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_COLLECTION_PLAYLIST_ALL_LIST);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_tracks){
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_COLLECTION_TRACK_ALL_LIST);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_albums){
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_COLLECTION_ALBUM_ALL_LIST);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_artists){
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_COLLECTION_ARTIST_ALL_LIST);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_videos){
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_COLLECTION_VIDEO_ALL_LIST);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
    }


    void TidalMycollection::slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고.
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            // Tidal favorite check
            if(this->flag_send_track == true){
                QVariantList arr_myFavoriteIds = ProcJsonEasy::getJsonArray(p_jsonObj, "TRACK").toVariantList();
                bool status = arr_myFavoriteIds.contains(this->track_id_fav);

                if(status == true && this->flag_track_fav == false){
                    // Tidal track Favorite del
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalMycollection::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                else if(status == false && this->flag_track_fav == true){
                    // Tidal track Favorite add
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalMycollection::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                this->flag_send_track = false;
            }
            else{

            }
        }
    }


    void TidalMycollection::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("id")){

            int id = ProcJsonEasy::getInt(jsonObj, "id");

            if(id > 0){
                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_track_fav);
                ratingInfo.insert("star", this->track_star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", "TIDAL");

                QJsonObject track;
                track.insert("duration", 0);
                track.insert("favorite", false);
                track.insert("id", id);
                track.insert("ownerId", 0);
                track.insert("sort", 0);
                track.insert("star", 0);
                track.insert("type", "TIDAL");

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalMycollection::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
            }
            else{
                QJsonObject data = this->jsonArr_tracks_toPlay[this->track_idx_fav].toObject();

                QJsonArray tmpArtists = ProcJsonEasy::getJsonArray(data, "artists");

                QJsonArray artists;
                for(int i = 0; i < tmpArtists.size(); i++){
                    QJsonObject artistName = tmpArtists.at(i).toObject();
                    artists.append(ProcJsonEasy::getString(artistName, "name"));
                }

                QJsonObject tmp_album = ProcJsonEasy::getJsonObject(data, "album");
                QString thumbnail = ProcJsonEasy::getString(tmp_album, "cover");
                QString thumbnailUrl = tidal::ConvertData::convertImagePath(thumbnail, tidal::ConvertData::ImageSize_Square::Size_750x750);

                QJsonObject track;
                track.insert("title", ProcJsonEasy::getString(data, "title"));
                track.insert("type", "TIDAL");
                track.insert("favorite", false);
                track.insert("artists", artists);
                track.insert("albums", ProcJsonEasy::getJsonObject(data, "album"));
                track.insert("star", 0);
                track.insert("thumbnailUrl", thumbnailUrl);
                track.insert("clientKey", ProcJsonEasy::getString(data, "id"));
                track.insert("duration", ProcJsonEasy::getString(data, "duration"));
                track.insert("data", data);

                QJsonArray tracks;
                tracks.append(track);

                QJsonObject json;
                json.insert("tracks", tracks);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalMycollection::slot_applyResult_addRating_track);
                proc_fav_track->request_rose_addRating_Track(json);
            }
        }
        else{

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void TidalMycollection::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void TidalMycollection::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->collection_track[i]->setFavoritesIds(flag, star);
                }
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    /**
     * @brief Playlist 목록 데이터를 화면에 보인다. Created 와 Favoriate 같은 Layout에 보이도록 한다.
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void TidalMycollection::slot_applyResult_myFavoritePlaylists(const QList<tidal::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_playlist_tidal->append(list_data);
            this->flag_playlist_tidal[1] = true;
        }

        this->flag_playlist_tidal[0] = true;
        this->setUIControl_appendWidget();

        /*Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);
        int start_index = this->list_playlist_tidal->count();
        this->list_playlist_tidal->append(list_data);
        //this->createList_itemPlaylsit_applyingWithData(*this->list_playlist_tidal, AbstractItem::ImageSizeMode::Square_200x200, this->hBox_playlist, start_index, SECTION_FOR_MORE_POPUP___playlists_tidal);
        this->createList_itemPlaylsit_applyingWithData(*this->list_playlist_tidal, AbstractItem::ImageSizeMode::Square_200x200, this->hBox_playlist_tidal, start_index, SECTION_FOR_MORE_POPUP___playlists_tidal);
        */
    }

    /**
     * @brief Playlist 목록 데이터를 화면에 보인다. - user created playlist
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void TidalMycollection::slot_applyResult_myCreatedPlaylists(const QList<tidal::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_playlist_created->append(list_data);
            this->flag_playlist_created[1] = true;
        }

        this->flag_playlist_created[0] = true;
        this->setUIControl_appendWidget();

        /*Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);
        int start_index = this->list_playlist_created->count();
        this->list_playlist_created->append(list_data);
        this->createList_itemPlaylsit_applyingWithData(list_data, AbstractItem::ImageSizeMode::Square_200x200, this->hBox_playlist, start_index, SECTION_FOR_MORE_POPUP___playlists_created);*/

        // request HTTP API
        //this->request_playlistData_tidal();
    }    


    /**
     * @brief Album 목록 데이터를 화면에 보인다.
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void TidalMycollection::slot_applyResult_myFavoriteAlbums(const QList<tidal::AlbumItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_album->append(list_data);
            this->flag_album[1] = true;
        }

        this->flag_album[0] = true;
        this->setUIControl_appendWidget();

        /*Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);
        this->list_album->append(list_data);
        this->createList_itemAlbum_applyingWithData(list_data, AbstractItem::ImageSizeMode::Square_200x200, this->hBox_album, 0, SECTION_FOR_MORE_POPUP___albums);*/
    }


    /**
     * @brief [slot] Track 데이터를 받아서 처리함
     * @param list_data
     */
    void TidalMycollection::slot_applyResult_myFavoriteTracks(const QList<tidal::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            if(list_data.size() > 5)
            {
                for(int i = 0; i < 5; i++){
                    this->jsonArr_tracks_toPlay.append(jsonArr_dataToPlay.at(i).toObject());
                }
            }
            else{
                this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;
            }

            this->list_track->append(list_data);
            this->flag_track[1] = true;
        }

        this->flag_track[0] = true;
        this->setUIControl_appendWidget();

        /*Q_UNUSED(flag_lastPage);
        this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;
        this->list_track->append(list_data);
        this->createList_itemTrackDelegate_applyingWithData_showImage(list_data, this->listWidget_track, 0, SECTION_FOR_MORE_POPUP___tracks, true);*/
    }

    /**
     * @brief [Slot] ListWidgetItem 관련 - 썸네일 다운로드 완료
     */
    /*void TidalMycollection::slot_thumbnailDownloaded_fotListWidget_track(){

        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());
        this->applyImageData_itemTrackDelegate_withFileDownload(fileDownloader_sender, this->listWidget_track, this->list_track);
    }*/


    /**
     * @brief TidalMycollection::slot_applyResult_myFavoriteArtists
     * @param list_data
     */
    void TidalMycollection::slot_applyResult_myFavoriteArtists(const QList<tidal::ArtistItemData> &list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_artist->append(list_data);
            this->flag_artist[1] = true;
        }

        this->flag_artist[0] = true;
        this->setUIControl_appendWidget();

        /*Q_UNUSED(flag_lastPage);
        this->list_artist->append(list_data);
        this->createList_itemArtist_applyingWithData(list_data, AbstractItem::ImageSizeMode::Square_180x180, this->hBox_artist, 0, SECTION_FOR_MORE_POPUP___artists);*/
    }


    /**
     * @brief TidalMycollection::slot_applyResult_myFavoriteVideos
     * @param list_data
     */
    void TidalMycollection::slot_applyResult_myFavoriteVideos(const QList<tidal::VideoItemData> &list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_video->append(list_data);
            this->flag_video[1] = true;
        }

        this->flag_video[0] = true;
        this->setUIControl_appendWidget();

        /*Q_UNUSED(flag_lastPage);
        this->list_video->append(list_data);
        this->createList_itemVideo_applyingWithData(list_data, AbstractItem::ImageSizeMode::Ractangle_340x188, this->hBox_video, 0, SECTION_FOR_MORE_POPUP___videos);*/
    }



    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalMycollection::slot_clickedItemPlaylist(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___playlists_created){
            this->proc_clicked_itemPlaylist(this->list_playlist_created, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___playlists_tidal){
            this->proc_clicked_itemPlaylist(this->list_playlist_tidal, clickMode, index, section);
        }

    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalMycollection::slot_clickedItemAlbum(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalMycollection::slot_clickedItemArtist(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemArtist(this->list_artist, clickMode, index, section);
    }


    /**
     * @brief [slot] override - ItemVideo 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalMycollection::slot_clickedItemVideo(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___videos){

            if(clickMode == AbstractItem::ClickMode::PlayBtn || clickMode == AbstractItem::ClickMode::AllBox){

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

                QJsonArray tmpJsonArr = QJsonArray();
                for(int i = index; i < this->list_video->size(); i++){
                    QJsonObject tmpJsonObj = this->list_video->at(i).jsonObj_toPlay;
                    tmpJsonArr.append(tmpJsonObj);
                }

                // Rose Play 요청
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_videolist(tmpJsonArr, playType);
            }
            else{
                this->proc_clicked_itemVideo(this->list_video, clickMode, index, section);
            }
        }
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    /*void TidalMycollection::slot_clickedItemTrack_inList(const int idx, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode){

        int section = ((AbstractTidalTrackDelegate*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
    }*/
    void TidalMycollection::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___tracks;

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->collection_track[idx]->getFavoritesStars();
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
                this->track_id_fav = this->list_track->at(idx).id;

                if(this->track_star_fav == 1){
                    // Tidal Track Favorite add
                    ProcCommon *proc = new ProcCommon(this);
                    proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                else if(this->track_star_fav == 0){
                    // Tidal Track Favorite del
                    ProcCommon *proc = new ProcCommon(this);
                    proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }

                // favorite 정보 가져오기
                ProcCommon *proc_fav = new ProcCommon(this);
                connect(proc_fav, &ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalMycollection::slot_tidal_completeReq_listAll_myFavoritesIds);
                proc_fav->request_tidal_getAll_favorites();
                this->flag_send_track = true;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalMycollection::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("TIDAL", QString("%1").arg(this->track_id_fav));
                this->flag_check_track = true;

                this->collection_track[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }

        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
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
    void TidalMycollection::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___playlists_created){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist_created, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___playlists_tidal){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist_tidal, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___artists){
            this->proc_clicked_optMorePopup_fromArtist(this->list_artist, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___videos){
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
                QJsonArray tmpJsonArr = QJsonArray();
                for(int i = index; i < this->list_video->size(); i++){
                    QJsonObject tmpJsonObj = this->list_video->at(i).jsonObj_toPlay;
                    tmpJsonArr.append(tmpJsonObj);
                }

                // Rose Play 요청
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_videolist(tmpJsonArr, clickMode);
            }
            else{
                this->proc_clicked_optMorePopup_fromVideo(this->list_video, index, clickMode);
            }
        }
    }
}
