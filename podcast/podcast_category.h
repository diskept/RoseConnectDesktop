#ifndef PODCASTCATEGORY_H
#define PODCASTCATEGORY_H

#include <QWidget>
#include "widget/flowlayout.h"
#include "widget/clickablelabel.h"
/**
 * @brief  팟캐스트-카테고리 @n
 *      카테고리 목록 GET hifimp/v2/rest/rose/IF_ROSEPODCAST_001 API 사용
 */
class Podcast_Category : public QWidget
{
    Q_OBJECT
public:
    explicit Podcast_Category(QWidget *parent = nullptr);
    void requestData(bool flagNeedRefresh=false);

signals:
    void signal_reqSubPage(const QJsonObject &p_jsonObject);

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
