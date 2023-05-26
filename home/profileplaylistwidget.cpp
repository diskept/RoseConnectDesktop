#include "home/profileplaylistwidget.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/filedownloader.h"
#include "common/networkhttp.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "widget/ElidedLabel.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QKeyEvent>
#include <QPainter>


const int PROFILEIMG_SIZE = 30;
const int HTTP_GET_PLAYLIST_MY = 88;
const int HTTP_GET_PLAYLIST_FRIEND = 77;
const int HTTP_FETCH_PLAYLIST_TRACK = 66;
const int HTTP_FETCH_PLAYLIST_INFO = 55;
const int HTTP_GET_PLAYLIST_MY_MORE = 44;
const int HTTP_GET_PLAYLIST_FRIEND_MORE = 43;


/** ===================================================================================================
 *              class ProfileRowBtn 프로필 닉네임 버튼
 * ===================================================================================================
 */
/**
 * @brief ProfileRowBtn::ProfileRowBtn 생성자
 * @param parent
 */
ProfileRowBtn::ProfileRowBtn(QWidget *parent) : QPushButton(parent)
{
    this->setFixedHeight(60);
    this->setCursor(Qt::PointingHandCursor);
    this->setUIControl();
}


void ProfileRowBtn::setUIControl(){

    lb_img = new QLabel();
    lb_img->setFixedSize(PROFILEIMG_SIZE,PROFILEIMG_SIZE);
    lb_username = new QLabel();
    lb_username->setStyleSheet("font-size:14px;color:#E5E5E4;font:bold;");

    btn_open = GSCommon::getUIBtnImg("btn_open", ":/images/icon_open_w_30.png");
    btn_close = GSCommon::getUIBtnImg("btn_close", ":/images/icon_hide_w_30.png");
    btn_open->setProperty("arrowType", "open");
    btn_close->setProperty("arrowType", "hide");
    btn_open->setFixedSize(30,30);
    btn_close->setFixedSize(30,30);
    btn_open->setCursor(Qt::PointingHandCursor);
    btn_close->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(25,0,0,0);
    hl_total->setSpacing(0);
    hl_total->setAlignment(Qt::AlignLeft);
    hl_total->addWidget(lb_img);
    hl_total->addSpacing(10);
    hl_total->addWidget(lb_username, 1, Qt::AlignLeft);
    hl_total->addWidget(btn_open);
    hl_total->addWidget(btn_close);
    hl_total->addSpacing(5);
    this->setLayout(hl_total);

    this->flagExpanded = false;
    this->btn_close->setVisible(false);
    this->setStyleSheet("background-color:transparent;");

    // 커넥션
    connect(btn_open, &QPushButton::clicked, this, &ProfileRowBtn::clicked);
    connect(btn_close, &QPushButton::clicked, this, &ProfileRowBtn::clicked);

}


/**
 * @brief ProfileBtn::setDataProfileBtn 데이터 세팅
 * @param p_jsonObject
 */
void ProfileRowBtn::setDataProfileBtn(QJsonObject p_jsonObject){
    QString tmp_username = "";
    QString tmp_imgPath = "";
    this->lb_username->setText("");
    /*if(p_jsonObject.contains("username")){
        tmp_username = p_jsonObject["username"].toString();
    }
    if(p_jsonObject.contains("main_profileimg")){
        tmp_imgPath = p_jsonObject["main_profileimg"].toString();
    }*/
    if(p_jsonObject.contains("name")){
        tmp_username = p_jsonObject["name"].toString();
    }
    if(p_jsonObject.contains("profileImage")){
        tmp_imgPath = p_jsonObject["profileImage"].toString();
    }
    this->lb_username->setText(tmp_username);
    if(tmp_imgPath!=""){
        FileDownloader *tmp_fileDownLoader = new FileDownloader(this);
        connect(tmp_fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImageMainProfile()));
        tmp_fileDownLoader->setImageURL(QUrl(tmp_imgPath));
    }else{
        QPixmap *tmp_pixmap = GSCommon::getUIPixmapImg(":images/profile_def_30.png");
        this->lb_img->setPixmap(*tmp_pixmap);
    }

    // 현 메인윈도우 사이즈에 맞게 small 모드 or big 모드
    if(global.width_mainwindow<=MAINWINDOW_SIZE_BOUNDARY){
        this->setMinSize();
    }else{
        this->setMaxSize();
    }
}


