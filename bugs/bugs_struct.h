#ifndef BUGS_STRUCT_H
#define BUGS_STRUCT_H

#include <QString>
#include "common/global.h"



namespace bugs {

    //const QString TEXT_VIEW_ALL = "View All";

    const int IMAGE_SIZE_OF_TRACK = 60;

    /**
     * @brief The BugsTokenInfo struct
     * @details Bugs API 반환값 참고 @n
     *      {"access_token":"b9950a84-e72c-4a07-b371-8e31b547ad78","expires_in":2591999,
     *          "refresh_token":"c9f7a91f-20a4-44fc-ac4b-3e975b195d85","scope":"read","token_type":"bearer"}
     */
    struct BugsTokenInfo{
        QString access_token = "";           ///< Bugs 서버가 발급해주는 access token - 30일용
        QString refresh_token = "";          ///< Bugs 서버가 발급해주는 refresh_token - 180일용
        int expires_in = 0;                 ///< Bugs에서는 int 로 주지만, Rose 에게 줄 때는 String 으로 교환해야함
        QString token_type = "";
        QString scope = "";
    };


    /**
     * @brief Bugs 서비스 관련한 Rose 와의 Session 정보
     */
    struct RoseSessionInfo_forBugs {
        QString BUGS_ProductDisplayEndDt = "";
        bool BUGS_StreamingYn = false;
        QString BUGS_ExpiresIn = "";
        QString BUGS_ProductName = "";
        QString BUGS_Nickname = "";

        QString BUGS_RefreshToken = "";
        QString BUGS_TokenType = "";
        QString BUGS_AccessToken = "";
        bool BUGS_FlacPremiumYn = false;
        bool BUGS_AdultYn = false;

        QString BUGS_Scope = "";
    };


    /**
     * @brief Bugs API 요청시마다 헤더 세팅해야하는 값
     */
    struct BugsReqHeader{
        QString client = "";
        QString app_name = "";
        QString app_version = "";
        QString platform_name = "";
        QString platform_version = "";
        QString pc_model_name = "Mac";
        QString pc_model_comp = "Apple";
    };


    /**
     * @brief The ItemPositionData struct
     */
    struct ItemPositionData {
        int section = 0;
        int index = 0;
        bool likes_yn = false;
        QString data_id = "";
    };


    /**
     * @brief Bugs Track 아이템
     */
    /*struct TrackItemData {
        int track_id = 0;
        QString track_title = "";
        QString len = "";                ///< duration
        bool likes_yn = false;              ///< 좋아요 여부
        bool service_yn = false;            ///< 금지곡 여부
        bool adult_yn = false;              ///< 성인곡 여부

        QString album_title = "";
        QString album_image = "";
        QString bitrates = "";

        int album_id = 0;                         ///< Album 연결을 위해
        QList<int> list_artist_id;            ///< Artist 연결을 위해
        QList<QString> list_artist_nm;        ///< Artist 연결을 위해

        QList<int> list_mv_id;                ///< Video 연결을 위해
    };*/


    /**
     * @brief Bugs Album 아이템
     */
    struct AlbumItemData {
        int album_id = 0;
        QString title = "";
        QString descr = "";//cheon210604
        QString upd_dt = "";//cheon210604
        int track_count = 0;//cheon210604
        QString album_image = "";

        int total_count = 0;

        bool valid_yn = false;              ///< 금지곡 여부인가 싶음.
        bool likes_yn = false;

        QList<int> list_artist_id;            ///< Artist 연결을 위해
        QList<QString> list_artist_nm;        ///< Artist 연결을 위해
    };


    /**
     * @brief Bugs Artist 아이템
     */
    struct ArtistItemData {
        int artist_id = 0;
        QString artist_nm = "";
        QString image = "";
        bool likes_yn = false;
        //QString profile_url = "";//CHEON210606

        int total_count = 0;

        QString group_cd_nm = "";
        QString gender_cd_nm = "";
        QList<QString> list_genre_nm;
    };


    /**
     * @brief Bugs MV 아이템
     */
    struct VideoItemData {
        int mv_id = 0;
        QString mv_title = "";
        QString len = "";
        QString image = "";
        bool likes_yn = false;
        bool adult_yn = false;
        bool service_yn = false;

        int total_count = 0;

        int track_id = 0;
        int album_id = 0;
        QList<QString> bitrates;//c220528

        QList<int> list_artist_id;            ///< Artist 연결을 위해
        QList<QString> list_artist_nm;        ///< Artist 연결을 위해
    };



    /**
     * @brief Bugs Track 아이템
     */
    struct TrackItemData {
        int track_id = 0;
        int track_no = 0;
        QString track_title = "";
        QString len = "";                   ///< duration
        bool likes_yn = false;              ///< 좋아요 여부
        bool service_yn = false;            ///< 금지곡 여부
        bool adult_yn = false;              ///< 성인곡 여부

        QString album_title = "";
        QString album_image = "";
        QString bitrates = "";

        int total_count = 0;
        int chart_rank = 0;

        int album_id = 0;                     ///< Album 연결을 위해
        QList<int> list_artist_id;            ///< Artist 연결을 위해
        QList<QString> list_artist_nm;        ///< Artist 연결을 위해

        QList<int> list_mv_id;                ///< Video 연결을 위해
    };


    /**
     * @brief PD 앨범
     */
    struct PD_AlbumItemData {
        int es_album_id = 0;
        QString title = "";
        QString descr = "";//cheon210604
        QString upd_dt = "";//cheon210604
        int track_count = 0;//cheon210604
        QString image = "";
        bool likes_yn = false;
        QString pd_nickname = "";            ///< PD nickname

        int total_count = 0;

        QList<int> list_tag_id;            ///< tags 연결을 위해
        QList<QString> list_tag_nm;        ///< tags 연결을 위해
    };


    /**
     * @brief My Album 아이템. (Tidal의 user's playlist 와 같음)
     */
    struct MyAlbumItemData {
        int playlist_id = 0;
        QString title = "";
        QString image = "";
        int track_count = 0;
        int total_count = 0;
    };



    /**
     * @brief 벅스 태그 struct
     */
    struct BugsTag {
        int tag_id = 0;
        QString tag_nm = "";
    };



    //------------------- 페이지 이동에서 사용되는 Struct 모음 ----------------------------------------------------------------------------//

    /**
     * @brief PageInfo - BugsTrackListAll 클래스를 위한
     */
    struct PageInfo_TrackAllView{
        QString api_subPath = "";
        QString pathTitle = "";
    };


    /**
     * @brief PageInfo - BugsAlbumAllView_ofArtist 클래스를 위한
     */
    struct PageInfo_AlbumAllView_ofArtist{
        int artist_id = 0;
        QString filterOpt = "";                      // enum 으로 받지 않고 String 으로 받는다. include가 looping 되므로.
        QString mainTitle = "";
    };


    /**
     * @brief PageInfo - BugsTrackAllView_ofArtist 클래스를 위한
     */
    struct PageInfo_TrackAllView_ofArtist{
        int artist_id = 0;
        QString filterOpt = "";
        QString sortOpt = "";
        QString mainTitle = "";
    };


    /**
     * @brief PageInfo - BugsPDAlbumListAll 클래스를 위한
     */
    struct PageInfo_PDAlbumAllView{
        QString sortOpt = "";
        QString tag_ids = "";
        QString mainTitle = "";
    };



}


#endif // BUGS_STRUCT_H
