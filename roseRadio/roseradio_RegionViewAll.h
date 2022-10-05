#ifndef ROSERADIO_REGIONVIEWALL_H
#define ROSERADIO_REGIONVIEWALL_H

#include "roseRadio/roseradio_struct.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include <QCoreApplication>


namespace roseRadio {


    class RoseRadioRegionViewAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT

    public:
        explicit RoseRadioRegionViewAll(QWidget * parent = nullptr);
        ~RoseRadioRegionViewAll();

        void setJsonObject_forData(const QJsonObject&) override;
        void setActivePage() override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_Region(const QJsonArray&, const int, const bool);

        void slot_clickedItemRegion();

    private:
        void setUIControl_Region();

        void request_more_RegionData();
        void request_more_RegionDraw();

        QWidget* setRegion_pushButton(const int index, const QString name);

    private:
        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;
        QWidget *widget_region;
        FlowLayout *flowLayout_region;

        QString apiPath = "";
        QString title = "";
        QString listType = "";
        int regionId = 0;

        QJsonArray jsonArr_region;

        QList<roseRadio::RoseRadioRegionInfo> *list_region;

        int next_offset = 0;
        int region_total_cnt = 0;
        int region_draw_cnt = 0;

        bool flagReqMore_region = false;
        bool flag_lastPage_region = false;

        bool flag_flow_draw = false;
        bool flag_region_draw = false;
    };
};
#endif // ROSERADIO_REGIONVIEWALL_H
