#include "musiclist_collectiontrack.h"
#include "common/networkhttp.h"
#include "common/global.h"
#include "common/gscommon.h"
#include "widget/toastmsg.h"
#include "widget/optionpopup.h"
#include <common/sqlitehelper.h>
#include <QWheelEvent>
#include "common/rosesettings.h"
#include <QSettings>

const int HTTP_FAVORITE_TRACKS = 99;
const int HTTP_NETWORK_PLAY = 96;

/**
 * @brief MusicList_CollectionTrack::MusicList_CollectionTrack : 음악-마이컬렉션-트랙-ViewAll
 * @param parent
 */
MusicList_CollectionTrack::MusicList_CollectionTrack(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief MusicList_CollectionTrack::setInit : 초기 세팅
 */
void MusicList_CollectionTrack::setInit(){
    linker = Linker::getInstance();
    connect(linker, SIGNAL(signal_favoriteTrackChanged()), SLOT(slot_favoriteTrackChanged()));


    listPopup = new QList<DataPopup*>();
    delegate = new MusicCollectionTrackDelegate(this);;
    proc_addPlaylist = new Proc_AddTackInPlayList(this);

    connect(delegate, SIGNAL(signal_clickedBtn(int,int)), this, SLOT(slot_delegateClicked(int,int)));
    connect(proc_addPlaylist, SIGNAL(signal_playlistClicked(int)), this, SLOT(slot_playlistClicked(int)));
    connect(proc_addPlaylist, SIGNAL(signal_newPlaylistNameEdited(QString)), this, SLOT(slot_newPlaylistNameEdited(QString)));
}

/**
 * @brief MusicList_CollectionTrack::setUIControl : UI 세팅
 */
void MusicList_CollectionTrack::setUIControl(){

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
    box_option->setContentsMargins(0,10,0,10);
    box_option->addWidget(btn_play);
    box_option->addWidget(btn_shuffle);


    listWidget = new CustomListWidget(this);
    listWidget->setItemDelegate(delegate);
    listWidget->setCursor(Qt::PointingHandCursor);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setStyleSheet("border:none;");


    QVBoxLayout *boxContents = new QVBoxLayout;
    boxContents->setSpacing(0);
    boxContents->setContentsMargins(33,0,28,0);
    boxContents->addLayout(box_option);
    boxContents->addWidget(listWidget);

    QVBoxLayout *box = new QVBoxLayout;
    box->setSpacing(0);
    box->setContentsMargins(0,0,0,0);
    box->setAlignment(Qt::AlignTop);
    box->addLayout(boxContents);

    setLayout(box);

    // 커넥트
    connect(btn_play, SIGNAL(clicked()), this, SLOT(slot_clickedPlayAll()));
    connect(btn_shuffle, SIGNAL(clicked()), this, SLOT(slot_clickedPlayShuffle()));
}

/**
 * @brief MusicList_CollectionTrack::requestInitData : 데이터 요청
 */
void MusicList_CollectionTrack::requestInitData(){
    listPopup->clear();
    listWidget->clear();
    totalCount = 0;

    requestFavorTracks();
}

/**
 * @brief MusicList_CollectionTrack::getJsonObject_OptionPopup : 옵션팝업에 사용할 데이터 반환
 * @param p_index
 * @return
 */
QJsonObject MusicList_CollectionTrack::getJsonObject_OptionPopup(const int &p_index){
    DataPopup *data = listPopup->at(p_index);
    QString tmp_thumbnail = data->property("thumbnail").toString();
    QString tmp_url = data->property("url").toString();
    QString tmp_macAddress = data->property("mac_address").toString();

    QJsonObject jsonTracks = data->getJsonData();
    jsonTracks.insert("ip", "?"+tmp_macAddress);
    jsonTracks.insert("isFile", true);
    jsonTracks.insert("isServer", true);
    jsonTracks.insert("name", tmp_url.split("/").last());      // 파일명
    jsonTracks.insert("path", "http://127.0.0.1:7080"+(tmp_url.replace(tmp_url.split("/").last(),"")));
    jsonTracks.insert("thumbnail", tmp_thumbnail);
    QJsonArray tmp_array;
    tmp_array.append(jsonTracks);

    QJsonObject tmp_jsonObject;
    tmp_jsonObject.insert(KEY_OP_TITLE_MAIN, data->getTitle());
    tmp_jsonObject.insert(KEY_OP_TITLE_SUB, data->getArtist());
    tmp_jsonObject.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::MusicNetwork);       // 강제로 넣어주는 경우 : AbstractMainContain 페이지가 아닌 다른 페이지에서 옵션팝업을 띄우는 경우
    tmp_jsonObject.insert(KEY_OP_TYPE, data->getId().isEmpty() ? OptionPopup::TypeMenu::Music_Collection_Track_Unidentified : OptionPopup::TypeMenu::Music_Collection_Track);
    tmp_jsonObject.insert(KEY_OP_albumImg, data->getAlbum_art());
    tmp_jsonObject.insert(KEY_OP_cntStar, 0);
    tmp_jsonObject.insert(KEY_OP_DATA, tmp_array);
    tmp_jsonObject.insert("isAlbum", false);

    return tmp_jsonObject;
}


