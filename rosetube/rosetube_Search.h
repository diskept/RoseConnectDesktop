#ifndef ROSETUBE_SEARCH_H
#define ROSETUBE_SEARCH_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "rosetube/ItemTrack_rosetube.h"

#include "common/common_struct.h"

#include "widget/myqwidget.h"//c220727
#include "widget/customlineedit.h"//c220727

#include <QCoreApplication>


namespace rosetube {

    /**
     * @brief 濡쒖쫰쒕툕 寃됯껐怨붾㈃ @n
     *      濡쒖쫰쒕툕 寃됯껐怨紐⑸줉쇰줈 援ъ꽦 @n
     *      newpipe_api/search API ъ슜
     */
    class RoseTube_Search : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseTube_Search(QWidget *parent = nullptr);
        ~RoseTube_Search();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    signals:
        void signal_clickedViewAll(const QJsonObject &p_jsonObject);
        void signal_changedMenuName(const QString &p_menuName, const QString &menu);

    protected slots:

        void slot_responseHttp(const int&, const QJsonObject&) override;

        // about Item
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_getShareLink(const QString &link);//c220818 share link
        void slot_searchData(const QJsonArray&);

    private:
        void setUIControl_basic();

        void setUIControl_requestGetSearchTrack();

        int get_rose_playType(OptMorePopup::ClickMode clickMode);
        QJsonArray get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode);
        QJsonArray subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex);
        QJsonArray reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex);

    private:
        common::SearchData data_search;

        rosetube::ItemTrack_rosetube *search_track[99999];

        // UI
        QLabel *label_mainTitle;

        QVBoxLayout *vbox_contents;
        QWidget *widget_contents;

        FlowLayout *flowLayout_search_track;
        QWidget *widget_search_content;

        QString searchNextId;
        QString searchNextUrl;

        QJsonArray jsonArr_tracks_toPlay;
        QJsonArray jsonArr_tracks_toTemp;

        bool flagReqMore_search = false;
    };
};

#endif // ROSETUBE_SEARCH_H
