#ifndef PODCASTCATEGORYDETAIL_H
#define PODCASTCATEGORYDETAIL_H

#include <QWidget>
#include <widget/flowlayout.h>

class PodcastCategoryDetail : public QWidget
{
    Q_OBJECT
public:
    explicit PodcastCategoryDetail(QWidget *parent = nullptr);

    void requestData(const int &genreID);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

private:
    void setInit();
    void setUIControl();
    void setResultOfChannelList(const QJsonObject &p_jsonObject);
    void appendChannel(const QJsonObject &jsonData);

    int currentGenreId;
    FlowLayout *flowLayout;
};

#endif // PODCASTCATEGORYDETAIL_H
