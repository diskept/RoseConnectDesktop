#ifndef CONVERTDATA_FORBUGS_H
#define CONVERTDATA_FORBUGS_H

#include <QObject>
#include "bugs/bugs_struct.h"
#include "widget/OptMorePopup.h"
#include "bugs/BugsChooseFilterOpt.h"


namespace bugs {

    /**
     * @brief Bugs관련 클래스들이 사용하는 Struct 와  Json 변환을 담당하는 클래스
     */
    class ConvertData_forBugs : public QObject
    {
    public:

        /**
         * @brief Tidal에서 지원하는 정사각형 이미지의 사이즈. Album, Artist, Track, Playlist 등
         * @details https://listen.tidal.com/my-collection/playlists 에서 HTML 소스로 체크함
         */
        enum class ImageSize_Square {
            Size_75x75 = 75
            , Size_140x140 = 140
            , Size_200x200 = 200
            , Size_350x350 = 350
            , Size_500x500 = 500
            , Size_1000x1000 = 1000


        };
        Q_ENUM(ImageSize_Square);

        /// MyAlbum 이미지 사이즈
        enum class ImageSize_Square_My_Album {
            Size_75x75 = 75
            , Size_140x140 = 140
            , Size_200x200 = 200
            , Size_350x350 = 350
            , Size_500x500 = 500
            , Size_1000x1000 = 1000
        };
        Q_ENUM(ImageSize_Square_My_Album);

        /// PD's Album 이미지 사이즈
        enum class ImageSize_Square_PD_Album {
            Size_60x60 = 60
            , Size_120x120 = 120
            , Size_130x130 = 130
            , Size_148x148 = 148
            , Size_250x250 = 250
            , Size_500x500 = 500
        };
        Q_ENUM(ImageSize_Square_PD_Album);

        /**
         * @brief Tidal에서 지원하는 직사각형 이미지의 사이즈. Video 등.
         * @details https://listen.tidal.com/my-collection/videos 에서 HTML 소스로 체크함
         */
        enum class ImageSize_Ractangle {
            Size_75w = 75
            , Size_100w = 100
            , Size_140w = 140
            , Size_200w = 200
            , Size_350w = 350
            , Size_500w = 500
        };
        Q_ENUM(ImageSize_Ractangle);


        // RoseSessionInfo_forBugs
        static QJsonObject getObjectJson_roseSessionInfo(const bugs::RoseSessionInfo_forBugs &data);
        static bugs::RoseSessionInfo_forBugs convertData_roseSessionInfo(const QJsonObject &jsonObj);


        // 데이터 관련 Struct ---------------------------------------------------------------

        // about bugs::ItemPositionData
        static QJsonObject getObjectJson_itemPositionData(const bugs::ItemPositionData &data);
        static bugs::ItemPositionData convertData_itemPositionData(const QJsonObject &jsonObj);


        // about bugs::TrackItemData
        static QJsonObject getObjectJson_trackData(const bugs::TrackItemData &data);
        static bugs::TrackItemData convertData_trackData(const QJsonObject &jsonObj);

        // about bugs::AlbumItemData
        static QJsonObject getObjectJson_albumData(const bugs::AlbumItemData &data);
        static bugs::AlbumItemData convertData_albumData(const QJsonObject &jsonObj);

        // about bugs::PD_AlbumItemData
        static QJsonObject getObjectJson_pd_albumData(const bugs::PD_AlbumItemData &data);
        static bugs::PD_AlbumItemData convertData_pd_albumData(const QJsonObject &jsonObj);

        // about bugs::MyAlbumItemData
        static QJsonObject getObjectJson_my_albumData(const bugs::MyAlbumItemData &data);
        static bugs::MyAlbumItemData convertData_my_albumData(const QJsonObject &jsonObj);


        // about bugs::ArtistItemData
        static QJsonObject getObjectJson_artistData(const bugs::ArtistItemData &data);
        static bugs::ArtistItemData convertData_artistData(const QJsonObject &jsonObj);

