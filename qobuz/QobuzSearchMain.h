#ifndef QOBUZSEARCHMAIN_H
#define QOBUZSEARCHMAIN_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "qobuz/ItemAlbum_forQobuz.h"
#include "qobuz/ItemArtist_forQobuz.h"
#include "qobuz/ItemPlaylist_forQobuz.h"

#include "common/common_struct.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief 검색결과 - 검색어에 대한 검색결과화면 클래스 (Qobuz)
     */
    class QobuzSearchMain : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:

        explicit QobuzSearchMain(QWidget *parent = nullptr);
        ~QobuzSearchMain();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_applyResult_mostPopular(const QJsonObject&, const QString) override;
        void slot_applyResult_artists(const QList<qobuz::ArtistItemData>&, const bool) override;
        void slot_applyResult_albums(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_tracks(const QList<qobuz::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_playlists(const QList<qobuz::PlaylistItemData>&, const QJsonArray&, const bool) override;

        // about Item
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode) override;

        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        // about clicks
        void slot_clickBtn_Filter();
        void slot_clickBtn_Filter_close();

        void slot_clickBtn_chooseFilter();
        void slot_clickBtn_apply();

        void slot_clicked_btnSelect();

    private :
        void setUIControl_chooseFilter();
        void setUIControl_chooseFilter_makeData();

        void setUIControl_basic();
        void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forArtist(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forPlaylist(QLayout *p_layout);

    private:
        common::SearchData data_search;

        //===============================================================================
        // Filter Area
        //===============================================================================
        QString btnStyle_normal;
        QString btnStyle_selected;

        bool flagSet_filter = false;             ///< filter 데이터 세팅 여부

        QWidget *widget_btnFilter;

        FlowLayout *flow_option_filter;
        QWidget *widget_option_filter;

        QPushButton *btn_filter_ico;            ///< filter icon

        QLabel *lb_filter[10];

        QWidget *widget_filter;                 ///< filter button drawing widget

        QPushButton *btn_HiRes;
        QPushButton *btn_NewRelease;

        QLabel *lb_HiRes;
        QLabel *lb_NewRelease;

        QPushButton *btn_filter[10];

        QStringList filterList;

        int btn_filter_cnt = 0;
        int enable_filter_id[10];

        bool flag_hiRes = false;
        bool flag_newRelease = false;

        bool flag_filter[10];
        bool filter_change = false;

        //===============================================================================
        // Body Area
        //===============================================================================
        PlaylistTrackDetailInfo_RHV *search_track[5];

        qobuz::ItemArtist_qobuz *search_mostPopular_artist;
        qobuz::ItemAlbum_qobuz *search_mostPopular_album;
        qobuz::ItemPlaylist_qobuz *search_mostPopular_playlist;

        qobuz::ItemArtist_qobuz *search_artist[15];
        qobuz::ItemAlbum_qobuz *search_album[15];
        qobuz::ItemPlaylist_qobuz *search_playlist[15];

        // UI
        QLabel *label_mainTitle;

        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QWidget *widget_mostPopular;
        QWidget *widget_artist;
        QWidget *widget_album;
        QWidget *widget_track;
        QWidget *widget_playlist;

        QVBoxLayout *vBox_mostPopular;
        QVBoxLayout *vBox_artist;
        QVBoxLayout *vBox_album;
        QVBoxLayout *vBox_track;
        QVBoxLayout *vBox_playlist;

        QHBoxLayout *hBox_mostPopular;
        QHBoxLayout *hBox_artist;
        QHBoxLayout *hBox_album;
        QHBoxLayout *hBox_playlist;

        QList<qobuz::ArtistItemData> *list_mostPopular_artist;
        QList<qobuz::AlbumItemData> *list_mostPopular_album;
        QList<qobuz::TrackItemData> *list_mostPopular_track;
        QList<qobuz::PlaylistItemData> *list_mostPopular_playlist;
        QList<qobuz::ArtistItemData> *list_artist;
        QList<qobuz::AlbumItemData> *list_album;
        QList<qobuz::TrackItemData> *list_track;
        QList<qobuz::PlaylistItemData> *list_playlist;

        QJsonArray jsonArr_tracks_toPlay;

        QString mostPopular_type = "";

        bool flag_mostPopular[2] = {false, false};
        bool flag_artist[2] = {false, false};
        bool flag_album[2] = {false, false};
        bool flag_track[2] = {false, false};
        bool flag_playlist[2] = {false, false};

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };

}


#endif // QOBUZSEARCHMAIN_H
