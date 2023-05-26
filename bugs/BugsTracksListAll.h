#ifndef BUGSTRACKSLISTALL_H
#define BUGSTRACKSLISTALL_H

#include "bugs/AbstractBugsSubWidget.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QCoreApplication>


namespace bugs {

    /**
     * @brief 벅스 Track All 화면 클래스
     */
    class BugsTracksListAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsTracksListAll(QWidget *parent = nullptr);
        ~BugsTracksListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    protected slots:
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

        void slot_btnClicked_playAll() override;
        void slot_btnClicked_playShuffle() override;

    private:
        void request_more_trackData();              ///< function - get more data : track
        void request_more_trackDraw();

    private slots:
        void slot_applyResult_recentlyTracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject&);

    private:
        bugs::PageInfo_TrackAllView data_pagaInfo;

        QLabel *label_mainTitle;
        PlaylistTrackDetailInfo_RHV *track_listAll[999999];

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vBox_tracks;

        QList<bugs::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

        // getting more data
        bool flagReqMore_track = false;             ///< 현재 get more data 처리중 상태인지의 여부 (중복 요청 방지용) : track
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
     * @brief 벅스 Track All 화면 클래스
     */
    class BugsMostTracksListAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsMostTracksListAll(QWidget *parent = nullptr);
        ~BugsMostTracksListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    protected slots:
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

        void slot_btnClicked_playAll() override;
        void slot_btnClicked_playShuffle() override;

    private:
        void request_more_trackData();              ///< function - get more data : track
        void request_more_trackDraw();

    private slots:
        void slot_applyResult_mostTracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject&);

    private:
        bugs::PageInfo_TrackAllView data_pagaInfo;

        QLabel *label_mainTitle;
        PlaylistTrackDetailInfo_RHV *track_listAll[999999];

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vBox_tracks;

        QList<bugs::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

        // getting more data
        bool flagReqMore_track = false;             ///< 현재 get more data 처리중 상태인지의 여부 (중복 요청 방지용) : track
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
#endif // BUGSTRACKSLISTALL_H

