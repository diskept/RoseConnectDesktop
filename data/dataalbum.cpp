#include "dataalbum.h"

#include "common/ProcJsonEasy.h"

const QString key_album = "album";
const QString key_album_art = "album_art";
const QString key_album_id = "album_id";
const QString key_album_key = "album_key";
const QString key_artist = "artist";
const QString key_artist_key = "artist_key";
const QString key_composer = "composer";
const QString key_composercnt = "composercnt";
const QString key_data = "data";
const QString key_duration = "duration";
const QString key_gener_song_cnt = "gener_song_cnt";
const QString key_isSelected = "isSelected";
const QString key_num_of_song = "num_of_song";
const QString key_num_of_tracks = "num_of_tracks";
const QString key_title = "title";
const QString key_track_id = "track_id";
const QString key_id = "id";
const QString key_favorite = "favorite";
const QString key_album_mime_type = "mime_type";
const QString key_album_samplerate = "samplerate";
const QString key_album_bitdepth = "bitdepth";
const QString key_section = "section";

/**
 * @brief DataAlbum::DataAlbum : 앨범 데이터 클래스
 * @details API 최근앨범목록의 키값을 변수로 사용
 * @param parent QObject
 */
DataAlbum::DataAlbum(QObject *parent)
    : QObject(parent)
{

}

DataAlbum::~DataAlbum(){

}

QJsonObject DataAlbum::getDataTotal(){

    QJsonObject tmp_json;
    tmp_json.insert(key_album, this->album);
    tmp_json.insert(key_album_art, this->album_art);
    tmp_json.insert(key_album_id, this->album_id);
    tmp_json.insert(key_album_key, this->album_key);
    tmp_json.insert(key_artist, this->artist);
    tmp_json.insert(key_artist_key, this->artist_key);
    tmp_json.insert(key_composer, this->composer);
    tmp_json.insert(key_composercnt, this->composercnt);
    tmp_json.insert(key_data, this->data);
    tmp_json.insert(key_duration, this->duration);
    tmp_json.insert(key_gener_song_cnt, this->gener_song_cnt);
    tmp_json.insert(key_isSelected, this->isSelected);
    tmp_json.insert(key_num_of_song, this->num_of_song);
    tmp_json.insert(key_num_of_tracks, this->num_of_tracks);
    tmp_json.insert(key_title, this->title);
    tmp_json.insert(key_track_id, this->track_id);
    tmp_json.insert(key_id, this->id);
    tmp_json.insert(key_album_mime_type, this->mime_type);
    tmp_json.insert(key_album_samplerate, this->sample_rate);
    tmp_json.insert(key_album_bitdepth, this->bit_depth);
    return tmp_json;
}
/**
 * @brief DataAlbum::setData : 데이터 세팅
 * @param p_json QJsonObject API 데이터
 */
