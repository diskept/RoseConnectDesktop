#ifndef BUGSMYCOLLECTION_H
#define BUGSMYCOLLECTION_H

#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/ItemTrackSqure_bugs.h"
#include "bugs/ItemMyAlbum_bugs.h"
#include "bugs/BugsSettings.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QWidget>
namespace bugs {

    /**
     * @brief 벅스 > MyCollection 에 대한 클래스
     */
    class BugsMyCollection : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsMyCollection(QWidget *parent = nullptr);
        ~BugsMyCollection();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_applyResult_trackLike(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_trackRecently(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_trackMost(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_my_albums(const QList<bugs::MyAlbumItemData>&, const bool) override;

        void slot_clickedItemTrackSqure(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode) override;
        void slot_clickedItemMyAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_applyResult_getShareLink(const QString &link);

        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject&);


    private:
        void setUIControl_requestBugs();

        void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forPlaylist(QLayout *p_layout);
    private:
        Linker *linker;//cheon211008

        // 관리 필요한 Layout UI
        bugs::BugsSettings *bugsSettings;

        QString page = "";

        QLabel *lb_subTitle[20];
        QPushButton *btnView_all[20];

        QWidget *widget_login_contents;

        //===============================================================================

        bugs::ItemTrackSqure_bugs *myCollection_track_like[15];
        PlaylistTrackDetailInfo_RHV *myCollection_track_recently[5];
        PlaylistTrackDetailInfo_RHV *myCollection_track_most[5];
        bugs::ItemMyAlbum_bugs *myCollection_album_myAlbum[15];

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QWidget *widget_trackLike;
        QWidget *widget_trackRecently;
        QWidget *widget_trackMost;
        QWidget *widget_albumMyalbum;

        QVBoxLayout *vBox_trackLike;
        QVBoxLayout *vBox_trackRecently;
        QVBoxLayout *vBox_trackMost;
        QVBoxLayout *vBox_albumMyalbum;

        QHBoxLayout *hBox_trackLike;
        QHBoxLayout *hBox_trackRecently;
        QHBoxLayout *hBox_trackMost;
        QHBoxLayout *hBox_albumMyalbum;

        QJsonArray jsonArr_trackLike_toPlay;
        QJsonArray jsonArr_trackRecently_toPlay;
        QJsonArray jsonArr_trackMost_toPlay;


        QList<bugs::TrackItemData> *list_trackLike;
        QList<bugs::TrackItemData> *list_trackRecently;
        QList<bugs::TrackItemData> *list_trackMost;
        QList<bugs::MyAlbumItemData> *list_albumMyalbum;

        bool flag_trackLike[2] = {false, false};
        bool flag_trackRecently[2] = {false, false};
        bool flag_trackMost[2] = {false, false};
        bool flag_albumMyalbum[2] = {false, false};

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;


    };

};

#endif // BUGSMYCOLLECTION_H
