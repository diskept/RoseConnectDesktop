#ifndef RADIOEDIT_H
#define RADIOEDIT_H

#include "widget/flowlayout.h"
#include "data/dataradio.h"
#include <QWidget>
#include <QJsonObject>
#include <common/linker.h>
#include <widget/imageradiocheckable.h>

/**
 * @brief 라디오 > 사용자 채널 > 편집 @n
 *      사용자 채널 편집화면으로 현재 삭제만 가능
 *
 * @note 라디오 사용자 채널명, 순서 변경 미구현 ==> API 미지원
 */
class RadioEdit : public QWidget
{
    Q_OBJECT
public:
    explicit RadioEdit(QWidget *parent = nullptr);
    ~RadioEdit();

    void requestData(bool flagRefresh = false);


private slots:
    void slot_checkedChanged(int);
    void slot_clickedDel();
    void slot_clickedHoverItem(QString);
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

private:

    const int HTTP_CHANNEL_LIST = 99;
    const int HTTL_DEL_CHANNEL = 98;

    void setInit();
    void setUIControl();
    void requestDelChannel(int p_index);
    void showRemoveConfirmDlg(int p_index = -1);
    void appendRadioChannel(const QJsonObject &p_jsonObject);
    void setResultOfDelChannel(const QJsonObject &p_jsonObject);
    void setResultFoChannelList(const QJsonObject &p_jsonObject);

    Linker *linker;
    QWidget *widget_topContrl;
    FlowLayout *flowLayout;
    QWidget *wg_top;
    QCheckBox *checkBox;

    QList<ImageRadioCheckable*> *listItem;
    QList<DataRadio*> dataList;
    QList<QString> *listPath;
    QList<QString> *listCode;


private slots:

};
#endif // RADIOEDIT_H
