#ifndef INTROWIFI_H
#define INTROWIFI_H

#include <QWidget>
#include <QLabel>

/**
 * @brief 인트로 Step 2, WIFI 연결 화면
 *
 * @note 인트로 화면은 최초 설치시 일회성으로 보임
 */
class IntroWifi : public QWidget
{
    Q_OBJECT
public:
    explicit IntroWifi(QWidget *parent = nullptr);
    ~IntroWifi();

signals:
    void clickedNext();

private:
    QLabel *lb_img;
    QWidget *wg_right;

    void setInit();
    void setUIControl();
    void resizeEvent(QResizeEvent *event) override;


};

#endif // INTROWIFI_H
