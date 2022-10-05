#include "tidal/ConvertData.h"

#include "common/ProcJsonEasy.h"

#include <QJsonDocument>


namespace tidal {


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about Session info
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief RoseSessionInfo_forTidal struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData::getObjectJson_roseSessionInfo(const tidal::RoseSessionInfo_forTidal &data){

        QJsonObject jsonObj;
        jsonObj.insert("authorization_token", data.TIDAL_AccessToken);
        jsonObj.insert("refresh_token", data.TIDAL_RefreshToken);
        jsonObj.insert("country_code", data.TIDAL_CountryCode);
        jsonObj.insert("highestSoundQuality", QString("%1").arg(data.TIDAL_HighestSoundQuality));
        jsonObj.insert("sessionId", data.TIDAL_SessionId);
        jsonObj.insert("soundQuality", QString("%1").arg(data.TIDAL_SoundQuality));
        jsonObj.insert("userId", QString("%1").arg(data.TIDAL_UserID));
        jsonObj.insert("user_id_name", data.TIDAL_UserName);

        return jsonObj;
    }


    /**
     * @brief JsonObject을 RoseSessionInfo_forTidal struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    tidal::RoseSessionInfo_forTidal ConvertData::convertData_roseSessionInfo(const QJsonObject &jsonObj){

        // test debug 28.01.2021 Added Jeon
        //qDebug() << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";

        tidal::RoseSessionInfo_forTidal data_output;
        data_output.TIDAL_AccessToken = ProcJsonEasy::getString(jsonObj, "authorization_token");
        data_output.TIDAL_RefreshToken = ProcJsonEasy::getString(jsonObj, "refresh_token");
        data_output.TIDAL_CountryCode = ProcJsonEasy::getString(jsonObj, "country_code");
        data_output.TIDAL_HighestSoundQuality = ProcJsonEasy::getInt(jsonObj, "highestSoundQuality");
        data_output.TIDAL_SessionId = ProcJsonEasy::getString(jsonObj, "sessionId");
        data_output.TIDAL_SoundQuality = ProcJsonEasy::getInt(jsonObj, "soundQuality");
        data_output.TIDAL_UserID = ProcJsonEasy::getInt(jsonObj, "userId");
        data_output.TIDAL_UserName = ProcJsonEasy::getString(jsonObj, "user_id_name");

        return data_output;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : functions - Tidal 데이터 변환 관련
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 타이달 API에서 반환하는 이미지 key값에 대해서, 타이달 Resource URL로 변환하여 반환
     * @details  베어앱 노트 참고 - [[Rose Tidal - 이미지 URL화 방법]]
     * @param tidalImageKey 타이달API에서 반환하는 이미지 key값
     * @param imageSize
     * @return 타이달 Resource Url 반환
     */
    QString ConvertData::convertImagePath(const QString tidalImageKey, const ImageSize_Square imageSize){

        if(tidalImageKey.isEmpty()){
            return "";
        }
        else{
            QString output = tidalImageKey;
            output.replace(QString("-"), QString("/"), Qt::CaseSensitive);
//            output.insert(0, "https://resources.tidal.com/images/");          // https  (맥에서 너무 느려서)
            output.insert(0, "http://resources.tidal.com/images/");             // http
            output.append("/");
            output.append( ConvertData::getImageJpgName(imageSize) );
            return output;
        }
    }


    /**
     * @brief 타이달 API에서 반환하는 이미지 key값에 대해서, 타이달 Resource URL로 변환하여 반환
     * @details  베어앱 노트 참고 - [[Rose Tidal - 이미지 URL화 방법]]
     * @param tidalImageKey
     * @param imageSize
     * @return
     */
    QString ConvertData::convertImagePath(const QString tidalImageKey, const ImageSize_Ractangle imageSize){

        if(tidalImageKey.isEmpty()){
            return "";
        }
        else{
            QString output = tidalImageKey;
            output.replace(QString("-"), QString("/"), Qt::CaseSensitive);
//            output.insert(0, "https://resources.tidal.com/images/");          // https  (맥에서 너무 느려서)
            output.insert(0, "http://resources.tidal.com/images/");             // http
            output.append("/");
            output.append( ConvertData::getImageJpgName(imageSize) );
            return output;
        }
    }


