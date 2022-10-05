#ifndef CONVERTDATA_FORQOBUZ_H
#define CONVERTDATA_FORQOBUZ_H

#include <QObject>
#include <QJsonObject>
#include "qobuz/qobuz_struct.h"
#include "widget/OptMorePopup.h"
#include "delegate/AbstractEditListDelegate.h"


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

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
            Size_160x160
            , Size_320x320
            , Size_480x480
            , Size_640x640
            , Size_750x750
            , Size_1080x1080
        };
        Q_ENUM(ImageSize_Square);

        /**
         * @brief Qobuz에서 지원하는 직사각형 이미지의 사이즈. Video 등.
         * @details https://listen.qobuz.com/my-collection/videos 에서 HTML 소스로 체크함
         */
        enum ImageSize_Ractangle {
            Size_160x90
            , Size_320x180
            , Size_480x270
            , Size_640x360
            , Size_800x450
            , Size_1280x720
        };
        Q_ENUM(ImageSize_Ractangle);


        // RoseSessionInfo_forQobuz
        static QJsonObject getObjectJson_roseSessionInfo(const qobuz::RoseSessionInfo_forQobuz &data);
        static qobuz::RoseSessionInfo_forQobuz convertData_roseSessionInfo(const QJsonObject &jsonObj);


        // Qobuz Image Resource ID ---> image URL
        static QString convertImagePath(const QString qobuzImageKey, const ImageSize_Square imageSize);
        static QString convertImagePath(const QString qobuzImageKey, const ImageSize_Ractangle imageSize);
        static QString getImageJpgName(const ImageSize_Square imageSize);
        static QString getImageJpgName(const ImageSize_Ractangle imageSize);

        // Qobuz 기타 데이터 처리
        static QString getCreatorName(const QJsonObject p_jsonObj_creator);


        // 데이터 변환 관련 (Qobuz API 결과값 Json <------> struct)
        static qobuz::TrackItemData make_trackData_fromQobuzJsonObj(const QJsonObject &jsonObj);
        static qobuz::AlbumItemData make_albumData_fromQobuzJsonObj(const QJsonObject &jsonObj);
        static qobuz::PlaylistItemData make_playlistData_fromQobuzJsonObj(const QJsonObject &jsonObj);
        static qobuz::ArtistItemData make_artistData_fromQobuzJsonObj(const QJsonObject &jsonObj);
        static qobuz::VideoItemData make_videoData_fromQobuzJsonObj(const QJsonObject &jsonObj);

        static qobuz::TrackItemData_apple make_trackData_fromAppleJsonObj(const QJsonObject &jsonObj);      //c220304


        // 데이터 관련 Struct ---------------------------------------------------------------

        // about bugs::ItemPositionData
        static QJsonObject getObjectJson_itemPositionData(const qobuz::ItemPositionData &data);
        static qobuz::ItemPositionData convertData_itemPositionData(const QJsonObject &jsonObj);


        // about PlaylistItemData
        static QJsonObject getObjectJson_playlistData(const qobuz::PlaylistItemData &data);
        static qobuz::PlaylistItemData convertData_playlistData(const QJsonObject &jsonObj);

        // about AlbumItemData
        static QJsonObject getObjectJson_albumData(const qobuz::AlbumItemData &data);
        static qobuz::AlbumItemData convertData_albumData(const QJsonObject &jsonObj);

        // about TrackItemData
        static QJsonObject getObjectJson_trackData(const qobuz::TrackItemData &data);
        static qobuz::TrackItemData convertData_trackItemData(const QJsonObject &jsonObj);

        // about ArtistItemData
        static QJsonObject getObjectJson_artistData(const qobuz::ArtistItemData &data);
        static qobuz::ArtistItemData convertData_artistItemData(const QJsonObject &jsonObj);

        // about VideoItemData
        static QJsonObject getObjectJson_videoData(const qobuz::VideoItemData &data);
        static qobuz::VideoItemData convertData_videoItemData(const QJsonObject &jsonObj);

        // convert ItemData to OptMorePopup::HeaderData
        static OptMorePopup::HeaderData getConvertOptHeaderData(const qobuz::AlbumItemData &data);
        static OptMorePopup::HeaderData getConvertOptHeaderData(const qobuz::PlaylistItemData &data);
        static OptMorePopup::HeaderData getConvertOptHeaderData(const qobuz::TrackItemData &data);
        static OptMorePopup::HeaderData getConvertOptHeaderData(const qobuz::ArtistItemData &data);
        static OptMorePopup::HeaderData getConvertOptHeaderData(const qobuz::VideoItemData &data);




        static editDelegate::ItemEditData convertEditItem_fromTrackData(const qobuz::TrackItemData &data);




        // 데이터 관련 Struct ---------------------------------------------------------------


        // 페이지 이동관련된 Struct ---------------------------------------------------------------

        // about PageInfo_TrackAllView
        static QJsonObject getObjectJson_pageInfo_trackAllView(const qobuz::PageInfo_TrackAllView &data);
        static qobuz::PageInfo_TrackAllView convertData_pageInfo_trackAllView(const QJsonObject &jsonObj);

        // about PageInfo_AlbumAllView
        static QJsonObject getObjectJson_pageInfo_albumAllView(const qobuz::PageInfo_AlbumAllView &data);
        static qobuz::PageInfo_AlbumAllView convertData_pageInfo_albumAllView(const QJsonObject &jsonObj);

        // about PageInfo_PlaylistAllView
        static QJsonObject getObjectJson_pageInfo_playlistAllView(const qobuz::PageInfo_PlaylistAllView &data);
        static qobuz::PageInfo_PlaylistAllView convertData_pageInfo_playlistAllView(const QJsonObject &jsonObj);

        // about PageInfo_ArtistAllView
        static QJsonObject getObjectJson_pageInfo_artistAllView(const qobuz::PageInfo_ArtistAllView &data);
        static qobuz::PageInfo_ArtistAllView convertData_pageInfo_artistAllView(const QJsonObject &jsonObj);

        // about PageInfo_VideoAllView
        static QJsonObject getObjectJson_pageInfo_videoAllView(const qobuz::PageInfo_VideoAllView &data);
        static qobuz::PageInfo_VideoAllView convertData_pageInfo_videoAllView(const QJsonObject &jsonObj);


    };

};

#endif // CONVERTDATA_H
