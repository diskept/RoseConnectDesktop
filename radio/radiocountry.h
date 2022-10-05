#ifndef RADIOCOUNTRY_H
#define RADIOCOUNTRY_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "common/linker.h"

#include "data/dataradio.h"

#include "widget/flowlayout.h"

#include <QWidget>
#include <QSettings>
#include <QGridLayout>


namespace radio {

    /**
     * @brief 라디오 > 국가 @n
     *      해당 국가의 라디오 채널 목록으로 구성
     *
     * @note 국가명 클릭시 QCombobox show, 국가변경 가능
     */
    class RadioCountry : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RadioCountry(QWidget *parent = nullptr);
        ~RadioCountry();

        void requestData();

        void hide_comboBoxWidget(bool flagCallSignal = false);
        void show_topBtnControl();
        void hide_topBtnControl();
        void showComboBoxPopup();

    signals:
        void signal_changedCurrSubMenuName(const QString &p_subMenuName); ///< 서브메뉴명 변경 요청 시그널

    protected slots:
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;

    private slots:
        void slot_clickedHoverItem(const QString &p_code);
        void slot_clickedCountryItem();
        void slot_changedFavorChannel(const bool &flagChangedInFavor);
        void slot_clickedRefresh();
        void slot_clickedAddChannel();

    private:
        void setInit();
        void setUIControl();
        void clearChannelList();
        void requestCountryList();
        void requestFavoriteList();
        void requestChannelList();
        void setTopUIControl(QWidget *parent);
        void setResultOfChannelList(const QJsonObject &p_jsonObject);
        void setResultOfCountryList(const QJsonObject &p_jsonObject);
        void setResultOfFavoriteList(const QJsonObject &p_jsonObject);
        void appendRadioChannel(const QJsonObject &p_jsonObject);
        void setCountryItemFocusStyle(const int &p_indexFocus);

        void resizeEvent(QResizeEvent *event) override;

    private:
        Linker *linker;

        QWidget *widget_topContrl;

        int currCountryIndex=-1;
        int currCountryId;
        QString currCountryName;
        QSettings *settings;

        //bool flagHasCountryList = false;
        //bool flagHasFavorList = false;

        /// 현재 콤보박스가 show되었는지 여부
        bool flagShowedComboBox = false;
        QWidget *widget_country;
        QWidget *widget_combobox;

        QWidget *widget_radio;
        FlowLayout *flowLayout_radio;

        QList<QString> favorUrlList;
        QList<DataRadio*> dataList;
        QList<QWidget*> *listConntry;
    };
};

#endif // RADIOCOUNTRY_H
