#ifndef APPLEPLAYLISTDETAIL_H
#define APPLEPLAYLISTDETAIL_H

#include "apple/Apple_struct.h"
#include "apple/AppleSettings.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/dialogNotice.h"

#include <QCoreApplication>


namespace apple {

    /**
     * @brief APPLEMUSIC의 Playlist 상세보기 화면
     */
    class ApplePlaylistDetail : public AbstractAppleSubWidget
    {
        Q_OBJECT
    public:
        explicit ApplePlaylistDetail(QWidget *parent = nullptr);
        ~ApplePlaylistDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    protected slots:
        // playlistImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_applyResult_playlist(const QJsonObject&);
        void slot_applyResult_playlist_tracks(const QJsonArray&);

        void slot_applyResult_checkRating_playlist(const QJsonObject&);
        void slot_applyResult_getRating_playlist(const QJsonArray&);
        void slot_applyResult_addRating_playlist(const QJsonObject&);

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_addRating_track(const QJsonObject&);

    private:
        // function about UI
        void setUIControl_basic();

    private:
        apple::AppleSettings *appleSettings;

        AbstractImageDetailContents_RHV *playlist_detail_info;
        PlaylistTrackDetailInfo_RHV *playlist_tracks[99999];

        // UI
        QVBoxLayout *box_login_contents;
        QWidget *widget_login_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        // data
        apple::PlaylistItemData data_playlist;

        QJsonObject login_wait_Object;
        QJsonObject jsonObj_playlistInfo;
        QJsonArray jsonArr_tracks_toPlay;                       ///< Track 전체를 재생하기 위함

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;

        bool flag_draw = false;

        int roseServer_playlistId = 0;
        int roseServer_trackId[99999];

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
#endif // APPLEPLAYLISTDETAIL_H
