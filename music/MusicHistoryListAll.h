#ifndef MUSICHISTORYLISTALL_H
#define MUSICHISTORYLISTALL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "roseHome/ItemHistory_rosehome.h"

#include <QCoreApplication>


namespace music {

    /**
     * @brief Histrory의 리스트 전체보기 화면 클래스
     */
    class HistoryListAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit HistoryListAll(QWidget *parent = nullptr);
        ~HistoryListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

    private slots:
        void slot_applyResult_history(const QList<roseHome::HistoryItemData>&, const QJsonArray&, const bool);

    protected:
        void resizeEvent(QResizeEvent *event) override;

    private:
        void setUIControl_history();

    private:
        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_history;    ///< history's items

        // Data
        roseHome::ItemHistory_rosehome *listHistory_all[25];

        QList<roseHome::HistoryItemData> *list_history;

        roseHome::PageInfo_HistoryAllView data_pageInfo;

        int history_widget_width = 0;
        int history_widget_margin = 0;
    };
};
#endif // MUSICHISTORYLISTALL_H
