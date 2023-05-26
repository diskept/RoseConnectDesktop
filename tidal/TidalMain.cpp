#include "tidal/TidalMain.h"

#include "tidal/ProcCommon.h"
#include "tidal/ProcRosePlay_withTidal.h"

#include "common/common_struct.h"
#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "widget/toastmsg.h"

#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QMetaObject>
#include <QResizeEvent>



namespace tidal {


    /**
     * @brief 타이달 메인 생성자
     * @param parent
     */
    TidalMain::TidalMain(QWidget *parent) : AbstractMainContent(parent){

        //connect(linker, SIGNAL(signal_search(QString)), SLOT(process_searchWord(QString)));
        connect(linker, SIGNAL(signal_search(QString)), SLOT(slot_search(QString)));
        connect(linker, SIGNAL(signal_dragEnterEvent_hide_show(bool)), SLOT(slot_dragEnterEvent_hide_show(bool)));//c220730
        connect(linker, SIGNAL(signal_dropEvent_hide_show(bool)), SLOT(slot_dropEvent_hide_show(bool)));//c220730
        connect(linker, SIGNAL(signal_loginTidalAcount()), this, SLOT(slot_loginAcount()));//c230426

        make_CustomLineEdit();//c220730

        this->setUIControl();
    }

    void TidalMain::slot_loginAcount(){//c230426
        print_debug();
        QString pageCode_firstPage;
        ToastMsg::show(this, "", tr("Your Tidal account status has been changed by another device."));
        global.user_forTidal.set_logoutState();

        if(global.enable_section_left == true){
            global.enable_section_left = false;
        }

        pageCode_firstPage = PAGECODE_T_SETTINGS;


        QJsonObject jsonObj_first;
        jsonObj_first[KEY_PAGE_CODE] = pageCode_firstPage;

        if(!pageCode_firstPage.isEmpty()){
            this->goToMoveNewOrderPage(jsonObj_first);
        }

    }
    void TidalMain::slot_overrideSigalSearch(bool b){//c220728
        print_debug();
        if(b){
            emit linker->signal_clickedMovePageRoseTubeSearch();
        }else{
            slot_dragEnterEvent_hide_show(false);
            emit linker->signal_checkQueue(30, "");//c220729
        }

    }

