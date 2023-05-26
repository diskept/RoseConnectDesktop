#include "roseHome/rosehomeMain.h"

#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"
#include "common/global.h"

#include "widget/toastmsg.h"

#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QMetaObject>
#include <QResizeEvent>


namespace roseHome {

    const int HTTP_USERPICK_PLAYLIST = 8;

    /**
     * @brief roseHome 메인 생성자
     * @param parent
     */
    roseHomeMain::roseHomeMain(QWidget *parent) : AbstractMainContent(parent){

        this->linker = Linker::getInstance();
        connect(this->linker, &Linker::signal_clicked_movePage, this, &roseHomeMain::goToMoveNewOrderPage);
        connect(this->linker, SIGNAL(signal_dragEnterEvent_hide_show(bool)), SLOT(slot_dragEnterEvent_hide_show(bool)));//c220730
        connect(this->linker, SIGNAL(signal_dropEvent_hide_show(bool)), SLOT(slot_dropEvent_hide_show(bool)));//c220730

        this->make_CustomLineEdit();//c220730

        //this->le_search_back->setFixedSize(global.width_mainwindow-200,global.height_mainwindow);
        //make_CustomLineEdit();//c220730

        /*this->le_search_back = new CustomLineEdit();//c220727
        this->le_search_back->setObjectName("le_search");
        //this->le_search_back->setStyleSheet("#le_search { background-color:white; border-radius:20px; font-size:15px; color:#333333; padding-left:35px;}");
        this->le_search_back->setStyleSheet("#le_search { background-color:#333333;  font-size:23px; color:#333333; padding-left:35px;}");
        this->le_search_back->setMaxLength(200);//c220803
        this->le_search_back->setText("Supports drag and drop function. Drop it here.");
        if(global.width_mainwindow==0){
            this->le_search_back->setFixedSize(800,500);
        }else{
            this->le_search_back->setFixedSize(global.width_mainwindow-200,global.height_mainwindow);
        }

        this->le_search_back->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
        this->le_search_back->hide();*/

        this->setUIControl();
    }


    roseHomeMain::~roseHomeMain(){

        this->deleteLater();
    }


    void roseHomeMain::slot_overrideSigalSearch(bool b){//c220728
        print_debug();
        if(b){
            emit linker->signal_clickedMovePageRoseTubeSearch();
        }else{
            slot_dragEnterEvent_hide_show(false);
            emit linker->signal_checkQueue(30, "");//c220729
        }
    }


    void roseHomeMain::slot_dragEnterEvent_hide_show(bool show){//c220826_1
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


    void roseHomeMain::slot_dragEnterEvent_restore(){//c220826_1
        print_debug();
        this->curr_widget = this->stackedWidget_content->currentWidget();//c220824_4
        this->le_search_back->clear();
        emit linker->signal_searchBarFocusChanged(false);
        this->le_search_back->clearFocus();
        this->curr_widget->show();
        this->le_search_back->hide();
    }

    void roseHomeMain::setUIControl(){
        QString pageCode_firstPage = PAGECODE_RH_HOME;
        QJsonObject jsonObj_first;
        jsonObj_first[KEY_PAGE_CODE] = pageCode_firstPage;

        this->goToMoveNewOrderPage(jsonObj_first);
    }


    /**
     * @brief roseHome 각 하위 페이지 클래스 마다, 발생하는 signal_clickedMovePage 시그널을 공통으로 처리한다.
     * @param p_jsonData : 요청한 페이지에 대한 정보가 담겨있다. JsonObject 형식. @n
     *    @li KEY_PAGE_CODE : 해당 페이지 클래스에 대응되는 Unique한 값이다. @see gscommon.h 파일에 정의.
     *    @li KEY_DATA : 옵션 JsonObject. 해당 페이지 클래스에 그대로 넘겨서 보내줄 데이터이다. {} 일 수 있음.
     */
    void roseHomeMain::slot_clickedMoveSubPage(const QJsonObject &p_jsonData){

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
    QJsonObject roseHomeMain::getJsonObject_forMainTitle(){
        QJsonObject jsonObj;
        jsonObj["name"] = tr("RoseHome");
        jsonObj[KEY_MAIN_CODE] = GSCommon::MainMenuCode::RoseHome;

        return jsonObj;
    }


    /**
     * @brief TopMenuBar 객체에게 전달하기 위한 Sub 타이틀 정보 여러개를 JsonArray 로 생성하여 반환
     * @return QJsonArray* :
     */
    QJsonArray roseHomeMain::getJsonArray_forSubTitles(){
        QJsonArray p_jsonArray_titlSub ;
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Home")}, {"code", PAGECODE_RH_HOME} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Favorite")}, {"code", PAGECODE_RH_FAVORITE} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("User Picks")}, {"code", PAGECODE_RH_USERPICK} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Friend")}, {"code", PAGECODE_RH_FRIEND} });

