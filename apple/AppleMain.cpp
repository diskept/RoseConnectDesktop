#include "apple/AppleMain.h"

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


namespace apple {

    /**
     * @brief APPLE 메인 생성자
     * @param parent
     */
    AppleMain::AppleMain(QWidget *parent) : AbstractMainContent(parent){

        linker = Linker::getInstance();

        connect(linker, &Linker::signal_clicked_movePage, this, &AppleMain::goToMoveNewOrderPage);

        this->setUIControl();
    }


    AppleMain::~AppleMain(){

        this->deleteLater();
    }


    void AppleMain::setUIControl(){

        QString pageCode_firstPage = PAGECODE_APPLE_HOME;
        QJsonObject jsonObj_first;
        jsonObj_first[KEY_PAGE_CODE] = pageCode_firstPage;

        this->goToMoveNewOrderPage(jsonObj_first);
    }


    /**
     * @brief Apple Music 각 하위 페이지 클래스 마다, 발생하는 signal_clickedMovePage 시그널을 공통으로 처리한다.
     * @param p_jsonData : 요청한 페이지에 대한 정보가 담겨있다. JsonObject 형식. @n
     *    @li KEY_PAGE_CODE : 해당 페이지 클래스에 대응되는 Unique한 값이다. @see gscommon.h 파일에 정의.
     *    @li KEY_DATA : 옵션 JsonObject. 해당 페이지 클래스에 그대로 넘겨서 보내줄 데이터이다. {} 일 수 있음.
     */
    void AppleMain::slot_clickedMoveSubPage(const QJsonObject &p_jsonData){

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
    QJsonObject AppleMain::getJsonObject_forMainTitle(){

        QJsonObject jsonObj;
        jsonObj["name"] = tr("Apple Music");
        jsonObj[KEY_MAIN_CODE] = GSCommon::MainMenuCode::AppleMusic;

        return jsonObj;
    }


    /**
     * @brief TopMenuBar 객체에게 전달하기 위한 Sub 타이틀 정보 여러개를 JsonArray 로 생성하여 반환
     * @return QJsonArray* :
     */
    QJsonArray AppleMain::getJsonArray_forSubTitles(){

        QJsonArray p_jsonArray_titlSub ;
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Home")}, {"code", PAGECODE_APPLE_HOME} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Explore")}, {"code", PAGECODE_APPLE_EXPLORE} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Settings")}, {"code", PAGECODE_APPLE_SETTINGS} });

