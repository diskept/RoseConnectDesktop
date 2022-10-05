#ifndef TIDALHOME_H
#define TIDALHOME_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemAlbum.h"
#include "tidal/ItemPlaylist.h"
#include "tidal/TidalSettings.h"

#include "roseHome/ItemAlbum_rosehome.h"
#include "roseHome/ItemPlaylist_rosehome.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include "widget/myqwidget.h"//c220729

#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief "TIDAL > 홈" 화면
     */
    class TidalHome : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:

        explicit TidalHome(QWidget *parent = nullptr);
        ~TidalHome();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        // about Item
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_time_out();
        void slot_hide_msg();

        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_mixes_track(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_mixes_video(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_albums_suggested(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_albums_suggested_top(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_playlists_master(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_albums_master(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_playlists_focus(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_playlists_rising(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_albums_visual(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool) override;

        void slot_applyResult_recentlyAlbum(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_recentlyPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_recentlyTrack(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_myPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_userPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_getRating_track(const QJsonArray&);

        // about clicks
        void changedOnlyTabUI_notSendSignal(QString p_step);
        void slot_changedSubTabUI(const QJsonObject &p_data);

        void slot_change_device_state(const QString&);
        void slot_applyResult_getShareLink(const QString&) ;//c220826_2

    private:
        // 초기 UI화면 구성        
        void setUIControl_requestRose();
        void setUIControl_requestTidal();

        void setUIControl_appendWidget_rose();
        void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QWidget* setUIControl_subTitle_withSubMenu(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forPlaylists(QLayout *p_layout);

    private:
        Linker *linker;//cheon211008

        // 관리 필요한 Layout UI
        tidal::TidalSettings *tidalSettings;

        QLabel *lb_subTitle[20];
        QPushButton *btnView_all[20];

        QWidget *widget_login_contents;

        QVBoxLayout *box_total_home_contents;//c220729
        QWidget *widget_total_home_contents;//c220729
        //===============================================================================
        TopMenuBar *menubar;

        QStackedWidget *stackRecent;

        QString contentStep;

        roseHome::ItemAlbum_rosehome *home_recently_album[15];
        roseHome::ItemPlaylist_rosehome *home_recently_playlist[15];
        roseHome::ItemPlaylist_rosehome *home_myPlaylist[15];
        roseHome::ItemPlaylist_rosehome *home_userPlaylist[15];

        PlaylistTrackDetailInfo_RHV *home_recently_track[5];

        QVBoxLayout *box_rose_contents;         ///< Qobuz Rose Body Layout
        QWidget *widget_rose_contents;          ///< Qobuz Rose Body Widget

        QWidget *widget_recentPlay;
        QWidget *widget_myPlaylist;
        QWidget *widget_userPlaylist;
        QWidget *widget_recentlyTrack;

        QVBoxLayout *vBox_recentlyPlay;
        QVBoxLayout *vBox_myPlaylist;
        QVBoxLayout *vBox_userPlaylist;
        QVBoxLayout *vBox_recentlyTrack;

        QHBoxLayout *hBox_recentlyAlbum;
        QHBoxLayout *hBox_recentlyPlaylist;
        QHBoxLayout *hBox_myPlaylist;
        QHBoxLayout *hBox_userPlaylist;

        QJsonArray jsonArr_tracks_toPlay;

        // data
        QList<roseHome::AlbumItemData> *list_recentlyAlbum;
        QList<roseHome::PlaylistItemData> *list_recentlyPlaylist;
        QList<roseHome::PlaylistItemData> *list_myPlaylist;
        QList<roseHome::PlaylistItemData> *list_userPlaylist;
        QList<roseHome::TrackItemData> *list_recentlytrack;

        bool flag_album[2] = {false, false};
        bool flag_playlist[2] = {false, false};
        bool flag_track[2] = {false, false};
        bool flag_myPlaylist[2] = {false, false};
        bool flag_userPlaylist[2] = {false, false};

        //===============================================================================
        tidal::ItemPlaylist *home_mixesTrack[15];
        tidal::ItemPlaylist *home_mixesVideo[15];
        tidal::ItemAlbum *home_suggestedAlbum[15];
        tidal::ItemAlbum *home_suggestedTop[15];
        tidal::ItemPlaylist *home_playlistMaster[15];
        tidal::ItemAlbum *home_albumMaster[15];
        tidal::ItemPlaylist *home_playlistFocus[15];
        tidal::ItemPlaylist *home_playlistRising[15];
        tidal::ItemAlbum *home_visualAlbum[15];

        QVBoxLayout *box_home_contents;         ///< Qobuz Main Body Layout
        QWidget *widget_home_contents;          ///< Qobuz Main Body Widget

        QWidget *widget_mixesTrack;
        QWidget *widget_mixesVideo;
        QWidget *widget_suggestedAlbum;
        QWidget *widget_suggestedTOP;
        QWidget *widget_masterPlaylist;
        QWidget *widget_masterAlbum;
        QWidget *widget_focusplaylist;
        QWidget *widget_risingplaylist;
        QWidget *widget_visualAlbum;

        QVBoxLayout *vBox_mixesTrack;
        QVBoxLayout *vBox_mixesVideo;
        QVBoxLayout *vBox_suggestedAlbum;
        QVBoxLayout *vBox_suggestedTOP;
        QVBoxLayout *vBox_masterPlaylist;
        QVBoxLayout *vBox_masterAlbum;
        QVBoxLayout *vBox_focusplaylist;
        QVBoxLayout *vBox_risingplaylist;
        QVBoxLayout *vBox_visualAlbum;

        QHBoxLayout *hBox_mixesTrack;
        QHBoxLayout *hBox_mixesVideo;
        QHBoxLayout *hBox_suggestedAlbum;
        QHBoxLayout *hBox_suggestedTOP;
        QHBoxLayout *hBox_masterPlaylist;
        QHBoxLayout *hBox_masterAlbum;
        QHBoxLayout *hBox_focusplaylist;
        QHBoxLayout *hBox_risingplaylist;
        QHBoxLayout *hBox_visualAlbum;

        // data        
        QList<tidal::PlaylistItemData> *list_mixesTrack;
        QList<tidal::PlaylistItemData> *list_mixesVideo;
        QList<tidal::AlbumItemData> *list_albumSuggested;
        QList<tidal::AlbumItemData> *list_albumSuggested_Top;
        QList<tidal::PlaylistItemData> *list_playlistMaster;
        QList<tidal::AlbumItemData> *list_albumMaster;
        QList<tidal::PlaylistItemData> *list_playlistFocus;
        QList<tidal::PlaylistItemData> *list_playlistRising;
        QList<tidal::AlbumItemData> *list_visualAlbum;
        QList<tidal::AlbumItemData> *list_visualAlbum_viewall;

        QString page = "";

        bool flag_mixesTrack[2] = {false, false};
        bool flag_mixesVideo[2] = {false, false};
        bool flag_suggestedAlbum[2] = {false, false};
        bool flag_suggestedTOP[2] = {false, false};
        bool flag_masterPlaylist[2] = {false, false};
        bool flag_masterAlbum[2] = {false, false};
        bool flag_focusplaylist[2] = {false, false};
        bool flag_risingplaylist[2] = {false, false};
        bool flag_visualAlbum[2] = {false, false};

        int track_idx_fav = 0;
        int track_star_fav = 0;

        bool flag_track_fav = false;
        bool flag_send_track = false;
        bool flag_check_track = false;

        // data
        bool flagInitDraw = false;
        bool flagLoginCheck = false;
        bool flagLogoutCheck = false;

        tidal::AlbumItemData data_album;
        QList<tidal::TrackItemData> *list_track;           ///< Album 내의 Track 정보
    };

}

#endif // TIDALHOME_H
