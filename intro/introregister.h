#ifndef INTROREGISTER_H
#define INTROREGISTER_H

#include "common/findip.h"
#include "data/datadevice.h"
#include <QWidget>
#include <QList>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QStackedWidget>

#include "widget/searchdevicewidget.h"

#include <QLabel>
#include <QLineEdit>

/**
 * @brief 인트로 Step 3, 등록 화면
 *      @sa SearchDeviceWidget 를 이용해 로즈 기기 검색 및 선택 기능 제공
 *
 * @note 인트로 화면은 최초 설치시 일회성으로 보임
 */
class IntroRegister : public QWidget
{
    Q_OBJECT
public:
    explicit IntroRegister(QWidget *parent = nullptr);
    ~IntroRegister();

    void searchAllDevice();
    void saveDeviceInfoDB();


private:
    QLabel *lb_img;
    QWidget *wg_right;
    SearchDeviceWidget *wg_search;

    void setInit();
    void setUIControl();

    void resizeEvent(QResizeEvent *event) override;

signals:
    void clickedNext();


private slots:



    void complete();

};

#endif // INTROREGISTER_H
