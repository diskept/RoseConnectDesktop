#ifndef VERTICALSCROLLAREA_H
#define VERTICALSCROLLAREA_H

#include <QScrollArea>
#include <QDebug>

/**
 * @brief Custom QScrollArea @n
 *      resize 이벤트 시 마다 widget의 가로 길이를 최대로 지정
 */
class VerticalScrollArea : public QScrollArea {

    void resizeEvent(QResizeEvent *event){
        QScrollArea::resizeEvent(event);
        widget()->setFixedWidth(width());
    }

};

#endif // VERTICALSCROLLAREA_H
