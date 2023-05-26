#ifndef BUGSHOME_H
#define BUGSHOME_H

#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/ItemAlbum_bugs.h"
#include "bugs/ItemVideo_bugs.h"
#include "bugs/BugsSettings.h"

#include "roseHome/ItemAlbum_rosehome.h"
#include "roseHome/ItemPlaylist_rosehome.h"
#include "roseHome/ItemArtist_rosehome.h"
#include "roseHome/ItemHistory_rosehome.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include "widget/myqwidget.h"//c220729

#include <QCoreApplication>


namespace bugs {

    /**
     * @brief 벅스 > Home 화면 클래스
     */
    class BugsHome : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsHome(QWidget *parent = nullptr);
        ~BugsHome();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_dialog_share_link(QString link);//c220902_1

        // about Item
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_applyResult_getShareLink(const QString &link);//c220825_1
        void slot_time_out();
        void slot_hide_msg();
        void slot_gotoRoseHome();//c230322_3

        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_trackRecently(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_albumRecently(const QList<bugs::AlbumItemData>&, const bool);
        void slot_applyResult_videoRecently(const QList<bugs::VideoItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_trackBugsChart(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_albumBugsChart(const QList<bugs::AlbumItemData>&, const bool);
        void slot_applyResult_videoBugsChart(const QList<bugs::VideoItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_recentlyAlbum(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_recentlyPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_recentlyTrack(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_recentlyArtist(const QList<roseHome::ArtistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_historylist(const QList<roseHome::HistoryItemData>&, const QJsonArray&);
        void slot_applyResult_myPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_userPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_recentlyAlbumCheck(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_recentlyPlaylistCheck(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_recentlyTrackCheck(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_recentlyArtistCheck(const QList<roseHome::ArtistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_myPlaylistCheck(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_myPlaylistDelete(const QJsonObject&);

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        // about clicks
        void changedOnlyTabUI_notSendSignal(QString p_step);
        void slot_changedSubTabUI(const QJsonObject &p_data);

        void slot_change_device_state(const QString&);

    private:
        void request_login_setSesstionBugs();//c230426
        // 초기 UI화면 구성
        void setUIControl_requestRose();
        void setUIControl_requestBugs();

        void setUIControl_appendWidget_rose();
        void setUIControl_appendWidget();

        void setUIControl_checkWidget_rose();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QWidget* setUIControl_subTitle_withSubMenu(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forVideo(QLayout *p_layout);

    private:
        Linker *linker;//cheon211008

        // 관리 필요한 Layout UI
        bugs::BugsSettings *bugsSettings;

        QString page = "";

        QLabel *lb_subTitle[20];
        QPushButton *btnView_all[20];

        QWidget *widget_login_contents;

        //===============================================================================
        TopMenuBar *menubar;

        QStackedWidget *stackRecent;

        QString contentStep;

        roseHome::ItemAlbum_rosehome *home_recently_album[15];
        roseHome::ItemPlaylist_rosehome *home_recently_playlist[15];
        roseHome::ItemArtist_rosehome *home_recently_artist[15];
        roseHome::ItemHistory_rosehome *home_historylist[15];
        roseHome::ItemPlaylist_rosehome *home_myPlaylist[15];
        roseHome::ItemPlaylist_rosehome *home_userPlaylist[15];

        PlaylistTrackDetailInfo_RHV *home_recently_track[5];

        QVBoxLayout *box_rose_contents;         ///< Bugs Rose Body Layout
        QWidget *widget_rose_contents;          ///< Bugs Rose Body Widget

        QWidget *widget_recentPlay;
        QWidget *widget_recentArtist;
        QWidget *widget_historylist;
        QWidget *widget_myPlaylist;
        QWidget *widget_userPlaylist;
        QWidget *widget_recentlyTrack;

        QVBoxLayout *vBox_recentlyPlay;
        QVBoxLayout *vBox_recentlyArtist;
        QVBoxLayout *vBox_historylist;
        QVBoxLayout *vBox_myPlaylist;
        QVBoxLayout *vBox_userPlaylist;
        QVBoxLayout *vBox_recentlyTrack;

        QHBoxLayout *hBox_recentlyAlbum;
        QHBoxLayout *hBox_recentlyPlaylist;
        QHBoxLayout *hBox_recentlyArtist;
        QHBoxLayout *hBox_historylist;
        QHBoxLayout *hBox_myPlaylist;
        QHBoxLayout *hBox_userPlaylist;

        QScrollArea *album_scrollArea;
        QScrollArea *playlist_scrollArea;

        QJsonArray jsonArr_tracks_toPlay;

        // data
        QList<roseHome::AlbumItemData> *list_recentlyAlbum;
        QList<roseHome::PlaylistItemData> *list_recentlyPlaylist;
        QList<roseHome::TrackItemData> *list_recentlytrack;
        QList<roseHome::ArtistItemData> *list_recentlyArtist;
        QList<roseHome::HistoryItemData> *list_Historylist;
        QList<roseHome::PlaylistItemData> *list_myPlaylist;
        QList<roseHome::PlaylistItemData> *list_userPlaylist;

        bool flag_album[2] = {false, false};
        bool flag_playlist[2] = {false, false};
        bool flag_track[2] = {false, false};
        bool flag_artist[2] = {false, false};
        bool flag_historylist[2] = {false, false};
        bool flag_myPlaylist[2] = {false, false};
        bool flag_userPlaylist[2] = {false, false};

        bool flag_recentAlbum_check[2] = {false, false};
        bool flag_recentPlaylist_check[2] = {false, false};
        bool flag_recentTrack_check[2] = {false, false};
        bool flag_recentArtist_check[2] = {false, false};
        bool flag_myPlaylist_check[2] = {false, false};

        //===============================================================================
        bugs::ItemAlbum_bugs *home_album_recently[15];
        bugs::ItemAlbum_bugs *home_album_bugsChart[15];

        bugs::ItemVideo_bugs *home_video_recently[15];
        bugs::ItemVideo_bugs *home_video_bugsChart[15];

        PlaylistTrackDetailInfo_RHV *home_track_recently[5];
        PlaylistTrackDetailInfo_RHV *home_track_bugsChart[5];

        QVBoxLayout *box_home_contents;         ///< Bugs Main Body Layout
        QWidget *widget_home_contents;          ///< Bugs Main Body Widget

        QVBoxLayout *box_total_home_contents;//c220729
        QWidget *widget_total_home_contents;//c220729

        QWidget *widget_trackRecently;
        QWidget *widget_albumRecently;
        QWidget *widget_videoRecently;
        QWidget *widget_trackBugsChart;
        QWidget *widget_albumBugsChart;
        QWidget *widget_videoBugsChart;

        QVBoxLayout *vBox_trackRecently;
        QVBoxLayout *vBox_albumRecently;
        QVBoxLayout *vBox_videoRecently;
        QVBoxLayout *vBox_trackBugsChart;
        QVBoxLayout *vBox_albumBugsChart;
        QVBoxLayout *vBox_videoBugsChart;

        QHBoxLayout *hBox_trackRecently;
        QHBoxLayout *hBox_albumRecently;
        QHBoxLayout *hBox_videoRecently;
        QHBoxLayout *hBox_trackBugsChart;
        QHBoxLayout *hBox_albumBugsChart;
        QHBoxLayout *hBox_videoBugsChart;

        QVBoxLayout *vBox_recentlyTrack_info;

        QJsonArray jsonArr_recently_toPlay;
        QJsonArray jsonArr_bugsChart_toPlay;

        QJsonArray video_recently_toPlay;
        QJsonArray video_bugsChart_toPlay;

        // data
        QList<bugs::TrackItemData> *list_trackRecently;
        QList<bugs::AlbumItemData> *list_albumRecently;
        QList<bugs::VideoItemData> *list_videoRecently;
        QList<bugs::TrackItemData> *list_trackBugsChart;
        QList<bugs::AlbumItemData> *list_albumBugsChart;
        QList<bugs::VideoItemData> *list_videoBugsChart;

        bool flag_trackRecently[2] = {false, false};
        bool flag_albumRecently[2] = {false, false};
        bool flag_videoRecently[2] = {false, false};
        bool flag_trackBugsChart[2] = {false, false};
        bool flag_albumBugsChart[2] = {false, false};
        bool flag_videoBugsChart[2] = {false, false};

        int track_idx_fav = 0;
        int track_star_fav = 0;

        bool flag_track_fav = false;
        bool flag_send_track = false;
        bool flag_check_track = false;

        // data
        bool flagInitDraw = false;
        bool flagLoginCheck = false;
        bool flagLogoutCheck = false;
    };

};


#endif // BUGSHOME_H
