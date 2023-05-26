#include "rosetube/rosetube_HistoryDetail.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QScroller>


namespace rosetube {

    // subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX_SUBTITLE_HistoryPlaylist = 1;
    const int BTN_IDX_SUBTITLE_HistoryAlbum = 2;
    const int BTN_IDX_SUBTITLE_HistoryTrack = 3;
    const int BTN_IDX_SUBTITLE_HistoryArtist = 4;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___HistoryPlaylist = 1;
    const int SECTION_FOR_MORE_POPUP___HistoryAlbum = 2;
    const int SECTION_FOR_MORE_POPUP___HistoryTrack = 3;
    const int SECTION_FOR_MORE_POPUP___HistoryArtist = 4;

    const int HTTP_MY_PLAYLIST = 5;


    /**
     * @brief "RoseHome > 홈" 화면을 위한 생성자. @see PAGECODE_RH_HOME
     * @param parent
     */
    RoseTubeHistoryDetail::RoseTubeHistoryDetail(QWidget *parent) : AbstractRoseHomeSubWidget(VerticalScroll_rosefilter, parent) {

        this->linker = Linker::getInstance();

        this->label_mainTitle = this->get_addUIControl_mainTitle("");

        //data
        this->list_historyAlbum = new QList<roseHome::AlbumItemData>();
        this->list_historyPlaylist = new QList<roseHome::PlaylistItemData>();
        this->list_historyTrack = new QList<roseHome::TrackItemData>;
        this->list_historyArtist = new QList<roseHome::ArtistItemData>();

        this->jsonArr_tracks_toPlay = QJsonArray();
    }


    /**
     * @brief 소멸자.
     */
    RoseTubeHistoryDetail::~RoseTubeHistoryDetail(){

        this->deleteLater();
    }


