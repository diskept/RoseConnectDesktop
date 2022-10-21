#include "home/abstractmaincontent.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/commonGetRedirectUrl.h"    //j220903 twitter

#include "widget/AbstractAddPlaylist_RHV.h"
#include "widget/toastmsg.h"//c220730

#include <QGraphicsOpacityEffect>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>


const int HTTP_FAVORITE_ADD = 99;
const int HTTP_FAVORITE_RADIO_IN_OTHER = 98;
const int HTTP_FAVORITE_RADIO_IN_FAVOR = 97;
const int HTTP_FAVORITE_PLAY_LIST = 96;
const int MIN_WIDTH_STACKWIDGET = 944;

/**
 * @brief AbstractMainContent::AbstractMainContent 생성자
 * @param parent
 */
AbstractMainContent::AbstractMainContent(QWidget *parent) : MyQWidget(parent)
{
    this->setInit();
    this->setUIControlBasic();
    this->setUIPlayScreenBG();
}

AbstractMainContent::~AbstractMainContent(){
    this->deleteLater();
}

/**
 * @brief AbstractMainContent::setInit : 초기 세팅 함수
 */
void AbstractMainContent::setInit(){

    this->indexPageOrdering_curr = -1;

    // Set linker
    linker = Linker::getInstance();
    connect(linker, &Linker::signal_connected, this, &AbstractMainContent::slot_linker_connectedDevice);
    connect(linker, SIGNAL(signal_showPlayScreenBG(QPixmap)), this, SLOT(slot_showBG(QPixmap)));
    connect(linker, &Linker::signal_addPlaylistFinish, this, &AbstractMainContent::goToBackPage);

    // 음악 플레이리스트 담기 PROC
    proc_add_music = new Proc_AddTackInPlayList(this);
    connect(proc_add_music, SIGNAL(signal_playlistClicked(int)), this, SLOT(slot_playlistClickedMusic(int)));
    connect(proc_add_music, SIGNAL(signal_newPlaylistNameEdited(QString)), this, SLOT(slot_newPlaylistNameEditedMusic(QString)));

    // 로즈튜브 플레이리스트 담기 PROC
    proc_add_rosetube = new Proc_AddRosetubeInPlayList(this);
    connect(proc_add_rosetube, SIGNAL(signal_playlistClicked(int)), this, SLOT(slot_playlistClickedRosetube(int)));
    connect(proc_add_rosetube, SIGNAL(signal_newPlaylistNameEdited(QString)), this, SLOT(slot_newPlaylistNameEditedRosetube(QString)));
}

/**
 * @brief AbstractMainContent::setUIControlBasic UI 세팅
 */
void AbstractMainContent::setUIControlBasic(){

    this->setStyleSheet("background-color:#212121");

    this->list_pageCodeOrdering = new QList<QString>;
    this->list_pageDataOrdering = new QList<QJsonObject>;


    // 로즈튜브에서 사용하는 배경 위젯
    widget_roseTubeBG = new QWidget();
    widget_roseTubeBG->setObjectName("widget_roseTubeBG");
    widget_roseTubeBG->setStyleSheet("#widget_roseTubeBG { background-color:#171717; }");
    widget_roseTubeBG->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    lb_roseTubeBG = new QLabel(widget_roseTubeBG);
    lb_roseTubeBG->setStyleSheet("background-color:transparent;");
    lb_roseTubeBG->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lb_roseTubeBG->hide();

    // 네비 Bar
    topNaviBar = new TopNaviBar(widget_roseTubeBG);
    widget_roseTubeBG->setFixedHeight(topNaviBar->height());

    // 상단 메뉴 Bar
    topMenuBar = new TopMenuBar();

    // 실 컨텐츠 레이아웃
    stackedWidget_content = new QStackedWidget();
    stackedWidget_content->setStyleSheet("background-color:transparent;");
    stackedWidget_content->setMinimumWidth(MIN_WIDTH_STACKWIDGET);


    // TEST >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // 음악/비디오/라디오/로즈튜브 등등 전체 메인메뉴에서 공통으로 사용하는 Page
    this->playFullScreenRelation = new PlayFullScreenRelation();
    connect(this->playFullScreenRelation, SIGNAL(signal_mustChangeMainMenuAndMovePage(int, QJsonObject)), this, SLOT(slot_mustChangeMainMenuAndMovePage(int, QJsonObject)));        // added by sunnyfish
    this->stackedWidget_content->addWidget(this->playFullScreenRelation);
    // TEST <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    // 레이아웃
    vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(this->widget_roseTubeBG, 0, Qt::AlignTop);
    vl_total->addSpacing(1);
    vl_total->addWidget(this->topMenuBar, 0, Qt::AlignTop);
    vl_total->addWidget(this->stackedWidget_content, 9);

    wg_total = new QWidget();
    wg_total->setObjectName("sectionContent");
    wg_total->setLayout(vl_total);

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);
    vlayout->addWidget(wg_total);

    this->setLayout(vlayout);


    // 커넥트
    connect(this->topMenuBar, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(goToMoveNewOrderPage(QJsonObject)) );
    connect(this->topNaviBar, SIGNAL(clickedBack()), this, SLOT(goToBackPage()));
    connect(this->topNaviBar, SIGNAL(clickedNext()), this, SLOT(goToNextPage()));
    connect(this->topNaviBar, SIGNAL(clickedRemote(bool)), this, SIGNAL(showRemoteWidget(bool)));

    connect(linker, SIGNAL(signal_clickedHoverItem(QString, QJsonObject)), this, SLOT(slot_clickedHoverItem(QString, QJsonObject)));
    connect(linker, SIGNAL(signal_clickedMovePage(QJsonObject)), this, SLOT(goToMoveNewOrderPage(QJsonObject)));

}

/**
 * @brief AbstractMainContent::setUIPlayScreenBG : 전체재생 화면 BG UI 세팅
 */
