#include "rosetube_viewall.h"
#include "common/networkhttp.h"
#include "common/global.h"
#include "widget/framerosetube.h"
#include "widget/framerosetubeplaylist.h"
#include "common/rosesettings.h"

#include <QSettings>
#include <QJsonArray>
#include <QWheelEvent>
#include <QScrollBar>

const int HTTP_EXPLORE = 99;
const int HTTP_RECENT_PLAY = 98;
const int HTTP_MY_PLAY_LIST = 97;
const int HTTP_FRIEND_PLAY_LIST = 96;
const int HTTP_ROSETUBE_BEST = 95;
const int HTTP_RECOMMEND = 94;
const int HTTP_RECOMMEND_GENRE = 93;
const int HTTP_COMMON = 92;
const int HTTP_FAVORITE_TRACKS = 91;
const int HTTP_NETWORK_PLAY = 90;

/**
 * @brief RoseTube_ViewAll::RoseTube_ViewAll : 로즈튜브 View ALL
 * @details 로즈튜브 관련 View All 위젯
 * {@link setData(QJsonObject)} Json 데이터의 type값을 이용해
 * 홈-최근재생 로즈튜브, 홈-My플레이리스트, 홈-친구플레이리스트 등을 구분한다.
 *
 * @param parent
 */
RoseTube_ViewAll::RoseTube_ViewAll(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief RoseTube_ViewAll::setInit : 초기 세팅
 */
void RoseTube_ViewAll::setInit(){
    linker = Linker::getInstance();
    connect(linker, SIGNAL(signal_myPlaylistChanged()), SLOT(slot_myPlaylistChanged()));
}

/**
 * @brief RoseTube_ViewAll::setUIControl : UI 세팅
 * @return
 */
void RoseTube_ViewAll::setUIControl(){

    // ------------------------------------------------------
    //  전체재생, 셔플재생 UI
    // ------------------------------------------------------
    QPixmap pixmap_play(":/images/icon_list_allplay.png");
    QPushButton *btn_play = new QPushButton;
    btn_play->setIcon(pixmap_play);
    btn_play->setIconSize(pixmap_play.rect().size());
    btn_play->setText(tr("Play All"));
    //btn_play->setText(tr("전체재생"));
    btn_play->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    btn_play->setCursor(Qt::PointingHandCursor);


    QPixmap pixmap_shuffle(":/images/icon_list_shuffle.png");
    QPushButton *btn_shuffle = new QPushButton;
    btn_shuffle->setIcon(pixmap_shuffle);
    btn_shuffle->setIconSize(pixmap_play.rect().size());
    btn_shuffle->setText(tr("Shuffle Play"));
    //btn_shuffle->setText(tr("셔플재생"));
    btn_shuffle->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    btn_shuffle->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *box_option = new QHBoxLayout;
    box_option->setAlignment(Qt::AlignLeft);
    box_option->setSpacing(30);
    box_option->setContentsMargins(0,0,0,0);
    box_option->addWidget(btn_play);
    box_option->addWidget(btn_shuffle);

    widget_option = new QWidget(this);
    widget_option->setContentsMargins(0,0,0,0);
    widget_option->setLayout(box_option);


    flowLayout = new FlowLayout(0,0,0);


    wg_ScrollArea = new QWidget;
    wg_ScrollArea->setContentsMargins(0,0,0,0);
    wg_ScrollArea->setLayout(flowLayout);

    scrollArea = new QScrollArea;
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(wg_ScrollArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("#scrollArea { border:0px; }");

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(80,10,70,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget_option);
    vl_total->addSpacing(10);
    vl_total->addWidget(scrollArea);

    setLayout(vl_total);

    // 커넥트
    connect(btn_play, SIGNAL(clicked()), this, SLOT(slot_clickedPlayAll()));
    connect(btn_shuffle, SIGNAL(clicked()), this, SLOT(slot_clickedPlayShuffle()));

}


/**
 * @brief RoseTube_ViewAll::setData : 데이터 세팅
 * @details QJsonObject의 type값 확인 후 Http 통신으로 타입에 관련된 데이터 요청
 * @param p_jsonObject
 */
void RoseTube_ViewAll::setData(const QJsonObject &p_jsonObject){

    const QString jsonKey_index = "index";
    const QString jsonKey_type = "type";
    const QString jsonKey_title = "title";

    QString tmp_title = "";
    QString tmp_type = "";
    int tmp_index = -1;

    if(p_jsonObject.contains(jsonKey_index)){
        tmp_index = p_jsonObject[jsonKey_index].toInt();
    }
    if(p_jsonObject.contains(jsonKey_type)){
        tmp_type = p_jsonObject[jsonKey_type].toString();
    }
    if(p_jsonObject.contains(jsonKey_title)){
        tmp_title = p_jsonObject[jsonKey_title].toString();
    }

    totalCount = 0;
    flagReqMore = false;
    type = tmp_type;
    title = tmp_title;
    exploreIndex = tmp_index;

    GSCommon::clearLayout(flowLayout);
    requestData();
}

/**
 * @brief RoseTube_ViewAll::appendRecentPlay : 홈-최근재생로즈튜브 아이템 세팅
 * @param p_jsonObject QJsonObject item info
 */
void RoseTube_ViewAll::appendRecentPlay(const QJsonObject &p_jsonObject){
    FrameRoseTube *tmp_widget = new FrameRoseTube(OptionPopup::TypeMenu::RoseTube_Common,true);
    tmp_widget->setData(p_jsonObject);
    tmp_widget->setHover();
    flowLayout->addWidget(tmp_widget);

    if(flowLayout->count() == this->current_count){
        flagReqMore = false;
        if(flowLayout->count() == this->totalCount){
            tmp_widget->setRequestTrackInfoAfterSetData();
        }
    }
}

/**
 * @brief RoseTube_ViewAll::appendMyPlayList : 홈-My플레이리스트 아이템 세팅
 * @param p_jsonObject QJsonObject item info
 */
void RoseTube_ViewAll::appendMyPlayList(const QJsonObject &p_jsonObject){
    const QString jsonKey_type = "type";

    if(p_jsonObject.contains(jsonKey_type) && p_jsonObject[jsonKey_type].toString()=="youtube"){
        FrameRoseTubePlayList *tmp_widget = new FrameRoseTubePlayList(OptionPopup::TypeMenu::RoseTube_Home_PlayList_My,true);
        tmp_widget->setData(p_jsonObject);
        tmp_widget->setHover();
        flowLayout->addWidget(tmp_widget);
    }
}

/**
 * @brief RoseTube_ViewAll::appendFriendPlayList : 친구 플레이리스트 앨범 추가
 * @param p_jsonObject
 */
void RoseTube_ViewAll::appendFriendPlayList(const QJsonObject &p_jsonObject){

    FrameRoseTubePlayList *tmp_widget = new FrameRoseTubePlayList(OptionPopup::TypeMenu::RoseTube_Home_PlayList_Friend,true);
    tmp_widget->setData(p_jsonObject);
    tmp_widget->setHover();
    flowLayout->addWidget(tmp_widget);
}


/**
 * @brief RoseTube_ViewAll::appendTOP10 : 로즈튜브 TOP 20
 * @param p_jsonObject QJsonObject response
 */
void RoseTube_ViewAll::appendRosetubeBest(const QJsonObject &p_jsonObject){

    FrameRoseTube *tmp_widget = new FrameRoseTube(OptionPopup::TypeMenu::RoseTube_Common,true);
    //tmp_widget->setRequestTrackInfoAfterSetData();
    tmp_widget->setData(p_jsonObject);
    tmp_widget->setHover();
    flowLayout->addWidget(tmp_widget);

    if(flowLayout->count() == this->current_count){
        flagReqMore = false;
        if(flowLayout->count() == this->totalCount){
            tmp_widget->setRequestTrackInfoAfterSetData();
        }
    }
}

/**
 * @brief RoseTube_ViewAll::appendRecommendList : 로즈튜브 추천리스트
 * @param p_jsonObject QJsonObject response
 */
void RoseTube_ViewAll::appendRecommendList(const QJsonObject &p_jsonObject){
    FrameRoseTubePlayList *tmp_widget = new FrameRoseTubePlayList(OptionPopup::TypeMenu::RoseTube_Home_PlayList_Friend,true);
    tmp_widget->setData(p_jsonObject);
    tmp_widget->setHover();
    flowLayout->addWidget(tmp_widget);
}

/**
 * @brief RoseTube_ViewAll::appendRecommendList : 로즈튜브 추천리스트
 * @param p_jsonObject QJsonObject response
 */
void RoseTube_ViewAll::appendRecommendGenre(const QJsonObject &p_jsonObject){
    FrameRoseTube *tmp_widget = new FrameRoseTube(OptionPopup::TypeMenu::RoseTube_Common,true);
    //tmp_widget->setRequestTrackInfoAfterSetData();
    tmp_widget->setData(p_jsonObject);
    tmp_widget->setHover();
    flowLayout->addWidget(tmp_widget);

    if(flowLayout->count() == this->current_count){
        flagReqMore = false;
        if(flowLayout->count() == this->totalCount){
            tmp_widget->setRequestTrackInfoAfterSetData();
        }
    }
}

/**
 * @brief RoseTube_ViewAll::appendEXPLORE_RoseTube : 탐색-로즈튜브 아이템 세팅
 * @param p_jsonObject
 */
void RoseTube_ViewAll::appendEXPLORE_RoseTube(const QJsonObject &p_jsonObject){
    //const QString jsonKey_nm = "nm";
    const QString jsonKey_URLS = "_URLs";
    const QString jsonKey_tracks = "tracks";

    if(p_jsonObject.contains(jsonKey_URLS)){
        QJsonArray jsonArrURLs = p_jsonObject[jsonKey_URLS].toArray();

        for(int i = 0 ; i < jsonArrURLs.size(); i++){
            QJsonObject jsonItems = jsonArrURLs.at(i).toObject();


            if(jsonItems.contains(jsonKey_tracks)){
                QJsonArray jsonArrTrack = jsonItems[jsonKey_tracks].toArray();
                for(int j=0; j < jsonArrTrack.size(); j++){

                    FrameRoseTube *tmp_widget = new FrameRoseTube(OptionPopup::TypeMenu::RoseTube_Common,true,this);
                    tmp_widget->setRequestTrackInfoAfterSetData();
                    tmp_widget->setData(jsonArrTrack.at(j).toObject());
                    tmp_widget->setHover();
                    flowLayout->addWidget(tmp_widget);
                }
            }
        }
    }
}

/**
 * @brief RoseTube_ViewAll::requestData : 컨텐츠 데이터 요청
 * @details type, index 정보로 판단하여 관련 API 호출
 * recentPlay: 홈-최근 재생 로즈튜브
 * explore 탐색-플레이리스트
 */
void RoseTube_ViewAll::requestData(){

    widget_option->hide();

    if(type=="recentPlay"){
        widget_option->show();
        requestRecentPlay();
    }else if(type=="myPlayList"){
        requestMyPlayList();
    }else if(type=="friendPlayList"){
        requestFriendPlayList();
    }else if(type =="Playlist1" || type =="Playlist2"){
        requestRecommendList();
    }else if(type =="Albumlist1" || type =="Albumlist2" || type =="Albumlist3" || type =="Albumlist4" || type =="Albumlist5" || type =="Albumlist6" || type =="Albumlist7"){
        if(title == "Rose Tube Best")
        {
            int send_no = 0;

            /*if(type == "Albumlist1"){
                send_no = global.ROSETUBE_API_ALBUM1;
            }
            else if(type == "Albumlist2"){
                send_no = global.ROSETUBE_API_ALBUM2;
            }
            else if(type == "Albumlist3"){
                send_no = global.ROSETUBE_API_ALBUM3;
            }
            else if(type == "Albumlist4"){
                send_no = global.ROSETUBE_API_ALBUM4;
            }
            else if(type == "Albumlist5"){
                send_no = global.ROSETUBE_API_ALBUM5;
            }
            else if(type == "Albumlist6"){
                send_no = global.ROSETUBE_API_ALBUM6;
            }
            else if(type == "Albumlist7"){
                send_no = global.ROSETUBE_API_ALBUM7;
            }*/

            widget_option->show();
            requestRosetubeBest(send_no);
        }
        else{
            requestRecommendGenre();
        }
    }else if(type=="explore"){
        widget_option->show();
        requestEXPLORE();
    }else if(type=="common"){
        widget_option->show();
        requestRoseCommon();
    }else if(type=="favoriteTrack"){
        widget_option->show();
        requestFavorTracks();
    }
    /*else if(type=="best"){
        widget_option->show();
        requestRosetubeBest();
    }else if(type=="recommend"){
        requestRecommendList();
    }else if(type=="Steinway" || type=="hot" || type=="vocal" || type=="kpop" || type=="pop"){
        requestRecommendGenre();
    }*/
}


/**
 * @brief RoseTube_ViewAll::requestRecentPlay : 홈-최근 재생 로즈튜브 정보 요청
 * @details 사용 API : {MOD_LAGACY_BETA}/playlist/fetch
 */
void RoseTube_ViewAll::requestRecentPlay(){
    if(!global.user.getAccess_token().isEmpty()){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        /*QUrlQuery params;
        params.addQueryItem("playlistno",global.user.getRosetubeRecentPlaylist());
        params.addQueryItem("username", global.user.getUsername());
        params.addQueryItem("type","youtube");
        //params.addQueryItem("range_start","0");
        //params.addQueryItem("range_end", "30");

        network->request(HTTP_RECENT_PLAY, QString("%1/playlist/track/fetch?range_start=%2&range_end=%3")
                         .arg(global.legacy_mod).arg(flowLayout->count()).arg(flowLayout->count() + 30), params, true, true);

        QUrlQuery params;
        params.addQueryItem("range_end", "25");
        if(this->flagReqMore == true){
            params.addQueryItem("range_start", QString("%1").arg(this->current_count));
        }
        else{
            params.addQueryItem("range_start", "0");
        }
        params.addQueryItem("playlistno", global.user.getRosetubeRecentPlaylist());

        network->request(HTTP_RECENT_PLAY, QString("%1/playlist/track/fetch")
                         .arg(global.legacy_mod_api), params, true, true);*/

        QJsonObject json;
        network->request(HTTP_RECENT_PLAY, QString("%1/member/track/recent?type=YOUTUBE&excludeMediaType=&page=0&size15=").arg(global.legacy_v1), json, false, true);

    }
}

/**
 * @brief RoseTube_ViewAll::requestMyPlayList : 홈-My 플레이리스트
 * @details 사용 API : {MOD_LAGACY_BETA}/playlist/all/mod
 */
void RoseTube_ViewAll::requestMyPlayList(){
    if(!global.user.getAccess_token().isEmpty()){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("type","youtube");
        params.addQueryItem("username",global.user.getUsername());
        //params.addQueryItem("range_start",QString::number(flowLayout->count()));
        //params.addQueryItem("range_end",QString::number(flowLayout->count()+20));

        network->request(HTTP_MY_PLAY_LIST, QString("%1/playlist/fetch?range_start=%2&range_end=%3")
                         .arg(global.legacy_mod).arg(flowLayout->count()).arg(flowLayout->count()+30), params, true, true);
    }
}

/**
 * @brief RoseTube_ViewAll::requestFriendPlayList
 * @details 사용 API : {MOD_LAGACY_BETA}/playlist/friend
 */
void RoseTube_ViewAll::requestFriendPlayList(){
    if(!global.user.getAccess_token().isEmpty()){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        int tmp_page = 0;
        tmp_page = flowLayout->count() / 30;

        QJsonObject json;
        json.insert("type", "youtube");
        json.insert("page", 0);
        json.insert("size", 100);
        network->request(HTTP_FRIEND_PLAY_LIST, QString("%1/playlist/friend?page=%1&size=30")
                         .arg(global.legacy_v3_api).arg(tmp_page)
                         , json, true, true);
    }
}

/**
 * @brief RoseTube_ViewAll::requestTOP10 : 로즈튜브 TOP 10 요청
 * @details 사용 API {MOD_LAGACY_BETA}/menu/get
 */
void RoseTube_ViewAll::requestRosetubeBest(int list_no){

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    //QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);

    /*QUrlQuery params;
    params.addQueryItem("name","rosetube_home");
    params.addQueryItem("lang",settings->value(rosesettings.SETTINGS_LANG,"kr").toString());

    network->request(HTTP_ROSETUBE_BEST, QString("%1/menu/get")
                     .arg(global.legacy_mod), params, true,true);*/

    QUrlQuery params;
    params.addQueryItem("range_end", "30");
    if(this->flagReqMore == true){
        params.addQueryItem("range_start", QString("%1").arg(this->current_count));
    }
    else{
        params.addQueryItem("range_start", "0");
    }
    params.addQueryItem("playlistno", QString("%1").arg(list_no));

    network->request(HTTP_ROSETUBE_BEST, QString("%1/playlist/track/fetch")
                     .arg(global.legacy_mod_api), params, true, true);
}

/**
 * @brief RoseTube_ViewAll::requestRecommendList : 로즈튜브 추천리스트 요청
 * @details 사용 API {MOD_LEGACY_BETA}/menu/get2
 */
void RoseTube_ViewAll::requestRecommendList(){

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    /*QUrlQuery params;
    params.addQueryItem("name","rosetube_home");

    network->request(HTTP_RECOMMEND, QString("%1/menu/get")
                     .arg(global.legacy_mod), params, true,true);*/

    QUrlQuery params;
    params.addQueryItem("name", "rosetube_home");
    params.addQueryItem("lang", "ko");

    network->request(HTTP_RECOMMEND, QString("%1/menu/get")
                     .arg(global.legacy_mod_api), params, true, true);
}

/**
 * @brief RoseTube_ViewAll::requestRecommendGenre : 로즈튜브 추천리스트 요청
 * @details 사용 API {MOD_LEGACY_BETA}/menu/get2
 */
void RoseTube_ViewAll::requestRecommendGenre(){

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    /*QUrlQuery params;
    params.addQueryItem("name","rosetube_home");

    network->request(HTTP_RECOMMEND, QString("%1/menu/get")
                     .arg(global.legacy_mod), params, true,true);*/

    QUrlQuery params;
    params.addQueryItem("range_end", "30");
    if(this->flagReqMore == true){
        params.addQueryItem("range_start", QString("%1").arg(this->current_count));
    }
    else{
        params.addQueryItem("range_start", "0");
    }

    /*if(type=="Albumlist1"){
        params.addQueryItem("playlistno", QString("%1").arg(global.ROSETUBE_API_ALBUM1));
    }
    else if(type=="Albumlist2"){
        params.addQueryItem("playlistno", QString("%1").arg(global.ROSETUBE_API_ALBUM2));
    }
    else if(type=="Albumlist3"){
        params.addQueryItem("playlistno", QString("%1").arg(global.ROSETUBE_API_ALBUM3));
    }
    else if(type=="Albumlist4"){
        params.addQueryItem("playlistno", QString("%1").arg(global.ROSETUBE_API_ALBUM4));
    }
    else if(type=="Albumlist5"){
        params.addQueryItem("playlistno", QString("%1").arg(global.ROSETUBE_API_ALBUM5));
    }
    else if(type=="Albumlist6"){
        params.addQueryItem("playlistno", QString("%1").arg(global.ROSETUBE_API_ALBUM6));
    }
    else if(type=="Albumlist7"){
        params.addQueryItem("playlistno", QString("%1").arg(global.ROSETUBE_API_ALBUM7));
    }*/

    network->request(HTTP_RECOMMEND_GENRE, QString("%1/playlist/track/fetch")
                     .arg(global.legacy_mod_api), params, true, true);
}

/**
 * @brief RoseTube_ViewAll::requestEXPLORE : 탐색-플레이리스트 정보 요청
 * @details 탐색-1 : Classical Music in 4k UHD
 * 탐색-2 : 드라마OST M/V
 * 탐색-1,탐색-2 공통 API 사용 {{MOD_LAGACY_BETA}}/menu/get?name=rosetube_explore
 */
void RoseTube_ViewAll::requestEXPLORE(){

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("name","rosetube_explore");

    network->request(HTTP_EXPLORE, QString("%1/menu/get")
                     .arg(global.legacy_mod), params, true,true);
}

/**
 * @brief RoseTube_ViewAll::requestRoseCommon : 탐색-플레이리스트 아이디를 이용한 정보 요청
 */
void RoseTube_ViewAll::requestRoseCommon(){
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("playlistno", QString::number(exploreIndex));
    //params.addQueryItem("range_start","0");
    //params.addQueryItem("range_end", "30");

    network->request(HTTP_COMMON, QString("%1/playlist/track/fetch?range_start=%2&range_end=%3")
                     .arg(global.legacy_mod_api).arg(flowLayout->count()).arg(flowLayout->count()+30), params, true,true);
}

/**
 * @brief RoseTube_ViewAll::requestFavorTracks
 */
void RoseTube_ViewAll::requestFavorTracks(){

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("playlistno","0");

    network->request(HTTP_FAVORITE_TRACKS, QString("%1/playlist/track/fetch?range_start=%2&range_end=%3")
                     .arg(global.legacy_mod_api).arg(flowLayout->count()).arg(flowLayout->count()+30)
                     , params, true,true);
}

/**
 * @brief RoseTube_ViewAll::requestPlayAll : 전체 재생 요청
 * @param p_flagShuffle
 * @note ChildWidget 타입이 Track인 경우에만 해당
 */
void RoseTube_ViewAll::requestPlayAll(bool p_flagShuffle){
    QJsonArray jsonArrYoutube;

    for(int i = 0 ; i < flowLayout->count(); i++){
        FrameRoseTube *widget = qobject_cast<FrameRoseTube*>(flowLayout->itemAt(i)->widget());
        QJsonObject jsonTrackData = widget->getData();


        QString videoID = jsonTrackData["playurl"].toString().split("?v=").last();

        QJsonObject jsonYoutube;
        jsonYoutube.insert("channelId", jsonTrackData["channelId"].toString());
        jsonYoutube.insert("channelName", jsonTrackData["channelTitle"].toString());        
        jsonYoutube.insert("duration", jsonTrackData["duration_convert"].toString());
        jsonYoutube.insert("id", videoID);
        jsonYoutube.insert("isFavorite", jsonTrackData["favorites"].toString()=="1" ? true : false);
        jsonYoutube.insert("isLiveStream",false);
        jsonYoutube.insert("thumbnailUrl", jsonTrackData["thumbnails"].toString());
        jsonYoutube.insert("thumbsUpPercentage",0);
        jsonYoutube.insert("title", jsonTrackData["title"].toString());
        jsonYoutube.insert("viewsCount", jsonTrackData["viewCount"].toString());

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

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/youtubePlay.playlist.add")
                     .arg(global.device.getDeviceIP()).arg(global.port)
                     , jsonParam, true);
}

/**
 * @brief RoseTube_ViewAll::setResultOfRecentPlay : 홈-최근재생로즈튜브 정보 요청 처리
 * @param p_jsonObject QJsonObject response
 */
void RoseTube_ViewAll::setResultOfRecentPlay(const QJsonObject &p_jsonObject){

    if(ProcJsonEasy::get_flagOk(p_jsonObject) == true){

        if(p_jsonObject.contains("totalcount")){
            totalCount = ProcJsonEasy::getInt(p_jsonObject, "totalcount");
        }

        QJsonArray jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObject, "tracks");

        this->current_count += jsonArrSub.size();
        if(jsonArrSub.size() > 0){
            for(int i=0; i < jsonArrSub.size(); i++){
                appendRecentPlay(jsonArrSub.at(i).toObject());
            }
        }
    }

    /*const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_playLists = "playlists";
    const QString jsonKey_tracks = "tracks";
    const QString jsonKey_totalcount = "totalcount";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_totalcount)){

            if(p_jsonObject.contains(jsonKey_totalcount)){
                totalCount = p_jsonObject[jsonKey_totalcount].toInt();
            }

            if(p_jsonObject.contains(jsonKey_tracks)){
                QJsonArray jsonArrTracks = p_jsonObject[jsonKey_tracks].toArray();

                for(int i = 0 ; i < jsonArrTracks.size(); i++){
                    appendRecentPlay(jsonArrTracks.at(i).toObject());
                }
            }
        }
    }*/
}

/**
 * @brief RoseTube_ViewAll::setResultOfMyPlayList
 * @details : {V3_LEGACY_BETA}/playlist/all/mod API 사용
 * 특이사항 : 로즈튜브 뿐만 아니라 모든 즐겨찾기 리스트 반환
 * API에 로즈튜브만 요청하는 param 없음.
 * @param p_jsonObject
 */
void RoseTube_ViewAll::setResultOfMyPlayList(const QJsonObject &p_jsonObject){


    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";
    const QString jsonKey_playlists = "playlists";
    const QString jsonKey_totalcount = "totalcount";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_playlists)){
            QJsonArray jsonArr = p_jsonObject[jsonKey_playlists].toArray();

            for(int i = 0; i < jsonArr.size(); i++){
                appendMyPlayList(jsonArr.at(i).toObject());
            }
        }
        if(p_jsonObject.contains(jsonKey_totalcount)){
            int tmp_totalCount = p_jsonObject[jsonKey_totalcount].toInt();

            if(flowLayout->count() < tmp_totalCount){
                requestMyPlayList();
            }
        }
    }
}

