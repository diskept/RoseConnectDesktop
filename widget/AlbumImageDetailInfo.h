#ifndef ALBUMIMAGEDETAILINFO_H
#define ALBUMIMAGEDETAILINFO_H

#include "widget/AbstractImageDetailInfo.h"
#include "tidal/tidal_struct.h"
#include "qobuz/qobuz_struct.h"
#include "bugs/bugs_struct.h"

/**
 * @brief 앨범 상세뷰에서 상단에 이미지와 앨범 타이틀, 아티스트 등의 정보가 보이는 Widget
 */
class AlbumImageDetailInfo : public AbstractImageDetailInfo
{
    Q_OBJECT

public:
    explicit AlbumImageDetailInfo(QWidget *parent = nullptr);

    void setMyData(bool flagMine) override;
    void initView() override;           // 기본
    void initView_forBugs();            // only Bugs

    void setData_fromTidalData(const tidal::AlbumItemData &data_album);         // Tidal의 정보로 세팅
    void setData_fromBugsData(const bugs::AlbumItemData &data_album);           // Bugs의 정보로 세팅
    void setData_fromQobuzData(const qobuz::AlbumItemData &data_album);         // Qobuz의 정보로 세팅

    void setFavoritesIds_forTidal(QVariantList p_arr_myFavoriteIds) override;
    void setFavoritesIds_forQobuz(QVariantList p_arr_myFavoriteIds) override;
    void setFavorites_forQobuz(const bool flag);
    void updateDate_playlist_bioText(const QString &descriptionText);//cheon210605

private:
    bool flagNeededInit;

    QString playlist_descriptionText="";

};

#endif // ALBUMIMAGEDETAILINFO_H
