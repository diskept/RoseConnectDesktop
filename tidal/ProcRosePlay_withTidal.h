#ifndef PROCROSEPLAY_WITHTIDAL_H
#define PROCROSEPLAY_WITHTIDAL_H

#include "tidal/AbstractProcRosePlay.h"

#include <QWidget>
//#include "widget/OptMorePopup.h"
//#include "tidal/tidal_struct.h"
//#include <QJsonArray>
#include "widget/AbstractImageDetailInfo.h"
#include "common/linker.h"//c1209


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal관련해서 Rose API 요청을 담당하는 처리 클래스
     * @details 객체 1개당 1개의 request만 처리하도록 한다.
     */
    class ProcRosePlay_withTidal : public AbstractProcRosePlay
    {
        Q_OBJECT

    public:
        enum HttpRequestType {
            Set_Session_Info = 4800
            , Get_Session_Info

            , PlayItem_of_Album = 4805         ///< Album의 트랙정보 요청
            , GetAll_UserFavorites      ///< 사용자의 모든 좋아요 정보 반환 (종류별로 id 목록들을 반환함)
            , Rose_tidalPlay_set_queue = 4810
            , Rose_tidalPlay_set_queue_video

            , Rose_tidalPlay_current_playlist
            , Rose_tidalPlay_current_playlist_delete
        };
        Q_ENUM(HttpRequestType);

        explicit ProcRosePlay_withTidal(QWidget *parent = nullptr);

        // tidal_set_session_info & tidal_get_session_info
        void request_set_session_info(const tidal::RoseSessionInfo_forTidal& p_sessionInfo);
        void request_get_session_info();

        // play by album_id
        void requestPlayRose_byAlbumID(const int album_id, OptMorePopup::ClickMode clickMode);
        void requestPlayRose_byAlbumID(const int album_id, AbstractImageDetailInfo::BtnClickMode btnClickMode);
        void requestPlayRose_byVisualAlbumID(const int album_id, const int trackCnt, OptMorePopup::ClickMode clickMode);
        void requestPlayRose_byVisualAlbumID(const int album_id, const int trackCnt, AbstractImageDetailInfo::BtnClickMode btnClickMode);

        // play video
        void requestPlayRose_video(const QJsonObject jsonObj_toPlay, OptMorePopup::ClickMode clickMode);
        void requestPlayRose_videolist(const QJsonArray jsonObj_toPlay, OptMorePopup::ClickMode clickMode);

        // play by playlist_uuid
        void requestPlayRose_byPlaylistUUID(const tidal::PlaylistItemData&, OptMorePopup::ClickMode clickMode);

        // play tracks
        void requestPlayRose_byTracks(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode, PlayShuffleMode shuffleMode = PlayShuffleMode::JustPlay);

        void request_tidalPlay_current_playlist(const int currentPosition, const QJsonObject& jsonObj_track);
        void request_recommPlay_current_playlist(const int currentPosition, const QJsonObject& jsonObj_track);

        void request_tidalPlay_current_playlist_delete(const int totalCnt, const QJsonArray& jsonArr_deletePosition, const QJsonArray& jsonArr_deleteArr);
        void request_tidalPlay_current_playlist_delete(const int totalCnt, const int deletePosition, const QJsonObject& jsonObj_deleteObj);


    signals:
        void completeReq_tidalPlay_current_playlist_delete();
        void signal_completeReq_get_session_info(const tidal::RoseSessionInfo_forTidal&);

    protected slots:
        // Rose에게 Session 요청
        void checkResult_set_session_info(const QJsonObject& p_jsonObj);
        void checkResult_get_session_info(const QJsonObject& p_jsonObj);

        void slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj) override;
        void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&) override;
        void slot_applyResult_items_of_playlist(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool&);

    private:
        // Rose에게 재생요청
        void request_rose_tidalPlay_set_queue(const int playType, const QJsonArray &jsonArr_tidalModeItem, PlayShuffleMode shuffleMode=PlayShuffleMode::NoUse);
        void request_rose_tidalPlay_set_queue_video(const int playType, const QJsonObject& jsonObj_toPlay, PlayShuffleMode shuffleMode=PlayShuffleMode::JustPlay);
        void request_rose_tidalPlay_set_queue_videolist(const int playType, const QJsonArray &jsonObj_toPlay, PlayShuffleMode shuffleMode=PlayShuffleMode::JustPlay);

    private:
        QJsonArray jsonArr_tracks_toPlay;
        QString playlist_uuid_check = "";
        int playlist_tracks_offset = 0;
    private://c1209
        Linker *linker;

    };

};

#endif // PROCROSEPLAY_WITHTIDAL_H