void AbstractMainContent::setUIPlayScreenBG(){
    // -----------------------------------------------------
    //  전체재생 배경
    // -----------------------------------------------------
    QGraphicsBlurEffect *effect = new QGraphicsBlurEffect();
    effect->setBlurRadius(30);

    lb_BG = new QLabel(wg_total);
    lb_BG->setGraphicsEffect(effect);
    lb_BG->lower();
    lb_BG->hide();

    lb_BGOpacity = new QLabel(lb_BG);
    lb_BGOpacity->setStyleSheet("background-color: rgba(0,0,0,180);");
}

/**
 * @brief AbstractMainContent::showBG : 재생화면 BG Show
 */
void AbstractMainContent::showPlayScreenBG(){

    // ----------------------------------------------------------------
    //  현재 전체재생 화면인 경우에만 show
    // ----------------------------------------------------------------
    if(stackedWidget_content->currentWidget()==playFullScreenRelation){
        lb_BG->resize(wg_total->width(), wg_total->height());
        lb_BGOpacity->resize(wg_total->width(), wg_total->height());
        lb_BG->show();
        lb_BGOpacity->show();
        widget_roseTubeBG->setStyleSheet("#widget_roseTubeBG {background-color:transparent;}");
        topMenuBar->setBGTransparent(true);
    }
}

/**
 * @brief AbstractMainContentForPodcast::hideBG : 재생화면 채널 BG Hide
 */
void AbstractMainContent::hidePlayScreenBG(){
    lb_BG->hide();
    lb_BGOpacity->hide();
    widget_roseTubeBG->setStyleSheet("#widget_roseTubeBG {background-color:#171717;}");
    topMenuBar->setBGTransparent(false);
}

/**
 * @brief AbstractMainContentForPodcast::slot_showBG : [슬롯] 팟캐스트 BG SHow 요청
 * @param pixmap QPixmap
 */