        return p_jsonArray_titlSub;
    }


    /**
     * @brief roseHomeMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
     * @param p_pageCode
     */
    void AppleMain::movePageOnly(QString p_pageCode, const QJsonObject &p_data){

        QString pageCode = "";
        QJsonObject data;

        if(global.user.isValid() == false && (p_pageCode != PAGECODE_APPLE_HOME)){
            pageCode = PAGECODE_APPLE_HOME;
            data[KEY_PAGE_CODE] = PAGECODE_APPLE_HOME;
        }
        else{
            pageCode = p_pageCode;
            data = p_data;
        }

        QJsonObject tmp_jsonObj_titleMain = this->getJsonObject_forMainTitle();
        QJsonArray tmp_jsonArr_titlSub = this->getJsonArray_forSubTitles();

        // 부모 virtual 함수 호출
        AbstractMainContent::movePageOnly(pageCode, data);

        if(p_pageCode == "playfullscreen"){
            tmp_jsonObj_titleMain = QJsonObject();
            tmp_jsonArr_titlSub = QJsonArray();

            this->topMenuBar->setDataTopMenuBar(tmp_jsonObj_titleMain, &tmp_jsonArr_titlSub);
        }
        else{
            // 자식 override 함수 서술
            this->topMenuBar->setSelectedSubMenu(pageCode);
            this->topMenuBar->setDataTopMenuBar(tmp_jsonObj_titleMain, &tmp_jsonArr_titlSub);
        }
        this->topMenuBar->setVisible(true);


        // 서브 메뉴페이지에 대한 처리
        if(pageCode == PAGECODE_APPLE_HOME){
            this->sub_home = this->procCommon_showSubPage<apple::AppleHome>(true, this->sub_home, data);
        }
        /*else if(pageCode == PAGECODE_RH_FAVORITE){
            this->sub_favorite = this->procCommon_showSubPage<roseHome::RoseFavorite>(true, this->sub_favorite, data);
        }
        else if(pageCode == PAGECODE_RH_USERPICK){//cheon211015
            print_debug();
            //this->sub_userpick  = nullptr;
            this->sub_userpick = this->procCommon_showSubPage<roseHome::RoseHomeUserPick>(true, this->sub_userpick, data);
        }
        else if(pageCode == PAGECODE_RH_USERPAGE){//c0124
            print_debug();
            this->sub_userpage = this->procCommon_showSubPage<roseHome::userpickUserpage>(true, this->sub_userpage, data);
        }
        else if(pageCode == PAGECODE_RH_FRIEND){//cheon211018
            this->sub_friend = this->procCommon_showSubPage<roseHome::RoseHomeFriend>(true, this->sub_friend, data);
        }*/

        else if(pageCode == PAGECODE_APPLE_HOME_RECENTLY_ALBUM){
            this->sub_albumListAll_rose = this->procCommon_showSubPage<apple::AppleAlbumListAll_Rose>(false, this->sub_albumListAll_rose, data);
        }
        else if(pageCode == PAGECODE_APPLE_HOME_RECENTLY_PLAYLIST){
            this->sub_playlistListAll_rose = this->procCommon_showSubPage<apple::ApplePlaylistListAll_Rose>(false, this->sub_playlistListAll_rose, data);
        }
        else if(pageCode == PAGECODE_APPLE_HOME_RECENTLY_TRACK){
            this->sub_trackListAll_rose = this->procCommon_showSubPage<apple::AppleTrackListAll_Rose>(false, this->sub_trackListAll_rose, data);
        }
        else if(pageCode == PAGECODE_APPLE_HOME_MY_PLAYLIST){
            this->sub_myplaylistListAll_rose = this->procCommon_showSubPage<apple::AppleMyPlaylistListAll_Rose>(false, this->sub_myplaylistListAll_rose, data);
        }
        else if(pageCode == PAGECODE_APPLE_HOME_USER_PLAYLIST){
            this->sub_userplaylistListAll_rose = this->procCommon_showSubPage<apple::AppleUserPlaylistListAll_Rose>(false, this->sub_userplaylistListAll_rose, data);
        }

        else if(pageCode == PAGECODE_APPLE_ALBUM_DETAIL){
            sub_albumDetail = this->procCommon_showSubPage<apple::AppleAlbumDetail>(false, this->sub_albumDetail, data);
        }
        else if(pageCode == PAGECODE_APPLE_PLAYLIST_DETAIL){
            this->sub_playlistDetail = this->procCommon_showSubPage<apple::ApplePlaylistDetail>(false, this->sub_playlistDetail, data);
        }
        else if(pageCode == PAGECODE_APPLE_PLAYLIST_ROSE_DETAIL){
            this->sub_playlistDetail_rose = this->procCommon_showSubPage<apple::ApplePlaylistDetail_Rose>(false, this->sub_playlistDetail_rose, data);
        }
        /*else if(pageCode == PAGECODE_RH_ALBUM_LIST_ALL_VIEW){
            this->sub_albumAll = this->procCommon_showSubPage<roseHome::RoseHomeAlbumListAll>(false, this->sub_albumAll, data);
        }
        else if(pageCode == PAGECODE_RH_PLAYLIST_LIST_ALL_VIEW){
            this->sub_playlistAll = this->procCommon_showSubPage<roseHome::RoseHomePlaylistListAll>(false, this->sub_playlistAll, data);
        }
        else if(pageCode == PAGECODE_RH_TRACK_LIST_ALL_VIEW){
            this->sub_tracklistAll = this->procCommon_showSubPage<roseHome::RoseHomeTrackListAll>(false, this->sub_tracklistAll, data);
        }
        else if(pageCode == PAGECODE_RH_ROSETUBE_LIST_ALL_VIEW){
            this->sub_rosetubeAll = this->procCommon_showSubPage<roseHome::RoseHomeRosetubeListAll>(false, this->sub_rosetubeAll, data);
        }
        else if(pageCode == PAGECODE_RH_ADD_PLAYLIST){
            this->sub_roseAddPlaylist = this->procCommon_showSubPage<roseHome::RoseHomeAddPlaylist>(false, this->sub_roseAddPlaylist, data);
        }*/
    }

    /**
     * @brief QobuzMain::procCommon_showSubPage
     * @param flagShow_topBar
     * @param sub_widget
     */
    template<typename T_AppleMusicHomeSub> T_AppleMusicHomeSub* AppleMain::procCommon_showSubPage(const bool flagShow_topBar, T_AppleMusicHomeSub* sub_widget, const QJsonObject &jsonObj_data){

        this->topMenuBar->setVisible(flagShow_topBar);

        if(sub_widget == nullptr){
            sub_widget = new T_AppleMusicHomeSub();
            this->stackedWidget_content->addWidget(sub_widget);
            sub_widget->setConnectSiganl_toMovePage(this, SLOT(slot_clickedMoveSubPage(const QJsonObject&)));
        }
        stackedWidget_content->setCurrentWidget(sub_widget);

        if(ProcJsonEasy::getString(jsonObj_data, "pageCode") == PAGECODE_APPLE_HOME || ProcJsonEasy::getString(jsonObj_data, "pageCode") == PAGECODE_APPLE_EXPLORE){
            sub_widget->setJsonObject_forData(jsonObj_data);
        }
        else{
            sub_widget->setJsonObject_forData(ProcJsonEasy::getJsonObject(jsonObj_data, KEY_DATA));
        }
        sub_widget->setActivePage();

        return sub_widget;
    }
}
