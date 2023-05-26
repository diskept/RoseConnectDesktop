#ifndef TOPMENUSUBITEM_H
#define TOPMENUSUBITEM_H

#include <QPushButton>
#include <QWidget>

/**
 * @brief The TopMenuSubItem class 상단 메뉴바의 서브메뉴 Item
 */
class TopMenuSubItem : public QWidget
{
    Q_OBJECT
public:
    enum UIMode{
        TopMenu,        ///< 상단 서브메뉴용
        AlbumDetail,    ///< 앨범상세 화면용
        FriendManager,  ///< 친구관리 화면용
        FriendSubmenubar,  ///< 친구관리sub 화면용//cheon211018
        ListSubMenuBar
    };
    explicit TopMenuSubItem(QJsonObject p_data, UIMode uiMode=TopMenu, QWidget *parent = nullptr);
    void setSelectedStyle(bool p_flagSelected);

    void setSubMenuText(QString p_text){ this->btn_icon->setText(p_text); };
    QString getSubMenuCode(){ return this->subMenuCode; };
    QString getSubMenuName(){ return this->subMenuName; };//c230321

signals:
    void clickedBtn(QString p_subCode);

private slots:
    void clickedSubMenu();

private:
    UIMode uiMode;
    /** * @brief subMenuCode 서브메뉴의 code */
    QString subMenuCode;
    QString subMenuName;//c230321
    QPushButton *btn_icon;
};

#endif // TOPMENUSUBITEM_H
