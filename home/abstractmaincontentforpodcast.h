#ifndef ABSTRACTMAINCONTENTFORPODCAST_H
#define ABSTRACTMAINCONTENTFORPODCAST_H

#include "home/abstractmaincontent.h"

/**
 * @brief 팟캐스트 대항목 클래스@n
 *      @sa AbstractMainContent 상속
 * @note 팟캐스트 채널상세의 배경(blur) 추가
 */
class AbstractMainContentForPodcast : public AbstractMainContent
{
    Q_OBJECT
public:
    explicit AbstractMainContentForPodcast(QWidget *parent = nullptr);

protected:
    void showPodcastChannelBG();
    void hidePodcastChannelBG();

private:
     QLabel *lb_podcastBG;
     QLabel *lb_bgOpacity;

     void setInit();
     void setUIControl();
     void resizeEvent(QResizeEvent *event) override;


private slots:
     void slot_showBG(const QPixmap &pixmap);
};

#endif // ABSTRACTMAINCONTENTFORPODCAST_H
