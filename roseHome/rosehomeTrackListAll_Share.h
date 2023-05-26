#ifndef ROSEHOMETRACKLISTALL_SHARE_H
#define ROSEHOMETRACKLISTALL_SHARE_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QCoreApplication>


namespace roseHome {

    /**
     * @brief RoseHome의 공유트랙 보기 화면 클래스
     */
    class RoseHomeTrackListAll_Share : public AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseHomeTrackListAll_Share(QWidget *parent = nullptr);
        ~RoseHomeTrackListAll_Share();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    protected slots:
        void slot_applyResult_tracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_btnClicked_playAll();
        void slot_btnClicked_playShuffle();

    private:
        void setUIControl_tracks();
        void setUIControl_filter();

        void request_more_trackData();

        void initAll();

    private:
        QLabel *label_mainTitle;
        PlaylistTrackDetailInfo_RHV *track_listAll[999999];

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vBox_tracks;

        // data
        QList<roseHome::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

        QString type = "";
        QString str_id = "";
        int int_id = 0;

        QString title = "";

        int track_totalCount = 0;

        bool flag_widget_visible = false;

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };
};

#endif // ROSEHOMETRACKLISTALL_SHARE_H
