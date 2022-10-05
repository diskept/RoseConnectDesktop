#ifndef RADIOUSER_H
#define RADIOUSER_H

#include "widget/flowlayout.h"
#include "data/dataradio.h"
#include <QWidget>
#include <common/linker.h>

/**
 * @brief 라디오 > 사용자채널 @n
 *      사용자가 추가한 라디오 채널 목록으로 구성
 *
 * @note 채널편집 버튼 클릭시 @sa RadioEdit 채널 편집 화면으로 이동
 */
class RadioUser : public QWidget
{
    Q_OBJECT
public:
    explicit RadioUser(QWidget *parent = nullptr);
    ~RadioUser();

    void requestData(bool flagRefresh = false);
    void show_topBtnControl();
    void hide_topBtnControl();

signals:
    void signal_clickedEdit(const QJsonObject &p_jsonObject);

private:

    const int HTTP_CHANNEL_LIST = 99;
    const int HTTP_FAVORITE_LIST = 97;

    void setInit();
    void setUIControl();
    void setTopUIControl(QWidget *parent);
    void clearChannelList();
    void requestFavoriteList();
    void setResultOfChannelList(const QJsonObject &p_jsonObject);
    void setResultOfFavoriteList(const QJsonObject &p_jsonObject);
    void appendRadioChannel(const QJsonObject &p_jsonObject);
    void resizeEvent(QResizeEvent *event) override{
        Q_UNUSED(event);
        widget_topContrl->setGeometry((this->width()-this->widget_topContrl->sizeHint().width()), 0,  this->widget_topContrl->sizeHint().width(), this->widget_topContrl->sizeHint().height());
    };

    Linker *linker;
    QWidget *widget_topContrl;
    FlowLayout *flowLayout;
    QWidget *wg_top;
    QWidget *wg_check;
    QJsonObject jsonData;

    QList<QString> favorUrlList;
    QList<DataRadio*> dataList;



private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedHoverItem(const QString &p_code);
    void slot_clickedModify();
    void slot_clickedRefresh();
    void slot_clickedAddChannel();
    void slot_changedUserChannelList();
    void slot_changedFavorChannel(const bool &flagChangedInFavor);


};

#endif // RADIOUSER_H
