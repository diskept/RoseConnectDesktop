#ifndef FRAMEMUSICARTIST_H
#define FRAMEMUSICARTIST_H

#include "framewidget.h"
#include "data/datamusicartist.h"
#include "data/datapopup.h"
#include "widget/imagelabel.h"
#include <QList>

/**
 * @brief 음악-븐류-아티스트 위젯 @n
 *
 * @note @sa FrameWidget 상속
 */
class FrameMusicArtist : public FrameWidget
{
    Q_OBJECT
public:
    explicit FrameMusicArtist(QWidget *parent = nullptr);

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

    DataMusicArtist *data;
    QList<DataPopup*> *dataPopupList;

    ImageLabel *lb_img;
    QLabel *lb_artist;
    QLabel *lb_numSongs;

    bool flagNeedDraw=true;

protected slots:
    void slot_clickedHover(const QString &p_code) override;
    void slot_clickedItem();
};

#endif // FRAMEMUSICARTIST_H