/**
 * @brief MusicList_CollectionTrack::getJsonArr_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index if -1, 앨범 전체
 * @return QJsonArray
 */
QJsonArray MusicList_CollectionTrack::getJsonArr_TrackData(const int &p_index){
    QJsonArray jsonArray;

    if(p_index<0){
        for(int i= 0 ; i < listPopup->size(); i++){
            jsonArray.append(getJsonObject_TrackData(i));
        }
    }else{
        jsonArray.append(getJsonObject_TrackData(p_index));
    }

    return jsonArray;
}

/**
 * @brief MusicList_CollectionTrack::getJsonObject_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index
 * @return QJsonObject
 */
QJsonObject MusicList_CollectionTrack::getJsonObject_TrackData(const int &p_index){

    QJsonObject jsonTrack;

    if(p_index >=0 && p_index < listPopup->size()){
        DataPopup *dataPopup = listPopup->at(p_index);

        // 앨범
        jsonTrack.insert("album", dataPopup->getAlbum());

        // 아티스트
        QJsonObject jsonArtist;
        jsonArtist.insert("name", dataPopup->getArtist());
        QJsonArray jsonArrArtists;
        jsonArrArtists.append(jsonArtist);
        jsonTrack.insert("artists", jsonArrArtists);

        // 클라이언트 아이디 = 오디오 아이디
        jsonTrack.insert("client_id", dataPopup->getId());

        // 썸네일
        QJsonObject jsonThumb;
        jsonThumb.insert("url", dataPopup->getAlbum_art());

        QJsonArray jsonArrThumbs;
        jsonArrThumbs.append(jsonThumb);
        jsonTrack.insert("thumbnails", jsonArrThumbs);

        // 타이틀
        jsonTrack.insert("title", dataPopup->getTitle());

        // 재생 URL
        jsonTrack.insert("url", dataPopup->getData());

        jsonTrack.insert("isFavorite", false);
        jsonTrack.insert("isSelected", false);
        jsonTrack.insert("mac_address", global.device.getDeviceID());
        jsonTrack.insert("type","music");
    }


    return jsonTrack;
}

/**
 * @brief MusicList_CollectionTrack::requestFavorTracks
 */
void MusicList_CollectionTrack::requestFavorTracks(){

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    int tmp_page = 0;
    tmp_page = listWidget->count()/30;


    QJsonObject json;
    json.insert("local_ip", global.device.getDeviceIP());
    json.insert("mac_address", global.device.getDeviceID());
    json.insert("permission","1");
    network->request(HTTP_FAVORITE_TRACKS, QString("%1/playlist/0?page=%2&size=30")
                     .arg(global.legacy_v3).arg(tmp_page)
                     , json, true,true);
}

/**
 * @brief MusicList_CollectionTrack::setResultOfFavoriteTrack : 사용자 즐겨찾기(하트) 트랙 요청결과 처리
 * @param p_jsonObject QJsonObject response
 */
