#include "BugsMyCollection.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"
#include "widget/toastmsg.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include <QDebug>
#include <QScroller>

namespace bugs {


    // subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX___track_like = 1;
    const int BTN_IDX___track_recently = 2;
    const int BTN_IDX___track_most = 3;
    const int BTN_IDX___album_my_album = 4;

    const int SECTION_FOR_MORE_POPUP___track_like = 1;
    const int SECTION_FOR_MORE_POPUP___track_recently = 2;
    const int SECTION_FOR_MORE_POPUP___track_most = 3;
    const int SECTION_FOR_MORE_POPUP___my_album = 4;


    const QString BUGS_API_SUBPAHT___track_like = "mymusic/like/track";
    const QString BUGS_API_SUBPAHT___track_recently = "mymusic/track/listened/recent";
    const QString BUGS_API_SUBPAHT___track_most = "mymusic/track/listened/most";



    /**
     * @brief "BUGS > My Collection" 화면을 위한 생성자.
     * @param parent
     */
    BugsMyCollection::BugsMyCollection(QWidget *parent) : AbstractBugsSubWidget(VerticalScroll_filter, parent) {

        this->list_trackLike = new QList<bugs::TrackItemData>();
        this->list_trackRecently = new QList<bugs::TrackItemData>();
        this->list_trackMost = new QList<bugs::TrackItemData>();
        this->list_albumMyalbum = new QList<bugs::MyAlbumItemData>();


    }

    BugsMyCollection::~BugsMyCollection(){

        this->deleteLater();
    }

    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsMyCollection::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            this->page = pageCode;

            // init data for Bugs
            this->list_trackLike->clear();
            this->list_trackRecently->clear();
            this->list_trackMost->clear();
            this->list_albumMyalbum->clear();

            this->jsonArr_trackLike_toPlay = QJsonArray();
            this->jsonArr_trackRecently_toPlay = QJsonArray();
            this->jsonArr_trackMost_toPlay = QJsonArray();


            flag_trackLike[0] = false;
            flag_trackRecently[0] = false;
            flag_trackMost[0] = false;
            flag_albumMyalbum[0] = false;

