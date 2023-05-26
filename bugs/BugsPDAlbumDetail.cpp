#include "bugs/BugsPDAlbumDetail.h"

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
#include <QPainterPath>


namespace bugs {

    const QString ICON_PATH___favorite_off = ":/images/rosehome/heart_ico0.png";
    const QString ICON_PATH___favorite_on = ":/images/rosehome/heart_ico1.png";
    const QString ICON_PATH___favorite_on2 = ":/images/rosehome/heart_ico2.png";
    const QString ICON_PATH___favorite_on3 = ":/images/rosehome/heart_ico3.png";

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___pd_album = 1;
    const int SECTION_FOR_MORE_POPUP___track = 2;


    /**
     * @brief
     * @param parent
     */
    BugsPDAlbumDetail::BugsPDAlbumDetail(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_filter, parent) {

        this->linker = Linker::getInstance();

        // data
        this->list_track = new QList<bugs::TrackItemData>();
    }


    /**
     * @brief 소멸자.
     */
    BugsPDAlbumDetail::~BugsPDAlbumDetail(){

        this->deleteLater();
    }


    /**
     * @brief BugsPDAlbumDetail::setJsonObject_forData
     * @param jsonObj
     */
    void BugsPDAlbumDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        bugs::PD_AlbumItemData tmp_data_pd_album = ConvertData_forBugs::convertData_pd_albumData(jsonObj);
        this->flagNeedReload = false;

        if(tmp_data_pd_album.es_album_id != this->data_pd_album.es_album_id){
            this->flagNeedReload = true;

            if(this->flag_widget_visible == true){
                this->widget_main_contents->hide();

                this->box_contents->removeWidget(this->widget_main_contents);
                GSCommon::clearLayout(this->box_main_contents);

                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->flag_widget_visible = false;
            }
            else{
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);
            }

            this->data_pd_album = tmp_data_pd_album;

            this->list_track->clear();

            this->jsonArr_tracks_toPlay = QJsonArray();

            this->playlist_star_fav = 0;

            this->flag_playlist_fav = false;
            this->flag_send_playlist = false;
            this->flag_check_playlist = false;

            this->track_favoriteOffset = 0;
            this->track_id_fav = 0;
            this->track_idx_fav = 0;

