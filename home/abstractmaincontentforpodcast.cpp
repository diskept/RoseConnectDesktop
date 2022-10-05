#include "abstractmaincontentforpodcast.h"

#include <QGraphicsDropShadowEffect>

const int HEIGHT_BG = 480;

AbstractMainContentForPodcast::AbstractMainContentForPodcast(QWidget *parent)
    : AbstractMainContent(parent)
{

    setInit();
    setUIControl();
}

/**
 * @brief AbstractMainContentForPodcast::setInit : 초기 세팅
 */
void AbstractMainContentForPodcast::setInit(){
    connect(linker, SIGNAL(signal_showPodcastBG(QPixmap)), this, SLOT(slot_showBG(QPixmap)));
}

/**
 * @brief AbstractMainContentForPodcast::setUIControl : UI 세팅
 */
void AbstractMainContentForPodcast::setUIControl(){

    QGraphicsBlurEffect *effect = new QGraphicsBlurEffect();
    effect->setBlurRadius(50);

    lb_podcastBG = new QLabel(wg_total);
    lb_podcastBG->setGraphicsEffect(effect);
    lb_podcastBG->setFixedHeight(HEIGHT_BG);
    lb_podcastBG->lower();
    lb_podcastBG->hide();

    lb_bgOpacity = new QLabel(lb_podcastBG);
    lb_bgOpacity->setFixedHeight(HEIGHT_BG);
    lb_bgOpacity->setStyleSheet("background-color: rgba(0,0,0,180);");
}

/**
 * @brief AbstractMainContentForPodcast::showBG : 팟캐스트 채널 BG Show
 */
void AbstractMainContentForPodcast::showPodcastChannelBG(){
    lb_podcastBG->show();
    lb_bgOpacity->show();
    widget_roseTubeBG->setStyleSheet("#widget_roseTubeBG {background-color:transparent;}");
    topMenuBar->setBGTransparent(true);
}

/**
 * @brief AbstractMainContentForPodcast::hideBG : 팟캐스트 채널 BG Hide
 */
void AbstractMainContentForPodcast::hidePodcastChannelBG(){
    lb_podcastBG->hide();
    lb_bgOpacity->hide();
    widget_roseTubeBG->setStyleSheet("#widget_roseTubeBG {background-color:#171717;}");
    topMenuBar->setBGTransparent(false);
}

void AbstractMainContentForPodcast::resizeEvent(QResizeEvent *event){
    AbstractMainContent::resizeEvent(event);

    lb_podcastBG->resize(this->width(),HEIGHT_BG);
    lb_bgOpacity->resize(this->width(),HEIGHT_BG);
}

/**
 * @brief AbstractMainContentForPodcast::slot_showBG : [슬롯] 팟캐스트 BG SHow 요청
 * @param pixmap QPixmap
 */
void AbstractMainContentForPodcast::slot_showBG(const QPixmap &pixmap){

    if(pixmap.isNull()==false){
        lb_podcastBG->setPixmap(pixmap.scaled(this->width(), 1000, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }

    showPodcastChannelBG();
}
