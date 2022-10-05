#include "proc_addtackinplaylist.h"
#include "common/networkhttp.h"
#include "common/global.h"
#include "widget/toastmsg.h"
#include <QHttpMultiPart>
#include <QPainter>
#include <QStandardPaths>

const int HTTP_NETWORK_ADD_PLAYLIST_TRACK = 89;
const int HTTP_NETWORK_NEW_PLAYLIST_TRACK = 88;

/**
 * @brief Proc_AddTackInPlayList::Proc_AddTackInPlayList : 음악 : 플레이리스트 담기 PROC
 * @details
 * 플레이리스트 선택, 플레이리스명 입력 Dlg 이용해 담기 가능</br>
 * Dlg 이용없이 담기 가능
 * @param parent
 */
Proc_AddTackInPlayList::Proc_AddTackInPlayList(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();
}

Proc_AddTackInPlayList::~Proc_AddTackInPlayList(){

    this->deleteLater();
}

/**
 * @brief Proc_AddTackInPlayList::setInit : 초기 세팅
 */
void Proc_AddTackInPlayList::setInit(){
    linker = Linker::getInstance();

    this->fileDownloader = new FileDownloader(this);
    connect(this->fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_loadImage()));
}

/**
 * @brief Proc_AddTackInPlayList::setUIControl : UI 세팅
 */
void Proc_AddTackInPlayList::setUIControl(){

    dlg_playList = new Dialog::Dialog_Playlist(Dialog::Dialog_Playlist::PlayListType::MUSIC, this);
    connect(dlg_playList, SIGNAL(signal_clickedPlaylist(int)), this, SIGNAL(signal_playlistClicked(int)));
}

/**
 * @brief Proc_AddTackInPlayList::setTrackJsonArray : 즐겨찾기에 담을 트랙 정보 JsonArray 세팅
 * @param p_jsonArray
 */
void Proc_AddTackInPlayList::setTrackJsonArray(QJsonArray p_jsonArray){
    jsonArrTracks = p_jsonArray;
}

/**
 * @brief Proc_AddTackInPlayList::requestAddPlayListTrack : 플레이리스트 담기
 * @param p_playlistId
 * @note 세팅된 QJsonArray 트랙정보 이용
 */
void Proc_AddTackInPlayList::requestAddPlayListTrack(const int &p_playlistId){
    requestAddPlayListTrack(p_playlistId, jsonArrTracks);
}

/**
 * @brief Proc_AddTackInPlayList::requestAddPlayListTrack : 플레이리스트 담기
 * @param p_playlistId
 * @param p_jsonTracks
 */
void Proc_AddTackInPlayList::requestAddPlayListTrack(const int &p_playlistId, const QJsonArray &p_jsonTracks){
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject jsonParam;
    jsonParam.insert("local_ip", global.device.getDeviceIP());
    jsonParam.insert("mac_address", global.device.getDeviceID());
    jsonParam.insert("tracks", p_jsonTracks);

    network->request(HTTP_NETWORK_ADD_PLAYLIST_TRACK, QString("%1/playlist/%2/track/add")
                     .arg(global.legacy_v3_api)
                     .arg(p_playlistId), jsonParam, true,true);
}

/**
 * @brief Proc_AddTackInPlayList::showDlgOfPlayList : 플레이리스트 선택 Dlg Show
 */
void Proc_AddTackInPlayList::showDlgOfPlayList(){
    dlg_playList->requestMyPlayList();
    dlg_playList->show();
}

/**
 * @brief Proc_AddTackInPlayList::showDlgOfNewPlayList : 신규 플레이리스트 이름 입력 Dlg Show
 */
void Proc_AddTackInPlayList::showDlgOfNewPlayList(){
    Dialog::Dialog_NewPlaylist *dlg_newPlaylist = new Dialog::Dialog_NewPlaylist(this);
    int tmp_result = dlg_newPlaylist->exec();
    if(tmp_result == QDialog::Accepted){
        emit signal_newPlaylistNameEdited(dlg_newPlaylist->getName());
    }
    delete dlg_newPlaylist;
}

/**
 * @brief Proc_AddTackInPlayList::requestNewPlaylistTrack : 플레이리스트 신규생성 후 담기
 * @param p_name
 * @note 세팅된 QJsonArray 이용<br>
 * 로즈기기에서 다운로드 받을 썸네일 정보가 있는 경우다운로드 후 플레이리스트 등록
 */
