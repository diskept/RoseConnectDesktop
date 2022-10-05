#ifndef PODCAST_SUBSCRIPTION_H
#define PODCAST_SUBSCRIPTION_H

#include "common/linker.h"

#include "data/data_pc_channel.h"

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QList>


namespace podcast {

    /**
     * @brief 팟캐스트 구독 @n
     *      사용자가 구독중인 채널 목록으로 구성
     */
    class Podcast_Subscription : public QWidget
    {
        Q_OBJECT
    public:
        explicit Podcast_Subscription(QWidget *parent = nullptr);
        ~Podcast_Subscription();

        void requestData(bool flagNeedRefresh = false);

    signals:
        void signal_clickedChannel(const QJsonObject &p_jsonObject);

    private slots:
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
        void slot_clickedChannel();
        void slot_clickedCancelSubscribe();

    private:
        void setInit();
        void setUIControl();
        void appendSubscriptionUI(const QJsonObject &p_jsonObject);
        void setResultOfSubscription(const QJsonObject &p_jsonObject);
        void setResultOfCancelSubscribe(const QJsonObject &p_jsonObject);
        void requestCancelSubscribe(const int &p_index);

        QLabel *lb_empty;
        QWidget *wg_contents;
        QWidget *wg_boxMain;
        QVBoxLayout *box_contents;
        QStackedWidget *stackedWidget;

        QList<Data_PC_Channel*> *listData;
        Linker *linker;

    };
};
#endif // PODCAST_SUBSCRIPTION_H