void ProfileRowBtn::setMinSize(){
    this->lb_username->setVisible(false);
    if(this->flagExpanded){
        this->btn_close->setVisible(false);
    }else{
        this->btn_open->setVisible(false);
    }
}


void ProfileRowBtn::setMaxSize(){
    this->lb_username->setVisible(true);
    if(this->flagExpanded){
        this->btn_close->setVisible(true);
    }else{
        this->btn_open->setVisible(true);
    }
}


void ProfileRowBtn::setToggle(){
    if(this->flagExpanded){
        this->btn_open->setVisible(true);
        this->btn_close->setVisible(false);
    }else{
        this->btn_open->setVisible(false);
        this->btn_close->setVisible(true);
    }
    this->flagExpanded = !this->flagExpanded;

    // 현 메인윈도우 사이즈에 맞게 small 모드 or big 모드
    if(global.width_mainwindow<=MAINWINDOW_SIZE_BOUNDARY){
        this->setMinSize();
    }else{
        this->setMaxSize();
    }
}


/**
 * @brief ProfileBtn::slot_loadImageMainProfile [SLOT] 프로필 메인 이미지 세팅
 */
void ProfileRowBtn::slot_loadImageMainProfile(){

    FileDownloader *tmp_fileDownloader = qobject_cast<FileDownloader*>(sender());

    //QPixmap *pixmap_albumImg = new QPixmap();
    QPixmap pixmap_albumImg;
    bool flagLoad = pixmap_albumImg.loadFromData(tmp_fileDownloader->downloadedData());
    if(flagLoad){
        QPixmap tmp_pixmap = pixmap_albumImg.scaled(PROFILEIMG_SIZE, PROFILEIMG_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // KeepAspectRatio

        // 빈 Pixmap
        QPixmap pixmap_painter = QPixmap(QSize(PROFILEIMG_SIZE,PROFILEIMG_SIZE));
        pixmap_painter.fill(Qt::transparent);

        QPainter painter (&pixmap_painter);
        painter.setRenderHint(QPainter::Antialiasing, true);
        QBrush brush = QBrush(tmp_pixmap);
        painter.setBrush(brush);
        painter.drawRoundedRect(0, 0, PROFILEIMG_SIZE, PROFILEIMG_SIZE, PROFILEIMG_SIZE/2, PROFILEIMG_SIZE/2);    // border-radius:4px
        this->lb_img->setPixmap(pixmap_painter);
    }

    tmp_fileDownloader->deleteLater();
}





/** ===================================================================================================
 *              class PlayListRowBtn 플레이리스트명 Row 버튼
 * ===================================================================================================
 */
/**
 * @brief PlayListRowBtn::PlayListRowBtn 생성자
 * @param parent
 */
PlayListRowBtn::PlayListRowBtn(QWidget *parent) : QPushButton(parent)
{
    this->setFixedHeight(60);
    this->setCursor(Qt::PointingHandCursor);
    this->setUIControl();
}


/**
 * @brief PlayListRowBtn::setUIControl UI 생성
 */
void PlayListRowBtn::setUIControl(){

    // UI 세팅
    lb_icon = GSCommon::getUILabelImg(":/images/playlist/mus_mini_play_ico.png");
    lb_icon->setObjectName("lb_icon");
    //lb_icon->setStyleSheet("#lb_icon { background-color:#4C4C4C; }");
    lb_icon->setStyleSheet("#lb_icon { background-color:transparent; }");
    lb_title = new QLabel();
    lb_title->setMaximumWidth(180);
    //lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, lb_title->width(), lb_title->font()));

    lb_title->setStyleSheet("font-size:14px;color:#E5E5E4;");

    QHBoxLayout *hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(25,15,0,15);
    hl_total->setSpacing(0);
    hl_total->setAlignment(Qt::AlignLeft);
    hl_total->addWidget(lb_icon, 0, Qt::AlignLeft);
    hl_total->addSpacing(10);
    hl_total->addWidget(lb_title, 1, Qt::AlignLeft);

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setLayout(hl_total);
    widget_total->setStyleSheet("#widget_total { background-color:#171717; }");
    QVBoxLayout *vl_total = new QVBoxLayout();

    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget_total);
    vl_total->addWidget(GSCommon::getHorizontalBar("#252525", 2));

    this->setLayout(vl_total);
}


