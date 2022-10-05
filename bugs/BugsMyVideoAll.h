#ifndef BUGSMYVIDEOALL_H
#define BUGSMYVIDEOALL_H

#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/ItemVideo_bugs.h"
#include "bugs/BugsChooseFilterOpt.h"

#include "widget/FilterWidget.h"

#include <QCoreApplication>


namespace bugs {

    /**
     * @brief Bugs의 Recently Video 전체목록 화면
     */
    class BugsVideoRecentlyAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsVideoRecentlyAll(QWidget *parent = nullptr);
        ~BugsVideoRecentlyAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;                         ///< filter Box 사이즈 조절을 위해

    protected slots:
        //void slot_btnClicked_playAll() override;

        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_videoRecently(const QList<bugs::VideoItemData>&, const QJsonArray&, const bool);

    private:
        void setUIControl_videos();
        void setUiControl_filter();

        void request_more_videoData();
        void request_more_videoDraw();

        void initAll();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_videos;    ///< video's items

        // Data
        bugs::ItemVideo_bugs *listVideo_all[999999];

        QList<bugs::VideoItemData> *list_video;
        QJsonArray jsonArr_videos_toPlay;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                                ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        QString page;

        int video_widget_cnt = 0;

        int next_offset = 0;
        int video_total_cnt = 0;
        int video_draw_cnt = 0;

        bool flagReqMore_video = false;
        bool flag_lastPage_video = false;

        bool flag_flow_draw = false;
        bool flag_video_draw = false;
    };





    /**
     * @brief Bugs의 Recently Video 전체목록 화면
     */
    class BugsVideoChartAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsVideoChartAll(QWidget *parent = nullptr);
        ~BugsVideoChartAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;                         ///< filter Box 사이즈 조절을 위해

    protected slots:
        //void slot_btnClicked_playAll() override;

        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_videoChart(const QList<bugs::VideoItemData>&, const QJsonArray&, const bool);

    private:
        void setUIControl_videos();
        void setUiControl_filter();

        void request_more_videoData();
        void request_more_videoDraw();

        void initAll();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_videos;    ///< video's items

        // Data
        bugs::ItemVideo_bugs *listVideo_all[999999];

        QList<bugs::VideoItemData> *list_video;
        QJsonArray jsonArr_videos_toPlay;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                                ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        QString page;

        int video_widget_cnt = 0;

        int next_offset = 0;
        int video_total_cnt = 0;
        int video_draw_cnt = 0;

        bool flagReqMore_video = false;
        bool flag_lastPage_video = false;

        bool flag_flow_draw = false;
        bool flag_video_draw = false;
    };
};
#endif // BUGSMYVIDEOALL_H
