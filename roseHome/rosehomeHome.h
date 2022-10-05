#ifndef ROSEHOMEHOME_H
#define ROSEHOMEHOME_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "roseHome/ItemAlbum_rosehome.h"
#include "roseHome/ItemPlaylist_rosehome.h"
#include "roseHome/ItemRecommends_rosehome.h"
#include "rosetube/ItemTrack_rosetube.h"

#include "common/linker.h"

#include "login/dialoglogin.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/myqwidget.h"//c220727


namespace roseHome {
    /**
     * @brief "RoseHome > 홈" 화면
     */
    class RoseHome : public AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:

        explicit RoseHome(QWidget *parent = nullptr);
        ~RoseHome();

        void setActivePage() override;

    protected slots:
        void slot_responseHttp(const int&, const QJsonObject&) override;

        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    public slots:
        void slot_changedSubTabUI(const QJsonObject &p_data) override;

    private slots:
        void slot_time_out();
        void slot_hide_msg();

        void slot_clickBtn_Filter();
        void slot_clickBtn_Filter_close();

        void slot_clickBtn_chooseMedia();
        void slot_clickBtn_applyMedia();

        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_Recommends(const QList<roseHome::RecommendsItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_recentlyAlbum(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_recentlyPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_recentlyRosetube(const QJsonArray&, const int&, const bool);
        void slot_applyResult_recentlyTrack(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_MyPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_NewPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_PopPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

        void slot_showLoginPage();
        void slot_getMyInfo_loginAfter();
        void slot_change_device_state(const QString&);

        void slot_applyResult_getRating_track(const QJsonArray&);

    private:
        // 초기 UI화면 구성
        void setUIControl_chooseFilter();
        void setUIControl_chooseFilter_fromData();

        void setUIControl_requestRose();

        void setUIControl_appendWidget_rose();

        QWidget* setUIControl_LoginBefore();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QWidget* setUIControl_subTitle_withSubMenu(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forPlaylists(QLayout *p_layout);

        void changedOnlyTabUI_notSendSignal(QString p_step);

    private:
        Linker *linker;

        //===============================================================================
        // Filter Area
        //===============================================================================
        QString btnStyle_normal;
        QString btnStyle_selected;

        QHBoxLayout *box_media_filter;
        FlowLayout *flow_media_filter;
        FlowLayout *flowLayout_media;           ///< Meida part

        QWidget *widget_filter;                 ///< Meida filter
        QPushButton *btn_filter_ico;            ///< Meida filter icon
        QPushButton *btn_filter[30];            ///< Meida button

        QLabel *lb_media[30];

        bool flag_filter[30];
        bool flagSet_media = false;             ///< Media 카테고리 데이터 세팅 여부

        int btn_filter_cnt = 0;
        int enable_media_id[30];

        QString str_media_name[30];
        QString str_media_type = "";

        QStringList mediaTypeList;

        bool filter_change = false;

        //===============================================================================
        // Body Area
        //===============================================================================
        Dialog::DialogLogin *dlg_rose_login;

        QWidget *widget_main_contents;
        QWidget *widget_login_contents;
        QStackedWidget *stackedWidget_Contents;

        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        TopMenuBar *menubar;

        QStackedWidget *stackRecent;

        QString contentStep;

        roseHome::ItemRecommends_rosehome *home_recommends[15];
        roseHome::ItemAlbum_rosehome *home_recently_album[15];
        roseHome::ItemPlaylist_rosehome *home_recently_playlist[15];
        roseHome::ItemPlaylist_rosehome *home_myPlaylist[15];
        roseHome::ItemPlaylist_rosehome *home_newPlaylist[15];
        roseHome::ItemPlaylist_rosehome *home_popPlaylist[15];

        rosetube::ItemTrack_rosetube *home_recently_rosetube[15];
        PlaylistTrackDetailInfo_RHV *home_recently_track[5];

        QVBoxLayout *box_rose_contents;
        QWidget *widget_rose_contents;

        QWidget *widget_recommends;
        QWidget *widget_recentlyPlay;
        QWidget *widget_recentlyRoseTube;
        QWidget *widget_recentlyTrack;
        QWidget *widget_myPlaylist;
        QWidget *widget_newPlaylist;
        QWidget *widget_popPlaylist;

        QVBoxLayout *vBox_recommends;
        QVBoxLayout *vBox_recentlyPlay;
        QVBoxLayout *vBox_recentlyRoseTube;
        QVBoxLayout *vBox_recentlyTrack;
        QVBoxLayout *vBox_myPlaylist;
        QVBoxLayout *vBox_newPlaylist;
        QVBoxLayout *vBox_popPlaylist;

        QHBoxLayout *hBox_recommends;
        QHBoxLayout *hBox_recentlyAlbum;
        QHBoxLayout *hBox_recentlyPlaylist;
        QHBoxLayout *hBox_recentlyRoseTube;
        QHBoxLayout *hBox_myPlaylist;
        QHBoxLayout *hBox_newPlaylist;
        QHBoxLayout *hBox_popPlaylist;

        // data
        QList<roseHome::RecommendsItemData> *list_Recommends;
        QList<roseHome::PlaylistItemData> *list_RecPlaylist;
        QList<roseHome::AlbumItemData> *list_RecentAlbum;
        QList<roseHome::PlaylistItemData> *list_RecentPlaylist;
        QList<roseHome::TrackItemData> *list_RecentTrack;
        QList<roseHome::PlaylistItemData> *list_MyPlaylist;
        QList<roseHome::PlaylistItemData> *list_NewPlaylist;
        QList<roseHome::PlaylistItemData> *list_PopPlaylist;

        QJsonArray jsonArr_rosetube;
        QJsonArray jsonArr_tracks_toPlay;

        QString recom_path = "";

        int recentRoseTube_cnt = 0;

        bool flag_request_api = false;

        bool flag_recommends[2] = {false, false};
        bool flag_recentAlbum[2] = {false, false};
        bool flag_recentPlaylist[2] = {false, false};
        bool flag_recentRosetube[2] = {false, false};
        bool flag_recentTrack[2] = {false, false};
        bool flag_myPlaylist[2] = {false, false};
        bool flag_newPlaylist[2] = {false, false};
        bool flag_popPlaylist[2] = {false, false};

        int track_idx_fav = 0;
        int track_star_fav = 0;

        bool flag_track_fav = false;
        bool flag_send_track = false;
        bool flag_check_track = false;
    };
};
#endif // ROSEHOMEHOME_H
