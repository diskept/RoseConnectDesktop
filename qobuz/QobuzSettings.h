#ifndef QOBUZSETTINGS_H
#define QOBUZSETTINGS_H

#include <QVBoxLayout>
#include <QStackedWidget>
#include "AbstractQobuzSubWidget.h"


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief "타이달 > 설정" 화면
     */
    class QobuzSettings : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzSettings(QWidget *parent = nullptr);
        ~QobuzSettings();

        void setActivePage() override;
        void slot_clickBtnLogout();//c230329

    signals:

    protected slots:
        void slot_acceptedDialogLogin() override;

    private slots:
        //void slot_clickBtnLogout();//c230329
        void slot_successProcLogout();

        void slot_change_device_state(const QString&);

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
}

#endif // QOBUZSETTINGS_H
