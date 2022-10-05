#ifndef DIALOGLOGIN_NETWORK_H
#define DIALOGLOGIN_NETWORK_H

#include <QDialog>
#include <QLineEdit>
#include <QKeyEvent>

namespace Dialog{

/**
 * @brief 네트워크 폴더 로그인 Dlg @n
 * @note Window 의존 @n
 *      QProcess를 이용해 Window net.exe 네트워크 등록 명령 처리 @n
 *      익명 로그인 지원 위해 ID, PW 유효성 검사하지 않음
 */
class DialogLogin_network : public QDialog
{
    Q_OBJECT
public:
    explicit DialogLogin_network(const QString &ip, const QString &hostName, QWidget *parent = nullptr);

    void setHostName(const QString &p_hostName);

private slots:
    void slot_ok();

private:

    const int DLG_WIDTH = 600;
    const int DLG_HEIGHT= 325;

    QString ip;
    QString hostName;

    QLineEdit *lineEdit_id;
    QLineEdit *lineEdit_pw;

    void setUIControl();
    bool checkUserInput();

    void keyPressEvent(QKeyEvent *event) override{
        if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
            return;
        }
        QDialog::keyPressEvent(event);
    };

};
};

#endif // DIALOGLOGIN_NETWORK_H
