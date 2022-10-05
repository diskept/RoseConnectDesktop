#ifndef ROSERADIO_COUNTRYDETAIL_H
#define ROSERADIO_COUNTRYDETAIL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "roseRadio/ItemSquare.h"

#include <QCoreApplication>


namespace roseRadio {

    class RoseRadioCountryDetail : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT

    public:
        explicit RoseRadioCountryDetail(QWidget * parent = nullptr);
        ~RoseRadioCountryDetail();

        void setJsonObject_forData(const QJsonObject&) override;
        void setActivePage() override;

    signals:
        void signal_clickedViewAll(const QJsonObject &p_jsonObject);

    protected slots:
        void slot_responseHttp(const int&, const QJsonObject&) override;

    private slots:
        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_Region(const QJsonArray&, const int, const bool);
        void slot_applyResult_Popular(const QJsonArray&, const int, const bool);

        void slot_applyResult_Favorite_state(const QJsonObject&);

        void slot_clickedItemRegion();

        void slot_clickedItemRoseRadio(const int section, const int index, const roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio clickMode);

    private:
        void setUIControl_requestRegion();

        void setUIControl_appendWidget_region();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);

    private:
        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;

        QVBoxLayout *box_region_contents;
        QWidget *widget_region_contents;

        QVBoxLayout *box_popular_contents;
        QWidget *widget_popular_contents;

        QLabel *lb_subTitle[5];
        QPushButton *btnView_all[5];

        QVBoxLayout *vBox_region;
        FlowLayout *flow_region;
        QWidget *widget_region;

        roseRadio::ItemSquare *region_popular[14];

        QVBoxLayout *vBox_popular;
        FlowLayout *flow_popular;
        QWidget *widget_popular;

        QJsonArray jsonArr_region;
        QJsonArray jsonArr_popular;

        QString apiPath = "";
        QString title = "";
        QString listType = "";
        int regionId = 0;

        int fav_section = 0;
        int fav_index = 0;
        int fav_id = 0;
        bool fav_flag = false;

        bool flag_region[2] = {false, false};
        bool flag_popular[2] = {false, false};
    };
};
#endif // ROSERADIO_COUNTRYDETAIL_H