            flag_trackLike[1] = false;
            flag_trackRecently[1] = false;
            flag_trackMost[1] = false;
            flag_albumMyalbum[1] = false;
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }

    /**
     * @brief BugsMyCollection::setActivePage
     */
    void BugsMyCollection::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            this->box_contents->removeWidget(this->widget_main_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            //---------------------//c220729  start


            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(this->widget_main_contents, 0, Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);


            //--------------------------------------------//c220729  end
            // init UI
            this->vBox_trackLike = new QVBoxLayout();
            this->vBox_trackRecently = new QVBoxLayout();
            this->vBox_trackMost = new QVBoxLayout();
            this->vBox_albumMyalbum = new QVBoxLayout();

            this->hBox_trackLike = new QHBoxLayout();
            this->hBox_trackRecently = new QHBoxLayout();
            this->hBox_trackMost = new QHBoxLayout();
            this->hBox_albumMyalbum = new QHBoxLayout();

            // init UI
            GSCommon::clearLayout(this->vBox_trackLike);
            GSCommon::clearLayout(this->vBox_trackRecently);
            GSCommon::clearLayout(this->vBox_trackMost);
            GSCommon::clearLayout(this->vBox_albumMyalbum);

            GSCommon::clearLayout(this->hBox_trackLike);
            GSCommon::clearLayout(this->hBox_trackRecently);
            GSCommon::clearLayout(this->hBox_trackMost);
            GSCommon::clearLayout(this->hBox_albumMyalbum);


            // sub Title UI
            for(int i = 0; i < 20; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            for(int i = 0; i < 15; i++){
                this->myCollection_track_like[i] = new bugs::ItemTrackSqure_bugs(i, SECTION_FOR_MORE_POPUP___track_like, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->myCollection_track_like[i], &bugs::ItemTrackSqure_bugs::signal_clicked, this, &BugsMyCollection::slot_clickedItemTrackSqure);
            }

            for (int i = 0; i < 5; i++) {
                this->myCollection_track_recently[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->myCollection_track_recently[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &BugsMyCollection::slot_clickedItemTrack_inList);
                this->myCollection_track_recently[i]->setProperty("index", i);
                this->myCollection_track_recently[i]->setObjectName("recently_tracks");
            }

            for (int i = 0; i < 5; i++) {
                this->myCollection_track_most[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->myCollection_track_most[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &BugsMyCollection::slot_clickedItemTrack_inList);
                this->myCollection_track_most[i]->setProperty("index", i);
                this->myCollection_track_most[i]->setObjectName("most_tracks");
            }

            for(int i = 0; i < 15; i++){
                this->myCollection_album_myAlbum[i] = new bugs::ItemMyAlbum_bugs(i, SECTION_FOR_MORE_POPUP___my_album, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->myCollection_album_myAlbum[i], &bugs::ItemMyAlbum_bugs::signal_clicked, this, &BugsMyCollection::slot_clickedItemMyAlbum);
            }

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->setUIControl_requestBugs();
        }
    }


    void BugsMyCollection::setUIControl_requestBugs(){

        // request HTTP API for like Track
        ProcBugsAPI *proc_like = new ProcBugsAPI(this);
        connect(proc_like, &ProcBugsAPI::completeReq_list_tracks, this, &BugsMyCollection::slot_applyResult_trackLike);
        proc_like->request_bugs_getList_tracks(BUGS_API_SUBPAHT___track_like, 10, 1);

        // request HTTP API for Recently Track
        ProcBugsAPI *proc_recently = new ProcBugsAPI(this);
        connect(proc_recently, &ProcBugsAPI::completeReq_list_tracks, this, &BugsMyCollection::slot_applyResult_trackRecently);
        proc_recently->request_bugs_getList_tracks(BUGS_API_SUBPAHT___track_recently, 5, 1);

        // request HTTP API for most Track
        ProcBugsAPI *proc_most = new ProcBugsAPI(this);
        connect(proc_most, &ProcBugsAPI::completeReq_list_tracks, this, &BugsMyCollection::slot_applyResult_trackMost);
        proc_most->request_bugs_getList_tracks(BUGS_API_SUBPAHT___track_most, 5, 1);

        // request HTTP API for my album
        ProcBugsAPI *proc_my = new ProcBugsAPI(this);
        connect(proc_my, &ProcBugsAPI::completeReq_list_my_albums, this, &BugsMyCollection::slot_applyResult_my_albums);
        proc_my->request_bugs_getList_my_albums(15, 1);          // 기본값으로 요청함 (doc 참고함)
    }


    void BugsMyCollection::setUIControl_appendWidget(){

        if(this->flag_trackLike[0] == true && this->flag_trackRecently[0] == true && this->flag_trackMost[0] == true && this->flag_albumMyalbum[0] == true){
            QString title = "";
            if(this->flag_trackLike[1] == true){
                title = "좋아한 음악";
                this->widget_trackLike = new QWidget();
                this->widget_trackLike = this->setUIControl_subTitle_withSideBtn(title + QString(" (%1)").arg(this->list_trackLike->at(0).total_count), "View all", BTN_IDX___track_like, this->vBox_trackLike);

                this->vBox_trackLike->addSpacing(10);

                this->hBox_trackLike = this->setUIControl_hBoxLayout_forAlbum(this->vBox_trackLike);

                for(int i = 0; i < this->list_trackLike->size(); i++){
                    this->myCollection_track_like[i]->setData(this->list_trackLike->at(i));
                    this->hBox_trackLike->addWidget(this->myCollection_track_like[i]);
                }

                this->box_main_contents->addLayout(this->vBox_trackLike);
                this->box_main_contents->addSpacing(45);
            }
            else{
                this->widget_trackLike = new QWidget();
                this->widget_trackLike = this->setUIControl_subTitle_withSideBtn(QString("좋아한 음악 (0)"), "View All", BTN_IDX___track_like, this->vBox_trackLike);

                this->vBox_trackLike->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedHeight(275);
                this->vBox_trackLike->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_trackLike);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_trackRecently[1] == true){
                title = "최근 들은 곡";
                this->widget_trackRecently = new QWidget();
                this->widget_trackRecently = this->setUIControl_subTitle_withSideBtn(title + QString(" (%1)").arg(this->list_trackRecently->at(0).total_count), "View All", BTN_IDX___track_recently, this->vBox_trackRecently);

                this->vBox_trackRecently->addSpacing(10);

                QList<QString> tmp_clientkey;
                for(int i = 0; i < this->list_trackRecently->size(); i++){
                    this->myCollection_track_recently[i]->setDataTrackInfo_Bugs(this->list_trackRecently->at(i), "Recently");
                    this->myCollection_track_recently[i]->resize(1550, 70);

                    this->vBox_trackRecently->addWidget(this->myCollection_track_recently[i]);
                    tmp_clientkey.append(QString("%1").arg(this->list_trackRecently->at(i).track_id));
                }

                this->box_main_contents->addLayout(this->vBox_trackRecently);
                this->box_main_contents->addSpacing(45);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsMyCollection::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("BUGS", tmp_clientkey);
                proc_fav_track->setProperty("whatTrack","recently");
            }
            else{
                this->widget_trackRecently = new QWidget();
                this->widget_trackRecently = this->setUIControl_subTitle_withSideBtn(QString("최근 들은 곡 (0)"), "View All", BTN_IDX___track_recently, this->vBox_trackRecently);

                this->vBox_trackRecently->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedHeight(275);
                this->vBox_trackRecently->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_trackRecently);
                this->box_main_contents->addSpacing(30);
            }


            if(this->flag_trackMost[1] == true){
                title = "많이 들은 곡";
                this->widget_trackMost = new QWidget();
                this->widget_trackMost = this->setUIControl_subTitle_withSideBtn(title + QString(" (%1)").arg(this->list_trackMost->at(0).total_count), "View All", BTN_IDX___track_most, this->vBox_trackMost);

                this->vBox_trackMost->addSpacing(10);

                QList<QString> tmp_clientkey;
                for(int i = 0; i < this->list_trackMost->size(); i++){
                    this->myCollection_track_most[i]->setDataTrackInfo_Bugs(this->list_trackMost->at(i), "Most");
                    this->myCollection_track_most[i]->resize(1550, 70);

                    this->vBox_trackMost->addWidget(this->myCollection_track_most[i]);
                    tmp_clientkey.append(QString("%1").arg(this->list_trackMost->at(i).track_id));
                }

                this->box_main_contents->addLayout(this->vBox_trackMost);
                this->box_main_contents->addSpacing(45);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsMyCollection::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("BUGS", tmp_clientkey);
                proc_fav_track->setProperty("whatTrack","most");
            }
            else{
                this->widget_trackMost = new QWidget();
                this->widget_trackMost = this->setUIControl_subTitle_withSideBtn(QString("많이 들은 곡 (0)"), "View All", BTN_IDX___track_most, this->vBox_trackMost);

                this->vBox_trackMost->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedHeight(275);
                this->vBox_trackMost->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_trackMost);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_albumMyalbum[1] == true){
                title = "내 앨범";
                this->widget_albumMyalbum = new QWidget();
                this->widget_albumMyalbum = this->setUIControl_subTitle_withSideBtn(title + QString(" (%1)").arg(this->list_albumMyalbum->at(0).total_count), "View All", BTN_IDX___album_my_album, this->vBox_albumMyalbum);

                this->vBox_albumMyalbum->addSpacing(10);

                this->hBox_albumMyalbum = this->setUIControl_hBoxLayout_forPlaylist(this->vBox_albumMyalbum);

                for(int i = 0; i < this->list_albumMyalbum->size(); i++){
                    this->myCollection_album_myAlbum[i]->setData(this->list_albumMyalbum->at(i));
                    this->hBox_albumMyalbum->addWidget(this->myCollection_album_myAlbum[i]);
                }

                this->box_main_contents->addLayout(this->vBox_albumMyalbum);
                this->box_main_contents->addSpacing(45);
            }
            else{
                this->widget_albumMyalbum = new QWidget();
                this->widget_albumMyalbum = this->setUIControl_subTitle_withSideBtn(QString("내 앨범 (0)"), "View All", BTN_IDX___album_my_album, this->vBox_albumMyalbum);

                this->vBox_albumMyalbum->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedHeight(275);
                this->vBox_albumMyalbum->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_albumMyalbum);
                this->box_main_contents->addSpacing(30);
            }

            ContentLoadingwaitingMsgHide();
        }
    }

    QWidget* BugsMyCollection::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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

    QHBoxLayout* BugsMyCollection::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QHBoxLayout *hBox_album = new QHBoxLayout();
        hBox_album->setSpacing(0);
        hBox_album->setContentsMargins(0, 0, 0, 0);
        hBox_album->setAlignment(Qt::AlignTop);
        hBox_album->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_content = new QWidget;
        widget_content->setLayout(hBox_album);
        widget_content->setContentsMargins(0, 0, 0, 0);

        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_content);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0, 0, 0, 0);
        tmp_scrollArea->setFixedHeight(285);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_album;
    }

    QHBoxLayout* BugsMyCollection::setUIControl_hBoxLayout_forPlaylist(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------
        QHBoxLayout *hBox_playlist = new QHBoxLayout();
        hBox_playlist->setSpacing(0);
        hBox_playlist->setContentsMargins(0,0,0,0);
        hBox_playlist->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_contentSub = new QWidget();
        widget_contentSub->setLayout(hBox_playlist);
        widget_contentSub->setContentsMargins(0,0,0,0);


        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_contentSub);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(305);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------

        QHBoxLayout *hBox_background = new QHBoxLayout();
        hBox_background->addWidget(tmp_scrollArea);

        QWidget *widget_background = new QWidget();
        widget_background->setLayout(hBox_background);
        widget_background->setContentsMargins(0,0,0,0);
        widget_background->setStyleSheet("background-color:#212121; border:0px;");

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_playlist;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------


    void BugsMyCollection::slot_applyResult_trackLike(const QList<bugs::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_trackLike->append(list_data);
            this->jsonArr_trackLike_toPlay = jsonArr_dataToPlay;
            this->flag_trackLike[1] = true;
        }

        this->flag_trackLike[0] = true;
        this->setUIControl_appendWidget();
    }

    void BugsMyCollection::slot_applyResult_trackRecently(const QList<bugs::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_trackRecently->append(list_data);
            this->jsonArr_trackRecently_toPlay = jsonArr_dataToPlay;
            this->flag_trackRecently[1] = true;
        }

        this->flag_trackRecently[0] = true;
        this->setUIControl_appendWidget();
    }

    void BugsMyCollection::slot_applyResult_trackMost(const QList<bugs::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_trackMost->append(list_data);
            this->jsonArr_trackMost_toPlay = jsonArr_dataToPlay;
            this->flag_trackMost[1] = true;
        }

        this->flag_trackMost[0] = true;
        this->setUIControl_appendWidget();
    }

    void BugsMyCollection::slot_applyResult_my_albums(const QList<bugs::MyAlbumItemData> &list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_albumMyalbum->append(list_data);
            this->flag_albumMyalbum[1] = true;
        }

        this->flag_albumMyalbum[0] = true;
        this->setUIControl_appendWidget();
    }

    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Functions
    //
    //-----------------------------------------------------------------------------------------------------------------------

    // 서브 타이틀별 우측 View All -------------------------------------------------------------------------------------------

    /**
     * @brief 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void BugsMyCollection::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX___track_like){
            QString title = this->lb_subTitle[btnId]->text();

            QJsonObject tmp_data;
            tmp_data.insert("title", title);
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS___MY_COLLECTION___LIKE_MUSIC);
            emit this->signal_clickedMovePage(tmp_data);
        }
        else if(btnId == BTN_IDX___track_recently){
            bugs::PageInfo_TrackAllView data_pageInfo;
            data_pageInfo.pathTitle = "최근 들은 곡";
            data_pageInfo.api_subPath = BUGS_API_SUBPAHT___track_recently;
            this->movePage_track_allView(data_pageInfo);
        }
        else if(btnId == BTN_IDX___track_most){
            bugs::PageInfo_TrackAllView data_pageInfo;
            data_pageInfo.pathTitle = "많이 들은 곡";
            data_pageInfo.api_subPath = BUGS_API_SUBPAHT___track_most;
            this->movePage_track_allView(data_pageInfo);              // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX___album_my_album){
            QString title = this->lb_subTitle[btnId]->text();

            QJsonObject tmp_data;
            tmp_data.insert("title", title);
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS___MY_COLLECTION___MYALBUM);
            emit this->signal_clickedMovePage(tmp_data);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief BugsHome::slot_clickedItemTrackSqure
     */
    void BugsMyCollection::slot_clickedItemTrackSqure(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemTrackSquare(this->list_trackLike, this->jsonArr_trackLike_toPlay, clickMode, index, section);
    }


    void BugsMyCollection::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        QString obj_name = sender()->objectName();


        if(obj_name == "recently_tracks"){
            if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

                if(this->flag_check_track == false){
                    this->track_star_fav = this->myCollection_track_recently[idx]->getFavoritesStars();
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
                        itemPosData.section = SECTION_FOR_MORE_POPUP___track_recently;
                        itemPosData.index = idx;
                        itemPosData.data_id = QString("%1").arg(this->list_trackRecently->at(idx).track_id);

                        ProcBugsAPI *proc = new ProcBugsAPI(this);
                        connect(proc, &bugs::ProcBugsAPI::completeReq_favarite_track, this, &BugsMyCollection::slot_bugs_completeReq_listAll_myFavoritesIds);
                        proc->setProperty("whatTrack", "recently");

                        if(this->track_star_fav == 0){
                            itemPosData.likes_yn = false;

                            proc->request_bugs_deleteFavorite_track(this->list_trackRecently->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                        }
                        else if(this->track_star_fav == 1){
                            itemPosData.likes_yn = true;

                            proc->request_bugs_addFavorite_track(this->list_trackRecently->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                        }

                        this->flag_send_track = true;
                    }

                    this->track_idx_fav = idx;

                    // request HTTP API - get favorite for Rose Server
                    roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                    connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsMyCollection::slot_applyResult_checkRating_track);
                    proc_favCheck_track->request_rose_checkRating_Track("BUGS", QString("%1").arg(this->list_trackRecently->at(idx).track_id));
                    proc_favCheck_track->setProperty("whatTrack", "recently");

                    this->flag_check_track = true;
                }
            }
            else{
                this->proc_clicked_itemTrack_inList(this->list_trackRecently, this->jsonArr_trackRecently_toPlay, clickMode, idx, SECTION_FOR_MORE_POPUP___track_recently);
            }

        }
        else if(obj_name == "most_tracks"){
            if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){
                if(this->flag_check_track == false){
                    this->track_star_fav = this->myCollection_track_most[idx]->getFavoritesStars();
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
                        itemPosData.section = SECTION_FOR_MORE_POPUP___track_most;
                        itemPosData.index = idx;
                        itemPosData.data_id = QString("%1").arg(this->list_trackMost->at(idx).track_id);

                        ProcBugsAPI *proc = new ProcBugsAPI(this);
                        connect(proc, &bugs::ProcBugsAPI::completeReq_favarite_track, this, &BugsMyCollection::slot_bugs_completeReq_listAll_myFavoritesIds);
                        proc->setProperty("whatTrack", "most");

                        if(this->track_star_fav == 0){
                            itemPosData.likes_yn = false;

                            proc->request_bugs_deleteFavorite_track(this->list_trackMost->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                        }
                        else if(this->track_star_fav == 1){
                            itemPosData.likes_yn = true;

                            proc->request_bugs_addFavorite_track(this->list_trackMost->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                        }

                        this->flag_send_track = true;
                    }

                    this->track_idx_fav = idx;

                    // request HTTP API - get favorite for Rose Server
                    roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                    connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsMyCollection::slot_applyResult_checkRating_track);
                    proc_favCheck_track->request_rose_checkRating_Track("BUGS", QString("%1").arg(this->list_trackMost->at(idx).track_id));
                    proc_favCheck_track->setProperty("whatTrack", "most");

                    this->flag_check_track = true;
                }
            }
            else{
                this->proc_clicked_itemTrack_inList(this->list_trackMost, this->jsonArr_trackMost_toPlay, clickMode, idx, SECTION_FOR_MORE_POPUP___track_most);
            }
        }
    }


    /**
     * @brief BugsMyCollection::slot_clickedItemMyAlbum
     * @param clickMode
     */
    void BugsMyCollection::slot_clickedItemMyAlbum(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemMyAlbum(this->list_albumMyalbum, clickMode, index, section);
    }


    void BugsMyCollection::slot_applyResult_getShareLink(const QString &link){

        this->shareLink = link;
    }

    //--------------------------------------------------------

    void BugsMyCollection::slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌.
        int idx = ProcJsonEasy::getInt(p_jsonObj, "index");
        int track_id = ProcJsonEasy::getString(p_jsonObj, "data_id").toInt();
        QString whatTrack = sender()->property("whatTrack").toString();

        if(whatTrack == "recently"){
            if(track_id == this->list_trackRecently->at(idx).track_id){
                qDebug() << "likes_yn : " << ProcJsonEasy::getBool(p_jsonObj, "likes_yn");
            }
        }
        else if(whatTrack == "most"){
            if(track_id == this->list_trackMost->at(idx).track_id){
                qDebug() << "likes_yn : " << ProcJsonEasy::getBool(p_jsonObj, "likes_yn");
            }
        }

        this->flag_send_track = false;
    }

    void BugsMyCollection::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

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
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsMyCollection::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
                proc_fav_track->setProperty("whatTrack",sender()->property("whatTrack").toString());
            }
            else{
                QString whatTrack = sender()->property("whatTrack").toString();

                QJsonObject track;

                QJsonObject data;
                if(whatTrack == "recently"){
                    data = this->jsonArr_trackRecently_toPlay[this->track_idx_fav].toObject();
                    track.insert("thumbnailUrl", this->list_trackRecently->at(this->track_idx_fav).album_image);
                }
                else if(whatTrack == "most"){
                    data = this->jsonArr_trackMost_toPlay[this->track_idx_fav].toObject();
                    track.insert("thumbnailUrl", this->list_trackMost->at(this->track_idx_fav).album_image);
                }
                track.insert("data", data);

                track.insert("clientKey", ProcJsonEasy::getInt(data, "track_id"));
                track.insert("duration", 0);
                track.insert("favorite", true);
                track.insert("ownerId", 0);
                track.insert("sort", 0);
                track.insert("star", 1);
                track.insert("title", ProcJsonEasy::getString(data, "track_title"));
                track.insert("type", "BUGS");

                QJsonArray tracks;
                tracks.append(track);

                QJsonObject json;
                json.insert("tracks", tracks);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsMyCollection::slot_applyResult_addRating_track);
                proc_fav_track->request_rose_addRating_Track(json);
                proc_fav_track->setProperty("whatTrack",sender()->property("whatTrack").toString());
            }
        }
        else{

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void BugsMyCollection::slot_applyResult_getRating_track(const QJsonArray &contents){

        QString whatTrack = sender()->property("whatTrack").toString();

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    if(whatTrack == "recently"){
                        this->myCollection_track_recently[i]->setFavoritesIds(flag, star);
                    }
                    else if(whatTrack == "most"){
                        this->myCollection_track_most[i]->setFavoritesIds(flag, star);
                    }
                }

            }
            else if(track_info.contains("message") && (track_info["message"].toString() == "정상")){
                if(whatTrack == "recently"){
                    this->myCollection_track_recently[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
                }
                else if(whatTrack == "most"){
                    this->myCollection_track_most[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
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

    void BugsMyCollection::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        QString whatTrack = sender()->property("whatTrack").toString();

        if(jsonObj.contains("flagOk")){

            if(jsonObj.contains("message") && (jsonObj["message"].toString() == "정상")){
                if(whatTrack == "recently"){
                    this->myCollection_track_recently[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
                }
                else if(whatTrack == "most"){
                    this->myCollection_track_most[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
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
    void BugsMyCollection::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___track_like){
            this->proc_clicked_optMorePopup_fromTrack(this->list_trackLike, this->jsonArr_trackLike_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___track_recently){
            this->proc_clicked_optMorePopup_fromTrack(this->list_trackRecently, this->jsonArr_trackRecently_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___track_most){
            this->proc_clicked_optMorePopup_fromTrack(this->list_trackMost, this->jsonArr_trackMost_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___my_album){
            this->proc_clicked_optMorePopup_fromMyAlbum(this->list_albumMyalbum, index, clickMode);
        }
    }
}

