#ifndef FRAMEVIDEO_H
#define FRAMEVIDEO_H

#include "framewidget.h"
#include "data/datavideoalbum.h"
#include "data/datapopup.h"
#include "widget/imagelabel.h"

#include <QJsonArray>
#include <QList>

/**
 * @brief 비디오 프레임 위젯 @n
 *
 * @note @sa FrameWidget 상속
 */
class FrameVideo : public FrameWidget
{
    Q_OBJECT
public:
    explicit FrameVideo(QWidget *parent = nullptr);
    ~FrameVideo();

    void setData(const QJsonObject &p_jsonObject) override;
    void setHover() override;

private:

    void setInit() override;
    void setUIControl() override;
    QJsonObject getDataForPopup() override;
    void setPopupData(const QJsonObject &p_jsonData);
    void requestVideo();
    void requestVideoThumbnail();
    void setResultOfVideoInfo(const QJsonObject &p_jsonData);
    void setResultofVideoThumbnail(const QJsonObject &p_jsonData);

    const int IMG_WIDTH = 200;      //252;
    const int IMG_HEIGHT = 281;     //140;
    const int MARGIN = 10;

    const int LABEL_HEIGHT = 18;
    const int SPACE_LABELS = 5;

    DataVideoAlbum *data;
    QList<DataPopup*> *dataPopupList;
    QJsonArray jsonArrVideos;

    ImageLabel *lb_img;
    QLabel *lb_title;

protected slots:
    void slot_clickedHover(const QString &p_code) override;
    void slot_clickedItem();
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

};

#endif // FRAMEVIDEO_H
