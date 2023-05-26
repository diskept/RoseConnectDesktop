#ifndef SETTINGOUPUTHDMI_H
#define SETTINGOUPUTHDMI_H

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


class SettingOutputHDMI : public QDialog
{
    Q_OBJECT
public:
    explicit SettingOutputHDMI(QWidget *parent = nullptr);

signals:
    void signal_loading_stateHDMI(const bool);
    void signal_setOutput_stateHDMI(const QJsonObject);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

    void slot_clickedQuestion_clarification();
    void slot_clickedCheckbox();
    void slot_clickedApply();

private:
    void setInit();
    void setUIControl();

    void getOutputSettingHDMI();
    void setOutputSettingHDMI();

private:
    QWidget *config_hdmi = nullptr;

    QComboBox *comboBox_maxSampleRate = nullptr;
    QComboBox *comboBox_sampleRate = nullptr;
    QComboBox *comboBox_dsdMode = nullptr;

    QPushButton *btn_mqaStream = nullptr;
    QPushButton *btn_apply = nullptr;
    QPushButton *btn_cancel = nullptr;

    QCheckBox *checkbox_mqaStream = nullptr;

    OnOffWidget *onoff_svc = nullptr;
    OnOffWidget *onoff_pass = nullptr;
    OnOffWidget *onoff_32bit = nullptr;

    QJsonObject jsonObj_HDMI_Setting;

    QString deviceType = "";
};
#endif // SETTINGOUPUTHDMI_H
