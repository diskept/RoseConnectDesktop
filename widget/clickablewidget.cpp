#include "clickablewidget.h"

#include <QStyleOption>
#include <QPainter>

ClickableWidget::ClickableWidget(QWidget *parent) : QWidget(parent)
{

}

ClickableWidget::~ClickableWidget(){

}

void ClickableWidget::mousePressEvent(QMouseEvent *event){
    Q_UNUSED(event);        // by sunnyfish
    emit clickedWidget();
}

void ClickableWidget::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