void MusicList_CollectionTrack::setResultOfFavoriteTrack(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_playlist = "playlist";
    const QString jsonKey_totalCount = "totalcount";
    const QString jsonKey_tracks = "tracks";
    const QString jsonKey_client_id = "client_id";
    const QString jsonKey_id = "id";
    const QString jsonKey_sort = "sort";
    const QString jsonKey_star = "star";
    const QString jsonKey_thumbnails = "thumbnails";
    const QString jsonKey_title = "title";
    const QString jsonKey_type = "type";
    const QString jsonKey_url = "url";
    const QString jsonKey_macaddress = "mac_address";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_playlist)){

            QJsonObject jsonPlaylist = p_jsonObject[jsonKey_playlist].toObject();

            if(jsonPlaylist.contains(jsonKey_totalCount)){
                totalCount = jsonPlaylist[jsonKey_totalCount].toInt();
            }

            if(jsonPlaylist.contains(jsonKey_tracks)){
                QJsonArray jsonArrTrack = jsonPlaylist[jsonKey_tracks].toArray();

                int beginIndex = listWidget->count();

                for(int i=0; i < jsonArrTrack.size(); i++){
                    QJsonObject jsonTrack = jsonArrTrack.at(i).toObject();

                    QString tmp_title;
                    QString tmp_thumbnail;
                    QString tmp_url;
                    QString tmp_mac_address;
                    QString tmp_client_id="";
                    int tmp_star=0;
                    int tmp_id=0;

                    if(jsonTrack.contains(jsonKey_title)){
                        tmp_title = jsonTrack[jsonKey_title].toString();
                    }
                    if(jsonTrack.contains(jsonKey_thumbnails)){
                        QJsonArray jsonArrThumb = jsonTrack[jsonKey_thumbnails].toArray();
                        if(jsonArrThumb.size() > 0){
                            tmp_thumbnail = jsonArrThumb.at(0).toObject()[jsonKey_url].toString();
                        }
                    }
                    if(jsonTrack.contains(jsonKey_url)){
                        tmp_url = jsonTrack[jsonKey_url].toString();
                    }
                    if(jsonTrack.contains(jsonKey_macaddress)){
                        tmp_mac_address = jsonTrack[jsonKey_macaddress].toString();
                    }
                    if(jsonTrack.contains(jsonKey_client_id)){
                        tmp_client_id = jsonTrack[jsonKey_client_id].toString();
                    }
                    if(jsonTrack.contains(jsonKey_star)){
                        tmp_star = jsonTrack[jsonKey_star].toInt();
                    }
                    if(jsonTrack.contains(jsonKey_id)){
                        tmp_id = jsonTrack[jsonKey_id].toInt();
                    }

                    DataPopup *dataPopup;
                    if(!tmp_url.isEmpty()){
                        dataPopup = getTrackInfoFromDB(tmp_url);
                    }else{
                        dataPopup = new DataPopup(this);
                    }
                    dataPopup->setFavorite_id(tmp_id);
                    dataPopup->setProperty("thumbnail", tmp_thumbnail);
                    dataPopup->setProperty("url", tmp_url);
                    dataPopup->setProperty("mac_address", tmp_mac_address);

                    listPopup->append(dataPopup);

                    QJsonObject json;
                    json.insert("index", beginIndex+i);
                    json.insert("title", tmp_title);
                    json.insert("artist", dataPopup->getArtist());
                    json.insert("album", dataPopup->getAlbum());
                    json.insert("isFavorite", "0");
                    json.insert("url", tmp_url);
                    json.insert("thumbnail", tmp_thumbnail);
                    json.insert("mac_address", tmp_mac_address);
                    json.insert("client_id", tmp_client_id);
                    json.insert("star", tmp_star);
                    json.insert("duration", GSCommon::convertTimeFormat(dataPopup->getDuration()));

                    QListWidgetItem *item = new QListWidgetItem;
                    item->setData(Qt::UserRole, json);
                    item->setData(Qt::DecorationRole, QPixmap(":images/def_mus_60.png"));

                    listWidget->addItem(item);

                    FileDownloader *fileDownloader = new FileDownloader(this);
                    fileDownloader->setProperty("index", beginIndex+i);
                    fileDownloader->setImageURL(tmp_thumbnail);
                    connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));

                }
            }
        }
    }
}

/**
 * @brief MusicList_CollectionTrack::getTrackInfoFromDB : GET 트랙 정보 FROM DB
 * @param p_clientId QString audio_id
 * @return
 */
