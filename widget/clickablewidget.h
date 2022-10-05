#ifndef CLIACKABLEWIDGET_H
#define CLIACKABLEWIDGET_H

#include <QWidget>

/**
 * @brief 클릭가능한 QWidget
 */
class ClickableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ClickableWidget(QWidget *parent = nullptr);
    ~ClickableWidget();
signals:
    void clickedWidget();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

#endif // CLIACKABLEWIDGET_H
