#ifndef FRAMEMUSICCOMPOSER_H
#define FRAMEMUSICCOMPOSER_H

#include "framewidget.h"
#include "data/datamusiccomposer.h"
#include "data/datapopup.h"
#include "widget/imagelabel.h"
#include <QList>

/**
 * @brief 음악-분류-작곡가 프레임 위젯 @n
 *
 * @note @sa FrameWidget 상속
 */
class FrameMusicComposer : public FrameWidget
{
    Q_OBJECT
public:
    explicit FrameMusicComposer(QWidget *parent = nullptr);

    void setData(const QJsonObject &p_jsonObject) override;
    void setHover() override;

private:

    void setInit() override;
    void setUIControl() override;
    QJsonObject getDataForPopup() override;
    void setPopupData();
    QJsonArray getDataForAddInPlaylist();
    QJsonObject convertDataPopupToJsonObject(DataPopup *p_dataPopup);

    void paintEvent(QPaintEvent *event) override;

    const int IMG_WIDTH = 186;
    const int IMG_HEIGHT = 186;
    const int MARGIN = 10;

    DataMusicComposer *data;
    QList<DataPopup*> *dataPopupList;

    ImageLabel *lb_img;
    QLabel *lb_composer;
    QLabel *lb_numSongs;

    bool flagNeedDraw=true;

protected slots:
    void slot_clickedHover(const QString &p_code) override;
    void slot_clickedItem();
};

#endif // FRAMEMUSICCOMPOSER_H
