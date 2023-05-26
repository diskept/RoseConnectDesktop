#ifndef REMOTEWIDGET_H
#define REMOTEWIDGET_H

#include "home/remotebtnitem.h"

#include "common/linker.h"
#include "common/networkhttp.h"

#include "widget/searchdevicewidget.h"
#include "widget/VerticalScrollArea.h"

#include <QSlider>
#include <QWidget>
#include <QMouseEvent>


/**
 * @brief The RemoteWidget class 리모컨 위젯 (리모컨 버튼과 현재 연결된 기기 표시)
 */
class RemoteWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RemoteWidget(QWidget *parent = nullptr);
    ~RemoteWidget();

    void setInitData();

signals:
    void connectNewDevice(QString p_ip);

    void clickedRemoteVU();
    void clickedRemoteEXTE();
    void clickedRemoteEXTH();//cheon01
    void clickedRemoteEXTT();//cheon01

protected:
    bool event(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

    void slot_connectedDevice();  // 장치 연결 슬롯
    void slot_searchWidget_adjust(bool flag);

    void slot_volume_set(int);
    void slot_volume_mute();
    void slot_volume_change(int&);
    void slot_slider_auto_change();

    void slot_clicked_remoteItem();

private:
    void setUIRose();
    void setUIRemoteBtn();

    void dialog_comfirm(NetworkHttp *network, int type);

private:
    Linker *linker;

    QTimer *timer;

    VerticalScrollArea *scrollArea_main;

    QVBoxLayout *vbox_remote_main;
    QStackedWidget *stackedWidget_remote;
    QWidget *widget_remote_onlyRose;
    QWidget *widget_remote_withAmp;

    QWidget *widget_curr_device;
    SearchDeviceWidget *widget_search;

    QLabel *lb_curr_img;
    QLabel *lb_curr_name;
    QLabel *lb_curr_id;
    QLabel *lb_curr_ip;

    QLabel *lb_timer;
    QLabel *lb_hdmi;
    QLabel *lb_volume;

    QPushButton *btn_mute;

    QSlider *slider_volume;

    int timer_set = 0;
    bool flag_airplay = false;
    bool flag_dlna = false;
    bool flag_spotify = false;
    bool flag_mute = false;

    bool flag_button_enable = false;
    bool flag_auto_change = false;
};





class VolSlider_Remote : public QSlider
{
    Q_OBJECT
public:
    VolSlider_Remote(QWidget *parent = nullptr) : QSlider(parent) {}

signals:
    void signal_vol_slider_clicked(int);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override {

        QSlider::mouseReleaseEvent(event);

        int value = this->maximum() - (event->y() / 3);

        if(value >= 99){
            value = 99;
        }
        else if(value <= 0){
            value = 0;
        }

        emit signal_vol_slider_clicked(value);
    };
};
#endif // REMOTEWIDGET_H
