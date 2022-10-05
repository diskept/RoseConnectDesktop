#ifndef ROSEHOME_STRUCT_H
#define ROSEHOME_STRUCT_H

#include <QJsonObject>
#include <QString>
#include <list>
#include <QStringList>
#include <QVariantList>


namespace roseHome {

    /**
     * @brief Rosehome의 Recommends 아이템
     */
    struct RecommendsItemData{
        int id = 0;
        int star = 0;
        int ownerId = 0;
        int thumbupCount = 0;
        int trackCount = 0;
        int sort = 0;        
        int duration = 0;
        int totalCount = 0;

        QString type = "";
        QString title = "";
        QString share = "";
        QString ownerName = "";
        QString thumbnail = "";
        QString registDateTime = "";
        QString clientKey = "";
        QString comment = "";

        bool favoriteTrack = false;
        bool favorite = false;
        bool thumbup = false;
        bool isRose = false;

        QList<QString> tags;
    };

    /**
     * @brief Rosehome의 album 아이템
     */
    struct AlbumItemData{
        int id = 0;
        int star = 0;
        int trackCount = 0;
        int totalCount = 0;
        int trackId = 0;
        int duration = 0;

        QString type = "";
        QString title = "";
        QString artist = "";
        QString thumbnail = "";
        QString playTime = "";
        QString clientKey = "";
        QString macAddress = "";
        QString mime = "";

        bool favorite = false;
    };

    /**
     * @brief Rosehome의 playlist 아이템
     */
    struct PlaylistItemData{
        int id = 0;
        int ownerId = 0;
        int sort = 0;
        int star = 0;
        int thumbupCount = 0;
        int trackCount = 0;
        int trackId = 0;
        int duration = 0;
        int totalCount = 0;

        QString clientKey = "";
        QString comment = "";
        QString ownerName = "";
        QString playTime = "";
        QString registDateTime = "";
        QString share = "";
        QString thumbnail = "";
        QString title = "";
        QString type = "";
        QString macAddress = "";

        bool favorite = false;
        bool favoriteTrack = false;
        bool isRose = false;
        bool thumbup = false;

        QList<QString> tags;
    };

    /**
     * @brief Rosehome Artist 아이템
     */
    struct ArtistItemData {

        QString imageUrl = "";
        QString name = "";
        QString description = "";
        QString biography = "";
        QString roseKey = "";

        int id = 0;
        int albums_count = 0;
        int tracks_count = 0;
        int artist_total_cnt = 0;
    };

    /**
     * @brief Rosehome의 track 아이템
     */
    struct TrackItemData{
        int duration = 0;
        int id = 0;
        int ownerId = 0;
        int playCount = 0;
        int sort = 0;
        int star = 0;
        int thumbupCount = 0;
        int trackCount = 0;
        int album_id = 0;
        int performer_id = 0;
        int maximum_bit_depth = 0;
        double maximum_sampling_rate = 0.0;
        int totalCount = 0;

        QString clientKey = "";
        QString comment = "";
        QString ownerName = "";
        QString playUrl = "";
        QString thumbnail = "";
        QString title = "";
        QString type = "";
        QString album_title = "";
        QString performer_name = "";
        QString work = "";
        QString version = "";
        QString bitrates = "";

        bool favorite = false;
        bool favoriteTrack = false;
        bool isRose = false;
        bool thumbup = false;
        bool hires = false;

        QList<QString> list_artist_name;        // Artist Artist 연결을 위해
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
        QString filter_type = "";

        int genre_id = 0;
        int artist_id = 0;
        int type_id = 0;
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
        QString filter_type = "";

        int genre_id = 0;
        int type_id = 0;
    };


    /**
     * @brief Tracklist 전체보기 페이지에 대한 페이지 Information 저장
     * @details All View struct를 별도로 구성함. 혹시 몰라서.
     */
    struct PageInfo_TrackAllView {
        QString api_subPath = "";
        QString pathTitle = "";
        QString type = "";
        QString genre_ids = "";
        QString filter_type = "";

        int genre_id = 0;
        int type_id = 0;
    };


    struct PageInfo_UserPage {
        QString api_subPath = "";
        QString pathTitle = "";
        QString type = "";

        int member_id = 0;
        int type_id = 0;
    };


    struct PageInfo_RadioAllView {
        QString api_subPath = "";
        QString pathTitle = "";
        QString type = "";

        int genre_id = 0;
    };
};

#endif // ROSEHOME_STRUCT_H
