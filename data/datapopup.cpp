#include "datapopup.h"

/**
 * @brief 옵션팝업 관련 데이터 클래스 @n
 *      벅스 타이달을 제외한 옵션팝업 사용하는 대항목 필수 데이터
 * @param parent
 */
DataPopup::DataPopup(QObject *parent) : QObject(parent)
{

}

QString DataPopup::getAlbum() const
{
    return album;
}

void DataPopup::setAlbum(const QString &value)
{
    album = value;
}

QString DataPopup::getId() const
{
    return id;
}

void DataPopup::setId(const QString &value)
{
    id = value;
}

int DataPopup::getAlbum_id() const
{
    return album_id;
}

void DataPopup::setAlbum_id(int value)
{
    album_id = value;
}

QString DataPopup::getArtist() const
{
    return artist;
}

void DataPopup::setArtist(const QString &value)
{
    artist = value;
}

QString DataPopup::getTitle() const
{
    return title;
}

void DataPopup::setTitle(const QString &value)
{
    title = value;
}

QString DataPopup::getData() const
{
    return data;
}

void DataPopup::setData(const QString &value){
    data = value;
}

void DataPopup::setJsonData(const QJsonObject &p_data){
    if(p_data.contains("album")){
        this->album = p_data["album"].toString();
    }
    if(p_data.contains("album_id")){
        this->album_id = p_data["album_id"].toInt();
    }
    if(p_data.contains("id")){
        this->id = p_data["id"].toString();
        if(this->id==""){
            this->id = QString("%1").arg(p_data["id"].toInt());   // 타입이 안맞아 저장 안되는경우가 있네
        }
    }
    if(p_data.contains("data")){
        this->data = p_data["data"].toString();
    }
    if(p_data.contains("album_key")){
        this->album_key = p_data["album_key"].toString();
    }
    if(p_data.contains("artist_key")){
        this->artist_key = p_data["artist_key"].toString();
    }
    if(p_data.contains("artist_id")){
        this->artist_id = QString(p_data["artist_id"].toInt());
    }
    if(p_data.contains("title")){
        this->title = p_data["title"].toString();
    }
    if(p_data.contains("artist")){
        this->artist = p_data["artist"].toString();
    }
    if(p_data.contains("album_art")){
        this->album_art = p_data["album_art"].toString();
    }
    if(p_data.contains("album_mime")){
        this->album_mime = p_data["album_mime"].toString();
    }
    else if(p_data.contains("mime_type")){
        this->album_mime = p_data["mime_type"].toString();
    }
    if(p_data.contains("samplerate")){
        this->samplerate = p_data["samplerate"].toInt();
    }
    if(p_data.contains("bitdepth")){
        this->bitdepth = p_data["bitdepth"].toInt();
    }

    // 비디오 전용
    if(p_data.contains("duration")){
        this->duration = p_data["duration"].toInt();
    }
    if(p_data.contains("filePath")){
        this->filePath = p_data["filePath"].toString();
    }


    // 라디오 전용
    if(p_data.contains("radio_nm")){
        this->title = p_data["radio_nm"].toString();
    }
    if(p_data.contains("radio_url")){
        this->data = p_data["radio_url"].toString();
    }
    if(p_data.contains("country")){
        this->country = p_data["country"].toString();
    }
    if(p_data.contains("favorite")){
        this->favorite = p_data["favorite"].toBool();
    }
    if(p_data.contains("isSelect")){
        this->isSelect = p_data["isSelect"].toBool();
    }
    if(p_data.contains("radio_thumbnail_url")){
        this->radio_thumbnail_url = p_data["radio_thumbnail_url"].toString();
    }

    // 로즈튜브 전용
    if(p_data.contains("channelId")){
        this->channelId = p_data["channelId"].toString();
    }
    if(p_data.contains("thumbnailUrl")){
        this->channelName = p_data["channelName"].toString();
    }
    if(p_data.contains("thumbnailUrl")){
        this->thumbnailUrl = p_data["thumbnailUrl"].toString();
    }

    // 네트워크 음원재생 전용
    if(p_data.contains("ext")){
        ext = p_data["ext"].toString();
    }
    if(p_data.contains("id")){
        if(this->id==""){       // 공용으로 사용되는 key 가 있어서..
            id_loginInfo = p_data["id"].toString();
        }
    }
    if(p_data.contains("pw")){
        pw_loginInfo = p_data["pw"].toString();
    }
    if(p_data.contains("smbParamPath")){
        smbParamPath = p_data["smbParamPath"].toString();
    }
    if(p_data.contains("ip")){
        ip = p_data["ip"].toString();
    }
    if(p_data.contains("isFile")){
        isFile = p_data["isFile"].toBool();
    }
    if(p_data.contains("isServer")){
        isServer = p_data["isServer"].toBool();
    }
    if(p_data.contains("name")){
        name = p_data["name"].toString();
    }
    if(p_data.contains("path")){
        path = p_data["path"].toString();
    }
    if(p_data.contains("thumbnail")){
        thumbnail = p_data["thumbnail"].toString();
    }

    if(p_data.contains("favorite_id")){
        favorite_id = p_data["favorite_id"].toInt();
    }
}

