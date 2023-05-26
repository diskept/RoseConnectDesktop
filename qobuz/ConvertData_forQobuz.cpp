#include "ConvertData_forQobuz.h"
#include "common/ProcJsonEasy.h"
#include "common/global.h"
#include <QDebug>
#include <QException>
#include <common/sqlitehelper.h>

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

namespace qobuz {

    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about Session info
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief RoseSessionInfo_forQobuz struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData::getObjectJson_roseSessionInfo(const qobuz::RoseSessionInfo_forQobuz &data){

        QJsonObject jsonObj;

        if(data.user_auth_token.isEmpty() && data.user_id.isEmpty()){
            jsonObj.insert("QOBUZ_LoginUserItem", "");
        }
        else{
            QJsonObject parameters;
            parameters.insert("hfp_purchase", data.hfp_purchases);
            parameters.insert("hires_purchases_streaming", data.hires_purchases);
            parameters.insert("hires_streaming", data.hires_streaming);
            parameters.insert("lossless_streaming", data.lossless_streaming);
            parameters.insert("lossy_streaming", data.lossy_streaming);
            parameters.insert("mobile_streaming", data.mobile_streaming);
            parameters.insert("offline_streaming", data.offline_streaming);

            QJsonObject credential;
            credential.insert("description", data.description);
            credential.insert("id", data.c_id);
            credential.insert("label", data.label);
            credential.insert("parameters", parameters);

            QJsonObject device;
            device.insert("device_manufacturer_id", data.device_man_id);
            device.insert("id", data.device_id);

            QJsonObject user;
            user.insert("credential", credential);
            user.insert("device", device);
            user.insert("email", data.email);
            user.insert("id", data.user_id);

            QJsonObject qobuz;
            qobuz.insert("user", user);
            qobuz.insert("user_auth_token", data.user_auth_token);

            jsonObj.insert("QOBUZ_LoginUserItem", qobuz);
        }

        //print_qobuz_func();
        //QJsonDocument doc(jsonObj);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "jsonObj = " << strJson << "\n";

        return jsonObj;
    }

    /**
     * @brief JsonObject을 RoseSessionInfo_forQobuz struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    qobuz::RoseSessionInfo_forQobuz ConvertData::convertData_roseSessionInfo(const QJsonObject &jsonObj){

        // test debug 17.02.2021 Added diskept
        //qDebug() << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";

        qobuz::RoseSessionInfo_forQobuz data_output;
        data_output.user_auth_token = ProcJsonEasy::getString(jsonObj, "user_auth_token");
        QJsonObject user = ProcJsonEasy::getJsonObject(jsonObj, "user");
        data_output.email = ProcJsonEasy::getString(user, "email");
        data_output.user_id = ProcJsonEasy::getString(user, "id");
        QJsonObject device = ProcJsonEasy::getJsonObject(user, "device");
        data_output.device_man_id = ProcJsonEasy::getString(device, "device_manufacturer_id");
        data_output.device_id = ProcJsonEasy::getString(device, "id");
        QJsonObject credential = ProcJsonEasy::getJsonObject(user, "credential");
        data_output.description = ProcJsonEasy::getString(credential, "description");
        data_output.c_id = ProcJsonEasy::getString(credential, "id");
        data_output.label = ProcJsonEasy::getString(credential, "label");
        QJsonObject parameters = ProcJsonEasy::getJsonObject(credential, "parameters");
        data_output.hfp_purchases = ProcJsonEasy::getBool(parameters, "hfp_purchase");
        data_output.hires_purchases = ProcJsonEasy::getBool(parameters, "hires_purchases_streaming");
        data_output.hires_streaming = ProcJsonEasy::getBool(parameters, "hires_streaming");
        data_output.lossless_streaming = ProcJsonEasy::getBool(parameters, "lossless_streaming");
        data_output.lossy_streaming = ProcJsonEasy::getBool(parameters, "lossy_streaming");
        data_output.mobile_streaming = ProcJsonEasy::getBool(parameters, "mobile_streaming");
        data_output.offline_streaming = ProcJsonEasy::getBool(parameters, "offline_streaming");

        return data_output;
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : functions - Qobuz 데이터 변환 관련
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 타이달 API에서 반환하는 이미지 key값에 대해서, 타이달 Resource URL로 변환하여 반환
     * @details  베어앱 노트 참고 - [[Rose Qobuz - 이미지 URL화 방법]]
     * @param qobuzImageKey 타이달API에서 반환하는 이미지 key값
     * @param imageSize
     * @return 타이달 Resource Url 반환
     */
    QString ConvertData::convertImagePath(const QString qobuzImageKey, const ImageSize_Square imageSize){
        if(qobuzImageKey.isEmpty()){
            return "";
        }
        else{
            QString output = qobuzImageKey;
            output.replace(QString("-"), QString("/"), Qt::CaseSensitive);
//            output.insert(0, "https://resources.qobuz.com/images/");          // https  (맥에서 너무 느려서)
            output.insert(0, "http://resources.tidal.com/images/");             // http
            output.append("/");
            output.append( ConvertData::getImageJpgName(imageSize) );
            return output;
        }
    }


