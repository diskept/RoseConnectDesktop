#include "musice_playlist.h"
#include "widget/clickablelabel.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/global.h"
#include "widget/toastmsg.h"
#include <QPixmapCache>
#include "login/dialog_newplaylist.h"
#include "widget/framewidget.h"
#include "widget/optionpopup.h"
#include <data/datapopup.h>
#include "common/rosesettings.h"
#include <QSettings>

const int max_start_point = 5;

const int HTTP_NETWORK_PLAYLIST_FETCH = 99;
const int HTTP_NETWORK_STAR = 97;
const int HTTP_NETWORK_HEART = 96;
const int HTTP_NETWORK_PLAY = 95;
const int HTTP_NETWORK_ADD_PLAYLIST_TRACK = 94;
const int HTTP_NETWORK_NEW_PLAYLIST = 93;
const int HTTP_NETWORK_TRACK_HEART_SET = 92;

/**
 * @brief Musice_playlist::Musice_playlist 생성자
 * @param parent
 */
Musice_playlist::Musice_playlist(QWidget *parent) : QWidget(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief Musice_playlist::setInit : 초기 세팅
 */
void Musice_playlist::setInit(){

    linker = Linker::getInstance();
    connect(linker, SIGNAL(signal_myPlaylistTrackChanged()), this, SLOT(slot_myPlaylistTrackChanged()));

    pixmap_heart_on.load(":/images/list_fav_icon_on.png");
    pixmap_heart_off.load(":/images/list_fav_icon.png");

    dataPlayList = new Data_M_PlayList(this);
    //list_data_RT_Track = new QList<Data_RT_Track*>();
    delegate = new MusicDelegate(this);

    dlg_playList = new Dialog::Dialog_Playlist(Dialog::Dialog_Playlist::PlayListType::YOUTUBE, this);

    connect(dlg_playList, SIGNAL(signal_clickedPlaylist(int)), this, SLOT(slot_playlistClicked(int)));
    connect(delegate, SIGNAL(signal_clickedBtn(int,int)), this, SLOT(slot_delegateClicked(int,int)));
}

/**
 * @brief Musice_playlist::setUIControl : UI 세팅
 */
void Musice_playlist::setUIControl(){

    // ------------------------------------------------------
    //  이미지
    // ------------------------------------------------------
    lb_image = new ImageLabel;
    lb_image->setType(ImageLabel::Type::MusicPlayListDetail);

    QVBoxLayout *box_image = new QVBoxLayout;
    box_image->setContentsMargins(0,0,0,0);
    //box_image->addSpacing(44);
    box_image->addWidget(lb_image);

    QWidget *wg_image = new QWidget;
    wg_image->setContentsMargins(0,0,0,0);
    wg_image->setLayout(box_image);


    // ------------------------------------------------------
    //  타이틀, 메모
    // ------------------------------------------------------
    lb_title = new QLabel;
    lb_title->setStyleSheet("color:#FFFFFF;font-size:30px;");
    lb_title->setContentsMargins(18,0,0,0);
    lb_title->setWordWrap(true);

    lb_memo = new QLabel;
    lb_memo->setStyleSheet("color:#CCCCCC;font-size:20px;");
    lb_memo->setContentsMargins(18,0,0,0);
    lb_memo->setWordWrap(true);

    // ------------------------------------------------------
    //  별점, 태그
    // ------------------------------------------------------
    QHBoxLayout *hl_star = new QHBoxLayout();
    hl_star->setSpacing(0);
    hl_star->setAlignment(Qt::AlignLeft);
    hl_star->addSpacing(5);
    this->list_lb_star = new QList<ClickableLabel*>();
    for(int i=0; i<max_start_point; i++){
        this->list_lb_star->append(new ClickableLabel());
        this->list_lb_star->at(i)->setPixmap(*GSCommon::getUIPixmapImg(":/images/star_icon_off.png"));
        this->list_lb_star->at(i)->setCursor(Qt::PointingHandCursor);
        hl_star->addWidget(this->list_lb_star->at(i));
        connect(this->list_lb_star->at(i), &ClickableLabel::signal_clicked, this, &Musice_playlist::slot_clickedStar);
    }
    widget_star = new QWidget();
    widget_star->setLayout(hl_star);

    lb_writer = new QLabel;
    lb_writer->setText("ROSEconnect");
    lb_writer->setStyleSheet("color:#B18658;font-size:16px;");
    lb_writer->setContentsMargins(18,0,0,0);

    flowLayout_tag = new FlowLayout(0,6,6);
    flowLayout_tag->setContentsMargins(18,0,0,0);

    // ------------------------------------------------------
    //  하트 , 더보기 버튼 UI
    // ------------------------------------------------------
    btn_heart = new QPushButton;
    btn_heart->setIcon(pixmap_heart_off);
    btn_heart->setIconSize(pixmap_heart_off.rect().size());
    btn_heart->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    btn_heart->setCursor(Qt::PointingHandCursor);
    btn_heart->setFixedSize(60,70);

    QPixmap pixmap_more(":/images/icon_menu.png");
    QPushButton *btn_more = new QPushButton;
    btn_more->setIcon(pixmap_more);
    btn_more->setIconSize(pixmap_more.rect().size());
    btn_more->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    btn_more->setCursor(Qt::PointingHandCursor);
    btn_more->setFixedSize(60,70);

    QHBoxLayout *box_option_1 = new QHBoxLayout;
    box_option_1->setAlignment(Qt::AlignLeft);
    box_option_1->setSpacing(0);
    box_option_1->setContentsMargins(0,0,0,0);
    //box_option_1->addWidget(btn_heart);
    box_option_1->addWidget(btn_more);


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

    QHBoxLayout *box_option_2 = new QHBoxLayout;
    box_option_2->setAlignment(Qt::AlignLeft);
    box_option_2->setSpacing(30);
    box_option_2->setContentsMargins(16,0,0,0);
    box_option_2->addWidget(btn_play);
    box_option_2->addWidget(btn_shuffle);

    // ------------------------------------------------------
    //  TOP 파트 박스 구성 및 레이아웃 세팅
    // ------------------------------------------------------
    QVBoxLayout *box_videoInfo = new QVBoxLayout;
    box_videoInfo->setAlignment(Qt::AlignVCenter);
    box_videoInfo->setSpacing(0);
    box_videoInfo->setContentsMargins(0,0,0,0);
    box_videoInfo->addWidget(lb_title);
    box_videoInfo->addSpacing(10);
    box_videoInfo->addWidget(lb_memo);
    box_videoInfo->addWidget(widget_star);
    box_videoInfo->addSpacing(3);
    box_videoInfo->addWidget(lb_writer);
    box_videoInfo->addSpacing(11);
    box_videoInfo->addLayout(flowLayout_tag);
    box_videoInfo->addSpacing(2);
    box_videoInfo->addLayout(box_option_1);
    box_videoInfo->addLayout(box_option_2);

    QHBoxLayout *box_top = new QHBoxLayout;
    box_top->setContentsMargins(0,0,0,0);
    box_top->setSpacing(12);
    box_top->setAlignment(Qt::AlignLeft);
    box_top->addWidget(wg_image,0, Qt::AlignTop);
    box_top->addLayout(box_videoInfo,1);


    // -----------------------------------------------------
    //  영상 목록 ListWidget
    // -----------------------------------------------------
    listWidget = new QListWidget;
    listWidget->setItemDelegate(delegate);
    listWidget->setCursor(Qt::PointingHandCursor);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setStyleSheet("border:none;");



    QWidget *widget_line = new QWidget();
    widget_line->setObjectName("widget_line");
    widget_line->setFixedHeight(2);
    widget_line->setStyleSheet("#widget_line { background-color:#2A2A2A; }");

    QVBoxLayout *box_total = new QVBoxLayout;
    box_total->setSpacing(0);
    box_total->setContentsMargins(33,10,28,0);
    box_total->setAlignment(Qt::AlignTop);
    box_total->addLayout(box_top);
    box_total->addSpacing(30);
    box_total->addWidget(widget_line);
    box_total->addWidget(listWidget);

    setLayout(box_total);

    // 커넥트
    connect(btn_heart, SIGNAL(clicked()), this, SLOT(slot_clickedHeart()));
    connect(btn_more, SIGNAL(clicked()), this, SLOT(slot_clickedMore()));
    connect(btn_play, SIGNAL(clicked()), this, SLOT(slot_clickedPlay()));
    connect(btn_shuffle, SIGNAL(clicked()), this, SLOT(slot_clickedShuffle()));
}

/**
 * @brief Musice_playlist::setFavoriteUI : (하트)즐겨찾기 UI 세팅
 */
void Musice_playlist::setFavoriteUI(){
    if(dataPlayList->getFavorites()==false){
        btn_heart->setIcon(pixmap_heart_off);
    }else{
        btn_heart->setIcon(pixmap_heart_on);
    }
}

/**
 * @brief Musice_playlist::setStarUI : 별점 세팅
 */
void Musice_playlist::setStarPointUI(){
    for(int i= 0 ; i < max_start_point; i++){
        if(i < dataPlayList->getStar()){
            list_lb_star->at(i)->setPixmap(*GSCommon::getUIPixmapImg(":/images/star_icon_on.png"));
        }else{
            list_lb_star->at(i)->setPixmap(*GSCommon::getUIPixmapImg(":/images/star_icon_off.png"));
        }
    }
}

/**
 * @brief Musice_playlist::setTagListUI : 태그 세팅
 */
void Musice_playlist::setTagListUI(){
    GSCommon::clearLayout(flowLayout_tag);
    QList<QString> list_tag = dataPlayList->getTags();
    for(int i = 0 ; i < list_tag.size(); i++){
        QLabel *lb_tag = new QLabel;
        lb_tag->setText("#"+list_tag.at(i));
        lb_tag->setContentsMargins(7,2,7,2);
        lb_tag->setStyleSheet("background-color:#333333;font-size:14px;color:#919191;");

        flowLayout_tag->addWidget(lb_tag);
    }
}

/**
 * @brief Musice_playlist::setPlayListData : 플레이리스트 데이터 세팅
 * @param p_jsonObject
 */
void Musice_playlist::setPlayListData(const QJsonObject &p_jsonObject){

    dataPlayList->setData(p_jsonObject);

    if(this->dataPlayList->getFlagPlayListMy()==true){
        // 마이 플레이리스트
        this->delegate->setShowHeartIcon(true);
        this->widget_star->setVisible(true);
        //this->btn_heart->setVisible(true);
    }else{
        // 친구 플레이리스트
        this->delegate->setShowHeartIcon(false);
        this->widget_star->setVisible(false);
        //this->btn_heart->setVisible(false);
    }


    // 메뉴항목 타이틀 변경
    emit signal_changedMenuName(dataPlayList->getTitle());

    this->playlistNo = 0;
    if(p_jsonObject.contains("playListNo")){
        this->playlistNo = p_jsonObject["playListNo"].toInt(0);
    }
    this->type = "";
    if(p_jsonObject.contains("type")){
        this->type = p_jsonObject["type"].toString();
    }
    this->thumbnail_type = "";
    if(p_jsonObject.contains("thumbnail")){
        this->thumbnail_type = p_jsonObject["thumbnail"].toString();
    }

    // 플레이리스트 정보 및 트랙목록 요청
    requestPlaysListFetch();
}

/**
 * @brief Musice_playlist::appendTrackUI : Track UI추가
 * @param p_index int index
 */
void Musice_playlist::appendTrackUI(const int &p_index){

    // --------------------------------------------------
    //  UserRole 데이터 세팅
    // --------------------------------------------------
    QJsonObject json;
    json.insert("index", p_index);
    //json.insert("duration", list_data_RT_Track->at(p_index)->getConvertDuration());
    json.insert("title", this->dataPlayList->getTrackList().at(p_index).title);
    json.insert("isFavorite",this->dataPlayList->getTrackList().at(p_index).isFavorite);
    json.insert("artist", this->dataPlayList->getTrackList().at(p_index).artist);

    QListWidgetItem *item = new QListWidgetItem;
    item->setData(Qt::UserRole, json);

    // --------------------------------------------------
    //  DecorationRole 데이터 세팅
    // --------------------------------------------------
    QPixmap pixmap;
    QString tmp_imgPath = this->dataPlayList->getTrackList().at(p_index).thumb_url;

    item->setData(Qt::DecorationRole, QPixmap(":images/def_mus_60.png"));

    FileDownloader *fileDownloader = new FileDownloader(this);
    fileDownloader->setProperty("index", p_index);
    fileDownloader->setImageURL(tmp_imgPath);
    connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));

    listWidget->addItem(item);
}

