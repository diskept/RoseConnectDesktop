#ifndef SETTINGROSETUBE_H
#define SETTINGROSETUBE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
/**
 * @brief The SettingAppMenu class 설정 > 로즈튜브 옵션
 */
class SettingRoseTube : public QWidget
{
    Q_OBJECT
public:
    explicit SettingRoseTube(QWidget *parent = nullptr);

signals:

private:
    void setUIControl();

};

#endif // SETTINGROSETUBE_H
