#include "widget/clickablelabel.h"

#include "widget/toastmsg.h"

#include <QEvent>
#include <QMouseEvent>
#include <common/gscommon.h>


/**
 * @brief ClickableLabel::ClickableLabel : Clickable 라벨
 * @details 라벨 mousepressed 인 경우 clicked() SIGNAL 발생
 * @param parent
 */
ClickableLabel::ClickableLabel(QWidget *parent) : QLabel(parent)
{
    this->setCursor(Qt::PointingHandCursor);
}


ClickableLabel::ClickableLabel(const QString &text, QWidget *parent) : QLabel(parent)
{
    this->setCursor(Qt::PointingHandCursor);
    this->setText(text);
}


/**
 * @brief ClickableLabel::~ClickableLabel
 */
ClickableLabel::~ClickableLabel() {

    this->deleteLater();
}


/**
 * @brief ClickableLabel::setImageResource
 * @param p_url
 */
void ClickableLabel::setImageResource(QString p_url){

    QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(p_url);

    this->setPixmap(*pixmap_icon);                                   //버퍼에 있는 이미지를 QLabel에 출력
    this->resize(pixmap_icon->width(), pixmap_icon->height());       //QLabel의 크기를 이미지 사이즈에 맞추어 조절한다.
}


/**
 * @brief custom signal 발생을 위해서 event 체크함
 * @param event
 * @return
 */
bool ClickableLabel::event(QEvent *event){

    // mouse over & out
    if (event->type() == QEvent::Enter){
        emit this->signal_mouseHovered();
    }
    else if (event->type()==QEvent::Leave) {
        emit this->signal_mouseOut();
    }

    // mouse click MouseButtonRelease
    QMouseEvent *eventMouse = (QMouseEvent*)event;
    if(eventMouse->button() == Qt::LeftButton){

        QRect rect_this = this->rect();
        QPoint point_mouse = eventMouse->pos();
        if(eventMouse->type() == QEvent::MouseButtonPress){
            if(rect_this.contains(point_mouse)){
                this->flagMousePressed = true;
            }
        }
        else if(event->type() == QEvent::MouseButtonRelease){
            if(this->flagMousePressed && rect_this.contains(point_mouse)){
                this->flagMousePressed = false;     // init
                ToastMsg::delay(this,"", tr("delay"), 500);//c230311
                emit this->signal_clicked();               // emit signal
            }
        }
    }

    return QWidget::event(event);
}
