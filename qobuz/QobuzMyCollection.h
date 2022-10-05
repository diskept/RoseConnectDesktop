#ifndef QOBUZMYCOLLECTION_H
#define QOBUZMYCOLLECTION_H

#include "qobuz/AbstractQobuzSubWidget.h"
#include "delegate/QobuzTrackDelegate.h"

#include <QStackedWidget>

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {


    /**
     * @brief Qobuz > My Collection 화면 클래스
     */
    class QobuzMycollection : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzMycollection(QWidget *parent = nullptr);
        ~QobuzMycollection();

        void setActivePage() override;

    protected slots:
        void slot_acceptedDialogLogin() override;

    private slots:
        void slot_clickBtn_specificPart();

    private:
        // 초기 UI화면 구성
        void setUIControl_notLogin();
        void setUIControl_logined();

        void setUIControl_btnH_part();

    private:
        QWidget *widget_logined = nullptr;
        QWidget *widget_notLogin = nullptr;

        bool list_drawing = false;

    };

}

#endif // QOBUZMYCOLLECTION_H
