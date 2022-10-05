#include "dataplaylist.h"

#include <QJsonArray>

/**
 * @brief DataPlayList::DataPlayList : 플레이리스트 데이터 클래스
 * @details API MY 플레이리스트 키값을 변수로 사용, thumbnails 만 변경함.
 * @param parent
 */
DataPlayList::DataPlayList(QObject *parent)
    : QObject(parent)
{

}

DataPlayList::~DataPlayList(){

}

void DataPlayList::setData(const QJsonObject &p_json){
    const QString key_album_key = "album_key";
    const QString key_id = "id";
    const QString key_isFavorite = "isFavorite";
    const QString key_isFavoriteTrack = "isFavoriteTrack";
    const QString key_memo = "memo";
    const QString key_name = "name";
    const QString key_share = "share";
    const QString key_star = "star";
    const QString key_tags = "tags";
    const QString key_thumbnails = "thumbnails";
    const QString key_url = "url";
    const QString key_totalcount = "totalcount";
    const QString key_type = "type";
    const QString key_user_name = "user_name";

    if(p_json.contains(key_album_key)){
        album_key = p_json[key_album_key].toString("");
    }
    if(p_json.contains(key_id)){
        id = p_json[key_id].toInt(0);
    }
    if(p_json.contains(key_isFavorite)){
        isFavorite = p_json[key_isFavorite].toBool(false);
    }
    if(p_json.contains(key_isFavoriteTrack)){
        isFavoriteTrack = p_json[key_isFavoriteTrack].toBool(false);
    }
    if(p_json.contains(key_memo)){
        memo = p_json[key_memo].toString("");
    }
    if(p_json.contains(key_name)){
        name = p_json[key_name].toString("");
    }
    if(p_json.contains(key_share)){
        share = p_json[key_share].toString("");
    }
    if(p_json.contains(key_star)){
        star = p_json[key_star].toInt(0);
    }
    if(p_json.contains(key_tags)){
        tags = p_json[key_tags].toString("");
    }
    if(p_json.contains(key_thumbnails)){
        QJsonArray jsonArrThumb = p_json[key_thumbnails].toArray();

        if(jsonArrThumb.size() > 0){
            QJsonObject jsonThumb = jsonArrThumb.at(0).toObject();
            if(jsonThumb.contains(key_type)){
                thumb_type = jsonThumb[key_type].toString("");
            }
            if(jsonThumb.contains(key_url)){
                thumb_url = jsonThumb[key_url].toString("");
            }
        }
    }
    if(p_json.contains(key_totalcount)){
        totalcount = p_json[key_totalcount].toInt(0);
    }
    if(p_json.contains(key_type)){
        type = p_json[key_type].toString("");
    }
    if(p_json.contains(key_user_name)){
        user_name = p_json[key_user_name].toString("");
    }

}

QString DataPlayList::getAlbum_key() const
{
    return album_key;
}

/**
 * @brief DataPlayList::getId 플레이리스트 NO (PK)
 * @return
 */
int DataPlayList::getId() const
{
    return id;
}

bool DataPlayList::getIsFavorite() const
{
    return isFavorite;
}

bool DataPlayList::getIsFavoriteTrack() const
{
    return isFavoriteTrack;
}

QString DataPlayList::getMemo() const
{
    return memo;
}

QString DataPlayList::getName() const
{
    return name;
}

QString DataPlayList::getShare() const
{
    return share;
}

int DataPlayList::getStar() const
{
    return star;
}

QString DataPlayList::getTags() const
{
    return tags;
}

QString DataPlayList::getThumb_type() const
{
    return thumb_type;
}

QString DataPlayList::getThumb_url() const
{
    return thumb_url;
}

int DataPlayList::getTotalcount() const
{
    return totalcount;
}

QString DataPlayList::getType() const
{
    return type;
}

QString DataPlayList::getUser_name() const
{
    return user_name;
}
