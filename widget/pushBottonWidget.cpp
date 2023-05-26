#include "widget/pushBottonWidget.h"

#include "widget/toastmsg.h"


#include <QPushButton>
#include <QThread>


pushBottonWidget::pushBottonWidget(QPushButton *parent)
: QPushButton(parent)
{




}

pushBottonWidget::~pushBottonWidget()
{

}

/**
 * @brief OnOffWidget::setInit : 초기 세팅
 */


/**
 * @brief OnOffWidget::mousePressEvent : [Override] 마우스 누름 이벤트
 * @details
 */
void pushBottonWidget::mousePressEvent(QMouseEvent*)
{

    print_debug();
    ToastMsg::delay(this,"", tr("delay"), 1000);//c230316
    emit signal_clicked();
}