/**
 * @brief Musice_playlist::getPlayList : 재생을 위한 트랙 정보를 JSONArray로 반환
 * @param p_index int if < 0 all list
 * @return
 * App 디버깅 내용및 음악 재생 API 참고하여 구성함.
 */
QJsonArray Musice_playlist::getPlayList(int p_index){
    QJsonArray jsonArr;

    if(p_index >=0 ){
        QJsonObject jsonTracks;
        jsonTracks.insert("ip", this->dataPlayList->getTrackList()[p_index].mac_address);
        jsonTracks.insert("isFile", true);
        jsonTracks.insert("isServer", true);
        jsonTracks.insert("id", this->dataPlayList->getTrackList()[p_index].client_id); // 오디오 PK
        jsonTracks.insert("name", this->dataPlayList->getTrackList()[p_index].title);
        jsonTracks.insert("path", this->dataPlayList->getTrackList()[p_index].file_url);
        jsonTracks.insert("data", this->dataPlayList->getTrackList()[p_index].file_url);
        jsonTracks.insert("thumbnail", this->dataPlayList->getTrackList()[p_index].thumb_url);
        jsonTracks.insert("album", this->dataPlayList->getTrackList()[p_index].album);
        jsonTracks.insert("title", this->dataPlayList->getTrackList()[p_index].title);
        jsonTracks.insert("name", this->dataPlayList->getTrackList()[p_index].title);
        jsonTracks.insert("artist", this->dataPlayList->getTrackList()[p_index].artist);
        jsonTracks.insert("favorite_id", this->dataPlayList->getTrackList()[p_index].id); // favorite id

        QStringList tmp_strListPath = jsonTracks["path"].toString().split("/");
        QString tmp_path;
        for(int i= 0 ; i < tmp_strListPath.count(); i++){
            if(i < tmp_strListPath.count()-1){
                tmp_path += tmp_strListPath.at(i)+"/";
            }
        }
        jsonTracks.insert("ip", "?"+jsonTracks["ip"].toString());
        jsonTracks.insert("name", tmp_strListPath.last());
        jsonTracks.insert("path", "http://127.0.0.1:7080"+tmp_path);
        jsonTracks.insert("isShare", jsonTracks["ip"].toString()==global.device.getDeviceID() ? false : true);

        jsonArr.append(jsonTracks);
    }else{
        for(int i = 0 ; i < this->dataPlayList->getTrackList().count(); i++){
            QJsonObject jsonTracks;
            jsonTracks.insert("ip", this->dataPlayList->getTrackList()[i].mac_address);
            jsonTracks.insert("isFile", true);
            jsonTracks.insert("isServer", true);
            jsonTracks.insert("id", this->dataPlayList->getTrackList()[i].client_id); // 오디오 PK
            jsonTracks.insert("name", this->dataPlayList->getTrackList()[i].title);
            jsonTracks.insert("path", this->dataPlayList->getTrackList()[i].file_url);
            jsonTracks.insert("data", this->dataPlayList->getTrackList()[i].file_url);
            jsonTracks.insert("thumbnail", this->dataPlayList->getTrackList()[i].thumb_url);
            jsonTracks.insert("album", this->dataPlayList->getTrackList()[i].album);
            jsonTracks.insert("title", this->dataPlayList->getTrackList()[i].title);
            jsonTracks.insert("name", this->dataPlayList->getTrackList()[i].title);
            jsonTracks.insert("artist", this->dataPlayList->getTrackList()[i].artist);
            jsonTracks.insert("favorite_id", this->dataPlayList->getTrackList()[i].id); // favorite id

            QStringList tmp_strListPath = jsonTracks["path"].toString().split("/");
            QString tmp_path;
            for(int i= 0 ; i < tmp_strListPath.count(); i++){
                if(i < tmp_strListPath.count()-1){
                    tmp_path += tmp_strListPath.at(i)+"/";
                }
            }
            jsonTracks.insert("ip", "?"+jsonTracks["ip"].toString());
            jsonTracks.insert("name", tmp_strListPath.last());
            jsonTracks.insert("path", "http://127.0.0.1:7080"+tmp_path);
            jsonTracks.insert("isShare", jsonTracks["ip"].toString()==global.device.getDeviceID() ? false : true);
            jsonArr.append(jsonTracks);
        }
    }
    return jsonArr;
}

