#ifndef PODCAST_HOME_H
#define PODCAST_HOME_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "widget/myqwidget.h"//c220729
#include "widget/customlineedit.h"//c220727

#include <QCoreApplication>


namespace podcast {

    /**
     * @brief 팟캐스트 홈(인기) @n
     *      팟캐스트 인기 채널 목록으로 구성 (설정의 국가 옵션에 따라 인기채널 다름)
     */
    class Podcast_Home : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit Podcast_Home(QWidget *parent = nullptr);
        ~Podcast_Home();

        void requestData();

    protected slots:
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;

    private:
        void setInit();
        void setUIControl();
        void setResultOfChannelList(const QJsonObject &p_jsonObject);
        void appendChannel(const QJsonObject &jsonData);
        void resizeEvent(QResizeEvent *event) override;

    private:
        // 관리 필요한 Layout UI
        FlowLayout *flowLayout_podcast;

        int podcast_widget_width = 0;
        int podcast_widget_margin = 0;

        bool flag_active_page = false;
    };
};

#endif // PODCAST_HOME_H