/**
 * @brief PlayListRowBtn::setDataPlayListRowBtn 외부로부터 데이터 세팅
 * @param p_jsonObject
 */
void PlayListRowBtn::setDataPlayListRowBtn(QJsonObject p_jsonObject){

    this->dataPlayListRow.title = ProcJsonEasy::getString(p_jsonObject, "title");
    this->dataPlayListRow.type = ProcJsonEasy::getString(p_jsonObject, "type");
    this->dataPlayListRow.no = ProcJsonEasy::getInt(p_jsonObject, "id");
    this->dataPlayListRow.totalcount = ProcJsonEasy::getInt(p_jsonObject, "trackCount");

    QPixmap *pixmap_icon;
    if(this->dataPlayListRow.type=="MUSIC"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/home_music.png");
    }
    else if(this->dataPlayListRow.type=="VIDEO"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/home_video.png");
    }
    else if(this->dataPlayListRow.type=="YOUTUBE"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/home_tube.png");
    }
    else if(this->dataPlayListRow.type=="POD"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/playlist/pod_mini_play_ico.png");
    }
    else if(this->dataPlayListRow.type=="TIDAL"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/home_tidal.png");
    }
    else if(this->dataPlayListRow.type=="BUGS"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/home_bugs.png");
    }
    else if(this->dataPlayListRow.type=="QOBUZ"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/home_qobuz.png");
    }
    else if(this->dataPlayListRow.type=="APPLE_MUSIC"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/home_apple.png");
    }
    else{
        // default 추가하였음 by sunnyfish  (compiler checking point - warning)
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/home_all.png");
    }


    // 데이터 세팅
    /*if(p_jsonObject.contains("title")){
        this->dataPlayListRow.title = p_jsonObject["title"].toString();
    }
    if(p_jsonObject.contains("type")){
        this->dataPlayListRow.type = p_jsonObject["type"].toString();
    }
    if(p_jsonObject.contains("no")){
        this->dataPlayListRow.no = p_jsonObject["no"].toInt();
    }
    if(p_jsonObject.contains("totalcount")){
        this->dataPlayListRow.totalcount = p_jsonObject["totalcount"].toInt();
    }

    QPixmap *pixmap_icon;
    if(this->dataPlayListRow.type=="music"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/playlist/mus_mini_play_ico.png");
    }
    else if(this->dataPlayListRow.type=="video"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/playlist/video_mini_play_ico.png");
    }
    else if(this->dataPlayListRow.type=="youtube"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/playlist/tube_mini_play_ico.png");
    }
    else if(this->dataPlayListRow.type=="pod"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/playlist/pod_mini_play_ico.png");
    }
    else if(this->dataPlayListRow.type=="tidal"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/playlist/tidal_mini_play_ico.png");
    }
    else if(this->dataPlayListRow.type=="bugs"){
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/playlist/bugs_mini_play_ico.png");
    }
    else{
        // default 추가하였음 by sunnyfish  (compiler checking point - warning)
        pixmap_icon = GSCommon::getUIPixmapImg(":/images/playlist/mus_mini_play_ico.png");
    }*/

    this->lb_icon->setPixmap(*pixmap_icon);
    //this->lb_icon->resize(PROFILEIMG_SIZE, PROFILEIMG_SIZE);
    this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->dataPlayListRow.title, lb_title->width(), lb_title->font()));
    if(this->lb_title->text().contains("…")){
        this->lb_title->setToolTip(this->dataPlayListRow.title);
        this->lb_title->setToolTipDuration(2000);
    }
}


