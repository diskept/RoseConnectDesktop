#ifndef ROSERADIO_COUNTRYVIEWALL_H
#define ROSERADIO_COUNTRYVIEWALL_H

#include "roseRadio/roseradio_struct.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include <QCoreApplication>


namespace roseRadio {


    class RoseRadioCountryViewAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT

    public:
        explicit RoseRadioCountryViewAll(QWidget * parent = nullptr);
        ~RoseRadioCountryViewAll();

        void setJsonObject_forData(const QJsonObject&) override;
        void setActivePage() override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_Country(const QJsonArray&, const int, const bool);

        void slot_clickedItemCountry();

    private:
        void setUIControl_Country();

        void request_more_countryData();
        void request_more_countryDraw();

        QWidget* setCountry_pushButton(const int index, const QString name);

    private:
        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;
        QWidget *widget_country;
        FlowLayout *flowLayout_country;

        QString apiPath = "";
        QString title = "";
        QString listType = "";
        int genreId = 0;

        QList<roseRadio::RoseRadioRegionInfo> *list_countryInfo;
        QJsonArray jsonArr_country;

        int next_offset = 0;
        int country_total_cnt = 0;
        int country_draw_cnt = 0;

        bool flagReqMore_country = false;
        bool flag_lastPage_country = false;

        bool flag_flow_draw = false;
        bool flag_country_draw = false;
    };
};
#endif // ROSERADIO_COUNTRYVIEWALL_H
