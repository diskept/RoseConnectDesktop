#ifndef TIDALEXPLORE_H
#define TIDALEXPLORE_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemAlbum.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/listwidget.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief "TIDAL > 탐색" 화면
     */
    class TidalExplore : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalExplore(QWidget *parent = nullptr);
        ~TidalExplore();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_albums(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool) override;

        // about Item
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_hide_msg();

        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        // about clicks
        void slot_clickBtn_choseDetailGenre();
        void slot_clickBtn_choseDetailMood();
        void slot_clickBtn_specificPart();          ///< New, Top, Rising 클릭

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);

    private:
        // 초기 UI화면 구성
        void setUIControl_chooseGenres();
        void setUIControl_chooseMoods();
        void setUIControl_btnH_part();

        void setUIControl_appendWidget();

        // 페이지 이동
        void movePage_genre_viewAsList();

        // 데이터를 화면에 적용
        void applyUIControl_chooseGenres_fromlistData();
        void applyUIControl_chooseMoods_fromlistData();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);

    private:
        // 관리 필요한 Layout UI
        bool flagLoginCheck = false;
        bool flagLogoutCheck = false;
        bool flagSet_genre = false;                 ///< 장르 카테고리 데이터 세팅 여부
        bool flagSet_mood = false;                  ///< 무드 카테고리 데이터 세팅 여부

        QVBoxLayout *box_genre_mood;
        FlowLayout *flowLayout_genre;       ///< Genres part
        FlowLayout *flowLayout_mood;        ///< Moods part

        QString page = "";

        QLabel *lb_subTitle[5];
        QPushButton *btnView_all[5];

        PlaylistTrackDetailInfo_RHV *explore_newTrack[5];

        tidal::ItemAlbum *explore_newAlbum[15];

        QVBoxLayout *box_explore_contents;    ///< Qobuz explore Body Layout
        QWidget *widget_explore_contents;

        QWidget *widget_newTrack;
        QWidget *widget_newAlbum;

        QVBoxLayout *vBox_newTrack;
        QVBoxLayout *vBox_newAlbum;

        QListWidget *listWidget_track;          ///< suggested new tracks part
        QListWidgetItem *listWidget_trackItem[5];
        QHBoxLayout *hBox_newAlbum;             ///< suggested new albums part

        // data
        QList<tidal::TrackItemData> *list_track;
        QList<tidal::AlbumItemData> *list_album;

        QJsonArray jsonArr_tracks_toPlay;

        bool flag_track[2] = {false, false};
        bool flag_album[2] = {false, false};

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };
}

#endif // TIDALEXPLORE_H
