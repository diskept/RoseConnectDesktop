#include "TidalNewMain.h"

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

    const int BTN_IDX_SUBTITLE_tracks = 5;
    const int BTN_IDX_SUBTITLE_albums = 6;
    const int BTN_IDX_SUBTITLE_playlists = 7;
    const int BTN_IDX_SUBTITLE_videos = 8;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___tracks = 1;
    const int SECTION_FOR_MORE_POPUP___albums = 2;
    const int SECTION_FOR_MORE_POPUP___playlists = 3;
    const int SECTION_FOR_MORE_POPUP___videos = 4;

    const QString TIDAL_API_PATH_ALBUM = "featured/new/albums";
    const QString TIDAL_API_PATH_TRACK = "featured/new/tracks";
    const QString TIDAL_API_PATH_PLAYLIST = "featured/new/playlists";
    const QString TIDAL_API_PATH_VIDEO = "featured/new/videos";

    const QString TIDAL_MAIN_TITLE = "NEW";
    const QString TIDAL_SUB_TITLE___album = "New Albums";
    const QString TIDAL_SUB_TITLE___track = "New Tracks";
    const QString TIDAL_SUB_TITLE___playlist = "New Playlists";
    const QString TIDAL_SUB_TITLE___video = "New Videos";


    /**
     * @brief "TIDAL > 탐색 > NEW 메인" 화면의 생성자
     * @param parent
     */
    TidalNewMain::TidalNewMain(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_filter, parent){

        this->addUIControl_mainTitle(TIDAL_MAIN_TITLE);

        this->list_album = new QList<tidal::AlbumItemData>();
        this->list_track = new QList<tidal::TrackItemData>();
        this->list_playlist = new QList<tidal::PlaylistItemData>();
        this->list_video = new QList<tidal::VideoItemData>();

    }


    /**
     * @brief 소멸자.
     */
    TidalNewMain::~TidalNewMain(){
        this->deleteLater();
    }

    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalNewMain::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            this->page = pageCode;

            // init data
            this->list_album->clear();
            this->list_track->clear();
            this->list_playlist->clear();
            this->list_video->clear();

            this->flag_album[0] = false;
            this->flag_track[0] = false;
            this->flag_playlist[0] = false;
            this->flag_video[0] = false;

            this->flag_album[1] = false;
            this->flag_track[1] = false;
            this->flag_playlist[1] = false;
            this->flag_video[1] = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            ProcCommon *proc_album = new ProcCommon(this);
            connect(proc_album, &ProcCommon::completeReq_list_albums, this, &TidalNewMain::slot_applyResult_albums);
            proc_album->request_tidal_getList_albums(TIDAL_API_PATH_ALBUM, 10, 0);

            ProcCommon *proc_track = new ProcCommon(this);
            connect(proc_track, &ProcCommon::completeReq_list_tracks, this, &TidalNewMain::slot_applyResult_tracks);
            proc_track->request_tidal_getList_tracks(TIDAL_API_PATH_TRACK, 5, 0);

            ProcCommon *proc_playlist = new ProcCommon(this);
            connect(proc_playlist, &ProcCommon::completeReq_list_playlists, this, &TidalNewMain::slot_applyResult_playlists);
            proc_playlist->request_tidal_getList_playlists(TIDAL_API_PATH_PLAYLIST,10,0);

            ProcCommon *proc_video = new ProcCommon(this);
            connect(proc_video, &ProcCommon::completeReq_list_videos, this, &TidalNewMain::slot_applyResult_videos);
            proc_video->request_tidal_getList_videos(TIDAL_API_PATH_VIDEO, 10, 0);

        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }

    /**
     * @brief 타이달 > 탐색 > "장르" 1개 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalNewMain::setActivePage(){

        if(this->flagNeedReload == true){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            this->box_contents->removeWidget(this->widget_main_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 10, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // init UI
            this->vBox_album = new QVBoxLayout();
            this->vBox_track = new QVBoxLayout();
            this->vBox_playlist = new QVBoxLayout();
            this->vBox_video = new QVBoxLayout();

            this->hBox_album = new QHBoxLayout();
            this->hBox_track = new QHBoxLayout();
            this->hBox_playlist = new QHBoxLayout();
            this->hBox_video = new QHBoxLayout();

            // clear UI
            GSCommon::clearLayout(this->vBox_album);
            GSCommon::clearLayout(this->vBox_track);
            GSCommon::clearLayout(this->vBox_playlist);
            GSCommon::clearLayout(this->vBox_video);

            GSCommon::clearLayout(this->hBox_album);
            GSCommon::clearLayout(this->hBox_track);
            GSCommon::clearLayout(this->hBox_playlist);
            GSCommon::clearLayout(this->hBox_video);

            // sub Title UI
            for(int i = 0; i < 10; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }


            for(int i = 0; i < 10; i++){
                this->new_album[i] = new tidal::ItemAlbum(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->new_album[i], &tidal::ItemAlbum::signal_clicked, this, &TidalNewMain::slot_clickedItemAlbum);
            }

            for (int i = 0; i < 5; i++) {
                this->new_track[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->new_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TidalNewMain::slot_clickedItemTrack_inList);
                this->new_track[i]->setProperty("index", i);
                this->new_track[i]->setObjectName("new_tracks");
            }

            for(int i = 0; i < 10; i++){
                this->new_playlist[i] = new tidal::ItemPlaylist(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->new_playlist[i], &tidal::ItemPlaylist::signal_clicked, this, &TidalNewMain::slot_clickedItemPlaylist);
            }

            for(int i = 0; i < 10; i++){
                this->new_video[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___videos, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157);
                connect(this->new_video[i], &tidal::ItemVideo::signal_clicked, this, &TidalNewMain::slot_clickedItemVideo);
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------


    void TidalNewMain::setUIControl_appendWidget(){

        if(this->flag_album[0] == true && this->flag_track[0] == true && this->flag_playlist[0] == true && this->flag_video[0] == true){

            if(this->flag_track[1] == true){
                this->widget_track = new QWidget();
                this->widget_track = this->setUIControl_subTitle_withSideBtn(QString("New Tracks (%1)").arg(this->list_track->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_tracks, this->vBox_track);

                this->vBox_track->addSpacing(10);

                QList<QString> tmp_clientkey;
                int maxCount = this->list_track->size();
                if(this->list_track->size() > 5){
                    maxCount = 5;
                }

                for(int i = 0; i < maxCount; i++){
                    this->new_track[i]->setDataTrackInfo_Tidal(this->list_track->at(i));
                    this->new_track[i]->resize(1550, 70);

                    this->vBox_track->addWidget(this->new_track[i]);

                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));
                }

                this->box_main_contents->addLayout(this->vBox_track);
                this->box_main_contents->addSpacing(30);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalNewMain::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("TIDAL", tmp_clientkey);
            }
            else{
                this->widget_track = new QWidget();
                this->widget_track = this->setUIControl_subTitle_withSideBtn(QString("New Tracks (0)"), "View All", BTN_IDX_SUBTITLE_tracks, this->vBox_track);

                this->vBox_track->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
                noData_widget->setFixedHeight(275);
                this->vBox_track->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_track);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_album[1] == true){
                this->widget_album = new QWidget();
                this->widget_album = this->setUIControl_subTitle_withSideBtn(QString("New Albums (%1)").arg(this->list_album->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_albums, this->vBox_album);

                this->vBox_album->addSpacing(10);

                this->hBox_album = this->setUIControl_hBoxLayout_forAlbum(this->vBox_album);

                for(int i = 0; i < this->list_album->size(); i++){
                    this->new_album[i]->setData(this->list_album->at(i));
                    this->hBox_album->addWidget(this->new_album[i]);
                }

                this->box_main_contents->addLayout(this->vBox_album);
                this->box_main_contents->addSpacing(30);
            }
            else{
                this->widget_album = new QWidget();
                this->widget_album = this->setUIControl_subTitle_withSideBtn(QString("New Albums (0)"), "View All", BTN_IDX_SUBTITLE_albums, this->vBox_album);

                this->vBox_album->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedHeight(285);
                this->vBox_album->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_album);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_playlist[1] == true){
                this->widget_playlist = new QWidget();
                this->widget_playlist = this->setUIControl_subTitle_withSideBtn(QString("New Playlists (%1)").arg(this->list_playlist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_playlists, this->vBox_playlist);
                this->vBox_playlist->addSpacing(10);

                this->hBox_playlist = this->setUIControl_hBoxLayout_forPlaylists(this->vBox_playlist);

                for(int i = 0; i < this->list_playlist->size(); i++){
                    this->new_playlist[i]->setData(this->list_playlist->at(i));
                    this->hBox_playlist->addWidget(this->new_playlist[i]);
                }

                this->box_main_contents->addLayout(this->vBox_playlist);
                this->box_main_contents->addSpacing(30);
            }
            else{
                this->widget_playlist = new QWidget();
                this->widget_playlist = this->setUIControl_subTitle_withSideBtn(QString("New Playlists (0)"), "View All", BTN_IDX_SUBTITLE_playlists, this->vBox_playlist);

                this->vBox_playlist->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedHeight(305);
                this->vBox_playlist->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_playlist);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_video[1] == true){
                this->widget_video = new QWidget();
                this->widget_video = this->setUIControl_subTitle_withSideBtn(QString("New Videos (%1)").arg(this->list_video->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_videos, this->vBox_video);
                this->vBox_video->addSpacing(10);

                this->hBox_video = this->setUIControl_hBoxLayout_forVideo(this->vBox_video);

                for(int i = 0; i < this->list_video->size(); i++){
                    this->new_video[i]->setData(this->list_video->at(i));
                    this->hBox_video->addWidget(this->new_video[i]);
                }

                this->box_main_contents->addLayout(this->vBox_video);
                this->box_main_contents->addSpacing(30);
            }
            else{
                this->widget_video = new QWidget();
                this->widget_video = this->setUIControl_subTitle_withSideBtn(QString("New Videos (0)"), "View All", BTN_IDX_SUBTITLE_videos, this->vBox_video);

                this->vBox_video->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedHeight(285);
                this->vBox_video->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_video);
                this->box_main_contents->addSpacing(30);
            }

            ContentLoadingwaitingMsgHide();
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------


    QWidget* TidalNewMain::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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

    QHBoxLayout* TidalNewMain::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

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

    QHBoxLayout* TidalNewMain::setUIControl_hBoxLayout_forPlaylists(QLayout *p_layout){

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

    QHBoxLayout* TidalNewMain::setUIControl_hBoxLayout_forVideo(QLayout *p_layout){

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
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    //void TidalNewMain::slot_clickBtn_subTitle_side(const int btnId){
    void TidalNewMain::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_tracks){
            PageInfo_TrackAllView data_page;
            data_page.pathTitle = TIDAL_SUB_TITLE___track;
            data_page.api_subPath = TIDAL_API_PATH_TRACK;
            this->movePage_track_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_albums){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = TIDAL_SUB_TITLE___album;
            data_page.api_subPath = TIDAL_API_PATH_ALBUM;
            this->movePage_album_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_playlists){
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = TIDAL_SUB_TITLE___playlist;
            data_page.api_subPath = TIDAL_API_PATH_PLAYLIST;
            this->movePage_playlist_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_videos){
            PageInfo_VideoAllView data_page;
            data_page.pathTitle = TIDAL_SUB_TITLE___video;
            data_page.api_subPath = TIDAL_API_PATH_VIDEO;
            this->movePage_video_allView(data_page);
        }
    }


    void TidalNewMain::slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고.
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            // Tidal favorite check
            if(this->flag_send_track == true){
                QVariantList arr_myFavoriteIds = ProcJsonEasy::getJsonArray(p_jsonObj, "TRACK").toVariantList();
                bool status = arr_myFavoriteIds.contains(this->track_id_fav);

                if(status == true && this->flag_track_fav == false){
                    // Tidal track Favorite del
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalNewMain::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                else if(status == false && this->flag_track_fav == true){
                    // Tidal track Favorite add
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalNewMain::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                this->flag_send_track = false;
            }
            else{

            }
        }
    }


    void TidalNewMain::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

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
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalNewMain::slot_applyResult_getRating_track);
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
                connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalNewMain::slot_applyResult_addRating_track);
                proc_fav_track->request_rose_addRating_Track(json);
            }
        }
        else{

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void TidalNewMain::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void TidalNewMain::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->new_track[i]->setFavoritesIds(flag, star);
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
     * @brief Album 목록 데이터를 화면에 보인다.
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void TidalNewMain::slot_applyResult_albums(const QList<tidal::AlbumItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_album->append(list_data);
            this->flag_album[1] = true;
        }

        this->flag_album[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Track 데이터를 받아서 처리함
     * @param list_data
     */
    void TidalNewMain::slot_applyResult_tracks(const QList<tidal::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
    }

    void TidalNewMain::slot_applyResult_playlists(const QList<tidal::PlaylistItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_playlist->append(list_data);
            this->flag_playlist[1] = true;
        }

        this->flag_playlist[0] = true;
        this->setUIControl_appendWidget();
    }

    void TidalNewMain::slot_applyResult_videos(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_video->append(list_data);
            this->flag_video[1] = true;
        }

        this->flag_video[0] = true;
        this->setUIControl_appendWidget();
    }

    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalNewMain::slot_clickedItemAlbum(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void TidalNewMain::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___tracks;

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->new_track[idx]->getFavoritesStars();
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
                connect(proc_fav, &ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalNewMain::slot_tidal_completeReq_listAll_myFavoritesIds);
                proc_fav->request_tidal_getAll_favorites();
                this->flag_send_track = true;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalNewMain::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("TIDAL", QString("%1").arg(this->track_id_fav));
                this->flag_check_track = true;

                this->new_track[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }

        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }

    void TidalNewMain::slot_clickedItemPlaylist(const ItemPlaylist::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);
    }


    void TidalNewMain::slot_clickedItemVideo(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

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
    void TidalNewMain::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, section, clickMode);
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
