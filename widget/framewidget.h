#ifndef FRAMEWIDGET_H
#define FRAMEWIDGET_H

#include "common/linker.h"

#include <QWidget>
#include <QJsonObject>


/**
 * @brief 프레임 위젯 @n
 *
 * @note 각 메뉴의 앨범, 아티스트, 장르, 로즈튜브 등등의 프레임
 */
class FrameWidget : public QWidget
{
    Q_OBJECT
public:
    enum Type
    {
        RecentAlbum =0,     ///< 음악-홈-최근앨범
        AddedAlbum,         ///< 음악-홈-최근추가
        MyPlayList,         ///< 음악-홈-My플레이리스트
        FriendPlayList,     ///< 음악-홈-Friend플레이리스트
        Artist,             ///< 음악-홈-Artist
        Video,              ///< 비디오-홈
        Radio,              ///< 라디오
        MusicTrack,         ///< 음악-분류-곡
        MusicGenre,         ///< 음악-분류-장르
        MusicArtist,        ///< 음악-분류-아티스트
        MusicComposer,      ///< 음악-분류-작곡가
        MusicAlbum,         ///< 음악-앨범
        RoseTubePlayList,   ///< 로즈튜브-홈-My플레이리스트
        RoseTube,           ///< 로즈튜브-홈-TOP20
        PodCast,            ///< 팟캐스트
    };

    FrameWidget(Type p_type, QWidget *parent = nullptr);
    ~FrameWidget();

    virtual void setData(const QJsonObject &p_jsonObject) = 0;
    virtual void setHover() = 0;

protected:

    Type type;
    Linker *linker;

    virtual void setInit() = 0;
    virtual void setUIControl() = 0;
    virtual QJsonObject getDataForPopup() = 0;

protected slots:
    virtual void slot_clickedHover(const QString &p_code) = 0;
};

#endif // FRAMEWIDGET_H
