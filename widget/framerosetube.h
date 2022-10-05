#ifndef FRAMEROSETUBE_H
#define FRAMEROSETUBE_H

#include "widget/framewidget.h"
#include "widget/imagelabel.h"
#include "widget/optionpopup.h"

#include "data/data_rt_track.h"
#include "data/datapopup.h"

#include <QLabel>
#include <QList>


/**
 * @brief 로즈튜브 영상 프레임 위젯 @n
 *
 * @note @sa FrameWidget 상속
 */
class FrameRoseTube : public FrameWidget
{
    Q_OBJECT
public:
    explicit FrameRoseTube(OptionPopup::TypeMenu p_typeMenu_optionPopup, bool flagSmallUI = false, QWidget *parent = nullptr);
    ~FrameRoseTube();

    void setDataFromCrawling(const QJsonObject &p_jsonObject);
    void setRequestTrackInfoAfterSetData();
    void setData(const QJsonObject &p_jsonObject) override;
    QJsonObject getData();
    void setHover() override;
    void setHover2();

private:
    OptionPopup::TypeMenu typeMenu_optionPopup;
    void setInit() override;
    void setUIControl() override;
    QJsonObject getDataForPopup() override;
    void setPopupData();
    void requestTrackInfo();
    void requestChangeTrackFavorites();
    void setResultOfGetTrackInfo(const QJsonObject &p_jsonObject);
    void setResultOfChangeTrackFavorites(const QJsonObject &p_jsonObject);

    void paintEvent(QPaintEvent *event) override;
    const int IMG_WIDTH = 360;      //340;
    const int IMG_HEIGHT = 200;     //188;
    const int IMG_WIDTH_S = 284;    //252;
    const int IMG_HEIGHT_S = 157;   //140;
    const int MARGIN = 10;

    const int LABEL_HEIGHT = 20;
    const int SPACE_LABELS = 5;

    Data_RT_Track *data_RT_Track;
    DataPopup *dataPopup;

    QJsonObject dataJsonObj;

    bool flagCrawlingData=false;
    bool flagSmallUI;

    ImageLabel *lb_img;
    QLabel *lb_title;
    QLabel *lb_artist;
    QLabel *lb_num;

    bool flagNeedTrackData = false;
    bool flagNeedDraw = true;

protected slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedHover(const QString &p_code) override;
    void slot_clickedItem();

};
#endif // FRAMEROSETUBE_H
