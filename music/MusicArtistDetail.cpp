#include "music/MusicArtistDetail.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"
#include "common/rosesettings.h"
#include "common/sqlitehelper.h"

#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"
#include "widget/optionpopup.h"

#include <QDebug>
#include <QPainter>
#include <QScrollArea>
#include <QSettings>
#include <QPixmapCache>


namespace music {

    const int BTN_IDX_SUBTITLE_tracks = 1;
    const int BTN_IDX_SUBTITLE_albums = 2;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___artist = 0;
    const int SECTION_FOR_MORE_POPUP___tracks = 1;
    const int SECTION_FOR_MORE_POPUP___albums = 2;

    const int HTTP_NETWORK_PLAY = 99;


    /**
     * @brief
     * @param parent
     */
    ArtistDetail::ArtistDetail(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_rosetube, parent) {

        this->linker = Linker::getInstance();

        this->list_artistAlbums = new QList<roseHome::AlbumItemData>();
    }


    /**
     * @brief 소멸자.
     */
    ArtistDetail::~ArtistDetail(){

        this->deleteLater();
    }


    /**
     * @brief RoseHomeAlbumDetail::setJsonObject_forData
     * @param jsonObj
     */
    void ArtistDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        /*QJsonDocument doc(jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << "jsonObj = " << strJson;*/

        QJsonArray tmpArr = ProcJsonEasy::getJsonArray(jsonObj, "op_data");
        QString artistName = "";
        if(tmpArr.size() > 0){
            artistName = ProcJsonEasy::getString(tmpArr.at(0).toObject(), "artist");
        }
        else{
            artistName = ProcJsonEasy::getString(jsonObj, "op_mainTitle");
        }

        this->flagNeedReload = false;

