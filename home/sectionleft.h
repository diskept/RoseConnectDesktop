#ifndef SECTIONLEFT_H
#define SECTIONLEFT_H

#include "home/menuitem.h"
#include "home/profileplaylistwidget.h"

#include "common/linker.h"

#include <QPushButton>
#include <QWidget>
#include <QMutex>


/**
 * @brief The SectionLeft class 왼쪽 메인메뉴 섹션 (플레이리스트 목록 포함)
 */
class SectionLeft : public QWidget
{
    Q_OBJECT
public:
    explicit SectionLeft(QWidget *parent = nullptr);
    ~SectionLeft();

    void setSmallSize();
    void setBigSize();
    void requestPlayList();


signals:
    void changedMenu(QString menuCode);
    void signal_goPagePlayListDetail(const QJsonObject &p_jsonObject);

public slots:
    void clickedMenu(QString p_menuCode);
    void slot_setSelectMenu(QString p_menuCode);//cheon211008

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedProfileRow();

private:
    void setUIControl();
    void appendMenuItem(const QString p_menuName, const QString p_menuCode, const QString p_iconPath);

private:
    Linker *linker;//cheon211008

    QList<MenuItem*> list_menuItem;     ///< list_subMenuItem 메인메뉴 객체 리스트
    QList<ProfilePlayListWidget*> list_profile_my;
    QList<ProfilePlayListWidget*> list_profile_friend;

    QVBoxLayout *vl_mainMenu;
    QVBoxLayout *vl_logo;
    QVBoxLayout *vl_playList_my;
    QVBoxLayout *vl_playList_friend;

    QLabel *lb_title_playList_my;
    QLabel *lb_title_playList_friend;

    QWidget *widget_bar_my;
    QWidget *widget_bar_friend;

    QLabel *lb_logo;
};

#endif // SECTIONLEFT_H
