#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include <QLineEdit>

#include <common/linker.h>

/**
 * @brief 커스텀 QLineEdit @n
 *      QLineEdit 한글 문제 때문에 커스텀 @n
 *      QLineEdit 에서는 문자열 변화 sigal&slot으로 작성중인 마지막 글자를 확인할 수 없다. @n
 *      기본 signal 대신 @sa CustomLineEdit::signal_textChanged() 사용
 */
class CustomLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    CustomLineEdit(QWidget *parent = nullptr);
    QString getTextAddedPreedistStr();

signals:
    void signal_textChanged(const QString &p_text);
    void signal_clickedEnter(const QString &p_text);
    void signal_clickedDown(const QString &p_text);//c220904_1
    void signal_clickedUp(const QString &p_text);//c220904_1
    void signal_dragDrop_search();//c220726
    void signal_dragDrop_clear();//c220726
    void signal_dragDrop_search(bool);
    void signal_dropEvent_hide_show(bool);//c220728
    void signal_dragLeaveEvent_hide_show(bool);




private:

    Linker *linker;


    void dragLeaveEvent(QDragLeaveEvent *event)override;
    //void dragMoveEvent(QDragMoveEvent *event)override;
    void dragEnterEvent(QDragEnterEvent* event)override;
    void mouseReleaseEvent(QMouseEvent *)override;//c220726
    void dropEvent(QDropEvent *event) override;//c220726
    void inputMethodEvent(QInputMethodEvent *) override;
    void keyPressEvent(QKeyEvent *) override;//c220904_1
   // void filterEvent(const QEvent *event);//c220826_1
    void focusInEvent(QFocusEvent *) override;
    void focusOutEvent(QFocusEvent *) override;
    void contextMenuEvent(QContextMenuEvent *event) override;//cheon210728-test

    QString preeditStr;
};

#endif // CUSTOMLINEEDIT_H
