#ifndef TOASTMSG_H
#define TOASTMSG_H

#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QWidget>
#include <qlabel.h>

/**
 * @brief The MsgBox class 안드로이드 Toast 와 같은 메시지 알림창
 */
class ToastMsg : public QWidget
{
    Q_OBJECT
public:
    explicit ToastMsg(QWidget *parent = nullptr);
    /**
     * @brief show
     * @param parent
     * @param p_title test
     * @param p_msgText testsetes
     */
    static void show(QWidget *parent, const QString &p_title, const QString &p_msgText);
    static void show(QWidget *parent, const QString &p_title, const QString &p_msgText, int duration);
    static void show(QWidget *parent, const QString &p_title, const QString &p_msgText, int duration, int line2);
    static void show(QWidget *parent, const QString &p_title, const QString &p_msgText, int duration, int line2, int );//c220930_2

private slots:
    //void slot_message_hide();
};

#endif // TOASTMSG_H
