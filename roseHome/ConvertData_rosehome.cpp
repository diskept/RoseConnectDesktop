#include "ConvertData_rosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/global.h"

#include <QDebug>
#include <QException>
#include <common/sqlitehelper.h>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

namespace roseHome {

    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about AlbumItemData
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief AlbumItemData struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData::getObjectJson_albumData(const roseHome::AlbumItemData &data){

        QJsonObject jsonObj;
        jsonObj.insert("id", data.id);
        jsonObj.insert("star", data.star);
        jsonObj.insert("totalCount", data.totalCount);
        jsonObj.insert("trackId", data.trackId);

        jsonObj.insert("type", data.type);
        jsonObj.insert("title", data.title);
        jsonObj.insert("thumbnail", data.thumbnail);
        jsonObj.insert("playTime", data.playTime);
        jsonObj.insert("clientKey", data.clientKey);
        jsonObj.insert("macAddress", data.macAddress);

        jsonObj.insert("favorite", data.favorite);

        return jsonObj;
    }

    /**
     * @brief JsonObject을 AlbumItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    roseHome::AlbumItemData ConvertData::convertData_albumData(const QJsonObject &jsonObj){

        roseHome::AlbumItemData data_output;
        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        data_output.star = ProcJsonEasy::getInt(jsonObj, "star");
        data_output.totalCount = ProcJsonEasy::getInt(jsonObj, "totalCount");
        data_output.trackCount = ProcJsonEasy::getInt(jsonObj, "trackCount");

        data_output.type = ProcJsonEasy::getString(jsonObj, "type");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.thumbnail = ProcJsonEasy::getString(jsonObj, "thumbnail");
        data_output.playTime = ProcJsonEasy::getString(jsonObj, "playTime");
        data_output.clientKey = ProcJsonEasy::getString(jsonObj, "clientKey");

        if(jsonObj.contains("data")){
            QJsonObject tmpData = ProcJsonEasy::getJsonObject(jsonObj, "data");

            data_output.trackId = ProcJsonEasy::getInt(tmpData, "trackId");
            data_output.macAddress = ProcJsonEasy::getString(tmpData, "macAddress");
        }
        else{
            data_output.trackId = ProcJsonEasy::getInt(jsonObj, "trackId");
            data_output.macAddress = ProcJsonEasy::getString(jsonObj, "macAddress");
        }

        data_output.favorite = ProcJsonEasy::getBool(jsonObj, "favorite");

        return data_output;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about PlaylistItemData
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief PlaylistItemData struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData::getObjectJson_playlistData(const roseHome::PlaylistItemData &data){

        QJsonObject jsonObj;
        jsonObj.insert("id", data.id);
        jsonObj.insert("ownerId", data.ownerId);
        jsonObj.insert("sort", data.sort);
        jsonObj.insert("star", data.star);
        jsonObj.insert("thumbupCount", data.thumbupCount);
        jsonObj.insert("trackCount", data.trackCount);
        jsonObj.insert("trackId", data.trackId);
        jsonObj.insert("totalCount", data.totalCount);

        jsonObj.insert("clientKey", data.clientKey);
        jsonObj.insert("comment", data.comment);
        jsonObj.insert("ownerName", data.ownerName);
        jsonObj.insert("playTime", data.playTime);
        jsonObj.insert("registDateTime", data.registDateTime);
        jsonObj.insert("share", data.share);
        jsonObj.insert("thumbnail", data.thumbnail);
        jsonObj.insert("title", data.title);
        jsonObj.insert("type", data.type);
        jsonObj.insert("macAddress", data.macAddress);

        jsonObj.insert("favorite", data.favorite);
        jsonObj.insert("favoriteTrack", data.favoriteTrack);
        jsonObj.insert("isRose", data.isRose);
        jsonObj.insert("thumbup", data.thumbup);

        QJsonArray jsonArr_tags;
        foreach(QString tmp_tag, data.tags){
            jsonArr_tags.append(tmp_tag);
        }
        jsonObj.insert("tags", jsonArr_tags);

        return jsonObj;
    }

    /**
     * @brief JsonObject을 PlaylistItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    roseHome::PlaylistItemData ConvertData::convertData_playlistData(const QJsonObject &jsonObj){

        roseHome::PlaylistItemData data_output;
        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        data_output.ownerId = ProcJsonEasy::getInt(jsonObj, "ownerId");
        data_output.sort = ProcJsonEasy::getInt(jsonObj, "sort");
        data_output.star = ProcJsonEasy::getInt(jsonObj, "star");
        data_output.thumbupCount = ProcJsonEasy::getInt(jsonObj, "thumbupCount");
        data_output.trackCount = ProcJsonEasy::getInt(jsonObj, "trackCount");
        data_output.trackId = ProcJsonEasy::getInt(jsonObj, "trackId");
        data_output.totalCount = ProcJsonEasy::getInt(jsonObj, "totalCount");

        data_output.clientKey = ProcJsonEasy::getString(jsonObj, "clientKey");
        data_output.comment = ProcJsonEasy::getString(jsonObj, "comment");
        data_output.ownerName = ProcJsonEasy::getString(jsonObj, "ownerName");
        data_output.playTime = ProcJsonEasy::getString(jsonObj, "playTime");
        data_output.registDateTime = ProcJsonEasy::getString(jsonObj, "registDateTime");
        data_output.share = ProcJsonEasy::getString(jsonObj, "share");
        data_output.thumbnail = ProcJsonEasy::getString(jsonObj, "thumbnail");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.type = ProcJsonEasy::getString(jsonObj, "type");
        data_output.macAddress = ProcJsonEasy::getString(jsonObj, "macAddress");

        data_output.favorite = ProcJsonEasy::getBool(jsonObj, "favorite");
        data_output.favoriteTrack = ProcJsonEasy::getBool(jsonObj, "favoriteTrack");
        data_output.isRose = ProcJsonEasy::getBool(jsonObj, "isRose");
        data_output.thumbup = ProcJsonEasy::getBool(jsonObj, "thumbup");

        if(data_output.share == "ALL"){
            data_output.share = "Public";
        }
        else if(data_output.share == "FRIEND"){
            data_output.share = "Friend";
        }
        else if(data_output.share == "PRIVATE"){
            data_output.share = "Private";
        }

        if(data_output.type == "YOUTUBE" && data_output.ownerName == "rose"){
            data_output.ownerName = "rosetube";
        }

        QVariantList tmp_tags = ProcJsonEasy::getJsonArray(jsonObj, "tags").toVariantList();
        foreach(QVariant tmp_tag, tmp_tags){
            data_output.tags.append(tmp_tag.value<QString>());
        }

        return data_output;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about TrackItemData
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief TrackItemData struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData::getObjectJson_trackData(const roseHome::TrackItemData &data){

        QJsonObject jsonObj;
        jsonObj.insert("duration", data.duration);
        jsonObj.insert("id", data.id);
        jsonObj.insert("ownerId", data.ownerId);
        jsonObj.insert("playCount", data.playCount);
        jsonObj.insert("sort", data.sort);
        jsonObj.insert("star", data.star);
        jsonObj.insert("thumbupCount", data.thumbupCount);
        jsonObj.insert("trackCount", data.trackCount);
        jsonObj.insert("album_id", data.album_id);

        jsonObj.insert("clientKey", data.clientKey);
        jsonObj.insert("comment", data.comment);
        jsonObj.insert("ownerName", data.ownerName);
        jsonObj.insert("playUrl", data.playUrl);
        jsonObj.insert("thumbnail", data.thumbnail);
        jsonObj.insert("title", data.title);
        jsonObj.insert("type", data.type);
        jsonObj.insert("album_title", data.album_title);

        jsonObj.insert("favorite", data.favorite);
        jsonObj.insert("favoriteTrack", data.favoriteTrack);
        jsonObj.insert("isRose", data.isRose);
        jsonObj.insert("thumbup", data.thumbup);

        QJsonArray jsonArr_name;
        foreach(QString tmp_name, data.list_artist_name){
            jsonArr_name.append(tmp_name);
        }
        jsonObj.insert("list_artist_name", jsonArr_name);

        return jsonObj;
    }


    /**
     * @brief JsonObject을 TrackItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    roseHome::TrackItemData ConvertData::convertData_trackItemData(const QJsonObject &jsonObj){

        roseHome::TrackItemData data_output;
        /*data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        data_output.ownerId = ProcJsonEasy::getInt(jsonObj, "ownerId");
        data_output.playCount = ProcJsonEasy::getInt(jsonObj, "playCount");
        data_output.sort = ProcJsonEasy::getInt(jsonObj, "sort");
        data_output.star = ProcJsonEasy::getInt(jsonObj, "star");
        data_output.thumbupCount = ProcJsonEasy::getInt(jsonObj, "thumbupCount");
        data_output.trackCount = ProcJsonEasy::getInt(jsonObj, "trackCount");
        data_output.album_id = ProcJsonEasy::getInt(jsonObj, "album_id");

        data_output.clientKey = ProcJsonEasy::getString(jsonObj, "clientKey");
        data_output.comment = ProcJsonEasy::getString(jsonObj, "comment");
        data_output.ownerName = ProcJsonEasy::getString(jsonObj, "ownerName");
        data_output.playUrl = ProcJsonEasy::getString(jsonObj, "playUrl");
        data_output.thumbnail = ProcJsonEasy::getString(jsonObj, "thumbnail");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.type = ProcJsonEasy::getString(jsonObj, "type");
        data_output.album_title = ProcJsonEasy::getString(jsonObj, "album_title");

        data_output.favorite = ProcJsonEasy::getBool(jsonObj, "favorite");
        data_output.favoriteTrack = ProcJsonEasy::getBool(jsonObj, "favoriteTrack");
        data_output.isRose = ProcJsonEasy::getBool(jsonObj, "isRose");
        data_output.thumbup = ProcJsonEasy::getBool(jsonObj, "thumbup");

        QVariantList tmp_list_name = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_name").toVariantList();
        foreach(QVariant tmp_val, tmp_list_name){
            data_output.list_artist_name.append(tmp_val.value<QString>());
        }*/

        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        data_output.ownerId = ProcJsonEasy::getInt(jsonObj, "ownerId");
        data_output.playCount = ProcJsonEasy::getInt(jsonObj, "playCount");
        data_output.sort = ProcJsonEasy::getInt(jsonObj, "sort");
        data_output.star = ProcJsonEasy::getInt(jsonObj, "star");

