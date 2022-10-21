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
    void slot_showHideReadme();
    void slot_showHideUpdatelist();
    void slot_showHideNewfeatureMoive();

private:
    QPushButton *btn_license, *btn_readme, *btn_updatelist, *btn_newfeature;
    QLabel *lb_licese;
    QLabel *lb_readme;
    QLabel *lb_updatelist;
    QLabel *lb_newfeature;
    QPixmap *pixmap_icon_show;
    QPixmap *pixmap_icon_hide;
    QLabel *lb_icon_arrow;

    void setUIControl();
    void readFileLicense();
    void readFileReadme();
    void readFileUpdatelist();
    QWidget* getUIControlTitleSub(QString p_title, QString p_version);
    QPushButton* getUIControlTitleSubClick(QString p_title);
};

#endif // SETTINGAPPINFO_H
