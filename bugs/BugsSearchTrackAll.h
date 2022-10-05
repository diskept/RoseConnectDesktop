#ifndef BUGSSEARCHTRACKALL_H
#define BUGSSEARCHTRACKALL_H

#include "AbstractBugsSubWidget.h"
#include "delegate/BugsTrackDelegate.h"
#include <QListWidget>
#include "common/common_struct.h"


namespace bugs {

    /**
     * @brief 검색결과 - Track 전체 목록 클래스
     */
    class BugsSearchTrackAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsSearchTrackAll(QWidget *parent = nullptr);
        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void applyLoginSuccessState() override;


    protected:
        void setUIControl_logined() override;
        void proc_preUpdate_favorite_track(const ItemPositionData&) override;

        void proc_wheelEvent_to_getMoreData() override;


    protected slots:

        void slot_btnClicked_playAll() override;

        void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool) override;

        void slot_clickedItemTrack_inList(const int, const bugs::BugsTrackDelegate::ClickMode) override;
        void slot_thumbnailDownloaded_fotListWidget_delegate() override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;


    private:

        common::SearchData data_search;

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


#endif // BUGSSEARCHTRACKALL_H
