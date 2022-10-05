#include "dataartist.h"

const QString key_artist = "artist";
const QString key_id = "_id";
const QString key_number_of_albums = "number_of_albums";
const QString key_number_of_tracks = "number_of_tracks";
const QString key_album_art = "album_art";

/**
 * @brief DataArtist::DataArtist : 아티스트 데이터 클래스
 * @details API 추천아티스트 목록의 키값을 변수로 사용
 * @param parent
 */
DataArtist::DataArtist(QObject *parent) : QObject(parent)
{

}

void DataArtist::setData(const QJsonObject &p_json){

    if(p_json.contains(key_artist)){
        artist = p_json[key_artist].toString("");
    }
    if(p_json.contains(key_id)){
        _id = p_json[key_id].toInt(0);
    }
    if(p_json.contains(key_number_of_albums)){
        number_of_albums = p_json[key_number_of_albums].toInt(0);
    }
    if(p_json.contains(key_number_of_tracks)){
        number_of_tracks = p_json[key_number_of_tracks].toInt(0);
    }
    if(p_json.contains(key_album_art)){
        album_art = p_json[key_album_art].toString("");
    }
}

QString DataArtist::getArtist() const
{
    return artist;
}

int DataArtist::getId() const
{
    return _id;
}

QString DataArtist::getHead() const
{
    return head;
}

int DataArtist::getNumber_of_albums() const
{
    return number_of_albums;
}

int DataArtist::getNumber_of_tracks() const
{
    return number_of_tracks;
}

QString DataArtist::getAlbum_art() const
{
    return album_art;
}

