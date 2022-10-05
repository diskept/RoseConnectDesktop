#ifndef MUSICHOME_H
#define MUSICHOME_H

#include "common/linker.h"
#include "common/findip.h"

#include "home/topmenubar.h"

#include "login/dialoglogin.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"
#include "roseHome/ItemArtist_rosehome.h"
#include "roseHome/ItemAlbum_rosehome.h"
#include "roseHome/ItemPlaylist_rosehome.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include "widget/myqwidget.h"//c220729


/**
 * @brief The MusicHome class 음악 > 홈 메뉴 화면 @n
 *      최근재생앨범, 최근추가된앨범, My 플레이리스트(로그인), Friend 플레이리스트(로그인), 아티스트 항목으로 구성
 */
namespace music {

    class MusicHome : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit MusicHome(QWidget *parent = nullptr);
        ~MusicHome();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

        void show_topLabelControl();
        void hide_topLabelControl();

    signals:
        void signal_clickedViewAll(QString p_viewAllMode);
        void signal_clickedViewAll_slot_clickedViewAll_Recommended(QString p_viewAllMode, QJsonObject jsonObj);

    protected slots:

        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;

        // about Item
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void resizeEvent(QResizeEvent *event) override;

    private slots:
        void slot_topLabelChanged();
        void slot_musicRecentChanged();

        void slot_time_out();
        void slot_hide_msg();
        void slot_showLoginPage();
        void slot_getMyInfo_loginAfter();
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
        void slot_changedSubTabUI(const QJsonObject &p_data) override;

    private :
        // 초기 UI화면 구성
        void setTopUIControl(QWidget *parent);
        void requestDataCount();

        void setUIControl_requestRose();
        void setUIControl_requestMusic();

        void setUIControl_appendWidget_rose();
        void setUIControl_appendWidget();

        QJsonArray getDataForPlayMusic(const int);

        QWidget* setUIControl_LoginBefore();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QWidget* setUIControl_subTitle_withSubMenu(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forArtist(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forPlaylists(QLayout *p_layout);

    private:
        Linker *linker;

        FindIP *findIP;

        Dialog::DialogLogin *dlg_rose_login;

        QString page = "";

        QLabel *lb_subTitle[20];
        QPushButton *btnView_all[20];

        //===============================================================================
        TopMenuBar *menubar;

        QStackedWidget *stackRecent;

        QString contentStep;

        roseHome::ItemAlbum_rosehome *home_recently_album[15];
        roseHome::ItemPlaylist_rosehome *home_recently_playlist[15];
        roseHome::ItemPlaylist_rosehome *home_myPlaylist[15];
        roseHome::ItemPlaylist_rosehome *home_userPlaylist[15];

        PlaylistTrackDetailInfo_RHV *home_recently_track[5];

        QWidget *widget_login;

        QVBoxLayout *box_rose_contents;
        QVBoxLayout *box_login_contents;

        QVBoxLayout *box_total_home_contents;//c220729
        QWidget *widget_total_home_contents;//c220729

        QWidget *widget_rose_contents;
        QWidget *widget_login_contents;
        QStackedWidget *stackedWidget_Contents;

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

        int track_idx_fav = 0;
        int track_star_fav = 0;

        bool flag_track_fav = false;
        bool flag_send_track = false;
        bool flag_check_track = false;

        //===============================================================================
        roseHome::ItemAlbum_rosehome *home_recently_Add[15];
        roseHome::ItemArtist_roseHome *home_recommand_Artist[15];

        QWidget *widget_recentlyAdds;
        QWidget *widget_recommandArtist;

        QVBoxLayout *vBox_recentlyAdds;
        QVBoxLayout *vBox_recommandArtist;

        QHBoxLayout *hBox_recentlyAdds;
        QHBoxLayout *hBox_recommandArtist;

        QList<roseHome::AlbumItemData> *list_RecentAdds;
        QList<roseHome::ArtistItemData> *list_RecommandArtist;

        QJsonArray jsonArr_artists;

        //===============================================================================
        QLabel *lb_album_cnt;
        QLabel *lb_track_cnt;
        QLabel *lb_artist_cnt;

        QWidget *widget_album;
        QWidget *widget_track;
        QWidget *widget_artist;

        bool flag_topLabel = false;
    };
};

#endif // MUSICHOME_H
