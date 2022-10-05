#include "tidal/TidalArtistDetail.h"

#include "tidal/ProcCommon.h"
#include "tidal/ConvertData.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include <QScroller>
#include <QPushButton>


namespace tidal {

    const int BTN_IDX_SUBTITLE_tracks = 1;
    const int BTN_IDX_SUBTITLE_albums = 2;
    const int BTN_IDX_SUBTITLE_videos = 3;
    const int BTN_IDX_SUBTITLE_artists = 4;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___artists_main = 0;

    const int SECTION_FOR_MORE_POPUP___tracks = 1;
    const int SECTION_FOR_MORE_POPUP___albums = 2;
    const int SECTION_FOR_MORE_POPUP___videos = 3;
    const int SECTION_FOR_MORE_POPUP___artists = 4;



    /**
     * @brief
     * @param parent
     */
    TidalArtistDetail::TidalArtistDetail(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_filter, parent) {

        // data
        this->list_track = new QList<tidal::TrackItemData>();
        this->list_album = new QList<tidal::AlbumItemData>();
        this->list_video = new QList<tidal::VideoItemData>();
        this->list_artist = new QList<tidal::ArtistItemData>();
    }

    /**
     * @brief 소멸자.
     */
    TidalArtistDetail::~TidalArtistDetail(){

        this->deleteLater();
    }


    /**
     * @brief TidalArtistDetail::setJsonObject_forData
     * @param jsonObj
     */
    void TidalArtistDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        tidal::ArtistItemData tmp_data_artist = ConvertData::convertData_artistItemData(jsonObj);

        this->flagNeedReload = false;

