#ifndef FRAMEMUSICALBUM_H
#define FRAMEMUSICALBUM_H

#include "framewidget.h"
#include "data/dataalbum.h"
#include "data/datapopup.h"
#include "widget/imagelabel.h"
#include <QList>

/**
 * @brief 음악-분류-앨범 프레임 위젯 @n
 *
 * @note @sa FrameWidget 상속
 */
class FrameMusicAlbum : public FrameWidget
{
    Q_OBJECT
public:
    explicit FrameMusicAlbum(QWidget *parent = nullptr);
    ~FrameMusicAlbum();

    void setData(const QJsonObject &p_jsonObject) override;
    void setHover() override;
    void setBackground_label_base();//c220415
    void setBackground_label_end();//c220415

private slots:
    void slot_clickedItem();

private:

    void setInit() override;
    void setUIControl() override;
    QJsonObject getDataForPopup() override;
    QJsonArray getDataForAddInPlaylist();
    QJsonArray getDataForPlayMusic();

    void paintEvent(QPaintEvent *event) override;

    const int IMG_WIDTH = 186;
    const int IMG_HEIGHT = 186;
    const int MARGIN = 10;

    const int LABEL_HEIGHT = 24;
    const int SPACE_LABELS = 3;

    DataAlbum *data;

    ImageLabel *lb_img;
    QLabel *lb_title;
    QLabel *lb_artist;
    QWidget *label_artist_song;
    QLabel *lb_numSongs;

    bool flagNeedDraw=true;
    QWidget *label_base2;
    QVBoxLayout *vl_box3Layout;//c220415

protected slots:
    void slot_clickedHover(const QString &p_code) override;
};

#endif // FRAMEMUSICALBUM_H
