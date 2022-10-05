#ifndef PLAYLISTIMAGEDETAILINFO_H
#define PLAYLISTIMAGEDETAILINFO_H

#include <QWidget>
#include "widget/AbstractImageDetailInfo.h"
#include "tidal/tidal_struct.h"
#include "qobuz/qobuz_struct.h"
/**
 * @brief Playlist 상세뷰에서 상단에 이미지와 Playlist 타이틀, 만든이, Description 등의 정보가 보이는 Widget
 * @details Tidal 에서 사용중
 */
class PlaylistImageDetailInfo : public AbstractImageDetailInfo
{
    Q_OBJECT
public:
    explicit PlaylistImageDetailInfo(QWidget *parent = nullptr);    

    void setMyData(bool flagMine) override;

    void initView() override;
    void setData_fromTidalData(const tidal::PlaylistItemData &data_playlist);
    void setFavoritesIds_forTidal(QVariantList p_arr_myFavoriteIds) override;


    void set_display_forEditMode();
    void updateDate_playlist_bioText(const QString &bioText);//cheon210603

private:
    QString playlist_descriptionText;//cheon210603

};

class PlaylistImageDetailInfo_forQobuz : public AbstractImageDetailInfo_forQobuz
{
    Q_OBJECT
public:
    explicit PlaylistImageDetailInfo_forQobuz(QWidget *parent = nullptr);

    void setMyData(bool flagMine) override;

    void initView() override;
    void setData_fromQobuzData(const qobuz::PlaylistItemData &data_playlist);
    void setFavoritesIds_forQobuz(QVariantList p_arr_myFavoriteIds) override;
    void setFavorites_forQobuz(const bool flag);

    void set_display_forEditMode();
    void updateDate_playlist_bioText(const QString &bioText);//cheon210603

private:
    QString playlist_descriptionText;//cheon210603

};

#endif // PLAYLISTIMAGEDETAILINFO_H
