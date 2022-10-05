#include "music/MusicTrackListAll_rose.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/rosesettings.h"
#include "common/ProcJsonEasy.h"
#include "common/sqlitehelper.h"

#include "widget/NoData_Widget.h"
#include "widget/optionpopup.h"
#include "widget/toastmsg.h"
#include "widget/VerticalScrollArea.h"

#include <QDebug>
#include <QLabel>
#include <QScroller>
#include <QScrollBar>
#include <QPushButton>
#include <QPixmapCache>


namespace music {

    const int SECTION_FOR_MORE_POPUP___tracks = 0;

    const int HTTP_NETWORK_PLAY = 99;

    QString ICON_PATH___playAll = ":/images/icon_list_allplay.png";
    QString ICON_PATH___playShuffle = ":/images/icon_list_shuffle.png";


    /**
     * @brief "ROSEHOME > REcENT TRACK" 화면을 위한 생성자. @see PAGECODE_RH_TRACK_LIST_ALL_VIEW
     * @param parent
     */
    TrackListAll::TrackListAll(QWidget *parent) : AbstractRoseHomeSubWidget(VerticalScroll_rosefilter, parent) {

        // data
        this->list_track = new QList<roseHome::TrackItemData>();

        // UI
        this->setUIControl_tracks();
    }


    /**
    * @brief 소멸자.
    */
    TrackListAll::~TrackListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TrackListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::PageInfo_TrackAllView data_pageInfo = roseHome::ConvertData::convertData_pageInfo_trackAllView(jsonObj);
        this->flagNeedReload = false;

