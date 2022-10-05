#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <common/filedownloader.h>
#include <common/gscommon.h>

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QList>

/**
 * @brief 이미지 라벨 @n
 *      이미지 패스 세팅 시 @sa FileDownloader 를 이용해 이미지 다운로드 @n
 *      hover 기능 (하트,재생,더보기 옵션) 제공
 */
class ImageLabel : public QWidget
{
    Q_OBJECT


public:
    explicit ImageLabel(QWidget *parent=nullptr);
    ~ImageLabel();

    enum Type
    {
        Album = 0,
        Artist,
        PlayListMy,
        playListMy_C,           ///< 음악 My 플레이리스트 View All
        playListFriend_C,
        PlayListFriend,
        MusicPlayListDetail,    ///< 음악의 플레이리스트 상세화면
        Video,
        MusicCategory,
        RoseTube,
        RoseTubeList,
        RoseTube_s,
        RoseTubeList_s,
        RoseTubeDetail,
        IconSmallRound,
        IconSmallRect,
        Avatar,
        Podcast,
        None300,

        Tidal_Album = 90
        , Tidal_Playlist
//        , Tidal_Track
        , Tidal_Artist
    };


    void setImageUrl(QString p_url);
    void setImageResource(QString p_path);
    void setImageDefault();
    void setType(Type p_type);
    void setHover();
    void setHover2();
    void setDuration(const QString &p_duration);
    void setFavorStatus(bool flagOn);
    void setMimeType(QString mime);

    void setHidden_play(bool flagHidden);           ///< 버튼 각각에 대한 hidden 설정
    void setHidden_favorite(bool flagHidden);       ///< 버튼 각각에 대한 hidden 설정
    void setHidden_more(bool flagHidden);           ///< 버튼 각각에 대한 hidden 설정

signals:
    void clickedHoverItem(QString p_code);
    void clickedItem();
    void setPixmapCompleted(QPixmap);

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private :

    const QString HOVER_FAV_ON_IMG = ":/images/album_fav_icon_on.png";
    const QString HOVER_FAV_OFF_IMG = ":/images/album_fav_icon_off.png";
    const QString HOVER_PLAY_IMG = ":/images/album_play_icon.png";
    const QString HOVER_MORE_IMG = ":/images/album_more_icon.png";


    const int MARGIN = 10;
    const int IMG_WIDTH = 200;
    const int IMG_HEIGHT = 200;
    const int IMG_WIDTH_ARTIST = 180;
    const int IMG_HEIGTH_ARTIST = 180;
    const int IMG_WIDTH_MUSIC_PLAYLISTDETAIL = 300;
    const int IMG_HEIGTH_MUSIC_PLAYLISTDETAIL = 300;
    const int IMG_WIDTH_AVATAR = 180;
    const int IMG_HEIGHT_AVATAR = 180;
    const int IMG_WIDTH_VIDEO = 200;        //252;
    const int IMG_HEIGHT_VIDEO = 281;       //140;
    const int IMG_WIDTH_ROSETUBE_VIDEO = 284;   //252;
    const int IMG_HEIGHT_ROSETUBE_VIDEO = 157;  //140;
    const int IMG_WIDTH_MUSIC_CATEGORY = 186;
    const int IMG_HEIGHT_MUSIC_CATEGORY = 186;
    const int IMG_WIDTH_ROSETUBE = 360;     //340;
    const int IMG_HEIGHT_ROSETUBE = 200;    //188;
    const int IMG_WIDTH_ICON_SMALL = 60;
    const int IMG_HEIGTH_ICON_SMALL = 60;
    const int IMG_WIDTH_PODCAST = 200;      //214;
    const int IMG_HEIGHT_PODCAST = 200;     //214;
    const int IMG_WIDTH_ROSETUBE_DETAIL = 300;
    const int IMG_HEIGHT_ROSETUBE_DETAIL = 166;

    void setInit();
    void setUIControl();
    void setHoverUIControl();
    QString getDefualImgPath() const;

    Type type=Album;
    //QString imgPath; 캐시 key 주석

    QLabel *labelIMG;
    QLabel *playList_bar_165;
    QLabel *playList_bar_180;
    QLabel *playList_bar_220;
    QLabel *playList_bar_300;
    QLabel *playTime;
    QLabel *mime_type;
    QPushButton *hoverWidget;    
    FileDownloader *fileDownLoader;

    QPushButton *btn_play;
    QPushButton *btn_favorite;
    QPushButton *btn_more;

    QList<QString> listHoverPath;
    QList<QString> listHoverCode;
    QList<QPushButton*> list_hoverPushBtn;

    bool flagUseHover = false;
    bool flagUseHover2 = false;
    bool flagFavorOn = false;
    int imgWidth = IMG_WIDTH;
    int imgHeight = IMG_HEIGHT;

    QVBoxLayout *boxLayout;

private slots:
    void loadImage();
    void clickedHoverItem();
    void slot_clickedItem();
};

#endif // IMAGELABEL_H
