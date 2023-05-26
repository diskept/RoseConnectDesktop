#ifndef PODCAST_CATEGORY_DETAIL_H
#define PODCAST_CATEGORY_DETAIL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include <QWidget>
#include <QCoreApplication>


namespace podcast {

    /**
     * @brief 팟 캐스트 카테고리 상세 @n
     *      해당 카테고리의 채널 목록으로 구성
     */
    class Podcast_Category_Detail : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit Podcast_Category_Detail(QWidget *parent = nullptr);
        ~Podcast_Category_Detail();

        void requestData(const int &genreID);

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

        int currentGenreId;
    };
};

#endif // PODCAST_CATEGORY_DETAIL_H
