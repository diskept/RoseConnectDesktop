#ifndef BUGSMYPLAYLISTDETAIL_H
#define BUGSMYPLAYLISTDETAIL_H

#include "bugs/AbstractBugsSubWidget.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QCoreApplication>


namespace bugs {

    /**
     * @brief RoseHome의 Playlist 상세보기 화면
     */
    class BugsPlaylistDetail : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsPlaylistDetail(QWidget *parent = nullptr);
        ~BugsPlaylistDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    signals:
        void signal_applyResult_tracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);

    protected slots:
        // PlaylistImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_getShareLink(const QString &link);//c220818
        void slot_applyResult_playlist(const roseHome::PlaylistItemData&);
        void slot_applyResult_playlist_tracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_getRating_playlist(const QJsonArray&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_getRating_thumbup(const QJsonObject&);

    private:
        void request_more_trackData();
        void request_more_trackDraw();

    private:
        AbstractImageDetailContents_RHV *playlist_detail_info;
        PlaylistTrackDetailInfo_RHV *playlist_track_info[99999];

        // UI
        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        // data
        roseHome::PlaylistItemData data_playlist;
        QList<roseHome::TrackItemData> *list_track;

        QJsonArray jsonArr_tracks_toPlay;                       ///< Track 전체를 재생하기 위함

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;

        bool flag_draw = false;

        int ownerId_chk = 0;

        int track_drawCount = 0;
        int track_totalCount = 0;
        int track_beforeOffset = 0;
        int track_currentOffset = 0;
        int track_favoriteOffset = 0;

        int track_id_fav = 0;
        int track_idx_fav = 0;

        int playlist_star_fav = 0;
        int track_star_fav = 0;

        bool flag_playlist_fav = false;
        bool flag_track_fav = false;
        bool flag_playlist_thumb = false;

        bool flag_send_playlist = false;
        bool flag_check_playlist = false;
        bool flag_send_track = false;
        bool flag_check_track = false;
        bool flag_check_thumb = false;

        bool flag_track_ok = false;
    };
};

#endif // BUGSMYPLAYLISTDETAIL_H
