#ifndef BUGSMYLIKEMUSIC_H
#define BUGSMYLIKEMUSIC_H

#include "bugs/AbstractBugsSubWidget.h"
#include <QWidget>
#include "delegate/BugsTrackDelegate.h"
#include "delegate/BugsArtistDelegate.h"
#include "widget/FilterWidget.h"
#include "widget/SegmentTabWidget.h"
#include "BugsChooseFilterOpt.h"

namespace bugs {

    /**
     * @brief 벅스 > My Collection > 좋아하는 음악 전체관할 Class
     */
    class BugsMyLikeMusic : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsMyLikeMusic(QWidget *parent = nullptr);

        void applyLoginSuccessState() override;
        void setActivePage() override;

        void resizeEvent(QResizeEvent *event) override;         // filter Box 사이즈 조절을 위해

    protected:
        void setUIControl_logined() override;
        void proc_preUpdate_favorite_track(const ItemPositionData&) override;
        void proc_preUpdate_favorite_album(const ItemPositionData&) override;
        void proc_preUpdate_favorite_artist(const ItemPositionData&) override;
        void proc_preUpdate_favorite_pd_album(const ItemPositionData&) override;

        void proc_wheelEvent_to_getMoreData() override;


    protected slots:

        void slot_btnClicked_playAll() override;
        void slot_btnClicked_playShuffle() override;

        // about 서브 타이틀의 버튼
        void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_albums(const QList<bugs::AlbumItemData>&, const bool) override;
        void slot_applyResult_artists(const QList<bugs::ArtistItemData>&, const bool) override;
        void slot_applyResult_pd_albums(const QList<bugs::PD_AlbumItemData>&, const bool) override;

        void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode) override;
        void slot_clickedItemTrack_inList2(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode);
        void slot_thumbnailDownloaded_fotListWidget_delegate() override;

        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemPD_Album(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;


    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);
        void slot_segmentTag_clicked(int clickedIndex);

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject&);


    private:
        // function
        void setUIControl_filterAndTab();

        void initAll();

        void setUIControl_body_Track();
        void setUIControl_body_Album();
        void setUIControl_body_Artist();
        void setUIControl_body_PD_Album();

        QWidget *widget_playBtn;

        QStackedWidget *stackedWidget_content;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;           // 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수


        int selected_tabIndex;     ///< 현재 몇번째 tab 인덱스 보고 있는지
        SegmentTabWidget *segmentTabWidget;


        // getting more data
        bool flagReqMore_track = false;             ///< 현재 get more data 처리중 상태인지의 여부 (중복 요청 방지용) : track
        bool flag_lastPage_track = false;
        void request_more_trackData();              ///< function - get more data : track
        void request_more_trackDraw();

        bool flagReqMore_album = false;             ///< 현재 get more data 처리중 상태인지의 여부 (중복 요청 방지용) : album
        bool flag_lastPage_album = false;
        void request_more_albumData();              ///< function - get more data : album

        bool flagReqMore_artist = false;            ///< 현재 get more data 처리중 상태인지의 여부 (중복 요청 방지용) : artist
        bool flag_lastPage_artist = false;
        void request_more_artistData();             ///< function - get more data : artist

        bool flagReqMore_pd_album = false;            ///< 현재 get more data 처리중 상태인지의 여부 (중복 요청 방지용) : PD's Album
        bool flag_lastPage_pd_album = false;
        void request_more_pd_albumData();             ///< function - get more data : PD's Album


        // 좋아한 음악 > Track
        PlaylistTrackDetailInfo_RHV *track_listAll[999999];
        QList<bugs::TrackItemData> *list_track;
        QVBoxLayout *Vbox_track;
        QJsonArray jsonArr_tracks_toPlay;
        VerticalScrollArea *scrollArea_track;
        QWidget *widget_inScroll_track;

        // 좋아한 음악 > Album
        QList<bugs::AlbumItemData> *list_album;
        FlowLayout *flowLayout_album;
        VerticalScrollArea *scrollArea_album;
        QWidget *widget_inScroll_album;

        // 좋아한 음악 > Artist
        BugsArtistDelegate *delegate_artist;
        QListWidget *listWidget_artist;
        QList<bugs::ArtistItemData> *list_artist;
        VerticalScrollArea *scrollArea_artist;
        QWidget *widget_inScroll_artist;

        // 좋아한 음악 > PD's Album
        QList<bugs::PD_AlbumItemData> *list_pd_album;
        FlowLayout *flowLayout_pd_album;
        VerticalScrollArea *scrollArea_pd_album;
        QWidget *widget_inScroll_pd_album;

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


#endif // BUGSMYLIKEMUSIC_H