        if(artistName != this->artist_name){
            this->flagNeedReload = true;

            this->artist_name = artistName;
            this->artist_image = ProcJsonEasy::getString(jsonObj, "op_albumImg");

            this->jsonArrTracks = QJsonArray();
            this->list_artistAlbums->clear();

            if(this->flag_track_ok == true){

                this->widget_main_contents->hide();

                this->box_contents->removeWidget(this->widget_main_contents);
                GSCommon::clearLayout(this->box_contents);

                this->flag_track_ok = false;
            }

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(70, 0, 70, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // Album 상세정보
            this->artist_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Music_artist);
            connect(this->artist_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &ArtistDetail::slot_detailInfo_btnClicked);

            this->artist_detail_info->initView();

            QJsonObject tmpData;
            tmpData.insert("image", this->artist_image);
            tmpData.insert("title", this->artist_name);

            this->artist_detail_info->setData_fromMusicData(tmpData);

            this->box_main_contents->addWidget(this->artist_detail_info);
            this->box_main_contents->addSpacing(50);

            for(int i = 0; i < 5; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            this->vbox_trackList = new QVBoxLayout();
            this->vbox_albumList = new QVBoxLayout();

            this->flow_albums = new FlowLayout(0, 0, 20);

            this->widget_album = new QWidget();
            this->widget_album->setStyleSheet("background-color:transparent;");
            this->widget_album->setLayout(this->flow_albums);

            // clear UI
            GSCommon::clearLayout(this->vbox_trackList);
            GSCommon::clearLayout(this->vbox_albumList);

            GSCommon::clearLayout(this->flow_albums);

            this->vbox_albumList->addWidget(this->widget_album);

            for(int i = 0; i < 5; i++) {
                this->artist_tracks[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->artist_tracks[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &ArtistDetail::slot_clickedItemTrack_inList);

                this->artist_tracks[i]->setProperty("index", i);
                this->artist_tracks[i]->setObjectName("top_tracks");
            }

            for(int i = 0; i < 14; i++){
                this->artist_albums[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                connect(this->artist_albums[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &ArtistDetail::slot_clickedItemAlbum);
            }
        }
    }


    /**
     * @brief RoseHomePlaylistDetail_Rose 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void ArtistDetail::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->setDataTrackFromDB();
            this->setDataAlbumFromDB();
            ContentLoadingwaitingMsgHide();
        }
    }


    void ArtistDetail::setDataTrackFromDB(){

        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){

            QString strQuery = "";
            strQuery = " SELECT count(A.album) AS track_count ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            if(this->artist_name.front() == "'" || this->artist_name.back() == "'"){
                strQuery += " WHERE A.artist_id=(SELECT artist_id from artists where artist='\"%1\"') ORDER BY A.track ";
            }
            else{
                strQuery += " WHERE A.artist_id=(SELECT artist_id from artists where artist='%1') ORDER BY A.track ";
            }

            QVariantList data_cnt;
            sqlite->exec(strQuery.arg(this->artist_name), data_cnt);

            int track_cnt = 0;
            if(data_cnt.size() > 0){
                QJsonObject tmp_data = data_cnt.at(0).toJsonObject();
                track_cnt = tmp_data["track_count"].toInt();

                QVBoxLayout *vbox_trackLayout = new QVBoxLayout();
                vbox_trackLayout->setContentsMargins(0, 0, 0, 0);
                vbox_trackLayout->setAlignment(Qt::AlignTop);

                QWidget *widget_trackLayout = new QWidget();
                widget_trackLayout->setFixedSize(1500, 360);
                widget_trackLayout->setStyleSheet("background-color:#212121;");
                widget_trackLayout->setLayout(vbox_trackLayout);

                this->vbox_trackList->addWidget(widget_trackLayout);

                this->setUIControl_subTitle_withSideBtn(QString("Tracks (%1)").arg(track_cnt), "View All", BTN_IDX_SUBTITLE_tracks);
                this->box_main_contents->addSpacing(10);
                this->box_main_contents->addLayout(this->vbox_trackList);
                this->box_main_contents->addSpacing(30);

                strQuery = "";
                strQuery = " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art , A.mime_type, A.samplerate, A.bitdepth ";
                strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id";
                if(this->artist_name.front() == "'" || this->artist_name.back() == "'"){
                    strQuery += " WHERE A.artist_id=(SELECT artist_id from artists where artist= '\"%1\"') ORDER BY A.track LIMIT 5 ";
                }
                else{
                    strQuery += " WHERE A.artist_id=(SELECT artist_id from artists where artist='%1') ORDER BY A.track LIMIT 5 ";
                }

                QVariantList data;
                sqlite->exec(strQuery.arg(this->artist_name), data);

                QJsonArray tmpArr;
                if(data.size() > 0){
                    for(int i = 0 ; i < data.size(); i++){
                        QJsonObject tmp_data = data.at(i).toJsonObject();
                        tmpArr.append(tmp_data);

                        this->artist_tracks[i]->setDataTrackInfo_Music(tmp_data);
                        //this->vbox_trackList->addWidget(this->artist_tracks[i]);
                        vbox_trackLayout->addWidget(this->artist_tracks[i]);
                    }
                }
                this->jsonArrTracks = tmpArr;

                this->flag_track_ok = true;
            }
            else{
                this->setUIControl_subTitle_withSideBtn(QString("Tracks (%1)").arg(track_cnt), "View All", BTN_IDX_SUBTITLE_tracks);
                this->box_main_contents->addSpacing(10);
                this->box_main_contents->addLayout(this->vbox_trackList);
                this->box_main_contents->addSpacing(30);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
                noData_widget->setFixedSize(1500, 350);

                this->vbox_trackList->addWidget(noData_widget);

                this->flag_track_ok = true;
            }
        }

        sqlite->close();
        delete sqlite;
    }


    void ArtistDetail::setDataAlbumFromDB(){

        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            strQuery = " SELECT count(AI.album) AS album_count ";
            strQuery += " FROM album_info AS AI ";
            strQuery += " LEFT OUTER JOIN album_art AS AA ";
            strQuery += " ON AI._id=AA.album_id ";
            if(this->artist_name.front() == "'" || this->artist_name.back() == "'" || this->artist_name.contains("'")){
                strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist=\"%1\")";
            }
            else{
                strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist='%1') ";
            }

            QVariantList data_cnt;
            sqlite->exec(strQuery.arg(this->artist_name), data_cnt);

            int album_cnt = 0;
            if(data_cnt.size() > 0){
                QJsonObject tmp_data = data_cnt.at(0).toJsonObject();
                album_cnt = tmp_data["album_count"].toInt();

                this->setUIControl_subTitle_withSideBtn(QString("Albums (%1)").arg(album_cnt), "View All", BTN_IDX_SUBTITLE_albums);
                this->box_main_contents->addSpacing(10);
                this->box_main_contents->addLayout(this->vbox_albumList);
                this->box_main_contents->addSpacing(30);

                strQuery = "";
                strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art ";
                strQuery += " FROM album_info AS AI ";
                strQuery += " LEFT OUTER JOIN album_art AS AA ";
                strQuery += " ON AI._id=AA.album_id ";

                if(this->artist_name.front() == "'" || this->artist_name.back() == "'" || this->artist_name.contains("'")){
                    strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist=\"%1\") ORDER BY AI.album LIMIT 14 ";
                }
                else{
                    strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist='%1') ORDER BY AI.album LIMIT 14 ";
                }

                QVariantList data;
                sqlite->exec(strQuery.arg(this->artist_name), data);

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

                        this->list_artistAlbums->append(data_output);

                        this->artist_albums[i]->setData(data_output);
                        this->flow_albums->addWidget(this->artist_albums[i]);
                    }
                }
            }
            else{
                this->setUIControl_subTitle_withSideBtn(QString("Albums (%1)").arg(album_cnt), "View All", BTN_IDX_SUBTITLE_albums);
                this->box_main_contents->addSpacing(10);
                this->box_main_contents->addLayout(this->vbox_albumList);
                this->box_main_contents->addSpacing(30);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(1500, 500);

                this->flow_albums->addWidget(noData_widget);
            }
        }

        sqlite->close();
        delete sqlite;
    }


    void ArtistDetail::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId){

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


    // 서브 타이틀별 우측 View All ---------------------------------------------------------------------------------------------------

    /**
     * @brief 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void ArtistDetail::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_tracks){
            QJsonObject jsonData;
            jsonData.insert(KEY_PAGE_CODE, PAGECODE_VA_ARTIST_TRACKLIST);
            jsonData.insert("title", QString("'%1' - ").arg(this->artist_name) + tr("Tracks"));
            jsonData.insert("artist", this->artist_name);

            emit linker->signal_clickedMovePage(jsonData);
        }
        else if(btnId == BTN_IDX_SUBTITLE_albums){
            QJsonObject jsonData;
            jsonData.insert(KEY_PAGE_CODE, PAGECODE_VA_ARTIST_ALBUMLIST);
            jsonData.insert("title", QString("'%1' - ").arg(this->artist_name) + tr("Albums"));
            jsonData.insert("artist", this->artist_name);

            emit linker->signal_clickedMovePage(jsonData);
        }
    }


    void ArtistDetail::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

        if(p_id == HTTP_NETWORK_PLAY){
            QJsonDocument doc(p_jsonObject);
            QString strJson(doc.toJson(QJsonDocument::Compact));
            //qDebug() << "jsonObj = " << strJson;
        }
    }


    /**
     * @brief [Slot] ArtistImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void ArtistDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

        if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){

            /*if(this->flag_check_artist == false){

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

                this->artist_detail_info->setFavorite(this->flag_artist_fav, this->artist_star_fav);
            }*/
        }
        else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){

            roseHome::AlbumItemData data_album;
            data_album.title = this->artist_name;
            data_album.thumbnail = this->artist_image;

            // OptMorePopup 띄우기
            this->makeObj_optMorePopup(OptMorePopup::Rosehome_Artist, roseHome::ConvertData::getConvertOptHeaderData(data_album), 0, SECTION_FOR_MORE_POPUP___artist, true);
        }
    }