        data_output.clientKey = ProcJsonEasy::getString(jsonObj, "clientKey");
        data_output.comment = ProcJsonEasy::getString(jsonObj, "comment");
        data_output.ownerName = ProcJsonEasy::getString(jsonObj, "ownerName");
        data_output.playUrl = ProcJsonEasy::getString(jsonObj, "playUrl");
        data_output.thumbnail = ProcJsonEasy::getString(jsonObj, "thumbnailUrl");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.type = ProcJsonEasy::getString(jsonObj, "type");

        data_output.favorite = ProcJsonEasy::getBool(jsonObj, "favorite");

        QJsonArray artist_jsonArr = ProcJsonEasy::getJsonArray(jsonObj, "artists");

        for(int i = 0; i < artist_jsonArr.count(); i++){
            data_output.list_artist_name.append(artist_jsonArr.at(i).toString());
        }

        if(jsonObj.contains("data")){
            QJsonObject data_obj = ProcJsonEasy::getJsonObject(jsonObj, "data");
            QJsonObject album_obj = ProcJsonEasy::getJsonObject(data_obj, "album");

            data_output.album_title = ProcJsonEasy::getString(album_obj, "title");
            data_output.album_id = ProcJsonEasy::getInt(album_obj, "id");
            data_output.favoriteTrack = ProcJsonEasy::getBool(data_obj, "isFavoriteTrack");

            if(data_output.type == "YOUTUBE"){

                if(data_obj.contains("detailContents")){
                    QJsonObject contents_obj = ProcJsonEasy::getJsonObject(data_obj, "detailContents");

                    QString duration = ProcJsonEasy::getString(contents_obj, "duration");
                    QString time = duration;
                    QString hour;
                    QString minute;
                    QString second;

                    if(duration.indexOf("PT") >= 0){
                        time = duration.right(duration.size() - 2);
                    }
                    if(time.indexOf("H") >= 0){
                        hour = time.section("H", 0, -2);
                        time = time.mid(time.indexOf("H") + 1);
                    }
                    if(time.indexOf("M") >= 0){
                        minute = time.section("M", 0, -2);
                        time = time.mid(time.indexOf("M") + 1);
                    }
                    if(time.indexOf("S") >= 0){
                        second = time.section("S", 0, -2);
                    }


                    if(hour.isEmpty()){
                        if(minute.isEmpty()){
                            data_output.duration = second.rightJustified(2,'0').toInt();
                        }
                        else{
                            data_output.duration = (minute.toInt() * 60) + second.rightJustified(2,'0').toInt();
                        }
                    }
                    else{
                        data_output.duration = (hour.toInt() * 60) + (minute.toInt() * 60) + second.rightJustified(2,'0').toInt();
                    }
                }

                if(data_obj.contains("snippet")){
                    QJsonObject snippet_obj = ProcJsonEasy::getJsonObject(data_obj, "snippet");

                    data_output.album_title = ProcJsonEasy::getString(snippet_obj, "channelTitle");
                    data_output.album_id = ProcJsonEasy::getInt(snippet_obj, "channelId");
                    data_output.favoriteTrack = ProcJsonEasy::getBool(snippet_obj, "isFavoriteTrack");
                }
            }
            else if(data_output.type == "TIDAL"){

                if(data_output.duration == 0){
                    data_output.duration = ProcJsonEasy::getInt(data_obj, "duration");
                }

                if(data_output.album_title.isEmpty()){
                    data_output.album_title = ProcJsonEasy::getString(data_obj, "title");
                }

                if(data_output.list_artist_name.size() == 0){
                    QJsonArray artist_jsonArr = ProcJsonEasy::getJsonArray(data_obj, "artists");

                    for(int i = 0; i < artist_jsonArr.count(); i++){
                        data_output.list_artist_name.append(artist_jsonArr.at(i).toObject()["name"].toString());
                    }
                }

                QString audioQuality = ProcJsonEasy::getString(data_obj, "audioQuality");

                if(audioQuality == "HI_RES"){
                    data_output.hires = true;
                }
            }
            else if(data_output.type == "BUGS"){

                if(data_obj.contains("len")){
                    QString tmp_split = "";
                    QStringList splitToken;

                    tmp_split = ProcJsonEasy::getString(data_obj, "len");
                    splitToken = tmp_split.split(":");

                    int tmp_duration = 0;

                    if(splitToken.size() > 2){
                        tmp_duration = (splitToken.at(0).toInt() * 3600) + (splitToken.at(1).toInt() * 60) + splitToken.at(2).toInt();
                    }
                     else if(splitToken.size() == 2){
                        tmp_duration = (splitToken.at(0).toInt() * 60) + splitToken.at(1).toInt();
                    }

                    data_output.duration = tmp_duration;
                }

                if(data_obj.contains("mv_title")){
                    data_output.album_title = ProcJsonEasy::getString(data_obj, "mv_title");
                }
                else{
                    if(data_output.album_title.isEmpty()){
                        data_output.album_title = ProcJsonEasy::getString(data_obj, "title");
                    }
                }

                if(data_output.list_artist_name.size() == 0){
                    QJsonArray artist_jsonArr = ProcJsonEasy::getJsonArray(data_obj, "artists");

                    for(int i = 0; i < artist_jsonArr.count(); i++){
                        data_output.list_artist_name.append(artist_jsonArr.at(i).toObject()["artist_nm"].toString());
                    }
                }

                QJsonArray bitrates = ProcJsonEasy::getJsonArray(data_obj, "bitrates");

                if(bitrates.contains("flac") == true){
                    data_output.bitrates = "FLAC";
                }
                else if(bitrates.contains("aac320") == true){
                    data_output.bitrates = "AAC320";
                }
                else if(bitrates.contains("320k") == true || bitrates.contains("mp3") == true){
                    data_output.bitrates = "MP3";
                }
                else if(bitrates.contains("aac") == true){
                    data_output.bitrates = "AAC";
                }
                else if(bitrates.contains("fullhd") == true){
                    data_output.bitrates = "FULL HD";
                }
                else if(bitrates.contains("hd") == true){
                    data_output.bitrates = "HD";
                }
                else if(bitrates.contains("sd") == true){
                    data_output.bitrates = "SD";
                }

                if(!data_output.bitrates.isEmpty()){
                    data_output.hires = true;
                }

            }
            else if(data_output.type == "QOBUZ"){

                if(data_output.duration == 0){
                    data_output.duration = ProcJsonEasy::getInt(data_obj, "duration");
                }

                if(data_output.album_title.isEmpty()){
                    data_output.album_title = ProcJsonEasy::getString(data_obj, "title");
                }

                if(data_output.work.isEmpty()){
                    data_output.work = ProcJsonEasy::getString(data_obj, "work");
                }

                if(data_output.version.isEmpty()){
                    data_output.version = ProcJsonEasy::getString(data_obj, "version");
                }

                QList<QString> tmp_list_name;
                if(data_output.list_artist_name.size() > 0){
                    foreach(QString tmp_name, data_output.list_artist_name){
                        tmp_list_name.append(tmp_name);
                    }
                }

                if(data_output.list_artist_name.size() == 0 || tmp_list_name.contains("null")){
                    data_output.list_artist_name.clear();
                    if(album_obj.contains("artist")){
                        QJsonObject artist_obj = ProcJsonEasy::getJsonObject(album_obj, "artist");
                        data_output.list_artist_name.append(ProcJsonEasy::getString(artist_obj, "name"));
                    }
                    else if(data_obj.contains("composer")){
                        QJsonObject artist_obj = ProcJsonEasy::getJsonObject(data_obj, "composer");
                        data_output.list_artist_name.append(ProcJsonEasy::getString(artist_obj, "name"));
                    }
                }

                data_output.maximum_bit_depth = ProcJsonEasy::getInt(data_obj, "maximum_bit_depth");
                data_output.maximum_sampling_rate = ProcJsonEasy::getDouble(data_obj, "maximum_sampling_rate");

                data_output.work = ProcJsonEasy::getString(data_obj, "work");
                data_output.version = ProcJsonEasy::getString(data_obj, "version");

                QJsonObject performer = ProcJsonEasy::getJsonObject(data_obj, "performer");
                data_output.performer_id = ProcJsonEasy::getInt(performer, "id");
                data_output.performer_name = ProcJsonEasy::getString(performer, "name");

                data_output.hires = ProcJsonEasy::getBool(data_obj, "hires");
            }
            else if(data_output.type == "APPLE_MUSIC"){

                QJsonObject attributes = ProcJsonEasy::getJsonObject(data_obj, "attributes");

                if(data_output.album_title.isEmpty()){
                    data_output.album_title = ProcJsonEasy::getString(attributes, "albumName");
                }

                if(data_output.list_artist_name.size() == 0){
                    data_output.list_artist_name.clear();
                    data_output.list_artist_name.append(ProcJsonEasy::getString(attributes, "artistName"));
                }

                if(data_output.duration == 0){
                    data_output.duration = ProcJsonEasy::getInt(attributes, "durationInMillis");
                }
            }
        }