        return p_jsonArray_titlSub;
    }


    /**
     * @brief roseHomeMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
     * @param p_pageCode
     */
    void roseHomeMain::movePageOnly(QString p_pageCode, const QJsonObject &p_data){

        QString pageCode = "";
        QJsonObject data;

        if(global.user.isValid() == false && (p_pageCode != PAGECODE_RH_HOME)){
            pageCode = PAGECODE_RH_HOME;
            data[KEY_PAGE_CODE] = PAGECODE_RH_HOME;
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


       // make_CustomLineEdit();//c220730

        print_debug();
        qDebug() << "pageCode=" << pageCode;
        // 서브 메뉴페이지에 대한 처리
        if(pageCode == PAGECODE_RH_HOME){
            this->sub_home = this->procCommon_showSubPage<roseHome::RoseHome>(true, this->sub_home, data);
        }        
        else if(pageCode == PAGECODE_RH_FAVORITE){
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
        }

        else if(pageCode == PAGECODE_RH_ALBUM_DETAIL){
            sub_albumDetail = this->procCommon_showSubPage<roseHome::RoseHomeAlbumDetail>(false, this->sub_albumDetail, data);
        }
        else if(pageCode == PAGECODE_RH_ALBUM_LIST_ALL_VIEW){
            this->sub_albumAll = this->procCommon_showSubPage<roseHome::RoseHomeAlbumListAll>(false, this->sub_albumAll, data);
        }
        else if(pageCode == PAGECODE_RH_ARTIST_DETAIL){
            this->sub_artistDetail = this->procCommon_showSubPage<roseHome::RoseHomeArtistDetail>(false, this->sub_artistDetail, data);
        }
        else if(pageCode == PAGECODE_RH_ARTIIST_LIST_ALL_VIEW){
            this->sub_artistAll = this->procCommon_showSubPage<roseHome::RoseHomeArtistListAll>(false, this->sub_artistAll, data);
        }
        else if(pageCode == PAGECODE_RH_PLAYLIST_DETAIL){
            this->sub_playlistDetail = this->procCommon_showSubPage<roseHome::RoseHomePlaylistDetail>(false, this->sub_playlistDetail, data);
        }
        else if(pageCode == PAGECODE_RH_PLAYLIST_ROSE_DETAIL){
            this->sub_playlistDetail_rose = this->procCommon_showSubPage<roseHome::RoseHomePlaylistDetail_Rose>(false, this->sub_playlistDetail_rose, data);
        }
        else if(pageCode == PAGECODE_RH_HISTORY_DETAIL){
            this->sub_historyDetail = this->procCommon_showSubPage<roseHome::RoseHomeHisotryDetail>(false, this->sub_historyDetail, data);
        }
        else if(pageCode == PAGECODE_RH_PLAYLIST_LIST_ALL_VIEW){
            this->sub_playlistAll = this->procCommon_showSubPage<roseHome::RoseHomePlaylistListAll>(false, this->sub_playlistAll, data);
        }
        else if(pageCode == PAGECODE_RH_TRACK_LIST_ALL_VIEW){
            this->sub_tracklistAll = this->procCommon_showSubPage<roseHome::RoseHomeTrackListAll>(false, this->sub_tracklistAll, data);
        }
        else if(pageCode == PAGECODE_RH_RECENTLY_LIST_DELETE){
            this->sub_roseRecentlyDelete = this->procCommon_showSubPage<roseHome::RoseHomeRecentlyListDelete>(false, this->sub_roseRecentlyDelete, data);
        }
        else if(pageCode == PAGECODE_RH_TRACK_SHARE_LIST_ALL_VIEW){
            this->sub_tracklistAll_share = this->procCommon_showSubPage<roseHome::RoseHomeTrackListAll_Share>(false, this->sub_tracklistAll_share, data);
        }
        else if(pageCode == PAGECODE_RH_ROSETUBE_LIST_ALL_VIEW){
            this->sub_rosetubeAll = this->procCommon_showSubPage<roseHome::RoseHomeRosetubeListAll>(false, this->sub_rosetubeAll, data);
        }
        else if(pageCode == PAGECODE_RH_PLAYLIST_HISTORY_ALL_VIEW){
            this->sub_playlistHistoryAll = this->procCommon_showSubPage<roseHome::RoseHomePlaylistHistoryAll>(false, this->sub_playlistHistoryAll, data);
        }
        else if(pageCode == PAGECODE_RH_ALBUM_HISTORY_ALL_VIEW){
            this->sub_albumHistoryAll = this->procCommon_showSubPage<roseHome::RoseHomeAlbumHistoryAll>(false, this->sub_albumHistoryAll, data);
        }
        else if(pageCode == PAGECODE_RH_TRACK_HISTORY_ALL_VIEW){
            this->sub_trackHistroyAll = this->procCommon_showSubPage<roseHome::RoseHomeTrackHistoryAll>(false, this->sub_trackHistroyAll, data);
        }
        else if(pageCode == PAGECODE_RH_ARTIST_HISTORY_ALL_VIEW){
            this->sub_artistHistroyAll = this->procCommon_showSubPage<roseHome::RoseHomeArtistHistoryAll>(false, this->sub_artistHistroyAll, data);
        }
        else if(pageCode == PAGECODE_RH_HISTORY_LIST_VIEW){
            this->sub_historyAll = this->procCommon_showSubPage<roseHome::RoseHomeHisotryListAll>(false, this->sub_historyAll, data);
        }
        else if(pageCode == PAGECODE_RH_ADD_PLAYLIST){
            this->sub_roseAddPlaylist = this->procCommon_showSubPage<roseHome::RoseHomeAddPlaylist>(false, this->sub_roseAddPlaylist, data);
        }
    }

    /**
     * @brief QobuzMain::procCommon_showSubPage
     * @param flagShow_topBar
     * @param sub_widget
     */
    template<typename T_roseHomeSub> T_roseHomeSub* roseHomeMain::procCommon_showSubPage(const bool flagShow_topBar, T_roseHomeSub* sub_widget, const QJsonObject &jsonObj_data){

        this->topMenuBar->setVisible(flagShow_topBar);

        if(sub_widget == nullptr || ProcJsonEasy::getString(jsonObj_data, "pageCode") == PAGECODE_RH_ARTIST_DETAIL){        //  || ProcJsonEasy::getString(jsonObj_data, "pageCode") ==  PAGECODE_RH_RECENTLY_LIST_DELETE
            print_debug();
            sub_widget = new T_roseHomeSub();
            this->stackedWidget_content->addWidget(sub_widget);
            this->stackedWidget_content->addWidget(le_search_back);//c220730
            sub_widget->setConnectSiganl_toMovePage(this, SLOT(slot_clickedMoveSubPage(const QJsonObject&)));
        }

        QString tmpPageCode = ProcJsonEasy::getString(jsonObj_data, "pageCode");

        stackedWidget_content->setCurrentWidget(sub_widget);
        if(tmpPageCode == PAGECODE_RH_FAVORITE){
            sub_widget->setJsonObject_forData(jsonObj_data);
        }
        else{
            sub_widget->setJsonObject_forData(ProcJsonEasy::getJsonObject(jsonObj_data, KEY_DATA));
        }
        sub_widget->setActivePage();

        return sub_widget;
    }


    void roseHomeMain::requestGetRoseHome_forUserpickInfo(){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        QJsonObject json;
        network->request(30011, QString("%1/member/playlist/userpick/all?sortType=PLAYLIST_RECENT&page=0&size=5").arg(global.legacy_v1), json, false, true);
        print_debug();
        qDebug() << "userpick_apicmd--" << QString("%1/member/playlist/userpick/all?sortType=PLAYLIST_RECENT&page=0&size=5").arg(global.legacy_v1);

    //HTTP_ROSEHOME_USERPICK_GET
    }




    void roseHomeMain::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

        print_debug();
        //QJsonDocument doc(p_jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "roseHomeMain::slot_responseHttp--p_jsonObject= " << strJson;

        //Q_UNUSED(p_jsonObj);
        if(p_id == HTTP_USERPICK_PLAYLIST){
            //print_debug();
            //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "roseHomeMain::slot_responseHttp--p_jsonObject= " << strJson;

        }else if(p_id == 3344){//c220727
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
