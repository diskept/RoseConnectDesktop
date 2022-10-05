#include "rosetube/rosetubemain.h"

#include "common/common_struct.h"

#include "widget/toastmsg.h"
#include <common/networkhttp.h>

/**
 * @brief RoseTubeMain::RoseTubeMain : 로즈튜브 메인
 * @param parent
 */
RoseTubeMain::RoseTubeMain(QWidget *parent) : AbstractMainContent(parent)
{

    this->setInit();
    this->setUIControl();
}


/**
 * @brief RoseTubeMain::setInit : 초기 세팅
 */
void RoseTubeMain::setInit(){

    currMainMenuCode = GSCommon::MainMenuCode::RoseTube;

    // linker connect
    connect(linker, SIGNAL(signal_changeRoseTubeBG(QPixmap)), this, SLOT(slot_changeRoseTubeBG(QPixmap)));
    connect(linker, SIGNAL(signal_search(QString)), SLOT(slot_search(QString)));
    connect(linker, SIGNAL(signal_menumoveRoseTube()), SLOT(slot_totalsearch()));//cheon211008

    connect(linker, SIGNAL(signal_dragEnterEvent_hide_show(bool)), SLOT(slot_dragEnterEvent_hide_show(bool)));//c220730
    connect(linker, SIGNAL(signal_dropEvent_hide_show(bool)), SLOT(slot_dropEvent_hide_show(bool)));//c220730

    make_CustomLineEdit();//c220730

}


/**
 * @brief RoseTubeMain::setUIControl : UI 세팅
 */
