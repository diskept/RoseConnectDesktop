#include "data/datadevice.h"

#include "common/rosesettings.h"

#include <QJsonDocument>
#include <QSettings>
#include <QDebug>

const QString key_dbFileSize = "dbFileSize";
const QString key_deviceID = "deviceID";
const QString key_deviceIP = "deviceIP";
const QString key_deviceName = "deviceName";
const QString key_deviceRoseToken = "deviceRoseToken";
const QString key_deviceType = "deviceType";
const QString key_deviceVersion = "deviceVersion";
const QString key_isDbScanning = "isDbScanning";
const QString key_musicPlayType = "musicPlaytype";

/**
 * @brief DataDevice::DataDevice : 로즈 기기 데이터 클래스
 * @details API 장치연결의 키값을 변수로 사용
 * @param parent QObject
 */
DataDevice::DataDevice(QObject *parent) : QObject(parent)
{
}

DataDevice::~DataDevice(){

}

void DataDevice::setData(const QJsonObject &p_json){

    /*QJsonDocument doc(p_json);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug() << "ROSE DEVICE DATA RCV : " << strJson;*/

    if(p_json.contains(key_dbFileSize)){
        dbFileSize = p_json[key_dbFileSize].toDouble(0);
    }
    if(p_json.contains(key_deviceID)){
        deviceID = p_json[key_deviceID].toString("");
    }
    if(p_json.contains(key_deviceIP)){
        deviceIP = p_json[key_deviceIP].toString("");
    }
    if(p_json.contains(key_deviceName)){
        deviceName = p_json[key_deviceName].toString("");
    }
    if(p_json.contains(key_deviceRoseToken)){
        deviceRoseToken = p_json[key_deviceRoseToken].toString("");
    }
    if(p_json.contains(key_deviceType)){
        deviceType = p_json[key_deviceType].toString("");
    }
    if(p_json.contains(key_deviceVersion)){
        deviceVersion = p_json[key_deviceVersion].toString("");
    }
    if(p_json.contains(key_isDbScanning)){
        isDbScanning = p_json[key_isDbScanning].toBool(false);
    }
    if(p_json.contains(key_musicPlayType)){
        musicPlayType = p_json[key_musicPlayType].toInt(0);

        /*QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
        int index = settings->value(rosesettings.SETTINGS_TRACK_OPTION).toInt();

        int playType = 0;

        switch (index) {
            case 0:
                playType = 13;
                break;
            case 1:
                playType = 12;
                break;
            case 2:
                playType = 15;
                break;
            case 3:
                playType = 16;
                break;
            case 4:
                playType = 17;
                break;
            case 5:
                playType = 18;
                break;
            case 6:
                playType = 19;
                break;
        }

        if(musicPlayType != playType){
            int tmp_index = 0;
            switch (musicPlayType) {
                case 13:
                    tmp_index = 0;
                    break;
                case 12:
                    tmp_index = 1;
                    break;
                case 15:
                    tmp_index = 2;
                    break;
                case 16:
                    tmp_index = 3;
                    break;
                case 17:
                    tmp_index = 4;
                    break;
                case 18:
                    tmp_index = 5;
                    break;
                case 19:
                    tmp_index = 6;
                    break;
            }

            QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
            settings->setValue(rosesettings.SETTINGS_TRACK_OPTION, tmp_index);
        }*/
    }
}

void DataDevice::setData(const QVariantList &p_data){

    if(p_data.size() > 0){
        QMap<QString, QVariant> map = p_data.at(0).toMap();
        if(map.contains(key_dbFileSize)){
            dbFileSize = map[key_dbFileSize].toInt();
        }
        if(map.contains(key_deviceID)){
            deviceID = map[key_deviceID].toString();
        }
        if(map.contains(key_deviceIP)){
            deviceIP = map[key_deviceIP].toString();
        }
        if(map.contains(key_deviceName)){
            deviceName = map[key_deviceName].toString();
        }
        if(map.contains(key_deviceRoseToken)){
            deviceRoseToken = map[key_deviceRoseToken].toString();
        }
        if(map.contains(key_deviceType)){
            deviceType = map[key_deviceType].toString();
        }
        if(map.contains(key_deviceVersion)){
            deviceVersion = map[key_deviceVersion].toString();
        }
        if(map.contains(key_isDbScanning)){
            isDbScanning = map[key_isDbScanning].toBool();
        }
        if(map.contains(key_musicPlayType)){
            musicPlayType = map[key_musicPlayType].toInt();
        }


    }
}

long DataDevice::getDbFileSize() const
{
    return dbFileSize;
}

QString DataDevice::getDeviceID() const
{
    return deviceID;
}

QString DataDevice::getDeviceIP() const
{
    return deviceIP;
}

QString DataDevice::getDeviceName() const
{
    return deviceName;
}

QString DataDevice::getDeviceRoseToken() const
{
    return deviceRoseToken;
}

QString DataDevice::getDeviceType() const
{
    return deviceType;
}

QString DataDevice::getDeviceVersion() const
{
    return deviceVersion;
}

bool DataDevice::getIsDbScanning() const
{
    return isDbScanning;
}

int DataDevice::getMusicPlayType() const
{
    return this->musicPlayType;
}

void DataDevice::setDeviceType(const QString &value)
{
    deviceType = value;
}

void DataDevice::setIsDbScanning(const bool &flag){
    this->isDbScanning = flag;
}

void DataDevice::setMusicPlayType(const int &playType)
{
    this->musicPlayType = playType;
}
