#ifndef ROSETUBE_PLAYLIST_H
#define ROSETUBE_PLAYLIST_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QCoreApplication>


namespace rosetube {

    /**
     * @brief 濡쒖쫰쒕툕 뚮젅대━ㅽ듃 곸꽭 @n
     *      뚮젅대━ㅽ듃 basic info 뺤씤 諛蹂꾩젏 泥섎━, 洹몃━怨몃옓 뺣낫 愿由섑듃,蹂꾩젏,젣) 媛     *
     * @note 몃옓 젣My Playlist쒗븯媛     */
    class RoseTube_PlayList : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseTube_PlayList(QWidget *parent = nullptr);
        ~RoseTube_PlayList();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 섏씠吏 Show 붿껌  곗씠꾨떖諛쏅뒗 ⑸룄
        void setActivePage() override;

    protected slots:
        // PlaylistImageDetailInfosignal 泥섎━瑜꾪빐
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_playlist(const roseHome::PlaylistItemData&);
        void slot_applyResult_playlist_tracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_getRating_thumbup(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);

    private:
        void request_more_trackData();
        void request_more_trackDraw();

    private:
        Linker *linker;

        AbstractImageDetailContents_RHV *playlist_detail_info;
        PlaylistTrackDetailInfo_RHV *playlist_track_info[99999];

        QString playlistTitle = "";
        int playlistId = 0;

        // UI
        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        // data
        roseHome::PlaylistItemData data_playlist;
        QList<roseHome::TrackItemData> *list_track;

        QJsonArray jsonArr_tracks_toPlay;                       ///< Track 꾩껜瑜ъ깮섍린 꾪븿

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;

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
        bool flag_playlist_thumb = false;

        bool flag_send_playlist = false;
        bool flag_check_playlist = false;
        bool flag_send_track = false;
        bool flag_check_track = false;
        bool flag_check_thumb = false;

        bool flag_track_ok = false;
    };
};

#endif // ROSETUBE_PLAYLIST_H
