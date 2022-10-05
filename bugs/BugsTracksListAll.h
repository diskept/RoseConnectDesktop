#ifndef BUGSTRACKSLISTALL_H
#define BUGSTRACKSLISTALL_H

#include "AbstractBugsSubWidget.h"
#include "delegate/BugsTrackDelegate.h"
#include <QListWidget>


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
//        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)
        void applyLoginSuccessState() override;


    protected:
        void setUIControl_logined() override;
        void proc_preUpdate_favorite_track(const ItemPositionData&) override;

        void proc_wheelEvent_to_getMoreData() override;


    protected slots:
        void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemTrack_inList(const int, const bugs::BugsTrackDelegate::ClickMode) override;

        void slot_thumbnailDownloaded_fotListWidget_delegate() override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

        void slot_btnClicked_playAll() override;


    private:

        bugs::PageInfo_TrackAllView data_pagaInfo;

        QWidget *widget_mainTitle;
        QLabel *label_mainTitle;


        // getting more data
        bool flagReqMore_track = false;             ///< 현재 get more data 처리중 상태인지의 여부 (중복 요청 방지용) : track
        bool flag_lastPage_track = false;
        void request_more_trackData();              ///< function - get more data : track

        QListWidget *listWidget_track;
        BugsTrackDelegate *delegate;
        QList<bugs::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

    };

};


#endif // BUGSTRACKSLISTALL_H
