#ifndef INTROWELCOME_H
#define INTROWELCOME_H

#include <QWidget>
#include <QLabel>

/**
 * @brief 인트로 Step 1, 웰컴 화면
 *
 * @note 인트로 화면은 최초 설치시 일회성으로 보임
 */
class IntroWelcome : public QWidget
{
    Q_OBJECT
public:
    explicit IntroWelcome(QWidget *parent = nullptr);
    ~IntroWelcome();

signals:
    void clickedNext();

private:

    QLabel *lb_img;
    QWidget *wg_right;

    void setInit();
    void setUIControl();

    void resizeEvent(QResizeEvent *event) override;

};

#endif // INTROWELCOME_H