DataPopup* MusicList_CollectionTrack::getTrackInfoFromDB(const QString &p_url){

    DataPopup *dataPopup = new DataPopup(this);

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();

    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
        strQuery += " FROM audio A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
        strQuery += " WHERE A._data='"+p_url+"'";
        QVariantList dataDB;
        sqlite->exec(strQuery, dataDB);
        if(dataDB.size() >0){
            dataPopup->setJsonData(dataDB.at(0).toJsonObject());
        }
    }

    sqlite->close();
    delete sqlite;

    return dataPopup;
}

/**
 * @brief MusicList_CollectionTrack::getPlayMusicInfo : 재생음원 정보 JsonArray 형태 반환
 * @param p_index if p_index < 0, 전체 재생
 * @return QJsonArray
 */
QJsonArray MusicList_CollectionTrack::getPlayMusicInfoArray(const int &p_index){

    QJsonArray jsonArrPlay;
    if(p_index < 0){
        for(int i = 0 ; i < listWidget->count(); i++){
            QJsonObject jsonMusicObject = getPlayMusicInfoObject(i);

            if(!jsonMusicObject.empty()){
                jsonArrPlay.append(jsonMusicObject);
            }
        }
    }else{
        jsonArrPlay.append(getPlayMusicInfoObject(p_index));
    }
    return jsonArrPlay;
}

/**
 * @brief MusicFolder_USBFileList::getPlayMusicInfoObject : 재생음원 정보 JsonObject 형태 반환
 * @param p_index int index
 * @param p_dataPopup
 * @return QJsonObject
 */
QJsonObject MusicList_CollectionTrack::getPlayMusicInfoObject(const int &p_index){

    QListWidgetItem *item = listWidget->item(p_index);

    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    QString tmp_title = map["title"].toString();
    QString tmp_url = map["url"].toString();
    QString tmp_thumbnail = map["thumbnail"].toString();
    QString tmp_macAddress = map["mac_address"].toString();

    QJsonObject jsonTracks;
    jsonTracks.insert("ext","");
    jsonTracks.insert("id","");
    jsonTracks.insert("ip", "?"+tmp_macAddress);
    jsonTracks.insert("isCloud",false);
    jsonTracks.insert("isNetworkAdd",false);
    jsonTracks.insert("isFile", true);
    jsonTracks.insert("isServer", true);
    jsonTracks.insert("isShare", false);
    jsonTracks.insert("isThumbNailGetting", tmp_thumbnail.isEmpty() ? true : false);
    jsonTracks.insert("name", tmp_url.split("/").last());      // 파일명
    jsonTracks.insert("path", "http://127.0.0.1:7080"+(tmp_url.replace(tmp_url.split("/").last(),"")));
    jsonTracks.insert("thumbnail", tmp_thumbnail);
    jsonTracks.insert("pw","");
    jsonTracks.insert("rcloneServe","");
    jsonTracks.insert("smbParamPath","");

    return jsonTracks;
}

/**
 * @brief MusicList_CollectionTrack::playMusic : 음원 재생
 * @param jsonArrMusic QJsonArray 재생음원 정보
 * @param flagPlayShuffle bool if true, 셔플재생
 */
void MusicList_CollectionTrack::playMusic(QJsonArray jsonArrMusic, bool flagPlayShuffle){
    QJsonObject jsonData;
    jsonData.insert("musicPlayType",15);
    jsonData.insert("music", jsonArrMusic);
    jsonData.insert("playType",20);
    jsonData.insert("shuffle", flagPlayShuffle? 1 : 0);
    jsonData.insert("roseToken", global.device.getDeviceRoseToken());

    if(global.Queue_track_count != 0) {
        print_debug();emit linker->signal_checkQueue(27, "");

        return;
    }
    print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
    global.Queue_track_count += jsonArrMusic.count();     // 220419 queue count

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3")
                     .arg(global.device.getDeviceIP())
                     .arg(global.port).arg("music_network_play"), jsonData, true);
}

/**
 * @brief MusicList_CollectionTrack::slot_thumbnailDownloaded : [슬롯] 썸네일 다운로드 완료
 */
void MusicList_CollectionTrack::slot_thumbnailDownloaded(){

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
 * @brief MusicList_CollectionTrack::responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void MusicList_CollectionTrack::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id == HTTP_FAVORITE_TRACKS){
        setResultOfFavoriteTrack(p_jsonObject);
        flagReqMore = false;
    }

   sender()->deleteLater();
}

