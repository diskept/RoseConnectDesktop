#include "rosetube/rosetube_ExploreGenre.h"

#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"
#include "roseHome/ProcCommon_forRosehome.h"//c220818

#include <QScroller>


namespace rosetube {

    const int HTTP_GENRE = 99;
    const int HTTP_CACHE = 98;

    const int SECTION_FOR_MORE_POPUP___PLAYLIST = 100;


    RoseTubeExploreGenre::RoseTubeExploreGenre(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_filter, parent) {

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_logined()), SLOT(slot_getMyInfo_loginAfter()));
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));

        this->label_mainTitle = this->get_addUIControl_mainTitle("Genre Listview");
    }


    RoseTubeExploreGenre::~RoseTubeExploreGenre(){

        this->deleteLater();
    }


    void RoseTubeExploreGenre::setJsonObject_forData(const QJsonObject &jsonObj){

        int tmp_id = ProcJsonEasy::getInt(jsonObj, "menu_id");
        QString tmp_typeName = ProcJsonEasy::getString(jsonObj, "nm");
        QString tmp_typeSubName = ProcJsonEasy::getString(jsonObj, "type");
        this->flagNeedReload = false;

        if(this->typeName != tmp_typeName || this->typeSubName != tmp_typeSubName || tmp_id != this->typeId){
            this->flagNeedReload = true;

            this->typeName = tmp_typeName;
            this->typeSubName = tmp_typeSubName;
            this->typeId = tmp_id;

            this->label_mainTitle->setText(this->typeName);

            this->flag_track_idx = 0;

            this->rosetubeCategory = QJsonArray();

            this->genre_totalCount = 0;
            this->genre_drawCount = 0;
        }
    }


    void RoseTubeExploreGenre::setActivePage(){

        if(global.user.isValid() == true){

            if(this->flagNeedReload){

                // 항상 부모클래스의 함수 먼저 호출
                AbstractRoseHomeSubWidget::setActivePage();

                this->box_contents->removeWidget(this->widget_explore_contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->box_explore_contents = new QVBoxLayout();
                this->box_explore_contents->setSpacing(0);
                this->box_explore_contents->setContentsMargins(0, 0, 0, 0);
                this->box_explore_contents->setAlignment(Qt::AlignTop);

                this->widget_explore_contents = new QWidget();
                this->widget_explore_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_explore_contents->setLayout(this->box_explore_contents);

                this->widget_login_contents = setUIControl_LoginBefore();
                this->widget_login_contents->hide();

                this->stackedWidget_Contents = new QStackedWidget();
                this->stackedWidget_Contents->addWidget(this->widget_explore_contents);
                this->stackedWidget_Contents->addWidget(this->widget_login_contents);

                this->box_contents->addWidget(this->stackedWidget_Contents, 0, Qt::AlignTop);
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                // sub Title UI
                for(int i = 0; i < 50; i++){
                    this->lb_subTitle[i] = new QLabel();
                    this->btnView_all[i] = new QPushButton();
                }

                for(int i = 0; i < 50; i++){
                    this->vBox_Track[i] = new QVBoxLayout();                    
                    GSCommon::clearLayout(this->vBox_Track[i]);
                    this->vBox_Track[i]->setAlignment(Qt::AlignTop);

                    this->hBox_Track[i] = new QHBoxLayout();
                    GSCommon::clearLayout(this->hBox_Track[i]);
                }

                for(int i = 0; i < 50; i++){
                    this->jsonArr_rosetubeTrack[i] = QJsonArray();
                }

                if(!global.user.getAccess_token().isEmpty()){

                    this->setUIControl_requestGenre();
                }

                this->widget_explore_contents->show();
                this->widget_login_contents->hide();
                this->stackedWidget_Contents->setCurrentIndex(0);
            }
        }
        else{

            this->flagNeedReload = true;

            this->box_contents->removeWidget(this->widget_explore_contents);
            GSCommon::clearLayout(this->box_contents);

            this->box_explore_contents = new QVBoxLayout();
            this->box_explore_contents->setSpacing(0);
            this->box_explore_contents->setContentsMargins(0, 0, 0, 0);
            this->box_explore_contents->setAlignment(Qt::AlignTop);

            this->widget_explore_contents = new QWidget();
            this->widget_explore_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_explore_contents->setLayout(this->box_explore_contents);

            this->widget_login_contents = setUIControl_LoginBefore();
            this->widget_login_contents->hide();

            this->stackedWidget_Contents = new QStackedWidget();
            this->stackedWidget_Contents->addWidget(this->widget_explore_contents);
            this->stackedWidget_Contents->addWidget(this->widget_login_contents);

            this->box_contents->addWidget(this->stackedWidget_Contents);

            this->widget_explore_contents->hide();
            this->widget_login_contents->show();
            this->stackedWidget_Contents->setCurrentIndex(1);
        }
    }


    QWidget* RoseTubeExploreGenre::setUIControl_LoginBefore(){

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
        btn_login->setFixedSize(230, 55);
        btn_login->setGeometry(635, top + lb_text->sizeHint().height() + 30, 230, 55);


        // 커넥션
        connect(btn_login, &QPushButton::clicked, this, &RoseTubeExploreGenre::slot_showLoginPage);

        return widget_loginBefore;
    }


    void RoseTubeExploreGenre::proc_wheelEvent_to_getMoreData(){

        if((this->genre_totalCount > this->genre_drawCount) && (this->flag_draw == false)){

            this->flag_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->setUIControl_drawGenre();
        }
    }


    void RoseTubeExploreGenre::setUIControl_drawGenre(){

        int start_idx = this->genre_drawCount;
        int max_cnt = ((this->genre_totalCount - this->genre_drawCount) > 10 ) ? 10 : (this->genre_totalCount - this->genre_drawCount);
        this->genre_drawCount += max_cnt;

        for(int i = start_idx; i < this->genre_drawCount; i++){
            QJsonObject jsonObj = this->rosetubeCategory.at(i).toObject();

            if(jsonObj["listType"].toString() == "TRACK"){
                global.ROSETUBE_GENRE_TRACK[this->flag_track_idx] = ProcJsonEasy::getInt(jsonObj, "id");
                QJsonObject trackObj = ProcJsonEasy::getJsonObject(jsonObj, "playlist");

                QJsonArray trackArr = ProcJsonEasy::getJsonArray(trackObj, "tracks");

                int maxCount = 0;
                if(trackArr.size() > 10){
                    maxCount = 10;
                }
                else{
                    maxCount = trackArr.size();
                }

                this->widget_roseTube_Track[this->flag_track_idx] = new QWidget();
                QString subTitle = ProcJsonEasy::getString(jsonObj, "name");
                if(trackArr.size() > 0){
                    this->widget_roseTube_Track[this->flag_track_idx] = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(ProcJsonEasy::getInt(trackObj, "trackCount")), "View All", this->flag_track_idx, this->vBox_Track[this->flag_track_idx]);
                }
                else{
                    this->widget_roseTube_Track[this->flag_track_idx] = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", this->flag_track_idx, this->vBox_Track[this->flag_track_idx]);
                }

                this->vBox_Track[this->flag_track_idx]->addSpacing(10);

                if(maxCount == 0){
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                    noData_widget->setFixedSize(1500, 300);

                    this->vBox_Track[this->flag_track_idx]->addWidget(noData_widget);
                }
                else{
                    this->hBox_Track[this->flag_track_idx]->setSpacing(0);
                    this->hBox_Track[this->flag_track_idx]->setContentsMargins(0, 0, 0, 0);
                    this->hBox_Track[this->flag_track_idx]->setAlignment(Qt::AlignTop);
                    this->hBox_Track[this->flag_track_idx]->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_Track[this->flag_track_idx]);
                    widget_content->setContentsMargins(0, 0, 0, 0);

                    QScrollArea *playlist_scrollArea = new QScrollArea();
                    playlist_scrollArea->setWidget(widget_content);
                    playlist_scrollArea->setWidgetResizable(false);
                    playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                    playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                    playlist_scrollArea->setFixedHeight(300);

                    QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_Track[this->flag_track_idx]->addWidget(playlist_scrollArea, 0, Qt::AlignTop);

                    for(int j = 0; j < maxCount; j++){
                        this->home_rosetube_track[i][j] = new rosetube::ItemTrack_rosetube(j, i, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                        connect(this->home_rosetube_track[i][j], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTubeExploreGenre::slot_clickedItemPlaylist);
                    }

                    for(int j = 0; j < maxCount; j++){
                        this->home_rosetube_track[i][j]->setData(trackArr.at(j).toObject());

                        this->hBox_Track[this->flag_track_idx]->addWidget(this->home_rosetube_track[i][j]);
                    }
                }

                this->box_explore_contents->addLayout(this->vBox_Track[this->flag_track_idx]);
                this->box_explore_contents->addSpacing(30);

                this->flag_track_idx++;
            }
            else if(jsonObj["listType"].toString() == "PLAYLIST"){
                global.ROSETUBE_GENRE_TRACK[this->flag_track_idx] = ProcJsonEasy::getInt(jsonObj, "id");

                QJsonArray trackArr = ProcJsonEasy::getJsonArray(jsonObj, "playlists");
                this->jsonArr_rosetubeTrack[this->flag_track_idx] = trackArr;

                int maxCount = 0;
                if(trackArr.size() > 10){
                    maxCount = 10;
                }
                else{
                    maxCount = trackArr.size();
                }

                this->widget_roseTube_Track[this->flag_track_idx] = new QWidget();
                QString subTitle = ProcJsonEasy::getString(jsonObj, "name");
                if(trackArr.size() > 0){
                    this->widget_roseTube_Track[this->flag_track_idx] = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(trackArr.size()), "View All", this->flag_track_idx, this->vBox_Track[this->flag_track_idx]);
                }
                else{
                    this->widget_roseTube_Track[this->flag_track_idx] = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", this->flag_track_idx, this->vBox_Track[this->flag_track_idx]);
                }

                this->vBox_Track[this->flag_track_idx]->addSpacing(10);

                if(maxCount == 0){
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                    noData_widget->setFixedSize(1500, 335);

                    this->vBox_Track[this->flag_track_idx]->addWidget(noData_widget);
                }
                else{
                    this->hBox_Track[this->flag_track_idx]->setSpacing(0);
                    this->hBox_Track[this->flag_track_idx]->setContentsMargins(0, 0, 0, 0);
                    this->hBox_Track[this->flag_track_idx]->setAlignment(Qt::AlignTop);
                    this->hBox_Track[this->flag_track_idx]->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_Track[this->flag_track_idx]);
                    widget_content->setContentsMargins(0, 0, 0, 0);

                    QScrollArea *playlist_scrollArea = new QScrollArea();
                    playlist_scrollArea->setWidget(widget_content);
                    playlist_scrollArea->setWidgetResizable(false);
                    playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                    playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                    playlist_scrollArea->setFixedHeight(335);

                    QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_Track[this->flag_track_idx]->addWidget(playlist_scrollArea, 0, Qt::AlignTop);

                    for(int j = 0; j < maxCount; j++){
                        this->home_rosetube_playlist[i][j] = new rosetube::ItemPlaylist_rosetube(j, SECTION_FOR_MORE_POPUP___PLAYLIST + i, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                        connect(this->home_rosetube_playlist[i][j], &rosetube::ItemPlaylist_rosetube::signal_clicked, this, &RoseTubeExploreGenre::slot_clickedItemPlaylist);
                    }

                    for(int j = 0; j < maxCount; j++){
                        this->home_rosetube_playlist[i][j]->setData(trackArr.at(j).toObject());

                        this->hBox_Track[this->flag_track_idx]->addWidget(this->home_rosetube_playlist[i][j]);
                    }
                }

                this->box_explore_contents->addLayout(this->vBox_Track[this->flag_track_idx]);
                this->box_explore_contents->addSpacing(30);

                this->flag_track_idx++;
            }
        }

        this->slot_hide_msg();
        this->flag_draw = false;
    }

    void RoseTubeExploreGenre::setUIControl_requestGenre(){

        ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + QString("/member/youtube/keyword/%1?page=0&size=50").arg(this->typeId);
        QJsonObject json;

        network->request(HTTP_GENRE
                         , url
                         , json
                         , false
                         , true);
    }


    void RoseTubeExploreGenre::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << "p_jsonObj = " << strJson;*/

        if(p_id == HTTP_GENRE){

            const QString jsonKey_submenu = "roseTubeKeyword";
            const QString jsonKey_type = "roseTubeSubCategories";

            if(p_jsonObj.contains(jsonKey_submenu)){
                QJsonObject tmpObj = ProcJsonEasy::getJsonObject(p_jsonObj, jsonKey_submenu);
                QJsonArray tmpCategory = ProcJsonEasy::getJsonArray(tmpObj, jsonKey_type);

                this->genre_totalCount = 0;
                this->slot_applyResult_subGenre(tmpCategory);
            }
        }
        else if(p_id == HTTP_CACHE){
            if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
            }
        }

        sender()->deleteLater();
    }


    void RoseTubeExploreGenre::slot_applyResult_subGenre(const QJsonArray &jsonArr){

        if(jsonArr.size() > 0){

            if(this->genre_totalCount == 0){
                this->rosetubeCategory = QJsonArray();
                this->rosetubeCategory = jsonArr;

                this->flag_track_idx = 0;
                this->genre_totalCount = jsonArr.size();

                if(this->genre_totalCount > 10){
                    this->genre_drawCount = 10;
                }
                else{
                    this->genre_drawCount = this->genre_totalCount;
                }
            }

            for(int i = 0; i < this->genre_drawCount; i++){
                QJsonObject jsonObj = jsonArr.at(i).toObject();

                if(jsonObj["listType"].toString() == "TRACK"){
                    global.ROSETUBE_GENRE_TRACK[this->flag_track_idx] = ProcJsonEasy::getInt(jsonObj, "id");
                    QJsonObject trackObj = ProcJsonEasy::getJsonObject(jsonObj, "playlist");

                    QJsonArray trackArr = ProcJsonEasy::getJsonArray(trackObj, "tracks");
                    this->jsonArr_rosetubeTrack[this->flag_track_idx] = trackArr;

                    int maxCount = 0;
                    if(trackArr.size() > 10){
                        maxCount = 10;
                    }
                    else{
                        maxCount = trackArr.size();
                    }

                    this->widget_roseTube_Track[this->flag_track_idx] = new QWidget();
                    QString subTitle = ProcJsonEasy::getString(jsonObj, "name");
                    if(trackArr.size() > 0){
                        this->widget_roseTube_Track[this->flag_track_idx] = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(ProcJsonEasy::getInt(trackObj, "trackCount")), "View All", this->flag_track_idx, this->vBox_Track[this->flag_track_idx]);
                    }
                    else{
                        this->widget_roseTube_Track[this->flag_track_idx] = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", this->flag_track_idx, this->vBox_Track[this->flag_track_idx]);
                    }

                    this->vBox_Track[this->flag_track_idx]->addSpacing(10);

                    if(maxCount == 0){
                        NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                        noData_widget->setFixedSize(1500, 300);

                        this->vBox_Track[this->flag_track_idx]->addWidget(noData_widget);
                    }
                    else{
                        this->hBox_Track[this->flag_track_idx]->setSpacing(0);
                        this->hBox_Track[this->flag_track_idx]->setContentsMargins(0, 0, 0, 0);
                        this->hBox_Track[this->flag_track_idx]->setAlignment(Qt::AlignTop);
                        this->hBox_Track[this->flag_track_idx]->setSizeConstraint(QLayout::SetFixedSize);

                        QWidget *widget_content = new QWidget;
                        widget_content->setLayout(this->hBox_Track[this->flag_track_idx]);
                        widget_content->setContentsMargins(0, 0, 0, 0);

                        QScrollArea *playlist_scrollArea = new QScrollArea();
                        playlist_scrollArea->setWidget(widget_content);
                        playlist_scrollArea->setWidgetResizable(false);
                        playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                        playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                        playlist_scrollArea->setFixedHeight(300);

                        QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                        //----------------------------------------------------------------------------------------------------  BODY : END

                        // Apply Main Layout with spacing
                        this->vBox_Track[this->flag_track_idx]->addWidget(playlist_scrollArea, 0, Qt::AlignTop);

                        for(int j = 0; j < maxCount; j++){
                            this->home_rosetube_track[i][j] = new rosetube::ItemTrack_rosetube(j, i, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                            connect(this->home_rosetube_track[i][j], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTubeExploreGenre::slot_clickedItemPlaylist);
                        }

                        for(int j = 0; j < maxCount; j++){
                            this->home_rosetube_track[i][j]->setData(trackArr.at(j).toObject());

                            this->hBox_Track[this->flag_track_idx]->addWidget(this->home_rosetube_track[i][j]);
                        }
                    }

                    this->box_explore_contents->addLayout(this->vBox_Track[this->flag_track_idx]);
                    this->box_explore_contents->addSpacing(30);

                    this->flag_track_idx++;
                }
                else if(jsonObj["listType"].toString() == "PLAYLIST"){
                    global.ROSETUBE_GENRE_TRACK[this->flag_track_idx] = ProcJsonEasy::getInt(jsonObj, "id");

                    QJsonArray trackArr = ProcJsonEasy::getJsonArray(jsonObj, "playlists");
                    this->jsonArr_rosetubeTrack[this->flag_track_idx] = trackArr;

                    int maxCount = 0;
                    if(trackArr.size() > 10){
                        maxCount = 10;
                    }
                    else{
                        maxCount = trackArr.size();
                    }

                    this->widget_roseTube_Track[this->flag_track_idx] = new QWidget();
                    QString subTitle = ProcJsonEasy::getString(jsonObj, "name");
                    if(trackArr.size() > 0){
                        this->widget_roseTube_Track[this->flag_track_idx] = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (%1)").arg(trackArr.size()), "View All", this->flag_track_idx, this->vBox_Track[this->flag_track_idx]);
                    }
                    else{
                        this->widget_roseTube_Track[this->flag_track_idx] = this->setUIControl_subTitle_withSideBtn(subTitle + QString(" (0)"), "View All", this->flag_track_idx, this->vBox_Track[this->flag_track_idx]);
                    }

                    this->vBox_Track[this->flag_track_idx]->addSpacing(10);

                    if(maxCount == 0){
                        NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                        noData_widget->setFixedSize(1500, 335);

                        this->vBox_Track[this->flag_track_idx]->addWidget(noData_widget);
                    }
                    else{
                        this->hBox_Track[this->flag_track_idx]->setSpacing(0);
                        this->hBox_Track[this->flag_track_idx]->setContentsMargins(0, 0, 0, 0);
                        this->hBox_Track[this->flag_track_idx]->setAlignment(Qt::AlignTop);
                        this->hBox_Track[this->flag_track_idx]->setSizeConstraint(QLayout::SetFixedSize);

                        QWidget *widget_content = new QWidget;
                        widget_content->setLayout(this->hBox_Track[this->flag_track_idx]);
                        widget_content->setContentsMargins(0, 0, 0, 0);

                        QScrollArea *playlist_scrollArea = new QScrollArea();
                        playlist_scrollArea->setWidget(widget_content);
                        playlist_scrollArea->setWidgetResizable(false);
                        playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                        playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                        playlist_scrollArea->setFixedHeight(335);

                        QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                        //----------------------------------------------------------------------------------------------------  BODY : END

                        // Apply Main Layout with spacing
                        this->vBox_Track[this->flag_track_idx]->addWidget(playlist_scrollArea, 0, Qt::AlignTop);

                        for(int j = 0; j < maxCount; j++){
                            this->home_rosetube_playlist[i][j] = new rosetube::ItemPlaylist_rosetube(j, SECTION_FOR_MORE_POPUP___PLAYLIST + i, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                            connect(this->home_rosetube_playlist[i][j], &rosetube::ItemPlaylist_rosetube::signal_clicked, this, &RoseTubeExploreGenre::slot_clickedItemPlaylist);
                        }

                        for(int j = 0; j < maxCount; j++){
                            this->home_rosetube_playlist[i][j]->setData(trackArr.at(j).toObject());

                            this->hBox_Track[this->flag_track_idx]->addWidget(this->home_rosetube_playlist[i][j]);
                        }
                    }

                    this->box_explore_contents->addLayout(this->vBox_Track[this->flag_track_idx]);
                    this->box_explore_contents->addSpacing(30);

                    this->flag_track_idx++;
                }
            }

            this->flag_draw = false;
            this->slot_hide_msg();
        }
        else{
            this->slot_hide_msg();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
            noData_widget->setFixedSize(1500, 500);

            this->box_explore_contents->addWidget(noData_widget);
        }
    }


    void RoseTubeExploreGenre::slot_hide_msg(){

        ContentLoadingwaitingMsgHide();
    }


    void RoseTubeExploreGenre::slot_showLoginPage(){

        this->dlg_rose_login = new Dialog::DialogLogin(this);
        this->dlg_rose_login->exec();
    }


    void RoseTubeExploreGenre::slot_getMyInfo_loginAfter(){

        if(global.user.isValid() == true && this->stackedWidget_Contents->currentIndex() == 1){

            this->flagNeedReload = false;
            this->setActivePage();
        }
    }


    void RoseTubeExploreGenre::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_ROSE)){

            global.user.setDeviceChange(false);

            this->flagNeedReload = false;
            this->setActivePage();
        }
    }


    void RoseTubeExploreGenre::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        QJsonObject tmp_data;
        tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME);
        tmp_data.insert("title", sender()->property("title").toString());
        tmp_data.insert("subCategoryId", global.ROSETUBE_GENRE_TRACK[btnId]);
        tmp_data.insert("type", "track");

        // 부모에게 페이지 변경하라고 시그널 보냄
        emit this->signal_clickedViewAll(tmp_data);
    }


    QWidget* RoseTubeExploreGenre::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RoseTubeExploreGenre::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        if(section >= SECTION_FOR_MORE_POPUP___PLAYLIST){
            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Track 바로 재생 - clicked 항목만 재생
                int curr_clickMode = global.device.getMusicPlayType();

                OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                switch(curr_clickMode)
                {
                    case 13:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last;
                        break;
                    case 12:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty;
                        break;
                    case 15:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_RightNow;
                        break;
                    case 16:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext;
                        break;
                    default:
                        playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_RightNow;
                        break;
                }

                QJsonObject tmpObj = this->jsonArr_rosetubeTrack[section - SECTION_FOR_MORE_POPUP___PLAYLIST].at(index).toObject();
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(ProcJsonEasy::getInt(tmpObj, "id"), playType);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                QJsonObject tmpObj = this->jsonArr_rosetubeTrack[section - SECTION_FOR_MORE_POPUP___PLAYLIST].at(index).toObject();
                // Playlist Detail 페이지 진입
                QJsonObject tmp_data;
                tmp_data.insert("id", ProcJsonEasy::getInt(tmpObj, "id"));
                tmp_data.insert("ownerId", ProcJsonEasy::getInt(tmpObj, "ownerId"));
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYDETAIL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_MYDETAIL);
                // 부모에게 페이지 변경하라고 시그널 보냄
                emit this->signal_clickedViewAll(tmp_data);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                QJsonObject tmpObj = this->jsonArr_rosetubeTrack[section - SECTION_FOR_MORE_POPUP___PLAYLIST].at(index).toObject();

                QJsonObject data = ProcJsonEasy::getJsonObject(tmpObj, "data");
                QJsonObject snippet = ProcJsonEasy::getJsonObject(data, "snippet");

                OptMorePopup::HeaderData data_header;
                data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                data_header.sub_title = ProcJsonEasy::getString(tmpObj, "ownerName").toUpper();
                data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnail");
                data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(tmpObj, "id"));
                data_header.type = "YOUTUBE";      //j220906 share link
                data_header.flagProcStar = false;
                data_header.isShare = true;      //j220906 share link

                // OptMorePopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosetube_Playlist, data_header, index, section);
            }
        }
        else if(section < this->genre_totalCount){
            if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add){

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

                        QJsonObject ratingInfo;
                        ratingInfo.insert("favorite", this->flag_track_fav);
                        ratingInfo.insert("star", this->track_star_fav);
                        ratingInfo.insert("thumbup", false);
                        ratingInfo.insert("type", );

                        QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

                        QJsonObject json;
                        json.insert("ratingInfo", ratingInfo);
                        json.insert("track", track);

                        // request HTTP API - get favorite for Rose Server
                        roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                        connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseHome::slot_applyResult_getRating_track);
                        proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

                        this->flag_check_track = true;
                    }*/
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

                    roseHome::TrackItemData data_playlist;
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byTracks(data_playlist, this->jsonArr_rosetubeTrack[section], index, playType);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    QJsonObject tmpObj = this->jsonArr_rosetubeTrack[section].at(index).toObject();

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
    }

    void RoseTubeExploreGenre::slot_applyResult_getShareLink(const QString &link){//c220818 share link

        this->shareLink = link;
    }

    void RoseTubeExploreGenre::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section < this->genre_totalCount){
          /* if(clickMode == OptMorePopup::ClickMode::Share){////c220826_2
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }*/
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
                procRosePlay->requestPlayRose_byTracks(data_track, this->jsonArr_rosetubeTrack[section], index, clickMode);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubscribInfo){
                QJsonObject tmpObj = this->jsonArr_rosetubeTrack[section].at(index).toObject();
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
            else if(clickMode == OptMorePopup::ClickMode::Share){//c220818
                print_debug();//c220818-3dot
                setUIShare();//c220818
                print_debug();
                qDebug() << "this->shareLink="<<this->shareLink;//c220818


            }
            else if(clickMode == OptMorePopup::ClickMode::Caching_ahead){
                QJsonObject trackInfo = this->jsonArr_rosetubeTrack[section].at(index).toObject();

                QString channelId = "";
                QString channelName = "";
                QString duration = "";
                QString id = "";
                QString thumbnailUrl = "";
                QString title = "";

                if(trackInfo.contains("data")){
                    QJsonObject trackData = ProcJsonEasy::getJsonObject(trackInfo, "data");
                    QJsonObject detailContents = ProcJsonEasy::getJsonObject(trackData, "detailContents");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(trackData, "snippet");
                    QJsonObject trackStatistic = ProcJsonEasy::getJsonObject(trackData, "detailContents");

                    QString tmpDuration = ProcJsonEasy::getString(detailContents, "duration");

                    QString time = tmpDuration;
                    QString hour;
                    QString minute;
                    QString second;

                    if(tmpDuration.indexOf("PT") >= 0){
                        time = tmpDuration.right(tmpDuration.size() - 2);
                    }
                    if(time.indexOf("H") >= 0){
                        hour = time.section("H", 0, -2);
                        time = time.mid(time.indexOf("H") + 1);
                    }
                    if(time.indexOf("M") >= 0){
                        minute = time.section("M", 0, -2);
                        time = time.mid(time.indexOf("M") + 1);
                    }
                    if(time.indexOf("S") >= 0){
                        second = time.section("S", 0, -2);
                    }

                    if(hour.isEmpty()){
                        if(minute.isEmpty()){
                            duration = "0:" + second.rightJustified(2, '0');
                        }
                        else{
                            duration = minute + ":" + second.rightJustified(2, '0');
                        }
                    }
                    else{
                        duration = hour + ":" + minute.rightJustified(2, '0') + ":" + second.rightJustified(2, '0');
                    }

                    channelId = ProcJsonEasy::getString(snippet, "channelId");
                    channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                    id = ProcJsonEasy::getString(trackInfo, "clientKey");
                    thumbnailUrl = ProcJsonEasy::getString(trackInfo, "thumbnailUrl");
                    title = ProcJsonEasy::getString(trackInfo, "title");
                }
                else{
                    if(trackInfo.contains("channelTitle")){
                        channelName = ProcJsonEasy::getString(trackInfo, "channelTitle");
                    }
                    else if(trackInfo.contains("channelName")){
                        channelName = ProcJsonEasy::getString(trackInfo, "channelName");
                    }

                    int time = ProcJsonEasy::getInt(trackInfo, "duration");
                    if(time == 0){
                        QString tmpDur = ProcJsonEasy::getString(trackInfo, "duration");

                        if(tmpDur.isEmpty()){
                            duration = "";
                        }
                        else{
                            duration = tmpDur;
                        }
                    }
                    else if(time >= 3600){
                         duration = QDateTime::fromTime_t(time).toUTC().toString("hh:mm:ss");
                    }
                    else{
                        duration = QDateTime::fromTime_t(time).toUTC().toString("mm:ss");
                    }

                    channelId = ProcJsonEasy::getString(trackInfo, "channelId");
                    id = ProcJsonEasy::getString(trackInfo, "id");
                    thumbnailUrl = ProcJsonEasy::getString(trackInfo, "thumbnailUrl");
                    title = ProcJsonEasy::getString(trackInfo, "title");
                }

                QJsonObject data;
                data.insert("channelId", channelId);
                data.insert("channelName", channelName);
                data.insert("duration", duration);
                data.insert("id", id);
                data.insert("isFavorite", false);
                data.insert("isLiveStream", false);
                data.insert("thumbnailUrl", thumbnailUrl);
                data.insert("thumbsUpPercentage", 0);
                data.insert("title", title);
                data.insert("viewsCount", "-1");

                QJsonObject json;
                json.insert("data", data);
                json.insert("isDelete", false);

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_download_item";
                network->request(HTTP_CACHE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
        }
    }
}