    /**
     * @brief 타이달 API에서 반환하는 이미지 key값에 대해서, 타이달 Resource URL로 변환하여 반환
     * @details  베어앱 노트 참고 - [[Rose Qobuz - 이미지 URL화 방법]]
     * @param qobuzImageKey
     * @param imageSize
     * @return
     */
    QString ConvertData::convertImagePath(const QString qobuzImageKey, const ImageSize_Ractangle imageSize){
        if(qobuzImageKey.isEmpty()){
            return "";
        }
        else{
            QString output = qobuzImageKey;
            output.replace(QString("-"), QString("/"), Qt::CaseSensitive);
//            output.insert(0, "https://resources.qobuz.com/images/");          // https  (맥에서 너무 느려서)
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
     * @details id=0 이면 QOBUZ 을 반환하고, 아니면 name을 반환
     * @param p_jsonObj_creator JSON 중 creator 에 해당되는 Object 정보.
     * @return
     */
    QString ConvertData::getCreatorName(const QJsonObject p_jsonObj_creator){
        int creator_id = ProcJsonEasy::getInt(p_jsonObj_creator, "id", 0);
        if(creator_id == 0){
            return "QOBUZ";
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
     * @brief QOBUZ가 Track 정보 반환하는 JsonObject로부터 TrackItemData 로 변환해서 반환한다.
     * @param jsonObj
     * @return
     */
    qobuz::TrackItemData ConvertData::make_trackData_fromQobuzJsonObj(const QJsonObject &jsonObj){//cheon210916-total

        //print_qobuz_func();
        //qDebug() << "7. [QOBUZ][SEQ][FUNC] : ConvertData_forQobuz.cpp -> qobuz::TrackItemData ConvertData::make_trackData_fromQobuzJsonObj(const QJsonObject &jsonObj)";

        //QJsonDocument doc(jsonObj);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "qobuz::TrackItemData ConvertData::make_trackData_fromQobuzJsonObj---p_jsonObj : " << strJson << "\n";

        // 정보 담을 struct
        //qobuz::TrackItemData data_output;
        //data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        //data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        //data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        //data_output.audioQuality = ProcJsonEasy::getString(jsonObj, "audioQuality");

        //QJsonObject jsonObj_album = ProcJsonEasy::getJsonObject(jsonObj, "album");
        //data_output.albumName = ProcJsonEasy::getString(jsonObj_album, "title");
        //data_output.album_id = ProcJsonEasy::getInt(jsonObj_album, "id");

        // 앨범 이미지
        //QString album_cover = ProcJsonEasy::getString(jsonObj_album, "cover");
        //data_output.image = album_cover.isEmpty() ? "" : qobuz::ConvertData::convertImagePath(album_cover, ImageSize_Square::Size_160x160);

        // for artist
        //QJsonArray tmp_jsonArr_artist = ProcJsonEasy::getJsonArray(jsonObj, "artists");
        //data_output.list_artist_id = ProcJsonEasy::getListInt_fromJsonArr(tmp_jsonArr_artist, "id");
        //data_output.list_artist_name = ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_artist, "name");

        qobuz::TrackItemData data_output;

        //QJsonObject jsonPar_album = ProcJsonEasy::getJsonObject(p_jsonObj, "tracks");
        //QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonPar_album, "items");

        // for album
        QJsonObject jsonPar_album = ProcJsonEasy::getJsonObject(jsonObj, "album");
        data_output.album_duration = ProcJsonEasy::getInt(jsonPar_album, "duration");
        data_output.album_hires = ProcJsonEasy::getBool(jsonPar_album, "hires");
        data_output.album_hires_streamable = ProcJsonEasy::getBool(jsonPar_album, "hires_streamable");
        data_output.album_id = ProcJsonEasy::getString(jsonPar_album, "id");

        QJsonObject jsonPar_image = ProcJsonEasy::getJsonObject(jsonPar_album, "image");
        data_output.album_image_large = ProcJsonEasy::getString(jsonPar_image, "large");
        data_output.album_image_small = ProcJsonEasy::getString(jsonPar_image, "small");
        data_output.album_image_thumbnail = ProcJsonEasy::getString(jsonPar_image, "thumbnail");

        data_output.album_streamable = ProcJsonEasy::getBool(jsonPar_album, "streamable");
        data_output.album_title = ProcJsonEasy::getString(jsonPar_album, "title");
        data_output.album_version = ProcJsonEasy::getString(jsonPar_album, "version");
        data_output.album_tracks_count = ProcJsonEasy::getInt(jsonPar_album, "tracks_count");

        QJsonObject jsonPar_comporser = ProcJsonEasy::getJsonObject(jsonObj, "composer");
        data_output.composer_id = ProcJsonEasy::getInt(jsonPar_comporser, "id");
        data_output.composer_name = ProcJsonEasy::getString(jsonPar_comporser, "name");

        data_output.parental_warning = ProcJsonEasy::getBool(jsonObj,"parental_warning");


        if(jsonObj.contains("qobuz_id"))
        {
            data_output.qobuz_id = ProcJsonEasy::getInt(jsonObj, "qobuz_id");
        }
        else
        {
            data_output.qobuz_id = ProcJsonEasy::getInt(jsonPar_album, "qobuz_id");
        }

        // for artist
        if(jsonObj.contains("artist"))
        {
            QJsonObject jsonPar_artist = ProcJsonEasy::getJsonObject(jsonObj, "artist");
            data_output.list_artist_albums_count.append(ProcJsonEasy::getInt(jsonPar_artist, "albums_count"));
            data_output.list_artist_id.append(ProcJsonEasy::getInt(jsonPar_artist, "id"));
            data_output.list_artist_name.append(ProcJsonEasy::getString(jsonPar_artist, "name"));
        }
        else if(jsonObj.contains("album"))
        {
            QJsonObject jsonPar_split = ProcJsonEasy::getJsonObject(jsonObj, "album");
            QJsonObject jsonPar_artist = ProcJsonEasy::getJsonObject(jsonPar_split, "artist");
            data_output.list_artist_albums_count.append(ProcJsonEasy::getInt(jsonPar_artist, "albums_count"));
            data_output.list_artist_id.append(ProcJsonEasy::getInt(jsonPar_artist, "id"));
            data_output.list_artist_name.append(ProcJsonEasy::getString(jsonPar_artist, "name"));
        }

        // for track
        data_output.copyright = ProcJsonEasy::getString(jsonObj, "copyright");
        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        data_output.hires = ProcJsonEasy::getBool(jsonObj, "hires");
        data_output.hires_streamable = ProcJsonEasy::getBool(jsonObj, "hires_streamable");
        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        data_output.maximum_bit_depth = ProcJsonEasy::getInt(jsonObj, "maximum_bit_depth");
        data_output.maximum_sampling_rate = ProcJsonEasy::getDouble(jsonObj, "maximum_sampling_rate");
        data_output.media_number = ProcJsonEasy::getInt(jsonObj, "media_number");

        QJsonObject jsonPar_performer = ProcJsonEasy::getJsonObject(jsonObj, "performer");
        data_output.performer_id = ProcJsonEasy::getInt(jsonPar_performer, "id");
        data_output.performer_name = ProcJsonEasy::getString(jsonPar_performer, "name");

        data_output.performers = ProcJsonEasy::getString(jsonObj, "performers");
        data_output.previewable = ProcJsonEasy::getBool(jsonObj, "previewable");
        data_output.purchasable = ProcJsonEasy::getBool(jsonObj, "purchasable");
        data_output.sampleable = ProcJsonEasy::getBool(jsonObj, "sampleable");
        data_output.streamable = ProcJsonEasy::getBool(jsonObj, "streamable");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.version = ProcJsonEasy::getString(jsonObj, "version");
        data_output.work = ProcJsonEasy::getString(jsonObj, "work");
        data_output.track_number = ProcJsonEasy::getInt(jsonObj, "track_number");


        return data_output;
    }


    /**
     * @brief Qobuz가 Album 정보 반환하는 JsonObject로부터 TrackItemData 로 변환해서 반환한다.
     * @param jsonObj
     * @return
     */
    qobuz::AlbumItemData ConvertData::make_albumData_fromQobuzJsonObj(const QJsonObject &jsonObj){

        //qDebug() << "[QOBUZ][SEQ][FUNC] : ConvertData_forQobuz.cpp -> qobuz::AlbumItemData ConvertData::make_albumData_fromQobuzJsonObj(const QJsonObject &jsonObj)";
        //print_qobuz_func();
        //qDebug() << "URL -> https://www.qobuz.com/api.json/0.2/album/getFeatured?type=new-releases Info";

        // 정보 담을 struct
        AlbumItemData data_output;
        //data_output.image =  qobuz::ConvertData::convertImagePath(ProcJsonEasy::getString(jsonObj, "cover"), ImageSize_Square::Size_320x320);
        //data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        //data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        //data_output.numberOfTracks = ProcJsonEasy::getInt(jsonObj, "numberOfTracks");

        // for album info       Added diskept 28/12/2020
        data_output.id = ProcJsonEasy::getString(jsonObj, "id");                // Qobuz's Album ID is of type string   Added diskept 27/12/2020
        data_output.hires = ProcJsonEasy::getBool(jsonObj, "hires");
        data_output.hires_streamable = ProcJsonEasy::getBool(jsonObj, "hires_streamable");
        data_output.streamable = ProcJsonEasy::getBool(jsonObj, "streamable");

        data_output.maximum_bit_depth = ProcJsonEasy::getInt(jsonObj, "maximum_bit_depth");//cheon210717-hires
        data_output.maximum_channel_count = ProcJsonEasy::getInt(jsonObj, "maximum_channel_count");//cheon210717-hires
        data_output.maximum_sampling_rate = ProcJsonEasy::getDouble(jsonObj, "maximum_sampling_rate");//cheon210717-hires


        //qDebug() << "\n\nAlbumItemData ConvertData::make_albumData_fromQobuzJsonObj---data_output.maximum_bit_depth" << data_output.maximum_bit_depth << "\n\n";//cheon210717-hires

        QJsonObject jsonObjectArticles;
        if(jsonObj.contains("articles")){//cheontidalhires
           QJsonArray jsonArray = jsonObj.value("articles").toArray();
           if(jsonArray.count() > 0){
               jsonObjectArticles = jsonArray[0].toObject();
           }
        }

        data_output.release_date_original = ProcJsonEasy::getString(jsonObj, "release_date_original");
        data_output.release_date_stream = ProcJsonEasy::getString(jsonObj, "release_date_stream");//cheontidalhires
        data_output.resolution = ProcJsonEasy::getString(jsonObjectArticles, "description");//cheontidalhires
        data_output.description = ProcJsonEasy::getString(jsonObj, "description");
        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");//cheontidalhires
        data_output.popularity = ProcJsonEasy::getInt(jsonObj, "popularity");//cheon210603
        //qDebug() << "data_output.description : " << data_output.description;


        QJsonObject jsonPar_image = ProcJsonEasy::getJsonObject(jsonObj, "image");

        data_output.image_large = ProcJsonEasy::getString(jsonPar_image, "large");
        data_output.image_small = ProcJsonEasy::getString(jsonPar_image, "small");
        data_output.image_thumbnail = ProcJsonEasy::getString(jsonPar_image, "thumbnail");

        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.version = ProcJsonEasy::getString(jsonObj, "version");
        data_output.tracks_count = ProcJsonEasy::getInt(jsonObj, "tracks_count");

        //qDebug() << "Album image url : " <<data_output.image_small << "title : " <<data_output.title
        //         << "id : " << data_output.id << "track count : " << data_output.tracks_count;

        // for label
        QJsonObject jsonPar_label = ProcJsonEasy::getJsonObject(jsonObj, "label");
        data_output.label_albums_count = ProcJsonEasy::getInt(jsonPar_label, "albums_count");
        data_output.label_id = ProcJsonEasy::getInt(jsonPar_label, "id");
        data_output.label_name = ProcJsonEasy::getString(jsonPar_label, "name");

        // for composer
        QJsonObject jsonPar_composer = ProcJsonEasy::getJsonObject(jsonObj, "composer");
        data_output.composer_id = ProcJsonEasy::getInt(jsonPar_composer, "id");
        data_output.composer_name = ProcJsonEasy::getString(jsonPar_composer, "name");

        // for genre
        QJsonObject jsonPar_genre = ProcJsonEasy::getJsonObject(jsonObj, "genre");
        data_output.genre_name = ProcJsonEasy::getString(jsonPar_genre, "name");

        // for artist
        QJsonObject jsonPar_artist = ProcJsonEasy::getJsonObject(jsonObj, "artist");
        data_output.artist_albums_count = ProcJsonEasy::getInt(jsonPar_artist, "albums_count");
        data_output.artist_id = ProcJsonEasy::getInt(jsonPar_artist, "id");
        data_output.artist_image = ProcJsonEasy::getString(jsonPar_artist, "image");
        data_output.artist_name = ProcJsonEasy::getString(jsonPar_artist, "name");
        data_output.artist_picture = ProcJsonEasy::getString(jsonPar_artist, "picture");

        // for artists
        QJsonArray tmp_jsonArr_artists = ProcJsonEasy::getJsonArray(jsonObj, "artists");
        data_output.list_artist_id = ProcJsonEasy::getListInt_fromJsonArr(tmp_jsonArr_artists, "id");
        data_output.list_artist_name = ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_artists, "name");

        //qDebug() << "Artist id : " << data_output.list_artist_id << "Name : " << data_output.list_artist_name << "\n";


        return data_output;
    }


    /**
     * @brief Qobuz가 Playlist 정보 반환하는 JsonObject로부터 TrackItemData 로 변환해서 반환한다.
     * @param jsonObj
     * @return
     */
    qobuz::PlaylistItemData ConvertData::make_playlistData_fromQobuzJsonObj(const QJsonObject &jsonObj){

        //qDebug() << "[QOBUZ][SEQ][FUNC] : ConvertData_forQobuz.cpp -> qobuz::AlbumItemData ConvertData::make_albumData_fromQobuzJsonObj(const QJsonObject &jsonObj)";
        //print_qobuz_func();
        //qDebug() << "URL->https://www.qobuz.com/api.json/0.2/playlist/getFeatured?type=editor-picks Info";

        // 정보 담을 struct
        PlaylistItemData data_output;
        //data_output.image =  qobuz::ConvertData::convertImagePath(ProcJsonEasy::getString(jsonObj, "squareImage"), ImageSize_Square::Size_320x320);
        //data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        //data_output.uuid = ProcJsonEasy::getString(jsonObj, "uuid");
        //data_output.creatorName = qobuz::ConvertData::getCreatorName(ProcJsonEasy::getJsonObject(jsonObj, "creator"));
        //data_output.description = ProcJsonEasy::getString(jsonObj, "description");
        //data_output.numberOfTracks = ProcJsonEasy::getInt(jsonObj, "numberOfTracks");

        bool img_rectangle = jsonObj.contains("image_rectangle");
        bool img_square = jsonObj.contains("images300");

        // for album info       Added diskept 30/12/2020
        if(img_rectangle)
        {
            QJsonArray jsonArr_image_rect = ProcJsonEasy::getJsonArray(jsonObj, "image_rectangle");
            data_output.image = jsonArr_image_rect[0].toString();
        }
        else if(img_square)
        {
            QJsonArray jsonArr_image = ProcJsonEasy::getJsonArray(jsonObj, "images300");
            data_output.image = jsonArr_image[0].toString();
        }
        else
        {
            data_output.image = "";
        }

        data_output.title = ProcJsonEasy::getString(jsonObj, "name");
        int tmp_id = ProcJsonEasy::getInt(jsonObj, "id");
        char ctmp_id[30];
        sprintf(ctmp_id, "%d", tmp_id);
        data_output.id = ctmp_id;
        if(jsonObj.contains("lastUpdated")){
            data_output.lastUpdated = ProcJsonEasy::getString(jsonObj, "lastUpdated");
        }
        else if(jsonObj.contains("updated_at")){
            int tmp_date = ProcJsonEasy::getInt(jsonObj, "updated_at");
            data_output.lastUpdated = QString("%1").arg(tmp_date);
        }
        data_output.popularity = ProcJsonEasy::getInt(jsonObj, "popularity");
        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        data_output.description = ProcJsonEasy::getString(jsonObj, "description");
        data_output.numberOfTracks = ProcJsonEasy::getInt(jsonObj, "numberOfTracks");
        data_output.hires = ProcJsonEasy::getBool(jsonObj, "hires");
        data_output.hires_streamable = ProcJsonEasy::getBool(jsonObj, "hires_streamable");

        QJsonObject jsonObj_owner = ProcJsonEasy::getJsonObject(jsonObj, "owner");
        data_output.ownerName = ProcJsonEasy::getString(jsonObj_owner, "name");
        data_output.tracks_count = ProcJsonEasy::getInt(jsonObj, "tracks_count");
        data_output.album_total_cnt = ProcJsonEasy::getInt(jsonObj, "similarPlaylist");
        //data_output.flagCreatedByUser = ProcJsonEasy::getBool(jsonObj, "");

        //qDebug() << "image url : " <<data_output.image << "title : " <<data_output.title << "id : " << data_output.id
        //         << "ownerName : " << data_output.ownerName << "track count : " << data_output.tracks_count; //<< "description : " << data_output.description

        return data_output;
    }



    /**
     * @brief QOBUZ가 Artist 정보 반환하는 JsonObject로부터 ArtistItemData 로 변환해서 반환한다.
     * @param jsonObj
     * @return
     */
    qobuz::ArtistItemData ConvertData::make_artistData_fromQobuzJsonObj(const QJsonObject &jsonObj){
        // 정보 담을 struct
        ArtistItemData data_output;
        //data_output.image =  qobuz::ConvertData::convertImagePath(ProcJsonEasy::getString(jsonObj, "picture"), ImageSize_Square::Size_320x320);
        data_output.name = ProcJsonEasy::getString(jsonObj, "name");
        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        data_output.albums_count = ProcJsonEasy::getInt(jsonObj, "albums_count");

        if(jsonObj.contains("image")){
            QJsonObject jsonPar_image = ProcJsonEasy::getJsonObject(jsonObj, "image");
            if(!jsonPar_image.isEmpty()){
                bool img_square = jsonPar_image.contains("large");
                if(img_square)
                {
                    data_output.image_large = ProcJsonEasy::getString(jsonPar_image, "large");
                    data_output.image_medium = ProcJsonEasy::getString(jsonPar_image, "medium");
                    data_output.image_small = ProcJsonEasy::getString(jsonPar_image, "small");
                }
            }
        }
        else if(jsonObj.contains("picture")){
            if(!ProcJsonEasy::getString(jsonObj, "picture").isEmpty()){
                data_output.image_large = ProcJsonEasy::getString(jsonObj, "picture");
                data_output.image_medium = ProcJsonEasy::getString(jsonObj, "picture");
                data_output.image_small = ProcJsonEasy::getString(jsonObj, "picture");
            }
            else{
                data_output.image_medium = "qobuz_artist";
            }
        }

        QJsonObject jsonPar_biography = ProcJsonEasy::getJsonObject(jsonObj, "biography");
        data_output.biography = ProcJsonEasy::getString(jsonPar_biography, "content");

        return data_output;
    }


    /**
     * @brief TIDAL이 Video 정보 반환하는 JsonObject로부터 VideoItemData 로 변환해서 반환한다.
     * @param jsonObj
     * @return
     */
    qobuz::VideoItemData ConvertData::make_videoData_fromQobuzJsonObj(const QJsonObject &jsonObj){
        // 정보 담을 struct
        VideoItemData data_output;
        data_output.image =  qobuz::ConvertData::convertImagePath(ProcJsonEasy::getString(jsonObj, "imageId"), ImageSize_Ractangle::Size_320x180);
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
    QJsonObject ConvertData::getObjectJson_itemPositionData(const qobuz::ItemPositionData &data){
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
    qobuz::ItemPositionData ConvertData::convertData_itemPositionData(const QJsonObject &jsonObj){
        qobuz::ItemPositionData data_output;
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
    QJsonObject ConvertData::getObjectJson_playlistData(const qobuz::PlaylistItemData &data){
        QJsonObject jsonObj;
        jsonObj.insert("image", data.image);
        jsonObj.insert("title", data.title);
        jsonObj.insert("id", data.id);
        jsonObj.insert("ownerName", data.ownerName);
        jsonObj.insert("description", data.description);
        jsonObj.insert("album_total_cnt", data.album_total_cnt);
        jsonObj.insert("hires", data.hires);
        jsonObj.insert("hires_streamable", data.hires_streamable);
        jsonObj.insert("duration", data.duration);

        jsonObj.insert("release_date_stream", data.release_date_stream);
        jsonObj.insert("popularity", data.popularity);//cheon210603
        jsonObj.insert("lastUpdated", data.lastUpdated);//cheon210603

        jsonObj.insert("numberOfTracks", data.tracks_count);
        jsonObj.insert("flagCreatedByUser", data.flagCreatedByUser);

        return jsonObj;
    }

    /**
     * @brief JsonObject을 PlaylistItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    qobuz::PlaylistItemData ConvertData::convertData_playlistData(const QJsonObject &jsonObj){
        qobuz::PlaylistItemData data_output;
        data_output.image = ProcJsonEasy::getString(jsonObj, "image");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        //data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        data_output.id = ProcJsonEasy::getString(jsonObj, "id");
        data_output.ownerName = ProcJsonEasy::getString(jsonObj, "ownerName");
        data_output.description = ProcJsonEasy::getString(jsonObj, "description");
        data_output.album_total_cnt = ProcJsonEasy::getInt(jsonObj, "album_total_cnt");
        data_output.hires = ProcJsonEasy::getBool(jsonObj, "hires");
        data_output.hires_streamable = ProcJsonEasy::getBool(jsonObj, "hires_streamable");
        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");


        data_output.release_date_stream = ProcJsonEasy::getString(jsonObj, "release_date_stream");
        data_output.popularity = ProcJsonEasy::getInt(jsonObj, "popularity");//cheon210603
        data_output.lastUpdated = ProcJsonEasy::getString(jsonObj, "lastUpdated");//cheon210603

        data_output.tracks_count = ProcJsonEasy::getInt(jsonObj, "numberOfTracks");
        data_output.flagCreatedByUser = ProcJsonEasy::getBool(jsonObj, "flagCreatedByUser");
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
    QJsonObject ConvertData::getObjectJson_albumData(const qobuz::AlbumItemData &data){
        QJsonObject jsonObj;
        jsonObj.insert("image_large", data.image_large);
        jsonObj.insert("image_small", data.image_small);
        jsonObj.insert("image_thumbnail", data.image_thumbnail);
        jsonObj.insert("title", data.title);        
        jsonObj.insert("version", data.version);
        jsonObj.insert("id", data.id);
        jsonObj.insert("album_total_cnt", data.album_total_cnt);
        jsonObj.insert("hires", data.hires);
        jsonObj.insert("hires_streamable", data.hires_streamable);
        jsonObj.insert("streamable", data.streamable);

        jsonObj.insert("maximum_bit_depth", data.maximum_bit_depth);//cheon210717-hires
        jsonObj.insert("maximum_channel_count", data.maximum_channel_count);//cheon210717-hires
        jsonObj.insert("maximum_sampling_rate", data.maximum_sampling_rate);//cheon210717-hires
        //qDebug() << "\n\nConvertData::getObjectJson_albumData---data_output.maximum_bit_depth" << data.maximum_bit_depth << "\n\n";//cheon210717-hires

        jsonObj.insert("tracks_count", data.tracks_count);
        jsonObj.insert("description", data.description);//cheontidalhires
        jsonObj.insert("release_date_stream", data.release_date_stream);//cheontidalhires
        jsonObj.insert("duration", data.duration);//cheontidalhires
        jsonObj.insert("popularity", data.popularity);//cheontidalhires
        jsonObj.insert("numberOfTracks", data.numberOfTracks);//cheon210604

        jsonObj.insert("label_id", data.label_id);
        jsonObj.insert("label_name", data.label_name);

        jsonObj.insert("composer_id", data.composer_id);
        jsonObj.insert("composer_name", data.composer_name);

        jsonObj.insert("genre_name", data.genre_name);

        jsonObj.insert("artist_albums_count", data.artist_albums_count);
        jsonObj.insert("artist_id", data.artist_id);
        jsonObj.insert("artist_image", data.artist_image);
        jsonObj.insert("artist_name", data.artist_name);
        jsonObj.insert("artist_picture", data.artist_picture);

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
    qobuz::AlbumItemData ConvertData::convertData_albumData(const QJsonObject &jsonObj){
       // qDebug()<<"77777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777";//cheontidalhires
       // QJsonDocument doc(jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " AlbumItemData ConvertData::convertData_albumData()-jsonObj =: " << strJson;//cheonprint//cheontidalhires

        qobuz::AlbumItemData data_output;
        data_output.image_large = ProcJsonEasy::getString(jsonObj, "image_large");
        data_output.image_small = ProcJsonEasy::getString(jsonObj, "image_small");
        data_output.image_thumbnail = ProcJsonEasy::getString(jsonObj, "image_thumbnail");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.version = ProcJsonEasy::getString(jsonObj, "version");
        data_output.id = ProcJsonEasy::getString(jsonObj, "id");
        data_output.album_total_cnt = ProcJsonEasy::getInt(jsonObj, "album_total_cnt");
        data_output.hires = ProcJsonEasy::getBool(jsonObj, "hires");
        data_output.hires_streamable = ProcJsonEasy::getBool(jsonObj, "hires_streamable");
        data_output.streamable = ProcJsonEasy::getBool(jsonObj, "streamable");

        data_output.tracks_count = ProcJsonEasy::getInt(jsonObj, "tracks_count");
        data_output.description = ProcJsonEasy::getString(jsonObj, "description");//cheontidalhires

        data_output.maximum_bit_depth  = ProcJsonEasy::getInt(jsonObj, "maximum_bit_depth");//cheon210717-hires
        data_output.maximum_channel_count  = ProcJsonEasy::getInt(jsonObj, "maximum_channel_count");//cheon210717-hires
        data_output.maximum_sampling_rate  = ProcJsonEasy::getDouble(jsonObj, "maximum_sampling_rate");//cheon210717-hires
        //qDebug() << "\n\nConvertData::convertData_albumData---data_output.maximum_bit_depth" << data_output.maximum_bit_depth << "\n\n";//cheon210717-hires

        data_output.release_date_stream = ProcJsonEasy::getString(jsonObj, "release_date_stream");//cheontidalhires
        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        data_output.popularity = ProcJsonEasy::getInt(jsonObj, "popularity");//cheontidalhires
        data_output.numberOfTracks = ProcJsonEasy::getInt(jsonObj, "tracks_count");//cheon210604

        data_output.label_id = ProcJsonEasy::getInt(jsonObj, "label_id");
        data_output.label_name = ProcJsonEasy::getString(jsonObj, "label_name");

        data_output.composer_id = ProcJsonEasy::getInt(jsonObj, "composer_id");
        data_output.composer_name = ProcJsonEasy::getString(jsonObj, "composer_name");

        data_output.genre_name = ProcJsonEasy::getString(jsonObj, "genre_name");

        data_output.artist_albums_count = ProcJsonEasy::getInt(jsonObj, "artist_albums_count");
        data_output.artist_id = ProcJsonEasy::getInt(jsonObj, "artist_id");
        data_output.artist_image = ProcJsonEasy::getString(jsonObj, "artist_image");
        data_output.artist_name = ProcJsonEasy::getString(jsonObj, "artist_name");
        data_output.artist_picture = ProcJsonEasy::getString(jsonObj, "artist_picture");

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
    QJsonObject ConvertData::getObjectJson_trackData(const qobuz::TrackItemData &data){             // FIXED :: 31/12/2020
        QJsonObject jsonObj;
        //jsonObj.insert(("image", data.image);)
        //jsonObj.insert("albumName", data.album_title);
        //jsonObj.insert("duration", data.duration);

        //jsonObj.insert("album_id", data.album_id);

        //QJsonArray jsonArr_id;
        //foreach(int tmp_id, data.list_artist_id){
        //    jsonArr_id.append(tmp_id);
        //}
        //jsonObj.insert("list_artist_id", jsonArr_id);

        //QJsonArray jsonArr_name;
        //foreach(QString tmp_name, data.list_artist_name){
        //    jsonArr_name.append(tmp_name);
        //}
        //jsonObj.insert("list_artist_name", jsonArr_name);

        jsonObj.insert("album_duration", data.album_duration);
        jsonObj.insert("album_hires", data.album_hires);
        jsonObj.insert("album_hires_streamable", data.album_hires_streamable);
        jsonObj.insert("album_id", data.album_id);
        jsonObj.insert("album_image_large", data.album_image_large);
        jsonObj.insert("album_image_small", data.album_image_small);
        jsonObj.insert("album_image_thumbnail", data.album_image_thumbnail);
        jsonObj.insert("album_title", data.album_title);
        jsonObj.insert("album_version", data.album_version);
        jsonObj.insert("album_tracks_count", data.album_tracks_count);

        QJsonArray jsonArr_artist_albums_count;
        foreach(int tmp_id, data.list_artist_albums_count){
            jsonArr_artist_albums_count.append(tmp_id);
        }
        jsonObj.insert("list_artist_albums_count", jsonArr_artist_albums_count);

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

        jsonObj.insert("composer_id", data.composer_id);
        jsonObj.insert("composer_name", data.composer_name);

        jsonObj.insert("copyright", data.copyright);
        jsonObj.insert("duration", data.duration);
        jsonObj.insert("hires", data.hires);
        jsonObj.insert("hires_streamable", data.hires_streamable);
        jsonObj.insert("id", data.id);
        jsonObj.insert("maximum_bit_depth", data.maximum_bit_depth);
        jsonObj.insert("maximum_sampling_rate", data.maximum_sampling_rate);
        jsonObj.insert("media_number", data.media_number);
        jsonObj.insert("performer_id", data.performer_id);
        jsonObj.insert("performer_name", data.performer_name);
        jsonObj.insert("performers", data.performers);
        jsonObj.insert("previewable", data.previewable);
        jsonObj.insert("purchasable", data.purchasable);
        jsonObj.insert("qobuz_id", data.qobuz_id);
        jsonObj.insert("sampleable", data.sampleable);
        jsonObj.insert("streamable", data.streamable);
        jsonObj.insert("title", data.title);
        jsonObj.insert("version", data.version);
        jsonObj.insert("work", data.work);
        jsonObj.insert("track_number", data.track_number);

        return jsonObj;
    }

    /**
     * @brief JsonObject을 TrackItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    qobuz::TrackItemData ConvertData::convertData_trackItemData(const QJsonObject &jsonObj){             // FIXED :: 31/12/2020
        qobuz::TrackItemData data_output;

        data_output.album_duration = ProcJsonEasy::getInt(jsonObj, "album_duration");
        data_output.album_hires = ProcJsonEasy::getBool(jsonObj, "album_hires");
        data_output.album_hires_streamable = ProcJsonEasy::getBool(jsonObj, "album_hires_streamable");
        data_output.album_id = ProcJsonEasy::getString(jsonObj, "album_id");
        data_output.album_image_large = ProcJsonEasy::getString(jsonObj, "album_image_large");
        data_output.album_image_small = ProcJsonEasy::getString(jsonObj, "album_image_small");
        data_output.album_image_thumbnail = ProcJsonEasy::getString(jsonObj, "album_image_thumbnail");
        data_output.album_title = ProcJsonEasy::getString(jsonObj, "album_title");
        data_output.album_version = ProcJsonEasy::getString(jsonObj, "album_version");
        data_output.album_tracks_count = ProcJsonEasy::getInt(jsonObj, "album_tracks_count");

        data_output.composer_id = ProcJsonEasy::getInt(jsonObj, "composer_id");
        data_output.composer_name = ProcJsonEasy::getString(jsonObj, "composer_name");

        data_output.copyright = ProcJsonEasy::getString(jsonObj, "copyright");
        data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        data_output.hires = ProcJsonEasy::getBool(jsonObj, "hires");
        data_output.hires_streamable = ProcJsonEasy::getBool(jsonObj, "hires_streamable");
        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");

        data_output.maximum_bit_depth = ProcJsonEasy::getInt(jsonObj, "maximum_bit_depth");
        data_output.maximum_sampling_rate = ProcJsonEasy::getInt(jsonObj, "maximum_sampling_rate");
        data_output.media_number = ProcJsonEasy::getInt(jsonObj, "media_number");
        data_output.performer_id = ProcJsonEasy::getInt(jsonObj, "performer_id");
        data_output.performer_name = ProcJsonEasy::getString(jsonObj, "performer_name");
        data_output.performers = ProcJsonEasy::getString(jsonObj, "performers");
        data_output.previewable = ProcJsonEasy::getBool(jsonObj, "previewable");
        data_output.purchasable = ProcJsonEasy::getBool(jsonObj, "purchasable");
        data_output.qobuz_id = ProcJsonEasy::getInt(jsonObj, "qobuz_id");
        data_output.sampleable = ProcJsonEasy::getBool(jsonObj, "sampleable");
        data_output.streamable = ProcJsonEasy::getBool(jsonObj, "streamable");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.version = ProcJsonEasy::getString(jsonObj, "version");
        data_output.work = ProcJsonEasy::getString(jsonObj, "work");
        data_output.track_number = ProcJsonEasy::getInt(jsonObj, "track_number");

        QVariantList tmp_list_aac = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_albums_count").toVariantList();
        foreach(QVariant tmp_val, tmp_list_aac){
            data_output.list_artist_albums_count.append(tmp_val.value<int>());
        }

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
    QJsonObject ConvertData::getObjectJson_artistData(const qobuz::ArtistItemData &data){
        QJsonObject jsonObj;
        //jsonObj.insert("image", data.image);
        jsonObj.insert("image_large", data.image_large);
        jsonObj.insert("image_medium", data.image_medium);
        jsonObj.insert("image_small", data.image_small);
        jsonObj.insert("biography", data.biography);
        jsonObj.insert("name", data.name);
        jsonObj.insert("id", data.id);
        return jsonObj;
    }

    /**
     * @brief JsonObject을 ArtistItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    qobuz::ArtistItemData ConvertData::convertData_artistItemData(const QJsonObject &jsonObj){
        qobuz::ArtistItemData data_output;
        //data_output.image = ProcJsonEasy::getString(jsonObj, "image");
        data_output.image_large = ProcJsonEasy::getString(jsonObj, "image_large");
        data_output.image_medium = ProcJsonEasy::getString(jsonObj, "image_medium");
        data_output.image_small = ProcJsonEasy::getString(jsonObj, "image_small");
        data_output.biography = ProcJsonEasy::getString(jsonObj, "biography");
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
    QJsonObject ConvertData::getObjectJson_videoData(const qobuz::VideoItemData &data){
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
    qobuz::VideoItemData ConvertData::convertData_videoItemData(const QJsonObject &jsonObj){
        qobuz::VideoItemData data_output;
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
     * @brief qobuz::AlbumItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData(const qobuz::AlbumItemData &data){
        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.title;
        data_header.sub_title = data.list_artist_name.join(",");
        data_header.imageUrl = data.image_thumbnail;
        data_header.data_pk = QString("%1").arg(data.id);
        data_header.type = "QOBUZ";      //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = true;      //j220906 share link

        return data_header;
    }

    /**
     * @brief qobuz::TrackItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData(const qobuz::TrackItemData &data){
        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.title;
        data_header.sub_title = data.list_artist_name.join(",");
        data_header.imageUrl = data.album_image_thumbnail;
        data_header.data_pk = QString("%1").arg(data.id);
        data_header.type = "QOBUZ";      //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = true;      //j220906 share link

        return data_header;
    }

    /**
     * @brief qobuz::PlaylistItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData(const qobuz::PlaylistItemData &data){
        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.title;
        data_header.sub_title = data.ownerName;
        data_header.imageUrl = data.image;
        data_header.data_pk = data.id;
        data_header.type = "QOBUZ";      //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = true;      //j220906 share link

        return data_header;
    }

    /**
     * @brief qobuz::ArtistItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData(const qobuz::ArtistItemData &data){
        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.name;
        data_header.sub_title = data.biography;
        //data_header.imageUrl = data.image;
        data_header.imageUrl = data.image_large;
        data_header.data_pk = QString("%1").arg(data.id);
        data_header.type = "QOBUZ";      //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = true;      //j220906 share link

        return data_header;
    }


    /**
     * @brief qobuz::ArtistItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData(const qobuz::VideoItemData &data){
        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.title;
        data_header.sub_title = data.list_artist_name.join(",");
        data_header.imageUrl = data.image;
        data_header.data_pk = QString("%1").arg(data.id);
        data_header.type = "QOBUZ";      //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = true;      //j220906 share link

        return data_header;
    }


    /**
     * @brief ConvertData::convertEditItem_fromTrackData
     * @param data
     * @return
     */
    editDelegate::ItemEditData ConvertData::convertEditItem_fromTrackData(const qobuz::TrackItemData &data_track){
       editDelegate::ItemEditData data_output;
       data_output.album_image = data_track.album_image_thumbnail;
       data_output.track_title = data_track.title;
       data_output.artist_name = data_track.list_artist_name.join(",");
       data_output.album_title = data_track.album_title;
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
    QJsonObject ConvertData::getObjectJson_pageInfo_trackAllView(const qobuz::PageInfo_TrackAllView &data){
        QJsonObject jsonObj;
        jsonObj.insert("pathTitle", data.pathTitle);
        jsonObj.insert("api_subPath", data.api_subPath);
        jsonObj.insert("page", data.page);
        jsonObj.insert("artist_id", data.artist_id);
        return jsonObj;
    }

    /**
     * @brief JsonObject을 PageInfo_TrackAllView struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    qobuz::PageInfo_TrackAllView ConvertData::convertData_pageInfo_trackAllView(const QJsonObject &jsonObj){
        qobuz::PageInfo_TrackAllView data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");
        data_output.page = ProcJsonEasy::getString(jsonObj, "page");
        data_output.artist_id = ProcJsonEasy::getInt(jsonObj, "artist_id");
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
    QJsonObject ConvertData::getObjectJson_pageInfo_albumAllView(const qobuz::PageInfo_AlbumAllView &data){
        QJsonObject jsonObj;
        jsonObj.insert("pathTitle", data.pathTitle);
        jsonObj.insert("api_subPath", data.api_subPath);
        jsonObj.insert("type", data.type);
        jsonObj.insert("genre_id", data.genre_id);
        jsonObj.insert("genre_ids", data.genre_ids);
        jsonObj.insert("artist_id", data.artist_id);
        jsonObj.insert("page", data.page);

        if(data.list_sameAritst.count() > 0){
            QJsonArray list_sameArtist;
            for(int i = 0; i < data.list_sameAritst.count(); i++){
                QJsonObject sameArtist = getObjectJson_albumData(data.list_sameAritst.at(i));
                list_sameArtist.append(sameArtist);
            }

            jsonObj.insert("sameArtist", list_sameArtist);
        }

        return jsonObj;
    }

    /**
     * @brief JsonObject을 PageInfo_AlbumAllView struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    qobuz::PageInfo_AlbumAllView ConvertData::convertData_pageInfo_albumAllView(const QJsonObject &jsonObj){
        qobuz::PageInfo_AlbumAllView data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");
        data_output.type = ProcJsonEasy::getString(jsonObj, "type");
        data_output.genre_id = ProcJsonEasy::getInt(jsonObj, "genre_id");
        data_output.genre_ids = ProcJsonEasy::getString(jsonObj, "genre_ids");
        data_output.artist_id = ProcJsonEasy::getInt(jsonObj, "artist_id");
        data_output.page = ProcJsonEasy::getString(jsonObj, "page");

        if(jsonObj.contains("sameArtist")){
            QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj, "sameArtist");

            for(int i = 0 ; i < jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                AlbumItemData tmp_data = ConvertData::convertData_albumData(tmp_json);
                data_output.list_sameAritst.append(tmp_data);
            }

        }

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
    QJsonObject ConvertData::getObjectJson_pageInfo_playlistAllView(const qobuz::PageInfo_PlaylistAllView &data){
        QJsonObject jsonObj;
        jsonObj.insert("pathTitle", data.pathTitle);
        jsonObj.insert("api_subPath", data.api_subPath);
        jsonObj.insert("type", data.type);
        jsonObj.insert("genre_id", data.genre_id);
        jsonObj.insert("genre_ids", data.genre_ids);

        if(data.list_similarPlaylist.count() > 0){
            QJsonArray list_similarPlaylist;
            for(int i = 0; i < data.list_similarPlaylist.count(); i++){
                QJsonObject similarplaylist = getObjectJson_playlistData(data.list_similarPlaylist.at(i));
                list_similarPlaylist.append(similarplaylist);
            }

            jsonObj.insert("similarplaylist", list_similarPlaylist);
        }

        return jsonObj;
    }

    /**
     * @brief JsonObject을 PageInfo_PlaylistAllView struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    qobuz::PageInfo_PlaylistAllView ConvertData::convertData_pageInfo_playlistAllView(const QJsonObject &jsonObj){
        qobuz::PageInfo_PlaylistAllView data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");
        data_output.type = ProcJsonEasy::getString(jsonObj, "type");
        data_output.genre_id = ProcJsonEasy::getInt(jsonObj, "genre_id");
        data_output.genre_ids = ProcJsonEasy::getString(jsonObj, "genre_ids");

        if(jsonObj.contains("similarplaylist")){
            QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj, "similarplaylist");

            for(int i = 0 ; i < jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                PlaylistItemData tmp_data = ConvertData::convertData_playlistData(tmp_json);
                data_output.list_similarPlaylist.append(tmp_data);
            }

        }

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
    QJsonObject ConvertData::getObjectJson_pageInfo_artistAllView(const qobuz::PageInfo_ArtistAllView &data){
        QJsonObject jsonObj;
        jsonObj.insert("pathTitle", data.pathTitle);
        jsonObj.insert("api_subPath", data.api_subPath);
        jsonObj.insert("artist_id", data.artist_id);
        return jsonObj;
    }

    /**
     * @brief JsonObject을 PageInfo_ArtistAllView struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    qobuz::PageInfo_ArtistAllView ConvertData::convertData_pageInfo_artistAllView(const QJsonObject &jsonObj){
        qobuz::PageInfo_ArtistAllView data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");
        data_output.artist_id = ProcJsonEasy::getInt(jsonObj, "artist_id");
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
    QJsonObject ConvertData::getObjectJson_pageInfo_videoAllView(const qobuz::PageInfo_VideoAllView &data){
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
    qobuz::PageInfo_VideoAllView ConvertData::convertData_pageInfo_videoAllView(const QJsonObject &jsonObj){
        qobuz::PageInfo_VideoAllView data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");
        return data_output;
    }


    /**
     * @brief QOBUZ가 Track 정보 반환하는 JsonObject로부터 TrackItemData 로 변환해서 반환한다.
     * @param jsonObj
     * @return
     */
    qobuz::TrackItemData_apple ConvertData::make_trackData_fromAppleJsonObj(const QJsonObject &jsonObj){//c220603

        //print_qobuz_func();
        //qDebug() << "7. [QOBUZ][SEQ][FUNC] : ConvertData_forQobuz.cpp -> qobuz::TrackItemData ConvertData::make_trackData_fromQobuzJsonObj(const QJsonObject &jsonObj)";

        //QJsonDocument doc(jsonObj);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "qobuz::TrackItemData ConvertData::make_trackData_fromQobuzJsonObj---p_jsonObj : " << strJson << "\n";

        // 정보 담을 struct
        //qobuz::TrackItemData data_output;
        //data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        //data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        //data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        //data_output.audioQuality = ProcJsonEasy::getString(jsonObj, "audioQuality");

        //QJsonObject jsonObj_album = ProcJsonEasy::getJsonObject(jsonObj, "album");
        //data_output.albumName = ProcJsonEasy::getString(jsonObj_album, "title");
        //data_output.album_id = ProcJsonEasy::getInt(jsonObj_album, "id");

        // 앨범 이미지
        //QString album_cover = ProcJsonEasy::getString(jsonObj_album, "cover");
        //data_output.image = album_cover.isEmpty() ? "" : qobuz::ConvertData::convertImagePath(album_cover, ImageSize_Square::Size_160x160);

        // for artist
        //QJsonArray tmp_jsonArr_artist = ProcJsonEasy::getJsonArray(jsonObj, "artists");
        //data_output.list_artist_id = ProcJsonEasy::getListInt_fromJsonArr(tmp_jsonArr_artist, "id");
        //data_output.list_artist_name = ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_artist, "name");

        qobuz::TrackItemData_apple data_output;

        //QJsonObject jsonPar_album = ProcJsonEasy::getJsonObject(p_jsonObj, "tracks");
        //QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonPar_album, "items");

        // for album
        QJsonObject jsonPar_album = ProcJsonEasy::getJsonObject(jsonObj, "album");
        data_output.album_duration = ProcJsonEasy::getInt(jsonPar_album, "duration");
        data_output.album_hires = ProcJsonEasy::getBool(jsonPar_album, "hires");
        data_output.album_hires_streamable = ProcJsonEasy::getBool(jsonPar_album, "hires_streamable");
        data_output.album_id = ProcJsonEasy::getString(jsonPar_album, "id");

        QJsonObject jsonPar_image = ProcJsonEasy::getJsonObject(jsonPar_album, "image");
        data_output.album_image_large = ProcJsonEasy::getString(jsonPar_image, "large");
        data_output.album_image_small = ProcJsonEasy::getString(jsonPar_image, "small");
        data_output.album_image_thumbnail = ProcJsonEasy::getString(jsonPar_image, "thumbnail");

        data_output.album_streamable = ProcJsonEasy::getBool(jsonPar_album, "streamable");
        data_output.album_title = ProcJsonEasy::getString(jsonPar_album, "title");
        data_output.album_version = ProcJsonEasy::getString(jsonPar_album, "version");
        data_output.album_tracks_count = ProcJsonEasy::getInt(jsonPar_album, "tracks_count");



        QJsonObject jsonPar_comporser = ProcJsonEasy::getJsonObject(jsonObj, "composer");
        data_output.composer_id = ProcJsonEasy::getInt(jsonPar_comporser, "id");
        data_output.composer_name = ProcJsonEasy::getString(jsonPar_comporser, "name");

        if(jsonObj.contains("qobuz_id"))
        {
            data_output.qobuz_id = ProcJsonEasy::getInt(jsonObj, "qobuz_id");
        }
        else
        {
            data_output.qobuz_id = ProcJsonEasy::getInt(jsonPar_album, "qobuz_id");
        }

        // for artist
        if(jsonObj.contains("attributes"))
        {
            QJsonObject jsonPar_attributes = ProcJsonEasy::getJsonObject(jsonObj, "attributes");
            QJsonObject jsonPar_artist = ProcJsonEasy::getJsonObject(jsonPar_attributes, "artwork");//c220603
            //data_output.list_artist_albums_count.append(ProcJsonEasy::getInt(jsonPar_artwork, "albums_count"));
            //data_output.list_artist_id.append(ProcJsonEasy::getInt(jsonPar_artwork, "id"));
            data_output.title.append(ProcJsonEasy::getString(jsonPar_attributes, "name"));
            data_output.list_artist_name.append(ProcJsonEasy::getString(jsonPar_attributes, "artistName"));
            data_output.album_image_thumbnail = ProcJsonEasy::getString(jsonPar_artist, "url");
            data_output.image_width = ProcJsonEasy::getInt(jsonPar_artist, "width");//c220603
            data_output.image_height = ProcJsonEasy::getInt(jsonPar_artist, "height");//
            data_output.duration = ProcJsonEasy::getInt(jsonPar_attributes, "durationInMillis")/1000;


            //if(jsonPar_attributes.contains("album"))
            //{
             //           QJsonObject jsonPar_split = ProcJsonEasy::getJsonObject(jsonObj, "album");


            //}
        }

        // for track
        data_output.copyright = ProcJsonEasy::getString(jsonObj, "copyright");
        //data_output.duration = ProcJsonEasy::getInt(jsonObj, "duration");
        data_output.hires = ProcJsonEasy::getBool(jsonObj, "hires");
        data_output.hires_streamable = ProcJsonEasy::getBool(jsonObj, "hires_streamable");
        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        data_output.maximum_bit_depth = ProcJsonEasy::getInt(jsonObj, "maximum_bit_depth");
        data_output.maximum_sampling_rate = ProcJsonEasy::getDouble(jsonObj, "maximum_sampling_rate");
        data_output.media_number = ProcJsonEasy::getInt(jsonObj, "media_number");

        QJsonObject jsonPar_performer = ProcJsonEasy::getJsonObject(jsonObj, "performer");
        data_output.performer_id = ProcJsonEasy::getInt(jsonPar_performer, "id");
        data_output.performer_name = ProcJsonEasy::getString(jsonPar_performer, "name");

        data_output.performers = ProcJsonEasy::getString(jsonObj, "performers");
        data_output.previewable = ProcJsonEasy::getBool(jsonObj, "previewable");
        data_output.purchasable = ProcJsonEasy::getBool(jsonObj, "purchasable");
        data_output.sampleable = ProcJsonEasy::getBool(jsonObj, "sampleable");
        data_output.streamable = ProcJsonEasy::getBool(jsonObj, "streamable");
        //data_output.title = ProcJsonEasy::getString(jsonObj, "name");
        data_output.version = ProcJsonEasy::getString(jsonObj, "version");
        data_output.work = ProcJsonEasy::getString(jsonObj, "work");
        data_output.track_number = ProcJsonEasy::getInt(jsonObj, "track_number");

        return data_output;
    }
}
