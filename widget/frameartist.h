#ifndef FRAMEARTIST_H
#define FRAMEARTIST_H

#include "framewidget.h"
#include "data/datapopup.h"
#include "data/dataartist.h"
#include "widget/imagelabel.h"

/**
 * @brief 아티스트 프레임 위젯 @n
 *
 * @note @sa FrameWidget 상속
 */
class FrameArtist : public FrameWidget
{
    Q_OBJECT
public:
    explicit FrameArtist(QWidget *parent = nullptr);
    ~FrameArtist();

    void setData(const QJsonObject &p_jsonObject) override;
    void setHover() override;
private:

    void setInit() override;
    void setUIControl() override;
    QJsonObject getDataForPopup() override;
    void selectMusicInfo();
    void setPopupData(QMap<QString, QVariant> p_map);
    QJsonArray getDataForAddInPlaylist();
    QJsonObject convertDataPopupToJsonObject(DataPopup *p_dataPopup);

    void paintEvent(QPaintEvent *event) override;

    const int IMG_WIDTH = 180;
    const int IMG_HEIGHT = 180;
    const int MARGIN = 10;

    DataArtist *data;
    QList<DataPopup*> *dataPopupList;

    ImageLabel *lb_img;
    QLabel *lb_title;
    QLabel *lb_album_track;

    bool flagNeedDraw=true;

protected slots:
    void slot_clickedHover(const QString &p_code) override;
    void slot_clickedItem();
};

#endif // FRAMEARTIST_H
