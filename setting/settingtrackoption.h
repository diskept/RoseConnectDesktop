#ifndef SETTINGTRACKOPTION_H
#define SETTINGTRACKOPTION_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include "common/linker.h"
/**
 * @brief The SettingAppMenu class 설정 > 곡 옵션
 */
class SettingTrackOption : public QWidget
{
    Q_OBJECT
public:
    explicit SettingTrackOption(QWidget *parent = nullptr);

    void getSettingInfo();

signals:

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_changedIndexPod(int index);
    void slot_getSettingInfo();

private:
    Linker *linker;
    QTimer *timer;
    QComboBox *comboBox_pod;

    void setUIControl();
    QWidget* getUIControlOption(QString p_title);

    QList<int> list_keyValue;

};

#endif // SETTINGTRACKOPTION_H
