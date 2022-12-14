#include "qobuz/QobuzMain.h"

#include "qobuz/ProcCommon_forQobuz.h"
#include "qobuz/ProcRosePlay_withQobuz.h"

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


namespace qobuz {


    /**
     * @brief QOBUZ 메인 생성자
     * @param parent
     */
    QobuzMain::QobuzMain(QWidget *parent) : AbstractMainContent(parent){

        //connect(linker, SIGNAL(signal_search(QString)), SLOT(process_searchWord(QString)));
        connect(linker, SIGNAL(signal_search(QString)), SLOT(slot_search(QString)));

        connect(linker, SIGNAL(signal_dragEnterEvent_hide_show(bool)), this, SLOT(slot_dragEnterEvent_hide_show(bool)));//c220730
        connect(linker, SIGNAL(signal_dropEvent_hide_show(bool)), this,SLOT(slot_dropEvent_hide_show(bool)));//c220730

        make_CustomLineEdit();//c220730

        if(global.user_forQobuz.isLogined() == false){
            // Rose 장비로부터 Session 정보를 요청
            qobuz::ProcRosePlay_withQobuz *procRose = new qobuz::ProcRosePlay_withQobuz(this);
            connect(procRose, &qobuz::ProcRosePlay_withQobuz::signal_completeReq_get_session_info, this, &QobuzMain::slot_completeReq_get_session_info);
            procRose->request_get_session_info();
        }
        else{//cheon210617-login
            if(global.qobuzloginChange){//cheon210617-login

            }

            //qobuz::ProcCommon *procQobuz = new qobuz::ProcCommon(this);//cheon210617-login
            //qobuz::UserLoginInfo qobuz_userLoginInfo = procQobuz->getLoginInfo_qobuzDB();//cheon210617-login
            //connect(procQobuz, SIGNAL(failedLogin(const QString&)), this, SLOT(slot_failedLogin(const QString&)));//cheon210617-login
            //connect(procQobuz, SIGNAL(successLogin()), this, SLOT(slot_successLogin()));//cheon210617-login
            //procQobuz->request_qobuzLogin(qobuz_userLoginInfo);//cheon210617-login
            //qDebug() << "로그인이 되어 있고 기기가 로그인이 않되어 있으면 커넥트에서 기기를 자동로그인할것";//cheon210617-login
        }

        this->setUIControl();
    }


    void QobuzMain::slot_failedLogin(const QString& errorMsg){//cheon210617-login

        if(global.user_forQobuz.isLogined() == false){
            setUIControl2();
            ToastMsg::show(this, "", errorMsg);
        }
        else{
            emit successLogin();
            setUIControl1();
        }
    }


    /**
     * @brief 로그인 성공! 성공 시그널 발생.
     */
    void QobuzMain::slot_successLogin(){//cheon210617-login

        emit successLogin();
        setUIControl1();
    }


    /**
     * @brief 검색어에 대하여 Qobuz 검색 메인 페이지를 보이도록 한다.
     */
    void QobuzMain::process_searchWord(const QString &search_word){

        if(search_word.size() >= 2){
            global.search_text = search_word;//searchtextcopy

            common::SearchData data_search;
            data_search.search_word = search_word;

            QJsonObject jsonObj_search;
            jsonObj_search[KEY_DATA] = common::ConvertData_common::getObjectJson_searchData(data_search);
            jsonObj_search[KEY_PAGE_CODE] = PAGECODE_Q_SEARCH_MAIN;

            this->goToMoveNewOrderPage(jsonObj_search);
        }
        else{
            ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
        }
    }


    void QobuzMain::slot_search(const QString &search_word){

        if(search_word.size() >= 2){
            global.search_text = search_word;//searchtextcopy

            common::SearchData data_search;
            data_search.search_word = search_word;

            QJsonObject jsonObj_search;
            jsonObj_search[KEY_DATA] = common::ConvertData_common::getObjectJson_searchData(data_search);
            jsonObj_search[KEY_PAGE_CODE] = PAGECODE_Q_SEARCH_MAIN;

            this->goToMoveNewOrderPage(jsonObj_search);
        }
        else{
            ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
        }
    }


