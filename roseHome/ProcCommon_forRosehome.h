#ifndef PROCCOMMON_FORROSEHOME_H
#define PROCCOMMON_FORROSEHOME_H

#include "roseHome/rosehome_struct.h"

//#include <QObject>
//#include <QJsonObject>


namespace roseHome {

    /**
     * @brief Rosehome에서 사용하는 전용 처리담당 클래스.
     */
    class ProcCommon : public QObject
    {
        Q_OBJECT

    public:
        /**
         * @brief Rose Sever용 Request - about Rose API
         */
        enum HttpRequestType {
            GetList_Recommend = 10              ///< RoseHome의 Recommend 목록 요청

            , GetList_Recently_Albums           ///< RoseHome의 Recently Album 목록 요청
            , GetList_Recently_Playlist         ///< RoseHome의 Recently Playlist 목록 요청
            , GetList_Recently_Track            ///< RoseHome의 Recently Track 목록 요청
            , GetList_Recently_Rosetube         ///< RoseHome의 Recently Rosetube 목록 요청

            , GetList_Favorite_Albums           ///< RoseHome의 Favortie Album 목록 요청
            , GetList_Favorite_Playlist         ///< RoseHome의 Favortie Playlist 목록 요청
            , GetList_Favorite_Track            ///< RoseHome의 Favortie Track 목록 요청

            , GetList_My_Playlist               ///< RoseHome의 My Playlist 목록 요청
            , GetList_Users_Playlist            ///< RoseHome의 New, Pop Playlist 목록 요청

            , GetList_Items_Playlist            ///< RoseHome의 playlist의 track 목록 요청
            , GetList_Items_Album               ///< RoseHome의 album의 track 목록 요청
            , GetList_Items_Track               ///< RoseHome의 Track의 track 목록 요청
            , GetList_Items_Rosetube            ///< Share link의 track 목록 요청

            , Favorite_Check_Rating_Album
            , Favorite_Check_Rating_Playlist
            , Favorite_Check_Rating_Track

            , Favorite_Creat_Rating_Album
            , Favorite_Creat_Rating_Playlist
            , Favorite_Creat_Rating_Track

            , Favorite_Get_Rating_Album
            , Favorite_Get_Rating_Playlist
            , Favorite_Get_Rating_Track
            , Favorite_Get_Rating_Artist
            , Thumbup_get_Rating_Playlist

            , Favorite_Add_Rating_Album
            , Favorite_Add_Rating_Playlist
            , Favorite_Add_Rating_Track
            , Favorite_Add_Rating_Artist
            , Thumbup_Add_Rating_Playlist

            , Favorite_Delete_Rating_Album
            , Favorite_Delete_Rating_Playlist
            , Favorite_Delete_Rating_Track
            , Favorite_Delete_Rating_Artist
            , Thumbup_Delete_Rating_Playlist

            , Create_Playlist_Rose
            , Delete_Playlist_Rose
            , AddTracks_Playlist_Rose
            , DeleteTracks_Playlist_Rose

            , GetList_FeaturedChannel           ///< RoseRadio의 Featured 목록 요청

            , GetList_Recently_Radio            ///< RoseRadio의 Recently Channel 목록 요청
            , GetList_Favorite_Radio            ///< RoseRadio의 Favortie channel 목록 요청

            , GetList_Item_LocalPopular         ///< RoseRadio의 Local Popular channel 목록 요청
            , GetList_Item_Local                ///< RoseRadio의 Local channel 목록 요청
            , GetList_Items_Genre
            , GetList_Items_Language
            , GetList_Items_Search

            , Getlist_Radio_Genre
            , GetList_Radio_Country
            , GetList_Radio_Region
            , GetList_Radio_Language

            , Favorite_Add_Radio

            , Get_Share_link
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

    signals:
        // recommend, album, playlist, tarck
        void completeReq_list_recommeds(const QList<roseHome::RecommendsItemData>&, const QJsonArray&, const bool);
        void completeReq_list_playlists(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void completeReq_list_albums(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool);
        void completeReq_list_tracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);
        void completeReq_list_rosetubes(const QJsonArray&, const int&, const bool);