    void RoseTubeHistoryDetail::setJsonObject_forData(const QJsonObject& jsonObj){

        roseHome::HistoryItemData tmphistory = roseHome::ConvertData::converData_historyItemData(jsonObj);
        this->flagNeedReload = false;

        if(tmphistory.yearMonth != this->historyData.yearMonth || tmphistory.filter_type != this->historyData.filter_type){
            this->flagNeedReload = true;

            this->historyData = tmphistory;

            if(this->historyData.yearMonth == "My Most Listened"){
                this->label_mainTitle->setText(this->historyData.yearMonth);
            }
            else{
                QDate chageDate = QDate::fromString(this->historyData.yearMonth, "yyyyMM");

                QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
                this->label_mainTitle->setText(QLocale().toString(chageDate, "MMMM yyyy"));

                QLocale::setDefault(QLocale::system());
            }


            this->list_historyAlbum->clear();
            this->list_historyPlaylist->clear();
            this->list_historyTrack->clear();
            this->list_historyArtist->clear();

            flag_historyAlbum[0] = false;
            flag_historyPlaylist[0] = false;
            flag_historyTrack[0] = false;
            flag_historyArtist[0] = false;

            flag_historyAlbum[1] = false;
            flag_historyPlaylist[1] = false;
            flag_historyTrack[1] = false;
            flag_historyArtist[1] = false;

            this->jsonArr_tracks_toPlay = QJsonArray();

            this->track_idx_fav = 0;
            this->track_star_fav = 0;

            this->flag_track_fav = false;
            this->flag_send_track = false;
            this->flag_check_track = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    void RoseTubeHistoryDetail::setActivePage(){

        if(this->flagNeedReload == true){
            AbstractRoseHomeSubWidget::setActivePage();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            this->box_rose_contents = new QVBoxLayout();
            this->box_rose_contents->setSpacing(0);
            this->box_rose_contents->setContentsMargins(0, 30, 0, 0);
            this->box_rose_contents->setAlignment(Qt::AlignTop);

            this->widget_rose_contents = new QWidget();
            this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_rose_contents->setLayout(this->box_rose_contents);

            this->box_contents->addWidget(this->widget_rose_contents, Qt::AlignTop);


            // init UI
            this->vBox_historyPlaylist = new QVBoxLayout();
            this->vBox_historyAlbum = new QVBoxLayout();
            this->vBox_historyTrack = new QVBoxLayout();
            this->vBox_historyArtist = new QVBoxLayout();

            this->hBox_historyAlbum = new QHBoxLayout();
            this->hBox_historyPlaylist = new QHBoxLayout();
            this->hBox_historyTrack = new QHBoxLayout();
            this->hBox_historyArtist = new QHBoxLayout();

            GSCommon::clearLayout(this->vBox_historyPlaylist);
            GSCommon::clearLayout(this->vBox_historyAlbum);
            GSCommon::clearLayout(this->vBox_historyTrack);
            GSCommon::clearLayout(this->vBox_historyArtist);

            GSCommon::clearLayout(this->hBox_historyAlbum);
            GSCommon::clearLayout(this->hBox_historyPlaylist);
            GSCommon::clearLayout(this->hBox_historyTrack);
            GSCommon::clearLayout(this->hBox_historyArtist);

            // sub Title UI
            for(int i = 0; i < 10; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            for(int i = 0; i < 15; i++){
                this->home_history_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___HistoryPlaylist, tidal::AbstractItem::ImageSizeMode::Square_200x200, HTTP_MY_PLAYLIST, true);
                connect(this->home_history_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseTubeHistoryDetail::slot_clickedItemPlaylist);
            }

            for(int i = 0; i < 15; i++){
                this->home_history_rosetube[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___HistoryTrack, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                connect(this->home_history_rosetube[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTubeHistoryDetail::slot_clickedItemPlaylist);
            }

            /*for(int i = 0; i < 15; i++){
                this->home_history_album[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___HistoryAlbum, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                connect(this->home_history_album[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &RoseTubeHistoryDetail::slot_clickedItemAlbum);
            }

            for (int i = 0; i < 5; i++) {
                this->home_history_track[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->home_history_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &RoseTubeHistoryDetail::slot_clickedItemTrack_inList);
                this->home_history_track[i]->setProperty("index", i);
                this->home_history_track[i]->setObjectName("history track");
            }

            for(int i = 0; i < 15; i++){
                this->home_history_artist[i] = new roseHome::ItemArtist_rosehome(i, SECTION_FOR_MORE_POPUP___HistoryArtist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_history_artist[i], &roseHome::ItemArtist_rosehome::signal_clicked, this, &RoseTubeHistoryDetail::slot_clickedItemArtist);
            }*/

            roseHome::ProcCommon *proc_playlist = new roseHome::ProcCommon(this);
            connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_playlists, this, &RoseTubeHistoryDetail::slot_applyResult_historyPlaylist);
            proc_playlist->request_rose_getList_historyPlaylists(this->historyData.yearMonth, this->historyData.filter_type, 0, 10);

            roseHome::ProcCommon *proc_track = new roseHome::ProcCommon(this);
            connect(proc_track, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseTubeHistoryDetail::slot_applyResult_historyTrack);
            proc_track->request_rose_getList_historyTracks(this->historyData.yearMonth, this->historyData.filter_type, 0, 10);

            /*roseHome::ProcCommon *proc_album = new roseHome::ProcCommon(this);
            connect(proc_album, &roseHome::ProcCommon::completeReq_list_albums, this, &RoseTubeHistoryDetail::slot_applyResult_historyAlbum);
            proc_album->request_rose_getList_historyAlbums(this->historyData.yearMonth, this->historyData.filter_type, 0, 10);

            roseHome::ProcCommon *proc_artist = new roseHome::ProcCommon(this);
            connect(proc_artist, &roseHome::ProcCommon::completeReq_list_artists, this, &RoseTubeHistoryDetail::slot_applyResult_historyArtist);
            proc_artist->request_rose_getList_historyArtists(this->historyData.yearMonth, this->historyData.filter_type, 0, 10);*/
        }
    }


    QWidget* RoseTubeHistoryDetail::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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



    void RoseTubeHistoryDetail::setUIControl_appendWidget_hitsory(){

        //if(this->flag_historyPlaylist[0] == true && this->flag_historyAlbum[0] == true && this->flag_historyTrack[0] == true && this->flag_historyArtist[0] == true){
        if(this->flag_historyPlaylist[0] == true && this->flag_historyTrack[0] == true){

            if(this->flag_historyPlaylist[0] == true){

                this->widget_historyPlaylist = new QWidget();
                QString subTitle = tr("History-Playlist");
                if(this->list_historyPlaylist->size() > 0){
                    this->widget_historyPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->list_historyPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_HistoryPlaylist, this->vBox_historyPlaylist);
                }
                else{
                    this->widget_historyPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_HistoryPlaylist, this->vBox_historyPlaylist);
                }

                this->vBox_historyPlaylist->addSpacing(10);
                this->vBox_historyPlaylist->setAlignment(Qt::AlignTop);

                if(this->flag_historyPlaylist[1] == true){
                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->hBox_historyPlaylist->setSpacing(0);
                    this->hBox_historyPlaylist->setContentsMargins(0, 0, 0, 0);
                    this->hBox_historyPlaylist->setAlignment(Qt::AlignTop);
                    this->hBox_historyPlaylist->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_historyPlaylist);
                    widget_content->setContentsMargins(0, 0, 0, 0);

                    QScrollArea *playlist_scrollArea = new QScrollArea();
                    playlist_scrollArea->setWidget(widget_content);
                    playlist_scrollArea->setWidgetResizable(false);
                    playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                    playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                    playlist_scrollArea->setFixedHeight(315);
                    playlist_scrollArea->setAlignment(Qt::AlignTop);

                    QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_historyPlaylist->addWidget(playlist_scrollArea, 0, Qt::AlignTop);

                    int maxCount = 0;
                    if(this->list_historyPlaylist->size() > 10){
                        maxCount = 10;
                    }
                    else{
                        maxCount = this->list_historyPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_history_playlist[i]->setData(this->list_historyPlaylist->at(i));
                        this->hBox_historyPlaylist->addWidget(this->home_history_playlist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 315);

                    this->vBox_historyPlaylist->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_historyPlaylist);
                this->box_rose_contents->addSpacing(30);
            }

            if(this->flag_historyTrack[0] == true){

                this->widget_historyTrack = new QWidget();
                QString subTitle = tr("History-Track");
                if(this->list_historyTrack->size() > 0){
                    this->widget_historyTrack = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->list_historyTrack->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_HistoryTrack, this->vBox_historyTrack);
                }
                else{
                    this->widget_historyTrack = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_HistoryTrack, this->vBox_historyTrack);
                }

                this->vBox_historyTrack->addSpacing(10);

                if(this->flag_historyTrack[1] == true){
                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->hBox_historyTrack->setSpacing(0);
                    this->hBox_historyTrack->setContentsMargins(0, 0, 0, 0);
                    this->hBox_historyTrack->setAlignment(Qt::AlignTop);
                    this->hBox_historyTrack->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_historyTrack);
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
                    this->vBox_historyTrack->addWidget(playlist_scrollArea, 0, Qt::AlignTop);

                    int maxCount = 0;
                    if(this->jsonArr_tracks_toPlay.count() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->jsonArr_tracks_toPlay.count();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_history_rosetube[i]->setData(this->jsonArr_tracks_toPlay.at(i).toObject());
                        this->hBox_historyTrack->addWidget(this->home_history_rosetube[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                    noData_widget->setFixedSize(1500, 308);

                    this->vBox_historyTrack->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_historyTrack);
                this->box_rose_contents->addSpacing(30);
            }

            /*if(this->flag_historyAlbum[0] == true){

                this->widget_historyAlbum = new QWidget();
                QString subTitle = tr("History-Album");
                this->widget_historyAlbum = this->setUIControl_subTitle_withSideBtn(subTitle, "View All", BTN_IDX_SUBTITLE_HistoryAlbum, this->vBox_historyAlbum);

                this->vBox_historyAlbum->addSpacing(10);

                if(this->flag_historyAlbum[1] == true){

                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->hBox_historyAlbum->setSpacing(0);
                    this->hBox_historyAlbum->setContentsMargins(0, 0, 0, 0);
                    this->hBox_historyAlbum->setAlignment(Qt::AlignTop);
                    this->hBox_historyAlbum->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_historyAlbum);
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
                    this->vBox_historyAlbum->addWidget(album_scrollArea);

                    int maxCount = 0;
                    if(this->list_historyAlbum->size() > 10){
                        maxCount = 10;
                    }
                    else{
                        maxCount = this->list_historyAlbum->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_history_album[i]->setData(this->list_historyAlbum->at(i));
                        this->hBox_historyAlbum->addWidget(this->home_history_album[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                    noData_widget->setFixedSize(1500, 305);

                    this->vBox_historyAlbum->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_historyAlbum);
                this->box_rose_contents->addSpacing(30);
            }

            if(this->flag_historyTrack[0] == true){

                // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.
                QHBoxLayout *tmp_hBox = new QHBoxLayout();
                tmp_hBox->setSpacing(0);
                tmp_hBox->setContentsMargins(0, 0, 0, 0);
                tmp_hBox->setAlignment(Qt::AlignTop);

                this->widget_historyTrack = new QWidget();
                this->widget_historyTrack->setStyleSheet("background-color:#0d0d0d; border:0px;");
                this->widget_historyTrack->setContentsMargins(40, 50, 47, 35);
                this->widget_historyTrack->setLayout(tmp_hBox);

                QString subTitle = tr("History-Track");
                this->lb_subTitle[BTN_IDX_SUBTITLE_HistoryTrack]->setText(subTitle);
                this->lb_subTitle[BTN_IDX_SUBTITLE_HistoryTrack]->setStyleSheet("font-size:24px;font-weight:bold;color:#FFFFFF;");
                tmp_hBox->addWidget(this->lb_subTitle[BTN_IDX_SUBTITLE_HistoryTrack], 0, Qt::AlignVCenter);

                // side button - it works when the button text is not empty
                this->btnView_all[BTN_IDX_SUBTITLE_HistoryTrack]->setText("View All");
                this->btnView_all[BTN_IDX_SUBTITLE_HistoryTrack]->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
                this->btnView_all[BTN_IDX_SUBTITLE_HistoryTrack]->setProperty("idx", BTN_IDX_SUBTITLE_HistoryTrack);
                this->btnView_all[BTN_IDX_SUBTITLE_HistoryTrack]->setCursor(Qt::PointingHandCursor);
                connect(this->btnView_all[BTN_IDX_SUBTITLE_HistoryTrack], SIGNAL(clicked()), this, SLOT(slot_clickBtn_subTitle_viewAll()));

                tmp_hBox->addWidget(this->btnView_all[BTN_IDX_SUBTITLE_HistoryTrack], 1, Qt::AlignVCenter | Qt::AlignRight);

                this->vBox_historyTrack->addWidget(this->widget_historyTrack);

                QVBoxLayout *vBox_historyTrack_info = new QVBoxLayout();
                vBox_historyTrack_info->setSpacing(0);
                vBox_historyTrack_info->setContentsMargins(40, 0, 54, 0);
                vBox_historyTrack_info->setAlignment(Qt::AlignTop);

                QWidget *widget_historyTrack_info = new QWidget();
                widget_historyTrack_info->setStyleSheet("background-color:#0d0d0d; border:0px;");
                widget_historyTrack_info->setContentsMargins(0, 0, 0, 0);
                widget_historyTrack_info->setFixedHeight(411);
                widget_historyTrack_info->setLayout(vBox_historyTrack_info);

                this->vBox_historyTrack->addWidget(widget_historyTrack_info);

                if(this->flag_historyTrack[1] == true){
                    int maxCount = 0;
                    if(this->list_historyTrack->at(0).totalCount > 5){
                        maxCount = 5;
                    }
                    else{
                        maxCount = this->list_historyTrack->at(0).totalCount;
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_history_track[i]->setDataTrackInfo_RoseMain(this->list_historyTrack->at(i));
                        this->home_history_track[i]->setFavoritesIds(this->list_historyTrack->at(i).favorite, this->list_historyTrack->at(i).star);
                        this->home_history_track[i]->resize(1550, 70);

                        vBox_historyTrack_info->addWidget(this->home_history_track[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
                    noData_widget->setFixedSize(1500, 350);

                    vBox_historyTrack_info->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_historyTrack);
                this->box_rose_contents->addSpacing(60);
            }

            if(this->flag_historyArtist[0] == true){

                this->widget_historyArtist = new QWidget();
                QString subTitle = tr("History-Artist");
                this->widget_historyArtist = this->setUIControl_subTitle_withSideBtn(subTitle, "View All", BTN_IDX_SUBTITLE_HistoryArtist, this->vBox_historyArtist);

                this->vBox_historyArtist->addSpacing(10);

                if(this->flag_historyArtist[1] == true){

                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->hBox_historyArtist->setSpacing(0);
                    this->hBox_historyArtist->setContentsMargins(0, 0, 0, 0);
                    this->hBox_historyArtist->setAlignment(Qt::AlignTop);
                    this->hBox_historyArtist->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_historyArtist);
                    widget_content->setContentsMargins(0, 0, 0, 0);

                    QScrollArea *artist_scrollArea = new QScrollArea();
                    artist_scrollArea->setWidget(widget_content);
                    artist_scrollArea->setWidgetResizable(false);
                    artist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    artist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    artist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                    artist_scrollArea->setContentsMargins(0, 0, 0, 0);
                    artist_scrollArea->setFixedHeight(255);

                    QScroller::grabGesture(artist_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_historyArtist->addWidget(artist_scrollArea);

                    int maxCount = 0;
                    if(this->list_historyArtist->size() > 10){
                        maxCount = 10;
                    }
                    else{
                        maxCount = this->list_historyArtist->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_history_artist[i]->setData(this->list_historyArtist->at(i));
                        this->hBox_historyArtist->addWidget(this->home_history_artist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Artist_NoData);
                    noData_widget->setFixedSize(1500, 255);

                    this->vBox_historyArtist->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_historyArtist);
                this->box_rose_contents->addSpacing(30);
            }*/

            ContentLoadingwaitingMsgHide();
        }
    }


    void RoseTubeHistoryDetail::slot_applyResult_historyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);
        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.length() > 0)
        {
            this->list_historyPlaylist->append(list_data);
            this->flag_historyPlaylist[1] = true;
        }

        this->flag_historyPlaylist[0] = true;
        this->setUIControl_appendWidget_hitsory();
    }


    void RoseTubeHistoryDetail::slot_applyResult_historyAlbum(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);
        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.length() > 0)
        {
            this->list_historyAlbum->append(list_data);
            this->flag_historyAlbum[1] = true;
        }

        this->flag_historyAlbum[0] = true;
        this->setUIControl_appendWidget_hitsory();
    }


    void RoseTubeHistoryDetail::slot_applyResult_historyTrack(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_historyTrack->append(list_data);
            this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;
            this->flag_historyTrack[1] = true;
        }

        this->flag_historyTrack[0] = true;
        this->setUIControl_appendWidget_hitsory();
    }


    void RoseTubeHistoryDetail::slot_applyResult_historyArtist(const QList<roseHome::ArtistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);
        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.length() > 0)
        {
            this->list_historyArtist->append(list_data);
            this->flag_historyArtist[1] = true;
        }

        this->flag_historyArtist[0] = true;
        this->setUIControl_appendWidget_hitsory();
    }


    void RoseTubeHistoryDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->home_history_track[i]->setFavoritesIds(flag, star);
                }
            }
            else if(track_info.contains("message") && (track_info["message"].toString() == "정상")){
                this->home_history_track[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void RoseTubeHistoryDetail::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_HistoryPlaylist){
            roseHome::PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = this->historyData.yearMonth;
            data_page.filter_type = this->historyData.filter_type;
            data_page.type = "history";
            data_page.type_id = HTTP_MY_PLAYLIST;

            QJsonObject jsonObj_move = roseHome::ConvertData::getObjectJson_pageInfo_playlistAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_HISTORY_PLAYLIST_VIEW);

            emit linker->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX_SUBTITLE_HistoryTrack){
            roseHome::PageInfo_TrackAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = this->historyData.yearMonth;
            data_page.filter_type = this->historyData.filter_type;
            data_page.type = "history";

            QJsonObject jsonObj_move = roseHome::ConvertData::getObjectJson_pageInfo_trackAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_HISTORY_TRACK_VIEW);

            emit linker->signal_clickedMovePage(jsonObj_move);
        }
        /*else if(btnId == BTN_IDX_SUBTITLE_HistoryAlbum){
            roseHome::PageInfo_AlbumAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = this->historyData.yearMonth;
            data_page.filter_type = this->historyData.filter_type;
            data_page.type = "history";

            QJsonObject jsonObj_move = roseHome::ConvertData::getObjectJson_pageInfo_albumAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_VA_ALBUM_HISTORY_ALL_VIEW);

            emit linker->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX_SUBTITLE_HistoryArtist){
            roseHome::PageInfo_ArtistAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = this->historyData.yearMonth;
            data_page.filter_type = this->historyData.filter_type;
            data_page.type = "history";

            QJsonObject jsonObj_move = roseHome::ConvertData::getObjectJson_pageInfo_artistAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_VA_ARTIST_HISTORY_ALL_VIEW);

            emit linker->signal_clickedMovePage(jsonObj_move);
        }*/
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
    void RoseTubeHistoryDetail::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리 - Album 파트가 여러개임
        if(section == SECTION_FOR_MORE_POPUP___HistoryAlbum){
            //this->proc_clicked_itemAlbum(this->list_historyAlbum, clickMode, index, section);

            roseHome::AlbumItemData data_album = this->list_historyAlbum->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, roseHome::ConvertData::getConvertOptHeaderData(data_album), index, SECTION_FOR_MORE_POPUP___HistoryAlbum, true);
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
    }


    /**
         * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
         * @param clickMode
         */
    void RoseTubeHistoryDetail::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___HistoryPlaylist){
            //this->proc_clicked_itemPlaylist(this->list_historyPlaylist, clickMode, index, section);

            roseHome::PlaylistItemData data_playlist = this->list_historyPlaylist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data_playlist), index, SECTION_FOR_MORE_POPUP___HistoryPlaylist, true);
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
                jsonData.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYDETAIL);

                emit linker->signal_clickedMovePage(jsonData);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___HistoryTrack){

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
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byTracks(data_playlist, this->jsonArr_tracks_toPlay, index, playType);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(index).toObject();

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
    }


    /**
         * @brief [Slot] PlaylistTrackDetailInfo_RHV signal에 대하여 처리
         * @param clickMode
         */
    void RoseTubeHistoryDetail::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___HistoryTrack;

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->home_history_track[idx]->getFavoritesStars();
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
                ratingInfo.insert("type", this->list_historyTrack->at(idx).type);

                QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseTubeHistoryDetail::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

                this->flag_check_track = true;
            }
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_historyTrack, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }


    /**
         * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
         * @param clickMode
         */
    void RoseTubeHistoryDetail::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리 - Album 파트가 여러개임
        if(section == SECTION_FOR_MORE_POPUP___HistoryArtist){
            this->proc_clicked_itemArtist(this->list_historyArtist, clickMode, index, section);
        }
    }


    /**
         * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
         * @param clickMode
         * @param index
         * @param section
         */
    void RoseTubeHistoryDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___HistoryPlaylist){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_historyPlaylist, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___HistoryTrack){
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
                procRosePlay->requestPlayRose_byTracks(data_track, this->jsonArr_tracks_toPlay, index, clickMode);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubscribInfo){
                QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(index).toObject();
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
            else if(clickMode == OptMorePopup::ClickMode::Share){//c220818
                setUIShare();//c220818
            }
        }
        /*else if(section == SECTION_FOR_MORE_POPUP___HistoryAlbum){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_historyAlbum, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___HistoryTrack){
            this->proc_clicked_optMorePopup_fromTrack(this->list_historyTrack, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___HistoryArtist){
            //this->proc_clicked_optMorePopup_from(this->list_historyArtist, index, section, clickMode);
        }*/
    }
}