/**
 * @brief RoseTube_ViewAll::setResultOfTop20 : 로즈튜브 TOP 20 요청 결과 처리
 * @details 현재 TOP20 전용 API 없음
 * {{MOD_LAGACY_BETA}}/menu/get?name=rosetube_home API 사용
 * Rose Tube Best 데이터를 활용함.
 * @param p_jsonData QJsonObject response
 */
void RoseTube_ViewAll::setResultOfRosetubeBest(const QJsonObject &p_jsonData){

    //QJsonDocument doc(p_jsonData);
    //QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() << "ROSE TUBE RCV : " << strJson;

    if(ProcJsonEasy::get_flagOk(p_jsonData) == true){

        if(p_jsonData.contains("totalcount")){
            totalCount = ProcJsonEasy::getInt(p_jsonData, "totalcount");
        }

        QJsonArray jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonData, "tracks");

        this->current_count += jsonArrSub.size();
        if(jsonArrSub.size() > 0){
            for(int i=0; i < jsonArrSub.size(); i++){
                appendRosetubeBest(jsonArrSub.at(i).toObject());
            }
        }
    }

    /*const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_submenu = "submenu";
    const QString jsonKey_nm = "nm";
    const QString jsonKey_id = "id";
    const QString jsonKey_URLs = "_URLs";
    const QString jsonKey_tracks = "tracks";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool(false)){
        if(p_jsonData.contains(jsonKey_submenu)){
            QJsonArray jsonArrSub = p_jsonData[jsonKey_submenu].toArray();

            /// Rose Tube BEST는 API 목록에서 항상 0번째 위치
            if(jsonArrSub.size() > 0){
                QJsonObject jsonData = jsonArrSub.at(0).toObject();

                if(jsonData.contains(jsonKey_URLs)){
                    QJsonArray jsonArrAll = jsonData[jsonKey_URLs].toArray();

                    if(jsonArrAll.size() > 0){
                        QJsonObject jsonBest = jsonArrAll.at(0).toObject();

                        if(jsonBest.contains(jsonKey_tracks)){

                            QJsonArray jsonArrBest = jsonBest[jsonKey_tracks].toArray();

                            for(int i=0; i < jsonArrBest.size(); i++){
                                appendTOP10(jsonArrBest.at(i).toObject());
                            }
                        }
                    }
                }
            }
        }
    }*/
}

