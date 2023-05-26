#ifndef ONOFFWIDGET_H
#define ONOFFWIDGET_H

#include <QWidget>
#include <QLinearGradient>
#include <QLabel>
#include <QPushButton>
#include "common/global.h"

/**
 * @brief 아이폰 스타일의 on/off 위젯
 */
class pushBottonWidget : public QPushButton
{
    Q_OBJECT


public:
    explicit pushBottonWidget(QPushButton *parent = nullptr);
    ~pushBottonWidget();

    void mousePressEvent(QMouseEvent *event) override;


signals:
    void signal_clicked();

private:

};


#endif // ONOFFWIDGET_H
