#include "desktopmediaplayerthread.h"
#include <QEventLoop>
#include <QTime>
#include <QImage>
#include <QJsonObject>
#include <QJsonDocument>
#include <common/global.h>

/**
 * @brief MediaPlayerThread::MediaPlayerThread : 미디어 정보 GET 쓰레드
 * @details 파일로부터 미디어 정보를 수집하기 위해 QMediaPlayer 사용<br>
 * QMediaPlayer 생성및 파일 URL 세팅 시 lock. 쓰레드 작업으로 전환<br>
 * 파일 로딩 후 파일명, 파일경로, 앨범커버 이미지, 재생시간, 앨범명, 아티스트 , 파일타입 정보를 시그널로 전달
 * @param p_filePath
 * @param p_index
 * @param parent
 */
DesktopMediaPlayerThread::DesktopMediaPlayerThread(const QString &p_filePath, QObject *parent)
    : QThread(parent)//c220609
    , filePath(p_filePath)
{
    linker = Linker::getInstance();

    connect(linker, SIGNAL(siganl_desktopModeStart()), this, SLOT(slot_desktopModeStart()));//c220704
    connect(linker, SIGNAL(signal_playMusic_desktop(QJsonArray, bool)), this, SLOT(slot_playMusic_desktop(QJsonArray, bool)));//c220704

}

DesktopMediaPlayerThread::~DesktopMediaPlayerThread(){
    this->working = false;
    this->quit();
    this->wait(5000);
}




void DesktopMediaPlayerThread::run()
{
    print_debug();

    int tmp_duration = global.music_player->metaData("Duration").toInt();
    //connect(music_player, SIGNAL(mediaChanged(QUrl::fromLocalFile(filePath))), SLOT(slot_loadedMeidaFile()));

    while(working)
    {

        DesktopModetimePlayValue();
        tmp_duration = global.music_player->metaData("Duration").toInt();
        if(tmp_duration > global.music_player->position()){
            //
        }else{
            //global.music_player->stop();
            emit signal_stopThread();
           // qDebug() << "thread-quit";
        }
        QThread::sleep(1);
        //qDebug() << global.music_player->state();
    }

}

void DesktopMediaPlayerThread::select_table(QString str_table){
    print_debug();
    qDebug() << "str_table=" << str_table;
    SqliteHelper *sqliteHelperSel = new SqliteHelper();
    QSqlError err = sqliteHelperSel->addConnectionLocal();
    QVariantList data = QVariantList();
    if(err.type() == QSqlError::NoError){
        print_debug();
        QString strQuery_select = "SELECT  *";
        strQuery_select += " FROM ";
        strQuery_select += QString(" %1").arg(str_table);

        qDebug() << "strQuery_insert="<<strQuery_select;
        sqliteHelperSel->exec(strQuery_select, data);
        qDebug() << str_table << " table:data.size()=" << data.size();
        global.music_playlist->clear();
        if(data.size() > 0){
            for(int i =0; i < data.size(); i++){
                QJsonObject jsonData = data.at(i).toJsonObject();
                QJsonDocument doc1(jsonData);  QString strJson(doc1.toJson(QJsonDocument::Compact));  qDebug() << " DirFileListPage::select_table---jsonData_obj = " << strJson;

                if(jsonData.contains("row")&&jsonData.contains("filePath")){
                    if(jsonData["row"].toInt() >0 && !jsonData["filePath"].toString().isEmpty()){
                        global.music_playlist->insertMedia(i, QUrl::fromLocalFile(jsonData["filePath"].toString()));
                    }

                }
            }
            //print_debug();

        }else{
           // print_debug();

        }

    }
    sqliteHelperSel->close();
    delete sqliteHelperSel;

    qDebug() << "global.music_playlist->mediaCount()=" << global.music_playlist->mediaCount();


}


