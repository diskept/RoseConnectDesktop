#ifndef SETTINGAPPMENU_H
#define SETTINGAPPMENU_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
/**
 * @brief 설정 > 앱 메뉴 편집
 */
class SettingAppMenu : public QWidget
{
    Q_OBJECT
public:
    explicit SettingAppMenu(QWidget *parent = nullptr);

signals:

private:
    void setUIControl();

};

#endif // SETTINGAPPMENU_H