void PlayListRowBtn::setMinSize(){
    this->lb_title->setVisible(false);
}


void PlayListRowBtn::setMaxSize(){
    this->lb_title->setVisible(true);
}





/** ===================================================================================================
 *              class ProfilePlayListWidget 프로필 버튼 과 플레이리스트 포함된 클래스
 * ===================================================================================================
 */
/**
 * @brief ProfilePlayListWidget::ProfilePlayListWidget 생성자
 * @param p_profileWho
 * @param parent
 */
ProfilePlayListWidget::ProfilePlayListWidget(ProfileWhoType p_profileWho, QWidget *parent) : QWidget(parent)
{
    this->type_profileWho = p_profileWho;

    this->setUIControl();
}


/**
 * @brief ProfilePlayListWidget::setUIControl UI 세팅
 */
void ProfilePlayListWidget::setUIControl(){

    this->profileRowBtn = new ProfileRowBtn();

    this->vl_playList =new QVBoxLayout();
    this->vl_playList->setContentsMargins(0,0,0,0);
    this->vl_playList->setSpacing(0);

    // 목록 더보기 버튼
    this->btn_list_open_ico = GSCommon::getUIBtnImg("btn_list_open_ico", ":/images/list_open_ico.png");
    this->btn_list_open_ico->setFixedHeight(30);
    this->btn_list_open_ico->setCursor(Qt::PointingHandCursor);
    this->btn_list_open_ico->setStyleSheet(this->btn_list_open_ico->styleSheet().replace("background-color:rgba(0,0,0,0);", "background-color:#171717;"));

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(this->profileRowBtn);
    vl_total->addLayout(this->vl_playList);
    vl_total->addWidget(this->btn_list_open_ico);
    this->setLayout(vl_total);

    // 더보기 버튼 hide
    this->btn_list_open_ico->setVisible(false);

    // 커넥션
    connect(this->profileRowBtn, &ProfileRowBtn::clicked, this, &ProfilePlayListWidget::slot_clickedProfileRowBtn);
    connect(this->btn_list_open_ico, &QPushButton::clicked, this, &ProfilePlayListWidget::slot_clickedMore);
}


/**
 * @brief ProfilePlayListWidget::setDataProfileInfo 데이터 세팅
 * @param p_jsonObject
 */
void ProfilePlayListWidget::setDataProfileInfo(QJsonObject p_jsonObject){

    this->pkNo = 0;
    /*if(p_jsonObject.contains("no")){
        this->pkNo = p_jsonObject["no"].toInt();
    }*/
    if(p_jsonObject.contains("id")){
        this->pkNo = p_jsonObject["id"].toInt();
    }

    // 프로필 UI 세팅
    this->profileRowBtn->setDataProfileBtn(p_jsonObject);
}


/**
 * @brief ProfilePlayListWidget::slot_clickedProfileRow [SLOT] 프로필 Row 클릭시 처리
 */
void ProfilePlayListWidget::slot_clickedProfileRowBtn(){

    // 전체 목록 초기화
    this->totalCount = 0;
    this->cntMoreClicked = 0;
    this->list_playListRowBtn.clear();
    GSCommon::clearLayout(this->vl_playList);
    this->btn_list_open_ico->setVisible(false);

    if(this->profileRowBtn->getFlagExpaneded()==true){
        // 펼쳐진 상태에는 API 호출하지 않고 클리어만 한다.
    }
    else{
        // 접혀진 상태일때만 API 호출한다.
        roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
        connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &ProfilePlayListWidget::slot_applyResult_MyPlaylist);
        proc_myPlaylist->request_rose_getList_myPlaylists("member/playlist", "", this->cntMoreClicked, 20);

        /*NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        QJsonObject json;
        json.insert("local_ip", global.device.getDeviceIP());
        json.insert("mac_address", global.device.getDeviceID());
        if(this->type_profileWho==ProfileWhoType::My){
            // 마이 플레이리스트 경우
            network->request(HTTP_GET_PLAYLIST_MY, QString("%1/playlist/all/mod/?page=%2&size=20")
                             .arg(global.legacy_v3_api).arg(this->cntMoreClicked), json, true, true);
        }
        else{
            // 친구 플레이리스트 경우
            network->request(HTTP_GET_PLAYLIST_FRIEND, QString("%1/playlist/friend/mod/%2?page=0&size=20")
                             .arg(global.legacy_v3_api).arg(this->pkNo), json, true, true);
        }*/
    }

    this->profileRowBtn->setToggle();
}


