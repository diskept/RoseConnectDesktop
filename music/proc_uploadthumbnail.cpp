#include "proc_uploadthumbnail.h"
#include "common/global.h"
#include <QDateTime>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardPaths>

/**
 * @brief Proc_UploadThumbnail::Proc_UploadThumbnail : 썸네일 업로드
 * @param parent
 */
Proc_UploadThumbnail::Proc_UploadThumbnail(QObject *parent)
    : QObject(parent)
{
    setInit();
}

Proc_UploadThumbnail::~Proc_UploadThumbnail(){

    this->deleteLater();
}

/**
 * @brief Proc_UploadThumbnail::setInit : 초기 세팅
 */
void Proc_UploadThumbnail::setInit(){

    this->fileDownloader = new FileDownloader(this);
    connect(this->fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_loadImage()));
}

/**
 * @brief Proc_UploadThumbnail::uploadThumnail : 썸네일 업로드 요청
 * @param p_thumbPath
 * @param p_url
 * @note 이미지 다운로드 후 업로드
 */
void Proc_UploadThumbnail::uploadThumnail(const QString &p_thumbPath, const QString &p_url){

    fileDownloader->setProperty("type","single");
    fileDownloader->setProperty("url", p_url);
    fileDownloader->setImageURL(p_thumbPath);
}

/**
 * @brief Proc_UploadThumbnail::uploadThumnail : 썸네일 업로드 요청
 * @param pixmap QPixmap
 * @param p_url QString playUrl
 */
void Proc_UploadThumbnail::uploadThumnail(const QPixmap &p_pixmap, const QString &p_url){

    pixmapThumb = p_pixmap;

    qint64 tmp_time = QDateTime::currentMSecsSinceEpoch();
    QString tmp_savedfileName = "tmp_thumb.jpg";

    //QFile file(tmp_savedfileName);
    QFile file(QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).arg(tmp_savedfileName));
    file.open(QIODevice::WriteOnly);
    pixmapThumb.save(&file, "JPG");
    file.close();

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->setBoundary("356bb3c7-a864-48f1-8dc3-5d7965188eff");

    QHttpPart playUrlPart;
    playUrlPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"playUrl\""));
    playUrlPart.setBody(p_url.toUtf8());

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

void Proc_UploadThumbnail::uploadThumbnails(const QString &p_thumbPath, const QStringList &listUrl){
    fileDownloader->setProperty("type","multiple");
    fileDownloader->setProperty("url", listUrl);
    fileDownloader->setImageURL(p_thumbPath);
}

/**
 * @brief Proc_UploadThumbnail::uploadThumbnails : 썸네일 업로드 요청
 * @param pixmap QPixmap
 * @param listUrl QStringList playUrl
 * @note 여러 파일 & 동일 썸네일
 */
void Proc_UploadThumbnail::uploadThumbnails(const QPixmap &p_pixmap, const QStringList &listUrl){
    pixmapThumb = p_pixmap;

    qint64 tmp_time = QDateTime::currentMSecsSinceEpoch();
    QString tmp_savedfileName = "tmp_thumb.jpg";

    QFile file(QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).arg(tmp_savedfileName));
    file.open(QIODevice::WriteOnly);
    pixmapThumb.save(&file, "JPG");
    file.close();

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->setBoundary("356bb3c7-a864-48f1-8dc3-5d7965188eff");

    for(int i = 0 ; i < listUrl.size(); i++){
        QHttpPart playUrlPart;
        playUrlPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"playUrl\""));
        playUrlPart.setBody(listUrl.at(i).toUtf8());

        multiPart->append(playUrlPart);
    }

    for(int i = 0 ; i < listUrl.size(); i++){
        QHttpPart macPart;
        macPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"macAddress\""));
        macPart.setBody(global.device.getDeviceID().toUtf8());
        multiPart->append(macPart);
    }

    fileUpload = new QFile(QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).arg(tmp_savedfileName));
    fileUpload->open(QIODevice::ReadOnly);

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("multipart/form-data"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\""+QString::number(tmp_time)+".jpg\""));
    imagePart.setBodyDevice(fileUpload);
    fileUpload->setParent(multiPart);



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
 * @brief Proc_UploadThumbnail::slot_replyFinished : [슬롯] QNetworkReply finisehd
 */
void Proc_UploadThumbnail::slot_replyFinished(){

    QString tmp_fileName = sender()->property("filename").toString();

    QFile tmp_file(tmp_fileName);
    if(tmp_file.exists()){
        QFile::remove(tmp_fileName);
    }
    tmp_file.close();
}

/**
 * @brief Proc_UploadThumbnail::slot_networkManagerFinished : [슬롯]QNetworkAccessManager finished
 */
void Proc_UploadThumbnail::slot_networkManagerFinished(){

    if(fileUpload->isOpen()){
        fileUpload->close();
    }
    emit signal_uploadedThumbnail();
}

/**
 * @brief Proc_UploadThumbnail::slot_loadImage : [슬롯] 이미지 로딩 완료
 * @note 플레이리스트 커버 이미지 로딩(옵션팝업에서 플레이리스트 신규 생성 후 담기 기능을 한 경우)
 */
void Proc_UploadThumbnail::slot_loadImage(){

    QString tmp_type = sender()->property("type").toString();

    QImage image;
    bool flagLoad = image.loadFromData(this->fileDownloader->downloadedData());

    if(flagLoad){
        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(image);
        if(tmp_type=="single"){
            QString tmp_playUrl = sender()->property("url").toString();
            uploadThumnail(tmp_pixmap, tmp_playUrl);
        }else{
            QStringList tmp_playUrlList = sender()->property("url").toStringList();
            uploadThumbnails(tmp_pixmap, tmp_playUrlList);
        }

    }else{
        QPixmap tmp_pixmap(":images/def_mus_200.png");

        if(tmp_type=="single"){
            QString tmp_playUrl = sender()->property("url").toString();
            uploadThumnail(tmp_pixmap, tmp_playUrl);
        }else{
            QStringList tmp_playUrlList = sender()->property("url").toStringList();
            uploadThumbnails(tmp_pixmap, tmp_playUrlList);
        }
    }

    this->fileDownloader->deleteLater();
}