QJsonObject DataPopup::getJsonData() const
{
    QJsonObject tmp_json;

    // 공통
    tmp_json.insert("id", id);
    tmp_json.insert("title", title);
    tmp_json.insert("album_art", album_art);

    // 뮤직 전용
    tmp_json.insert("album", album);
    tmp_json.insert("album_id", album_id);
    tmp_json.insert("data", data);
    tmp_json.insert("artist", artist);
    tmp_json.insert("album_key", album_key);
    tmp_json.insert("artist_key", artist_key);
    tmp_json.insert("artist_id", artist_id);
    tmp_json.insert("album_mime", album_mime);
    tmp_json.insert("samplerate", samplerate);
    tmp_json.insert("bitdepth", bitdepth);

    // 비디오 전용
    tmp_json.insert("duration", duration);
    tmp_json.insert("filePath", filePath);

    // 라디오 전용
    tmp_json.insert("radio_nm", title);
    tmp_json.insert("radio_url", data);
    tmp_json.insert("country", country);
    tmp_json.insert("favorite", favorite==true?1:0);
    tmp_json.insert("isSelect", isSelect);
    tmp_json.insert("radio_thumbnail_url", radio_thumbnail_url);

    // 로즈튜브 전용
    tmp_json.insert("channelId", channelId);
    tmp_json.insert("channelName", channelName);
    tmp_json.insert("thumbnailUrl", thumbnailUrl);

    // Favorite 전용
    tmp_json.insert("favorite_id", favorite_id);


    return tmp_json;
}

/**
 * @brief DataPopup::getJsonDataForNetworkPlay:네트워크 음악 재생을 위한 데이터 반환 [음악 전용]
 * @note 관련 API : ROSEIP:PORT/music_network_play
 * @details [추가 부연설명함] 현 함수는, 마이/친구 플레이리스트의 음원과 폴더의 파일 재생할때 사용되는다.
 * 플레이리스트 파일재생과 폴더의 파일재생 차이점 분석한거 적어둠.
 * ext, id, ip, pw, smbParamPath 이 5개 값만 차이가 있고 나머지는 동일하게 세팅되는것으로 확인된다.
 * 마이/친구 플레이리스트 음원 재생될때는 ext,id,pw,smbParamPath가 empty이고 ip가 로즈기기 맥어드레스 세팅된다.
 * 폴더의 파일 재생될때는 ext가 확장자, id와pw가 네트워크 로그인정보, ip가 로즈기기 ip, smbParamPath가 네트워크 쌈바접근경로URL 가 세팅된다.
 * 나머지 bool변수는 모두 동일하게 세팅되는것으로 추정된다.
 * @return
 */
QJsonObject DataPopup::getJsonDataForNetworkPlay() const
{
     QJsonObject tmp_json;
     tmp_json.insert("ext", ext);
     tmp_json.insert("id", id_loginInfo);
     tmp_json.insert("pw", pw_loginInfo);
     tmp_json.insert("smbParamPath", smbParamPath);
     tmp_json.insert("ip", ip);
     tmp_json.insert("isFile", isFile);
     tmp_json.insert("isServer", isServer);
     tmp_json.insert("name", name);
     tmp_json.insert("path", path);
     tmp_json.insert("thumbnail", thumbnail);

     // 멤버로 세팅안하는 key 들.. 고정값으로 강제세팅
     tmp_json.insert("isCloud", false);
     tmp_json.insert("isDir", false);
     tmp_json.insert("isNetworkAdd", false);
     tmp_json.insert("isShare", false);
     tmp_json.insert("isThumbNailGetting", false);
     tmp_json.insert("isUp", false);
     tmp_json.insert("rcloneServe", "");
     return tmp_json;
}

int DataPopup::getDuration() const
{
    return duration;
}

void DataPopup::setDuration(int value)
{
    duration = value;
}

QString DataPopup::getFilePath() const
{
    return filePath;
}

void DataPopup::setFilePath(const QString &value)
{
    filePath = value;
}

void DataPopup::setName(const QString &p_name){
    name = p_name;
}

void DataPopup::setPath(const QString &p_path){
    path = p_path;
}

QString DataPopup::getAlbum_art() const
{
    return album_art;
}

void DataPopup::setAlbum_art(const QString &value)
{
    album_art = value;
}

QString DataPopup::getAlbum_key() const
{
    return album_key;
}

void DataPopup::setAlbum_key(const QString &value)
{
    album_key = value;
}

QString DataPopup::getArtist_key() const
{
    return artist_key;
}

void DataPopup::setArtist_key(const QString &value)
{
    artist_key = value;
}

QString DataPopup::getArtist_id() const
{
    return artist_id;
}

void DataPopup::setArtist_id(const QString &value)
{
    artist_id = value;
}

QString DataPopup::getAlbum_mime() const
{
    return album_mime;
}

void DataPopup::setAlbum_mime(const QString &value)
{
    album_mime = value;
}

int DataPopup::getFavorite_id() const
{
    return favorite_id;
}

void DataPopup::setFavorite_id(int value)
{
    favorite_id = value;
}




