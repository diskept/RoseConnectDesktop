#ifndef PODCASTCATEGORY_H
#define PODCASTCATEGORY_H

#include <QWidget>
#include "widget/flowlayout.h"

class PodcastCategory : public QWidget
{
    Q_OBJECT
public:
    explicit PodcastCategory(QWidget *parent = nullptr);
    void requestData();

signals:
    void signal_clickedViewAll(const QJsonObject &p_jsonObject);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedItem();

private:
    void setInit();
    void setUIControl();
    void setResultOfCategory(const QJsonObject &p_jsonData);
    void appendCategoryUI(const int &p_id, const QString &p_name);

    FlowLayout *flowLayout;

};

#endif // PODCASTCATEGORY_H
