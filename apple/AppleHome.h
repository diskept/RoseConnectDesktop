#ifndef APPLEHOME_H
#define APPLEHOME_H

#include "apple/AbstractAppleSubWidget.h"

#include "roseHome/ItemAlbum_rosehome.h"
#include "roseHome/ItemPlaylist_rosehome.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"


namespace apple {

    /**
     * @brief "APPLE > 홈" 화면
     */
    class AppleHome : public AbstractAppleSubWidget
    {
        Q_OBJECT
    public:

        explicit AppleHome(QWidget *parent = nullptr);
        ~AppleHome();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

        void slot_changedSubTabUI(const QJsonObject &p_data) override;

    private slots:
        void slot_time_out();
        void slot_hide_msg();

        void slot_change_device_state(const QString&);

        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_recentlyAlbum(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_recentlyPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_recentlyTrack(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_myPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_userPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_getRating_track(const QJsonArray&);

        // about clicks
        void changedOnlyTabUI_notSendSignal(QString p_step);

    private :
        // 초기 UI화면 구성
        void setUIControl_requestRose();
        //void setUIControl_requestApple();

        void setUIControl_appendWidget_rose();
        //void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QWidget* setUIControl_subTitle_withSubMenu(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forPlaylists(QLayout *p_layout);

    private:
        Linker *linker;//cheon211008

        // 관리 필요한 Layout UI
        //tidal::TidalSettings *tidalSettings;

        QLabel *lb_subTitle[20];
        QPushButton *btnView_all[20];

        QWidget *widget_login_contents;

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

        QString page = "";

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
#endif // APPLEHOME_H
