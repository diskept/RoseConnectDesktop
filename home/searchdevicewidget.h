#ifndef SEARCHDEVICEWIDGET_H
#define SEARCHDEVICEWIDGET_H

#include "data/datadevice.h"
#include "common/findip.h"

#include <QWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QProgressBar>

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

private:
    QMetaObject::Connection sereachConID[255];//c211213
    FindIP *findIp;
    QList<DataDevice*> list_device;
    QList<QLabel*> list_checkImg;
    QLineEdit *le_ip;
    QVBoxLayout *boxDeviceList;
    QLabel *lb_status;
    QStackedWidget *stackDeviceList;

    QProgressBar *searchProgressBar = new QProgressBar;     //cheon05_progress_bar

    //int flagReqUseIP;
    bool flagReqUseIP;
    int cntResponse;
    int cntWaitingResponse;
    int selectedIndex;

    void setInit();
    void setUIControl();
    void clearDeviceList();
    void appendDeviceWidget(const int &p_index, const QString &p_deviceName, const QString &p_deviceIP, const QString &p_deviceType );
    void showStatusMsg(QString p_msg);
    void setResultOfDeviceList(const QJsonObject &p_jsonObject);


signals:
    void signal_searchDeviceBtnClick();//c220623
    void signal_selectedDevice();//c220623
    void signal_selectedDevice(QString p_ip);


private slots:
    void searchDeviceIP();
    void responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_selectedDevice();

};

#endif // SEARCHDEVICEWIDGET_H
