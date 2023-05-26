#ifndef BUGSTRACKLISTALL_SHARE_H
#define BUGSTRACKLISTALL_SHARE_H

#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/ItemVideo_bugs.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QCoreApplication>


namespace bugs {

    /**
     * @brief Bugs의 공유트랙 보기 화면 클래스
     */
    class BugsTrackListAll_Share : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsTrackListAll_Share(QWidget *parent = nullptr);
        ~BugsTrackListAll_Share();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

        void applyLoginSuccessState() override;

    protected slots:
        void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;

        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;
        void slot_applyResult_videoRecently(const QList<bugs::VideoItemData>&, const QJsonArray&, const bool);

        void slot_btnClicked_playAll() override;
        void slot_btnClicked_playShuffle() override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        //void slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject&);

    private:
        void setUIControl_tracks();
        void setUIControl_filter();

        void request_more_trackData();

    private:
        QLabel *label_mainTitle;
        PlaylistTrackDetailInfo_RHV *track_listAll[999999];
        bugs::ItemVideo_bugs *listVideo_all[999999];

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vBox_tracks;

        FlowLayout *flowLayout_track;
        QWidget *widget_share_content;

        // data
        QList<bugs::TrackItemData> *list_track;
        QList<bugs::VideoItemData> *list_video;

        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

        QString type = "";
        QString subType = "";
        QString str_id = "";
        int int_id = 0;

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
#endif // BUGSTRACKLISTALL_SHARE_H