    void TidalMain::slot_dragEnterEvent_hide_show(bool show){//c220826_1
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


    void TidalMain::slot_dragEnterEvent_restore(){//c220826_1
        print_debug();
        this->curr_widget = this->stackedWidget_content->currentWidget();//c220824_4
        this->le_search_back->clear();
        emit linker->signal_searchBarFocusChanged(false);
        this->le_search_back->clearFocus();
        this->curr_widget->show();
        this->le_search_back->hide();
    }


    /**
     * @brief 검색어에 대하여 Tidal 검색 메인 페이지를 보이도록 한다.
     */
    void TidalMain::process_searchWord(const QString &search_word){

        if(search_word.size() >= 2){
            global.search_text = search_word;//searchtextcopy

            common::SearchData data_search;
            data_search.search_word = search_word;

            QJsonObject jsonObj_search;
            jsonObj_search[KEY_DATA] = common::ConvertData_common::getObjectJson_searchData(data_search);
            jsonObj_search[KEY_PAGE_CODE] = PAGECODE_T_SEARCH_MAIN;

            this->goToMoveNewOrderPage(jsonObj_search);
        }
        else{
            ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
        }
    }


    void TidalMain::slot_search(const QString &search_word){

        if(search_word.size() >= 2){
            global.search_text = search_word;//searchtextcopy

            common::SearchData data_search;
            data_search.search_word = search_word;

            QJsonObject jsonObj_search;
            jsonObj_search[KEY_DATA] = common::ConvertData_common::getObjectJson_searchData(data_search);
            jsonObj_search[KEY_PAGE_CODE] = PAGECODE_T_SEARCH_MAIN;

            this->goToMoveNewOrderPage(jsonObj_search);
        }
        else{
            ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
        }
    }


    /**
     * @brief TidalMain::setUIControl : UI 세팅
     */
    void TidalMain::setUIControl(){

        // default : 첫번째 서브메뉴 - (TIDAL > Home)
        QString pageCode_firstPage;
        if(global.user_forTidal.isLogined() == true)
        {
            pageCode_firstPage = PAGECODE_T_HOME;
        }
        else {
            if(global.enable_section_left == true){
                global.enable_section_left = false;
            }

            pageCode_firstPage = PAGECODE_T_SETTINGS;
        }

        QJsonObject jsonObj_first;
        jsonObj_first[KEY_PAGE_CODE] = pageCode_firstPage;

        if(!pageCode_firstPage.isEmpty()){
            this->goToMoveNewOrderPage(jsonObj_first);
        }
    }


    /**
     * @brief TIDAL 각 하위 페이지 클래스 마다, 발생하는 signal_clickedMovePage 시그널을 공통으로 처리한다.
     * @param p_jsonData : 요청한 페이지에 대한 정보가 담겨있다. JsonObject 형식. @n
     *    @li KEY_PAGE_CODE : 해당 페이지 클래스에 대응되는 Unique한 값이다. @see gscommon.h 파일에 정의.
     *    @li KEY_DATA : 옵션 JsonObject. 해당 페이지 클래스에 그대로 넘겨서 보내줄 데이터이다. {} 일 수 있음.
     */
    void TidalMain::slot_clickedMoveSubPage(const QJsonObject &p_jsonData){

        // 페이지 히스토리 관리 관련된 Abstract 함수 호출
        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = p_jsonData[KEY_PAGE_CODE].toString();
        tmp_data[KEY_DATA] = p_jsonData;
        this->goToMoveNewOrderPage(tmp_data);
    }


    /**
     * @brief TopMenuBar 객체에게 전달하기 위한 Main 타이틀 정보를 JsonObject 로 생성하여 반환
     * @return QJsonObject
     */
    QJsonObject TidalMain::getJsonObject_forMainTitle(){

        QJsonObject jsonObj;
        jsonObj["name"] = tr("Tidal");
        //jsonObj["name"] = "타이달";
        jsonObj[KEY_MAIN_CODE] = GSCommon::MainMenuCode::Tidal;
        return jsonObj;
    }


    /**
     * @brief TopMenuBar 객체에게 전달하기 위한 Sub 타이틀 정보 여러개를 JsonArray 로 생성하여 반환
     * @return QJsonArray* :
     */
    QJsonArray TidalMain::getJsonArray_forSubTitles(){

        QJsonArray p_jsonArray_titlSub ;
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Home")}, {"code", PAGECODE_T_HOME} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Explore")}, {"code", PAGECODE_T_EXPLORE} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Video")}, {"code", PAGECODE_T_VIDEO} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Master")}, {"code", PAGECODE_T_MASTER} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("My TIDAL")}, {"code", PAGECODE_T_MY_COLLECTION} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Setting")}, {"code", PAGECODE_T_SETTINGS} });

        return p_jsonArray_titlSub;
    }


    /**
     * @brief TidalMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
     * @param p_pageCode
     */
    //void TidalMain::movePageOnly(QString p_pageCode, const QJsonObject &p_data){
    void TidalMain::movePageOnly(QString t_pageCode, const QJsonObject &t_data){

        QString p_pageCode;
        QJsonObject p_data;
        if(global.user_forTidal.isLogined())
        {
            p_pageCode = t_pageCode;
            p_data = t_data;

            global.user_forTidal.setMain(true);
        }
        else
        {
            p_pageCode = PAGECODE_T_SETTINGS;
            p_data[KEY_PAGE_CODE] = p_pageCode;
        }

        QJsonObject tmp_jsonObj_titleMain = this->getJsonObject_forMainTitle();
        QJsonArray tmp_jsonArr_titlSub = this->getJsonArray_forSubTitles();

        // 부모 virtual 함수 호출
        AbstractMainContent::movePageOnly(p_pageCode, p_data);
        //qDebug() << "tidal-------------------------------------------------------------------------";
        //QJsonDocument doc(p_data);//cheon4_code
        //QString strJson(doc.toJson(QJsonDocument::Compact));//cheon4_code
        //qDebug() << strJson;//cheon4_code

        if(p_pageCode == "playfullscreen"){
            tmp_jsonObj_titleMain = QJsonObject();
            tmp_jsonArr_titlSub = QJsonArray();

            this->topMenuBar->setDataTopMenuBar(tmp_jsonObj_titleMain, &tmp_jsonArr_titlSub);
        }
        else{
            // 자식 override 함수 서술
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(tmp_jsonObj_titleMain, &tmp_jsonArr_titlSub);
        }
        this->topMenuBar->setVisible(true);


        // 서브 메뉴페이지에 대한 처리
        if(p_pageCode==PAGECODE_T_HOME){
            this->sub_home = this->procCommon_showSubPage<tidal::TidalHome>(true, this->sub_home, p_data);
        }else if(p_pageCode==PAGECODE_T_EXPLORE){
            this->sub_explore = this->procCommon_showSubPage<tidal::TidalExplore>(true, this->sub_explore, p_data);
        }else if(p_pageCode == PAGECODE_T_EXPLORE_GENRES_CHOOSE_ALL){
            this->sub_genresChoose = this->procCommon_showSubPage<tidal::TidalGenresChoose>(false, this->sub_genresChoose, p_data);
        }else if(p_pageCode == PAGECODE_T_EXPLORE_GENRES_MAIN){
            this->sub_genresMain = this->procCommon_showSubPage<tidal::TidalGenresMain>(false, this->sub_genresMain, p_data);
        }else if(p_pageCode == PAGECODE_T_EXPLORE_MOODS_MAIN){
            this->sub_moodsMain = this->procCommon_showSubPage<tidal::TidalMoodsMain>(false, this->sub_moodsMain, p_data);
        }else if(p_pageCode == PAGECODE_T_EXPLORE_NEW_MAIN){
            this->sub_newMain = this->procCommon_showSubPage<tidal::TidalNewMain>(false, this->sub_newMain, p_data);
        }else if(p_pageCode == PAGECODE_T_EXPLORE_TOP_MAIN){
            this->sub_topMain = this->procCommon_showSubPage<tidal::TidalTop20Main>(false, this->sub_topMain, p_data);
        }else if(p_pageCode == PAGECODE_T_EXPLORE_RISING_MAIN){
            this->sub_risingMain = this->procCommon_showSubPage<tidal::TidalRisingMain>(false, this->sub_risingMain, p_data);
        }else if(p_pageCode==PAGECODE_T_VIDEO){
            this->sub_video = this->procCommon_showSubPage<tidal::TidalVideo>(true, this->sub_video, p_data);
        }else if(p_pageCode==PAGECODE_T_MASTER){
            this->sub_master = this->procCommon_showSubPage<tidal::TidalMaster>(true, this->sub_master, p_data);
        }else if(p_pageCode==PAGECODE_T_SETTINGS){
            this->sub_settings = this->procCommon_showSubPage<tidal::TidalSettings>(true, this->sub_settings, p_data);

        }else if(p_pageCode==PAGECODE_T_PLAYLIST_DETAIL){
            this->sub_playlistDetail = this->procCommon_showSubPage<tidal::TidalPlaylistDetail>(false, this->sub_playlistDetail, p_data);
        }else if(p_pageCode==PAGECODE_T_MIXES_DETAIL){
            this->sub_mixesDetail = this->procCommon_showSubPage<tidal::TidalMixesDetail>(false, this->sub_mixesDetail, p_data);
        }else if(p_pageCode==PAGECODE_T_ALBUM_DETAIL){
            this->sub_albumDetail = this->procCommon_showSubPage<tidal::TidalAlbumDetail>(false, this->sub_albumDetail, p_data);
        }else if(p_pageCode==PAGECODE_T_TRACK_ALL_LIST_VIEW){
            this->sub_tracksListAll = this->procCommon_showSubPage<tidal::TidalTracksListAll>(false, this->sub_tracksListAll, p_data);
        }else if(p_pageCode==PAGECODE_T_TRACK_SHARE_ALL_LIST_VIEW){
            this->sub_trackListAll_Share = this->procCommon_showSubPage<tidal::TidalTrackListAll_Share>(false, this->sub_trackListAll_Share, p_data);
        }else if(p_pageCode==PAGECODE_T_PLAYLIST_ALL_LIST_VIEW){
            this->sub_playlistAll = this->procCommon_showSubPage<tidal::TidalPlaylistAll>(false, this->sub_playlistAll, p_data);
        }else if(p_pageCode==PAGECODE_T_ALBUM_ALL_LIST_VIEW){
            this->sub_albumAll = this->procCommon_showSubPage<tidal::TidalAlbumListAll>(false, this->sub_albumAll, p_data);
        }else if(p_pageCode==PAGECODE_T_ARTIST_ALL_LIST_VIEW){
            this->sub_artistAll = this->procCommon_showSubPage<tidal::TidalArtistListAll>(false, this->sub_artistAll, p_data);
        }else if(p_pageCode==PAGECODE_T_ARTIST_DETAIL){
            this->sub_artistDetail = this->procCommon_showSubPage<tidal::TidalArtistDetail>(false, this->sub_artistDetail, p_data);
        }else if(p_pageCode==PAGECODE_T_VIDEO_ALL_LIST_VIEW){
            this->sub_videoAll = this->procCommon_showSubPage<tidal::TidalVideoListAll>(false, this->sub_videoAll, p_data);
        }else if(p_pageCode==PAGECODE_T_ADD_PLAYLIST){
            this->sub_addPlaylist = this->procCommon_showSubPage<tidal::TidalAddPlaylist>(false, this->sub_addPlaylist, p_data);
        }else if(p_pageCode==PAGECODE_T_HISTORY_LIST_VIEW){
            this->sub_historyListAll = this->procCommon_showSubPage<tidal::TidalHistoryListAll>(false, this->sub_historyListAll, p_data);
        }else if(p_pageCode==PAGECODE_T_HISTORY_DETAIL){
            this->sub_historyDetail = this->procCommon_showSubPage<tidal::TidalHistoryDetail>(false, this->sub_historyDetail, p_data);
        }else if(p_pageCode==PAGECODE_T_HISTORY_PLAYLIST_VIEW){
            this->sub_historyPlaylistAll = this->procCommon_showSubPage<tidal::TidalPlaylistHistoryAll>(false, this->sub_historyPlaylistAll, p_data);
        }else if(p_pageCode==PAGECODE_T_HISTORY_ALBUM_VIEW){
            this->sub_historyAlbumAll = this->procCommon_showSubPage<tidal::TidalAlbumHistoryAll>(false, this->sub_historyAlbumAll, p_data);
        }else if(p_pageCode==PAGECODE_T_HISTORY_TRACK_VIEW){
            this->sub_historyTrackAll = this->procCommon_showSubPage<tidal::TidalTrackHistoryAll>(false, this->sub_historyTrackAll, p_data);
        }else if(p_pageCode==PAGECODE_T_HISTORY_ARTIST_VIEW){
            this->sub_historyArtistAll = this->procCommon_showSubPage<tidal::TidalArtistHistoryAll>(false, this->sub_historyArtistAll, p_data);
        }else if(p_pageCode==PAGECODE_T_MY_RECENTLY_LIST_DELETE){
            this->sub_recentlyListDelete = this->procCommon_showSubPage<tidal::TidalRecentlyListDelete>(false, this->sub_recentlyListDelete, p_data);

        }
        else if(p_pageCode==PAGECODE_T_MY_COLLECTION){
            this->sub_myCollection = this->procCommon_showSubPage<tidal::TidalMycollection>(true, this->sub_myCollection, p_data);
        }else if(p_pageCode==PAGECODE_T_MY_COLLECTION_TIDALPALYLIST_ALL_LIST){
            this->sub_myTidalPlaylistAll = this->procCommon_showSubPage<tidal::TidalMyTidalPlaylistAll>(false, this->sub_myTidalPlaylistAll, p_data);
        }else if(p_pageCode==PAGECODE_T_MY_COLLECTION_PLAYLIST_ALL_LIST){
            this->sub_myPlaylistAll = this->procCommon_showSubPage<tidal::TidalMyPlaylistAll>(false, this->sub_myPlaylistAll, p_data);
        }else if(p_pageCode==PAGECODE_T_MY_COLLECTION_PLAYLIST_DETAIL){
            this->sub_myPlaylistDetail = this->procCommon_showSubPage<tidal::TidalMyPlaylistDetail>(false, this->sub_myPlaylistDetail, p_data);
        }else if(p_pageCode==PAGECODE_T_MY_COLLECTION_PLAYLIST_EDIT){
            this->sub_myPlaylistEdit = this->procCommon_showSubPage<tidal::TidalMyPlaylistEdit>(false, this->sub_myPlaylistEdit, p_data);
        }else if(p_pageCode==PAGECODE_T_MY_COLLECTION_ALBUM_ALL_LIST){
            this->sub_myAlbumAll = this->procCommon_showSubPage<tidal::TidalMyAlbumAll>(false, this->sub_myAlbumAll, p_data);
        }else if(p_pageCode==PAGECODE_T_MY_COLLECTION_ARTIST_ALL_LIST){
            this->sub_myArtistAll = this->procCommon_showSubPage<tidal::TidalMyArtistAll>(false, this->sub_myArtistAll, p_data);
        }else if(p_pageCode==PAGECODE_T_MY_COLLECTION_TRACK_ALL_LIST){
            this->sub_myTrackAll = this->procCommon_showSubPage<tidal::TidalMyTrackAll>(false, this->sub_myTrackAll, p_data);
        }else if(p_pageCode==PAGECODE_T_MY_COLLECTION_VIDEO_ALL_LIST){
            this->sub_myVideoAll = this->procCommon_showSubPage<tidal::TidalMyVideoAll>(false, this->sub_myVideoAll, p_data);
        }else if(p_pageCode==PAGECODE_T_MY_ROSE_PLAYLIST_ALL_LIST){
            this->sub_myRosePlaylistAll = this->procCommon_showSubPage<tidal::TidalMyRosePlaylistAll>(false, this->sub_myRosePlaylistAll, p_data);
        }else if(p_pageCode==PAGECODE_T_USER_ROSE_PLAYLIST_ALL_LIST){
            this->sub_userRosePlaylistAll = this->procCommon_showSubPage<tidal::TidalUserRosePlaylistAll>(false, this->sub_userRosePlaylistAll, p_data);
        }else if(p_pageCode==PAGECODE_T_MY_ROSE_PLAYLIST_DETAIL){
            this->sub_myRosePlaylistDetail = this->procCommon_showSubPage<tidal::TidalMyPlaylistDetail_ROSE>(false, this->sub_myRosePlaylistDetail, p_data);
        }else if(p_pageCode==PAGECODE_T_MY_RECENTLY_ALBUM_ALL_LIST){
            this->sub_recentlyAlbumAll = this->procCommon_showSubPage<tidal::TidalRecentlyAlbumAll>(false, this->sub_recentlyAlbumAll, p_data);
        }else if(p_pageCode==PAGECODE_T_MY_RECENTLY_PLAYLIST_ALL_LIST){
            this->sub_recentlyPlaylistAll = this->procCommon_showSubPage<tidal::TidalRecentlyPlaylistAll>(false, this->sub_recentlyPlaylistAll, p_data);
        }else if(p_pageCode==PAGECODE_T_MY_RECENTLY_TRACK_ALL_LIST){
            this->sub_recentlyTrackAll = this->procCommon_showSubPage<tidal::TidalRecentlyTrackAll>(false, this->sub_recentlyTrackAll, p_data);
        }else if(p_pageCode==PAGECODE_T_MY_RECENTLY_ARTIST_ALL_LIST){
            this->sub_recentlyArtistAll = this->procCommon_showSubPage<tidal::TidalRecenltyArtistAll>(false, this->sub_recentlyArtistAll, p_data);

        }else if(p_pageCode==PAGECODE_T_SEARCH_MAIN){
            this->sub_searchMain = this->procCommon_showSubPage<tidal::TidalSearchMain>(false, this->sub_searchMain, p_data);
        }else if(p_pageCode==PAGECODE_T_SEARCH_TRACK_ALL_VIEW){
            this->sub_searchTrackAll = this->procCommon_showSubPage<tidal::TidalSearchTrackAll>(false, this->sub_searchTrackAll, p_data);
        }else if(p_pageCode==PAGECODE_T_SEARCH_ARTIST_ALL_VIEW){
            this->sub_searchArtistAll = this->procCommon_showSubPage<tidal::TidalSearchArtistAll>(false, this->sub_searchArtistAll, p_data);
        }else if(p_pageCode==PAGECODE_T_SEARCH_ALBUM_ALL_VIEW){
            this->sub_searchAlbumAll = this->procCommon_showSubPage<tidal::TidalSearchAlbumAll>(false, this->sub_searchAlbumAll, p_data);
        }else if(p_pageCode==PAGECODE_T_SEARCH_PLAYLIST_ALL_VIEW){
            this->sub_searchPlaylistAll = this->procCommon_showSubPage<tidal::TidalSearchPlaylistAll>(false, this->sub_searchPlaylistAll, p_data);
        }else if(p_pageCode==PAGECODE_T_SEARCH_VIDEO_ALL_VIEW){
            this->sub_searchVideoAll = this->procCommon_showSubPage<tidal::TidalSearchVidelAll>(false, this->sub_searchVideoAll, p_data);
        }
    }


    /**
     * @brief TidalMain::procCommon_showSubPage
     * @param flagShow_topBar
     * @param sub_widget
     */
    template<typename T_TidalSub> T_TidalSub* TidalMain::procCommon_showSubPage(const bool flagShow_topBar, T_TidalSub* sub_widget, const QJsonObject &jsonObj_data){

        this->topMenuBar->setVisible(flagShow_topBar);
        //this->animate_fadeOut_forSubWidget(this->stackedWidget_content->currentWidget());

        if(sub_widget == nullptr){
            sub_widget = new T_TidalSub();            
            this->stackedWidget_content->addWidget(sub_widget);
            this->stackedWidget_content->addWidget(le_search_back);//c220730
            sub_widget->setConnectSiganl_toMovePage(this, SLOT(slot_clickedMoveSubPage(const QJsonObject&)));
            //sub_widget->setConnectSiganl_toShouldGoBackward(this, SLOT(goToBackPage));
        }

        stackedWidget_content->setCurrentWidget(sub_widget);
        if(ProcJsonEasy::getString(jsonObj_data, "pageCode") == PAGECODE_T_HOME || ProcJsonEasy::getString(jsonObj_data, "pageCode") == PAGECODE_T_EXPLORE || ProcJsonEasy::getString(jsonObj_data, "pageCode") == PAGECODE_T_VIDEO
             || ProcJsonEasy::getString(jsonObj_data, "pageCode") == PAGECODE_T_MASTER  || ProcJsonEasy::getString(jsonObj_data, "pageCode") == PAGECODE_T_MY_COLLECTION){
            sub_widget->setJsonObject_forData(jsonObj_data);
        }
        else{
            sub_widget->setJsonObject_forData(ProcJsonEasy::getJsonObject(jsonObj_data, KEY_DATA));
        }
        sub_widget->setActivePage();
        //this->animate_fadeIn_forSubWidget(sub_widget);                                  // fade-in

        return sub_widget;
    }


    /**
     * @brief TidalMain::animate_fadeIn_forSubWidget
     * @details 참고링크 @see https://stackoverflow.com/questions/19087822/how-to-make-qt-widgets-fade-in-or-fade-out/32985126
     * @param sub_pageWidget
     */
    void TidalMain::animate_fadeIn_forSubWidget(QWidget *p_widget){

        QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
        p_widget->setGraphicsEffect(eff);
        QPropertyAnimation *animation = new QPropertyAnimation(eff,"opacity");
        animation->setDuration(250);
        animation->setStartValue(0);
        animation->setEndValue(1);
        animation->setEasingCurve(QEasingCurve::InBack);
        animation->start(QPropertyAnimation::DeleteWhenStopped);
    }


    /**
     * @brief TidalMain::animate_fadeOut_forSubWidget
     * @details 참고링크 @see https://stackoverflow.com/questions/19087822/how-to-make-qt-widgets-fade-in-or-fade-out/32985126
     * @param sub_pageWidget
     */
    void TidalMain::animate_fadeOut_forSubWidget(QWidget *p_widget){

        QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
        p_widget->setGraphicsEffect(eff);
        QPropertyAnimation *animation = new QPropertyAnimation(eff,"opacity");
        animation->setDuration(0);
        animation->setStartValue(1);
        animation->setEndValue(0);
        animation->setEasingCurve(QEasingCurve::OutBack);
        animation->start(QPropertyAnimation::DeleteWhenStopped);
        //connect(animation, SIGNAL(finished()), this, SLOT(hideThisWidget()));
        // now implement a slot called hideThisWidget() to do
        // things like hide any background dimmer, etc.
    }


    void TidalMain::resizeEvent(QResizeEvent *event){

        AbstractMainContent::resizeEvent(event);
    }


    void TidalMain::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

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
                if(global.user_forTidal.getHeighestSound() <= 0){//c230422
                    print_debug();emit linker->signal_checkQueue(227, "");
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

}
