#ifndef STARPOINTWIDGET_H
#define STARPOINTWIDGET_H

#include <QWidget>
#include "clickablelabel.h"
#include <QList>

/**
 * @brief 별점 위젯 클래스
 */
class StarPointWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StarPointWidget(QWidget *parent = nullptr);
    void setStarRate(int p_starRate);
    int getStarRate();

signals:
    void signal_clicked(const int starRate);

private slots:
    void slot_clicked_starBtn();

private :
    void setUIControl();
    QList<ClickableLabel*> *list_star;
    int starRate;

    bool isValid_starRate(int p_starRate);
};

#endif // STARPOINTWIDGET_H
