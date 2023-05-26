#ifndef PROCBUGSAPI_H
#define PROCBUGSAPI_H

#include <QObject>
#include <QWidget>
#include <QJsonObject>
#include "bugs_struct.h"
#include "widget/toastmsg.h"//c220707
#include "bugs/ProcRoseAPI_withBugs.h"//c220707

namespace bugs {

    /**
     * @brief 벅스에서만 사용하는 전용 처리담당 클래스. (다른 파트에서 사용을 해야하는 경우에는 @se ProcBugs_forOuter 클래스 사용할 것)
     */
    class ProcBugsAPI : public QObject
    {
         Q_OBJECT

    public:

        /**
         * @brief Bugs용 Request API
         */
        enum HttpRequestType {
            Get_Token
            , Get_MemberInfo

            , GetList_Genres
            , GetList_Theme             ///< 테마 조회
            , GetList_Tags_inTheme      ///< 테마 내에 태그 리스트 조회

            , GetList_Tracks            ///< track 목록 요청
            , GetList_Albums            ///< album 목록 요청
            , GetList_Videos            ///< video 목록 요청
            , GetList_Artists
            , GetList_PD_Albums
            , GetList_My_Albums

            , GetList_Items_Album           ///< bugs track 목록 요청 (in the general Album)
            , GetList_Items_PD_Album        ///< bugs track 목록 요청 (in the PD's Album)
            , GetList_Items_MyAlbum         ///< bugs track 목록 요청 (in my Album)

            , Get_AlbumInfo             ///< Album 1개 정보 요청
            , Get_PD_AlbumInfo          ///< PD_Album 1개 정보 요청
            , Get_ArtistInfo            ///< Artist 1개 정보 요청
            , Get_VideoInfo             ///< Video 1개 정보 요청

            , Favorite_Add_Album
            , Favorite_Add_PD_Album
            , Favorite_Add_Artist
            , Favorite_Add_Track
            , Favorite_Add_Playlist
            , Favorite_Add_Video

            , Favorite_Delete_Album
            , Favorite_Delete_PD_Album
            , Favorite_Delete_Artist
            , Favorite_Delete_Track
            , Favorite_Delete_Playlist
            , Favorite_Delete_Video

        };
        Q_ENUM(HttpRequestType);


        /// Artist의 Track Sort 옵션
        enum class BugsAritst_TrackSort{
            Popular
            , Recent
            , Word
        };
        Q_ENUM(BugsAritst_TrackSort);

        /// Artist의 Track Filter 옵션
        enum class BugsArtist_TrackFilter{
            All
            , Release
            , Join
        };
        Q_ENUM(BugsArtist_TrackFilter);

        /// Artist의 Album Filter 옵션
        enum class BugsArtist_Albumfilter {
            Release
            , Join
            , Compil
            , JoinCompil
        };
        Q_ENUM(BugsArtist_Albumfilter);

        enum class BugsMyMusic_Sort {
            Recent      ///< 최근 좋아요 순
            , Name      ///< 가나다 순
        };
        Q_ENUM(BugsMyMusic_Sort);

        enum class BugsPDAlbum_Sort {
            Recent          ///< 최신
            , Popular       ///< 인기
        };
        Q_ENUM(BugsPDAlbum_Sort);

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

        enum class BugsSearch_Sort{
            exact               ///< 정확도 순
            , popularity        ///< 인기순
            , recent            ///< 최신순
            , order             ///< 가나다 순
        };
        Q_ENUM(BugsSearch_Sort);

    signals:
        void signal_completeReq_getToken(const bool flagSuccess, const QString errorMsg);
        void signal_completeReq_getMemberInfo(const QString errorMsg, const QJsonObject& jsonObj_member, const QJsonObject& jsonObj_product);

        void completeReq_list_genre_setDone();
        void completeReq_list_theme_setDone();

        void completeReq_list_tags_in_theme(const QList<bugs::BugsTag> &list_tag);


