#include "mediaplayerthread.h"
#include <QEventLoop>
#include <QTime>
#include <QImage>
#include <QJsonObject>
#include <common/global.h>//c220609

/**
 * @brief MediaPlayerThread::MediaPlayerThread : 미디어 정보 GET 쓰레드
 * @details 파일로부터 미디어 정보를 수집하기 위해 QMediaPlayer 사용<br>
 * QMediaPlayer 생성및 파일 URL 세팅 시 lock. 쓰레드 작업으로 전환<br>
 * 파일 로딩 후 파일명, 파일경로, 앨범커버 이미지, 재생시간, 앨범명, 아티스트 , 파일타입 정보를 시그널로 전달
 * @param p_filePath
 * @param p_index
 * @param parent
 */
MediaPlayerThread::MediaPlayerThread(const QString &p_filePath, const int &p_index, QObject *parent)
    : QThread(parent)
    , filePath(p_filePath)
    , index(p_index)
{

}

MediaPlayerThread::~MediaPlayerThread(){
    this->quit();
    this->wait();
}

/**
 * @brief MediaPlayerThread::run
 */
void MediaPlayerThread::run(){

    QMediaPlayer *player = new QMediaPlayer;
    player->setMedia(QUrl::fromLocalFile(filePath));

    QEventLoop *loop = new QEventLoop;
        connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), SLOT(slot_loadedMeidaFile(QMediaPlayer::MediaStatus)));
        connect(this, &MediaPlayerThread::signal_done,  [=](){

            if(!flagDone){
                loop->quit();
                loop->deleteLater();
                player->deleteLater();

            }
        });
    loop->exec();

}


/**
 * @brief MediaPlayerThread::slot_loadedMeidaFile : [슬롯] 미디어 파일로드
 * @details 미디어 파일 로딩이 완료된 경우에 대한 처리<br>
 * 미디어 파일 정보 GET, 앨범 이미지 pixmap 생성 후 시그널 발생<br>
 * QMediaPlayer delete
 * @param p_status
 */
void MediaPlayerThread::slot_loadedMeidaFile(QMediaPlayer::MediaStatus p_status){//c220609
//print_debug();
    if(p_status== QMediaPlayer::MediaStatus::LoadedMedia){
        QMediaPlayer *player = qobject_cast<QMediaPlayer*>(sender());

        //int tmp_duration = player->metaData("Duration").toInt();
        QTime tmp_timeDuration = QTime::fromMSecsSinceStartOfDay(player->metaData("Duration").toInt()); // QTime("00:03:27.700")
        QString tmp_author = player->metaData("Author").toString();
        QString tmp_title = player->metaData("Title").toString();
        QString tmp_albumTitle = player->metaData("AlbumTitle").toString();
        QString tmp_duration = tmp_timeDuration.toString("mm:ss");

        QImage image = player->metaData("ThumbnailImage").value<QImage>();
        QPixmap pixmap;
        if(image.isNull()){
            pixmap.load(":images/def_mus_60.png");
            //pixmap.load(":images/def_mus_200.png");
        }else{
            pixmap = QPixmap::fromImage(image);
            //pixmap = pixmap.scaled(60,60);
            //if(!global.desktopPlayerMode){//c220609
                pixmap = pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
           // }else{
           //     pixmap = pixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
           // }
        }

        QJsonObject json;
        json.insert("index", index);
        json.insert("filePath", filePath);
        json.insert("fileName", tmp_title);
        json.insert("duration", tmp_duration);
        json.insert("author", tmp_author);
        json.insert("albumTitle", tmp_albumTitle);
        json.insert("title", tmp_title);
        json.insert("isFile", true);

        emit signal_loadedMedia(json, pixmap);
        emit signal_done();
    }
}
