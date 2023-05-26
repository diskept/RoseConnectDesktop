#ifndef PROCCOMMON_FORQOBUZ_H
#define PROCCOMMON_FORQOBUZ_H

#include "qobuz/qobuz_struct.h"

#include "common/linker.h"//230421

#include <QObject>
#include <QJsonObject>


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";



namespace qobuz {

    static QList<qobuz::GenreInfo> list_genre;      ///< Qobuz Genres (최초 한번만 가져오면, 그뒤로 계속 재사용함)
    static QList<qobuz::MoodInfo> list_mood;        ///< Qobuz Moods & Activities (최초 한번만 가져오면, 그뒤로 계속 재사용함)

    /**
     * @brief Qobuz에서만 사용하는 전용 처리담당 클래스. (다른 파트에서 사용을 해야하는 경우에는 @se ProcQobuz_forOuter 클래스 사용할 것)
     */
    class ProcCommon : public QObject
    {
         Q_OBJECT

    public:
        /**
         * @brief Qobuz용 Request - about QOBUZ API
         */
        enum HttpRequestType {
            GetList_Playlists = 10              ///< Qobuz의 playlist 목록 요청
            , GetList_Albums                    ///< Qobuz의 album 목록 요청
            , GetList_Artists                   ///< Qobuz의 artist 목록 요청
            , GetList_Tracks                    ///< Qobuz의 track 목록 요청
            , GetList_Appearson                 ///< Qobuz의 artist의 apears on 목록 요청
            , GetList_LastRelease               ///< Qobuz의 artist의 apears on 목록 요청
            , GetList_Videos                    ///< Qobuz의 video 목록 요청

            , GetList_Items_Playlist            ///< Qobuz의 playlist의 track 목록 요청
            , GetList_Items_Album               ///< Qobuz의 album의 track 목록 요청


            , GetList_My_Favorite_Playlists      ///< My playlists 목록 요청 (Favorite)
            , GetList_My_Favorite_Albums         ///< My Albums 목록 요청 (Favorite)
            , GetList_My_Favorite_Tracks         ///< My Tracks 목록 요청 (Favorite)
            , GetList_My_Favorite_Videos         ///< My Videos 목록 요청 (Favorite)
            , GetList_My_Favorite_Artists        ///< My Artists 목록 요청 (Favorite)

            , Get_AlbumInfo                      ///< Album 1개 정보 요청
            , Get_ArtistInfo                     ///< Artist 1개 정보 요청
            , Get_ArtistBioText                  ///< Artist의 신상정보 텍스트 반환

            , GetAll_UserFavorites               ///< 사용자의 모든 좋아요 정보 반환 (종류별로 id 목록들을 반환함)

            , GetList_My_Purchases_Albums
            , GetList_My_Purchases_Tracks

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

            , GetList_My_Created_Playlists       ///< My playlists 목록 요청 (사용자가 만든)
            , UserCreatePlaylist_Delete
            , UserCreatePlaylist_Add_Tracks
            , GetList_TrackIds_Playlist
            , GetList_TrackIds_Album
            , DeleteItems_in_MyCreatedPlaylist
            , Create_userPlaylist
            , Delete_userPlaylist
            , AddTracks_userPlaylist

            , Search_GetList_MostPopular
            , Search_GetList_Track
            , Search_GetList_Artist
            , Search_GetList_Album
            , Search_GetList_Playlist
            , Search_GetList_Video


            , GetList_Genres = 1001
            , GetList_Moods

            , Login = 1900
            , Logout
            , Login_termCall//c230422

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

        enum class QobuzSearch_ContentType{
            MostPopular
            , Tracks
            , Artists
            , Albums
            , Playlists
            , Videos
        };
        Q_ENUM(QobuzSearch_ContentType);


    signals:
        void successLogin();                                ///< 로그인 처리 성공 시그널
        void failedLogin(const QString& errorMsg);          ///< 로그인 처리 실패 시그널
        void successLogout();                               ///< 로그아웃 처리 성공 시그널

