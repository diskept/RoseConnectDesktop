#include "bugs/BugsMain.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/DataCommon_Bugs.h"
#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"

#include "common/common_struct.h"
#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "widget/toastmsg.h"

#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QMetaObject>


namespace bugs {


    /**
     * @brief Bugs 메인 생성자
     * @param parent
     */
    BugsMain::BugsMain(QWidget *parent) : AbstractMainContent(parent){

        //connect(linker, SIGNAL(signal_search(QString)), SLOT(process_searchWord(QString)));
        connect(linker, SIGNAL(signal_search(QString)), SLOT(slot_search(QString)));
        connect(linker, SIGNAL(signal_dragEnterEvent_hide_show(bool)), SLOT(slot_dragEnterEvent_hide_show(bool)));//c220730
        connect(linker, SIGNAL(signal_dropEvent_hide_show(bool)), SLOT(slot_dropEvent_hide_show(bool)));//c220730

        make_CustomLineEdit();//c220730

        if(global.user_forBugs.isLogined() == false){
            // Rose 장비로부터 Session 정보를 요청
            ProcRoseAPI_withBugs *procRose = new ProcRoseAPI_withBugs(this);
            connect(procRose, &ProcRoseAPI_withBugs::signal_completeReq_get_session_info, this, &BugsMain::slot_completeReq_get_session_info);
            procRose->request_get_session_info();
        }
        else{
            //request_login_getMemberInfo();
        }

        this->setUIControl();
    }

    void BugsMain::slot_overrideSigalSearch(bool b){//c220728
        print_debug();
        if(b){
            emit linker->signal_clickedMovePageRoseTubeSearch();
        }else{
            slot_dragEnterEvent_hide_show(false);
            emit linker->signal_checkQueue(30, "");//c220729
        }

    }

