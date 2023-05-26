#include "rosetube/rosetube_Collection.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/rosesettings.h"

#include "login/dialoglogin.h"

#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"

#include <QDebug>
#include <QScroller>


namespace rosetube {

    const int HTTP_CACHING_LOCK = 0;
    const int HTTP_CACHING_CONTENTS = 1;
    const int HTTP_CACHING_LOCK_ITEM = 2;
    const int HTTP_CACHING_LOCK_ROSE = 3;
    const int HTTP_CACHING_DELETE_ITEM = 4;

    const int HTTP_SET_QUEUE = 99;

    // subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX_SUBTITLE_MyPlaylists = 0;
    const int BTN_IDX_SUBTITLE_MyFavorites = 1;
    const int BTN_IDX_SUBTITLE_MyCaching = 2;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___MyPlaylists = 0;
    const int SECTION_FOR_MORE_POPUP___MyFavorites = 1;
    const int SECTION_FOR_MORE_POPUP___MyCaching = 2;


    RoseTube_Collection::RoseTube_Collection(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_rosefilter, parent){

        this->linker = Linker::getInstance();
        connect(this->linker, SIGNAL(signal_logined()), SLOT(slot_getMyInfo_loginAfter()));
        connect(this->linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));
        connect(this->linker, SIGNAL(signal_cacheState_change()), SLOT(slot_change_cache_state()));
        connect(this->linker, SIGNAL(signal_cacheList_change()), SLOT(slot_change_cache_list()));