        return data_output;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : convert ItemData to OptMorePopup::HeaderData
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief qobuz::AlbumItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData(const roseHome::AlbumItemData &data){//c220903_2
        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.title;
        data_header.sub_title = data.artist;
        data_header.imageUrl = data.thumbnail;
        data_header.clientKey = data.clientKey;
        data_header.data_pk = QString("%1").arg(data.id);
        data_header.type = data.type;      //j220906 share link
        data_header.flagProcStar = false;
        data_header.isRose = false;
        data_header.isShare = true;      //j220906 share link

        return data_header;
    }


    /**
     * @brief qobuz::PlaylistItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData(const roseHome::PlaylistItemData &data){
        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.title;
        data_header.sub_title = data.ownerName;
        data_header.imageUrl = data.thumbnail;
        data_header.data_pk = QString("%1").arg(data.id);
        data_header.type = data.type;      //j220906 share link
        data_header.flagProcStar = false;
        data_header.isRose = data.isRose;

        if(data.share == "Private" || data.share == "Friend"){      //j220906 share link
            data_header.isShare = false;
        }
        else if(data.type == "TIDAL" && data.title.contains("Mix")){
            data_header.isShare = false;
        }
        else{
            data_header.isShare = true;
        }

        return data_header;
    }


    /**
     * @brief qobuz::TrackItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData(const roseHome::TrackItemData &data){
        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.title;
        data_header.sub_title = data.list_artist_name.join(",");
        data_header.imageUrl = data.thumbnail;
        data_header.data_pk = QString("%1").arg(data.id);
        data_header.type = data.type;      //j220906 share link
        data_header.flagProcStar = false;
        data_header.isRose = data.isRose;
        data_header.isShare = true;      //j220906 share link

        return data_header;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about PageInfo_AlbumAllView
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief PageInfo_AlbumAllView struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData::getObjectJson_pageInfo_albumAllView(const roseHome::PageInfo_AlbumAllView &data){
        QJsonObject jsonObj;
        jsonObj.insert("pathTitle", data.pathTitle);
        jsonObj.insert("api_subPath", data.api_subPath);
        jsonObj.insert("type", data.type);
        jsonObj.insert("genre_id", data.genre_id);
        jsonObj.insert("genre_ids", data.genre_ids);
        jsonObj.insert("artist_id", data.artist_id);
        jsonObj.insert("page", data.page);
        jsonObj.insert("filter_type", data.filter_type);

        return jsonObj;
    }

    /**
     * @brief JsonObject을 PageInfo_AlbumAllView struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    roseHome::PageInfo_AlbumAllView ConvertData::convertData_pageInfo_albumAllView(const QJsonObject &jsonObj){
        roseHome::PageInfo_AlbumAllView data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");
        data_output.type = ProcJsonEasy::getString(jsonObj, "type");
        data_output.genre_id = ProcJsonEasy::getInt(jsonObj, "genre_id");
        data_output.genre_ids = ProcJsonEasy::getString(jsonObj, "genre_ids");
        data_output.artist_id = ProcJsonEasy::getInt(jsonObj, "artist_id");
        data_output.page = ProcJsonEasy::getString(jsonObj, "page");
        data_output.filter_type = ProcJsonEasy::getString(jsonObj, "filter_type");

        return data_output;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about PageInfo_PlaylistAllView
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief PageInfo_PlaylistAllView struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData::getObjectJson_pageInfo_playlistAllView(const roseHome::PageInfo_PlaylistAllView &data){
        QJsonObject jsonObj;
        jsonObj.insert("pathTitle", data.pathTitle);
        jsonObj.insert("api_subPath", data.api_subPath);
        jsonObj.insert("type", data.type);
        jsonObj.insert("genre_id", data.genre_id);
        jsonObj.insert("genre_ids", data.genre_ids);
        jsonObj.insert("type_id", data.type_id);
        jsonObj.insert("filter_type", data.filter_type);

        return jsonObj;
    }

    QJsonObject ConvertData::getObjectJson_pageInfo_UserPage(const roseHome::PageInfo_UserPage &data){
        QJsonObject jsonObj;
        jsonObj.insert("pathTitle", data.pathTitle);
        jsonObj.insert("api_subPath", data.api_subPath);
        jsonObj.insert("type", data.type);
        jsonObj.insert("member_id", data.member_id);

        jsonObj.insert("type_id", data.type_id);

        return jsonObj;
    }



    roseHome::PageInfo_UserPage ConvertData::getObjectJson_pageInfo_UserPage(const QJsonObject &jsonObj){

        roseHome::PageInfo_UserPage data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");
        data_output.type = ProcJsonEasy::getString(jsonObj, "type");
        data_output.member_id = ProcJsonEasy::getInt(jsonObj, "member_id");

        data_output.type_id = ProcJsonEasy::getInt(jsonObj, "type_id");

        return data_output;


    }

    /**
     * @brief JsonObject을 PageInfo_PlaylistAllView struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    roseHome::PageInfo_PlaylistAllView ConvertData::convertData_pageInfo_playlistAllView(const QJsonObject &jsonObj){
        roseHome::PageInfo_PlaylistAllView data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");
        data_output.type = ProcJsonEasy::getString(jsonObj, "type");
        data_output.genre_id = ProcJsonEasy::getInt(jsonObj, "genre_id");
        data_output.genre_ids = ProcJsonEasy::getString(jsonObj, "genre_ids");
        data_output.type_id = ProcJsonEasy::getInt(jsonObj, "type_id");
        data_output.filter_type = ProcJsonEasy::getString(jsonObj, "filter_type");

        return data_output;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about PageInfo_trackAllView
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief PageInfo_trackAllView struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData::getObjectJson_pageInfo_trackAllView(const roseHome::PageInfo_TrackAllView &data){
        QJsonObject jsonObj;
        jsonObj.insert("pathTitle", data.pathTitle);
        jsonObj.insert("api_subPath", data.api_subPath);
        jsonObj.insert("type", data.type);
        jsonObj.insert("genre_id", data.genre_id);
        jsonObj.insert("genre_ids", data.genre_ids);
        jsonObj.insert("type_id", data.type_id);
        jsonObj.insert("filter_type", data.filter_type);

        return jsonObj;
    }

    /**
     * @brief JsonObject을 PageInfo_trackAllView struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    roseHome::PageInfo_TrackAllView ConvertData::convertData_pageInfo_trackAllView(const QJsonObject &jsonObj){
        roseHome::PageInfo_TrackAllView data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");
        data_output.type = ProcJsonEasy::getString(jsonObj, "type");
        data_output.genre_id = ProcJsonEasy::getInt(jsonObj, "genre_id");
        data_output.genre_ids = ProcJsonEasy::getString(jsonObj, "genre_ids");
        data_output.type_id = ProcJsonEasy::getInt(jsonObj, "type_id");
        data_output.filter_type = ProcJsonEasy::getString(jsonObj, "filter_type");

        return data_output;
    }


    QJsonObject ConvertData::getObjectJson_pageInfo_radioAllView(const roseHome::PageInfo_RadioAllView &data){

        QJsonObject jsonObj;
        jsonObj.insert("pathTitle", data.pathTitle);
        jsonObj.insert("api_subPath", data.api_subPath);
        jsonObj.insert("type", data.type);
        jsonObj.insert("genre_id", data.genre_id);

        return jsonObj;
    }

    /**
     * @brief JsonObject을 PageInfo_AlbumAllView struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    roseHome::PageInfo_RadioAllView ConvertData::convertData_pageInfo_radioAllView(const QJsonObject &jsonObj){

        roseHome::PageInfo_RadioAllView data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");
        data_output.type = ProcJsonEasy::getString(jsonObj, "type");
        data_output.genre_id = ProcJsonEasy::getInt(jsonObj, "genre_id");

        return data_output;
    }
}
