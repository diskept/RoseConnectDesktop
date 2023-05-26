#ifndef QOBUZMYPLAYLISTDETAIL_H
#define QOBUZMYPLAYLISTDETAIL_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "delegate/QobuzTrackDelegate.h"

#include "widget/PlaylistImageDetailInfo.h"
#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QListWidget>


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief My Playlist 1개 상세화면. 편집 기능 연결됨
     */
    class QobuzMyPlaylistDetail : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzMyPlaylistDetail(QWidget *parent = nullptr);
        ~QobuzMyPlaylistDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots :
        // PlaylistImageDetailInfo의 signal 처리를 위해
        //void slot_detailInfo_btnClicked(const AbstractImageDetailInfo::BtnClickMode) override;
        void slot_detailInfo_btnClicked(const AbstractImageDetailInfo_forQobuz::BtnClickMode) override;
        void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode) override;

        void slot_thumbnailDownloaded_fotListWidget_track() override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

        // My created playlist 처리에 대해서 UI갱신 처리
        void proc_preUpdate_delete_myCreatePlaylist(const ItemPositionData&) override;

    private slots:
        void slot_applyResult_items_of_playlist(const QList<qobuz::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject&);

    private:
        // function about UI
        void setUIControl_basic();

        int  track_count = 0;
        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;
        void request_more_trackData();

        // UI
        QLabel *label_mainTitle;
        //PlaylistImageDetailInfo *playlistImageDetailInfo;
        PlaylistImageDetailInfo_forQobuz *playlistImageDetailInfo;
        QListWidget *listWidget_track;

        // delegate
        QobuzTrackDelegate *delegate;

        // data
        qobuz::PlaylistItemData data_playlist;
        QList<qobuz::TrackItemData> *list_track;    ///< Playlist 내의 Track 정보
        QJsonArray jsonArr_tracks_toPlay;                  ///< Track 전체를 재생하기 위함
    };





    /**
     * @brief My Playlist 1개 상세화면. 편집 기능 연결됨
     */
    class QobuzMyPlaylistDetail_ROSE : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzMyPlaylistDetail_ROSE(QWidget *parent = nullptr);
        ~QobuzMyPlaylistDetail_ROSE();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        // PlaylistImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_playlist(const roseHome::PlaylistItemData&);
        void slot_applyResult_playlist_tracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);

    private:
        void request_more_trackData();
        void request_more_trackDraw();

        void slot_applyResult_getShareLink(const QString &link);//c220818

        void slot_applyResult_getRating_playlist(const QJsonArray&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_getRating_thumbup(const QJsonObject&);

        void slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject&);

    private:
        AbstractImageDetailContents_RHV *playlist_detail_info;
        PlaylistTrackDetailInfo_RHV *playlist_track_info[99999];

        // UI
        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        // data
        roseHome::PlaylistItemData data_playlist;
        QList<roseHome::TrackItemData> *list_track;

        QJsonArray jsonArr_tracks_toPlay;                       ///< Track 전체를 재생하기 위함

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;

        bool flag_draw = false;

        int ownerId_chk = 0;

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
        bool flag_playlist_thumb = false;

        bool flag_send_playlist = false;
        bool flag_check_playlist = false;
        bool flag_send_track = false;
        bool flag_check_track = false;
        bool flag_check_thumb = false;

        bool flag_track_ok = false;
    };

};

#endif // QOBUZMYPLAYLISTDETAIL_H
