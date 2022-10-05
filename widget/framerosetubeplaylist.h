#ifndef FRAMEROSETUBEPLAYLIST_H
#define FRAMEROSETUBEPLAYLIST_H

#include "widget/framewidget.h"
#include "widget/imagelabel.h"
#include "widget/optionpopup.h"

#include "data/data_rt_playlist.h"
#include "data/data_rt_track.h"
#include "data/datapopup.h"

#include <QLabel>
#include <QList>


/**
 * @brief 로즈튜브 플레이리스트 프레임 위젯 @n
 *
 * @note @sa FrameWidget 상속
 */
class FrameRoseTubePlayList : public FrameWidget
{
    Q_OBJECT
public:
    explicit FrameRoseTubePlayList(OptionPopup::TypeMenu p_typeMenu_optionPopup, bool flagSmallUI = false, QWidget *parent = nullptr);
    ~FrameRoseTubePlayList();

    void setData(const QJsonObject &p_jsonObject) override;
    void setHover() override;

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData);

private:

    OptionPopup::TypeMenu typeMenu_optionPopup;

    void setInit() override;
    void setUIControl() override;
    QJsonObject getDataForPopup() override;
    void setPopupData(const int &p_index);
    void requestTracks();
    void setResultOfTracks(const QJsonObject &p_jsonObject);

    void paintEvent(QPaintEvent *event) override;
    const int IMG_WIDTH = 360;
    const int IMG_HEIGHT = 200;
    const int IMG_WIDTH_S = 284;
    const int IMG_HEIGHT_S = 157;
    const int MARGIN = 10;

    const int LABEL_HEIGHT = 20;
    const int SPACE_LABELS = 5;

    bool flagSmallUI;

    QList<OptionPopup*> list_popup;

    Data_RT_PlayList *data_RT_PlayList;
    QList<DataPopup*> *dataPopupList;
    QList<Data_RT_Track*> *dataTracks;

    ImageLabel *lb_img;
    QLabel *lb_title;
    QLabel *label_owner;
    QLabel *image_thumb;
    QLabel *label_thumb;

    bool flagNeedDraw=true;

protected slots:
    void slot_clickedHover(const QString &p_code) override;
    void slot_clickedItem();

};
#endif // FRAMEROSETUBEPLAYLIST_H