/**
 * @brief Musice_playlist::changeTrackFavoritesUI : 트랙 즐겨찾기 정보 변경
 * @param p_index
 */
void Musice_playlist::changeTrackFavoritesUI(const int &p_index){

    if(p_index >=0 && p_index < this->dataPlayList->getTrackList().count()){
        bool tmp_favorites = this->dataPlayList->getTrackList().at(p_index).isFavorite;
        QListWidgetItem *item = listWidget->item(p_index);
        QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
        map["isFavorite"] = !tmp_favorites;
        this->dataPlayList->setFavoritesTrack(p_index, !tmp_favorites);
        item->setData(Qt::UserRole, map);
    }
}

/**
 * @brief Musice_playlist::requestAddPlaylistTrack : 플레이리스트에 곡 추가
 * @param p_playlistno int 대상 플레이리스트 no
 */
void Musice_playlist::requestAddPlaylistTrack(const int &p_playlistno, const QString &p_playurl){
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("playlistno",QString::number(p_playlistno));
    params.addQueryItem("playurl",p_playurl);
    params.addQueryItem("username",global.user.getUsername());

    network->request(HTTP_NETWORK_ADD_PLAYLIST_TRACK, QString("%1/playlist/track/insert")
                     .arg(global.legacy_mod), params, true,true);
}