void Proc_AddTackInPlayList::requestNewPlaylistTrack(const QString &p_name){
    if(jsonArrTracks.size() > 0){
        QJsonObject jsonTrack = jsonArrTracks.at(0).toObject();

        QString tmp_playUrl = jsonTrack["url"].toString();
        QString tmp_thumb;
        QJsonArray jsonArrThumb = jsonTrack["thumbnails"].toArray();
        if(jsonArrThumb.size() > 0){
            QJsonObject jsonThumb = jsonArrThumb.at(0).toObject();
            if(jsonThumb.contains("url")){
                tmp_thumb = jsonThumb["url"].toString();
            }
        }

        if(tmp_thumb.isEmpty()){
            requestNewPlaylistTrack(p_name, jsonArrTracks);
        }else{
            QString tmp_imgURL = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_thumb);
            fileDownloader->setProperty("name", p_name);
            fileDownloader->setProperty("url", tmp_playUrl);
            fileDownloader->setImageURL(tmp_imgURL);
        }
    }
}

/**
 * @brief Proc_AddTackInPlayList::requestNewPlaylistTrack : 플레이리스트 신규생성 후 담기
 * @param p_name
 * @param p_jsonTracks
 * @note 대표 이미지 X
 */
void Proc_AddTackInPlayList::requestNewPlaylistTrack(const QString &p_name, const QJsonArray &p_jsonTracks){

    requestNewPlaylistTrack(QPixmap(), "", p_name, p_jsonTracks);
}

/**
 * @brief Proc_AddTackInPlayList::requestNewPlaylistTrack : 플레이리스트 신규생성 후 담기
 * @param QString
 * @note 대표 이미지 O
 */
void Proc_AddTackInPlayList::requestNewPlaylistTrack(const QPixmap &pixmap, const QString &p_firstPlayUrl,  const QString &p_name, const QJsonArray &p_jsonTracks){

    pixmap_albumImg = pixmap;
    playUrl = p_firstPlayUrl;

    QJsonObject jsonPlayList;
    jsonPlayList.insert("isFavorite", false);
    jsonPlayList.insert("isFavoriteTrack", false);
    jsonPlayList.insert("local_key", QString::number(QDateTime::currentMSecsSinceEpoch()));
    jsonPlayList.insert("memo","");
    jsonPlayList.insert("name", p_name);
    jsonPlayList.insert("share","2");
    jsonPlayList.insert("tags","");
    jsonPlayList.insert("star",0);
    jsonPlayList.insert("totalcount",0);
    jsonPlayList.insert("tracks", p_jsonTracks);

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject jsonParam;
    jsonParam.insert("local_ip", global.device.getDeviceIP());
    jsonParam.insert("mac_address", global.device.getDeviceID());
    jsonParam.insert("playlist",jsonPlayList);

    network->request(HTTP_NETWORK_NEW_PLAYLIST_TRACK, QString("%1/playlist/create")
                     .arg(global.legacy_v3_api), jsonParam, true,true);
}

/**
 * @brief Proc_AddTackInPlayList::saveAlbumImgToFile : 앨범 이미지 저장
 * @param p_playUrl
 * @note 저장한 이미지는 플레이리스트 커버 이미지로 서버 전송
 */
void Proc_AddTackInPlayList::saveAlbumImgToFile(const QString &p_playUrl){

    qint64 tmp_time = QDateTime::currentMSecsSinceEpoch();
    QString tmp_savedfileName = "tmp_thumb.jpg";


    QFile file(QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).arg(tmp_savedfileName));
    //QFile file(tmp_savedfileName);
    file.open(QIODevice::WriteOnly);
    pixmap_albumImg.save(&file, "JPG");
    file.close();

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->setBoundary("356bb3c7-a864-48f1-8dc3-5d7965188eff");

    QHttpPart playUrlPart;
    playUrlPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"playUrl\""));
    playUrlPart.setBody(p_playUrl.toUtf8());

    QHttpPart macPart;
    macPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"macAddress\""));
    macPart.setBody(global.device.getDeviceID().toUtf8());

    fileUpload = new QFile(QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).arg(tmp_savedfileName));
    fileUpload->open(QIODevice::ReadOnly);

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("multipart/form-data"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\""+QString::number(tmp_time)+".jpg\""));
    imagePart.setBodyDevice(fileUpload);
    fileUpload->setParent(multiPart);

    multiPart->append(playUrlPart);
    multiPart->append(macPart);
    multiPart->append(imagePart);

    QUrl url("https://api.roseaudio.kr/legacy/v3/file/trackthumbnail");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; charset=utf-8; boundary=\"356bb3c7-a864-48f1-8dc3-5d7965188eff\"");
    QString header = global.user.getToken_type()+" "+global.user.getAccess_token();
    request.setRawHeader("Authorization", header.toUtf8());


    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply *reply = manager->post(request, multiPart);
    reply->setProperty("filename", tmp_savedfileName);
    multiPart->setParent(reply);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_networkManagerFinished()));
    connect(reply, SIGNAL(finished()), this, SLOT(slot_replyFinished()));
}

