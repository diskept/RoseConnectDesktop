#include "roseHome/ProcCommon_forRosehome.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/rosehome_struct.h"

#include "common/global.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"
#include "common/sqlitehelper.h"

#include <QDebug>
#include <QException>


namespace roseHome {

    /**
     * @brief 기본 생성자.
     */
    ProcCommon::ProcCommon(QWidget *parent) : QObject(parent) {
    }


    /**
     * @brief 소멸자. 메모리 해제 체크확인용
     */
    ProcCommon::~ProcCommon(){

        this->deleteLater();
    }


    void ProcCommon::request_rose_getList_recommends(const QString path, const int page, const int pageSize){

        Q_UNUSED(page);
        Q_UNUSED(pageSize);

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + QString("/%1").arg(path);
        QJsonObject json;

        network->request(GetList_Recommend
                         , url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_getList_recentlyAlbums(const QString path, const QString mediaType, const int page, const int pageSize){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = "";
        if(mediaType.contains(",")){
            url = global.legacy_v1 + QString("/%1").arg(path) + QString("?type=&excludeMediaType=&mediaTypes=%1").arg(mediaType) + QString("&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        }
        else{
            url = global.legacy_v1 + QString("/%1").arg(path) + QString("?type=%1&excludeMediaType=").arg(mediaType) + QString("&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        }

        QJsonObject json;

        network->request(GetList_Recently_Albums
                         , url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_getList_recentlyPlaylists(const QString path, const QString mediaType, const int page, const int pageSize){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = "";
        if(mediaType.contains(",")){
            url = global.legacy_v1 + QString("/%1").arg(path) + QString("?type=&excludeMediaType=&mediaTypes=%1").arg(mediaType) + QString("&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        }
        else{
            url = global.legacy_v1 + QString("/%1").arg(path) + QString("?type=%1&excludeMediaType=").arg(mediaType) + QString("&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        }

        QJsonObject json;

        network->request(GetList_Recently_Playlist
                         , url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_getList_recentlyTracks(const QString path, const QString mediaType, const int page, const int pageSize){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = "";
        if(mediaType == ""){
            url = global.legacy_v1 + QString("/%1?type=&excludeMediaType=YOUTUBE").arg(path) + QString("&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        }
        else if(mediaType == "YOUTUBE"){
            url = global.legacy_v1 + QString("/%1").arg(path) + QString("?type=&excludeMediaType=%1").arg(mediaType) + QString("&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        }
        else if(mediaType == "ROSETUBE"){
            //https://api.roseaudio.kr/v1/member/track/recent?type=&excludeMediaType=YOUTUBE&mediaTypes=YOUTUBE&page=0&size=20
            url = global.legacy_v1 + QString("/%1").arg(path) + QString("?type=&excludeMediaType=YOUTUBE&mediaTypes=YOUTUBE") + QString("&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        }
        else if(mediaType.contains(",")){
            url = global.legacy_v1 + QString("/%1").arg(path) + QString("?type=&excludeMediaType=&mediaTypes=%1").arg(mediaType) + QString("&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        }
        else{
            url = global.legacy_v1 + QString("/%1").arg(path) + QString("?type=%1&excludeMediaType=").arg(mediaType) + QString("&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        }
        QJsonObject json;

        network->request(GetList_Recently_Track
                         , url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_getList_recentlyRosetube(const QString path, const QString mediaType, const int page, const int pageSize){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + QString("/%1").arg(path) + QString("?type=%1&excludeMediaType=").arg(mediaType) + QString("&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        QJsonObject json;

        network->request(GetList_Recently_Rosetube
                         , url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_getList_myPlaylists(const QString path, const QString mediaType, const int page, const int pageSize){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url ="";
        if(mediaType.contains(",")){
            url = global.legacy_v1 + QString("/%1").arg(path) + QString("/member/%1").arg(global.user.getSub()) + QString("?sortType=PLAYLIST_RECENT&mediaType=&mediaTypes=%1").arg(mediaType) + QString("&page=%1").arg(page) + QString("&size=%1&isRose=true").arg(pageSize);
        }
        else{
            url = global.legacy_v1 + QString("/%1").arg(path) + QString("/member/%1").arg(global.user.getSub()) + QString("?sortType=PLAYLIST_RECENT&mediaType=%1").arg(mediaType) + QString("&page=%1").arg(page) + QString("&size=%1&isRose=true").arg(pageSize);
        }

        QJsonObject json;

        network->request(GetList_My_Playlist
                         , url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_getList_usersPlaylists(const QString path, const QString sortType, const QString mediaType, const int page, const int pageSize){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url ="";
        if(mediaType.contains(",")){
            url = global.legacy_v1 + QString("/%1").arg(path) + QString("?sortType=%1").arg(sortType) + QString("&mediaType=&mediaType=&mediaTypes=%1").arg(mediaType) + QString("&page=%1").arg(page) + QString("&size=%1&isRose=true").arg(pageSize);
        }
        else{
            url = global.legacy_v1 + QString("/%1").arg(path) + QString("?sortType=%1").arg(sortType) + QString("&mediaType=%1").arg(mediaType) + QString("&page=%1").arg(page) + QString("&size=%1&isRose=true").arg(pageSize);
        }
        QJsonObject json;

        network->request(GetList_Users_Playlist
                         , url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_getList_favoriteAlbums(const QString sortType, const QString mediaType, const int page, const int pageSize){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + QString("/member/album/favorite?") + QString("sortType=%1").arg(sortType) + QString("&mediaType=%1").arg(mediaType) + QString("&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        QJsonObject json;

        network->request(GetList_Favorite_Albums
                         , url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_getList_favoritePlaylists(const QString sortType, const QString mediaType, const int page, const int pageSize){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + QString("/member/playlist?") + QString("sortType=%1").arg(sortType) + QString("&mediaType=%1&isFavorite=true").arg(mediaType) + QString("&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        QJsonObject json;

        network->request(GetList_Favorite_Playlist
                         , url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_getList_favoriteTracks(const QString sortType, const QString mediaType, const int page, const int pageSize){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + QString("/member/track/favorite?") + QString("sortType=%1").arg(sortType) + QString("&mediaType=%1&isFavorite=true").arg(mediaType) + QString("&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        QJsonObject json;

        network->request(GetList_Favorite_Track
                         , url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_get_album(const int album_id, const int page, const int pageSize){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + QString("/member/album/%1").arg(album_id) + QString("?&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        QJsonObject json;

        network->request(GetList_Items_Album
                         , url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_get_playlist(const int playlist_id, const int page, const int pageSize){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + QString("/member/playlist/%1").arg(playlist_id) + QString("?&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        QJsonObject json;

        network->request(GetList_Items_Playlist
                         , url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_get_track(const int track_id, const int page, const int pageSize){

        Q_UNUSED(page);
        Q_UNUSED(pageSize);

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + QString("/member/track/%1").arg(track_id); // + QString("?&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        QJsonObject json;

        network->request(GetList_Items_Track
                         , url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_get_rosetube(const QString track_id, const int page, const int pageSize){

        Q_UNUSED(page);
        Q_UNUSED(pageSize);

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.newpipe_v1 + QString("/get?playurl=%1").arg(track_id); // + QString("?&page=%1").arg(page) + QString("&size=%1").arg(pageSize);
        QJsonObject json;

        network->request(GetList_Items_Rosetube
                         , url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_checkRating_Playlist(const QString mediaType, const QString clientKey){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject playlist;
        playlist.insert("clientKey", clientKey);
        playlist.insert("type", mediaType);

        QJsonObject json;
        json.insert("playlist", playlist);

        network->request(Favorite_Check_Rating_Playlist
                         , QString("%1/member/playlist/check").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_checkRating_Album(const QString mediaType, const QString clientKey){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject album;
        album.insert("clientKey", clientKey);
        album.insert("type", mediaType);

        QJsonObject data = QJsonObject();
        if(mediaType == "MUSIC"){
            data.insert("macAddress", global.device.getDeviceID());
            data.insert("trackId", 0);

            album.insert("data", data);
        }

        QJsonObject json;
        json.insert("album", album);

        network->request(Favorite_Check_Rating_Album
                         , QString("%1/member/album/check").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_checkRating_Track(const QString mediaType, const QString clientKey){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject track_data;
        track_data.insert("clientKey", clientKey);
        track_data.insert("type", mediaType);

        QJsonArray tracks;
        tracks.append(track_data);

        QJsonObject json;
        json.insert("tracks", tracks);

        network->request(Favorite_Check_Rating_Track
                         , QString("%1/member/track/check").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_checkRating_Track(const QString mediaType, const QJsonArray &jsonArr){

        Q_UNUSED(mediaType);

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));


        QJsonObject json;
        json.insert("tracks", jsonArr);

        network->request(Favorite_Check_Rating_Track
                         , QString("%1/member/track/check").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_addRating_Playlist(const QJsonObject jsonObj){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json = jsonObj;

        network->request(Favorite_Creat_Rating_Playlist
                         , QString("%1/member/playlist").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_addRating_Album(const QJsonObject jsonObj){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json = jsonObj;

        network->request(Favorite_Creat_Rating_Album
                         , QString("%1/member/album").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_addRating_Track(const QJsonObject jsonObj){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json = jsonObj;

        network->request(Favorite_Creat_Rating_Track
                         , QString("%1/member/track").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_getRating_Playlist(const QString mediaType, const QString clientKey){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject content;
        content.insert("clientKey", clientKey);
        content.insert("star", 0);

        QJsonArray contents;
        contents.append(content);

        QJsonObject json;
        json.insert("contents", contents);
        json.insert("mediaType", mediaType);
        json.insert("last", false);
        json.insert("offset", 0);
        json.insert("page", 0);
        json.insert("size", 0);
        json.insert("totalCount", 0);
        json.insert("result", false);

        network->request(Favorite_Get_Rating_Playlist
                         , QString("%1/member/rating/PLAYLIST").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_getRating_Album(const QString mediaType, const QString clientKey){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //{"contents":[{"clientKey":"217014042","star":0}],"mediaType":"TIDAL","last":false,"offset":0,"page":0,"size":0,"totalCount":0,"result":false}
        QJsonObject content;
        content.insert("clientKey", clientKey);
        if(mediaType == "MUSIC"){
            content.insert("macAddress", global.device.getDeviceID());
        }
        content.insert("star", 0);

        QJsonArray contents;
        contents.append(content);

        QJsonObject json;
        json.insert("contents", contents);
        json.insert("mediaType", mediaType);
        json.insert("last", false);
        json.insert("offset", 0);
        json.insert("page", 0);
        json.insert("size", 0);
        json.insert("totalCount", 0);
        json.insert("result", false);

        //https://api.roseaudio.kr/v1/member/rating/ALBUM
        network->request(Favorite_Get_Rating_Album
                         , QString("%1/member/rating/ALBUM").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_getRating_Track(const QString mediaType, const QList<QString> clientkey){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonArray contents;
        QJsonObject trackData;
        for(int i = 0; i < clientkey.count(); i++)
        {
            trackData.insert("clientKey", clientkey.at(i));
            trackData.insert("star", 0);

            contents.append(trackData);
        }

        QJsonObject json;
        json.insert("contents", contents);
        json.insert("mediaType", mediaType);
        json.insert("last", false);
        json.insert("offset", 0);
        json.insert("page", 0);
        json.insert("size", 0);
        json.insert("totalCount", 0);
        json.insert("result", false);

        network->request(Favorite_Get_Rating_Track
                         , QString("%1/member/rating/TRACK").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_getRating_Track(const QString mediaType, const QJsonObject &jsonObj){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json;

        if(mediaType == "MUSIC"){
            json = jsonObj;
        }

        QString Url = global.legacy_v1 + "/member/rating?type=TRACK&mediaType=" + mediaType;

        network->request(Favorite_Get_Rating_Track
                         , Url
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_getRating_Thumbup(const QString mediaType, const QString clientkey){

        Q_UNUSED(mediaType);
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject tmp_json;
        network->request(Thumbup_get_Rating_Playlist
                         , QString("%1/member/playlist/thumbup?targetId=%2&type=PLAY_LIST").arg(global.legacy_v1).arg(clientkey)
                         , tmp_json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_setRating_Playlist(const roseHome::PlaylistItemData listDataInfo, const bool favorite, const int star){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //{"playlist":{"favorite":false,"favoriteTrack":false,"id":17955,"isRose":true,"ownerId":0,"star":0,"thumbup":false,"thumbupCount":0,"trackCount":0,"type":"TIDAL"},
        //"ratingInfo":{"favorite":true,"star":2,"thumbup":false,"type":"TIDAL"}}
        QJsonObject playlist;
        playlist.insert("favorite", listDataInfo.favorite);
        playlist.insert("favoriteTrack", listDataInfo.favoriteTrack);
        playlist.insert("id", listDataInfo.id);
        playlist.insert("isRose", listDataInfo.isRose);
        playlist.insert("ownerId", listDataInfo.ownerId);
        playlist.insert("thumbup", listDataInfo.thumbup);
        playlist.insert("thumbupCount", listDataInfo.thumbupCount);
        playlist.insert("trackCount", listDataInfo.trackCount);
        playlist.insert("type", listDataInfo.type);

        /*playlist.insert("clientKey", listDataInfo.clientKey);
        playlist.insert("favorite", favorite);
        playlist.insert("favoriteTrack", listDataInfo.favoriteTrack);
        playlist.insert("isRose", listDataInfo.isRose);
        playlist.insert("ownerId", listDataInfo.ownerId);
        playlist.insert("star", star);
        playlist.insert("thumbnail", listDataInfo.thumbnail);
        playlist.insert("thumbup", listDataInfo.thumbup);
        playlist.insert("thumbupCount", listDataInfo.thumbupCount);
        playlist.insert("trackCount", listDataInfo.trackCount);
        playlist.insert("type", listDataInfo.type);*/

        QJsonObject ratingInfo;
        ratingInfo.insert("favorite", favorite);
        ratingInfo.insert("star", star);
        ratingInfo.insert("thumbup", listDataInfo.thumbup);
        ratingInfo.insert("type", listDataInfo.type);

        QJsonObject json;
        json.insert("playlist", playlist);
        json.insert("ratingInfo", ratingInfo);

        network->request(Favorite_Add_Rating_Playlist
                         , QString("%1/member/rating/star?type=PLAYLIST").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_setRating_rosePlaylist(const QJsonObject jsonObj, const bool favorite, const int star){
        Q_UNUSED(favorite);
        Q_UNUSED(star);

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json = jsonObj;

        network->request(Favorite_Add_Rating_Playlist
                         , QString("%1/member/rating/star?type=PLAYLIST").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_setRating_Album(QJsonObject jsonObj, const bool favorite, const int star){

        Q_UNUSED(favorite);
        Q_UNUSED(star);

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json = jsonObj;

        network->request(Favorite_Add_Rating_Album
                         , QString("%1/member/rating/star?type=ALBUM").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_setRating_Track(const QJsonObject jsonObj, const bool favorite, const int star){

        Q_UNUSED(favorite);
        Q_UNUSED(star);

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json = jsonObj;

        network->request(Favorite_Add_Rating_Track
                         , QString("%1/member/rating/star?type=TRACK").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_setRating_Thumbup(const QJsonObject jsonObj, const bool thumbup){

        Q_UNUSED(thumbup);

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json = jsonObj;

        network->request(Thumbup_Add_Rating_Playlist
                         , QString("%1/member/playlist/thumbup").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_create_myPlaylist(const QJsonObject jsonObj){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject data;
        data.insert("macAddress", global.device.getDeviceID());
        data.insert("trackId", 0);

        QJsonArray tags;
        tags = ProcJsonEasy::getJsonArray(jsonObj, "tags");

        QJsonObject playlist;
        playlist.insert("clientKey", ProcJsonEasy::getString(jsonObj, "clientKey"));
        playlist.insert("comment", ProcJsonEasy::getString(jsonObj, "comment"));
        playlist.insert("data", data);
        playlist.insert("favorite", false);
        playlist.insert("favoriteTrack", false);
        playlist.insert("isRose", true);
        playlist.insert("ownerId", 0);
        playlist.insert("share", ProcJsonEasy::getString(jsonObj, "share"));
        playlist.insert("star", 0);
        playlist.insert("tags", tags);
        playlist.insert("thumbup", false);
        playlist.insert("thumbupCount", 0);
        playlist.insert("title", ProcJsonEasy::getString(jsonObj, "title"));
        playlist.insert("trackCount", 0);
        playlist.insert("type", ProcJsonEasy::getString(jsonObj, "type"));

        QJsonObject json;
        json.insert("playlist", playlist);

        network->request(Create_Playlist_Rose
                         , QString("%1/member/playlist").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_addTrack_myPlaylist(const QJsonObject jsonObj){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json;
        json = jsonObj;

        network->request(AddTracks_Playlist_Rose
                         , QString("%1/member/playlist/track").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_getList_radioChannel(const HttpRequestType type, const QString path, const int regionId, const QString sort, const int page, const int size){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = global.legacy_radio + path;

        /*if(type == GetList_FeaturedChannel){
            Url += QString("page=%1").arg(page) + QString("&size=%1").arg(size);
        }
        else if(type == GetList_Item_Local){*/
        if(type == GetList_Item_Local){
            Url += QString("regionId=%1").arg(regionId) + QString("&page=%1").arg(page) + QString("&size=%1").arg(size);
        }
        else if(type == GetList_Radio_Region){
            Url += QString("parentId=%1").arg(regionId) + QString("&page=%1").arg(page) + QString("&size=%1").arg(size);
        }
        else{
            Url += QString("page=%1").arg(page) + QString("&size=%1").arg(size);
        }

        if(!sort.isEmpty()){
            Url += QString("&sortType=%1").arg(sort);
        }

        QJsonObject json;

        network->request(type
                         , Url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_getList_radioSearch(const int regionId, const int genreId, const int languageId, const int page, const int size, const QString sort, const QString title){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = global.legacy_radio + QString("/channel?title=%1").arg(title) + QString("&page=%1").arg(page) + QString("&size=%1").arg(size);

        if(regionId > 0){
            Url += QString("&regionId=%1").arg(regionId);
        }

        if(genreId > 0){
            Url += QString("&genreId=%1").arg(genreId);
        }

        if(languageId > 0){
            Url += QString("&languageId=%1").arg(languageId);
        }


        if(!sort.isEmpty()){
            Url += QString("&sortType=%1").arg(sort);
        }

        QJsonObject json;

        network->request(GetList_Items_Search
                         , Url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_getList_radioGenre(const int page, const int size){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = global.legacy_radio + "/genre?";
        Url += QString("page=%1").arg(page) + QString("&size=%1").arg(size);

        QJsonObject json;

        network->request(Getlist_Radio_Genre
                         , Url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_getList_genreChannel(const QString path, const int id, const int page, const int size, const QString sort){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = global.legacy_radio + QString("%1").arg(path);
        Url += QString("genreId=%1").arg(id) + QString("&page=%1").arg(page) + QString("&size=%1").arg(size) + QString("&sortType=%1").arg(sort);

        QJsonObject json;

        network->request(GetList_Items_Genre
                         , Url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_getList_radioCountry(const int page, const int size){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = global.legacy_radio + "/region?";
        Url += QString("page=%1").arg(page) + QString("&size=%1").arg(size);

        QJsonObject json;

        network->request(GetList_Radio_Country
                         , Url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_getList_radioLanguage(const int page, const int size){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = global.legacy_radio + "/language?";
        Url += QString("page=%1").arg(page) + QString("&size=%1").arg(size);

        QJsonObject json;

        network->request(GetList_Radio_Language
                         , Url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_getList_languageChannel(const QString path, const int id, const int page, const int size, const QString sort){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = global.legacy_radio + QString("%1").arg(path);
        Url += QString("languageId=%1").arg(id) + QString("&page=%1").arg(page) + QString("&size=%1").arg(size) + QString("&sortType=%1").arg(sort);

        QJsonObject json;

        network->request(GetList_Items_Language
                         , Url
                         , json
                         , false
                         , true);
    }


    void ProcCommon::request_rose_setFavorite_Radio(const QJsonObject channel){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = global.legacy_radio + "/my/favorite";

        QJsonObject json = channel;

        network->request(Favorite_Add_Radio
                         , Url
                         , json
                         , true
                         , true);
    }


    void ProcCommon::request_rose_get_shareLink(const QString thumbnail, QString description, QString title, QString param){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = global.legacy_v1 + "/member/link/connect";

        QJsonObject parameta;
        parameta.insert("param", param);

        QJsonObject json;
        json.insert("thumbnail", thumbnail);
        json.insert("description", description);
        json.insert("title", title);
        json.insert("params", parameta);
        json.insert("uri", "hifirose");

        network->request(Get_Share_link
                         , Url
                         , json
                         , true
                         , true);
    }


    void ProcCommon::slot_responseHttp(int p_id, QJsonObject p_jsonObj){

        //QJsonDocument doc(p_jsonObj);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "p_jsonObj = " << strJson;

        switch (p_id) {

            // General
            case GetList_Recommend:                     setResult_list_recommends(p_jsonObj);                   break;
            case GetList_Recently_Albums:               setResult_list_recentlyAlbums(p_jsonObj);               break;
            case GetList_Recently_Playlist:             setResult_list_recentlyPlaylists(p_jsonObj);            break;
            case GetList_Recently_Track:                setResult_list_recentlyTracks(p_jsonObj);               break;
            case GetList_Recently_Rosetube:             setResult_list_recentlyRoseTube(p_jsonObj);             break;

            case GetList_My_Playlist:                   setResult_list_myPlaylists(p_jsonObj);                  break;
            case GetList_Users_Playlist:                setResult_list_usersPlaylists(p_jsonObj);               break;

            case GetList_Favorite_Albums:               setResult_list_favoriteAlbums(p_jsonObj);               break;
            case GetList_Favorite_Playlist:             setResult_list_favoritePlaylists(p_jsonObj);            break;
            case GetList_Favorite_Track:                setResult_list_favoriteTracks(p_jsonObj);               break;

            case GetList_Items_Album:                   setResult_album(p_jsonObj);                             break;
            case GetList_Items_Playlist:                setResult_playlist(p_jsonObj);                          break;
            case GetList_Items_Track:                   setResult_track(p_jsonObj);                             break;
            case GetList_Items_Rosetube:                setResult_rosetube(p_jsonObj);                          break;

            // check rating info
            case Favorite_Check_Rating_Playlist:        setResult_Favorite_checkRating_Playlist(p_jsonObj);     break;
            case Favorite_Check_Rating_Album:           setResult_Favorite_checkRating_Album(p_jsonObj);        break;
            case Favorite_Check_Rating_Track:           setResult_Favorite_checkRating_Track(p_jsonObj);        break;

            case Favorite_Creat_Rating_Playlist:        setResult_Favorite_checkRating_Playlist(p_jsonObj);     break;
            case Favorite_Creat_Rating_Album:           setResult_Favorite_checkRating_Album(p_jsonObj);        break;
            case Favorite_Creat_Rating_Track:           setResult_Favorite_checkRating_Track(p_jsonObj);        break;

            // get rating info
            case Favorite_Get_Rating_Playlist:          setResult_Favorite_getRating_Playlist(p_jsonObj);       break;
            case Favorite_Get_Rating_Album:             setResult_Favorite_getRating_Album(p_jsonObj);          break;
            case Favorite_Get_Rating_Track:             setResult_Favorite_getRating_Track(p_jsonObj);          break;
            case Thumbup_get_Rating_Playlist:           setResult_Thumbup_getRating_Playlist(p_jsonObj);        break;

            // set rating info
            case Favorite_Add_Rating_Playlist:          setResult_Favorite_setRating_Playlist(p_jsonObj);       break;
            case Favorite_Add_Rating_Album:             setResult_Favorite_setRating_Album(p_jsonObj);          break;
            case Favorite_Add_Rating_Track:             setResult_Favorite_setRating_Track(p_jsonObj);          break;
            case Thumbup_Add_Rating_Playlist:           setResult_Thumbup_setRating_Playlist(p_jsonObj);        break;

            case Create_Playlist_Rose:                  setResult_Create_myPlaylist(p_jsonObj);                 break;
            case AddTracks_Playlist_Rose:               setResult_AddTrack_myPlaylist(p_jsonObj);               break;

            // rose radio
            case GetList_FeaturedChannel:
            case GetList_Recently_Radio:
            case GetList_Favorite_Radio:
            case GetList_Item_LocalPopular:
            case GetList_Item_Local:
            case GetList_Items_Genre:
            case GetList_Items_Language:
            case GetList_Items_Search:                  setResult_list_radioChannel(p_jsonObj);                 break;
            case Getlist_Radio_Genre:                   setResult_list_radioGenre(p_jsonObj);                   break;
            case GetList_Radio_Country:
            case GetList_Radio_Region:                  setResult_list_radioCountry(p_jsonObj);                 break;
            case GetList_Radio_Language:                setResult_list_radioLanguage(p_jsonObj);                break;
            case Favorite_Add_Radio:                    setResult_Favorite_set_Radio(p_jsonObj);                break;

            // rose share link
            case Get_Share_link:                        setResult_share_link((p_jsonObj));                      break;
        }

        sender()->deleteLater();
    }


    // more data에 대한 last page 체크
    bool ProcCommon::get_flag_lastPage(const QJsonObject& p_jsonObj){

        int page = ProcJsonEasy::getInt(p_jsonObj, "page") + 1;
        int totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");

        return (totalCount > (page * 20)) ? false : true;
    }


    void ProcCommon::setResult_list_recommends(const QJsonObject &p_jsonObj){

        QList<roseHome::RecommendsItemData> list_output;
        QJsonArray jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObj, "recommends");
        bool flag_lastPage = true;      // default

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            if(jsonArrSub.size() > 0){

                for(int i = 0; i < jsonArrSub.count(); i++){
                    QJsonObject tmp_json = jsonArrSub.at(i).toObject();
                    QJsonObject playlist_json = ProcJsonEasy::getJsonObject(tmp_json, "playlist");

                    RecommendsItemData tmp_data;

                    tmp_data.id = ProcJsonEasy::getInt(playlist_json, "id");
                    tmp_data.star = ProcJsonEasy::getInt(playlist_json, "star");
                    tmp_data.ownerId =ProcJsonEasy::getInt(playlist_json, "ownerId");
                    tmp_data.thumbupCount = ProcJsonEasy::getInt(playlist_json, "thumbupCount");
                    tmp_data.trackCount = ProcJsonEasy::getInt(playlist_json, "trackCount");
                    tmp_data.sort = ProcJsonEasy::getInt(playlist_json, "sort");

                    tmp_data.type = ProcJsonEasy::getString(playlist_json, "type");
                    tmp_data.title = ProcJsonEasy::getString(playlist_json, "title");
                    tmp_data.comment = ProcJsonEasy::getString(playlist_json, "comment");
                    tmp_data.share = ProcJsonEasy::getString(playlist_json, "share");
                    tmp_data.ownerName = ProcJsonEasy::getString(playlist_json, "ownerName");
                    tmp_data.thumbnail = ProcJsonEasy::getString(playlist_json, "thumbnail");
                    tmp_data.registDateTime = ProcJsonEasy::getString(playlist_json, "registDateTime");
                    tmp_data.clientKey = ProcJsonEasy::getString(playlist_json, "clientKey");

                    tmp_data.favoriteTrack = ProcJsonEasy::getBool(playlist_json, "favoriteTrack");
                    tmp_data.favorite = ProcJsonEasy::getBool(playlist_json, "favorite");
                    tmp_data.thumbup = ProcJsonEasy::getBool(playlist_json, "thumbup");
                    tmp_data.isRose = ProcJsonEasy::getBool(playlist_json, "isRose");

                    QJsonArray tags_jsonArr = ProcJsonEasy::getJsonArray(playlist_json, "tags");
                    for(int i = 0; i < tags_jsonArr.count(); i++){
                        tmp_data.tags.append(tags_jsonArr.at(i).toString());
                    }

                    if(p_jsonObj.contains("totalCount")){
                        tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");
                    }
                    else{
                        tmp_data.totalCount = jsonArrSub.count();
                    }

                    list_output.append(tmp_data);
                }

                /*QJsonObject tmpObj = QJsonObject();
                tmpObj.insert("page", ProcJsonEasy::getInt(p_jsonObj, "page"));
                tmpObj.insert("totalCount", ProcJsonEasy::getInt(p_jsonObj, "totalCount"));

                flag_lastPage = this->get_flag_lastPage(tmpObj);*/

                flag_lastPage = ProcJsonEasy::getBool(p_jsonObj, "last");
            }
        }

        emit this->completeReq_list_recommeds(list_output, jsonArrSub, flag_lastPage);
    }


    void ProcCommon::setResult_list_recentlyAlbums(const QJsonObject &p_jsonObj){

        QList<roseHome::AlbumItemData> list_output;
        QJsonArray jsonArrSub = QJsonArray();
        bool flag_lastPage = true;

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObj, "recentList");

            if(jsonArrSub.size() > 0){
                for(int i = 0; i < jsonArrSub.count(); i++){
                    QJsonObject tmp_json = jsonArrSub.at(i).toObject();
                    QJsonObject album_json = ProcJsonEasy::getJsonObject(tmp_json, "album");

                    AlbumItemData tmp_data = roseHome::ConvertData::convertData_albumData(album_json);
                    tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");

                    list_output.append(tmp_data);
                }
            }

            /*QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("page", ProcJsonEasy::getInt(p_jsonObj, "page"));
            tmpObj.insert("totalCount", ProcJsonEasy::getInt(p_jsonObj, "totalCount"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);*/

            flag_lastPage = ProcJsonEasy::getBool(p_jsonObj, "last");
        }

        emit this->completeReq_list_albums(list_output, jsonArrSub, flag_lastPage);
    }


    void ProcCommon::setResult_list_recentlyPlaylists(const QJsonObject &p_jsonObj){

        QList<roseHome::PlaylistItemData> list_output;
        QJsonArray jsonArrSub = QJsonArray();
        bool flag_lastPage = true;

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObj, "recentList");

            if(jsonArrSub.size() > 0){

                for(int i = 0; i < jsonArrSub.count(); i++){
                    QJsonObject tmp_json = jsonArrSub.at(i).toObject();
                    QJsonObject playlist_json = ProcJsonEasy::getJsonObject(tmp_json, "playlist");

                    PlaylistItemData tmp_data = roseHome::ConvertData::convertData_playlistData(playlist_json);
                    tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");

                    list_output.append(tmp_data);
                }
            }

            /*QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("page", ProcJsonEasy::getInt(p_jsonObj, "page"));
            tmpObj.insert("totalCount", ProcJsonEasy::getInt(p_jsonObj, "totalCount"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);*/

            flag_lastPage = ProcJsonEasy::getBool(p_jsonObj, "last");
        }

        emit this->completeReq_list_playlists(list_output, jsonArrSub, flag_lastPage);
    }


    void ProcCommon::setResult_list_recentlyTracks(const QJsonObject &p_jsonObj){

        QList<roseHome::TrackItemData> list_output;
        QJsonArray jsonArrSub = QJsonArray();
        bool flag_lastPage = true;

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObj, "tracks");

            if(jsonArrSub.size() > 0){

                for(int i = 0; i < jsonArrSub.size(); i++){
                    QJsonObject track_obj = jsonArrSub.at(i).toObject();

                    roseHome::TrackItemData tmp_data = roseHome::ConvertData::convertData_trackItemData(track_obj);
                    tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");

                    list_output.append(tmp_data);
                }
            }

            /*QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("page", ProcJsonEasy::getInt(p_jsonObj, "page"));
            tmpObj.insert("totalCount", ProcJsonEasy::getInt(p_jsonObj, "totalCount"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);*/

            flag_lastPage = ProcJsonEasy::getBool(p_jsonObj, "last");
        }

        emit this->completeReq_list_tracks(list_output, jsonArrSub, flag_lastPage);
    }


    void ProcCommon::setResult_list_recentlyRoseTube(const QJsonObject &p_jsonObj){

        QJsonArray jsonArrSub = QJsonArray();
        bool flag_lastPage = true;
        int cnt_recentRosetube = 0;

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            if(p_jsonObj.contains("totalCount")){
                cnt_recentRosetube = ProcJsonEasy::getInt(p_jsonObj, "totalCount");
            }

            jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObj, "tracks");

            /*QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("page", ProcJsonEasy::getInt(p_jsonObj, "page"));
            tmpObj.insert("totalCount", ProcJsonEasy::getInt(p_jsonObj, "totalCount"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);*/

            flag_lastPage = ProcJsonEasy::getBool(p_jsonObj, "last");
        }

        emit this->completeReq_list_rosetubes(jsonArrSub, cnt_recentRosetube, flag_lastPage);
    }


    void ProcCommon::setResult_list_myPlaylists(const QJsonObject &p_jsonObj){

        QList<roseHome::PlaylistItemData> list_output;
        QJsonArray jsonArrSub = QJsonArray();
        bool flag_lastPage = true;

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObj, "playlists");

            if(jsonArrSub.size() > 0){

                for(int i = 0; i < jsonArrSub.count(); i++){
                    QJsonObject tmp_json = jsonArrSub.at(i).toObject();

                    PlaylistItemData tmp_data = roseHome::ConvertData::convertData_playlistData(tmp_json);
                    tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");

                    list_output.append(tmp_data);
                }
            }

            /*QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("page", ProcJsonEasy::getInt(p_jsonObj, "page"));
            tmpObj.insert("totalCount", ProcJsonEasy::getInt(p_jsonObj, "totalCount"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);*/

            flag_lastPage = ProcJsonEasy::getBool(p_jsonObj, "last");
        }

        emit this->completeReq_list_myplaylists(list_output, jsonArrSub, flag_lastPage);
    }


    void ProcCommon::setResult_list_usersPlaylists(const QJsonObject &p_jsonObj){

        QList<roseHome::PlaylistItemData> list_output;
        QJsonArray jsonArrSub = QJsonArray();
        bool flag_lastPage = true;

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            QJsonArray jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObj, "playlists");

            if(jsonArrSub.size() > 0){

                for(int i = 0; i < jsonArrSub.count(); i++){
                    QJsonObject tmp_json = jsonArrSub.at(i).toObject();

                    PlaylistItemData tmp_data = roseHome::ConvertData::convertData_playlistData(tmp_json);
                    tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");

                    list_output.append(tmp_data);
                }
            }

            /*QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("page", ProcJsonEasy::getInt(p_jsonObj, "page"));
            tmpObj.insert("totalCount", ProcJsonEasy::getInt(p_jsonObj, "totalCount"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);*/

            flag_lastPage = ProcJsonEasy::getBool(p_jsonObj, "last");
        }

        emit this->completeReq_list_usersplaylists(list_output, jsonArrSub, flag_lastPage);
    }


    void ProcCommon::setResult_list_favoriteAlbums(const QJsonObject &p_jsonObj){

        QList<roseHome::AlbumItemData> list_output;
        QJsonArray jsonArrSub = QJsonArray();
        bool flag_lastPage = true;

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObj, "albums");

            if(jsonArrSub.size() > 0){
                for(int i = 0; i < jsonArrSub.count(); i++){
                    QJsonObject album_json = jsonArrSub.at(i).toObject();

                    AlbumItemData tmp_data = roseHome::ConvertData::convertData_albumData(album_json);
                    tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");

                    list_output.append(tmp_data);
                }
            }

            /*QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("page", ProcJsonEasy::getInt(p_jsonObj, "page"));
            tmpObj.insert("totalCount", ProcJsonEasy::getInt(p_jsonObj, "totalCount"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);*/

            flag_lastPage = ProcJsonEasy::getBool(p_jsonObj, "last");
        }

        emit this->completeReq_list_albums(list_output, jsonArrSub, flag_lastPage);
    }


    void ProcCommon::setResult_list_favoritePlaylists(const QJsonObject &p_jsonObj){

        QList<roseHome::PlaylistItemData> list_output;
        QJsonArray jsonArrSub = QJsonArray();
        bool flag_lastPage = true;

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObj, "playlists");

            if(jsonArrSub.size() > 0){

                for(int i = 0; i < jsonArrSub.count(); i++){
                    QJsonObject playlist_json = jsonArrSub.at(i).toObject();

                    PlaylistItemData tmp_data = roseHome::ConvertData::convertData_playlistData(playlist_json);
                    tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");

                    list_output.append(tmp_data);
                }
            }

            /*QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("page", ProcJsonEasy::getInt(p_jsonObj, "page"));
            tmpObj.insert("totalCount", ProcJsonEasy::getInt(p_jsonObj, "totalCount"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);*/

            flag_lastPage = ProcJsonEasy::getBool(p_jsonObj, "last");
        }

        emit this->completeReq_list_playlists(list_output, jsonArrSub, flag_lastPage);
    }


    void ProcCommon::setResult_list_favoriteTracks(const QJsonObject &p_jsonObj){

        QList<roseHome::TrackItemData> list_output;
        QJsonArray jsonArrSub = QJsonArray();
        bool flag_lastPage = true;

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObj, "tracks");

            if(jsonArrSub.size() > 0){

                for(int i = 0; i < jsonArrSub.size(); i++){
                    QJsonObject track_obj = jsonArrSub.at(i).toObject();

                    roseHome::TrackItemData tmp_data = roseHome::ConvertData::convertData_trackItemData(track_obj);
                    tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");

                    list_output.append(tmp_data);
                }

                /*QJsonObject tmpObj = QJsonObject();
                tmpObj.insert("page", ProcJsonEasy::getInt(p_jsonObj, "page"));
                tmpObj.insert("totalCount", ProcJsonEasy::getInt(p_jsonObj, "totalCount"));

                flag_lastPage = this->get_flag_lastPage(tmpObj);*/

                flag_lastPage = ProcJsonEasy::getBool(p_jsonObj, "last");
            }
        }

        emit this->completeReq_list_tracks(list_output, jsonArrSub, flag_lastPage);
    }


    void ProcCommon::setResult_album(const QJsonObject &p_jsonObj){

        roseHome::AlbumItemData data_output;
        QJsonArray tracks = QJsonArray();
        QList<roseHome::TrackItemData> list_output;

        bool flag_lastPage = true;

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            QJsonObject album = ProcJsonEasy::getJsonObject(p_jsonObj, "album");

            data_output = roseHome::ConvertData::convertData_albumData(album);

            emit this->completeReq_albumlist(data_output);

            tracks = ProcJsonEasy::getJsonArray(album, "tracks");

            if(tracks.size() > 0){

                for(int i = 0; i < tracks.count(); i++){
                    QJsonObject tmp_json = tracks.at(i).toObject();

                    TrackItemData tmp_data = roseHome::ConvertData::convertData_trackItemData(tmp_json);
                    tmp_data.trackCount = ProcJsonEasy::getInt(album, "trackCount");

                    list_output.append(tmp_data);
                }
            }

            /*QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("page", ProcJsonEasy::getInt(p_jsonObj, "page"));
            tmpObj.insert("totalCount", ProcJsonEasy::getInt(p_jsonObj, "totalCount"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);*/

            flag_lastPage = ProcJsonEasy::getBool(p_jsonObj, "last");
            emit this->completeReq_list_tracks(list_output, tracks, flag_lastPage);
        }
        else{
            emit this->completeReq_albumlist(data_output);
            emit this->completeReq_list_tracks(list_output, tracks, flag_lastPage);
        }
    }


    void ProcCommon::setResult_playlist(const QJsonObject &p_jsonObj){

        roseHome::PlaylistItemData data_output;
        QJsonArray tracks = QJsonArray();
        QList<roseHome::TrackItemData> list_output;

        bool flag_lastPage = true;

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            QJsonObject playlist = ProcJsonEasy::getJsonObject(p_jsonObj, "playlist");

            data_output = roseHome::ConvertData::convertData_playlistData(playlist);
            data_output.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");

            emit this->completeReq_playlist(data_output);

            tracks = ProcJsonEasy::getJsonArray(playlist, "tracks");

            if(tracks.size() > 0){

                for(int i = 0; i < tracks.count(); i++){
                    QJsonObject tmp_json = tracks.at(i).toObject();

                    TrackItemData tmp_data = roseHome::ConvertData::convertData_trackItemData(tmp_json);
                    tmp_data.trackCount = ProcJsonEasy::getInt(playlist, "trackCount");

                    list_output.append(tmp_data);
                }
            }

            /*QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("page", ProcJsonEasy::getInt(p_jsonObj, "page"));
            tmpObj.insert("totalCount", ProcJsonEasy::getInt(p_jsonObj, "totalCount"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);*/

            flag_lastPage = ProcJsonEasy::getBool(p_jsonObj, "last");
            emit this->completeReq_list_tracks(list_output, tracks, flag_lastPage);
        }
        else{
            emit this->completeReq_playlist(data_output);
            emit this->completeReq_list_tracks(list_output, tracks, flag_lastPage);
        }
    }


    void ProcCommon::setResult_track(const QJsonObject &p_jsonObj){

        QList<roseHome::TrackItemData> list_output;
        QJsonArray jsonArrSub = QJsonArray();
        bool flag_lastPage = true;

        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObj, "tracks");

            if(jsonArrSub.size() > 0){

                for(int i = 0; i < jsonArrSub.size(); i++){
                    QJsonObject track_obj = jsonArrSub.at(i).toObject();

                    roseHome::TrackItemData tmp_data = roseHome::ConvertData::convertData_trackItemData(track_obj);
                    tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");

                    list_output.append(tmp_data);
                }

                /*QJsonObject tmpObj = QJsonObject();
                tmpObj.insert("page", ProcJsonEasy::getInt(p_jsonObj, "page"));
                tmpObj.insert("totalCount", ProcJsonEasy::getInt(p_jsonObj, "totalCount"));

                flag_lastPage = this->get_flag_lastPage(tmpObj);*/

                flag_lastPage = ProcJsonEasy::getBool(p_jsonObj, "last");
            }
        }

        emit this->completeReq_list_tracks(list_output, jsonArrSub, flag_lastPage);
    }

    void ProcCommon::setResult_rosetube(const QJsonObject &p_jsonObj){

        QJsonArray jsonArrSub = QJsonArray();
        bool flag_lastPage = true;
        int cnt_trackRosetube = 0;

        if(!p_jsonObj.isEmpty()){

            jsonArrSub.append(p_jsonObj);

            flag_lastPage = true;
            cnt_trackRosetube = 1;
        }

        emit this->completeReq_list_rosetubes(jsonArrSub, cnt_trackRosetube, flag_lastPage);
    }


    void ProcCommon::setResult_Favorite_checkRating_Playlist(const QJsonObject &p_jsonObj){

        emit completeCheck_rating_playlist(p_jsonObj);
    }


    void ProcCommon::setResult_Favorite_checkRating_Album(const QJsonObject &p_jsonObj){

        emit completeCheck_rating_album(p_jsonObj);
    }


    void ProcCommon::setResult_Favorite_checkRating_Track(const QJsonObject &p_jsonObj){

        emit completeCheck_rating_track(p_jsonObj);
    }


    void ProcCommon::setResult_Favorite_getRating_Playlist(const QJsonObject &p_jsonObj){

        QJsonArray contents;
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
            contents = ProcJsonEasy::getJsonArray(p_jsonObj, "contents");
        }

        emit completeReq_rating_playlist(contents);
    }


    void ProcCommon::setResult_Favorite_getRating_Album(const QJsonObject &p_jsonObj){

        QJsonArray contents;
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
            contents = ProcJsonEasy::getJsonArray(p_jsonObj, "contents");
        }

        emit completeReq_rating_album(contents);
    }


    void ProcCommon::setResult_Favorite_getRating_Track(const QJsonObject &p_jsonObj){

        //QJsonDocument doc(p_jsonObj);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "p_jsonObj = " << strJson;

        QJsonArray contents;
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            if(p_jsonObj.contains("contents")){
                contents = ProcJsonEasy::getJsonArray(p_jsonObj, "contents");
            }
            else if(p_jsonObj.contains("rating")){
                contents.append(ProcJsonEasy::getJsonObject(p_jsonObj, "rating"));
            }
        }

        emit completeReq_rating_track(contents);
    }


    void ProcCommon::setResult_Thumbup_getRating_Playlist(const QJsonObject &p_jsonObj){

        emit completeReq_rating_thumbup(p_jsonObj);
    }


    void ProcCommon::setResult_Favorite_setRating_Playlist(const QJsonObject &p_jsonObj){

        QJsonArray contents;
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            contents.append(p_jsonObj);
        }

        emit completeReq_rating_playlist(contents);
    }


    void ProcCommon::setResult_Favorite_setRating_Album(const QJsonObject &p_jsonObj){

        QJsonArray contents;
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            contents.append(p_jsonObj);
        }

        emit completeReq_rating_album(contents);
    }


    void ProcCommon::setResult_Favorite_setRating_Track(const QJsonObject &p_jsonObj){

        QJsonArray contents;
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            contents.append(p_jsonObj);
        }

        emit completeReq_rating_track(contents);
    }


    void ProcCommon::setResult_Thumbup_setRating_Playlist(const QJsonObject &p_jsonObj){

        emit completeReq_rating_thumbup(p_jsonObj);
    }


    void ProcCommon::setResult_Create_myPlaylist(const QJsonObject &p_jsonObj){

        emit completeReq_create_playlist(p_jsonObj);
    }


    void ProcCommon::setResult_AddTrack_myPlaylist(const QJsonObject &p_jsonObj){

        emit completeReq_addTrack_playlist(p_jsonObj);
    }


    void ProcCommon::setResult_list_radioChannel(const QJsonObject &p_jsonObj){

        QJsonArray contents;
        int totalCount = 0;
        bool flagLast = true;

        if(p_jsonObj.contains("code") && (p_jsonObj.value("code").toString() == "200")){

            totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");
            flagLast = ProcJsonEasy::getBool(p_jsonObj, "last");
            contents = ProcJsonEasy::getJsonArray(p_jsonObj, "radioChannels");
        }

        emit completeReq_list_roseRadio(contents, totalCount, flagLast);
    }


    void ProcCommon::setResult_list_radioGenre(const QJsonObject &p_jsonObj){

        QJsonArray Genre;
        int totalCount = 0;
        bool flagLast = true;

        if(p_jsonObj.contains("code") && (p_jsonObj.value("code").toString() == "200")){

            totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");
            flagLast = ProcJsonEasy::getBool(p_jsonObj, "last");
            Genre = ProcJsonEasy::getJsonArray(p_jsonObj, "radioGenres");
        }

        emit completeReq_list_roseRadio(Genre, totalCount, flagLast);
    }


    void ProcCommon::setResult_list_radioCountry(const QJsonObject &p_jsonObj){

        QJsonArray country;
        int totalCount = 0;
        bool flagLast = true;

        if(p_jsonObj.contains("code") && (p_jsonObj.value("code").toString() == "200")){

            totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");
            flagLast = ProcJsonEasy::getBool(p_jsonObj, "last");
            country = ProcJsonEasy::getJsonArray(p_jsonObj, "radioRegions");
        }

        emit completeReq_list_roseRadio(country, totalCount, flagLast);
    }


    void ProcCommon::setResult_list_radioLanguage(const QJsonObject &p_jsonObj){

        QJsonArray country;
        int totalCount = 0;
        bool flagLast = true;

        if(p_jsonObj.contains("code") && (p_jsonObj.value("code").toString() == "200")){

            totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalCount");
            flagLast = ProcJsonEasy::getBool(p_jsonObj, "last");
            country = ProcJsonEasy::getJsonArray(p_jsonObj, "radioLanguages");
        }

        emit completeReq_list_roseRadio(country, totalCount, flagLast);
    }


    void ProcCommon::setResult_Favorite_set_Radio(const QJsonObject &p_jsonObj){

        emit completeReq_favorite_radio(p_jsonObj);
    }


    void ProcCommon::setResult_share_link(const QJsonObject &p_jsonObj){

        QString link = "";

        if(p_jsonObj.contains("code") && (p_jsonObj.value("code").toString() == "200")){

            link = ProcJsonEasy::getString(p_jsonObj, "link");
        }

        emit completeReq_share_link(link);
    }
}
