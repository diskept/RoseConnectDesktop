#ifndef ROSEHOMETRACKLISTALL_H
#define ROSEHOMETRACKLISTALL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QCoreApplication>


namespace roseHome {

    /**
     * @brief RoseHome의 트랙 전체보기 화면 클래스
     */
    class RoseHomeTrackListAll : public AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseHomeTrackListAll(QWidget *parent = nullptr);
        ~RoseHomeTrackListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

        void resizeEvent(QResizeEvent *event) override;

    protected slots:
        void slot_applyResult_tracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_tracklistCheck(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject&);
        void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        void slot_btnClicked_playAll();
        void slot_btnClicked_playShuffle();

        void slot_btnClicked_Delete();

    private:
        void setUIControl_tracks();
        void setUIControl_filter();

        void request_more_trackData();
        void request_more_trackDraw();

        void initAll();

    private:
        QWidget *widget_mainTitle;

        QLabel *label_mainTitle;
        QPushButton *btn_mainTitle;
        QLabel *label_delete_icon;
        QLabel *label_delete;

        PlaylistTrackDetailInfo_RHV *track_listAll[999999];

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vBox_tracks;

        // data
        QList<roseHome::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

        PageInfo_TrackAllView current_pageInfo;

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;

        bool flag_widget_visible = false;

        bool flag_draw = false;

        int next_offset = 0;
        int track_totalCount = 0;
        int track_drawCount = 0;
        int track_favoriteOffset = 0;

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };





    /**
     * @brief RoseHome의 트랙 전체보기 화면 클래스
     */
    class RoseHomeTrackHistoryAll : public AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseHomeTrackHistoryAll(QWidget *parent = nullptr);
        ~RoseHomeTrackHistoryAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    protected slots:
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_historyTrack(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject&);
        void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        void slot_btnClicked_playAll();
        void slot_btnClicked_playShuffle();

    private:
        void setUIControl_tracks();

        void request_more_trackData();
        void request_more_trackDraw();

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

        PageInfo_TrackAllView data_pageInfo;

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;

        bool flag_widget_visible = false;

        bool flag_draw = false;

        int next_offset = 0;
        int track_totalCount = 0;
        int track_drawCount = 0;
        int track_favoriteOffset = 0;

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };
};

#endif // ROSEHOMETRACKLISTALL_H