/**
 * @brief Musice_playlist::requestPlay : 음악 재생요청
 * @param jsonPlayList QJsonArray 음악 재생 목록
 * @param flagShuffle bool if true, shuffle 재생
 */
void Musice_playlist::requestPlay(QJsonArray jsonPlayList, bool flagShuffle){    
    QJsonObject jsonData;
    /*jsonData.insert("musicPlayType",15);
    jsonData.insert("music", jsonPlayList);
    jsonData.insert("playType",20);
    jsonData.insert("shuffle", flagShuffle ? 1 : 0);
    jsonData.insert("roseToken", global.device.getDeviceRoseToken());

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3")
                     .arg(global.device.getDeviceIP())
                     .arg(global.port).arg("music_network_play"), jsonData, true);*/

    QJsonObject playlistData;
    playlistData.insert("clientKey", this->playlist_obj["clientKey"].toString());
    playlistData.insert("favorite", this->playlist_obj["favorite"].toBool());
    playlistData.insert("favoriteTrack", this->playlist_obj["favoriteTrack"].toBool());
    playlistData.insert("isRose", this->playlist_obj["isRose"].toBool());
    playlistData.insert("ownerId", this->playlist_obj["ownerId"].toInt());
    playlistData.insert("star", this->playlist_obj["star"].toInt());
    playlistData.insert("thumbup", this->playlist_obj["thumbup"].toBool());
    playlistData.insert("thumbupCount", this->playlist_obj["thumbupCount"].toInt());
    playlistData.insert("trackCount", this->playlist_obj["trackCount"].toInt());
    playlistData.insert("type", this->playlist_obj["type"].toString());

    //QString strplaylist = "{\"clientKey\":\"11502\",\"favorite\":false,\"favoriteTrack\":false,\"isRose\":true,\"ownerId\":0,\"star\":0,\"thumbup\":false,\"thumbupCount\":0,\"trackCount\":0,\"type\":\"UNITED\"}";

    QString strplaylist = "";
    strplaylist += QString("{\"clientKey\":\"%1\",").arg(this->playlist_obj["clientKey"].toString());
    strplaylist += QString("\"favorite\":%1,").arg(this->playlist_obj["favorite"].toBool());
    strplaylist += QString("\"favoriteTrack\":%1,").arg(this->playlist_obj["favoriteTrack"].toBool());
    strplaylist += QString("\"isRose\":%1,").arg(this->playlist_obj["isRose"].toBool());
    strplaylist += QString("\"ownerId\":%1,").arg(this->playlist_obj["ownerId"].toInt());
    strplaylist += QString("\"star\":%1,").arg(this->playlist_obj["star"].toInt());
    strplaylist += QString("\"thumbup\":%1,").arg(this->playlist_obj["thumbup"].toBool());
    strplaylist += QString("\"thumbupCount\":%1,").arg(this->playlist_obj["thumbupCount"].toInt());
    strplaylist += QString("\"trackCount\":%1").arg(this->playlist_obj["trackCount"].toInt());
    strplaylist += QString("\"type\":\"%1\"}").arg(this->playlist_obj["type"].toString());

    QJsonObject playlist;
    playlist.insert("playlist", strplaylist);
    playlist.insert("type", "rose_playlist");
    jsonData.insert("musicPlayType", 15);
    jsonData.insert("shuffle", flagShuffle ? 1 : 0);
    jsonData.insert("tracks", jsonPlayList);
    jsonData.insert("roseToken", global.device.getDeviceRoseToken());

    if(global.Queue_track_count != 0) {
        print_debug();emit linker->signal_checkQueue(27, "");

        return;
    }
    print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
    global.Queue_track_count += strplaylist.count();     // 220419 queue count

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3")
                     .arg(global.device.getDeviceIP())
                     .arg(global.port).arg("total_queue_play_list_add"), jsonData, true);
}

