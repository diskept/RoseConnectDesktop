#ifndef SETTINGROSEMENU_H
#define SETTINGROSEMENU_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
/**
 * @brief The SettingAppMenu class 설정 > 기기메뉴 편집
 */
class SettingRoseMenu : public QWidget
{
    Q_OBJECT
public:
    explicit SettingRoseMenu(QWidget *parent = nullptr);

signals:

private:
    void setUIControl();

};

#endif // SETTINGROSEMENU_H