/**
 * @brief MusicList_CollectionTrack::slot_delegateClicked : [슬롯] 트렉정보 delegateClick
 * @details QlistWidget item click 처리<br>
 * @param p_index
 * @param p_btnType
 */
void MusicList_CollectionTrack::slot_delegateClicked(const int &p_index, const int &p_btnType){

    QListWidgetItem *item = listWidget->item(p_index);

    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    //int tmp_index = map["index"].toInt();
    QString tmp_title = map["title"].toString();
    QString tmp_url = map["url"].toString();
    QString tmp_thumbnail = map["thumbnail"].toString();
    QString tmp_macAddress = map["mac_address"].toString();

    if(p_btnType == MusicCollectionTrackDelegate::BtnType::etc){
        playMusic(getPlayMusicInfoArray(p_index));
    }
    else if(p_btnType == MusicCollectionTrackDelegate::BtnType::playListAdd){
        // 플레이리스트 담기
        if(p_index >=0 && p_index < listPopup->size() && !listPopup->at(p_index)->getId().isEmpty()){
            proc_addPlaylist->setProperty("index", p_index);
            proc_addPlaylist->showDlgOfPlayList();
        }else{
            ToastMsg::show(this,"",tr("You cannot add playlists."));
            //ToastMsg::show(this,"",tr("플레이리스트 담기를 할 수 없습니다."));
        }
    }
    else if(p_btnType == MusicCollectionTrackDelegate::BtnType::more){
        // 더보기
        if(p_index >=0 && p_index < listPopup->size()){
            emit linker->signal_clickedHoverItem(HOVER_CODE_MORE, getJsonObject_OptionPopup(p_index));
        }else{
            ToastMsg::show(this,"",tr("The More option is not available."));
            //ToastMsg::show(this,"",tr("더보기 옵션을 사용할 수 없습니다."));
        }
    }
}

/**
 * @brief MusicList_CollectionTrack::slot_playlistClicked : [슬롯] 플레이리스트 선택 다이얼로그-플레이리스트 클릭
 * @param p_playlistno
 */
void MusicList_CollectionTrack::slot_playlistClicked(const int &p_playlistno){

    int tmp_index = sender()->property("index").toInt();

    if(p_playlistno >0){
        proc_addPlaylist->setTrackJsonArray(getJsonArr_TrackData(tmp_index));
        proc_addPlaylist->requestAddPlayListTrack(p_playlistno);
    }else{
        proc_addPlaylist->showDlgOfNewPlayList();
    }
}

/**
 * @brief MusicList_CollectionTrack::slot_newPlaylistNameEdited : [슬롯] 신규 플레이리스트 명 입력 완료
 * @param p_name
 * @note 신규 플레이리스트 생성
 */
void MusicList_CollectionTrack::slot_newPlaylistNameEdited(QString p_name){

    int tmp_index = sender()->property("index").toInt();

    proc_addPlaylist->setTrackJsonArray(getJsonArr_TrackData(tmp_index));
    proc_addPlaylist->requestNewPlaylistTrack(p_name);

}

/**
 * @brief MusicList_CollectionTrack::wheelEvent : 휠 이벤트
 * @param event
 */
void MusicList_CollectionTrack::wheelEvent(QWheelEvent *event){

    // 더 가져오기 실행
    if(event->delta() < 0){
        if(!flagReqMore && listWidget->count() < totalCount){
            flagReqMore = true;
            requestFavorTracks();
        }
    }
}

/**
 * @brief MusicList_CollectionTrack::slot_favoriteTrackChanged : [슬롯] Favorite 트랙 변경
 */
void MusicList_CollectionTrack::slot_favoriteTrackChanged(){
    requestInitData();
}

/**
 * @brief MusicList_CollectionTrack::slot_clickedPlayAll : [슬롯] 전체 재생
 */
void MusicList_CollectionTrack::slot_clickedPlayAll(){
    playMusic(getPlayMusicInfoArray(-1));
}

/**
 * @brief MusicList_CollectionTrack::slot_clickedPlayShuffle : [슬롯] 셔플 재생
 */
void MusicList_CollectionTrack::slot_clickedPlayShuffle(){
    playMusic(getPlayMusicInfoArray(-1),true);
}