    void ArtistDetail::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        // ClickMode 별로 처리
        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

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
            }*/
        }
        else{
            // ClickMode 별로 처리
            if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AllBox){
                // Track 바로 재생 - clicked 항목만 재생
                OptMorePopup::ClickMode curr_playType = OptMorePopup::ClickMode::Play_RightNow_withReorder;
                int playType = global.device.getMusicPlayType();

                if(playType == 15){
                    curr_playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                }
                else if(playType == 13){
                    curr_playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                }
                else if(playType == 12){
                    curr_playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                }
                else if(playType == 16){
                    curr_playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                }
                else if(playType == 17){
                    curr_playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                }
                else if(playType == 18){
                    curr_playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                }
                else if(playType == 19){
                    curr_playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                }

                QJsonArray jsonArr_tracks = this->get_rearrangeJsonArray_toPlayData(this->jsonArrTracks, idx, curr_playType);

                QJsonObject tmp_json;
                tmp_json.insert("musicPlayType", playType);
                tmp_json.insert("music", jsonArr_tracks);
                tmp_json.insert("currentPosition", 0);
                tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

                if(global.Queue_track_count != 0) {
                    print_debug();
                    emit this->linker->signal_checkQueue(27, "");

                    return;
                }
                print_debug();
                emit this->linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(HTTP_NETWORK_PLAY
                                 , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song")
                                 , tmp_json
                                 , true
                                 , true);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                // 플레이리스트 담기 - Track
                //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){
                // OptMorePopup 띄우기 필요
                QJsonObject tmpObj = this->jsonArrTracks.at(idx).toObject();

                QString tmpImg = ProcJsonEasy::getString(tmpObj, "album_art");
                if(!tmpImg.isEmpty()){
                    tmpImg = "http://" + global.device.getDeviceIP() + ":" + global.port_img + tmpImg;
                }

                OptMorePopup::HeaderData data_header;
                data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                data_header.sub_title = ProcJsonEasy::getString(tmpObj, "artist");
                data_header.imageUrl = tmpImg;
                data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(tmpObj, "id"));
                data_header.type = "MUSIC";      //j220906 share link
                data_header.flagProcStar = false;
                data_header.isShare = false;      //j220906 share link

                // OptMorePopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Track, data_header, idx, SECTION_FOR_MORE_POPUP___tracks);
            }
        }
    }


    void ArtistDetail::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리 - Album 파트가 여러개임

        if(section == SECTION_FOR_MORE_POPUP___albums){

            roseHome::AlbumItemData data_album = this->list_artistAlbums->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Music_Album, roseHome::ConvertData::getConvertOptHeaderData(data_album), index, SECTION_FOR_MORE_POPUP___albums, true);
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
    }


    void ArtistDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___tracks){

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
                int playType = this->get_rose_playType(clickMode);

                QJsonArray jsonArr_tracks = this->get_rearrangeJsonArray_toPlayData(this->jsonArrTracks, index, clickMode);

                QJsonObject jsonData;
                jsonData.insert("musicPlayType", playType);
                jsonData.insert("music", jsonArr_tracks);
                jsonData.insert("currentPosition", 0);
                jsonData.insert("roseToken", global.device.getDeviceRoseToken());

                if(global.queueTimerFlag && !global.queuelist_mouse_trigger_menu_flag && global.Queue_track_count != 0) {
                    emit this->linker->signal_checkQueue(27, "");

                    return;
                }
                emit this->linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3")
                                 .arg(global.device.getDeviceIP())
                                 .arg(global.port).arg("music_song")
                                 , jsonData
                                 , true
                                 , true);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___albums){

            roseHome::AlbumItemData data_album = this->list_artistAlbums->at(index);


            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                int playType = 0;
/*
                if(clickMode == OptMorePopup::ClickMode::Share){//c220826_2
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }*/
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
                    print_debug();emit linker->signal_checkQueue(27, "");

                    return;
                }
                print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
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
    }


    int ArtistDetail::get_rose_playType(OptMorePopup::ClickMode clickMode){


            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::Play_RightNow_withReorder
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
                ){
            // 바로 재생을 요청하는 타입
            return 15;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
                ){
            // 현재 Queue 끝에 추가
            return 13;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
                ){
            // 큐를 비우고 추가(재생)
            return 12;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
                ){
            return 16;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere){
            // 여기서부터 재생
            return 17;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty){
            // 큐 비우고 여기서부터 재생
            return 18;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last){
            // 여기서부터 큐 끝에 추가
            return 19;
        }
        else{
            return 0;           // unknown or not supported
        }
    }


    QJsonArray ArtistDetail::get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode){


            if(clickMode == OptMorePopup::ClickMode::Play_RightNow_withReorder
                || clickMode == OptMorePopup::ClickMode::Play_RightNow
        ){
            if(curr_index > 0){
                // 전체이지만 re-ordering
                QJsonArray jsonArr_output = this->reorderJsonArray(jsonArr_toPlayAll, curr_index);
                return jsonArr_output;
            }
            else{
                return jsonArr_toPlayAll;
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
        ){
            // 1곡만
            QJsonArray jsonArr_output;
            jsonArr_output.append(jsonArr_toPlayAll.at(curr_index).toObject());
            return jsonArr_output;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last
        ){
            // index ~ 마지막까지 구성
            if(curr_index > 0){
                QJsonArray jsonArr_output = this->subRange_JsonArray(jsonArr_toPlayAll, curr_index);
                return jsonArr_output;
            }
            else{
                return jsonArr_toPlayAll;
            }
        }
        else{
            // 그 이외의 경우
            return jsonArr_toPlayAll;
        }

    }


    /**
     * @brief AbstractProcRosePlay::subRange_JsonArray
     * @param p_jsonArr
     * @param startIndex
     * @return
     */
    QJsonArray ArtistDetail::subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex){

        const int len_data = p_jsonArr.count();

        if(startIndex > 0 && startIndex < len_data){
            QJsonArray jsonArr_subRange;
            for(int i = startIndex; i < len_data; i++){
                jsonArr_subRange.append(p_jsonArr.at(i));
            }
            return jsonArr_subRange;
        }
        else{
            // 유효하지 않는 경우, 그대로 반환
            return p_jsonArr;
        }
    }


    /**
     * @brief AbstractProcRosePlay::reorderJsonArray
     * @param p_jsonArr
     * @param startIndex
     * @return
     */
    QJsonArray ArtistDetail::reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex){

        const int len_data = p_jsonArr.count();

        if(startIndex > 0 && startIndex < len_data){
            QJsonArray jsonArr_reorder;
            for(int i = startIndex; i < len_data; i++){
                jsonArr_reorder.append(p_jsonArr.at(i));
            }
            for(int i = 0; i < startIndex; i++){
                jsonArr_reorder.append(p_jsonArr.at(i));
            }
            return jsonArr_reorder;
        }
        else{
            // 유효하지 않는 경우, 그대로 반환
            return p_jsonArr;
        }
    }


    QJsonArray ArtistDetail::getDataForPlayMusic(const int album_id){

        QJsonArray jsonArray;

        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();
        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, A.mime_type, A.samplerate, A.bitdepth, ART._data AS album_art ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            strQuery += " WHERE A.album_id=%1 ORDER BY A.track ";

            QVariantList dataDB;
            sqlite->exec(strQuery.arg(album_id), dataDB);
            if(dataDB.size() > 0){
                for(int i = 0 ; i < dataDB.size(); i++){
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
}