/**
 * @brief RoseTube_ViewAll::setResultOfRecommend : 추천리스트 정보 처리
 * @param p_jsonData
 */
void RoseTube_ViewAll::setResultOfRecommendList(const QJsonObject &p_jsonData){

    if(ProcJsonEasy::get_flagOk(p_jsonData) == true){
        QJsonArray jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonData, "submenu");

        if(jsonArrSub.size() > 0){
            for(int i = 0; i < jsonArrSub.size(); i++){
                QJsonObject tmpJsonObj = jsonArrSub.at(i).toObject();

                if(ProcJsonEasy::getString(tmpJsonObj, "nm") == "Rose Cafe Recommended Music"){
                    if(tmpJsonObj.contains("_URLs")){
                        QJsonArray tmpJsonArr = ProcJsonEasy::getJsonArray(tmpJsonObj, "_URLs");

                        if(tmpJsonArr.size() > 0){
                            for(int i = 0; i < tmpJsonArr.count(); i++){
                                 appendRecommendList(tmpJsonArr.at(i).toObject());
                            }
                        }
                    }
                }
            }
        }
    }

    /*const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_submenu = "submenu";
    const QString jsonKey_nm = "nm";
    const QString jsonKey_id = "id";
    const QString jsonKey_URLs = "_URLs";
    const QString jsonKey_tracks = "tracks";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool(false)){
        if(p_jsonData.contains(jsonKey_submenu)){
            QJsonArray jsonArrSub = p_jsonData[jsonKey_submenu].toArray();

            /// 로즈튜브 추천리스트는 API 목록에서 항상 1번째 위치
            if(jsonArrSub.size() > 1){
                QJsonObject jsonData = jsonArrSub.at(1).toObject();

                if(jsonData.contains(jsonKey_URLs)){

                    QJsonArray jsonArrAll = jsonData[jsonKey_URLs].toArray();

                    if(jsonArrAll.size() > 0){
                        for(int i =0; i < jsonArrAll.size(); i++){
                             appendRecommendList(jsonArrAll.at(i).toObject());
                        }
                    }
                }
            }
        }
    }*/
}

