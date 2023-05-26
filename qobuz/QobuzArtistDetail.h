#ifndef QOBUZARTISTDETAIL_H
#define QOBUZARTISTDETAIL_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "qobuz/ItemAlbum_forQobuz.h"
#include "qobuz/ItemArtist_forQobuz.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/dialogNotice.h"

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz > Artist 상세화면 클래스
     */
    class QobuzArtistDetail : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzArtistDetail(QWidget *parent = nullptr);
        ~QobuzArtistDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    protected slots:
        // ArtistImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;

        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode) override;

        void slot_applyResult_tracks(const QList<qobuz::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_appearson(const QList<qobuz::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_lastrelease(const QList<qobuz::AlbumItemData> &, const QJsonArray&, const bool);
        void slot_applyResult_albums(const QList<qobuz::AlbumItemData> &, const QJsonArray&, const bool) override;
        void slot_applyResult_artists(const QList<qobuz::ArtistItemData>&, const bool) override;


        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots :
        void slot_applyResult_artistInfo(qobuz::ArtistItemData data_artist);
        void slot_artistInfo_btnClicked();
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_checkRating_artist(const QJsonObject&);
        void slot_applyResult_getRating_artist(const QJsonArray&);
        void slot_applyResult_addRating_artist(const QJsonObject&);

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject&);

    private:
        // function about UI
        void setUIControl_basic();
        void setUIControl_appendWidget();

        void setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum();
        QHBoxLayout* setUIControl_hBoxLayout_forArtist();

    private:
        Linker *linker;
        DialogNotice *dlgArtistMore;

        AbstractImageDetailContents_RHV *artist_detail_info;
        PlaylistTrackDetailInfo_RHV *top_tracks[5];
        PlaylistTrackDetailInfo_RHV *appears_on[5];

        // UI
        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vBox_topTracks;
        QHBoxLayout *hBox_release;
        QHBoxLayout *hBox_album;
        QVBoxLayout *vBox_appearson;
        QHBoxLayout *hBox_relatedArtist;

        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        qobuz::ItemAlbum_qobuz *last_release[15];
        qobuz::ItemAlbum_qobuz *release[15];
        qobuz::ItemArtist_qobuz *related_artist[15];

        // data        
        qobuz::ArtistItemData data_artist;
        QList<qobuz::TrackItemData> *list_track;    ///< Top Tracks 정보
        QList<qobuz::AlbumItemData> *list_release;  ///< Last Release 정보
        QList<qobuz::AlbumItemData> *list_album;    ///< Artist의 album 목록
        QList<qobuz::TrackItemData> *list_appears;  ///< Top Tracks 정보
        QList<qobuz::ArtistItemData> *list_artist;  ///< Related Artist 정보

        QJsonArray jsonArr_tracks_toPlay;           ///< Track 전체를 재생하기 위함 (list_track 관련)
        QJsonArray jsonArr_appears_toPlay;          ///< appears 전체를 재생하기 위함 (list_track 관련)

        bool flag_page_load = false;

        bool flag_reload_page = false;
        bool flag_tracks[2] = {false, false};
        bool flag_lastRelease[2] = {false, false};
        bool flag_release[2] = {false, false};
        bool flag_appears[2] = {false, false};
        bool flag_suggestArtists[2] = {false, false};

        bool flag_topTracks = false;
        bool flag_appearsOn = false;

        bool flag_artist_fav = false;
        bool flag_send_artist = false;
        bool flag_check_artist = false;

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int artist_star_fav = 0;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };

};

#endif // QOBUZARTISTDETAIL_H
