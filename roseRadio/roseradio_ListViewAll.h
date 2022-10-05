#ifndef ROSERADIO_LISTVIEWALL_H
#define ROSERADIO_LISTVIEWALL_H

#include "roseRadio/ItemRectangle.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include <QCoreApplication>


namespace roseRadio {

    class RoseRadioListViewAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT

    public:
        explicit RoseRadioListViewAll(QWidget * parent = nullptr);
        ~RoseRadioListViewAll();

        void setJsonObject_forData(const QJsonObject&) override;
        void setActivePage() override;

    protected slots:
        void slot_responseHttp(const int&, const QJsonObject&) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_Channel(const QJsonArray&, const int, const bool);

        void slot_applyResult_Favorite_state(const QJsonObject&);

        void slot_clickedItemRoseRadio(const int, const int, const roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio);

    private:
        void setUIControl_RoseRadio();

        void request_more_roseData();
        void request_more_roseDraw();

    private:
        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;
        QWidget *widget_roseRadio;
        FlowLayout *flowLayout_roseRadio;

        QString apiPath = "";
        QString title = "";
        QString listType = "";
        int regionId = 0;

        roseRadio::ItemRectangle *viewAll_roseRadio[999999];

        QJsonArray jsonArr_roseRadio;

        int channel_widget_cnt = 0;

        int next_offset = 0;
        int roseRadio_total_cnt = 0;
        int roseRadio_draw_cnt = 0;

        bool flagReqMore_roseRadio = false;
        bool flag_lastPage_roseRadio = false;

        bool flag_flow_draw = false;
        bool flag_roseRadio_draw = false;

        int fav_section = 0;
        int fav_index = 0;
        int fav_id = 0;
        bool fav_flag = false;
    };
};
#endif // ROSERADIO_LISTVIEWALL_H