void AbstractMainContent::slot_showBG(const QPixmap &pixmap){

    if(pixmap.isNull()==false){
        lb_BG->setPixmap(pixmap.scaled(this->width(),this->height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }

    showPlayScreenBG();
}

/**
 * @brief AbstractMainContent::slot_mustChangeMainMenuAndMovePage
 * @param p_mainMenuCode
 * @param p_data
 */
void AbstractMainContent::slot_mustChangeMainMenuAndMovePage(int p_mainMenuCode, const QJsonObject &p_data){

   emit signal_mustChangeMainMenuAndMovePage(p_mainMenuCode, p_data);
}


/**
 * @brief AbstractMainContent::hideRemoteWidget 리모콘 위젯 아이콘 hide 해라
 * @param flagShow
 */
void AbstractMainContent::showhideRemoteWidget(bool flagShow){

    if(flagShow==true){
        this->topNaviBar->showRemoveWidget();
    }else{
        this->topNaviBar->hideRemoveWidget();
    }
}


/**
 * @brief AbstractMainContent::resetCurrPlayData 현재 재생중인 음원 정보가 변경되었다
 * @param p_jsonData
 */
void AbstractMainContent::resetCurrPlayData(const QJsonObject &p_jsonData){

    this->playFullScreenRelation->setDataPlayFullScreen(p_jsonData);
}


/**
 * @brief AbstractMainContent::slot_clickedHoverItem [SLOT] 앨범 hover 위 more/play/fav 아이콘을 클릭시 처리
 * @param p_code 더보기/재생/하트 (more/play/fav)
 * @param p_jsonData
 */
void AbstractMainContent::slot_clickedHoverItem(QString p_code, QJsonObject p_jsonData){


    const QString jsonKey_data_fav = "data_fav";
    const QString jsonKey_flag_favor_channel = "flagFavorChannel";


    // hide가 안되고 새로 생성되는 경우도 잇어서..항상 생성 직전에 delete
    for(int i=0; i<this->list_popup.count(); i++){
        this->list_popup.at(i)->hide();
        this->list_popup.at(i)->deleteLater();
        this->list_popup.clear();
    }

    // OptionPopup 멤버변수로 선언하여 불필요한 지속적인 생성을 막으려 했지만..
    // QMenu 특성(?)으로 인한 SizeHint를 이용하여 pos 값을 결정하는데 SizeHint가 제대로 세팅되지 않아 어려움이 있어(show전후로 크기가 다름뿐 아니라 값도 정확치않음) 매번 새로 생성하는것으로 대체하여 SizeHint가 예상된 값으로 나온다.
    OptionPopup *optionPopupNew = new OptionPopup();
    connect(optionPopupNew, SIGNAL(signal_clickedMovePage(QJsonObject)), this, SLOT(goToMoveNewOrderPage(QJsonObject)));
    connect(optionPopupNew, SIGNAL(signal_clickedMovePage_mustMainMenu(int, QJsonObject)), this, SIGNAL(signal_mustChangeMainMenuAndMovePage(int, QJsonObject)));

    this->list_popup.append(optionPopupNew);

    // 옵션팝업에 데이터 세팅
    if(p_jsonData.contains(KEY_MAIN_CODE)==false){
        p_jsonData.insert(KEY_MAIN_CODE, this->topMenuBar->getCurrMainMenuCode());  // key 추가
    }
    optionPopupNew->setDataPopup(p_jsonData);

    if(p_code=="more"){

        QPoint pos_mainWindow = QWidget::mapFromGlobal(this->pos());
        QPoint pos = QCursor::pos();
        int tmp_posX = pos.x();
        int tmp_posY = pos.y();

        if(  (tmp_posX - (-pos_mainWindow.x()) + optionPopupNew->sizeHint().width() ) > global.width_mainwindow ){
            tmp_posX = tmp_posX - optionPopupNew->sizeHint().width();
        }
        if( tmp_posY + optionPopupNew->sizeHint().height() > (-pos_mainWindow.y()) + global.height_mainwindow){
            int tmp_over = (tmp_posY + optionPopupNew->sizeHint().height()) - ((-pos_mainWindow.y()) + global.height_mainwindow);
            tmp_posY = tmp_posY - tmp_over;
        }

        optionPopupNew->showMenu(tmp_posX, tmp_posY);

    }else if(p_code=="play"){
        // ----------------------------------------------------------------------
        //  바로듣기
        //  옵션팝업에 데이터를 more과 동일하게 세팅해서 옵션팝업의 바로 듣기와 동일한 함수 호출하게 함
        // ----------------------------------------------------------------------
        if(p_jsonData.contains("rosetubeData")){
            QJsonObject tmpJson = ProcJsonEasy::getJsonObject(p_jsonData, "rosetubeData");
            optionPopupNew->slot_clickedPlayRoseTube(tmpJson);
        }
        else{
            optionPopupNew->slot_clickedPlay();
        }
    }else if(p_code=="fav"){
        if(p_jsonData.contains(KEY_OP_TYPE)){
            // 라디오 즐겨찾기 설정/해제
            if(p_jsonData[KEY_OP_TYPE].toInt() == OptionPopup::TypeMenu::Radio){
                if(p_jsonData.contains(jsonKey_data_fav) && p_jsonData.contains(jsonKey_flag_favor_channel)){
                    bool flagIsFavorChannel = p_jsonData[jsonKey_flag_favor_channel].toBool();
                    requestFavoriteForRadio(flagIsFavorChannel, p_jsonData[jsonKey_data_fav].toObject());
                }
            }
        }

    }else if(p_code=="addPlayList"){

        // ----------------------------------------------------------------------
        //  플레이리스트 담기
        //  음악 : 트랙 FULL 정보 필요
        //  로즈튜브 : 재생 URL 필요
        // ----------------------------------------------------------------------
        if(currMainMenuCode == GSCommon::MainMenuCode::Music){

            QJsonArray jsonArrTracks;
            if(p_jsonData.contains(KEY_OP_MUSIC_PLAYLIST_ADD_DATA)){
                jsonArrTracks = p_jsonData[KEY_OP_MUSIC_PLAYLIST_ADD_DATA].toArray();
            }

            proc_add_music->setTrackJsonArray(jsonArrTracks);
            proc_add_music->showDlgOfPlayList();
        }else if(currMainMenuCode == GSCommon::MainMenuCode::RoseTube){
            QString strPlayUrl;
            if(p_jsonData.contains(KEY_OP_ROSETUBE_PLAYURL)){
                strPlayUrl = p_jsonData[KEY_OP_ROSETUBE_PLAYURL].toString();
            }

            proc_add_rosetube->setPlayUrl(strPlayUrl);
            proc_add_rosetube->showDlgOfPlayList();
        }
    }
}

/**
 * @brief AbstractMainContent::requestFavoriteForMyPlayList : 음악-홈-마이플레이리스트 즐겨찾기 설정/해제
 * @details  API :{{MOD_LAGACY_BETA}}/playlist/favorites/add
             API :{{MOD_LAGACY_BETA}}/playlist/favorites/delete
 * @param p_playListNo int 플레이리스트 no
 * @param p_flagAdd bool if true, 즐겨찾기 설정, false, 즐겨찾기 해제
 */
//void AbstractMainContent::requestFavoriteForMyPlayList(const int &p_playListNo, const bool &p_flagAdd){
void AbstractMainContent::requestFavoriteForMyPlayList(const int &p_playListNo){


    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("playlistno", QString::number(p_playListNo));
    params.addQueryItem("username", global.user.getUsername());

    QString tmp_sub_url = global.api_playlist_favorite_add;
    QString tmp_url = QString("%1%2").arg(global.legacy_mod).arg(tmp_sub_url);



    network->request(HTTP_FAVORITE_PLAY_LIST, tmp_url, params, true, true);
}

/**
 * @brief AbstractMainContent::requestFavoriteForRadio : 라디오 즐겨찾기 설정/해제
 * @param p_jsonObject QJsonObject, 즐겨찾기 설정/해제 fav param
 */
void AbstractMainContent::requestFavoriteForRadio(const bool &flagIsFavorChannel, const QJsonObject &p_jsonObject){

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject json;
    json.insert("fav", p_jsonObject);

    int tmp_reqId = HTTP_FAVORITE_RADIO_IN_OTHER;
    if(flagIsFavorChannel)
        tmp_reqId = HTTP_FAVORITE_RADIO_IN_FAVOR;

    network->request(tmp_reqId, QString("http://%1:%2/set.radio.fav.url")
                     .arg(global.device.getDeviceIP())
                     .arg(global.port)
                     , json, true);
}

/**
 * @brief AbstractMainContent::setResultOfFavoriteForRadio : 라디오 즐겨찾기 설정/해제 요청 결과 처리
 * @param p_jsonObject
 */
void AbstractMainContent::setResultOfFavoriteForRadio(const bool &p_flagIsFavor, const QJsonObject &p_jsonData){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonData[jsonKey_status].toObject();
            // TODO:: Search Devices 16/02/2021 by disket
            //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){

                emit linker->signal_changedRadioFavorChannel(p_flagIsFavor);
            //}
        }
    }
}

/**
 * @brief AbstractMainContent::requestFavoriteAdd 하트 설정하는 API 호출
 * @param p_jsonObject
 */