/**
 * @brief Musice_playlist::requestPlaysListFetch : 플레이리스트 정보 요청
 * @note : star, favor 정보 얻기위함.
 */
void Musice_playlist::requestPlaysListFetch(){

    if(playlistNo > 0){
        listWidget->clear();

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject tmp_json;
        /*tmp_json.insert("local_ip", global.device.getDeviceIP());
        tmp_json.insert("mac_address", global.device.getDeviceID());
        tmp_json.insert("permission","1");*/

        if(this->dataPlayList->getFlagPlayListMy()==true){
            // 마이 플레이리스트
            /*network->request(HTTP_NETWORK_PLAYLIST_FETCH, QString("%1/playlist/%2?page=0&size=30")
                             .arg(global.legacy_v3_api).arg(playlistNo), tmp_json, true,true);*/
            network->request(HTTP_NETWORK_PLAYLIST_FETCH, QString("%1/member/playlist/%2?page=0&size=20")
                             .arg(global.legacy_v1).arg(this->playlistNo), tmp_json, false, true);

        }else{
            // 친구 플레이리스트
            /*network->request(HTTP_NETWORK_PLAYLIST_FETCH, QString("%1/playlist/friend/%2?page=0&size=20")
                             .arg(global.legacy_v3_api).arg(playlistNo), tmp_json, true,true);*/
            network->request(HTTP_NETWORK_PLAYLIST_FETCH, QString("%1/member/playlist/%2?page=0&size=20")
                             .arg(global.legacy_v1).arg(this->playlistNo), tmp_json, false, true);
        }
    }
}


/**
 * @brief Musice_playlist::requestNewPlaylistTrack : 새 플레이리스트 추가 요청
 */
void Musice_playlist::requestNewPlaylistTrack(const QString &p_name, const QString &p_playurl){
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("title",p_name);
    params.addQueryItem("type","youtube");
    params.addQueryItem("username",global.user.getUsername());
    params.addQueryItem("playurl", p_playurl);


    network->request(HTTP_NETWORK_NEW_PLAYLIST, QString("%1/playlist/new")
                     .arg(global.legacy_mod), params, true,true);
}

/**
 * @brief Musice_playlist::requestUpdateFavorites : 트랙 즐겨찾기(하트) 업데이트
 * @param p_playurl
 */
