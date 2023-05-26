#ifndef QOBUZHISTORYDETAIL_H
#define QOBUZHISTORYDETAIL_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "roseHome/ItemAlbum_rosehome.h"
#include "roseHome/ItemPlaylist_rosehome.h"
#include "roseHome/ItemArtist_rosehome.h"

#include "common/linker.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"


namespace qobuz {

    /**
     * @brief Histrory list의 상세보기 화면 클래스
     */
    class QobuzHistoryDetail : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzHistoryDetail(QWidget *parent = nullptr);
        ~QobuzHistoryDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    protected slots:
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_historyPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_historyAlbum(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_historyTrack(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_historyArtist(const QList<roseHome::ArtistItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject&);

    private:
        // 초기 UI화면 구성
        void setUIControl_appendWidget_hitsory();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);

    private:
        Linker *linker;

        QLabel *label_mainTitle;

        QVBoxLayout *box_rose_contents;
        QWidget *widget_rose_contents;

        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        roseHome::ItemPlaylist_rosehome *home_history_playlist[15];
        roseHome::ItemAlbum_rosehome *home_history_album[15];
        roseHome::ItemArtist_rosehome *home_history_artist[15];

        PlaylistTrackDetailInfo_RHV *home_history_track[5];

        QWidget *widget_historyPlaylist;
        QWidget *widget_historyAlbum;
        QWidget *widget_historyTrack;
        QWidget *widget_historyArtist;

        QVBoxLayout *vBox_historyPlaylist;
        QVBoxLayout *vBox_historyAlbum;
        QVBoxLayout *vBox_historyTrack;
        QVBoxLayout *vBox_historyArtist;

        QHBoxLayout *hBox_historyPlaylist;
        QHBoxLayout *hBox_historyAlbum;
        QHBoxLayout *hBox_historyArtist;

        // data
        QList<roseHome::PlaylistItemData> *list_historyPlaylist;
        QList<roseHome::AlbumItemData> *list_historyAlbum;
        QList<roseHome::TrackItemData> *list_historyTrack;
        QList<roseHome::ArtistItemData> *list_historyArtist;

        QJsonArray jsonArr_tracks_toPlay;

        roseHome::HistoryItemData historyData;

        bool flag_request_api = false;

        bool flag_historyAlbum[2] = {false, false};
        bool flag_historyPlaylist[2] = {false, false};
        bool flag_historyTrack[2] = {false, false};
        bool flag_historyArtist[2] = {false, false};

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;

        bool flag_track_fav = false;
        bool flag_send_track = false;
        bool flag_check_track = false;
    };
};
#endif // QOBUZHISTORYDETAIL_H