void AbstractMainContent::requestFavoriteAdd(QJsonObject p_jsonObject, DataPopup *p_dataPopup, QString p_albumImg){


    Q_UNUSED(p_jsonObject);


    if(!global.device.getDeviceIP().isEmpty()){

        QJsonArray tmp_jsonArrayThumb;
        QJsonObject tmp_dataThumb;
        tmp_dataThumb.insert("url", p_albumImg);
        tmp_jsonArrayThumb.append(tmp_dataThumb);
        QJsonArray tmp_jsonArrayArtist;
        QJsonObject tmp_dataArtist;
        tmp_dataArtist.insert("name", p_dataPopup->getArtist());
        tmp_jsonArrayArtist.append(tmp_dataArtist);
        QJsonArray tmp_jsonArray;
        QJsonObject tmp_data;
        //tmp_data.insert("album", dataPopup->getAlbum());
        //tmp_data.insert("artists", tmp_jsonArrayArtist);
        //tmp_data.insert("client_id", dataPopup->getId());
        //tmp_data.insert("isFavorite", false);
        //tmp_data.insert("isSelected", false);
        tmp_data.insert("mac_address", global.device.getDeviceID());
        //tmp_data.insert("thumbnails", tmp_jsonArrayThumb);
        //tmp_data.insert("title", dataPopup->getTitle());
        tmp_data.insert("type", "music");
        tmp_data.insert("url", p_dataPopup->getData());
        tmp_jsonArray.append(tmp_data);

        QJsonObject tmp_json;
        tmp_json.insert("favorite",true);
        tmp_json.insert("local_ip",global.device.getDeviceIP());
        tmp_json.insert("mac_address",global.device.getDeviceID());
        tmp_json.insert("tracks",tmp_jsonArray);


        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_FAVORITE_ADD, QString("%1%2").arg(global.legacy_v3).arg("/track/favorite/add"), tmp_json, true, true);

    }
}

/**
 * @brief AbstractMainContent::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void AbstractMainContent::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){
print_debug();
qDebug() << "p_id = " << p_id;
    if(p_id == HTTP_FAVORITE_ADD){
        print_debug();
    }
    else if(p_id == HTTP_FAVORITE_RADIO_IN_FAVOR){
        print_debug();
        setResultOfFavoriteForRadio(true, p_jsonObj);
    }
    else if(p_id == HTTP_FAVORITE_RADIO_IN_OTHER){
        print_debug();
        setResultOfFavoriteForRadio(false, p_jsonObj);
    }
    else if(p_id == HTTP_FAVORITE_PLAY_LIST){
        print_debug();
        // nothing
    }

    sender()->deleteLater();
}
/**
 * @brief AbstractMainContent::goToBackPage [SLOT][virtual] 이전 페이지 이동
 */
void AbstractMainContent::goToBackPage(){

    QString tmp_pageCode2 = this->list_pageCodeOrdering->at(this->indexPageOrdering_curr);

    this->indexPageOrdering_curr--;

    QString tmp_pageCode = this->list_pageCodeOrdering->at(this->indexPageOrdering_curr);
    QJsonObject tmp_data = this->list_pageDataOrdering->at(this->indexPageOrdering_curr);;

    if(tmp_pageCode2 == "albumInfo" && tmp_pageCode == "album"){
        tmp_data.insert("pageCode2", tmp_pageCode2);
    }
    else if(tmp_pageCode2 == "albumInfo" && tmp_pageCode == "viewAll_addedAlbum"){
        tmp_data.insert("pageCode2", tmp_pageCode2);
    }
    else if(tmp_pageCode2 == "artistInfo" && tmp_pageCode == "viewAll_recommendArtist"){
        tmp_data.insert("pageCode2", tmp_pageCode2);
    }
    else if(tmp_pageCode2 == "videoInfo" && tmp_pageCode == "home"){
        tmp_data.insert("pageCode2", tmp_pageCode2);
    }
    else if(tmp_pageCode2 == "search" && tmp_pageCode == "home"){
        tmp_data.insert("pageCode2", tmp_pageCode2);
    }
    else if(tmp_pageCode2 == "T_ADD_PLAYLIST" || tmp_pageCode2 == "Q_ADD_PLAYLIST" || tmp_pageCode2 == "RH_ADD_PLAYLIST"){
        this->list_pageCodeOrdering->removeAt(this->indexPageOrdering_curr + 1);
        this->list_pageDataOrdering->removeAt(this->indexPageOrdering_curr + 1);
    }

    //qDebug() << tmp_pageCode << tmp_data;

    // 기존페이지로 이동 및 기데이터 set
    this->movePageOnly(tmp_pageCode, tmp_data);

    if(this->indexPageOrdering_curr <= 0){
        this->topNaviBar->setEnabledBtnBack(false);
    }

    if((this->list_pageCodeOrdering->count() - 1) <= this->indexPageOrdering_curr){
        this->topNaviBar->setEnabledBtnNext(false);
    }else{
        this->topNaviBar->setEnabledBtnNext(true);
    }
}

/**
 * @brief AbstractMainContent::goToNextPage [SLOT][virtual] 다음 페이지 이동
 */
void AbstractMainContent::goToNextPage(){

    this->indexPageOrdering_curr++;
    QString tmp_pageCode = this->list_pageCodeOrdering->at(this->indexPageOrdering_curr);
    QJsonObject tmp_data = this->list_pageDataOrdering->at(this->indexPageOrdering_curr);

    // 기존페이지로 이동 및 기데이터 set
    this->movePageOnly(tmp_pageCode, tmp_data);

    if(this->indexPageOrdering_curr<=0){
        this->topNaviBar->setEnabledBtnBack(false);
    }else{
        this->topNaviBar->setEnabledBtnBack(true);
    }
    if((this->list_pageCodeOrdering->count()-1) <= this->indexPageOrdering_curr){
        this->topNaviBar->setEnabledBtnNext(false);
    }else{
        this->topNaviBar->setEnabledBtnNext(true);
    }
}


/**
 * @brief AbstractMainContent::goToMoveNewOrderPage [SLOT] 페이지 이동 (기존 list_pageCodeOrdering 없는 신규 페이지 이동)
 * @details 내부적으로 movePageOnly함수를 호출한다. 즉 순수 페이지 이동하는 것과 함께 back/next버튼 스타일과 list_pageCodeOrdering 관련 처리가 함께 서술됨
 * @param p_pageCode
 */