void DataAlbum::setData(const QJsonObject &p_json){

    this->album_key = ProcJsonEasy::getString(p_json, "album_key");
    this->artist_key = ProcJsonEasy::getString(p_json, "artist_key");
    this->composer = ProcJsonEasy::getString(p_json, "composer");
    this->data = ProcJsonEasy::getString(p_json, "data");
    this->duration = ProcJsonEasy::getString(p_json, "duration");
    this->id = ProcJsonEasy::getString(p_json, "id");
    this->track_id = ProcJsonEasy::getString(p_json, "track_id");

    this->artist_id = ProcJsonEasy::getInt(p_json, "artist_id");
    this->composercnt = ProcJsonEasy::getInt(p_json, "composercnt");
    this->gener_song_cnt = ProcJsonEasy::getInt(p_json, "gener_song_cnt");
    this->num_of_song = ProcJsonEasy::getInt(p_json, "num_of_song");

    this->isSelected = ProcJsonEasy::getInt(p_json, "isSelected");

    this->album = ProcJsonEasy::getString(p_json, "album");
    this->album_art = ProcJsonEasy::getString(p_json, "album_art");
    this->album_id = ProcJsonEasy::getInt(p_json, "album_id");
    this->artist = ProcJsonEasy::getString(p_json, "artist");
    if(p_json.contains("bitdepth")){
        this->bit_depth = ProcJsonEasy::getInt(p_json, "bitdepth");
    }
    else{
        this->bit_depth = ProcJsonEasy::getInt(p_json, "bit_depth");
    }
    this->num_of_song = ProcJsonEasy::getInt(p_json, "num_of_song");
    if(p_json.contains("samplerate")){
        this->sample_rate = ProcJsonEasy::getInt(p_json, "samplerate");
    }
    else{
        this->sample_rate = ProcJsonEasy::getInt(p_json, "sample_rate");
    }

    this->section = ProcJsonEasy::getString(p_json, "section");
    this->title = ProcJsonEasy::getString(p_json, "title");

    if(p_json.contains("mime_type")){
        QString tmp_data = ProcJsonEasy::getString(p_json, "mime_type");
        QStringList splitToken = tmp_data.split("/");


        if(splitToken.size() > 1){
            QString tmp_type = splitToken.at(1);
            if(tmp_type == "x-wav" || tmp_type == "wav"){
                this->mime_type = "WAV";
            }
            else if (tmp_type == "wv") {
                this->mime_type = "WV";
            }
            else if(tmp_type == "flac"){
                this->mime_type = "FLAC";
            }
            else if(tmp_type == "dff" || tmp_type == "dsf"){
                this->mime_type = "DSD";
            }
            else if(tmp_type == "mp4"){
                this->mime_type = "ALAC";
            }
            else if(tmp_type == "mpeg"){
                this->mime_type = "MP3";
            }
            else if(tmp_type == "amr"){
                this->mime_type = "AMR";
            }
            else if(tmp_type == "amr-wb"){
                this->mime_type = "AWB";
            }
            else if(tmp_type == "x-ms-wma"){
                this->mime_type = "WMA";
            }
            else if(tmp_type == "ape"){
                this->mime_type = "APE";
            }
            else if(tmp_type == "ogg"){
                this->mime_type = "OGG";
            }
            else if(tmp_type == "aac" || tmp_type == "aac-adts"){
                this->mime_type = "AAC";
            }
            else if(tmp_type == "x-matroska"){
                this->mime_type = "MKA";
            }
            else if(tmp_type == "midi"){
                this->mime_type = "MIDI";
            }
            else if(tmp_type == "sp-midi"){
                this->mime_type = "SMF";
            }
            else if(tmp_type == "imelody"){
                this->mime_type = "IMY";
            }
            else if(tmp_type == "x-mpegurl"){
                this->mime_type = "M3U8";
            }
            else if(tmp_type == "x-scpls"){
                this->mime_type = "PLS";
            }
            else if(tmp_type == "aiff"){
                this->mime_type = "AIFF";
            }
        }
    }
}

QString DataAlbum::getAlbum() const
{
    return album;
}

void DataAlbum::setAlbum_art(const QString &p_album_art){
    album_art = p_album_art;
}

QString DataAlbum::getAlbum_art() const
{
    return album_art;
}

int DataAlbum::getAlbum_id() const
{
    return album_id;
}

QString DataAlbum::getAlbum_key() const
{
    return album_key;
}

QString DataAlbum::getArtist() const
{
    return artist;
}

int DataAlbum::getArtist_id() const
{
    return artist_id;
}

QString DataAlbum::getArtist_key() const
{
    return artist_key;
}

QString DataAlbum::getComposer() const
{
    return composer;
}

int DataAlbum::getComposercnt() const
{
    return composercnt;
}

QString DataAlbum::getData() const
{
    return data;
}

QString DataAlbum::getDuration() const
{
    return duration;
}

int DataAlbum::getGener_song_cnt() const
{
    return gener_song_cnt;
}

QString DataAlbum::getId() const
{
    return id;
}

bool DataAlbum::getIsSelected() const
{
    return isSelected;
}

int DataAlbum::getNum_of_song() const
{
    return num_of_song;
}

int DataAlbum::getNum_of_tracks() const
{
    return num_of_tracks;
}

QString DataAlbum::getTitle() const
{
    return title;
}

QString DataAlbum::getTrack_id() const
{
    return track_id;
}

int DataAlbum::getFavorite() const
{
    return favorite;
}

QString DataAlbum::getSection() const{
    return section;
}

QString DataAlbum::getMimeType() const{
    return mime_type;
}

int DataAlbum::getSamplerate() const{
    return sample_rate;
}

int DataAlbum::getBitdepth() const{
    return bit_depth;
}
