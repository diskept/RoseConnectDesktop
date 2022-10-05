#include "StarPointWidget.h"

#include <QHBoxLayout>
#include <QVariant>

#include <common/gscommon.h>

const int STAR_RATE_MAX = 5;            ///< 별점 개수 Max
const int MENU_HEIGHT = 40;


StarPointWidget::StarPointWidget(QWidget *parent) : QWidget(parent){

    // data
    this->list_star = new QList<ClickableLabel*>();

    // UI
    this->setUIControl();
}



/**
 * @brief 기본 UI 구성
 */
void StarPointWidget::setUIControl(){

    // layout
    QHBoxLayout *hl_star = new QHBoxLayout();
    hl_star->setSpacing(20);
    hl_star->setContentsMargins(0,1,0,1);
    hl_star->setAlignment(Qt::AlignCenter);

    for(int i=0; i<STAR_RATE_MAX; i++){
        ClickableLabel *tmp = new ClickableLabel();
        tmp->setCursor(Qt::PointingHandCursor);
        tmp->setProperty("idx", i);
        hl_star->addWidget(tmp);
        connect(tmp, &ClickableLabel::signal_clicked, this,  &StarPointWidget::slot_clicked_starBtn);

        this->list_star->append(tmp);
    }

    this->setObjectName("widget_star");
    this->setFixedHeight(MENU_HEIGHT);
    this->setStyleSheet("#widget_star { border-bottom:1px solid #4D4D4D; border-radius:0px; }");
    this->setLayout(hl_star);

}


/**
 * @brief [Slot] 별점 클릭에 대한 이벤트 처리
 */
void StarPointWidget::slot_clicked_starBtn(){
    emit this->signal_clicked(sender()->property("idx").toInt()+1);
}


/**
 * @brief 별점 세팅
 * @param p_starRate
 */
void StarPointWidget::setStarRate(int p_starRate){
    // valid check - out of range
    if(this->isValid_starRate(p_starRate)){
        p_starRate = 0;
    }
    this->starRate = p_starRate;

    for(int i=0; i<p_starRate; i++){
        QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/pop_star_icon_on.png");
        this->list_star->at(i)->setPixmap(*pixmap_icon);
    }
    for(int i=p_starRate; i<STAR_RATE_MAX; i++){
        QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/pop_star_icon_off.png");
        this->list_star->at(i)->setPixmap(*pixmap_icon);
    }
}


/**
 * @brief 별점반환
 * @return
 */
int StarPointWidget::getStarRate(){
    return this->starRate;
}


/**
 * @brief 별점 valid 체크
 * @param p_starRate
 * @return
 */
bool StarPointWidget::isValid_starRate(int p_starRate){
    if(p_starRate<0 && p_starRate>STAR_RATE_MAX){
        return false;
    }

    return true;
}
