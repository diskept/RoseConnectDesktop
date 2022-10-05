#include "data_pc_channel.h"


/**
 * @brief 팟캐스트 채널 목록 데이터 Object @n
 *      API 팟캐스트 인기 채널 목록의 키값을 변수로 사용
 * @note collectionId 값의 경우 API 별로 type 다름. @n
 * 팟캐스트-인기 채널 목록 의 경우 int형, 팟캐스트-구독 채널 목록의 경우 String형 @n
 * @param parent
 */
Data_PC_Channel::Data_PC_Channel(QObject *parent)
    : QObject(parent)
{

}

Data_PC_Channel::Data_PC_Channel(const QJsonObject &p_json, QObject *parent)
    : QObject(parent)
{
    setData(p_json);
}

void Data_PC_Channel::setData(const QJsonObject &p_json){
    const QString key_artistName = "artistName";
    const QString key_collectionName = "collectionName";
    const QString key_feedUrl = "feedUrl";
    const QString key_artworkUrl100 = "artworkUrl100";
    const QString key_artworkUrl600 = "artworkUrl600";
    const QString key_collectionViewUrl = "collectionViewUrl";
    const QString key_collectionId = "collectionId";
    const QString key_isInit = "isInit";
    const QString key_isSubScribe = "isSubScribe";

    if(p_json.contains(key_artistName)){
        artistName = p_json[key_artistName].toString();
    }
    if(p_json.contains(key_collectionName)){
        collectionName = p_json[key_collectionName].toString();
    }
    if(p_json.contains(key_feedUrl)){
        feedUrl = p_json[key_feedUrl].toString();
    }
    if(p_json.contains(key_artworkUrl100)){
        artworkUrl100 = p_json[key_artworkUrl100].toString();
    }
    if(p_json.contains(key_artworkUrl600)){
        artworkUrl600 = p_json[key_artworkUrl600].toString();
    }
    if(p_json.contains(key_collectionViewUrl)){
        collectionViewUrl = p_json[key_collectionViewUrl].toString();
    }
    if(p_json.contains(key_collectionId)){

        // -----------------------------------------------
        //  MARK. collectionId 값 type 체크 후 String 변환
        // -----------------------------------------------
        if(p_json[key_collectionId].isString()){
            collectionId = p_json[key_collectionId].toString();
        }
        else{
            collectionId = QString::number(p_json[key_collectionId].toInt());
        }
    }
    if(p_json.contains(key_isInit)){
        isInit = p_json[key_isInit].toBool();
    }
    if(p_json.contains(key_isSubScribe)){
        isSubScribe = p_json[key_isSubScribe].toBool();
    }
}

QJsonObject Data_PC_Channel::getData()
{
    const QString key_artistName = "artistName";
    const QString key_collectionName = "collectionName";
    const QString key_feedUrl = "feedUrl";
    const QString key_artworkUrl100 = "artworkUrl100";
    const QString key_artworkUrl600 = "artworkUrl600";
    const QString key_collectionViewUrl = "collectionViewUrl";
    const QString key_collectionId = "collectionId";
    const QString key_isInit = "isInit";
    const QString key_isSubScribe = "isSubScribe";

    QJsonObject jsonData;
    jsonData.insert(key_artistName, artistName);
    jsonData.insert(key_collectionName, collectionName);
    jsonData.insert(key_feedUrl, feedUrl);
    jsonData.insert(key_artworkUrl100, artworkUrl100);
    jsonData.insert(key_artworkUrl600, artworkUrl600);
    jsonData.insert(key_collectionViewUrl, collectionViewUrl);
    jsonData.insert(key_collectionId, collectionId);
    jsonData.insert(key_isInit,isInit);
    jsonData.insert(key_isSubScribe, isSubScribe);

    return jsonData;
}

QString Data_PC_Channel::getArtistName() const
{
    return artistName;
}

QString Data_PC_Channel::getCollectionName() const
{
    return collectionName;
}

QString Data_PC_Channel::getFeedUrl() const
{
    return feedUrl;
}

QString Data_PC_Channel::getArtworkUrl100() const
{
    return artworkUrl100;
}

QString Data_PC_Channel::getArtworkUrl600() const
{
    return artworkUrl600;
}

QString Data_PC_Channel::getCollectionViewUrl() const
{
    return collectionViewUrl;
}

QString Data_PC_Channel::getCollectionId() const
{
    return collectionId;
}

bool Data_PC_Channel::getIsInit() const
{
    return isInit;
}

bool Data_PC_Channel::getIsSubScribe() const
{
    return isSubScribe;
}
