#include "datavideoalbum.h"

/**
 * @brief DataVideoAlbum::DataVideoAlbum : 비디오 앨범 데이터 클래스
 * @details API 비디오앨범 키값을 변수로 사용
 * @param parent
 */
DataVideoAlbum::DataVideoAlbum(QObject *parent) : QObject(parent)
{

}

DataVideoAlbum::DataVideoAlbum(QJsonObject p_jsonData, QObject *parent) : QObject(parent)
{
    setData(p_jsonData);
}

DataVideoAlbum::~DataVideoAlbum(){

}

void DataVideoAlbum::setData(const QJsonObject &p_json){

    const QString key_album = "album";
    const QString key_albumCnt = "albumCnt";
    const QString key_id = "id";
    const QString key_videoDateaAdd = "videoDateaAdd";
    const QString key_videoDuration = "videoDuration";
    const QString key_videoFilePath = "videoFilePath";
    const QString key_videoSize = "videoSize";
    const QString key_videoTitle = "videoTitle";

    if(p_json.contains(key_album)){
        album = p_json[key_album].toString("");
    }
    if(p_json.contains(key_albumCnt)){
        albumCnt = p_json[key_albumCnt].toInt(0);
    }
    if(p_json.contains(key_id)){
        //id = p_json[key_id].toString("");
        id = QString::number(p_json[key_id].toInt());
    }
    if(p_json.contains(key_videoDateaAdd)){
        videoDateaAdd = p_json[key_videoDateaAdd].toInt(0);
    }
    if(p_json.contains(key_videoDuration)){
        // API에서 duration값은 문자열로 반환함.
        QString tmp_duration = p_json[key_videoDuration].toString();

        QRegExp regex("\\d*");
        if(!tmp_duration.isEmpty() && regex.exactMatch(tmp_duration)){
            videoDuration = tmp_duration.toInt();
        }else{
            videoDuration = 0;
        }

    }
    if(p_json.contains(key_videoFilePath)){
        videoFilePath = p_json[key_videoFilePath].toString("");
    }
    if(p_json.contains(key_videoSize)){
        videoSize = p_json[key_videoSize].toInt(0);
    }
    if(p_json.contains(key_videoTitle)){
        videoTitle = p_json[key_videoTitle].toString("");
    }
}

QString DataVideoAlbum::getAlbum() const
{
    return album;
}

int DataVideoAlbum::getAlbumCnt() const
{
    return albumCnt;
}

QString DataVideoAlbum::getId() const
{
    return id;
}

int DataVideoAlbum::getVideoDateaAdd() const
{
    return videoDateaAdd;
}

int DataVideoAlbum::getVideoDuration() const
{
    return videoDuration;
}

QString DataVideoAlbum::getVideoFilePath() const
{
    return videoFilePath;
}

int DataVideoAlbum::getVideoSize() const
{
    return videoSize;
}

QString DataVideoAlbum::getVideoTitle() const
{
    return videoTitle;
}

void DataVideoAlbum::setCoverImg(const QString &path){
    this->videoCoverImg = path;
}

QString DataVideoAlbum::getVideoCoverImg() const{
    return this->videoCoverImg;
}
