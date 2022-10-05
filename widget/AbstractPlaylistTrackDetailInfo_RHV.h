#ifndef ABSTRACTPLAYLISTTRACKDETAILINFO_RHV_H
#define ABSTRACTPLAYLISTTRACKDETAILINFO_RHV_H

#include "widget/clickablelabel.h"

#include "common/filedownloader.h"
#include "common/linker.h"

#include "data/datapopup.h"

#include "roseHome/rosehome_struct.h"
#include "tidal/tidal_struct.h"
#include "bugs/bugs_struct.h"
#include "qobuz/qobuz_struct.h"

#include <QLabel>
#include <QWidget>


/**
 * @brief 플레이리스트 상세뷰에서 상단에 이미지와 플레이리스트 타이틀, 아티스트 등의 정보가 보이는 Widget
 */
class PlaylistTrackDetailInfo_RHV : public QPushButton
{
    Q_OBJECT

public:
    enum ClickMode{
        AllBox = 1              ///< Hover 영역 전체 박스를 클릭
        , MoreBtn               ///< 더보기 버튼 클릭
        , FavBtn                ///< 좋아요(하트) 버튼 클릭
        , AddCollectionBtn      ///< My Collection 추가 버튼 클릭
        , MusicVideoBtn         ///< Bugs Music Video 버튼 클릭
    };

    explicit PlaylistTrackDetailInfo_RHV(QWidget *parent = nullptr);
    ~PlaylistTrackDetailInfo_RHV();

    void setDataTrackInfo_Music(const QJsonObject&);
    void setDataTrackInfo_Tidal(const tidal::TrackItemData&);
    void setDataTrackInfo_Bugs(const bugs::TrackItemData&, const QString&);
    void setDataTrackInfo_Qobuz(const qobuz::TrackItemData&);
    void setDataAlbumInfo_Qobuz(const qobuz::AlbumItemData&);
    void setDataTrackInfo_Apple(const QJsonObject&);
    void setDataTrackInfo_Rose(const roseHome::TrackItemData&);
    void setDataTrackInfo_RoseMain(const roseHome::TrackItemData&);
    void setDataTrackInfo_RoseListEdit(const QJsonObject&);
    void setDataTrackInfo_Video(const QJsonObject&);

    void setFavoritesIds(bool p_myFavoriteIds, int p_star);
    bool getFavoritesIds();
    int getFavoritesStars();
    QLabel *getThumnailLabel();

signals:
    void clicked(const int index, const ClickMode clickMode);

private slots:

    void slot_clickBtn_signal();
    void slot_fileDownload_loadImage();

private:

    void setUIControl_basic();
    void setImage(QString imagePath);
    void paint_image(QPixmap &pixmap);
private:

    Linker *linker;
    QLabel *label_track_thumbnail;
    QPixmap *pixmap_albumImg_default;
    FileDownloader *fileDownLoader;
    QPixmap *pixmap_albumImg;

    QWidget *widget_info_main;

    //QLabel *label_track_thumbnail;
    QLabel *label_track_type;
    QLabel *label_track_hires;
    QLabel *label_track_resolution;
    QLabel *label_rank;
    QLabel *label_title;
    QLabel *label_artist;
    QLabel *label_album;
    QLabel *label_duration;

    QPushButton *btn_music_video;
    QPushButton *btn_play_list_add;
    QPushButton *btn_list_fav;
    QPushButton *btn_menu;

    QString playlist_type;

    bool myFavoriteIds = false;
    int myStarIds = 0;
};

#endif // ABSTRACTPLAYLISTTRACKDETAILINFO_RHV_H
