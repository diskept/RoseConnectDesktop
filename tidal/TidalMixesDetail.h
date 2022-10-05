#ifndef TIDALMIXESDETAIL_H
#define TIDALMIXESDETAIL_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemPlaylist.h"
#include "tidal/TidalSettings.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/dialogNotice.h"

#include <QCoreApplication>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal의 Playlist 상세보기 화면
     */
    class TidalMixesDetail : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalMixesDetail(QWidget *parent = nullptr);
        ~TidalMixesDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots :
        // PlaylistImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;

        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override ;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_checkRating_palylist(const QJsonObject&);
        void slot_applyResult_checkRating_track(const QJsonObject&);

        void slot_applyResult_addRating_playlist(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void slot_applyResult_getRating_playlist(const QJsonArray&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        void slot_tidal_logChange();

    private:
        // function about UI
        void setUIControl_basic();

        void request_more_trackData();
        void request_more_trackDraw();

    private:
        Linker *linker;

        DialogNotice *dlgAlbumInfo;

        tidal::TidalSettings *tidalSettings;
        AbstractImageDetailContents_RHV *playlist_detail_info;
        PlaylistTrackDetailInfo_RHV *playlist_tracks[9999];

        // UI
        QVBoxLayout *box_login_contents;
        QWidget *widget_login_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QHBoxLayout *hBox_similarPlaylists;

        // data
        tidal::ItemPlaylist *similarPlaylist[15];

        tidal::PlaylistItemData data_playlist;
        QList<tidal::TrackItemData> *list_track;                ///< Playlist 내의 Track 정보
        QList<tidal::PlaylistItemData> *list_similarPlaylist;

        QJsonObject login_wait_Object;
        QJsonArray jsonArr_tracks_toPlay;                       ///< Track 전체를 재생하기 위함

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;

        bool flag_similarPlaylist = false;

        bool flag_draw = false;

        int track_drawCount = 0;
        int track_totalCount = 0;
        int track_beforeOffset = 0;
        int track_currentOffset = 0;
        int track_favoriteOffset = 0;

        int track_id_fav = 0;
        int track_idx_fav = 0;

        int playlist_star_fav = 0;
        int track_star_fav = 0;

        bool flag_playlist_fav = false;
        bool flag_track_fav = false;
        bool flag_send_playlist = false;
        bool flag_check_playlist = false;
        bool flag_send_track = false;
        bool flag_check_track = false;

        bool flag_track_ok = false;
    };

};
#endif // TIDALMIXESDETAIL_H