void AbstractMainContent::goToMoveNewOrderPage(const QJsonObject &p_data){
print_debug();
qDebug() << "p_data=" << p_data;
    QString tmp_pageCode = p_data[KEY_PAGE_CODE].toString();
    QString tmp_contentStep = "";

    if(indexPageOrdering_curr >= 0){
        //qDebug() << tmp_pageCode << list_pageCodeOrdering->at(indexPageOrdering_curr);
        if(tmp_pageCode == list_pageCodeOrdering->at(indexPageOrdering_curr) && global.user_forBugs.dlg_isShow() == true){
            global.user_forBugs.dlg_set_state(false);
            return;
        }
        else if(tmp_pageCode == list_pageCodeOrdering->at(indexPageOrdering_curr) && global.user_forTidal.dlg_isShow() == true){
            global.user_forTidal.dlg_set_state(false);
            return;
        }
    }

    if(p_data.contains(KEY_CONTENT_STEP)){
        tmp_contentStep = p_data[KEY_CONTENT_STEP].toString();
    }

    // 2) 페이지 기억
    this->goToMoveNewOrderPage_subStep(p_data);

    // 3) 기존페이지가 아닌 새페이지 이동 및 데이터 init
    this->movePageOnly(tmp_pageCode, p_data);

}
/**
 * @brief AbstractMainContent::goToMoveNewOrderPage_subStep
 * @param p_data
 */
void AbstractMainContent::goToMoveNewOrderPage_subStep(const QJsonObject &p_data){

    QString tmp_pageCode = p_data[KEY_PAGE_CODE].toString();
    QString tmp_contentStep = "";
    if( p_data.contains(KEY_CONTENT_STEP)){
        tmp_contentStep = p_data[KEY_CONTENT_STEP].toString();
    }
    {

        // back하여 중간에서 새로 시작되는 경우 처리
        if(this->indexPageOrdering_curr < (this->list_pageCodeOrdering->count()-1)){
            // 현재 index로부터 이후에 담겨있는 orderPage 삭제
            for(int i=(this->list_pageCodeOrdering->count()-1); i>this->indexPageOrdering_curr; i--){
                this->list_pageCodeOrdering->removeAt(i);
                this->list_pageDataOrdering->removeAt(i);
            }
        }

        bool flagAddMemoryPage = true;
        if(tmp_pageCode==PAGECODE_ETC_PLAYFULLSCREEN || tmp_pageCode==PAGECODE_M_SEARCH){

            // 예외상황 : playfullscreen 은 이전에 저장된 기억된 페이지 기록은 삭제하고 방금 클릭한 것을 유효한것으로 기록 한다.
            for(int i=0; i<this->list_pageCodeOrdering->count(); i++){
                if(this->list_pageCodeOrdering->at(i)==tmp_pageCode){
                    this->list_pageCodeOrdering->removeAt(i);
                    this->list_pageDataOrdering->removeAt(i);
                    this->indexPageOrdering_curr--;
                    //flagAddMemoryPage = false;
                    break;
                }
            }
        }

        if(flagAddMemoryPage==true){
            this->list_pageCodeOrdering->append(tmp_pageCode);
            this->list_pageDataOrdering->append(p_data);
            this->indexPageOrdering_curr++;
            if(this->list_pageCodeOrdering->count()>1){
                this->topNaviBar->setEnabledBtnBack(true);
            }
            this->topNaviBar->setEnabledBtnNext(false);
        }
    }
}

/**
 * @brief AbstractMainContent::movePageOnly [virtual] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
 * @details virtual 에서는 공통적으로 이동되는 페이지만을 서술한다.
 *          override 에서는 각자 사용하는 페이지를 이동하는 내용을 서술한다.
 * @param p_pageCode
 * @param p_data
 */
void AbstractMainContent::movePageOnly(QString p_pageCode, const QJsonObject &p_data){

    Q_UNUSED(p_data);
    hidePlayScreenBG();

    // HomeMain에서 리모콘이랑 큐 위젯 hide 하기 위해 시그널 호출하였음..
    emit changedSubMenu(p_pageCode);
    this->topNaviBar->search_text_copy();//c220726
    //this->topNaviBar->search_text_clear();//c220726
    //print_debug();qDebug() << "###########################################################################################################";

    if(p_pageCode==PAGECODE_ETC_PLAYFULLSCREEN){
        this->topMenuBar->setSelectedSubMenu(p_pageCode);
        this->topMenuBar->setDataTopMenuBar(QJsonObject(), new QJsonArray());
        this->topMenuBar->setVisible(false);

        // [시그널발생] 이 화면은 외부에서 세팅하는 구조가 아니라 .. 화면이 그려질때.. SectionBottom에게 물어봐서 현재 재생정보를 세팅하는 구조로 변경함..
        emit signal_getCurrPlayData();
        stackedWidget_content->setCurrentWidget(playFullScreenRelation);
    }
}



/**
 * @brief AbstractMainContent::slot_playlistClicked : [슬롯] 플레이리스트 선택 다이얼로그-플레이리스트 클릭
 * @param p_playlistno int playlist no
 * @note 음악
 */
void AbstractMainContent::slot_playlistClickedMusic(const int &p_playlistno){

    if(p_playlistno >0){
        proc_add_music->requestAddPlayListTrack(p_playlistno);
    }else{
        proc_add_music->showDlgOfNewPlayList();
    }
}

/**
 * @brief AbstractMainContent::slot_playlistClickedRosetube : [슬롯] 플레이리스트 선택 다이얼로그-플레이리스트 클릭
 * @param p_playlistno int playlist no
 * @note 로즈튜브
 */
void AbstractMainContent::slot_playlistClickedRosetube(const int &p_playlistno){

    if(p_playlistno > 0){
        proc_add_rosetube->requestAddVideoInPlaylist(p_playlistno);
    }else{
        proc_add_rosetube->showDlgOfNewPlayList();
    }
}

/**
 * @brief AbstractMainContent::slot_newPlaylistNameEdited : [슬롯] 신규 플레이리스트 명 입력 완료
 * @param p_name
 * @note 음악, 신규 플레이리스트 생성
 */
void AbstractMainContent::slot_newPlaylistNameEditedMusic(QString p_name){

    proc_add_music->requestNewPlaylistTrack(p_name);
}

