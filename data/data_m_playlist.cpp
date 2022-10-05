#include "data_m_playlist.h"
#include "common/gscommon.h"

#include <QJsonArray>

/**
 * @brief 음악 플레이리스트 데이터 클래스 @n
 *      플레이리스의 트랙 정보를 struct로 관리
 */
Data_M_PlayList::Data_M_PlayList(QObject *parent) : QObject(parent)
{

}

void Data_M_PlayList::initData(){

    this->raw_jsonData = QJsonObject();
    this->raw_arr_thumbs = QJsonArray();
    this->raw_arr_tracks = QJsonArray();

    this->flagPlayListMy = false;
    this->albumKey = "";
    this->memo = "";
    this->isFavorite = false;
    this->isFavoriteTrack = false;
    this->id = 0;
    this->isRose = false;
    this->ownerId = 0;
    this->username = "";
    this->registDateTime = "";
    this->share = "0";
    this->sort = 0;
    this->star = 0;
    this->thumbup = 0;
    this->thumbupCount = 0;
    this->name = "";
    this->type = "";
    this->trackCount = 0;
    this->tags.clear();
    this->list_dataTracks.clear();

}

/**
 * @brief Data_M_PlayList::setData
 * @param p_json QJsonObject data
 */
void Data_M_PlayList::setData(const QJsonObject &p_json){

    this->initData();

    this->raw_jsonData = p_json;

    if(p_json.contains("flagPlayListMy")){
        this->flagPlayListMy = p_json["flagPlayListMy"].toBool();   // API 에서 던저주는 키가 아니다.. 내부적으로 구분하기 위해서 추가된 키 이다.
    }

    if(p_json.contains("album_key")){
        this->albumKey = p_json["album_key"].toString();
    }
    else if(p_json.contains("clientKey")){
        this->albumKey = p_json["clientKey"].toString();
    }
    if(p_json.contains("memo")){
        this->memo = p_json["memo"].toString();
    }
    else if(p_json.contains("comment")){
        this->memo = p_json["comment"].toString();
    }
    if(p_json.contains("isFavorite")){
        this->isFavorite = p_json["isFavorite"].toBool();
    }
    else if(p_json.contains("favorite")){
        this->isFavorite = p_json["favorite"].toBool();
    }
    if(p_json.contains("isFavoriteTrack")){
        this->isFavoriteTrack = p_json["isFavoriteTrack"].toBool();
    }
    else if(p_json.contains("favoriteTrack")){
        this->isFavoriteTrack = p_json["favoriteTrack"].toBool();
    }
    if(p_json.contains("id")){
        this->id = p_json["id"].toInt();
    }
    if(p_json.contains("isRose")){
        this->isRose = p_json["isRose"].toBool();
    }
    if(p_json.contains("ownerId")){
        this->ownerId = p_json["ownerId"].toInt();
    }
    if(p_json.contains("user_name")){
        this->username = p_json["user_name"].toString();
    }
    else if(p_json.contains("ownerName")){
        this->username = p_json["ownerName"].toString();
    }
    else if(p_json.contains("registDateTime")){
        this->registDateTime = p_json["registDateTime"].toString();
    }
    if(p_json.contains("share")){
        // 0:비공개, 1: 전체공개, 2: 친구공개
        //this->share = p_json["share"].toString();

        // 0:PRIVATE, 1:ALL, 2:FRIEND
        if(p_json["share"].toString() == "ALL"){
            this->share = "1";
        }
        else if(p_json["share"].toString() == "FRIEND"){
            this->share = "2";
        }
        else{
            this->share = "0";
        }
    }
    if(p_json.contains("sort")){
        this->sort = p_json["sort"].toInt();
    }
    if(p_json.contains("star")){
        this->star = p_json["star"].toInt();
    }
    if(p_json.contains("tags")){
        if(p_json["tags"].toString()!=""){
            QStringList strList = p_json["tags"].toString().split(",");
            for(int i = 0 ; i < strList.count(); i++){
                tags.append(strList.at(i));
            }
        }
    }
    if(p_json.contains("thumbnails")){
        this->raw_arr_thumbs = p_json["thumbnails"].toArray();
    }
    else if(p_json.contains("thumbnail")){
        //this->thumnail = (p_json["thumbnail"].toString());
        QJsonObject tmp_thumbnail = QJsonObject();
        tmp_thumbnail.insert("url", p_json["thumbnail"].toString());
        this->raw_arr_thumbs.append(tmp_thumbnail);
    }
    if(p_json.contains("thumbup")){
        this->thumbup = p_json["thumbup"].toBool();
    }
    if(p_json.contains("thumbupCount")){
        this->thumbupCount = p_json["thumbupCount"].toInt();
    }
    if(p_json.contains("name")){
        this->name = p_json["name"].toString();
    }
    else if(p_json.contains("title")){
        this->name = p_json["title"].toString();
    }
    if(p_json.contains("type")){
        this->type = p_json["type"].toString();
    }
    if(p_json.contains("trackCount")){
        this->trackCount = p_json["trackCount"].toInt();
    }

    if(p_json.contains("tracks")){
        QJsonArray tmp_arr = p_json["tracks"].toArray();
        this->raw_arr_tracks = tmp_arr;
        for(int i=0; i<tmp_arr.count(); i++){
            QJsonObject tmp_dataJson = tmp_arr.at(i).toObject();
            sDATA_M_Track tmp_dataTrack;
            if(tmp_dataJson.contains("album")){
                tmp_dataTrack.album = tmp_dataJson["album"].toString();
            }
            else if(tmp_dataJson.contains("albums")){
                QJsonArray tmp_arrAlbum = tmp_dataJson["albums"].toArray();
                if(tmp_arrAlbum.count() > 0){
                    tmp_dataTrack.album = tmp_arrAlbum.at(0).toString();
                }
            }
            if(tmp_dataJson.contains("artists")){
                QJsonArray tmp_arrArtist = tmp_dataJson["artists"].toArray();
                if(tmp_arrArtist.count()>0){
                    if(tmp_arrArtist.first().toObject().contains("name")){
                        tmp_dataTrack.artist = tmp_arrArtist.first().toObject()["name"].toString();
                    }
                    else{
                        tmp_dataTrack.artist = tmp_arrArtist.at(0).toString();
                    }
                }
            }
            if(tmp_dataJson.contains("client_id")){
                tmp_dataTrack.client_id = tmp_dataJson["client_id"].toString();
            }
            else if(tmp_dataJson.contains("clientKey")){
                tmp_dataTrack.client_id = tmp_dataJson["clientKey"].toString();
            }
            if(p_json.contains("comment")){
                tmp_dataTrack.comment = p_json["comment"].toString();
            }
            if(tmp_dataJson.contains("mac_address")){
                tmp_dataTrack.mac_address = tmp_dataJson["mac_address"].toString();
            }
            else if(tmp_dataJson.contains("data")){
                QJsonObject tmpData = tmp_dataJson["data"].toObject();
                tmp_dataTrack.mac_address = tmpData["macAddress"].toString();
            }
            if(tmp_dataJson.contains("duration")){
                tmp_dataTrack.duration = tmp_dataJson["duration"].toInt();
            }
            if(tmp_dataJson.contains("isFavorite")){
                tmp_dataTrack.isFavorite = tmp_dataJson["isFavorite"].toBool();
            }
            else if(tmp_dataJson.contains("favorite")){
                tmp_dataTrack.isFavorite = tmp_dataJson["favorite"].toBool();
            }
            if(tmp_dataJson.contains("id")){
                tmp_dataTrack.id = tmp_dataJson["id"].toInt();
            }
            if(tmp_dataJson.contains("ownerId")){
                tmp_dataTrack.ownerId = tmp_dataJson["ownerId"].toInt();
            }
            if(tmp_dataJson.contains("ownerName")){
                tmp_dataTrack.ownerName = tmp_dataJson["ownerName"].toString();
            }
            if(tmp_dataJson.contains("playCount")){
                tmp_dataTrack.playCount = tmp_dataJson["playCount"].toInt();
            }
            if(tmp_dataJson.contains("url")){
                tmp_dataTrack.file_url = tmp_dataJson["url"].toString();
            }
            else if(tmp_dataJson.contains("playUrl")){
                tmp_dataTrack.file_url = tmp_dataJson["playUrl"].toString();
            }
            if(tmp_dataJson.contains("playlistId")){
                tmp_dataTrack.playlistId = tmp_dataJson["playlistId"].toInt();
            }
            if(tmp_dataJson.contains("sort")){
                tmp_dataTrack.sort = tmp_dataJson["sort"].toInt();
            }
            if(tmp_dataJson.contains("star")){
                tmp_dataTrack.star = tmp_dataJson["star"].toInt();
            }
            if(tmp_dataJson.contains("thumbnails")){
                QJsonArray tmp_arrThumbs = tmp_dataJson["thumbnails"].toArray();
                if(tmp_arrThumbs.count()>0){
                    if(tmp_arrThumbs.first().toObject().contains("url")){
                        tmp_dataTrack.thumb_url = tmp_arrThumbs.first().toObject()["url"].toString();
                    }
                }
            }
            else if(tmp_dataJson.contains("thumbnailUrl")){
                tmp_dataTrack.thumb_url = tmp_dataJson["thumbnailUrl"].toString();
            }
            if(tmp_dataJson.contains("title")){
                tmp_dataTrack.title = tmp_dataJson["title"].toString();
            }
            if(tmp_dataJson.contains("type")){
                tmp_dataTrack.type = tmp_dataJson["type"].toString();
            }

            this->list_dataTracks.append(tmp_dataTrack);
        }
    }
}

