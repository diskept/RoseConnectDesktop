#ifndef BUGSTRACKLISTALL_OFARTIST_H
#define BUGSTRACKLISTALL_OFARTIST_H

#include "AbstractBugsSubWidget.h"
#include <QListWidget>
#include "widget/ArtistImageDetailInfo.h"
#include "delegate/BugsTrackDelegate.h"

namespace bugs {

    /**
     * @brief 벅스 Artist의 전체 트랙 목록 화면 클래스
     */
    class BugsTrackListAll_ofArtist : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsTrackListAll_ofArtist(QWidget *parent = nullptr);
        ~BugsTrackListAll_ofArtist();
        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void applyLoginSuccessState() override;

    protected slots:
        void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_thumbnailDownloaded_fotListWidget_delegate() override;

        void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;


    protected:
        void setUIControl_logined() override;
        void proc_preUpdate_favorite_track(const ItemPositionData&) override;
        void proc_wheelEvent_to_getMoreData() override;

    private:

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;
        void request_more_trackData();


        bugs::PageInfo_AlbumAllView_ofArtist data_pageInfo;

        // UI
        QLabel *label_mainTitle;

        // Track
        QListWidget *listWidget_track;
        BugsTrackDelegate *delegate;
        QList<bugs::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;

    };

};
#endif // BUGSTRACKLISTALL_OFARTIST_H
