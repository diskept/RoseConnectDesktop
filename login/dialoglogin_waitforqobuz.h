#ifndef DIALOGLOGIN_WAITFORQOBUZ_H
#define DIALOGLOGIN_WAITFORQOBUZ_H

#include "widget/onoffwidget.h"
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QJsonObject>


#include <QLineEdit>

namespace qobuz {

    /**
     * @brief Qobuz 서비스에 로그인하기 위한 다이얼로그
     * @author sunnyfish
     */
    class DialogLogin_WaitforQobuz : public QDialog
    {
        Q_OBJECT
    public:
        explicit DialogLogin_WaitforQobuz(QWidget *parent = nullptr);

    signals:
        void successLogin();

    private:

        const int DLG_WIDTH = 700;
        const int DLG_HEIGHT= 350;

        // 초기 UI화면 구성
        void setInit();
        void setUIControl();
        void setUIControl_topTitle();
        void setUIControl_body();

        // UI 구성
        QVBoxLayout *vl_total;      ///< 화면 UI구성 전체 마지막 주관하는 Layout

    private slots:
        void slot_successLogin();

    };
};

#endif // DIALOGLOGIN_WAITFORQOBUZ_H
