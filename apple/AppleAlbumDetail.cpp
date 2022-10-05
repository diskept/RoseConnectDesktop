#include "apple/AppleAlbumDetail.h"

#include "apple/ConvertData_forApple.h"
#include "apple/ProcCommon_forApple.h"
#include "apple/ProcRosePlay_withApple.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"

#include <QDebug>
#include <QPainter>
#include <QScrollArea>
#include <QPixmapCache>


namespace apple {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___album = 1;
    const int SECTION_FOR_MORE_POPUP___track = 2;

    QString ALBTAB_STEP_TRACK = "track";
    QString ALBTAB_STEP_CREDIT = "credit";


    /**
     * @brief
     * @param parent
     */
    AppleAlbumDetail::AppleAlbumDetail(QWidget *parent) : AbstractAppleSubWidget(MainUIType::VerticalScroll_filter, parent) {

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));
    }


    /**
     * @brief 소멸자.
     */
    AppleAlbumDetail::~AppleAlbumDetail(){

        this->deleteLater();
    }


    /**
     * @brief RoseHomeRoseHomeAlbumDetail_Rose::setJsonObject_forData
     * @param jsonObj
     */
    void AppleAlbumDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        apple::AlbumItemData tmp_data_album = apple::ConvertData::convertData_albumData(jsonObj);
        this->flagNeedReload = false;

        if(global.user_forApple.isLogined() == false){

            this->login_wait_Object = QJsonObject();
            this->login_wait_Object = jsonObj;

            this->appleSettings = new apple::AppleSettings();
            this->appleSettings->setActivePage();

            this->box_login_contents = new QVBoxLayout();
            this->box_login_contents->setSpacing(0);
            this->box_login_contents->setContentsMargins(0, 0, 0, 0);
            this->box_login_contents->addWidget(this->appleSettings);

            this->widget_login_contents = new QWidget();
            this->widget_login_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_login_contents->setLayout(this->box_login_contents);

            this->box_contents->addWidget(this->widget_login_contents);

            //connect(this->qobuzSettings, &QobuzSettings::signal_logState_change, this, &QobuzAlbumDetail::slot_qobuz_logChange);
        }
        else{

            if(tmp_data_album.id != this->data_album.id){

                this->flagNeedReload = true;

                if(this->flag_track_ok == true){
                    this->widget_main_contents->hide();

                    GSCommon::clearLayout(this->box_main_contents);
                    this->box_contents->removeWidget(this->widget_main_contents);

                    this->flag_track_ok = false;
                }

                this->data_album = tmp_data_album;

                this->jsonObj_albumInfo = QJsonObject();
                this->jsonArr_tracks_toPlay = QJsonArray();

                this->track_favoriteOffset = 0;
                this->track_id_fav = 0;
                this->track_idx_fav = 0;

                this->album_star_fav = 0;
                this->track_star_fav = 0;

                this->flag_album_fav = false;
                this->flag_track_fav = false;
                this->flag_send_album = false;
                this->flag_check_album = false;
                this->flag_send_track = false;
                this->flag_check_track = false;

                this->flag_credit_ok = false;

                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

                apple::ProcCommon *proc_album = new apple::ProcCommon(this);
                connect(proc_album, &apple::ProcCommon::completeReq_album, this, &AppleAlbumDetail::slot_applyResult_album);
                connect(proc_album, &apple::ProcCommon::completeReq_tracks, this, &AppleAlbumDetail::slot_applyResult_album_tracks);
                proc_album->request_apple_get_album(this->data_album.id);
            }
            else{

            }
        }
    }


    /**
     * @brief RoseHomePlaylistDetail_Rose 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void AppleAlbumDetail::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractAppleSubWidget::setActivePage();

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);

            this->flag_credit_ok = false;

            QJsonArray *p_jsonArray_titlSub = new QJsonArray();
            QJsonObject sub1 { {"name", tr("Track")}, {"code", ALBTAB_STEP_TRACK} };
            //QJsonObject sub2 { {"name", tr("Credit")}, {"code", ALBTAB_STEP_CREDIT} };

            p_jsonArray_titlSub->push_back(sub1);
            //p_jsonArray_titlSub->push_back(sub2);

            this->menubar = new TopMenuBar(TopMenuBar::ShowMenuStyle::AlbumDetail);
            this->menubar->setDataTopMenuBar(QJsonObject(), p_jsonArray_titlSub);

            connect(this->menubar, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(slot_changedSubTabUI(QJsonObject)));

            this->contentStep = "track";
            this->menubar->setSelectedSubMenuNoSignal(this->contentStep);
            this->menubar->hide();

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
    void AppleAlbumDetail::setUIControl_basic(){

        this->album_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Apple_album);
        connect(this->album_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &AppleAlbumDetail::slot_detailInfo_btnClicked);

        this->album_detail_info->initView();

        this->box_main_contents->addWidget(this->album_detail_info);

        // 앨범의 트랙정보 UI
        this->vl_tracks = new QVBoxLayout();
        this->vl_tracks->setContentsMargins(0,0,0,0);
        this->vl_tracks->setSpacing(0);
        this->vl_tracks->setAlignment(Qt::AlignTop);

        this->vl_credit = new QVBoxLayout();
        this->vl_credit->setContentsMargins(0,0,0,0);
        this->vl_credit->setSpacing(0);
        this->vl_credit->setAlignment(Qt::AlignTop);

        this->widget_tab_tracks = new QWidget();
        this->widget_tab_tracks->hide();

        this->widget_tab_credit = new QWidget();
        this->widget_tab_credit->hide();

        this->widget_tab_tracks->setLayout(this->vl_tracks);
        this->widget_tab_credit->setLayout(this->vl_credit);

        this->stackedwidget = new QStackedWidget();
        this->stackedwidget->addWidget(this->widget_tab_tracks);
        this->stackedwidget->addWidget(this->widget_tab_credit);
        this->stackedwidget->hide();

        this->vbox_trackList = new QVBoxLayout();
        this->vbox_trackList->setSpacing(0);
        this->vbox_trackList->setContentsMargins(0, 0, 0, 0);
        this->vbox_trackList->setAlignment(Qt::AlignTop);
        this->vbox_trackList->addWidget(this->menubar);
        this->vbox_trackList->addWidget(this->stackedwidget);

        this->box_main_contents->addLayout(this->vbox_trackList);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : [Slot]
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void AppleAlbumDetail::slot_applyResult_album(const QJsonObject &jsonObj){

        this->jsonObj_albumInfo = jsonObj;
        this->album_detail_info->setData_fromAppleData(jsonObj);

        // request HTTP API - favorite for Rose Server
        roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
        connect(proc_fav, &roseHome::ProcCommon::completeCheck_rating_album, this, &AppleAlbumDetail::slot_applyResult_checkRating_album);
        proc_fav->request_rose_checkRating_Album("APPLE_MUSIC", QString("%1").arg(this->data_album.id));
    }


    void AppleAlbumDetail::slot_applyResult_album_tracks(const QJsonArray &jsonArr){

        if(jsonArr.size() > 0){

            this->jsonArr_tracks_toPlay = QJsonArray();
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr);

            int max_cnt = this->jsonArr_tracks_toPlay.size();

            int makeDuration = 0;
            for(int i = 0; i < max_cnt; i++){
                QJsonObject tmpTrack = this->jsonArr_tracks_toPlay.at(i).toObject();
                QJsonObject tmpAttributes = ProcJsonEasy::getJsonObject(tmpTrack, "attributes");
                makeDuration += ProcJsonEasy::getInt(tmpAttributes, "durationInMillis");
            }
            makeDuration /= 1000;

            // Update UI
            QJsonObject albumObj;
            albumObj.insert("track_count", ProcJsonEasy::getInt(this->jsonObj_albumInfo, "trackCount"));
            albumObj.insert("duration", makeDuration);
            albumObj.insert("registDateTime", ProcJsonEasy::getString(this->jsonObj_albumInfo, "releaseDate"));

            this->album_detail_info->setData_Resolution(albumObj);

            int disc_num = 1;
            for(int i = 0; i < max_cnt; i++){
                QJsonObject tmpTrack = this->jsonArr_tracks_toPlay.at(i).toObject();
                QJsonObject tmpAttributes = ProcJsonEasy::getJsonObject(tmpTrack, "attributes");
                int cd_ico_cnt = ProcJsonEasy::getInt(tmpAttributes, "discNumber");
                int track_number = ProcJsonEasy::getInt(tmpAttributes, "trackNumber");

                this->album_tracks[i] = new AlbumTrackDetailInfo_RHV;
                connect(this->album_tracks[i], &AlbumTrackDetailInfo_RHV::clicked, this, &AppleAlbumDetail::slot_clickedItemTrack_inList);
                this->album_tracks[i]->setProperty("index", i);

                if((cd_ico_cnt > 1) && (track_number == 1)){
                    QWidget *tracks_header = new QWidget();
                    tracks_header->setFixedSize(1500, 52);
                    tracks_header->setContentsMargins(0, 0, 0, 0);

                    QLabel *label_cd_ico = GSCommon::getUILabelImg(":/images/qobuz/album_cd_ico.png", tracks_header);
                    label_cd_ico->setGeometry(0, 19, 30, 30);

                    QLabel *label_cd_Name = new QLabel(tracks_header);
                    label_cd_Name->setStyleSheet("color:#FFFFFF;font-size:24px;font-weight:bold;");
                    label_cd_Name->setGeometry(40, 16, 47, 36);
                    label_cd_Name->setText("DISC" + QString("%1").number(disc_num++));

                    this->vl_tracks->addWidget(tracks_header);
                }

                /*if(i == 0){
                    if(!list_data.at(i).work.isEmpty()){
                        QWidget *widget_work = new QWidget();
                        widget_work->setFixedSize(1470, 47);

                        QLabel *label_work = new QLabel(widget_work);
                        label_work->setStyleSheet("color:#FFFFFF;font-size:16px;font-weight:normal;");
                        label_work->setFixedSize(1470, 24);
                        label_work->setGeometry(30, 23, 0, 0);
                        label_work->setText(list_data.at(i).work);

                        this->vl_tracks->addWidget(widget_work);
                    }
                }
                else{
                    if(list_data.at(i-1).work != list_data.at(i).work){
                        if(!list_data.at(i).work.isEmpty()){
                            QWidget *widget_work = new QWidget();
                            widget_work->setFixedSize(1470, 47);

                            QLabel *label_work = new QLabel(widget_work);
                            label_work->setStyleSheet("color:#FFFFFF;font-size:16px;font-weight:normal;");
                            label_work->setFixedSize(1470, 24);
                            label_work->setGeometry(30, 23, 0, 0);
                            label_work->setText(list_data.at(i).work);

                            this->vl_tracks->addWidget(widget_work);
                        }
                    }
                }*/

                this->vl_tracks->addWidget(this->album_tracks[i]);
            }

            if(this->contentStep == "track"){
                this->widget_tab_tracks->show();
                this->stackedwidget->setFixedHeight(this->vl_tracks->sizeHint().height());
            }
            else{
                this->widget_tab_credit->show();
                this->stackedwidget->setFixedHeight(this->vl_credit->sizeHint().height());
            }

            QList<QString> tmp_clientkey;
            for(int i = 0; i < max_cnt; i++){
                QJsonObject track = this->jsonArr_tracks_toPlay.at(i).toObject();
                this->album_tracks[i]->setDataTrackInfo_Apple(track);

                QString id = ProcJsonEasy::getString(track, "id");
                tmp_clientkey.append(id);

                QCoreApplication::processEvents();
            }

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &AppleAlbumDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_getRating_Track("APPLE_MUSIC", tmp_clientkey);

            this->menubar->setSelectedSubMenuNoSignal(this->contentStep);

            QJsonObject p_data;
            p_data.insert("pageCode", "track");
            slot_changedSubTabUI(p_data);

            this->menubar->show();
            this->stackedwidget->show();

            ContentLoadingwaitingMsgHide();

            this->box_main_contents->addSpacing(30);

            this->flag_track_ok = true;
        }
        else{
            this->flag_track_ok = true;

            ContentLoadingwaitingMsgHide();


            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void AppleAlbumDetail::slot_applyResult_checkRating_album(const QJsonObject &jsonObj){

        this->roseServer_albumId = ProcJsonEasy::getInt(jsonObj, "id");

        if(this->roseServer_albumId > 0){

            if(this->flag_check_album == true){

                QJsonObject album;
                album.insert("favorite", false);
                album.insert("id", this->roseServer_albumId);
                album.insert("star", 0);
                album.insert("trackCount", 0);
                album.insert("type", "APPLE_MUSIC");

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_album_fav);
                ratingInfo.insert("star", this->album_star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", "APPLE_MUSIC");

                QJsonObject json;
                json.insert("album", album);
                json.insert("ratingInfo", ratingInfo);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
                connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_album, this, &AppleAlbumDetail::slot_applyResult_getRating_album);
                proc_fav_album->request_rose_setRating_Album(json, this->flag_album_fav, this->album_star_fav);
            }
            else{

                roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
                connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_album, this, &AppleAlbumDetail::slot_applyResult_getRating_album);
                proc_fav_album->request_rose_getRating_Album("APPLE_MUSIC", ProcJsonEasy::getString(this->jsonObj_albumInfo, "id"));
            }
        }
        else{

            QJsonObject artwork = ProcJsonEasy::getJsonObject(this->jsonObj_albumInfo, "artwork");
            QString url = ProcJsonEasy::getString(artwork, "url");
            url.replace("{w}x{h}", "600x600");

            QJsonObject album;
            album.insert("clientKey", ProcJsonEasy::getString(this->jsonObj_albumInfo, "id"));
            album.insert("favorite", false);
            album.insert("star", 0);
            album.insert("thumbnail", url);
            album.insert("title", ProcJsonEasy::getString(this->jsonObj_albumInfo, "name"));
            album.insert("trackCount", ProcJsonEasy::getInt(this->jsonObj_albumInfo, "trackCount"));
            album.insert("type", "APPLE_MUSIC");

            QJsonObject json;
            json.insert("album", album);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
            connect(proc_fav_album, &roseHome::ProcCommon::completeCheck_rating_album, this, &AppleAlbumDetail::slot_applyResult_addRating_album);
            proc_fav_album->request_rose_addRating_Album(json);
        }
    }


    void AppleAlbumDetail::slot_applyResult_getRating_album(const QJsonArray &contents){

        if(contents.size() > 0){

            QJsonObject tmp_star = contents.at(0).toObject();

            if(tmp_star.contains("star")){
                int star = ProcJsonEasy::getInt(tmp_star, "star");
                bool flag = star > 0 ? true : false;

                this->album_detail_info->setFavorite(flag, star);
            }
            else{

                this->album_detail_info->setFavorite(this->flag_album_fav, this->album_star_fav);
            }

            if(this->flag_check_album == true){
                this->flag_check_album = false;
            }
        }
    }


    void AppleAlbumDetail::slot_applyResult_addRating_album(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){

            // request HTTP API - favorite for Rose Server
            roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
            connect(proc_fav, &roseHome::ProcCommon::completeCheck_rating_album, this, &AppleAlbumDetail::slot_applyResult_checkRating_album);
            proc_fav->request_rose_checkRating_Album("APPLE_MUSIC", QString("%1").arg(this->data_album.id));

            if(this->flag_check_album == true){
                this->flag_check_album = false;
            }
        }
        else{
            this->flag_check_album = false;
        }
    }


    void AppleAlbumDetail::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("id")){

            int id = ProcJsonEasy::getInt(jsonObj, "id");

            if(id > 0){
                this->roseServer_trackId[this->track_idx_fav] = id;

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_track_fav);
                ratingInfo.insert("star", this->track_star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", "APPLE_MUSIC");

                QJsonObject track;
                track.insert("duration", 0);
                track.insert("favorite", false);
                track.insert("id", id);
                track.insert("ownerId", 0);
                track.insert("sort", 0);
                track.insert("star", 0);
                track.insert("type", "APPLE_MUSIC");

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &AppleAlbumDetail::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
            }
            else{

                QJsonObject data = this->jsonArr_tracks_toPlay[this->track_idx_fav].toObject();

                QJsonObject attributes = ProcJsonEasy::getJsonObject(data, "attributes");
                QJsonObject artwork = ProcJsonEasy::getJsonObject(attributes, "artwork");

                QString url = ProcJsonEasy::getString(artwork, "url");
                url.replace("{w}x{h}", "600x600");

                QJsonArray albums = QJsonArray();
                albums.append(ProcJsonEasy::getString(attributes, "albumName"));

                QJsonArray artists = QJsonArray();
                artists.append(ProcJsonEasy::getString(attributes, "artistName"));

                QJsonObject track;
                track.insert("title", ProcJsonEasy::getString(attributes, "name"));
                track.insert("type", "APPLE_MUSIC");
                track.insert("favorite", false);
                track.insert("artists", artists);
                track.insert("albums", albums);
                track.insert("star", 0);
                track.insert("thumbnailUrl", url);
                track.insert("clientKey", ProcJsonEasy::getString(data, "id"));
                track.insert("duration", ProcJsonEasy::getInt(attributes, "durationInMillis"));
                track.insert("data", data);

                QJsonArray tracks;
                tracks.append(track);

                QJsonObject json;
                json.insert("tracks", tracks);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &AppleAlbumDetail::slot_applyResult_addRating_track);
                proc_fav_track->request_rose_addRating_Track(json);
            }
        }
        else{

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void AppleAlbumDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){

            for(int i = 0; i < contents.size(); i++){

                QJsonObject track = contents.at(i).toObject();

                if(track.contains("star")){
                    QString clientKey = ProcJsonEasy::getString(track, "clientKey");
                    int star = ProcJsonEasy::getInt(track, "star");

                    QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(i).toObject();
                    QString trackID = ProcJsonEasy::getString(trackInfo, "id");

                    if(clientKey == trackID){

                        bool flag = (star > 0) ? true : false;
                        this->album_tracks[i]->setFavoritesIds(flag, star);
                    }
                }
                else{

                    this->album_tracks[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
                }

            }
        }

        if(this->flag_check_track == true){
            this->flag_check_track = false;
        }
    }


    void AppleAlbumDetail::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
            connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &AppleAlbumDetail::slot_applyResult_checkRating_track);
            proc_favCheck_track->request_rose_checkRating_Track("APPLE_MUSIC", ProcJsonEasy::getString(this->jsonArr_tracks_toPlay[this->track_idx_fav].toObject(), "id"));

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void AppleAlbumDetail::changedOnlyTabUI_notSendSignal(QString p_step){

        this->contentStep = p_step;

        // Tab 타이틀 변경
        this->menubar->setSelectedSubMenuNoSignal(contentStep);

        // DB 가져와서 데이터 세팅
        if(this->contentStep == ALBTAB_STEP_TRACK){
            this->stackedwidget->setCurrentIndex(0);
        }else if(this->contentStep == ALBTAB_STEP_CREDIT){
            this->stackedwidget->setCurrentIndex(1);
        }
    }


    void AppleAlbumDetail::slot_changedSubTabUI(const QJsonObject &p_data){

        QString tmp_step = p_data[KEY_PAGE_CODE].toString();

        // 페이지 변경 및 데이터 세팅
        this->changedOnlyTabUI_notSendSignal(tmp_step);
    }


    void AppleAlbumDetail::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_APPLE)){

            global.user.setDeviceChange(false);

            /*if(global.user_forApple.isLogined() == true){
                AbstractQobuzSubWidget::slot_acceptedDialogLogin();
            }
            else if(global.user_forApple.isLogined() == false){
                AbstractQobuzSubWidget::slot_acceptedDialogLogout();
            }*/

            this->data_album.id = 0;

            this->setJsonObject_forData(login_wait_Object);
            this->setActivePage();
        }
    }

    void AppleAlbumDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

        if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx2 ||
                clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx3 || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){

            if(this->flag_check_album == false){

                this->album_star_fav = this->album_detail_info->getFavoritesStars();
                this->flag_album_fav = false;

                if(this->album_star_fav == 3){
                    this->album_star_fav = 0;
                    this->flag_album_fav = false;
                }
                else if(this->album_star_fav >= 0 && this->album_star_fav < 3){
                    this->album_star_fav++;
                    this->flag_album_fav = true;
                }

                /*if(this->album_star_fav == 0 || this->album_star_fav == 1){
                    // Apple Favorite toggle
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzAlbumDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("album", QString("%1").arg(data_album.id), this->flag_album_fav);
                    this->flag_send_album = true;
                }*/

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_album = new roseHome::ProcCommon(this);
                connect(proc_favCheck_album, &roseHome::ProcCommon::completeCheck_rating_album, this, &AppleAlbumDetail::slot_applyResult_checkRating_album);
                proc_favCheck_album->request_rose_checkRating_Album("APPLE_MUSIC", QString("%1").arg(this->data_album.id));

                this->flag_check_album = true;
            }
        }
        else{
            if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::AddCollection){

            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){

                // OptMorePopup 띄우기 - 즐겨찾기 관련 메뉴를 가림. DetailView에 하트가 있음
                this->makeObj_optMorePopup(OptMorePopup::Apple_Album, apple::ConvertData::getConvertOptHeaderData_album(this->jsonObj_albumInfo), 0, SECTION_FOR_MORE_POPUP___album, true);
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayAll){

                // 전체재생
                apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
                procRosePlay->requestPlayRose_byTracks(this->jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, apple::ProcRosePlay_withApple::PlayShuffleMode::JustPlay);
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayShuffle){

                // 셔플재생
                apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
                procRosePlay->requestPlayRose_byTracks(this->jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, apple::ProcRosePlay_withApple::PlayShuffleMode::ShufflePlay);
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Share){

                this->shareLink = this->album_detail_info->detailInfo_shareLink;    //220905 share code ... --

                //220816 share code ... --
                setUIShare();//c220818
            }
        }
    }


    void AppleAlbumDetail::slot_clickedItemTrack_inList(const int idx, const AlbumTrackDetailInfo_RHV::ClickMode clickMode){

        if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                int star_cnt = this->album_tracks[idx]->getFavoritesStars();
                this->flag_track_fav = false;

                this->track_star_fav = star_cnt;

                if(this->track_star_fav == 3){
                    this->track_star_fav = 0;
                    this->flag_track_fav = false;
                }
                else if(this->track_star_fav >= 0 && this->track_star_fav < 3){
                    this->track_star_fav++;
                    this->flag_track_fav = true;
                }

                /*if(this->track_star_fav == 0 || this->track_star_fav == 1){
                    // Apple Favorite toggle
                    this->track_id_fav = this->list_track->at(idx).id;

                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzAlbumDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                    this->flag_send_track = true;
                }*/

                this->track_idx_fav = idx;
                QJsonObject jsonObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &AppleAlbumDetail::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("APPLE_MUSIC", ProcJsonEasy::getString(jsonObj, "id"));

                this->flag_check_track = true;
            }
        }
        else if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::AllBox){

            int playType = global.device.getMusicPlayType();

            OptMorePopup::ClickMode curr_clickMode = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
            switch(playType)
            {
                case 13:
                    curr_clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                    break;
                case 12:
                    curr_clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                    break;
                case 15:
                    curr_clickMode = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    break;
                case 16:
                    curr_clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                    break;
                case 17:
                    curr_clickMode = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                    break;
                case 18:
                    curr_clickMode = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                    break;
                case 19:
                    curr_clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                    break;
            }

            apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
            procRosePlay->requestPlayRose_byTracks(this->jsonArr_tracks_toPlay, idx, curr_clickMode);
        }
        else{
            // OptMorePopup 띄우기 - 즐겨찾기 관련 메뉴를 가림. DetailView에 하트가 있음
            this->makeObj_optMorePopup(OptMorePopup::Apple_Track, apple::ConvertData::getConvertOptHeaderData_track(this->jsonArr_tracks_toPlay.at(idx).toObject()), idx, SECTION_FOR_MORE_POPUP___track, true);
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void AppleAlbumDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___album){

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
                apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
                procRosePlay->requestPlayRose_byTracks(this->jsonArr_tracks_toPlay, 0, clickMode);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___track){

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
                // Rose Play 요청// Rose Play 요청
                apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
                procRosePlay->requestPlayRose_byTracks(this->jsonArr_tracks_toPlay, index, clickMode);
            }
        }
    }
}