        if(tmp_data_artist.id != this->data_artist.id){

            this->flagNeedReload = true;

            if(this->flag_track[1] == true){
                this->widget_main_contents->hide();
            }

            // clear Data
            this->list_track->clear();
            this->list_album->clear();
            this->list_video->clear();
            this->list_artist->clear();

            this->flag_track[0] = false;
            this->flag_album[0] = false;
            this->flag_video[0] = false;
            this->flag_artist[0] = false;

            this->flag_track[1] = false;
            this->flag_album[1] = false;
            this->flag_video[1] = false;
            this->flag_artist[1] = false;

            this->flag_check_track = false;
            this->flag_track_fav = false;
            this->flag_send_track = false;

            this->track_id_fav = 0;
            this->track_idx_fav = 0;
            this->track_star_fav = 0;

            this->data_artist = tmp_data_artist;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            if(this->data_artist.name.isEmpty()){
                // Artist 기본 정보를 가져와야 하는 경우임. (id만 전달받은 상태)
                ProcCommon *proc = new ProcCommon(this);
                connect(proc, &ProcCommon::completeReq_artist, this, &TidalArtistDetail::slot_applyResult_artistInfo);
                proc->request_tidal_get_artist(this->data_artist.id);
            }
            else if(this->data_artist.biography.isEmpty()){
                ProcCommon *proc_bio = new ProcCommon(this);
                connect(proc_bio, &ProcCommon::completeReq_artist_bioText, this, &TidalArtistDetail::slot_applyResult_artist_bioText);
                proc_bio->request_tidal_get_artist_bioText(this->data_artist.id);
            }

            // request HTTP API
            this->api_subPath_topTrack = QString("artists/%1/toptracks").arg(this->data_artist.id);     // track 목록
            this->api_subPath_album = QString("artists/%1/albums").arg(this->data_artist.id);           // album 목록
            this->api_subPath_video = QString("artists/%1/videos").arg(this->data_artist.id);           // video 목록
            this->api_subPath_relatedArtist = QString("artists/%1/related").arg(this->data_artist.id);  // ar4tist 목록

            /*(ProcCommon *proc_fav = new ProcCommon(this);
            connect(proc_fav, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalArtistDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
            proc_fav->request_tidal_getAll_favorites();*/

            ProcCommon *proc_track = new ProcCommon(this);
            connect(proc_track, &ProcCommon::completeReq_list_tracks, this, &TidalArtistDetail::slot_applyResult_tracks);
            proc_track->request_tidal_getList_tracks(this->api_subPath_topTrack, 5, 0);

            ProcCommon *proc_album = new ProcCommon(this);
            connect(proc_album, &ProcCommon::completeReq_list_albums, this, &TidalArtistDetail::slot_applyResult_albums);
            proc_album->request_tidal_getList_albums(this->api_subPath_album, 15, 0);

            ProcCommon *proc_video = new ProcCommon(this);
            connect(proc_video, &ProcCommon::completeReq_list_videos, this, &TidalArtistDetail::slot_applyResult_videos);
            proc_video->request_tidal_getList_videos(this->api_subPath_video, 15, 0);

            ProcCommon *proc_artist = new ProcCommon(this);
            connect(proc_artist, &ProcCommon::completeReq_list_artists, this, &TidalArtistDetail::slot_applyResult_artists);
            proc_artist->request_tidal_getList_artists(this->api_subPath_relatedArtist, 50, 0);
        }
        else{
            this->flag_reload_page = true;

            // 리로드 하지 않는 경우에는, favorite 정보만 다시 요청한다. (artist_id 가 변경되지 않고, 페이지가 다시 요청된 경우임)
            /*ProcCommon *proc = new ProcCommon(this);
            connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalArtistDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
            proc->request_tidal_getAll_favorites();*/
        }
    }

    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalArtistDetail::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            // clear UI
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

            this->setUIControl_basic();
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 화면 UI 세팅
     */
    void TidalArtistDetail::setUIControl_basic(){

        this->artist_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Tidal_artist);
        connect(this->artist_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &TidalArtistDetail::slot_detailInfo_btnClicked);
        //connect(this->artist_detail_info, &AbstractImageDetailContents_RHV::signal_clicked_artistMore, this, &TidalArtistDetail::slot_artistInfo_btnClicked);

        this->artist_detail_info->initView();

        this->box_main_contents->addWidget(this->artist_detail_info);
        this->box_main_contents->addSpacing(75);

        for(int i = 0; i < 10; i++){
            this->lb_subTitle[i] = new QLabel();
            this->btnView_all[i] = new QPushButton();
        }

        this->vBox_track = new QVBoxLayout();
        this->vBox_album = new QVBoxLayout();
        this->vBox_video = new QVBoxLayout();
        this->vBox_artist = new QVBoxLayout();

        this->hBox_album = new QHBoxLayout();
        this->hBox_video = new QHBoxLayout();
        this->hBox_artist = new QHBoxLayout();

        // clear UI
        GSCommon::clearLayout(this->vBox_track);
        GSCommon::clearLayout(this->vBox_album);
        GSCommon::clearLayout(this->vBox_video);
        GSCommon::clearLayout(this->vBox_artist);

        GSCommon::clearLayout(this->hBox_album);
        GSCommon::clearLayout(this->hBox_video);
        GSCommon::clearLayout(this->hBox_artist);

        for (int i = 0; i < 5; i++) {
            this->track[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TidalArtistDetail::slot_clickedItemTrack_inList);

            this->track[i]->setProperty("index", i);
            this->track[i]->setObjectName("track");
        }

        for(int i = 0; i < 15; i++){
            this->album[i] = new tidal::ItemAlbum(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->album[i], &tidal::ItemAlbum::signal_clicked, this, &TidalArtistDetail::slot_clickedItemAlbum);
        }

        for(int i = 0; i < 15; i++){
            this->video[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___videos, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200);
            connect(this->video[i], &tidal::ItemVideo::signal_clicked, this, &TidalArtistDetail::slot_clickedItemVideo);

        }

        for(int i = 0; i < 15; i++){
            this->artist[i] = new tidal::ItemArtist(i, SECTION_FOR_MORE_POPUP___artists, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->artist[i], &tidal::ItemArtist::signal_clicked, this, &TidalArtistDetail::slot_clickedItemArtist);
        }
    }


    void TidalArtistDetail::setUIControl_appendWidget(){

        if(this->flag_track[0] == true && this->flag_album[0] == true && this->flag_video[0] == true && this->flag_artist[0] == true){

            if(this->flag_track[1] == true){
                this->widget_track = new QWidget();
                this->widget_track = this->setUIControl_subTitle_withSideBtn(QString(tr("Top Tracks") + " (%1)").arg(this->list_track->at(0).id), "View All", BTN_IDX_SUBTITLE_tracks, this->vBox_track);

                this->vBox_track->addSpacing(10);

                int max_cnt = this->list_track->count();
                if(max_cnt > 5){
                    max_cnt = 5;
                }

                for(int i = 0; i < max_cnt; i++){
                    this->track[i]->setDataTrackInfo_Tidal(this->list_track->at(i));

                    this->vBox_track->addWidget(this->track[i]);
                }

                this->box_main_contents->addLayout(this->vBox_track);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_album[1] == true){
                this->widget_album = new QWidget();
                this->widget_album = this->setUIControl_subTitle_withSideBtn(QString(tr("Albums") + " (%1)").arg(this->list_album->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_albums, this->vBox_album);

                this->vBox_album->addSpacing(10);

                this->hBox_album = this->setUIControl_hBoxLayout_forAlbum(this->vBox_album);

                int max_cnt = this->list_album->count();
                if(max_cnt > 15){
                    max_cnt = 15;
                }

                for(int i = 0; i < max_cnt; i++){
                    this->album[i]->setData(this->list_album->at(i));
                    this->hBox_album->addWidget(this->album[i]);
                }

                this->box_main_contents->addLayout(this->vBox_album);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_video[1] == true){
                this->widget_video = new QWidget();
                this->widget_video = this->setUIControl_subTitle_withSideBtn(QString(tr("Videos") + " (%1)").arg(this->list_video->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_videos, this->vBox_video);

                this->vBox_video->addSpacing(10);

                this->hBox_video = this->setUIControl_hBoxLayout_forAlbum(this->vBox_video);

                int max_cnt = this->list_video->count();
                if(max_cnt > 15){
                    max_cnt = 15;
                }

                for(int i = 0; i < max_cnt; i++){
                    this->video[i]->setData(this->list_video->at(i));
                    this->hBox_video->addWidget(this->video[i]);
                }

                this->box_main_contents->addLayout(this->vBox_video);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_artist[1] == true){
                this->widget_artist = new QWidget();
                this->widget_artist = this->setUIControl_subTitle_withSideBtn(QString(tr("Related Artists") + " (%1)").arg(this->list_artist->at(0).artist_total_cnt), "View All", BTN_IDX_SUBTITLE_artists, this->vBox_artist);

                this->vBox_artist->addSpacing(10);

                this->hBox_artist = this->setUIControl_hBoxLayout_forAlbum(this->vBox_artist);

                int max_cnt = this->list_artist->count();
                if(max_cnt > 15){
                    max_cnt = 15;
                }

                for(int i = 0; i < max_cnt; i++){
                    this->artist[i]->setData(this->list_artist->at(i));
                    this->hBox_artist->addWidget(this->artist[i]);
                }

                this->box_main_contents->addLayout(this->vBox_artist);
                this->box_main_contents->addSpacing(30);
            }

            ContentLoadingwaitingMsgHide();
        }
    }


    QWidget* TidalArtistDetail::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QHBoxLayout* TidalArtistDetail::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

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
        tmp_scrollArea->setFixedHeight(275);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_album;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief TidalArtistDetail::slot_applyResult_artistInfo
     * @param data_artist
     */
    void TidalArtistDetail::slot_applyResult_artistInfo(tidal::ArtistItemData data_artist){

        // Update Data
        this->data_artist = data_artist;

        if(this->data_artist.biography.isEmpty()){
            ProcCommon *proc_bio = new ProcCommon(this);
            connect(proc_bio, &ProcCommon::completeReq_artist_bioText, this, &TidalArtistDetail::slot_applyResult_artist_bioText);
            proc_bio->request_tidal_get_artist_bioText(this->data_artist.id);
        }
        else{
            QJsonObject artistObj;
            artistObj.insert("name", this->data_artist.name);
            artistObj.insert("image", this->data_artist.image);
            artistObj.insert("biography", this->data_artist.biography);
            artistObj.insert("id", this->data_artist.id);

            // Update UI
            this->artist_detail_info->setData_fromTidalData(artistObj);
            this->artist_detail_info->setFavorite(false, 0);
        }
    }


    /**
     * @brief TidalArtistDetail::slot_applyResult_artist_bioText
     * @param bioText
     */
    void TidalArtistDetail::slot_applyResult_artist_bioText(const QString& bioText){

        this->data_artist.biography = bioText;

        QJsonObject artistObj;
        artistObj.insert("name", this->data_artist.name);
        artistObj.insert("image", this->data_artist.image);
        artistObj.insert("biography", this->data_artist.biography);
        artistObj.insert("id", this->data_artist.id);

        // Update UI
        this->artist_detail_info->setData_fromTidalData(artistObj);
        this->artist_detail_info->setFavorite(false, 0);
    }


    /**
     * @brief TidalArtistDetail::slot_tidal_completeReq_listAll_myFavoritesIds
     * @param p_jsonObj
     */
    /*void TidalArtistDetail::slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고.
        this->artistImageDetailInfo->setFavoritesIds_forTidal(ProcJsonEasy::getJsonArray(p_jsonObj, "ARTIST").toVariantList());
    }*/


    /**
     * @brief TidalAlbumDetail::slot_applyResult_tracks
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void TidalArtistDetail::slot_applyResult_tracks(const QList<tidal::TrackItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.count() > 0){

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
            }
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            this->list_track->append(list_data);
            this->flag_track[1] = true;
        }

        this->flag_track[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void TidalArtistDetail::slot_applyResult_albums(const QList<tidal::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.count() > 0){
            this->list_album->append(list_data);

            this->flag_album[1] = true;
        }

        this->flag_album[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Video 데이터를 받아서 처리함
     * @param list_data
     * @param flag_lastPage
     */
    void TidalArtistDetail::slot_applyResult_videos(const QList<tidal::VideoItemData> &list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.count() > 0){
            this->list_video->append(list_data);

            this->flag_video[1] = true;
        }

        this->flag_video[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief TidalArtistDetail::slot_applyResult_artists
     * @param list_data
     */
    void TidalArtistDetail::slot_applyResult_artists(const QList<tidal::ArtistItemData> &list_data, const bool flag_lastPath){

        Q_UNUSED(flag_lastPath);

        if(list_data.count() > 0){
            this->list_artist->append(list_data);

            this->flag_artist[1] = true;
        }

        this->flag_artist[0] = true;
        this->setUIControl_appendWidget();
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Sub-title 의 우측에 있는 버튼 클릭에 대한 Signal 처리
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void TidalArtistDetail::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_tracks){
            PageInfo_TrackAllView data_page;
            data_page.pathTitle = QString("%1 - Top Tracks").arg(this->data_artist.name);
            data_page.api_subPath = this->api_subPath_topTrack;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_trackAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_TRACK_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_albums){
            PageInfo_TrackAllView data_page;
            data_page.pathTitle = QString("%1 - Albums").arg(this->data_artist.name);
            data_page.api_subPath = this->api_subPath_album;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_trackAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_ALBUM_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal

        }
        else if(btnId == BTN_IDX_SUBTITLE_videos){
            PageInfo_VideoAllView data_page;
            data_page.pathTitle = QString("%1 - Videos").arg(this->data_artist.name);
            data_page.api_subPath = this->api_subPath_video;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_videoAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_VIDEO_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal

        }
        else if(btnId == BTN_IDX_SUBTITLE_artists){
            PageInfo_TrackAllView data_page;
            data_page.pathTitle = QString("%1 - Related Artists").arg(this->data_artist.name);
            data_page.api_subPath = this->api_subPath_relatedArtist;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_trackAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_ARTIST_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }

    }


    void TidalArtistDetail::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){
        int id = ProcJsonEasy::getInt(jsonObj, "id");

        QJsonObject jsonObj_track = QJsonObject();
        jsonObj_track = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();

        if(id <= 0){

            QJsonObject tmp_album = ProcJsonEasy::getJsonObject(jsonObj_track, "album");

            QJsonObject tmp_artist = ProcJsonEasy::getJsonObject(tmp_album, "artist");
            QJsonObject artist;
            artist.insert("albums_count", ProcJsonEasy::getInt(tmp_artist, "albums_count"));
            artist.insert("id", ProcJsonEasy::getInt(tmp_artist, "id"));
            artist.insert("name", ProcJsonEasy::getString(tmp_artist, "name"));

            QJsonObject tmp_label = ProcJsonEasy::getJsonObject(tmp_album, "label");
            QJsonObject label;
            label.insert("albums_count", ProcJsonEasy::getInt(tmp_label, "albums_count"));
            label.insert("id", ProcJsonEasy::getInt(tmp_label, "id"));
            label.insert("name", ProcJsonEasy::getString(tmp_label, "name"));

            QJsonObject album;
            album.insert("artist", artist);
            album.insert("duration", ProcJsonEasy::getInt(tmp_album, "duration"));
            album.insert("favCnt", 0);
            album.insert("hires", false);
            album.insert("hires_streamable", false);
            album.insert("id", ProcJsonEasy::getString(tmp_album, "id"));
            album.insert("image", ProcJsonEasy::getJsonObject(tmp_album, "image"));
            album.insert("label", label);
            album.insert("streamable", false);
            album.insert("title", ProcJsonEasy::getString(tmp_album, "title"));
            album.insert("tracks_count", 0);

            QJsonObject data;
            data.insert("album", album);
            data.insert("composer", ProcJsonEasy::getJsonObject(jsonObj_track, "composer"));
            data.insert("copyright", ProcJsonEasy::getString(jsonObj_track, "copyright"));
            data.insert("duration", ProcJsonEasy::getInt(jsonObj_track, "duration"));
            data.insert("favCnt", ProcJsonEasy::getInt(jsonObj_track, "favCnt"));
            data.insert("hires", ProcJsonEasy::getBool(jsonObj_track, "hires"));
            data.insert("hires_streamable", ProcJsonEasy::getBool(jsonObj_track, "hires_streamable"));
            data.insert("id", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            data.insert("maximum_bit_depth",  ProcJsonEasy::getInt(jsonObj_track, "maximum_bit_depth"));
            data.insert("maximum_sampling_rate",  ProcJsonEasy::getInt(jsonObj_track, "maximum_sampling_rate"));
            data.insert("media_number", ProcJsonEasy::getInt(jsonObj_track, "media_number"));
            data.insert("performer", ProcJsonEasy::getJsonObject(jsonObj_track, "performer"));
            data.insert("performers", ProcJsonEasy::getString(jsonObj_track, "performers"));
            data.insert("previewable", ProcJsonEasy::getBool(jsonObj_track, "previewable"));
            data.insert("purchasable", ProcJsonEasy::getBool(jsonObj_track, "purchasable"));
            data.insert("qobuz_id", ProcJsonEasy::getInt(jsonObj_track, "qobuz_id"));
            data.insert("sampleable", ProcJsonEasy::getBool(jsonObj_track, "sampleable"));
            data.insert("streamable", ProcJsonEasy::getBool(jsonObj_track, "streamable"));
            data.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            data.insert("track_number", ProcJsonEasy::getInt(jsonObj_track, "track_number"));
            data.insert("work", ProcJsonEasy::getString(jsonObj_track, "work"));

            QJsonObject tmp_thumbnail = ProcJsonEasy::getJsonObject(tmp_album, "image");
            QString thumbnail = ProcJsonEasy::getString(tmp_thumbnail, "large");

            QJsonObject track;
            track.insert("clientKey", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            track.insert("data", data);
            track.insert("duration", 0);
            track.insert("favorite", this->flag_track_fav);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 1);
            track.insert("thumbnailUrl", thumbnail);
            track.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            track.insert("type", "TIDAL");

            QJsonArray tracks;
            tracks.append(track);

            QJsonObject json;
            json.insert("tracks", tracks);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalArtistDetail::slot_applyResult_addRating_track);
            proc_fav_track->request_rose_addRating_Track(json);
        }
        else{

            QJsonObject tmp_album = ProcJsonEasy::getJsonObject(jsonObj_track, "album");

            QJsonObject tmp_artist = ProcJsonEasy::getJsonObject(tmp_album, "artist");
            QJsonObject artist;
            artist.insert("albums_count", ProcJsonEasy::getInt(tmp_artist, "albums_count"));
            artist.insert("id", ProcJsonEasy::getInt(tmp_artist, "id"));
            artist.insert("name", ProcJsonEasy::getString(tmp_artist, "name"));

            QJsonObject tmp_label = ProcJsonEasy::getJsonObject(tmp_album, "label");
            QJsonObject label;
            label.insert("albums_count", ProcJsonEasy::getInt(tmp_label, "albums_count"));
            label.insert("id", ProcJsonEasy::getInt(tmp_label, "id"));
            label.insert("name", ProcJsonEasy::getString(tmp_label, "name"));

            QJsonObject album;
            album.insert("duration", 0);
            album.insert("favCnt", 0);
            album.insert("hires", false);
            album.insert("hires_streamable", false);
            album.insert("id", ProcJsonEasy::getString(tmp_album, "id"));
            album.insert("image", ProcJsonEasy::getJsonObject(tmp_album, "image"));
            album.insert("streamable", false);
            album.insert("title", ProcJsonEasy::getString(tmp_album, "title"));
            album.insert("tracks_count", 0);

            QJsonObject data;
            data.insert("album", album);
            data.insert("artist", artist);
            data.insert("composer", ProcJsonEasy::getJsonObject(jsonObj_track, "composer"));
            data.insert("copyright", ProcJsonEasy::getString(jsonObj_track, "copyright"));
            data.insert("duration", ProcJsonEasy::getInt(jsonObj_track, "duration"));
            data.insert("favCnt", ProcJsonEasy::getInt(jsonObj_track, "favCnt"));
            data.insert("hires", ProcJsonEasy::getBool(jsonObj_track, "hires"));
            data.insert("hires_streamable", ProcJsonEasy::getBool(jsonObj_track, "hires_streamable"));
            data.insert("id", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            data.insert("maximum_bit_depth",  ProcJsonEasy::getInt(jsonObj_track, "maximum_bit_depth"));
            data.insert("maximum_sampling_rate",  ProcJsonEasy::getInt(jsonObj_track, "maximum_sampling_rate"));
            data.insert("media_number", ProcJsonEasy::getInt(jsonObj_track, "media_number"));
            data.insert("performer", ProcJsonEasy::getJsonObject(jsonObj_track, "performer"));
            data.insert("performers", ProcJsonEasy::getString(jsonObj_track, "performers"));
            data.insert("previewable", ProcJsonEasy::getBool(jsonObj_track, "previewable"));
            data.insert("purchasable", ProcJsonEasy::getBool(jsonObj_track, "purchasable"));
            data.insert("qobuz_id", ProcJsonEasy::getInt(jsonObj_track, "qobuz_id"));
            data.insert("sampleable", ProcJsonEasy::getBool(jsonObj_track, "sampleable"));
            data.insert("streamable", ProcJsonEasy::getBool(jsonObj_track, "streamable"));
            data.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            data.insert("track_number", ProcJsonEasy::getInt(jsonObj_track, "track_number"));

            QJsonObject tmp_thumbnail = ProcJsonEasy::getJsonObject(tmp_album, "image");
            QString thumbnail = ProcJsonEasy::getString(tmp_thumbnail, "large");

            QJsonObject track;
            track.insert("clientKey", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            track.insert("data", data);
            track.insert("duration", 0);
            track.insert("favorite", this->flag_track_fav);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 0);
            track.insert("thumbnailUrl", thumbnail);
            track.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            track.insert("type", "TIDAL");

            QJsonObject ratingInfo;
            ratingInfo.insert("favorite", this->flag_track_fav);
            ratingInfo.insert("star", this->track_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "TIDAL");

            QJsonObject json;
            json.insert("ratingInfo", ratingInfo);
            json.insert("track", track);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalArtistDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

        }
    }


    void TidalArtistDetail::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void TidalArtistDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->track[i]->setFavoritesIds(flag, star);
                }

                /*if(this->flag_reload_page == true){
                    this->flag_reload_page = false;

                    if(this->list_appears->size() > 0){

                        int max_cnt = 0;
                        if(this->list_appears->size() > 5){
                            max_cnt = 5;
                        }
                        else{
                            max_cnt = this->list_appears->size();
                        }

                        QList<QString> tmp_clientkey;
                        for(int i = 0; i < max_cnt; i++){
                            tmp_clientkey.append(QString("%1").arg(this->list_appears->at(i).id));
                        }

                        // request HTTP API - get favorite for Rose Server
                        this->flag_appearsOn = true;
                        roseHome::ProcCommon *proc_fav_appear = new roseHome::ProcCommon(this);
                        connect(proc_fav_appear, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalArtistDetail::slot_applyResult_getRating_track);
                        proc_fav_appear->request_rose_getRating_Track("QOBUZ", tmp_clientkey);
                    }
                }*/
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
     * @brief [Slot] ArtistImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void TidalArtistDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

        /*if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx2 ||
                clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx3 || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){*/
        if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){

            if(this->flag_check_artist == false){

                this->artist_star_fav = this->artist_detail_info->getFavoritesStars();
                this->flag_artist_fav = false;

                //if(this->artist_star_fav == 3){
                if(this->artist_star_fav == 1){
                    this->artist_star_fav = 0;
                    this->flag_artist_fav = false;
                }
                //else if(this->artist_star_fav >= 0 && this->artist_star_fav < 3){
                else if(this->artist_star_fav == 0){
                    this->artist_star_fav++;
                    this->flag_artist_fav = true;
                }

                if(this->artist_star_fav == 0 || this->artist_star_fav == 1){
                    //ProcCommon *proc = new ProcCommon(this);
                    //proc->request_tidal_addFavorite_artist(this->data_artist.id);
                }

                // request HTTP API - get favorite for Rose Server
                /*roseHome::ProcCommon *proc_favCheck_artist = new roseHome::ProcCommon(this);
                connect(proc_favCheck_artist, &roseHome::ProcCommon::completeCheck_rating_album, this, &QobuzArtistDetail::slot_applyResult_checkRating_artist);
                proc_favCheck_artist->request_rose_checkRating_Artist("QOBUZ", this->data_artist.id);
                this->flag_check_artist = true;*/

                this->artist_detail_info->setFavorite(this->flag_artist_fav, this->artist_star_fav);
            }
        }
        else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Share){

            this->shareLink = this->artist_detail_info->detailInfo_shareLink;    //220907 share code ... --

            //220816 share code ...
            setUIShare();//c220816
        }
        else{
            this->proc_clicked_imageDetailInfo_fromArtist(this->data_artist, this->jsonArr_tracks_toPlay, SECTION_FOR_MORE_POPUP___artists_main, clickMode);
        }
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void TidalArtistDetail::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){
        int section = ((AbstractTidalTrackDelegate*)sender())->section();

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->track[idx]->getFavoritesStars();
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
                    // Qobuz Favorite toggle
                    this->track_id_fav = this->list_track->at(idx).id;

                    /*ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalArtistDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                    this->flag_send_track = true;*/
                }

                this->track_idx_fav = idx;
                QJsonObject jsonObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalArtistDetail::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("TIDAL", QString("%1").arg(ProcJsonEasy::getInt(jsonObj, "id")));
                this->flag_check_track = true;

                //this->track[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
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
    void TidalArtistDetail::slot_clickedItemAlbum(const ItemAlbum::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
    }


    /**
     * @brief TidalArtistDetail::slot_clickedItemVideo
     * @param clickMode
     */
    void TidalArtistDetail::slot_clickedItemVideo(const ItemVideo::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemVideo(this->list_video, clickMode, index, section);
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalArtistDetail::slot_clickedItemArtist(const ItemArtist::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemArtist(this->list_artist, clickMode, index, section);
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
    void TidalArtistDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___artists_main){
            this->proc_clicked_optMorePopup_fromArtist(this->data_artist, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___videos){
            this->proc_clicked_optMorePopup_fromVideo(this->list_video, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___artists){
            this->proc_clicked_optMorePopup_fromArtist(this->list_artist, index, clickMode);
        }
    }
}
