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

private slots:
    void slot_showHideLicese();
    void slot_showHideReadme();
    void slot_showHideUpdatelist();
    void slot_showHideNewfeatureMoive();

private:
    void setUIControl();
    void readFileLicense();
    void readFileReadme();
    void readFileUpdatelist();

private:
    QPushButton *btn_license, *btn_readme, *btn_updatelist, *btn_newfeature;

    QLabel *lb_licese;
    QLabel *lb_readme;
    QLabel *lb_updatelist;
    QLabel *lb_newfeature;

    QPixmap *pixmap_icon_show;
    QPixmap *pixmap_icon_hide;
    QLabel *lb_icon_arrow;

    QPixmap *pixmap_icon_show_up;
    QPixmap *pixmap_icon_hide_up;
    QLabel *lb_icon_arrow_up;

    QPixmap *pixmap_icon_show_read;
    QPixmap *pixmap_icon_hide_read;
    QLabel *lb_icon_arrow_read;

    QWidget* getUIControlTitleSub(QString p_title, QString p_version);
    QPushButton* getUIControlTitleSubClick(QString p_title);
    QPushButton* getUIControlTitleSubClick_readme(QString p_title);
    QPushButton* getUIControlTitleSubClick_updatelist(QString p_title);
    QPushButton* getUIControlTitleSubClick_newfeature(QString p_title);
};

#endif // SETTINGAPPINFO_H