    /**
     * @brief QobuzMain::setUIControl : UI 세팅
     */
    void QobuzMain::setUIControl(){

        // default : 첫번째 서브메뉴 - (Qobuz > Home)
        //QString pageCode_firstPage = PAGECODE_Q_HOME;
        //QJsonObject jsonObj_first;
        //jsonObj_first[KEY_PAGE_CODE] = pageCode_firstPage;

        QString pageCode_firstPage;
        if(global.user_forQobuz.isLogined()){
            pageCode_firstPage = PAGECODE_Q_HOME;
        }        
        else{
            if(global.enable_section_left == true){
                global.enable_section_left = false;
            }

            pageCode_firstPage = PAGECODE_Q_SETTINGS;
        }

        QJsonObject jsonObj_first;
        jsonObj_first[KEY_PAGE_CODE] = pageCode_firstPage;
        //this->stackedWidget_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        if(!pageCode_firstPage.isEmpty()){
            this->goToMoveNewOrderPage(jsonObj_first);
        }
    }


    void QobuzMain::setUIControl1(){//cheon210617-login

        // default : 첫번째 서브메뉴 - (Qobuz > Home)
        //QString pageCode_firstPage = PAGECODE_Q_HOME;
        //QJsonObject jsonObj_first;
        //jsonObj_first[KEY_PAGE_CODE] = pageCode_firstPage;



        QString pageCode_firstPage;

        pageCode_firstPage = PAGECODE_Q_HOME;

        QJsonObject jsonObj_first;
        jsonObj_first[KEY_PAGE_CODE] = pageCode_firstPage;

        //this->stackedWidget_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        this->goToMoveNewOrderPage(jsonObj_first);
    }


    void QobuzMain::setUIControl2(){//cheon210617-login

        // default : 첫번째 서브메뉴 - (Qobuz > Home)
        //QString pageCode_firstPage = PAGECODE_Q_HOME;
        //QJsonObject jsonObj_first;
        //jsonObj_first[KEY_PAGE_CODE] = pageCode_firstPage;

        QString pageCode_firstPage;

        pageCode_firstPage = PAGECODE_Q_SETTINGS;

        QJsonObject jsonObj_first;
        jsonObj_first[KEY_PAGE_CODE] = pageCode_firstPage;

        //this->stackedWidget_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        this->goToMoveNewOrderPage(jsonObj_first);
    }


    /**
     * @brief Qobuz 각 하위 페이지 클래스 마다, 발생하는 signal_clickedMovePage 시그널을 공통으로 처리한다.
     * @param p_jsonData : 요청한 페이지에 대한 정보가 담겨있다. JsonObject 형식. @n
     *    @li KEY_PAGE_CODE : 해당 페이지 클래스에 대응되는 Unique한 값이다. @see gscommon.h 파일에 정의.
     *    @li KEY_DATA : 옵션 JsonObject. 해당 페이지 클래스에 그대로 넘겨서 보내줄 데이터이다. {} 일 수 있음.
     */
    void QobuzMain::slot_clickedMoveSubPage(const QJsonObject &p_jsonData){

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
    QJsonObject QobuzMain::getJsonObject_forMainTitle(){

        QJsonObject jsonObj;
        jsonObj["name"] = tr("Qobuz");
        //jsonObj["name"] = "코부즈";
        jsonObj[KEY_MAIN_CODE] = GSCommon::MainMenuCode::Qobuz;
        return jsonObj;
    }


    /**
     * @brief TopMenuBar 객체에게 전달하기 위한 Sub 타이틀 정보 여러개를 JsonArray 로 생성하여 반환
     * @return QJsonArray* :
     */
    QJsonArray QobuzMain::getJsonArray_forSubTitles(){

        QJsonArray p_jsonArray_titlSub ;
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Home")}, {"code", PAGECODE_Q_HOME} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Explore")}, {"code", PAGECODE_Q_EXPLORE} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("My QOBUZ")}, {"code", PAGECODE_Q_MY_COLLECTION} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Setting")}, {"code", PAGECODE_Q_SETTINGS} });

        return p_jsonArray_titlSub;
    }

    void QobuzMain::slot_overrideSigalSearch(bool b){//c220728
        print_debug();

        if(b){
            emit linker->signal_clickedMovePageRoseTubeSearch();
        }else{
            slot_dragEnterEvent_hide_show(false);
            emit linker->signal_checkQueue(30, "");//c220729
        }

    }

    void QobuzMain::slot_dragEnterEvent_hide_show(bool show){//c220826_1
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


    void QobuzMain::slot_dragEnterEvent_restore(){//c220826_1
        print_debug();
        this->curr_widget = this->stackedWidget_content->currentWidget();//c220824_4
        this->le_search_back->clear();
        emit linker->signal_searchBarFocusChanged(false);
        this->le_search_back->clearFocus();
        this->curr_widget->show();
        this->le_search_back->hide();
    }


    /**
     * @brief QobuzMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
     * @param p_pageCode
     */
    //void QobuzMain::movePageOnly(QString p_pageCode, const QJsonObject &p_data){
    void QobuzMain::movePageOnly(QString q_pageCode, const QJsonObject &q_data){

        QString p_pageCode;
        QJsonObject p_data;
        if(global.user_forQobuz.isLogined()){
            p_pageCode = q_pageCode;
            p_data = q_data;
        }
        else{
            p_pageCode = PAGECODE_Q_SETTINGS;
            p_data[KEY_PAGE_CODE] = p_pageCode;
        }

        QJsonObject tmp_jsonObj_titleMain = this->getJsonObject_forMainTitle();
        QJsonArray tmp_jsonArr_titlSub = this->getJsonArray_forSubTitles();

        // 부모 virtual 함수 호출
        AbstractMainContent::movePageOnly(p_pageCode, p_data);
        //qDebug() << "qobuz-------------------------------------------------------------------------";
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
        if(p_pageCode==PAGECODE_Q_HOME){
            this->sub_home = this->procCommon_showSubPage<qobuz::QobuzHome>(true, this->sub_home, p_data);
            //this->sub_home->hide();
        }else if(p_pageCode==PAGECODE_Q_EXPLORE){
            this->sub_explore = this->procCommon_showSubPage<qobuz::QobuzExplore>(true, this->sub_explore, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_CHOOSE_ALL){
            this->sub_genresChoose = this->procCommon_showSubPage<qobuz::QobuzGenresChoose>(false, this->sub_genresChoose, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_MAIN){
            this->sub_genresMain = this->procCommon_showSubPage<qobuz::QobuzGenresMain>(false, this->sub_genresMain, p_data);            
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_POP){
            this->sub_genresPop = this->procCommon_showSubPage<qobuz::QobuzGenresMain>(false, this->sub_genresPop, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_JAZZ){
            this->sub_genresJazz = this->procCommon_showSubPage<qobuz::QobuzGenresMain>(false, this->sub_genresJazz, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_CLASSIC){
            this->sub_genresClassic = this->procCommon_showSubPage<qobuz::QobuzGenresMain>(false, this->sub_genresClassic, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_ELECTRO){
            this->sub_genresElectro = this->procCommon_showSubPage<qobuz::QobuzGenresMain>(false, this->sub_genresElectro, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_SOUL){
            this->sub_genresSoul = this->procCommon_showSubPage<qobuz::QobuzGenresMain>(false, this->sub_genresSoul, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_FOLK){
            this->sub_genresFolk = this->procCommon_showSubPage<qobuz::QobuzGenresMain>(false, this->sub_genresFolk, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_HIPHOP){
            this->sub_genresHiphop = this->procCommon_showSubPage<qobuz::QobuzGenresMain>(false, this->sub_genresHiphop, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_COUNTRY){
            this->sub_genresCountry = this->procCommon_showSubPage<qobuz::QobuzGenresMain>(false, this->sub_genresCountry, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_METAL){
            this->sub_genresMetal = this->procCommon_showSubPage<qobuz::QobuzGenresMain>(false, this->sub_genresMetal, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_BLUES){
            this->sub_genresBlues = this->procCommon_showSubPage<qobuz::QobuzGenresMain>(false, this->sub_genresBlues, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_LATIN){
            this->sub_genresLatin = this->procCommon_showSubPage<qobuz::QobuzGenresMain>(false, this->sub_genresMain, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_SOUNDTRACKS){
            this->sub_genresSoundtracks = this->procCommon_showSubPage<qobuz::QobuzGenresMain>(false, this->sub_genresSoundtracks, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_WORLD){
            this->sub_genresWorld = this->procCommon_showSubPage<qobuz::QobuzGenresMain>(false, this->sub_genresWorld, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_GENRES_OTHER){
            this->sub_genresOther = this->procCommon_showSubPage<qobuz::QobuzGenresMain>(false, this->sub_genresOther, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_MOODS_MAIN){
            //this->sub_moodsMain = this->procCommon_showSubPage<qobuz::QobuzMoodsMain>(false, this->sub_moodsMain, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_NEW_MAIN){
            this->sub_newMain = this->procCommon_showSubPage<qobuz::QobuzNewMain>(false, this->sub_newMain, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_TOP_MAIN){
            this->sub_topMain = this->procCommon_showSubPage<qobuz::QobuzTop20Main>(false, this->sub_topMain, p_data);
        }else if(p_pageCode == PAGECODE_Q_EXPLORE_RISING_MAIN){
            //this->sub_risingMain = this->procCommon_showSubPage<qobuz::QobuzRisingMain>(false, this->sub_risingMain, p_data);
        }else if(p_pageCode==PAGECODE_Q_VIDEO){
            //this->sub_video = this->procCommon_showSubPage<qobuz::QobuzVideo>(true, this->sub_video, p_data);
        }else if(p_pageCode==PAGECODE_Q_MASTER){
            //this->sub_master = this->procCommon_showSubPage<qobuz::QobuzMaster>(true, this->sub_master, p_data);
        }else if(p_pageCode==PAGECODE_Q_SETTINGS){
            this->sub_settings = this->procCommon_showSubPage<qobuz::QobuzSettings>(true, this->sub_settings, p_data);

        }else if(p_pageCode==PAGECODE_Q_PLAYLIST_DETAIL){
            this->sub_playlistDetail = this->procCommon_showSubPage<qobuz::QobuzPlaylistDetail>(false, this->sub_playlistDetail, p_data);
        }else if(p_pageCode==PAGECODE_Q_ALBUM_DETAIL){
            this->sub_albumDetail = this->procCommon_showSubPage<qobuz::QobuzAlbumDetail>(false, this->sub_albumDetail, p_data);
        }else if(p_pageCode==PAGECODE_Q_TRACK_ALL_LIST_VIEW){
            this->sub_tracksListAll = this->procCommon_showSubPage<qobuz::QobuzTracksListAll>(false, this->sub_tracksListAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_TRACK_SHARE_ALL_LIST_VIEW){
            this->sub_trackListAll_Share = this->procCommon_showSubPage<qobuz::QobuzTrackListAll_Share>(false, this->sub_trackListAll_Share, p_data);
        }else if(p_pageCode==PAGECODE_Q_PLAYLIST_ALL_LIST_VIEW){
            this->sub_playlistAll = this->procCommon_showSubPage<qobuz::QobuzPlaylistAll>(false, this->sub_playlistAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_SIMILAR_PLAYLIST_VIEW){
            this->sub_similarlistAll = this->procCommon_showSubPage<qobuz::QobuzSimilarplaylistAll>(false, this->sub_similarlistAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_APPEARSON_ALL_LIST_VIEW){
            this->sub_appearsonListAll = this->procCommon_showSubPage<qobuz::QobuzAppearsonListAll>(false, this->sub_appearsonListAll, p_data);
        }else if(p_pageCode == PAGECODE_Q_LAST_RELEASE_LIST_VIEW){
            this->sub_lastRelease = this->procCommon_showSubPage<qobuz::QobuzLastReleaseList>(false, this->sub_lastRelease, p_data);
        }else if(p_pageCode == PAGECODE_Q_SAME_ARTIST_LIST_VIEW){
            this->sub_sameArtistAll = this->procCommon_showSubPage<qobuz::QobuzSameArtistAll>(false, this->sub_sameArtistAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_ALBUM_ALL_LIST_VIEW){
            this->sub_albumAll = this->procCommon_showSubPage<qobuz::QobuzAlbumListAll>(false, this->sub_albumAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_CLASSIC_ALL_LIST_VIEW){
            this->sub_classicAll = this->procCommon_showSubPage<qobuz::QobuzAlbumListAll>(false, this->sub_classicAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_IDEAL_DISCOGRAPHY_ALL_LIST_VIEW){
            this->sub_idealDiscographyAll = this->procCommon_showSubPage<qobuz::QobuzAlbumListAll>(false, this->sub_idealDiscographyAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_STILL_TRENDING_ALL_LIST_VIEW){
            this->sub_stillTrendingAll = this->procCommon_showSubPage<qobuz::QobuzAlbumListAll>(false, this->sub_stillTrendingAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_PRESS_AWARDS_ALL_LIST_VIEW){
            this->sub_pressAwardsAll = this->procCommon_showSubPage<qobuz::QobuzAlbumListAll>(false, this->sub_pressAwardsAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_QOBUZ_ISSIME_ALL_LIST_VIEW){
            this->sub_qobuzIssimeAll = this->procCommon_showSubPage<qobuz::QobuzAlbumListAll>(false, this->sub_qobuzIssimeAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_TOPRELEASE_ALL_LIST_VIEW){
            this->sub_topReleaseAll = this->procCommon_showSubPage<qobuz::QobuzAlbumListAll>(false, this->sub_topReleaseAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_ARTIST_ALL_LIST_VIEW){
            this->sub_artistAll = this->procCommon_showSubPage<qobuz::QobuzArtistListAll>(false, this->sub_artistAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_ARTIST_DETAIL){
            this->sub_artistDetail = this->procCommon_showSubPage<qobuz::QobuzArtistDetail>(false, this->sub_artistDetail, p_data);
        }else if(p_pageCode==PAGECODE_Q_VIDEO_ALL_LIST_VIEW){
            //this->sub_videoAll = this->procCommon_showSubPage<qobuz::QobuzVideoListAll>(false, this->sub_videoAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_ADD_PLAYLIST){
            this->sub_AddPlaylist = this->procCommon_showSubPage<qobuz::QobuzAddPlaylist>(false, this->sub_AddPlaylist, p_data);

        }else if(p_pageCode==PAGECODE_Q_MY_COLLECTION){
            this->sub_myCollection = this->procCommon_showSubPage<qobuz::QobuzMycollection>(true, this->sub_myCollection, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_COLLECTION_PLAYLIST_ALL_LIST){
            this->sub_myPlaylistAll = this->procCommon_showSubPage<qobuz::QobuzMyPlaylistAll>(false, this->sub_myPlaylistAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_COLLECTION_PLAYLIST_DETAIL){
            this->sub_myPlaylistDetail = this->procCommon_showSubPage<qobuz::QobuzMyPlaylistDetail>(false, this->sub_myPlaylistDetail, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_COLLECTION_PLAYLIST_EDIT){
            this->sub_myPlaylistEdit = this->procCommon_showSubPage<qobuz::QobuzMyPlaylistEdit>(false, this->sub_myPlaylistEdit, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_COLLECTION_FAVORITES){
            this->sub_myFavorites = this->procCommon_showSubPage<qobuz::QobuzMyFavorites>(false, this->sub_myFavorites, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_COLLECTION_ALBUM_ALL_LIST){
            this->sub_myAlbumAll = this->procCommon_showSubPage<qobuz::QobuzMyAlbumAll>(false, this->sub_myAlbumAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_COLLECTION_ARTIST_ALL_LIST){
            this->sub_myArtistAll = this->procCommon_showSubPage<qobuz::QobuzMyArtistAll>(false, this->sub_myArtistAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_COLLECTION_TRACK_ALL_LIST){
            this->sub_myTrackAll = this->procCommon_showSubPage<qobuz::QobuzMyTrackAll>(false, this->sub_myTrackAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_COLLECTION_VIDEO_ALL_LIST){
            //this->sub_myVideoAll = this->procCommon_showSubPage<qobuz::QobuzMyVideoAll>(false, this->sub_myVideoAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_COLLECTION_PURCHASES){
            this->sub_myPurchases = this->procCommon_showSubPage<qobuz::QobuzMyPurchases>(false, this->sub_myPurchases, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_COLLECTION_PURCHASES_ALBUM_LIST){
            this->sub_myPurchasesAlbumAll = this->procCommon_showSubPage<qobuz::QobuzMyPurchasesAlbumAll>(false, this->sub_myPurchasesAlbumAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_COLLECTION_PURCHASES_TRACK_LIST){
            this->sub_myPurchasesTrackAll = this->procCommon_showSubPage<qobuz::QobuzMyPurchasesTrackAll>(false, this->sub_myPurchasesTrackAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_RECENTLY_ALBUM_ALL_LIST){
            this->sub_recentlyAlbumAll = this->procCommon_showSubPage<qobuz::QobuzRecentlyAlbumAll>(false, this->sub_recentlyAlbumAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_RECENTLY_PLAYLIST_ALL_LIST){
            this->sub_recentlyPlaylistAll = this->procCommon_showSubPage<qobuz::QobuzRecentlyPlaylistAll>(false, this->sub_recentlyPlaylistAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_RECENTLY_TRACK_ALL_LIST){
            this->sub_recentlyTrackAll = this->procCommon_showSubPage<qobuz::QobuzRecentlyTrackAll>(false, this->sub_recentlyTrackAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_ROSE_PLAYLIST_DETAIL){
            this->sub_myRosePlaylistDetail = this->procCommon_showSubPage<qobuz::QobuzMyPlaylistDetail_ROSE>(false, this->sub_myRosePlaylistDetail, p_data);
        }else if(p_pageCode==PAGECODE_Q_MY_ROSE_PLAYLIST_ALL_LIST){
            this->sub_myRosePlaylistAll = this->procCommon_showSubPage<qobuz::QobuzMyRosePlaylistAll>(false, this->sub_myRosePlaylistAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_USER_ROSE_PLAYLIST_ALL_LIST){
            this->sub_userRosePlaylistAll = this->procCommon_showSubPage<qobuz::QobuzUserRosePlaylistAll>(false, this->sub_userRosePlaylistAll, p_data);

        }
        else if(p_pageCode==PAGECODE_Q_SEARCH_MAIN){
            this->sub_searchMain = this->procCommon_showSubPage<qobuz::QobuzSearchMain>(false, this->sub_searchMain, p_data);
        }else if(p_pageCode==PAGECODE_Q_SEARCH_TRACK_ALL_VIEW){
            this->sub_searchTrackAll = this->procCommon_showSubPage<qobuz::QobuzSearchTrackAll>(false, this->sub_searchTrackAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_SEARCH_ARTIST_ALL_VIEW){
            this->sub_searchArtistAll = this->procCommon_showSubPage<qobuz::QobuzSearchArtistAll>(false, this->sub_searchArtistAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_SEARCH_ALBUM_ALL_VIEW){
            this->sub_searchAlbumAll = this->procCommon_showSubPage<qobuz::QobuzSearchAlbumAll>(false, this->sub_searchAlbumAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_SEARCH_PLAYLIST_ALL_VIEW){
            this->sub_searchPlaylistAll = this->procCommon_showSubPage<qobuz::QobuzSearchPlaylistAll>(false, this->sub_searchPlaylistAll, p_data);
        }else if(p_pageCode==PAGECODE_Q_SEARCH_VIDEO_ALL_VIEW){
            //this->sub_searchVideoAll = this->procCommon_showSubPage<qobuz::QobuzSearchVidelAll>(false, this->sub_searchVideoAll, p_data);
        }
    }


    /**
     * @brief QobuzMain::procCommon_showSubPage
     * @param flagShow_topBar
     * @param sub_widget
     */
    template<typename T_QobuzSub> T_QobuzSub* QobuzMain::procCommon_showSubPage(const bool flagShow_topBar, T_QobuzSub* sub_widget, const QJsonObject &jsonObj_data){

        this->topMenuBar->setVisible(flagShow_topBar);
        //this->animate_fadeOut_forSubWidget(this->stackedWidget_content->currentWidget());
        if(sub_widget == nullptr){

            sub_widget = new T_QobuzSub();
            this->stackedWidget_content->addWidget(sub_widget);
            this->stackedWidget_content->addWidget(le_search_back);//c220730
            sub_widget->setConnectSiganl_toMovePage(this, SLOT(slot_clickedMoveSubPage(const QJsonObject&)));
            //sub_widget->setConnectSiganl_toShouldGoBackward(this, SLOT(goToBackPage));
        }

        stackedWidget_content->setCurrentWidget(sub_widget);
        if(ProcJsonEasy::getString(jsonObj_data, "pageCode") == PAGECODE_Q_EXPLORE || ProcJsonEasy::getString(jsonObj_data, "pageCode") == PAGECODE_Q_HOME){
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
     * @brief QobuzMain::animate_fadeIn_forSubWidget
     * @details 참고링크 @see https://stackoverflow.com/questions/19087822/how-to-make-qt-widgets-fade-in-or-fade-out/32985126
     * @param sub_pageWidget
     */
    void QobuzMain::animate_fadeIn_forSubWidget(QWidget *p_widget){

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
     * @brief QobuzMain::animate_fadeOut_forSubWidget
     * @details 참고링크 @see https://stackoverflow.com/questions/19087822/how-to-make-qt-widgets-fade-in-or-fade-out/32985126
     * @param sub_pageWidget
     */
    void QobuzMain::animate_fadeOut_forSubWidget(QWidget *p_widget){

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


    void QobuzMain::resizeEvent(QResizeEvent *event){

        AbstractMainContent::resizeEvent(event);
    }


    void QobuzMain::slot_completeReq_get_session_info(const RoseSessionInfo_forQobuz& sessionInfo){

        if(!sessionInfo.user_auth_token.isEmpty() && !sessionInfo.user_id.isEmpty()){
            qobuz::ProcCommon *procQobuz = new qobuz::ProcCommon(this);
            qobuz::UserLoginInfo qobuz_userLoginInfo = procQobuz->getLoginInfo_qobuzDB();

            // Rose 정보로 로그인 정보를 업데이트함
            global.user_forQobuz.setLogin(sessionInfo.user_auth_token, sessionInfo.email, sessionInfo.user_id, true);

            // DB 정보 처리
            /*qobuz_userLoginInfo.auth_token = sessionInfo.user_auth_token;
            qobuz_userLoginInfo.username = sessionInfo.email;
            qobuz_userLoginInfo.user_id = sessionInfo.user_id;
            qobuz_userLoginInfo.password = "";
            qobuz_userLoginInfo.flagSavedLoginInfo = true;

            procQobuz->saveLoginInfo_qobuzDB(qobuz_userLoginInfo);*/

            // Genre 종류 요청 (Qobuz 공통사용)
            procQobuz->request_qobuz_getListGenres();
        }

        this->setUIControl();
    }


    void QobuzMain::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

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
}
