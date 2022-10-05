#ifndef DIALOGVIDEOLIST_H
#define DIALOGVIDEOLIST_H

#include "common/linker.h"
#include "widget/flowlayout.h"
#include "widget/OptMorePopup.h"

#include "bugs/bugs_struct.h"

#include "tidal/AbstractItem.h"

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QScrollArea>



class DialogVideoList : public QDialog
{
    Q_OBJECT
public:
    explicit DialogVideoList(QWidget *parent = nullptr);
    ~DialogVideoList();

    void setData(const QJsonObject& jsonObj);

private slots:
    void slot_applyResult_videoInfo(bugs::VideoItemData data_video, const QJsonObject& jsonObj_dataToPlay);

    void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode);
    void slot_btnClicked_playAll();

    // about OptMorePopup
    void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int);


private:
    void setInit();
    void setUIControl();

    int checkValid_index(int length_ofData, int index);

    void proc_clicked_itemVideo(bugs::VideoItemData& data_video, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
    void proc_clicked_itemVideo(QList<bugs::VideoItemData>* list_video, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

    void proc_clicked_optMorePopup_fromVideo(bugs::VideoItemData& data_video, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode);
    void proc_clicked_optMorePopup_fromVideo(QList<bugs::VideoItemData>* list_video, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode);

    void makeObj_optMorePopup(const OptMorePopup::PopupMenuMode menuMode, const OptMorePopup::HeaderData &data_header, const int index=0, const int section=0, const bool flagForceHide_favorite=false);

    void slot_applyResult_getShareLink(const QString &link);//c220902_1

private:
    QString link_str;
    Linker *linker;

    QWidget *dlg_widget;
    QWidget *dlg_body_widget;
    QScrollArea *dlg_scrollArea;

    FlowLayout *dlg_layout;

    QLabel *lb_title;

    // data track
    bugs::TrackItemData data_track;

    // list Video
    QList<bugs::VideoItemData> *list_video;
    QJsonArray jsonArr_videos_toPlay;

};

#endif // DIALOG_VIDEOLIST_WITHTRACK_H