    /**
     * @brief ConvertData::getImageJpgName
     * @param imageSize
     * @return
     */
    QString ConvertData::getImageJpgName(const ImageSize_Square imageSize){

        int tmp_imgSize = 160;      // 기본값
        switch (imageSize) {
            case Size_160x160 : tmp_imgSize = 160;      break;
            case Size_320x320 : tmp_imgSize = 320;      break;
            case Size_480x480 : tmp_imgSize = 480;      break;
            case Size_640x640 : tmp_imgSize = 640;      break;
            case Size_750x750 : tmp_imgSize = 750;      break;
            case Size_1080x1080 : tmp_imgSize = 1080;      break;
        }

        return QString("%1x%1.jpg").arg(tmp_imgSize);
    }


    /**
     * @brief ConvertData::getImageJpgName
     * @param imageSize
     * @return
     */
    QString ConvertData::getImageJpgName(const ImageSize_Ractangle imageSize){

        int tmp_imgWidth = 160;      // 기본값
        int tmp_imgHeight = 90;      // 기본값
        switch (imageSize) {
            case Size_160x90 :  tmp_imgWidth = 160;     tmp_imgHeight = 90;       break;
            case Size_320x180 : tmp_imgWidth = 320;     tmp_imgHeight = 180;      break;
            case Size_480x270 : tmp_imgWidth = 480;     tmp_imgHeight = 270;      break;
            case Size_640x360 : tmp_imgWidth = 640;     tmp_imgHeight = 360;      break;
            case Size_800x450 : tmp_imgWidth = 750;     tmp_imgHeight = 450;      break;
            case Size_1280x720 : tmp_imgWidth = 1080;   tmp_imgHeight = 720;      break;
        }

        return QString("%1x%2.jpg").arg(tmp_imgWidth).arg(tmp_imgHeight);
    }


    /**
     * @brief creator 의 name 을 반환한다.
     * @details id=0 이면 TIDAL 을 반환하고, 아니면 name을 반환
     * @param p_jsonObj_creator JSON 중 creator 에 해당되는 Object 정보.
     * @return
     */
    QString ConvertData::getCreatorName(const QJsonObject p_jsonObj_creator){

        int creator_id = ProcJsonEasy::getInt(p_jsonObj_creator, "id", 0);
        if(creator_id == 0){
            return "TIDAL";
        }
        else{
            return ProcJsonEasy::getString(p_jsonObj_creator, "name", "");
        }
    }


//    /**
//     * @brief JsonArray 의 요소들에 대해서 startIndex가 0번째가 되도록 재배열 한다. 0~startIndex-1 까지의 데이터는 가장 뒤로 순서대로 붙인다.
//     * @param p_jsonArr
//     * @param startIndex
//     * @return
//     */
//    QJsonArray ConvertData::reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex){
//        const int len_data = p_jsonArr.count();
//        if(startIndex>0 && startIndex<len_data){
//            QJsonArray jsonArr_reorder;
//            for(int i=startIndex ; i<len_data ; i++){
//                jsonArr_reorder.append(p_jsonArr.at(i));
//            }
//            for(int i=0 ; i<startIndex ; i++){
//                jsonArr_reorder.append(p_jsonArr.at(i));
//            }
//            return jsonArr_reorder;
//        }
//        else{
//            // 유효하지 않는 경우, 그대로 반환
//            return p_jsonArr;
//        }
//    }

//    /**
//     * @brief ConvertData::subRange_JsonArray
//     * @param p_jsonArr
//     * @param startIndex
//     * @return
//     */
//    QJsonArray ConvertData::subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex){
//        const int len_data = p_jsonArr.count();
//        if(startIndex>0 && startIndex<len_data){
//            QJsonArray jsonArr_subRange;
//            for(int i=startIndex ; i<len_data ; i++){
//                jsonArr_subRange.append(p_jsonArr.at(i));
//            }
//            return jsonArr_subRange;
//        }
//        else{
//            // 유효하지 않는 경우, 그대로 반환
//            return p_jsonArr;
//        }
//    }