        void completeReq_albumlist(const roseHome::AlbumItemData&);
        void completeReq_playlist(const roseHome::PlaylistItemData&);

        void completeReq_list_myplaylists(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void completeReq_list_usersplaylists(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

        // third part & rose star check
        void completeCheck_rating_playlist(const QJsonObject&);
        void completeCheck_rating_album(const QJsonObject&);
        void completeCheck_rating_track(const QJsonObject&);

        void completeReq_rating_playlistRose(const QJsonArray&);
        void completeReq_rating_playlist(const QJsonArray&);
        void completeReq_rating_album(const QJsonArray&);
        void completeReq_rating_track(const QJsonArray&);
        void completeReq_rating_thumbup(const QJsonObject&);

        // my playlist
        void completeReq_create_playlist(const QJsonObject&);
        void completeReq_addTrack_playlist(const QJsonObject&);

        // rose radio
        void completeReq_list_roseRadio(const QJsonArray&, const int, const bool);

        void completeReq_favorite_radio(const QJsonObject&);

        // rose share link
        void completeReq_share_link(const QString&);


    public:
        explicit ProcCommon(QWidget *parent = nullptr);
        ~ProcCommon();

        // recommend, album, playlist, tarck
        void request_rose_getList_recommends(const QString path, const int page, const int pageSize);
        void request_rose_getList_recentlyAlbums(const QString path, const QString mediaType, const int page, const int pageSize);
        void request_rose_getList_recentlyPlaylists(const QString path, const QString mediaType, const int page, const int pageSize);
        void request_rose_getList_recentlyTracks(const QString path, const QString mediaType, const int page, const int pageSize);
        void request_rose_getList_recentlyRosetube(const QString path, const QString mediaType, const int page, const int pageSize);

        void request_rose_getList_myPlaylists(const QString path, const QString mediaType, const int page, const int pageSize);
        void request_rose_getList_usersPlaylists(const QString path, const QString sortType, const QString mediaType, const int page, const int pageSize);

        void request_rose_getList_favoriteAlbums(const QString sortType, const QString mediaType, const int page, const int pageSize);
        void request_rose_getList_favoritePlaylists(const QString sortType, const QString mediaType, const int page, const int pageSize);
        void request_rose_getList_favoriteTracks(const QString sortType, const QString mediaType, const int page, const int pageSize);

        void request_rose_get_album(const int album_id, const int page, const int pageSize);
        void request_rose_get_playlist(const int playlist_id, const int page, const int pageSize);
        void request_rose_get_track(const int track_id, const int page, const int pageSize);
        void request_rose_get_rosetube(const QString track_id, const int page, const int pageSize);

        // third part & rose star check
        void request_rose_checkRating_Playlist(const QString mediaType, const QString clientKey);
        void request_rose_checkRating_Album(const QString mediaType, const QString clientKey);
        void request_rose_checkRating_Track(const QString mediaType, const QString clientKey);
        void request_rose_checkRating_Track(const QString mediaType, const QJsonArray&);

        void request_rose_addRating_Playlist(const QJsonObject jsonObj);
        void request_rose_addRating_Album(const QJsonObject jsonObj);
        void request_rose_addRating_Track(const QJsonObject jsonObj);

        void request_rose_getRating_Playlist(const QString mediaType, const QString clientKey);
        void request_rose_getRating_Album(const QString mediaType, const QString clientKey);
        void request_rose_getRating_Track(const QString mediaType, const QList<QString> clientkey);
        void request_rose_getRating_Track(const QString mediaType, const QJsonObject&);
        void request_rose_getRating_Thumbup(const QString mediaType, const QString clientkey);

        void request_rose_setRating_Playlist(const roseHome::PlaylistItemData listDataInfo, const bool favorite, const int star);
        void request_rose_setRating_rosePlaylist(const QJsonObject jsonObj, const bool favorite, const int star);
        void request_rose_setRating_Album(const QJsonObject jsonObj, const bool favorite, const int star);
        void request_rose_setRating_Track(const QJsonObject jsonObj, const bool favorite, const int star);
        void request_rose_setRating_Thumbup(const QJsonObject jsonObj, const bool thumbup);

        // my playlist
        void request_rose_create_myPlaylist(const QJsonObject jsonObj);
        void request_rose_addTrack_myPlaylist(const QJsonObject jsonObj);

        // rose radio
        void request_rose_getList_radioChannel(const HttpRequestType type, const QString path, const int regionId, const QString sort, const int page, const int size);
        void request_rose_getList_radioSearch(const int regionId, const int genreId, const int languageId, const int page, const int size, const QString sort, const QString title);

        void request_rose_getList_radioGenre(const int page, const int size);
        void request_rose_getList_genreChannel(const QString path, const int id, const int page, const int size, const QString sort);
        void request_rose_getList_languageChannel(const QString path, const int id, const int page, const int size, const QString sort);
        void request_rose_getList_radioCountry(const int page, const int size);
        void request_rose_getList_radioLanguage(const int page, const int size);

        void request_rose_setFavorite_Radio(const QJsonObject channel);

        // rose share link
        void request_rose_get_shareLink(const QString thumbnail, QString description, QString title, QString param);

    private slots:
        void slot_responseHttp(int p_id, QJsonObject p_jsonObj);

    private:
        // more data에 대한 last page 체크
        bool get_flag_lastPage(const QJsonObject&);

        // recommend, album, playlist, tarck
        void setResult_list_recommends(const QJsonObject&);
        void setResult_list_recentlyAlbums(const QJsonObject&);
        void setResult_list_recentlyPlaylists(const QJsonObject&);
        void setResult_list_recentlyTracks(const QJsonObject&);
        void setResult_list_recentlyRoseTube(const QJsonObject&);

        void setResult_list_myPlaylists(const QJsonObject&);
        void setResult_list_usersPlaylists(const QJsonObject&);

        void setResult_list_favoriteAlbums(const QJsonObject&);
        void setResult_list_favoritePlaylists(const QJsonObject&);
        void setResult_list_favoriteTracks(const QJsonObject&);

        void setResult_album(const QJsonObject&);
        void setResult_playlist(const QJsonObject&);
        void setResult_track(const QJsonObject&);
        void setResult_rosetube(const QJsonObject&);

        // third part & rose star check
        void setResult_Favorite_checkRating_Playlist(const QJsonObject&);
        void setResult_Favorite_checkRating_Album(const QJsonObject&);
        void setResult_Favorite_checkRating_Track(const QJsonObject&);

        void setResult_Favorite_getRating_Playlist(const QJsonObject&);
        void setResult_Favorite_getRating_Album(const QJsonObject&);
        void setResult_Favorite_getRating_Track(const QJsonObject&);
        void setResult_Thumbup_getRating_Playlist(const QJsonObject&);

        void setResult_Favorite_setRating_Playlist(const QJsonObject&);
        void setResult_Favorite_setRating_Album(const QJsonObject&);
        void setResult_Favorite_setRating_Track(const QJsonObject&);
        void setResult_Thumbup_setRating_Playlist(const QJsonObject&);

        // my playlist
        void setResult_Create_myPlaylist(const QJsonObject&);
        void setResult_AddTrack_myPlaylist(const QJsonObject&);

        // rose radio
        void setResult_list_radioChannel(const QJsonObject&);
        void setResult_list_radioGenre(const QJsonObject&);
        void setResult_list_radioCountry(const QJsonObject&);
        void setResult_list_radioLanguage(const QJsonObject&);

        void setResult_Favorite_set_Radio(const QJsonObject&);

        // rose share link
        void setResult_share_link(const QJsonObject&);
    };
};

#endif // PROCCOMMON_FORROSEHOME_H
