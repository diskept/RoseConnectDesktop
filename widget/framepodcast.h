#ifndef FRAMEPODCAST_H
#define FRAMEPODCAST_H

#include "framewidget.h"
#include "data/data_pc_channel.h"
#include "widget/imagelabel.h"
#include <QLabel>

/**
 * @brief 팟캐스트 채널 프레임 위젯 @n
 *
 * @note @sa FrameWidget 상속
 */
class FramePodcast : public FrameWidget
{
    Q_OBJECT
public:
    explicit FramePodcast(QWidget *parent = nullptr);
    ~FramePodcast();

    void setData(const QJsonObject &p_jsonObject) override;
    void setHover() override;

private:
    void setInit() override;
    void setUIControl() override;
    QJsonObject getDataForPopup() override;

    void paintEvent(QPaintEvent *event) override;

    const int IMG_WIDTH = 214;
    const int IMG_HEIGHT = 214;
    const int MARGIN = 10;

    const int LABEL_HEIGHT = 22;
    const int SPACE_LABELS = 5;

    Data_PC_Channel *data;

    ImageLabel *lb_img;
    QLabel *lb_title;
    QLabel *lb_artist;

    bool flagNeedDraw=true;

protected slots:
    void slot_clickedHover(const QString &p_code) override;
    void slot_clickedItem();


};

#endif // FRAMEPODCAST_H
