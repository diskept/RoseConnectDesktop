#ifndef TIDALSETTINGS_H
#define TIDALSETTINGS_H

#include <QVBoxLayout>
#include <QStackedWidget>
#include "AbstractTidalSubWidget.h"
#include "tidal/AbstractTidalSubWidget.h"
#include <QWidget>

#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief "타이달 > 설정" 화면
     */
    class TidalSettings : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalSettings(QWidget *parent = nullptr);
        ~TidalSettings();
        void setActivePage() override;
        void slot_clickBtnLogout();//c230329

    signals:

    private slots:
        void slot_acceptedDialogLogin() override;
        //void slot_clickBtnLogout();//c230329
        void slot_successProcLogout();
        void slot_change_logout();

        void slot_applyResult_getSubscription(const QString errorMsg, const QJsonObject& jsonObj_session);

    private:
        // 초기 UI화면 구성
        void setUIControl_notLogin();
        void setUIControl_logined();

        QWidget *widget_logined = nullptr;
        QWidget *widget_notLogin = nullptr;

        QLabel *label_userInfo;
        QLabel *label_StreamingInfo;
        QStackedWidget *stacked_widget;

        QTimer *timer;
        bool login_state = false;

    };
}

#endif // TIDALSETTINGS_H
