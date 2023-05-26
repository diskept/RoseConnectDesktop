#include "bugs/BugsAlbumDetail.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"

#include <QScroller>
#include <QScrollArea>
#include <QPushButton>


namespace bugs {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int BTN_IDX_SUBTITLE_aritstAlbum = 3;

    const int SECTION_FOR_MORE_POPUP___albums = 1;
    const int SECTION_FOR_MORE_POPUP___track = 2;
    const int SECTION_FOR_MORE_POPUP___aritstAlbum = 3;

    QString ALBTAB_STEP_TRACK = "track";
    QString ALBTAB_STEP_CREDIT = "credit";


    /**
     * @brief
     * @param parent
     */
    BugsAlbumDetail::BugsAlbumDetail(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_filter, parent) {

        this->linker = Linker::getInstance();
        // data
        this->list_track = new QList<bugs::TrackItemData>();
        this->list_aritst_album = new QList<bugs::AlbumItemData>();
    }


    /**
     * @brief 소멸자.
     */
    BugsAlbumDetail::~BugsAlbumDetail(){

        this->deleteLater();
    }


    /**
     * @brief BugsAlbumDetail::setJsonObject_forData
     * @param jsonObj
     */
    void BugsAlbumDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        bugs::AlbumItemData tmp_data_album = ConvertData_forBugs::convertData_albumData(jsonObj);

        this->flagNeedReload = false;

        if(tmp_data_album.album_id != this->data_album.album_id){
            this->flagNeedReload = true;

            if(this->flag_track_ok == true){
                this->widget_main_contents->hide();

                this->box_contents->removeWidget(this->widget_main_contents);
                GSCommon::clearLayout(this->box_main_contents);

                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->flag_track_ok = false;
            }
            else{
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);
            }

            this->data_album = tmp_data_album;

            this->list_track->clear();
            this->list_aritst_album->clear();
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
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief BugsAlbumDetail::setActivePage
     */
    void BugsAlbumDetail::setActivePage(){

        if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;
            }

