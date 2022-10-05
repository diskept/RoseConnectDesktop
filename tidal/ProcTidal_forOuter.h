#ifndef PROCTIDAL_FOROUTER_H
#define PROCTIDAL_FOROUTER_H

#include <QObject>
#include <QWidget>
#include "tidal_struct.h"
#include "widget/OptMorePopup.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal 의 Abstract ~ 클래스를 상속받지 않은, 다른 클래스에서 Tidal 의 기능 사용을 위해서,
     * 그것만 처리하는 클래스를 만들어서 중계역할을 담당하는 클래스.
     */
    class ProcTidal_forOuter : public QObject
    {
        Q_OBJECT
    public:
        explicit ProcTidal_forOuter(QWidget *parent = nullptr);
        ~ProcTidal_forOuter();


        // get data
        static int extract_track_id_onTrackData(const QJsonObject& jsonObj_subAppCurrentData);
        static int extract_video_id_onVideoData(const QJsonObject& jsonObj_subAppCurrentData);

        // get data to send it to OptMorePopup
        static OptMorePopup::HeaderData getConvertOptHeaderData_track(const QJsonObject& jsonObj_subAppCurrentData);

        static QJsonObject makeJsonObj_toMovePage_albumDetail_fromTrack(const QJsonObject& jsonObj_subAppCurrentData);
        static QJsonObject makeJsonObj_toMovePage_artistDetail_fromTrack(const QJsonObject& jsonObj_subAppCurrentData);



        // request
        void request_flagFav_ofTrack(const QJsonObject& jsonObj_subAppCurrentData);                             // Track의 Fav 여부 요청
        void request_flagFav_ofVideo(const QJsonObject& jsonObj_subAppCurrentData);                             // Video의 Fav 여부 요청
        void request_tidal_add_track_in_myPlaylist(const QString playlist_uuid_target, const int track_id);     // 플레이리스트 담기 처리 요청

        void request_procFav_ofTrack(const bool curr_isFav, const QJsonObject& jsonObj_subAppCurrentData);      // Fav 처리


    signals:
        void completeReq_flagFav_onTrack(const int track_id, const bool flagFavOn);             // signal - Track의 Fav 현재값 반환
        void completeReq_flagFav_onVideo(const int track_id, const bool flagFavOn);             // signal - Video의 Fav 현재값 반환
        void completeReq_add_track_in_myPlaylist();                                             // signal - 플레이리스트 담기 처리완료


    private slots:
        void slot_tidal_completeReq_listAll_myFavoritesIds___forTrack(const QJsonObject& p_jsonObj);
        void slot_tidal_completeReq_listAll_myFavoritesIds___forVideo(const QJsonObject& p_jsonObj);
        void slot_done_addPlaylist();

    private:
//        bool flagShowDebug_funcName = true;
        bool flagShowDebug_funcName = false;

        QWidget *parent = nullptr;

        tidal::TrackItemData data_track;        // 현재 처리중인 Track 정보
        tidal::VideoItemData data_video;        // 현재 처리중인 Video 정보

    };

};


#endif // PROCTIDAL_FOROUTER_H
