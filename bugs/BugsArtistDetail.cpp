#include "bugs/BugsArtistDetail.h"

#include "bugs/ProcBugsAPI.h"
#include "bugs/ConvertData_forBugs.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include <QScroller>
#include <QPushButton>


namespace bugs {

    const int BTN_IDX_SUBTITLE_tracks = 1;
    const int BTN_IDX_SUBTITLE_artist_albums = 2;
    const int BTN_IDX_SUBTITLE_compilation_albums = 3;


    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___artist_main = 0;
    const int SECTION_FOR_MORE_POPUP___tracks = 1;
    const int SECTION_FOR_MORE_POPUP___artist_albums = 2;
    const int SECTION_FOR_MORE_POPUP___compilation_albums = 3;


    /**
     * @brief
     * @param parent
     */
    BugsArtistDetail::BugsArtistDetail(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_filter, parent) {

        this->linker =  Linker::getInstance();

        // data
        this->list_track = new QList<bugs::TrackItemData>();
        this->list_aritst_album = new QList<bugs::AlbumItemData>();
        this->list_compilation_album = new QList<bugs::AlbumItemData>();
    }


    /**
     * @brief 소멸자.
     */
    BugsArtistDetail::~BugsArtistDetail(){

        this->deleteLater();
    }


    /**
     * @brief TidalArtistDetail::setJsonObject_forData
     * @param jsonObj
     */
    void BugsArtistDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        bugs::ArtistItemData tmp_data_artist = ConvertData_forBugs::convertData_artistData(jsonObj);

        qDebug() << tmp_data_artist.artist_id << this->data_artist.artist_id;