            // 필요에 따라 리로드 한다.
            if(this->flagNeedReload){

                // 항상 부모클래스의 함수 먼저 호출
                AbstractBugsSubWidget::setActivePage();

                this->box_main_contents = new QVBoxLayout();
                this->box_main_contents->setSpacing(0);
                this->box_main_contents->setContentsMargins(0, 0, 0, 0);

                this->widget_main_contents = new QWidget();
                this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_main_contents->setLayout(this->box_main_contents);

                this->box_contents->addWidget(this->widget_main_contents);
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                this->flag_credit_ok = false;

                this->setUIControl_basic();

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

                // request HTTP API
                if(this->data_album.title.isEmpty()){
                    // Album 기본 정보를 가져와야 하는 경우임. (id만 전달받은 상태)
                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    connect(proc, &ProcBugsAPI::completeReq_album, this, &BugsAlbumDetail::slot_applyResult_albumInfo);
                    proc->request_bugs_get_album(this->data_album.album_id);
                }
            }
        }
        else if(this->is_bugs_logined() == false){
            this->m_flagCurrLogined = false;

            GSCommon::clearLayout(this->box_contents);      // Layout 초기화
            this->setUIControl_notLogin();
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
    void BugsAlbumDetail::setUIControl_basic(){

        this->album_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Bugs_album);
        connect(this->album_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &BugsAlbumDetail::slot_detailInfo_btnClicked);

        this->album_detail_info->initView();

        this->box_main_contents->addWidget(this->album_detail_info);

        // 앨범의 트랙정보 UI
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

        this->vBox_artistAlbum = new QVBoxLayout();
        this->hBox_artistAlbum = new QHBoxLayout();

        // clear UI
        GSCommon::clearLayout(this->vBox_artistAlbum);
        GSCommon::clearLayout(this->hBox_artistAlbum);

        for(int i = 0; i < 10; i++){
            this->lb_subTitle[i] = new QLabel();
            this->btnView_all[i] = new QPushButton();
        }

        for(int i = 0; i < 15; i++){
            this->artistAlbum[i] = new bugs::ItemAlbum_bugs(i, SECTION_FOR_MORE_POPUP___aritstAlbum, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->artistAlbum[i], &bugs::ItemAlbum_bugs::signal_clicked, this, &BugsAlbumDetail::slot_clickedItemAlbum);
        }

        if(!this->data_album.title.isEmpty()){
            this->slot_applyResult_albumInfo(this->data_album);
        }
    }


    void BugsAlbumDetail::setUIControl_appendWidget(){

        if(this->list_aritst_album->size() > 0){
            this->widget_artistAlbum = new QWidget();
            this->widget_artistAlbum = this->setUIControl_subTitle_withSideBtn(QString("앨범 (%1)").arg(this->list_aritst_album->at(0).total_count), "View All", BTN_IDX_SUBTITLE_aritstAlbum, this->vBox_artistAlbum);

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
    }


    QWidget* BugsAlbumDetail::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QHBoxLayout* BugsAlbumDetail::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

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
    void BugsAlbumDetail::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_aritstAlbum){
            if(this->list_aritst_album->size() > 0){
                if(global.user_forBugs.flag_rosehome == true){
                    global.user_forBugs.rosehome_obj = QJsonObject();
                    global.user_forBugs.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ALBUM_ALL_VIEW___OF_ARTIST);
                    PageInfo_AlbumAllView_ofArtist data_page;
                    data_page.artist_id = this->data_album.list_artist_id.at(0);
                    data_page.filterOpt = QVariant::fromValue(ProcBugsAPI::BugsArtist_Albumfilter::Release).toString();
                    data_page.mainTitle = "앨범";

                    QJsonObject tmp_data = ConvertData_forBugs::getObjectJson_pageInfo_albumAllView_ofArtist(data_page);
                    global.user_forBugs.rosehome_obj.insert(KEY_DATA, tmp_data);

                    emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Bugs));
                }
                else{
                    // Album All View (of Artist) 상세 페이지 이동
                    PageInfo_AlbumAllView_ofArtist data_page;
                    data_page.artist_id = this->data_album.list_artist_id.at(0);
                    data_page.filterOpt = QVariant::fromValue(ProcBugsAPI::BugsArtist_Albumfilter::Release).toString();
                    data_page.mainTitle = "앨범";

                    QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_pageInfo_albumAllView_ofArtist(data_page);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ALBUM_ALL_VIEW___OF_ARTIST);
                    emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                }
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief BugsAlbumDetail::slot_applyResult_albumInfo
     * @param data_album
     */
    void BugsAlbumDetail::slot_applyResult_albumInfo(bugs::AlbumItemData data_album){

        // Update Data
        this->data_album = data_album;

        QJsonObject albumObj;
        albumObj.insert("id", this->data_album.album_id);
        albumObj.insert("releaseDate", this->data_album.upd_dt);
        albumObj.insert("track_count", this->data_album.track_count);

        albumObj.insert("image", this->data_album.album_image);
        albumObj.insert("title", this->data_album.title);

        albumObj.insert("description", this->data_album.descr);

        QJsonArray jsonArr_name;
        foreach(QString tmp_name, this->data_album.list_artist_nm){
            jsonArr_name.append(tmp_name);
        }
        albumObj.insert("artist", jsonArr_name);

        this->album_detail_info->setData_fromBugsData(albumObj);

        // request HTTP API - get favorite for Rose Server
        roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
        connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_album, this, &BugsAlbumDetail::slot_applyResult_getRating_album);
        proc_fav_album->request_rose_getRating_Album("BUGS", QString("%1").arg(this->data_album.album_id));

        // get Tracks of the album
        ProcBugsAPI *proc_track = new ProcBugsAPI(this);
        connect(proc_track, &ProcBugsAPI::completeReq_list_items_of_album, this, &BugsAlbumDetail::slot_applyResult_tracks);
        proc_track->request_bugs_getList_items_of_album(this->data_album.album_id);

        // share link  //c221209  start
        QString param = "BUGS/ALBUM/" + QString("%1").arg(this->data_album.album_id);
        QString desc = this->data_album.list_artist_nm.join(",");

        roseHome::ProcCommon *proc_link = new roseHome::ProcCommon(this);
        connect(proc_link, &roseHome::ProcCommon::completeReq_share_link, this, &BugsAlbumDetail::slot_applyResult_getShareLink);
        proc_link->request_rose_get_shareLink(this->data_album.album_image, desc, this->data_album.title, param);
        //c221209  end

        this->track_favoriteOffset = 0;
    }


    void BugsAlbumDetail::slot_add_rosePlaylist_withBugs(const int &idx, const QJsonObject &dataObj){

        QString view_type = "";
        if(idx < 0){
            view_type = "create";
        }
        else{
            view_type = "add";
        }

        if(global.user_forBugs.flag_rosehome == false){

            QJsonObject jsonObj_move = dataObj;
            jsonObj_move.insert("view_type", view_type);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_PLAYLIST_ADD);

            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else{

            QJsonObject data = dataObj;
            data.insert("view_type", view_type);
            data.insert("type", "BUGS");

            QJsonObject jsonObj_move;
            jsonObj_move.insert("data", data);

            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RH_ADD_PLAYLIST);

            emit this->linker->signal_clicked_movePage(jsonObj_move);
        }
    }


    void BugsAlbumDetail::slot_applyResult_getShareLink(const QString &link){//c220818

        this->shareLink = link;
    }


    /**
     * @brief BugsAlbumDetail::slot_applyResult_tracks
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void BugsAlbumDetail::slot_applyResult_tracks(const QList<bugs::TrackItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        ContentLoadingwaitingMsgHide();      //cheon Tidal

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

                this->album_track_count = list_data.at(0).total_count;
                int max_cnt = list_data.at(0).total_count;
                int cd_ico_cnt = 0;
                for(int i = 0; i < max_cnt; i++){
                    if(list_data.at(i).track_no == 1){
                        cd_ico_cnt++;
                    }
                }

                int cd_num = 1;
                for(int i = 0; i < max_cnt; i++){
                    this->album_tracks[i] = new AlbumTrackDetailInfo_RHV;
                    connect(this->album_tracks[i], &AlbumTrackDetailInfo_RHV::clicked, this, &BugsAlbumDetail::slot_clickedItemTrack_inList);
                    this->album_tracks[i]->setProperty("index", i);

                    if((cd_ico_cnt > 1) && (list_data.at(i).track_no == 1)){
                        QWidget *tracks_header = new QWidget();
                        tracks_header->setFixedSize(1500, 52);
                        tracks_header->setContentsMargins(0, 0, 0, 0);

                        QLabel *label_cd_ico = GSCommon::getUILabelImg(":/images/qobuz/album_cd_ico.png", tracks_header);
                        label_cd_ico->setGeometry(0, 19, 30, 30);

                        int label_width = 0;
                        QLabel *label_cd_Name = new QLabel(tracks_header);
                        label_cd_Name->setStyleSheet("color:#FFFFFF;font-size:24px;font-weight:bold;");
                        label_cd_Name->setText("CD " + QString("%1").number(cd_num++));
                        label_width = label_cd_Name->sizeHint().width();
                        label_cd_Name->setGeometry(40, 16, label_width, 36);

                        this->vl_tracks->addWidget(tracks_header);
                    }

                    this->vl_tracks->addWidget(this->album_tracks[i]);
                }
            }

            this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;
            this->list_track->append(list_data);

            int tmpDuration = 0;
            QList<QString> tmp_clientkey;
            for(int i = 0; i < this->list_track->size(); i++){
                this->album_tracks[i]->setDataTrackInfo_Bugs(this->list_track->at(i));
                tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).track_id));

                QStringList strDuration = this->list_track->at(i).len.split(":");

                if(strDuration.size() == 3){
                    tmpDuration += (strDuration.at(0).toInt() * 3600) + (strDuration.at(1).toInt() * 60) + strDuration.at(2).toInt();
                }
                else if(strDuration.size() == 2){
                    tmpDuration += (strDuration.at(0).toInt() * 60) + strDuration.at(1).toInt();
                }
                else{
                    tmpDuration += strDuration.at(0).toInt();
                }

                QCoreApplication::processEvents();
            }

            if(this->data_album.total_count <= 0){
                QJsonObject tmpAlbum;
                tmpAlbum.insert("track_count", this->album_track_count);
                tmpAlbum.insert("duration", tmpDuration);
                tmpAlbum.insert("registDateTime", this->data_album.upd_dt);

                this->album_detail_info->setData_Resolution(tmpAlbum);
            }

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsAlbumDetail::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_getRating_Track("BUGS", tmp_clientkey);

            if(this->contentStep == "track"){
                this->widget_tab_tracks->show();
                this->stackedwidget->setFixedHeight(this->vl_tracks->sizeHint().height());
            }
            else{
                this->widget_tab_credit->show();
                this->stackedwidget->setFixedHeight(this->vl_credit->sizeHint().height());
            }

            this->menubar->setSelectedSubMenuNoSignal(this->contentStep);

            QJsonObject p_data;
            p_data.insert("pageCode", "track");
            this->slot_changedSubTabUI(p_data);

            this->menubar->show();
            this->stackedwidget->show();

            this->flag_track_ok = true;
        }

        // filtering
        ProcBugsAPI::BugsArtist_Albumfilter tmp_filter = ProcBugsAPI::BugsArtist_Albumfilter::Release;          // 기본값

        // getting album list - request HTTP API
        ProcBugsAPI *proc = new ProcBugsAPI(this);
        connect(proc, &ProcBugsAPI::completeReq_list_albums, this, &BugsAlbumDetail::slot_applyResult_albums);
        proc->request_bugs_getList_albums_of_artist(this->data_album.list_artist_id[0], 15, 1, tmp_filter);
    }


    /**
     * @brief BugsAlbumDetail::slot_applyResult_albums
     * @param list_data
     */
    void BugsAlbumDetail::slot_applyResult_albums(const QList<bugs::AlbumItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.count() > 0){
            this->list_aritst_album->append(list_data);

            this->box_main_contents->addSpacing(30);
            this->setUIControl_appendWidget();
        }
    }


    void BugsAlbumDetail::slot_applyResult_checkRating_album(const QJsonObject &jsonObj){

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
                ratingInfo.insert("type", "BUGS");

                QJsonObject json;
                json.insert("album", album);
                json.insert("ratingInfo", ratingInfo);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
                connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_album, this, &BugsAlbumDetail::slot_applyResult_getRating_album);
                proc_fav_album->request_rose_setRating_Album(json, this->flag_album_fav, this->album_star_fav);
            }
            else{

                roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
                connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_album, this, &BugsAlbumDetail::slot_applyResult_getRating_album);
                proc_fav_album->request_rose_getRating_Album("BUGS", QString("%1").arg(this->data_album.album_id));
            }
        }
        else{

            QJsonObject album;
            album.insert("clientKey", this->data_album.album_id);
            album.insert("favorite", false);
            album.insert("star", 0);
            album.insert("thumbnail", this->data_album.album_image);
            album.insert("title", this->data_album.title);
            album.insert("trackCount", this->album_track_count);
            album.insert("type", "BUGS");

            QJsonObject json;
            json.insert("album", album);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
            connect(proc_fav_album, &roseHome::ProcCommon::completeCheck_rating_album, this, &BugsAlbumDetail::slot_applyResult_addRating_album);
            proc_fav_album->request_rose_addRating_Album(json);
        }
    }


    void BugsAlbumDetail::slot_applyResult_getRating_album(const QJsonArray &contents){

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


    void BugsAlbumDetail::slot_applyResult_addRating_album(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){

            // request HTTP API - favorite for Rose Server
            roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
            connect(proc_fav, &roseHome::ProcCommon::completeCheck_rating_album, this, &BugsAlbumDetail::slot_applyResult_checkRating_album);
            proc_fav->request_rose_checkRating_Album("BUGS", QString("%1").arg(this->data_album.album_id));

            if(this->flag_check_album == true){
                this->flag_check_album = false;
            }
        }
        else{
            this->flag_check_album = false;
        }
    }

    void BugsAlbumDetail::slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌.
        int idx = ProcJsonEasy::getInt(p_jsonObj, "index");
        int track_id = ProcJsonEasy::getString(p_jsonObj, "data_id").toInt();

        if(track_id == this->list_track->at(idx).track_id){
            qDebug() << "likes_yn : " << ProcJsonEasy::getBool(p_jsonObj, "likes_yn");
        }

        this->flag_send_track = false;
    }


    void BugsAlbumDetail::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("id")){

            int id = ProcJsonEasy::getInt(jsonObj, "id");

            if(id > 0){
                this->roseServer_trackId[this->track_idx_fav] = id;

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
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsAlbumDetail::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
            }
            else{
                QJsonObject data = this->jsonArr_tracks_toPlay[this->track_idx_fav].toObject();

                QJsonArray tmpArtists = ProcJsonEasy::getJsonArray(data, "artists");

                QJsonArray artists;
                for(int i = 0; i < tmpArtists.size(); i++){
                    QJsonObject artistName = tmpArtists.at(i).toObject();
                    artists.append(ProcJsonEasy::getString(artistName, "artist_nm"));
                }

                QJsonObject track;
                track.insert("title", ProcJsonEasy::getString(data, "track_title"));
                track.insert("type", "BUGS");
                track.insert("favorite", false);
                track.insert("artists", artists);
                track.insert("albums", ProcJsonEasy::getJsonObject(data, "album"));
                track.insert("star", 0);
                track.insert("thumbnailUrl", this->data_album.album_image);
                track.insert("clientKey", ProcJsonEasy::getString(data, "id"));
                track.insert("duration", ProcJsonEasy::getString(data, "len"));
                track.insert("data", data);

                QJsonArray tracks;
                tracks.append(track);

                QJsonObject json;
                json.insert("tracks", tracks);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsAlbumDetail::slot_applyResult_addRating_track);
                proc_fav_track->request_rose_addRating_Track(json);
            }
        }
        else{

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void BugsAlbumDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){

            for(int i = 0; i < contents.size(); i++){

                QJsonObject track = contents.at(i).toObject();

                if(track.contains("star")){
                    QString clientKey = ProcJsonEasy::getString(track, "clientKey");
                    int star = ProcJsonEasy::getInt(track, "star");

                    QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(i).toObject();
                    QString trackID = QString("%1").arg(ProcJsonEasy::getInt(trackInfo, "track_id"));

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


    void BugsAlbumDetail::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
            connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsAlbumDetail::slot_applyResult_checkRating_track);
            proc_favCheck_track->request_rose_checkRating_Track("BUGS", QString("%1").arg(ProcJsonEasy::getInt(this->jsonArr_tracks_toPlay[this->track_idx_fav].toObject(), "id")));

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void BugsAlbumDetail::changedOnlyTabUI_notSendSignal(QString p_step){

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



    void BugsAlbumDetail::slot_changedSubTabUI(const QJsonObject &p_data){

        QString tmp_step = p_data[KEY_PAGE_CODE].toString();

        // 페이지 변경 및 데이터 세팅
        this->changedOnlyTabUI_notSendSignal(tmp_step);

    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [Slot] AlbumImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void BugsAlbumDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

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
                connect(proc_favCheck_album, &roseHome::ProcCommon::completeCheck_rating_album, this, &BugsAlbumDetail::slot_applyResult_checkRating_album);
                proc_favCheck_album->request_rose_checkRating_Album("BUGS", QString("%1").arg(this->data_album.album_id));

                this->flag_check_album = true;
            }
        }
        else{
            if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::AddCollection){

                QJsonObject json = QJsonObject();
                json.insert("tracks", this->jsonArr_tracks_toPlay);

                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::BUGS, json, this);
                dialog_playlist->request_playlist_fetch();
                connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &BugsAlbumDetail::slot_add_rosePlaylist_withBugs);
                int result = dialog_playlist->exec();

                if(result == QDialog::Accepted){
                    delete dialog_playlist;
                }
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){

                // OptMorePopup 띄우기 - 즐겨찾기 관련 메뉴를 가림. DetailView에 하트가 있음
                this->makeObj_optMorePopup(OptMorePopup::Bugs_Album, bugs::ConvertData_forBugs::getConvertOptHeaderData(this->data_album), 0, SECTION_FOR_MORE_POPUP___albums, true);
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayAll){

                // 전체재생
                bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byTracks(this->jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, bugs::ProcRoseAPI_withBugs::PlayShuffleMode::JustPlay);
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayShuffle){

                // 셔플재생
                bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byTracks(this->jsonArr_tracks_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, bugs::ProcRoseAPI_withBugs::PlayShuffleMode::ShufflePlay);
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Share){

                this->shareLink = this->album_detail_info->detailInfo_shareLink;    //220905 share code ... --

                //220816 share code ... --
                setUIShare();//c220818
            }
        }
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void BugsAlbumDetail::slot_clickedItemTrack_inList(const int idx, const AlbumTrackDetailInfo_RHV::ClickMode clickMode){

        if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->album_tracks[idx]->getFavoritesStars();
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
                    itemPosData.section = SECTION_FOR_MORE_POPUP___track;
                    itemPosData.index = idx;
                    itemPosData.data_id = QString("%1").arg(this->list_track->at(idx).track_id);

                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    connect(proc, &bugs::ProcBugsAPI::completeReq_favarite_track, this, &BugsAlbumDetail::slot_bugs_completeReq_listAll_myFavoritesIds);

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
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsAlbumDetail::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("BUGS", QString("%1").arg(this->list_track->at(idx).track_id));

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

            bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
            procRosePlay->requestPlayRose_byTracks(this->jsonArr_tracks_toPlay, idx, curr_clickMode);
        }
        else if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::MusicVideoBtn){
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, PlaylistTrackDetailInfo_RHV::ClickMode::MusicVideoBtn, idx, SECTION_FOR_MORE_POPUP___track);
        }
        else{
            // OptMorePopup 띄우기 - 즐겨찾기 관련 메뉴를 가림. DetailView에 하트가 있음
            this->makeObj_optMorePopup(OptMorePopup::Bugs_Track, bugs::ConvertData_forBugs::getConvertOptHeaderData(this->list_track->at(idx)), idx, SECTION_FOR_MORE_POPUP___track, true);
        }
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void BugsAlbumDetail::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_aritst_album, clickMode, index, section);
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
    void BugsAlbumDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->data_album, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___aritstAlbum){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_aritst_album, index, clickMode);
        }
    }

}