    /**
     * @brief TIDAL이 Track 정보 반환하는 JsonObject로부터 TrackItemData 로 변환해서 반환한다.
     * @param jsonObj
     * @return
     */
    tidal::TrackItemData ConvertData::make_trackData_fromTidalJsonObj(const QJsonObject &jsonObj){//cheon210916-total

        // 정보 담을 struct
        tidal::TrackItemData data_output;
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.version = ProcJsonEasy::getString(jsonObj, "version");

        data_output.audioQuality = ProcJsonEasy::getString(jsonObj, "audioQuality");
        data_output.quality = ProcJsonEasy::getString(jsonObj, "quality");
        data_output.type = ProcJsonEasy::getString(jsonObj, "type");

        data_output.streamStartDate = ProcJsonEasy::getString(jsonObj, "streamStartDate");
        data_output.streamReady = ProcJsonEasy::getBool(jsonObj, "streamReady");

        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        data_output.trackNumber = ProcJsonEasy::getInt(jsonObj, "trackNumber");

        data_output.playlist_id = ProcJsonEasy::getString(jsonObj, "itemUuid");

        QJsonObject jsonObj_album = ProcJsonEasy::getJsonObject(jsonObj, "album");
        data_output.albumName = ProcJsonEasy::getString(jsonObj_album, "title");
        data_output.album_id = ProcJsonEasy::getInt(jsonObj_album, "id");

        // 앨범 이미지
        QString album_cover = ProcJsonEasy::getString(jsonObj_album, "cover");
        QString album_image = ProcJsonEasy::getString(jsonObj, "imageId");

        data_output.image = album_cover.isEmpty() ? tidal::ConvertData::convertImagePath(album_image, ImageSize_Square::Size_160x160) : tidal::ConvertData::convertImagePath(album_cover, ImageSize_Square::Size_160x160);

        // for artist
        QJsonArray tmp_jsonArr_artist = ProcJsonEasy::getJsonArray(jsonObj, "artists");
        data_output.list_artist_id = ProcJsonEasy::getListInt_fromJsonArr(tmp_jsonArr_artist, "id");
        data_output.list_artist_name = ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_artist, "name");

