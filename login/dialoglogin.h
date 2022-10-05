#ifndef DIALOGLOGIN_H
#define DIALOGLOGIN_H

#include "common/linker.h"
#include "widget/onoffwidget.h"
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QJsonObject>


#include <QLineEdit>

namespace Dialog{

/**
 * @brief 로그인 Dlg @n
 *      자동 로그인 옵션 제겅 @n
 *      로그인 성공 후 @sa Linker::signal_logined() 시그널 발생
 *
 * @note
 *      자동 로그인 On 처리 과정 @n
 *      로즈 기기 연결 >> 로즈 기기 로그인 정보 Get >> 로그인 정보 없을 경우 자동 로그인
 */
class DialogLogin : public QDialog
{
    Q_OBJECT
public:
    explicit DialogLogin(QWidget *parent = nullptr);


signals:

private:

    const int DLG_WIDTH = 700;
    const int DLG_HEIGHT= 630;
    const int HTTP_LOGIN = 99;
    const int HTTP_SET_ROSE_USER_INFO = 88;

    Linker *linker;

    QLineEdit *le_id;
    QLineEdit *le_pw;
    OnOffWidget *onOff;

    void setInit();
    void setUIControl();
    void setResultOfLogin(const QJsonObject &p_jsonData);
    bool checkUserInput();
    void saveLoginInfo();
    void requestRoseLogin();


private slots:
    void slot_clickedLogin();
    void slot_clickedJoin();
    void slot_clickedFindId();
    void slot_responseHttp(int p_id, QJsonObject p_jsonData);

};

};


#endif // DIALOGLOGIN_H
