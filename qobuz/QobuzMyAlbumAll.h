#ifndef QOBUZMYALBUMALL_H
#define QOBUZMYALBUMALL_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "qobuz/ItemAlbum_forQobuz.h"

#include "roseHome/ItemAlbum_rosehome.h"

#include "bugs/BugsChooseFilterOpt.h"
#include "widget/FilterWidget.h"

#include <QCoreApplication>

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz의 My Album 전체목록 화면 ("좋아요" 선택한 목록 전체)
     */
    class QobuzMyAlbumAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzMyAlbumAll(QWidget *parent = nullptr);
        ~QobuzMyAlbumAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;    //c230223                     // filter Box 사이즈 조절을 위해

    protected slots:
        void slot_applyResult_myFavoriteAlbums(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

    private:
        void setUIControl_albums();
        void setUIControl_filter();

        void request_more_albumData();
        void request_more_albumDraw();

        void initAll();

    private:
        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        qobuz::ItemAlbum_qobuz *listAlbum_all[999999];

        QList<qobuz::AlbumItemData> *list_album;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                               ///< 현재 화면에서 선택된 필터링 코드
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
     * @brief Qobuz의 My Album 전체목록 화면 ("좋아요" 선택한 목록 전체)
     */
    class QobuzMyPurchasesAlbumAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzMyPurchasesAlbumAll(QWidget *parent = nullptr);
        ~QobuzMyPurchasesAlbumAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;                         // filter Box 사이즈 조절을 위해

    protected slots:
        void slot_applyResult_myFavoriteAlbums(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

    private:
        void setUIControl_albums();
        void setUIControl_filter();

        void request_more_albumData();
        void request_more_albumDraw();

        void initAll();

    private:
        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        qobuz::ItemAlbum_qobuz *listAlbum_all[999999];

        QList<qobuz::AlbumItemData> *list_album;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                               ///< 현재 화면에서 선택된 필터링 코드
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
     * @brief Qobuz의 Recently Album 전체목록 화면
     */
    class QobuzRecentlyAlbumAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzRecentlyAlbumAll(QWidget *parent = nullptr);
        ~QobuzRecentlyAlbumAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;                         // filter Box 사이즈 조절을 위해

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
        QVariant selected_filterCode;                               ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        QString page;
        QString title;

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
    class QobuzAlbumHistoryAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzAlbumHistoryAll(QWidget *parent = nullptr);
        ~QobuzAlbumHistoryAll();

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
};


#endif // QOBUZMYALBUMALL_H