void Musice_playlist::requestUpdateTrackFavorites(const int &p_index){

    NetworkHttp *network = new NetworkHttp(this);
    network->setProperty("index", p_index);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QString tmp_apiCode = "";
    if(this->dataPlayList->getTrackList()[p_index].isFavorite){
        // 설정
        tmp_apiCode = "add";
    }else{
        // 해제
        tmp_apiCode = "delete";
    }

    QJsonObject tmp_json;
    tmp_json.insert("favorite", this->dataPlayList->getTrackList()[p_index].isFavorite);
    tmp_json.insert("tracks", this->dataPlayList->getTrackJsonArrayOfIndex(p_index));
    tmp_json.insert("local_ip", global.device.getDeviceIP());
    tmp_json.insert("mac_address", global.device.getDeviceID());
    network->request(HTTP_NETWORK_TRACK_HEART_SET, QString("%1/track/favorite/%2").arg(global.legacy_v3_api).arg(tmp_apiCode), tmp_json, true,true);
}


/**
 * @brief Musice_playlist::setResultOfStar : 별정보 업데이트 요청결과처리
 * @param p_jsonObject QJsonObject response
 */
void Musice_playlist::setResultOfStar(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";

    bool flagOk = false;

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_status) && p_jsonObject[jsonKey_status].toString().toLower()=="ok"){
            // noting
            flagOk = true;
        }
    }

    if(!flagOk){
        dataPlayList->setStar(cntStarTmp);
        setStarPointUI();

        ToastMsg::show(this,"",tr("You cannot change the star rating."));
        //ToastMsg::show(this,"",tr("별점을 변경할 수 없습니다."));
    }
}

/**
 * @brief Musice_playlist::setResultOfFavorites : 즐겨찾기(하트) 변경 요청결과 처리
 * @param p_jsonObject QJsonObject response
 */
void Musice_playlist::setResultOfFavorites(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";    
    const QString jsonKey_status = "status";

    bool flagOk = false;

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_status) && p_jsonObject[jsonKey_status].toString().toLower()=="ok"){
            // noting
            flagOk = true;
        }
    }

    if(!flagOk){
        // -------------------------------------
        //  이전 상태 되돌리기
        // -------------------------------------
        if(dataPlayList->getFavorites()==false){
            dataPlayList->setFavorites(true);
        }else{
            dataPlayList->setFavorites(false);
        }
        setFavoriteUI();

        ToastMsg::show(this,"",tr("You cannot change your favorites."));
        //ToastMsg::show(this,"",tr("즐겨찾기를 변경할 수 없습니다."));
    }
}

/**
 * @brief Musice_playlist::setResultOfAddPlaylistTrack : 플레이리스트 곡 추가 요청결과처리
 * @param p_jsonObject QJsonObject response
 */
void Musice_playlist::setResultOfAddPlaylistTrack(const QJsonObject &p_jsonObject){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_result) && p_jsonObject[jsonKey_result].toString().toLower()=="success"){
            emit linker->signal_myPlaylistChanged();
        }
    }
}

/**
 * @brief Musice_playlist::setResultOfAddNewPlaylist : 새 플레이리스트 생성 및 곡 추가 요청결과처리
 * @param p_jsonObject
 */
void Musice_playlist::setResultOfAddNewPlaylist(const QJsonObject &p_jsonObject){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_result) && p_jsonObject[jsonKey_result].toString().toLower()=="success"){
            emit linker->signal_myPlaylistChanged();
        }
    }
}

/**
 * @brief Musice_playlist::setResultOfUpdateTrackFavorites : 트랙 하트 업데이트 요청결과처리
 * @param p_jsonObject
 */
void Musice_playlist::setResultOfUpdateTrackFavorites(const int &p_index, const QJsonObject &p_jsonObject){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";

    bool flagOk = false;

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_result) && p_jsonObject[jsonKey_result].toString().toLower()=="success"){
            flagOk = true;
        }
    }

    if(!flagOk){
        changeTrackFavoritesUI(p_index);
        ToastMsg::show(this,"",tr("You cannot change your favorite settings."));
        //ToastMsg::show(this,"",tr("즐겨찾기 설정을 변경할 수 없습니다."));
    }
}

/**
 * @brief Musice_playlist::setResultOfPlayListFetch : 플레이리스트 정보요쳥결과처리
 * @param p_jsonObject QJsonObject response
 * @note favorites, star 정보만 사용
 */
