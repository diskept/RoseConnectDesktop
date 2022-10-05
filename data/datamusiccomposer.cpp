#include "datamusiccomposer.h"

/**
 * @brief DataMusicComposer::DataMusicComposer : 음악-작곡가 데이터 클래스
 * @details DB 쿼리 키값 사용
 */
DataMusicComposer::DataMusicComposer(QObject *parent)
    : QObject(parent)
{

}

DataMusicComposer::~DataMusicComposer(){

}

void DataMusicComposer::setData(const QJsonObject &p_json){
    const QString key_composer = "composer";
    const QString key_number_of_tracks = "number_of_tracks";
    const QString key_album_art = "album_art";

    if(p_json.contains(key_composer)){
        composer = p_json[key_composer].toString("");
    }
    if(p_json.contains(key_number_of_tracks)){
        number_of_tracks = p_json[key_number_of_tracks].toInt(0);
    }
    if(p_json.contains(key_album_art)){
        album_art = p_json[key_album_art].toString("");
    }
}

int DataMusicComposer::getNumber_of_tracks() const
{
    return number_of_tracks;
}

QString DataMusicComposer::getComposer() const
{
    return composer;
}

void DataMusicComposer::setAlbum_art(const QString &p_album_art){
    album_art = p_album_art;
}

QString DataMusicComposer::getAlbum_art() const
{
    return album_art;
}
