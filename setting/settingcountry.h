#ifndef SETTINGCOUNTRY_H
#define SETTINGCOUNTRY_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
/**
 * @brief  설정 > 국가 채널 옵션
 */
class SettingCountry : public QWidget
{
    Q_OBJECT
public:
    explicit SettingCountry(QWidget *parent = nullptr);
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

#endif // SETTINGCOUNTRY_H
