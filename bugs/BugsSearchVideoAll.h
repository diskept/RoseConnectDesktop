#ifndef BUGSSEARCHVIDEOALL_H
#define BUGSSEARCHVIDEOALL_H

#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/ItemVideo_bugs.h"

#include "common/common_struct.h"

#include <QCoreApplication>


namespace bugs {

    /**
     * @brief 검색결과 - Video 전체 목록 클래스
     */
    class BugsSearchVideoAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsSearchVideoAll(QWidget *parent = nullptr);
        ~BugsSearchVideoAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)
        void resizeEvent(QResizeEvent *event) override;   //c230223

    protected slots:
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;

        void slot_applyResult_videos(const QList<bugs::VideoItemData>&, const QJsonArray&, const bool) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:

    private:
        void setUIControl_videos();

        void request_more_videoData();
        void request_more_videoDraw();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_video;    ///< album's items

        // Data
        bugs::ItemVideo_bugs *listVideo_all[999999];

        QList<bugs::VideoItemData> *list_video;
        QJsonArray jsonArr_videos_toPlay;

        QString page;
        common::SearchData data_search;

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

#endif // BUGSSEARCHVIDEOALL_H
