#ifndef ROSETUBE_LISTVIEWALL_H
#define ROSETUBE_LISTVIEWALL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "rosetube/ItemTrack_rosetube.h"
#include "rosetube/ItemPlaylist_rosetube.h"

#include <QCoreApplication>


namespace rosetube {

    class RosetubeViewAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RosetubeViewAll(QWidget *parent = nullptr);
        ~RosetubeViewAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    signals:
        void signal_clickedViewAll(const QJsonObject &p_jsonObject);

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
        void slot_applyResult_subCategories(const QJsonObject&);

    private:
        void setUIControl_RoseTube();

        void request_more_rosetubeData();

    private:
        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_rosetube;

        // Data
        rosetube::ItemTrack_rosetube *viewAll_rosetube_track[999999];
        rosetube::ItemPlaylist_rosetube *viewAll_rosetube_playlist[999999];

        QJsonArray jsonArr_rosetubeTrack;
        QJsonArray jsonArr_rosetubePlaylist;

        QString title = "";
        QString listtype = "";
        int subCategoryId = 0;

        int next_offset = 0;
        int rosetube_total_cnt = 0;
        int rosetube_draw_cnt = 0;

        bool flagReqMore_rosetube = false;
        bool flag_lastPage_rosetube = false;

        bool flag_flow_draw = false;
        bool flag_rosetube_draw = false;
    };
};
#endif // ROSETUBE_LISTVIEWALL_H