    void BugsMain::slot_dragEnterEvent_hide_show(bool show){//c220826_1
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


    void BugsMain::slot_dragEnterEvent_restore(){//c220826_1
        print_debug();
        this->curr_widget = this->stackedWidget_content->currentWidget();//c220824_4
        this->le_search_back->clear();
        emit linker->signal_searchBarFocusChanged(false);
        this->le_search_back->clearFocus();
        this->curr_widget->show();
        this->le_search_back->hide();
    }

    void BugsMain::request_login_getMemberInfo(){//cheon210619-login

            // get token info
            ProcBugsAPI *proc = new ProcBugsAPI(this);
            bugs::BugsTokenInfo tokenInfo = proc->getTokenInfo_bugsDB();


            // Rose 에게 bugs session 정보를 전달함
            bugs::RoseSessionInfo_forBugs sessionInfo;

            sessionInfo.BUGS_TokenType = tokenInfo.token_type;

            sessionInfo.BUGS_Scope = tokenInfo.scope;

            sessionInfo.BUGS_ExpiresIn = QString("%1").arg(tokenInfo.expires_in);
            sessionInfo.BUGS_RefreshToken = tokenInfo.refresh_token;
            sessionInfo.BUGS_AccessToken = tokenInfo.access_token;

            sessionInfo.BUGS_Nickname = global.user_forBugs.getNickName();
            sessionInfo.BUGS_ProductName = global.user_forBugs.getProductName();

            sessionInfo.BUGS_AdultYn = global.user_forBugs.getAdult_yn();
            sessionInfo.BUGS_ProductDisplayEndDt = global.user_forBugs.getProduct_display_end_dt();
            sessionInfo.BUGS_StreamingYn = global.user_forBugs.getStreaming_yn();
            sessionInfo.BUGS_FlacPremiumYn = global.user_forBugs.getFlac_premium_yn();
            global.user_forBugs.set_loginState(tokenInfo.access_token, tokenInfo.refresh_token);
            /*qDebug()<<"BUGS_TokenType" << sessionInfo.BUGS_TokenType;
            qDebug()<<"BUGS_Scope" << sessionInfo.BUGS_Scope;
            qDebug()<<"BUGS_ExpiresIn" << sessionInfo.BUGS_ExpiresIn;
            qDebug()<<"BUGS_RefreshToken" << sessionInfo.BUGS_RefreshToken;
            qDebug()<<"BUGS_AccessToken" << sessionInfo.BUGS_AccessToken;
            qDebug()<<"BUGS_Nickname" << sessionInfo.BUGS_Nickname;
            qDebug()<<"BUGS_ProductName" << sessionInfo.BUGS_ProductName;
            qDebug()<<"BUGS_AdultYn" << sessionInfo.BUGS_AdultYn;
            qDebug()<<"BUGS_ProductDisplayEndDt" << sessionInfo.BUGS_ProductDisplayEndDt;
            qDebug()<<"BUGS_StreamingYn" << sessionInfo.BUGS_StreamingYn;
            qDebug()<<"BUGS_FlacPremiumYn" << sessionInfo.BUGS_FlacPremiumYn;*/

            /*

            sessionInfo.BUGS_Nickname = ProcJsonEasy::getString(jsonObj_member, "nickname");
            sessionInfo.BUGS_AdultYn = ProcJsonEasy::getBool(jsonObj_member, "adult_yn");

            sessionInfo.BUGS_ProductName = ProcJsonEasy::getString(jsonObj_product, "product_name");
            sessionInfo.BUGS_ProductDisplayEndDt = ProcJsonEasy::getString(jsonObj_product, "product_display_end_dt");

            QJsonObject jsonObj_right = ProcJsonEasy::getJsonObject(jsonObj_product, "rights");
            sessionInfo.BUGS_StreamingYn = ProcJsonEasy::getBool(jsonObj_right, "streaming_yn");
            sessionInfo.BUGS_FlacPremiumYn = ProcJsonEasy::getBool(jsonObj_right, "flac_premium_yn");
*/
            ProcRoseAPI_withBugs *procRose = new ProcRoseAPI_withBugs(this);
            procRose->request_set_session_info(sessionInfo);


            // 사용자의 정보를 저장한다.
            //global.user_forBugs.setNickName(sessionInfo.BUGS_Nickname);
            //global.user_forBugs.setProductName(sessionInfo.BUGS_ProductName);


            // 로그인 상태를 반영하여 페이지 변경함
          //  this->applyLoginSuccessState();

    }


    /**
     * @brief 검색어에 대하여 Bugs 검색 메인 페이지를 보이도록 한다.
     */
    void BugsMain::process_searchWord(const QString &search_word){

        if(search_word.size() >= 2){
            global.search_text = search_word;//searchtextcopy

            common::SearchData data_search;
            data_search.search_word = search_word;

            QJsonObject jsonObj_search;
            jsonObj_search[KEY_DATA] = common::ConvertData_common::getObjectJson_searchData(data_search);
            jsonObj_search[KEY_PAGE_CODE] = PAGECODE_BUGS___SEARCH_MAIN;

            this->goToMoveNewOrderPage(jsonObj_search);
        }
        else{
            ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
        }
    }


    void BugsMain::slot_search(const QString &search_word){

        if(search_word.size() >= 2){
            global.search_text = search_word;//searchtextcopy

            common::SearchData data_search;
            data_search.search_word = search_word;

            QJsonObject jsonObj_search;
            jsonObj_search[KEY_DATA] = common::ConvertData_common::getObjectJson_searchData(data_search);
            jsonObj_search[KEY_PAGE_CODE] = PAGECODE_BUGS___SEARCH_MAIN;

            this->goToMoveNewOrderPage(jsonObj_search);
        }
        else{
            ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
        }
    }


    /**
     * @brief BugsMain::setUIControl : UI 세팅
     */
    void BugsMain::setUIControl(){

        // default : 첫번째 서브메뉴 - (Bugs > Home)
        //QString pageCode_firstPage = PAGECODE_BUGS___HOME;
        //QJsonObject jsonObj_first;
        //jsonObj_first[KEY_PAGE_CODE] = pageCode_firstPage;

        QString pageCode_firstPage;
        if(global.user_forBugs.isLogined() == true)
        {
            pageCode_firstPage = PAGECODE_BUGS___HOME;
        }
        else {
            if(global.enable_section_left == true){
                global.enable_section_left = false;
            }

            pageCode_firstPage = PAGECODE_BUGS___SETTING;
        }

        QJsonObject jsonObj_first;
        jsonObj_first[KEY_PAGE_CODE] = pageCode_firstPage;

        if(!pageCode_firstPage.isEmpty()){
            this->goToMoveNewOrderPage(jsonObj_first);
        }
    }


    /**
     * @brief Bugs 각 하위 페이지 클래스 마다, 발생하는 signal_clickedMovePage 시그널을 공통으로 처리한다.
     * @param p_jsonData : 요청한 페이지에 대한 정보가 담겨있다. JsonObject 형식. @n
     *    @li KEY_PAGE_CODE : 해당 페이지 클래스에 대응되는 Unique한 값이다. @see gscommon.h 파일에 정의.
     *    @li KEY_DATA : 옵션 JsonObject. 해당 페이지 클래스에 그대로 넘겨서 보내줄 데이터이다. {} 일 수 있음.
     */
    void BugsMain::slot_clickedMoveSubPage(const QJsonObject &p_jsonData){

        // 페이지 히스토리 관리 관련된 Abs함수
        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = p_jsonData[KEY_PAGE_CODE].toString();
        tmp_data[KEY_DATA] = p_jsonData;
        this->goToMoveNewOrderPage(tmp_data);
    }


    /**
     * @brief TopMenuBar 객체에게 전달하기 위한 Main 타이틀 정보를 JsonObject 로 생성하여 반환
     * @return QJsonObject
     */
    QJsonObject BugsMain::getJsonObject_forMainTitle(){

        QJsonObject jsonObj;
        jsonObj["name"] = tr("Bugs");
        //jsonObj["name"] = "벅스";
        jsonObj[KEY_MAIN_CODE] = GSCommon::MainMenuCode::Bugs;
        return jsonObj;
    }


    /**
     * @brief TopMenuBar 객체에게 전달하기 위한 Sub 타이틀 정보 여러개를 JsonArray 로 생성하여 반환
     * @return QJsonArray* :
     */
    QJsonArray BugsMain::getJsonArray_forSubTitles(){

        QJsonArray p_jsonArray_titlSub ;
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", "홈"}, {"code", PAGECODE_BUGS___HOME} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", "탐색"}, {"code", PAGECODE_BUGS___EXPLORE} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", "나의 벅스"}, {"code", PAGECODE_BUGS___MY_COLLECTION} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", "설정"}, {"code", PAGECODE_BUGS___SETTING} });

        return p_jsonArray_titlSub;
    }


    /**
     * @brief BugsMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
     * @param p_pageCode
     */
    //void BugsMain::movePageOnly(QString p_pageCode, const QJsonObject &p_data){
    void BugsMain::movePageOnly(QString b_pageCode, const QJsonObject &b_data){

        QString p_pageCode;
        QJsonObject p_data;
        if(global.user_forBugs.isLogined()){
            p_pageCode = b_pageCode;
            p_data = b_data;
        }
        else{
            p_pageCode = PAGECODE_BUGS___SETTING;
            p_data[KEY_PAGE_CODE] = p_pageCode;
        }

        QJsonObject tmp_jsonObj_titleMain = this->getJsonObject_forMainTitle();
        QJsonArray tmp_jsonArr_titlSub = this->getJsonArray_forSubTitles();

        // 부모 virtual 함수 호출
        AbstractMainContent::movePageOnly(p_pageCode, p_data);

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


        // 서브 메뉴페이지에 대한 처리 : edited by sunnyfish
        if(p_pageCode==PAGECODE_BUGS___HOME){
            this->sub_home = this->procCommon_showSubPage<bugs::BugsHome>(true, this->sub_home, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___HOME_MUSIC_RECENTLY){
            this->sub_musicRecently = this->procCommon_showSubPage<bugs::BugsMusicRecently>(false, this->sub_musicRecently, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___HOME_MUSIC_CHART){
            this->sub_musicChart = this->procCommon_showSubPage<bugs::BugsMusicChart>(false, this->sub_musicChart, p_data);

        }else if(p_pageCode == PAGECODE_BUGS___HOME_TRACK_RECENTLY){
            this->sub_trackRecentlyAll = this->procCommon_showSubPage<bugs::BugsTrackRecentlyAll>(false, this->sub_trackRecentlyAll, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___HOME_ALBUM_RECENTLY){
            this->sub_albumRecentlyAll = this->procCommon_showSubPage<bugs::BugsAlbumRecentlyAll>(false, this->sub_albumRecentlyAll, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___HOME_VIDEO_RECENTLY){
            this->sub_videoRecentlyAll = this->procCommon_showSubPage<bugs::BugsVideoRecentlyAll>(false, this->sub_videoRecentlyAll, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___HOME_TRACK_CHART){
            this->sub_trackChartAll = this->procCommon_showSubPage<bugs::BugsTrackChartAll>(false, this->sub_trackChartAll, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___HOME_ALBUM_CHART){
            this->sub_albumChartAll = this->procCommon_showSubPage<bugs::BugsAlbumChartAll>(false, this->sub_albumChartAll, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___HOME_VIDEO_CHART){
            this->sub_videoChartAll = this->procCommon_showSubPage<bugs::BugsVideoChartAll>(false, this->sub_videoChartAll, p_data);

        }else if(p_pageCode == PAGECODE_BUGS___EXPLORE){
            this->sub_explore = this->procCommon_showSubPage<bugs::BugsExplore>(true, this->sub_explore, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___EXPLORE___CHOOSE_THEME){
            this->sub_themeChoose = this->procCommon_showSubPage<bugs::BugsThemesChoose>(false, this->sub_themeChoose, p_data);

        }else if(p_pageCode == PAGECODE_BUGS___MY_COLLECTION){
            this->sub_myCollection = this->procCommon_showSubPage<bugs::BugsMyCollection>(true, this->sub_myCollection, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___MY_COLLECTION___LIKE_MUSIC){
            this->sub_myLikeMusic = this->procCommon_showSubPage<bugs::BugsMyLikeMusic>(false, this->sub_myLikeMusic, p_data);

        }else if(p_pageCode == PAGECODE_BUGS___SETTING){
            this->sub_settings = this->procCommon_showSubPage<bugs::BugsSettings>(true, this->sub_settings, p_data);


        }else if(p_pageCode == PAGECODE_BUGS___ALBUM_DETAIL){
            this->sub_albumDetail = this->procCommon_showSubPage<bugs::BugsAlbumDetail>(false, this->sub_albumDetail, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___PD_ALBUM_DETAIL){
            this->sub_pd_albumDetail = this->procCommon_showSubPage<bugs::BugsPDAlbumDetail>(false, this->sub_pd_albumDetail, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___MY_ALBUM_DETAIL){
            this->sub_my_albumDetail = this->procCommon_showSubPage<bugs::BugsMyAlbumDetail>(false, this->sub_my_albumDetail, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___ARTIST_DETAIL){
            this->sub_artistDetail = this->procCommon_showSubPage<bugs::BugsArtistDetail>(false, this->sub_artistDetail, p_data);


        }else if(p_pageCode == PAGECODE_BUGS___TRACK_ALL_VIEW){
            this->sub_trackAll = this->procCommon_showSubPage<bugs::BugsTracksListAll>(false, this->sub_trackAll, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___TRACK_SHARE_ALL_VIEW){
            this->sub_trackAll_Share = this->procCommon_showSubPage<bugs::BugsTrackListAll_Share>(false, this->sub_trackAll_Share, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___ALBUM_ALL_VIEW___OF_ARTIST){
            this->sub_albumAll_ofArtist = this->procCommon_showSubPage<bugs::BugsAlbumListAll_ofArtist>(false, this->sub_albumAll_ofArtist, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___TRACK_ALL_VIEW___OF_ARTIST){
            this->sub_trackAll_ofArtist = this->procCommon_showSubPage<bugs::BugsTrackListAll_ofArtist>(false, this->sub_trackAll_ofArtist, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___PD_ALBUM_ALL_VIEW){
            this->sub_pd_albumAll = this->procCommon_showSubPage<bugs::BugsPDAlbumListAll>(false, this->sub_pd_albumAll, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___VIDEO_ALL_VIEW___OF_TRACK){
            this->sub_videoAll_withTrack = this->procCommon_showSubPage<bugs::BugsVideoList_withTrack>(false, this->sub_videoAll_withTrack, p_data);


        }else if(p_pageCode == PAGECODE_BUGS___SEARCH_MAIN){
            this->sub_searchMain = this->procCommon_showSubPage<bugs::BugsSearchMain>(false, this->sub_searchMain, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___SEARCH_TRACK_ALL_VIEW){
            this->sub_searchTrackAll = this->procCommon_showSubPage<bugs::BugsSearchTrackAll>(false, this->sub_searchTrackAll, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___SEARCH_ARTIST_ALL_VIEW){
            this->sub_searchArtistAll = this->procCommon_showSubPage<bugs::BugsSearchArtistAll>(false, this->sub_searchArtistAll, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___SEARCH_ALBUM_ALL_VIEW){
            this->sub_searchAlbumAll = this->procCommon_showSubPage<bugs::BugsSearchAlbumAll>(false, this->sub_searchAlbumAll, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___SEARCH_PD_ALBUM_ALL_VIEW){
            this->sub_searchPD_AlbumAll = this->procCommon_showSubPage<bugs::BugsSearchPD_AlbumAll>(false, this->sub_searchPD_AlbumAll, p_data);
        }else if(p_pageCode == PAGECODE_BUGS___SEARCH_VIDEO_ALL_VIEW){
            this->sub_searchVideoAll = this->procCommon_showSubPage<bugs::BugsSearchVideoAll>(false, this->sub_searchVideoAll, p_data);


        }else if(p_pageCode == PAGECODE_BUGS__MY_RECENTLY_ALBUM_ALL_LIST){
            this->sub_recentlyAlbumAll = this->procCommon_showSubPage<bugs::BugsRecentlyAlbumAll>(false, this->sub_recentlyAlbumAll, p_data);
        }else if(p_pageCode == PAGECODE_BUGS__MY_RECENTLY_PLAYLIST_ALL_LIST){
            this->sub_recentlyPlaylistAll = this->procCommon_showSubPage<bugs::BugsRecentlyPlaylistAll>(false, this->sub_recentlyPlaylistAll, p_data);
        }else if(p_pageCode == PAGECODE_BUGS__MY_RECENTLY_TRACK_ALL_LIST){
            this->sub_recentlyTrackAll = this->procCommon_showSubPage<bugs::BugsRecentlyTrackAll>(false, this->sub_recentlyTrackAll, p_data);
        }else if(p_pageCode == PAGECODE_BUGS__MY_ROSE_PLAYLIST_DETAIL){
            this->sub_playlistDetail = this->procCommon_showSubPage<bugs::BugsPlaylistDetail>(false, this->sub_playlistDetail, p_data);
        }else if(p_pageCode == PAGECODE_BUGS__MY_ROSE_PLAYLIST_ALL_LIST){
            this->sub_myRosePlaylistAll = this->procCommon_showSubPage<bugs::BugsMyRosePlaylistAll>(false, this->sub_myRosePlaylistAll, p_data);
        }else if(p_pageCode == PAGECODE_BUGS__USER_ROSE_PLAYLIST_ALL_LIST){
            this->sub_userRosePlaylistAll = this->procCommon_showSubPage<bugs::BugsUserRosePlaylistAll>(false, this->sub_userRosePlaylistAll, p_data);
        }
    }



    /**
     * @brief BugsMain::procCommon_showSubPage
     * @param flagShow_topBar
     * @param sub_widget
     */
    template<typename T_BugsSub> T_BugsSub* BugsMain::procCommon_showSubPage(const bool flagShow_topBar, T_BugsSub* sub_widget, const QJsonObject &jsonObj_data){

        this->topMenuBar->setVisible(flagShow_topBar);

        if(sub_widget == nullptr){
            sub_widget = new T_BugsSub();
            this->stackedWidget_content->addWidget(sub_widget);
            this->stackedWidget_content->addWidget(le_search_back);//c220730
            sub_widget->setConnectSiganl_toMovePage(this, SLOT(slot_clickedMoveSubPage(const QJsonObject&)));
//            sub_widget->setConnectSiganl_toShouldGoBackward(this, SLOT(goToBackPage));
        }

        stackedWidget_content->setCurrentWidget(sub_widget);
        if(ProcJsonEasy::getString(jsonObj_data, "pageCode") == PAGECODE_BUGS___HOME){
            sub_widget->setJsonObject_forData(jsonObj_data);
        }
        else{
            sub_widget->setJsonObject_forData(ProcJsonEasy::getJsonObject(jsonObj_data, KEY_DATA));
        }
        sub_widget->setActivePage();

        return sub_widget;
    }



    /**
     * @brief BugsMain::slot_completeReq_get_session_info
     * @param sessionInfo
     */
    void BugsMain::slot_completeReq_get_session_info(const RoseSessionInfo_forBugs& sessionInfo){

        // Rose로부터 받은 Session 정보
        if(!sessionInfo.BUGS_AccessToken.isEmpty()){
            // 로그인 상태를 저장함
            global.user_forBugs.set_loginState(sessionInfo.BUGS_AccessToken, sessionInfo.BUGS_RefreshToken);
            global.user_forBugs.setNickName(sessionInfo.BUGS_Nickname);
            global.user_forBugs.setProductName(sessionInfo.BUGS_ProductName);

            AbstractBugsSubWidget *sub_curr = qobject_cast<AbstractBugsSubWidget*>(this->stackedWidget_content->currentWidget());
            sub_curr->applyLoginSuccessState();


            // 전체 장르 정보를 요청함 - 공통으로 사용 (세팅 안된 경우)
            DataCommon_Bugs& dataCommon_bugs = DataCommon_Bugs::instance();
            if(dataCommon_bugs.list_all.size() == 0){
                ProcBugsAPI *proc = new ProcBugsAPI(this);
                proc->request_bugs_getListGenres();
            }

        }
        else{
            // 로그아웃으로 처리함
            global.user_forBugs.set_logoutState();
        }
    }




    void BugsMain::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

        print_debug();
        QJsonDocument doc(p_jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "roseHomeMain::slot_responseHttp--p_jsonObject= " << strJson;

        //Q_UNUSED(p_jsonObj);
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

}