        this->flagNeedReload = false;
        if(tmp_data_artist.artist_id != this->data_artist.artist_id){

            this->flagNeedReload = true;

            if(this->flag_tracks[1] == true){
                this->widget_main_contents->hide();
            }

            // clear Data
            this->list_track->clear();
            this->list_aritst_album->clear();
            this->list_compilation_album->clear();

            this->flag_tracks[0] = false;
            this->flag_artistAlbums[0] = false;
            this->flag_compilationAlbums[0] = false;

            this->flag_tracks[1] = false;
            this->flag_artistAlbums[1] = false;
            this->flag_compilationAlbums[1] = false;

            this->flag_check_track = false;
            this->flag_track_fav = false;
            this->flag_send_track = false;

            this->track_id_fav = 0;
            this->track_idx_fav = 0;
            this->track_star_fav = 0;

            this->data_artist = tmp_data_artist;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            if(this->data_artist.biography.isEmpty()){
                ProcBugsAPI *proc = new ProcBugsAPI(this);
                // Artist 기본 정보를 가져와야 하는 경우임. (id만 전달받은 상태)
                connect(proc, &ProcBugsAPI::completeReq_artist, this, &BugsArtistDetail::slot_applyResult_artistInfo);
                proc->request_bugs_get_artist(this->data_artist.artist_id);
            }

            // get top track of the artist
            ProcBugsAPI *proc_track = new ProcBugsAPI(this);
            connect(proc_track, &ProcBugsAPI::completeReq_list_tracks, this, &BugsArtistDetail::slot_applyResult_tracks);
            proc_track->request_bugs_getList_tracks_of_artist(this->data_artist.artist_id, 5, 1
                                                        , ProcBugsAPI::BugsArtist_TrackFilter::All
                                                        , ProcBugsAPI::BugsAritst_TrackSort::Popular);

            // request - album release
            ProcBugsAPI *proc_album = new ProcBugsAPI(this);
            proc_album->setProperty("section", SECTION_FOR_MORE_POPUP___artist_albums);
            connect(proc_album, &ProcBugsAPI::completeReq_list_albums, this, &BugsArtistDetail::slot_applyResult_albums);
            proc_album->request_bugs_getList_albums_of_artist(this->data_artist.artist_id, 15, 1, ProcBugsAPI::BugsArtist_Albumfilter::Release);

            // request - album join & compil - 같은 함수 호출로 구분이 필요해서.
            ProcBugsAPI *proc_comAlbum = new ProcBugsAPI(this);
            proc_comAlbum->setProperty("section", SECTION_FOR_MORE_POPUP___compilation_albums);
            connect(proc_comAlbum, &ProcBugsAPI::completeReq_list_albums, this, &BugsArtistDetail::slot_applyResult_albums);
            proc_comAlbum->request_bugs_getList_albums_of_artist(this->data_artist.artist_id, 15, 1, ProcBugsAPI::BugsArtist_Albumfilter::JoinCompil);
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief BugsArtistDetail::setActivePage
     */
    void BugsArtistDetail::setActivePage(){

        if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;
            }

            // 필요에 따라 리로드 한다.
            if(this->flagNeedReload){

                // 항상 부모클래스의 함수 먼저 호출
                AbstractBugsSubWidget::setActivePage();

                this->box_contents->removeWidget(this->widget_main_contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->box_main_contents = new QVBoxLayout();
                this->box_main_contents->setSpacing(0);
                this->box_main_contents->setContentsMargins(0, 0, 0, 0);

                this->widget_main_contents = new QWidget();
                this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_main_contents->setLayout(this->box_main_contents);

                this->box_contents->addWidget(widget_main_contents);
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                this->setUIControl_basic();
            }
        }
        else{
            if(this->m_flagCurrLogined){
                // 로그인 ---> 로그아웃
                this->m_flagCurrLogined = false;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->setUIControl_notLogin();
            }
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
    void BugsArtistDetail::setUIControl_basic(){

        this->artist_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Bugs_artist);
        connect(this->artist_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &BugsArtistDetail::slot_detailInfo_btnClicked);
        connect(this->artist_detail_info, &AbstractImageDetailContents_RHV::signal_clicked_artistMore, this, &BugsArtistDetail::slot_artistInfo_btnClicked);

        this->artist_detail_info->initView();

        this->box_main_contents->addWidget(this->artist_detail_info);
        this->box_main_contents->addSpacing(75);

        for(int i = 0; i < 10; i++){
            this->lb_subTitle[i] = new QLabel();
            this->btnView_all[i] = new QPushButton();
        }

        this->vBox_artistTracks = new QVBoxLayout();
        this->vBox_artistAlbum = new QVBoxLayout();
        this->vBox_compilationAlbum = new QVBoxLayout();

        this->hBox_artistAlbum = new QHBoxLayout();
        this->hBox_compilationAlbum = new QHBoxLayout();

        // clear UI
        GSCommon::clearLayout(this->vBox_artistTracks);
        GSCommon::clearLayout(this->vBox_artistAlbum);
        GSCommon::clearLayout(this->vBox_compilationAlbum);

        GSCommon::clearLayout(this->hBox_artistAlbum);
        GSCommon::clearLayout(this->hBox_compilationAlbum);

        for (int i = 0; i < 5; i++) {
            this->artist_tracks[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->artist_tracks[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &BugsArtistDetail::slot_clickedItemTrack_inList);
            this->artist_tracks[i]->setProperty("index", i);
            this->artist_tracks[i]->setObjectName("top_tracks");
        }

        for(int i = 0; i < 15; i++){
            this->artistAlbum[i] = new bugs::ItemAlbum_bugs(i, SECTION_FOR_MORE_POPUP___artist_albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->artistAlbum[i], &bugs::ItemAlbum_bugs::signal_clicked, this, &BugsArtistDetail::slot_clickedItemAlbum);
        }


        for(int i = 0; i < 15; i++){
            this->compliationAlbum[i] = new bugs::ItemAlbum_bugs(i, SECTION_FOR_MORE_POPUP___compilation_albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->compliationAlbum[i], &bugs::ItemAlbum_bugs::signal_clicked, this, &BugsArtistDetail::slot_clickedItemAlbum);
        }

        if(!this->data_artist.artist_nm.isEmpty()){
            this->slot_applyResult_artistInfo(this->data_artist);
        }
    }


    void BugsArtistDetail::setUIControl_appendWidget(){

        if(this->flag_tracks[0] == true && this->flag_artistAlbums[0] == true && this->flag_compilationAlbums[0] == true){

            if(this->flag_tracks[1] == true){
                this->widget_artistTracks = new QWidget();
                this->widget_artistTracks = this->setUIControl_subTitle_withSideBtn(QString("곡 (%1)").arg(this->list_track->at(0).total_count), "View All", BTN_IDX_SUBTITLE_tracks, this->vBox_artistTracks);

                this->vBox_artistTracks->addSpacing(10);

                int max_cnt = this->list_track->count();
                if(max_cnt > 5){
                    max_cnt = 5;
                }

                QList<QString> tmp_clientkey;
                for(int i = 0; i < max_cnt; i++){
                    this->artist_tracks[i]->setDataTrackInfo_Bugs(this->list_track->at(i), "Artist");

                    this->vBox_artistTracks->addWidget(this->artist_tracks[i]);
                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).track_id));
                }

                this->box_main_contents->addLayout(this->vBox_artistTracks);
                this->box_main_contents->addSpacing(30);

                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsArtistDetail::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("BUGS", tmp_clientkey);
            }

            if(this->flag_artistAlbums[1] == true){
                this->widget_artistAlbum = new QWidget();
                this->widget_artistAlbum = this->setUIControl_subTitle_withSideBtn(QString("앨범 (%1)").arg(this->list_aritst_album->at(0).total_count), "View All", BTN_IDX_SUBTITLE_artist_albums, this->vBox_artistAlbum);

                this->vBox_artistAlbum->addSpacing(10);

                this->hBox_artistAlbum = this->setUIControl_hBoxLayout_forAlbum(this->vBox_artistAlbum);

                int max_cnt = this->list_aritst_album->count();
                if(max_cnt > 15){
                    max_cnt = 15;
                }

                for(int i = 0; i < max_cnt; i++){
                    this->artistAlbum[i]->setData(this->list_aritst_album->at(i));
                    this->hBox_artistAlbum->addWidget(this->artistAlbum[i]);
                }

                this->box_main_contents->addLayout(this->vBox_artistAlbum);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_compilationAlbums[1] == true){
                this->widget_compilationAlbum = new QWidget();
                this->widget_compilationAlbum = this->setUIControl_subTitle_withSideBtn(QString("참여/컨피레이션 앨범 (%1)").arg(this->list_compilation_album->at(0).total_count), "View All", BTN_IDX_SUBTITLE_compilation_albums, this->vBox_compilationAlbum);

                this->vBox_compilationAlbum->addSpacing(10);

                this->hBox_compilationAlbum = this->setUIControl_hBoxLayout_forAlbum(this->vBox_compilationAlbum);

                int max_cnt = this->list_compilation_album->count();
                if(max_cnt > 15){
                    max_cnt = 15;
                }

                for(int i = 0; i < max_cnt; i++){
                    this->compliationAlbum[i]->setData(this->list_compilation_album->at(i));
                    this->hBox_compilationAlbum->addWidget(this->compliationAlbum[i]);
                }

                this->box_main_contents->addLayout(this->vBox_compilationAlbum);
                this->box_main_contents->addSpacing(30);
            }

            ContentLoadingwaitingMsgHide();
        }
    }


    QWidget* BugsArtistDetail::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QHBoxLayout* BugsArtistDetail::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

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


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Sub-title 의 우측에 있는 버튼 클릭에 대한 Signal 처리
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void BugsArtistDetail::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_tracks){
            if(this->list_track->size() > 0){
                // Track All View (of Artist) 상세 페이지 이동
                PageInfo_TrackAllView_ofArtist data_page;
                data_page.artist_id = this->data_artist.artist_id;
                data_page.filterOpt = QVariant::fromValue(ProcBugsAPI::BugsArtist_TrackFilter::All).toString();
                data_page.sortOpt = QVariant::fromValue(ProcBugsAPI::BugsAritst_TrackSort::Popular).toString();
                data_page.mainTitle = QString("%1 - 곡").arg(this->data_artist.artist_nm);

                if(global.user_forBugs.flag_rosehome == true){
                    global.user_forBugs.rosehome_obj = QJsonObject();
                    global.user_forBugs.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_BUGS___TRACK_ALL_VIEW___OF_ARTIST);
                    QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_pageInfo_trackAllView_ofArtist(data_page);
                    global.user_forBugs.rosehome_obj.insert(KEY_DATA, jsonObj_move);

                    emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Bugs));
                }
                else{
                    QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_pageInfo_trackAllView_ofArtist(data_page);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___TRACK_ALL_VIEW___OF_ARTIST);

                    emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                }
            }
        }
        else if(btnId == BTN_IDX_SUBTITLE_artist_albums){
            if(this->list_aritst_album->size() > 0){
                // Album All View (of Artist) 상세 페이지 이동
                PageInfo_AlbumAllView_ofArtist data_page;
                data_page.artist_id = this->data_artist.artist_id;
                data_page.filterOpt = QVariant::fromValue(ProcBugsAPI::BugsArtist_Albumfilter::Release).toString();
                data_page.mainTitle = QString("%1 - 앨범").arg(this->data_artist.artist_nm);

                if(global.user_forBugs.flag_rosehome == true){
                    global.user_forBugs.rosehome_obj = QJsonObject();
                    global.user_forBugs.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ALBUM_ALL_VIEW___OF_ARTIST);
                    QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_pageInfo_albumAllView_ofArtist(data_page);
                    global.user_forBugs.rosehome_obj.insert(KEY_DATA, jsonObj_move);

                    emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Bugs));
                }
                else{
                    QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_pageInfo_albumAllView_ofArtist(data_page);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ALBUM_ALL_VIEW___OF_ARTIST);
                    emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                }
            }
        }
        else if(btnId == BTN_IDX_SUBTITLE_compilation_albums){
            if(this->list_compilation_album->size() > 0){
                // Album All View (of Artist) 상세 페이지 이동
                PageInfo_AlbumAllView_ofArtist data_page;
                data_page.artist_id = this->data_artist.artist_id;
                data_page.filterOpt = QVariant::fromValue(ProcBugsAPI::BugsArtist_Albumfilter::JoinCompil).toString();
                data_page.mainTitle = QString("%1 - 참여/컨피레이션 앨범").arg(this->data_artist.artist_nm);

                if(global.user_forBugs.flag_rosehome == true){
                    global.user_forBugs.rosehome_obj = QJsonObject();
                    global.user_forBugs.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ALBUM_ALL_VIEW___OF_ARTIST);
                    QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_pageInfo_albumAllView_ofArtist(data_page);
                    global.user_forBugs.rosehome_obj.insert(KEY_DATA, jsonObj_move);

                    emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Bugs));
                }
                else{
                    QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_pageInfo_albumAllView_ofArtist(data_page);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ALBUM_ALL_VIEW___OF_ARTIST);
                    emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                }
            }
        }

    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief BugsArtistDetail::slot_applyResult_artistInfo
     * @param data_artist
     */
    void BugsArtistDetail::slot_applyResult_artistInfo(bugs::ArtistItemData data_artist){

        // Update Data
        this->data_artist = data_artist;

        QJsonObject artistObj;
        artistObj.insert("name", this->data_artist.artist_nm);
        artistObj.insert("image", this->data_artist.image);
        artistObj.insert("biography", this->data_artist.biography);
        artistObj.insert("id", this->data_artist.artist_id);

        // Update UI
        this->artist_detail_info->setData_fromBugsData(artistObj);

        // request HTTP API - get favorite for Rose Server
        roseHome::ProcCommon *proc_favCheck_artist = new roseHome::ProcCommon(this);
        connect(proc_favCheck_artist, &roseHome::ProcCommon::completeCheck_rating_artist, this, &BugsArtistDetail::slot_applyResult_checkRating_artist);
        proc_favCheck_artist->request_rose_checkRating_Artist("BUGS", QString("%1").arg(this->data_artist.artist_id));
    }


    /**
     * @brief TidalAlbumDetail::slot_applyResult_tracks
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void BugsArtistDetail::slot_applyResult_tracks(const QList<bugs::TrackItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.count() > 0){

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
            }
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            this->list_track->append(list_data);
            this->flag_tracks[1] = true;
        }

        this->flag_tracks[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief BugsArtistDetail::slot_applyResult_albums
     * @param list_data
     */
    void BugsArtistDetail::slot_applyResult_albums(const QList<bugs::AlbumItemData> &list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);
        int section = sender()->property("section").toInt();

        if(section == SECTION_FOR_MORE_POPUP___artist_albums){

            if(list_data.count() > 0){
                this->list_aritst_album->append(list_data);

                this->flag_artistAlbums[1] = true;
            }

            this->flag_artistAlbums[0] = true;
        }
        else if(section == SECTION_FOR_MORE_POPUP___compilation_albums){

            if(list_data.count() > 0){
                this->list_compilation_album->append(list_data);

                this->flag_compilationAlbums[1] = true;
            }

            this->flag_compilationAlbums[0] = true;
        }

        this->setUIControl_appendWidget();
    }



    /**
     * @brief [Slot] ArtistImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void BugsArtistDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

        if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx2 ||
                clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx3 || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){

            if(this->flag_check_artist == false){

                this->artist_star_fav = this->artist_detail_info->getFavoritesStars();
                this->flag_artist_fav = false;

                if(this->artist_star_fav == 3){
                    this->artist_star_fav = 0;
                    this->flag_artist_fav = false;
                }
                else if(this->artist_star_fav >= 0 && this->artist_star_fav < 3){
                    this->artist_star_fav++;
                    this->flag_artist_fav = true;
                }

                if(this->artist_star_fav == 0 || this->artist_star_fav == 1){
                    // for sync data with UI (after response)
                    ItemPositionData itemPosData;
                    itemPosData.section = SECTION_FOR_MORE_POPUP___artist_main;
                    itemPosData.index = 0;
                    itemPosData.data_id = QString("%1").arg(data_artist.artist_id);
                    itemPosData.likes_yn = this->flag_artist_fav;

                    // Case 2 : Data, View를 먼저 업데이트 (사용자 이벤에 의한), 그 후 Http 요청
                    this->proc_preUpdate_favorite_artist(itemPosData);
                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){
                        proc->request_bugs_deleteFavorite_artist(data_artist.artist_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }
                    else{
                        proc->request_bugs_addFavorite_artist(data_artist.artist_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }
                }

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_artist = new roseHome::ProcCommon(this);
                connect(proc_favCheck_artist, &roseHome::ProcCommon::completeCheck_rating_artist, this, &BugsArtistDetail::slot_applyResult_checkRating_artist);
                proc_favCheck_artist->request_rose_checkRating_Artist("BUGS", QString("%1").arg(this->data_artist.artist_id));
                this->flag_check_artist = true;

                this->artist_detail_info->setFavorite(this->flag_artist_fav, this->artist_star_fav);
            }
        }
        else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Share){

            this->shareLink = this->artist_detail_info->detailInfo_shareLink;    //220907 share code ... --

            //220816 share code ...
            setUIShare();//c220816
        }
        else{
            this->proc_clicked_imageDetailInfo_fromArtist(this->data_artist, this->jsonArr_tracks_toPlay, SECTION_FOR_MORE_POPUP___artist_main, clickMode);
        }
    }


    void BugsArtistDetail::slot_artistInfo_btnClicked(){

        QJsonObject tmpData;
        tmpData.insert("artist", this->data_artist.artist_nm);
        tmpData.insert("biography", this->data_artist.biography);

        this->dlgArtistMore = new DialogNotice(this, DialogNotice::DlgNotice::Bugs_artist_more);
        this->dlgArtistMore->setData_formJsonObj(tmpData);
        this->dlgArtistMore->exec();
    }



    void BugsArtistDetail::slot_applyResult_checkRating_artist(const QJsonObject &jsonObj){

        int artistID = ProcJsonEasy::getInt(jsonObj, "id");

        if(artistID > 0){

            if(this->flag_check_artist == true){

                QJsonObject artist;
                artist.insert("id", artistID);

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_artist_fav);
                ratingInfo.insert("star", this->artist_star_fav);
                ratingInfo.insert("type", "BUGS");

                QJsonObject json;
                json.insert("artist", artist);
                json.insert("ratingInfo", ratingInfo);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
                connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_artist, this, &BugsArtistDetail::slot_applyResult_getRating_artist);
                proc_fav_album->request_rose_setRating_Artist(json, this->flag_artist_fav, this->artist_star_fav);
            }
            else{

                QJsonObject artist_id;
                artist_id.insert("id", artistID);

                QJsonObject json;
                json.insert("artist", artist_id);

                roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
                connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_artist, this, &BugsArtistDetail::slot_applyResult_getRating_artist);
                proc_fav_album->request_rose_getRating_Artist("BUGS", json);
            }
        }
        else{

            QJsonArray thumbnail;
            thumbnail.append(this->data_artist.image);

            QJsonObject artist;
            artist.insert("name", this->data_artist.artist_nm);
            artist.insert("type", "BUGS");
            artist.insert("comment", "");
            artist.insert("clientKey", QString("%1").arg(this->data_artist.artist_id));
            artist.insert("thumbnail", thumbnail);

            QJsonObject json;
            json.insert("artist", artist);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
            connect(proc_fav_album, &roseHome::ProcCommon::completeCheck_rating_artist, this, &BugsArtistDetail::slot_applyResult_addRating_artist);
            proc_fav_album->request_rose_addRating_Artist(json);
        }
    }


    void BugsArtistDetail::slot_applyResult_getRating_artist(const QJsonArray &contents){

        if(contents.size() > 0){

            QJsonObject tmp_star = contents.at(0).toObject();

            if(tmp_star.contains("star")){
                int star = ProcJsonEasy::getInt(tmp_star, "star");
                bool flag = star > 0 ? true : false;

                this->artist_detail_info->setFavorite(flag, star);
            }
            else{
                this->artist_detail_info->setFavorite(this->flag_artist_fav, this->artist_star_fav);
            }

            if(this->flag_check_artist == true){
                this->flag_check_artist = false;
            }
        }
    }


    void BugsArtistDetail::slot_applyResult_addRating_artist(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){

            // request HTTP API - favorite for Rose Server
            roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
            connect(proc_fav, &roseHome::ProcCommon::completeCheck_rating_artist, this, &BugsArtistDetail::slot_applyResult_checkRating_artist);
            proc_fav->request_rose_checkRating_Artist("BUGS", QString("%1").arg(this->data_artist.artist_id));

            if(this->flag_check_artist == true){
                this->flag_check_artist = false;
            }
        }
        else{
            this->flag_check_artist = false;
        }
    }

    void BugsArtistDetail::slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌.
        int idx = ProcJsonEasy::getInt(p_jsonObj, "index");
        int track_id = ProcJsonEasy::getString(p_jsonObj, "data_id").toInt();

        if(track_id == this->list_track->at(idx).track_id){
            qDebug() << "likes_yn : " << ProcJsonEasy::getBool(p_jsonObj, "likes_yn");
        }

        this->flag_send_track = false;
    }

    void BugsArtistDetail::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("id")){

            int id = ProcJsonEasy::getInt(jsonObj, "id");

            if(id > 0){
                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_track_fav);
                ratingInfo.insert("star", this->track_star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", "BUGS");

                QJsonObject track;
                track.insert("duration", 0);
                track.insert("favorite", false);
                track.insert("id", id);
                track.insert("ownerId", 0);
                track.insert("sort", 0);
                track.insert("star", 0);
                track.insert("type", "BUGS");

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsArtistDetail::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
            }
            else{

                QJsonObject data = this->jsonArr_tracks_toPlay[this->track_idx_fav].toObject();


                QJsonObject track;
                track.insert("clientKey", ProcJsonEasy::getInt(data, "track_id"));
                track.insert("duration", 0);
                track.insert("favorite", true);
                track.insert("ownerId", 0);
                track.insert("sort", 0);
                track.insert("star", 1);
                track.insert("thumbnailUrl", this->list_track->at(this->track_idx_fav).album_image);
                track.insert("title", ProcJsonEasy::getString(data, "track_title"));
                track.insert("type", "BUGS");
                track.insert("data", data);

                QJsonArray tracks;
                tracks.append(track);

                QJsonObject json;
                json.insert("tracks", tracks);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsArtistDetail::slot_applyResult_addRating_track);
                proc_fav_track->request_rose_addRating_Track(json);

            }
        }
        else{

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }

    void BugsArtistDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->artist_tracks[i]->setFavoritesIds(flag, star);
                }

            }
            else if(track_info.contains("message") && (track_info["message"].toString() == "정상")){
                this->artist_tracks[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void BugsArtistDetail::slot_applyResult_addRating_track(const QJsonObject &jsonObj){


        if(jsonObj.contains("flagOk")){

            if(jsonObj.contains("message") && (jsonObj["message"].toString() == "정상")){
                this->artist_tracks[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);

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
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void BugsArtistDetail::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___tracks;

        // ClickMode 별로 처리
        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){
            if(this->flag_check_track == false){
                this->track_star_fav = this->artist_tracks[idx]->getFavoritesStars();
                this->flag_track_fav = false;

                if(this->track_star_fav == 3){
                    this->track_star_fav = 0;

                    this->track_idx_fav = idx;
                    this->flag_track_fav = false;
                }
                else if(this->track_star_fav >= 0 && this->track_star_fav < 3){
                    this->track_star_fav++;

                    this->track_idx_fav = idx;
                    this->flag_track_fav = true;
                }

                if(this->track_star_fav == 0 || this->track_star_fav == 1){
                    // Bugs Favorite
                    ItemPositionData itemPosData;
                    itemPosData.section = section;
                    itemPosData.index = idx;
                    itemPosData.data_id = QString("%1").arg(this->list_track->at(idx).track_id);

                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    connect(proc, &bugs::ProcBugsAPI::completeReq_favarite_track, this, &BugsArtistDetail::slot_bugs_completeReq_listAll_myFavoritesIds);

                    if(this->track_star_fav == 0){
                        itemPosData.likes_yn = false;

                        proc->request_bugs_deleteFavorite_track(this->list_track->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }
                    else if(this->track_star_fav == 1){
                        itemPosData.likes_yn = true;

                        proc->request_bugs_addFavorite_track(this->list_track->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }

                    this->flag_send_track = true;
                }

                this->track_idx_fav = idx;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsArtistDetail::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("BUGS", QString("%1").arg(this->list_track->at(idx).track_id));

                this->flag_check_track = true;
            }
        }
        else{
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }


    /**
     * @brief BugsArtistDetail::slot_clickedItemAlbum
     */
    void BugsArtistDetail::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___artist_albums){
            this->proc_clicked_itemAlbum(this->list_aritst_album, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___compilation_albums){
            this->proc_clicked_itemAlbum(this->list_compilation_album, clickMode, index, section);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief BugsAlbumDetail::proc_preUpdate_favorite_artist
     * @param data_itemPos
     */
    /*void BugsArtistDetail::proc_preUpdate_favorite_artist(const ItemPositionData& data_itemPos){
        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___artist_main){
            // Update Data
            data_artist.likes_yn = data_itemPos.likes_yn;
        }
    }*/


    /**
     * @brief BugsArtistDetail::proc_preUpdate_favorite_track
     * @param data_itemPos
     */
    /*void BugsArtistDetail::proc_preUpdate_favorite_track(const ItemPositionData& data_itemPos){
        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___track){
            // Update Data
            bugs::TrackItemData data_track = this->list_track->at(data_itemPos.index);
            data_track.likes_yn = data_itemPos.likes_yn;
            this->list_track->replace(data_itemPos.index, data_track);

            // Update ListWidget
            this->listWidget_track->item(data_itemPos.index)->setData(Qt::UserRole, ConvertData_forBugs::getObjectJson_trackData(data_track));
        }
    }*/


    /**
     * @brief BugsMusicRecently::proc_preUpdate_favorite_album
     * @param data_itemPos
     */
    /*void BugsArtistDetail::proc_preUpdate_favorite_album(const ItemPositionData& data_itemPos){
        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___albums_release){
            // Update Data
            bugs::AlbumItemData data_album = this->list_album_release->at(data_itemPos.index);
            data_album.likes_yn = data_itemPos.likes_yn;
            this->list_album_release->replace(data_itemPos.index, data_album);
        }
        else if(data_itemPos.section == SECTION_FOR_MORE_POPUP___albums_joinCompil){
            // Update Data
            bugs::AlbumItemData data_album = this->list_album_joinCompil->at(data_itemPos.index);
            data_album.likes_yn = data_itemPos.likes_yn;
            this->list_album_joinCompil->replace(data_itemPos.index, data_album);
        }
    }*/




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
    void BugsArtistDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___artist_main){
            this->proc_clicked_optMorePopup_fromArtist(this->data_artist, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___artist_albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_aritst_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___compilation_albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_compilation_album, index, clickMode);
        }
    }
}
