#ifndef TIDALTRACKLISTALL_SHARE_H
#define TIDALTRACKLISTALL_SHARE_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemVideo.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QCoreApplication>


namespace tidal {

    /**
     * @brief Tidal의 공유트랙 보기 화면 클래스
     */
    class TidalTrackListAll_Share : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalTrackListAll_Share(QWidget *parent = nullptr);
        ~TidalTrackListAll_Share();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    protected slots:
        void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_videos(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage) override;

        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        void slot_btnClicked_playAll();
        void slot_btnClicked_playShuffle();

    private:
        void setUIControl_tracks();
        void setUIControl_filter();

        void request_more_trackData();

    private:
        QLabel *label_mainTitle;
        PlaylistTrackDetailInfo_RHV *track_listAll[999999];
        tidal::ItemVideo *listVideo_all[999999];

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vBox_tracks;

        FlowLayout *flowLayout_track;
        QWidget *widget_share_content;

        // data
        QList<tidal::TrackItemData> *list_track;
        QList<tidal::VideoItemData> *list_video;

        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

        QString type = "";
        QString subType = "";
        QString srtID = "";
        int revID = 0;

        QString title = "";

        int track_totalCount;

        bool flag_widget_visible = false;

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };
};
#endif // TIDALTRACKLISTALL_SHARE_H
