#ifndef BUGSMYCOLLECTION_H
#define BUGSMYCOLLECTION_H

#include "bugs/AbstractBugsSubWidget.h"
#include "delegate/BugsTrackDelegate.h"
#include <QWidget>
namespace bugs {

    /**
     * @brief 벅스 > MyCollection 에 대한 클래스
     */
    class BugsMyCollection : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsMyCollection(QWidget *parent = nullptr);

        void applyLoginSuccessState() override;


    protected:
        void setUIControl_logined() override;
        void proc_preUpdate_favorite_track(const ItemPositionData&) override;


    protected slots:
        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_side(const int) override;

        void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_my_albums(const QList<bugs::MyAlbumItemData>&, const bool) override;

        void slot_clickedItemTrackSqure(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode) override;
        void slot_thumbnailDownloaded_fotListWidget_delegate() override;

        void slot_clickedItemMyAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;


    private:
        // 좋아한 음악
        bool flagReq_like = false;
        QHBoxLayout *hBox_like;
        QList<bugs::TrackItemData> *list_track_like;
        QJsonArray jsonArr_tracks_toPlay_like;

        // 최근 들은 곡
        bool flagReq_recently = false;
        BugsTrackDelegate *delegate_recently;
        QListWidget *listWidget_track_recently;
        QList<bugs::TrackItemData> *list_track_recently;
        QJsonArray jsonArr_tracks_toPlay_recently;

        // 많이 들은 곡
        bool flagReq_most = false;
        BugsTrackDelegate *delegate_most;
        QListWidget *listWidget_track_most;
        QList<bugs::TrackItemData> *list_track_most;
        QJsonArray jsonArr_tracks_toPlay_most;

        // 내 앨범
        bool flagReq_my_album = false;
        FlowLayout *flowLayout_album_my;
        QList<bugs::MyAlbumItemData> *list_album_my;

    };

};

#endif // BUGSMYCOLLECTION_H
