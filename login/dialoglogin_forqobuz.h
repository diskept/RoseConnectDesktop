#ifndef DIALOGLOGIN_FORQOBUZ_H
#define DIALOGLOGIN_FORQOBUZ_H

//#include "common/linker.h"
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
    class DialogLogin_forQobuz : public QDialog
    {
        Q_OBJECT
    public:
        explicit DialogLogin_forQobuz(QWidget *parent = nullptr);

    signals:
        void successLogin();

    private:

        const int DLG_WIDTH = 700;
        const int DLG_HEIGHT= 450;

        // 초기 UI화면 구성
        void setInit();
        void setUIControl();
        void setUIControl_topTitle();
        void setUIControl_body();

        // HTTP 요청 및 응답처리
        bool checkUserInput();


        // UI 구성
        QVBoxLayout *vl_total;      ///< 화면 UI구성 전체 마지막 주관하는 Layout
        QLineEdit *lineEdit_email;
        QLineEdit *lineEdit_password;
        OnOffWidget *onOff;


    private slots:
        void slot_clickedLogin();
        void slot_failedLogin(const QString& errorMsg);
        void slot_successLogin();
        void slot_dialogClose();

    };

};

#endif // DIALOGLOGIN_FORQOBUZ_H
