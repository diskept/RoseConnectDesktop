#include "datamusicartist.h"

/**
 * @brief DataMusicArtist::DataMusicArtist : 음악-아티스트 데이터 클래스
 * @details DB 쿼리 키값 사용
 */
DataMusicArtist::DataMusicArtist(QObject *parent)
    : QObject(parent)
{

}

DataMusicArtist::~DataMusicArtist(){

}

void DataMusicArtist::setData(const QJsonObject &p_json){
    const QString key_artist_id = "artist_id";
    const QString key_artist = "artist";
    const QString key_number_of_tracks = "number_of_tracks";
    const QString key_number_of_albums = "number_of_albums";
    const QString key_album_art = "album_art";

    if(p_json.contains(key_artist_id)){
        artist_id = p_json[key_artist_id].toInt(0);
    }
    if(p_json.contains(key_artist)){
        artist = p_json[key_artist].toString("");
    }
    if(p_json.contains(key_number_of_tracks)){
        number_of_tracks = p_json[key_number_of_tracks].toInt(0);
    }
    if(p_json.contains(key_album_art)){
        album_art = p_json[key_album_art].toString("");
    }
    if(p_json.contains(key_number_of_tracks)){
        number_of_albums = p_json[key_number_of_albums].toInt(0);
    }
}

int DataMusicArtist::getArtist_id() const
{
    return artist_id;
}

int DataMusicArtist::getNumber_of_tracks() const
{
    return number_of_tracks;
}
int DataMusicArtist::getNumber_of_albums() const
{
    return number_of_albums;
}

QString DataMusicArtist::getArtist() const
{
    return artist;
}

void DataMusicArtist::setAlbum_art(const QString &p_album_art){
    album_art = p_album_art;
}

QString DataMusicArtist::getAlbum_art() const
{
    return album_art;
}