        // General
        void completeReq_list_playlists(const QList<qobuz::PlaylistItemData> &list_playlist, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_albums(const QList<qobuz::AlbumItemData> &list_album, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_mostPopular(const QJsonObject &jsonObj, const QString type);
        void completeReq_list_artists(const QList<qobuz::ArtistItemData> &list_artist, const bool flag_lastPage);
        void completeReq_list_tracks(const QList<qobuz::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_appearson(const QList<qobuz::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_lastrelease(const QList<qobuz::AlbumItemData> &list_album, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_videos(const QList<qobuz::VideoItemData> &list_video, const bool flag_lastPage);

        void completeReq_list_items_of_playlist(const QList<qobuz::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_items_of_album(const QList<qobuz::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);


        // User's Creative
        void completeReq_list_myCreatedPlaylists(const QList<qobuz::PlaylistItemData> &list_playlist, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage, const QJsonObject& jsonObj_opt);
        void completeReq_delete_my_playlist(const QJsonObject& jsonObj_opt);
        void completeReq_add_items_in_myPlaylist();
        void completeReq_list_trackIds_toAddPlaylist(const QString playlist_uuid_target, const QString track_ids);
        void completeReq_delete_items_in_myPlaylist();
        void completeReq_create_myPlaylist(const int playlist_id);
        void completeReq_addTracks_myPlaylist(const int tracks_count);


        // Favorite
        void completeReq_list_myFavoritePlaylists(const QList<qobuz::PlaylistItemData> &list_playlist, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_myFavoriteAlbums(const QList<qobuz::AlbumItemData> &list_album, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_myFavoriteTracks(const QList<qobuz::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_myFavoriteArtists(const QList<qobuz::ArtistItemData> &list_artist, const bool flag_lastPage);
        void completeReq_list_myFavoriteVideos(const QList<qobuz::VideoItemData> &list_video, const bool flag_lastPage);

        // Purchases
        void completeReq_list_myPurchasesAlbums(const QList<qobuz::AlbumItemData> &list_album, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_myPurchasesTracks(const QList<qobuz::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);


        // Specific
        void completeReq_album(const qobuz::AlbumItemData &data_album);
        void completeReq_playlist(const qobuz::PlaylistItemData &data_playlist);
        void completeReq_artist(const qobuz::ArtistItemData &data_artist);
        void completeReq_artist_bioText(const QString& bioText);

        void completeReq_listAll_myFavoritesIds(const QJsonObject &jsonObj);            ///<  setResult_listAll_myFavoritesIds 호출에 대한 결과값을 받을 수 있는 signal

        void completeReq_drawnext(const int idx);


    public :

        explicit ProcCommon(QWidget *parent = nullptr);
        ~ProcCommon();
        static QString convertImagePath(const QString qobuzImageKey, const int imageSize);

        // 로그인 정보 처리 (SQLlite3)
        void clearLoginInfo_qobuzDB();
        void saveLoginInfo_qobuzDB(UserLoginInfo &userLoginInfo);
        UserLoginInfo getLoginInfo_qobuzDB();

        void clearTablesInfo_qobuzDB();

        // HTTP 요청 - 공통으로 사용하는 타이달 API
        void request_qobuzLogin(UserLoginInfo userLoginInfo);
        void request_qobuzLogin_termCall(UserLoginInfo userLoginInfo);//c230422
        void request_qobuzLogout();

        void request_qobuz_getListGenres();
        void request_qobuz_getListMoods();

        //void request_qobuz_getList_playlists(const QString path, const int limit, const int offset);
        void request_qobuz_getList_playlists(const QString path, const QString type, const QString genre_ids, const int limit, const int offset);
        //void request_qobuz_getList_albums(const QString path, const int limit, const int offset);
        void request_qobuz_getList_albums(const QString path, const QString type, const QString genre_ids, const int limit, const int offset);
        void request_qobuz_getList_lastrelease(const QString path, const QString extra, const int artist_id, const int limit, const int offset);
        void request_qobuz_getList_artists(const QString path, const int artist_id, const int limit, const int offset);
        void request_qobuz_getList_tracks(const QString path, const int limit, const int offset);
        void request_qobuz_getList_track(const QString path, const int track_id, const int limit, const int offset);
        void request_qobuz_getList_appearson(const QString path, const QString extra, const int artist_id, const int limit, const int offset);
        void request_qobuz_getList_videos(const QString path, const int limit, const int offset);

        void request_qobuz_getList_items_of_playlist(const QString playlist_id, const int limit, const int offset);
        //void request_qobuz_getList_items_of_playlist(const int playlist_id, const int limit, const int offset);
        //void request_qobuz_getList_items_of_album(const int album_id);
        void request_qobuz_getList_items_of_album(const QString album_id);   // Added diskept 27/12/2020

        //void request_qobuz_get_album(const int album_id);
        void request_qobuz_get_album(const QString album_id);                // Added diskept 27/12/2020
        void request_qobuz_get_albuminfo(const QString album_id);
        void request_qobuz_get_artist(const int artist_id);
        void request_qobuz_get_artist_bioText(const int artist_id);



        // User's Playlist 관련
        void request_qobuz_getList_myCreatedPlaylists(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset, const QJsonObject& jsonObj_opt=QJsonObject());
        void request_qobuz_delete_myCreatedPlaylist(const QString playlist_id, const QJsonObject& jsonObj_opt);
        //void request_qobuz_delete_myCreatedPlaylist(const int playlist_id, const QJsonObject& jsonObj_opt);
        void request_qobuz_add_track_in_myPlaylist(const QString playlist_uuid_target, const int track_id);
        void request_qobuz_add_album_in_myPlaylist(const QString playlist_uuid_target, const int album_id);
        void request_qobuz_add_playlist_in_myPlaylist(const QString playlist_uuid_target, const QString playlist_uuid);

        void request_qobuz_delete_items_in_myPlaylist(const QString playlist_id_target, const QList<QString>& list_itemIds);
        //void request_qobuz_delete_items_in_myPlaylist(const int playlist_id_target, const QList<QString>& list_itemIds);
        void request_qobuz_create_myPlaylist(const QString playlislt_name, const QString description);
        void request_qobuz_addTracks_myPlaylist(const QString playlist_id, const QString track_id);


        // about my favorite collection
        void request_qobuz_getList_myFavoritePlaylists(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset);
        void request_qobuz_getList_myFavoriteAlbums(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset);
        void request_qobuz_getList_myFavoriteTracks(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset);
        void request_qobuz_getList_myFavoriteArtists(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset);
        void request_qobuz_getList_myFavoriteVideos(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset);


        // about my purchases collection
        void request_qobuz_getList_myPurchasesAlbums(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset);
        void request_qobuz_getList_myPurchasesTracks(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset);


        // Qobuz 회원의 즐겨찾기 정보 반환 (좋아요 여부를 확인하기 위함)
        void request_qobuz_getAll_favorites();
        void request_qobuz_get_favorite(const QString type, const QString id);
        void request_qobuz_set_favorite(const QString type, const QString id, const bool flag);

        // about adding Favorite - 일부러 종류별로 놔둠.
        //void request_qobuz_addFavorite_album(const int album_id);
        void request_qobuz_addFavorite_album(const QString album_id);       // Added diskept 27/12/2020
        void request_qobuz_addFavorite_artist(const int artist_id);
        void request_qobuz_addFavorite_track(const int track_id);
        void request_qobuz_addFavorite_playlist(const QString playlist_id);
        //void request_qobuz_addFavorite_playlist(const int playlist_id);
        void request_qobuz_addFavorite_video(const int video_id);

        // about deleting Favorite
        //void request_qobuz_deleteFavorite_album(const int album_id);
        void request_qobuz_deleteFavorite_album(const QString album_id);       // Added diskept 27/12/2020
        void request_qobuz_deleteFavorite_artist(const int artist_id);
        void request_qobuz_deleteFavorite_track(const int track_id);
        void request_qobuz_deleteFavorite_playlist(const QString playlist_id);
        //void request_qobuz_deleteFavorite_playlist(const int playlist_id);
        void request_qobuz_deleteFavorite_video(const int video_id);

        // search
        void request_qobuz_searchAllItems(const QString search_word, const int limit, const int offset, const QobuzSearch_ContentType contentType);


        // Data 관련 반환 - Genres, Moods
        static int getCount_listGenreInfo();
        static int getCount_listMoodsList();
        static qobuz::GenreInfo getGenreInfo(int idx);
        static qobuz::MoodInfo getMoodInfo(int idx);

        void request_qobuz_drawNext(const int idx);


    private:

        Linker *linker;//c230421

        void procQueryList(const QList<QString> &list_query);
//        QStringList getList_artists(const QJsonArray& jsonArr_artist);


        // HTTP 응답처리 함수
        void setResult_loginQobuz(const QJsonObject &p_jsonObj);
        void setResult_loginQobuz_termCall(const QJsonObject &p_jsonObj);//c230422
        void setResult_logoutQobuz(const QJsonObject &p_jsonObj);
        void setResult_listGenres(const QJsonObject &p_jsonObj);
        void setResult_listMoods(const QJsonObject &p_jsonObj);


        // more data에 대한 last page 체크
        bool get_flag_lastPage(const QJsonObject& p_jsonObj);

        void setResult_list_playlists(const QJsonObject &p_jsonObj);
        void setResult_list_albums(const QJsonObject &p_jsonObj);
        void setResult_list_artists(const QJsonObject &p_jsonObj);
        void setResult_list_tracks(const QJsonObject &p_jsonObj);
        void setResult_list_appearson(const QJsonObject &p_jsonObj);
        void setResult_list_lastrelease(const QJsonObject &p_jsonObj);
        void setResult_list_videos(const QJsonObject &p_jsonObj);

        void setResult_list_itmes_of_playlist(const QJsonObject &p_jsonObj);
        void setResult_list_itmes_of_album(const QJsonObject &p_jsonObj);

        void setResult_album(const QJsonObject &p_jsonObj);
        void setResult_artist(const QJsonObject &p_jsonObj);
        void setResult_artist_bioText(const QJsonObject &p_jsonObj);



        // about user's created playlist
        void setResult_list_myCreatedPlaylists(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt);
        void setResult_delete_myCreatedplaylist(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt);
        void setResult_add_items_in_myPlaylist(const QJsonObject &p_jsonObj);

        void request_qobuz_addPlaylist_items(QString playlist_uuid, QString itemIds);

        void request_qobuz_getList_trackIds_of_playlist(const QString playlist_uuid, const QJsonObject& jsonObj_opt);
        void setResult_list_trackId_of_playlist(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt);

        void request_qobuz_getList_trackIds_of_album(const int album_id, const QJsonObject& jsonObj_opt);
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
        void request_qobuz_addFavorie(const HttpRequestType requestType, const QString id_key, const QVariant id_val, const QString subPath);
        void request_qobuz_deleteFavorie(const HttpRequestType requestType, const QVariant id_val, const QString subPath);


        // about my purchases collection
        void setResult_list_myPurchasesAlbums(const QJsonObject &p_jsonObj);
        void setResult_list_myPurchasesTracks(const QJsonObject &p_jsonObj);


        // search
        void setResult_search_getList_mostPopular(const QJsonObject &p_jsonObj);
        void setResult_search_getList_track(const QJsonObject &p_jsonObj);
        void setResult_search_getList_artist(const QJsonObject &p_jsonObj);
        void setResult_search_getList_album(const QJsonObject &p_jsonObj);
        void setResult_search_getList_playlist(const QJsonObject &p_jsonObj);
        void setResult_search_getList_video(const QJsonObject &p_jsonObj);



        UserLoginInfo userLoginInfo;                    ///< 타이달 로그인을 위한 정보저장


    private slots:
        void slot_responseHttp(int p_id, QJsonObject p_jsonObj);

        void slot_getDone_trackIds_toAddPlaylist(const QString playlist_uuid_target, const QString track_ids);

    };

};

#endif // PROCCOMMON_H
