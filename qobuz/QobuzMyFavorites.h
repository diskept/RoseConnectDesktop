#ifndef QOBUZMYFAVORITES_H
#define QOBUZMYFAVORITES_H

#include "qobuz/AbstractQobuzSubWidget.h"
#include "qobuz/qobuz_struct.h"

#include "qobuz/ItemAlbum_forQobuz.h"
#include "qobuz/ItemArtist_forQobuz.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include "bugs/BugsChooseFilterOpt.h"

#include "widget/FilterWidget.h"
#include "widget/flowlayout.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QJsonArray>

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz의 My Playlist 전체목록 화면 ("좋아요" 선택한 목록 전체)
     */
    class QobuzMyFavorites : public AbstractQobuzSubWidget
    {
        Q_OBJECT

    public:
        explicit QobuzMyFavorites(QWidget *parent = nullptr);
        ~QobuzMyFavorites();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_applyResult_artists(const QList<qobuz::ArtistItemData>&, const bool) override;
        void slot_applyResult_albums(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_tracks(const QList<qobuz::TrackItemData>&, const QJsonArray&, const bool) override;

        // about Item
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode) override;

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

    private:
        // 초기 UI화면 구성
        void setUIControl_basic();
        void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forArtist(QLayout *p_layout);

    private:
        PlaylistTrackDetailInfo_RHV *search_track[5];

        qobuz::ItemArtist_qobuz *search_artist[15];
        qobuz::ItemAlbum_qobuz *search_album[15];

        // UI
        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QWidget *widget_artist;
        QWidget *widget_album;
        QWidget *widget_track;

        QVBoxLayout *vBox_artist;
        QVBoxLayout *vBox_album;
        QVBoxLayout *vBox_track;

        QHBoxLayout *hBox_artist;
        QHBoxLayout *hBox_album;

        QList<qobuz::ArtistItemData> *list_artist;
        QList<qobuz::AlbumItemData> *list_album;
        QList<qobuz::TrackItemData> *list_track;

        QJsonArray jsonArr_tracks_toPlay;

        QString page_reload_check = "";

        bool flag_artist[2] = {false, false};
        bool flag_album[2] = {false, false};
        bool flag_track[2] = {false, false};

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };

};

#endif // QOBUZMYFAVORITES_H
