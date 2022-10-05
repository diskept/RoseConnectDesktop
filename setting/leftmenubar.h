#ifndef LEFTMENUBAR_H
#define LEFTMENUBAR_H

#include <QPushButton>
#include <QWidget>

/**
 * @brief 설정의 왼쪽 서브메뉴 @n
 *      마이페이지,친구관리,미디어 라이브러리 등의 메뉴로 구성
 */
class LeftMenuBar : public QWidget
{
    Q_OBJECT
public:
    explicit LeftMenuBar(QWidget *parent = nullptr);
    void setSelectedSubMenu(QString p_subMenuCode);

signals:
    void changedSubMenu(const QJsonObject &p_data);

private slots:
    void slot_clickedMenu();

private:
    /** * @brief curr_subMenuCode 현재 선택된 서브메뉴의 code */
    QString curr_subMenuCode;

    /** * @brief list_subMenuItem 서브메뉴 객체 리스트 */
    QList<QPushButton*> list_subMenuItem;

    void setUIControl();
    QPushButton* createMenuBtn(const QString &text, const QString &code);
};

#endif // LEFTMENUBAR_H
