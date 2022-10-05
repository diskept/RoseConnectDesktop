#include "ConvertData_forBugs.h"
#include "common/ProcJsonEasy.h"
#include <QJsonDocument>
#include <QObject>
#include <QDebug>

namespace bugs {

    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about Session info
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief RoseSessionInfo_forBugs struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData_forBugs::getObjectJson_roseSessionInfo(const bugs::RoseSessionInfo_forBugs &data){
        QJsonObject jsonObj;
        jsonObj.insert("BUGS_TokenType", data.BUGS_TokenType);
        jsonObj.insert("BUGS_Scope", data.BUGS_Scope);
        jsonObj.insert("BUGS_ExpiresIn", data.BUGS_ExpiresIn);
        jsonObj.insert("BUGS_AccessToken", data.BUGS_AccessToken);
        jsonObj.insert("BUGS_RefreshToken", data.BUGS_RefreshToken);

        jsonObj.insert("BUGS_Nickname", data.BUGS_Nickname);
        jsonObj.insert("BUGS_TokenType", data.BUGS_TokenType);

        jsonObj.insert("BUGS_ProductName", data.BUGS_ProductName);
        jsonObj.insert("BUGS_ProductDisplayEndDt", data.BUGS_ProductDisplayEndDt);
        jsonObj.insert("BUGS_StreamingYn", data.BUGS_StreamingYn);
        jsonObj.insert("BUGS_FlacPremiumYn", data.BUGS_FlacPremiumYn);

        return jsonObj;
    }

    /**
     * @brief JsonObject을 RoseSessionInfo_forBugs struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    bugs::RoseSessionInfo_forBugs ConvertData_forBugs::convertData_roseSessionInfo(const QJsonObject &jsonObj){

        bugs::RoseSessionInfo_forBugs data_output;
        data_output.BUGS_ProductDisplayEndDt = ProcJsonEasy::getString(jsonObj, "BUGS_ProductDisplayEndDt");
        data_output.BUGS_StreamingYn = ProcJsonEasy::getBool(jsonObj, "BUGS_StreamingYn");
        data_output.BUGS_ExpiresIn = ProcJsonEasy::getString(jsonObj, "BUGS_ExpiresIn");
        data_output.BUGS_ProductName = ProcJsonEasy::getString(jsonObj, "BUGS_ProductName");
        data_output.BUGS_Nickname = ProcJsonEasy::getString(jsonObj, "BUGS_Nickname");

        data_output.BUGS_RefreshToken = ProcJsonEasy::getString(jsonObj, "BUGS_RefreshToken");
        data_output.BUGS_AccessToken = ProcJsonEasy::getString(jsonObj, "BUGS_AccessToken");
        data_output.BUGS_FlacPremiumYn = ProcJsonEasy::getBool(jsonObj, "BUGS_FlacPremiumYn");
        data_output.BUGS_AdultYn = ProcJsonEasy::getBool(jsonObj, "BUGS_AdultYn");
        data_output.BUGS_Scope = ProcJsonEasy::getString(jsonObj, "BUGS_Scope");

        return data_output;
    }





    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : functions - Bugs 데이터 변환 관련
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief
     * @param tidalImageKey 타이달API에서 반환하는 이미지 key값
     * @param imageSize
     * @return 타이달 Resource Url 반환
     */
    template<typename T_ImageSize> QString ConvertData_forBugs::convertImagePath(QString url, QString path, const T_ImageSize imageSize){
        if(path.isEmpty()){
            return "";
        }
        else{
            QString output = url.replace("https://", "http://");
            output += QString("%1").arg(static_cast<int>(imageSize)) + path;

            return output;
        }
    }


//    /**
//     * @brief ConvertData_forBugs::convertImagePath
//     * @param url
//     * @param path
//     * @param imageSize
//     * @return
//     */
//    QString ConvertData_forBugs::convertImagePath(QString url, QString path, const ImageSize_Square_PD_Album imageSize){
//        if(path.isEmpty()){
//            return "";
//        }
//        else{
//            QString output = url.replace("https://", "http://");
//            output += QString("%1").arg(static_cast<int>(imageSize)) + path;

//            return output;
//        }
//    }

//    /**
//     * @brief ConvertData_forBugs::convertImagePath
//     * @param url
//     * @param path
//     * @param imageSize
//     * @return
//     */
//    QString ConvertData_forBugs::convertImagePath(QString url, QString path, const ImageSize_Square_My_Album imageSize){
//        if(path.isEmpty()){
//            return "";
//        }
//        else{
//            QString output = url.replace("https://", "http://");
//            output += QString("%1").arg(static_cast<int>(imageSize)) + path;

//            return output;
//        }
//    }


//    /**
//     * @brief ConvertData_forBugs::convertImagePath
//     * @param url
//     * @param path
//     * @param imageSize
//     * @return
//     */
//    QString ConvertData_forBugs::convertImagePath(QString url, QString path, const ImageSize_Ractangle imageSize){
//        if(path.isEmpty()){
//            return "";
//        }
//        else{
//            QString output = url.replace("https://", "http://");
//            output += QString("%1").arg(static_cast<int>(imageSize)) + path;
//            return output;
//        }
//    }




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
    QJsonObject ConvertData_forBugs::getObjectJson_itemPositionData(const bugs::ItemPositionData &data){
        QJsonObject jsonObj;
        jsonObj.insert("section", data.section);
        jsonObj.insert("index", data.index);
        jsonObj.insert("data_id", data.data_id);
        jsonObj.insert("likes_yn", data.likes_yn);
        return jsonObj;
    }


