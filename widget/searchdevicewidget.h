#ifndef SEARCHDEVICEWIDGET_H
#define SEARCHDEVICEWIDGET_H

#include "data/datadevice.h"

#include "common/findip.h"
#include "common/linker.h"//c220623

#include <QWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QProgressBar>
#include <QPushButton>//c220625


/**
 * @brief 로즈기기 검색 Widget @n
 *      로컬 네트워크의 연결 가능한 로즈기기 검색 및 선택
 */
class SearchDeviceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SearchDeviceWidget(QWidget *parent=nullptr);
    ~SearchDeviceWidget();

    void searchDevice(QString p_ip);

    bool saveCurrDeviceData();
    bool getFlagSelected();
    bool getFlagHasDeviceList();

signals:
    void signal_searchWidget_adjust(bool flag);
    void signal_selectedDevice(QString p_ip);

private slots:
    void searchDeviceIP();
    void responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

    void slot_selectedDevice();

    void slot_searchWidget_adjust();

private:
    void setInit();
    void setUIControl();

    void clearDeviceList();
    void appendDeviceWidget(const int &p_index, const QString &p_deviceName, const QString &p_deviceIP, const QString &p_deviceType, const QString &p_deviceVersion);

    void showStatusMsg(QString p_msg);
    void setResultOfDeviceList(const QJsonObject &p_jsonObject);

private:
    Linker *linker;//c220623
    FindIP *findIp;

    QMetaObject::Connection sereachConID[255];//c211213

    QList<DataDevice*> list_device;

    QWidget *widget_search_device;
    QWidget *widget_device_info;

    QVBoxLayout *vbox_searchDevice;
    QStackedWidget *stackWidget_search;

    QProgressBar *progress_searchBar;

    QLineEdit *le_search_ip;
    QPushButton *btn_refresh_ico;

    QPushButton *btn_searchWidget_adjust;

    QLabel *lb_searchWidget_adjust;
    QLabel *lb_search_status;

    bool flagReqUseIP = false;

    int cntResponse = 0;
    int cntWaitingResponse = 0;
    int selectedIndex = 0;

    bool flag_adjust = false;
    QTimer* buttonTimer =  nullptr;//c230405
    int sectTerm = 0;
};

#endif // SEARCHDEVICEWIDGET_H
