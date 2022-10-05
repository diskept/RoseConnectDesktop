#ifndef FRAMERECENTALBUM_H
#define FRAMERECENTALBUM_H

#include "widget/framewidget.h"
#include "widget/imagelabel.h"

#include "data/dataalbum.h"
#include "data/datapopup.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"


/**
 * @brief 최근재생된앨범 프레임 위젯 @n
 *
 * @note @sa FrameWidget 상속
 */
class FrameRecentAlbum : public FrameWidget
{
    Q_OBJECT
public:
    explicit FrameRecentAlbum(QWidget *parent = nullptr);
    explicit FrameRecentAlbum(bool flagIsViewAllSize, QWidget *parent = nullptr);
    ~FrameRecentAlbum();

    void setData(const QJsonObject &p_jsonObject) override;
    void setHover() override;

private:

    void setInit() override;
    void setUIControl() override;
    QJsonObject getDataForPopup() override;
    QJsonArray getDataForAddInPlaylist();
    QJsonArray getDataForPlayMusic();
    void setAlbumArt();


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

    QJsonObject recv_albumData;
    QJsonArray jsonArr_Play;

    int pageCNT = 0;
    int track_count = 0;
    int album_id = 0;

protected slots:
    void slot_clickedHover(const QString &p_code) override;
    void slot_clickedItem();

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData);

};

#endif // FRAMERECENTALBUM_H
