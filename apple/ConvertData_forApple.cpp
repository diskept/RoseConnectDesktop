#include "apple/ConvertData_forApple.h"

#include "common/ProcJsonEasy.h"

#include <QJsonDocument>


namespace apple {

    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about Session info
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief RoseSessionInfo_forApple struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData::getObjectJson_roseSessionInfo(const apple::RoseSessionInfo_forApple &data){

        QJsonObject jsonObj;
        jsonObj.insert("userToken", data.APPLE_AccessToken);
        jsonObj.insert("storefront", data.APPLE_StoreFront);

        return jsonObj;
    }


    /**
     * @brief JsonObject을 RoseSessionInfo_forApple struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    apple::RoseSessionInfo_forApple ConvertData::convertData_roseSessionInfo(const QJsonObject &jsonObj){

        apple::RoseSessionInfo_forApple data_output;
        data_output.APPLE_AccessToken = ProcJsonEasy::getString(jsonObj, "userToken");
        data_output.APPLE_StoreFront = ProcJsonEasy::getString(jsonObj, "storefront");

        return data_output;
    }


    QJsonObject ConvertData::getObjectJson_albumData(const apple::AlbumItemData &data){

        QJsonObject jsonObj;
        jsonObj.insert("cllientKey", data.cllientKey);
        jsonObj.insert("thumbnail", data.thumbnail);
        jsonObj.insert("title", data.title);

        jsonObj.insert("id", data.id);
        jsonObj.insert("totalCount", data.totalCount);

        jsonObj.insert("star", data.star);
        jsonObj.insert("favorite", data.favorite);

        return jsonObj;
    }


    apple::AlbumItemData ConvertData::convertData_albumData(const QJsonObject &jsonObj){

        apple::AlbumItemData data_output;
        data_output.cllientKey = ProcJsonEasy::getString(jsonObj, "cllientKey");
        data_output.thumbnail = ProcJsonEasy::getString(jsonObj, "thumbnail");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");

        data_output.id = ProcJsonEasy::getInt(jsonObj, "id");
        data_output.totalCount = ProcJsonEasy::getInt(jsonObj, "totalCount");

        data_output.star = ProcJsonEasy::getInt(jsonObj, "star");
        data_output.favorite = ProcJsonEasy::getBool(jsonObj, "favorite");

        return data_output;
    }


    QJsonObject ConvertData::getObjectJson_playlistData(const apple::PlaylistItemData &data){

        QJsonObject jsonObj;
        jsonObj.insert("cllientKey", data.cllientKey);
        jsonObj.insert("thumbnail", data.thumbnail);
        jsonObj.insert("title", data.title);

        jsonObj.insert("id", data.id);
        jsonObj.insert("totalCount", data.totalCount);

        jsonObj.insert("star", data.star);
        jsonObj.insert("favorite", data.favorite);

        return jsonObj;
    }


    apple::PlaylistItemData ConvertData::convertData_playlistData(const QJsonObject &jsonObj){

        apple::PlaylistItemData data_output;
        data_output.cllientKey = ProcJsonEasy::getString(jsonObj, "cllientKey");
        data_output.thumbnail = ProcJsonEasy::getString(jsonObj, "thumbnail");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.id = ProcJsonEasy::getString(jsonObj, "id");

        data_output.totalCount = ProcJsonEasy::getInt(jsonObj, "totalCount");

        data_output.star = ProcJsonEasy::getInt(jsonObj, "star");
        data_output.favorite = ProcJsonEasy::getBool(jsonObj, "favorite");

        return data_output;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : convert ItemData to OptMorePopup::HeaderData
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief apple Album 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData_album(const QJsonObject &jsonObj){

        QJsonObject artwork = ProcJsonEasy::getJsonObject(jsonObj, "artwork");
        QString url = ProcJsonEasy::getString(artwork, "url");
        url.replace("{w}x{h}", "60x60");

        OptMorePopup::HeaderData data_header;
        data_header.main_title = ProcJsonEasy::getString(jsonObj, "name");
        data_header.sub_title = ProcJsonEasy::getString(jsonObj, "artistName");
        data_header.imageUrl = url;
        data_header.data_pk = ProcJsonEasy::getString(jsonObj, "id");
        data_header.type = "APPLE";     //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = true;     //j220906 share link

        return data_header;
    }


    /**
     * @brief apple Playlist 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData_playlist(const QJsonObject &jsonObj){

        QJsonObject artwork = ProcJsonEasy::getJsonObject(jsonObj, "artwork");
        QString url = ProcJsonEasy::getString(artwork, "url");
        url.replace("{w}x{h}", "60x60");

        OptMorePopup::HeaderData data_header;
        data_header.main_title = ProcJsonEasy::getString(jsonObj, "name");
        data_header.sub_title = ProcJsonEasy::getString(jsonObj, "artistName");
        data_header.imageUrl = url;
        data_header.data_pk = ProcJsonEasy::getString(jsonObj, "id");
        data_header.type = "APPLE";     //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = true;     //j220906 share link

        return data_header;
    }


    /**
     * @brief apple Track 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData::getConvertOptHeaderData_track(const QJsonObject &jsonObj){

        QJsonObject attributes = ProcJsonEasy::getJsonObject(jsonObj, "attributes");
        QJsonObject artwork = ProcJsonEasy::getJsonObject(attributes, "artwork");
        QString url = ProcJsonEasy::getString(artwork, "url");
        url.replace("{w}x{h}", "60x60");

        OptMorePopup::HeaderData data_header;
        data_header.main_title = ProcJsonEasy::getString(attributes, "name");
        data_header.sub_title = ProcJsonEasy::getString(attributes, "artistName");
        data_header.imageUrl = url;
        data_header.data_pk = ProcJsonEasy::getString(jsonObj, "id");
        data_header.type = "APPLE";     //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = true;     //j220906 share link

        return data_header;
    }
}
