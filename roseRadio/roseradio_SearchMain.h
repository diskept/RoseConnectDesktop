#ifndef ROSERADIO_SEARCHMAIN_H
#define ROSERADIO_SEARCHMAIN_H

#include "roseRadio/ItemRectangle.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "common/common_struct.h"

#include "login/dialoglogin.h"

#include <QCoreApplication>


namespace roseRadio {

    class roseRadioSearchMain : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:

        explicit roseRadioSearchMain(QWidget *parent = nullptr);
        ~roseRadioSearchMain();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_responseHttp(const int&, const QJsonObject&) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_showLoginPage();
        void slot_getMyInfo_loginAfter();
        void slot_change_device_state(const QString&);

        void slot_applyResult_searchChannel(const QJsonArray&, const int, const bool);

        void slot_applyResult_Favorite_state(const QJsonObject&);

        // about clicks
        void slot_clickBtn_Filter();

        void slot_clickBtn_Genre_close();
        void slot_clickBtn_Genre_choose();

        void slot_clickBtn_Country_close();
        void slot_clickBtn_Country_choose();

        void slot_clickBtn_Language_close();
        void slot_clickBtn_Language_choose();

        void slot_clickedItemRoseRadio(const int, const int, const ItemImageRoseRadio::ClickMode_RoseRadio);

    private:
        void setUIControl_Search();
        void setUIControl_filter();

        void request_more_searchData();
        void request_more_searchDraw();

        void requestGetRoseRadioGenreInfo();
        void requestGetRoseRadioRegionInfo();
        void requestGetRoseRadioLanguageInfo();

        void setResultOfRoseRadioGenreInfo(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage);
        void setResultOfRoseRadioRegionInfo(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage);
        void setResultOfRoseRadioLanguageInfo(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage);

        void applyUIControl_genres_fromData();
        void applyUIControl_country_fromData();
        void applyUIControl_language_fromData();

        void setUIControl_clear_channel();

        QWidget* setUIControl_LoginBefore();

    private:
        Linker *linker;

        Dialog::DialogLogin *dlg_rose_login;

        common::SearchData data_search;

        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;

        //===============================================================================
        FlowLayout *flow_filter;
        QHBoxLayout *hbox_filter;
        QWidget *widget_filter;

        QPushButton *btn_filter[5];

        QStringList filterTypeList;

        FlowLayout *flow_genre;
        QWidget *widget_genre;

        FlowLayout *flow_country;
        QWidget *widget_country;

        FlowLayout *flow_language;
        QWidget *widget_language;

        QPushButton *btn_genre[50];

        //===============================================================================
        QWidget *widget_main_contents;
        QWidget *widget_login_contents;
        QStackedWidget *stackedWidget_Contents;

        QWidget *widget_search;
        FlowLayout *flowLayout_search;

        roseRadio::ItemRectangle *viewAll_search[999999];

        QList<roseRadio::RoseRadioGenreInfo> *list_genreInfo;
        QList<roseRadio::RoseRadioRegionInfo> *list_countryInfo;
        QList<roseRadio::RoseRadioLanguageInfo> *list_languageInfo;

        QJsonArray jsonArr_search;

        int search_filter_cnt = 0;

        int search_genreID = 0;
        int search_countryID = 0;
        int search_languageID = 0;

        int next_offset = 0;
        int search_total_cnt = 0;
        int search_draw_cnt = 0;

        bool flagReqMore_search = false;
        bool flag_lastPage_search = false;

        bool flag_flow_draw = false;
        bool flag_search_draw = false;

        int fav_section = 0;
        int fav_index = 0;
        int fav_id = 0;
        bool fav_flag = false;

        QString btnStyle_normal;
        QString btnStyle_selected;

        int genre_offset = 0;
        int region_offset = 0;
        int language_offset = 0;

        int genre_total_count = 0;
        int region_total_count = 0;
        int language_total_count = 0;

        QJsonArray jsonArr_genre;
        QJsonArray jsonArr_region;
        QJsonArray jsonArr_language;

        bool flag_filter_ready = false;
    };
};
#endif // ROSERADIO_SEARCHMAIN_H
