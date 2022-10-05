#ifndef CONVERTDATA_H
#define CONVERTDATA_H

#include "tidal/tidal_struct.h"

#include "delegate/AbstractEditListDelegate.h"

#include "widget/OptMorePopup.h"

#include <QObject>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal 관련 클래스들이 사용하는 Struct 와  Json 변환을 담당하는 클래스
     */
    class ConvertData : public QObject
    {
        Q_OBJECT
    public:

        /**
         * @brief Tidal에서 지원하는 정사각형 이미지의 사이즈. Album, Artist, Track, Playlist 등
         * @details https://listen.tidal.com/my-collection/playlists 에서 HTML 소스로 체크함
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
         * @brief Tidal에서 지원하는 직사각형 이미지의 사이즈. Video 등.
         * @details https://listen.tidal.com/my-collection/videos 에서 HTML 소스로 체크함
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


        // RoseSessionInfo_forTidal
        static QJsonObject getObjectJson_roseSessionInfo(const tidal::RoseSessionInfo_forTidal &data);
        static tidal::RoseSessionInfo_forTidal convertData_roseSessionInfo(const QJsonObject &jsonObj);


        // Tidal Image Resource ID ---> image URL
        static QString convertImagePath(const QString tidalImageKey, const ImageSize_Square imageSize);
        static QString convertImagePath(const QString tidalImageKey, const ImageSize_Ractangle imageSize);
        static QString getImageJpgName(const ImageSize_Square imageSize);
        static QString getImageJpgName(const ImageSize_Ractangle imageSize);

        // Tidal 기타 데이터 처리
        static QString getCreatorName(const QJsonObject p_jsonObj_creator);


        // 데이터 변환 관련 (Tidal API 결과값 Json <------> struct)
        static tidal::TrackItemData make_trackData_fromTidalJsonObj(const QJsonObject &jsonObj);
        static tidal::TrackItemData make_visualData_fromTidalJsonObj(const QJsonObject &jsonObj);
        static tidal::AlbumItemData make_albumData_fromTidalJsonObj(const QJsonObject &jsonObj);        
        static tidal::PlaylistItemData make_playlistData_fromTidalJsonObj(const QJsonObject &jsonObj);
        static tidal::PlaylistItemData make_mixesData_fromTidalJsonObj(const QJsonObject &jsonObj);        
        static tidal::ArtistItemData make_artistData_fromTidalJsonObj(const QJsonObject &jsonObj);
        static tidal::VideoItemData make_videoData_fromTidalJsonObj(const QJsonObject &jsonObj);


        // 데이터 관련 Struct ---------------------------------------------------------------

        // about bugs::ItemPositionData
        static QJsonObject getObjectJson_itemPositionData(const tidal::ItemPositionData &data);
        static tidal::ItemPositionData convertData_itemPositionData(const QJsonObject &jsonObj);


        // about PlaylistItemData
        static QJsonObject getObjectJson_playlistData(const tidal::PlaylistItemData &data);
        static tidal::PlaylistItemData convertData_playlistData(const QJsonObject &jsonObj);

        // about AlbumItemData
        static QJsonObject getObjectJson_albumData(const tidal::AlbumItemData &data);
        static tidal::AlbumItemData convertData_albumData(const QJsonObject &jsonObj);

        // about TrackItemData
        static QJsonObject getObjectJson_trackData(const tidal::TrackItemData &data);
        static tidal::TrackItemData convertData_trackItemData(const QJsonObject &jsonObj);

        // about ArtistItemData
        static QJsonObject getObjectJson_artistData(const tidal::ArtistItemData &data);
        static tidal::ArtistItemData convertData_artistItemData(const QJsonObject &jsonObj);

        // about VideoItemData
        static QJsonObject getObjectJson_videoData(const tidal::VideoItemData &data);
        static tidal::VideoItemData convertData_videoItemData(const QJsonObject &jsonObj);


        // convert ItemData to OptMorePopup::HeaderData
        static OptMorePopup::HeaderData getConvertOptHeaderData(const tidal::AlbumItemData &data);
        static OptMorePopup::HeaderData getConvertOptHeaderData(const tidal::PlaylistItemData &data);
        static OptMorePopup::HeaderData getConvertOptHeaderData(const tidal::TrackItemData &data);
        static OptMorePopup::HeaderData getConvertOptHeaderData(const tidal::ArtistItemData &data);
        static OptMorePopup::HeaderData getConvertOptHeaderData(const tidal::VideoItemData &data);




        static editDelegate::ItemEditData convertEditItem_fromTrackData(const tidal::TrackItemData &data);




        // 데이터 관련 Struct ---------------------------------------------------------------


        // 페이지 이동관련된 Struct ---------------------------------------------------------------

        // about PageInfo_TrackAllView
        static QJsonObject getObjectJson_pageInfo_trackAllView(const tidal::PageInfo_TrackAllView &data);
        static tidal::PageInfo_TrackAllView convertData_pageInfo_trackAllView(const QJsonObject &jsonObj);

        // about PageInfo_AlbumAllView
        static QJsonObject getObjectJson_pageInfo_albumAllView(const tidal::PageInfo_AlbumAllView &data);
        static tidal::PageInfo_AlbumAllView convertData_pageInfo_albumAllView(const QJsonObject &jsonObj);

        // about PageInfo_PlaylistAllView
        static QJsonObject getObjectJson_pageInfo_playlistAllView(const tidal::PageInfo_PlaylistAllView &data);
        static tidal::PageInfo_PlaylistAllView convertData_pageInfo_playlistAllView(const QJsonObject &jsonObj);

        // about PageInfo_ArtistAllView
        static QJsonObject getObjectJson_pageInfo_artistAllView(const tidal::PageInfo_ArtistAllView &data);
        static tidal::PageInfo_ArtistAllView convertData_pageInfo_artistAllView(const QJsonObject &jsonObj);

        // about PageInfo_VideoAllView
        static QJsonObject getObjectJson_pageInfo_videoAllView(const tidal::PageInfo_VideoAllView &data);
        static tidal::PageInfo_VideoAllView convertData_pageInfo_videoAllView(const QJsonObject &jsonObj);


    };

};

#endif // CONVERTDATA_H
