#ifndef DIALOGJOIN_H
#define DIALOGJOIN_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>


namespace Dialog{

/**
 * @brief 회원가입 다이얼로그
 */
class DialogJoin : public QDialog
{
    Q_OBJECT
public:
    explicit DialogJoin(QWidget *parent = nullptr);

signals:

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

    void slot_clickedJoin();
    void slot_duplicateNickName();

private:
    bool flagDuplicateNickName = false;
    QLineEdit *le_email;
    QLineEdit *le_nickName;
    QLineEdit *le_pw;
    void setUIControl();
    void setResponseNickNameCheck(QJsonObject p_jsonObject);
    void setResponseJoinMember(QJsonObject p_jsonObject);

protected:

};

};

#endif // DIALOGJOIN_H
