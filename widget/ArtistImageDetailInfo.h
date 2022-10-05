#ifndef ARTISTIMAGEDETAILINFO_H
#define ARTISTIMAGEDETAILINFO_H

#include "widget/AbstractImageDetailInfo.h"
#include "tidal/tidal_struct.h"
#include "qobuz/qobuz_struct.h"
#include "bugs/bugs_struct.h"

/**
 * @brief Artist 상세뷰에서 상단에 아티스트의 이미지와 이름 등의 정보가 보이는 Widget
 */
class ArtistImageDetailInfo : public AbstractImageDetailInfo
{
    Q_OBJECT
public:
    explicit ArtistImageDetailInfo(QWidget *parent = nullptr);

    void initView() override;
    void initView_forBugs();

    void setData_fromTidalData(const tidal::ArtistItemData &data_artist);         // 타이달의 정보로 세팅
    void setData_fromQobuzData(const qobuz::ArtistItemData &data_artist);
    void setData_fromBugsData(const bugs::ArtistItemData &data_artist);           // Bugs의 정보로 세팅

    void setFavoritesIds_forTidal(QVariantList p_arr_myFavoriteIds) override;
    void setFavoritesIds_forQobuz(QVariantList p_arr_myFavoriteIds) override;
    void setFavorites_forQobuz(const bool flag);
    void updateDate_artist_bioText(const QString& bioText);


private:
    QString artist_bioText;

};


#endif // ARTISTIMAGEDETAILINFO_H
