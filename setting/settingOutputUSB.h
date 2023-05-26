#ifndef SETTINGOUPUTUSB_H
#define SETTINGOUPUTUSB_H

#include "home/remotebtnitem.h"

#include "widget/onoffwidget.h"
#include "widget/dialogconfirm.h"

#include <QDialog>
#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QKeyEvent>
#include <QJsonObject>
#include <QRadioButton>


class SettingOutputUSB : public QDialog
{
    Q_OBJECT
public:
    explicit SettingOutputUSB(QWidget *parent = nullptr);

signals:
    void signal_loading_stateUSB(const bool);
    void signal_setOutput_stateUSB(const QJsonObject);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

    void slot_clickedQuestion_clarification();
    void slot_clickedCheckbox();
    void slot_clickedApply();

private:
    void setInit();
    void setUIControl();

    void getOutputSettingUSB();
    void setOutputSettingUSB();

private:
    QWidget *config_usb = nullptr;

    QComboBox *comboBox_mqaType = nullptr;
    QComboBox *comboBox_maxSampleRate = nullptr;
    QComboBox *comboBox_sampleRate = nullptr;
    QComboBox *comboBox_dsdMode = nullptr;

    QPushButton *btn_mqaStream = nullptr;
    QPushButton *btn_apply = nullptr;
    QPushButton *btn_cancel = nullptr;

    QCheckBox *checkbox_mqaStream = nullptr;

    OnOffWidget *onoff_svc = nullptr;
    OnOffWidget *onoff_32bit = nullptr;

    QJsonObject jsonObj_USB_Setting;

    QString deviceType = "";
};
#endif // SETTINGOUPUTUSB_H
