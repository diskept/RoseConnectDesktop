#ifndef TIDAL_STRUCT_H
#define TIDAL_STRUCT_H

#include <QJsonObject>
#include <QString>
#include <list>
#include <QStringList>
#include <QVariantList>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

//    const int ITEM_LAYOUT_HEIGHT_PLAYLIST = 300;
//    const int ITEM_LAYOUT_HEIGHT_ALBUM = 280;

    const int IMAGE_SIZE_OF_TRACK = 60;

    const int HTTP_GET_SUGGESTED_NEW_TRACK = 1003;
    const int HTTP_GET_SUGGESTED_NEW_ALBUM = 1004;

    const QString KEY_DATA = "data";
    const QString KEY_SUBPATH_TIDAL_API = "path_api";

    //const QString TEXT_VIEW_ALL = "View All";

    /**
     * @brief 타이달 사용자 로그인을 위한 정보
     */
    struct UserLoginInfo {
        //QString username = "";           ///< 사용자 이메일
        //QString password = "";           ///< 사용자 비밀번호

        QString access_token = "";           ///< The access token
        QString refresh_token = "";          ///< The refresh token (optional)
        QString token_type = "";             ///< Will be Bearer
        int expires_in = 0;                  ///< Tidal Expiration time, in senconds 86400
        QString email = "";
        bool flagSavedLoginInfo = false;    ///< 로그인 자동저장 flag. 기본값은 false
    };

    /**
     * @brief The TidalTokenInfo struct
     * @details Tidal API 반환값 참고 @n
     */
    struct TidalTokenInfo{
        QString access_token = "";           ///< The access token
        QString refresh_token = "";          ///< The refresh token (optional)
        QString token_type = "";             ///< Will be Bearer
        int expires_in = 0;                  ///< Tidal Expiration time, in senconds 86400
        QString email = "";
        QString country_code = "";
        QString session_id = "";
        QString user_id = "";
    };


    struct RoseSessionInfo_forTidal {
        QString TIDAL_AccessToken = "";
        QString TIDAL_RefreshToken = "";
        QString TIDAL_CountryCode = "US";
        int TIDAL_HighestSoundQuality = 0;
        QString TIDAL_SessionId = "";
        int TIDAL_SoundQuality = 0;
        int TIDAL_UserID = 0;
        QString TIDAL_UserName = "";
    };



    /**
     * @brief The ItemPositionData struct
     */
    struct ItemPositionData {
        int section = 0;
        int index = 0;
        QString data_id = "";
    };


    /**
     * @brief 타이달의 playlist 아이템
     */
    struct PlaylistItemData {
        QString image = "";              ///< TIDAL image 데이터ID 형식       image, squareImage
        QString title = "";              ///< Playlist의 title
        QString uuid = "";               ///< Playlist PK 인것 같다.
        QString creatorName = "";        ///< Playlist creator
        QString description = "";

        QString audioQuality = "";              //cheontidalhires

        int duration = 0;
        int numberOfTracks = 0;         ///< 트랙 수
        int totalCount = 0;             ///< Playlist count
        bool flagCreatedByUser = false;         ///< 사용자가 만든 playlist 인지의 여부
        int popularity = 0; //cheon210603
        QString lastUpdated = "";//cheon210603

        bool flagMixes = false;
    };


    /**
     * @brief 타이달의 Track 아이템
     */
    struct TrackItemData {
        QString image = "";
        QString title = "";
        QString version = "";

        QString audioQuality = "";              //cheontidalhires
        QString quality = "";                   // (ex) MP4_1080P -> visual album
        QString type = "";                      // (ex) Music Video

        QString streamStartDate = "";           //cheontidalhires

        int id = 0;
        int duration = 0;
        int trackNumber = 0;

        bool streamReady = false;

        int totalCount = 0;                     ///< track count

        bool visual_type = false;

        QString playlist_id = "";

        int album_id = 0;                       // for goint to album detail view
        QString albumName = "";

        QList<int> list_artist_id;              // Artist 연결을 위해
        QList<QString> list_artist_name;        // Artist 연결을 위해

        bool adult_certification = false;
        bool allowStreaming = false;

    };


    /**
     * @brief 타이달의 Album 아이템
     */
    struct AlbumItemData {

        QString audioQuality = "";              //cheontidalhires
        QString streamStartDate = "";           //cheontidalhires
        QString releaseDate = "";

        QString image = "";
        QString title = "";
        QString version = "";

        int id = 0;

        int duration = 0;                       //cheontidalhires
        int popularity = 0;                     //cheontidalhires
        int numberOfTracks = 0;                 ///< 트랙 수
        int totalCount = 0;                     ///< Album count

        bool visualAlbum = false;               // Visual Album
        bool isVideo = false;                   // Visual Album
        bool streamReady = false;

        QList<int> list_artist_id;              // Artist 연결을 위해
        QList<QString> list_artist_name;        // Artist 연결을 위해
    };


    /**
     * @brief 타이달의 Artist 아이템
     */
    struct ArtistItemData {
        QString image = "";
        QString name = "";
        QString biography = "";

        int id = 0;
        int albums_count = 0;
        int artist_total_cnt = 0;
    };

    /**
     * @brief 타이달의 Video 아이템
     */
    struct VideoItemData {
        QString image = "";                     // imageId  (ex) 59ad30bc-d5c5-4f72-adcc-253150c1d96d
        QString title = "";
        int id = 0;
        int duration = 0;
        QString quality = "";                   // (ex) MP4_1080P
        QString type = "";                      // (ex) Music Video

        int totalCount = 0;                     ///< Video count

        QList<int> list_artist_id;              // 차후 연결을 할 것 같아서.
        QList<QString> list_artist_name;        // 차후 연결을 할 것 같아서.

        QJsonObject jsonObj_toPlay;             ///< 재생을 위한 JsonObj (Rose에 전달해야하는 값)
    };


    /**
     * @brief 장르에 대한 정보
     */
    struct GenreInfo {
        QString name = "";           ///< 장르명
        QString path = "";           ///< 장르 데이터 요청을 위한 Key
    };


    /**
     * @brief Mode & Activity에 대한 정보
     */
    struct MoodInfo {
        QString name = "";           ///< 무드명
        QString path = "";           ///< 무드 데이터 요청을 위한 Key
    };


    /**
     * @brief 트랙 전체보기 페이지에 대한 페이지 Info 저장
     * @details All View struct를 별도로 구성함. 혹시 몰라서.
     */
    struct PageInfo_TrackAllView {
        QString api_subPath = "";
        QString pathTitle = "";
    };

    /**
     * @brief Album 전체보기 페이지에 대한 페이지 Information 저장
     * @details All View struct를 별도로 구성함. 혹시 몰라서.
     */
    struct PageInfo_AlbumAllView {
        QString api_subPath = "";
        QString pathTitle = "";
    };

    /**
     * @brief Playlist 전체보기 페이지에 대한 페이지 Information 저장
     * @details All View struct를 별도로 구성함. 혹시 몰라서.
     */
    struct PageInfo_PlaylistAllView {
        QString api_subPath = "";
        QString pathTitle = "";
    };


    /**
     * @brief Artist 전체보기 페이지에 대한 페이지 Information 저장
     * @details All View struct를 별도로 구성함. 혹시 몰라서.
     */
    struct PageInfo_ArtistAllView {
        QString api_subPath = "";
        QString pathTitle = "";
    };


    /**
     * @brief Video 전체보기 페이지에 대한 페이지 Information 저장
     * @details All View struct를 별도로 구성함. 혹시 몰라서.
     */
    struct PageInfo_VideoAllView {
        QString api_subPath = "";
        QString pathTitle = "";
    };



    /**
     * @brief The TidalFeaturedPart enum
     */
    enum TidalFeaturedPart{
        New,
        Top,
        Rising
    };

}

#endif // TIDAL_STRUCT_H
