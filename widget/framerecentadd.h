#ifndef FRAMERECENTADD_H
#define FRAMERECENTADD_H

#include "framewidget.h"
#include "data/dataalbum.h"
#include "data/datapopup.h"
#include "widget/imagelabel.h"

/**
 * @brief 음악 최근차가된 앨범 프레임 위젯 @n
 *
 * @note @sa FrameWidget 상속
 */
class FrameRecentAdd: public FrameWidget
{
    Q_OBJECT
public:
    explicit FrameRecentAdd(QWidget *parent = nullptr);
    explicit FrameRecentAdd(bool flagIsViewAllSize, QWidget *parent = nullptr);
    ~FrameRecentAdd();

    void setData(const QJsonObject &p_jsonObject) override;
    void setHover() override;

private:

    void setInit() override;
    void setUIControl() override;

    QJsonObject getDataForPopup() override;
    QJsonArray getDataForAddInPlaylist();
    QJsonArray getDataForPlayMusic();
    void setPopupData();

    void paintEvent(QPaintEvent *event) override;

    const int IMG_WIDTH = 200;
    const int IMG_HEIGHT = 200;
    const int IMG_WIDTH_S = 186;
    const int MARGIN = 0;

    const int LABEL_HEIGHT = 18;
    const int SPACE_LABELS = 5;

    DataAlbum *data;
    DataPopup *dataPopup;

    ImageLabel *lb_img;
    QLabel *lb_title;
    QLabel *lb_artist;

    bool flagNeedDraw=true;
    bool flagIsViewAllSize=false;

protected slots:
    void slot_clickedHover(const QString &p_code) override;
    void slot_clickedItem();

};

#endif // FRAMERECENTADD_H
