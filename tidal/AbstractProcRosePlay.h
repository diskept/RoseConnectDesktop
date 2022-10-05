#ifndef ABSTRACTPROCROSEPLAY_H
#define ABSTRACTPROCROSEPLAY_H

#include "widget/OptMorePopup.h"
#include "roseHome/rosehome_struct.h"
#include "tidal/tidal_struct.h"
#include "bugs/bugs_struct.h"
#include "qobuz/qobuz_struct.h"
#include "apple/Apple_struct.h"

#include <QObject>
#include <QJsonArray>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


/**
 * @brief Rose API 호출 처리를 담당하는 클래스들의 Abstract Class
 * @details Bugs, Tidal, Qobuz 에서 사용
 */
class AbstractProcRosePlay : public QObject
{
    Q_OBJECT
public:

    enum PlayShuffleMode {
        JustPlay                ///< shuffle=0
        , ShufflePlay           ///< shuffle=1
        , NoUse                 ///< shuffle 옵션 자체가 없음
    };
    Q_ENUM(PlayShuffleMode);

    explicit AbstractProcRosePlay(QWidget *parent = nullptr);

    signals :
        void signal_cannotRosePlay(const QString errorMsg);

    protected slots:
        virtual void slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj)=0;
        virtual void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&){ };
        virtual void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&){ };
        virtual void slot_applyResult_tracks(const QList<qobuz::TrackItemData>&, const QJsonArray&){ };
        virtual void slot_applyResult_tracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool){ };
        virtual void slot_applyResult_tracks(const QJsonArray&){ };

    protected:
        QWidget *parent;            ///< ProcCommon 생성 목적으로 가지고 있는다.
        OptMorePopup::ClickMode curr_clickMode;
        PlayShuffleMode curr_shuffleMode;

        int get_rose_playType(OptMorePopup::ClickMode clickMode);

        QJsonArray get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode);
        QJsonArray subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex);
        QJsonArray reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex);

};

#endif // ABSTRACTPROCROSEPLAY_H
