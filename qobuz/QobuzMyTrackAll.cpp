#include "qobuz/QobuzMyTrackAll.h"

#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ProcCommon_forQobuz.h"
#include "qobuz/ProcRosePlay_withQobuz.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "common/rosesettings.h"

#include "widget/NoData_Widget.h"

#include <QSettings>
#include <QResizeEvent>
#include <QPixmapCache>
#include <QScrollBar>


namespace qobuz {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___tracks = 1;

    const int GET_ITEM_SIZE___ONCE = 100;


    /**
     * @brief
     * @param parent
     */
    QobuzMyTrackAll::QobuzMyTrackAll(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll_rosefilter, parent) {

        // data
        this->list_track = new QList<qobuz::TrackItemData>();
        this->list_filterOpt_sort = AbstractQobuzSubWidget::get_list_filterOptData_forMyCollection();

        // UI
        this->setUIControl_tracks();
    }


    /**
     * @brief 소멸자.
     */
    QobuzMyTrackAll::~QobuzMyTrackAll(){

        this->deleteLater();
    }


    /**
     * @brief
     * @param jsonObj
     */
    void QobuzMyTrackAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(this->page != pageCode){
            this->flagNeedReload = true;

            this->page = pageCode;

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
    void QobuzMyTrackAll::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

            /*// init
            this->chooseFilterOpt->hideFilterBox();

            // init Data
            this->selected_filterCode = this->list_filterOpt_sort.first().opt_code;         // default

            QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
            QString def_name = this->list_filterOpt_sort.first().opt_name;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(def_code, def_name);*/

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
            connect(btn_playAll, &QPushButton::clicked, this, &QobuzMyTrackAll::slot_btnClicked_playAll);

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
            connect(btn_shuffleAll, &QPushButton::clicked, this, &QobuzMyTrackAll::slot_btnClicked_playShuffle);

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
    void QobuzMyTrackAll::setUIControl_tracks(){

        this->addUIControl_mainTitle("Tracks");

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &QobuzMyTrackAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyAlbumAll::setUIControl_filter
     */
    void QobuzMyTrackAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &QobuzMyTrackAll::slot_filterClicked);


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
    void QobuzMyTrackAll::initAll(){

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
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void QobuzMyTrackAll::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)){

            this->flag_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void QobuzMyTrackAll::resizeEvent(QResizeEvent *event){

        AbstractQobuzSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }
    }


    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void QobuzMyTrackAll::slot_filterClicked(){

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
    void QobuzMyTrackAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

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
    void QobuzMyTrackAll::request_more_trackData(){

        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // next_offset
            int next_offset = this->list_track->size();

            // filtering
            QString selected_sortOpt = this->selected_filterCode.toString();
            ProcCommon::DataOrderOption sortOpt = ProcCommon::DataOrderOption::NAME;                // 기본값
            ProcCommon::DataOrderDirection orderDirection = ProcCommon::DataOrderDirection::ASC;    // 기본값

            if(selected_sortOpt == QVariant::fromValue(ProcCommon::DataOrderOption::DATE).toString()){
                sortOpt = ProcCommon::DataOrderOption::DATE;
                orderDirection = ProcCommon::DataOrderDirection::DESC;
            }

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_myFavoriteTracks, this, &QobuzMyTrackAll::slot_applyResult_myFavoriteTracks);
            proc->request_qobuz_getList_myFavoriteTracks(sortOpt, orderDirection, GET_ITEM_SIZE___ONCE, next_offset);

        }
    }


    void QobuzMyTrackAll::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > 100 ) ? 100 : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzMyTrackAll::slot_clickedItemTrack_inList);
            this->track_listAll[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->track_listAll[i]);
        }

        QList<QString> tmp_clientkey;
        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i]->setDataTrackInfo_Qobuz(this->list_track->at(i));
            tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));

            QCoreApplication::processEvents();
        }

        if(this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);
        }

        roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
        connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzMyTrackAll::slot_applyResult_getRating_track);
        proc_fav_track->request_rose_getRating_Track("QOBUZ", tmp_clientkey);

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
    void QobuzMyTrackAll::slot_applyResult_myFavoriteTracks(const QList<qobuz::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
                this->track_totalCount = list_data.at(0).appearson_count;
            }

            int start_index = this->list_track->count();

            this->list_track->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                int max_cnt = this->list_track->size();
                this->track_drawCount = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzMyTrackAll::slot_clickedItemTrack_inList);
                    this->track_listAll[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->track_listAll[i]);
                }

                QList<QString> tmp_clientkey;
                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i]->setDataTrackInfo_Qobuz(this->list_track->at(i));
                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));

                    QCoreApplication::processEvents();
                }

                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzMyTrackAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("QOBUZ", tmp_clientkey);

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
            //if(abs_ani_dialog_wait->isHidden() != true){
                ContentLoadingwaitingMsgHide();
            //}

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void QobuzMyTrackAll::slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고. => OptMorePopup 에서 하라고, 가려줌
//        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
//            bool status  = ProcJsonEasy::getBool(p_jsonObj, "status");

