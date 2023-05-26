#ifndef BUGSPDALBUMDETAIL_H
#define BUGSPDALBUMDETAIL_H

#include "bugs/AbstractBugsSubWidget.h"

#include "widget/AbstractImageDetailContents_RHV.h"

#include <QCoreApplication>
#include <QListWidget>


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
        void setActivePage() override;

    protected slots:
        // ArtistImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;

        void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_applyResult_pd_albumInfo(bugs::PD_AlbumItemData data_pd_album);
        void slot_add_rosePlaylist_withBugs(const int&, const QJsonObject&);
        void slot_applyResult_getShareLink(const QString&);         //j220905 share link

        // PDAlbumImageDetailInfo signal 처리 - tag 버튼 클릭
        void slot_clickedItempdAlbum_inTag(const int);

        void slot_applyResult_checkRating_pdAlbum(const QJsonObject&);
        void slot_applyResult_getRating_pdAlbum(const QJsonArray&);
        void slot_applyResult_addRating_pdAlbum(const QJsonObject&);

        void slot_bugs_completeReq_pdAlbum_myFavoritesIds(const QJsonObject&);

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void slot_bugs_completeReq_track_myFavoritesIds(const QJsonObject&);

        void slot_listwidgetItemPressed(QListWidgetItem *item);

        void slot_clicked_btnVideolist();
        void slot_clicked_btnFavorite();
        void slot_clicked_btnMenu();

        void slot_thumbnailDownload_list();

    private:
        // function about UI
        void setUIControl_basic();

        inline void iniDataInfo_pdAlbumList_Item(const bugs::TrackItemData &data_track, const int &p_index);
        inline void setDataInfo_pdAlbumList_Item(const bugs::TrackItemData &data_track, const int &p_index);

    private:
        Linker *linker;

        AbstractImageDetailContents_RHV *pd_album_detail_info;

        // UI
        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QListWidget *listWidget_contents;

        QLabel *lb_thumb[999];
        QLabel *lb_explicit[999];
        QLabel *lb_title[999];
        QLabel *lb_album[999];
        QLabel *lb_resolution[999];
        QLabel *lb_artist[999];
        QLabel *lb_duration[999];

        QPushButton *btn_list_fav[999];

        bugs::PD_AlbumItemData data_pd_album;
        QList<bugs::TrackItemData> *list_track;     ///< Album 내의 Track 정보

        QJsonArray jsonArr_tracks_toPlay;           ///< Track 전체를 재생하기 위함

        bool flag_widget_visible = false;

        bool flag_draw = false;

        int track_totalCount = 0;
        int track_drawCount = 0;
        int track_favoriteOffset = 0;

        bool flag_playlist_fav = false;
        bool flag_send_playlist = false;
        bool flag_check_playlist = false;

        int playlist_star_fav = 0;

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav[9999999];
    };
};
#endif // BUGSPDALBUMDETAIL_H
