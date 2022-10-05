#ifndef BUGSALBUMDETAIL_H
#define BUGSALBUMDETAIL_H

#include "AbstractBugsSubWidget.h"
#include <QListWidget>
#include "widget/AlbumImageDetailInfo.h"
#include "delegate/BugsAlbumTrackDelegate.h"

namespace bugs {

    /**
     * @brief 벅스 일반앨범 상세 페이지에 대한 클래스
     */
    class BugsAlbumDetail : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsAlbumDetail(QWidget *parent = nullptr);
        ~BugsAlbumDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void applyLoginSuccessState() override;

    protected slots:

        // AlbumImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailInfo::BtnClickMode) override;
        void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode) override;
        void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_applyResult_albumInfo(bugs::AlbumItemData data_album);
        void slot_applyResult_getShareLink(const QString &link);//c220818
        void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&);

    protected:
        void setUIControl_logined() override;
        void proc_preUpdate_favorite_track(const ItemPositionData&) override;
        void proc_preUpdate_favorite_album(const ItemPositionData&) override;

    private:
        // 메인 타이틀
        QLabel *label_mainTitle;


        // album detail
        bugs::AlbumItemData data_album;
        AlbumImageDetailInfo *albumImageDetailInfo;

        // track list
        BugsAlbumTrackDelegate *delegate_track;
        QListWidget *listWidget_track;                    ///< Track 목록을 담기 위함
        QList<bugs::TrackItemData> *list_track;    ///< Album 내의 Track 정보
        QJsonArray jsonArr_tracks_toPlay;                  ///< Track 전체를 재생하기 위함


    };

};


#endif // BUGSALBUMDETAIL_H
