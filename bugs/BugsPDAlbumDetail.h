#ifndef BUGSPDALBUMDETAIL_H
#define BUGSPDALBUMDETAIL_H

#include "AbstractBugsSubWidget.h"
#include <QListWidget>
#include "widget/PDAlbumImageDetailInfo.h"
#include "delegate/BugsTrackDelegate.h"

namespace bugs {

    /**
     * @brief PD앨범에 대한 상세화면 클래스
     */
    class BugsPDAlbumDetail : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsPDAlbumDetail(QWidget *parent = nullptr);
        ~BugsPDAlbumDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void applyLoginSuccessState() override;

    protected slots:

        // PDAlbumImageDetailInfo signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailInfo::BtnClickMode) override;
        void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode) override;

        void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_thumbnailDownloaded_fotListWidget_delegate() override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_applyResult_pd_albumInfo(bugs::PD_AlbumItemData data_pd_album);
        void slot_applyResult_getShareLink(const QString&);         //j220905 share link

        // PDAlbumImageDetailInfo signal 처리 - tag 버튼 클릭
        void slot_pdAlbumInfo_tagBtnClicked(int index_tag);

    protected:
        void setUIControl_logined() override;
        void proc_preUpdate_favorite_track(const ItemPositionData&) override;
        void proc_preUpdate_favorite_pd_album(const ItemPositionData&) override;

    private:
        // 메인 타이틀
        QLabel *label_mainTitle;

        // pd_album detail
        bugs::PD_AlbumItemData data_pd_album;
        bugs::PDAlbumImageDetailInfo *pd_albumImageDetailInfo;

        // track list
        BugsTrackDelegate *delegate_track;
        QListWidget *listWidget_track;                    ///< Track 목록을 담기 위함
        QList<bugs::TrackItemData> *list_track;    ///< Album 내의 Track 정보
        QJsonArray jsonArr_tracks_toPlay;                  ///< Track 전체를 재생하기 위함


    };

};


#endif // BUGSPDALBUMDETAIL_H
