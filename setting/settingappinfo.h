#ifndef SETTINGAPPINFO_H
#define SETTINGAPPINFO_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

/**
 * @brief  설정 > 앱 정보
 */
class SettingAppInfo : public QWidget
{
    Q_OBJECT
public:
    explicit SettingAppInfo(QWidget *parent = nullptr);

signals:
private slots:
    void slot_showHideLicese();

private:
    QPushButton *btn_license;
    QLabel *lb_licese;
    QPixmap *pixmap_icon_show;
    QPixmap *pixmap_icon_hide;
    QLabel *lb_icon_arrow;

    void setUIControl();
    void readFileLicense();
    QWidget* getUIControlTitleSub(QString p_title, QString p_version);
    QWidget* getUIControlTitleSubClick(QString p_title);
};

#endif // SETTINGAPPINFO_H
