#include "datanetworkfolder.h"

/**
 * @brief DataNetworkFolder::DataNetworkFolder : 네트워크 폴더 정보
 * @details API 키값 그대로 사용
 * 관련 API : IP:PORT/external_usb
 * @param parent
 */
DataNetworkFolder::DataNetworkFolder(QObject *parent)
    : QObject(parent)
{

}

void DataNetworkFolder::setData(const QJsonObject &p_json){
    const QString jsonKey_ext ="ext";
    const QString jsonKey_id = "id";
    const QString jsonKey_ip = "ip";
    const QString jsonKey_isCloud = "isCloud";
    const QString jsonKey_isDir = "isDir";
    const QString jsonKey_isFile = "isFile";
    const QString jsonKey_isServer = "isServer";
    const QString jsonKey_isShare = "isShare";
    const QString jsonKey_isUp = "isUp";
    const QString jsonKey_name = "name";
    const QString jsonKey_path = "path";
    const QString jsonKey_pw = "pw";
    const QString jsonKey_rcloneServe = "rcloneServe";
    const QString jsonKey_smbParamPath = "smbParamPath";
    const QString jsonKey_thumbnail = "thumbnail";

    if(p_json.contains(jsonKey_ext)){
        ext = p_json[jsonKey_ext].toString();
    }
    if(p_json.contains(jsonKey_id)){
        id = p_json[jsonKey_id].toString();
    }
    if(p_json.contains(jsonKey_ip)){
        ip = p_json[jsonKey_ip].toString();
    }
    if(p_json.contains(jsonKey_isCloud)){
        isCloud = p_json[jsonKey_isCloud].toBool();
    }
    if(p_json.contains(jsonKey_isDir)){
        isDir = p_json[jsonKey_isDir].toBool();
    }
    if(p_json.contains(jsonKey_isFile)){
        isFile = p_json[jsonKey_isFile].toBool();
    }
    if(p_json.contains(jsonKey_isServer)){
        isServer = p_json[jsonKey_isServer].toBool();
    }
    if(p_json.contains(jsonKey_isShare)){
        isShare = p_json[jsonKey_isShare].toBool();
    }
    if(p_json.contains(jsonKey_isUp)){
        isUp = p_json[jsonKey_isUp].toBool();
    }
    if(p_json.contains(jsonKey_name)){
        name = p_json[jsonKey_name].toString();
    }
    if(p_json.contains(jsonKey_path)){
        path = p_json[jsonKey_path].toString();
    }
    if(p_json.contains(jsonKey_pw)){
        pw = p_json[jsonKey_pw].toString();
    }
    if(p_json.contains(jsonKey_rcloneServe)){
        rcloneServe = p_json[jsonKey_rcloneServe].toString();
    }
    if(p_json.contains(jsonKey_smbParamPath)){
        smbParamPath = p_json[jsonKey_smbParamPath].toString();
    }
    if(p_json.contains(jsonKey_thumbnail)){
        thumbnail = p_json[jsonKey_thumbnail].toString();
    }
}

QJsonObject DataNetworkFolder::getData(){
    QJsonObject json;
    json.insert("ext",ext);
    json.insert("id",id);
    json.insert("ip",ip);
    json.insert("isCloud", isCloud);
    json.insert("isDir", isDir);
    json.insert("isFile", isFile);
    json.insert("isServer", isServer);
    json.insert("isShare", isShare);
    json.insert("isUp", isUp);
    json.insert("name", name);
    json.insert("path", path);
    json.insert("pw", pw);
    json.insert("rcloneServe", rcloneServe);
    json.insert("smbParamPath", smbParamPath);
    json.insert("thumbnail", thumbnail);

    return json;
}

QString DataNetworkFolder::getExt() const
{
    return ext;
}

QString DataNetworkFolder::getId() const
{
    return id;
}

QString DataNetworkFolder::getIp() const
{
    return ip;
}

bool DataNetworkFolder::getIsCloud() const
{
    return isCloud;
}

bool DataNetworkFolder::getIsDir() const
{
    return isDir;
}

bool DataNetworkFolder::getIsFile() const
{
    return isFile;
}

bool DataNetworkFolder::getIsServer() const
{
    return isServer;
}

bool DataNetworkFolder::getIsShare() const
{
    return isShare;
}

bool DataNetworkFolder::getIsUp() const
{
    return isUp;
}

QString DataNetworkFolder::getName() const
{
    return name;
}

QString DataNetworkFolder::getPath() const
{
    return path;
}

QString DataNetworkFolder::getPw() const
{
    return pw;
}

QString DataNetworkFolder::getRcloneServe() const
{
    return rcloneServe;
}

QString DataNetworkFolder::getSmbParamPath() const
{
    return smbParamPath;
}

QString DataNetworkFolder::getThumbnail() const
{
    return thumbnail;
}

void DataNetworkFolder::setName(const QString &value)
{
    name = value;
}

void DataNetworkFolder::setPath(const QString &value)
{
    path = value;
}

QString DataNetworkFolder::getConvertSmbPath() const{
    return QString("smb://%1:%2@%3/%4").arg(id).arg(pw).arg(ip).arg(path=="/" ? "" : path);
}
