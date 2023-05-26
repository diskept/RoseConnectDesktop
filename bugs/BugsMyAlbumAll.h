#ifndef BUGSMYALBUMALL_H
#define BUGSMYALBUMALL_H

#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/ItemAlbum_bugs.h"
#include "bugs/ItemMyAlbum_bugs.h"

#include "roseHome/ItemAlbum_rosehome.h"

#include "bugs/BugsChooseFilterOpt.h"
#include "widget/FilterWidget.h"

#include <QCoreApplication>


namespace bugs {

    /**
     * @brief Bugs의 My Album 전체목록 화면
     */
    class BugsMyAlbumlistAll : public AbstractBugsSubWidget
    {
        Q_OBJECT

    public:
        explicit BugsMyAlbumlistAll(QWidget *parent = nullptr);
        ~BugsMyAlbumlistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_applyResult_my_albums(const QList<bugs::MyAlbumItemData>&, const bool) override;

        void slot_clickedItemMyAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

        void resizeEvent(QResizeEvent *event) override;                         ///< filter Box 사이즈 조절을 위해

    private:
        // 초기 UI화면 구성
        void setUIControl_album();

        void request_more_albumData();
        void request_more_albumDraw();

    private:
        Linker *linker;

        // 관리 필요한 Layout UI
        QWidget *widget_mainTitle;

        QLabel *label_mainTitle;
        QPushButton *btn_mainTitle;
        QLabel *label_delete_icon;
        QLabel *label_delete;

        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        bugs::ItemMyAlbum_bugs *myCollection_album_myAlbum[999999];

        QList<bugs::MyAlbumItemData> *list_Myalbum;


        QString page = "";
        QString title = "";

        int album_widget_width = 0;
        int album_widget_margin = 0;

        int album_widget_cnt = 0;

        int next_offset = 0;
        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        // getting more data
        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };






    /**
     * @brief Bugs의 Recently Album 전체목록 화면
     */
    class BugsRecentlyAlbumAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsRecentlyAlbumAll(QWidget *parent = nullptr);
        ~BugsRecentlyAlbumAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;                         ///< filter Box 사이즈 조절을 위해

    protected slots:
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_myAlbums_rose(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_albumlistCheck(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool);

        void slot_btnClicked_Delete();

    private:
        void setUIControl_albums();
        void setUIControl_filter();

        void request_more_albumData();
        void request_more_albumDraw();

        void initAll();

    private:
        Linker *linker;

        // 관리 필요한 Layout UI
        QWidget *widget_mainTitle;

        QLabel *label_mainTitle;
        QPushButton *btn_mainTitle;
        QLabel *label_delete_icon;
        QLabel *label_delete;

        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        roseHome::ItemAlbum_rosehome *listAlbum_all[999999];

        QList<roseHome::AlbumItemData> *list_album;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                                ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        QString page = "";
        QString title = "";

        int album_widget_width = 0;
        int album_widget_margin = 0;

        int album_widget_cnt = 0;

        int next_offset = 0;
        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };





    /**
     * @brief RoseHome의 앨범 전체보기 화면 클래스
     */
    class BugsAlbumHistoryAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsAlbumHistoryAll(QWidget *parent = nullptr);
        ~BugsAlbumHistoryAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

        void resizeEvent(QResizeEvent *event) override;

    private slots:
        void slot_applyResult_historyAlbum(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool);

    private:
        void setUIControl_albums();

        void request_more_albumData();
        void request_more_albumDraw();

    private:
        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        roseHome::ItemAlbum_rosehome *listAlbum_all[999999];

        QList<roseHome::AlbumItemData> *list_album;

        roseHome::PageInfo_AlbumAllView data_pageInfo;

        int album_widget_width = 0;
        int album_widget_margin = 0;

        int album_widget_cnt = 0;

        int next_offset = 0;
        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };





    /**
     * @brief Bugs의 Recently Album 전체목록 화면
     */
    class BugsAlbumRecentlyAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsAlbumRecentlyAll(QWidget *parent = nullptr);
        ~BugsAlbumRecentlyAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;   //c230223                      ///< filter Box 사이즈 조절을 위해

    protected slots:
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_albumRecently(const QList<bugs::AlbumItemData>&, const bool);

    private:
        void setUIControl_albums();
        void setUIControl_filter();

        void request_more_albumData();
        void request_more_albumDraw();

        void initAll();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        bugs::ItemAlbum_bugs *listAlbum_all[999999];

        QList<bugs::AlbumItemData> *list_album;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                                ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        QString page;

        int album_widget_cnt = 0;

        int next_offset = 0;
        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };





    /**
     * @brief Bugs의 Recently Album 전체목록 화면
     */
    class BugsAlbumChartAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsAlbumChartAll(QWidget *parent = nullptr);
        ~BugsAlbumChartAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;                         ///< filter Box 사이즈 조절을 위해

    protected slots:
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_albumChart(const QList<bugs::AlbumItemData>&, const bool);

    private:
        void setUIControl_albums();
        void setUIControl_filter();

        void request_more_albumData();
        void request_more_albumDraw();

        void initAll();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        bugs::ItemAlbum_bugs *listAlbum_all[999999];

        QList<bugs::AlbumItemData> *list_album;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                                ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        QString page;

        int album_widget_cnt = 0;

        int next_offset = 0;
        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };
};

#endif // BUGSMYALBUMALL_H
