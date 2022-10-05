#ifndef PROCBUGS_FOROUTER_H
#define PROCBUGS_FOROUTER_H

#include <QObject>

#include <QObject>
#include <QWidget>
#include "bugs_struct.h"
#include "widget/OptMorePopup.h"

namespace bugs {

    /**
     * @brief Bugs 의 Abstract ~ 클래스를 상속받지 않은, 다른 클래스에서 Bugs 의 기능 사용을 위해서,
     * 그것만 처리하는 클래스를 만들어서 중계역할을 담당하는 클래스.
     */
    class ProcBugs_forOuter : public QObject
    {
        Q_OBJECT

    public:
        explicit ProcBugs_forOuter(QWidget *parent = nullptr);
        ~ProcBugs_forOuter();

        static int extract_track_id_onTrackData(const QJsonObject& jsonObj_subAppCurrentData);
        static int extract_video_id_onVideoData(const QJsonObject& jsonObj_subAppCurrentData);

        // get data
        static bool extract_track_likes_yn(const QJsonObject& jsonObj_subAppCurrentData);
        static bool extract_track_has_musicVideo(const QJsonObject& jsonObj_subAppCurrentData);

        static OptMorePopup::HeaderData getConvertOptHeaderData_of_track(const QJsonObject& jsonObj_subAppCurrentData);
        static OptMorePopup::HeaderData getConvertOptHeaderData_of_video(const QJsonObject& jsonObj_subAppCurrentData);

        static QJsonObject makeJsonObj_toMovePage_albumDetail_fromTrack(const QJsonObject& jsonObj_subAppCurrentData);
        static QJsonObject makeJsonObj_toMovePage_artistDetail_fromTrack(const QJsonObject& jsonObj_subAppCurrentData);

        static QJsonObject makeJsonObj_toMovePage_artistDetail_fromVideo(const QJsonObject& jsonObj_subAppCurrentData);


        void request_procFav_ofTrack(const bool curr_isFav, const QJsonObject& jsonObj_subAppCurrentData);      // Fav 처리

    private:
        QWidget *parent = nullptr;
//        bool flagShowDebug_funcName = true;
        bool flagShowDebug_funcName = false;

    };

}


#endif // PROCBUGS_FOROUTER_H
