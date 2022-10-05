#ifndef MUSICSEARCHMAIN_H
#define MUSICSEARCHMAIN_H

#include "common/linker.h"
#include "common/common_struct.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"
#include "roseHome/ItemAlbum_rosehome.h"
#include "roseHome/ItemArtist_rosehome.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"


/**
 * @brief 음악-검색결과 화면 @n
 *      음악 검색결과를 트랙, 앨범, 아티스트 항목으로 Show @n
 *      각 항목의 최소 갯수만 보여주며, 각 항목 View All 클릭 시 전체 항목 Show
 */
namespace music {

    class MusicSearch : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit MusicSearch(QWidget *parent = nullptr);
        ~MusicSearch();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        // about Item
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;

        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_checkTrackId(const QJsonObject&);
        void slot_applyResult_getTrckRating(const QJsonArray&);
        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_addRating_track(const QJsonObject&);

    private:
        void setUIControl_basic();

        void setUIControl_requestArtist();
        void setUIControl_requestAlbum();
        void setUIControl_requestTrack();

        void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forArtist(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);

        QJsonArray getDataForPlayMusic(const int);

        int get_rose_playType(OptMorePopup::ClickMode clickMode);
        QJsonArray get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode);
        QJsonArray subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex);
        QJsonArray reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex);

    private:
        common::SearchData data_search;

        // UI
        QLabel *label_mainTitle;

        QLabel *lb_subTitle[5];
        QPushButton *btnView_all[5];

        roseHome::ItemArtist_roseHome *search_artist[15];
        roseHome::ItemAlbum_rosehome *search_album[15];

        PlaylistTrackDetailInfo_RHV *search_track[5];

        QVBoxLayout *box_search_contents;

        QWidget *widget_search_contents;

        QWidget *widget_search_artist;
        QWidget *widget_search_album;
        QWidget *widget_search_track;

        QVBoxLayout *vBox_search_artist;
        QVBoxLayout *vBox_search_album;
        QVBoxLayout *vBox_search_track;

        QHBoxLayout *hBox_search_artist;
        QHBoxLayout *hBox_search_album;

        QList<roseHome::ArtistItemData> *list_search_artist;
        QList<roseHome::AlbumItemData> *list_search_album;
        QList<roseHome::TrackItemData> *list_search_track;

        QJsonArray jsonArr_artists;
        QJsonArray jsonArr_albums;
        QJsonArray jsonArr_tracks;

        QJsonArray jsonArr_tracks_toPlay;

        int search_track_count = 0;
        int track_svr_idx = 0;

        bool flag_artist = false;
        bool flag_album = false;
        bool flag_track = false;

        int track_idx_fav = 0;
        int track_star_fav = 0;

        bool flag_track_fav = false;
        bool flag_send_track = false;
        bool flag_check_track = false;
    };
};

#endif // MUSICSEARCHMAIN_H