//            // Qobuz favorite toggle check
//            if(this->flag_send_track == true){
//                if((status == true && this->flag_track_fav == false) || (status == false && this->flag_track_fav == true)){
//                    // Qobuz Favorite toggle
//                    ProcCommon *proc = new ProcCommon(this);
//                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzMyTrackAll::slot_qobuz_completeReq_listAll_myFavoritesIds);
//                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
//                }
//                this->flag_send_track = false;
//            }
//        }
    }


    void QobuzMyTrackAll::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){
        int id = ProcJsonEasy::getInt(jsonObj, "id");

        QJsonObject jsonObj_track = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();

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
            connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzMyTrackAll::slot_applyResult_checkComplete_track);
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
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzMyTrackAll::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

        }
    }


    void QobuzMyTrackAll::slot_applyResult_checkComplete_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void QobuzMyTrackAll::slot_applyResult_getRating_track(const QJsonArray &contents){

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

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }

            this->track_favoriteOffset += GET_ITEM_SIZE___ONCE;
        }
        else{
            this->flag_check_track = false;
        }
    }


    void QobuzMyTrackAll::slot_btnClicked_playAll(){

        // 전체재생
        ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
        procRosePlay->requestPlayRose_byTracks(jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::JustPlay);
    }


    void QobuzMyTrackAll::slot_btnClicked_playShuffle(){

        // 셔플재생
        ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
        procRosePlay->requestPlayRose_byTracks(jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::ShufflePlay);
    }


    /**
     * @brief QobuzTracksListAll::slot_clickedItemTrack_inList
     * @param index
     * @param clickMode
     */
    void QobuzMyTrackAll::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

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

                if(this->track_star_fav == 0 || this->track_star_fav == 1){
                    // Qobuz Favorite toggle
                    this->track_id_fav = this->list_track->at(idx).id;

                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzMyTrackAll::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                    this->flag_send_track = true;
                }

                this->track_idx_fav = idx;
                QJsonObject jsonObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzMyTrackAll::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("QOBUZ", QString("%1").arg(ProcJsonEasy::getInt(jsonObj, "id")));
                this->flag_check_track = true;

                this->track_listAll[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
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
    void QobuzMyTrackAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }





    /**
     * @brief
     * @param parent
     */
    QobuzMyPurchasesTrackAll::QobuzMyPurchasesTrackAll(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll_rosefilter, parent) {

        // data
        this->list_track = new QList<qobuz::TrackItemData>();
        this->list_filterOpt_sort = AbstractQobuzSubWidget::get_list_filterOptData_forMyCollection();

        // UI
        this->setUIControl_tracks();
    }


    /**
     * @brief 소멸자.
     */
    QobuzMyPurchasesTrackAll::~QobuzMyPurchasesTrackAll(){

        this->deleteLater();
    }


    /**
     * @brief
     * @param jsonObj
     */
    void QobuzMyPurchasesTrackAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(this->page != pageCode){
            this->flagNeedReload = true;

            this->page = pageCode;

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
    void QobuzMyPurchasesTrackAll::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

            /*// init
            this->chooseFilterOpt->hideFilterBox();

            // init Data
            this->selected_filterCode = this->list_filterOpt_sort.first().opt_code;         // default

            QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
            QString def_name = this->list_filterOpt_sort.first().opt_name;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(def_code, def_name);*/

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
            connect(btn_playAll, &QPushButton::clicked, this, &QobuzMyPurchasesTrackAll::slot_btnClicked_playAll);

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
            connect(btn_shuffleAll, &QPushButton::clicked, this, &QobuzMyPurchasesTrackAll::slot_btnClicked_playShuffle);

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
        }
    }


    // MARK : Create UI and Set --------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void QobuzMyPurchasesTrackAll::setUIControl_tracks(){

        this->addUIControl_mainTitle("Tracks");

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &QobuzMyPurchasesTrackAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyAlbumAll::setUIControl_filter
     */
    void QobuzMyPurchasesTrackAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &QobuzMyPurchasesTrackAll::slot_filterClicked);


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
    void QobuzMyPurchasesTrackAll::initAll(){

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
    void QobuzMyPurchasesTrackAll::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)){

            this->flag_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void QobuzMyPurchasesTrackAll::resizeEvent(QResizeEvent *event){

        AbstractQobuzSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }
    }


    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void QobuzMyPurchasesTrackAll::slot_filterClicked(){

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
    void QobuzMyPurchasesTrackAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

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
    void QobuzMyPurchasesTrackAll::request_more_trackData(){

        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // next_offset
            int next_offset = this->list_track->size();

            // filtering
            QString selected_sortOpt = this->selected_filterCode.toString();
            ProcCommon::DataOrderOption sortOpt = ProcCommon::DataOrderOption::NAME;                // 기본값
            ProcCommon::DataOrderDirection orderDirection = ProcCommon::DataOrderDirection::ASC;    // 기본값

            if(selected_sortOpt == QVariant::fromValue(ProcCommon::DataOrderOption::DATE).toString()){
                sortOpt = ProcCommon::DataOrderOption::DATE;
                orderDirection = ProcCommon::DataOrderDirection::DESC;
            }

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_myPurchasesTracks, this, &QobuzMyPurchasesTrackAll::slot_applyResult_myFavoriteTracks);
            proc->request_qobuz_getList_myPurchasesTracks(sortOpt, orderDirection, GET_ITEM_SIZE___ONCE, next_offset);

        }
    }


    void QobuzMyPurchasesTrackAll::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > 100 ) ? 100 : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzMyPurchasesTrackAll::slot_clickedItemTrack_inList);
            this->track_listAll[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->track_listAll[i]);
        }

        QList<QString> tmp_clientkey;
        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i]->setDataTrackInfo_Qobuz(this->list_track->at(i));
            tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));

            QCoreApplication::processEvents();
        }

        if(this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);
        }

        roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
        connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzMyPurchasesTrackAll::slot_applyResult_getRating_track);
        proc_fav_track->request_rose_getRating_Track("QOBUZ", tmp_clientkey);

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
    void QobuzMyPurchasesTrackAll::slot_applyResult_myFavoriteTracks(const QList<qobuz::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                this->track_totalCount = list_data.at(0).appearson_count;
            }

            int start_index = this->list_track->count();

            this->list_track->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                int max_cnt = this->list_track->size();
                this->track_drawCount = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzMyPurchasesTrackAll::slot_clickedItemTrack_inList);
                    this->track_listAll[i]->setProperty("index", i);

                    this->box_main_contents->addWidget(this->track_listAll[i]);
                }

                QList<QString> tmp_clientkey;
                for(int i = start_index; i < max_cnt; i++){
                    this->track_listAll[i]->setDataTrackInfo_Qobuz(this->list_track->at(i));
                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));

                    QCoreApplication::processEvents();
                }

                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzMyPurchasesTrackAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("QOBUZ", tmp_clientkey);

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
            //if(abs_ani_dialog_wait->isHidden() != true){
                ContentLoadingwaitingMsgHide();
            //}

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void QobuzMyPurchasesTrackAll::slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고. => OptMorePopup 에서 하라고, 가려줌
//        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
//            bool status  = ProcJsonEasy::getBool(p_jsonObj, "status");

