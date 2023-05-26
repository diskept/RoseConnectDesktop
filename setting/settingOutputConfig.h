#ifndef SETTINGOUPUTCONFIG_H
#define SETTINGOUPUTCONFIG_H

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


class SettingOutputConfig : public QDialog
{
    Q_OBJECT
public:
    explicit SettingOutputConfig(QString, QWidget *parent = nullptr);

signals:
    void signal_loading_stateConfig(const bool);
    void signal_setOutput_stateConfig(const QJsonObject);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

    void slot_clickedQuestion_clarification();
    void slot_clickedCheckbox();
    void slot_clickedPreOutLevel(int idx);
    void slot_clickedPreOutLevelOnOff(bool flagOn);
    void slot_clickedSoftwareVolumeOnOff(bool flagOn);
    void slot_clickedApply();

private:
    void setInit();
    void setUIControl();
    void setUIControl_etcConfig();
    void setUIControl_preampConfig();
    void setUIControl_speakerConfig();

    void getOutputSettingConfig();
    void setOutputSettingConfig();

private:
    QWidget *config_etc = nullptr;
    QWidget *config_pre = nullptr;
    QWidget *config_spk = nullptr;

    QWidget *widget_speaker_type = nullptr;
    QWidget *widget_preout_level = nullptr;
    QWidget *widget_software_volume = nullptr;
    QWidget *widget_DACs32bit = nullptr;
    QWidget *widget_firFilter = nullptr;
    QWidget *widget_pcm_freq = nullptr;
    QWidget *widget_dsd_mode = nullptr;

    QComboBox *comboBox_mqaType = nullptr;
    QComboBox *comboBox_maxSampleRate = nullptr;
    QComboBox *comboBox_sampleRate = nullptr;
    QComboBox *comboBox_dsdMode = nullptr;
    QComboBox *comboBox_preoutLevel = nullptr;
    QComboBox *comboBox_speakerType = nullptr;
    QComboBox *comboBox_firFilter = nullptr;

    QPushButton *btn_mqaStream = nullptr;
    QPushButton *btn_apply = nullptr;
    QPushButton *btn_cancel = nullptr;

    QCheckBox *checkbox_mqaStream = nullptr;

    OnOffWidget *onoff_svc = nullptr;
    OnOffWidget *onoff_32bit = nullptr;
    OnOffWidget *onoff_pre = nullptr;

    QJsonObject jsonObj_config_Setting;

    QString deviceType = "";
    QString configType = "";
};
#endif // SETTINGOUPUTCONFIG_H
