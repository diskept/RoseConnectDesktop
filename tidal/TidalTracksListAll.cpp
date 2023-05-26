#include "TidalTracksListAll.h"

#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"
#include "tidal/ProcRosePlay_withTidal.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "common/rosesettings.h"

#include "widget/NoData_Widget.h"

#include <QSettings>
#include <QScrollBar>
#include <QResizeEvent>
#include <QPixmapCache>

namespace tidal {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___tracks = 1;

    const int GET_ITEM_SIZE___ONCE = 50;


    /**
     * @brief
     * @param parent
     */
    TidalTracksListAll::TidalTracksListAll(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_rosefilter, parent) {
        // data
        this->list_track = new QList<tidal::TrackItemData>();
        this->list_filterOpt_sort = AbstractTidalSubWidget::get_list_filterOptData_forMyCollection();

        // UI
        this->setUIControl_tracks();
    }

    /**
     * @brief 소멸자.
     */
    TidalTracksListAll::~TidalTracksListAll(){

        this->deleteLater();
    }


    void TidalTracksListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        tidal::PageInfo_TrackAllView data_param = ConvertData::convertData_pageInfo_trackAllView(jsonObj);
        QString tmp_current = this->api_subPath;

        this->flagNeedReload = false;

        if(tmp_current != data_param.api_subPath){
            this->flagNeedReload = true;

            if(this->flag_widget_visible == true){
                this->widget_main_contents->hide();
                this->flag_widget_visible = false;
            }

            this->label_mainTitle->setText(data_param.pathTitle);

            this->api_subPath = data_param.api_subPath;

            // init
            this->list_track->clear();
            this->jsonArr_tracks_toPlay = QJsonArray();

            this->track_id_fav = 0;
            this->track_idx_fav = 0;
            this->track_star_fav = 0;

            this->track_totalCount = 0;
            this->track_drawCount = 0;
            this->track_favoriteOffset = 0;
            this->before_scrollMax = 0;

            this->flag_draw = false;
            this->flag_check_track = false;
            this->flag_track_fav = false;
            this->flag_send_track = false;

            // request HTTP API
            this->flagReqMore_track = false;
            this->flag_lastPage_track = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }

    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalTracksListAll::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            /*// init
            this->chooseFilterOpt->hideFilterBox();

            // init Data
            this->selected_filterCode = this->list_filterOpt_sort.first().opt_code;         // default

            QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
            QString def_name = this->list_filterOpt_sort.first().opt_name;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(def_code, def_name);
            */

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
            connect(btn_playAll, &QPushButton::clicked, this, &TidalTracksListAll::slot_btnClicked_playAll);

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
            connect(btn_shuffleAll, &QPushButton::clicked, this, &TidalTracksListAll::slot_btnClicked_playShuffle);

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
        }
    }

    // MARK : Create UI and Set --------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void TidalTracksListAll::setUIControl_tracks(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Tracks");

        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // filtering
        //this->setUiControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &TidalTracksListAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyAlbumAll::setUiControl_filter
     */
    void TidalTracksListAll::setUiControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &TidalTracksListAll::slot_filterClicked);


        QHBoxLayout *hBox_filter = new QHBoxLayout();
        hBox_filter->setSpacing(0);
        hBox_filter->setContentsMargins(0,0,0,0);
        hBox_filter->addWidget(this->filterWidget, 0, Qt::AlignVCenter | Qt::AlignLeft);
        hBox_filter->addStretch(1);
        hBox_filter->addSpacing(77);

        QWidget *tmp_widget_forFilter = new QWidget();
        tmp_widget_forFilter->setContentsMargins(0,0,0,0);
        tmp_widget_forFilter->setObjectName("widget_forFilter");
        tmp_widget_forFilter->setStyleSheet("#widget_forFilter { background-color:#171717; }");
        tmp_widget_forFilter->setLayout(hBox_filter);
        tmp_widget_forFilter->setFixedHeight(70);

        this->box_mainTitle->addWidget(tmp_widget_forFilter);
        this->addUIControl_dividedLine_onMain();
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void TidalTracksListAll::initAll(){

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

        this->widget_main_contents->hide();
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

        this->vBox_tracks = new QVBoxLayout();
        this->vBox_tracks->setSpacing(0);
        this->vBox_tracks->setContentsMargins(0, 0, 0, 0);
        GSCommon::clearLayout(this->vBox_tracks);

        this->box_main_contents->addLayout(this->vBox_tracks);


        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void TidalTracksListAll::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum()) && (this->before_scrollMax < this->scrollArea_main->verticalScrollBar()->maximum())){

            //qDebug() << "[Debug]" << __FUNCTION__ << " line::" << __LINE__ << this->scrollArea_main->verticalScrollBar()->value() << this->scrollArea_main->verticalScrollBar()->maximum() << this->before_scrollMax;

            this->flag_draw = true;
            this->before_scrollMax = this->scrollArea_main->verticalScrollBar()->maximum();

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void TidalTracksListAll::resizeEvent(QResizeEvent *event){

        AbstractTidalSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }
    }


    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void TidalTracksListAll::slot_filterClicked(){

        if(this->chooseFilterOpt->isVisible()){
            this->chooseFilterOpt->hideFilterBox();
        }
        else{
            // Filter Box Show
            this->chooseFilterOpt->clearFilter();
            this->chooseFilterOpt->addFilterGroup(tr("Sort"), this->list_filterOpt_sort, this->selected_filterCode);
            //this->chooseFilterOpt->addFilterGroup(tr("정렬"), this->list_filterOpt_sort, this->selected_filterCode);
            this->chooseFilterOpt->showFilterBox(QPoint(30, 130), this->size());
        }
    }

    /**
     * @brief 필터링 옵션박스 Widget에서 필터링을 하나 클릭한 경우이다.
     * @param selectedCode
     */
    void TidalTracksListAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

        // 코드가 다른 경우에 대해서만 처리한다.
        if(this->selected_filterCode != selected_filterCode){

            // update
            this->selected_filterCode = selected_filterCode;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(selected_filterCode.toString(), selected_filterName);

            // clear - 옵션코드가 바뀌었으므로 모두 초기화한다.
            this->initAll();

            this->scrollTop();
            this->request_more_trackData();

        }

    }

    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void TidalTracksListAll::request_more_trackData(){

        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // next_offset
            int next_offset = this->list_track->size();

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_tracks, this, &TidalTracksListAll::slot_applyResult_tracks);
            proc->request_tidal_getList_tracks(this->api_subPath, GET_ITEM_SIZE___ONCE, next_offset);

        }
    }

    void TidalTracksListAll::request_more_trackDraw(){

         qDebug() << "[Debug]" << __FUNCTION__ << " line::" << __LINE__;

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > GET_ITEM_SIZE___ONCE ) ? GET_ITEM_SIZE___ONCE : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TidalTracksListAll::slot_clickedItemTrack_inList);
            this->track_listAll[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->track_listAll[i]);
        }

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i]->setDataTrackInfo_Tidal(this->list_track->at(i));

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
    void TidalTracksListAll::slot_applyResult_tracks(const QList<tidal::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            if(list_data.size() == 0){
                ContentLoadingwaitingMsgHide();      //cheon Tidal

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
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TidalTracksListAll::slot_clickedItemTrack_inList);
                    this->track_listAll[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->track_listAll[i]);
                }

                QList<QString> tmp_clientkey;
                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i]->setDataTrackInfo_Tidal(this->list_track->at(i));
                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));

                    QCoreApplication::processEvents();
                }

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalTracksListAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("TIDAL", tmp_clientkey);

                if(this->track_totalCount > this->list_track->count()){
                    this->request_more_trackData();
                }
                else{
                    this->box_main_contents->addSpacing(30);
                }

                ContentLoadingwaitingMsgHide();
                this->flag_widget_visible = true;
            }
            else{
                if(this->track_totalCount > this->list_track->count()){
                    this->request_more_trackData();
                }
            }
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }

    void TidalTracksListAll::slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고.
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            // Tidal favorite check
            if(this->flag_send_track == true){
                QVariantList arr_myFavoriteIds = ProcJsonEasy::getJsonArray(p_jsonObj, "TRACK").toVariantList();
                bool status = arr_myFavoriteIds.contains(this->track_id_fav);

                if(status == true && this->flag_track_fav == false){
                    // Tidal track Favorite del
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalTracksListAll::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                else if(status == false && this->flag_track_fav == true){
                    // Tidal track Favorite add
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalTracksListAll::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                this->flag_send_track = false;
            }
            else{

            }
        }
    }

    void TidalTracksListAll::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

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
            connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalTracksListAll::slot_applyResult_addRating_track);
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
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalTracksListAll::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

        }
    }


    void TidalTracksListAll::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void TidalTracksListAll::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->track_listAll[this->track_favoriteOffset + i]->setFavoritesIds(flag, star);
                }

                this->track_favoriteOffset += 20;
            }
            else if(track_info.contains("message") && (track_info["message"].toString() == "정상")){
                this->track_listAll[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }


        }
        else{
            this->flag_check_track = false;
        }
    }


    void TidalTracksListAll::slot_btnClicked_playAll(){

        // 전체재생
        ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
        procRosePlay->requestPlayRose_byTracks(jsonArr_tracks_toPlay,0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::JustPlay);
    }


    void TidalTracksListAll::slot_btnClicked_playShuffle(){

        // 셔플재생
        ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
        procRosePlay->requestPlayRose_byTracks(jsonArr_tracks_toPlay,0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::ShufflePlay);
    }

    /**
     * @brief QobuzTracksListAll::slot_clickedItemTrack_inList
     * @param index
     * @param clickMode
     */
    void TidalTracksListAll::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

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
                connect(proc_fav, &ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalTracksListAll::slot_tidal_completeReq_listAll_myFavoritesIds);
                proc_fav->request_tidal_getAll_favorites();
                this->flag_send_track = true;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalTracksListAll::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("TIDAL", QString("%1").arg(this->track_id_fav));
                this->flag_check_track = true;

                this->track_listAll[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }
        }
        else{
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, SECTION_FOR_MORE_POPUP___tracks);
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void TidalTracksListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }

}