void DesktopMediaPlayerThread::DesktopModetimePlayValue(){//c220525


    // 로즈기기의 현재 재생 컨트롤러 정보 가져오기
    //if(!global.device.getDeviceIP().isEmpty()){

    if(global.desktopPlayerMode){
       //print_debug();
       //qDebug() << global.music_player;
        global.music_player->setVolume(global.setTextVolumn);
        //QTime tmp_duration = QTime::fromMSecsSinceStartOfDay(global.music_player->metaData("Duration").toInt()); // QTime("00:03:27.700")
       // qDebug() << "tmp_duration = " << tmp_duration;

       // qDebug() << "music_player->state()= " << global.music_player->state();
       // qDebug() << "music_player->position()= " << global.music_player->position();
        QJsonObject tmp_json;//isPlaying
        QJsonObject tmp_json_data;
        if(QMediaPlayer::PlayingState == global.music_player->state()){
            tmp_json_data.insert("isPlaying", true);
        }else{
            tmp_json_data.insert("isPlaying", false);
        }
        //qDebug() << global.music_player->currentMedia().canonicalUrl().path();
//global.repeatMode
        tmp_json_data.insert("URL", global.music_player->currentMedia().canonicalUrl().path() );
        tmp_json_data.insert("playType","MUSIC");
        tmp_json_data.insert("repeatMode",global.repeatMode);
        tmp_json_data.insert("shuffleMode",global.shuffleMode);
        tmp_json_data.insert("curPosition", QString("%1").arg(global.music_player->position()));
        tmp_json_data.insert("artistName", global.music_player->metaData("Author").toString());
        tmp_json_data.insert("AlbumTitle", global.music_player->metaData("AlbumTitle").toString());

        tmp_json_data.insert("titleName", global.music_player->metaData("Title").toString());
        if(global.music_player->metaData("AlbumTitle").toString()==nullptr || global.music_player->metaData("Title").toString()==nullptr){
            tmp_json_data.insert("titleName", global.music_player->currentMedia().canonicalUrl().path() );
            tmp_json_data.insert("artistName", tr("your's collection"));
        }
        tmp_json_data.insert("duration", QString("%1").arg(global.music_player->metaData("Duration").toInt()));
        //tmp_json_data.insert("albumImg", tmp_duration.toString("mm:ss"));
        tmp_json.insert("data", tmp_json_data);
        tmp_json.insert("flagOk", true);
        QImage image = global.music_player->metaData("ThumbnailImage").value<QImage>();
        QPixmap pixmap;
        if(image.isNull()){
            pixmap.load(":images/def_mus_60.png");
            //pixmap.load(":images/def_mus_200.png");
        }else{
            pixmap = QPixmap::fromImage(image);
            //pixmap = pixmap.scaled(60,60);
            pixmap = pixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        //QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nMusicFolder_USB::slot_timePlayValue()--p_jsonObject=" << strJson  << "\n";

        emit linker->signal_localPlayMusicInfo(tmp_json, pixmap);

    }


}

void DesktopMediaPlayerThread::stop()
{
    //this->working = false;
    this->quit();
    //this->wait(5000);
}

void DesktopMediaPlayerThread::slot_playMusic_desktop(QJsonArray jsonArrMusic, bool flagPlayShuffle){//c220615
    print_debug();
#if 0  //DESKTOP_MODE   //c220711
    //if(global.desktopPlayerMode){
        for(int i = 0 ; i < jsonArrMusic.count(); i++){//c220615
            QJsonObject tmp_json111;
            tmp_json111 = jsonArrMusic[i].toObject();
            print_debug();
            QJsonDocument doc(tmp_json111);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nMediaFileTree::playMusic--p_jsonObject=" << strJson  << "\n";

        }

        QJsonObject examobjtmp;//cheon120812-iso
        //jsonArrMusic_curr = jsonArrMusic;
        examobjtmp = jsonArrMusic[0].toObject();//cheon120812-iso
        qDebug() <<  "MediaFileTree::playMusic--ext : " << examobjtmp["ext"].toString();//cheon120812-iso
        qDebug() << "MediaFileTree::playMusic---global.isoflag=" << global.isoflag;//cheon120812-iso

        QStringList splitToken = examobjtmp["path"].toString().split("@");
        //-----------------------------------------------------------
        //QString temp = "//192.168.1.4/Video";
        //QString temp = "\\\\192.168.1.4\\Media1\\Music1\\pop\\\"[24Bit  Vinyl Rip] 엔야 Enya -  Shepherd Moons 1991\"";
        QString temp = QString("//%1/%2").arg(splitToken.at(1)).arg(examobjtmp["name"].toString());
        qDebug() << "temp="<< temp;
        QFileInfo check_file(temp);

        if(check_file.exists()){
            qDebug() << "QFileInfo--check--OK";
        }else{
            qDebug() << "QFileInfo--check--Fail";
        }
        global.music_player->setMedia(QUrl::fromLocalFile(temp));
        //global.music_player->setMedia(QUrl(temp));
       // global.music_player->setVolume(global.setTextVolumn);
        global.music_player->play();
        //---------------------------------------------------------------
        /*
         * //global.music_playlist->insertMedia(row, QUrl::fromLocalFile(tmp_filePath));
    qDebug()<<"global.setTextVolumn = "<< global.setTextVolumn;

    //global.musicPlaylistMediaContent
    global.desktopPlayerMode = true;
    global.music_playlist->setCurrentIndex(index.toInt());
    //global.music_player->setMedia(QUrl::fromLocalFile(p_fileInfo_filePath));

    global.music_player->setVolume(global.setTextVolumn);

    //global.music_player->play();//아래의 커넥트 선언으로 한곡 반복 플레이

    emit linker->siganl_desktopModeStart();
    */
/*
        QString temp = "\\\\192.168.1.4\\Media1\\Music1\\pop\\\"[24Bit  Vinyl Rip] 엔야 Enya -  Shepherd Moons 1991\\\"05 Angeles.flac\"";
        QFileInfo check_file(temp);

        if(check_file.exists()){
            qDebug() << "QFileInfo--check--OK";
        }else{
            qDebug() << "QFileInfo--check--Fail";
        }
        global.music_player->setMedia(QUrl::fromLocalFile(temp));
        //global.music_player->setMedia(QUrl(temp));
       // global.music_player->setVolume(global.setTextVolumn);
        global.music_player->play();
       // print_debug();
    //}
*/
#endif
}

/**
 * @brief MediaPlayerThread::slot_loadedMeidaFile : [슬롯] 미디어 파일로드
 * @details 미디어 파일 로딩이 완료된 경우에 대한 처리<br>
 * 미디어 파일 정보 GET, 앨범 이미지 pixmap 생성 후 시그널 발생<br>
 * QMediaPlayer delete
 * @param p_status
 */
void DesktopMediaPlayerThread::slot_loadedMeidaFile(){

    //if(p_status== QMediaPlayer::MediaStatus::LoadedMedia){
        QMediaPlayer *player = qobject_cast<QMediaPlayer*>(sender());

        int tmp_duratio1n = player->metaData("Duration").toInt();
        print_debug();
        qDebug() << "tmp_duratio1n=" << tmp_duratio1n;
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
            pixmap = pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        QJsonObject json;
       // json.insert("index", index);
        json.insert("filePath", filePath);
        json.insert("fileName", tmp_title);
        json.insert("duration", tmp_duration);
        json.insert("author", tmp_author);
        json.insert("albumTitle", tmp_albumTitle);
        json.insert("title", tmp_title);
        json.insert("isFile", true);

       // emit signal_loadedMedia(json, pixmap);
        //emit signal_done();
    //}
}
