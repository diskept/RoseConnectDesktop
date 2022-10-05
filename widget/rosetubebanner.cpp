#include "rosetubebanner.h"
#include <QApplication>

/**
 * @brief RoseTubeBanner::RoseTubeBanner : 로즈튜브-구독-배너 이미지
 * @details 이미지 사이즈 1060/175, width 에 맞춰 heigth 변경함.
 * @param parent
 */
RoseTubeBanner::RoseTubeBanner(QWidget *parent) : QLabel(parent)
{
    QSizePolicy size(sizePolicy());
    size.setHeightForWidth(true);
    setSizePolicy(size);
}

int RoseTubeBanner::heightForWidth(int p_w) const
{
    QLabel::heightForWidth(p_w);
    QApplication::postEvent(const_cast<RoseTubeBanner*>(this), new QEvent(QEvent::UpdateRequest));

    return p_w*bannerHeight/bannerWidth;
}

QSize RoseTubeBanner::sizeHint() const
{
    return QSize(bannerWidth, heightForWidth(bannerWidth));
}


void RoseTubeBanner::resizeEvent(QResizeEvent *event){
    Q_UNUSED(event);
    setFixedHeight(heightForWidth(this->width()));
}
