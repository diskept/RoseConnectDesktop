#include "qobuz/QobuzArtistDetail.h"

#include "qobuz/ProcCommon_forQobuz.h"
#include "qobuz/ConvertData_forQobuz.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"

#include <QScroller>
#include <QPushButton>


namespace qobuz {

    const int BTN_IDX_SUBTITLE_tracks = 1;
    const int BTN_IDX_SUBTITLE_lastrelease = 2;
    const int BTN_IDX_SUBTITLE_albums = 3;
    const int BTN_IDX_SUBTITLE_appearson = 4;
    const int BTN_IDX_SUBTITLE_artists = 5;


    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___tracks = 1;
    const int SECTION_FOR_MORE_POPUP___last_release = 2;
    const int SECTION_FOR_MORE_POPUP___albums = 3;
    const int SECTION_FOR_MORE_POPUP___appears_on = 4;
    const int SECTION_FOR_MORE_POPUP___artists_related = 5;

    const int SECTION_FOR_MORE_POPUP___artists_main = 14;

    /**
     * @brief
     * @param parent
     */
    QobuzArtistDetail::QobuzArtistDetail(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll_filter, parent) {

        // data
        this->list_track = new QList<qobuz::TrackItemData>();
        this->list_album = new QList<qobuz::AlbumItemData>();
        this->list_release = new QList<qobuz::AlbumItemData>();
        this->list_appears = new QList<qobuz::TrackItemData>();
        this->list_artist = new QList<qobuz::ArtistItemData>();
    }


    /**
     * @brief 소멸자.
     */
    QobuzArtistDetail::~QobuzArtistDetail(){

        this->deleteLater();
    }


    /**
     * @brief QobuzArtistDetail::setJsonObject_forData
     * @param jsonObj
     */
    void QobuzArtistDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        qobuz::ArtistItemData tmp_data_artist = ConvertData::convertData_artistItemData(jsonObj);

