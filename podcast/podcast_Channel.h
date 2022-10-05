#ifndef PODCAST_CHANNEL_H
#define PODCAST_CHANNEL_H

#include "common/linker.h"

#include "data/data_pc_channel.h"
#include "delegate/podcastcontentsdelegate.h"

#include "widget/imagelabel.h"
#include "widget/dialogconfirm.h"

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>


namespace podcast {

    /**
     * @brief 팟 캐스트 채널 상세 @n
     *      해당 채널 basic info 및 컨텐츠 목록으로 구성
     *
     * @note 컨텐츠에 따라 duration 정보 없을 수 있음.
     */
    class Podcast_Channel : public QWidget
    {
        Q_OBJECT
    public:
        explicit Podcast_Channel(QWidget *parent = nullptr);

        void setData(const QJsonObject &p_jsonData);
        void requestSubScribeList();

    private slots:
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
        void slot_clickedSubscribe();
        void slot_delegateClicked(const int &p_index,const int &p_btnType);
        void slot_loadedThumb(QPixmap);

    private:
        void setInit();
        void setUIControl();
        void requestSubscribe();
        void requestContents();
        void setResultOfSubscribe(const QJsonObject &p_jsonData);
        void setResultOfSubscribeList(const QJsonObject &p_jsonData);
        void setResultOfContentsList(const QJsonObject &p_jsonData);
        void setResultOfPodcastPlay(const QJsonObject &p_jsonData);
        void setSubscribeBtnStyle(bool flagIsSubscribe);
        void appendContentsUI(const int &p_index, const QJsonObject &p_jsonData);

        QString getDayOfWeekStr(const int &p_dayOfWeek);
        QString getReleaseDate(const QString &p_strDate);
        QString getDuration(QString p_duration);

    private:
        Linker *linker;
        PodcastContentsDelegate *delegate;

        DialogConfirm *dlgSubscribe;

        //QPixmap pixmap_subscribe_on;
        //QPixmap pixmap_subscribe_off;
        //QString btnStyle_subscribe_on;
        //QString btnStyle_subscribe_off;

        ImageLabel *lb_img;
        QLabel *lb_title;
        QLabel *lb_artist;
        QPushButton *btn_subscribe;
        QLabel *lb_subscribe_img;
        QLabel *lb_subscribe;

        QListWidget *listWidget;

        Data_PC_Channel *data;

        bool flagSubscribe = false;
    };
};
#endif // PODCAST_CHANNEL_H