/**
 * @brief ProfilePlayListWidget::slot_clickedMore [SLOT] 더보기 버튼 클릭시
 */
void ProfilePlayListWidget::slot_clickedMore(){

    if(this->flag_lastPage == false){
        this->cntMoreClicked++;     // 더보기 클릭 횟수

        // 접혀진 상태일때만 API 호출한다.
        roseHome::ProcCommon *proc_myPlaylist = new roseHome::ProcCommon(this);
        connect(proc_myPlaylist, &roseHome::ProcCommon::completeReq_list_myplaylists, this, &ProfilePlayListWidget::slot_applyResult_MyPlaylist);
        proc_myPlaylist->request_rose_getList_myPlaylists("member/playlist", "", this->cntMoreClicked, 20);
    }


    /*this->cntMoreClicked++;     // 더보기 클릭 횟수

    NetworkHttp *network = new NetworkHttp();
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    QJsonObject json;
    json.insert("local_ip", global.device.getDeviceIP());
    json.insert("mac_address", global.device.getDeviceID());

    if(this->type_profileWho == ProfileWhoType::My){
        // 마이 플레이리스트 경우
        network->request(HTTP_GET_PLAYLIST_MY_MORE, QString("%1/playlist/all/mod/?page=%2&size=20")
                         .arg(global.legacy_v3_api).arg(cntMoreClicked), json, true, true);

    }
    else if(this->type_profileWho == ProfileWhoType::Friend){
        // 친구 플레이리스트 경우
        network->request(HTTP_GET_PLAYLIST_FRIEND_MORE, QString("%1/playlist/friend/mod/%2?page=%3&size=20")
                         .arg(global.legacy_v3_api).arg(this->pkNo).arg(this->cntMoreClicked), json, true, true);

    }*/
}


void ProfilePlayListWidget::slot_applyResult_MyPlaylist(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

    Q_UNUSED(list_data);

    this->flag_lastPage = flag_lastPage;

    QJsonObject tmpJson;
    tmpJson.insert("playlists", jsonArr_dataToPlay);
    this->setAddPlayListRowBtn(tmpJson);
}



/**
 * @brief ProfilePlayListWidget::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void ProfilePlayListWidget::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id==HTTP_GET_PLAYLIST_MY || p_id==HTTP_GET_PLAYLIST_FRIEND){

        this->setAddPlayListRowBtn(p_jsonObject);
    }
    else if(p_id==HTTP_GET_PLAYLIST_MY_MORE || p_id==HTTP_GET_PLAYLIST_FRIEND_MORE){

        this->setAddPlayListRowBtn(p_jsonObject);
    }
    else if(p_id==HTTP_FETCH_PLAYLIST_TRACK){

    }
    else if(p_id==HTTP_FETCH_PLAYLIST_INFO){

    }
    sender()->deleteLater();
}


/**
 * @brief ProfilePlayListWidget::setAddPlayListRowBtn
 * @param p_jsonObject
 */
