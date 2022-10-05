#ifndef SETTINGDISPLAY_H
#define SETTINGDISPLAY_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
/**
 * @brief  설정 > 디스플레이
 */
class SettingDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit SettingDisplay(QWidget *parent = nullptr);
    void requestGetValueDisplay();

signals:

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void setTextVolumn(int p_value);

private:
    QLabel *lb_volumnNum;
    QSlider *slider_vol;
    void setUIControl();
    QWidget* getUIControlOption();

};

#endif // SETTINGDISPLAY_H