void RoseTube_ViewAll::setResultOfRecommendGenre(const QJsonObject &p_jsonData){

    //QJsonDocument doc(p_jsonData);
    //QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() << "ROSE TUBE RCV : " << strJson;

    if(ProcJsonEasy::get_flagOk(p_jsonData) == true){

        if(p_jsonData.contains("totalcount")){
            totalCount = ProcJsonEasy::getInt(p_jsonData, "totalcount");
        }

        QJsonArray jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonData, "tracks");

        this->current_count += jsonArrSub.size();
        if(jsonArrSub.size() > 0){
            for(int i=0; i < jsonArrSub.size(); i++){
                appendRosetubeBest(jsonArrSub.at(i).toObject());
            }
        }
    }
}

/**
 * @brief RoseTube_ViewAll::setResultOfFriendPlayList
 * @details : {V3_LEGACY_BETA}/playlist/friend API 사용
 * 특이사항 : 음악-홈-친구리스트와 동일 API
 * @param p_jsonObject
 */
void RoseTube_ViewAll::setResultOfFriendPlayList(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_playLists = "playlists";
    const QString jsonKey_totalcount = "totalcount";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_totalcount)){

            if(p_jsonObject.contains(jsonKey_totalcount)){
                totalCount = p_jsonObject[jsonKey_totalcount].toInt(0);
            }

            if(p_jsonObject.contains(jsonKey_playLists)){
                QJsonArray jsonArr = p_jsonObject[jsonKey_playLists].toArray();

                for(int i = 0; i < jsonArr.size(); i++){
                    QJsonObject jsonPlayList = jsonArr.at(i).toObject();
                    appendFriendPlayList(jsonArr.at(i).toObject());
                }
            }

        }
    }
}

