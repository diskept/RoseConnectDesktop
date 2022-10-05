#ifndef APPLESETTINGS_H
#define APPLESETTINGS_H

#include "apple/AbstractAppleSubWidget.h"

#include <QVBoxLayout>
#include <QStackedWidget>


namespace apple{

    /**
     * @brief "APPLE > 설정" 화면
     */
    class AppleSettings : public AbstractAppleSubWidget
    {
        Q_OBJECT
    public:
        explicit AppleSettings(QWidget *parent = nullptr);
        ~AppleSettings();

        void setActivePage() override;

    /*protected slots:
        void slot_acceptedDialogLogin() override;

    private slots:
        void slot_clickBtnLogout();
        void slot_successProcLogout();

        void slot_change_device_state(const QString&);*/

    private:
        // 초기 UI화면 구성
        void setUIControl_notLogin();
        void setUIControl_logined();

    private:
        Linker *linker;

        QWidget *widget_logined = nullptr;
        QWidget *widget_notLogin = nullptr;

        QLabel *label_userInfo;
        QLabel *label_StreamingInfo;
        QStackedWidget *stacked_widget;
    };
};
#endif // APPLESETTINGS_H