        this->flagNeedReload = false;
        if(tmp_data_artist.id != this->data_artist.id){

            this->flagNeedReload = true;

            if(this->flag_tracks[1] == true){
                this->widget_main_contents->hide();
            }

            // clear Data
            this->list_track->clear();
            this->list_release->clear();
            this->list_album->clear();
            this->list_appears->clear();
            this->list_artist->clear();

            this->flag_tracks[0] = false;
            this->flag_lastRelease[0] = false;
            this->flag_release[0] = false;
            this->flag_appears[0] = false;
            this->flag_suggestArtists[0] = false;

            this->flag_tracks[1] = false;
            this->flag_lastRelease[1] = false;
            this->flag_release[1] = false;
            this->flag_appears[1] = false;
            this->flag_suggestArtists[1] = false;

            this->flag_topTracks = false;
            this->flag_appearsOn = false;

            this->flag_check_track = false;
            this->flag_track_fav = false;
            this->flag_send_track = false;

            this->track_id_fav = 0;
            this->track_idx_fav = 0;
            this->track_star_fav = 0;

            this->data_artist = tmp_data_artist;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            if(this->data_artist.biography.isEmpty()){
                // Artist 기본 정보를 가져와야 하는 경우임. (id만 전달받은 상태)
                ProcCommon *proc = new ProcCommon(this);
                connect(proc, &ProcCommon::completeReq_artist, this, &QobuzArtistDetail::slot_applyResult_artistInfo);
                proc->request_qobuz_get_artist(this->data_artist.id);
            }

            // request HTTP API
            ProcCommon *proc_release = new ProcCommon(this);
            connect(proc_release, &ProcCommon::completeReq_list_albums, this, &QobuzArtistDetail::slot_applyResult_albums);
            connect(proc_release, &ProcCommon::completeReq_list_lastrelease, this, &QobuzArtistDetail::slot_applyResult_lastrelease);
            connect(proc_release, &ProcCommon::completeReq_list_tracks, this, &QobuzArtistDetail::slot_applyResult_tracks);
            proc_release->request_qobuz_getList_lastrelease("artist/get", "playlists,albums_with_last_release", data_artist.id, 15, 0);

            ProcCommon *proc_appears = new ProcCommon(this);
            connect(proc_appears, &ProcCommon::completeReq_list_appearson, this, &QobuzArtistDetail::slot_applyResult_appearson);
            proc_appears->request_qobuz_getList_appearson("artist/get", "tracks_appears_on", data_artist.id, 100, 0);

            ProcCommon *proc_artist = new ProcCommon(this);
            connect(proc_artist, &ProcCommon::completeReq_list_artists, this, &QobuzArtistDetail::slot_applyResult_artists);
            proc_artist->request_qobuz_getList_artists("artist/getSimilarArtists", data_artist.id, 15, 0);
        }
        else{
            this->flag_reload_page = true;

            this->flag_send_artist = false;
            ProcCommon *proc_fav_artist = new ProcCommon(this);
            connect(proc_fav_artist, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzArtistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
            proc_fav_artist->request_qobuz_get_favorite("artist", QString("%1").arg(this->data_artist.id));

            if(this->list_track->size() > 0){

                int max_cnt = 0;
                if(this->list_track->size() > 5){
                    max_cnt = 5;
                }
                else{
                    max_cnt = this->list_track->size();
                }

                QList<QString> tmp_clientkey;
                for(int i = 0; i < max_cnt; i++){
                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));
                }

                // request HTTP API - get favorite for Rose Server
                this->flag_topTracks = true;
                roseHome::ProcCommon *proc_fav_top = new roseHome::ProcCommon(this);
                connect(proc_fav_top, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzArtistDetail::slot_applyResult_getRating_track);
                proc_fav_top->request_rose_getRating_Track("QOBUZ", tmp_clientkey);
            }
        }
    }

    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void QobuzArtistDetail::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

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


    void QobuzArtistDetail::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId){

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

        this->box_main_contents->addWidget(widget_box_subTitle, 0, Qt::AlignTop);
    }


    QHBoxLayout* QobuzArtistDetail::setUIControl_hBoxLayout_forAlbum(){
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
        this->box_main_contents->addWidget(tmp_scrollArea, 0, Qt::AlignTop);

        // return
        return hBox_album;
    }


    QHBoxLayout* QobuzArtistDetail::setUIControl_hBoxLayout_forArtist(){
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
        tmp_scrollArea->setFixedHeight(250);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        this->box_main_contents->addWidget(tmp_scrollArea, 0, Qt::AlignTop);

        // return
        return hBox_album;
    }


    /**
     * @brief 화면 UI 세팅
     */
    void QobuzArtistDetail::setUIControl_basic(){

        this->artist_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Qobuz_artist);
        connect(this->artist_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &QobuzArtistDetail::slot_detailInfo_btnClicked);
        connect(this->artist_detail_info, &AbstractImageDetailContents_RHV::signal_clicked_artistMore, this, &QobuzArtistDetail::slot_artistInfo_btnClicked);

        this->artist_detail_info->initView();

        this->box_main_contents->addWidget(this->artist_detail_info);
        this->box_main_contents->addSpacing(75);

        for(int i = 0; i < 10; i++){
            this->lb_subTitle[i] = new QLabel();
            this->btnView_all[i] = new QPushButton();
        }

        this->vBox_topTracks = new QVBoxLayout();
        this->hBox_release = new QHBoxLayout();
        this->hBox_album = new QHBoxLayout();
        this->vBox_appearson = new QVBoxLayout();
        this->hBox_relatedArtist = new QHBoxLayout();

        // clear UI
        GSCommon::clearLayout(this->vBox_topTracks);
        GSCommon::clearLayout(this->hBox_release);
        GSCommon::clearLayout(this->hBox_album);
        GSCommon::clearLayout(this->vBox_appearson);
        GSCommon::clearLayout(this->hBox_relatedArtist);

        for (int i = 0; i < 5; i++) {
            this->top_tracks[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->top_tracks[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzArtistDetail::slot_clickedItemTrack_inList);
            this->top_tracks[i]->setProperty("index", i);
            this->top_tracks[i]->setObjectName("top_tracks");
        }

        for(int i = 0; i < 15; i++){
            this->last_release[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___last_release, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->last_release[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzArtistDetail::slot_clickedItemAlbum);
        }

        for(int i = 0; i < 15; i++){
            this->release[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->release[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzArtistDetail::slot_clickedItemAlbum);

        }

        for (int i = 0; i < 5; i++) {
            this->appears_on[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->appears_on[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzArtistDetail::slot_clickedItemTrack_inList);
            this->appears_on[i]->setProperty("index", i);
            this->appears_on[i]->setObjectName("appears_on");
        }

        for(int i = 0; i < 15; i++){
            this->related_artist[i] = new qobuz::ItemArtist_qobuz(i, SECTION_FOR_MORE_POPUP___artists_related, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->related_artist[i], &qobuz::ItemArtist_qobuz::signal_clicked, this, &QobuzArtistDetail::slot_clickedItemArtist);
        }
    }


    void QobuzArtistDetail::setUIControl_appendWidget(){

        if(this->flag_tracks[0] == true && this->flag_lastRelease[0] == true && this->flag_release[0] == true
                && this->flag_appears[0] == true && this->flag_suggestArtists[0] == true){

            if(this->flag_tracks[1] == true){
                this->setUIControl_subTitle_withSideBtn(QString("Top tracks (%1)").arg(this->list_track->count()), "View All", BTN_IDX_SUBTITLE_tracks);
                this->box_main_contents->addSpacing(10);

                this->box_main_contents->addLayout(this->vBox_topTracks);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_lastRelease[1] == true){
                this->setUIControl_subTitle_withSideBtn("Last release ", "View All", BTN_IDX_SUBTITLE_lastrelease);
                this->box_main_contents->addSpacing(10);

                this->hBox_release = this->setUIControl_hBoxLayout_forAlbum();
                this->box_main_contents->addSpacing(30);

                int max_cnt = this->list_release->count();
                if(max_cnt > 15){
                    max_cnt = 15;
                }

                for(int i = 0; i < max_cnt; i++){
                    this->last_release[i]->setData(this->list_release->at(i));
                    this->hBox_release->addWidget(this->last_release[i]);
                }
            }

            if(this->flag_release[1] == true){
                this->setUIControl_subTitle_withSideBtn("Release ", "View All", BTN_IDX_SUBTITLE_albums);
                this->box_main_contents->addSpacing(10);

                this->hBox_album = this->setUIControl_hBoxLayout_forAlbum();
                this->box_main_contents->addSpacing(30);

                int max_cnt = this->list_album->count();
                if(max_cnt > 15){
                    max_cnt = 15;
                }

                for(int i = 0; i < max_cnt; i++){
                    this->release[i]->setData(this->list_album->at(i));
                    this->hBox_album->addWidget(this->release[i]);
                }
            }

            if(this->flag_appears[1] == true){
                this->setUIControl_subTitle_withSideBtn(QString("Appears on (%1)").arg(this->list_appears->at(0).appearson_count), "View All", BTN_IDX_SUBTITLE_appearson);
                this->box_main_contents->addSpacing(10);

                this->box_main_contents->addLayout(this->vBox_appearson);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_suggestArtists[1] == true){
                this->setUIControl_subTitle_withSideBtn("Artist Suggestions ", "View All", BTN_IDX_SUBTITLE_artists);
                this->box_main_contents->addSpacing(10);

                this->hBox_relatedArtist = this->setUIControl_hBoxLayout_forArtist();
                this->box_main_contents->addSpacing(30);

                int max_cnt = this->list_artist->count();
                if(max_cnt > 15){
                    max_cnt = 15;
                }

                for(int i = 0; i < max_cnt; i++){
                    this->related_artist[i]->setData(this->list_artist->at(i));
                    this->hBox_relatedArtist->addWidget(this->related_artist[i]);
                }
            }

            ContentLoadingwaitingMsgHide();
        }
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
    void QobuzArtistDetail::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_tracks){
            PageInfo_TrackAllView data_page;
            data_page.pathTitle = QString("%1 - Top tracks").arg(this->data_artist.name);
            data_page.api_subPath = "artist/get";
            data_page.artist_id = this->data_artist.id;
            data_page.page = "playlists";

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_trackAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_APPEARSON_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_lastrelease){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = QString("%1 - Last release").arg(this->data_artist.name);
            data_page.api_subPath = "artist/get";
            data_page.artist_id = this->data_artist.id;
            data_page.type = "albums_with_last_release";

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_LAST_RELEASE_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal

        }
        else if(btnId == BTN_IDX_SUBTITLE_albums){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = QString("%1 - Release").arg(this->data_artist.name);
            //data_page.api_subPath = this->api_subPath_album;
            data_page.api_subPath = "artist/get";
            data_page.artist_id = this->data_artist.id;
            data_page.page = "albums_without_last_release";

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ALBUM_ALL_LIST_VIEW);            
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal

        }
        else if(btnId == BTN_IDX_SUBTITLE_appearson){
            PageInfo_TrackAllView data_page;
            data_page.pathTitle = QString("%1 - Appears on").arg(this->data_artist.name);
            data_page.api_subPath = "artist/get";
            data_page.artist_id = this->data_artist.id;
            data_page.page = "tracks_appears_on";

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_trackAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_APPEARSON_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_artists){
            PageInfo_ArtistAllView data_page;
            data_page.pathTitle = QString("%1 - Artist Suggestions").arg(this->data_artist.name);
            data_page.api_subPath = "artist/getSimilarArtists";
            data_page.artist_id = this->data_artist.id;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_artistAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ARTIST_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }

    }





    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief QobuzArtistDetail::slot_applyResult_artistInfo
     * @param data_artist
     */
    void QobuzArtistDetail::slot_applyResult_artistInfo(qobuz::ArtistItemData data_artist){

        // Update Data
        this->data_artist = data_artist;

        QJsonObject artistObj;
        artistObj.insert("name", this->data_artist.name);
        artistObj.insert("image", this->data_artist.image_large);
        artistObj.insert("biography", this->data_artist.biography);
        artistObj.insert("id", this->data_artist.id);

        // Update UI
        this->artist_detail_info->setData_fromQobuzData(artistObj);
        this->artist_detail_info->setFavorite(false, 0);

        this->flag_send_artist = false;
        ProcCommon *proc_fav_artist = new ProcCommon(this);
        connect(proc_fav_artist, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzArtistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
        proc_fav_artist->request_qobuz_get_favorite("artist", QString("%1").arg(this->data_artist.id));
    }


    void QobuzArtistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){

        // Favorite 정보를 전달해줌. 알아서 처리하라고. => OptMorePopup 에서 하라고, 가려줌
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
            bool status  = ProcJsonEasy::getBool(p_jsonObj, "status");

            // Qobuz favorite toggle check
            if(this->flag_send_artist == true){
                if((status == true && this->flag_artist_fav == false) || (status == false && this->flag_artist_fav == true)){
                    // Qobuz Favorite Artist toggle
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzArtistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("artist", QString("%1").arg(this->data_artist.id), this->flag_track_fav);
                }
                this->flag_send_track = false;
            }
            else if(this->flag_send_track == true){
                if((status == true && this->flag_track_fav == false) || (status == false && this->flag_track_fav == true)){
                    // Qobuz Favorite Track toggle
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzArtistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                }
                this->flag_send_track = false;
            }
            else{
                if(status == false && this->flag_send_artist == false){
                    this->artist_detail_info->setFavorite(false, 0);
                }
                else if(status == true && this->flag_send_artist == false){
                    this->artist_detail_info->setFavorite(true, 1);
                }
            }
        }
    }


    void QobuzArtistDetail::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){
        int id = ProcJsonEasy::getInt(jsonObj, "id");

        QJsonObject jsonObj_track = QJsonObject();
        if(this->flag_topTracks == true){
            jsonObj_track = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();
        }
        else if(this->flag_appearsOn == true){
            jsonObj_track = this->jsonArr_appears_toPlay.at(this->track_idx_fav).toObject();
        }

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
            track.insert("type", "QOBUZ");

            QJsonArray tracks;
            tracks.append(track);

            QJsonObject json;
            json.insert("tracks", tracks);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzArtistDetail::slot_applyResult_addRating_track);
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
            track.insert("type", "QOBUZ");

            QJsonObject ratingInfo;
            ratingInfo.insert("favorite", this->flag_track_fav);
            ratingInfo.insert("star", this->track_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "QOBUZ");

            QJsonObject json;
            json.insert("ratingInfo", ratingInfo);
            json.insert("track", track);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzArtistDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

        }
    }


    void QobuzArtistDetail::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }

            if(this->flag_topTracks == true){
                this->flag_topTracks = false;
            }
            else if(this->flag_appearsOn == true){
                this->flag_appearsOn = false;
            }
        }
        else{
            this->flag_check_track = false;
            this->flag_topTracks = false;
            this->flag_appearsOn = false;
        }
    }


    void QobuzArtistDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){                
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    if(this->flag_topTracks == true){
                        this->top_tracks[i]->setFavoritesIds(flag, star);
                        this->flag_topTracks = false;                        
                    }
                    else if(this->flag_appearsOn == true){
                        this->appears_on[i]->setFavoritesIds(flag, star);
                        this->flag_appearsOn = false;
                    }
                }

                if(this->flag_reload_page == true){
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
                        connect(proc_fav_appear, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzArtistDetail::slot_applyResult_getRating_track);
                        proc_fav_appear->request_rose_getRating_Track("QOBUZ", tmp_clientkey);
                    }
                }
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
            this->flag_topTracks = false;
            this->flag_appearsOn = false;
        }
    }


    /**
     * @brief QobuzAlbumDetail::slot_applyResult_tracks
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void QobuzArtistDetail::slot_applyResult_tracks(const QList<qobuz::TrackItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.count() > 0){

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
            }
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            this->list_track->append(list_data);

            int max_cnt = this->list_track->count();
            if(max_cnt > 5){
                max_cnt = 5;
            }

            QList<QString> tmp_clientkey;
            for(int i = 0; i < max_cnt; i++){
                this->top_tracks[i]->setDataTrackInfo_Qobuz(this->list_track->at(i));
                this->top_tracks[i]->resize(1550, 70);

                this->vBox_topTracks->addWidget(this->top_tracks[i]);

                tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));
            }

            // request HTTP API - get favorite for Rose Server
            this->flag_topTracks = true;
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzArtistDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_getRating_Track("QOBUZ", tmp_clientkey);

            this->flag_tracks[1] = true;
        }

        this->flag_tracks[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief QobuzAlbumDetail::slot_applyResult_tracks
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void QobuzArtistDetail::slot_applyResult_appearson(const QList<qobuz::TrackItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.count() > 0){

            if(this->list_track->count() == 0){
                this->jsonArr_appears_toPlay = QJsonArray();
            }
            ProcJsonEasy::mergeJsonArray(this->jsonArr_appears_toPlay, jsonArr_dataToPlay);

            this->list_appears->append(list_data);

            int max_cnt = this->list_appears->count();
            if(max_cnt > 5){
                max_cnt = 5;
            }

            QList<QString> tmp_clientkey;
            for(int i = 0; i < max_cnt; i++){
                this->appears_on[i]->setDataTrackInfo_Qobuz(this->list_appears->at(i));
                this->appears_on[i]->resize(1550, 70);

                this->vBox_appearson->addWidget(this->appears_on[i]);

                tmp_clientkey.append(QString("%1").arg(this->list_appears->at(i).id));
            }

            // request HTTP API - get favorite for Rose Server
            this->flag_appearsOn = true;
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzArtistDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_getRating_Track("QOBUZ", tmp_clientkey);

            this->flag_appears[1] = true;
        }

        this->flag_appears[0] = true;
        this->setUIControl_appendWidget();
    }


    void QobuzArtistDetail::slot_applyResult_lastrelease(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray&jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.count() > 0){
            this->list_release->append(list_data);

            this->flag_lastRelease[1] = true;
        }

        this->flag_lastRelease[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzArtistDetail::slot_applyResult_albums(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.count() > 0){
            this->list_album->append(list_data);

            this->flag_release[1] = true;
        }

        this->flag_release[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief QobuzArtistDetail::slot_applyResult_artists
     * @param list_data
     */
    void QobuzArtistDetail::slot_applyResult_artists(const QList<qobuz::ArtistItemData> &list_data, const bool flag_lastPath){

        Q_UNUSED(flag_lastPath);

        if(list_data.count() > 0){
            this->list_artist->append(list_data);

            this->flag_suggestArtists[1] = true;
        }

        this->flag_suggestArtists[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [Slot] ArtistImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void QobuzArtistDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

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
                    // Qobuz Favorite toggle
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzArtistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("artist", QString("%1").arg(data_artist.id), this->flag_artist_fav);
                    this->flag_send_artist = true;
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
            setUIShare();
        }
        else{
            this->proc_clicked_imageDetailInfo_fromArtist(this->data_artist, this->jsonArr_tracks_toPlay, SECTION_FOR_MORE_POPUP___artists_main, clickMode);
        }
    }


    void QobuzArtistDetail::slot_artistInfo_btnClicked(){

        QJsonObject tmpData;
        tmpData.insert("artist", this->data_artist.name);
        tmpData.insert("biography", this->data_artist.biography);

        this->dlgArtistMore = new DialogNotice(this, DialogNotice::DlgNotice::Qobuz_artist_more);
        this->dlgArtistMore->setData_formJsonObj(tmpData);
        this->dlgArtistMore->exec();
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void QobuzArtistDetail::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        QString obj_name = sender()->objectName();

        if(obj_name == "top_tracks"){
            if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

                if(this->flag_check_track == false){
                    this->track_star_fav = this->top_tracks[idx]->getFavoritesStars();
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

                        ProcCommon *proc = new ProcCommon(this);
                        connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzArtistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                        proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                        this->flag_send_track = true;
                    }

                    this->track_idx_fav = idx;
                    QJsonObject jsonObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

                    // request HTTP API - get favorite for Rose Server
                    roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                    connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzArtistDetail::slot_applyResult_checkRating_track);
                    proc_favCheck_track->request_rose_checkRating_Track("QOBUZ", QString("%1").arg(ProcJsonEasy::getInt(jsonObj, "id")));
                    this->flag_check_track = true;
                    this->flag_topTracks = true;

                    this->top_tracks[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
                }
            }
            else{
                // ClickMode 별로 처리
                this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, SECTION_FOR_MORE_POPUP___tracks);
            }
        }
        else if(obj_name == "appears_on"){
            if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

                if(this->flag_check_track == false){
                    this->track_star_fav = this->appears_on[idx]->getFavoritesStars();
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

                        ProcCommon *proc = new ProcCommon(this);
                        connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzArtistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                        proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                        this->flag_send_track = true;
                    }

                    this->track_idx_fav = idx;
                    QJsonObject jsonObj = this->jsonArr_appears_toPlay.at(idx).toObject();

                    // request HTTP API - get favorite for Rose Server
                    roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                    connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzArtistDetail::slot_applyResult_checkRating_track);
                    proc_favCheck_track->request_rose_checkRating_Track("QOBUZ", QString("%1").arg(ProcJsonEasy::getInt(jsonObj, "id")));
                    this->flag_check_track = true;
                    this->flag_appearsOn = true;

                    this->appears_on[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
                }
            }
            else{
                // ClickMode 별로 처리
                this->proc_clicked_itemTrack_inList(this->list_appears, this->jsonArr_appears_toPlay, clickMode, idx, SECTION_FOR_MORE_POPUP___appears_on);
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
    void QobuzArtistDetail::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzArtistDetail::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___artists_main){
            // main
            this->proc_clicked_itemArtist(this->data_artist, clickMode, index, section);
        }
        else{
            // related artist
            this->proc_clicked_itemArtist(this->list_artist, clickMode, index, section);
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
    void QobuzArtistDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___artists_main){
            this->proc_clicked_optMorePopup_fromArtist(this->data_artist, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___last_release){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_release, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___appears_on){
            this->proc_clicked_optMorePopup_fromTrack(this->list_appears, this->jsonArr_appears_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___artists_related){
            this->proc_clicked_optMorePopup_fromArtist(this->list_artist, index, clickMode);
        }
    }

}
