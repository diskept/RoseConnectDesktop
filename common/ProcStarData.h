#ifndef PROCSTARDATA_H
#define PROCSTARDATA_H

#include <QObject>
#include "widget/OptMorePopup.h"

namespace common {

    /**
     * @brief 별점 처리 전용 클래스.  OptMorePopup::PopupMenuMode 에 따라 다름
     * @details 주로 OptMorePopup 클래스에서 자동으로 처리를 위해서 사용을 하지만, 다른 클래스에서도 사용할 수 있다.
     */
    class ProcStarData : public QObject
    {
        Q_OBJECT
    public:
        explicit ProcStarData(QWidget *parent = nullptr);
        ~ProcStarData();

        void request_getStarPoint(const OptMorePopup::PopupMenuMode, const QString data_pk);            // data_pk 는 아래 proc_reqRose~ 함수의 id 해당하는 데이터라고 보면 됨
        void request_setStarPoint(const OptMorePopup::PopupMenuMode, const QString data_pk, const int starRate);


    signals :
        void signal_completed_getStarRate(const int starRate);          ///< 별점 정보 Getting 완료 시그널
        void signal_completed_setStarRate(const int starRate);          ///< 별점 정보 Setting 완료 시그널. (전달 받았던 starRate를 받음)

    private slots:
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);


    private :

        int curr_starRate;           ///< signal_completed_setStarRate 발생 시 전달해주기 위한 값.  request_setStarPoint 요청 시 저장해둔다.

        // 실제 각각 Rose API 호출하는 녀석들임. 현재 공통으로 사용하더라도, 차후 API 변경건이 있을수 있으므로 함수를 메뉴별, 타입단위로 구분함
        void proc_reqRose___music_album___get(const QString& id_album);
        void proc_reqRose___music_album___set(const QString& id_album, const int starRate);

        void proc_reqRose___music_playlist___get(const QString& id_playlist);
        void proc_reqRose___music_playlist___set(const QString& id_playlist, const int starRate);

        void proc_reqRose___music_track___get(const QString& id_track);
        void proc_reqRose___music_track___set(const QString& id_track, const int starRate);

        // rosetube 에서, 해당 API 체크 후에 parameter를 보내도록 한다.
        void proc_reqRose___rosetube_video___get(const QString& id_channelId);
        void proc_reqRose___rosetube_video___set(const QString& id_channelId, const int starRate);

    };



};


#endif // PROCSTARDATA_H
