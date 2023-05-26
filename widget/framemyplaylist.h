#ifndef FRAMEMYPLAYLIST_H
#define FRAMEMYPLAYLIST_H

#include "framewidget.h"
#include "data/dataplaylist.h"
#include "data/datapopup.h"
#include "widget/imagelabel.h"

#include <data/data_m_playlist.h>

/**
 * @brief 음악 마이플레이리스트 프레임 위젯 @n
 *
 * @note @sa FrameWidget 상속
 */
class FrameMyPlayList: public FrameWidget
{
    Q_OBJECT
public:
    explicit FrameMyPlayList(QWidget *parent = nullptr);
    explicit FrameMyPlayList(bool flagIsViewAllSize, QWidget *parent = nullptr);
    ~FrameMyPlayList();

    void setData(const QJsonObject &p_jsonObject) override;
    void setHover() override;
private:

    void setInit() override;
    void setUIControl() override;
    QJsonObject getDataForPopup() override;
    QJsonArray getPlayList(int p_index = -1);
    void setPopupData(QMap<QString, QVariant>);
    void setPopupData(const QJsonObject &p_jsonObject);
    void requestMusic();
    void setResultOfMusicInfo(const QJsonObject &p_jsonObject);
    void setResultOfMusicInfoNew(const QJsonObject &p_jsonObject);
    void getTrackInfoFromDB();

    void paintEvent(QPaintEvent *event) override;

    const int IMG_WIDTH = 200;
    const int IMG_HEIGHT = 200;
    const int IMG_WIDTH_S = 186;
    const int MARGIN = 0;

    const int LABEL_HEIGHT = 22;
    const int SPACE_LABELS = 5;

    Data_M_PlayList *dataPlayList;
    //DataPlayList *data;
    //QList<DataPopup*> *dataPopupList;

    ImageLabel *lb_img;
    QLabel *lb_title;

    bool flagNeedDraw=true;
    bool flagIsViewAllSize=false;
    QStringList listPlayUrl;

protected slots:
    void slot_clickedHover(const QString &p_code) override;
    void slot_clickedItem();
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

};


#endif // FRAMEMYPLAYLIST_H
