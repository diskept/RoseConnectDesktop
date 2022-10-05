#ifndef PROCROSEPLAY_WITHROSEHOME_H
#define PROCROSEPLAY_WITHROSEHOME_H

//#include "widget/AbstractProcRosePlay_RHV.h"
//#include "widget/AbstractImageDetailInfo_RHV.h"
//#include "widget/OptMorePopup.h"
#include <common/linker.h>
#include "tidal/AbstractProcRosePlay.h"

#include <QWidget>
#include <QJsonArray>


namespace roseHome {

    /**
     * @brief Rosehome관련해서 Rose API 요청을 담당하는 처리 클래스
     * @details 객체 1개당 1개의 request만 처리하도록 한다.
     */
    class ProcRosePlay_withRosehome : public AbstractProcRosePlay
    {
        Q_OBJECT

    public:
        enum HttpRequestType {
            Set_Session_Info = 4800
            , Get_Session_Info

            , Rose_total_set_queue = 5200
            , Rose_total_set_queue_video

            , Rose_total_current_playlist
            , Rose_total_current_playlist_delete
        };
        Q_ENUM(HttpRequestType);

        explicit ProcRosePlay_withRosehome(QWidget *parent = nullptr);

        // tidal_set_session_info & tidal_get_session_info
        //void request_set_session_info(const tidal::RoseSessionInfo_forTidal& p_sessionInfo);
        void request_get_session_info();

        // play album
        void requestPlayRose_byAlbumID(const int album_id, OptMorePopup::ClickMode clickMode);
        void requestPlayRose_byAlbum(const roseHome::AlbumItemData&, const QJsonArray&, const int, OptMorePopup::ClickMode, PlayShuffleMode shuffleMode = PlayShuffleMode::JustPlay);

        // play playlist
        void requestPlayRose_byPlaylistID(const int playlist_id, OptMorePopup::ClickMode clickMode);
        void requestPlayRose_byPlaylist(const roseHome::PlaylistItemData&, const QJsonArray&, const int, OptMorePopup::ClickMode, PlayShuffleMode shuffleMode = PlayShuffleMode::JustPlay);

        // play tracks
        void requestPlayRose_byTracks(const roseHome::TrackItemData&, const QJsonArray&, const int, OptMorePopup::ClickMode, PlayShuffleMode shuffleMode = PlayShuffleMode::JustPlay);

        //void request_totalPlay_current_playlist(const int currentPosition, const QJsonObject& jsonObj_track);

        //void request_totalPlay_current_playlist_delete(const int totalCnt, const QJsonArray& jsonArr_deletePosition, const QJsonArray& jsonArr_deleteArr);
        //void request_totalPlay_current_playlist_delete(const int totalCnt, const int deletePosition, const QJsonObject& jsonObj_deleteObj);

    signals:
        void completeReq_tidalPlay_current_playlist_delete();
        void signal_completeReq_get_session_info();

    protected slots:
        void slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj) override;

    private slots:
        void slot_applyResult_albumTracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_playlsitTracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);

    private:
        // Rose에게 재생요청
        void request_rose_album_set_queue(const int, const QJsonObject&, const QJsonArray&, PlayShuffleMode shuffleMode = PlayShuffleMode::NoUse);
        void request_rose_playlist_set_queue(const int, const QJsonObject&, const QJsonArray&, PlayShuffleMode shuffleMode = PlayShuffleMode::NoUse);
        void request_rose_tracks_set_queue(const int, const QJsonObject&, const QJsonArray& , PlayShuffleMode shuffleMode = PlayShuffleMode::NoUse);

    private:
        QJsonArray jsonArr_tracks_toPlay;
        int album_id_check = 0;
        int playlist_id_check = 0;

        int pageCnt = 0;
        Linker *linker;
    };
};
#endif // PROCROSEPLAY_WITHROSEHOME_H
