#ifndef BUGSSEARCHMAIN_H
#define BUGSSEARCHMAIN_H

#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/ItemAlbum_bugs.h"
#include "bugs/ItemArtist_bugs.h"
#include "bugs/ItemPD_Album_bugs.h"
#include "bugs/ItemVideo_bugs.h"

#include "common/common_struct.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
//#include "delegate/BugsTrackDelegate.h"


namespace bugs {

    /**
     * @brief 검색결과 - 검색어에 대한 검색결과화면 클래스 (Bugs)
     */
    class BugsSearchMain : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsSearchMain(QWidget *parent = nullptr);
        ~BugsSearchMain();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

        void applyLoginSuccessState() override;

    protected slots:
        void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_artists(const QList<bugs::ArtistItemData>&, const bool) override;
        void slot_applyResult_albums(const QList<bugs::AlbumItemData>&, const bool) override;
        void slot_applyResult_pd_albums(const QList<bugs::PD_AlbumItemData>&, const bool) override;
        void slot_applyResult_videos(const QList<bugs::VideoItemData>&, const QJsonArray&, const bool) override;

        // about Item
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemPD_Album(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;

        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject&);

    private:
        void setUIControl_basic();
        void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forArtist(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forPDAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forVideo(QLayout *p_layout);

    private:
        common::SearchData data_search;

        PlaylistTrackDetailInfo_RHV *search_track[5];

        bugs::ItemArtist_bugs *search_artist[15];
        bugs::ItemAlbum_bugs *search_album[15];
        bugs::ItemPD_Album_bugs *search_pdAlbum[15];
        bugs::ItemVideo_bugs *search_video[15];

        // UI
        QLabel *label_mainTitle;

        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents = nullptr;        ///< j230407 Removing ghosting effect

        QWidget *widget_artist;
        QWidget *widget_album;
        QWidget *widget_track;
        QWidget *widget_pdAlbum;
        QWidget *widget_video;

        QVBoxLayout *vBox_artist;
        QVBoxLayout *vBox_album;
        QVBoxLayout *vBox_track;
        QVBoxLayout *vBox_pdAlbum;
        QVBoxLayout *vBox_video;

        QHBoxLayout *hBox_artist;
        QHBoxLayout *hBox_album;
        QHBoxLayout *hBox_pdAlbum;
        QHBoxLayout *hBox_video;

        QList<bugs::ArtistItemData> *list_artist;
        QList<bugs::AlbumItemData> *list_album;
        QList<bugs::TrackItemData> *list_track;
        QList<bugs::PD_AlbumItemData> *list_pdAlbum;
        QList<bugs::VideoItemData> *list_video;

        QJsonArray jsonArr_tracks_toPlay;
        QJsonArray jsonArr_videos_toPlay;

        bool flag_artist[2] = {false, false};
        bool flag_album[2] = {false, false};
        bool flag_track[2] = {false, false};
        bool flag_pdAlbum[2] = {false, false};
        bool flag_video[2] = {false, false};

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };

};


#endif // BUGSSEARCHMAIN_H