//            // Qobuz favorite toggle check
//            if(this->flag_send_track == true){
//                if((status == true && this->flag_track_fav == false) || (status == false && this->flag_track_fav == true)){
//                    // Qobuz Favorite toggle
//                    ProcCommon *proc = new ProcCommon(this);
//                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzMyPurchasesTrackAll::slot_qobuz_completeReq_listAll_myFavoritesIds);
//                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
//                }
//                this->flag_send_track = false;
//            }
//        }
    }


    void QobuzMyPurchasesTrackAll::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){
        int id = ProcJsonEasy::getInt(jsonObj, "id");

        QJsonObject jsonObj_track = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();

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
            connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzMyPurchasesTrackAll::slot_applyResult_checkComplete_track);
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
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzMyPurchasesTrackAll::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

        }
    }


    void QobuzMyPurchasesTrackAll::slot_applyResult_checkComplete_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void QobuzMyPurchasesTrackAll::slot_applyResult_getRating_track(const QJsonArray &contents){

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

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }

            this->track_favoriteOffset += GET_ITEM_SIZE___ONCE;
        }
        else{
            this->flag_check_track = false;
        }
    }


    void QobuzMyPurchasesTrackAll::slot_btnClicked_playAll(){

        // 전체재생
        ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
        procRosePlay->requestPlayRose_byTracks(jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::JustPlay);
    }


    void QobuzMyPurchasesTrackAll::slot_btnClicked_playShuffle(){

        // 셔플재생
        ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
        procRosePlay->requestPlayRose_byTracks(jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::ShufflePlay);
    }


    /**
     * @brief QobuzTracksListAll::slot_clickedItemTrack_inList
     * @param index
     * @param clickMode
     */
    void QobuzMyPurchasesTrackAll::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                //this->track_star_fav = this->track_listAll[idx].getFavoritesStars();
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
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzMyPurchasesTrackAll::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                    this->flag_send_track = true;
                }

                this->track_idx_fav = idx;
                QJsonObject jsonObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzMyPurchasesTrackAll::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("QOBUZ", QString("%1").arg(ProcJsonEasy::getInt(jsonObj, "id")));
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
    void QobuzMyPurchasesTrackAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }





    /**
     * @brief
     * @param parent
     */
    QobuzRecentlyTrackAll::QobuzRecentlyTrackAll(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll_rosefilter, parent) {

        this->linker = Linker::getInstance();

        // data
        this->list_track = new QList<roseHome::TrackItemData>();
        this->list_filterOpt_sort = AbstractQobuzSubWidget::get_list_filterOptData_forMyCollection();

        // UI
        this->setUIControl_tracks();
    }


    /**
     * @brief 소멸자.
     */
    QobuzRecentlyTrackAll::~QobuzRecentlyTrackAll(){

        this->deleteLater();
    }


    /**
     * @brief
     * @param jsonObj
     */
    void QobuzRecentlyTrackAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        QString mainTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(this->page != pageCode || this->title != mainTitle){
            this->flagNeedReload = true;

            this->page = pageCode;
            this->title = mainTitle;

            this->label_mainTitle->setText(mainTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

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

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_tracks, this, &QobuzRecentlyTrackAll::slot_applyResult_tracklistCheck);
            proc->request_rose_getList_recentlyTracks("member/track/recent", "QOBUZ", 0, 10);
        }
    }


    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void QobuzRecentlyTrackAll::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

            /*// init
            this->chooseFilterOpt->hideFilterBox();

            // init Data
            this->selected_filterCode = this->list_filterOpt_sort.first().opt_code;         // default

            QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
            QString def_name = this->list_filterOpt_sort.first().opt_name;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(def_code, def_name);*/

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
            connect(btn_playAll, &QPushButton::clicked, this, &QobuzRecentlyTrackAll::slot_btnClicked_playAll);

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
            connect(btn_shuffleAll, &QPushButton::clicked, this, &QobuzRecentlyTrackAll::slot_btnClicked_playShuffle);

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
        }
    }


    // MARK : Create UI and Set --------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void QobuzRecentlyTrackAll::setUIControl_tracks(){

        GSCommon::clearLayout(this->box_mainTitle);

        this->widget_mainTitle = new QWidget();
        this->widget_mainTitle->setStyleSheet("background-color:#171717;");
        this->widget_mainTitle->setMinimumSize(800, 69);
        this->widget_mainTitle->resize(global.LmtCnt, 69);

        this->label_mainTitle = new QLabel(this->widget_mainTitle);
        this->label_mainTitle->setStyleSheet("background-color:transparent; font-size:26px; color:#FFFFFF;");
        this->label_mainTitle->setText("");
        this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

        this->btn_mainTitle = new QPushButton(this->widget_mainTitle);
        this->btn_mainTitle->setObjectName("btn_mainTitle");
        this->btn_mainTitle->setCursor(Qt::PointingHandCursor);
        this->btn_mainTitle->setStyleSheet("background-color:transparent;");

        connect(this->btn_mainTitle, &QPushButton::clicked, this, &QobuzRecentlyTrackAll::slot_btnClicked_Delete);

        this->label_delete_icon = GSCommon::getUILabelImg(":/images/q_del_ico.png", this->btn_mainTitle);
        this->label_delete_icon->setGeometry(0, 10, 50, 50);

        this->label_delete = new QLabel(this->btn_mainTitle);
        this->label_delete->setText(tr("Delete"));
        this->label_delete->setStyleSheet("background-color:transparent;font-size:20px;color:#FFFFFF;");
        this->label_delete->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        this->label_delete->setGeometry(60, 23, this->label_delete->sizeHint().width(), this->label_delete->sizeHint().height());

        this->btn_mainTitle->setGeometry(global.LmtCnt - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);

        this->box_mainTitle->addWidget(this->widget_mainTitle);

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        // filtering
        //this->setUIControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &QobuzRecentlyTrackAll::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyAlbumAll::setUIControl_filter
     */
    void QobuzRecentlyTrackAll::setUIControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &QobuzRecentlyTrackAll::slot_filterClicked);


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
    void QobuzRecentlyTrackAll::initAll(){

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
    void QobuzRecentlyTrackAll::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)){

            this->flag_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void QobuzRecentlyTrackAll::resizeEvent(QResizeEvent *event){

        AbstractQobuzSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }

        this->widget_mainTitle->resize(global.LmtCnt, 69);
        this->btn_mainTitle->setGeometry(global.LmtCnt - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);
    }


    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void QobuzRecentlyTrackAll::slot_filterClicked(){

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
    void QobuzRecentlyTrackAll::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

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
    void QobuzRecentlyTrackAll::request_more_trackData(){

        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // next_offset
            if(this->list_track->size() == 0){
                this->next_offset = 0;
            }
            else{
                this->next_offset++;
            }

            // filtering
            /*QString selected_sortOpt = this->selected_filterCode.toString();
            ProcCommon::DataOrderOption sortOpt = ProcCommon::DataOrderOption::NAME;                // 기본값
            ProcCommon::DataOrderDirection orderDirection = ProcCommon::DataOrderDirection::ASC;    // 기본값

            if(selected_sortOpt == QVariant::fromValue(ProcCommon::DataOrderOption::DATE).toString()){
                sortOpt = ProcCommon::DataOrderOption::DATE;
                orderDirection = ProcCommon::DataOrderDirection::DESC;
            }*/

            // request HTTP API
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_tracks, this, &QobuzRecentlyTrackAll::slot_applyResult_myTracks_rose);
            proc->request_rose_getList_recentlyTracks("member/track/recent", "QOBUZ", this->next_offset, 100);
        }
    }


    void QobuzRecentlyTrackAll::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > 100 ) ? 100 : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzRecentlyTrackAll::slot_clickedItemTrack_inList);
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
    void QobuzRecentlyTrackAll::slot_applyResult_myTracks_rose(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

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
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzRecentlyTrackAll::slot_clickedItemTrack_inList);
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

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }


    void QobuzRecentlyTrackAll::slot_applyResult_tracklistCheck(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_track->count() == 0){
                QString tmpTitle = tr("Recently played tracks") + QString(" (%1)").arg(list_data.at(0).totalCount);
                this->label_mainTitle->setText(tmpTitle);
                this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                this->initAll();

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                this->request_more_trackData();

                this->flagNeedReload = true;
                this->setActivePage();
            }
            else{
                for(int i = 0; i < list_data.count(); i++){
                    if(list_data.at(i).title != this->list_track->at(i).title){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_track->at(0).totalCount) || (change_flag > 0)){

                    QString tmpTitle = tr("Recently played tracks") + QString(" (%1)").arg(list_data.at(0).totalCount);
                    this->label_mainTitle->setText(tmpTitle);
                    this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                    this->initAll();

                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                    this->request_more_trackData();

                    this->flagNeedReload = true;
                    this->setActivePage();
                }
                else{
                    ContentLoadingwaitingMsgHide();
                }
            }
        }
        else if(list_data.size() == 0){

            QString tmpTitle = tr("Recently played tracks (0)");
            this->label_mainTitle->setText(tmpTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

            this->initAll();

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackData();

            this->flagNeedReload = true;
            this->setActivePage();
        }
        else{
            ContentLoadingwaitingMsgHide();
        }
    }


    void QobuzRecentlyTrackAll::slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고. => OptMorePopup 에서 하라고, 가려줌
