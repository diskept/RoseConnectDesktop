#ifndef MENUITEM_H
#define MENUITEM_H

#include <QLabel>
#include <QWidget>
#include <QHBoxLayout>

/**
 * @brief The MenuItem class 왼쪽 섹션의 메인메뉴 Item
 */
class MenuItem : public QWidget
{
    Q_OBJECT
public:
    explicit MenuItem(const QString p_menuName,const QString p_menuCode, const QString p_iconPath, QWidget *parent = nullptr);

    void setSelectedMenu();
    void setUnSelectedMenu();
    void hideMenuName();
    void showMenuName();

    QString getMenuCode(){ return this->menuCode; };
    QString getMenuName(){ return this->menuName; };

protected:
    void mousePressEvent(QMouseEvent *event);

signals:
    //void clicked(MenuItem *menuItem);
    void clicked(QString p_menuCode);

private:
    /**     * @brief flagSelected 현재 선택된 상태인지 유무      */
    bool flagSelected = false;
    QString menuCode;
    QString menuName;
    QLabel *lb_icon;
    QLabel *lb_text;
    QPixmap *pixmap_buffer;
    QPixmap *pixmap_buffer_selected;
    QHBoxLayout *hl_menu;
    void clearLayout(QLayout *layout);
};

#endif // MENUITEM_H
