#ifndef APPLE_STRUCT_H
#define APPLE_STRUCT_H

#include <list>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVariantList>


namespace apple {

    /**
     * @brief Apple Session Info struct
     * @details Apple API 반환값 참고 @n
     */
    struct RoseSessionInfo_forApple {
        QString APPLE_AccessToken = "";
        QString APPLE_StoreFront = "";
    };

    /**
     * @brief 장르에 대한 정보
     */
    struct GenreInfo {
        QString name = "";           ///< 장르명
        QString href = "";           ///< 장르 데이터 요청을 위한 path
        QString type = "";

        int id = 0;

        QString parentName = "";

        int parentId = 0;
    };

    /**
     * @brief Apple의 Album 아이템
     */
    struct AlbumItemData {
        QString cllientKey = "";
        QString thumbnail = "";
        QString title = "";

        int id = 0;
        int totalCount = 0;

        int star = 0;
        bool favorite = false;
    };

    /**
     * @brief Apple의 Playlist 아이템
     */
    struct PlaylistItemData {
        QString cllientKey = "";
        QString thumbnail = "";
        QString title = "";
        QString id = 0;

        int totalCount = 0;

        int star = 0;
        bool favorite = false;
    };

};
#endif // APPLE_STRUCT_H