/**
 * @brief AbstractMainContent::slot_newPlaylistNameEditedRosetube : [슬롯] 신규 플레이리스트 명 입력 완료
 * @param p_name
 * @note 로즈튜브, 신규 플레이리스트 생성
 */
void AbstractMainContent::slot_newPlaylistNameEditedRosetube(QString p_name){

    proc_add_rosetube->requestNewVideoInPlaylist(p_name);
}

void AbstractMainContent::make_CustomLineEdit(){//c220730
    print_debug();
    qDebug() << "global.width_mainwindow=" << global.width_mainwindow;
    this->le_search_back = new CustomLineEdit();//c220727
    this->le_search_back->setObjectName("le_search");
    //this->le_search_back->setStyleSheet("#le_search { background-color:white; border-radius:20px; font-size:15px; color:#333333; padding-left:35px;}");
    this->le_search_back->setStyleSheet("#le_search { background-color:#333333;  font-size:23px; color:#333333; padding-left:35px;}");
    this->le_search_back->setMaxLength(200);//c220803
    //this->le_search_back->setText("Supports drag and drop function. Drop it here.");//c220930
    //this->le_search_back->setFixedSize(global.width_mainwindow-200,global.height_mainwindow);//c220930
    this->le_search_back->setFixedSize(this->width(),global.height_mainwindow);//c220930
    this->le_search_back->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
    this->le_search_back->hide();
}


void AbstractMainContent::slot_dropEvent_hide_show(bool show){//c220730
    print_debug();
    QTimer::singleShot(500, this, SLOT(slot_get_dragDropText()));


}

void AbstractMainContent::slot_overrideSigalSearch(bool b){//c220730

}

QString AbstractMainContent::getUrlResponse(QString tmp_str){//c220818

    print_debug();
    QUrl url(QString("%1").arg(tmp_str));
    QNetworkRequest networkRequest(url);
    QNetworkAccessManager manager;
    QEventLoop event;
    QObject::connect(&manager, &QNetworkAccessManager::finished, &event, &QEventLoop::quit);
/*
    connect(&manager, &QNetworkAccessManager::finished,
        this, [=](QNetworkReply *reply) {
            if (reply->error()) {
                qDebug() << reply->errorString();
                return;
            }
            QString response = reply->readAll();
            qDebug() << "+++++++++++++" << response;
        }
    );
*/
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    networkRequest.setSslConfiguration(config);
    auto* const pResponse = manager.get(networkRequest);
    event.exec();
    pResponse->deleteLater();
/*
connect(manager, &QNetworkAccessManager::finished,
    this, [=](QNetworkReply *reply) {
        if (reply->error()) {
            qDebug() << reply->errorString();
            return;
        }
        QString response = reply->readAll();
        qDebug() << response;
    }
);
 */
    //qDebug() << "pResponse->readAll()---" << pResponse->readAll();
    return pResponse->readAll();
}

//#include <QNetworkRequest>

void AbstractMainContent::slot_redirectUrl(const QString url){  //j220903 twitter

    if(this->redirectUrl_before != url){
        this->redirectUrl_before = url;
        this->le_search_back->setText(url);
        this->slot_get_dragDropText();
    }
    else if(this->redirectUrl_before == url){
        this->redirectUrl_before = "";
    }
}

