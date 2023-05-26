#ifndef SETTINGINPUTCONFIG_H
#define SETTINGINPUTCONFIG_H

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

class SettingInputConfig : public QDialog
{
    Q_OBJECT
public:
    explicit SettingInputConfig(QString, QWidget *parent = nullptr);

signals:
    void signal_loading_stateConfig(const bool);
    void signal_setInput_stateConfig(const QJsonObject);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

    void slot_clickedQuestion_clarification();

    void slot_clickedApply();

private:
    void setInit();
    void setUIControl();

    void setUIControl_inputConfig();

    void getInputSettingConfig();
    void setInputSettingConfig();

private:
    QWidget *config_input = nullptr;

    QWidget *widget_eARC_power = nullptr;

    QPushButton *btn_apply = nullptr;
    QPushButton *btn_cancel = nullptr;

    OnOffWidget *onoff_eARC_power = nullptr;

    QJsonObject jsonObj_config_Setting;
    QString version = "";

    QString deviceType = "";
    QString configType = "";
};
#endif // SETTINGINPUTCONFIG_H