        return data_output;
    }


    /**
     * @brief TIDAL이 visual 정보 반환하는 JsonObject로부터 TrackItemData 로 변환해서 반환한다.
     * @param jsonObj
     * @return
     */
    tidal::TrackItemData ConvertData::make_visualData_fromTidalJsonObj(const QJsonObject &jsonObj){

        //QJsonDocument doc(jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << strJson;

        // 정보 담을 struct
        tidal::TrackItemData data_output;
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.version = ProcJsonEasy::getString(jsonObj, "version");

        data_output.audioQuality = ProcJsonEasy::getString(jsonObj, "audioQuality");
        data_output.quality = ProcJsonEasy::getString(jsonObj, "quality");
        data_output.type = ProcJsonEasy::getString(jsonObj, "type");

        data_output.streamStartDate = ProcJsonEasy::getString(jsonObj, "streamStartDate");
        data_output.streamReady = ProcJsonEasy::getBool(jsonObj, "streamReady");

        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        //data_output.trackNumber = ProcJsonEasy::getInt(jsonObj, "volumeNumber");

        data_output.visual_type = ProcJsonEasy::getString(jsonObj, "type") == "Music Video" ? true : false;

        data_output.playlist_id = ProcJsonEasy::getString(jsonObj, "itemUuid");

        QJsonObject jsonObj_album = ProcJsonEasy::getJsonObject(jsonObj, "album");
        data_output.albumName = ProcJsonEasy::getString(jsonObj_album, "title");
        data_output.album_id = ProcJsonEasy::getInt(jsonObj_album, "id");

        // 앨범 이미지
        QString album_cover = ProcJsonEasy::getString(jsonObj_album, "cover");
        QString album_image = ProcJsonEasy::getString(jsonObj, "imageId");

        data_output.image = album_cover.isEmpty() ? tidal::ConvertData::convertImagePath(album_image, ImageSize_Square::Size_640x640) : tidal::ConvertData::convertImagePath(album_cover, ImageSize_Square::Size_640x640);

        // for artist
        QJsonArray tmp_jsonArr_artist = ProcJsonEasy::getJsonArray(jsonObj, "artists");
        data_output.list_artist_id = ProcJsonEasy::getListInt_fromJsonArr(tmp_jsonArr_artist, "id");
        data_output.list_artist_name = ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_artist, "name");

        return data_output;
    }


    /**
     * @brief TIDAL이 Album 정보 반환하는 JsonObject로부터 TrackItemData 로 변환해서 반환한다.
     * @param jsonObj
     * @return
     */
    tidal::AlbumItemData ConvertData::make_albumData_fromTidalJsonObj(const QJsonObject &jsonObj){

        // 정보 담을 struct
        //QJsonDocument doc(jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "AlbumItemData ConvertData::make_albumData_fromTidalJsonObj---"<< strJson;

        AlbumItemData data_output;
        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");

        data_output.audioQuality = ProcJsonEasy::getString(jsonObj, "audioQuality");
        data_output.releaseDate = ProcJsonEasy::getString(jsonObj, "releaseDate");
        data_output.streamStartDate = ProcJsonEasy::getString(jsonObj, "streamStartDate");
        data_output.streamReady = ProcJsonEasy::getBool(jsonObj, "streamReady");

        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        data_output.popularity = ProcJsonEasy::getInt(jsonObj, "popularity");//cheontidalhires

        data_output.image =  tidal::ConvertData::convertImagePath(ProcJsonEasy::getString(jsonObj, "cover"), ImageSize_Square::Size_640x640);

        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.version = ProcJsonEasy::getString(jsonObj, "version");        

        int video_cnt = ProcJsonEasy::getInt(jsonObj, "numberOfVideos");
        if(video_cnt > 0){
            data_output.isVideo = true;
            data_output.visualAlbum = true;

            data_output.numberOfTracks = ProcJsonEasy::getInt(jsonObj, "numberOfVideos");
        }
        else{
            data_output.numberOfTracks = ProcJsonEasy::getInt(jsonObj, "numberOfTracks");
        }

        // for artist
        QJsonArray tmp_jsonArr_artist = ProcJsonEasy::getJsonArray(jsonObj, "artists");
        data_output.list_artist_id = ProcJsonEasy::getListInt_fromJsonArr(tmp_jsonArr_artist, "id");
        data_output.list_artist_name = ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_artist, "name");

        return data_output;
    }


    /**
     * @brief TIDAL이 mixes 정보 반환하는 JsonObject로부터 TrackItemData 로 변환해서 반환한다.
     * @param jsonObj
     * @return
     */
    tidal::PlaylistItemData ConvertData::make_mixesData_fromTidalJsonObj(const QJsonObject &jsonObj){

        int i;
        QString tmp_artist;
       //QJsonDocument doc(jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "PlaylistItemData ConvertData::make_mixesData_fromTidalJsonObj---"<< strJson;

        // 정보 담을 struct
        PlaylistItemData data_output;
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.uuid = ProcJsonEasy::getString(jsonObj, "id");

        data_output.audioQuality = ProcJsonEasy::getString(jsonObj, "audioQuality");

        QJsonObject p_image = ProcJsonEasy::getJsonObject(jsonObj, "images");
        data_output.image = ProcJsonEasy::getString(p_image, "L");

        QJsonArray p_artist = ProcJsonEasy::getJsonArray(jsonObj, "artists");
        for(i = 0; i < p_artist.count(); i++){
            QJsonObject tmp_json = p_artist.at(i).toObject();
            if(i == 0){
                tmp_artist = ProcJsonEasy::getString(tmp_json, "name");
            }
            else{
                tmp_artist += ", " + ProcJsonEasy::getString(tmp_json, "name");
            }
        }
        data_output.creatorName = tmp_artist;
        data_output.description = "mixes";

        return data_output;
    }


    /**
     * @brief TIDAL이 Playlist 정보 반환하는 JsonObject로부터 TrackItemData 로 변환해서 반환한다.
     * @param jsonObj
     * @return
     */
    tidal::PlaylistItemData ConvertData::make_playlistData_fromTidalJsonObj(const QJsonObject &jsonObj){

        //QJsonDocument doc(jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "ConvertData::make_playlistData_fromTidalJsonObj---"<< strJson;

        // 정보 담을 struct
        PlaylistItemData data_output;
        data_output.image =  tidal::ConvertData::convertImagePath(ProcJsonEasy::getString(jsonObj, "squareImage"), ImageSize_Square::Size_640x640);
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.uuid = ProcJsonEasy::getString(jsonObj, "uuid");
        data_output.creatorName = tidal::ConvertData::getCreatorName(ProcJsonEasy::getJsonObject(jsonObj, "creator"));
        data_output.description = ProcJsonEasy::getString(jsonObj, "description");
        data_output.numberOfTracks = ProcJsonEasy::getInt(jsonObj, "numberOfTracks");
        data_output.popularity = ProcJsonEasy::getInt(jsonObj, "popularity");//cheon210603
        data_output.lastUpdated = ProcJsonEasy::getString(jsonObj, "lastUpdated");//cheon210603
        data_output.audioQuality = ProcJsonEasy::getString(jsonObj, "audioQuality");
        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");

        return data_output;
    }


    /**
     * @brief TIDAL이 Artist 정보 반환하는 JsonObject로부터 ArtistItemData 로 변환해서 반환한다.
     * @param jsonObj
     * @return
     */
    tidal::ArtistItemData ConvertData::make_artistData_fromTidalJsonObj(const QJsonObject &jsonObj){

        // 정보 담을 struct
        ArtistItemData data_output;
        data_output.image =  tidal::ConvertData::convertImagePath(ProcJsonEasy::getString(jsonObj, "picture"), ImageSize_Square::Size_750x750);
        data_output.name = ProcJsonEasy::getString(jsonObj, "name");
        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        return data_output;
    }


    /**
     * @brief TIDAL이 Video 정보 반환하는 JsonObject로부터 VideoItemData 로 변환해서 반환한다.
     * @param jsonObj
     * @return
     */
    tidal::VideoItemData ConvertData::make_videoData_fromTidalJsonObj(const QJsonObject &jsonObj){

        // 정보 담을 struct
        VideoItemData data_output;
        data_output.image =  tidal::ConvertData::convertImagePath(ProcJsonEasy::getString(jsonObj, "imageId"), ImageSize_Ractangle::Size_640x360);
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        data_output.type = ProcJsonEasy::getString(jsonObj, "type");
        data_output.quality = ProcJsonEasy::getString(jsonObj, "quality");

        // for artist
        QJsonArray tmp_jsonArr_artist = ProcJsonEasy::getJsonArray(jsonObj, "artists");
        data_output.list_artist_id = ProcJsonEasy::getListInt_fromJsonArr(tmp_jsonArr_artist, "id");
        data_output.list_artist_name = ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_artist, "name");

        // Rose에게 재생 요청을 하기 위해
        data_output.jsonObj_toPlay = jsonObj;

        return data_output;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about ItemPositionData
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief TrackItemData struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData::getObjectJson_itemPositionData(const tidal::ItemPositionData &data){

        QJsonObject jsonObj;
        jsonObj.insert("section", data.section);
        jsonObj.insert("index", data.index);
        jsonObj.insert("data_id", data.data_id);
        return jsonObj;
    }


    /**
     * @brief JsonObject을 ItemPositionData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    tidal::ItemPositionData ConvertData::convertData_itemPositionData(const QJsonObject &jsonObj){

        tidal::ItemPositionData data_output;
        data_output.section = ProcJsonEasy::getInt(jsonObj, "section");
        data_output.index = ProcJsonEasy::getInt(jsonObj, "index");
        data_output.data_id = ProcJsonEasy::getString(jsonObj, "data_id");
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
    QJsonObject ConvertData::getObjectJson_playlistData(const tidal::PlaylistItemData &data){

        QJsonObject jsonObj;
        jsonObj.insert("image", data.image);
        jsonObj.insert("title", data.title);
        jsonObj.insert("uuid", data.uuid);
        jsonObj.insert("creatorName", data.creatorName);
        jsonObj.insert("description", data.description);
        jsonObj.insert("numberOfTracks", data.numberOfTracks);
        jsonObj.insert("flagCreatedByUser", data.flagCreatedByUser);
        jsonObj.insert("lastUpdated", data.lastUpdated);//cheon210603
        jsonObj.insert("popularity", data.popularity);//cheon210603
        jsonObj.insert("audioQuality", data.audioQuality);
        jsonObj.insert("duration", data.duration);
        return jsonObj;
    }


    /**
     * @brief JsonObject을 PlaylistItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    tidal::PlaylistItemData ConvertData::convertData_playlistData(const QJsonObject &jsonObj){

        //QJsonDocument doc(jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "ConvertData::convertData_playlistData---"<< strJson;

        tidal::PlaylistItemData data_output;
        data_output.image = ProcJsonEasy::getString(jsonObj, "image");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.uuid = ProcJsonEasy::getString(jsonObj, "uuid");
        data_output.creatorName = ProcJsonEasy::getString(jsonObj, "creatorName");
        data_output.description = ProcJsonEasy::getString(jsonObj, "description");
        data_output.numberOfTracks = ProcJsonEasy::getInt(jsonObj, "numberOfTracks");
        data_output.flagCreatedByUser = ProcJsonEasy::getBool(jsonObj, "flagCreatedByUser");
        data_output.popularity = ProcJsonEasy::getInt(jsonObj, "popularity");//cheon210603
        data_output.lastUpdated = ProcJsonEasy::getString(jsonObj, "lastUpdated");//cheon210603
        data_output.audioQuality = ProcJsonEasy::getString(jsonObj, "audioQuality");
        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");

        return data_output;
    }


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
    QJsonObject ConvertData::getObjectJson_albumData(const tidal::AlbumItemData &data){

        QJsonObject jsonObj;
        jsonObj.insert("id", data.id);
        jsonObj.insert("audioQuality", data.audioQuality);
        jsonObj.insert("releaseDate", data.releaseDate);
        jsonObj.insert("streamStartDate", data.streamStartDate);
        jsonObj.insert("streamReady", data.streamReady);

        jsonObj.insert("duration", data.duration);
        jsonObj.insert("popularity", data.popularity);

        jsonObj.insert("image", data.image);

        jsonObj.insert("title", data.title);
        jsonObj.insert("version", data.version);
        jsonObj.insert("numberOfTracks", data.numberOfTracks);

        jsonObj.insert("isVideo", data.isVideo);
        jsonObj.insert("visualAlbum", data.visualAlbum);

        jsonObj.insert("totalCount", data.totalCount);

        QJsonArray jsonArr_id;
        foreach(int tmp_id, data.list_artist_id){
            jsonArr_id.append(tmp_id);
        }
        jsonObj.insert("list_artist_id", jsonArr_id);

        QJsonArray jsonArr_name;
        foreach(QString tmp_name, data.list_artist_name){
            jsonArr_name.append(tmp_name);
        }
        jsonObj.insert("list_artist_name", jsonArr_name);

        return jsonObj;
    }


    /**
     * @brief JsonObject을 AlbumItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    tidal::AlbumItemData ConvertData::convertData_albumData(const QJsonObject &jsonObj){

        tidal::AlbumItemData data_output;
        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        data_output.audioQuality = ProcJsonEasy::getString(jsonObj, "audioQuality");
        data_output.releaseDate = ProcJsonEasy::getString(jsonObj, "releaseDate");
        data_output.streamStartDate = ProcJsonEasy::getString(jsonObj, "streamStartDate");
        data_output.streamReady = ProcJsonEasy::getBool(jsonObj, "streamReady");

        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        data_output.popularity = ProcJsonEasy::getInt(jsonObj, "popularity");

        data_output.image = ProcJsonEasy::getString(jsonObj, "image");

        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.version = ProcJsonEasy::getString(jsonObj, "version");
        data_output.numberOfTracks = ProcJsonEasy::getInt(jsonObj, "numberOfTracks");

        data_output.visualAlbum = ProcJsonEasy::getBool(jsonObj, "visualAlbum");
        data_output.isVideo = ProcJsonEasy::getBool(jsonObj, "isVideo");

        data_output.totalCount = ProcJsonEasy::getInt(jsonObj, "totalCount");

        QVariantList tmp_list_id = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_id").toVariantList();
        foreach(QVariant tmp_val, tmp_list_id){
            data_output.list_artist_id.append(tmp_val.value<int>());
        }

        QVariantList tmp_list_name = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_name").toVariantList();
        foreach(QVariant tmp_val, tmp_list_name){
            data_output.list_artist_name.append(tmp_val.value<QString>());
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
    QJsonObject ConvertData::getObjectJson_trackData(const tidal::TrackItemData &data){

        QJsonObject jsonObj;
        jsonObj.insert("title", data.title);
        jsonObj.insert("version", data.version);

        jsonObj.insert("audioQuality", data.audioQuality);
        jsonObj.insert("quality", data.quality);
        jsonObj.insert("type", data.type);

        jsonObj.insert("streamStartDate", data.streamStartDate);
        jsonObj.insert("streamReady", data.streamReady);

        jsonObj.insert("id", data.id);
        jsonObj.insert("duration", data.duration);
        jsonObj.insert("trackNumber", data.trackNumber);

        jsonObj.insert("playlist_id", data.playlist_id);

        jsonObj.insert("albumName", data.albumName);
        jsonObj.insert("album_id", data.album_id);
        jsonObj.insert("image", data.image);

        QJsonArray jsonArr_id;
        foreach(int tmp_id, data.list_artist_id){
            jsonArr_id.append(tmp_id);
        }
        jsonObj.insert("list_artist_id", jsonArr_id);

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
    tidal::TrackItemData ConvertData::convertData_trackItemData(const QJsonObject &jsonObj){

        tidal::TrackItemData data_output;
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.version = ProcJsonEasy::getString(jsonObj, "version");

        data_output.audioQuality = ProcJsonEasy::getString(jsonObj, "audioQuality");
        data_output.quality = ProcJsonEasy::getString(jsonObj, "quality");
        data_output.type = ProcJsonEasy::getString(jsonObj, "type");

        data_output.streamStartDate = ProcJsonEasy::getString(jsonObj, "streamStartDate");
        data_output.streamReady = ProcJsonEasy::getBool(jsonObj, "streamReady");

        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        data_output.trackNumber = ProcJsonEasy::getInt(jsonObj, "trackNumber");

        data_output.playlist_id = ProcJsonEasy::getString(jsonObj, "playlist_id");

        data_output.albumName = ProcJsonEasy::getString(jsonObj, "albumName");
        data_output.album_id = ProcJsonEasy::getInt(jsonObj, "album_id");
        data_output.image = ProcJsonEasy::getString(jsonObj, "image");

        QVariantList tmp_list_id = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_id").toVariantList();
        foreach(QVariant tmp_val, tmp_list_id){
            data_output.list_artist_id.append(tmp_val.value<int>());
        }

        QVariantList tmp_list_name = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_name").toVariantList();
        foreach(QVariant tmp_val, tmp_list_name){
            data_output.list_artist_name.append(tmp_val.value<QString>());
        }

        return data_output;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about ArtistItemData
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief ArtistItemData struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData::getObjectJson_artistData(const tidal::ArtistItemData &data){

        QJsonObject jsonObj;
        jsonObj.insert("image", data.image);
        jsonObj.insert("name", data.name);
        jsonObj.insert("id", data.id);
        return jsonObj;
    }

    /**
     * @brief JsonObject을 ArtistItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    tidal::ArtistItemData ConvertData::convertData_artistItemData(const QJsonObject &jsonObj){

        tidal::ArtistItemData data_output;
        data_output.image = ProcJsonEasy::getString(jsonObj, "image");
        data_output.name = ProcJsonEasy::getString(jsonObj, "name");
        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        return data_output;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about VideoItemData
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief VideoItemData struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData::getObjectJson_videoData(const tidal::VideoItemData &data){

        QJsonObject jsonObj;
        jsonObj.insert("image", data.image);
        jsonObj.insert("title", data.title);
        jsonObj.insert("id", data.id);
        jsonObj.insert("duration", data.duration);
        jsonObj.insert("type", data.type);
        jsonObj.insert("quality", data.quality);

        QJsonArray jsonArr_id;
        foreach(int tmp_id, data.list_artist_id){
            jsonArr_id.append(tmp_id);
        }
        jsonObj.insert("list_artist_id", jsonArr_id);

        QJsonArray jsonArr_name;
        foreach(QString tmp_name, data.list_artist_name){
            jsonArr_name.append(tmp_name);
        }
        jsonObj.insert("list_artist_name", jsonArr_name);

        jsonObj.insert("jsonObj_toPlay", data.jsonObj_toPlay);

        return jsonObj;
    }


    /**
     * @brief JsonObject을 VideoItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    tidal::VideoItemData ConvertData::convertData_videoItemData(const QJsonObject &jsonObj){

        tidal::VideoItemData data_output;
        data_output.image = ProcJsonEasy::getString(jsonObj, "image");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        data_output.type = ProcJsonEasy::getString(jsonObj, "type");
        data_output.quality = ProcJsonEasy::getString(jsonObj, "quality");

        QVariantList tmp_list_id = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_id").toVariantList();
        foreach(QVariant tmp_val, tmp_list_id){
            data_output.list_artist_id.append(tmp_val.value<int>());
        }

        QVariantList tmp_list_name = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_name").toVariantList();
        foreach(QVariant tmp_val, tmp_list_name){
            data_output.list_artist_name.append(tmp_val.value<QString>());
        }

        data_output.jsonObj_toPlay = ProcJsonEasy::getJsonObject(jsonObj, "jsonObj_toPlay");

        return data_output;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : convert ItemData to OptMorePopup::HeaderData
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief tidal::AlbumItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData(const tidal::AlbumItemData &data){

        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.title;
        data_header.sub_title = data.list_artist_name.join(",");
        data_header.imageUrl = data.image;
        data_header.data_pk = QString("%1").arg(data.id);
        data_header.type = "TIDAL";      //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = true;      //j220906 share link

        return data_header;
    }


    /**
     * @brief tidal::TrackItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData(const tidal::TrackItemData &data){

        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.title;
        data_header.sub_title = data.list_artist_name.join(",");
        data_header.imageUrl = data.image;
        data_header.data_pk = QString("%1").arg(data.id);
        data_header.type = "TIDAL";      //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = true;      //j220906 share link

        return data_header;
    }


    /**
     * @brief tidal::PlaylistItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData(const tidal::PlaylistItemData &data){

        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.title;
        data_header.sub_title = data.creatorName;
        data_header.imageUrl = data.image;
        data_header.data_pk = data.uuid;
        data_header.type = "TIDAL";      //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = true;      //j220906 share link
        if(data.description == "mixes"){ //j220906 share link
            data_header.isMix = true;
        }
        else{
            data_header.isMix = false;
        }

        return data_header;
    }


    /**
     * @brief tidal::ArtistItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData(const tidal::ArtistItemData &data){

        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.name;
        data_header.sub_title = "";
        data_header.imageUrl = data.image;
        data_header.data_pk = QString("%1").arg(data.id);
        data_header.type = "TIDAL";      //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = true;      //j220906 share link

        return data_header;
    }


    /**
     * @brief tidal::ArtistItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData(const tidal::VideoItemData &data){

        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.title;
        data_header.sub_title = data.list_artist_name.join(",");
        data_header.imageUrl = data.image;
        data_header.data_pk = QString("%1").arg(data.id);
        data_header.type = "TIDAL";      //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = true;           //j220906 share link

        return data_header;
    }


    /**
     * @brief ConvertData::convertEditItem_fromTrackData
     * @param data
     * @return
     */
    editDelegate::ItemEditData ConvertData::convertEditItem_fromTrackData(const tidal::TrackItemData &data_track){

       editDelegate::ItemEditData data_output;
       data_output.album_image = data_track.image;
       data_output.track_title = data_track.title;
       data_output.artist_name = data_track.list_artist_name.join(",");
       data_output.album_title = data_track.albumName;
       data_output.flagChecked = false;         // 초기값

       return data_output;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about PageInfo_TrackAllView
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief PageInfo_TrackAllView struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData::getObjectJson_pageInfo_trackAllView(const tidal::PageInfo_TrackAllView &data){

        QJsonObject jsonObj;
        jsonObj.insert("pathTitle", data.pathTitle);
        jsonObj.insert("api_subPath", data.api_subPath);

        return jsonObj;
    }


    /**
     * @brief JsonObject을 PageInfo_TrackAllView struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    tidal::PageInfo_TrackAllView ConvertData::convertData_pageInfo_trackAllView(const QJsonObject &jsonObj){

        tidal::PageInfo_TrackAllView data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");

        return data_output;
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
    QJsonObject ConvertData::getObjectJson_pageInfo_albumAllView(const tidal::PageInfo_AlbumAllView &data){

        QJsonObject jsonObj;
        jsonObj.insert("pathTitle", data.pathTitle);
        jsonObj.insert("api_subPath", data.api_subPath);

        return jsonObj;
    }


    /**
     * @brief JsonObject을 PageInfo_AlbumAllView struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    tidal::PageInfo_AlbumAllView ConvertData::convertData_pageInfo_albumAllView(const QJsonObject &jsonObj){

        tidal::PageInfo_AlbumAllView data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");

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
    QJsonObject ConvertData::getObjectJson_pageInfo_playlistAllView(const tidal::PageInfo_PlaylistAllView &data){

        QJsonObject jsonObj;
        jsonObj.insert("pathTitle", data.pathTitle);
        jsonObj.insert("api_subPath", data.api_subPath);

        return jsonObj;
    }


    /**
     * @brief JsonObject을 PageInfo_PlaylistAllView struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    tidal::PageInfo_PlaylistAllView ConvertData::convertData_pageInfo_playlistAllView(const QJsonObject &jsonObj){

        tidal::PageInfo_PlaylistAllView data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");

        return data_output;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about PageInfo_ArtistAllView
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief PageInfo_ArtistAllView struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData::getObjectJson_pageInfo_artistAllView(const tidal::PageInfo_ArtistAllView &data){

        QJsonObject jsonObj;
        jsonObj.insert("pathTitle", data.pathTitle);
        jsonObj.insert("api_subPath", data.api_subPath);

        return jsonObj;
    }


    /**
     * @brief JsonObject을 PageInfo_ArtistAllView struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    tidal::PageInfo_ArtistAllView ConvertData::convertData_pageInfo_artistAllView(const QJsonObject &jsonObj){

        tidal::PageInfo_ArtistAllView data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");

        return data_output;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about PageInfo_VideoAllView
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief PageInfo_VideoAllView struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData::getObjectJson_pageInfo_videoAllView(const tidal::PageInfo_VideoAllView &data){

        QJsonObject jsonObj;
        jsonObj.insert("pathTitle", data.pathTitle);
        jsonObj.insert("api_subPath", data.api_subPath);

        return jsonObj;
    }


    /**
     * @brief JsonObject을 PageInfo_VideoAllView struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    tidal::PageInfo_VideoAllView ConvertData::convertData_pageInfo_videoAllView(const QJsonObject &jsonObj){

        tidal::PageInfo_VideoAllView data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");

        return data_output;
    }

}