void RoseTubeMain::setUIControl(){

    // 메뉴 페이지
    this->roseHome = new rosetube::RoseTubeHome();

    // 스택에 추가

    this->stackedWidget_content->addWidget(this->roseHome);
     this->stackedWidget_content->addWidget(le_search_back);//c220730

    // 커넥트
    connect(this->roseHome, SIGNAL(signal_clickedViewAll(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));


    // default : 첫번째 서브메뉴
    QJsonObject tmp_data;
    tmp_data[KEY_PAGE_CODE] = PAGECODE_RT_HOME;
    this->goToMoveNewOrderPage(tmp_data);
}

void RoseTubeMain::slot_overrideSigalSearch(bool b){//c220728
    print_debug();
    if(b){
        emit linker->signal_clickedMovePageRoseTubeSearch();
    }else{
        slot_dragEnterEvent_hide_show(false);
        emit linker->signal_checkQueue(30, "");//c220729
    }

}

void RoseTubeMain::slot_dragEnterEvent_hide_show(bool show){//c220826_1
    print_debug();
    qDebug()<<"##########################################################################################################";
    if(show){
        if(global.width_mainwindow==0){
            this->le_search_back->setFixedSize(800,500);
        }else{
            this->le_search_back->setFixedSize(global.width_mainwindow-200,global.height_mainwindow);
        }
        this->curr_widget = this->stackedWidget_content->currentWidget();
        this->curr_widget->hide();
        this->le_search_back->show();
        QTimer::singleShot(3000, this, SLOT(slot_dragEnterEvent_restore()));

    }else{
        print_debug();
        this->curr_widget = this->stackedWidget_content->currentWidget();//c220824_4
        this->le_search_back->clear();
        emit linker->signal_searchBarFocusChanged(false);
        this->le_search_back->clearFocus();
        this->curr_widget->show();
        this->le_search_back->hide();
    }

}


void RoseTubeMain::slot_dragEnterEvent_restore(){//c220826_1
    print_debug();
    this->curr_widget = this->stackedWidget_content->currentWidget();//c220824_4
    this->le_search_back->clear();
    emit linker->signal_searchBarFocusChanged(false);
    this->le_search_back->clearFocus();
    this->curr_widget->show();
    this->le_search_back->hide();
}

void RoseTubeMain::slot_totalsearch(){

    if(global.search_text.size() >= 2){
        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = PAGECODE_RT_SEARCH;
        tmp_data[KEY_DATA] = global.search_text;
        this->goToMoveNewOrderPage(tmp_data);
    }
    else{
        ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
        //ToastMsg::show(this, "", tr("검색어는 2글자 이상 입력해주세요."));
    }
}


/**
 * @brief RoseTubeMain::slot_search : [슬롯] 검색 요청
 * @param p_text
 */
void RoseTubeMain::slot_search(const QString &search_word){

    if(search_word.size() >= 2){
        global.search_text = search_word;//searchtextcopy

        common::SearchData data_search;
        data_search.search_word = search_word;

        QJsonObject jsonObj_search;
        jsonObj_search[KEY_DATA] = common::ConvertData_common::getObjectJson_searchData(data_search);
        jsonObj_search[KEY_PAGE_CODE] = PAGECODE_RT_SEARCH;

        this->goToMoveNewOrderPage(jsonObj_search);
    }
    else{
        ToastMsg::show(this, "", tr("Please enter at least two letters for the search word."));
        //ToastMsg::show(this, "", tr("검색어는 2글자 이상 입력해주세요."));
    }
}

/**
 * @brief RoseTubeMain::slot_clickedViewAll [SLOT] 구독 채널 row 클릭시
 * @param p_viewAllMode
 */
void RoseTubeMain::slot_clickedViewAll(const QJsonObject &p_jsonData){

    // 페이지 히스토리 관리 관련된 Abs함수
    QJsonObject tmp_data;
    tmp_data[KEY_PAGE_CODE] = p_jsonData[KEY_VIEWALL_MODE].toString();
    tmp_data[KEY_DATA] = p_jsonData;
    this->goToMoveNewOrderPage(tmp_data);
}


/**
 * @brief RoseTubeMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
 * @param p_pageCode
 */
void RoseTubeMain::movePageOnly(QString p_pageCode, const QJsonObject &p_data){

    this->clearRoseTubeBG();

    QString pageCode = "";
    QJsonObject data;

    if(global.user.isValid() == false && (p_pageCode != PAGECODE_RT_HOME)){
        pageCode = PAGECODE_RT_HOME;
        data[KEY_PAGE_CODE] = PAGECODE_RT_HOME;
    }
    else{
        pageCode = p_pageCode;
        data = p_data;
    }

    const QString jsonKey_playList_info = "playList_info";
    QJsonObject p_jsonObject_titleMain;
    QJsonArray *p_jsonArray_titlSub = new QJsonArray();

    p_jsonObject_titleMain["name"] = tr("RoseTube");
    p_jsonObject_titleMain[KEY_MAIN_CODE] = GSCommon::MainMenuCode::RoseTube;
    QJsonObject sub1 { {"name", tr("Home")}, {"code", PAGECODE_RT_HOME} };
    QJsonObject sub2 { {"name", tr("Explore")}, {"code", PAGECODE_RT_EXPLORE} };
    QJsonObject sub3 { {"name", tr("Rose Subscription")}, {"code", PAGECODE_RT_SUB} };
    QJsonObject sub4 { {"name", tr("My RoseTube")}, {"code", PAGECODE_RT_COLLECTION} };
    //QJsonObject sub5 { {"name", tr("YOUTUBE")}, {"code", PAGECODE_RT_COLLECTION} };
    //QJsonObject sub6 { {"name", tr("Settings")}, {"code", PAGECODE_RT_COLLECTION} };

    p_jsonArray_titlSub->push_back(sub1);
    p_jsonArray_titlSub->push_back(sub2);
    p_jsonArray_titlSub->push_back(sub3);
    p_jsonArray_titlSub->push_back(sub4);
    //p_jsonArray_titlSub->push_back(sub5);
    //p_jsonArray_titlSub->push_back(sub6);

    this->topMenuBar->setVisible(true);

    // 부모 virtual 함수 호출
    AbstractMainContent::movePageOnly(pageCode, data);

    // 자식 override 함수 서술
    this->topMenuBar->setSelectedSubMenu(p_pageCode);
    this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, p_jsonArray_titlSub);
    if(p_pageCode==PAGECODE_RT_HOME){

        if(roseHome==nullptr){
            this->roseHome = new rosetube::RoseTubeHome();
            this->stackedWidget_content->addWidget(this->roseHome);
            connect(this->roseHome, SIGNAL(signal_clickedViewAll(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
        }

        this->topMenuBar->setSelectedSubMenu(p_pageCode);
        this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, p_jsonArray_titlSub);

        //roseHome->requestRecentRefresh();
        this->roseHome->setJsonObject_forData(p_data);
        this->roseHome->setActivePage();
        stackedWidget_content->setCurrentWidget(this->roseHome);

    }else if(p_pageCode==PAGECODE_RT_EXPLORE){

        if(this->roseExplore==nullptr){
            this->roseExplore = new rosetube::RoseTubeExplore();
            this->stackedWidget_content->addWidget(this->roseExplore);
            connect(this->roseExplore, SIGNAL(signal_clickedViewAll(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
        }

        this->roseExplore->setActivePage();
        stackedWidget_content->setCurrentWidget(this->roseExplore);

    }else if(p_pageCode==PAGECODE_RT_SUB){

        if(this->roseSubscription==nullptr){
            this->roseSubscription = new rosetube::RoseTube_Subscription();
            this->stackedWidget_content->addWidget(this->roseSubscription);
            connect(this->roseSubscription, SIGNAL(signal_clickedChannel(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
        }

        //this->roseSubscription->requestData();
        this->roseSubscription->setActivePage();
        stackedWidget_content->setCurrentWidget(this->roseSubscription);

    } else if(p_pageCode==PAGECODE_RT_COLLECTION){

        if(this->roseCollection==nullptr){
            this->roseCollection = new rosetube::RoseTube_Collection();
            this->stackedWidget_content->addWidget(this->roseCollection);
            connect(this->roseCollection, SIGNAL(signal_clickedViewAll(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
        }

        this->roseCollection->setJsonObject_forData(p_data);
        this->roseCollection->setActivePage();
        stackedWidget_content->setCurrentWidget(this->roseCollection);

    } else if(p_pageCode==PAGECODE_VA_COLLECTION_CACHE){

        if(this->roseCacheViewAll==nullptr){
            this->roseCacheViewAll = new rosetube::RoseTubeCacheViewAll();
            this->stackedWidget_content->addWidget(this->roseCacheViewAll);
            connect(this->roseCacheViewAll, SIGNAL(signal_clickedViewAll(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
        }

        p_jsonObject_titleMain["name"] = tr("Cache content");
        this->topMenuBar->setSelectedSubMenu(p_pageCode);
        this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());

        if(p_data.contains(KEY_DATA)){
            QJsonObject jsonData = p_data[KEY_DATA].toObject();
            this->roseCacheViewAll->setJsonObject_forData(jsonData);
        }
        this->roseCacheViewAll->setActivePage();
        stackedWidget_content->setCurrentWidget(this->roseCacheViewAll);

    } else if(p_pageCode==PAGECODE_VA_TRACK_SHARE_LIST){

        if(this->roseShareViewAll==nullptr){
            this->roseShareViewAll = new rosetube::RoseTubeTrackListAll_Share();
            this->stackedWidget_content->addWidget(this->roseShareViewAll);
            connect(this->roseShareViewAll, SIGNAL(signal_clickedViewAll(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
        }

        this->topMenuBar->setVisible(false);

        this->roseShareViewAll->setJsonObject_forData(ProcJsonEasy::getJsonObject(p_data, KEY_DATA));
        this->roseShareViewAll->setActivePage();
        stackedWidget_content->setCurrentWidget(this->roseShareViewAll);

    } else if(p_pageCode==PAGECODE_RT_SEARCH){

        if(this->roseSearch==nullptr){
            this->roseSearch = new rosetube::RoseTube_Search();
            this->stackedWidget_content->addWidget(this->roseSearch);
            connect(this->roseSearch, SIGNAL(signal_clickedViewAll(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
            //connect(roseSearch, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));//cheon211008
            //connect(roseSearch, SIGNAL(signal_changedMenuName(QString, QString)), this->topMenuBar, SLOT(slot_changedMenuNameText_search(QString, QString)));//cheon211008
        }

        //this->topMenuBar->setSelectedSubMenu(p_pageCode);
        //this->topMenuBar->setDataTopMenuBar(QJsonObject(),new QJsonArray());
        this->topMenuBar->setVisible(false);

        //common::SearchData tmp_data = common::ConvertData_common::convertData_searchData(p_data["data"].toObject());
        //this->roseSearch->search(tmp_data.search_word);
        this->roseSearch->setJsonObject_forData(ProcJsonEasy::getJsonObject(p_data, KEY_DATA));
        this->roseSearch->setActivePage();
        //roseSearch->search(p_data[KEY_DATA].toString());
        stackedWidget_content->setCurrentWidget(this->roseSearch);

    }else if(p_pageCode==PAGECODE_VA_EXPLORE_CATEGORY){

        if(viewAll_category==nullptr){
            this->viewAll_category = new CategorySelector();
            this->stackedWidget_content->addWidget(this->viewAll_category);
            connect(this->viewAll_category, SIGNAL(signal_clickedViewAll(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
        }

        if(p_data.contains(KEY_DATA)){
            QJsonObject jsonData = p_data[KEY_DATA].toObject();
            if(jsonData.contains("title")){
                p_jsonObject_titleMain["name"] = jsonData["title"].toString();
            }
            this->viewAll_category->setData(jsonData);
        }
        this->topMenuBar->setSelectedSubMenu(p_pageCode);
        this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain,new QJsonArray());
        stackedWidget_content->setCurrentWidget(viewAll_category);

    }else if(p_pageCode==PAGECODE_VA_HOME){

        /*if(viewAll_playList==nullptr){
            viewAll_playList = new RoseTube_ViewAll();
            this->stackedWidget_content->addWidget(viewAll_playList);
        }

        if(p_data.contains(KEY_DATA)){
            QJsonObject jsonData = p_data[KEY_DATA].toObject();
            if(jsonData.contains("title")){
                p_jsonObject_titleMain["name"] = jsonData["title"].toString();
            }
            viewAll_playList->setData(jsonData);
        }
        this->topMenuBar->setSelectedSubMenu(p_pageCode);
        this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain,new QJsonArray());
        stackedWidget_content->setCurrentWidget(viewAll_playList);*/

        if(this->roseViewAll==nullptr){
            this->roseViewAll = new rosetube::RosetubeViewAll();
            this->stackedWidget_content->addWidget(this->roseViewAll);
            connect(this->roseViewAll, SIGNAL(signal_clickedViewAll(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
        }

        this->topMenuBar->setVisible(false);

        if(p_data.contains(KEY_DATA)){
            QJsonObject jsonData = p_data[KEY_DATA].toObject();
            this->roseViewAll->setJsonObject_forData(jsonData);
        }
        this->roseViewAll->setActivePage();
        stackedWidget_content->setCurrentWidget(this->roseViewAll);

    }else if(p_pageCode==PAGECODE_VA_HOME_ROSE){

        if(this->roseRecentViewAll==nullptr){
            this->roseRecentViewAll = new rosetube::RoseTubeRecentlyPlaylistAll();
            this->stackedWidget_content->addWidget(this->roseRecentViewAll);
            connect(this->roseRecentViewAll, SIGNAL(signal_clickedViewAll(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
        }

        this->topMenuBar->setVisible(false);

        if(p_data.contains(KEY_DATA)){
            QJsonObject jsonData = p_data[KEY_DATA].toObject();
            this->roseRecentViewAll->setJsonObject_forData(jsonData);
        }
        this->roseRecentViewAll->setActivePage();
        stackedWidget_content->setCurrentWidget(this->roseRecentViewAll);

    }else if(p_pageCode==PAGECODE_VA_HOME_MYROSE){

        if(this->roseMyViewAll==nullptr){
            this->roseMyViewAll = new rosetube::RosetubeMyRosePlaylistAll();
            this->stackedWidget_content->addWidget(this->roseMyViewAll);
            connect(this->roseMyViewAll, SIGNAL(signal_clickedViewAll(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
        }

        this->topMenuBar->setVisible(false);
        //p_jsonObject_titleMain["name"] = tr("My Playlists");
        //this->topMenuBar->setSelectedSubMenu(p_pageCode);
        //this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());

        if(p_data.contains(KEY_DATA)){
            QJsonObject jsonData = p_data[KEY_DATA].toObject();
            this->roseMyViewAll->setJsonObject_forData(jsonData);
        }
        this->roseMyViewAll->setActivePage();
        stackedWidget_content->setCurrentWidget(this->roseMyViewAll);

    }else if(p_pageCode==PAGECODE_VA_HOME_USERROSE){

        if(this->roseUserViewAll==nullptr){
            this->roseUserViewAll = new rosetube::RosetubeUserRosePlaylistAll();
            this->stackedWidget_content->addWidget(this->roseUserViewAll);
            connect(this->roseUserViewAll, SIGNAL(signal_clickedViewAll(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
        }

        this->topMenuBar->setVisible(false);
        //p_jsonObject_titleMain["name"] = tr("RoseTube Playlist On Rose");
        //this->topMenuBar->setSelectedSubMenu(p_pageCode);
        //this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());

        if(p_data.contains(KEY_DATA)){
            QJsonObject jsonData = p_data[KEY_DATA].toObject();
            this->roseUserViewAll->setJsonObject_forData(jsonData);
        }
        this->roseUserViewAll->setActivePage();
        stackedWidget_content->setCurrentWidget(this->roseUserViewAll);

    }else if(p_pageCode==PAGECODE_VA_HOME_MYDETAIL){

        if(this->roseMyPlaylistDetail==nullptr){
            this->roseMyPlaylistDetail = new rosetube::RosetubeMyPlaylistDetail();
            this->stackedWidget_content->addWidget(this->roseMyPlaylistDetail);
        }

        this->topMenuBar->setVisible(false);

        if(p_data.contains(KEY_DATA)){
            QJsonObject jsonData = p_data[KEY_DATA].toObject();
            this->roseMyPlaylistDetail->setJsonObject_forData(jsonData);
        }
        this->roseMyPlaylistDetail->setActivePage();
        stackedWidget_content->setCurrentWidget(this->roseMyPlaylistDetail);

    }else if(p_pageCode==PAGECODE_VA_EXPLORE_PLAYLIST){

        if(viewAll_playList==nullptr){
            viewAll_playList = new RoseTube_ViewAll();
            this->stackedWidget_content->addWidget(viewAll_playList);
        }

        if(p_data.contains(KEY_DATA)){
            QJsonObject jsonData = p_data[KEY_DATA].toObject();
            if(jsonData.contains("title")){
                p_jsonObject_titleMain["name"] = jsonData["title"].toString();
            }
            viewAll_playList->setData(jsonData);
        }
        this->topMenuBar->setSelectedSubMenu(p_pageCode);
        this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain,new QJsonArray());
        stackedWidget_content->setCurrentWidget(viewAll_playList);

    }else if(p_pageCode==PAGECODE_VA_COLLECTION_FAV){

        if(this->roseFavorite==nullptr){
            this->roseFavorite = new rosetube::RoseTubeFavorite();
            this->stackedWidget_content->addWidget(this->roseFavorite);
            connect(this->roseFavorite, SIGNAL(signal_clickedViewAll(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
        }

        p_jsonObject_titleMain["name"] = tr("Favorites");
        this->topMenuBar->setSelectedSubMenu(p_pageCode);
        this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());

        if(p_data.contains(KEY_DATA)){
            QJsonObject jsonData = p_data[KEY_DATA].toObject();
            this->roseFavorite->setJsonObject_forData(jsonData);
        }
        this->roseFavorite->setActivePage();

        stackedWidget_content->setCurrentWidget(this->roseFavorite);

    }else if(p_pageCode==PAGECODE_VA_CATEGORY_DETAILS){

        /*if(viewAll_detail==nullptr){
            viewAll_detail = new RoseTube_ViewAllDetail;
            this->stackedWidget_content->addWidget(viewAll_detail);
            connect(viewAll_detail, SIGNAL(signal_clickedViewAll(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
        }
        if(p_data.contains(KEY_DATA)){

            QJsonObject jsonData = p_data[KEY_DATA].toObject();
            if(jsonData.contains("title")){
                p_jsonObject_titleMain["name"] = jsonData["title"].toString();
            }
            viewAll_detail->setData(jsonData);
        }
        this->topMenuBar->setSelectedSubMenu(p_pageCode);
        this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain,new QJsonArray());
        stackedWidget_content->setCurrentWidget(viewAll_detail);*/

        if(this->roseExploreGenre==nullptr){
            this->roseExploreGenre = new rosetube::RoseTubeExploreGenre;
            this->stackedWidget_content->addWidget(this->roseExploreGenre);
            connect(this->roseExploreGenre, SIGNAL(signal_clickedViewAll(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
        }

        //this->topMenuBar->setSelectedSubMenu(p_pageCode);
        //this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
        this->topMenuBar->setVisible(false);

        QJsonObject jsonData;
        if(p_data.contains(KEY_DATA)){

            jsonData = p_data[KEY_DATA].toObject();
            if(jsonData.contains("title")){
                p_jsonObject_titleMain["name"] = jsonData["title"].toString();
            }
        }
        this->roseExploreGenre->setJsonObject_forData(jsonData);
        this->roseExploreGenre->setActivePage();
        stackedWidget_content->setCurrentWidget(this->roseExploreGenre);

    }else if(p_pageCode==PAGECODE_VA_ROSE_PLAY_LIST){

        if(this->viewAll_playList==nullptr){
            this->viewAll_playList = new RoseTube_ViewAll();
            this->stackedWidget_content->addWidget(viewAll_playList);
        }

        if(p_data.contains(KEY_DATA)){
            QJsonObject jsonData = p_data[KEY_DATA].toObject();
            if(jsonData.contains("title")){
                p_jsonObject_titleMain["name"] = jsonData["title"].toString();
            }
            this->viewAll_playList->setData(jsonData);
        }
        this->topMenuBar->setSelectedSubMenu(p_pageCode);
        this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain,new QJsonArray());
        stackedWidget_content->setCurrentWidget(viewAll_playList);
    }
    else if(p_pageCode==PAGECODE_SUB_CHANNEL){

        /*if(roseChannel==nullptr){
            roseChannel = new RoseTube_Channel();
            this->stackedWidget_content->addWidget(roseChannel);
        }
        this->topMenuBar->setSelectedSubMenu(p_pageCode);
        this->topMenuBar->setDataTopMenuBar(QJsonObject(),new QJsonArray());
        this->topMenuBar->setVisible(false);
        stackedWidget_content->setCurrentWidget(roseChannel);
        if(p_data.contains(KEY_DATA)){
            QJsonObject jsonData = p_data[KEY_DATA].toObject();
            roseChannel->setData(jsonData);
        }*/

        if(this->roseChannel==nullptr){
            this->roseChannel = new rosetube::RoseTube_Channel();
            this->stackedWidget_content->addWidget(this->roseChannel);
        }

        this->topMenuBar->setVisible(false);

        QJsonObject jsonData;
        if(p_data.contains(KEY_DATA)){
            jsonData = p_data[KEY_DATA].toObject();
        }
        this->roseChannel->setJsonObject_forData(jsonData);
        this->roseChannel->setActivePage();
        stackedWidget_content->setCurrentWidget(this->roseChannel);

    }
    else if(p_pageCode==PAGECODE_OP_PLAYLISTINFO){

        if(this->rosePlayList==nullptr){
            this->rosePlayList = new rosetube::RoseTube_PlayList;
            this->stackedWidget_content->addWidget(this->rosePlayList);
            connect(this->rosePlayList, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
        }

        this->topMenuBar->setVisible(false);
        //this->topMenuBar->setSelectedSubMenu(p_pageCode);
        //this->topMenuBar->setDataTopMenuBar(QJsonObject(), new QJsonArray());

        QJsonObject jsonPlayListInfo;
        if(p_data.contains(jsonKey_playList_info)){
            jsonPlayListInfo = p_data[jsonKey_playList_info].toObject();
        }
        this->rosePlayList->setJsonObject_forData(jsonPlayListInfo);
        this->rosePlayList->setActivePage();
        stackedWidget_content->setCurrentWidget(rosePlayList);
    }
}

/**
 * @brief AbstractMainContent::slot_changeRoseTubeBG 로즈튜브배경을 세팅한다.
 * @param p_pixmap
 */
void RoseTubeMain::slot_changeRoseTubeBG(const QPixmap &p_pixmap){


    this->pixmap_roseTubeBG = new QPixmap(p_pixmap);
    if(stackedWidget_content->currentWidget()==roseChannel){

        // 배경 세팅
        this->flagSetRoseTubeBG = true;
        this->widget_roseTubeBG->setStyleSheet("#widget_roseTubeBG { background-color:transparent; }");
        if(!p_pixmap.isNull()){
            QPixmap tmp_pixmap = this->pixmap_roseTubeBG->scaled(this->widget_roseTubeBG->size().width(), 200, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            this->lb_roseTubeBG->setPixmap(tmp_pixmap);
        }
        this->widget_roseTubeBG->setFixedHeight(200);
        this->lb_roseTubeBG->resize(this->widget_roseTubeBG->size());
        this->lb_roseTubeBG->show();

    }
}

/**
 * @brief AbstractMainContent::clearRoseTubeBG 로즈튜브배경 클리어 (원상복구)
 */
void RoseTubeMain::clearRoseTubeBG(){

    // 배경 세팅
    this->flagSetRoseTubeBG = false;
    widget_roseTubeBG->setStyleSheet("#widget_roseTubeBG { background-color:#171717; }");
    widget_roseTubeBG->setFixedHeight(topNaviBar->height());
    this->lb_roseTubeBG->setPixmap(QPixmap());
    //this->lb_roseTubeBG->resize(topNaviBar->size());

    this->lb_roseTubeBG->hide();
}

void RoseTubeMain::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

    print_debug();
    Q_UNUSED(p_jsonObj);
    if(p_id == 3344){//c220727
        print_debug();
        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            print_debug();
            ToastMsg::show(this,"", tr("Play the selected YouTube content."), 2000, 0);

        }else{
            print_debug();
            ToastMsg::show(this,"", tr("Failed to perform playback of the selected YouTube content."), 2000, 0);

        }
        slot_overrideSigalSearch(false);

    }
    else if(p_id == 5678){//c220803
        print_debug();
        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            qDebug() << "test";
            print_debug();

            bool p_flagShuffle = false;
            QJsonArray jsonArrYoutube;

            QJsonArray tmp_jsonArrYoutube = p_jsonObj["track"].toArray();
            //QJsonObject tmp_json;
            QJsonObject jsonTrackData;
            jsonTrackData.insert("playurl",global.dragDrop_pathStr);
            qDebug()<<"global.dragDrop_pathStr="<<global.dragDrop_pathStr;
            //---//c220802
            QString videoID;
            QString tmp_Path4 = global.dragDrop_pathStr.split("&").at(0);
            if((global.dragDrop_pathStr.contains("&")&& global.dragDrop_pathStr.split("&").at(1).size()>0)){
                videoID = tmp_Path4.split("?v=").last();
            }else{
                videoID = jsonTrackData["playurl"].toString().split("?v=").last();
            }
            //---------------------
            if(tmp_jsonArrYoutube.count()>0){
                for(int i = 0 ; i < tmp_jsonArrYoutube.count(); i++){

                    jsonTrackData = tmp_jsonArrYoutube.at(i).toObject();
                    QString videoID = jsonTrackData["url"].toString().split("?v=").last();

                    QJsonObject jsonYoutube;
                    jsonYoutube.insert("channelId", p_jsonObj["channelId"].toString());
                    jsonYoutube.insert("channelName", p_jsonObj["uploaderName"].toString());
                    jsonYoutube.insert("duration", QString("%1").arg(p_jsonObj["duration"].toInt()));
                    jsonYoutube.insert("id", videoID);
                    jsonYoutube.insert("isFavorite", p_jsonObj["favorites"].toString()=="1" ? true : false);
                    jsonYoutube.insert("isLiveStream",p_jsonObj["streamType"].toString()=="LIVE_STREAM" ? true : false);
                    jsonYoutube.insert("thumbnailUrl", p_jsonObj["thumbnailUrl"].toString());
                    jsonYoutube.insert("thumbsUpPercentage",0);
                    jsonYoutube.insert("title", p_jsonObj["name"].toString());
                    jsonYoutube.insert("viewsCount", QString("%1").arg(p_jsonObj["viewCount"].toInt()));

                    // jsonArrYoutube.append(jsonYoutube);
                }
            }else{
                qDebug()<<"videoID="<<videoID;
                QJsonObject jsonYoutube;
                jsonYoutube.insert("channelId", p_jsonObj["channelId"].toString());
                jsonYoutube.insert("channelName", p_jsonObj["uploaderName"].toString());
                jsonYoutube.insert("duration", QString("%1").arg(p_jsonObj["duration"].toInt()));
                jsonYoutube.insert("id", videoID);
                jsonYoutube.insert("isFavorite", p_jsonObj["favorites"].toString()=="1" ? true : false);
                jsonYoutube.insert("isLiveStream",p_jsonObj["streamType"].toString()=="LIVE_STREAM" ? true : false);
                jsonYoutube.insert("thumbnailUrl", p_jsonObj["thumbnailUrl"].toString());
                jsonYoutube.insert("thumbsUpPercentage",0);
                jsonYoutube.insert("title", p_jsonObj["name"].toString());
                jsonYoutube.insert("viewsCount", QString("%1").arg(p_jsonObj["viewCount"].toInt()));

                jsonArrYoutube.append(jsonYoutube);
            }

            QJsonObject jsonParam;
            jsonParam.insert("roseToken", global.device.getDeviceRoseToken());
            jsonParam.insert("shuffle", p_flagShuffle ? 1 : 0);
            jsonParam.insert("youtubePlayType", 15);
            jsonParam.insert("youtube", jsonArrYoutube);

            if(global.Queue_track_count != 0) {
                print_debug();emit linker->signal_checkQueue(27, "");

                return;
            }
            print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
            global.Queue_track_count += jsonArrYoutube.count();     // 220419 queue count
            emit linker->signal_checkQueue(32, QString(tr(" Play %1 : %2")).arg(p_jsonObj["uploaderName"].toString()).arg(p_jsonObj["name"].toString().left(20)));//c220802


            NetworkHttp *network = new NetworkHttp(this);
            connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));

            network->request(3344, QString("http://%1:%2/youtubePlay.playlist.add")
                             .arg(global.device.getDeviceIP()).arg(global.port)
                             , jsonParam, true);
        }else{

            print_debug();
            qDebug() << "ProcJsonEasy::get_flagOk(p_jsonObj) = "<< ProcJsonEasy::get_flagOk(p_jsonObj);

            emit linker->signal_checkQueue(33, "");//c220803

        }


        slot_overrideSigalSearch(false);
    }

    sender()->deleteLater();
}
