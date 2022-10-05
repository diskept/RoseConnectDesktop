#ifndef CUSTOMLISTWIDGET_H
#define CUSTOMLISTWIDGET_H

#include <QListWidget>

/**
 * @brief 커스텀 QListWidget @n
 *      hover 시 마우스 커서 항상 point Hand 적용
 */
class CustomListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit CustomListWidget(QWidget *parent = nullptr);

protected:
    virtual void enterEvent(QEvent *) override;
    virtual void leaveEvent(QEvent *) override;
};

#endif // CUSTOMLISTWIDGET_H
