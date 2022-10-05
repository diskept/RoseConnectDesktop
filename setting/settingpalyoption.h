#ifndef SETTINGPALYOPTION_H
#define SETTINGPALYOPTION_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
/**
 * @brief  설정 > 음원 재생 옵션
 */
class SettingPlayOption : public QWidget
{
    Q_OBJECT
public:
    explicit SettingPlayOption(QWidget *parent = nullptr);
    void getSettingInfo();

signals:

private slots:
    void slot_changedIndexPod(int index);

private:
    QComboBox *comboBox_pod;

    void setUIControl();
    QWidget* getUIControlOption(QString p_title);

    QList<int> list_keyValue;

};

#endif // SETTINGPALYOPTION_H
