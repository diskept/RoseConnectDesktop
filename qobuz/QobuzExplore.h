#ifndef QOBUZEXPLORE_H
#define QOBUZEXPLORE_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "qobuz/ItemAlbum_forQobuz.h"
#include "qobuz/ItemPlaylist_forQobuz.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief "QOBUZ > 탐색" 화면
     */
    class QobuzExplore : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzExplore(QWidget *parent = nullptr);
        ~QobuzExplore();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_applyResult_tracks(const QList<qobuz::TrackItemData>&, const QJsonArray&, const bool) override;

        // about Item
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_hide_msg();

        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_idealDiscography(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_stillTrending(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_pressAwards(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_qobuzIssime(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_topRelease(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool);

        void slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        void slot_applyResult_checkRating_album(const QJsonObject&);
        void slot_applyResult_addRating_album(const QJsonObject&);
        void slot_applyResult_getRating_album(const QJsonArray&);

        // about clicks
        void slot_clickBtn_Filter();
        void slot_clickBtn_Filter_close();
        void slot_clickBtn_Filter_clear();
        void slot_clickBtn_chooseDetailGenre();

    private:
        // 초기 UI화면 구성
        void setUIControl_chooseGenres();
        void setUIControl_requestGenres();

        void setUIControl_appendWidget();
        void setUIControl_initialized();

        // 데이터를 화면에 적용
        void applyUIControl_chooseGenres_fromlistData();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);

    private:
        // 관리 필요한 Layout UI
        bool flagSet_genre = false;             ///< 장르 카테고리 데이터 세팅 여부

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

        PlaylistTrackDetailInfo_RHV *explore_topRelease[5];

        qobuz::ItemAlbum_qobuz *explore_idealDiscography[15];
        qobuz::ItemAlbum_qobuz *explore_stillTranding[15];
        qobuz::ItemAlbum_qobuz *explore_pressAwards[15];
        qobuz::ItemAlbum_qobuz *explore_qobuzIssime[15];

        QVBoxLayout *box_explore_contents;    ///< Qobuz explore Body Layout
        QWidget *widget_explore_contents;

        QWidget *widget_idealDiscography;
        QWidget *widget_stillTranding;
        QWidget *widget_pressAwards;
        QWidget *widget_qobuzIssime;
        QWidget *widget_topRelease;

        QVBoxLayout *vBox_idealDiscography;
        QVBoxLayout *vBox_stillTranding;
        QVBoxLayout *vBox_pressAwards;
        QVBoxLayout *vBox_qobuzIssime;
        QVBoxLayout *vBox_topRelease;

        QHBoxLayout *hBox_idealDiscography;     ///< ideal discography part
        QHBoxLayout *hBox_stillTranding;        ///< still tranding albums part
        QHBoxLayout *hBox_pressAwards;          ///< press awards albums part
        QHBoxLayout *hBox_qobuzIssime;          ///< qobuz issime albums part

        // data
        QList<qobuz::AlbumItemData> *list_idealDiscography;
        QList<qobuz::AlbumItemData> *list_still_trending;
        QList<qobuz::AlbumItemData> *list_press_awards;
        QList<qobuz::AlbumItemData> *list_qobuz_issime;
        QList<qobuz::AlbumItemData> *list_top_release;

        QJsonArray jsonArr_albums;

        QString str_genre_name[30];

        QString btnStyle_normal;
        QString btnStyle_selected;

        QString page = "";

        int btn_filter_cnt = 0;
        int enable_genre_id[30];

        bool flag_filter[30];

        bool flag_ideal[2] = {false, false};
        bool flag_still[2] = {false, false};
        bool flag_press[2] = {false, false};
        bool flag_qobuz[2] = {false, false};
        bool flag_top[2] = {false, false};

        bool flag_check_album = false;
        bool flag_album_fav = false;
        bool flag_send_track = false;

        QString album_id_fav = "";
        int album_idx_fav = 0;
        int album_star_fav = 0;

        int topRelease_idx = 0;

        bool flag_album_fav_chk = false;
    };
}

#endif // QOBUZEXPLORE_H
