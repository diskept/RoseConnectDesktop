#ifndef QOBUZ_STRUCT_H
#define QOBUZ_STRUCT_H

#include <QJsonObject>
#include <QString>
#include <list>
#include <QStringList>
#include <QVariantList>


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

//    const int ITEM_LAYOUT_HEIGHT_PLAYLIST = 300;
//    const int ITEM_LAYOUT_HEIGHT_ALBUM = 280;

    const int IMAGE_SIZE_OF_TRACK = 60;

    const int HTTP_GET_SUGGESTED_NEW_TRACK = 1003;
    const int HTTP_GET_SUGGESTED_NEW_ALBUM = 1004;

    const QString KEY_DATA = "data";
    const QString KEY_SUBPATH_QOBUZ_API = "path_api";

    //const QString TEXT_VIEW_ALL = "View All";

    /**
     * @brief Qobuz 사용자 로그인을 위한 정보
     */
    struct UserLoginInfo {
        QString username = "";              ///< 사용자 이메일
        QString password = "";              ///< 사용자 비밀번호
        QString user_id = "";
        QString auth_token = "";            ///< 로그인 인증토큰
        bool flagSavedLoginInfo = false;    ///< 로그인 자동저장 flag. 기본값은 false
    };


    struct RoseSessionInfo_forQobuz {
        QString description = "";
        QString c_id = "";
        QString label = "";
        bool hfp_purchases = false;
        bool hires_purchases = false;
        bool hires_streaming = false;
        bool lossless_streaming = false;
        bool lossy_streaming = false;
        bool mobile_streaming = false;
        bool offline_streaming = false;
        QString device_man_id = "";
        QString device_id = "";
        QString email = "";
        QString user_id = "";
        QString user_auth_token = "";
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
     * @brief Qobuz의 playlist 아이템
     */
    struct PlaylistItemData {
        //QString image = "";              ///< QOBUZ image 데이터ID 형식       image, RectangleImage
        //QString title = "";              ///< Playlist의 title
        //QString uuid = "";               ///< Playlist PK 인것 같다.
        //QString creatorName = "";        ///< Playlist creator
        //QString description = "";
        //int numberOfTracks = 0;         ///< 트랙 수
        //bool flagCreatedByUser = false;         ///< 사용자가 만든 playlist 인지의 여부

        QString image = "";                 ///< QOBUZ image 데이터ID 형식       image, RectangleImage
        QString title = "";                 ///< Playlist의 title
        QString id = "";                     ///< Playlist PK 인것 같다.

        QString release_date_stream = "";   //cheontidalhires
        QString lastUpdated = "";           //cheon210603
        int popularity = 0;                 //cheon210603
        int duration = 0;                   //cheontidalhires

        QString description = "";
        int numberOfTracks = 0;             ///< 트랙 수
        bool hires = false;
        bool hires_streamable = false;

        QString ownerName = "";             ///< Playlist creator
        int tracks_count = 0;               ///< 트랙 수
        int album_total_cnt = 0;
        bool flagCreatedByUser = false;     ///< 사용자가 만든 playlist 인지의 여부
    };


    /**
     * @brief 타이달의 Track 아이템
     */
    struct TrackItemData {
        //QString image = "";
        //QString title = "";
        //QString albumName = "";
        //int duration = 0;
        //int id = 0;
        //QString audioQuality = "";

        //int album_id = 0;                       // for goint to album detail view
        //QList<int> list_artist_id;              // Artist 연결을 위해
        //QList<QString> list_artist_name;        // Artist 연결을 위해

        // album info
        int appearson_count = 0;
        int album_duration = 0;
        bool album_hires = false;
        bool album_hires_streamable = false;
        QString album_id = "";
        QString album_image_large = "";
        QString album_image_small = "";
        QString album_image_thumbnail = "";
        bool album_streamable = false;
        QString album_title = "";
        QString album_version = "";
        int album_tracks_count = 0;

        int composer_id = 0;
        QString composer_name = "";

        QString copyright = "";
        int duration = 0;
        bool hires = false;
        bool hires_streamable = false;
        int id = 0;
        int maximum_bit_depth = 0;
        double maximum_sampling_rate = 0.0;
        int media_number = 0;
        int performer_id = 0;
        QString performer_name = "";
        QString performers = "";
        bool previewable = false;
        bool purchasable = false;
        int qobuz_id = 0;
        bool sampleable = false;
        bool streamable = false;
        QString title = "";
        QString version = "";
        QString work = "";//cheon210812-work
        int track_number = 0;

        // artist info
        QList<int> list_artist_albums_count;
        QList<int> list_artist_id;              // Artist 연결을 위해
        QList<QString> list_artist_name;        // Artist 연결을 위해

        bool parental_warning = false;
    };


    /**
     * @brief 타이달의 Album 아이템
     */
    struct AlbumItemData {
        //QString image = "";
        //QString title = "";
        //int id = 0;        
        //int numberOfTracks = 0;         ///< 트랙 수

        // Added diskept 28/12/2020 redefine struct
        QString id = "";                        // Qobuz의 Album id는 String Type     Added diskept 27/12/2020

        int artist_albums_count = 0;
        int artist_id = 0;
        QString artist_name = "";
        QString artist_image = "";
        QString artist_picture = "";

        int duration = 0;                       //cheontidalhires

        bool hires = false;
        bool hires_streamable = false;

        QString image_large = "";
        QString image_small = "";
        QString image_thumbnail = "";

        int maximum_bit_depth = 0;                 //cheon210717-hires
        int maximum_channel_count = 0;             //cheon210717-hires
        double maximum_sampling_rate = 0.0;        //cheon210717-hires

        QString release_date_original = "";
        QString release_date_stream = "";       //cheontidalhires

        bool streamable = false;

        QString title = "";
        QString version = "";
        int tracks_count = 0;

        QString resolution = "";
        QString description = "";               //cheontidalhires
        int popularity = 0;                     //cheontidalhires

        int numberOfTracks = 0;                 //cheon211110604

        int album_total_cnt = 0;

        int label_albums_count = 0;
        int label_id = 0;
        QString label_name = "";

        int composer_id = 0;                    // Composer 연결을 위해
        QString composer_name = "";             // Composer 연결을 위해

        QString genre_name = "";

        QList<int> list_artist_id;              // Artist 연결을 위해
        QList<QString> list_artist_name;        // Artist 연결을 위해
    };


    /**
     * @brief Qobuz의 Artist 아이템
     * Added diskept 28/12/2020
     */
    struct ArtistItemData {
        //QString image = "";
        QString image_large = "";
        QString image_medium = "";
        QString image_small = "";
        QString name = "";
        QString biography = "";

        int id = 0;
        int albums_count = 0;
        int artist_total_cnt = 0;
    };

    /**
     * @brief 타이달의 Track 아이템
     */
    struct TrackItemData_apple {                //c220304
        //QString image = "";
        //QString title = "";
        //QString albumName = "";
        //int duration = 0;
        //int id = 0;
        //QString audioQuality = "";

        //int album_id = 0;                       // for goint to album detail view
        //QList<int> list_artist_id;              // Artist 연결을 위해
        //QList<QString> list_artist_name;        // Artist 연결을 위해

        // album info
        int appearson_count = 0;
        int album_duration = 0;
        bool album_hires = false;
        bool album_hires_streamable = false;
        QString album_id = "";
        int image_width = 0,image_height = 0;//c220603
        QString album_image_large = "";
        QString album_image_small = "";
        QString album_image_thumbnail = "";
        bool album_streamable = false;
        QString album_title = "";
        QString album_version = "";
        int album_tracks_count = 0;

        int composer_id = 0;
        QString composer_name = "";

        QString copyright = "";
        int duration = 0;
        bool hires = false;
        bool hires_streamable = false;
        int id = 0;
        int maximum_bit_depth = 0;
        double maximum_sampling_rate = 0.0;
        int media_number = 0;
        int performer_id = 0;
        QString performer_name = "";
        QString performers = "";
        bool previewable = false;
        bool purchasable = false;
        int qobuz_id = 0;
        bool sampleable = false;
        bool streamable = false;
        QString title = "";
        QString version = "";
        QString work = "";//cheon210812-work
        int track_number = 0;

        // artist info
        QList<int> list_artist_albums_count;
        QList<int> list_artist_id;              // Artist 연결을 위해
        QList<QString> list_artist_name;        // Artist 연결을 위해
    };

    /**
     * @brief 타이달의 Video 아이템
     */
    struct VideoItemData {
        QString image = "";              // imageId  (ex) 59ad30bc-d5c5-4f72-adcc-253150c1d96d
        QString title = "";
        int id = 0;
        int duration = 0;
        QString quality = "";            // (ex) MP4_1080P
        QString type = "";               // (ex) Music Video
        QList<int> list_artist_id;              // 차후 연결을 할 것 같아서.
        QList<QString> list_artist_name;        // 차후 연결을 할 것 같아서.

        QJsonObject jsonObj_toPlay;             ///< 재생을 위한 JsonObj (Rose에 전달해야하는 값)
    };



    /**
     * @brief 장르에 대한 정보
     */
    struct GenreInfo {
        int id = 0;
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
        QString page = "";
        int artist_id = 0;
    };

    /**
     * @brief Album 전체보기 페이지에 대한 페이지 Information 저장
     * @details All View struct를 별도로 구성함. 혹시 몰라서.
     */
    struct PageInfo_AlbumAllView {
        QString api_subPath = "";
        QString pathTitle = "";
        QString type = "";
        QString page = "";
        QString genre_ids = "";
        int genre_id = 0;
        int artist_id = 0;

        QList<qobuz::AlbumItemData> list_sameAritst;
    };

    /**
     * @brief Playlist 전체보기 페이지에 대한 페이지 Information 저장
     * @details All View struct를 별도로 구성함. 혹시 몰라서.
     */
    struct PageInfo_PlaylistAllView {
        QString api_subPath = "";
        QString pathTitle = "";
        QString type = "";
        QString genre_ids = "";
        int genre_id = 0;

        QList<qobuz::PlaylistItemData> list_similarPlaylist;
    };


    /**
     * @brief Artist 전체보기 페이지에 대한 페이지 Information 저장
     * @details All View struct를 별도로 구성함. 혹시 몰라서.
     */
    struct PageInfo_ArtistAllView {
        QString api_subPath = "";
        QString pathTitle = "";
        int artist_id = 0;
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
     * @brief The QobuzFeaturedPart enum
     */
    enum QobuzFeaturedPart{
        New,
        Top,
        Rising
    };


    /**
     * @brief The QobuzFeaturedPart enum
     */
    enum QobuzMyCollectionPart{
        MyPlaylist,
        Favorites,
        MyPunchases,
        Settings
    };

}

#endif // QOBUZ_STRUCT_H
