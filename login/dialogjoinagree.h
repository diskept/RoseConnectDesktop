#ifndef DIALOGJOINAGREE_H
#define DIALOGJOINAGREE_H

#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QWidget>


namespace Dialog{

/**
 * @brief 회원가입 개인정보 동의서 다이얼로그
 */
class DialogJoinAgree : public QDialog
{
    Q_OBJECT
public:
    explicit DialogJoinAgree(QWidget *parent = nullptr);

signals:

private slots:
    void slot_clickedJoin();
    void slot_clickedCheckBox(int state);

private:
    QLabel *lb_agree_content;
    QCheckBox *checkbox_agree;
    QPushButton *btn_join;
    void setUIControl();

protected:
    void showEvent(QShowEvent *) override;
};

};
#endif // DIALOGJOINAGREE_H