void ProfilePlayListWidget::setAddPlayListRowBtn(QJsonObject p_jsonObject){

    QJsonArray tmpArr = ProcJsonEasy::getJsonArray(p_jsonObject, "playlists");

    for(int i = 0; i < tmpArr.size(); i++){
        QJsonObject tmpObj = tmpArr.at(i).toObject();

        // UI 세팅
        PlayListRowBtn *playListRowBtn = new PlayListRowBtn();
        playListRowBtn->setDataPlayListRowBtn(tmpObj);
        connect(playListRowBtn, &PlayListRowBtn::clicked, this, &ProfilePlayListWidget::slot_clickedPlayListRowBtn);

        this->list_playListRowBtn.append(playListRowBtn);
        this->vl_playList->addWidget(playListRowBtn);
    }

    if(!this->flag_lastPage){
        this->btn_list_open_ico->setVisible(true);
    }
    else{
        this->btn_list_open_ico->setVisible(false);
    }


    /*if(p_jsonObject.contains("totalcount")){
        this->totalCount = p_jsonObject["totalcount"].toString().toInt();
    }
    if(p_jsonObject.contains("playlists")){
        QJsonArray array = p_jsonObject["playlists"].toArray();
        for(int i=0; i<array.count(); i++){
            QJsonObject tmp_data = array[i].toObject();

            // UI 세팅
            PlayListRowBtn *playListRowBtn = new PlayListRowBtn();
            playListRowBtn->setDataPlayListRowBtn(tmp_data);
            connect(playListRowBtn, &PlayListRowBtn::clicked, this, &ProfilePlayListWidget::slot_clickedPlayListRowBtn);
            this->list_playListRowBtn.append(playListRowBtn);
            this->vl_playList->addWidget(playListRowBtn);
        }
    }

    // 더보기 버튼 show/hide
    if(this->list_playListRowBtn.count() < this->totalCount){
        this->btn_list_open_ico->setVisible(true);
    }else{
        this->btn_list_open_ico->setVisible(false);
    }*/
}


/**
 * @brief ProfilePlayListWidget::slot_clickedPlayListRowBtn [SLOT] 플레이리스트 타이틀 Row 클릭시 처리
 */
void ProfilePlayListWidget::slot_clickedPlayListRowBtn(){

    PlayListRowBtn *tmp_sender = dynamic_cast<PlayListRowBtn*>(sender());
    //int tmp_no = tmp_sender->getPlayListNoPK();
    /*QString tmp_type = tmp_sender->getPlayListType();

    QJsonObject tmp_json;
    QJsonObject tmp_plyListInfo;
    if(tmp_type=="music"){

        tmp_json.insert("playListNo", tmp_sender->getPlayListNoPK());
        if(this->type_profileWho == ProfileWhoType::My){
            tmp_json.insert("flagPlayListMy", true); // false : 친구플레이리스트
        }
        else if(this->type_profileWho == ProfileWhoType::Friend){
            tmp_json.insert("flagPlayListMy", false); // false : 친구플레이리스트
        }
        tmp_json.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);

    }else if(tmp_type=="youtube"){

        tmp_json.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::RoseTube);
        //tmp_plyListInfo.insert("no", tmp_sender->getPlayListNoPK());
        tmp_plyListInfo.insert("id", tmp_sender->getPlayListNoPK());
    }
    tmp_json.insert(KEY_PAGE_CODE, PAGECODE_OP_PLAYLISTINFO);
    tmp_json.insert("playList_info", tmp_plyListInfo);*/

    QJsonObject tmp_data;
    tmp_data.insert("id", tmp_sender->getPlayListNoPK());

    QJsonObject tmp_json;
    tmp_json.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::RoseHome);
    tmp_json.insert(KEY_PAGE_CODE, PAGECODE_RH_PLAYLIST_ROSE_DETAIL);
    tmp_json.insert("data", tmp_data);

    emit this->signal_clickedPlayListTitle(tmp_json);
}


void ProfilePlayListWidget::setMinWidthSize(){
    this->profileRowBtn->setMinSize();
    for(int i=0; i<this->list_playListRowBtn.count(); i++){
        this->list_playListRowBtn[i]->setMinSize();
    }
}
void ProfilePlayListWidget::setMaxWidthSize(){
    this->profileRowBtn->setMaxSize();
    for(int i=0; i<this->list_playListRowBtn.count(); i++){
        this->list_playListRowBtn[i]->setMaxSize();
    }
}


