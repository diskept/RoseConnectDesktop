#ifndef BUGSARTISTDETAIL_H
#define BUGSARTISTDETAIL_H

#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/ItemAlbum_bugs.h"
#include "bugs/ItemArtist_bugs.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/dialogNotice.h"

#include <QListWidget>


namespace bugs {

    /**
     * @brief 벅스 Artist에 대한 상세 페이지 클래스
     */
    class BugsArtistDetail : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsArtistDetail(QWidget *parent = nullptr);
        ~BugsArtistDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    protected slots:
        // ArtistImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;

        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;

        void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_albums(const QList<bugs::AlbumItemData>&, const bool) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_applyResult_artistInfo(bugs::ArtistItemData data_artist);
        void slot_artistInfo_btnClicked();

        void slot_applyResult_checkRating_artist(const QJsonObject&);
        void slot_applyResult_getRating_artist(const QJsonArray&);
        void slot_applyResult_addRating_artist(const QJsonObject&);

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        void slot_clickBtn_subTitle_viewAll();

    private:
        // function about UI
        void setUIControl_basic();
        void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);

    private:
        Linker *linker;
        DialogNotice *dlgArtistMore;

        AbstractImageDetailContents_RHV *artist_detail_info;
        PlaylistTrackDetailInfo_RHV *artist_tracks[5];

        // UI
        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QWidget *widget_artistTracks;
        QWidget *widget_artistAlbum;
        QWidget *widget_compilationAlbum;

        QVBoxLayout *vBox_artistTracks;
        QVBoxLayout *vBox_artistAlbum;
        QVBoxLayout *vBox_compilationAlbum;

        QHBoxLayout *hBox_artistAlbum;
        QHBoxLayout *hBox_compilationAlbum;

        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        bugs::ItemAlbum_bugs *artistAlbum[15];
        bugs::ItemAlbum_bugs *compliationAlbum[15];

        //data
        bugs::ArtistItemData data_artist;

        QList<bugs::TrackItemData> *list_track;
        QList<bugs::AlbumItemData> *list_aritst_album;
        QList<bugs::AlbumItemData> *list_compilation_album;

        QJsonArray jsonArr_tracks_toPlay;           ///< Track 전체를 재생하기 위함 (list_track 관련)

        bool flag_reload_page = false;
        bool flag_tracks[2] = {false, false};
        bool flag_artistAlbums[2] = {false, false};
        bool flag_compilationAlbums[2] = {false, false};

        bool flag_artist_fav = false;
        bool flag_send_artist = false;
        bool flag_check_artist = false;

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int artist_star_fav = 0;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };

};
#endif // BUGSARTISTDETAIL_H