/**
 * @brief AlbumDetail::setResultOfAddPlayListTrack : 플레이리스트 곡 추가 요청결과처리
 * @param p_jsonData QJsonObject response
 */
void Proc_AddTackInPlayList::setResultOfAddPlayListTrack(const QJsonObject &p_jsonData){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";
    const QString jsonKey_message = "message";

    bool flagOk = false;
    QString tmp_errorMsg = "";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_result) && p_jsonData[jsonKey_result].toString().toLower()=="success"){
            flagOk = true;
            emit linker->signal_myPlaylistChanged();
        }else{
            if(p_jsonData.contains(jsonKey_message)){
                tmp_errorMsg = p_jsonData[jsonKey_message].toString();
            }
        }
    }

    // show Error
    if(!flagOk){
        if(tmp_errorMsg.isEmpty()){
            tmp_errorMsg = tr("It cannot be added to the playlist.");
            //tmp_errorMsg = tr("플레이리스트에 추가할 수 없습니다.");
        }
        ToastMsg::show(this,"",tmp_errorMsg);
    }
}

/**
 * @brief AlbumDetail::setResultOfNewPlayListTrack : 플레이리스트 생성후 곡 추가 요청결과처리
 * @param p_jsonData QJsonObject response
 */
void Proc_AddTackInPlayList::setResultOfNewPlayListTrack(const QJsonObject &p_jsonData){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";
    const QString jsonKey_message = "message";

    bool flagOk = false;
    QString tmp_errorMsg = "";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_result) && p_jsonData[jsonKey_result].toString().toLower()=="success"){
            flagOk = true;

            if(playUrl.isEmpty()){
                emit linker->signal_myPlaylistChanged();
            }else{
                saveAlbumImgToFile(playUrl);
            }
        }else{
            if(p_jsonData.contains(jsonKey_message)){
                tmp_errorMsg = p_jsonData[jsonKey_message].toString();
            }
        }
    }

    // show Error
    if(!flagOk){
        if(tmp_errorMsg.isEmpty()){
            tmp_errorMsg = tr("You cannot create a playlist.");
            //tmp_errorMsg = tr("플레이리스트를 생성할 수 없습니다.");
        }
        ToastMsg::show(this,"",tmp_errorMsg);
    }
}


/**
 * @brief Proc_AddTackInPlayList::slot_replyFinished : [슬롯] QNetworkReply finisehd
 */
void Proc_AddTackInPlayList::slot_replyFinished(){

    QString tmp_fileName = sender()->property("filename").toString();

    QFile tmp_file(tmp_fileName);
    if(tmp_file.exists()){
        QFile::remove(tmp_fileName);
    }
}

/**
 * @brief Proc_AddTackInPlayList::slot_networkManagerFinished : [슬롯]QNetworkAccessManager finished
 */
void Proc_AddTackInPlayList::slot_networkManagerFinished(){

    if(fileUpload->isOpen()){
        fileUpload->close();
    }
    emit linker->signal_myPlaylistChanged();
}

/**
 * @brief Proc_AddTackInPlayList::slot_responseHttp : [슬롯]  Http 요청 결과 처리
 * @param p_id
 * @param p_jsonObject
 */
void Proc_AddTackInPlayList::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    switch (p_id) {
    case HTTP_NETWORK_ADD_PLAYLIST_TRACK :
        setResultOfAddPlayListTrack(p_jsonObject);
        break;
    case HTTP_NETWORK_NEW_PLAYLIST_TRACK :
        setResultOfNewPlayListTrack(p_jsonObject);
        break;
    }

    sender()->deleteLater();
}

/**
 * @brief Proc_AddTackInPlayList::slot_loadImage : [슬롯] 이미지 로딩 완료
 * @note 플레이리스트 커버 이미지 로딩(옵션팝업에서 플레이리스트 신규 생성 후 담기 기능을 한 경우)
 */
void Proc_AddTackInPlayList::slot_loadImage(){

    QString tmp_playlistName = sender()->property("name").toString();
    QString tmp_playUrl = sender()->property("url").toString();

    QImage image;
    bool flagLoad = image.loadFromData(this->fileDownloader->downloadedData());

    if(flagLoad){
        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(image);
        requestNewPlaylistTrack(tmp_pixmap, tmp_playUrl, tmp_playlistName, jsonArrTracks);
    }else{
        QPixmap tmp_pixmap(":images/def_mus_200.png");
        requestNewPlaylistTrack(tmp_pixmap, tmp_playUrl, tmp_playlistName, jsonArrTracks);
    }

    this->fileDownloader->deleteLater();
}