        this->list_MyPlaylist = new QList<roseHome::PlaylistItemData>();
        this->list_MyFavorite = new QList<roseHome::TrackItemData>();
    }


    RoseTube_Collection::~RoseTube_Collection(){

        this->deleteLater();
    }


    void RoseTube_Collection::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            this->page = pageCode;

            // init data
            this->list_MyPlaylist->clear();
            this->list_MyFavorite->clear();

            this->jsonArr_myFavorite = QJsonArray();
            this->jsonArr_myCaching = QJsonArray();
            this->jsonArr_myCachLock = QJsonArray();
            this->jsonObj_CacheLock = QJsonObject();

            this->flag_myFavorite[0] = false;
            this->flag_myPlaylist[0] = false;
            this->flag_myCaching[0] = false;

            this->flag_myFavorite[1] = false;
            this->flag_myPlaylist[1] = false;
            this->flag_myCaching[1] = false;

            this->flag_page_reload = false;
        }
        else{
            this->flag_myFavorite_check[0] = false;
            this->flag_myPlaylist_check[0] = false;

            this->flag_myFavorite_check[1] = false;
            this->flag_myPlaylist_check[1] = false;

            this->flag_page_reload = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
            connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &RoseTube_Collection::slot_applyResult_MyPlaylistCheck);
            proc_myPlaylist->request_rose_getList_myPlaylists("member/playlist" , "YOUTUBE", 0, 15);

            roseHome::ProcCommon *proc_myFavorite = new roseHome::ProcCommon(this);
            connect(proc_myFavorite, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseTube_Collection::slot_applyResult_favoriteTracksCheck);
            proc_myFavorite->request_rose_getList_favoriteTracks("TRACK_RECENT", "YOUTUBE", 0, 15);

            QJsonObject json;

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_lock";
            network->request(HTTP_CACHING_LOCK,
                             Url,
                             json,
                             true,
                             true);
        }
    }


    void RoseTube_Collection::setActivePage(){

        if(this->flagNeedReload){

            if(global.user.isValid() == true){

                // 항상 부모클래스의 함수 먼저 호출
                AbstractRoseHomeSubWidget::setActivePage();

                this->box_contents->removeWidget(this->widget_rose_contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->box_rose_contents = new QVBoxLayout();
                this->box_rose_contents->setSpacing(0);
                this->box_rose_contents->setContentsMargins(0, 0, 0, 0);

                this->widget_rose_contents = new QWidget();
                this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_rose_contents->setLayout(this->box_rose_contents);

                this->widget_login_contents = setUIControl_LoginBefore();
                this->widget_login_contents->hide();

                this->stackedWidget_Contents = new QStackedWidget();
                this->stackedWidget_Contents->addWidget(this->widget_rose_contents);
                this->stackedWidget_Contents->addWidget(this->widget_login_contents);

                this->box_contents->addWidget(this->stackedWidget_Contents, 0, Qt::AlignTop);
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                // init UI
                this->vBox_myPlaylist = new QVBoxLayout();
                this->vBox_myFavorite = new QVBoxLayout();
                this->vBox_myCaching = new QVBoxLayout();

                this->hBox_myPlaylist = new QHBoxLayout();
                this->hBox_myFavorite = new QHBoxLayout();
                this->hBox_myCaching = new QHBoxLayout();

                GSCommon::clearLayout(this->vBox_myPlaylist);
                GSCommon::clearLayout(this->vBox_myFavorite);
                GSCommon::clearLayout(this->vBox_myCaching);

                GSCommon::clearLayout(this->hBox_myPlaylist);
                GSCommon::clearLayout(this->hBox_myFavorite);
                GSCommon::clearLayout(this->hBox_myCaching);

                // sub Title UI
                for(int i = 0; i < 10; i++){
                    this->lb_subTitle[i] = new QLabel();
                    this->btnView_all[i] = new QPushButton();
                }

                for(int i = 0; i < 15; i++){
                    this->collection_myPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___MyPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                }

                for(int i = 0; i < 15; i++){
                    this->collection_myFavorite[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___MyFavorites, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                }

                for(int i = 0; i < 15; i++){
                    this->collection_myCaching[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___MyCaching, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                }

                if(!global.user.getAccess_token().isEmpty()){

                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223

                    this->setUIControl_requestRose();

                    this->widget_rose_contents->show();
                    this->widget_login_contents->hide();
                    this->stackedWidget_Contents->setCurrentIndex(0);
                }
            }
            else{
                this->box_contents->removeWidget(this->widget_rose_contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->box_rose_contents = new QVBoxLayout();
                this->box_rose_contents->setSpacing(0);
                this->box_rose_contents->setContentsMargins(0, 0, 0, 0);

                this->widget_rose_contents = new QWidget();
                this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_rose_contents->setLayout(this->box_rose_contents);

                this->widget_login_contents = setUIControl_LoginBefore();
                this->widget_login_contents->hide();

                this->stackedWidget_Contents = new QStackedWidget();
                this->stackedWidget_Contents->addWidget(this->widget_rose_contents);
                this->stackedWidget_Contents->addWidget(this->widget_login_contents);

                this->box_contents->addWidget(this->stackedWidget_Contents);

                this->widget_rose_contents->hide();
                this->widget_login_contents->show();
                this->stackedWidget_Contents->setCurrentIndex(1);
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    QWidget* RoseTube_Collection::setUIControl_LoginBefore(){

        QWidget *widget_loginBefore = new QWidget();
        widget_loginBefore->setObjectName("widget_loginBefore");
        widget_loginBefore->setFixedHeight(500);
        widget_loginBefore->setStyleSheet("#widget_loginBefore { background:qlineargradient(x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #222222, stop:1 #464646); }");

        int left, top = 0;
        QLabel *lb_text = new QLabel(widget_loginBefore);
        lb_text->setText(tr("You can use various services through Login."));
        lb_text->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:30px;");

        left = (1500 - lb_text->sizeHint().width()) / 2;
        top = (500 - 55 - 30 - lb_text->sizeHint().height()) / 2;
        lb_text->setGeometry(left, top, lb_text->sizeHint().width(), lb_text->sizeHint().height());

        QPushButton *btn_login = new QPushButton(widget_loginBefore);
        btn_login->setText(tr("Log-in"));
        btn_login->setCursor(Qt::PointingHandCursor);
        btn_login->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:20px;border:3px solid #CCCCCC;border-radius:25;");
        btn_login->setFixedSize(230,55);
        btn_login->setGeometry(635, top + lb_text->sizeHint().height() + 30, 230, 55);


        // 커넥션
        connect(btn_login, &QPushButton::clicked, this, &RoseTube_Collection::slot_showLoginPage);

        return widget_loginBefore;
    }


    void RoseTube_Collection::setUIControl_requestRose(){

        QJsonObject json;

        roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
        connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &RoseTube_Collection::slot_applyResult_MyPlaylist);
        proc_myPlaylist->request_rose_getList_myPlaylists("member/playlist" , "YOUTUBE", 0, 15);

        roseHome::ProcCommon *proc_myFavorite = new roseHome::ProcCommon(this);
        connect(proc_myFavorite, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseTube_Collection::slot_applyResult_favoriteTracks);
        proc_myFavorite->request_rose_getList_favoriteTracks("TRACK_RECENT", "YOUTUBE", 0, 15);

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_lock";
        network->request(HTTP_CACHING_LOCK,
                         Url,
                         json,
                         true,
                         true);
    }


    void RoseTube_Collection::setUIControl_appendWidget_rose(){

        if(this->flag_myPlaylist[0] == true && this->flag_myFavorite[0] == true && this->flag_myCaching[0] == true){

            if(this->flag_myPlaylist[0] == true){
                this->flag_myPlaylist[0] = false;

                this->widget_myPlaylist = new QWidget();
                QString subTitle = tr("My playlists");
                if(this->list_MyPlaylist->size() > 0){
                    this->widget_myPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->list_MyPlaylist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_MyPlaylists, this->vBox_myPlaylist);
                }
                else{
                    this->widget_myPlaylist = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_MyPlaylists, this->vBox_myPlaylist);
                }

                this->vBox_myPlaylist->addSpacing(10);

                //----------------------------------------------------------------------------------------------------  BODY : START
                this->hBox_myPlaylist->setSpacing(0);
                this->hBox_myPlaylist->setContentsMargins(0, 0, 0, 0);
                this->hBox_myPlaylist->setAlignment(Qt::AlignTop);
                this->hBox_myPlaylist->setSizeConstraint(QLayout::SetFixedSize);

                QWidget *widget_content = new QWidget;
                widget_content->setLayout(this->hBox_myPlaylist);
                widget_content->setContentsMargins(0, 0, 0, 0);

                QScrollArea *playlist_scrollArea = new QScrollArea();
                playlist_scrollArea->setWidget(widget_content);
                playlist_scrollArea->setWidgetResizable(false);
                playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                playlist_scrollArea->setFixedHeight(311);

                QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_myPlaylist->addWidget(playlist_scrollArea);

                if(this->flag_myPlaylist[1] == true){
                    this->flag_myPlaylist[1] = false;

                    int maxCount = 0;
                    if(this->list_MyPlaylist->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_MyPlaylist->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->collection_myPlaylist[i]->setData(this->list_MyPlaylist->at(i));
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->collection_myPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseTube_Collection::slot_clickedItemPlaylist);
                        this->hBox_myPlaylist->addWidget(this->collection_myPlaylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 311);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_myPlaylist->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_myPlaylist);
                this->box_rose_contents->addSpacing(30);
            }

            if(this->flag_myFavorite[0] == true){
                this->flag_myFavorite[0] = false;

                this->widget_myFavorite = new QWidget();
                QString subTitle = tr("Favorites");
                if(this->list_MyFavorite->size() > 0){
                    this->widget_myFavorite = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(this->list_MyFavorite->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_MyFavorites, this->vBox_myFavorite);
                }
                else{
                    this->widget_myFavorite = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", BTN_IDX_SUBTITLE_MyFavorites, this->vBox_myFavorite);
                }

                this->vBox_myFavorite->addSpacing(10);

                //----------------------------------------------------------------------------------------------------  BODY : START
                this->hBox_myFavorite->setSpacing(0);
                this->hBox_myFavorite->setContentsMargins(0, 0, 0, 0);
                this->hBox_myFavorite->setAlignment(Qt::AlignTop);
                this->hBox_myFavorite->setSizeConstraint(QLayout::SetFixedSize);

                QWidget *widget_content = new QWidget;
                widget_content->setLayout(this->hBox_myFavorite);
                widget_content->setContentsMargins(0, 0, 0, 0);

                QScrollArea *playlist_scrollArea = new QScrollArea();
                playlist_scrollArea->setWidget(widget_content);
                playlist_scrollArea->setWidgetResizable(false);
                playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                playlist_scrollArea->setFixedHeight(308);

                QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_myFavorite->addWidget(playlist_scrollArea, 0, Qt::AlignTop);

                if(this->flag_myFavorite[1] == true){
                    this->flag_myFavorite[1] = false;

                    int maxCount = 0;
                    if(this->list_MyFavorite->size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->list_MyFavorite->size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->collection_myFavorite[i]->setData(this->jsonArr_myFavorite.at(i).toObject());
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->collection_myFavorite[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTube_Collection::slot_clickedItemPlaylist);
                        this->hBox_myFavorite->addWidget(this->collection_myFavorite[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                    noData_widget->setFixedSize(1500, 308);
                    noData_widget->setObjectName("NoData");

                    this->hBox_myFavorite->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_myFavorite);
                this->box_rose_contents->addSpacing(30);
            }

            if(this->flag_myCaching[0] == true){
                this->flag_myCaching[0] = false;

                this->widget_myCaching = new QWidget();
                QString subTitle = tr("Cached Contents");
                this->widget_myCaching = this->setUIControl_subTitle_withSideBtn(subTitle, "View All", BTN_IDX_SUBTITLE_MyCaching, this->vBox_myCaching);

                this->vBox_myCaching->addSpacing(10);

                //----------------------------------------------------------------------------------------------------  BODY : START
                this->hBox_myCaching->setSpacing(0);
                this->hBox_myCaching->setContentsMargins(0, 0, 0, 0);
                this->hBox_myCaching->setAlignment(Qt::AlignTop);
                this->hBox_myCaching->setSizeConstraint(QLayout::SetFixedSize);

                QWidget *widget_content = new QWidget;
                widget_content->setLayout(this->hBox_myCaching);
                widget_content->setContentsMargins(0, 0, 0, 0);

                QScrollArea *playlist_scrollArea = new QScrollArea();
                playlist_scrollArea->setWidget(widget_content);
                playlist_scrollArea->setWidgetResizable(false);
                playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                playlist_scrollArea->setFixedHeight(308);

                QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_myCaching->addWidget(playlist_scrollArea, 0, Qt::AlignTop);

                if(this->flag_myCaching[1] == true){
                    this->flag_myCaching[1] = false;

                    int maxCount = 0;
                    if(this->jsonArr_myCaching.size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->jsonArr_myCaching.size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        QJsonObject tmpCaching = this->jsonArr_myCaching.at(i).toObject();
                        tmpCaching.insert("favorite_view", false);

                        QString trackId = ProcJsonEasy::getString(tmpCaching, "id");

                        if(this->jsonObj_CacheLock.contains(trackId)){
                            tmpCaching.insert("flag_lock", true);

                            this->jsonArr_myCachLock.append(true);
                        }
                        else{
                            tmpCaching.insert("flag_lock", false);

                            this->jsonArr_myCachLock.append(false);
                        }

                        this->collection_myCaching[i]->setData(tmpCaching);
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->collection_myCaching[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTube_Collection::slot_clickedItemPlaylist);
                        this->hBox_myCaching->addWidget(this->collection_myCaching[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                    noData_widget->setFixedSize(1500, 308);
                    noData_widget->setObjectName("NoData");

                    this->hBox_myCaching->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_myCaching);
                this->box_rose_contents->addSpacing(30);
            }

            ContentLoadingwaitingMsgHide();
        }
    }


    void RoseTube_Collection::setUIControl_checkWidget_rose(){

        if(this->flag_myPlaylist_check[0] == true && this->flag_myFavorite_check[0] == true){

            if(this->flag_myPlaylist_check[1] == true){
                this->flag_myPlaylist_check[0] = false;
                this->flag_myPlaylist_check[1] = false;

                QString subTitle = tr("My playlists");
                if(this->list_MyPlaylist->size() > 0){
                    this->lb_subTitle[BTN_IDX_SUBTITLE_MyPlaylists]->setText(subTitle + QString(" (%1)").arg(this->list_MyPlaylist->at(0).totalCount));
                }
                else{
                    this->lb_subTitle[BTN_IDX_SUBTITLE_MyPlaylists]->setText(subTitle + QString(" (0)"));
                }

                QWidget *tmpWidget = this->hBox_myPlaylist->itemAt(0)->widget();
                //qDebug() << this->hBox_myPlaylist->count() << tmpWidget->objectName();

                if(tmpWidget->objectName() != "NoData"){
                    for(int i = 0; i < this->hBox_myPlaylist->count(); i++){
                        this->collection_myPlaylist[i]->hide();
                        this->collection_myPlaylist[i]->disconnect();
                        this->hBox_myPlaylist->removeWidget(this->collection_myPlaylist[i]);
                    }
                }
                GSCommon::clearLayout(this->hBox_myPlaylist);

                int maxCount = 0;
                if(this->list_MyPlaylist->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_MyPlaylist->size();
                }

                if(maxCount > 0){
                    for(int i = 0; i < maxCount; i++){
                        this->collection_myPlaylist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___MyPlaylists, tidal::AbstractItem::ImageSizeMode::Square_200x200, 5, true);
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->collection_myPlaylist[i]->setData(this->list_MyPlaylist->at(i));
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->collection_myPlaylist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseTube_Collection::slot_clickedItemPlaylist);
                        this->hBox_myPlaylist->addWidget(this->collection_myPlaylist[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                    noData_widget->setFixedSize(1500, 311);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_myPlaylist->addWidget(noData_widget);
                }
            }

            if(this->flag_myFavorite_check[1] == true){
                this->flag_myFavorite_check[0] = false;
                this->flag_myFavorite_check[1] = false;

                QString subTitle = tr("Favorites");
                if(this->list_MyFavorite->size() > 0){
                    this->lb_subTitle[BTN_IDX_SUBTITLE_MyFavorites]->setText(subTitle + QString(" (%1)").arg(this->list_MyFavorite->at(0).totalCount));
                }
                else{
                    this->lb_subTitle[BTN_IDX_SUBTITLE_MyFavorites]->setText(subTitle + QString(" (0)"));
                }

                QWidget *tmpWidget = this->hBox_myFavorite->itemAt(0)->widget();
                //qDebug() << this->hBox_myFavorite->count() << tmpWidget->objectName();

                if(tmpWidget->objectName() != "NoData"){
                    for(int i = 0; i < this->hBox_myFavorite->count(); i++){
                        this->collection_myFavorite[i]->hide();
                        this->collection_myFavorite[i]->disconnect();
                        this->hBox_myFavorite->removeWidget(this->collection_myFavorite[i]);
                    }
                }
                GSCommon::clearLayout(this->hBox_myFavorite);

                int maxCount = 0;
                if(this->list_MyFavorite->size() > 15){
                    maxCount = 15;
                }
                else{
                    maxCount = this->list_MyFavorite->size();
                }

                if(maxCount > 0){
                    for(int i = 0; i < maxCount; i++){
                        this->collection_myFavorite[i] = new rosetube::ItemTrack_rosetube(i, BTN_IDX_SUBTITLE_MyFavorites, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->collection_myFavorite[i]->setData(this->jsonArr_myFavorite.at(i).toObject());
                        QCoreApplication::processEvents();
                    }

                    for(int i = 0; i < maxCount; i++){
                        connect(this->collection_myFavorite[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTube_Collection::slot_clickedItemPlaylist);
                        this->hBox_myFavorite->addWidget(this->collection_myFavorite[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                    noData_widget->setFixedSize(1500, 308);
                    noData_widget->setObjectName("Nodata");

                    this->hBox_myFavorite->addWidget(noData_widget);
                }
            }

            ContentLoadingwaitingMsgHide();
        }
    }


    void RoseTube_Collection::setUIControl_appendWidget_Delete(){

        QWidget *tmpWidget = this->hBox_myCaching->itemAt(0)->widget();
        //qDebug() << this->hBox_myPlaylist->count() << tmpWidget->objectName();

        if(tmpWidget->objectName() != "NoData"){
            for(int i = 0; i < this->hBox_myCaching->count(); i++){
                this->collection_myCaching[i]->hide();
                this->collection_myCaching[i]->disconnect();
                this->hBox_myCaching->removeWidget(this->collection_myCaching[i]);
            }
        }
        GSCommon::clearLayout(this->hBox_myCaching);

        int maxCount = 0;
        if(this->jsonArr_myCaching.size() > 15){
            maxCount = 15;
        }
        else{
            maxCount = this->jsonArr_myCaching.size();
        }

        if(maxCount > 0){
            for(int i = 0; i < maxCount; i++){
                this->collection_myCaching[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___MyCaching, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
            }

            for(int i = 0; i < maxCount; i++){
                QJsonObject tmpCaching = this->jsonArr_myCaching.at(i).toObject();
                tmpCaching.insert("favorite_view", false);

                QString trackId = ProcJsonEasy::getString(tmpCaching, "id");

                if(this->jsonObj_CacheLock.contains(trackId)){
                    tmpCaching.insert("flag_lock", true);

                    this->jsonArr_myCachLock.append(true);
                }
                else{
                    tmpCaching.insert("flag_lock", false);

                    this->jsonArr_myCachLock.append(false);
                }

                this->collection_myCaching[i]->setData(tmpCaching);
                QCoreApplication::processEvents();
            }

            for(int i = 0; i < maxCount; i++){
                connect(this->collection_myCaching[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTube_Collection::slot_clickedItemPlaylist);
                this->hBox_myCaching->addWidget(this->collection_myCaching[i]);
            }
        }
        else{            
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
            noData_widget->setFixedSize(1500, 308);
            noData_widget->setObjectName("Nodata");

            this->hBox_myCaching->addWidget(noData_widget);
        }

        ContentLoadingwaitingMsgHide();
    }


    QWidget* RoseTube_Collection::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

        // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.
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
            this->btnView_all[btnId]->setProperty("title", subTitle);
            this->btnView_all[btnId]->setCursor(Qt::PointingHandCursor);
            connect(this->btnView_all[btnId], SIGNAL(clicked()), this, SLOT(slot_clickBtn_subTitle_viewAll()));

            tmp_hBox->addWidget(this->btnView_all[btnId], 1, Qt::AlignVCenter | Qt::AlignRight);
        }

        // Apply Main Layout with spacing
        p_layout->addWidget(widget_box_subTitle);

        return widget_box_subTitle;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RoseTube_Collection::slot_applyResult_MyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_MyPlaylist->append(list_data);
            this->flag_myPlaylist[1] = true;
        }

        this->flag_myPlaylist[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseTube_Collection::slot_applyResult_favoriteTracks(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_MyFavorite->append(list_data);
            this->jsonArr_myFavorite = jsonArr_dataToPlay;
            this->flag_myFavorite[1] = true;
        }

        this->flag_myFavorite[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseTube_Collection::slot_applyResult_MyPlaylistCheck(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_MyPlaylist->count() == 0){
                this->list_MyPlaylist->clear();
                this->list_MyPlaylist->append(list_data);

                this->flag_myPlaylist_check[1] = true;
            }
            else{
                int maxCount = (list_data.count() > this->list_MyPlaylist->count()) ? this->list_MyPlaylist->count() : list_data.count();

                for(int i = 0; i < maxCount; i++){
                    if((list_data.at(i).id != this->list_MyPlaylist->at(i).id) || (list_data.at(i).title != this->list_MyPlaylist->at(i).title) || (list_data.at(i).star != this->list_MyPlaylist->at(i).star)){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_MyPlaylist->at(0).totalCount) || (change_flag > 0)){
                    this->list_MyPlaylist->clear();
                    this->list_MyPlaylist->append(list_data);

                    this->flag_myPlaylist_check[1] = true;
                }
            }
        }
        else if(list_data.size() == 0 && this->list_MyPlaylist->size() != 0){
            this->list_MyPlaylist->clear();

            this->flag_myPlaylist_check[1] = true;
        }

        this->flag_myPlaylist_check[0] = true;
        this->setUIControl_checkWidget_rose();
    }


    void RoseTube_Collection::slot_applyResult_favoriteTracksCheck(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_MyFavorite->count() == 0){
                this->list_MyFavorite->clear();
                this->jsonArr_myFavorite = QJsonArray();

                this->list_MyFavorite->append(list_data);
                this->jsonArr_myFavorite = jsonArr_dataToPlay;

                this->flag_myFavorite_check[1] = true;
            }
            else{
                int maxCount = (list_data.count() > this->list_MyFavorite->count()) ? this->list_MyFavorite->count() : list_data.count();

                for(int i = 0; i < maxCount; i++){
                    if(jsonArr_dataToPlay.at(i) != this->jsonArr_myFavorite.at(i)){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_MyFavorite->at(0).totalCount) || (change_flag > 0)){
                    this->list_MyFavorite->clear();
                    this->jsonArr_myFavorite = QJsonArray();

                    this->list_MyFavorite->append(list_data);
                    this->jsonArr_myFavorite = jsonArr_dataToPlay;

                    this->flag_myFavorite_check[1] = true;
                }
            }
        }
        else if(list_data.size() == 0 && this->list_MyFavorite->count() != 0){
            this->list_MyFavorite->clear();

            this->flag_myFavorite_check[1] = true;
        }

        this->flag_myFavorite_check[0] = true;
        this->setUIControl_checkWidget_rose();
    }


    void RoseTube_Collection::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){

            QJsonObject tmpObj = contents.at(0).toObject();

            if(tmpObj.value("flagOk").toBool() == true && tmpObj.value("message").toString() == "정상"){
                if(this->flag_fav_type == SECTION_FOR_MORE_POPUP___MyFavorites){
                    this->collection_myFavorite[this->flag_fav_idx]->setFavorite_btnHeart(this->flag_fav_star == 0 ? false : true, this->flag_fav_star);
                }
                else if(this->flag_fav_type == SECTION_FOR_MORE_POPUP___MyCaching){
                    this->collection_myCaching[this->flag_fav_idx]->setFavorite_btnHeart(this->flag_fav_star == 0 ? false : true, this->flag_fav_star);
                }
            }
        }

        ContentLoadingwaitingMsgHide();

        if(contents.size() > 0 && this->flag_fav_star == 0){
            roseHome::ProcCommon *proc_myFavorite = new roseHome::ProcCommon(this);
            connect(proc_myFavorite, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseTube_Collection::slot_applyResult_favoriteTracksCheck);
            proc_myFavorite->request_rose_getList_favoriteTracks("TRACK_RECENT", "YOUTUBE", 0, 15);

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
        }
    }


    void RoseTube_Collection::slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj){

        if(p_id == HTTP_CACHING_LOCK){

            if(p_jsonObj.contains("lock")){
                this->jsonObj_CacheLock = ProcJsonEasy::getJsonObject(p_jsonObj, "lock");
            }

            if(this->flag_cache_lock_send == true){
                this->flag_cache_lock_send = false;
                this->collection_myCaching[this->cache_lock_index]->setCacheImgShow(this->cache_lock_state);

                this->jsonArr_myCachLock.replace(this->cache_lock_index, this->cache_lock_state);
            }
            else{
                QJsonObject json;
                json.insert("page", 0);

                NetworkHttp *network = new NetworkHttp();
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/tube_cache_contents_get";
                network->request(HTTP_CACHING_CONTENTS,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
        }
        else if(p_id == HTTP_CACHING_CONTENTS){

            if(p_jsonObj.contains("data")){
                QJsonObject dataObj = ProcJsonEasy::getJsonObject(p_jsonObj, "data");
                QJsonArray jsonArr = ProcJsonEasy::getJsonArray(dataObj, "arr");

                int maxCNT = 0;
                if(jsonArr.size() > 15){
                    maxCNT = 15;
                }
                else{
                    maxCNT = jsonArr.size();
                }

                if(this->flag_page_reload == true){

                    QJsonArray compareArr = QJsonArray();

                    for(int i = 0; i < maxCNT; i++){
                        compareArr.append(jsonArr.at(i).toObject());
                    }

                    if(this->jsonArr_myCaching == compareArr){
                        this->flag_page_reload = false;

                        ContentLoadingwaitingMsgHide();

                        return;
                    }
                    else{
                        this->jsonArr_myCaching = compareArr;
                    }
                }
                else{
                    if(this->flag_cache_delete_send == true){
                        this->jsonArr_myCaching = QJsonArray();
                    }

                    for(int i = 0; i < maxCNT; i++){
                        this->jsonArr_myCaching.append(jsonArr.at(i).toObject());
                    }
                }

                if(maxCNT > 0){
                    this->flag_myCaching[1] = true;
                }
            }

            this->flag_myCaching[0] = true;

            if(this->flag_cache_delete_send == true){
                this->flag_cache_delete_send = false;
                this->setUIControl_appendWidget_Delete();
            }
            else if(this->flag_page_reload == true){
                this->flag_page_reload = false;
                this->setUIControl_appendWidget_Delete();
            }
            else{
                this->setUIControl_appendWidget_rose();
            }
        }
        else if(p_id == HTTP_CACHING_LOCK_ITEM){

            if(p_jsonObj.contains("code") && (p_jsonObj.value("code").toString() == "G0000")){

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_lock";
                QJsonObject json;
                network->request(HTTP_CACHING_LOCK,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
        }
        else if(p_id == HTTP_CACHING_LOCK_ROSE){

            if(p_jsonObj.contains("lock")){
                QJsonObject tmpLock = ProcJsonEasy::getJsonObject(p_jsonObj, "lock");

                for(int i = 0; i < this->jsonArr_myCaching.count(); i++){
                    QJsonObject tmpId = this->jsonArr_myCaching.at(i).toObject();
                    QString cacheId = ProcJsonEasy::getString(tmpId, "id");

                    if(tmpLock.contains(cacheId) == true){
                        this->jsonArr_myCachLock.replace(i, true);
                        this->collection_myCaching[i]->setCacheImgShow(true);
                    }
                    else{
                        this->jsonArr_myCachLock.replace(i, false);
                        this->collection_myCaching[i]->setCacheImgShow(false);
                    }
                }
            }
        }
        else if(p_id == HTTP_CACHING_DELETE_ITEM){

            if(p_jsonObj.contains("code") && (p_jsonObj.value("code").toString() == "G0000")){

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_lock";
                QJsonObject json;
                network->request(HTTP_CACHING_LOCK,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
        }

        sender()->deleteLater();
    }


    void RoseTube_Collection::slot_showLoginPage(){

        this->dlg_rose_login = new Dialog::DialogLogin(this);
        dlg_rose_login->exec();
    }


    void RoseTube_Collection::slot_getMyInfo_loginAfter(){

        if(global.user.isValid() == true){

            this->page = "";
            QJsonObject tmpCode = QJsonObject();
            tmpCode.insert("pageCode", "myCollection");

            this->setJsonObject_forData(tmpCode);
            this->setActivePage();
        }
    }


    void RoseTube_Collection::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_ROSE)){

            global.user.setDeviceChange(false);

            this->page = "";
            QJsonObject tmpCode = QJsonObject();
            tmpCode.insert("pageCode", "myCollection");

            this->setJsonObject_forData(tmpCode);
            this->setActivePage();
        }
    }


    void RoseTube_Collection::slot_change_cache_state(){

        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_lock";
        QJsonObject json;
        network->request(HTTP_CACHING_LOCK_ROSE,
                         Url,
                         json,
                         true,
                         true);
    }


    void RoseTube_Collection::slot_change_cache_list(){

        this->flag_cache_delete_send = true;

        QJsonObject json;
        json.insert("page", 0);

        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/tube_cache_contents_get";
        network->request(HTTP_CACHING_CONTENTS,
                         Url,
                         json,
                         true,
                         true);
    }


    void RoseTube_Collection::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_MyPlaylists){

            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYROSE);
            tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_MYROSE);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedViewAll(tmp_data);
        }
        else if(btnId == BTN_IDX_SUBTITLE_MyFavorites){

            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_COLLECTION_FAV);
            tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_COLLECTION_FAV);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedViewAll(tmp_data);
        }
        else if(btnId == BTN_IDX_SUBTITLE_MyCaching){

            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_COLLECTION_CACHE);
            tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_COLLECTION_CACHE);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedViewAll(tmp_data);
        }
    }

    void RoseTube_Collection::slot_applyResult_getShareLink(const QString &link){//c220828_2

        this->shareLink = link;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RoseTube_Collection::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___MyPlaylists){
            roseHome::PlaylistItemData data_playlist = this->list_MyPlaylist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                print_debug();

                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, roseHome::ConvertData::getConvertOptHeaderData(data_playlist), index, SECTION_FOR_MORE_POPUP___MyPlaylists, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입
                QJsonObject tmp_data;
                tmp_data.insert("id", data_playlist.id);
                tmp_data.insert("ownerId", data_playlist.ownerId);
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYDETAIL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_MYDETAIL);

                // 부모에게 페이지 변경하라고 시그널 보냄
                emit this->signal_clickedViewAll(tmp_data);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___MyFavorites){

            if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx2
                    || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx3 || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){

                bool flag_fav = false;
                int star_fav = 0;

                if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add){
                    flag_fav = true;
                    star_fav = 1;
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx2){
                    flag_fav = true;
                    star_fav = 2;
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx3){
                    flag_fav = true;
                    star_fav = 3;
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){
                    flag_fav = true;
                    star_fav = 0;
                }

                this->flag_fav_idx = index;
                this->flag_fav_star = star_fav;
                this->flag_fav_type = SECTION_FOR_MORE_POPUP___MyFavorites;

                QJsonObject track = this->jsonArr_myFavorite.at(index).toObject();

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", flag_fav);
                ratingInfo.insert("star", star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", "YOUTUBE");

                QJsonObject json;
                json.insert("track", track);
                json.insert("ratingInfo", ratingInfo);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseTube_Collection::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, flag_fav, star_fav);

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }
            else{

                if(clickMode == tidal::AbstractItem::ClickMode::AllBox || clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
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

                    //roseHome::TrackItemData data_playlist;
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byTracks(this->list_MyFavorite->at(index), this->jsonArr_myFavorite, index, playType);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    QJsonObject tmpObj = this->jsonArr_myFavorite.at(index).toObject();

                    QJsonObject data = ProcJsonEasy::getJsonObject(tmpObj, "data");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(data, "snippet");

                    OptMorePopup::HeaderData data_header;
                    data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                    data_header.sub_title = ProcJsonEasy::getString(snippet, "channelTitle");
                    data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                    data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(tmpObj, "id"));
                    data_header.type = "YOUTUBE";      //j220906 share link
                    data_header.flagProcStar = false;
                    data_header.isShare = true;      //j220906 share link

                    // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Rosetube_Video, data_header, index, section);
                }
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___MyCaching){

            if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx2
                    || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx3 || clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){

                bool flag_fav = false;
                int star_fav = 0;

                if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add){
                    flag_fav = true;
                    star_fav = 1;
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx2){
                    flag_fav = true;
                    star_fav = 2;
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Addx3){
                    flag_fav = true;
                    star_fav = 3;
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){
                    flag_fav = true;
                    star_fav = 0;
                }

                this->flag_fav_idx = index;
                this->flag_fav_star = star_fav;
                this->flag_fav_type = SECTION_FOR_MORE_POPUP___MyCaching;

                QJsonObject track = this->jsonArr_myCaching.at(index).toObject();

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", flag_fav);
                ratingInfo.insert("star", star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", "YOUTUBE");

                QJsonObject json;
                json.insert("track", track);
                json.insert("ratingInfo", ratingInfo);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseTube_Collection::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, flag_fav, star_fav);

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }
            else{

                if(clickMode == tidal::AbstractItem::ClickMode::AllBox || clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
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

                    // needed to get JsonArray rearranged to play on the Rose
                    QJsonArray youtube = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_myCaching, index, playType);

                    // Rose Play 요청
                    QJsonObject json = QJsonObject();
                    json.insert("youtube", youtube);
                    json.insert("youtubePlayType", curr_clickMode);
                    json.insert("roseToken", global.device.getDeviceRoseToken());

                    NetworkHttp *network = new NetworkHttp();
                    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                    QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/youtubePlay.playlist.add";
                    network->request(HTTP_SET_QUEUE,
                                     Url,
                                     json,
                                     true,
                                     true);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    QJsonObject tmpObj = this->jsonArr_myCaching.at(index).toObject();

                    OptMorePopup::HeaderData data_header;
                    data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                    data_header.sub_title = ProcJsonEasy::getString(tmpObj, "channelName");
                    data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                    data_header.data_pk = "https://youtu.be/" + ProcJsonEasy::getString(tmpObj, "id");
                    data_header.type = "YOUTUBE";      //j220906 share link
                    data_header.cache_lock = this->jsonArr_myCachLock.at(index).toBool();
                    data_header.flagProcStar = false;
                    data_header.isShare = true;      //j220906 share link

                    // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Rosetube_Caching, data_header, index, section);
                }
            }
        }
    }


    void RoseTube_Collection::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___MyPlaylists){
            if(clickMode == OptMorePopup::ClickMode::Edit){
                QString view_type = "edit";

                QJsonObject data;
                data.insert("view_type", view_type);
                data.insert("playlist_id", this->list_MyPlaylist->at(index).id);
                data.insert("type", "ROSE");

                QJsonObject jsonObj_move;
                jsonObj_move.insert("data", data);

                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RT_ADDPLAYLIST);

                emit linker->signal_clickedMovePage(jsonObj_move);
            }
            else{
                this->proc_clicked_optMorePopup_fromPlaylist(this->list_MyPlaylist, index, SECTION_FOR_MORE_POPUP___MyPlaylists, clickMode);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___MyFavorites){
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
                roseHome::TrackItemData data_track;
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data_track, this->jsonArr_myFavorite, index, clickMode);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubscribInfo){
                QJsonObject tmpObj = this->jsonArr_myFavorite.at(index).toObject();
                QJsonObject dataObj = ProcJsonEasy::getJsonObject(tmpObj, "data");
                QJsonObject snippet = ProcJsonEasy::getJsonObject(dataObj, "snippet");
                QString channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                QString channelId = ProcJsonEasy::getString(snippet, "channelId");

                QJsonObject tmp_data;
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert("title", channelName);
                tmp_data.insert("channel_id", channelId);

                emit this->signal_clickedViewAll(tmp_data);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___MyCaching){

            QJsonObject tmpObj = this->jsonArr_myCaching.at(index).toObject();

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
                // needed to get JsonArray rearranged to play on the Rose
                int playType = this->get_rose_playType(clickMode);
                QJsonArray youtube = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_myCaching, index, clickMode);

                // Rose Play 요청
                QJsonObject json = QJsonObject();
                json.insert("youtube", youtube);
                json.insert("youtubePlayType", playType);
                json.insert("roseToken", global.device.getDeviceRoseToken());

                NetworkHttp *network = new NetworkHttp();
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/youtubePlay.playlist.add";
                network->request(HTTP_SET_QUEUE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubscribInfo){

                QString channelName = ProcJsonEasy::getString(tmpObj, "channelName");
                QString channelId = ProcJsonEasy::getString(tmpObj, "channelId");

                QJsonObject tmp_data;
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert("title", channelName);
                tmp_data.insert("channel_id", channelId);

                emit this->signal_clickedViewAll(tmp_data);
            }
            else if(clickMode == OptMorePopup::ClickMode::Cache_Lock){

                if(this->flag_cache_lock_send == false){
                    QJsonObject json;
                    json.insert("isLock", true);
                    json.insert("data", tmpObj);

                    NetworkHttp *network = new NetworkHttp();
                    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                    QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/tube_cache_lock_item";
                    network->request(HTTP_CACHING_LOCK_ITEM,
                                     Url,
                                     json,
                                     true,
                                     true);

                    this->cache_lock_index = index;
                    this->cache_lock_state = true;
                    this->flag_cache_lock_send = true;
                }
            }
            else if(clickMode == OptMorePopup::ClickMode::Cache_Unlock){

                if(this->flag_cache_lock_send == false){
                    QJsonObject json;
                    json.insert("isLock", false);
                    json.insert("data", tmpObj);

                    NetworkHttp *network = new NetworkHttp();
                    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                    QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/tube_cache_lock_item";
                    network->request(HTTP_CACHING_LOCK_ITEM,
                                     Url,
                                     json,
                                     true,
                                     true);

                    this->cache_lock_index = index;
                    this->cache_lock_state = false;
                    this->flag_cache_lock_send = true;
                }
            }
            else if(clickMode == OptMorePopup::ClickMode::Delete){

                if(this->flag_cache_delete_send == false){
                    QJsonObject json;
                    json.insert("data", tmpObj);
                    json.insert("isDelete", true);

                    NetworkHttp *network = new NetworkHttp();
                    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                    QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/tube_cache_download_item";
                    network->request(HTTP_CACHING_DELETE_ITEM,
                                     Url,
                                     json,
                                     true,
                                     true);

                    this->flag_cache_delete_send = true;

                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                }
            }
        }
    }


    /**
     * @brief OptMorePopup::ClickMode 에 따라 Rose로 요청할 playType 정보를 int 로 반환한다.
     * @details Tidal의 음원, 비디오 모두 똑같다. playType 옵션값이.   음악, 비디오 등의 메뉴에서도 똑같은지는 확인해봐야.
     * @param clickMode
     * @return
     */
    int RoseTube_Collection::get_rose_playType(OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
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
            // 현재 큐 끝에 추가
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
            // 여기부터 재생
            return 17;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty){
            // 큐 비우고 여기부터 재생
            return 18;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last){
            // 여기부터 큐 끝에 추가
            return 19;
        }
        else{
            return 0;           // unknown or not supported
        }
    }


    /**
     * @brief OptMorePopup::ClickMode에 따라서 JsonArray(Track정보)를 재가공해서 반환한다.
     * @param jsonArr_toPlayAll
     * @param curr_index
     * @param clickMode
     * @return
     */
    QJsonArray RoseTube_Collection::get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
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
    QJsonArray RoseTube_Collection::subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex){

        const int len_data = p_jsonArr.count();

        if(startIndex>0 && startIndex<len_data){
            QJsonArray jsonArr_subRange;
            for(int i=startIndex ; i<len_data ; i++){
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
    QJsonArray RoseTube_Collection::reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex){

        const int len_data = p_jsonArr.count();

        if(startIndex>0 && startIndex<len_data){
            QJsonArray jsonArr_reorder;
            for(int i=startIndex ; i<len_data ; i++){
                jsonArr_reorder.append(p_jsonArr.at(i));
            }
            for(int i=0 ; i<startIndex ; i++){
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
