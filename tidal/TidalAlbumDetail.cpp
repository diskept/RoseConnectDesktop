#include "tidal/TidalAlbumDetail.h"

#include "tidal/ProcCommon.h"
#include "tidal/ConvertData.h"

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

namespace tidal {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int BTN_IDX_SUBTITLE_sameArtist = 3;
    const int BTN_IDX_SUBTITLE_suggestArtist = 4;

    const int SECTION_FOR_MORE_POPUP___tracks = 1;
    const int SECTION_FOR_MORE_POPUP___albums = 2;
    const int SECTION_FOR_MORE_POPUP___sameArtist = 3;
    const int SECTION_FOR_MORE_POPUP___suggestArtist = 4;

    QString ALBTAB_STEP_TRACK = "track";
    QString ALBTAB_STEP_CREDIT = "credit";


    /**
     * @brief
     * @param parent
     */
    TidalAlbumDetail::TidalAlbumDetail(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_filter, parent) {

        linker = Linker::getInstance();

        // data
        this->list_track = new QList<tidal::TrackItemData>();
        this->list_sameArtist = new QList<tidal::AlbumItemData>();
        this->list_suggestArtist = new QList<tidal::ArtistItemData>();

        this->dlgAlbumInfo = new DialogNotice(this, DialogNotice::DlgNotice::Qobuz_album_info);
    }


    /**
     * @brief 소멸자.
     */
    TidalAlbumDetail::~TidalAlbumDetail(){

        this->deleteLater();
    }


    /**
     * @brief TidalAlbumDetail::setJsonObject_forData
     * @param jsonObj
     */
    void TidalAlbumDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        print_debug();
        qDebug() << "jsonObj=" << jsonObj;
        tidal::AlbumItemData tmp_data_album = ConvertData::convertData_albumData(jsonObj);

        this->flagNeedReload = false;

