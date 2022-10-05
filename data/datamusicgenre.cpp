#include "datamusicgenre.h"

/**
 * @brief DataMusicGenre::DataMusicGenre : 음악-장르 데이터 클래스
 * @details DB 쿼리 키값 사용
 * @param parent
 */
DataMusicGenre::DataMusicGenre(QObject *parent) : QObject(parent)
{

}

DataMusicGenre::~DataMusicGenre(){

}

void DataMusicGenre::setData(const QJsonObject &p_json){
    const QString key_genre_id = "_id";
    const QString key_name = "name";
    const QString key_number_of_tracks = "numalbums";
    const QString key_album_art = "album_art";

    if(p_json.contains(key_genre_id)){
        genre_id = p_json[key_genre_id].toInt(0);
    }
    if(p_json.contains(key_name)){
        name = p_json[key_name].toString("");
    }
    if(p_json.contains(key_number_of_tracks)){
        numalbums = p_json[key_number_of_tracks].toInt(0);
    }
    if(p_json.contains(key_album_art)){
        album_art = p_json[key_album_art].toString("");
    }
}

int DataMusicGenre::getGenre_id() const
{
    return genre_id;
}

int DataMusicGenre::getNumber_of_album() const
{
    return numalbums;
}

QString DataMusicGenre::getName() const
{
    return name;
}

void DataMusicGenre::setAlbum_art(const QString &p_album_art){
    album_art = p_album_art;
}

QString DataMusicGenre::getAlbum_art() const
{
    return album_art;
}
