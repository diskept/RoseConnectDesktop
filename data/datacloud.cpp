#include "data/datacloud.h"

#include "common/ProcJsonEasy.h"


/**
 * @brief DataCloud::DataCloud : 클라우드 정보
 * @details API 키값 그대로 사용
 * 관련 API : IP:PORT/get_rclone_info
 * @param parent
 */
DataCloud::DataCloud(QObject *parent)
    : QObject(parent)
{

}

void DataCloud::setData(const QJsonObject &p_json){

    const QString jsonKey_isAlias = "isAlias";
    const QString jsonKey_isCache = "isCache";
    const QString jsonKey_isCrypt = "isCrypt";
    const QString jsonKey_isDrawerPinned = "isDrawerPinned";
    const QString jsonKey_isPinned = "isPinned";
    const QString jsonKey_name = "name";
    const QString jsonKey_type = "type";
    const QString jsonKey_typeReadable = "typeReadable";
    const QString jsonKey_path = "path";
    const QString jsonKey_isInstall = "isInstall";
    const QString jsonKey_mimeType = "mimeType";

    if(p_json.contains("remote")){
        QJsonObject tmpRemote = ProcJsonEasy::getJsonObject(p_json, "remote");
        json = tmpRemote;

        if(p_json.contains(jsonKey_name)){
            name = p_json[jsonKey_name].toString();
        }
        if(p_json.contains(jsonKey_path)){
            path = p_json[jsonKey_path].toString();
        }
        if(p_json.contains(jsonKey_isInstall)){
            isInstall = p_json[jsonKey_isInstall].toBool();
        }
        if(p_json.contains(jsonKey_mimeType)){
            mimeType = p_json[jsonKey_mimeType].toString();
        }

        if(tmpRemote.contains(jsonKey_isAlias)){
            isAlias = ProcJsonEasy::getBool(tmpRemote, jsonKey_isAlias);
        }
        if(tmpRemote.contains(jsonKey_isCache)){
            isCache = ProcJsonEasy::getBool(tmpRemote, jsonKey_isCache);
        }
        if(tmpRemote.contains(jsonKey_isCrypt)){
            isCrypt = ProcJsonEasy::getBool(tmpRemote, jsonKey_isCrypt);
        }
        if(tmpRemote.contains(jsonKey_isDrawerPinned)){
            isDrawerPinned = ProcJsonEasy::getBool(tmpRemote, jsonKey_isDrawerPinned);
        }
        if(tmpRemote.contains(jsonKey_isPinned)){
            isPinned = ProcJsonEasy::getBool(tmpRemote, jsonKey_isPinned);
        }
        if(tmpRemote.contains(jsonKey_type)){
            type = ProcJsonEasy::getBool(tmpRemote, jsonKey_type);
        }
        if(tmpRemote.contains(jsonKey_typeReadable)){
            typeReadable = ProcJsonEasy::getBool(tmpRemote, jsonKey_typeReadable);
        }
    }
    else{
        json = p_json;

        if(p_json.contains(jsonKey_isAlias)){
            isAlias = p_json[jsonKey_isAlias].toBool();
        }
        if(p_json.contains(jsonKey_isCache)){
            isCache = p_json[jsonKey_isCache].toBool();
        }
        if(p_json.contains(jsonKey_isCrypt)){
            isCrypt = p_json[jsonKey_isCrypt].toBool();
        }
        if(p_json.contains(jsonKey_isDrawerPinned)){
            isDrawerPinned = p_json[jsonKey_isDrawerPinned].toBool();
        }
        if(p_json.contains(jsonKey_isPinned)){
            isPinned = p_json[jsonKey_isPinned].toBool();
        }
        if(p_json.contains(jsonKey_name)){
            name = p_json[jsonKey_name].toString();
        }
        if(p_json.contains(jsonKey_type)){
            type = p_json[jsonKey_type].toInt();
        }
        if(p_json.contains(jsonKey_typeReadable)){
            typeReadable = p_json[jsonKey_typeReadable].toString();
        }
        if(p_json.contains(jsonKey_path)){
            path = p_json[jsonKey_path].toString();
        }
        if(p_json.contains(jsonKey_isInstall)){
            isInstall = p_json[jsonKey_isInstall].toBool();
        }
        if(p_json.contains(jsonKey_mimeType)){
            mimeType = p_json[jsonKey_mimeType].toString();
        }
    }
}

bool DataCloud::getIsAlias() const
{
    return isAlias;
}

bool DataCloud::getIsCache() const
{
    return isCache;
}

bool DataCloud::getIsCrypt() const
{
    return isCrypt;
}

bool DataCloud::getIsDrawerPinned() const
{
    return isDrawerPinned;
}

bool DataCloud::getIsPinned() const
{
    return isPinned;
}

QString DataCloud::getName() const
{
    return name;
}

int DataCloud::getType() const
{
    return type;
}

QString DataCloud::getTypeReadable() const
{
    return typeReadable;
}

QString DataCloud::getPath() const
{
    return path;
}

bool DataCloud::getisisInstall() const
{
    return isInstall;
}

QString DataCloud::getMimeType() const
{
    return mimeType;
}
