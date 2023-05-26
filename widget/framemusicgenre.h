#ifndef FRAMEMUSICGENRE_H
#define FRAMEMUSICGENRE_H

#include "framewidget.h"
#include "data/datamusicgenre.h"
#include "data/datapopup.h"
#include "widget/imagelabel.h"
#include <QList>

/**
 * @brief 음악-분류-장르 프레임 위젯 @n
 *
 * @note @sa FrameWidget 상속
 */
class FrameMusicGenre : public FrameWidget
{
    Q_OBJECT
public:
    explicit FrameMusicGenre(QWidget *parent = nullptr);

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
    const int IMG_WIDTH = 200;
    const int IMG_HEIGHT = 200;
    const int MARGIN = 17;

    const int LABEL_HEIGHT = 24;
    const int SPACE_LABELS = 3;

    DataMusicGenre *data;
    QList<DataPopup*> *dataPopupList;

    ImageLabel *lb_img;
    QLabel *lb_genre;
    QLabel *lb_numSongs;

    bool flagNeedDraw=true;

protected slots:
    void slot_clickedHover(const QString &p_code) override;
    void slot_clickedItem();
};

#endif // FRAMEMUSICGENRE_H