//        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
//            bool status  = ProcJsonEasy::getBool(p_jsonObj, "status");

//            // Qobuz favorite toggle check
//            if(this->flag_send_track == true){
//                if((status == true && this->flag_track_fav == false) || (status == false && this->flag_track_fav == true)){
//                    // Qobuz Favorite toggle
//                    ProcCommon *proc = new ProcCommon(this);
//                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzRecentlyTrackAll::slot_qobuz_completeReq_listAll_myFavoritesIds);
//                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
//                }
//                this->flag_send_track = false;
//            }
//        }
    }


    void QobuzRecentlyTrackAll::slot_applyResult_getRating_track(const QJsonArray &contents){

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


    void QobuzRecentlyTrackAll::slot_btnClicked_playAll(){

        // 전체재생
        roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
        procRosePlay->requestPlayRose_byTracks(this->list_track->at(0), this->jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow_withReorder, ProcRosePlay_withQobuz::PlayShuffleMode::JustPlay);
    }


    void QobuzRecentlyTrackAll::slot_btnClicked_playShuffle(){

        // 셔플재생
        roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
        procRosePlay->requestPlayRose_byTracks(this->list_track->at(0), this->jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow_withReorder, ProcRosePlay_withQobuz::PlayShuffleMode::ShufflePlay);
    }


    void QobuzRecentlyTrackAll::slot_btnClicked_Delete(){

        QStringList tmpTitle = this->title.split("(");

        QJsonObject tmpObj;
        tmpObj.insert("contents_type", tmpTitle.at(0));
        tmpObj.insert("list_type", "TRACK");
        tmpObj.insert("api_subPath", "member/track/recent");
        tmpObj.insert("filter_type", "QOBUZ");

        QJsonObject tmpObj_move;
        tmpObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_MY_RECENTLY_LIST_DELETE);
        tmpObj_move.insert(KEY_DATA, tmpObj);

        emit this->linker->signal_clickedMovePage(tmpObj_move);
    }


    /**
     * @brief QobuzTracksListAll::slot_clickedItemTrack_inList
     * @param index
     * @param clickMode
     */
    void QobuzRecentlyTrackAll::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->track_listAll[idx]->getFavoritesStars();
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
                    // Qobuz Favorite toggle
                    this->track_id_fav = this->list_track->at(idx).clientKey.toInt();

                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzRecentlyTrackAll::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                    this->flag_send_track = true;
                }

                QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QString track_type = ProcJsonEasy::getString(track, "type");

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_track_fav);
                ratingInfo.insert("star", this->track_star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", track_type);

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzRecentlyTrackAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
                this->flag_check_track = true;

                this->track_listAll[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }
        }
        else{
            // ClickMode 별로 처리
            roseHome::TrackItemData data = list_track->at(idx);

            if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AllBox){
                // Track 바로 재생 - clicked 항목만 재생
                OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::Play_RightNow_withReorder;

                if(global.device.getMusicPlayType() == 15){
                    playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                }
                else if(global.device.getMusicPlayType() == 13){
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                }
                else if(global.device.getMusicPlayType() == 12){
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                }
                else if(global.device.getMusicPlayType() == 16){
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                }
                else if(global.device.getMusicPlayType() == 17){
                    playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                }
                else if(global.device.getMusicPlayType() == 18){
                    playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                }
                else if(global.device.getMusicPlayType() == 19){
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                }

                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(this->list_track->at(idx), this->jsonArr_tracks_toPlay, idx, playType);

                /*QJsonArray send_track_json = QJsonArray();
                send_track_json.append(this->jsonArr_tracks_toPlay.at(idx).toObject());

                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data, send_track_json, idx, OptMorePopup::ClickMode::Play_RightNow_withReorder);*/
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                // 플레이리스트 담기 - Track
                //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){
                // OptMorePopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Track, roseHome::ConvertData::getConvertOptHeaderData(data), idx, SECTION_FOR_MORE_POPUP___tracks);
            }
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void QobuzRecentlyTrackAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___tracks){
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
                // Rose Play 요청
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(this->list_track->at(index), this->jsonArr_tracks_toPlay, index, clickMode);
            }
        }
    }





    /**
     * @brief "ROSEHOME > REcENT TRACK" 화면을 위한 생성자. @see PAGECODE_RH_TRACK_LIST_ALL_VIEW
     * @param parent
     */
    QobuzTrackHistoryAll::QobuzTrackHistoryAll(QWidget *parent) : AbstractQobuzSubWidget(VerticalScroll_rosefilter, parent) {

        // data
        this->list_track = new QList<roseHome::TrackItemData>();

        // UI
        this->setUIControl_tracks();
    }


    /**
    * @brief 소멸자.
    */
    QobuzTrackHistoryAll::~QobuzTrackHistoryAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzTrackHistoryAll::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::PageInfo_TrackAllView tmp_pageInfo = roseHome::ConvertData::convertData_pageInfo_trackAllView(jsonObj);
        this->flagNeedReload = false;

        if(tmp_pageInfo.api_subPath != this->data_pageInfo.api_subPath || tmp_pageInfo.pathTitle != this->data_pageInfo.pathTitle || tmp_pageInfo.filter_type != this->data_pageInfo.filter_type){
            this->flagNeedReload = true;

            this->data_pageInfo = tmp_pageInfo;

            this->label_mainTitle->setText(this->data_pageInfo.pathTitle);

            if(this->flag_widget_visible == true){
                this->widget_main_contents->hide();
                this->box_contents->removeWidget(this->widget_main_contents);
                GSCommon::clearLayout(this->box_contents);

                this->flag_widget_visible = false;
            }

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

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief RoseHomeTrackListAll::setActivePage
     */
    void QobuzTrackHistoryAll::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

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
            connect(btn_playAll, &QPushButton::clicked, this, &QobuzTrackHistoryAll::slot_btnClicked_playAll);

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
            connect(btn_shuffleAll, &QPushButton::clicked, this, &QobuzTrackHistoryAll::slot_btnClicked_playShuffle);

            this->box_filter->addWidget(this->widget_filter_contents);


            this->box_contents->removeWidget(this->widget_main_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

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


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void QobuzTrackHistoryAll::setUIControl_tracks(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("");
        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();
    }

    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void QobuzTrackHistoryAll::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->list_track->size() > this->track_drawCount) && (this->flag_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void QobuzTrackHistoryAll::request_more_trackData(){

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
            roseHome::ProcCommon *proc_track = new roseHome::ProcCommon(this);
            connect(proc_track, &roseHome::ProcCommon::completeReq_list_tracks, this, &QobuzTrackHistoryAll::slot_applyResult_historyTrack);
            proc_track->setProperty("idx", this->next_offset);
            proc_track->request_rose_getList_historyTracks(this->data_pageInfo.api_subPath, this->data_pageInfo.filter_type, this->next_offset, 20);
        }
    }

    void QobuzTrackHistoryAll::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > 20 ) ? 20 : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        for(int i = start_index; i < this->track_drawCount; i++){
            this->track_listAll[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzTrackHistoryAll::slot_clickedItemTrack_inList);
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
    void QobuzTrackHistoryAll::slot_applyResult_historyTrack(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            if(list_data.size() == 0){
                ContentLoadingwaitingMsgHide();
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
                    connect(this->track_listAll[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzTrackHistoryAll::slot_clickedItemTrack_inList);
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

            if(this->list_track->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->box_main_contents->addWidget(noData_widget);
            }
        }
    }

    void QobuzTrackHistoryAll::slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고. => OptMorePopup 에서 하라고, 가려줌
//        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
//            bool status  = ProcJsonEasy::getBool(p_jsonObj, "status");

//            // Qobuz favorite toggle check
//            if(this->flag_send_track == true){
//                if((status == true && this->flag_track_fav == false) || (status == false && this->flag_track_fav == true)){
//                    // Qobuz Favorite toggle
//                    ProcCommon *proc = new ProcCommon(this);
//                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzTrackHistoryAll::slot_qobuz_completeReq_listAll_myFavoritesIds);
//                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
//                }
//                this->flag_send_track = false;
//            }
//        }
    }


    void QobuzTrackHistoryAll::slot_applyResult_getRating_track(const QJsonArray &contents){

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


    void QobuzTrackHistoryAll::slot_btnClicked_playAll(){

        roseHome::TrackItemData data_track;

        // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
        procRosePlay->requestPlayRose_byTracks(data_track, jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow_withReorder, AbstractProcRosePlay::PlayShuffleMode::JustPlay);
    }

    void QobuzTrackHistoryAll::slot_btnClicked_playShuffle(){

        roseHome::TrackItemData data_track;

        // Track shuffle 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
        roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
        procRosePlay->requestPlayRose_byTracks(data_track, jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow_withReorder, AbstractProcRosePlay::PlayShuffleMode::ShufflePlay);
    }


    /**
     * @brief [Slot] PlaylistTrackDetailInfo_RHV signal에 대하여 처리
     * @param clickMode
     */
    void QobuzTrackHistoryAll::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

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
                if(this->track_star_fav == 0 || this->track_star_fav == 1){
                    // Qobuz Favorite toggle
                    this->track_id_fav = this->list_track->at(idx).clientKey.toInt();

                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzTrackHistoryAll::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                    this->flag_send_track = true;
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
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzTrackHistoryAll::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

                this->flag_check_track = true;
            }
        }
        else{
            // ClickMode 별로 처리
            //this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, SECTION_FOR_MORE_POPUP___tracks);
            roseHome::TrackItemData data = this->list_track->at(idx);

            if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AllBox){
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

                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data, this->jsonArr_tracks_toPlay, idx, playType);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                // 플레이리스트 담기 - Track
                //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){
                // OptMorePopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Track, roseHome::ConvertData::getConvertOptHeaderData(data), idx, SECTION_FOR_MORE_POPUP___tracks);
            }
        }
    }

    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void QobuzTrackHistoryAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___tracks){
            //this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            else if(clickMode == OptMorePopup::ClickMode::Play_RightNow
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
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(this->list_track->at(index), this->jsonArr_tracks_toPlay, index, clickMode);
            }
        }
    }
}
