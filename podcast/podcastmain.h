#ifndef PODCASTMAIN_H
#define PODCASTMAIN_H

#include "home/abstractmaincontentforpodcast.h"

#include "podcast/podcast_home.h"
#include "podcast/podcast_category.h"
#include "podcast/podcast_category_detail.h"
#include "podcast/podcast_Subscription.h"
#include "podcast/podcast_Channel.h"
#include "podcast/podcast_search.h"

#include <QWidget>
#include <QMutex>


namespace podcast {

    /**
     * @brief 팟캐스트 메인메뉴의 총괄 @n
     *      팟캐스트 대항목 관련 모든 QWidget 관리
     */
    class PodCastMain : public AbstractMainContentForPodcast
    {
        Q_OBJECT
    public:
        explicit PodCastMain(QWidget *parent = nullptr);
        ~PodCastMain();

    signals:

    protected:
        void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;
        void mousePressEvent(QMouseEvent *event) override;

    private slots:
        void slot_clickedViewAll(const QJsonObject &p_jsonData);
        void slot_search(const QString &p_text);

    private:
        void setInit();
        void setUIControl();

    private :
        // ★ Linker 변수 이미 있으니 추가하지마세요.. protected에 있어요

        // 메뉴 관련 페이지  ----------------------
        // 메뉴 : 팟캐스트 > 인기
        Podcast_Home *podcastHome=nullptr;
        Podcast_Category *podcastCategory=nullptr;
        Podcast_Subscription *podcastSubscribe=nullptr;
        Podcast_Category_Detail *viewAll_category=nullptr;
        Podcast_Channel *podcastChannel=nullptr;
        Podcast_Search *podcastSearch=nullptr;
    };
};
#endif // PODCASTMAIN_H
