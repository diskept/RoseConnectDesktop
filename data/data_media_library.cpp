#include "data_media_library.h"

/**
 * @brief 미디어 라이브러리 데이터 클래스 @n
 * @param QObject
 */
Data_Media_Library::Data_Media_Library(QObject *parent)
    : QObject(parent)
{

}

/**
 * @brief 미디어 라이브러리 데이터 클래스 @n
 * @param p_jsonData QJsonObject data
 * @param parent QObject
 */
Data_Media_Library::Data_Media_Library(const QJsonObject &p_jsonData, QObject *parent)
    : QObject(parent)
{
    setData(p_jsonData);
}

/**
 * @brief Data_Media_Library::setData : setData
 * @param p_json
 */
void Data_Media_Library::setData(const QJsonObject &p_json){

    jsonData = p_json;

    const QString key_date_finished = "date_finished";
    const QString key_dbStatus = "dbStatus";

    const QString key_info = "info";
    const QString key_ext = "ext";
    const QString key_id = "id";
    const QString key_ip = "ip";
    const QString key_isCloud = "isCloud";
    const QString key_isFile = "isFile";
    const QString key_isDir = "isDir";
    const QString key_isServer = "isServer";
    const QString key_isShare = "isShare";
    const QString key_isUp = "isUp";
    const QString key_name = "name";
    const QString key_path = "path";
    const QString key_pw = "pw";
    const QString key_rcloneServe = "rcloneServe";
    const QString key_smbParamPath = "smbParamPath";
    const QString key_thumbnail = "thumbnail";

    const QString key_numalbums = "numalbums";
    const QString key_numartists = "numartists";
    const QString key_songCnt = "songCnt";
    const QString key_videoCnt = "videoCnt";

    if(p_json.contains(key_date_finished)){
        date_finished = p_json[key_date_finished].toString();
    }
    if(p_json.contains(key_dbStatus)){
        dbStatus = p_json[key_dbStatus].toInt();
    }
    if(p_json.contains(key_numalbums)){
        numalbums = p_json[key_numalbums].toInt();
    }
    if(p_json.contains(key_numartists)){
        numartists = p_json[key_numartists].toInt();
    }
    if(p_json.contains(key_songCnt)){
        songCnt = p_json[key_songCnt].toInt();
    }
    if(p_json.contains(key_videoCnt)){
        videoCnt = p_json[key_videoCnt].toInt();
    }

    if(p_json.contains(key_info)){
        QJsonObject jsonInfo = p_json[key_info].toObject();

        if(jsonInfo.contains(key_ext)){
            ext = jsonInfo[key_ext].toString();
        }
        if(jsonInfo.contains(key_id)){
            id = jsonInfo[key_id].toString();
        }
        if(jsonInfo.contains(key_ip)){
            ip = jsonInfo[key_ip].toString();
        }
        if(jsonInfo.contains(key_isCloud)){
            isCloud = jsonInfo[key_isCloud].toBool();
        }
        if(jsonInfo.contains(key_isDir)){
            isDir = jsonInfo[key_isDir].toBool();
        }
        if(jsonInfo.contains(key_isFile)){
            isFile = jsonInfo[key_isFile].toBool();
        }
        if(jsonInfo.contains(key_isServer)){
            isServer = jsonInfo[key_isServer].toBool();
        }
        if(jsonInfo.contains(key_isShare)){
            isShare = jsonInfo[key_isShare].toBool();
        }
        if(jsonInfo.contains(key_isUp)){
            isUp = jsonInfo[key_isUp].toBool();
        }
        if(jsonInfo.contains(key_name)){
            name = jsonInfo[key_name].toString();
        }
        if(jsonInfo.contains(key_path)){
            path = jsonInfo[key_path].toString();
        }
        if(jsonInfo.contains(key_pw)){
            pw = jsonInfo[key_pw].toString();
        }
        if(jsonInfo.contains(key_rcloneServe)){
            rcloneServe = jsonInfo[key_rcloneServe].toString();
        }
        if(jsonInfo.contains(key_smbParamPath)){
            smbParamPath = jsonInfo[key_smbParamPath].toString();
        }
        if(jsonInfo.contains(key_thumbnail)){
            thumbnail = jsonInfo[key_thumbnail].toString();
        }
    }
}

/**
 * @brief Data_Media_Library::getStatus : DB 상태값 반환
 * @return
 */
QString Data_Media_Library::getStrDbStatus() const
{
    QString tmp_status;

    if(dbStatus==0){
        tmp_status = tr("Waiting");
    }
    else if(dbStatus==1){
        tmp_status = tr("Scanning");
    }
    else if(dbStatus==2){
        tmp_status = tr("Importing Tracks")+" "+QString::number(songCnt+videoCnt);
        tmp_status += " ("+tr("Audio")+" "+QString::number(songCnt);
        tmp_status += ", ";
        tmp_status += tr("Video")+" "+QString::number(videoCnt)+")";
    }else{
        tmp_status = tr("Not Connected");
    }

    return tmp_status;
}

QString Data_Media_Library::getDate_finished() const
{
    return date_finished;
}

int Data_Media_Library::getDbStatus() const
{
    return dbStatus;
}

int Data_Media_Library::getNumalbums() const
{
    return numalbums;
}

int Data_Media_Library::getNumartists() const
{
    return numartists;
}

int Data_Media_Library::getSongCnt() const
{
    return songCnt;
}

int Data_Media_Library::getVideoCnt() const
{
    return videoCnt;
}

QString Data_Media_Library::getExt() const
{
    return ext;
}

QString Data_Media_Library::getId() const
{
    return id;
}

QString Data_Media_Library::getIp() const
{
    return ip;
}

bool Data_Media_Library::getIsCloud() const
{
    return isCloud;
}

bool Data_Media_Library::getIsDir() const
{
    return isDir;
}

bool Data_Media_Library::getIsFile() const
{
    return isFile;
}

bool Data_Media_Library::getIsServer() const
{
    return isServer;
}

bool Data_Media_Library::getIsShare() const
{
    return isShare;
}

bool Data_Media_Library::getIsUp() const
{
    return isUp;
}

QString Data_Media_Library::getName() const
{
    return name;
}

QString Data_Media_Library::getPath() const
{
    return path;
}

QString Data_Media_Library::getPw() const
{
    return pw;
}

QString Data_Media_Library::getRcloneServe() const
{
    return rcloneServe;
}

QString Data_Media_Library::getSmbParamPath() const
{
    return smbParamPath;
}

QString Data_Media_Library::getThumbnail() const
{
    return thumbnail;
}

QJsonObject Data_Media_Library::getJsonData() const
{
    return jsonData;
}
