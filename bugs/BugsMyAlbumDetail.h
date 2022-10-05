#ifndef BUGSMYALBUMDETAIL_H
#define BUGSMYALBUMDETAIL_H

#include "AbstractBugsSubWidget.h"
#include <QListWidget>
#include "delegate/BugsTrackDelegate.h"

namespace bugs {

    /**
     * @brief 벅스 > My 앨범 상세화면 클래스
     */
    class BugsMyAlbumDetail : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsMyAlbumDetail(QWidget *parent = nullptr);
        ~BugsMyAlbumDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void applyLoginSuccessState() override;

        void proc_wheelEvent_to_getMoreData() override;

    protected slots:
        void slot_btnClicked_playAll() override;
        void slot_btnClicked_playShuffle() override;

        void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode) override;

        void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_thumbnailDownloaded_fotListWidget_delegate() override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void setUIControl_logined() override;
        void proc_preUpdate_favorite_track(const ItemPositionData&) override;

    private:
        // 메인 타이틀
        QLabel *label_mainTitle;

        bugs::MyAlbumItemData data_my_album;

        // getting more data
        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;
        void request_more_trackData();

        // track list
        BugsTrackDelegate *delegate_track;
        QListWidget *listWidget_track;                      ///< Track 목록을 담기 위함
        QList<bugs::TrackItemData> *list_track;             ///< My Album 내의 Track 정보
        QJsonArray jsonArr_tracks_toPlay;                   ///< Track 전체를 재생하기 위함

    };

};

#endif // BUGSMYALBUMDETAIL_H
