#ifndef QOBUZMYARTISTALL_H
#define QOBUZMYARTISTALL_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "qobuz/ItemArtist_forQobuz.h"

#include "roseHome/ItemArtist_rosehome.h"

#include "bugs/BugsChooseFilterOpt.h"
#include "widget/FilterWidget.h"

#include <QCoreApplication>

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz의 My Artist 전체목록 화면 ("좋아요" 선택한 목록 전체)
     */
    class QobuzMyArtistAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzMyArtistAll(QWidget *parent = nullptr);
        ~QobuzMyArtistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;         // filter Box 사이즈 조절을 위해

    protected slots:
        void slot_applyResult_myFavoriteArtists(const QList<qobuz::ArtistItemData>& list_data, const bool) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

    private:
        // 초기 UI화면 구성
        void setUIControl_artists();
        void setUIControl_filter();

        void request_more_artistData();
        void request_more_artistDraw();

        void initAll();

    private:
        FlowLayout *flowLayout_artists;

        // Data
        qobuz::ItemArtist_qobuz *listArtist_all[99999];

        QList<qobuz::ArtistItemData> *list_artist;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                               ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        QString page;

        int artist_widget_cnt = 0;

        int next_offset = 0;
        int artist_total_cnt = 0;
        int artist_draw_cnt = 0;

        bool flagReqMore_artist = false;
        bool flag_lastPage_artist = false;

        bool flag_flow_draw = false;
        bool flag_artist_draw = false;
    };





    /**
     * @brief Qobuz의 최근 재생 아티스트 전체보기 화면 클래스
     */
    class QobuzRecenltyArtistAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzRecenltyArtistAll(QWidget *parent = nullptr);
        ~QobuzRecenltyArtistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;                         // filter Box 사이즈 조절을 위해

    protected slots:
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_recentlyArtists(const QList<roseHome::ArtistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_artistlistCheck(const QList<roseHome::ArtistItemData>&, const QJsonArray&, const bool);

        void slot_btnClicked_Delete();

    private:
        void setUIControl_artists();
        void setUIControl_filter();

        void request_more_artistData();
        void request_more_artistDraw();

        void initAll();

    private:
        Linker *linker;

        // 관리 필요한 Layout UI
        QWidget *widget_mainTitle;

        QLabel *label_mainTitle;
        QPushButton *btn_mainTitle;
        QLabel *label_delete_icon;
        QLabel *label_delete;

        FlowLayout *flowLayout_artists;    ///< artist's items

        // Data
        roseHome::ItemArtist_rosehome *listArtist_all[999999];

        QList<roseHome::ArtistItemData> *list_artist;

        QString page = "";
        QString title = "";

        int artist_widget_width = 0;
        int artist_widget_margin = 0;

        int artist_widget_cnt = 0;

        int next_offset = 0;
        int artist_total_cnt = 0;
        int artist_draw_cnt = 0;

        bool flagReqMore_artist = false;
        bool flag_lastPage_artist = false;

        bool flag_flow_draw = false;
        bool flag_artist_draw = false;
    };





    /**
    * @brief RoseHome의 앨범 전체보기 화면 클래스
    */
    class QobuzArtistHistoryAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzArtistHistoryAll(QWidget *parent = nullptr);
        ~QobuzArtistHistoryAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;

    protected slots:
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_historyArtist(const QList<roseHome::ArtistItemData>&, const QJsonArray&, const bool);

    private:
        void setUIControl_artists();

        void request_more_artistData();
        void request_more_artistDraw();

    private:
        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_artists;    ///< artist's items

        // Data
        roseHome::ItemArtist_rosehome *listArtist_all[9999];

        QList<roseHome::ArtistItemData> *list_artist;

        roseHome::PageInfo_ArtistAllView data_pageInfo;

        int artist_widget_width = 0;
        int artist_widget_margin = 0;

        int artist_widget_cnt = 0;

        int next_offset = 0;
        int artist_total_cnt = 0;
        int artist_draw_cnt = 0;

        bool flagReqMore_artist = false;
        bool flag_lastPage_artist = false;

        bool flag_flow_draw = false;
        bool flag_artist_draw = false;
    };
};
#endif // QOBUZMYARTISTALL_H