        // about bugs::VideoItemData
        static QJsonObject getObjectJson_videoData(const bugs::VideoItemData &data);
        static bugs::VideoItemData convertData_videoData(const QJsonObject &jsonObj);


        // about bugs::FilterOptionData
        static QJsonObject getObjectJson_filterOptionData(const bugs::FilterOptionData &data);
        static bugs::FilterOptionData convertData_filterOptionData(const QJsonObject &jsonObj);


        // convert ItemData to OptMorePopup::HeaderData
        static OptMorePopup::HeaderData getConvertOptHeaderData(const bugs::AlbumItemData &data);
        static OptMorePopup::HeaderData getConvertOptHeaderData(const bugs::PD_AlbumItemData &data);
        static OptMorePopup::HeaderData getConvertOptHeaderData(const bugs::MyAlbumItemData &data);
        static OptMorePopup::HeaderData getConvertOptHeaderData(const bugs::TrackItemData &data);
        static OptMorePopup::HeaderData getConvertOptHeaderData(const bugs::ArtistItemData &data);
        static OptMorePopup::HeaderData getConvertOptHeaderData(const bugs::VideoItemData &data);



        // Rose에서 String으로 반환한다. (Bugs에서는 int로 반환해주는 값인데)
        static QList<int> convert_listString_to_listInt(QList<QString> p_list_str);

        // 데이터 변환 관련 (Bugs API 결과값 Json <------> struct)
        static bugs::TrackItemData make_trackData_fromBugsJsonObj(const QJsonObject &jsonObj);
        static bugs::AlbumItemData make_albumData_fromBugsJsonObj(const QJsonObject &jsonObj);
        static bugs::ArtistItemData make_artistData_fromBugsJsonObj(const QJsonObject &jsonObj);
        static bugs::VideoItemData make_videoData_fromBugsJsonObj(const QJsonObject &jsonObj);
        static bugs::TrackItemData make_trackData_fromBugsJsonObj_75(const QJsonObject &jsonObj);
        static bugs::PD_AlbumItemData make_pd_albumData_fromBugsJsonObj(const QJsonObject &jsonObj);
        static bugs::MyAlbumItemData make_my_albumData_fromBugsJsonObj(const QJsonObject &jsonObj);



        // 페이지 이동관련된 Struct ---------------------------------------------------------------

        // about PageInfo_TrackAllView
        static QJsonObject getObjectJson_pageInfo_trackAllView(const bugs::PageInfo_TrackAllView &data);
        static bugs::PageInfo_TrackAllView convertData_pageInfo_trackAllView(const QJsonObject &jsonObj);

        // about PageInfo_AlbumAllView_ofArtist
        static QJsonObject getObjectJson_pageInfo_albumAllView_ofArtist(const bugs::PageInfo_AlbumAllView_ofArtist &data);
        static bugs::PageInfo_AlbumAllView_ofArtist convertData_pageInfo_albumAllView_ofArtist(const QJsonObject &jsonObj);

        // about PageInfo_TrackAllView_ofArtist
        static QJsonObject getObjectJson_pageInfo_trackAllView_ofArtist(const bugs::PageInfo_TrackAllView_ofArtist &data);
        static bugs::PageInfo_TrackAllView_ofArtist convertData_pageInfo_trackAllView_ofArtist(const QJsonObject &jsonObj);

        // about PageInfo_PDAlbumAllView
        static QJsonObject getObjectJson_pageInfo_pd_albumAllView(const bugs::PageInfo_PDAlbumAllView &data);
        static bugs::PageInfo_PDAlbumAllView convertData_pageInfo_pd_albumAllView(const QJsonObject &jsonObj);



    private:
        template<typename T_ImageSize>
        static QString convertImagePath(QString url, QString path, const T_ImageSize imageSize);

    };

};

#endif // CONVERTDATA_FORBUGS_H