    /**
     * @brief JsonObject을 ItemPositionData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    bugs::ItemPositionData ConvertData_forBugs::convertData_itemPositionData(const QJsonObject &jsonObj){
        bugs::ItemPositionData data_output;
        data_output.section = ProcJsonEasy::getInt(jsonObj, "section");
        data_output.index = ProcJsonEasy::getInt(jsonObj, "index");
        data_output.data_id = ProcJsonEasy::getString(jsonObj, "data_id");
        data_output.likes_yn = ProcJsonEasy::getBool(jsonObj, "likes_yn");
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
    QJsonObject ConvertData_forBugs::getObjectJson_trackData(const bugs::TrackItemData &data){
        QJsonObject jsonObj;
        jsonObj.insert("track_id", data.track_id);
        jsonObj.insert("track_title", data.track_title);
        jsonObj.insert("len", data.len);
        jsonObj.insert("likes_yn", data.likes_yn);
        jsonObj.insert("service_yn", data.service_yn);
        jsonObj.insert("adult_yn", data.adult_yn);

        jsonObj.insert("album_title", data.album_title);
        jsonObj.insert("album_image", data.album_image);
        jsonObj.insert("album_id", data.album_id);

        jsonObj.insert("bitrates", data.bitrates);

        QJsonArray jsonArr_id;
        foreach(int tmp_id, data.list_artist_id){
            jsonArr_id.append(tmp_id);
        }
        jsonObj.insert("list_artist_id", jsonArr_id);

        QJsonArray jsonArr_name;
        foreach(QString tmp_name, data.list_artist_nm){
            jsonArr_name.append(tmp_name);
        }
        jsonObj.insert("list_artist_nm", jsonArr_name);

        QJsonArray jsonArr_mv_id;
        foreach(int tmp_id, data.list_mv_id){
            jsonArr_mv_id.append(tmp_id);
        }
        jsonObj.insert("list_mv_id", jsonArr_mv_id);

        return jsonObj;
    }

    /**
     * @brief JsonObject을 TrackItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    bugs::TrackItemData ConvertData_forBugs::convertData_trackData(const QJsonObject &jsonObj){
      //  QJsonDocument doc(jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "bugs::TrackItemData ConvertData_forBugs::convertData_trackData()-jsonObj =: " << strJson;//cheonprint//cheontidalhires

        bugs::TrackItemData data_output;
        data_output.track_id = ProcJsonEasy::getInt(jsonObj, "track_id");
        data_output.track_title = ProcJsonEasy::getString(jsonObj, "track_title");
        data_output.album_title = ProcJsonEasy::getString(jsonObj, "album_title");
        data_output.album_image = ProcJsonEasy::getString(jsonObj, "album_image");
        data_output.len = ProcJsonEasy::getString(jsonObj, "len");
        data_output.likes_yn = ProcJsonEasy::getBool(jsonObj, "likes_yn");
        data_output.service_yn = ProcJsonEasy::getBool(jsonObj, "service_yn");
        data_output.adult_yn = ProcJsonEasy::getBool(jsonObj, "adult_yn");

        data_output.album_id = ProcJsonEasy::getInt(jsonObj, "album_id");

        data_output.bitrates = ProcJsonEasy::getString(jsonObj, "bitrates");

        QVariantList tmp_list_id = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_id").toVariantList();
        foreach(QVariant tmp_val, tmp_list_id){
            data_output.list_artist_id.append(tmp_val.value<int>());
        }

        QVariantList tmp_list_name = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_nm").toVariantList();
        foreach(QVariant tmp_val, tmp_list_name){
            data_output.list_artist_nm.append(tmp_val.value<QString>());
        }

        QVariantList tmp_list_mv_id = ProcJsonEasy::getJsonArray(jsonObj, "list_mv_id").toVariantList();
        foreach(QVariant tmp_val, tmp_list_mv_id){
            data_output.list_mv_id.append(tmp_val.value<int>());
        }

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
    QJsonObject ConvertData_forBugs::getObjectJson_albumData(const bugs::AlbumItemData &data){
        QJsonObject jsonObj;
        jsonObj.insert("album_id", data.album_id);
        jsonObj.insert("title", data.title);
        //jsonObj.insert("descr", data.descr);//cheon210604
        //jsonObj.insert("track_count", data.track_count);//cheon210604
        jsonObj.insert("upd_dt", data.upd_dt);//cheon210604
        //qDebug() << "\n\n\n\n\ndata.genre_nm" << data.genre_nm;
        //jsonObj.insert("genre_nm", data.genre_nm);//cheon210605
        jsonObj.insert("album_image", data.album_image);
        jsonObj.insert("likes_yn", data.likes_yn);
        jsonObj.insert("valid_yn", data.valid_yn);

        QJsonArray jsonArr_id;
        foreach(int tmp_id, data.list_artist_id){
            jsonArr_id.append(tmp_id);
        }
        jsonObj.insert("list_artist_id", jsonArr_id);

        QJsonArray jsonArr_name;
        foreach(QString tmp_name, data.list_artist_nm){
            jsonArr_name.append(tmp_name);
        }
        jsonObj.insert("list_artist_nm", jsonArr_name);

        return jsonObj;
    }

    /**
     * @brief JsonObject을 AlbumItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    bugs::AlbumItemData ConvertData_forBugs::convertData_albumData(const QJsonObject &jsonObj){
        //QJsonDocument doc(jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "77777777777777bugs::AlbumItemData AlbumItemData ConvertData::convertData_albumData()-jsonObj =: " << strJson;//cheonprint//cheontidalhires

        bugs::AlbumItemData data_output;
        data_output.album_id = ProcJsonEasy::getInt(jsonObj, "album_id");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        //data_output.descr = ProcJsonEasy::getString(jsonObj, "descr");//cheon210604
        data_output.upd_dt = ProcJsonEasy::getString(jsonObj, "upd_dt");//cheon210604
        //data_output.track_count = ProcJsonEasy::getInt(jsonObj, "track_count");//cheon210604
        data_output.album_image = ProcJsonEasy::getString(jsonObj, "album_image");
        data_output.likes_yn = ProcJsonEasy::getBool(jsonObj, "likes_yn");
        data_output.valid_yn = ProcJsonEasy::getBool(jsonObj, "valid_yn");

        QVariantList tmp_list_id = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_id").toVariantList();
        foreach(QVariant tmp_val, tmp_list_id){
            data_output.list_artist_id.append(tmp_val.value<int>());
        }

        QVariantList tmp_list_name = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_nm").toVariantList();
        foreach(QVariant tmp_val, tmp_list_name){
            data_output.list_artist_nm.append(tmp_val.value<QString>());
        }

        return data_output;
    }





    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about PD_AlbumItemData
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief PD_AlbumItemData struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData_forBugs::getObjectJson_pd_albumData(const bugs::PD_AlbumItemData &data){

        QJsonObject jsonObj;
        jsonObj.insert("es_album_id", data.es_album_id);
        jsonObj.insert("title", data.title);
        jsonObj.insert("descr", data.descr);//cheon210604
        jsonObj.insert("upd_dt", data.upd_dt);//cheon210604
        jsonObj.insert("track_count", data.track_count);//cheon210604
        jsonObj.insert("image", data.image);
        jsonObj.insert("likes_yn", data.likes_yn);
        jsonObj.insert("pd_nickname", data.pd_nickname);

        QJsonArray jsonArr_id;
        foreach(int tmp_id, data.list_tag_id){
            jsonArr_id.append(tmp_id);
        }
        jsonObj.insert("list_tag_id", jsonArr_id);

        QJsonArray jsonArr_name;
        foreach(QString tmp_name, data.list_tag_nm){
            jsonArr_name.append(tmp_name);
        }
        jsonObj.insert("list_tag_nm", jsonArr_name);
        //QJsonDocument doc(jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "888888bugs::AlbumItemData AlbumItemData ConvertData::convertData_albumData()-jsonObj =: " << strJson;//cheonprint//cheontidalhires

        return jsonObj;
    }

    /**
     * @brief JsonObject을 PD_AlbumItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    bugs::PD_AlbumItemData ConvertData_forBugs::convertData_pd_albumData(const QJsonObject &jsonObj){
        //QJsonDocument doc(jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "666666666666bugs::AlbumItemData AlbumItemData ConvertData::convertData_albumData()-jsonObj =: " << strJson;//cheonprint//cheontidalhires

        bugs::PD_AlbumItemData data_output;
        data_output.es_album_id = ProcJsonEasy::getInt(jsonObj, "es_album_id");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.descr = ProcJsonEasy::getString(jsonObj, "descr");//cheon210604
        data_output.upd_dt = ProcJsonEasy::getString(jsonObj, "upd_dt");//cheon210604
        data_output.track_count = ProcJsonEasy::getInt(jsonObj, "track_count");//cheon210604

        data_output.image = ProcJsonEasy::getString(jsonObj, "image");
        data_output.likes_yn = ProcJsonEasy::getBool(jsonObj, "likes_yn");
        data_output.pd_nickname = ProcJsonEasy::getString(jsonObj, "pd_nickname");

        QVariantList tmp_list_id = ProcJsonEasy::getJsonArray(jsonObj, "list_tag_id").toVariantList();
        foreach(QVariant tmp_val, tmp_list_id){
            data_output.list_tag_id.append(tmp_val.value<int>());
        }

        QVariantList tmp_list_name = ProcJsonEasy::getJsonArray(jsonObj, "list_tag_nm").toVariantList();
        foreach(QVariant tmp_val, tmp_list_name){
            data_output.list_tag_nm.append(tmp_val.value<QString>());
        }

        return data_output;
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about MyAlbumItemData
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief MyAlbumItemData struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData_forBugs::getObjectJson_my_albumData(const bugs::MyAlbumItemData &data){
        QJsonObject jsonObj;
        jsonObj.insert("playlist_id", data.playlist_id);
        jsonObj.insert("title", data.title);
        jsonObj.insert("image", data.image);
        jsonObj.insert("track_count", data.track_count);

        return jsonObj;
    }

    /**
     * @brief JsonObject을 MyAlbumItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    bugs::MyAlbumItemData ConvertData_forBugs::convertData_my_albumData(const QJsonObject &jsonObj){
        bugs::MyAlbumItemData data_output;
        data_output.playlist_id = ProcJsonEasy::getInt(jsonObj, "playlist_id");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.image = ProcJsonEasy::getString(jsonObj, "image");
        data_output.track_count = ProcJsonEasy::getInt(jsonObj, "track_count");

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
    QJsonObject ConvertData_forBugs::getObjectJson_artistData(const bugs::ArtistItemData &data){
        QJsonObject jsonObj;
        jsonObj.insert("artist_id", data.artist_id);
        jsonObj.insert("artist_nm", data.artist_nm);
        jsonObj.insert("image", data.image);
        jsonObj.insert("likes_yn", data.likes_yn);
        //jsonObj.insert("profile_url", data.profile_url);//cheon210606

        jsonObj.insert("gender_cd_nm", data.gender_cd_nm);
        jsonObj.insert("group_cd_nm", data.group_cd_nm);

        QJsonArray jsonArr_genre_nm;
        foreach(QString tmp_genre_nm, data.list_genre_nm){
            jsonArr_genre_nm.append(tmp_genre_nm);
        }
        jsonObj.insert("list_genre_nm", jsonArr_genre_nm);

        return jsonObj;
    }

    /**
     * @brief JsonObject을 ArtistItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    bugs::ArtistItemData ConvertData_forBugs::convertData_artistData(const QJsonObject &jsonObj){
        bugs::ArtistItemData data_output;
        data_output.artist_id = ProcJsonEasy::getInt(jsonObj, "artist_id");
        data_output.artist_nm = ProcJsonEasy::getString(jsonObj, "artist_nm");
        data_output.image = ProcJsonEasy::getString(jsonObj, "image");
        data_output.likes_yn = ProcJsonEasy::getBool(jsonObj, "likes_yn");
        //data_output.profile_url = ProcJsonEasy::getString(jsonObj, "profile_url");//cheon210606

        data_output.group_cd_nm = ProcJsonEasy::getString(jsonObj, "group_cd_nm");
        data_output.gender_cd_nm = ProcJsonEasy::getString(jsonObj, "gender_cd_nm");

        QVariantList tmp_list_genre_nm = ProcJsonEasy::getJsonArray(jsonObj, "list_genre_nm").toVariantList();
        foreach(QVariant tmp_val, tmp_list_genre_nm){
            data_output.list_genre_nm.append(tmp_val.value<QString>());
        }

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
    QJsonObject ConvertData_forBugs::getObjectJson_videoData(const bugs::VideoItemData &data){
        QJsonObject jsonObj;
        jsonObj.insert("mv_id", data.mv_id);
        jsonObj.insert("mv_title", data.mv_title);
        jsonObj.insert("len", data.len);
        jsonObj.insert("image", data.image);
        jsonObj.insert("likes_yn", data.likes_yn);
        jsonObj.insert("adult_yn", data.adult_yn);
        jsonObj.insert("service_yn", data.service_yn);

        jsonObj.insert("album_id", data.album_id);
        jsonObj.insert("track_id", data.track_id);

        QJsonArray jsonArr_id;
        foreach(int tmp_id, data.list_artist_id){
            jsonArr_id.append(tmp_id);
        }
        jsonObj.insert("list_artist_id", jsonArr_id);

        QJsonArray jsonArr_name;
        foreach(QString tmp_name, data.list_artist_nm){
            jsonArr_name.append(tmp_name);
        }
        jsonObj.insert("list_artist_nm", jsonArr_name);

        return jsonObj;
    }

    /**
     * @brief JsonObject을 VideoItemData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    bugs::VideoItemData ConvertData_forBugs::convertData_videoData(const QJsonObject &jsonObj){
        bugs::VideoItemData data_output;
        data_output.mv_id = ProcJsonEasy::getInt(jsonObj, "mv_id");
        data_output.mv_title = ProcJsonEasy::getString(jsonObj, "mv_title");
        data_output.image = ProcJsonEasy::getString(jsonObj, "image");
        data_output.len = ProcJsonEasy::getString(jsonObj, "len");
        data_output.likes_yn = ProcJsonEasy::getBool(jsonObj, "likes_yn");
        data_output.adult_yn = ProcJsonEasy::getBool(jsonObj, "adult_yn");
        data_output.service_yn = ProcJsonEasy::getBool(jsonObj, "service_yn");

        data_output.track_id = ProcJsonEasy::getInt(jsonObj, "track_id");
        data_output.album_id = ProcJsonEasy::getInt(jsonObj, "album_id");

        QVariantList tmp_list_id = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_id").toVariantList();
        foreach(QVariant tmp_val, tmp_list_id){
            data_output.list_artist_id.append(tmp_val.value<int>());
        }

        QVariantList tmp_list_name = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_nm").toVariantList();
        foreach(QVariant tmp_val, tmp_list_name){
            data_output.list_artist_nm.append(tmp_val.value<QString>());
        }

        return data_output;
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about FilterOptionData
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief FilterOptionData struct 타입을 JsonObject로 반환
     * @param data
     * @return
     */
    QJsonObject ConvertData_forBugs::getObjectJson_filterOptionData(const bugs::FilterOptionData &data){
        QJsonObject jsonObj;
        jsonObj.insert("opt_code", data.opt_code.toString());       // QVariant -> QString
        jsonObj.insert("opt_name", data.opt_name);

        return jsonObj;
    }