        // General
        void completeReq_list_tracks(const QList<bugs::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_albums(const QList<bugs::AlbumItemData> &list_album, const bool flag_lastPage);
        void completeReq_list_videos(const QList<bugs::VideoItemData> &list_video, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage);
        void completeReq_list_artists(const QList<bugs::ArtistItemData> &list_artist, const bool flag_lastPage);
        void completeReq_list_pd_albums(const QList<bugs::PD_AlbumItemData> &list_pd_album, const bool flag_lastPage);
        void completeReq_list_my_albums(const QList<bugs::MyAlbumItemData> &list_my_album, const bool flag_lastPage);

        void completeReq_list_items_of_album(const QList<bugs::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool);
        void completeReq_list_items_of_pd_album(const QList<bugs::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool);
        void completeReq_list_items_of_my_album(const QList<bugs::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool);


        // Specific
        void completeReq_album(const bugs::AlbumItemData &data_album);
        void completeReq_pd_album(const bugs::PD_AlbumItemData &data_pd_album);
        void completeReq_artist(const bugs::ArtistItemData &data_artist);
        void completeReq_video(const bugs::VideoItemData &data_video, const QJsonObject& jsonObj_dataToPlay);


        // Favorite
        void completeReq_favarite_track(const QJsonObject& jsonObj_opt);
        void failedReq_favorite_track(const QJsonObject& jsonObj_opt);

        void completeReq_favarite_album(const QJsonObject& jsonObj_opt);
        void failedReq_favorite_album(const QJsonObject& jsonObj_opt);

        void completeReq_favarite_pd_album(const QJsonObject& jsonObj_opt);
        void failedReq_favorite_pd_album(const QJsonObject& jsonObj_opt);

        void completeReq_favarite_artist(const QJsonObject& jsonObj_opt);
        void failedReq_favorite_artist(const QJsonObject& jsonObj_opt);

        void completeReq_favarite_video(const QJsonObject& jsonObj_opt);
        void failedReq_favorite_video(const QJsonObject& jsonObj_opt);


    public :
        explicit ProcBugsAPI(QWidget *parent = nullptr);
        ~ProcBugsAPI();

        void clearTablesInfo_bugsDB();
        void saveTokenInfo_bugsDB(BugsTokenInfo&);
        BugsTokenInfo getTokenInfo_bugsDB();

        static QString get_headerForRequest_userAgent();
        static QString makeStr_header_userAgent(bugs::BugsReqHeader& reqHeader);

        // HTTP 요청 - 공통으로 사용하는 타이달 API
        void request_bugs_getTokenAndSave();
        void request_bugs_getMemberInfo();
        void request_logout();


        // 공통 사용 - genre, theme
        void request_bugs_getListGenres();
        void request_bugs_getListThemes();
        void request_bugs_getListTags_in_theme(const QString theme_code);


        void request_bugs_getList_tracks(const QString api_subPath, const int size=10, const int page=1);
        void request_bugs_getList_tracks_of_myMusic(const int size, const int page, const BugsMyMusic_Sort sortOpt);
        void request_bugs_getList_tracks_of_artist(const int artist_id, const int size, const int page, const BugsArtist_TrackFilter filterOpt, const BugsAritst_TrackSort sortOpt);
        void request_bugs_getList_albums_of_artist(const int artist_id, const int size, const int page, const BugsArtist_Albumfilter filterOpt);

        void request_bugs_getList_albums(const QString api_subPath, const int size=10, const int page=1);
        void request_bugs_getList_albums_of_myMusic(const int size, const int page, const BugsMyMusic_Sort sortOpt);

        void request_bugs_getList_artists_of_myMusic(const int size, const int page, const BugsMyMusic_Sort sortOpt);

        void request_bugs_getList_videos(const QString api_subPath, const int size=10, const int page=1);

        // My Album
        void request_bugs_getList_my_albums(const int size=10, const int page=1);

        // PD Album
        void request_bugs_getList_pd_albums(const int size, const int page, const BugsPDAlbum_Sort sortOpt, const QString tag_ids="");
        void request_bugs_getList_pd_albums_of_myMusic(const int size, const int page, const BugsMyMusic_Sort sortOpt);



        void request_bugs_getList_items_of_album(const int album_id);
        void request_bugs_getList_items_of_pd_album(const int es_album_id);
        void request_bugs_getList_items_of_my_album(const int playlist_id, const int page, const int size);

        void request_bugs_get_album(const int album_id);
        void request_bugs_get_pd_album(const int es_album_id);
        void request_bugs_get_artist(const int artist_id);
        void request_bugs_get_video(const int mv_id);


        // about adding Favorite - 일부러 종류별로 놔둠.
        void request_bugs_addFavorite_album(const int album_id, const QJsonObject& jsonObj_opt);
        void request_bugs_addFavorite_pd_album(const int es_album_id, const QJsonObject& jsonObj_opt);
        void request_bugs_addFavorite_artist(const int artist_id, const QJsonObject& jsonObj_opt);
        void request_bugs_addFavorite_track(const int track_id, const QJsonObject& jsonObj_opt);

        // about deleting Favorite
        void request_bugs_deleteFavorite_album(const int album_id, const QJsonObject& jsonObj_opt);
        void request_bugs_deleteFavorite_pd_album(const int es_album_id, const QJsonObject& jsonObj_opt);
        void request_bugs_deleteFavorite_artist(const int artist_id, const QJsonObject& jsonObj_opt);
        void request_bugs_deleteFavorite_track(const int track_id, const QJsonObject& jsonObj_opt);


        // search
        void request_bugs_search_track(const QString search_word, const int page, const int size, const BugsSearch_Sort sortOpt);
        void request_bugs_search_artist(const QString search_word, const int page, const int size, const BugsSearch_Sort sortOpt);
        void request_bugs_search_album(const QString search_word, const int page, const int size, const BugsSearch_Sort sortOpt);
        void request_bugs_search_pd_album(const QString search_word, const int page, const int size, const BugsSearch_Sort sortOpt);
        void request_bugs_search_video(const QString search_word, const int page, const int size, const BugsSearch_Sort sortOpt);


    private:

        Linker *linker;//c230421
        void procQueryList(const QList<QString> &list_query);


        // HTTP 응답처리 함수
        void setResult_getToken(const QJsonObject &p_jsonObj);
        void setResult_getMemberInfo(const QJsonObject &p_jsonObj);

        void setResult_list_genres(const QJsonObject &p_jsonObj);
        void setResult_list_themes(const QJsonObject &p_jsonObj);
        void setResult_list_tags_in_themes(const QJsonObject &p_jsonObj);

        void setResult_list_tracks(const QJsonObject &p_jsonObj);
        void setResult_list_albums(const QJsonObject &p_jsonObj);
        void setResult_list_videos(const QJsonObject &p_jsonObj);
        void setResult_list_artists(const QJsonObject &p_jsonObj);
        void setResult_list_pd_albums(const QJsonObject &p_jsonObj);
        void setResult_list_my_album(const QJsonObject &p_jsonObj);

        void setResult_list_itmes_of_album(const QJsonObject &p_jsonObj);
        void setResult_list_itmes_of_pd_album(const QJsonObject &p_jsonObj);
        void setResult_list_itmes_of_my_album(const QJsonObject &p_jsonObj);

        void setResult_album(const QJsonObject &p_jsonObj);
        void setResult_pd_album(const QJsonObject &p_jsonObj);
        void setResult_artist(const QJsonObject &p_jsonObj);
        void setResult_video(const QJsonObject &p_jsonObj);


        // HTTP 응답처리 함수 about Favorite (add, delete --> response가 같음. 같은 함수 사용)
        void setResult_favorite_track(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt);
        void setResult_favorite_album(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt);
        void setResult_favorite_pd_album(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt);
        void setResult_favorite_artist(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt);
        void setResult_favorite_video(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt);


        // about adding/deleting favorite (common type)
        void request_bugs_addFavorie(const HttpRequestType requestType, const QVariant id_val, const QString api_subPath, const QJsonObject& jsonObj_opt);
        void request_bugs_deleteFavorie(const HttpRequestType requestType, const QVariant id_val, const QString api_subPath, const QJsonObject& jsonObj_opt);


        void request_bugs_search(const QString search_word, const int page, const int size, const QString sortOpt, const QString api_subPath, const HttpRequestType httpReqtype);


    private slots:
        void slot_responseHttp(int p_id, QJsonObject p_jsonObj);

    };

};

#endif // PROCBUGSAPI_H
