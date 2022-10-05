#ifndef FRAMEFRIENDPLAYLIST_H
#define FRAMEFRIENDPLAYLIST_H

#include "framewidget.h"
#include "data/dataplaylist.h"
#include "data/datapopup.h"
#include "widget/imagelabel.h"

#include <data/data_m_playlist.h>

/**
 * @brief 친구 플레이리스트 프레임 위젯 @n
 *
 * @note @sa FrameWidget 상속
 */
class FrameFriendPlayList : public FrameWidget
{
    Q_OBJECT
public:
    explicit FrameFriendPlayList(QWidget *parent = nullptr);
    explicit FrameFriendPlayList(bool flagIsViewAllSize, QWidget *parent = nullptr);
    ~FrameFriendPlayList();

    void setData(const QJsonObject &p_jsonObject) override;
    void setHover() override;
private:

    void setInit() override;
    void setUIControl() override;
    QJsonObject getDataForPopup() override;
    QJsonArray getPlayList(int p_index = -1);
    void setPopupData(const QString &p_title, const QString &p_path);
    void requestMusic();
    void setResultOfMusicInfo(const QJsonObject &p_jsonObject);
    void setResultOfMusicInfoNew(const QJsonObject &p_jsonObject);

    void paintEvent(QPaintEvent *event) override;

    const int IMG_WIDTH = 200;
    const int IMG_HEIGHT = 200;
    const int IMG_WIDTH_S = 186;
    const int MARGIN = 0;

    const int LABEL_HEIGHT = 18;
    const int SPACE_LABELS = 5;


    Data_M_PlayList *dataPlayList;
    //DataPlayList *data;
    //QList<DataPopup*> *dataPopupList;

    ImageLabel *lb_img;
    QLabel *lb_title;
    QLabel *lb_user;
    QStringList playTitleList;
    QStringList playUrlList;

    bool flagNeedDraw=true;
    bool flagIsViewAllSize=false;

protected slots:
    void slot_clickedHover(const QString &p_code) override;
    void slot_clickedItem();
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

};

#endif // FRAMEFRIENDPLAYLIST_H