    /**
     * @brief JsonObject을 FilterOptionData struct 타입으로 반환
     * @param jsonObj
     * @return
     */
    bugs::FilterOptionData ConvertData_forBugs::convertData_filterOptionData(const QJsonObject &jsonObj){
        bugs::FilterOptionData data_output;
        data_output.opt_code = ProcJsonEasy::getString(jsonObj, "opt_code");
        data_output.opt_name = ProcJsonEasy::getString(jsonObj, "opt_name");

        return data_output;
    }





    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : convert ItemData to OptMorePopup::HeaderData
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief bugs::AlbumItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData_forBugs::getConvertOptHeaderData(const bugs::AlbumItemData &data){
        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.title;
        data_header.sub_title = data.list_artist_nm.join(",");
        data_header.imageUrl = data.album_image;
        data_header.data_pk = QString("%1").arg(data.album_id);
        data_header.type = "BUGS";      //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = true;     //j220906 share link

        return data_header;
    }

    /**
     * @brief bugs::PD_AlbumItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData_forBugs::getConvertOptHeaderData(const bugs::PD_AlbumItemData &data){
        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.title;
        data_header.sub_title = data.pd_nickname;
        data_header.imageUrl = data.image;
        data_header.data_pk = QString("%1").arg(data.es_album_id);
        data_header.type = "BUGS";      //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = true;     //j220906 share link

        return data_header;
    }

    /**
     * @brief bugs::MyAlbumItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData_forBugs::getConvertOptHeaderData(const bugs::MyAlbumItemData &data){
        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.title;
        data_header.sub_title = QString("%1곡").arg(data.track_count);
        data_header.imageUrl = data.image;
        data_header.data_pk = QString("%1").arg(data.playlist_id);
        data_header.type = "BUGS";      //j220906 share link
        data_header.flagProcStar = false;
        //data_header.flagFavOn = data.likes_yn;
        data_header.isShare = true;     //j220906 share link

        return data_header;
    }


    /**
     * @brief bugs::TrackItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData_forBugs::getConvertOptHeaderData(const bugs::TrackItemData &data){
        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.track_title;
        data_header.sub_title = data.list_artist_nm.join(",");
        data_header.imageUrl = data.album_image;
        data_header.data_pk = QString("%1").arg(data.track_id);
        data_header.type = "BUGS";      //j220906 share link
        data_header.flagProcStar = false;
        data_header.flagFavOn = data.likes_yn;              ///< 좋아요 초기값
        data_header.isShare = true;     //j220906 share link

        return data_header;
    }


    /**
     * @brief bugs::ArtistItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData_forBugs::getConvertOptHeaderData(const bugs::ArtistItemData &data){
        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.artist_nm;
        data_header.sub_title = "";
        data_header.imageUrl = data.image;
        data_header.data_pk = QString("%1").arg(data.artist_id);
        data_header.type = "BUGS";      //j220906 share link
        data_header.flagProcStar = false;
        data_header.flagFavOn = data.likes_yn;              ///< 좋아요 초기값
        data_header.isShare = true;     //j220906 share link

        return data_header;
    }


    /**
     * @brief bugs::VideoItemData 정보를 OptMorePopup::HeaderData 로 변환하여 반환
     * @param data
     * @return
     */
    OptMorePopup::HeaderData ConvertData_forBugs::getConvertOptHeaderData(const bugs::VideoItemData &data){
        OptMorePopup::HeaderData data_header;
        data_header.main_title = data.mv_title;
        data_header.sub_title = data.list_artist_nm.join(",");
        data_header.imageUrl = data.image;
        data_header.data_pk = QString("%1").arg(data.mv_id);
        data_header.type = "BUGS";      //j220906 share link
        data_header.flagProcStar = false;
        data_header.flagFavOn = data.likes_yn;              ///< 좋아요 초기값
        data_header.isShare = true;      //j220906 share link

        return data_header;
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : 데이터 변환 관련 (Bugs API 결과값 Json <------> struct)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief ConvertData_forBugs::convert_listString_to_listInt
     * @param p_list_str
     * @return
     */
    QList<int> ConvertData_forBugs::convert_listString_to_listInt(QList<QString> p_list_str){
        QList<int> list_int;
        foreach(QString tmp_str, p_list_str){
            list_int.append(tmp_str.toInt());
        }
        return list_int;
    }


    /**
     * @brief Bugs Track 정보 반환하는 JsonObject로부터 TrackItemData 로 변환해서 반환한다.
     * @param jsonObj
     * @return
     */
    bugs::TrackItemData ConvertData_forBugs::make_trackData_fromBugsJsonObj(const QJsonObject &jsonObj){

        // 정보 담을 struct
        /*bugs::TrackItemData data_output;
        data_output.track_id = ProcJsonEasy::getInt(jsonObj, "track_id");
        if(data_output.track_id == 0){
            data_output.track_id = ProcJsonEasy::getString(jsonObj, "track_id").toInt();       // Rose 가 줄때는.. String임..
        }
        data_output.track_title = ProcJsonEasy::getString(jsonObj, "track_title");

        QString tmp_len = ProcJsonEasy::getString(jsonObj, "len");
        tmp_len.remove(QRegExp("^0"));
        data_output.len = tmp_len;

        data_output.adult_yn = ProcJsonEasy::getBool(jsonObj, "adult_yn");


        // 앨범 Obj
        QJsonObject jsonObj_album = ProcJsonEasy::getJsonObject(jsonObj, "album");
        data_output.album_title = ProcJsonEasy::getString(jsonObj_album, "title");
        data_output.album_id = ProcJsonEasy::getInt(jsonObj_album, "album_id");
        if(data_output.album_id == 0){
            data_output.album_id = ProcJsonEasy::getString(jsonObj_album, "album_id").toInt();       // Rose 가 줄때는.. String임..
        }*/

        // 정보 담을 struct
       bugs::TrackItemData data_output;
       data_output.track_id = ProcJsonEasy::getInt(jsonObj, "track_id");
       if(data_output.track_id == 0){
           data_output.track_id = ProcJsonEasy::getString(jsonObj, "track_id").toInt();       // Rose 가 줄때는.. String임..
       }
       data_output.track_title = ProcJsonEasy::getString(jsonObj, "track_title");

       QString tmp_len = ProcJsonEasy::getString(jsonObj, "len");
       tmp_len.remove(QRegExp("^0"));
       data_output.len = tmp_len;

       data_output.adult_yn = ProcJsonEasy::getBool(jsonObj, "adult_yn");



       QJsonArray bitrates1 = ProcJsonEasy::getJsonArray(jsonObj, "bitrates");//cheon210605

       /*if(bitrates1.contains("flac") == true){
           data_output.bitrates = "FLAC";
       }
       else if(bitrates1.contains("flac") == false && bitrates1.contains("320k") == true){
           data_output.bitrates = "MP3";
       }*/

       if(bitrates1.contains("flac") == true){
           data_output.bitrates = "FLAC";
       }
       else if(bitrates1.contains("aac320") == true){
           data_output.bitrates = "AAC320";
       }
       else if(bitrates1.contains("320k") == true){
           data_output.bitrates = "320K";
       }
       else if(bitrates1.contains("aac") == true){
           data_output.bitrates = "AAC";
       }
       else if(bitrates1.contains("mp3") == true){
           data_output.bitrates = "MP3";
       }
       else if(bitrates1.contains("fullhd") == true){
           data_output.bitrates = "FULL HD";
       }
       else if(bitrates1.contains("hd") == true){
           data_output.bitrates = "HD";
       }
       else if(bitrates1.contains("sd") == true){
           data_output.bitrates = "SD";
       }

       /*QList<QString> list_output;//cheon210605
       //qDebug() << "\n\n\nbitrates1.count()" << bitrates1.count();
       //qDebug() << "\n\n\n";
       for(int i=0 ; i<bitrates1.count() ; i++){//cheon210605
           QString tmp_jsonObj = bitrates1.at(i).toString();//cheon210605
           list_output.append(tmp_jsonObj);//cheon210605
       }//cheon210605

       foreach(QString n, list_output){
           qDebug() << n;
           data_output.bitrates = n;//cheon210605
       }*/

       //qDebug() << "\n\n\ndata_output.bitrates" << data_output.bitrates;
       //qDebug() << "\n\n\n";

       // 앨범 Obj
       QJsonObject jsonObj_album = ProcJsonEasy::getJsonObject(jsonObj, "album");
       data_output.album_title = ProcJsonEasy::getString(jsonObj_album, "title");   //+ QString(" (") + data_output.bitrates + QString(")");//cheon210605
       data_output.album_id = ProcJsonEasy::getInt(jsonObj_album, "album_id");
       if(data_output.album_id == 0){
           data_output.album_id = ProcJsonEasy::getString(jsonObj_album, "album_id").toInt();       // Rose 가 줄때는.. String임..
       }

        // 앨범 이미지
        QJsonObject jsonObj_albumImg = ProcJsonEasy::getJsonObject(jsonObj_album, "image");
        data_output.album_image = ConvertData_forBugs::convertImagePath(ProcJsonEasy::getString(jsonObj_albumImg, "url")
                                                                        , ProcJsonEasy::getString(jsonObj_albumImg, "path")
                                                                        , ImageSize_Square::Size_350x350);

        // rights obj
        QJsonObject jsonObj_rights = ProcJsonEasy::getJsonObject(jsonObj, "rights");
        QJsonObject jsonObj_streaming = ProcJsonEasy::getJsonObject(jsonObj_rights, "streaming");
        data_output.service_yn = ProcJsonEasy::getBool(jsonObj_streaming, "service_yn");

        // Adhoc Obj
        QJsonObject jsonObj_adhoc_attr = ProcJsonEasy::getJsonObject(jsonObj, "adhoc_attr");
        data_output.likes_yn = ProcJsonEasy::getBool(jsonObj_adhoc_attr, "likes_yn");

        // for artist
        QJsonArray tmp_jsonArr_artist = ProcJsonEasy::getJsonArray(jsonObj, "artists");
        data_output.list_artist_id = ProcJsonEasy::getListInt_fromJsonArr(tmp_jsonArr_artist, "artist_id");
        data_output.list_artist_nm = ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_artist, "artist_nm");

        // artist_id 관련 추가 체크 :: Rose 가 줄때는.. String임..
        if(data_output.list_artist_id.count() > 0 && data_output.list_artist_id.first() == 0){
            data_output.list_artist_id = ConvertData_forBugs::convert_listString_to_listInt(ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_artist, "artist_id"));
        }