void Musice_playlist::setResultOfPlayListFetch(const QJsonObject &p_jsonObject){
    const QString jsonKey_flagOk = "flagOk";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains("playlist")){
            /*QJsonObject tmp_data = p_jsonObject["playlist"].toObject();
            tmp_data.insert("isFavorite", true);    // 항상 true 로 강제 세팅. 현재 API 는 false 로 오는것이 존재함.
            dataPlayList->setData(tmp_data);*/
            this->playlist_obj = QJsonObject();
            this->playlist_obj = p_jsonObject["playlist"].toObject();
            dataPlayList->setData(this->playlist_obj);
        }
    }

    // 메뉴항목 타이틀 변경
    emit signal_changedMenuName(dataPlayList->getTitle());

    // --------------------------------------------------
    //  플레이리스트 기본 데이터 세팅
    // --------------------------------------------------
    lb_writer->setText(this->dataPlayList->getUsername());
    if(this->dataPlayList->getUsername()==global.user.getUsername()){
        // 마이 플레이리스트인 경우...
        int toshare = this->dataPlayList->getToshare().toInt(); // 0:비공개, 1: 전체공개, 2: 친구공개
        switch(toshare){
        case 0: this->lb_writer->setText(tr("Private")); break;
            //case 0: this->lb_writer->setText(tr("비공개")); break;
        case 1: this->lb_writer->setText(tr("All open")); break;
            //case 1: this->lb_writer->setText(tr("전체공개")); break;
        case 2: this->lb_writer->setText(tr("Friends open")); break;
            //case 2: this->lb_writer->setText(tr("친구공개")); break;
        }
    }

    lb_image->setImageDefault();
    if(this->type == "UNITED" && !this->thumbnail_type.isEmpty()){
        lb_image->setImageUrl(this->thumbnail_type);
    }
    else{
        lb_image->setImageUrl(dataPlayList->getConverimg());
    }
    lb_title->setText(dataPlayList->getTitle());
    lb_memo->setText(dataPlayList->getMemo());

    // 하트/별 세팅
    setFavoriteUI();
    setStarPointUI();

    // 태그 세팅
    setTagListUI();

    // 트랙정보 세팅
    for(int i = 0 ; i < this->dataPlayList->getTrackList().size(); i++){
        appendTrackUI(i);
    }
}


/**
 * @brief Musice_playlist::slot_responseHttp : [슬롯] Http 요청
 * @param p_id
 * @param p_jsonData
 */
void Musice_playlist::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData){
    if(p_id == HTTP_NETWORK_STAR){
        setResultOfStar(p_jsonData);
    }
    else if(p_id == HTTP_NETWORK_PLAYLIST_FETCH){
        // 플레이리스트 정보 세팅 (기본정보 및 트랙목록)
        setResultOfPlayListFetch(p_jsonData);
    }
    else if(p_id == HTTP_NETWORK_HEART){
        setResultOfFavorites(p_jsonData);
    }
    else if(p_id == HTTP_NETWORK_ADD_PLAYLIST_TRACK){
        setResultOfAddPlaylistTrack(p_jsonData);
    }
    else if(p_id == HTTP_NETWORK_NEW_PLAYLIST){
        setResultOfAddNewPlaylist(p_jsonData);
    }
    else if(p_id == HTTP_NETWORK_TRACK_HEART_SET){
        setResultOfUpdateTrackFavorites(sender()->property("index").toInt(), p_jsonData);
    }

    sender()->deleteLater();
}

/**
 * @brief Musice_playlist::slot_playlistClicked : [슬롯] 플레이리스트 선택 다이얼로그-플레이리스트 클릭
 * @param p_playlistno
 */
void Musice_playlist::slot_playlistClicked(const int &p_playlistno){

    if(p_playlistno >0){
        requestAddPlaylistTrack(p_playlistno, sender()->property("playurl").toString());
    }else{

        Dialog::Dialog_NewPlaylist *dlg_newPlaylist = new Dialog::Dialog_NewPlaylist(this);
        int tmp_result = dlg_newPlaylist->exec();

        if(tmp_result == QDialog::Accepted){
            requestNewPlaylistTrack(dlg_newPlaylist->getName(),sender()->property("playurl").toString());
        }

        delete dlg_newPlaylist;
    }
}

/**
 * @brief Musice_playlist::slot_delegateClicked : [슬롯] 트렉정보 delegateClick
 * @details QlistWidget item click 처리<br>
 * @param p_index
 * @param p_btnType
 */
void Musice_playlist::slot_delegateClicked(const int &p_index, const int &p_btnType){
    QListWidgetItem *item = listWidget->item(p_index);

    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    int tmp_index = map["index"].toInt();
    QString tmp_favorites = map["isFavorite"].toString();

    if(p_btnType == MusicDelegate::BtnType::etc){

        // 버튼 제외 기타 여백 클릭 => 음악 재생
        requestPlay(getPlayList(tmp_index),false);
    }
    else if(p_btnType == MusicDelegate::BtnType::playListAdd){
    }
    else if(p_btnType == MusicDelegate::BtnType::trackHeart){

        // 하트 클릭
        changeTrackFavoritesUI(tmp_index);
        requestUpdateTrackFavorites(tmp_index);
    }
    else if(p_btnType == MusicDelegate::BtnType::more){

        QJsonArray jsonArrAlbum;
        jsonArrAlbum = getPlayList(tmp_index);

        QJsonObject jsonData;
        jsonData.insert(KEY_OP_TITLE_MAIN, this->dataPlayList->getTrackList()[tmp_index].title);
        jsonData.insert(KEY_OP_TITLE_SUB, this->dataPlayList->getTrackList()[tmp_index].artist);
        if(this->dataPlayList->getFlagPlayListMy()==true){
            jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Etc_PlayListDetailTrack_My);
        }else{
            jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Etc_PlayListDetailTrack_Friend);
        }
        jsonData.insert(KEY_OP_albumImg,  this->dataPlayList->getTrackList()[tmp_index].thumb_url);
        jsonData.insert(KEY_OP_cntStar, 0);
        jsonData.insert(KEY_OP_DATA, jsonArrAlbum);
        jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::MusicNetwork);
        jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_PLAYLISTINFO);
        jsonData.insert(KEY_OP_PLAYLISTNO, playlistNo);
        emit linker->signal_clickedHoverItem(HOVER_CODE_MORE, jsonData);
    }
}