        if(this->page != data_pageInfo.type || this->title != data_pageInfo.pathTitle){
            this->flagNeedReload = true;

            this->page = data_pageInfo.type;
            this->title = data_pageInfo.pathTitle;
            this->api_subPath = data_pageInfo.api_subPath;

            this->label_mainTitle->setText(this->title);

            // init
            this->list_track->clear();
            this->jsonArr_tracks_toPlay = QJsonArray();

            this->track_id_fav = 0;
            this->track_idx_fav = 0;
            this->track_star_fav = 0;

            this->track_totalCount = 0;
            this->track_drawCount = 0;
            this->track_favoriteOffset = 0;

            this->flag_draw = false;
            this->flag_check_track = false;
            this->flag_track_fav = false;
            this->flag_send_track = false;

            // request HTTP API
            this->flagReqMore_track = false;
            this->flag_lastPage_track = false;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackData();
        }
    }


    /**
     * @brief TrackListAll::setActivePage
     */
    void TrackListAll::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            this->box_filter->removeWidget(this->widget_filter_contents);

            this->widget_filter_contents = new QWidget();
            this->widget_filter_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_filter_contents->setFixedSize(1500, 50);

            int width = 0;
            int left = 0;
            // All
            QPixmap pixmap_play(":/images/icon_list_allplay.png");
            QPushButton *btn_playAll = new QPushButton(this->widget_filter_contents);
            btn_playAll->setIcon(pixmap_play);
            btn_playAll->setIconSize(pixmap_play.rect().size());
            btn_playAll->setText("Play All");
            btn_playAll->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            btn_playAll->setCursor(Qt::PointingHandCursor);

            width = btn_playAll->sizeHint().width();
            btn_playAll->setGeometry(left, 0, width, 50);
            connect(btn_playAll, &QPushButton::clicked, this, &TrackListAll::slot_btnClicked_playAll);

            // shuffle
            QPixmap pixmap_shuffle(":/images/icon_list_shuffle.png");
            QPushButton *btn_shuffleAll = new QPushButton(this->widget_filter_contents);
            btn_shuffleAll->setIcon(pixmap_shuffle);
            btn_shuffleAll->setIconSize(pixmap_shuffle.rect().size());
            btn_shuffleAll->setText("Shuffle Play");
            btn_shuffleAll->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            btn_shuffleAll->setCursor(Qt::PointingHandCursor);

            left = width + 30;
            width = btn_shuffleAll->sizeHint().width();
            btn_shuffleAll->setGeometry(left, 0, width, 50);
            connect(btn_shuffleAll, &QPushButton::clicked, this, &TrackListAll::slot_btnClicked_playShuffle);

            this->box_filter->addWidget(this->widget_filter_contents);

            this->box_contents->removeWidget(this->widget_main_contents);

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            this->vBox_tracks = new QVBoxLayout();
            this->vBox_tracks->setSpacing(0);
            this->vBox_tracks->setContentsMargins(0, 0, 0, 0);
            GSCommon::clearLayout(this->vBox_tracks);

            this->box_main_contents->addLayout(this->vBox_tracks);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void TrackListAll::setUIControl_tracks(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Tracks");
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void TrackListAll::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void TrackListAll::request_more_trackData(){

        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // next_offset
            if(this->list_track->size() == 0){
                this->next_offset = 0;
            }
            else{
                this->next_offset++;
            }

            // request HTTP API
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_tracks, this, &TrackListAll::slot_applyResult_tracks);
            proc->request_rose_getList_recentlyTracks(this->api_subPath, "MUSIC", this->next_offset, 20);

        }
    }


    void TrackListAll::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > 20 ) ? 20 : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TrackListAll::slot_clickedItemTrack_inList);
            this->track_listAll[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->track_listAll[i]);
        }

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i]->setDataTrackInfo_Rose(this->list_track->at(i));
            this->track_listAll[i]->setFavoritesIds(this->list_track->at(i).favorite, this->list_track->at(i).star);

            QCoreApplication::processEvents();
        }

        if(this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] Track 데이터를 받아서 처리함
     * @param list_data
     */
    void TrackListAll::slot_applyResult_tracks(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            if(list_data.size() == 0){
                //if(abs_ani_dialog_wait->isHidden() != true){
                    ContentLoadingwaitingMsgHide();
                //}

                return;
            }

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
                this->track_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_track->count();

            this->list_track->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                int max_cnt = this->list_track->size();
                this->track_drawCount = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TrackListAll::slot_clickedItemTrack_inList);
                    this->track_listAll[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->track_listAll[i]);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i]->setDataTrackInfo_Rose(this->list_track->at(i));
                    this->track_listAll[i]->setFavoritesIds(this->list_track->at(i).favorite, this->list_track->at(i).star);

                    QCoreApplication::processEvents();
                }

                ContentLoadingwaitingMsgHide();
                this->flag_widget_visible = true;

                if(this->track_totalCount > this->list_track->count()){
                    this->request_more_trackData();
                }
                else{
                    this->box_main_contents->addSpacing(30);
                }
            }
            else{

                if(this->track_totalCount > this->list_track->count()){
                    this->request_more_trackData();
                }
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void TrackListAll::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->track_listAll[this->track_favoriteOffset + i]->setFavoritesIds(flag, star);
                }
            }
            else if(track_info.contains("message") && (track_info["message"].toString() == "정상")){
                this->track_listAll[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void TrackListAll::slot_btnClicked_playAll(){

        roseHome::TrackItemData data_track;

        // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
        procRosePlay->requestPlayRose_byTracks(data_track, jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow_withReorder, AbstractProcRosePlay::PlayShuffleMode::JustPlay);
    }

    void TrackListAll::slot_btnClicked_playShuffle(){

        roseHome::TrackItemData data_track;

        // Track shuffle 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
        procRosePlay->requestPlayRose_byTracks(data_track, jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow_withReorder, AbstractProcRosePlay::PlayShuffleMode::ShufflePlay);
    }


    /**
     * @brief [Slot] PlaylistTrackDetailInfo_RHV signal에 대하여 처리
     * @param clickMode
     */
    void TrackListAll::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->track_listAll[idx]->getFavoritesStars();
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
                ratingInfo.insert("type", this->list_track->at(idx).type);

                QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TrackListAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

                this->flag_check_track = true;
            }
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, SECTION_FOR_MORE_POPUP___tracks);
        }
    }

    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void TrackListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }





    /**
     * @brief "Artist > Track viewall" 화면을 위한 생성자.
     * @see PAGECODE_RH_TRACK_LIST_ALL_VIEW
     * @param parent
     */
    ArtistTrackListAll::ArtistTrackListAll(QWidget *parent) : AbstractRoseHomeSubWidget(VerticalScroll_rosefilter, parent) {

        this->linker = Linker::getInstance();

        // data
        this->list_track = new QList<roseHome::TrackItemData>();

        // UI
        this->setUIControl_tracks();
    }


    /**
    * @brief 소멸자.
    */
    ArtistTrackListAll::~ArtistTrackListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void ArtistTrackListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString tmpTitle = ProcJsonEasy::getString(jsonObj, "title");
        QString tmpArtist = ProcJsonEasy::getString(jsonObj, "artist");
        this->flagNeedReload = false;

        if(this->title != tmpTitle){
            this->flagNeedReload = true;

            this->artist = tmpArtist;
            this->title = tmpTitle;
            this->label_mainTitle->setText(this->title);

            // init
            this->list_track->clear();
            this->jsonArr_tracks_toPlay = QJsonArray();
            this->jsonArr_tracks_toPlay = ProcJsonEasy::getJsonArray(jsonObj, "data");

            this->track_id_fav = 0;
            this->track_idx_fav = 0;
            this->track_star_fav = 0;

            this->track_totalCount = 0;
            this->track_drawCount = 0;
            this->track_favoriteOffset = 0;

            this->flag_draw = false;
            this->flag_check_track = false;
            this->flag_track_fav = false;
            this->flag_send_track = false;

            // request HTTP API
            this->flagReqMore_track = false;
            this->flag_lastPage_track = false;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
        }
    }


    /**
     * @brief TrackListAll::setActivePage
     */
    void ArtistTrackListAll::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            this->box_filter->removeWidget(this->widget_filter_contents);

            if(this->flag_widget_visible == true){
                this->flag_widget_visible = false;

                this->widget_main_contents->hide();
                this->box_contents->removeWidget(this->widget_main_contents);

                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);
            }

            this->widget_filter_contents = new QWidget();
            this->widget_filter_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_filter_contents->setFixedSize(1500, 50);

            int width = 0;
            int left = 0;
            // All
            QPixmap pixmap_play(":/images/icon_list_allplay.png");
            QPushButton *btn_playAll = new QPushButton(this->widget_filter_contents);
            btn_playAll->setIcon(pixmap_play);
            btn_playAll->setIconSize(pixmap_play.rect().size());
            btn_playAll->setText("Play All");
            btn_playAll->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            btn_playAll->setCursor(Qt::PointingHandCursor);

            width = btn_playAll->sizeHint().width();
            btn_playAll->setGeometry(left, 0, width, 50);
            connect(btn_playAll, &QPushButton::clicked, this, &ArtistTrackListAll::slot_btnClicked_playAll);

            // shuffle
            QPixmap pixmap_shuffle(":/images/icon_list_shuffle.png");
            QPushButton *btn_shuffleAll = new QPushButton(this->widget_filter_contents);
            btn_shuffleAll->setIcon(pixmap_shuffle);
            btn_shuffleAll->setIconSize(pixmap_shuffle.rect().size());
            btn_shuffleAll->setText("Shuffle Play");
            btn_shuffleAll->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            btn_shuffleAll->setCursor(Qt::PointingHandCursor);

            left = width + 30;
            width = btn_shuffleAll->sizeHint().width();
            btn_shuffleAll->setGeometry(left, 0, width, 50);
            connect(btn_shuffleAll, &QPushButton::clicked, this, &ArtistTrackListAll::slot_btnClicked_playShuffle);

            this->box_filter->addWidget(this->widget_filter_contents);

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);

            this->vBox_tracks = new QVBoxLayout();
            this->vBox_tracks->setSpacing(0);
            this->vBox_tracks->setContentsMargins(0, 0, 0, 0);
            GSCommon::clearLayout(this->vBox_tracks);

            this->box_main_contents->addLayout(this->vBox_tracks);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            this->setDataTrackFromDB();
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void ArtistTrackListAll::setUIControl_tracks(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Tracks");
    }


    void ArtistTrackListAll::setDataTrackFromDB(){

        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){

            QString strQuery = "";
            strQuery = " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art , A.mime_type, A.samplerate, A.bitdepth ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id";
            if(this->artist.front() == "'" || this->artist.back() == "'"){
                strQuery += " WHERE A.artist_id=(SELECT artist_id from artists where artist= '\"%1\"') ORDER BY A.track ";
            }
            else{
                strQuery += " WHERE A.artist_id=(SELECT artist_id from artists where artist='%1') ORDER BY A.track ";
            }

            QVariantList data;
            sqlite->exec(strQuery.arg(this->artist), data);

            QJsonArray tmpArr;
            if(data.size() > 0){
                for(int i = 0 ; i < data.size(); i++){
                    QJsonObject tmp_data = data.at(i).toJsonObject();
                    tmpArr.append(tmp_data);
                }
            }
            this->jsonArr_tracks_toPlay = tmpArr;
        }

        sqlite->close();
        delete sqlite;

        this->track_svr_idx = 0;
        QList<roseHome::TrackItemData> tmp_list_track;
        this->slot_applyResult_tracks(tmp_list_track, this->jsonArr_tracks_toPlay, true);
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void ArtistTrackListAll::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->flag_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------


    void ArtistTrackListAll::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > 100 ) ? 100 : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &ArtistTrackListAll::slot_clickedItemTrack_inList);
            this->track_listAll[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->track_listAll[i]);
        }

        ContentLoadingwaitingMsgHide();

        for(int i = start_index; i < this->track_drawCount; i++){
            QJsonObject tmpData = this->jsonArr_tracks_toPlay.at(i).toObject();
            tmpData.insert("favorite", false);

            this->track_listAll[i]->setDataTrackInfo_Music(tmpData);
            //this->track_listAll[i]->setDataTrackInfo_Music(this->jsonArr_tracks_toPlay.at(i).toObject());

            QCoreApplication::processEvents();
        }

        /*if(this->track_svr_idx == start_index){
            QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(this->track_svr_idx).toObject();

            QJsonObject data = QJsonObject();
            data.insert("macAddress", global.device.getDeviceID());
            data.insert("trackId", 0);

            QJsonObject track = QJsonObject();
            track.insert("data", data);
            track.insert("playUrl", ProcJsonEasy::getString(trackInfo, "data"));
            track.insert("type", "MUSIC");

            QJsonArray tracks = QJsonArray();
            tracks.append(track);

            // request HTTP API - track check for Rose Server
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &SearchTrackListAll::slot_applyResult_checkTrackId);
            proc->request_rose_checkRating_Track("MUSIC", tracks);
        }*/

        if(this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);
        }

        this->flag_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] Track 데이터를 받아서 처리함
     * @param list_data
     */
    void ArtistTrackListAll::slot_applyResult_tracks(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(list_data);

        if(jsonArr_dataToPlay.size() > 0){
            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            int start_index = this->list_track->count();
            this->track_totalCount = this->jsonArr_tracks_toPlay.size();

            if(start_index == 0){
                int max_cnt = (this->jsonArr_tracks_toPlay.size() > 100) ? 100 : this->jsonArr_tracks_toPlay.size();
                this->track_drawCount = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV();
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &ArtistTrackListAll::slot_clickedItemTrack_inList);
                    this->track_listAll[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->track_listAll[i]);
                }

                ContentLoadingwaitingMsgHide();

                for(int i = start_index; i < max_cnt; i++){
                    QJsonObject tmpData = this->jsonArr_tracks_toPlay.at(i).toObject();
                    tmpData.insert("favorite", false);

                    this->track_listAll[i]->setDataTrackInfo_Music(tmpData);
                    //this->track_listAll[i]->setDataTrackInfo_Music(this->jsonArr_tracks_toPlay.at(i).toObject());

                    QCoreApplication::processEvents();
                }

                /*if(this->track_svr_idx == 0){
                    QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(this->track_svr_idx).toObject();

                    QJsonObject data = QJsonObject();
                    data.insert("macAddress", global.device.getDeviceID());
                    data.insert("trackId", 0);

                    QJsonObject track = QJsonObject();
                    track.insert("data", data);
                    track.insert("playUrl", ProcJsonEasy::getString(trackInfo, "data"));
                    track.insert("type", "MUSIC");

                    QJsonArray tracks = QJsonArray();
                    tracks.append(track);

                    // request HTTP API - track check for Rose Server
                    roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                    connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &SearchTrackListAll::slot_applyResult_checkTrackId);
                    proc->request_rose_checkRating_Track("MUSIC", tracks);
                }*/

                this->flag_widget_visible = true;

                if(this->track_totalCount == this->track_drawCount){
                    this->box_main_contents->addSpacing(30);
                }
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void ArtistTrackListAll::slot_applyResult_checkTrackId(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk") && jsonObj.value("flagOk").toBool()){

            if(this->track_svr_idx < this->track_drawCount){

                int id = ProcJsonEasy::getInt(jsonObj, "id");

                QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(this->track_svr_idx).toObject();

                //qDebug() << "MusicSearch::slot_applyResult_checkTrackId -> Index : " << this->track_svr_idx << ", rcv ID : " << id;
                if(id > 0){
                    QString track_id = QString("%1").arg(ProcJsonEasy::getInt(trackInfo, "id"));
                    QString playUrl = ProcJsonEasy::getString(trackInfo, "data");

                    QJsonObject data;
                    data.insert("macAddress", global.device.getDeviceID());
                    data.insert("playUrl", playUrl);
                    data.insert("clientKey", track_id);

                    QJsonObject tmpjson;
                    tmpjson.insert("id", id);
                    tmpjson.insert("favorite", false);
                    tmpjson.insert("playUrl", playUrl);
                    tmpjson.insert("clientKey", track_id);
                    tmpjson.insert("data", data);

                    QJsonObject track;
                    track.insert("track", tmpjson);

                    roseHome::ProcCommon *proc_rating = new roseHome::ProcCommon(this);
                    connect(proc_rating, &roseHome::ProcCommon::completeReq_rating_track, this, &ArtistTrackListAll::slot_applyResult_getTrckRating);
                    proc_rating->request_rose_getRating_Track("MUSIC", track);
                }
                else{
                    this->track_svr_idx++;

                    trackInfo = this->jsonArr_tracks_toPlay.at(this->track_svr_idx).toObject();

                    QJsonObject data = QJsonObject();
                    data.insert("macAddress", global.device.getDeviceID());
                    data.insert("trackId", 0);

                    QJsonObject track = QJsonObject();
                    track.insert("data", data);
                    track.insert("playUrl", ProcJsonEasy::getString(trackInfo, "data"));
                    track.insert("type", "MUSIC");

                    //QJsonDocument doc(track);
                    //QString strJson(doc.toJson(QJsonDocument::Compact));
                    //qDebug() << "MusicSearch::slot_applyResult_checkTrackId -> " << "track = " << strJson;

                    QJsonArray tracks = QJsonArray();
                    tracks.append(track);

                    // request HTTP API - track check for Rose Server
                    roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                    connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &ArtistTrackListAll::slot_applyResult_checkTrackId);
                    proc->request_rose_checkRating_Track("MUSIC", tracks);
                }
            }
        }
    }


    void ArtistTrackListAll::slot_applyResult_getTrckRating(const QJsonArray &jsonObj){

        QJsonObject rcvJson = jsonObj.at(0).toObject();

        bool flag_fav = ProcJsonEasy::getBool(rcvJson, "favorite");
        int star_fav = ProcJsonEasy::getInt(rcvJson, "star");

        this->track_listAll[this->track_svr_idx]->setFavoritesIds(flag_fav, star_fav);

        this->track_svr_idx++;

        QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(this->track_svr_idx).toObject();

        QJsonObject data = QJsonObject();
        data.insert("macAddress", global.device.getDeviceID());
        data.insert("trackId", 0);

        QJsonObject track = QJsonObject();
        track.insert("data", data);
        track.insert("playUrl", ProcJsonEasy::getString(trackInfo, "data"));
        track.insert("type", "MUSIC");

        //QJsonDocument doc(track);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "MusicSearch::slot_applyResult_getTrckRating -> " << "track = " << strJson;

        QJsonArray tracks = QJsonArray();
        tracks.append(track);

        // request HTTP API - track check for Rose Server
        roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
        connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &ArtistTrackListAll::slot_applyResult_checkTrackId);
        proc->request_rose_checkRating_Track("MUSIC", tracks);
    }


    void ArtistTrackListAll::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        //QJsonDocument doc(jsonObj);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "jsonObj = " << strJson;

        int id = ProcJsonEasy::getInt(jsonObj, "id");
        QJsonObject trackData = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();

        if(id > 0){
            QJsonObject ratingInfo = QJsonObject();
            ratingInfo.insert("favorite", this->flag_track_fav);
            ratingInfo.insert("star", this->track_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "MUSIC");

            QJsonObject track = QJsonObject();
            track.insert("duration", 0);
            track.insert("favorite", false);
            track.insert("id", id);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 0);
            track.insert("type", "MUSIC");

            QJsonObject jsonData = QJsonObject();
            jsonData.insert("ratingInfo", ratingInfo);
            jsonData.insert("track", track);

            // request HTTP API - track check for Rose Server
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_rating_track, this, &ArtistTrackListAll::slot_applyResult_getRating_track);
            proc->request_rose_setRating_Track(jsonData, this->flag_track_fav, this->track_star_fav);
        }
        else{
            QJsonObject data = QJsonObject();
            data.insert("macAddress", global.device.getDeviceID());
            data.insert("trackId", 0);

            QJsonArray albums = QJsonArray();
            albums.append(ProcJsonEasy::getString(trackData, "album"));

            QJsonArray artists = QJsonArray();
            artists.append(ProcJsonEasy::getString(trackData, "artist"));

            QJsonObject track = QJsonObject();
            track.insert("albums", albums);
            track.insert("artists", artists);
            track.insert("clientKey", QString("%1").arg(ProcJsonEasy::getInt(trackData, "id")));
            track.insert("data", data);
            track.insert("duration", ProcJsonEasy::getInt(trackData, "duration") / 1000);
            track.insert("favorite", this->flag_track_fav);
            track.insert("ownerId", 0);
            track.insert("playUrl", ProcJsonEasy::getString(trackData, "data"));
            track.insert("sort", 0);
            track.insert("star", this->track_star_fav);
            track.insert("thumbnailUrl", ProcJsonEasy::getString(trackData, "album_art"));
            track.insert("title", ProcJsonEasy::getString(trackData, "title"));
            track.insert("type", "MUSIC");

            QJsonArray tracks = QJsonArray();
            tracks.append(track);

            QJsonObject json = QJsonObject();
            json.insert("tracks", tracks);

            // request HTTP API - track check for Rose Server
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &ArtistTrackListAll::slot_applyResult_addRating_track);
            proc->request_rose_addRating_Track(json);
        }
    }


    void ArtistTrackListAll::slot_applyResult_getRating_track(const QJsonArray &jsonObj){

        //QJsonDocument doc(jsonObj);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "jsonObj = " << strJson;

        QJsonObject rcvJson = jsonObj.at(0).toObject();

        if(rcvJson.contains("flagOk") && rcvJson.value("flagOk").toBool()){
            this->track_listAll[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
        }

        if(this->flag_check_track == true){
            this->flag_check_track = false;
        }
    }


    void ArtistTrackListAll::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk") && jsonObj.value("flagOk").toBool()){
            this->track_listAll[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
        }

        if(this->flag_check_track == true){
            this->flag_check_track = false;
        }
    }


    void ArtistTrackListAll::slot_btnClicked_playAll(){

        // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        QJsonObject tmp_json;
        tmp_json.insert("musicPlayType", 15);
        tmp_json.insert("music", this->jsonArr_tracks_toPlay);
        tmp_json.insert("shuffle", 0);
        tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

        if(global.Queue_track_count != 0) {
            emit this->linker->signal_checkQueue(27, "");

            return;
        }
        emit this->linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += this->jsonArr_tracks_toPlay.count();     // 220419 queue count

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_NETWORK_PLAY
                         , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song")
                         , tmp_json
                         , true
                         , true);
    }


    void ArtistTrackListAll::slot_btnClicked_playShuffle(){

        // Track shuffle 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        QJsonObject tmp_json;
        tmp_json.insert("musicPlayType", 15);
        tmp_json.insert("music", this->jsonArr_tracks_toPlay);
        tmp_json.insert("shuffle", 1);
        tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

        if(global.Queue_track_count != 0) {
            emit linker->signal_checkQueue(27, "");

            return;
        }
        emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += this->jsonArr_tracks_toPlay.count();     // 220419 queue count

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_NETWORK_PLAY
                         , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song")
                         , tmp_json
                         , true
                         , true);
    }


    /**
     * @brief [Slot] PlaylistTrackDetailInfo_RHV signal에 대하여 처리
     * @param clickMode
     */
    void ArtistTrackListAll::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->track_listAll[idx]->getFavoritesStars();
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

                QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QJsonObject data = QJsonObject();
                data.insert("macAddress", global.device.getDeviceID());
                data.insert("trackId", 0);

                QJsonObject track = QJsonObject();
                track.insert("data", data);
                track.insert("playUrl", ProcJsonEasy::getString(trackInfo, "data"));
                track.insert("type", "MUSIC");

                QJsonArray tracks = QJsonArray();
                tracks.append(track);

                // request HTTP API - track check for Rose Server
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &ArtistTrackListAll::slot_applyResult_checkRating_track);
                proc->request_rose_checkRating_Track("MUSIC", tracks);

                this->flag_check_track = true;
            }
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

                QJsonArray jsonArr_tracks = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_tracks_toPlay, idx, curr_playType);

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
                QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

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

    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void ArtistTrackListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

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

                QJsonArray jsonArr_tracks = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_tracks_toPlay, index, clickMode);

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
    }


    int ArtistTrackListAll::get_rose_playType(OptMorePopup::ClickMode clickMode){

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


    QJsonArray ArtistTrackListAll::get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode){

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
    QJsonArray ArtistTrackListAll::subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex){

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
    QJsonArray ArtistTrackListAll::reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex){

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





    /**
     * @brief "ROSEHOME > REcENT TRACK" 화면을 위한 생성자. @see PAGECODE_RH_TRACK_LIST_ALL_VIEW
     * @param parent
     */
    SearchTrackListAll::SearchTrackListAll(QWidget *parent) : AbstractRoseHomeSubWidget(VerticalScroll_rosefilter, parent) {

        this->linker = Linker::getInstance();

        // data
        this->list_track = new QList<roseHome::TrackItemData>();

        // UI
        this->setUIControl_tracks();
    }


    /**
    * @brief 소멸자.
    */
    SearchTrackListAll::~SearchTrackListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void SearchTrackListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString tmpTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(this->title != tmpTitle){
            this->flagNeedReload = true;

            this->title = tmpTitle;

            this->label_mainTitle->setText(this->title);

            // init
            this->list_track->clear();
            this->jsonArr_tracks_toPlay = QJsonArray();
            this->jsonArr_tracks_toPlay = ProcJsonEasy::getJsonArray(jsonObj, "data");

            this->track_id_fav = 0;
            this->track_idx_fav = 0;
            this->track_star_fav = 0;

            this->track_totalCount = 0;
            this->track_drawCount = 0;
            this->track_favoriteOffset = 0;

            this->flag_draw = false;
            this->flag_check_track = false;
            this->flag_track_fav = false;
            this->flag_send_track = false;

            // request HTTP API
            this->flagReqMore_track = false;
            this->flag_lastPage_track = false;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
        }
    }


    /**
     * @brief TrackListAll::setActivePage
     */
    void SearchTrackListAll::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            this->box_filter->removeWidget(this->widget_filter_contents);

            this->widget_filter_contents = new QWidget();
            this->widget_filter_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_filter_contents->setFixedSize(1500, 50);

            int width = 0;
            int left = 0;
            // All
            QPixmap pixmap_play(":/images/icon_list_allplay.png");
            QPushButton *btn_playAll = new QPushButton(this->widget_filter_contents);
            btn_playAll->setIcon(pixmap_play);
            btn_playAll->setIconSize(pixmap_play.rect().size());
            btn_playAll->setText("Play All");
            btn_playAll->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            btn_playAll->setCursor(Qt::PointingHandCursor);

            width = btn_playAll->sizeHint().width();
            btn_playAll->setGeometry(left, 0, width, 50);
            connect(btn_playAll, &QPushButton::clicked, this, &SearchTrackListAll::slot_btnClicked_playAll);

            // shuffle
            QPixmap pixmap_shuffle(":/images/icon_list_shuffle.png");
            QPushButton *btn_shuffleAll = new QPushButton(this->widget_filter_contents);
            btn_shuffleAll->setIcon(pixmap_shuffle);
            btn_shuffleAll->setIconSize(pixmap_shuffle.rect().size());
            btn_shuffleAll->setText("Shuffle Play");
            btn_shuffleAll->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            btn_shuffleAll->setCursor(Qt::PointingHandCursor);

            left = width + 30;
            width = btn_shuffleAll->sizeHint().width();
            btn_shuffleAll->setGeometry(left, 0, width, 50);
            connect(btn_shuffleAll, &QPushButton::clicked, this, &SearchTrackListAll::slot_btnClicked_playShuffle);

            this->box_filter->addWidget(this->widget_filter_contents);

            this->box_contents->removeWidget(this->widget_main_contents);

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);

            this->vBox_tracks = new QVBoxLayout();
            this->vBox_tracks->setSpacing(0);
            this->vBox_tracks->setContentsMargins(0, 0, 0, 0);
            GSCommon::clearLayout(this->vBox_tracks);

            this->box_main_contents->addLayout(this->vBox_tracks);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            this->track_svr_idx = 0;
            QList<roseHome::TrackItemData> tmp_list_track;
            this->slot_applyResult_tracks(tmp_list_track, this->jsonArr_tracks_toPlay, true);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void SearchTrackListAll::setUIControl_tracks(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Tracks");
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void SearchTrackListAll::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->flag_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------


    void SearchTrackListAll::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > 100 ) ? 100 : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &SearchTrackListAll::slot_clickedItemTrack_inList);
            this->track_listAll[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->track_listAll[i]);
        }

        for(int i = start_index; i < this->track_drawCount; i++){
            QJsonObject tmpData = this->jsonArr_tracks_toPlay.at(i).toObject();
            tmpData.insert("favorite", false);

            this->track_listAll[i]->setDataTrackInfo_Music(tmpData);
            //this->track_listAll[i]->setDataTrackInfo_Music(this->jsonArr_tracks_toPlay.at(i).toObject());

            QCoreApplication::processEvents();
        }

        /*if(this->track_svr_idx == start_index){
            QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(this->track_svr_idx).toObject();

            QJsonObject data = QJsonObject();
            data.insert("macAddress", global.device.getDeviceID());
            data.insert("trackId", 0);

            QJsonObject track = QJsonObject();
            track.insert("data", data);
            track.insert("playUrl", ProcJsonEasy::getString(trackInfo, "data"));
            track.insert("type", "MUSIC");

            QJsonArray tracks = QJsonArray();
            tracks.append(track);

            // request HTTP API - track check for Rose Server
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &SearchTrackListAll::slot_applyResult_checkTrackId);
            proc->request_rose_checkRating_Track("MUSIC", tracks);
        }*/

        if(this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] Track 데이터를 받아서 처리함
     * @param list_data
     */
    void SearchTrackListAll::slot_applyResult_tracks(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(list_data);

        if(jsonArr_dataToPlay.size() > 0){
            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            int start_index = this->list_track->count();
            this->track_totalCount = this->jsonArr_tracks_toPlay.size();

            if(start_index == 0){
                int max_cnt = (this->jsonArr_tracks_toPlay.size() > 100) ? 100 : this->jsonArr_tracks_toPlay.size();
                this->track_drawCount = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV();
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &SearchTrackListAll::slot_clickedItemTrack_inList);
                    this->track_listAll[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->track_listAll[i]);
                }

                for(int i = start_index; i < max_cnt; i++){
                    QJsonObject tmpData = this->jsonArr_tracks_toPlay.at(i).toObject();
                    tmpData.insert("favorite", false);

                    this->track_listAll[i]->setDataTrackInfo_Music(tmpData);
                    //this->track_listAll[i]->setDataTrackInfo_Music(this->jsonArr_tracks_toPlay.at(i).toObject());

                    QCoreApplication::processEvents();
                }

                /*if(this->track_svr_idx == 0){
                    QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(this->track_svr_idx).toObject();

                    QJsonObject data = QJsonObject();
                    data.insert("macAddress", global.device.getDeviceID());
                    data.insert("trackId", 0);

                    QJsonObject track = QJsonObject();
                    track.insert("data", data);
                    track.insert("playUrl", ProcJsonEasy::getString(trackInfo, "data"));
                    track.insert("type", "MUSIC");

                    QJsonArray tracks = QJsonArray();
                    tracks.append(track);

                    // request HTTP API - track check for Rose Server
                    roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                    connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &SearchTrackListAll::slot_applyResult_checkTrackId);
                    proc->request_rose_checkRating_Track("MUSIC", tracks);
                }*/

                ContentLoadingwaitingMsgHide();
                this->flag_widget_visible = true;

                if(this->track_totalCount <= this->list_track->count()){
                    this->box_main_contents->addSpacing(30);
                }
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void SearchTrackListAll::slot_applyResult_checkTrackId(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk") && jsonObj.value("flagOk").toBool()){

            if(this->track_svr_idx < this->track_drawCount){

                int id = ProcJsonEasy::getInt(jsonObj, "id");

                QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(this->track_svr_idx).toObject();

                //qDebug() << "MusicSearch::slot_applyResult_checkTrackId -> Index : " << this->track_svr_idx << ", rcv ID : " << id;
                if(id > 0){
                    QString track_id = QString("%1").arg(ProcJsonEasy::getInt(trackInfo, "id"));
                    QString playUrl = ProcJsonEasy::getString(trackInfo, "data");

                    QJsonObject data;
                    data.insert("macAddress", global.device.getDeviceID());
                    data.insert("playUrl", playUrl);
                    data.insert("clientKey", track_id);

                    QJsonObject tmpjson;
                    tmpjson.insert("id", id);
                    tmpjson.insert("favorite", false);
                    tmpjson.insert("playUrl", playUrl);
                    tmpjson.insert("clientKey", track_id);
                    tmpjson.insert("data", data);

                    QJsonObject track;
                    track.insert("track", tmpjson);

                    roseHome::ProcCommon *proc_rating = new roseHome::ProcCommon(this);
                    connect(proc_rating, &roseHome::ProcCommon::completeReq_rating_track, this, &SearchTrackListAll::slot_applyResult_getTrckRating);
                    proc_rating->request_rose_getRating_Track("MUSIC", track);
                }
                else{
                    this->track_svr_idx++;

                    trackInfo = this->jsonArr_tracks_toPlay.at(this->track_svr_idx).toObject();

                    QJsonObject data = QJsonObject();
                    data.insert("macAddress", global.device.getDeviceID());
                    data.insert("trackId", 0);

                    QJsonObject track = QJsonObject();
                    track.insert("data", data);
                    track.insert("playUrl", ProcJsonEasy::getString(trackInfo, "data"));
                    track.insert("type", "MUSIC");

                    //QJsonDocument doc(track);
                    //QString strJson(doc.toJson(QJsonDocument::Compact));
                    //qDebug() << "MusicSearch::slot_applyResult_checkTrackId -> " << "track = " << strJson;

                    QJsonArray tracks = QJsonArray();
                    tracks.append(track);

                    // request HTTP API - track check for Rose Server
                    roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                    connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &SearchTrackListAll::slot_applyResult_checkTrackId);
                    proc->request_rose_checkRating_Track("MUSIC", tracks);
                }
            }
        }
    }


    void SearchTrackListAll::slot_applyResult_getTrckRating(const QJsonArray &jsonObj){

        QJsonObject rcvJson = jsonObj.at(0).toObject();

        bool flag_fav = ProcJsonEasy::getBool(rcvJson, "favorite");
        int star_fav = ProcJsonEasy::getInt(rcvJson, "star");

        this->track_listAll[this->track_svr_idx]->setFavoritesIds(flag_fav, star_fav);

        this->track_svr_idx++;

        QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(this->track_svr_idx).toObject();

        QJsonObject data = QJsonObject();
        data.insert("macAddress", global.device.getDeviceID());
        data.insert("trackId", 0);

        QJsonObject track = QJsonObject();
        track.insert("data", data);
        track.insert("playUrl", ProcJsonEasy::getString(trackInfo, "data"));
        track.insert("type", "MUSIC");

        //QJsonDocument doc(track);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "MusicSearch::slot_applyResult_getTrckRating -> " << "track = " << strJson;

        QJsonArray tracks = QJsonArray();
        tracks.append(track);

        // request HTTP API - track check for Rose Server
        roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
        connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &SearchTrackListAll::slot_applyResult_checkTrackId);
        proc->request_rose_checkRating_Track("MUSIC", tracks);
    }


    void SearchTrackListAll::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        //QJsonDocument doc(jsonObj);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "jsonObj = " << strJson;

        int id = ProcJsonEasy::getInt(jsonObj, "id");
        QJsonObject trackData = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();

        if(id > 0){
            QJsonObject ratingInfo = QJsonObject();
            ratingInfo.insert("favorite", this->flag_track_fav);
            ratingInfo.insert("star", this->track_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "MUSIC");

            QJsonObject track = QJsonObject();
            track.insert("duration", 0);
            track.insert("favorite", false);
            track.insert("id", id);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 0);
            track.insert("type", "MUSIC");

            QJsonObject jsonData = QJsonObject();
            jsonData.insert("ratingInfo", ratingInfo);
            jsonData.insert("track", track);

            // request HTTP API - track check for Rose Server
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_rating_track, this, &SearchTrackListAll::slot_applyResult_getRating_track);
            proc->request_rose_setRating_Track(jsonData, this->flag_track_fav, this->track_star_fav);
        }
        else{
            QJsonObject data = QJsonObject();
            data.insert("macAddress", global.device.getDeviceID());
            data.insert("trackId", 0);

            QJsonArray albums = QJsonArray();
            albums.append(ProcJsonEasy::getString(trackData, "album"));

            QJsonArray artists = QJsonArray();
            artists.append(ProcJsonEasy::getString(trackData, "artist"));

            QJsonObject track = QJsonObject();
            track.insert("albums", albums);
            track.insert("artists", artists);
            track.insert("clientKey", QString("%1").arg(ProcJsonEasy::getInt(trackData, "id")));
            track.insert("data", data);
            track.insert("duration", ProcJsonEasy::getInt(trackData, "duration") / 1000);
            track.insert("favorite", this->flag_track_fav);
            track.insert("ownerId", 0);
            track.insert("playUrl", ProcJsonEasy::getString(trackData, "data"));
            track.insert("sort", 0);
            track.insert("star", this->track_star_fav);
            track.insert("thumbnailUrl", ProcJsonEasy::getString(trackData, "album_art"));
            track.insert("title", ProcJsonEasy::getString(trackData, "title"));
            track.insert("type", "MUSIC");

            QJsonArray tracks = QJsonArray();
            tracks.append(track);

            QJsonObject json = QJsonObject();
            json.insert("tracks", tracks);

            // request HTTP API - track check for Rose Server
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &SearchTrackListAll::slot_applyResult_addRating_track);
            proc->request_rose_addRating_Track(json);
        }
    }


    void SearchTrackListAll::slot_applyResult_getRating_track(const QJsonArray &jsonObj){

        //QJsonDocument doc(jsonObj);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "jsonObj = " << strJson;

        QJsonObject rcvJson = jsonObj.at(0).toObject();

        if(rcvJson.contains("flagOk") && rcvJson.value("flagOk").toBool()){
            this->track_listAll[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
        }

        if(this->flag_check_track == true){
            this->flag_check_track = false;
        }
    }


    void SearchTrackListAll::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk") && jsonObj.value("flagOk").toBool()){
            this->track_listAll[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
        }

        if(this->flag_check_track == true){
            this->flag_check_track = false;
        }
    }


    void SearchTrackListAll::slot_btnClicked_playAll(){

        // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        QJsonObject tmp_json;
        tmp_json.insert("musicPlayType", 15);
        tmp_json.insert("music", this->jsonArr_tracks_toPlay);
        tmp_json.insert("shuffle", 0);
        tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

        if(global.Queue_track_count != 0) {
            emit this->linker->signal_checkQueue(27, "");

            return;
        }
        emit this->linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += this->jsonArr_tracks_toPlay.count();     // 220419 queue count

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_NETWORK_PLAY
                         , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song")
                         , tmp_json
                         , true
                         , true);
    }


    void SearchTrackListAll::slot_btnClicked_playShuffle(){

        // Track shuffle 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        QJsonObject tmp_json;
        tmp_json.insert("musicPlayType", 15);
        tmp_json.insert("music", this->jsonArr_tracks_toPlay);
        tmp_json.insert("shuffle", 1);
        tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

        if(global.Queue_track_count != 0) {
            emit linker->signal_checkQueue(27, "");

            return;
        }
        emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += this->jsonArr_tracks_toPlay.count();     // 220419 queue count

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_NETWORK_PLAY
                         , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song")
                         , tmp_json
                         , true
                         , true);
    }


    /**
     * @brief [Slot] PlaylistTrackDetailInfo_RHV signal에 대하여 처리
     * @param clickMode
     */
    void SearchTrackListAll::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->track_listAll[idx]->getFavoritesStars();
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

                QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QJsonObject data = QJsonObject();
                data.insert("macAddress", global.device.getDeviceID());
                data.insert("trackId", 0);

                QJsonObject track = QJsonObject();
                track.insert("data", data);
                track.insert("playUrl", ProcJsonEasy::getString(trackInfo, "data"));
                track.insert("type", "MUSIC");

                QJsonArray tracks = QJsonArray();
                tracks.append(track);

                // request HTTP API - track check for Rose Server
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeCheck_rating_track, this, &SearchTrackListAll::slot_applyResult_checkRating_track);
                proc->request_rose_checkRating_Track("MUSIC", tracks);

                this->flag_check_track = true;
            }
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

                QJsonArray jsonArr_tracks = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_tracks_toPlay, idx, curr_playType);

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
                QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

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

    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void SearchTrackListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

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

                QJsonArray jsonArr_tracks = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_tracks_toPlay, index, clickMode);

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
    }


    int SearchTrackListAll::get_rose_playType(OptMorePopup::ClickMode clickMode){

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


    QJsonArray SearchTrackListAll::get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode){


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
    QJsonArray SearchTrackListAll::subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex){

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
    QJsonArray SearchTrackListAll::reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex){

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
}
