#include "qobuz/QobuzAlbumDetail.h"

#include "qobuz/ProcCommon_forQobuz.h"
#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ItemAlbum_forQobuz.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"
#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"

#include <QDebug>
#include <QPainter>
#include <QScroller>
#include <QScrollArea>
#include <QPixmapCache>
#include <QJsonDocument>


namespace qobuz {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int BTN_IDX_SUBTITLE_sameArtist = 3;
    const int BTN_IDX_SUBTITLE_suggestArtist = 4;

    const int SECTION_FOR_MORE_POPUP___albums = 1;
    const int SECTION_FOR_MORE_POPUP___track = 2;
    const int SECTION_FOR_MORE_POPUP___sameArtist = 3;
    const int SECTION_FOR_MORE_POPUP___suggestArtist = 4;

    QString ALBTAB_STEP_TRACK = "track";
    QString ALBTAB_STEP_CREDIT = "credit";

    /**
     * @brief
     * @param parent
     */
    QobuzAlbumDetail::QobuzAlbumDetail(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll_filter, parent) {

        // data
        this->list_track = new QList<qobuz::TrackItemData>();
        this->list_sameArtist = new QList<qobuz::AlbumItemData>();
        this->list_suggestArtist = new QList<qobuz::ArtistItemData>();
    }

    /**
     * @brief 소멸자.
     */
    QobuzAlbumDetail::~QobuzAlbumDetail(){

        this->deleteLater();
    }


