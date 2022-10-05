#include "rosetube/rosetube_Explore.h"

#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"
#include "roseHome/ProcCommon_forRosehome.h"//c220818

#include <QScroller>


namespace rosetube {

    const int HTTP_CATEGORY = 99;
    const int HTTP_EXPLORE = 98;
    const int HTTP_CACHE = 97;


    RoseTubeExplore::RoseTubeExplore(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_filter, parent) {

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_logined()), SLOT(slot_getMyInfo_loginAfter()));
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));

        this->rosetubeCategory = QJsonArray();
    }


    RoseTubeExplore::~RoseTubeExplore(){

        this->deleteLater();
    }


    void RoseTubeExplore::setActivePage(){

        if(global.user.isValid() == true){

            if(this->flagNeedReload == false){
                this->flagNeedReload = true;

                // 항상 부모클래스의 함수 먼저 호출
                AbstractRoseHomeSubWidget::setActivePage();

                this->box_contents->removeWidget(this->widget_explore_contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->box_explore_contents = new QVBoxLayout();
                this->box_explore_contents->setSpacing(0);
                this->box_explore_contents->setContentsMargins(0, 0, 0, 0);

                this->widget_explore_contents = new QWidget();
                this->widget_explore_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_explore_contents->setLayout(this->box_explore_contents);

                this->widget_login_contents = setUIControl_LoginBefore();
                this->widget_login_contents->hide();

                this->stackedWidget_Contents = new QStackedWidget();
                this->stackedWidget_Contents->addWidget(this->widget_explore_contents);
                this->stackedWidget_Contents->addWidget(this->widget_login_contents);

                this->box_contents->addWidget(this->stackedWidget_Contents);
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                // sub Title UI
                for(int i = 0; i < 20; i++){
                    this->lb_subTitle[i] = new QLabel();
                    this->btnView_all[i] = new QPushButton();
                }

                for(int i = 0; i < 20; i++){
                    this->vBox_Track[i] = new QVBoxLayout();
                    GSCommon::clearLayout(this->vBox_Track[i]);

                    this->hBox_Track[i] = new QHBoxLayout();
                    GSCommon::clearLayout(this->hBox_Track[i]);
                }

                for(int i = 0; i < 20; i++){
                    for(int j = 0; j < 10; j++){
                        this->home_rosetube_track[i][j] = new rosetube::ItemTrack_rosetube(j, i, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200, true);
                        connect(this->home_rosetube_track[i][j], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTubeExplore::slot_clickedItemPlaylist);
                    }
                }

                this->vBox_Explore = new QVBoxLayout();
                GSCommon::clearLayout(this->vBox_Explore);

                this->flag_track_idx = 0;

                if(!global.user.getAccess_token().isEmpty()){

                    this->setUIControl_requestCategory();
                    this->setUIControl_requestExplore();
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
            this->box_contents->setAlignment(Qt::AlignTop);

            this->box_explore_contents = new QVBoxLayout();
            this->box_explore_contents->setSpacing(0);
            this->box_explore_contents->setContentsMargins(0, 0, 0, 0);

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


    QWidget* RoseTubeExplore::setUIControl_LoginBefore(){

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
        connect(btn_login, &QPushButton::clicked, this, &RoseTubeExplore::slot_showLoginPage);

        return widget_loginBefore;
    }


    void RoseTubeExplore::setUIControl_requestCategory(){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + "/member/youtube/keyword?";
        QJsonObject json;

        if(global.lang == 0){
            url += "languageType=en&page=0&size=10";
        }
        else if(global.lang == 1){
            url += "languageType=ko&page=0&size=10";
        }

        network->request(HTTP_CATEGORY
                         , url
                         , json
                         , false
                         , true);
    }


    void RoseTubeExplore::setUIControl_requestExplore(){

        ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + "/member/youtube/explorer?";
        QJsonObject json;

        if(global.lang == 0){
            url += "languageType=en&page=0&size=10";
        }
        else if(global.lang == 1){
            url += "languageType=ko&page=0&size=10";
        }

        network->request(HTTP_EXPLORE
                         , url
                         , json
                         , false
                         , true);
    }


    void RoseTubeExplore::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << "p_jsonObj = " << strJson;*/

        if(p_id == HTTP_CATEGORY){
            const QString jsonKey_submenu = "roseTubeKeywords";
            const QString jsonKey_type = "keywordType";
            const QString jsonKey_name = "name";
            const QString jsonKey_id = "keywordId";

            QJsonObject genreObj = QJsonObject();
            QJsonArray genreArr = QJsonArray();

            QJsonObject situationObj = QJsonObject();
            QJsonArray situationArr = QJsonArray();

            if(p_jsonObj.contains(jsonKey_submenu)){
                QJsonArray jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObj, jsonKey_submenu);

                for(int i = 0; i < jsonArrSub.size(); i++){
                    QJsonObject jsonCategory = jsonArrSub.at(i).toObject();

                    /// API 반환값에는 장르별/상황별 정보 외에 플레이리스트 정보도 포함되어 있다.
                    if(jsonCategory.contains(jsonKey_type)){
                        QString type = jsonCategory[jsonKey_type].toString().toLower();

                        if(type == "genre"){
                            if(!genreObj.contains("nm")){
                                genreObj.insert("nm", jsonCategory[jsonKey_type].toString());
                            }

                            QString tmpName = ProcJsonEasy::getString(jsonCategory, jsonKey_name);
                            int tmpId = ProcJsonEasy::getInt(jsonCategory, jsonKey_id);

                            QJsonObject genreInfo = QJsonObject();
                            genreInfo.insert("nm", tmpName);
                            genreInfo.insert("id", tmpId);
                            genreArr.append(genreInfo);
                        }
                        else if(type == "situation"){
                            if(!situationObj.contains("nm")){
                                situationObj.insert("nm", jsonCategory[jsonKey_type].toString());
                            }

                            QString tmpName = ProcJsonEasy::getString(jsonCategory, jsonKey_name);
                            int tmpId = ProcJsonEasy::getInt(jsonCategory, jsonKey_id);

                            QJsonObject situationInfo = QJsonObject();
                            situationInfo.insert("nm", tmpName);
                            situationInfo.insert("id", tmpId);
                            situationArr.append(situationInfo);
                        }
                    }
                }

                //0 : 장르별, 1 : 상황별
                if(genreArr.size() > 0){
                    genreObj.insert("submenu", genreArr);
                    appendCategory(0, genreObj);
                }
                if(situationArr.size() > 0){
                    situationObj.insert("submenu", situationArr);
                    appendCategory(1, situationObj);
                }
            }
        }
        else if(p_id == HTTP_EXPLORE){

            const QString jsonKey_submenu = "roseTubeCategory";
            const QString jsonKey_type = "roseTubeSubCategories";

            if(p_jsonObj.contains(jsonKey_submenu)){
                QJsonObject tmpObj = ProcJsonEasy::getJsonObject(p_jsonObj, jsonKey_submenu);
                QJsonArray tmpCategory = ProcJsonEasy::getJsonArray(tmpObj, jsonKey_type);

                this->slot_applyResult_subCategories(tmpCategory);
            }
        }
        if(p_id == HTTP_CACHE){
            if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
            }
        }

        sender()->deleteLater();
    }


    void RoseTubeExplore::appendCategorySelectorUI(const int &p_index, const QString &p_title, const QJsonArray &p_jsonArr){

        Q_UNUSED(p_index);

        const QString jsonKey_id = "id";
        const QString jsonKey_nm = "nm";

        QLabel *labelSubTitle = new QLabel;
        labelSubTitle->setText(p_title);
        labelSubTitle->setStyleSheet("font-size:22px;color:#FFFFFF;");

        QHBoxLayout *boxTop = new QHBoxLayout;
        boxTop->setSpacing(0);
        boxTop->setContentsMargins(0,0,28,0);
        boxTop->addWidget(labelSubTitle, 0, Qt::AlignVCenter);


        QWidget *widget_top = new QWidget;
        widget_top->setContentsMargins(0,0,0,0);
        widget_top->setLayout(boxTop);

        FlowLayout *flowLayout = new FlowLayout(0, 20, 20);
        flowLayout->setSizeConstraint(QLayout::SetMinimumSize);

        for(int i = 0 ; i < p_jsonArr.size(); i++){
            QJsonObject jsonData = p_jsonArr.at(i).toObject();

            int tmp_id = 0;
            QString tmp_nm = "";
            if(jsonData.contains(jsonKey_id)){
                tmp_id = jsonData[jsonKey_id].toInt();
            }
            if(jsonData.contains(jsonKey_nm)){
                tmp_nm = jsonData[jsonKey_nm].toString();

                if(tmp_nm.contains("&")){
                    tmp_nm.replace("&", "&&");
                }
            }


            QString btn_style = "QPushButton{ ";
            btn_style += "padding-left:37px; padding-right:37px; padding-top:10px; padding-bottom:10px;";
            btn_style += "background:transparent;color:#FFFFFF;font-size:18px;";
            btn_style += "border:2px solid #494949; border-radius:20px;}";
            btn_style += "QPushButton:hover{background-color:#BE905F;}";


            QPushButton *btn = new QPushButton;
            btn->setText(tmp_nm);
            btn->setStyleSheet(btn_style);
            btn->setProperty("menu_id", tmp_id);
            btn->setProperty("nm", tmp_nm);
            btn->setProperty("title", tmp_nm);
            btn->setProperty("type", p_title);
            btn->setCursor(Qt::PointingHandCursor);

            connect(btn, SIGNAL(clicked()), this, SLOT(slot_clickedCategory()));

            flowLayout->addWidget(btn);
        }

        QWidget *wg_flow = new QWidget;
        wg_flow->setLayout(flowLayout);
        wg_flow->setContentsMargins(0,10,10,0);

        QVBoxLayout *box_category = new QVBoxLayout;
        box_category->setSpacing(0);
        box_category->setContentsMargins(0,10,0,0);
        box_category->addWidget(widget_top);
        box_category->addWidget(wg_flow);

        this->vBox_Explore->addLayout(box_category);
        this->vBox_Explore->addSpacing(30);
    }


    void RoseTubeExplore::appendCategory(const int &p_index, const QJsonObject &p_jsonObject){

        const QString jsonKey_submenu = "submenu";
        const QString jsonKey_nm = "nm";

        QString tmp_category;

        if(p_jsonObject.contains(jsonKey_nm)){
            tmp_category = p_jsonObject[jsonKey_nm].toString();
        }
        if(p_jsonObject.contains(jsonKey_submenu)){
            appendCategorySelectorUI(p_index, tmp_category, p_jsonObject[jsonKey_submenu].toArray());
        }
    }


    void RoseTubeExplore::slot_applyResult_subCategories(const QJsonArray &jsonArr){

        if(jsonArr.size() > 0){
            for(int i = 0; i < jsonArr.size(); i++){
                QJsonObject jsonObj = jsonArr.at(i).toObject();

                if(jsonObj["listType"].toString() == "TRACK"){
                    global.ROSETUBE_EXPLORE_TRACK[this->flag_track_idx] = ProcJsonEasy::getInt(jsonObj, "id");
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
                        this->vBox_Track[this->flag_track_idx]->addWidget(playlist_scrollArea);

                        for(int j = 0; j < maxCount; j++){
                            this->home_rosetube_track[this->flag_track_idx][j]->setData(trackArr.at(j).toObject());

                            this->hBox_Track[this->flag_track_idx]->addWidget(this->home_rosetube_track[this->flag_track_idx][j]);
                        }
                    }

                    if(this->flag_track_idx == 0){
                        this->box_explore_contents->addLayout(this->vBox_Explore);
                        this->box_explore_contents->addSpacing(20);
                    }

                    this->box_explore_contents->addLayout(this->vBox_Track[this->flag_track_idx]);
                    this->box_explore_contents->addSpacing(30);

                    this->flag_track_idx++;
                }
            }

            this->slot_hide_msg();
        }
    }


    void RoseTubeExplore::slot_hide_msg(){

        ContentLoadingwaitingMsgHide();
    }


    void RoseTubeExplore::slot_showLoginPage(){

        this->dlg_rose_login = new Dialog::DialogLogin(this);
        dlg_rose_login->exec();
    }


    void RoseTubeExplore::slot_getMyInfo_loginAfter(){

        if(global.user.isValid() == true && this->stackedWidget_Contents->currentIndex() == 1){

            this->rosetubeCategory = QJsonArray();

            this->flagNeedReload = false;
            this->setActivePage();
        }
    }


    void RoseTubeExplore::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_ROSE)){

            global.user.setDeviceChange(false);

            this->rosetubeCategory = QJsonArray();

            this->flagNeedReload = false;
            this->setActivePage();
        }
    }


    void RoseTubeExplore::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        QJsonObject tmp_data;
        tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME);
        tmp_data.insert("title", sender()->property("title").toString());
        tmp_data.insert("subCategoryId", global.ROSETUBE_EXPLORE_TRACK[btnId]);
        tmp_data.insert("type", "track");

        // 부모에게 페이지 변경하라고 시그널 보냄
        emit this->signal_clickedViewAll(tmp_data);
    }


    void RoseTubeExplore::slot_clickedCategory(){

        QJsonObject tmp_data;
        tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_CATEGORY_DETAILS);
        tmp_data.insert("menu_id", sender()->property("menu_id").toInt());
        tmp_data.insert("nm", sender()->property("nm").toString().replace("&&", "&"));
        tmp_data.insert("title", sender()->property("title").toString());
        tmp_data.insert("type", sender()->property("type").toString());

        // 부모에게 페이지 변경하라고 시그널 보냄
        emit this->signal_clickedViewAll(tmp_data);
    }


    QWidget* RoseTubeExplore::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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

    void RoseTubeExplore::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
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
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){//c220826_2  이하
                QJsonObject tmpObj = this->jsonArr_rosetubeTrack[section].at(index).toObject();

                print_debug();
                qDebug() << "tmpObj=" << tmpObj;

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

    void RoseTubeExplore::slot_applyResult_getShareLink(const QString &link){//c220818 share link

        QString tmpStr = link;
        QStringList tmpLink = tmpStr.split("?");

        this->shareLink = link;//tmpLink.at(0);
    }

    void RoseTubeExplore::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        /*if(clickMode == OptMorePopup::ClickMode::Share){////c220826_2
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
