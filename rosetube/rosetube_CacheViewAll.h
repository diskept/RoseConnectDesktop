#ifndef ROSETUBE_CACHEVIEWALL_H
#define ROSETUBE_CACHEVIEWALL_H

#include "rosetube/ItemTrack_rosetube.h"
#include "rosetube/ItemPlaylist_rosetube.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include <QCoreApplication>


namespace rosetube {

    class RoseTubeCacheViewAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT

    public:
        explicit RoseTubeCacheViewAll(QWidget * parent = nullptr);
        ~RoseTubeCacheViewAll();

        void setJsonObject_forData(const QJsonObject&) override;
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
        void slot_hide_msg();

    private:
        void setUIControl_RoseTube();
        void setUIControl_appendWidget_cache();

        void request_more_rosetubeData();

        int get_rose_playType(OptMorePopup::ClickMode clickMode);

        QJsonArray get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode);
        QJsonArray subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex);
        QJsonArray reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex);

    private:
        // 관리 필요한 Layout UI
        QVBoxLayout *box_rt_contents;
        QWidget *widget_rt_contents;

        QWidget *widget_rosetube;
        FlowLayout *flowLayout_rosetube;

        QString page = "";

        // Data
        rosetube::ItemTrack_rosetube *viewAll_rosetube_track[999999];

        QJsonArray jsonArr_rosetubeTrack;
        QJsonObject jsonObj_CacheLock;

        int cache_lock_index = 0;
        bool cache_lock_state = false;
        bool flag_cache_lock_send = false;

        bool flag_cache_delete_send = false;

        int next_offset = 0;
        int rosetube_total_cnt = 0;
        int rosetube_draw_cnt = 0;

        bool flagReqMore_rosetube = false;
        bool flag_lastPage_rosetube = false;

        bool flag_flow_draw = false;
        bool flag_rosetube_draw = false;
    };
};
#endif // ROSETUBE_CACHEVIEWALL_H
