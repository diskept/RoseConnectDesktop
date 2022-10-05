#ifndef PROCCOMMON_H
#define PROCCOMMON_H

#include "tidal_struct.h"

#include <QObject>
#include <QJsonObject>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    static QList<tidal::GenreInfo> list_genre;      ///< Tidal Genres (최초 한번만 가져오면, 그뒤로 계속 재사용함)
    static QList<tidal::MoodInfo> list_mood;        ///< Tidal Moods & Activities (최초 한번만 가져오면, 그뒤로 계속 재사용함)

    /**
     * @brief Tidal에서만 사용하는 전용 처리담당 클래스. (다른 파트에서 사용을 해야하는 경우에는 @se ProcTidal_forOuter 클래스 사용할 것)
     */
    class ProcCommon : public QObject
    {
         Q_OBJECT

    public:
        /**
         * @brief Tidal용 Request - about TIDAL API
         */
        enum HttpRequestType {
            Get_Token
            , Get_refreshToken
            , Get_Session
            , Get_Subscription

            , GetList_Mixes = 10            ///< 타이달의 mixes 목록 요청
            , GetList_Playlists             ///< 타이달의 playlist 목록 요청
            , GetList_Albums                ///< 타이달의 album 목록 요청
            , GetList_Visual                ///< 타이달의 visual 목록 요청
            , GetList_Artists               ///< 타이달의 artist 목록 요청
            , GetList_Tracks                ///< 타이달의 track 목록 요청
            , GetList_Videos                ///< 타이달의 video 목록 요청

            , GetList_Items_Mixes           ///< 타이달의 mixes의 track 목록 요청
            , GetList_Items_Playlist        ///< 타이달의 playlist의 track 목록 요청
            , GetList_Items_Album           ///< 타이달의 album의 track 목록 요청
            , GetList_Items_Visual          ///< 타이달의 visual의 track 목록 요청


            , GetList_My_Favorite_Playlists      ///< My playlists 목록 요청 (Favorite)
            , GetList_My_Favorite_Albums         ///< My Albums 목록 요청 (Favorite)
            , GetList_My_Favorite_Tracks         ///< My Tracks 목록 요청 (Favorite)
            , GetList_My_Favorite_Videos         ///< My Videos 목록 요청 (Favorite)
            , GetList_My_Favorite_Artists        ///< My Artists 목록 요청 (Favorite)

            , Get_PlaylistInfo          ///< Playlist 1개 정보 요청
            , Get_AlbumInfo             ///< Album 1개 정보 요청
            , Get_ArtistInfo            ///< Artist 1개 정보 요청
            , Get_ArtistBioText         ///< Artist의 신상정보 텍스트 반환

            , GetAll_UserFavorites      ///< 사용자의 모든 좋아요 정보 반환 (종류별로 id 목록들을 반환함)

            , Favorite_Add_Album
            , Favorite_Add_Artist
            , Favorite_Add_Track
            , Favorite_Add_Playlist
            , Favorite_Add_Video

            , Favorite_Delete_Album
            , Favorite_Delete_Artist
            , Favorite_Delete_Track
            , Favorite_Delete_Playlist
            , Favorite_Delete_Video


            , GetList_My_Created_Playlists      ///< My playlists 목록 요청 (사용자가 만든)
            , UserCreatePlaylist_Delete
            , UserCreatePlaylist_Add_Tracks
            , GetList_TrackIds_Playlist
            , GetList_TrackIds_Album
            , DeleteItems_in_MyCreatedPlaylist
            , Create_userPlaylist
            , Delete_userPlaylist
            , AddTracks_userPlaylist

            , Search_GetList_Track
            , Search_GetList_Artist
            , Search_GetList_Album
            , Search_GetList_Playlist
            , Search_GetList_Video
            , Search_GetList_TopHit


            , GetList_Genres = 1001
            , GetList_Moods

            , Login = 1900
            , Logout


        };
        Q_ENUM(HttpRequestType);


        /**
         * @brief 타이달 데이터 정렬 옵션
         */
        enum DataOrderOption {
            NAME            ///< 이름순 정렬
            , DATE          ///< 날짜순 정렬
        };
        Q_ENUM(DataOrderOption);

        /**
         * @brief 타이달 데이터 정렬 방향 (오름차순, 내림차순)
         */
        enum DataOrderDirection {
            DESC            ///< 내림차순
            , ASC           ///< 올림차순
        };
        Q_ENUM(DataOrderDirection);

        enum class TidalSearch_ContentType{
            Tracks
            , Artists
            , Albums
            , Playlists
            , Videos
            , TopHit
        };
        Q_ENUM(TidalSearch_ContentType);


    signals:
        void successLogin();                                ///< 로그인 처리 성공 시그널
        void failedLogin(const QString& errorMsg);          ///< 로그인 처리 실패 시그널
        void successLogout();                               ///< 로그아웃 처리 성공 시그널

        void signal_completeReq_getToken(const bool flagSuccess, const QString errorMsg);
        void signal_completeReq_getRefreshToken(const bool flagSuccess, const QString errorMsg);
        void signal_completeReq_getSession(const QString errorMsg, const QJsonObject& jsonObj_session);
        void signal_completeReq_getSubscription(const QString errorMsg, const QJsonObject& jsonObj_session);
        void signal_completeReq_getListGenre(const bool flagSuccess, const QString errorMsg);
        void signal_completeReq_getListMoods(const bool flagSuccess, const QString errorMsg);

        // General
        void completeReq_list_mixes(const QList<tidal::PlaylistItemData> &list_playlist, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_playlists(const QList<tidal::PlaylistItemData> &list_playlist, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_albums(const QList<tidal::AlbumItemData> &list_album, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_visuals(const QList<tidal::AlbumItemData> &list_album, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_artists(const QList<tidal::ArtistItemData> &list_artist, const bool flag_lastPage);
        void completeReq_list_tracks(const QList<tidal::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_videos(const QList<tidal::VideoItemData> &list_video, const bool flag_lastPage);
        void completeReq_list_topHit(const QList<tidal::ArtistItemData> &list_artist, const bool flag_lastPage);

        void completeReq_list_items_of_mixes(const QList<tidal::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_items_of_playlist(const QList<tidal::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_items_of_album(const QList<tidal::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_items_of_visual(const QList<tidal::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);


        // User's Creative
        void completeReq_list_myCreatedPlaylists(const QList<tidal::PlaylistItemData> &list_playlist, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage, const QJsonObject& jsonObj_opt);
        void completeReq_delete_my_playlist(const QJsonObject& jsonObj_opt);
        void completeReq_add_items_in_myPlaylist();
        void completeReq_list_trackIds_toAddPlaylist(const QString playlist_uuid_target, const QString track_ids);
        void completeReq_delete_items_in_myPlaylist();
        void completeReq_create_myPlaylist(const QString uuid);
        void completeReq_addTracks_myPlaylist(const bool flag);


        // Favorite
        void completeReq_list_myFavoritePlaylists(const QList<tidal::PlaylistItemData> &list_playlist, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_myFavoriteAlbums(const QList<tidal::AlbumItemData> &list_album, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_myFavoriteTracks(const QList<tidal::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_myFavoriteArtists(const QList<tidal::ArtistItemData> &list_artist, const bool flag_lastPage);
        void completeReq_list_myFavoriteVideos(const QList<tidal::VideoItemData> &list_video, const bool flag_lastPage);

        // Specific
        void completeReq_playlist(const tidal::PlaylistItemData &data_playlist);
        void completeReq_album(const tidal::AlbumItemData &data_album);
        void completeReq_artist(const tidal::ArtistItemData &data_artist);
        void completeReq_artist_bioText(const QString& bioText);

        void completeReq_listAll_myFavoritesIds(const QJsonObject &jsonObj);            ///<  setResult_listAll_myFavoritesIds 호출에 대한 결과값을 받을 수 있는 signal


    public :

        explicit ProcCommon(QWidget *parent = nullptr);
        ~ProcCommon();
        static QString convertImagePath(const QString tidalImageKey, const int imageSize);

        // 로그인 정보 처리 (SQLlite3)
        void clearLoginInfo_tidalDB();
        void saveLoginInfo_tidalDB(UserLoginInfo &userLoginInfo);
        UserLoginInfo getLoginInfo_tidalDB();

        // OAth
        void clearTablesInfo_tidalDB();
        void saveTokenInfo_tidalDB(TidalTokenInfo&);
        TidalTokenInfo getTokenInfo_tidalDB();

        void request_tidal_getTokenAndSave();
        void request_tidal_getRefreshTokenAndSave();
        void request_tidal_getSession();
        void request_tidal_getSubscription(int user_id);
        void request_tidal_setLogOut();


        // HTTP 요청 - 공통으로 사용하는 타이달 API
        void request_tidalLogin(UserLoginInfo userLoginInfo);
        void request_tidalLogout();

        void request_tidal_getListGenres();
        void request_tidal_getListMoods();

        void request_tidal_getList_mixes(const QString path, const int limit, const int offset);
        void request_tidal_getList_playlists(const QString path, const int limit, const int offset);
        void request_tidal_getList_albums(const QString path, const int limit, const int offset);
        void request_tidal_getList_visual(const QString path, const int limit, const int offset);
        void request_tidal_getList_artists(const QString path, const int limit, const int offset);
        void request_tidal_getList_tracks(const QString path, const int limit, const int offset);
        void request_tidal_getList_videos(const QString path, const int limit, const int offset);

        void request_tidal_getList_items_of_mixes(const QString mixes_uuid, const int limit, const int offset);
        void request_tidal_getList_items_of_playlist(const QString playlist_uuid, const int limit, const int offset);
        void request_tidal_getList_items_of_album(const int album_id);
        void request_tidal_getList_items_of_visual(const int album_id, const int limit);

        void request_tidal_get_playlist(const QString playlist_id);
        void request_tidal_get_album(const int album_id);
        void request_tidal_get_artist(const int artist_id);
        void request_tidal_get_artist_bioText(const int artist_id);



        // User's Playlist 관련
        void request_tidal_getList_myCreatedPlaylists(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset, const QJsonObject& jsonObj_opt=QJsonObject());
        void request_tidal_delete_myCreatedPlaylist(const QString playlist_uuid, const QJsonObject& jsonObj_opt);
        void request_tidal_add_track_in_myPlaylist(const QString playlist_uuid_target, const int track_id);
        void request_tidal_add_album_in_myPlaylist(const QString playlist_uuid_target, const int album_id);
        void request_tidal_add_playlist_in_myPlaylist(const QString playlist_uuid_target, const QString playlist_uuid);

        void request_tidal_delete_items_in_myPlaylist(const QString playlist_uuid_target, const QList<QString>& list_itemIds);
        void request_tidal_create_myPlaylist(const int user_id, const QString playlsit_title);
        void request_tidal_addTracks_myPlaylist(const QString playlist_id, const QString track_id);



        // about my favorite collection
        void request_tidal_getList_myFavoritePlaylists(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset);
        void request_tidal_getList_myFavoriteAlbums(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset);
        void request_tidal_getList_myFavoriteTracks(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset);
        void request_tidal_getList_myFavoriteArtists(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset);
        void request_tidal_getList_myFavoriteVideos(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset);



        // Tidal 회원의 즐겨찾기 정보 반환 (좋아요 여부를 확인하기 위함)
        void request_tidal_getAll_favorites();
        void request_tidal_set_favorite(const QString type, const QString id);
        void request_tidal_del_favorite(const QString type, const QString id);


        // about adding Favorite - 일부러 종류별로 놔둠.
        void request_tidal_addFavorite_album(const int album_id);
        void request_tidal_addFavorite_artist(const int artist_id);
        void request_tidal_addFavorite_track(const int track_id);
        void request_tidal_addFavorite_playlist(const QString playlist_uuid);
        void request_tidal_addFavorite_video(const int video_id);

        // about deleting Favorite
        void request_tidal_deleteFavorite_album(const int album_id);
        void request_tidal_deleteFavorite_artist(const int artist_id);
        void request_tidal_deleteFavorite_track(const int track_id);
        void request_tidal_deleteFavorite_playlist(const QString playlist_uuid);
        void request_tidal_deleteFavorite_video(const int video_id);

        // search
        void request_tidal_searchAllItems(const QString search_word, const int limit, const int offset, const TidalSearch_ContentType contentType);


        // Data 관련 반환 - Genres, Moods
        static int getCount_listGenreInfo();
        static int getCount_listMoodsList();
        static tidal::GenreInfo getGenreInfo(int idx);
        static tidal::MoodInfo getMoodInfo(int idx);


    private:
        void procQueryList(const QList<QString> &list_query);
//        QStringList getList_artists(const QJsonArray& jsonArr_artist);


        // HTTP 응답처리 함수
        void setResult_loginTidal(const QJsonObject &p_jsonObj);
        void setResult_logoutTidal(const QJsonObject &p_jsonObj);

        // oAuth 인증 응답처리 함수
        void setResult_getToken(const QJsonObject &p_jsonObj);
        void setResult_getRefreshToken(const QJsonObject &p_jsonObj);
        void setResult_getSessionInfo(const QJsonObject &p_jsonObj);
        void setResult_getSubscription(const QJsonObject &p_jsonObj);

        void setResult_listGenres(const QJsonObject &p_jsonObj);
        void setResult_listMoods(const QJsonObject &p_jsonObj);


        // more data에 대한 last page 체크
        bool get_flag_lastPage(const QJsonObject& p_jsonObj);

        void setResult_list_mixeslists(const QJsonObject &p_jsonObj);
        void setResult_list_playlists(const QJsonObject &p_jsonObj);
        void setResult_list_albums(const QJsonObject &p_jsonObj);
        void setResult_list_viuals(const QJsonObject &p_jsonObj);
        void setResult_list_artists(const QJsonObject &p_jsonObj);
        void setResult_list_tracks(const QJsonObject &p_jsonObj);
        void setResult_list_videos(const QJsonObject &p_jsonObj);

        void setResult_list_itmes_of_mixes(const QJsonObject &p_jsonObj);
        void setResult_list_itmes_of_playlist(const QJsonObject &p_jsonObj);
        void setResult_list_itmes_of_album(const QJsonObject &p_jsonObj);
        void setResult_list_itmes_of_visual(const QJsonObject &p_jsonObj);

        void setResult_playlist(const QJsonObject &p_jsonObj);
        void setResult_album(const QJsonObject &p_jsonObj);
        void setResult_artist(const QJsonObject &p_jsonObj);
        void setResult_artist_bioText(const QJsonObject &p_jsonObj);



        // about user's created playlist
        void setResult_list_myCreatedPlaylists(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt);
        void setResult_delete_myCreatedplaylist(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt);
        void setResult_add_items_in_myPlaylist(const QJsonObject &p_jsonObj);

        void request_tidal_addPlaylist_items(QString playlist_uuid, QString itemIds);

        void request_tidal_getList_trackIds_of_playlist(const QString playlist_uuid, const QJsonObject& jsonObj_opt);
        void setResult_list_trackId_of_playlist(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt);

        void request_tidal_getList_trackIds_of_album(const int album_id, const QJsonObject& jsonObj_opt);
        void setResult_list_trackId_of_album(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt);

        void setResult_delete_items_in_myPlaylist(const QJsonObject &p_jsonObj);
        void setResult_create_myPlaylist(const QJsonObject &p_jsonObj);
        void setResult_addTracks_myPlaylsit(const QJsonObject &p_jsonObj);




        // about my favorite collection
        void setResult_list_myFavoritePlaylists(const QJsonObject &p_jsonObj);
        void setResult_list_myFavoriteAlbums(const QJsonObject &p_jsonObj);
        void setResult_list_myFavoriteTracks(const QJsonObject &p_jsonObj);
        void setResult_list_myFavoriteArtists(const QJsonObject &p_jsonObj);
        void setResult_list_myFavoriteVideos(const QJsonObject &p_jsonObj);

        void setResult_listAll_myFavoritesIds(const QJsonObject &p_jsonObj);

        // about adding/deleting favorite (common type)
        void request_tidal_addFavorie(const HttpRequestType requestType, const QString id_key, const QVariant id_val, const QString subPath);
        void request_tidal_deleteFavorie(const HttpRequestType requestType, const QVariant id_val, const QString subPath);


        // search
        void setResult_search_getList_track(const QJsonObject &p_jsonObj);
        void setResult_search_getList_artist(const QJsonObject &p_jsonObj);
        void setResult_search_getList_album(const QJsonObject &p_jsonObj);
        void setResult_search_getList_playlist(const QJsonObject &p_jsonObj);
        void setResult_search_getList_video(const QJsonObject &p_jsonObj);
        void setResult_search_getList_topHit(const QJsonObject &p_jsonObj);


        UserLoginInfo userLoginInfo;                    ///< 타이달 로그인을 위한 정보저장


    private slots:
        void slot_responseHttp(int p_id, QJsonObject p_jsonObj);

        void slot_getDone_trackIds_toAddPlaylist(const QString playlist_uuid_target, const QString track_ids);

        void slot_getToken(const bool flagSuccess, const QString errorMsg);

    private:
        bool visual_album_offset = false;
    };

};

#endif // PROCCOMMON_H