        // 관련 동영상
        QJsonArray jsonArr_mvs = ProcJsonEasy::getJsonArray(jsonObj, "mvs");
        data_output.list_mv_id = ProcJsonEasy::getListInt_fromJsonArr(jsonArr_mvs, "mv_id");

        return data_output;
    }


    bugs::TrackItemData ConvertData_forBugs::make_trackData_fromBugsJsonObj_75(const QJsonObject &jsonObj){

        //QJsonDocument doc(jsonObj);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "qobuz::TrackItemData ConvertData::make_trackData_fromTidalJsonObj---p_jsonObj : " << strJson << "\n";

        // 정보 담을 struct
        /*bugs::TrackItemData data_output;
        data_output.track_id = ProcJsonEasy::getInt(jsonObj, "track_id");
        if(data_output.track_id == 0){
            data_output.track_id = ProcJsonEasy::getString(jsonObj, "track_id").toInt();       // Rose 가 줄때는.. String임..
        }
        data_output.track_title = ProcJsonEasy::getString(jsonObj, "track_title");

        QString tmp_len = ProcJsonEasy::getString(jsonObj, "len");
        tmp_len.remove(QRegExp("^0"));
        data_output.len = tmp_len;

        data_output.adult_yn = ProcJsonEasy::getBool(jsonObj, "adult_yn");


        // 앨범 Obj
        QJsonObject jsonObj_album = ProcJsonEasy::getJsonObject(jsonObj, "album");
        data_output.album_title = ProcJsonEasy::getString(jsonObj_album, "title");
        data_output.album_id = ProcJsonEasy::getInt(jsonObj_album, "album_id");
        if(data_output.album_id == 0){
            data_output.album_id = ProcJsonEasy::getString(jsonObj_album, "album_id").toInt();       // Rose 가 줄때는.. String임..
        }*/

        // 정보 담을 struct
       bugs::TrackItemData data_output;
       data_output.track_id = ProcJsonEasy::getInt(jsonObj, "track_id");
       if(data_output.track_id == 0){
           data_output.track_id = ProcJsonEasy::getString(jsonObj, "track_id").toInt();       // Rose 가 줄때는.. String임..
       }
       data_output.track_title = ProcJsonEasy::getString(jsonObj, "track_title");

       QString tmp_len = ProcJsonEasy::getString(jsonObj, "len");
       tmp_len.remove(QRegExp("^0"));
       data_output.len = tmp_len;

       data_output.adult_yn = ProcJsonEasy::getBool(jsonObj, "adult_yn");



       QJsonArray bitrates1 = ProcJsonEasy::getJsonArray(jsonObj, "bitrates");//cheon210605

       /*if(bitrates1.contains("flac") == true){
           data_output.bitrates = "FLAC";
       }
       else if(bitrates1.contains("flac") == false && bitrates1.contains("320k") == true){
           data_output.bitrates = "MP3";
       }*/

       if(bitrates1.contains("flac") == true){
           data_output.bitrates = "FLAC";
       }
       else if(bitrates1.contains("aac320") == true){
           data_output.bitrates = "AAC320";
       }
       else if(bitrates1.contains("320k") == true){
           data_output.bitrates = "320K";
       }
       else if(bitrates1.contains("aac") == true){
           data_output.bitrates = "AAC";
       }
       else if(bitrates1.contains("mp3") == true){
           data_output.bitrates = "MP3";
       }
       else if(bitrates1.contains("fullhd") == true){
           data_output.bitrates = "FULL HD";
       }
       else if(bitrates1.contains("hd") == true){
           data_output.bitrates = "HD";
       }
       else if(bitrates1.contains("sd") == true){
           data_output.bitrates = "SD";
       }

       /*QList<QString> list_output;//cheon210605
       //qDebug() << "\n\n\nbitrates1.count()" << bitrates1.count();
       //qDebug() << "\n\n\n";
       for(int i=0 ; i<bitrates1.count() ; i++){//cheon210605
           QString tmp_jsonObj = bitrates1.at(i).toString();//cheon210605
           list_output.append(tmp_jsonObj);//cheon210605
       }//cheon210605

       foreach(QString n, list_output){
           qDebug() << n;
           data_output.bitrates = n;//cheon210605
       }*/

       //qDebug() << "\n\n\ndata_output.bitrates" << data_output.bitrates;
       //qDebug() << "\n\n\n";

       // 앨범 Obj
       QJsonObject jsonObj_album = ProcJsonEasy::getJsonObject(jsonObj, "album");
       data_output.album_title = ProcJsonEasy::getString(jsonObj_album, "title");   //+ QString(" (") + data_output.bitrates + QString(")");//cheon210605
       data_output.album_id = ProcJsonEasy::getInt(jsonObj_album, "album_id");
       if(data_output.album_id == 0){
           data_output.album_id = ProcJsonEasy::getString(jsonObj_album, "album_id").toInt();       // Rose 가 줄때는.. String임..
       }

        // 앨범 이미지
        QJsonObject jsonObj_albumImg = ProcJsonEasy::getJsonObject(jsonObj_album, "image");
        data_output.album_image = ConvertData_forBugs::convertImagePath(ProcJsonEasy::getString(jsonObj_albumImg, "url")
                                                                        , ProcJsonEasy::getString(jsonObj_albumImg, "path")
                                                                        , ImageSize_Square::Size_75x75);

        // rights obj
        QJsonObject jsonObj_rights = ProcJsonEasy::getJsonObject(jsonObj, "rights");
        QJsonObject jsonObj_streaming = ProcJsonEasy::getJsonObject(jsonObj_rights, "streaming");
        data_output.service_yn = ProcJsonEasy::getBool(jsonObj_streaming, "service_yn");

        // Adhoc Obj
        QJsonObject jsonObj_adhoc_attr = ProcJsonEasy::getJsonObject(jsonObj, "adhoc_attr");
        data_output.likes_yn = ProcJsonEasy::getBool(jsonObj_adhoc_attr, "likes_yn");

        // for artist
        QJsonArray tmp_jsonArr_artist = ProcJsonEasy::getJsonArray(jsonObj, "artists");
        data_output.list_artist_id = ProcJsonEasy::getListInt_fromJsonArr(tmp_jsonArr_artist, "artist_id");
        data_output.list_artist_nm = ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_artist, "artist_nm");

        // artist_id 관련 추가 체크 :: Rose 가 줄때는.. String임..
        if(data_output.list_artist_id.count() > 0 && data_output.list_artist_id.first() == 0){
            data_output.list_artist_id = ConvertData_forBugs::convert_listString_to_listInt(ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_artist, "artist_id"));
        }

        // 관련 동영상
        QJsonArray jsonArr_mvs = ProcJsonEasy::getJsonArray(jsonObj, "mvs");
        data_output.list_mv_id = ProcJsonEasy::getListInt_fromJsonArr(jsonArr_mvs, "mv_id");

        return data_output;
    }



    /**
     * @brief Bugs Track 정보 반환하는 JsonObject로부터 TrackItemDatxwa 로 변환해서 반환한다.
     * @param jsonObj
     * @return
     */
    bugs::AlbumItemData ConvertData_forBugs::make_albumData_fromBugsJsonObj(const QJsonObject &jsonObj){
        //QJsonDocument doc(jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "99999999999999bugs::AlbumItemData AlbumItemData ConvertData::convertData_albumData()-jsonObj =: " << strJson;//cheonprint//cheontidalhires

        // 정보 담을 struct
        bugs::AlbumItemData data_output;
        data_output.album_id = ProcJsonEasy::getInt(jsonObj, "album_id");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.descr = ProcJsonEasy::getString(jsonObj, "descr");//cheon210604
        data_output.upd_dt = ProcJsonEasy::getString(jsonObj, "upd_dt");//cheon210604
        data_output.track_count = ProcJsonEasy::getInt(jsonObj, "track_count");//cheon210604

        //QJsonObject jsonObj_genres_attr = ProcJsonEasy::getJsonObject(jsonObj, "genres");//cheon210605
        //data_output.genre_nm = ProcJsonEasy::getString(jsonObj_genres_attr, "genre_nm");//cheon210605
        //qDebug() << "\n\n\n\n\ndata.genre_nm" << data_output.genre_nm;
        data_output.valid_yn = ProcJsonEasy::getBool(jsonObj, "valid_yn");

        // 앨범 이미지
        QJsonObject jsonObj_albumImg = ProcJsonEasy::getJsonObject(jsonObj, "image");
        data_output.album_image = ConvertData_forBugs::convertImagePath(ProcJsonEasy::getString(jsonObj_albumImg, "url")
                                                                        , ProcJsonEasy::getString(jsonObj_albumImg, "path")
                                                                        , ImageSize_Square::Size_1000x1000);

        QJsonObject jsonObj_adhoc_attr = ProcJsonEasy::getJsonObject(jsonObj, "adhoc_attr");
        data_output.likes_yn = ProcJsonEasy::getBool(jsonObj_adhoc_attr, "likes_yn");

        // for artist
        QJsonArray tmp_jsonArr_artist = ProcJsonEasy::getJsonArray(jsonObj, "artists");
        data_output.list_artist_id = ProcJsonEasy::getListInt_fromJsonArr(tmp_jsonArr_artist, "artist_id");
        data_output.list_artist_nm = ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_artist, "artist_nm");

        // artist_id 관련 추가 체크 :: Rose 가 줄때는.. String임..
        /*if(data_output.list_artist_id.count() > 0 && data_output.list_artist_id.first() == 0){
            data_output.list_artist_id = ConvertData_forBugs::convert_listString_to_listInt(ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_artist, "artist_id"));
        }*/

        return data_output;
    }


    /**
     * @brief ConvertData_forBugs::make_artistData_fromBugsJsonObj
     * @param jsonObj
     * @return
     */
    bugs::ArtistItemData ConvertData_forBugs::make_artistData_fromBugsJsonObj(const QJsonObject &jsonObj){

        // 정보 담을 struct
        bugs::ArtistItemData data_output;
        data_output.artist_id = ProcJsonEasy::getInt(jsonObj, "artist_id");
        data_output.artist_nm = ProcJsonEasy::getString(jsonObj, "artist_nm");

        // Artist 이미지
        QJsonObject jsonObj_img = ProcJsonEasy::getJsonObject(jsonObj, "image");
        data_output.image = ConvertData_forBugs::convertImagePath(ProcJsonEasy::getString(jsonObj_img, "url")
                                                                        , ProcJsonEasy::getString(jsonObj_img, "path")
                                                                        , ImageSize_Square::Size_1000x1000);

        QJsonObject jsonObj_adhoc_attr = ProcJsonEasy::getJsonObject(jsonObj, "adhoc_attr");
        data_output.likes_yn = ProcJsonEasy::getBool(jsonObj_adhoc_attr, "likes_yn");

        // type
        QJsonObject jsonObj_type = ProcJsonEasy::getJsonObject(jsonObj, "type");
        data_output.group_cd_nm = ProcJsonEasy::getString(jsonObj_type, "group_cd_nm");
        data_output.gender_cd_nm = ProcJsonEasy::getString(jsonObj_type, "gender_cd_nm");

        QJsonArray tmp_jsonArr_genres = ProcJsonEasy::getJsonArray(jsonObj, "genres");
        data_output.list_genre_nm = ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_genres, "genre_nm");

        return data_output;

    }


    /**
     * @brief ConvertData_forBugs::make_videoData_fromBugsJsonObj
     * @param jsonObj
     * @return
     */
    bugs::VideoItemData ConvertData_forBugs::make_videoData_fromBugsJsonObj(const QJsonObject &jsonObj){

        // 정보 담을 struct
        bugs::VideoItemData data_output;
        data_output.mv_id = ProcJsonEasy::getInt(jsonObj, "mv_id");
        if(data_output.mv_id == 0){
            data_output.album_id = ProcJsonEasy::getString(jsonObj, "mv_id").toInt();       // Rose 가 줄때는.. String임..
        }

        data_output.mv_title = ProcJsonEasy::getString(jsonObj, "mv_title");

        QVariantList tmp_bitrates = ProcJsonEasy::getJsonArray(jsonObj, "bitrates").toVariantList();//c220528
        foreach(QVariant tmp_val, tmp_bitrates){//c220528
            data_output.bitrates.append(tmp_val.value<QString>());//c220528
        }

        QString tmp_len = ProcJsonEasy::getString(jsonObj, "len");
        tmp_len.remove(QRegExp("^0"));
        if(tmp_len.length()>0 && tmp_len.at(0)==':'){
            tmp_len = "0" + tmp_len;
        }
        data_output.len = tmp_len;

        data_output.adult_yn = ProcJsonEasy::getBool(jsonObj, "adult_yn");

        // 이미지
        QJsonObject jsonObj_img = ProcJsonEasy::getJsonObject(jsonObj, "image");
        data_output.image = ConvertData_forBugs::convertImagePath(ProcJsonEasy::getString(jsonObj_img, "url")
                                                                        , ProcJsonEasy::getString(jsonObj_img, "path")
                                                                        , ImageSize_Ractangle::Size_350w);

        QJsonObject jsonObj_adhoc_attr = ProcJsonEasy::getJsonObject(jsonObj, "adhoc_attr");
        data_output.likes_yn = ProcJsonEasy::getBool(jsonObj_adhoc_attr, "likes_yn");

        // rights obj
        QJsonObject jsonObj_rights = ProcJsonEasy::getJsonObject(jsonObj, "rights");
        QJsonObject jsonObj_streaming = ProcJsonEasy::getJsonObject(jsonObj_rights, "streaming");
        data_output.service_yn = ProcJsonEasy::getBool(jsonObj_streaming, "service_yn");

        // linking data - track, album
        QJsonObject jsonObj_track = ProcJsonEasy::getJsonObject(jsonObj, "track");
        data_output.track_id = ProcJsonEasy::getInt(jsonObj_track, "track_id");
        if(data_output.track_id == 0){
            data_output.album_id = ProcJsonEasy::getString(jsonObj_track, "track_id").toInt();       // Rose 가 줄때는.. String임..
        }

        QJsonObject jsonObj_album = ProcJsonEasy::getJsonObject(jsonObj_track, "album");
        data_output.album_id = ProcJsonEasy::getInt(jsonObj_album, "album_id");
        if(data_output.album_id == 0){
            data_output.album_id = ProcJsonEasy::getString(jsonObj_album, "album_id").toInt();       // Rose 가 줄때는.. String임..
        }


        // for artist
        QJsonArray tmp_jsonArr_artist = ProcJsonEasy::getJsonArray(jsonObj, "artists");
        data_output.list_artist_id = ProcJsonEasy::getListInt_fromJsonArr(tmp_jsonArr_artist, "artist_id");
        data_output.list_artist_nm = ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_artist, "artist_nm");

        // artist_id 관련 추가 체크 :: Rose 가 줄때는.. String임..
        if(data_output.list_artist_id.count()>0 && data_output.list_artist_id.first()==0){
            data_output.list_artist_id = ConvertData_forBugs::convert_listString_to_listInt(ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_artist, "artist_id"));
        }

        return data_output;
    }



    /**
     * @brief ConvertData_forBugs::make_pdAlbumData_fromBugsJsonObj
     * @param jsonObj
     * @return
     */
    bugs::PD_AlbumItemData ConvertData_forBugs::make_pd_albumData_fromBugsJsonObj(const QJsonObject &jsonObj){
        //QJsonDocument doc(jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "333333333333333bugs::AlbumItemData AlbumItemData ConvertData::convertData_albumData()-jsonObj =: " << strJson;//cheonprint//cheontidalhires

        // 정보 담을 struct
        bugs::PD_AlbumItemData data_output;
        data_output.es_album_id = ProcJsonEasy::getInt(jsonObj, "es_album_id");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.descr = ProcJsonEasy::getString(jsonObj, "descr");//cheon210604
        data_output.track_count = ProcJsonEasy::getInt(jsonObj, "track_count");//cheon210604
        data_output.upd_dt = ProcJsonEasy::getString(jsonObj, "upd_dt");//cheon210604

        // 이미지
        QJsonObject jsonObj_img = ProcJsonEasy::getJsonObject(jsonObj, "image");
        data_output.image = ConvertData_forBugs::convertImagePath(ProcJsonEasy::getString(jsonObj_img, "url")
                                                                        , ProcJsonEasy::getString(jsonObj_img, "path")
                                                                        , ImageSize_Square_PD_Album::Size_250x250);

        // PD info
        QJsonObject jsonObj_musicpd = ProcJsonEasy::getJsonObject(jsonObj, "musicpd");
        data_output.pd_nickname = ProcJsonEasy::getString(jsonObj_musicpd, "nickname");

        // 좋아요
        QJsonObject jsonObj_adhoc_attr = ProcJsonEasy::getJsonObject(jsonObj, "adhoc_attr");
        data_output.likes_yn = ProcJsonEasy::getBool(jsonObj_adhoc_attr, "likes_yn");

        // for tags
        QJsonArray tmp_jsonArr_tags = ProcJsonEasy::getJsonArray(jsonObj, "tags");
        data_output.list_tag_id = ProcJsonEasy::getListInt_fromJsonArr(tmp_jsonArr_tags, "tag_id");
        data_output.list_tag_nm = ProcJsonEasy::getListString_fromJsonArr(tmp_jsonArr_tags, "tag_nm");

        return data_output;
    }



    /**
     * @brief ConvertData_forBugs::make_my_albumData_fromBugsJsonObj
     * @param jsonObj
     * @return
     */
    bugs::MyAlbumItemData ConvertData_forBugs::make_my_albumData_fromBugsJsonObj(const QJsonObject &jsonObj){

        // 정보 담을 struct
        bugs::MyAlbumItemData data_output;
        data_output.playlist_id = ProcJsonEasy::getInt(jsonObj, "playlist_id");
        data_output.title = ProcJsonEasy::getString(jsonObj, "title");
        data_output.track_count = ProcJsonEasy::getInt(jsonObj, "track_count");

        // 이미지
        QJsonObject jsonObj_img = ProcJsonEasy::getJsonObject(jsonObj, "image");
        data_output.image = ConvertData_forBugs::convertImagePath(ProcJsonEasy::getString(jsonObj_img, "url")
                                                                        , ProcJsonEasy::getString(jsonObj_img, "path")
                                                                        , ImageSize_Square_My_Album::Size_350x350);


        return data_output;
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : 페이지 이동을 위한 데이터 변환
    //
    //-----------------------------------------------------------------------------------------------------------------------

    // PageInfo_TrackAllView  --------------------------------------------------------------------------------------------

    /**
     * @brief ConvertData_forBugs::getObjectJson_pageInfo_trackAllView_ofArtist
     * @param data
     * @return
     */
    QJsonObject ConvertData_forBugs::getObjectJson_pageInfo_trackAllView(const bugs::PageInfo_TrackAllView &data){
        QJsonObject jsonObj;
        jsonObj.insert("pathTitle", data.pathTitle);
        jsonObj.insert("api_subPath", data.api_subPath);
        return jsonObj;
    }

    /**
     * @brief ConvertData_forBugs::convertData_pageInfo_trackAllView_ofArtist
     * @param jsonObj
     * @return
     */
    bugs::PageInfo_TrackAllView ConvertData_forBugs::convertData_pageInfo_trackAllView(const QJsonObject &jsonObj){
        bugs::PageInfo_TrackAllView data_output;
        data_output.pathTitle = ProcJsonEasy::getString(jsonObj, "pathTitle");
        data_output.api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");
        return data_output;
    }




    // PageInfo_AlbumAllView_ofArtist  --------------------------------------------------------------------------------------------

    /**
     * @brief ConvertData_forBugs::getObjectJson_pageInfo_albumAllView_ofArtist
     * @param data
     * @return
     */
    QJsonObject ConvertData_forBugs::getObjectJson_pageInfo_albumAllView_ofArtist(const bugs::PageInfo_AlbumAllView_ofArtist &data){
        QJsonObject jsonObj;
        jsonObj.insert("artist_id", data.artist_id);
        jsonObj.insert("filterOpt", data.filterOpt);
        jsonObj.insert("mainTitle", data.mainTitle);
        return jsonObj;
    }


    /**
     * @brief ConvertData_forBugs::convertData_pageInfo_albumAllView_ofArtist
     * @param jsonObj
     * @return
     */
    bugs::PageInfo_AlbumAllView_ofArtist ConvertData_forBugs::convertData_pageInfo_albumAllView_ofArtist(const QJsonObject &jsonObj){
        bugs::PageInfo_AlbumAllView_ofArtist data_output;
        data_output.artist_id = ProcJsonEasy::getInt(jsonObj, "artist_id");
        data_output.filterOpt = ProcJsonEasy::getString(jsonObj, "filterOpt");
        data_output.mainTitle = ProcJsonEasy::getString(jsonObj, "mainTitle");
        return data_output;
    }



    // PageInfo_TrackAllView_ofArtist  --------------------------------------------------------------------------------------------

    /**
     * @brief ConvertData_forBugs::getObjectJson_pageInfo_trackAllView_ofArtist
     * @param data
     * @return
     */
    QJsonObject ConvertData_forBugs::getObjectJson_pageInfo_trackAllView_ofArtist(const bugs::PageInfo_TrackAllView_ofArtist &data){
        QJsonObject jsonObj;
        jsonObj.insert("artist_id", data.artist_id);
        jsonObj.insert("filterOpt", data.filterOpt);
        jsonObj.insert("sortOpt", data.sortOpt);
        jsonObj.insert("mainTitle", data.mainTitle);
        return jsonObj;
    }

    /**
     * @brief ConvertData_forBugs::convertData_pageInfo_trackAllView_ofArtist
     * @param jsonObj
     * @return
     */
    bugs::PageInfo_TrackAllView_ofArtist ConvertData_forBugs::convertData_pageInfo_trackAllView_ofArtist(const QJsonObject &jsonObj){
        bugs::PageInfo_TrackAllView_ofArtist data_output;
        data_output.artist_id = ProcJsonEasy::getInt(jsonObj, "artist_id");
        data_output.filterOpt = ProcJsonEasy::getString(jsonObj, "filterOpt");
        data_output.sortOpt = ProcJsonEasy::getString(jsonObj, "sortOpt");
        data_output.mainTitle = ProcJsonEasy::getString(jsonObj, "mainTitle");
        return data_output;
    }



    // PageInfo_PDAlbumAllView  --------------------------------------------------------------------------------------------

    /**
     * @brief ConvertData_forBugs::getObjectJson_pageInfo_trackAllView_ofArtist
     * @param data
     * @return
     */
    QJsonObject ConvertData_forBugs::getObjectJson_pageInfo_pd_albumAllView(const bugs::PageInfo_PDAlbumAllView &data){
        QJsonObject jsonObj;
        jsonObj.insert("sortOpt", data.sortOpt);
        jsonObj.insert("tag_ids", data.tag_ids);
        jsonObj.insert("mainTitle", data.mainTitle);
        return jsonObj;
    }

    /**
     * @brief ConvertData_forBugs::convertData_pageInfo_trackAllView_ofArtist
     * @param jsonObj
     * @return
     */
    bugs::PageInfo_PDAlbumAllView ConvertData_forBugs::convertData_pageInfo_pd_albumAllView(const QJsonObject &jsonObj){
        bugs::PageInfo_PDAlbumAllView data_output;
        data_output.sortOpt = ProcJsonEasy::getString(jsonObj, "sortOpt");
        data_output.tag_ids = ProcJsonEasy::getString(jsonObj, "tag_ids");
        data_output.mainTitle = ProcJsonEasy::getString(jsonObj, "mainTitle");
        return data_output;
    }





}
