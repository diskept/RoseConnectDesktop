#include "customlistwidget.h"
#include <QApplication>
#include <QDebug>


/**
 * @brief CustomListWidget::CustomListWidget : QListWidget 상속
 * @note hover 시 마우스 커서 변경을 위함<br>
 * click 이후에도 마우스 커서 유지
 * @param parent
 */
CustomListWidget::CustomListWidget(QWidget *parent)
    : QListWidget(parent)
{

}


void CustomListWidget::enterEvent(QEvent *event){
    Q_UNUSED(event);

    QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
}

void CustomListWidget::leaveEvent(QEvent *){
    QApplication::restoreOverrideCursor();
}
