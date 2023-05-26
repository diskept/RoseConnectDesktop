#ifndef SETTINGMENU_H
#define SETTINGMENU_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

#include <widget/onoffwidget.h>
#include "common/linker.h"
/**
 * @brief The SettingAppMenu class 설정 > 비디오 옵션
 */
class SettingMenu : public QWidget//c230329
{
    Q_OBJECT
public:
    explicit SettingMenu(QWidget *parent = nullptr);
    void requestVideoSetData();

signals:

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_modeOnOff_Tidal();
    void slot_modeOnOff_Bugs();
    void slot_modeOnOff_Qobuz();

private:

    Linker *linker;
    OnOffWidget *onOff_Tidal, *onOff_Bugs, *onOff_Qobuz;
    //QPushButton *btn_onOffHover_Tidal, *btn_onOffHover_Bugs, *btn_onOffHover_Qobuz;
    void setUIControl();
    QWidget *getUIControlOptionTidal(QString p_title);
    QWidget *getUIControlOptionQobuz(QString p_title);
    QWidget *getUIControlOptionBugs(QString p_title);
    void changedIndexLeftmenu(int index, int menu);
    void selectTableLeftmenu();
};

#endif // SETTINGMENU_H
