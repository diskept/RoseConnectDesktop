#ifndef CONVERTDATA_ROSEHOME_H
#define CONVERTDATA_ROSEHOME_H

#include "rosehome_struct.h"

#include "widget/OptMorePopup.h"

#include <QObject>
#include <QJsonObject>


namespace roseHome {

    /**
     * @brief Qobuz 관련 클래스들이 사용하는 Struct 와  Json 변환을 담당하는 클래스
     */
    class ConvertData : public QObject
    {
        Q_OBJECT
    public:

        /**
         * @brief Qobuz에서 지원하는 정사각형 이미지의 사이즈. Album, Artist, Track, Playlist 등
         * @details https://listen.qobuz.com/my-collection/playlists 에서 HTML 소스로 체크함
         */
        enum ImageSize_Square {
            Size_200x200
            , Size_400x400
            , Size_430x430
            , Size_600x400
            , Size_860x860
        };
        Q_ENUM(ImageSize_Square);

        /**
         * @brief Qobuz에서 지원하는 직사각형 이미지의 사이즈. Video 등.
         * @details https://listen.qobuz.com/my-collection/videos 에서 HTML 소스로 체크함
         */
        enum ImageSize_Ractangle {
            Size_284x157
            , Size_360x200
            , Size_384x188
            , Size_568x314
            , Size_720x400
            , Size_768x376

        };
        Q_ENUM(ImageSize_Ractangle);


        // 데이터 관련 Struct ---------------------------------------------------------------

        // about PlaylistItemData
        static QJsonObject getObjectJson_playlistData(const roseHome::PlaylistItemData &data);
        static roseHome::PlaylistItemData convertData_playlistData(const QJsonObject &jsonObj);

        // about AlbumItemData
        static QJsonObject getObjectJson_albumData(const roseHome::AlbumItemData &data);
        static roseHome::AlbumItemData convertData_albumData(const QJsonObject &jsonObj);

        // about TrackItemData
        static QJsonObject getObjectJson_trackData(const roseHome::TrackItemData &data);
        static roseHome::TrackItemData convertData_trackItemData(const QJsonObject &jsonObj);

        // convert ItemData to OptMorePopup::HeaderData
        static OptMorePopup::HeaderData getConvertOptHeaderData(const roseHome::AlbumItemData &data);
        static OptMorePopup::HeaderData getConvertOptHeaderData(const roseHome::PlaylistItemData &data);
        static OptMorePopup::HeaderData getConvertOptHeaderData(const roseHome::TrackItemData &data);
        //static OptMorePopup::HeaderData getConvertOptHeaderData(const roseHome::ArtistItemData &data);
        //static OptMorePopup::HeaderData getConvertOptHeaderData(const roseHome::VideoItemData &data);


        // 페이지 이동관련된 Struct ---------------------------------------------------------------

        static roseHome::PageInfo_UserPage getObjectJson_pageInfo_UserPage(const QJsonObject &jsonObj);
        static QJsonObject getObjectJson_pageInfo_UserPage(const roseHome::PageInfo_UserPage &data);

        // about PageInfo_AlbumAllView
        static QJsonObject getObjectJson_pageInfo_albumAllView(const roseHome::PageInfo_AlbumAllView &data);
        static roseHome::PageInfo_AlbumAllView convertData_pageInfo_albumAllView(const QJsonObject &jsonObj);

        // about PageInfo_PlaylistAllView
        static QJsonObject getObjectJson_pageInfo_playlistAllView(const roseHome::PageInfo_PlaylistAllView &data);
        static roseHome::PageInfo_PlaylistAllView convertData_pageInfo_playlistAllView(const QJsonObject &jsonObj);

        // about PageInfo_trackAllView
        static QJsonObject getObjectJson_pageInfo_trackAllView(const roseHome::PageInfo_TrackAllView &data);
        static roseHome::PageInfo_TrackAllView convertData_pageInfo_trackAllView(const QJsonObject &jsonObj);

        static QJsonObject getObjectJson_pageInfo_radioAllView(const roseHome::PageInfo_RadioAllView &data);
        static roseHome::PageInfo_RadioAllView convertData_pageInfo_radioAllView(const QJsonObject &jsonObj);
    };
};

#endif // CONVERTDATA_ROSEHOME_H