            this->flag_track_fav = false;
            this->flag_send_track = false;
            this->flag_check_track = false;
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief BugsPDAlbumDetail::setActivePage
     */
    void BugsPDAlbumDetail::setActivePage(){

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

                this->setUIControl_basic();

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

                // request HTTP API
                if(this->data_pd_album.title.isEmpty()){
                    // Album 기본 정보를 가져와야 하는 경우임. (id만 전달받은 상태)
                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    connect(proc, &ProcBugsAPI::completeReq_pd_album, this, &BugsPDAlbumDetail::slot_applyResult_pd_albumInfo);
                    proc->request_bugs_get_pd_album(this->data_pd_album.es_album_id);
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
    void BugsPDAlbumDetail::setUIControl_basic(){

        this->pd_album_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Bugs_pdAlbum);
        connect(this->pd_album_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &BugsPDAlbumDetail::slot_detailInfo_btnClicked);
        connect(this->pd_album_detail_info, &AbstractImageDetailContents_RHV::signal_clicked_tag, this, &BugsPDAlbumDetail::slot_clickedItempdAlbum_inTag);

        this->pd_album_detail_info->initView();

        this->box_main_contents->addWidget(this->pd_album_detail_info);

        this->listWidget_contents = new QListWidget(this);
        this->listWidget_contents->setCursor(Qt::PointingHandCursor);
        this->listWidget_contents->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->listWidget_contents->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->listWidget_contents->setStyleSheet("background-color:transparent;");

        connect(this->listWidget_contents, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(slot_listwidgetItemPressed(QListWidgetItem *)));

        this->box_main_contents->addWidget(this->listWidget_contents);

        this->listWidget_contents->clear();

        if(!this->data_pd_album.title.isEmpty()){
            this->slot_applyResult_pd_albumInfo(this->data_pd_album);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief BugsPDAlbumDetail::slot_applyResult_pd_albumInfo
     * @param data_album
     */
    void BugsPDAlbumDetail::slot_applyResult_pd_albumInfo(bugs::PD_AlbumItemData data_pd_album){

        // Update Data
        this->data_pd_album = data_pd_album;

        // Update UI
        QJsonObject pdalbumObj;
        pdalbumObj.insert("image", this->data_pd_album.image);
        pdalbumObj.insert("title", this->data_pd_album.title);

        pdalbumObj.insert("id", this->data_pd_album.es_album_id);
        pdalbumObj.insert("artist", this->data_pd_album.pd_nickname);
        pdalbumObj.insert("releaseDate", this->data_pd_album.upd_dt);
        pdalbumObj.insert("track_count", this->data_pd_album.track_count);

        pdalbumObj.insert("description", this->data_pd_album.descr);

        QJsonArray jsonArr_tag;
        for(int i = 0; i < this->data_pd_album.list_tag_nm.count(); i++){
            QJsonObject tmpTag;
            tmpTag.insert("name", this->data_pd_album.list_tag_nm.at(i));
            tmpTag.insert("id", this->data_pd_album.list_tag_id.at(i));

            jsonArr_tag.append(tmpTag);
        }

        pdalbumObj.insert("tag", jsonArr_tag);

        this->pd_album_detail_info->setData_fromBugsData(pdalbumObj);

        // request HTTP API - get favorite for Rose Server
        roseHome::ProcCommon *proc_fav_pdAlbum = new roseHome::ProcCommon(this);
        connect(proc_fav_pdAlbum, &roseHome::ProcCommon::completeReq_rating_playlist, this, &BugsPDAlbumDetail::slot_applyResult_getRating_pdAlbum);
        proc_fav_pdAlbum->request_rose_getRating_Playlist("BUGS", QString("%1").arg(this->data_pd_album.es_album_id));

        // get Tracks of the PD album
        ProcBugsAPI *proc_track = new ProcBugsAPI(this);
        connect(proc_track, &ProcBugsAPI::completeReq_list_items_of_pd_album, this, &BugsPDAlbumDetail::slot_applyResult_tracks);
        proc_track->request_bugs_getList_items_of_pd_album(this->data_pd_album.es_album_id);

        // share link  //c221209  start
        /*QString param = "BUGS/ALBUM/" + QString("%1").arg(this->data_album.album_id);
        QString desc = this->data_album.list_artist_nm.join(",");

        roseHome::ProcCommon *proc_link = new roseHome::ProcCommon(this);
        connect(proc_link, &roseHome::ProcCommon::completeReq_share_link, this, &BugsAlbumDetail::slot_applyResult_getShareLink);
        proc_link->request_rose_get_shareLink(this->data_album.album_image, desc, this->data_album.title, param);
        //c221209  end

        this->track_favoriteOffset = 0;*/
    }


    void BugsPDAlbumDetail::slot_add_rosePlaylist_withBugs(const int &idx, const QJsonObject &dataObj){

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


    void BugsPDAlbumDetail::slot_applyResult_getShareLink(const QString &link){   //j220818 share link

        this->shareLink = link;
    }


    /**
     * @brief PDAlbumImageDetailInfo 에서 태그 버튼 클릭에 대한 처리
     * @param index_tag
     */
    void BugsPDAlbumDetail::slot_clickedItempdAlbum_inTag(const int idx){

        // check if the index is valid
        if(idx >=0 && idx < this->data_pd_album.list_tag_id.count()){
            // PA's Album All View 페이지 이동 (해당 tag에 대하여)
            PageInfo_PDAlbumAllView data_pageInfo;
            data_pageInfo.sortOpt = QVariant::fromValue(ProcBugsAPI::BugsPDAlbum_Sort::Recent).toString();
            data_pageInfo.tag_ids = QString("%1").arg(this->data_pd_album.list_tag_id.at(idx));
            data_pageInfo.mainTitle = this->data_pd_album.list_tag_nm.at(idx);

            this->movePage_pd_album_allView(data_pageInfo);
        }
    }


    void BugsPDAlbumDetail::slot_applyResult_checkRating_pdAlbum(const QJsonObject &jsonObj){

        int pdAlbumClientKey = ProcJsonEasy::getInt(jsonObj, "id");

        if(pdAlbumClientKey > 0){

            if(this->flag_check_playlist == true){

                QJsonObject playlist;
                playlist.insert("favorite", false);
                playlist.insert("favoriteTrack", false);
                playlist.insert("id", pdAlbumClientKey);
                playlist.insert("isRose", true);
                playlist.insert("ownerId", 0);
                playlist.insert("star", 0);
                playlist.insert("thumbup", false);
                playlist.insert("thumbupCount", 0);
                playlist.insert("trackCount", 0);
                playlist.insert("type", "BUGS");

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_playlist_fav);
                ratingInfo.insert("star", this->playlist_star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", "BUGS");

                QJsonObject json;
                json.insert("playlist", playlist);
                json.insert("ratingInfo", ratingInfo);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_pdAlbum = new roseHome::ProcCommon(this);
                connect(proc_fav_pdAlbum, &roseHome::ProcCommon::completeReq_rating_playlist, this, &BugsPDAlbumDetail::slot_applyResult_getRating_pdAlbum);
                proc_fav_pdAlbum->request_rose_setRating_rosePlaylist(json, this->flag_playlist_fav, this->playlist_star_fav);
            }
            else{

                roseHome::ProcCommon *proc_fav_pdAlbum = new roseHome::ProcCommon(this);
                connect(proc_fav_pdAlbum, &roseHome::ProcCommon::completeReq_rating_playlist, this, &BugsPDAlbumDetail::slot_applyResult_getRating_pdAlbum);
                proc_fav_pdAlbum->request_rose_getRating_Playlist("BUGS", QString("%1").arg(this->data_pd_album.es_album_id));
            }
        }
        else{

            QJsonObject data;
            data.insert("realType", "MUSIC_PD");

            QJsonObject playlist;
            playlist.insert("clientKey", this->data_pd_album.es_album_id);
            playlist.insert("data", data);

            QJsonObject json;
            json.insert("playlist", playlist);
            json.insert("favorite", true);
            json.insert("favoriteTrack", false);
            json.insert("isRose", false);
            json.insert("ownerId", 0);
            json.insert("star", 1);
            json.insert("thumbnail", this->data_pd_album.image);
            json.insert("thumbup", false);
            json.insert("thumbupCount", 0);
            json.insert("title", this->data_pd_album.title);
            json.insert("trackCount", 0);
            json.insert("type", "BUGS");

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_pdAlbum = new roseHome::ProcCommon(this);
            connect(proc_fav_pdAlbum, &roseHome::ProcCommon::completeCheck_rating_playlist, this, &BugsPDAlbumDetail::slot_applyResult_addRating_pdAlbum);
            proc_fav_pdAlbum->request_rose_addRating_Playlist(json);
        }
    }


    void BugsPDAlbumDetail::slot_applyResult_getRating_pdAlbum(const QJsonArray &contents){

        if(contents.size() > 0){

            QJsonObject tmp_star = contents.at(0).toObject();

            if(tmp_star.contains("star")){
                int star = ProcJsonEasy::getInt(tmp_star, "star");
                bool flag = star > 0 ? true : false;

                this->pd_album_detail_info->setFavorite(flag, star);
            }
            else{
                this->pd_album_detail_info->setFavorite(this->flag_playlist_fav, this->playlist_star_fav);
            }

            if(this->flag_check_playlist == true){
                this->flag_check_playlist = false;
            }
        }
    }


    void BugsPDAlbumDetail::slot_applyResult_addRating_pdAlbum(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            this->pd_album_detail_info->setFavorite(this->flag_playlist_fav, this->playlist_star_fav);

            if(this->flag_check_playlist == true){
                this->flag_check_playlist = false;
            }
        }
        else{
            this->flag_check_playlist = false;
        }
    }


    void BugsPDAlbumDetail::slot_bugs_completeReq_pdAlbum_myFavoritesIds(const QJsonObject& p_jsonObj){

        // Favorite 정보를 전달해줌.
        int pd_album_id = ProcJsonEasy::getString(p_jsonObj, "data_id").toInt();

        if(pd_album_id == this->data_pd_album.es_album_id){
            qDebug() << "pd album likes_yn : " << ProcJsonEasy::getBool(p_jsonObj, "likes_yn");
        }

        this->flag_send_playlist = false;
    }


    /**
     * @brief BugsPDAlbumDetail::slot_applyResult_tracks
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void BugsPDAlbumDetail::slot_applyResult_tracks(const QList<bugs::TrackItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            if(list_data.size() == 0){
                ContentLoadingwaitingMsgHide();      //cheon Tidal

                return;
            }

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
                this->track_totalCount = list_data.at(0).total_count;
            }

            int start_index = this->list_track->count();

            this->list_track->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            int height = this->list_track->count() * 70;
            this->listWidget_contents->setFixedHeight(height);

            if(start_index == 0){
                int max_cnt = this->list_track->count();
                this->track_drawCount = max_cnt;

                QList<QString> tmp_clientkey;
                this->track_favoriteOffset = start_index;
                for(int i = start_index; i < this->track_drawCount; i++){
                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).track_id));

                    this->iniDataInfo_pdAlbumList_Item(this->list_track->at(i), i);
                    this->setDataInfo_pdAlbumList_Item(this->list_track->at(i), i);

                    QCoreApplication::processEvents();
                }

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsPDAlbumDetail::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("BUGS", tmp_clientkey);

                ContentLoadingwaitingMsgHide();      //cheon Tidal

                this->flag_widget_visible = true;

                if(this->track_drawCount == this->track_totalCount){
                    this->box_main_contents->addSpacing(30);
                }
            }
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            if(this->list_track->size() <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->box_main_contents->addWidget(noData_widget);
            }
        }
    }


    inline void BugsPDAlbumDetail::iniDataInfo_pdAlbumList_Item(const bugs::TrackItemData &data_track, const int &p_index){

        QString thumbnail = data_track.album_image;

        int id = data_track.track_id;

        QWidget *widget_track_info = new QWidget();
        widget_track_info->setFixedSize(1550, 70);
        widget_track_info->setObjectName("widget_track_info");
        widget_track_info->setStyleSheet("#widget_track_info {background-color:transparent; border-bottom:1px solid #333333; }"
                                            "#widget_track_info:hover{background-color:#333333;} ");

        QString default_path = ":/images/bugs/bugs_def_430x2.png";

        QImage img_album;
        QPixmap pixmap_image;
        if(img_album.load(default_path)){
            QPixmap pixmapIMG = QPixmap(QSize(60, 60));
            pixmapIMG.fill(Qt::black);

            pixmap_image = QPixmap::fromImage(img_album);                                        //이미지를 버퍼에 옮긴다.
            pixmap_image = pixmap_image.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, 60, 60, 2, 2);

            int leftValue = (60 - pixmap_image.width()) / 2;
            int topValue = (60 - pixmap_image.height()) / 2;

            painter.setClipPath(path);
            painter.drawPixmap(leftValue, topValue, pixmap_image);
            painter.end();
        }

        this->lb_thumb[p_index] = new QLabel(widget_track_info);
        this->lb_thumb[p_index]->setFixedSize(60, 60);
        this->lb_thumb[p_index]->setGeometry(5, 5, 0, 0);
        this->lb_thumb[p_index]->setStyleSheet("background-color:transparent;");
        this->lb_thumb[p_index]->setPixmap(pixmap_image);

        FileDownloader *fileDownloader = new FileDownloader(this);
        fileDownloader->setProperty("index", p_index);
        fileDownloader->setImageURL(thumbnail);
        connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownload_list()));

        this->lb_explicit[p_index] = new QLabel(widget_track_info);
        this->lb_explicit[p_index]->setFixedSize(30, 30);
        this->lb_explicit[p_index]->setGeometry(180, 20, 0, 0);
        this->lb_explicit[p_index]->hide();

        this->lb_title[p_index] = new QLabel(widget_track_info);
        this->lb_title[p_index]->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
        this->lb_title[p_index]->setGeometry(180, 10, 720, 49);

        this->lb_album[p_index] = new QLabel(widget_track_info);
        this->lb_album[p_index]->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
        this->lb_album[p_index]->setWordWrap(true);
        this->lb_album[p_index]->setGeometry(937, 14, 300, 40);

        this->lb_resolution[p_index] = new QLabel(widget_track_info);
        this->lb_resolution[p_index]->setStyleSheet("background-color:transparent;color:#FFF;font-size:12px;font-weight:300;border:1px solid #FFF;border-radius:8px;");
        this->lb_resolution[p_index]->setGeometry(900, 10, 20, 49);
        this->lb_resolution[p_index]->hide();

        this->lb_artist[p_index] = new QLabel(widget_track_info);
        this->lb_artist[p_index]->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
        this->lb_artist[p_index]->setGeometry(180, 34, 680, 49);

        this->lb_duration[p_index] = new QLabel(widget_track_info);
        this->lb_duration[p_index]->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
        this->lb_duration[p_index]->setGeometry(1231, 24, 65, 20);

        QPushButton *btn_video_list = GSCommon::getUIBtnImg("btn_video_list", ":/images/list_video_ico.png", widget_track_info);
        btn_video_list->setGeometry(1350, 10, 50, 50);
        btn_video_list->setCursor(Qt::PointingHandCursor);
        btn_video_list->setProperty("idx", p_index);
        btn_video_list->hide();

        connect(btn_video_list, &QPushButton::clicked, this, &BugsPDAlbumDetail::slot_clicked_btnVideolist);

        if(data_track.list_mv_id.count() > 0){
            btn_video_list->show();
        }

        this->btn_list_fav[p_index] = GSCommon::getUIBtnImg("btn_list_fav", ICON_PATH___favorite_off, widget_track_info);
        this->btn_list_fav[p_index]->setGeometry(1400, 6, 50, 58);
        this->btn_list_fav[p_index]->setCursor(Qt::PointingHandCursor);
        this->btn_list_fav[p_index]->setProperty("idx", p_index);

        connect(this->btn_list_fav[p_index], &QPushButton::clicked, this, &BugsPDAlbumDetail::slot_clicked_btnFavorite);

        this->track_star_fav[p_index] = 0;

        QPushButton *btn_menu = GSCommon::getUIBtnImg("btn_menu", ":/images/icon_menu.png", widget_track_info);
        btn_menu->setGeometry(1450, 6, 50, 58);
        btn_menu->setCursor(Qt::PointingHandCursor);
        btn_menu->setProperty("idx", p_index);

        connect(btn_menu, &QPushButton::clicked, this, &BugsPDAlbumDetail::slot_clicked_btnMenu);

        QString tmpProperty = QString("IDX:%1,").arg(p_index) + QString("ID:%1").arg(id);

        QListWidgetItem *item = new QListWidgetItem();
        item->setWhatsThis(tmpProperty);
        item->setSizeHint(QSize(90, 70));

        this->listWidget_contents->addItem(item);

        this->listWidget_contents->setItemWidget(item, widget_track_info);
    }


    inline void BugsPDAlbumDetail::setDataInfo_pdAlbumList_Item(const bugs::TrackItemData &data_track, const int &p_index){

        QString title = data_track.track_title;
        QString artist = data_track.list_artist_nm.join(",");;
        QString album  = data_track.album_title;
        QString bitrates = data_track.bitrates;
        QString duration = data_track.len;

        bool explicit_flag = data_track.adult_yn;

        QLabel *tmp_title = new QLabel();
        tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
        tmp_title->setText(title);

        int title_width = 0;
        int title_width_resize = 0;
        title_width = tmp_title->sizeHint().width();

        QLabel *tmp_artist = new QLabel();
        tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
        tmp_artist->setText(artist);

        int artist_width = 0;
        artist_width = tmp_artist->sizeHint().width();

        int width = 0;
        int left = 0;
        if(explicit_flag == true){
            QString adult_certification_path = "";

            if(global.lang == 0){
                adult_certification_path = ":/images/tidal/tidal-e-ico.png";
            }
            else{
                adult_certification_path = ":/images/bugs/ico-19.png";
            }

            QImage img;
            QPixmap *img_adult_certification = new QPixmap();
            if(img.load(adult_certification_path)){
                *img_adult_certification = QPixmap::fromImage(img);
                *img_adult_certification = img_adult_certification->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

            this->lb_explicit[p_index]->setPixmap(*img_adult_certification);
            this->lb_explicit[p_index]->setFixedSize(30, 30);
            this->lb_explicit[p_index]->setGeometry(85, 20, 0, 0);
            this->lb_explicit[p_index]->show();

            if(!bitrates.isEmpty()){
                this->lb_resolution[p_index]->setText(QString("%1").arg(bitrates));
                this->lb_resolution[p_index]->setAlignment(Qt::AlignCenter);

                width = this->lb_resolution[p_index]->sizeHint().width() + 20;

                title_width += (15 + width);

                if(title_width > 680){
                    title_width_resize = 680 - (15 + width);
                    this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                    if(this->lb_title[p_index]->text().contains("…")){
                        this->lb_title[p_index]->setToolTip(title);
                        this->lb_title[p_index]->setToolTipDuration(2000);
                    }
                    this->lb_title[p_index]->setGeometry(135, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + width);
                    this->lb_title[p_index]->setText(title);
                    this->lb_title[p_index]->setGeometry(135, 10, title_width_resize, 25);
                }

                left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                this->lb_resolution[p_index]->setGeometry(left, 15, width, 16);
                this->lb_resolution[p_index]->show();
            }
            else{
                if(title_width > 680){
                    this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, 680, this->lb_title[p_index]->font()));
                    if(this->lb_title[p_index]->text().contains("…")){
                        this->lb_title[p_index]->setToolTip(title);
                        this->lb_title[p_index]->setToolTipDuration(2000);
                    }
                    this->lb_title[p_index]->setGeometry(135, 10, 680, 25);
                }
                else{
                    this->lb_title[p_index]->setText(title);
                    this->lb_title[p_index]->setGeometry(135, 10, title_width, 25);
                }
            }

            if(artist_width > 640){
                this->lb_artist[p_index]->setText(GSCommon::getTextCutFromLabelWidth(artist, 640, this->lb_artist[p_index]->font()));
                if(this->lb_artist[p_index]->text().contains("…")){
                    this->lb_artist[p_index]->setToolTip(artist);
                    this->lb_artist[p_index]->setToolTipDuration(2000);
                }
                this->lb_artist[p_index]->setGeometry(135, 34, 640, 25);
            }
            else{
                this->lb_artist[p_index]->setText(artist);
                this->lb_artist[p_index]->setGeometry(135, 34, artist_width, 25);
            }
        }
        else{
            if(!bitrates.isEmpty()){
                this->lb_resolution[p_index]->setText(QString("%1").arg(bitrates));
                this->lb_resolution[p_index]->setAlignment(Qt::AlignCenter);

                width = this->lb_resolution[p_index]->sizeHint().width() + 20;

                title_width += (15 + width);

                if(title_width > 720){
                    title_width_resize = 720 - (15 + width);
                    this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                    if(this->lb_title[p_index]->text().contains("…")){
                        this->lb_title[p_index]->setToolTip(title);
                        this->lb_title[p_index]->setToolTipDuration(2000);
                    }
                    this->lb_title[p_index]->setGeometry(85, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + width);
                    this->lb_title[p_index]->setText(title);
                    this->lb_title[p_index]->setGeometry(85, 10, title_width_resize, 25);
                }

                left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                this->lb_resolution[p_index]->setGeometry(left, 15, width, 16);
                this->lb_resolution[p_index]->show();
            }
            else{
                if(title_width > 720){
                    this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, 720, this->lb_title[p_index]->font()));
                    if(this->lb_title[p_index]->text().contains("…")){
                        this->lb_title[p_index]->setToolTip(title);
                        this->lb_title[p_index]->setToolTipDuration(2000);
                    }
                    this->lb_title[p_index]->setGeometry(85, 10, 720, 25);
                }
                else{
                    this->lb_title[p_index]->setText(title);
                    this->lb_title[p_index]->setGeometry(85, 10, title_width, 25);
                }
            }

            if(artist_width > 680){
                this->lb_artist[p_index]->setText(GSCommon::getTextCutFromLabelWidth(artist, 680, this->lb_artist[p_index]->font()));
                if(this->lb_artist[p_index]->text().contains("…")){
                    this->lb_artist[p_index]->setToolTip(artist);
                    this->lb_artist[p_index]->setToolTipDuration(2000);
                }
                this->lb_artist[p_index]->setGeometry(85, 34, 680, 25);
            }
            else{
                this->lb_artist[p_index]->setText(artist);
                this->lb_artist[p_index]->setGeometry(85, 34, artist_width, 25);
            }
        }

        QLabel *tmp_album = new QLabel();
        tmp_album->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
        tmp_album->setText(album);

        int album_title_width = 0;
        album_title_width = tmp_album->sizeHint().width();

        if(album_title_width > 220){
            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_album_set = "";

            tmp_split = album;
            splitToken = tmp_split.split(" ");

            tmp_album->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_album_set = splitToken.at(i);
                    }
                    else{
                        tmp_album_set += " " + splitToken.at(i);
                    }
                    tmp_album->setText(tmp_album_set);

                    if(tmp_album->sizeHint().width() >= 220){
                        tmp_album->setText("");
                        tmp_album_set.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_album->setText("");
            tmp_album->setText(tmp_album_set);

            album_title_width = tmp_album->sizeHint().width() + 220;

            QString strAlbum = album;
            QLabel *tmpAlbum = new QLabel();
            tmpAlbum->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
            tmpAlbum->setText(strAlbum.replace(tmp_album_set, ""));

            int tmpAlbum_width = tmpAlbum->sizeHint().width();

            if(tmpAlbum_width >= 220){
                this->lb_album[p_index]->setText(GSCommon::getTextCutFromLabelWidth(album, album_title_width, this->lb_album[p_index]->font()));
                if(this->lb_album[p_index]->text().contains("…")){
                    this->lb_album[p_index]->setToolTip(album);
                    this->lb_album[p_index]->setToolTipDuration(2000);
                }
            }
            else{
                this->lb_album[p_index]->setText(album);
            }
            this->lb_album[p_index]->setGeometry(937, 14, 220, 40);
        }
        else{
            this->lb_album[p_index]->setText(album);
            this->lb_album[p_index]->setGeometry(937, 24, 220, 20);
        }

        this->lb_duration[p_index]->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
        this->lb_duration[p_index]->setGeometry(1231, 24, 65, 20);
        this->lb_duration[p_index]->setText(duration);
    }


    void BugsPDAlbumDetail::slot_listwidgetItemPressed(QListWidgetItem *item){

        QString tmpStr = item->whatsThis();

        QStringList strSplit = tmpStr.split(",");
        QStringList split_idx = strSplit.at(0).split(":");

        int idx = split_idx.at(1).toInt();

        this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, PlaylistTrackDetailInfo_RHV::ClickMode::AllBox, idx, SECTION_FOR_MORE_POPUP___track);
    }


    void BugsPDAlbumDetail::slot_clicked_btnVideolist(){

        int idx = sender()->property("idx").toInt();

        this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, PlaylistTrackDetailInfo_RHV::ClickMode::MusicVideoBtn, idx, SECTION_FOR_MORE_POPUP___track);
    }


    void BugsPDAlbumDetail::slot_clicked_btnFavorite(){

        int idx = sender()->property("idx").toInt();

        if(this->flag_check_track == false){
            if(this->track_star_fav[idx] == 3){
                this->track_star_fav[idx] = 0;

                this->track_idx_fav = idx;
                this->flag_track_fav = false;
            }
            else if(this->track_star_fav[idx] >= 0 && this->track_star_fav[idx] < 3){
                this->track_star_fav[idx]++;

                this->track_idx_fav = idx;
                this->flag_track_fav = true;
            }

            QJsonObject trackObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

            if(this->track_star_fav[idx] == 0 || this->track_star_fav[idx] == 1){
                // Bugs Favorite
                ItemPositionData itemPosData;
                itemPosData.section = SECTION_FOR_MORE_POPUP___track;
                itemPosData.index = idx;
                itemPosData.data_id = QString("%1").arg(this->list_track->at(idx).track_id);

                ProcBugsAPI *proc = new ProcBugsAPI(this);
                connect(proc, &bugs::ProcBugsAPI::completeReq_favarite_track, this, &BugsPDAlbumDetail::slot_bugs_completeReq_track_myFavoritesIds);

                if(this->track_star_fav[idx] == 0){
                    itemPosData.likes_yn = false;

                    proc->request_bugs_deleteFavorite_track(this->list_track->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }
                else if(this->track_star_fav[idx] == 1){
                    itemPosData.likes_yn = true;

                    proc->request_bugs_addFavorite_track(this->list_track->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }

                this->flag_send_track = true;
            }

            this->track_idx_fav = idx;

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
            connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsPDAlbumDetail::slot_applyResult_checkRating_track);
            proc_favCheck_track->request_rose_checkRating_Track("BUGS", QString("%1").arg(this->list_track->at(idx).track_id));

            this->flag_check_track = true;
        }
    }


    void BugsPDAlbumDetail::slot_clicked_btnMenu(){

        int idx = sender()->property("idx").toInt();

        QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

        this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn, idx, SECTION_FOR_MORE_POPUP___track);
    }


    void BugsPDAlbumDetail::slot_thumbnailDownload_list(){

        FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
        int tmp_index = fileDownloader->property("index").toInt();

        if(tmp_index >= 0 && tmp_index < this->list_track->size()){
            QImage image;
            bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());
            if(flagLoaded){
                QPixmap pixmapIMG = QPixmap(QSize(60, 60));
                pixmapIMG.fill(Qt::black);

                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                QPainter painter (&pixmapIMG);
                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
                painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

                QPainterPath path = QPainterPath();
                path.addRoundedRect(0, 0, 60, 60, 2, 2);

                int leftValue = (60 - tmp_pixmap.width()) / 2;
                int topValue = (60 - tmp_pixmap.height()) / 2;

                painter.setClipPath(path);
                painter.drawPixmap(leftValue, topValue, tmp_pixmap);
                painter.end();

                this->lb_thumb[tmp_index]->setPixmap(pixmapIMG);
            }
        }

        fileDownloader->deleteLater();
    }


    void BugsPDAlbumDetail::slot_bugs_completeReq_track_myFavoritesIds(const QJsonObject& p_jsonObj){

        // Favorite 정보를 전달해줌.
        int idx = ProcJsonEasy::getInt(p_jsonObj, "index");
        int track_id = ProcJsonEasy::getString(p_jsonObj, "data_id").toInt();

        if(track_id == this->list_track->at(idx).track_id){
            qDebug() << "likes_yn : " << ProcJsonEasy::getBool(p_jsonObj, "likes_yn");
        }

        this->flag_send_track = false;
    }


    void BugsPDAlbumDetail::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("id")){

            int id = ProcJsonEasy::getInt(jsonObj, "id");

            if(id > 0){
                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_track_fav);
                ratingInfo.insert("star", this->track_star_fav[this->track_idx_fav]);
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
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsPDAlbumDetail::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav[this->track_idx_fav]);
            }
            else{
                QJsonObject data = this->jsonArr_tracks_toPlay[this->track_idx_fav].toObject();

                QJsonObject track;
                track.insert("clientKey", ProcJsonEasy::getInt(data, "track_id"));
                track.insert("data", data);
                track.insert("duration", 0);
                track.insert("favorite", true);
                track.insert("ownerId", 0);
                track.insert("sort", 0);
                track.insert("star", 1);
                track.insert("thumbnailUrl", this->list_track->at(this->track_idx_fav).album_image);
                track.insert("title", ProcJsonEasy::getString(data, "track_title"));
                track.insert("type", "BUGS");
                QJsonArray tracks;
                tracks.append(track);

                QJsonObject json;
                json.insert("tracks", tracks);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsPDAlbumDetail::slot_applyResult_addRating_track);
                proc_fav_track->request_rose_addRating_Track(json);
            }
        }
        else{

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void BugsPDAlbumDetail::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");

                    this->track_star_fav[this->track_favoriteOffset + i] = star;

                    if(this->track_star_fav[this->track_favoriteOffset + i] > 0){
                        switch (this->track_star_fav[this->track_favoriteOffset + i]) {
                            case 1:
                                this->btn_list_fav[this->track_favoriteOffset + i]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                              ICON_PATH___favorite_on + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                            case 2:
                                this->btn_list_fav[this->track_favoriteOffset + i]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                              ICON_PATH___favorite_on2 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                            case 3:
                                this->btn_list_fav[this->track_favoriteOffset + i]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                              ICON_PATH___favorite_on3 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                        }
                    }
                    else{
                        this->btn_list_fav[this->track_favoriteOffset + i]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                      ICON_PATH___favorite_off + "'); background-repeat:no-repeat; background-position:center center; } ");
                    }
                }
            }
            else if(track_info.contains("message") && (track_info["message"].toString() == "정상")){
                if(this->track_star_fav[this->track_idx_fav] > 0){
                    switch (this->track_star_fav[this->track_idx_fav]) {
                        case 1:
                            this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                          ICON_PATH___favorite_on + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                        case 2:
                            this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                          ICON_PATH___favorite_on2 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                        case 3:
                            this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                          ICON_PATH___favorite_on3 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                    }
                }
                else{
                    this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                  ICON_PATH___favorite_off + "'); background-repeat:no-repeat; background-position:center center; } ");
                }
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void BugsPDAlbumDetail::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){

            if(jsonObj.contains("message") && (jsonObj["message"].toString() == "정상")){
                if(this->track_star_fav[this->track_idx_fav] > 0){
                    switch (this->track_star_fav[this->track_idx_fav]) {
                    case 1:
                        this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                                               ICON_PATH___favorite_on + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                    case 2:
                        this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                                               ICON_PATH___favorite_on2 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                    case 3:
                        this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                                               ICON_PATH___favorite_on3 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                    }
                }
                else{
                    this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                                           ICON_PATH___favorite_off + "'); background-repeat:no-repeat; background-position:center center; } ");
                }
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
     * @brief [Slot] AlbumImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void BugsPDAlbumDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

        if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx2 ||
                clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx3 || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){

            if(this->flag_check_playlist == false){

                this->playlist_star_fav = this->pd_album_detail_info->getFavoritesStars();
                this->flag_playlist_fav = false;

                if(this->playlist_star_fav == 3){
                    this->playlist_star_fav = 0;
                    this->flag_playlist_fav = false;
                }
                else if(this->playlist_star_fav >= 0 && this->playlist_star_fav < 3){
                    this->playlist_star_fav++;
                    this->flag_playlist_fav = true;
                }

                if(this->playlist_star_fav == 0 || this->playlist_star_fav == 1){
                    // Bugs Favorite
                    ItemPositionData itemPosData;
                    itemPosData.section = SECTION_FOR_MORE_POPUP___pd_album;
                    itemPosData.index = 0;
                    itemPosData.data_id = QString("%1").arg(this->data_pd_album.es_album_id);

                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    connect(proc, &bugs::ProcBugsAPI::completeReq_favarite_pd_album, this, &BugsPDAlbumDetail::slot_bugs_completeReq_pdAlbum_myFavoritesIds);

                    if(this->playlist_star_fav == 0){
                        itemPosData.likes_yn = false;

                        proc->request_bugs_deleteFavorite_pd_album(data_pd_album.es_album_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }
                    else if(this->playlist_star_fav == 1){
                        itemPosData.likes_yn = true;

                        proc->request_bugs_addFavorite_pd_album(this->data_pd_album.es_album_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }

                    this->flag_send_playlist = true;
                }

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_pdAlbum = new roseHome::ProcCommon(this);
                connect(proc_favCheck_pdAlbum, &roseHome::ProcCommon::completeCheck_rating_playlist, this, &BugsPDAlbumDetail::slot_applyResult_checkRating_pdAlbum);
                proc_favCheck_pdAlbum->request_rose_checkRating_Playlist("BUGS", QString("%1").arg(this->data_pd_album.es_album_id));

                this->flag_check_playlist = true;
            }
        }
        else{
            if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::AddCollection){

                QJsonObject json = QJsonObject();
                json.insert("tracks", this->jsonArr_tracks_toPlay);

                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::BUGS, json, this);
                dialog_playlist->request_playlist_fetch();
                connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &BugsPDAlbumDetail::slot_add_rosePlaylist_withBugs);
                int result = dialog_playlist->exec();

                if(result == QDialog::Accepted){
                    delete dialog_playlist;
                }
            }
            else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){

                // OptMorePopup 띄우기 - 즐겨찾기 관련 메뉴를 가림. DetailView에 하트가 있음
                this->makeObj_optMorePopup(OptMorePopup::Bugs_PD_Album, bugs::ConvertData_forBugs::getConvertOptHeaderData(this->data_pd_album), 0, SECTION_FOR_MORE_POPUP___pd_album, true);
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

                this->shareLink = this->pd_album_detail_info->detailInfo_shareLink;    //220905 share code ... --

                //220816 share code ... --
                setUIShare();//c220818
            }
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
    void BugsPDAlbumDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___pd_album){
            this->proc_clicked_optMorePopup_fromPD_Album(this->data_pd_album, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }

}
