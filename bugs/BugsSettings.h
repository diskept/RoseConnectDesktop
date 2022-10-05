#ifndef BUGSSETTINGS_H
#define BUGSSETTINGS_H

#include "bugs/AbstractBugsSubWidget.h"
#include <QStackedWidget>
#include <QWidget>

namespace bugs {

    /**
     * @brief 벅스 > 설정 화면 클래스
     */
    class BugsSettings : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsSettings(QWidget *parent = nullptr);
        ~BugsSettings();

        void applyLoginSuccessState() override;

    private slots:
        void slot_change_state();

    private:
        // 초기 UI화면 구성
        void setUIControl_logined() override;

    private:
        QTimer *timer;

        QLabel *label_userInfo;
        QLabel *label_ticketName;

        bool flag_login_check = false;
        bool flag_login_before = false;
    };

};

#endif // BUGSSETTINGS_H