/**
 * @brief Musice_playlist::slot_thumbnailDownloaded : [슬롯] 썸네일 다운로드 완료
 */
void Musice_playlist::slot_thumbnailDownloaded(){

    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    int tmp_index = fileDownloader->property("index").toInt();
    if(tmp_index>=0 && tmp_index<listWidget->count()){
        QImage image;
        bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());
        if(flagLoaded){
            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            listWidget->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
        }
    }

    fileDownloader->deleteLater();
}

/**
 * @brief Musice_playlist::slot_clickedStar : [슬롯] 별 점수 클릭
 */
void Musice_playlist::slot_clickedStar(){

    int tmp_index = this->list_lb_star->indexOf(dynamic_cast<ClickableLabel*>(this->sender()));
    int cntStar = tmp_index+1;

    cntStarTmp = dataPlayList->getStar();
    dataPlayList->setStar(cntStar);
    setStarPointUI();

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject tmp_json;
    tmp_json.insert("id",QString::number(dataPlayList->getNo()));
    tmp_json.insert("star", cntStar);
    tmp_json.insert("local_ip", global.device.getDeviceIP());
    tmp_json.insert("mac_address", global.device.getDeviceID());

    network->request(HTTP_NETWORK_STAR, QString("%1/playlist/star").arg(global.legacy_v3_api), tmp_json, true,true);
}

/**
 * @brief Musice_playlist::slot_clickedHeart : [슬롯] 즐겨찾기(하트) 클릭 (트렉 아닌 플레이리스트앨범의 하트를 의미함)
 */
void Musice_playlist::slot_clickedHeart(){
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    if(dataPlayList->getFavorites()==false){
        dataPlayList->setFavorites(true);
    }else{
        dataPlayList->setFavorites(false);
    }
    setFavoriteUI();

    QJsonObject tmp_json;
    tmp_json.insert("favorite", this->dataPlayList->getFavorites());
    tmp_json.insert("local_ip", global.device.getDeviceIP());
    tmp_json.insert("mac_address", global.device.getDeviceID());

    network->request(HTTP_NETWORK_HEART, QString("%1/playlist/%2/favorite")
                     .arg(global.legacy_v3_api).arg(this->dataPlayList->getNo()), tmp_json, true,true);

}

/**
 * @brief Musice_playlist::slot_clickedMore [SLOT] 더보기 클릭 (기본정보에서의 더보기)
 */
void Musice_playlist::slot_clickedMore(){
    QJsonArray jsonArrAlbum = getPlayList(-1);
    QJsonObject jsonData;
    jsonData.insert(KEY_OP_TITLE_MAIN, this->dataPlayList->getTitle());
    jsonData.insert(KEY_OP_TITLE_SUB, "");
    jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Etc_PlayListDetailInfo);
    jsonData.insert(KEY_OP_albumImg,  this->dataPlayList->getConverimg());
    jsonData.insert(KEY_OP_cntStar, this->dataPlayList->getStar());
    jsonData.insert(KEY_OP_DATA, jsonArrAlbum);
    jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::MusicNetwork);
    jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_PLAYLISTINFO);

    emit linker->signal_clickedHoverItem(HOVER_CODE_MORE, jsonData);
}

/**
 * @brief Musice_playlist::slot_clickedPlay : [슬롯] 전체재생 클릭
 */
void Musice_playlist::slot_clickedPlay(){

    //requestPlay(getPlayList(-1),false);

    QJsonArray tmpArr = this->playlist_obj["tracks"].toArray();
    requestPlay(tmpArr, false);

}

/**
 * @brief Musice_playlist::slot_clickedShuffle : [슬롯] 셔플재생 클릭
 */
void Musice_playlist::slot_clickedShuffle(){

    //requestPlay(getPlayList(-1),true);

    QJsonArray tmpArr = this->playlist_obj["tracks"].toArray();
    requestPlay(tmpArr, false);

}

/**
 * @brief Musice_playlist::slot_myPlaylistTrackChanged : [슬롯] 마이플레이리스트 트랙 변경
 */
void Musice_playlist::slot_myPlaylistTrackChanged(){

    requestPlaysListFetch();
}
