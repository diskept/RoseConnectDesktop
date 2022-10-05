#ifndef TIDALARTISTDETAIL_H
#define TIDALARTISTDETAIL_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemAlbum.h"
#include "tidal/ItemArtist.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/dialogNotice.h"

#include <QListWidget>
#include "widget/ArtistImageDetailInfo.h"
#include "delegate/TidalTrackDelegate.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal > Artist 상세화면 클래스
     */
    class TidalArtistDetail : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalArtistDetail(QWidget *parent = nullptr);
        ~TidalArtistDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    protected slots:
        // ArtistImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;

        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode) override;

        void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_albums(const QList<tidal::AlbumItemData> &, const QJsonArray&, const bool) override;
        void slot_applyResult_videos(const QList<tidal::VideoItemData>&, const bool) override;
        void slot_applyResult_artists(const QList<tidal::ArtistItemData>&, const bool) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots :
        void slot_applyResult_artistInfo(tidal::ArtistItemData data_artist);
        void slot_applyResult_artist_bioText(const QString& bioText);

        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        /*void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&);*/

    private:
        // function about UI
        void setUIControl_basic();
        void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forVideo(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forArtist(QLayout *p_layout);

    private:
        DialogNotice *dlgArtistMore;

        AbstractImageDetailContents_RHV *artist_detail_info;
        PlaylistTrackDetailInfo_RHV *track[5];

        // UI
        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QWidget *widget_track;
        QWidget *widget_album;
        QWidget *widget_video;
        QWidget *widget_artist;

        QVBoxLayout *vBox_track;
        QVBoxLayout *vBox_album;
        QVBoxLayout *vBox_video;
        QVBoxLayout *vBox_artist;

        QHBoxLayout *hBox_album;
        QHBoxLayout *hBox_video;
        QHBoxLayout *hBox_artist;

        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        tidal::ItemAlbum *album[15];
        tidal::ItemVideo *video[15];
        tidal::ItemArtist *artist[15];

        //data
        tidal::ArtistItemData data_artist;

        QList<tidal::TrackItemData> *list_track;
        QList<tidal::AlbumItemData> *list_album;
        QList<tidal::VideoItemData> *list_video;
        QList<tidal::ArtistItemData> *list_artist;

        QJsonArray jsonArr_tracks_toPlay;           ///< Track 전체를 재생하기 위함 (list_track 관련)

        QString api_subPath_topTrack;               ///< Artist의 Top track 을 위한 API의 subPath
        QString api_subPath_album;                  ///< Artist의 Album 을 위한 API의 subPath
        QString api_subPath_video;                  ///< Artist의 Video 을 위한 API의 subPath
        QString api_subPath_relatedArtist;          ///< Artist의 Related Artist 을 위한 API의 subPath

        bool flag_reload_page = false;
        bool flag_track[2] = {false, false};
        bool flag_album[2] = {false, false};
        bool flag_video[2] = {false, false};
        bool flag_artist[2] = {false, false};

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

#endif // TIDALARTISTDETAIL_H
