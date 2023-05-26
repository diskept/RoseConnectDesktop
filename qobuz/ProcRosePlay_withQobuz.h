#ifndef PROCROSEPLAY_WITHQOBUZ_H
#define PROCROSEPLAY_WITHQOBUZ_H

#include "tidal/AbstractProcRosePlay.h"

#include "common/linker.h"//c1209

#include "widget/OptMorePopup.h"
#include "widget/AbstractImageDetailInfo.h"

#include <QWidget>
#include <QJsonArray>


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz관련해서 Rose API 요청을 담당하는 처리 클래스
     * @details 객체 1개당 1개의 request만 처리하도록 한다.
     */
    class ProcRosePlay_withQobuz : public AbstractProcRosePlay
    {
        Q_OBJECT

    public:
        enum HttpRequestType {
            Set_Session_Info = 5200
            , Get_Session_Info

            , PlayItem_of_Album = 5205         ///< Album의 트랙정보 요청
            , GetAll_UserFavorites           ///< 사용자의 모든 좋아요 정보 반환 (종류별로 id 목록들을 반환함)
            , Rose_qobuzPlay_set_queue = 5210
            , Rose_qobuzPlay_set_queue_video

            , Rose_qobuzPlay_current_playlist
            , Rose_qobuzPlay_current_playlist_delete
        };
        Q_ENUM(HttpRequestType);

        explicit ProcRosePlay_withQobuz(QWidget *parent = nullptr);

        // qobuz_set_session_info & qobuz_get_session_info
        void request_set_session_info(const qobuz::RoseSessionInfo_forQobuz& p_sessionInfo);
        void request_get_session_info();

        // play by album_id
        //void requestPlayRose_byAlbumID(const int album_id, OptMorePopup::ClickMode clickMode);
        //void requestPlayRose_byAlbumID(const int album_id, AbstractImageDetailInfo::BtnClickMode btnClickMode);   // Added diskept 27/12/2020
        void requestPlayRose_byAlbumID(const QString album_id, OptMorePopup::ClickMode clickMode);
        void requestPlayRose_byAlbumID(const QString album_id, AbstractImageDetailInfo::BtnClickMode btnClickMode);

        // play video
        void requestPlayRose_video(const QJsonObject jsonObj_toPlay, OptMorePopup::ClickMode clickMode);

        // play by playlist_uuid
        void requestPlayRose_byPlaylistID(const qobuz::PlaylistItemData&, OptMorePopup::ClickMode clickMode);

        // play tracks
        void requestPlayRose_byTracks(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode, PlayShuffleMode shuffleMode = PlayShuffleMode::JustPlay);

        void request_qobuzPlay_current_playlist(const int currentPosition, const QJsonObject& jsonObj_track);

        void request_qobuzPlay_current_playlist_delete(const int totalCnt, const QJsonArray& jsonArr_deletePosition, const QJsonArray& jsonArr_deleteArr);
        void request_qobuzPlay_current_playlist_delete(const int totalCnt, const int deletePosition, const QJsonObject& jsonObj_deleteObj);

        void request_applePlay_current_playlist(const int currentPosition, const QJsonObject& jsonObj_track);//c220304

    signals:
        void completeReq_qobuzPlay_current_playlist_delete();
        void signal_completeReq_get_session_info(const qobuz::RoseSessionInfo_forQobuz&);

    protected slots:
        // Rose에게 Session 요청
        void checkResult_set_session_info(const QJsonObject& p_jsonObj);
        void checkResult_get_session_info(const QJsonObject& p_jsonObj);

        void slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj) override;
        void slot_applyResult_tracks(const QList<qobuz::TrackItemData>&, const QJsonArray&) override;
        void slot_applyResult_items_of_playlist(const QList<qobuz::TrackItemData>&, const QJsonArray&, const bool&);

    private:
        // Rose에게 재생요청
        void request_rose_qobuzPlay_set_queue(const int playType, const QJsonArray &jsonArr_qobuzModeItem, PlayShuffleMode shuffleMode=PlayShuffleMode::NoUse);
        void request_rose_qobuzPlay_set_queue_video(const int playType, const QJsonObject& jsonObj_toPlay, PlayShuffleMode shuffleMode=PlayShuffleMode::JustPlay);

    private://c1209
        Linker *linker;

        QJsonArray jsonArr_tracks_toPlay;
        QString playlist_id_check = "";
        int playlist_tracks_offset = 0;
    };

};

#endif // PROCROSEPLAY_WITHQOBUZ_H