/**
 * @brief RoseTube_ViewAll::setResultOfEXPLORE : 탐색-플레이리스트 정보 요청 처리
 * @param p_jsonObject QJsonObject response
 */
void RoseTube_ViewAll::setResultOfEXPLORE(const QJsonObject &p_jsonObject){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_submenu = "submenu";
    const QString jsonKey_type = "type";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_submenu)){
            QJsonArray jsonArrSub = p_jsonObject[jsonKey_submenu].toArray();


            int countPlayList = 0;
            for(int i=0; i < jsonArrSub.size(); i++){
                QJsonObject jsonCategory = jsonArrSub.at(i).toObject();

                /// API 반환값에는 장르별/상황별 정보 외에 플레이리스트 정보도 포함되어 있다.
                if(jsonCategory.contains(jsonKey_type)){
                    QString type = jsonCategory[jsonKey_type].toString().toLower();

                    if(type=="roseplaylist"){

                        if(exploreIndex==countPlayList){
                            appendEXPLORE_RoseTube(jsonCategory);
                            break;
                        }
                        countPlayList++;
                    }
                }
            }
        }
    }
}

/**
 * @brief RoseTube_ViewAll::setResultOfRoseCommon : 플레이리스트 곡 정보 요청 결과 처리
 * @param p_jsonObject
 */
