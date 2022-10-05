#ifndef ABSTRACTALBUMIMAGEDETAILINFO_RHV_H
#define ABSTRACTALBUMIMAGEDETAILINFO_RHV_H

#include "widget/AbstractImageDetailInfo_RHV.h"
#include "widget/AbstractAlbumTrackDetailInfo_RHV.h"

#include "roseHome/rosehome_struct.h"
#include "tidal/AbstractItem.h"
#include "tidal/tidal_struct.h"
#include "qobuz/qobuz_struct.h"
#include "bugs/bugs_struct.h"

/**
 * @brief 앨범 상세뷰에서 상단에 이미지와 앨범 타이틀, 아티스트 등의 정보가 보이는 Widget
 */
class AlbumImageDetailInfo_RHV : public AbstractImageDetailInfo_RHV
{
    Q_OBJECT

public:
    explicit AlbumImageDetailInfo_RHV(ContentsUIType p_ContentsUIType, QWidget *parent = nullptr);
    ~AlbumImageDetailInfo_RHV();

    void setMyData(bool flagMine) override;
    void initView() override;           // 기본
    void initView_forBugs();            // only Bugs

    void setData_fromTidalData(const tidal::AlbumItemData &data_album);                     // Tidal의 정보로 세팅
    void setData_fromBugsData(const bugs::AlbumItemData &data_album);                       // Bugs의 정보로 세팅

    void setData_fromQobuzData_album(const qobuz::AlbumItemData &data_album);                     // Qobuz의 album 정보로 세팅
    void setData_fromQobuzData_track(const QList<qobuz::TrackItemData>&, const QJsonArray&);      // Qobuz의 track 정보로 세팅
    void setData_fromQobuzData_credit(const QList<qobuz::TrackItemData>&, const QJsonArray&);      // Qobuz의 credit 정보로 세팅
    void setData_fromQobuzData_artist(const QList<qobuz::AlbumItemData>&);                         // Qobuz의 same Artist 정보 세팅

    void setData_fromRoseData(const roseHome::AlbumItemData &data_album);                   // Rose server의 album 정보로 세팅

    void setFavoritesIds_forTidal(QVariantList p_arr_myFavoriteIds) override;
    void setFavoritesIds_forQobuz(QVariantList p_arr_myFavoriteIds) override;
    void setFavorites_forQobuz(const bool flag);

    void setFavoritesIds(bool p_myFavoriteIds, int p_star) override;
    bool getFavoritesIds() override;
    int getFavoritesStars() override;

    void updateDate_playlist_bioText(const QString &descriptionText);                       //cheon210605

    AlbumTrackDetailInfo_RHV *albumTrackDetailInfo_RHV[1000];
    AlbumTrackDetailInfoCredit_RHV *albumTrackDetailInfoCredit_RHV[1000];

signals:
    void complete_applying_track();
    void list_Clicked(const int, const AlbumTrackDetailInfo_RHV::ClickMode);
    void album_Clicked(const tidal::AbstractItem::ClickMode);
    void sameArtist_Click(const int, const int, const int);
    void sameArtist_btnVeiwAll_Click(const QString);

public slots:
    void slot_changedSubTabUI(const QJsonObject &p_data);

private slots:
    void slot_detailInfo_btnClicked(const int, const AlbumTrackDetailInfo_RHV::ClickMode);
    void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode);
    void slot_clickBtn_subTitle_viewAll();

private:
    void setUIControl_trackList();
    void setUIControl_sameArtist();
    void changedOnlyTabUI_notSendSignal(QString p_step);

    void setUIControl_subTitle_withSideBtn(const QString, const QString);
    QHBoxLayout* setUIControl_hBoxLayout_forAlbum();

private:    
    TopMenuBar *menubar;
    QWidget *widget_tracks_header;
    QWidget *widget_credit_header;
    QVBoxLayout *vbox_trackList;

    QVBoxLayout *vbox_contents;
    QHBoxLayout *hBox_sameArtist;

    QLabel *lb_subTitle;
    QPushButton *btnView_all;

    bool flagNeededInit;
    bool flag_trackList = false;
    bool flag_sameArtist = false;

    QString playlist_descriptionText="";

};

#endif // ALBUMIMAGEDETAILINFO_RHV_H