QJsonObject Data_M_PlayList::getData(){
    return raw_jsonData;
}


/**
 * @brief Data_M_PlayList::getTrackJsonArrayOfIndex 해당 index의 Track데이터를 QJsonArray 형태로 반환한다.
 * @param p_index
 * @return
 */
QJsonArray Data_M_PlayList::getTrackJsonArrayOfIndex(int p_index){
    QJsonArray tmp_array;
    if(p_index>=0 && p_index<this->raw_arr_tracks.count()){
        tmp_array.append(this->raw_arr_tracks[p_index].toObject());
    }
    return tmp_array;
}


/**
 * @brief Data_M_PlayList::getJsonDataTrackForOptionPopup 옵션팝업에 트랙정보를 세팅하기 위한.. 한 개 트랙정보의 json 반환
 * @param p_index
 * @return
 */
QJsonObject Data_M_PlayList::getJsonDataTrackForOptionPopup(int p_index){
    QJsonObject jsonTracks;
    if(p_index>=0 && p_index<this->raw_arr_tracks.count()){
        jsonTracks.insert("ext","");            // 플레이리스트에서는 empty
        jsonTracks.insert("id","");             // 플레이리스트에서는 empty
        jsonTracks.insert("pw","");             // 플레이리스트에서는 empty
        jsonTracks.insert("smbParamPath","");   // 플레이리스트에서는 empty
        jsonTracks.insert("ip", this->list_dataTracks.at(p_index).mac_address);
        jsonTracks.insert("isFile", true);
        jsonTracks.insert("isServer", true);
        jsonTracks.insert("title", this->list_dataTracks.at(p_index).title);
        jsonTracks.insert("name", this->list_dataTracks.at(p_index).title);
        jsonTracks.insert("path", this->list_dataTracks.at(p_index).file_url);
        jsonTracks.insert("thumbnail", this->list_dataTracks.at(p_index).thumb_url);
    }
    return jsonTracks;
}

QString Data_M_PlayList::getConverimg(){

    QString tmp_imgPath = "";
    if(this->raw_arr_thumbs.count() > 0){
        QJsonObject tmp_data = this->raw_arr_thumbs.first().toObject();
        if(tmp_data.contains("url")){
            tmp_imgPath = tmp_data["url"].toString();
        }
    }
    else if(this->raw_arr_thumbs.count() == 0){
        tmp_imgPath = this->thumnail;
    }
    return tmp_imgPath;
}
