#ifndef REMOTEWIDGET_H
#define REMOTEWIDGET_H

#include "home/remotebtnitem.h"

#include "common/linker.h"
#include "common/networkhttp.h"

#include "widget/searchdevicewidget.h"
#include "widget/VerticalScrollArea.h"

#include <QSlider>
#include <QWidget>


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

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

    void slot_connectedDevice();  // 장치 연결 슬롯
    void slot_searchWidget_adjust(bool flag);

    void slot_volume_set(int p_value);
    void slot_volume_mute();

    void slot_clicked_remoteItem();

private:
    void setUIRose();
    void setUIRemoteBtn();

    void dialog_comfirm(NetworkHttp *network, int type);

private:
    Linker *linker;

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
};

#endif // REMOTEWIDGET_H
