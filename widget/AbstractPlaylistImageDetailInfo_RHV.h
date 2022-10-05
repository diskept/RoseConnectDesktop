#ifndef ABSTRACTPLAYLISTIMAGEDETAILINFO_RHV_H
#define ABSTRACTPLAYLISTIMAGEDETAILINFO_RHV_H

#include "widget/AbstractImageDetailInfo_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include "roseHome/rosehome_struct.h"
#include "tidal/AbstractItem.h"
#include "tidal/tidal_struct.h"
#include "qobuz/qobuz_struct.h"
#include "bugs/bugs_struct.h"

/**
 * @brief Playlist 상세뷰에서 상단에 이미지와 Playlist 타이틀, 만든이, Description 등의 정보가 보이는 Widget
 */
class PlaylistImageDetailInfo_RHV : public AbstractImageDetailInfo_RHV
{
    Q_OBJECT

public:
    explicit PlaylistImageDetailInfo_RHV(ContentsUIType p_ContentsUIType, QWidget *parent = nullptr);
    ~PlaylistImageDetailInfo_RHV();

    void setMyData(bool flagMine) override;
    void initView() override;

    //void setData_fromTidalData(const tidal::PlaylistItemData &data_playlist);

    void setData_fromQobuzData(const qobuz::PlaylistItemData &data_playlist);
    void setData_fromQobuzData(const QList<qobuz::TrackItemData>&, const QJsonArray&);
    void setData_fromQobuzData(const QList<qobuz::PlaylistItemData>&);

    void setData_fromRoseData(const roseHome::PlaylistItemData &data_playlist);
    void setData_fromRoseData(const QList<roseHome::TrackItemData>&, const QJsonArray&);

    //void setFavoritesIds_forTidal(QVariantList p_arr_myFavoriteIds) override;
    //void setFavoritesIds_forQobuz(QVariantList p_arr_myFavoriteIds) override;

    void setFavoritesIds(bool p_myFavoriteIds, int p_star) override;
    bool getFavoritesIds() override;
    int getFavoritesStars() override;

signals:
    void list_Clicked(const int, const PlaylistTrackDetailInfo_RHV::ClickMode);
    void album_Clicked(const tidal::AbstractItem::ClickMode);
    void similarPlaylist_Click(const int, const int, const int);
    void similarPlaylist_btnVeiwAll_Click(const QString);

private slots:
    void slot_detailInfo_btnClicked(const int, const PlaylistTrackDetailInfo_RHV::ClickMode);
    void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode);
    void slot_clickBtn_subTitle_viewAll();

private:
    void setUIControl_trackList();
    void setUIControl_similarPlaylists();

    void setUIControl_subTitle_withSideBtn(const QString, const QString);
    QHBoxLayout* setUIControl_hBoxLayout_forPlaylists();

private:
    QVBoxLayout *vbox_trackList;

    QVBoxLayout *vbox_contents;
    QHBoxLayout *hBox_similarPlaylists;

    QLabel *lb_subTitle;
    QPushButton *btnView_all;

    bool flagNeededInit;
    bool flag_trackList = false;
    bool flag_similarPlaylist = false;

    QString playlist_descriptionText = "";
};

#endif // ABSTRACTPLAYLISTIMAGEDETAILINFO_RHV_H
