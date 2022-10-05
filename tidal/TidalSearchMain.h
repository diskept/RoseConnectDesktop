#ifndef TIDALSEARCHMAIN_H
#define TIDALSEARCHMAIN_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemAlbum.h"
#include "tidal/ItemArtist.h"
#include "tidal/ItemPlaylist.h"

#include "common/common_struct.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief 검색결과 - 검색어에 대한 검색결과화면 클래스 (Tidal)
     */
    class TidalSearchMain : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:

        explicit TidalSearchMain(QWidget *parent = nullptr);
        ~TidalSearchMain();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_applyResult_topHit(const QList<tidal::ArtistItemData>&, const bool) override;
        void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_artists(const QList<tidal::ArtistItemData>&, const bool) override;
        void slot_applyResult_albums(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_playlists(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_videos(const QList<tidal::VideoItemData>&, const bool) override;

        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode) override;

        // about Item
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&);

    private :
        void setUIControl_basic();
        void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forArtist(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forPlaylist(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forVideo(QLayout *p_layout);

    private:
        common::SearchData data_search;

        PlaylistTrackDetailInfo_RHV *search_track[5];

        tidal::ItemArtist *search_topHit;

        tidal::ItemArtist *search_artist[15];
        tidal::ItemAlbum *search_album[15];
        tidal::ItemPlaylist *search_playlist[15];
        tidal::ItemVideo *search_video[15];

        // UI
        QLabel *label_mainTitle;

        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QWidget *widget_topHit;
        QWidget *widget_artist;
        QWidget *widget_album;
        QWidget *widget_track;
        QWidget *widget_playlist;
        QWidget *widget_video;

        QVBoxLayout *vBox_topHit;
        QVBoxLayout *vBox_artist;
        QVBoxLayout *vBox_album;
        QVBoxLayout *vBox_track;
        QVBoxLayout *vBox_playlist;
        QVBoxLayout *vBox_video;

        QHBoxLayout *hBox_topHit;
        QHBoxLayout *hBox_artist;
        QHBoxLayout *hBox_album;
        QHBoxLayout *hBox_playlist;
        QHBoxLayout *hBox_video;

        QList<tidal::ArtistItemData> *list_topHit;
        QList<tidal::ArtistItemData> *list_artist;
        QList<tidal::AlbumItemData> *list_album;
        QList<tidal::TrackItemData> *list_track;
        QList<tidal::PlaylistItemData> *list_playlist;
        QList<tidal::VideoItemData> *list_video;

        QJsonArray jsonArr_tracks_toPlay;

        bool flag_topHit[2] = {false, false};
        bool flag_artist[2] = {false, false};
        bool flag_album[2] = {false, false};
        bool flag_track[2] = {false, false};
        bool flag_playlist[2] = {false, false};
        bool flag_video[2] = {false, false};

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };

}


#endif // TIDALSEARCHMAIN_H
