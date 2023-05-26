#ifndef QOBUZHOME_H
#define QOBUZHOME_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "qobuz/ItemAlbum_forQobuz.h"
#include "qobuz/ItemPlaylist_forQobuz.h"
#include "qobuz/QobuzSettings.h"

#include "roseHome/ItemAlbum_rosehome.h"
#include "roseHome/ItemPlaylist_rosehome.h"
#include "roseHome/ItemArtist_rosehome.h"
#include "roseHome/ItemHistory_rosehome.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/myqwidget.h"//c220729

#include <QCoreApplication>


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief "QOBUZ > 홈" 화면
     */
    class QobuzHome : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:

        explicit QobuzHome(QWidget *parent = nullptr);
        ~QobuzHome();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
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
        void slot_gotoRoseHome();//c230322_3
        void slot_time_out();
        void slot_hide_msg();

        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_newReleases(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_editorPicks(const QList<qobuz::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_idealDiscography(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_qobuzIssime(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool);

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

        void slot_applyResult_getShareLink(const QString &link);//c220818

        void slot_applyResult_myPlaylistDelete(const QJsonObject&);

        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        // about clicks
        void slot_clickBtn_Filter();
        void slot_clickBtn_Filter_close();
        void slot_clickBtn_Filter_clear();
        void slot_clickBtn_chooseDetailGenre();

        void changedOnlyTabUI_notSendSignal(QString p_step);
        void slot_changedSubTabUI(const QJsonObject &p_data);

        void slot_change_device_state(const QString&);

    private :
        // 초기 UI화면 구성
        void setUIControl_chooseGenres();
        void setUIControl_requestGenres();

        void setUIControl_requestRose();

        void setUIControl_appendWidget_rose();
        void setUIControl_checkWidget_rose();

        void setUIControl_appendWidget();

        //void setUIControl_initialized_rose();
        void setUIControl_initialized();

        // 데이터를 화면에 적용
        void applyUIControl_chooseGenres_fromlistData();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QWidget* setUIControl_subTitle_withSubMenu(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forPlaylists(QLayout *p_layout);

    private:
        Linker *linker;

        // 관리 필요한 Layout UI        
        bool flagSet_genre = false;             ///< 장르 카테고리 데이터 세팅 여부

        qobuz::QobuzSettings *qobuzSettings;

        QWidget *widget_btnFilter;//c220805
        //int label_size = 0;//c220805
        //QHBoxLayout *box_genre_filter;
        FlowLayout *flow_genre_filter;
        QHBoxLayout *hbox_genre_filter;
        QWidget *widget_genre_filter;
        FlowLayout *flowLayout_genre;           ///< Genres part

        QWidget *widget_filter;                 ///< Genres filter
        QPushButton *btn_filter_ico;            ///< Genres filter icon
        QPushButton *btn_filter_clear;          ///< Genres clear button
        QPushButton *btn_filter[30];            ///< Genres button
        QLabel *lb_filter[30];

        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        //QVBoxLayout *box_total_home_contents;//c220729
        //QWidget *widget_total_home_contents;//c220729

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

        QVBoxLayout *box_rose_contents;         ///< Qobuz Rose Body Layout
        QWidget *widget_rose_contents;          ///< Qobuz Rose Body Widget

        QWidget *widget_recentPlay;
        QWidget *widget_recentlyTrack;
        QWidget *widget_recentArtist;
        QWidget *widget_historylist;
        QWidget *widget_myPlaylist;
        QWidget *widget_userPlaylist;

        QVBoxLayout *vBox_recentlyPlay;
        QVBoxLayout *vBox_recentlyTrack;
        QVBoxLayout *vBox_recentlyArtist;
        QVBoxLayout *vBox_historylist;
        QVBoxLayout *vBox_myPlaylist;
        QVBoxLayout *vBox_userPlaylist;

        QHBoxLayout *hBox_recentlyAlbum;
        QHBoxLayout *hBox_recentlyPlaylist;
        QHBoxLayout *hBox_recentlyArtist;
        QHBoxLayout *hBox_historylist;
        QHBoxLayout *hBox_myPlaylist;
        QHBoxLayout *hBox_userPlaylist;

        QVBoxLayout *vBox_recentlyTrack_info;

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
        qobuz::ItemAlbum_qobuz *home_newReleases[15];
        qobuz::ItemPlaylist_qobuz *home_editorPicks[15];
        qobuz::ItemAlbum_qobuz *home_idealDiscography[15];
        qobuz::ItemAlbum_qobuz *home_qobuzIssime[15];

        QVBoxLayout *box_home_contents;         ///< Qobuz Main Body Layout
        QWidget *widget_home_contents;          ///< Qobuz Main Body Widget

        QWidget *widget_newReleases;
        QWidget *widget_editorPicks;
        QWidget *widget_idealDiscography;
        QWidget *widget_qobuzIssime;

        QVBoxLayout *vBox_newReleases;
        QVBoxLayout *vBox_editorPicks;
        QVBoxLayout *vBox_idealDiscography;
        QVBoxLayout *vBox_qobuzIssime;

        QHBoxLayout *hBox_newReleases;
        QHBoxLayout *hBox_editorPicks;
        QHBoxLayout *hBox_idealDiscography;
        QHBoxLayout *hBox_qobuzIssime;

        // data
        QList<qobuz::AlbumItemData> *list_newReleases;
        QList<qobuz::PlaylistItemData> *list_editorPicks;
        QList<qobuz::AlbumItemData> *list_idealDiscography;
        QList<qobuz::AlbumItemData> *list_qobuz_issime;

        QString str_genre_name[30];

        QString btnStyle_normal;
        QString btnStyle_selected;

        QString page = "";

        int btn_filter_cnt = 0;
        int enable_genre_id[30];

        bool flag_filter[30];

        bool flag_release[2] = {false, false};
        bool flag_editor[2] = {false, false};
        bool flag_ideal[2] = {false, false};
        bool flag_qobuz[2] = {false, false};

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;

        bool flag_track_fav = false;
        bool flag_send_track = false;
        bool flag_check_track = false;
    };

}

#endif // QOBUZHOME_H
