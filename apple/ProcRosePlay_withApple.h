#ifndef PROCROSEPLAY_WITHAPPLE_H
#define PROCROSEPLAY_WITHAPPLE_H

#include "tidal/AbstractProcRosePlay.h"

#include "common/linker.h"

#include "widget/OptMorePopup.h"

#include <QWidget>
#include <QJsonArray>


namespace apple {

    class ProcRosePlay_withApple : public AbstractProcRosePlay
    {
        Q_OBJECT

    public:
        enum HttpRequestType {
            Set_Session_Info = 0
            , Get_Session_Info

            , Rose_applePlay_set_queue
        };
        Q_ENUM(HttpRequestType);

        explicit ProcRosePlay_withApple(QWidget *parent = nullptr);

        // apple_music_set_session_info & apple_music_get_session_info
        void request_set_session_info(const apple::RoseSessionInfo_forApple&);
        void request_get_session_info();

        // play Album
        void requestPlayRose_byAlbumID(const int, OptMorePopup::ClickMode);

        // play Playlist
        void requestPlayRose_byPlaylistID(const QString, OptMorePopup::ClickMode);

        // play Tracks
        void requestPlayRose_byTracks(const QJsonArray&, const int, OptMorePopup::ClickMode, PlayShuffleMode shuffleMode = PlayShuffleMode::JustPlay);

    signals:
        //void completeReq_tidalPlay_current_playlist_delete();
        void signal_completeReq_get_session_info(const apple::RoseSessionInfo_forApple&);

    protected slots:
        void slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj) override;
        void slot_applyResult_tracks(const QJsonArray&) override;

    private slots:


    private:
        // Rose에게 재생요청
        void request_rose_applePlay_set_queue(const int playType, const QJsonArray &jsonArr_appleModeItem, PlayShuffleMode shuffleMode=PlayShuffleMode::NoUse);

    private:
        Linker *linker;
    };
};
#endif // PROCROSEPLAY_WITHAPPLE_H
