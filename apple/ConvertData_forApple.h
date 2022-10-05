#ifndef CONVERTDATA_FORAPPLE_H
#define CONVERTDATA_FORAPPLE_H

#include "apple/Apple_struct.h"

#include "widget/OptMorePopup.h"

#include <QObject>


namespace apple {

    class ConvertData : public QObject
    {
        Q_OBJECT
    public:

        // RoseSessionInfo_forApple
        static QJsonObject getObjectJson_roseSessionInfo(const apple::RoseSessionInfo_forApple&);
        static apple::RoseSessionInfo_forApple convertData_roseSessionInfo(const QJsonObject&);

        // 데이터 관련 Struct ---------------------------------------------------------------

        // about AlbumItemData
        static QJsonObject getObjectJson_albumData(const apple::AlbumItemData&);
        static apple::AlbumItemData convertData_albumData(const QJsonObject&);

        // about PlaylistItemData
        static QJsonObject getObjectJson_playlistData(const apple::PlaylistItemData&);
        static apple::PlaylistItemData convertData_playlistData(const QJsonObject&);

        // convert ItemData to OptMorePopup::HeaderData
        static OptMorePopup::HeaderData getConvertOptHeaderData_album(const QJsonObject&);
        static OptMorePopup::HeaderData getConvertOptHeaderData_playlist(const QJsonObject&);
        static OptMorePopup::HeaderData getConvertOptHeaderData_track(const QJsonObject&);
    };
};
#endif // CONVERTDATA_FORAPPLE_H
