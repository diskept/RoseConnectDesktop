#ifndef PROCROSEAPI_WITHBUGS_H
#define PROCROSEAPI_WITHBUGS_H

#include "tidal/AbstractProcRosePlay.h"
#include <QWidget>
#include "widget/OptMorePopup.h"
//#include "tidal/tidal_struct.h"
#include <QJsonArray>
#include "widget/AbstractImageDetailInfo.h"
#include "common/linker.h"//c1209

namespace bugs {

    /**
     * @brief Bugs관련해서 Rose API 요청을 담당하는 처리 클래스
     * @details 객체 1개당 1개의 request만 처리하도록 한다.
     */
    class ProcRoseAPI_withBugs: public AbstractProcRosePlay
    {
        Q_OBJECT
    public:
        enum HttpRequestType {
            Set_Session_Info = 5000
            , Get_Session_Info

            , Rose_bugs_set_queue = 5010
            , Rose_bugs_set_queue_video

            , Rose_bugs_set_queue_single

            , Rose_bugsPlay_current_playlist_delete
        };
        Q_ENUM(HttpRequestType);


        explicit ProcRoseAPI_withBugs(QWidget *parent = nullptr);

        // bugs_set_session_info & bugs_get_session_info
        void request_set_session_info(const bugs::RoseSessionInfo_forBugs& p_sessionInfo);
        void request_get_session_info();



        // play by album_id
        void requestPlayRose_byAlbumID(const int album_id, OptMorePopup::ClickMode clickMode);
        void requestPlayRose_byAlbumID(const int album_id, AbstractImageDetailInfo::BtnClickMode btnClickMode);

        void requestPlayRose_byPD_AlbumID(const int es_album_id, OptMorePopup::ClickMode clickMode);
        void requestPlayRose_byPD_AlbumID(const int es_album_id, AbstractImageDetailInfo::BtnClickMode btnClickMode);

        void requestPlayRose_byMyAlbumID(const int playlist_id, OptMorePopup::ClickMode clickMode);
        void requestPlayRose_byMyAlbumID(const int playlist_id, AbstractImageDetailInfo::BtnClickMode btnClickMode);


        // play tracks
        void requestPlayRose_byTracks(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode, PlayShuffleMode shuffleMode = PlayShuffleMode::JustPlay);
        void requestPlayRose_byVideos(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode, PlayShuffleMode shuffleMode = PlayShuffleMode::JustPlay);

        void request_setQueue_single(const int currentPosition, const QJsonObject& jsonObj_track);//cheon210916-total

        void request_bugsPlay_current_playlist_delete(const int totalCnt, const QJsonArray& jsonArr_deletePosition, const QJsonArray& jsonArr_deleteArr);
        void request_bugsPlay_current_playlist_delete(const int totalCnt, const int deletePosition, const QJsonObject& jsonObj_deleteObj);


    signals:
        void signal_completeReq_get_session_info(const bugs::RoseSessionInfo_forBugs&);
        void completeReq_bugsPlay_current_playlist_delete();

    protected slots:
        void slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj) override;
        void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&) override;

    private slots:
        // Rose에게 Session 요청
        void checkResult_set_session_info(const QJsonObject& p_jsonObj);
        void checkResult_get_session_info(const QJsonObject& p_jsonObj);

        // Rose에게 재생요청
        void request_rose_bugsPlay_set_queue(const int playType, const QJsonArray &jsonArr_tidalModeItem, PlayShuffleMode shuffleMode=PlayShuffleMode::NoUse);
        void request_rose_bugsPlay_set_queue_video(const int playType, const QJsonArray& jsonArr_tidalModeItem, PlayShuffleMode shuffleMode=PlayShuffleMode::JustPlay);
    private://c1209
        Linker *linker;

    };

}


#endif // PROCROSEAPI_WITHBUGS_H
