#ifndef ROSEHOMEARTISTLISTALL_H
#define ROSEHOMEARTISTLISTALL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "roseHome/ItemArtist_rosehome.h"

#include <QCoreApplication>


namespace roseHome {

    /**
     * @brief RoseHome의 아티스트 전체보기 화면 클래스
     */
    class RoseHomeArtistListAll : public AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseHomeArtistListAll(QWidget *parent = nullptr);
        ~RoseHomeArtistListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_applyResult_artists(const QList<roseHome::ArtistItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

        void resizeEvent(QResizeEvent *event) override;

    private slots:
        void slot_applyResult_artistlistCheck(const QList<roseHome::ArtistItemData>&, const QJsonArray&, const bool);

        void slot_btnClicked_Delete();

        //void slot_filterClicked();
        //void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

    private:
        void setUIControl_artists();
        void setUIControl_filter();

        void request_more_artistData();
        void request_more_artistDraw();

        void initAll();

    private:
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

        // Filtering
        //QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        //QVariant selected_filterCode;                               ///< 현재 화면에서 선택된 필터링 코드
        //FilterWidget *filterWidget;
        //bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        PageInfo_ArtistAllView current_pageInfo;

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
    class RoseHomeArtistHistoryAll : public AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseHomeArtistHistoryAll(QWidget *parent = nullptr);
        ~RoseHomeArtistHistoryAll();

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
        roseHome::ItemArtist_rosehome *listArtist_all[999999];

        QList<roseHome::ArtistItemData> *list_artist;

        PageInfo_ArtistAllView data_pageInfo;

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

#endif // ROSEHOMEARTISTLISTALL_H