void RoseTube_ViewAll::setResultOfRoseCommon(const QJsonObject &p_jsonObject){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_tracks = "tracks";
    const QString jsonKey_totalCount = "totalcount";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){

        if(p_jsonObject.contains(jsonKey_totalCount)){
            totalCount = p_jsonObject[jsonKey_totalCount].toInt();
        }

        if(p_jsonObject.contains(jsonKey_tracks)){
            QJsonArray jsonArrTrack = p_jsonObject[jsonKey_tracks].toArray();
            for(int j=0; j < jsonArrTrack.size(); j++){
                FrameRoseTube *tmp_widget = new FrameRoseTube(OptionPopup::TypeMenu::RoseTube_Common,true,this);
                tmp_widget->setData(jsonArrTrack.at(j).toObject());
                tmp_widget->setHover();
                flowLayout->addWidget(tmp_widget);
            }
        }
    }
}

/**
 * @brief RoseTube_ViewAll::slot_responseHttp : [슬롯] Http 요청 결과 처리
 * @param p_id 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void RoseTube_ViewAll::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id==HTTP_EXPLORE){
        setResultOfEXPLORE(p_jsonObject);
    }else if(p_id==HTTP_RECENT_PLAY){
        setResultOfRecentPlay(p_jsonObject);
    }else if(p_id==HTTP_MY_PLAY_LIST){
        setResultOfMyPlayList(p_jsonObject);
    }else if(p_id==HTTP_FRIEND_PLAY_LIST){
        setResultOfFriendPlayList(p_jsonObject);
    }else if(p_id==HTTP_ROSETUBE_BEST){
        setResultOfRosetubeBest(p_jsonObject);
    }else if(p_id==HTTP_RECOMMEND){
        setResultOfRecommendList(p_jsonObject);
    }else if(p_id==HTTP_RECOMMEND_GENRE){
        setResultOfRecommendGenre(p_jsonObject);
    }else if(p_id==HTTP_COMMON){
        setResultOfRoseCommon(p_jsonObject);
    }else if(p_id==HTTP_FAVORITE_TRACKS){
        setResultOfRoseCommon(p_jsonObject);
        flagReqMore = false;
    }

    sender()->deleteLater();
}

/**
 * @brief RoseTube_ViewAll::slot_clickedPlayAll : [슬롯] 전체 재생
 */
void RoseTube_ViewAll::slot_clickedPlayAll(){

    requestPlayAll();
}

/**
 * @brief RoseTube_ViewAll::slot_clickedPlayShuffle : [슬롯]
 */
void RoseTube_ViewAll::slot_clickedPlayShuffle(){

    requestPlayAll(true);
}

/**
 * @brief RoseTube_ViewAll::wheelEvent : 휠 이벤트
 * @param event
 */
void RoseTube_ViewAll::wheelEvent(QWheelEvent *event){

    // -------------------------------
    //  pixelDelta().y() always 0
    // -------------------------------
    // 더 가져오기 실행

    if(event->delta() < 0){
        if(!flagReqMore && flowLayout->count() <= this->totalCount){
            flagReqMore = true;
            requestData();
        }
    }
}


/**
 * @brief RoseTube_ViewAll::slot_myPlaylistChanged : [슬롯] My 플레이리스트 변경
 */
void RoseTube_ViewAll::slot_myPlaylistChanged(){
    GSCommon::clearLayout(flowLayout);
    requestData();
}