void AbstractMainContent::slot_get_dragDropText(){//c220820
    print_debug();

    //QString videoID = jsonTrackData["playurl"].toString().split("?v=").last();
    //le_search->displayText();
    qDebug() << "le_search->text()=" << this->le_search_back->text();
    if(this->le_search_back->text().size() < 2){
        return;
    }

    // Drag and Drop Error fixed 10/17/2022 by diskept
    QString strInput = this->le_search_back->text();
    QString strRelplace = "";
    QString tmp_str = "";

    if(strInput.contains("\n")){
        QStringList removeNewline = strInput.split("\n");
        strRelplace = removeNewline.at(0);
    }
    else{
        strRelplace = strInput;
    }

    if(strRelplace.contains("\t")){
        QStringList removeTab = strRelplace.split("\t");
        tmp_str = removeTab.at(0);
    }
    else{
         tmp_str = strRelplace;
    }

    tmp_str.replace(" ", "");

    global.dragDrop_pathStr = tmp_str;
    QString tmp_Path4 = tmp_str.split("&").at(0);
    qDebug() << "tmp_Path4=" << tmp_Path4;
    QString tmp_Path1 = tmp_str.split("//").first();
    QString tmp_Path2 = tmp_str.split("//").last();
    QString tmp_Path3 = tmp_str.split("/").last();

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));

    QJsonObject json;

    //220726-tmp le_search->clear();
    if(tmp_str.contains("https://t.")){ //j220903 twitter

        this->redirectUrl_before = tmp_str;

        CommonGetRedirectUrl *redirectUrl = new CommonGetRedirectUrl("ROSE", tmp_str);
        connect(redirectUrl, &CommonGetRedirectUrl::signal_redirectUrl, this, &AbstractMainContent::slot_redirectUrl);
    }
    else if(tmp_Path1.contains("http")&& tmp_Path2.size() > 0){
        qDebug() << "tmp_str.split(?v=).last()=" << tmp_str.split("?v=").last();
        if((tmp_str.contains("?v=")&& tmp_str.split("?v=").last().size()>0) || ( tmp_str.contains("/shorts/")&& tmp_str.split("/shorts/").last().size()>0)){
            print_debug();
            if(tmp_str.contains("start_radio")){
                print_debug();
                QStringList tmpPlaylist = tmp_str.split("&");
                qDebug() << "---" << QString("https://api.roseaudio.kr/newpipe/v1/playlist?playlistUrl=https://www.youtube.com/playlist?%1").arg(tmpPlaylist[1]);
                network->request(5678
                                 , QString("https://api.roseaudio.kr/newpipe/v1/playlist?playlistUrl=https://www.youtube.com/playlist?%1").arg(tmpPlaylist[1])
                        , json
                        , false
                        , true);
            }
            else if((tmp_str.contains("&")&& tmp_str.split("&").at(1).size()>0)){
                print_debug();
                //qDebug() << QString("https://api.roseaudio.kr/newpipe/v1/playlist?playlistUrl=https://www.youtube.com/playlist?%1").arg(tmp_str.split("&").at(1));
                //qDebug() << "network->request=" << QString("https://api.roseaudio.kr/newpipe/v1/get?playurl=%1").arg(tmp_Path4);
                qDebug() << "network->request=" << QString("https://api.roseaudio.kr/newpipe/v1/playlist?playlistUrl=https://www.youtube.com/playlist?%1").arg(tmp_str.split("&").at(1));
                network->request(5678
                                 // , QString("https://api.roseaudio.kr/newpipe/v1/playlist?playlistUrl=https://www.youtube.com/playlist?%1").arg(tmp_str.split("&").at(1))
                                 , QString("https://api.roseaudio.kr/newpipe/v1/get?playurl=%1").arg(tmp_Path4)
                                 , json
                                 , false
                                 , true);
            }else if(tmp_str.contains("?v=")&& tmp_str.split("?v=").last().size()>0){
                global.search_text = tmp_str.split("?v=").last();
                qDebug() << "global.search_text" << global.search_text;
                //this->slot_returnPressed(tmp_Path3);
                print_debug();
                //emit linker->signal_checkQueue(11, "");
                slot_overrideSigalSearch(true);
                //emit linker->signal_clickedMovePageRoseTubeSearchCall();
                print_debug();
                this->le_search_back->clear();
            }else{
                qDebug() << "network->request=" << QString("https://api.roseaudio.kr/newpipe/v1/get?playurl=%1").arg(tmp_str);
                network->request(5678
                                 //, QString("https://api.roseaudio.kr/newpipe/v1/playlist?playlistUrl=%1").arg(tmp_str)
                                 , QString("https://api.roseaudio.kr/newpipe/v1/get?playurl=%1").arg(tmp_str)
                                 , json
                                 , false
                                 , true);
            }
        }
        else if(tmp_Path2.contains("youtube.com/c/") || tmp_Path2.contains("youtube.com/user/")){
            print_debug();
            if(tmp_str.split("/").count() > 1 && tmp_Path3.size()>0){
                print_debug();

                global.search_text = tmp_Path3;
                qDebug() << "tmp_Path3=" << tmp_Path3;
                //this->slot_returnPressed(tmp_Path3);
                print_debug();
                //emit linker->signal_checkQueue(11, "");
                slot_overrideSigalSearch(true);
                //emit linker->signal_clickedMovePageRoseTubeSearchCall();
                print_debug();
                this->le_search_back->clear();

            }else{
                slot_overrideSigalSearch(false);
                qDebug() << "Drag-and-drop playback is not supported except for YouTube content.";
                // ToastMsg::show(this,"", tr("Drag-and-drop playback is not supported except for YouTube content."), 2000, 0);

            }
        }
        else if(tmp_Path2.contains("link.roseaudio.kr")){//c220820
            //https://dev.link.roseaudio.kr/connect/AIR5Cz4leB?param=QOBUZ/ALBUM/hie6erjj8564a
            print_debug();
            qDebug() << "---" << tmp_str.split("?param=").first();
            //----------------------------
            QString get_str;

            if(!tmp_str.split("https://").first().isEmpty()) {//c220820
                print_debug();
                if(tmp_str.split("link.roseaudio.kr/connect/").size()>=2){
                    QString se_str = tmp_str.split("link.roseaudio.kr/connect/").at(1).split(")").at(0);
                    qDebug() << "se_str--" << se_str;
                    se_str = QString("https://link.roseaudio.kr/connect/%1").arg(se_str);
                    qDebug() << "se_str--" << se_str;
                    get_str = getUrlResponse(se_str);
                    qDebug() << "get_str--" << get_str;
                }
                //get_str = getUrlResponse( tmp_str.split("?param=").first());//tmp_str);
                //ToastMsg::show(this, "", tr("The input stream format does not match."));
                get_str = get_str.toUtf8().replace("\\", "");
                global.search_text = tmp_str.split("https://").first().split(" (").first();//c220824_4
                qDebug() << "global.search_text--" << global.search_text;
                slot_overrideSigalSearch(true);
                if(get_str.contains("TIDAL/")){//c220820
                    print_debug();
                    emit linker->signal_clickedMovePageTidalSearch();
                }else if(get_str.contains("BUGS/")){
                    print_debug();
                    emit linker->signal_clickedMovePageBugsSearch();
                }else if(get_str.contains("QOBUZ/")){
                    print_debug();
                    emit linker->signal_clickedMovePageQobuzSearch();
                }else if(get_str.contains("VIDEO/")){
                    print_debug();
                    emit linker->signal_clickedMovePageVideoSearch();
                }else if(get_str.contains("ROSERADIO/")){
                    print_debug();
                    emit linker->signal_clickedMovePageRoseRadioSearch();
                }else if(get_str.contains("APPLEMUSIC/")){
                    print_debug();
                    //emit linker->signal_clickedMovePageAppleMusicSearch();
                }else{

                }
            }
            else{
                print_debug();
                QString ty = tmp_str.split("?param=").first();
                qDebug() << "ty==" << ty;
                get_str = getUrlResponse( ty);//tmp_str);
                //qDebug() << "pResponse->readAll()---" << get_str.toUtf8();
                get_str = get_str.toUtf8().replace("\\", "");
                qDebug() << "pResponse->readAll()---" << get_str;
                if(get_str.split("<title>").size()<=0) {
                    ToastMsg::show(this, "", tr("The input stream format does not match."));
                    return;
                }
                qDebug() << "tmp_str.split(<title>).at(1)=" << get_str.split("<title>").at(1);
                qDebug() << "tmp_str.split(<title>).at(0)=" <<  get_str.split("<title>").at(1).split("</title>").at(0);
                QString track_title = get_str.split("<title>").at(1).split("</title>").at(0);

                //-------------------------------
                global.search_text = get_str.split("?param=").at(1).split("\";").at(0);

                //slot_overrideSigalSearch(true);
                //slot_returnPressed(tmp_Path3);
                if(get_str.contains("param=")){
                    print_debug();
                    if(get_str.contains("/TRACK/")){
                        print_debug();
                        slot_overrideSigalSearch(false);

                        // emit linker->signal_clickedMovePageSearch_share(tmp_str);

                        //------------------------------------------------------------

                        //slot_overrideSigalSearch(true);
                        //slot_returnPressed(tmp_Path3);
                        if(get_str.contains("MSUIC")){
                            emit linker->signal_clickedMovePageMusicSearch();
                        }
                        else if(get_str.contains("ROSE_TOTAL")){
                            print_debug();
                            //emit linker->signal_clickedMovePageSearch_share("ROSE_TOTAL/TRACK/"+global.search_text);
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220826_1
                        }
                        else if(get_str.contains("ROSE_TUBE")){
                            print_debug();
                            if(!get_str.contains("ROSE_TUBE/TRACK/") && get_str.contains("https://i.ytimg.com")){//c220828_3
                                QString temp = get_str.split("https://i.ytimg.com/").at(1).split("/").at(1);
                                global.search_text = temp;
                                qDebug() <<"temp="<<temp;
                                emit linker->signal_clickedMovePageRoseTubeSearch();
                                //emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220826_1
                            }
                            else if(get_str.contains("ROSE_TUBE/TRACK/")){//c220828_3
                                QString track_url_id = get_str.split("ROSE_TUBE/TRACK/").at(1).split("\";").at(0);
                                qDebug() << "track_url_id---" << track_url_id;
                                global.search_text = "ROSE_TUBE/TRACK/" + track_url_id;
                                //emit linker->signal_clickedMovePageRoseTubeSearch();
                                emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220826_1
                            }
                            else{
                                print_debug();
                                //emit linker->signal_clickedMovePageSearch_share("ROSE_TOTAL/TRACK/"+global.search_text);
                                emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220826_1
                            }
                        }
                        else if(get_str.contains("TIDAL")){//c220820
                            print_debug();

                            //QString track_url_id = get_str.split("TIDAL/TRACK/").at(1).split("\";").at(0);
                            //qDebug() << "track_url_id---" << track_url_id;
                            //global.search_text = track_url_id;
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                        }
                        else if(get_str.contains("BUGS")){
                            print_debug();
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                            //emit linker->signal_clickedMovePageBugsSearch();
                        }
                        else if(get_str.contains("QOBUZ")){
                            print_debug();
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                            //emit linker->signal_clickedMovePageQobuzSearch();
                        }
                        else if(get_str.contains("VIDEO")){
                            print_debug();
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                            //emit linker->signal_clickedMovePageVideoSearch();
                        }
                        else if(get_str.contains("ROSERADIO")){
                            print_debug();
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                            //emit linker->signal_clickedMovePageRoseRadioSearch();
                        }
                        else if(get_str.contains("APPLEMUSIC")){
                            print_debug();
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                            //emit linker->signal_clickedMovePageAppleMusicSearch();
                        }
                        else{
                            emit linker->signal_checkQueue(34, "");//c220824_4
                        }
                    }
                    else if(get_str.contains("/VIDEO/")){
                        if(get_str.contains("TIDAL")){//c220820
                            print_debug();

                            //QString track_url_id = get_str.split("TIDAL/TRACK/").at(1).split("\";").at(0);
                            //qDebug() << "track_url_id---" << track_url_id;
                            //global.search_text = track_url_id;
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                        }
                    }
                    else if(get_str.contains("/mv/")){
                        if(get_str.contains("BUGS")){//c220820
                            print_debug();

                            //QString track_url_id = get_str.split("TIDAL/TRACK/").at(1).split("\";").at(0);
                            //qDebug() << "track_url_id---" << track_url_id;
                            //global.search_text = track_url_id;
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                        }
                    }
                    else{
                        slot_overrideSigalSearch(false);
                        emit linker->signal_clickedMovePageSearch_share(global.search_text);
                    }
                }

                //slot_returnPressed(tmp_Path3);
                this->le_search_back->clear();
            }

            print_debug();
            this->le_search_back->clear();
        }
        else{

            print_debug();
            qDebug() << "tmp_str=" << tmp_str;
            QString get_str = getUrlResponse(tmp_str);
            //qDebug() << "pResponse->readAll()---" << get_str.toUtf8();
            get_str = get_str.toUtf8().replace("\\", "");
            qDebug() << "pResponse->readAll()---" << get_str;

            print_debug();
            if(tmp_str.split("/").count() > 1 && tmp_Path3.size()>0){
                print_debug();
                global.search_text = tmp_Path3;
                //emit linker->signal_clickedMovePageRoseTubeSearch();
                slot_returnPressed(tmp_Path3);
                le_search_back->clear();

            }
            else{
                qDebug() << "Drag-and-drop playback is not supported except for YouTube content.";
                ToastMsg::show(this,"", tr("Drag-and-drop playback is not supported except for YouTube content."), 2000, 0);
            }
        }
    }
    else {
        print_debug();
        global.dragDrop_pathStr = "";
        qDebug()<< "tmp_str= " << tmp_str;
        if(tmp_str.size()>=2){
            global.search_text = tmp_str;
            slot_returnPressed(tmp_str);
            this->le_search_back->clear();
        }else{
            ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            slot_overrideSigalSearch(false);
        }
    }
    print_debug();
    qDebug() << "global.dragDrop_pathStr=" << global.dragDrop_pathStr;
}

void AbstractMainContent::slot_returnPressed(const QString p_text){//c220730
    print_debug();
    qDebug() << "p_text=" << p_text;

    // btn_del->show();

    if(p_text.size()< 2) {

        ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
        return;
    }

    emit linker->signal_search(p_text);



    //emit linker->signal_sugestionbarOld_sel(p_text);
    //le_search->setText(p_text);//c22715

}


