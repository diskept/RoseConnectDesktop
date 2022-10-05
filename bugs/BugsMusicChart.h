#ifndef BUGSMUSICCHART_H
#define BUGSMUSICCHART_H

#include "bugs/AbstractBugsSubWidget.h"
#include <QWidget>
#include "delegate/BugsTrackDelegate.h"
#include "widget/FilterWidget.h"
#include "widget/SegmentTabWidget.h"
#include "BugsChooseFilterOpt.h"
#include "widget/StackedWidget_AR.h"

namespace bugs {

    /**
     * @brief 벅스 > Home > 벅스 차트에 대한 클래스
     */
    class BugsMusicChart : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsMusicChart(QWidget *parent = nullptr);

        void applyLoginSuccessState() override;
        void setActivePage() override;

        void resizeEvent(QResizeEvent *event) override;         // filter Box 사이즈 조절을 위해

    protected:
        void setUIControl_logined() override;
        void proc_preUpdate_favorite_track(const ItemPositionData&) override;
        void proc_preUpdate_favorite_album(const ItemPositionData&) override;

        void proc_wheelEvent_to_getMoreData() override;


    protected slots:
        void slot_btnClicked_playAll() override;
        void slot_btnClicked_playShuffle() override;

        // about 서브 타이틀의 버튼
        void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_albums(const QList<bugs::AlbumItemData>&, const bool) override;
        void slot_applyResult_videos(const QList<bugs::VideoItemData>&, const QJsonArray&, const bool) override;

        void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode) override;
        void slot_thumbnailDownloaded_fotListWidget_delegate() override;

        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);
        void slot_segmentTag_clicked(int clickedIndex);


    private:
        // function
        void setUIControl_filterAndTab();

        void initAll();

        void setUIControl_body_Track();
        void setUIControl_body_Album();
        void setUIControl_body_Video();


        QStackedWidget *stackedWidget_content;

        // Filtering
        QVariant selected_filterCode;           // 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수


        int selected_tabIndex;     ///< 현재 몇번째 tab 인덱스 보고 있는지
        SegmentTabWidget *segmentTabWidget;


        // getting more data
        bool flagReqMore_track = false;             ///< 현재 get more data 처리중 상태인지의 여부 (중복 요청 방지용) : track
        bool flag_lastPage_track = false;
        void request_more_trackData();              ///< function - get more data : track

        bool flagReqMore_album = false;             ///< 현재 get more data 처리중 상태인지의 여부 (중복 요청 방지용) : album
        bool flag_lastPage_album = false;
        void request_more_albumData();              ///< function - get more data : album

        bool flagReqMore_video = false;             ///< 현재 get more data 처리중 상태인지의 여부 (중복 요청 방지용) : video
        bool flag_lastPage_video = false;
        void request_more_videoData();              ///< function - get more data : video


        // 최신 음악 > 트랙
        BugsTrackDelegate *delegate;
        QListWidget *listWidget_track;
        QList<bugs::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;
        VerticalScrollArea *scrollArea_track;
        QWidget *widget_inScroll_track;

        // 최신 음악 > 앨범
        QList<bugs::AlbumItemData> *list_album;
        FlowLayout *flowLayout_album;
        VerticalScrollArea *scrollArea_album;
        QWidget *widget_inScroll_album;

        // 최신 음악 > 영상
        QList<bugs::VideoItemData> *list_video;
        FlowLayout *flowLayout_video;
        QJsonArray jsonArr_videos_toPlay;
        VerticalScrollArea *scrollArea_video;
        QWidget *widget_inScroll_video;


    };

};

#endif // BUGSMUSICCHART_H
