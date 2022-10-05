#ifndef ROSETUBE_SUBSCRIPTION_H
#define ROSETUBE_SUBSCRIPTION_H

#include "login/dialoglogin.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "widget/dialogconfirm.h"

#include <QListWidget>


namespace rosetube {

    /**
     * @brief 濡쒖쫰쒕툕-援щ룆 @n
     *      ъ슜먭 援щ룆以묒씤 梨꾨꼸 紐⑸줉쇰줈 援ъ꽦 @n
     *      紐⑸줉 대┃@sa RoseTube_Channel 梨꾨꼸 붾㈃쇰줈 대룞
     */
    class RoseTube_Subscription : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseTube_Subscription(QWidget *parent = nullptr);
        ~RoseTube_Subscription();

        void setActivePage() override;

    signals:
        void signal_clickedChannel(const QJsonObject &p_jsonObject);

    protected slots:
        void slot_responseHttp(const int&, const QJsonObject&) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_hide_msg();
        void slot_showLoginPage();
        void slot_getMyInfo_loginAfter();
        void slot_change_device_state(const QString&);

        void slot_applyResult_subscription(const QJsonObject&);

        void slot_clickSubscription();
        void slot_clickSubsChannel();

    private:
        // 珥덇린 UI붾㈃ 援ъ꽦
        void setUIControl_requestSubscription();
        void setUIControl_drawSubscription();

        void setUIControl_requestSetSubscription();

        QWidget* setUIControl_LoginBefore();

        QWidget* setUIControl_subscription_list(const int idx, const QString channel_id, const QString imagePath, const QString title, const QString description);

    private:
        Linker *linker;

        DialogConfirm *dlgSubscribe;
        Dialog::DialogLogin *dlg_rose_login;

        QWidget *widget_subscription[99999];

        QVBoxLayout *box_rose_contents;
        QVBoxLayout *box_login_contents;

        QWidget *widget_rose_contents;
        QWidget *widget_login_contents;
        QStackedWidget *stackedWidget_Contents;

        QJsonArray subscription_Arr;

        QString cancle_channel_id = "";

        int next_offset = 0;
        int subscription_total_cnt = 0;
        int subscription_draw_cnt = 0;

        // getting more data
        bool flagReqMore_subscription = false;
        bool flag_lastPage_subscription = false;

        bool flag_subscription_draw = false;
    };
};

#endif // ROSETUBE_SUBSCRIPTION_H
