#ifndef SETTINGVIDEO_H
#define SETTINGVIDEO_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

#include <widget/onoffwidget.h>
/**
 * @brief The SettingAppMenu class 설정 > 비디오 옵션
 */
class SettingVideo : public QWidget
{
    Q_OBJECT
public:
    explicit SettingVideo(QWidget *parent = nullptr);
    void requestVideoSetData();

signals:

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_modeOnOff();

private:

    OnOffWidget *onOff;

    void setUIControl();
    QWidget *getUIControlOption(QString p_title);
};

#endif // SETTINGVIDEO_H
