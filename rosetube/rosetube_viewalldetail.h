#ifndef ROSETUBE_VIEWALLDETAIL_H
#define ROSETUBE_VIEWALLDETAIL_H

#include "common/linker.h"
#include <QWidget>
#include <QVBoxLayout>
#include "widget/framerosetube.h"
#include "QEvent"

/**
 * @brief 로즈튜브 > 탐색 > 장르별, 상황별 상세 @n
 *      장르별/상황별 Sub 페이지, 주제별로 영상 가로배치 @n
 *      주제별 영상은 최소 갯수만 Show, 각 주제 View All 클릭시 전체 Show
 */
class RoseTube_ViewAllDetail : public QWidget
{
    Q_OBJECT
public:
    explicit RoseTube_ViewAllDetail(QWidget *parent = nullptr);
    void setData(const QJsonObject &p_jsonObject);

signals:
    void signal_clickedViewAll(const QJsonObject &p_jsonObject);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedViewAll();

private:
    void setInit();
    void setUIControl();
    void appendPlayList(const QJsonObject &p_jsonObject);
    void appendPlayListItem(const QString &p_title, const QJsonArray &p_jsonArray);

    void requestData();
    void setResultOfCategoryData(const QJsonObject &p_jsonObject);

    Linker *linker;
    int typeId;
    QString typeName;
    QString typeSubName;

    bool flagReqMore=false;
    QVBoxLayout *box_contents;
    QWidget *wg_contents;

    QList<QJsonObject> *listJson;

    void wheelEvent(QWheelEvent *event) override;

};

#endif // ROSETUBE_VIEWALLDETAIL_H