        if(global.user_forTidal.isLogined() == false){

            this->login_wait_Object = QJsonObject();
            this->login_wait_Object = jsonObj;

            this->tidalSettings = new TidalSettings();
            this->tidalSettings->setActivePage();

            this->box_login_contents = new QVBoxLayout();
            this->box_login_contents->setSpacing(0);
            this->box_login_contents->setContentsMargins(0, 0, 0, 0);
            this->box_login_contents->addWidget(this->tidalSettings);

            this->widget_login_contents = new QWidget();
            this->widget_login_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_login_contents->setLayout(this->box_login_contents);

            this->box_contents->addWidget(this->widget_login_contents);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            //connect(this->tidalSettings, &TidalSettings::signal_logState_change, this, &TidalAlbumDetail::slot_tidal_logChange);
            connect(this->linker, SIGNAL(signal_logined_tidal()), this, SLOT(slot_tidal_logChange()));
        }
        else{

            if(tmp_data_album.id != this->data_album.id){

                this->flagNeedReload = true;

                if(this->flag_track_ok == true){
                    this->widget_main_contents->hide();
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

                if(this->data_album.title.isEmpty()){
                    // album 기본 정보를 가져와야 하는 경우임. (id만 전달받은 상태)
                    ProcCommon *proc_album = new ProcCommon(this);
                    connect(proc_album, &ProcCommon::completeReq_album, this, &TidalAlbumDetail::slot_applyResult_albumInfo);
                    proc_album->request_tidal_get_album(this->data_album.id);
                }
                else{
                    // request HTTP API - get favorite for Rose Server
                    roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
                    connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_album, this, &TidalAlbumDetail::slot_applyResult_getRating_album);
                    proc_fav_album->request_rose_getRating_Album("TIDAL", QString("%1").arg(this->data_album.id));

                    // album track 정보를 가져와야 하는 경우임.
                    ProcCommon *proc_tracks = new ProcCommon(this);

                    if(this->data_album.visualAlbum == true){
                        connect(proc_tracks, &ProcCommon::completeReq_list_items_of_visual, this, &TidalAlbumDetail::slot_applyResult_tracks);
                        proc_tracks->request_tidal_getList_items_of_visual(this->data_album.id, this->data_album.numberOfTracks);
                    }
                    else{
                        connect(proc_tracks, &ProcCommon::completeReq_list_items_of_album, this, &TidalAlbumDetail::slot_applyResult_tracks);
                        proc_tracks->request_tidal_getList_items_of_album(this->data_album.id);
                    }
                }

                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

                this->track_favoriteOffset = 0;
            }
            else{
                // 리로드 하지 않는 경우에는, favorite 정보만 다시 요청한다. (album_id 가 변경되지 않고, 페이지가 다시 요청된 경우임)
                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
                connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_album, this, &TidalAlbumDetail::slot_applyResult_getRating_album);
                proc_fav_album->request_rose_getRating_Album("TIDAL", QString("%1").arg(this->data_album.id));

                QList<QString> tmp_clientkey;
                for(int i = 0; i < this->list_track->size(); i++){
                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));
                }

                this->track_favoriteOffset = 0;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalAlbumDetail::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("TIDAL", tmp_clientkey);
            }
        }
    }


    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalAlbumDetail::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            if(this->flag_track_ok == true){
                this->widget_main_contents->hide();

                GSCommon::clearLayout(this->box_main_contents);
                this->box_contents->removeWidget(this->widget_main_contents);

                this->flag_track_ok = false;
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
    void TidalAlbumDetail::setUIControl_basic(){

        // 앨범 상세정보
        this->album_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Tidal_album);
        connect(this->album_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &TidalAlbumDetail::slot_detailInfo_btnClicked);

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

        if(!this->data_album.title.isEmpty() && this->data_album.visualAlbum == false){

            // Update UI
            QString title_ver = "";
            if(this->data_album.version.isEmpty()){
                title_ver = this->data_album.title;
            }
            else{
                title_ver = this->data_album.title + QString(" (%1)").arg(this->data_album.version);
            }

            QJsonObject albumObj;
            albumObj.insert("id", this->data_album.id);
            albumObj.insert("audioQuality", this->data_album.audioQuality);
            albumObj.insert("releaseDate", this->data_album.releaseDate);
            albumObj.insert("streamStartDate", this->data_album.streamStartDate);
            albumObj.insert("streamReady", this->data_album.streamReady);

            albumObj.insert("duration", this->data_album.duration);

            albumObj.insert("image", this->data_album.image);

            albumObj.insert("title", this->data_album.title);
            albumObj.insert("version", this->data_album.version);

            albumObj.insert("track_count", this->data_album.numberOfTracks);

            QJsonArray jsonArr_name;
            foreach(QString tmp_name, this->data_album.list_artist_name){
                jsonArr_name.append(tmp_name);
            }
            albumObj.insert("artist", jsonArr_name);

            this->album_detail_info->setData_fromTidalData(albumObj);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
            connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_album, this, &TidalAlbumDetail::slot_applyResult_getRating_album);
            proc_fav_album->request_rose_getRating_Album("TIDAL", QString("%1").arg(this->data_album.id));


        }
    }


    void TidalAlbumDetail::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId){

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


    QHBoxLayout* TidalAlbumDetail::setUIControl_hBoxLayout_forAlbum(){
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


    QHBoxLayout* TidalAlbumDetail::setUIControl_hBoxLayout_forArtist(){
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


    void TidalAlbumDetail::setUIControl_sameArtsit(){

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
    }

    void TidalAlbumDetail::setUIControl_suggestArtsits(){

        if(this->list_suggestArtist->count() > 0){
            this->box_main_contents->addSpacing(70);

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
        }
        else{
            this->box_main_contents->addSpacing(30);
        }

    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief TidalAlbumDetail::slot_applyResult_albumInfo
     * @param data_album
     */
    void TidalAlbumDetail::slot_applyResult_albumInfo(tidal::AlbumItemData data_album){

        if(data_album.title == "error"){

            QJsonObject albumObj;
            albumObj.insert("id", "");
            albumObj.insert("audioQuality", "");
            albumObj.insert("releaseDate", "");
            albumObj.insert("streamStartDate", "");
            albumObj.insert("streamReady", "");
            albumObj.insert("duration", 0);
            albumObj.insert("image", "");
            albumObj.insert("title", "");
            albumObj.insert("version", "");
            albumObj.insert("track_count", 0);
            albumObj.insert("artist", "");

            this->album_detail_info->setData_fromTidalData(albumObj);

            this->flag_track_ok = true;
            ContentLoadingwaitingMsgHide();

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

            this->dlgAlbumInfo->setData_formJsonObj(tmpData);
            this->dlgAlbumInfo->exec();
        }
        else{
            // Update Data
            this->data_album = data_album;

            if(this->data_album.visualAlbum == false){
                QJsonObject albumObj;
                albumObj.insert("id", this->data_album.id);
                print_debug();
                qDebug() << "this->data_album.id=" << this->data_album.id;
                albumObj.insert("audioQuality", this->data_album.audioQuality);
                albumObj.insert("releaseDate", this->data_album.releaseDate);
                albumObj.insert("streamStartDate", this->data_album.streamStartDate);
                albumObj.insert("streamReady", this->data_album.streamReady);

                albumObj.insert("duration", this->data_album.duration);

                albumObj.insert("image", this->data_album.image);

                albumObj.insert("title", this->data_album.title);
                albumObj.insert("version", this->data_album.version);
                albumObj.insert("track_count", this->data_album.numberOfTracks);

                QJsonArray jsonArr_name;
                foreach(QString tmp_name, this->data_album.list_artist_name){
                    jsonArr_name.append(tmp_name);
                }
                albumObj.insert("artist", jsonArr_name);

                this->album_detail_info->setData_fromTidalData(albumObj);
            }

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
            connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_album, this, &TidalAlbumDetail::slot_applyResult_getRating_album);
            proc_fav_album->request_rose_getRating_Album("TIDAL", QString("%1").arg(this->data_album.id));
print_debug();



            // album track 정보를 가져와야 하는 경우임.
            ProcCommon *proc_tracks = new ProcCommon(this);

            if(this->data_album.visualAlbum == true){
                connect(proc_tracks, &ProcCommon::completeReq_list_items_of_visual, this, &TidalAlbumDetail::slot_applyResult_tracks);
                proc_tracks->request_tidal_getList_items_of_visual(this->data_album.id, this->data_album.numberOfTracks);
            }
            else{
                connect(proc_tracks, &ProcCommon::completeReq_list_items_of_album, this, &TidalAlbumDetail::slot_applyResult_tracks);
                proc_tracks->request_tidal_getList_items_of_album(this->data_album.id);
            }
        }
    }


    void TidalAlbumDetail::slot_applyResult_getShareLink(const QString &link){//c220818
print_debug();
        this->shareLink = link;
    }

    void TidalAlbumDetail::slot_applyResult_sameArtist(const QList<tidal::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(flag_lastPage);
        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.count() > 0){
            this->list_sameArtist->append(list_data);

            for(int i = 0; i < 15; i++){
                this->sameArtist[i] = new tidal::ItemAlbum(i, SECTION_FOR_MORE_POPUP___sameArtist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->sameArtist[i], &tidal::ItemAlbum::signal_clicked, this, &TidalAlbumDetail::slot_clickedItemAlbum);
            }

            this->box_main_contents->addSpacing(70);
            this->setUIControl_sameArtsit();
        }

        setUIControl_suggestArtsits();
    }


    void TidalAlbumDetail::slot_applyResult_suggestArtsit(const QList<tidal::ArtistItemData> &list_data, const bool flag_lastPage){
        Q_UNUSED(flag_lastPage);

        if(list_data.count() > 0){
            this->list_suggestArtist->append(list_data);

            for(int i = 0; i < 15; i++){
                this->artistSuggestions[i] = new tidal::ItemArtist(i, SECTION_FOR_MORE_POPUP___suggestArtist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->artistSuggestions[i], &tidal::ItemArtist::signal_clicked, this, &TidalAlbumDetail::slot_clickedItemArtist);
            }
        }
    }


    /**
     * @brief TidalAlbumDetail::slot_tidal_completeReq_listAll_myFavoritesIds
     * @param p_jsonObj
     */
    void TidalAlbumDetail::slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            // Tidal favorite check
            if(this->flag_send_album == true){
                QVariantList arr_myFavoriteIds = ProcJsonEasy::getJsonArray(p_jsonObj, "ALBUM").toVariantList();
                bool status = arr_myFavoriteIds.contains(data_album.id);

                if(status == true && this->flag_album_fav == false){
                    // Tidal album Favorite del
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalAlbumDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_del_favorite("albums", QString("%1").arg(data_album.id));
                }
                else if(status == false && this->flag_album_fav == true){
                        // Tidal album Favorite add
                        ProcCommon *proc = new ProcCommon(this);
                        connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalAlbumDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                        proc->request_tidal_set_favorite("albums", QString("%1").arg(data_album.id));
                }
                this->flag_send_album = false;
            }
            else if(this->flag_send_track == true){
                QVariantList arr_myFavoriteIds = ProcJsonEasy::getJsonArray(p_jsonObj, "TRACK").toVariantList();
                bool status = arr_myFavoriteIds.contains(this->track_id_fav);

                if(status == true && this->flag_track_fav == false){
                    // Tidal track Favorite del
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalAlbumDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                else if(status == false && this->flag_track_fav == true){
                    // Tidal track Favorite add
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalAlbumDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                this->flag_send_track = false;
            }
            else{

            }
        }
    }


    void TidalAlbumDetail::slot_applyResult_checkRating_album(const QJsonObject &jsonObj){

        int id = ProcJsonEasy::getInt(jsonObj, "id");
print_debug();
        if(id <= 0){

            QJsonObject album;
            album.insert("type", "TIDAL");
            album.insert("title", this->data_album.title);
            album.insert("star", 1);
            album.insert("favorite", true);
            album.insert("clientKey", QString("%1").arg(this->data_album.id));
            album.insert("thumbnail", this->data_album.image);
            album.insert("trackCount", this->data_album.numberOfTracks);
            album.insert("duration", this->data_album.duration);

            QJsonObject json;
            json.insert("album", album);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
            connect(proc_fav_album, &roseHome::ProcCommon::completeCheck_rating_album, this, &TidalAlbumDetail::slot_applyResult_addRating_album);
            proc_fav_album->request_rose_addRating_Album(json);
        }
        else{
            //{"album":{"favorite":false,"id":347230,"star":0,"trackCount":0,"type":"TIDAL"},
            //"ratingInfo":{"favorite":false,"star":0,"thumbup":false,"type":"TIDAL"}}
            QJsonObject album;
            album.insert("favorite", false);
            album.insert("id", id);
            album.insert("star", 0);
            album.insert("trackCount", 0);
            album.insert("type", "TIDAL");

            QJsonObject ratingInfo;
            ratingInfo.insert("favorite", this->flag_album_fav);
            ratingInfo.insert("star", this->album_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "TIDAL");

            QJsonObject json;
            json.insert("album", album);
            json.insert("ratingInfo", ratingInfo);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
            connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_album, this, &TidalAlbumDetail::slot_applyResult_getRating_album);
            proc_fav_album->request_rose_setRating_Album(json, this->flag_album_fav, this->album_star_fav);
        }
    }


    void TidalAlbumDetail::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        int id = ProcJsonEasy::getInt(jsonObj, "id");

        QJsonObject jsonObj_track = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();
        if(id <= 0){

            /*QJsonObject tmp_album = ProcJsonEasy::getJsonObject(jsonObj_track, "album");
            QJsonObject album;
            album.insert("cover", ProcJsonEasy::getString(tmp_album, "cover"));
            album.insert("id", ProcJsonEasy::getInt(tmp_album, "id"));
            album.insert("releaseDate", ProcJsonEasy::getString(tmp_album, "releaseDate"));
            album.insert("title", ProcJsonEasy::getString(tmp_album, "title"));

            QJsonObject tmp_artist = ProcJsonEasy::getJsonObject(jsonObj_track, "artist");
            QJsonObject artist;
            artist.insert("id", ProcJsonEasy::getInt(tmp_artist, "id"));
            artist.insert("name", ProcJsonEasy::getString(tmp_artist, "name"));
            artist.insert("picture", ProcJsonEasy::getInt(tmp_artist, "picture"));

            QJsonArray tmp_artists = ProcJsonEasy::getJsonArray(jsonObj_track, "artists");
            QJsonArray artists;
            for(int i = 0; i < tmp_artists.size(); i++){
                QJsonObject tmpArtist = tmp_artists.at(i).toObject();

                QJsonObject artist;
                artist.insert("id", ProcJsonEasy::getInt(tmpArtist, "id"));
                artist.insert("name", ProcJsonEasy::getString(tmpArtist, "name"));
                artist.insert("picture", ProcJsonEasy::getInt(tmpArtist, "picture"));

                artists.append(artist);
            }

            QJsonObject data;
            data.insert("album", album);
            data.insert("artist", artist);
            data.insert("artists", artists);
            data.insert("audioQuality", ProcJsonEasy::getString(jsonObj_track, "audioQuality"));
            data.insert("copyright", ProcJsonEasy::getString(jsonObj_track, "copyright"));
            data.insert("description", ProcJsonEasy::getString(jsonObj_track, "description"));

            QString thumbnail = ProcJsonEasy::getString(tmp_album, "cover");
            QString thumbnailUrl = tidal::ConvertData::convertImagePath(thumbnail, tidal::ConvertData::ImageSize_Square::Size_750x750);

            QJsonObject track;
            track.insert("clientKey", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            track.insert("data", data);
            track.insert("duration", ProcJsonEasy::getInt(jsonObj_track, "duration"));
            track.insert("favorite", this->flag_track_fav);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 1);
            track.insert("thumbnailUrl", thumbnailUrl);
            track.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            track.insert("type", "TIDAL");*/

            QJsonArray tmp_artists = ProcJsonEasy::getJsonArray(jsonObj_track, "artists");
            QJsonArray artists;
            for(int i = 0; i < tmp_artists.size(); i++){
                QJsonObject tmpArtist = tmp_artists.at(i).toObject();

                QJsonObject artist;
                artist.insert("name", ProcJsonEasy::getString(tmpArtist, "name"));

                artists.append(artist);
            }

            QJsonObject tmp_album = ProcJsonEasy::getJsonObject(jsonObj_track, "album");
            QString thumbnail = ProcJsonEasy::getString(tmp_album, "cover");
            QString thumbnailUrl = tidal::ConvertData::convertImagePath(thumbnail, tidal::ConvertData::ImageSize_Square::Size_320x320);

            QJsonObject track;
            track.insert("artists", artists);
            track.insert("clientKey", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            track.insert("comment", "");

            track.insert("data", jsonObj_track);

            track.insert("duration", ProcJsonEasy::getInt(jsonObj_track, "duration"));
            track.insert("favorite", false);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 0);
            track.insert("thumbnailUrl", thumbnailUrl);
            track.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            track.insert("type", "TIDAL");

            QJsonArray tracks;
            tracks.append(track);

            QJsonObject json;
            json.insert("tracks", tracks);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalAlbumDetail::slot_applyResult_addRating_track);
            proc_fav_track->request_rose_addRating_Track(json);
        }
        else{
            //{"ratingInfo":{"favorite":true,"star":1,"thumbup":false,"type":"TIDAL"},
            //"track":{"duration":0,"favorite":false,"id":2546294,"ownerId":0,"sort":0,"star":0,"type":"TIDAL"}}
            QJsonObject ratingInfo;
            ratingInfo.insert("favorite", this->flag_track_fav);
            ratingInfo.insert("star", this->track_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "TIDAL");

            QJsonObject track;
            track.insert("duration", 0);
            track.insert("favorite", false);
            track.insert("id", id);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 0);
            track.insert("type", "TIDAL");

            QJsonObject json;
            json.insert("ratingInfo", ratingInfo);
            json.insert("track", track);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalAlbumDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

        }
    }


    void TidalAlbumDetail::slot_applyResult_addRating_album(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_album == true){
                this->flag_check_album = false;
            }
        }
        else{
            this->flag_check_album = false;
        }
    }


    void TidalAlbumDetail::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void TidalAlbumDetail::slot_applyResult_getRating_album(const QJsonArray &contents){

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


    void TidalAlbumDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

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
     * @brief TidalAlbumDetail::slot_applyResult_tracks
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void TidalAlbumDetail::slot_applyResult_tracks(const QList<tidal::TrackItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() <= 0){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->vl_tracks->addWidget(noData_widget, 0, Qt::AlignTop);

            this->menubar->setSelectedSubMenuNoSignal(this->contentStep);

            QJsonObject p_data;
            p_data.insert("pageCode", "track");
            slot_changedSubTabUI(p_data);

            this->menubar->show();
            this->stackedwidget->show();

            this->flag_track_ok = true;
        }
        else{
            if(this->data_album.visualAlbum == true){
                // Update UI
                QString title_ver = "";
                if(this->data_album.version.isEmpty()){
                    title_ver = this->data_album.title;
                }
                else{
                    title_ver = this->data_album.title + QString(" (%1)").arg(this->data_album.version);
                }

                QJsonObject albumObj;
                albumObj.insert("id", this->data_album.id);
                albumObj.insert("audioQuality", this->data_album.audioQuality);
                albumObj.insert("releaseDate", this->data_album.releaseDate);
                albumObj.insert("streamStartDate", this->data_album.streamStartDate);
                albumObj.insert("streamReady", this->data_album.streamReady);

                albumObj.insert("duration", this->data_album.duration);

                albumObj.insert("image", this->data_album.image);

                albumObj.insert("title", this->data_album.title);
                albumObj.insert("version", this->data_album.version);

                this->data_album.numberOfTracks = list_data.size();
                albumObj.insert("track_count", this->data_album.numberOfTracks);

                QJsonArray jsonArr_name;
                foreach(QString tmp_name, this->data_album.list_artist_name){
                    jsonArr_name.append(tmp_name);
                }
                albumObj.insert("artist", jsonArr_name);

                this->album_detail_info->setData_fromTidalData(albumObj);
            }

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();

                int max_cnt = list_data.at(0).totalCount;
                int cd_ico_cnt = 0;
                if(this->data_album.visualAlbum == false){
                    for(int i = 0; i < max_cnt; i++){
                        if(list_data.at(i).trackNumber == 1){
                            cd_ico_cnt++;
                        }
                    }
                }

                int cd_num = 1;
                for(int i = 0; i < max_cnt; i++){
                    this->album_tracks[i] = new AlbumTrackDetailInfo_RHV;
                    connect(this->album_tracks[i], &AlbumTrackDetailInfo_RHV::clicked, this, &TidalAlbumDetail::slot_clickedItemTrack_inList);
                    this->album_tracks[i]->setProperty("index", i);

                    if((cd_ico_cnt > 1) && (list_data.at(i).trackNumber == 1)){
                        QWidget *tracks_header = new QWidget();
                        tracks_header->setFixedSize(1500, 52);
                        tracks_header->setContentsMargins(0, 0, 0, 0);

                        QLabel *label_cd_ico = GSCommon::getUILabelImg(":/images/qobuz/album_cd_ico.png", tracks_header);
                        label_cd_ico->setGeometry(0, 19, 30, 30);

                        int label_width = 0;
                        QLabel *label_cd_Name = new QLabel(tracks_header);
                        label_cd_Name->setStyleSheet("color:#FFFFFF;font-size:24px;font-weight:bold;");
                        label_cd_Name->setText("Volume " + QString("%1").number(cd_num++));
                        label_width = label_cd_Name->sizeHint().width();
                        label_cd_Name->setGeometry(40, 16, label_width, 36);

                        this->vl_tracks->addWidget(tracks_header);
                    }

                    this->vl_tracks->addWidget(this->album_tracks[i]);
                }
            }

            this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;
            this->list_track->append(list_data);

            if(this->data_album.title.isEmpty()){
                data_album.title = list_data.at(0).albumName;
                data_album.numberOfTracks = list_data.at(0).totalCount;
                data_album.image = list_data.at(0).image;
                data_album.list_artist_id = list_data.at(0).list_artist_id;
                data_album.list_artist_name = list_data.at(0).list_artist_name;

                slot_applyResult_albumInfo(data_album);
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
                this->album_tracks[i]->setDataTrackInfo_Tidal(this->list_track->at(i));
                tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));

                QCoreApplication::processEvents();
            }

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalAlbumDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_getRating_Track("TIDAL", tmp_clientkey);

            this->menubar->setSelectedSubMenuNoSignal(this->contentStep);

            QJsonObject p_data;
            p_data.insert("pageCode", "track");
            slot_changedSubTabUI(p_data);

            this->menubar->show();
            this->stackedwidget->show();

            this->box_main_contents->addSpacing(30);

            this->flag_track_ok = true;

            /*ProcCommon *proc_next = new ProcCommon(this);
            connect(proc_next, &ProcCommon::completeReq_drawnext, this, &QobuzAlbumDetail::slot_applyResult_creditInfo);
            proc_next->request_qobuz_drawNext(1);*/
        }

        ContentLoadingwaitingMsgHide();      //cheon Tidal
    }



    void TidalAlbumDetail::changedOnlyTabUI_notSendSignal(QString p_step){

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



    void TidalAlbumDetail::slot_changedSubTabUI(const QJsonObject &p_data){

        QString tmp_step = p_data[KEY_PAGE_CODE].toString();

        // 페이지 변경 및 데이터 세팅
        this->changedOnlyTabUI_notSendSignal(tmp_step);
    }


    /**
     * @brief [Slot] AlbumImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void TidalAlbumDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

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

                if(this->album_star_fav == 1){
                    // Tidal album Favorite add
                    ProcCommon *proc = new ProcCommon(this);
                    proc->request_tidal_set_favorite("albums", QString("%1").arg(this->data_album.id));
                }
                else if(this->album_star_fav == 0){
                    // Tidal album Favorite del
                    ProcCommon *proc = new ProcCommon(this);
                    proc->request_tidal_del_favorite("albums", QString("%1").arg(this->data_album.id));
                }

                // favorite 정보 가져오기
                ProcCommon *proc_fav = new ProcCommon(this);
                connect(proc_fav, &ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalAlbumDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                proc_fav->request_tidal_getAll_favorites();
                this->flag_send_album = true;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_album = new roseHome::ProcCommon(this);
                connect(proc_favCheck_album, &roseHome::ProcCommon::completeCheck_rating_album, this, &TidalAlbumDetail::slot_applyResult_checkRating_album);
                proc_favCheck_album->request_rose_checkRating_Album("TIDAL", QString("%1").arg(this->data_album.id));
                this->flag_check_album = true;

                this->album_detail_info->setFavorite(this->flag_album_fav, this->album_star_fav);
            }
        }
        else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Share){//c220818

            this->shareLink = this->album_detail_info->detailInfo_shareLink;    //220905 share code ... --

            //220816 share code ...
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
    void TidalAlbumDetail::slot_clickedItemTrack_inList(const int idx, const AlbumTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___tracks;

        if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->album_tracks[idx]->getFavoritesStars();
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
                connect(proc_fav, &ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalAlbumDetail::slot_tidal_completeReq_listAll_myFavoritesIds);
                proc_fav->request_tidal_getAll_favorites();
                this->flag_send_track = true;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalAlbumDetail::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("TIDAL", QString("%1").arg(this->track_id_fav));
                this->flag_check_track = true;

                this->album_tracks[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
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
    void TidalAlbumDetail::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_sameArtist, clickMode, index, section);
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalAlbumDetail::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemArtist(this->list_suggestArtist, clickMode, index, section);

    }


    void TidalAlbumDetail::slot_clickBtn_subTitle_viewAll(){

        /*int btnId = sender()->property("idx").toInt();

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
        }*/
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void TidalAlbumDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->data_album, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___sameArtist){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_sameArtist, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___suggestArtist){
            this->proc_clicked_optMorePopup_fromArtist(this->list_suggestArtist, index, clickMode);
        }
    }


    void TidalAlbumDetail::slot_tidal_logChange(){

        this->widget_login_contents->hide();

        this->setJsonObject_forData(this->login_wait_Object);
        this->setActivePage();
    }

}
