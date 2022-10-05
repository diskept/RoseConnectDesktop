#include "datavideo.h"

const QString key_ext = "ext";
const QString key_id = "id";
const QString key_ip = "ip";
const QString key_isCloud = "isCloud";
const QString key_isDir = "isDir";
const QString key_isFile = "isFile";
const QString key_isServer = "isServer";
const QString key_isShare = "isShare";
const QString key_isUp = "isUp";
const QString key_name = "name";
const QString key_path = "path";
const QString key_pw = "pw";
const QString key_smbParamPath = "smbParamPath";
const QString key_thumbnail = "thumbnail";

/**
 * @brief DataVideo::DataVideo : Video 데이터 클래스
 * @details API 비디오-폴더-USB의 키값을 변수로 사용
 * @param parent QObject
 */
DataVideo::DataVideo(QObject *parent)
    : QObject(parent)
{

}

DataVideo::~DataVideo(){

}

void DataVideo::setData(const QJsonObject &p_json){
    if(p_json.contains(key_ext)){
        ext = p_json[key_ext].toString("");
    }
    if(p_json.contains(key_id)){
        id = p_json[key_id].toString("");
    }
    if(p_json.contains(key_ip)){
        ip = p_json[key_ip].toString("");
    }
    if(p_json.contains(key_isCloud)){
        isCloud = p_json[key_isCloud].toBool(false);
    }
    if(p_json.contains(key_isDir)){
        isDir = p_json[key_isDir].toBool(false);
    }
    if(p_json.contains(key_isFile)){
        isFile = p_json[key_isFile].toBool(false);
    }
    if(p_json.contains(key_isServer)){
        isServer = p_json[key_isServer].toBool(false);
    }
    if(p_json.contains(key_isShare)){
        isShare = p_json[key_isShare].toBool(false);
    }
    if(p_json.contains(key_isUp)){
        isUp = p_json[key_isUp].toBool(false);
    }
    if(p_json.contains(key_name)){
        name = p_json[key_name].toString("");
    }
    if(p_json.contains(key_path)){
        path = p_json[key_path].toString("");
    }
    if(p_json.contains(key_pw)){
        pw = p_json[key_pw].toString("");
    }
    if(p_json.contains(key_smbParamPath)){
        smbParamPath = p_json[key_smbParamPath].toString("");
    }
    if(p_json.contains(key_thumbnail)){
        thumbnail = p_json[key_thumbnail].toString("");
    }
}

QString DataVideo::getExt() const
{
    return ext;
}

QString DataVideo::getId() const
{
    return id;
}

QString DataVideo::getIp() const
{
    return ip;
}

bool DataVideo::getIsCloud() const
{
    return isCloud;
}

bool DataVideo::getIsDir() const
{
    return isDir;
}
bool DataVideo::getIsFile() const
{
    return isFile;
}

bool DataVideo::getIsServer() const
{
    return isServer;
}

bool DataVideo::getIsShare() const
{
    return isShare;
}

bool DataVideo::getIsUp() const
{
    return isUp;
}

QString DataVideo::getName() const
{
    return name;
}

QString DataVideo::getPath() const
{
    return path;
}

QString DataVideo::getPw() const
{
    return pw;
}

QString DataVideo::getRcloneServe() const
{
    return rcloneServe;
}

QString DataVideo::getSmbParamPath() const
{
    return smbParamPath;
}

QString DataVideo::getThumbnail() const
{
    return thumbnail;
}
