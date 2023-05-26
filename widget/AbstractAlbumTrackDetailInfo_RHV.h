#ifndef ABSTRACTALBUMTRACKDETAILINFO_RHV_H
#define ABSTRACTALBUMTRACKDETAILINFO_RHV_H

#include "clickablelabel.h"

#include "tidal/tidal_struct.h"
#include "bugs/bugs_struct.h"
#include "qobuz/qobuz_struct.h"

#include <QLabel>
#include <QWidget>

#include <data/datapopup.h>

#include <common/linker.h>

/**
 * @brief 앨범 상세뷰에서 상단에 이미지와 앨범 타이틀, 아티스트 등의 정보가 보이는 Widget
 */
class AlbumTrackDetailInfo_RHV : public QPushButton
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

    explicit AlbumTrackDetailInfo_RHV(QWidget *parent = nullptr);
    ~AlbumTrackDetailInfo_RHV();

    void setDataTrackInfo_Music(const QJsonObject&);
    void setDataTrackInfo_Tidal(const tidal::TrackItemData&);
    void setDataTrackInfo_Bugs(const bugs::TrackItemData&);
    void setDataTrackInfo_Qobuz(const qobuz::TrackItemData&);
    void setDataTrackInfo_Apple(const QJsonObject&);
    void setDataTrackInfo_CDplay(const QJsonObject&);//c220724

    void setFavoritesIds(bool p_myFavoriteIds, int p_star);
    bool getFavoritesIds();
    int getFavoritesStars();

signals:
    void clicked(const int index, const AlbumTrackDetailInfo_RHV::ClickMode clickMode);

private slots:
    void slot_clickBtn_signal();

private:
    void setUIControl_basic();

private:
    Linker *linker;

    QWidget *widget_info_main;

    QLabel *label_track_hires;
    QLabel *label_list_play;
    QLabel *label_track_number;
    QLabel *label_title;
    QLabel *label_composer;
    QLabel *label_creatorName;
    QLabel *label_artist;
    QLabel *label_resolution;
    QLabel *label_duration;
    QLabel *label_adult_certification;

    QPushButton *btn_music_video;
    QPushButton *btn_play_list_add;
    QPushButton *btn_list_fav;
    QPushButton *btn_menu;

    bool myFavoriteIds = false;
    int myStarIds = 0;
};





class AlbumTrackDetailInfoCredit_RHV : public QPushButton
{
    Q_OBJECT

public:
    explicit AlbumTrackDetailInfoCredit_RHV(QWidget *parent = nullptr);
    ~AlbumTrackDetailInfoCredit_RHV();

    void setDataTrackInfo_Qobuz(const qobuz::TrackItemData&);

    void setFavoritesIds(bool p_myFavoriteIds, int p_star);
    bool getFavoritesIds();
    int getFavoritesStars();

signals:
    void clicked(const int index, const AlbumTrackDetailInfo_RHV::ClickMode clickMode);

private slots:
    void slot_clickBtn_signal();

private:
    void setUIControl_basic();

private:
    Linker *linker;

    QWidget *widget_info_main;

    QLabel *label_list_play;
    QLabel *label_track_number;
    QLabel *label_title;
    QLabel *label_composer;
    QLabel *label_creatorName;
    QLabel *label_artist;
    QLabel *label_duration;

    QPushButton *btn_play_list_add;
    QPushButton *btn_list_fav;
    QPushButton *btn_menu;

    bool myFavoriteIds = false;
    int myStarIds = 0;
};

#endif // ALBUMTRACKDETAILINFO_RHV_H