    /**
     * @brief QobuzAlbumDetail::setJsonObject_forData
     * @param jsonObj
     */
    void QobuzAlbumDetail::setJsonObject_forData(const QJsonObject &jsonObj){


        qobuz::AlbumItemData tmp_data_album = ConvertData::convertData_albumData(jsonObj);

        this->flagNeedReload = false;

        if(global.user_forQobuz.isLogined() == false){

            this->login_wait_Object = QJsonObject();
            this->login_wait_Object = jsonObj;

            this->qobuzSettings = new QobuzSettings();
            this->qobuzSettings->setActivePage();

            this->box_login_contents = new QVBoxLayout();
            this->box_login_contents->setSpacing(0);
            this->box_login_contents->setContentsMargins(0, 0, 0, 0);
            this->box_login_contents->addWidget(this->qobuzSettings);

            this->widget_login_contents = new QWidget();
            this->widget_login_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_login_contents->setLayout(this->box_login_contents);

            this->box_contents->addWidget(this->widget_login_contents);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            connect(this->qobuzSettings, &QobuzSettings::signal_logState_change, this, &QobuzAlbumDetail::slot_qobuz_logChange);
        }
        else{

            if(tmp_data_album.id != this->data_album.id){

                this->flagNeedReload = true;

                if(this->flag_track_ok == true){
                    this->widget_main_contents->hide();

                    this->box_contents->removeWidget(this->widget_main_contents);
                    GSCommon::clearLayout(this->box_main_contents);

                    this->flag_track_ok = false;
                }

                this->data_album = tmp_data_album;

                this->list_track->clear();
                this->list_sameArtist->clear();
                this->list_suggestArtist->clear();
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

                // request HTTP API
                ProcCommon *proc_album = new ProcCommon(this);
                connect(proc_album, &ProcCommon::completeReq_album, this, &QobuzAlbumDetail::slot_applyResult_albumInfo);
                //connect(proc_album, &ProcCommon::completeReq_list_items_of_album, this, &QobuzAlbumDetail::slot_applyResult_tracks);
                connect(proc_album, &ProcCommon::completeReq_list_albums, this, &QobuzAlbumDetail::slot_applyResult_sameArtist);
                proc_album->request_qobuz_get_albuminfo(this->data_album.id);

                ProcCommon *proc_track = new ProcCommon(this);
                connect(proc_track, &ProcCommon::completeReq_list_items_of_album, this, &QobuzAlbumDetail::slot_applyResult_tracks);
                proc_track->request_qobuz_getList_items_of_album(this->data_album.id);

                if(this->data_album.artist_id > 0){
                    ProcCommon *proc_artist = new ProcCommon(this);
                    connect(proc_artist, &ProcCommon::completeReq_list_artists, this, &QobuzAlbumDetail::slot_applyResult_suggestArtsit);
                    proc_artist->request_qobuz_getList_artists("artist/getSimilarArtists", this->data_album.artist_id, 15, 0);
                }

                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

                this->track_favoriteOffset = 0;
            }
            else{
                // 리로드 하지 않는 경우에는, favorite 정보만 다시 요청한다. (album_id 가 변경되지 않고, 페이지가 다시 요청된 경우임)
                // request HTTP API - favorite for Rose Server
                roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
                connect(proc_fav, &roseHome::ProcCommon::completeReq_rating_album, this, &QobuzAlbumDetail::slot_applyResult_getRating_album);
                proc_fav->request_rose_getRating_Album("QOBUZ", this->data_album.id);

                QList<QString> tmp_clientkey;
                for(int i = 0; i < this->list_track->size(); i++){
                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));
                }

                this->track_favoriteOffset = 0;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzAlbumDetail::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("QOBUZ", tmp_clientkey);
            }
        }

    }

    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void QobuzAlbumDetail::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

            if(this->flag_track_ok == true){
                GSCommon::clearLayout(this->box_main_contents);
                this->box_contents->removeWidget(this->widget_main_contents);
            }

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
            QJsonObject sub2 { {"name", tr("Credit")}, {"code", ALBTAB_STEP_CREDIT} };

            p_jsonArray_titlSub->push_back(sub1);
            p_jsonArray_titlSub->push_back(sub2);

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
    void QobuzAlbumDetail::setUIControl_basic(){

        // 앨범 상세정보 UI
        this->album_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Qobuz_album);
        connect(this->album_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &QobuzAlbumDetail::slot_detailInfo_btnClicked);

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

        this->hBox_sameArtist = new QHBoxLayout();
        this->hBox_sameArtist->setSpacing(0);
        this->hBox_sameArtist->setContentsMargins(0, 0, 0, 0);

        this->hBox_suggestArtist = new QHBoxLayout();
        this->hBox_suggestArtist->setSpacing(0);
        this->hBox_suggestArtist->setContentsMargins(0, 0, 0, 0);

    }


    void QobuzAlbumDetail::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId){

        // box_main_contents 에 담을 widget, layout 생성.  box_main_contents에 먼저 담는다.
        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        tmp_hBox->setSpacing(0);
        tmp_hBox->setContentsMargins(0, 0, 0, 0);
        tmp_hBox->setAlignment(Qt::AlignTop);

        QWidget *widget_box_subTitle = new QWidget();
        widget_box_subTitle->setContentsMargins(0, 0, 0, 0);
        widget_box_subTitle->setLayout(tmp_hBox);

        QLabel *lb_subTitle = new QLabel();
        lb_subTitle->setText(subTitle);
        lb_subTitle->setStyleSheet("font-size:24px;font-weight:bold;color:#FFFFFF;");
        tmp_hBox->addWidget(lb_subTitle, 0, Qt::AlignVCenter);

        // side button - it works when the button text is not empty
        if(btnText.isEmpty() == false){
            QPushButton *btnView_all = new QPushButton();
            btnView_all->setText(btnText);
            btnView_all->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
            btnView_all->setProperty("idx", btnId);
            btnView_all->setCursor(Qt::PointingHandCursor);
            connect(btnView_all, SIGNAL(clicked()), this, SLOT(slot_clickBtn_subTitle_viewAll()));

            tmp_hBox->addWidget(btnView_all, 1, Qt::AlignVCenter | Qt::AlignRight);
        }

        this->box_main_contents->addWidget(widget_box_subTitle, 0, Qt::AlignTop);

    }


    QHBoxLayout* QobuzAlbumDetail::setUIControl_hBoxLayout_forAlbum(){

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


    QHBoxLayout* QobuzAlbumDetail::setUIControl_hBoxLayout_forArtist(){

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


    void QobuzAlbumDetail::setUIControl_sameArtsit(){

        if(this->list_sameArtist->count() > 0){

            this->setUIControl_subTitle_withSideBtn("By the same artist ", "View All", BTN_IDX_SUBTITLE_sameArtist);
            this->box_main_contents->addSpacing(10);

            this->hBox_sameArtist = this->setUIControl_hBoxLayout_forAlbum();

            int max_cnt = this->list_sameArtist->count();
            if(max_cnt > 15){
                max_cnt = 15;
            }

            for(int i = 0; i < max_cnt; i++){
                this->sameArtist[i]->setData(this->list_sameArtist->at(i));
                this->hBox_sameArtist->addWidget(this->sameArtist[i]);
            }

            this->box_main_contents->addSpacing(30);
        }

    }

    void QobuzAlbumDetail::setUIControl_suggestArtsits(){

        if(this->list_suggestArtist->count() > 0){

            this->setUIControl_subTitle_withSideBtn("Artist suggestions ", "View All", BTN_IDX_SUBTITLE_suggestArtist);
            this->box_main_contents->addSpacing(10);

            this->hBox_suggestArtist = this->setUIControl_hBoxLayout_forArtist();

            int max_cnt = this->list_suggestArtist->count();
            if(max_cnt > 15){
                max_cnt = 15;
            }

            for(int i = 0; i < max_cnt; i++){
                this->artistSuggestions[i]->setData(this->list_suggestArtist->at(i));
                this->hBox_suggestArtist->addWidget(this->artistSuggestions[i]);
            }

            this->box_main_contents->addSpacing(30);
        }

    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief QobuzAlbumDetail::slot_applyResult_albumInfo
     * @param data_album
     */
    void QobuzAlbumDetail::slot_applyResult_albumInfo(qobuz::AlbumItemData album_info){

        ContentLoadingwaitingMsgHide();//c220616
        if(album_info.title == "error"){

            QJsonObject albumObj;
            albumObj.insert("image", "");
            albumObj.insert("title", "");
            albumObj.insert("hires", false);
            albumObj.insert("artist","");
            albumObj.insert("hires", 0);
            albumObj.insert("duration", 0);
            albumObj.insert("sampleRate", 0);
            albumObj.insert("bitDepth", 0);
            albumObj.insert("release_date_stream", "");
            albumObj.insert("description", "");
            albumObj.insert("id", "");

            this->album_detail_info->setData_fromQobuzData(albumObj);

            QJsonObject tmpData;
            tmpData.insert("image", "");
            tmpData.insert("title", "Error");
            tmpData.insert("artist", "");
            tmpData.insert("composer", "");
            tmpData.insert("copyright", "");
            tmpData.insert("released", "");
            tmpData.insert("genre", "");
            tmpData.insert("track", "");
            tmpData.insert("duration", "");
            tmpData.insert("description", "");

            this->dlgAlbumInfo = new DialogNotice(this, DialogNotice::DlgNotice::Qobuz_album_info);
            this->dlgAlbumInfo->setData_formJsonObj(tmpData);
            this->dlgAlbumInfo->exec();
        }
        else{
            int tmpArtist_id = this->data_album.artist_id;

            // Update Data
            this->data_album = album_info;
            print_debug();
            qDebug() << "this->data_album.id=" << this->data_album.id ;
            // Update UI
            QString title_ver = "";
            if(this->data_album.version.isEmpty()){
                title_ver = this->data_album.title;
            }
            else{
                title_ver = this->data_album.title + QString(" (%1)").arg(this->data_album.version);
            }

            QJsonObject albumObj;
            albumObj.insert("image", this->data_album.image_large);
            albumObj.insert("title", title_ver);
            albumObj.insert("hires", this->data_album.hires);
            if(this->data_album.artist_name.isEmpty()){
                albumObj.insert("artist", this->data_album.list_artist_name.join(","));
            }
            else{
                albumObj.insert("artist", this->data_album.artist_name);
            }
            if(this->data_album.numberOfTracks != 0){
                albumObj.insert("numberOfTracks", this->data_album.numberOfTracks);
            }
            else{
                albumObj.insert("numberOfTracks", this->data_album.tracks_count);
            }
            albumObj.insert("duration", this->data_album.duration);
            albumObj.insert("sampleRate", this->data_album.maximum_sampling_rate);
            albumObj.insert("bitDepth", this->data_album.maximum_bit_depth);
            albumObj.insert("release_date_stream", this->data_album.release_date_stream);
            albumObj.insert("description", this->data_album.description);
            albumObj.insert("id", this->data_album.id);

            this->album_detail_info->setData_fromQobuzData(albumObj);            

            // request HTTP API - favorite for Rose Server
            roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
            connect(proc_fav, &roseHome::ProcCommon::completeReq_rating_album, this, &QobuzAlbumDetail::slot_applyResult_getRating_album);
            proc_fav->request_rose_getRating_Album("QOBUZ", this->data_album.id);

            // share link
            QString param = "QOBUZ/ALBUM/" + QString("%1").arg(this->data_album.id);
            QString desc = this->data_album.list_artist_name.join(",");

            roseHome::ProcCommon *proc_link = new roseHome::ProcCommon(this);
            connect(proc_link, &roseHome::ProcCommon::completeReq_share_link, this, &QobuzAlbumDetail::slot_applyResult_getShareLink);
            proc_link->request_rose_get_shareLink(this->data_album.image_large, desc, this->data_album.title, param);

            if(this->data_album.artist_id != tmpArtist_id){
                ProcCommon *proc_artist = new ProcCommon(this);
                connect(proc_artist, &ProcCommon::completeReq_list_artists, this, &QobuzAlbumDetail::slot_applyResult_suggestArtsit);
                proc_artist->request_qobuz_getList_artists("artist/getSimilarArtists", this->data_album.artist_id, 15, 0);
            }
        }

    }

    //c220818 share link
    void QobuzAlbumDetail::slot_applyResult_getShareLink(const QString &link){

        this->shareLink = link;
    }

    void QobuzAlbumDetail::slot_applyResult_sameArtist(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);
        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.count() > 0){
            this->list_sameArtist->append(list_data);

            for(int i = 0; i < 15; i++){
                this->sameArtist[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___sameArtist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->sameArtist[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzAlbumDetail::slot_clickedItemAlbum);
            }
        }

    }



    void QobuzAlbumDetail::slot_applyResult_suggestArtsit(const QList<qobuz::ArtistItemData> &list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.count() > 0){
            this->list_suggestArtist->append(list_data);

            for(int i = 0; i < 15; i++){
                this->artistSuggestions[i] = new qobuz::ItemArtist_qobuz(i, SECTION_FOR_MORE_POPUP___suggestArtist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->artistSuggestions[i], &qobuz::ItemArtist_qobuz::signal_clicked, this, &QobuzAlbumDetail::slot_clickedItemArtist);
            }
        }

    }


    /**
     * @brief QobuzAlbumDetail::slot_qobuz_completeReq_listAll_myFavoritesIds
     * @param p_jsonObj
     */
    void QobuzAlbumDetail::slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){

        // Favorite 정보를 전달해줌. 알아서 처리하라고.
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
            bool status  = ProcJsonEasy::getBool(p_jsonObj, "status");

            // Qobuz favorite toggle check
            if(this->flag_send_album == true){
                if((status == true && this->flag_album_fav == false) || (status == false && this->flag_album_fav == true)){
                    // Qobuz Favorite toggle
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzAlbumDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("album", QString("%1").arg(data_album.id), this->flag_album_fav);
                }
                this->flag_send_album = false;
            }
            else if(this->flag_send_track == true){
                if((status == true && this->flag_track_fav == false) || (status == false && this->flag_track_fav == true)){
                    // Qobuz Favorite toggle
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzAlbumDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                }
                this->flag_send_track = false;
            }
            else{

            }
        }

    }


    void QobuzAlbumDetail::slot_applyResult_checkRating_album(const QJsonObject &jsonObj){

        int id = ProcJsonEasy::getInt(jsonObj, "id");

        if(id <= 0){

            QJsonObject album;
            album.insert("clientKey", this->data_album.id);
            album.insert("favorite", true);
            album.insert("star", 1);
            album.insert("thumbnail", this->data_album.image_large);
            album.insert("title", this->data_album.title);
            album.insert("trackCount", 0);
            album.insert("type", "QOBUZ");

            QJsonObject json;
            json.insert("album", album);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
            connect(proc_fav_album, &roseHome::ProcCommon::completeCheck_rating_album, this, &QobuzAlbumDetail::slot_applyResult_addRating_album);
            proc_fav_album->request_rose_addRating_Album(json);
        }
        else{

            QJsonObject album;
            //album.insert("clientKey", this->data_album.id);
            album.insert("id", id);
            album.insert("star", 0);
            album.insert("trackCount", 0);
            album.insert("type", "QOBUZ");

            QJsonObject ratingInfo;
            ratingInfo.insert("favorite", this->flag_album_fav);
            ratingInfo.insert("star", this->album_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "QOBUZ");

            QJsonObject json;
            json.insert("album", album);
            json.insert("ratingInfo", ratingInfo);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
            connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_album, this, &QobuzAlbumDetail::slot_applyResult_getRating_album);
            proc_fav_album->request_rose_setRating_Album(json, this->flag_album_fav, this->album_star_fav);
        }

    }


    void QobuzAlbumDetail::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        int id = ProcJsonEasy::getInt(jsonObj, "id");

        QJsonObject jsonObj_track = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();
        if(id <= 0){

            QJsonObject artist;
            artist.insert("albums_count", this->data_album.artist_albums_count);
            artist.insert("id", this->data_album.artist_id);
            artist.insert("name", this->data_album.artist_name);

            QJsonObject image;
            image.insert("large", this->data_album.image_large);
            image.insert("small", this->data_album.image_small);
            image.insert("thumbnail", this->data_album.image_thumbnail);

            QJsonObject label;
            label.insert("albums_count", this->data_album.label_albums_count);
            label.insert("id", this->data_album.label_id);
            label.insert("name", this->data_album.label_name);

            QJsonObject album;
            album.insert("artist", artist);
            album.insert("duration", this->data_album.duration);
            album.insert("favCnt", 0);
            album.insert("hires", this->data_album.hires);
            album.insert("hires_streamable", this->data_album.hires_streamable);
            album.insert("id", this->data_album.id);
            album.insert("image", image);
            album.insert("label", label);
            album.insert("maximum_bit_depth",  this->data_album.maximum_bit_depth);
            album.insert("maximum_channel_count",  this->data_album.maximum_channel_count);
            album.insert("maximum_sampling_rate", this->data_album.maximum_sampling_rate);
            album.insert("release_date_original",  this->data_album.release_date_original);
            album.insert("release_date_stream",  this->data_album.release_date_stream);
            album.insert("streamable", this->data_album.streamable);
            album.insert("title", this->data_album.title);
            album.insert("tracks_count", this->data_album.tracks_count);

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

            QJsonObject track;
            track.insert("clientKey", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            track.insert("data", data);
            track.insert("duration", 0);
            track.insert("favorite", this->flag_track_fav);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 1);
            track.insert("thumbnailUrl", this->data_album.image_small);
            track.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            track.insert("type", "QOBUZ");

            QJsonArray tracks;
            tracks.append(track);

            QJsonObject json;
            json.insert("tracks", tracks);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzAlbumDetail::slot_applyResult_addRating_track);
            proc_fav_track->request_rose_addRating_Track(json);
        }
        else{

            QJsonObject ratingInfo;
            ratingInfo.insert("favorite", this->flag_track_fav);
            ratingInfo.insert("star", this->track_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "QOBUZ");

            QJsonObject track;
            track.insert("duration", 0);
            track.insert("favorite", false);
            track.insert("id", id);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 0);
            track.insert("type", "QOBUZ");

            QJsonObject json;
            json.insert("ratingInfo", ratingInfo);
            json.insert("track", track);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzAlbumDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
        }

    }


    void QobuzAlbumDetail::slot_applyResult_addRating_album(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_album == true){
                this->flag_check_album = false;
            }
        }
        else{
            this->flag_check_album = false;
        }
    }


    void QobuzAlbumDetail::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void QobuzAlbumDetail::slot_applyResult_getRating_album(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject tmp_star = contents.at(0).toObject();

            if(tmp_star.contains("star")){
                int star = ProcJsonEasy::getInt(tmp_star, "star");
                bool flag = star > 0 ? true : false;

                this->album_detail_info->setFavorite(flag, star);
            }

            if(this->flag_check_album == true){
                this->flag_check_album = false;
            }
        }
    }


    void QobuzAlbumDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->album_tracks[this->track_favoriteOffset + i]->setFavoritesIds(flag, star);

                    if(this->flag_credit_ok == true){
                        this->album_credit[this->track_favoriteOffset + i]->setFavoritesIds(flag, star);
                    }
                }
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }





    /**
     * @brief QobuzAlbumDetail::slot_applyResult_track
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void QobuzAlbumDetail::slot_applyResult_tracks(const QList<qobuz::TrackItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);
        ContentLoadingwaitingMsgHide();//c220620
        if(list_data.size() <= 0){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->vl_tracks->addWidget(noData_widget, 0, Qt::AlignTop);

            QJsonObject p_data;
            p_data.insert("pageCode", "track");
            slot_changedSubTabUI(p_data);

            this->menubar->show();
            this->stackedwidget->show();

            this->flag_track_ok = true;
        }
        else{
            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();

                int max_cnt = list_data.at(0).album_tracks_count;
                int cd_ico_cnt = 0;
                for(int i = 0; i < max_cnt; i++){
                    if(list_data.at(i).track_number == 1){
                        cd_ico_cnt++;
                    }
                }

                int cd_num = 1;
                for(int i = 0; i < max_cnt; i++){
                    print_debug();
                    // qDebug()<<"77777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777";//cheontidalhires
                    //QJsonDocument doc(jsonArr_dataToPlay.at(i).toObject());  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "QobuzAlbumDetail::slot_applyResult_tracks()-jsonObj =: " << strJson;//cheonprint//cheontidalhires

                    this->album_tracks[i] = new AlbumTrackDetailInfo_RHV;
                    connect(this->album_tracks[i], &AlbumTrackDetailInfo_RHV::clicked, this, &QobuzAlbumDetail::slot_clickedItemTrack_inList);
                    this->album_tracks[i]->setProperty("index", i);

                    if((cd_ico_cnt > 1) && (list_data.at(i).track_number == 1)){
                        QWidget *tracks_header = new QWidget();
                        tracks_header->setFixedSize(1500, 52);
                        tracks_header->setContentsMargins(0, 0, 0, 0);

                        QLabel *label_cd_ico = GSCommon::getUILabelImg(":/images/qobuz/album_cd_ico.png", tracks_header);
                        label_cd_ico->setGeometry(0, 19, 30, 30);

                        QLabel *label_cd_Name = new QLabel(tracks_header);
                        label_cd_Name->setStyleSheet("color:#FFFFFF;font-size:24px;font-weight:bold;");
                        label_cd_Name->setGeometry(40, 16, 47, 36);
                        label_cd_Name->setText("CD" + QString("%1").number(cd_num++));

                        this->vl_tracks->addWidget(tracks_header);
                    }

                    if(i == 0){
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
                    }

                    this->vl_tracks->addWidget(this->album_tracks[i]);
                }
            }

            this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;
            this->list_track->append(list_data);

            qobuz::AlbumItemData tmpAlbum;
            if(this->data_album.title.isEmpty()){
                tmpAlbum.title = list_data.at(0).album_title;
                tmpAlbum.tracks_count = list_data.at(0).album_tracks_count;
                tmpAlbum.image_large = list_data.at(0).album_image_large;
                tmpAlbum.image_small = list_data.at(0).album_image_small;
                tmpAlbum.image_thumbnail = list_data.at(0).album_image_thumbnail;
                tmpAlbum.list_artist_id = list_data.at(0).list_artist_id;
                tmpAlbum.list_artist_name = list_data.at(0).list_artist_name;
                tmpAlbum.id = list_data.at(0).id;

                slot_applyResult_albumInfo(tmpAlbum);
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
            for(int i = 0; i < this->list_track->size(); i++){
                this->album_tracks[i]->setDataTrackInfo_Qobuz(this->list_track->at(i));
                tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));

                QCoreApplication::processEvents();
            }

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzAlbumDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_getRating_Track("QOBUZ", tmp_clientkey);

            this->menubar->setSelectedSubMenuNoSignal(this->contentStep);

            QJsonObject p_data;
            p_data.insert("pageCode", "track");
            slot_changedSubTabUI(p_data);

            this->menubar->show();
            this->stackedwidget->show();

            this->box_main_contents->addSpacing(30);

            this->setUIControl_sameArtsit();
            this->setUIControl_suggestArtsits();

            this->flag_track_ok = true;

            ProcCommon *proc_next = new ProcCommon(this);
            connect(proc_next, &ProcCommon::completeReq_drawnext, this, &QobuzAlbumDetail::slot_applyResult_creditInfo);
            proc_next->request_qobuz_drawNext(1);
        }

        //ContentLoadingwaitingMsgHide();//c220616
    }


    void QobuzAlbumDetail::slot_applyResult_creditInfo(const int idx){

        if(idx > 0){
            int max_cnt = this->list_track->at(0).album_tracks_count;
            int cd_ico_cnt = 0;
            for(int i = 0; i < max_cnt; i++){
                if(this->list_track->at(i).track_number == 1){
                    cd_ico_cnt++;
                }
            }

            int cd_num = 1;
            for(int i = 0; i < max_cnt; i++){
                this->album_credit[i] = new AlbumTrackDetailInfoCredit_RHV;
                connect(this->album_credit[i], &AlbumTrackDetailInfoCredit_RHV::clicked, this, &QobuzAlbumDetail::slot_clickedItemTrack_inList);
                this->album_credit[i]->setProperty("index", i);

                if((cd_ico_cnt > 1) && (this->list_track->at(i).track_number == 1)){
                    QWidget *tracks_header = new QWidget();
                    tracks_header->setFixedSize(1500, 52);
                    tracks_header->setContentsMargins(0, 0, 0, 0);

                    QLabel *label_cd_ico = GSCommon::getUILabelImg(":/images/qobuz/album_cd_ico.png", tracks_header);
                    label_cd_ico->setGeometry(0, 19, 30, 30);

                    QLabel *label_cd_Name = new QLabel(tracks_header);
                    label_cd_Name->setStyleSheet("color:#FFFFFF;font-size:24px;font-weight:bold;");
                    label_cd_Name->setGeometry(40, 16, 47, 36);
                    label_cd_Name->setText("CD" + QString("%1").number(cd_num++));

                    this->vl_credit->addWidget(tracks_header);
                }

                if(i == 0){
                    if(!this->list_track->at(i).work.isEmpty()){
                        QWidget *widget_work = new QWidget();
                        widget_work->setFixedSize(1470, 47);

                        QLabel *label_work = new QLabel(widget_work);
                        label_work->setStyleSheet("color:#FFFFFF;font-size:16px;font-weight:normal;");
                        label_work->setFixedSize(1470, 24);
                        label_work->setGeometry(30, 23, 0, 0);
                        label_work->setText(this->list_track->at(i).work);

                        this->vl_credit->addWidget(widget_work);
                    }
                }
                else{
                    if(this->list_track->at(i-1).work != this->list_track->at(i).work){
                        QWidget *widget_work = new QWidget();
                        widget_work->setFixedSize(1470, 47);

                        if(!this->list_track->at(i).work.isEmpty()){
                            QLabel *label_work = new QLabel(widget_work);
                            label_work->setStyleSheet("color:#FFFFFF;font-size:16px;font-weight:normal;");
                            label_work->setFixedSize(1470, 24);
                            label_work->setGeometry(30, 23, 0, 0);
                            label_work->setText(this->list_track->at(i).work);
                        }

                        this->vl_credit->addWidget(widget_work);
                    }
                }

                this->vl_credit->addWidget(this->album_credit[i]);
            }
        }

        for(int i = 0; i < this->list_track->size(); i++){
            this->album_credit[i]->setDataTrackInfo_Qobuz(this->list_track->at(i));

            QCoreApplication::processEvents();
        }

        this->flag_credit_ok = true;

        if(this->album_credit[0]->getFavoritesStars() != this->album_tracks[0]->getFavoritesStars()){
            for(int i = 0; i < this->list_track->size(); i++){
                int star = this->album_tracks[i]->getFavoritesStars();

                if(star > 0){
                    this->album_credit[i]->setFavoritesIds(true, star);
                }
                else{
                    this->album_credit[i]->setFavoritesIds(false, star);
                }
            }
        }

    }


    void QobuzAlbumDetail::changedOnlyTabUI_notSendSignal(QString p_step){

        this->contentStep = p_step;

        // Tab 타이틀 변경
        this->menubar->setSelectedSubMenuNoSignal(contentStep);

        // DB 가져와서 데이터 세팅
        if(contentStep == ALBTAB_STEP_TRACK){
            this->stackedwidget->setCurrentIndex(0);
            this->stackedwidget->setFixedHeight(this->vl_tracks->sizeHint().height());
        }
        else if(contentStep == ALBTAB_STEP_CREDIT){
            this->stackedwidget->setCurrentIndex(1);
            this->stackedwidget->setFixedHeight(this->vl_credit->sizeHint().height());
        }
    }



    void QobuzAlbumDetail::slot_changedSubTabUI(const QJsonObject &p_data){

        QString tmp_step = p_data[KEY_PAGE_CODE].toString();

        // 페이지 변경 및 데이터 세팅
        this->changedOnlyTabUI_notSendSignal(tmp_step);

    }


    /**
     * @brief [Slot] AlbumImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void QobuzAlbumDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

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

                if(this->album_star_fav == 0 || this->album_star_fav == 1){
                    // Qobuz Favorite toggle
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzAlbumDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("album", QString("%1").arg(data_album.id), this->flag_album_fav);
                    this->flag_send_album = true;
                }

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_album = new roseHome::ProcCommon(this);
                connect(proc_favCheck_album, &roseHome::ProcCommon::completeCheck_rating_album, this, &QobuzAlbumDetail::slot_applyResult_checkRating_album);
                proc_favCheck_album->request_rose_checkRating_Album("QOBUZ", this->data_album.id);
                this->flag_check_album = true;

                this->album_detail_info->setFavorite(this->flag_album_fav, this->album_star_fav);
            }
        }
        else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Info){

            QJsonObject tmpData;
            tmpData.insert("image", data_album.image_large);
            tmpData.insert("title", data_album.title);
            tmpData.insert("artist", data_album.artist_name);
            tmpData.insert("composer", data_album.composer_name);
            tmpData.insert("copyright", data_album.label_name);
            tmpData.insert("released", data_album.release_date_original);
            tmpData.insert("genre", data_album.genre_name);
            tmpData.insert("track", data_album.tracks_count);
            tmpData.insert("duration", data_album.duration);
            tmpData.insert("description", data_album.description);

            this->dlgAlbumInfo = new DialogNotice(this, DialogNotice::DlgNotice::Qobuz_album_info);
            this->dlgAlbumInfo->setData_formJsonObj(tmpData);
            this->dlgAlbumInfo->exec();
        }
        else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Share){

            this->shareLink = this->album_detail_info->detailInfo_shareLink;    //220905 share code ... --

            //220816 share code ... --
            setUIShare();//c220818
        }
        else{
            this->proc_clicked_imageDetailInfo_fromAlbum(this->data_album, this->jsonArr_tracks_toPlay, SECTION_FOR_MORE_POPUP___albums, clickMode);
        }

    }

    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void QobuzAlbumDetail::slot_clickedItemTrack_inList(const int idx, const AlbumTrackDetailInfo_RHV::ClickMode clickMode){
        int section = SECTION_FOR_MORE_POPUP___track;

        if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                int star_cnt1 = this->album_tracks[idx]->getFavoritesStars();
                int star_cnt2 = this->album_credit[idx]->getFavoritesStars();
                this->flag_track_fav = false;

                this->track_star_fav = star_cnt1 > star_cnt2 ? star_cnt1 : star_cnt2;

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
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzAlbumDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                    this->flag_send_track = true;
                }

                this->track_idx_fav = idx;
                QJsonObject jsonObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzAlbumDetail::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("QOBUZ", QString("%1").arg(ProcJsonEasy::getInt(jsonObj, "id")));
                this->flag_check_track = true;


                this->album_tracks[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
                this->album_credit[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
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
    void QobuzAlbumDetail::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_sameArtist, clickMode, index, section);

    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzAlbumDetail::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemArtist(this->list_suggestArtist, clickMode, index, section);


    }


    void QobuzAlbumDetail::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_sameArtist){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = QString("%1 - By the same artist").arg(this->data_album.artist_name);
            data_page.list_sameAritst.append(*this->list_sameArtist);

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_SAME_ARTIST_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_suggestArtist){
            PageInfo_ArtistAllView data_page;
            data_page.pathTitle = QString("%1 - Artist suggestions").arg(this->data_album.artist_name);
            data_page.api_subPath = "artist/getSimilarArtists";
            data_page.artist_id = this->data_album.artist_id;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_artistAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ARTIST_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }

    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void QobuzAlbumDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(data_album, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___sameArtist){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_sameArtist, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___suggestArtist){
            this->proc_clicked_optMorePopup_fromArtist(this->list_suggestArtist, index, clickMode);
        }

    }


    void QobuzAlbumDetail::slot_qobuz_logChange(){

        this->widget_login_contents->hide();

        this->setJsonObject_forData(this->login_wait_Object);
        this->setActivePage();


    }

}
