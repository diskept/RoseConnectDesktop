#ifndef TIDALMYVIDEOALL_H
#define TIDALMYVIDEOALL_H

#include "AbstractTidalSubWidget.h"
#include "bugs/BugsChooseFilterOpt.h"
#include "widget/FilterWidget.h"

#include <QCoreApplication>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";

namespace tidal {

    /**
     * @brief Tidal의 My Video 전체목록 화면 ("좋아요" 선택한 목록 전체)
     */
    class TidalMyVideoAll : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalMyVideoAll(QWidget *parent = nullptr);
        ~TidalMyVideoAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)
        void resizeEvent(QResizeEvent *event) override;         // filter Box 사이즈 조절을 위해

    protected slots:
        void slot_applyResult_myFavoriteVideos(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage) override;
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_btnClicked_playAll();
        void slot_btnClicked_playShuffle();

    private:
        void setUIControl_video();
        void setUIControl_filter();

        void request_more_videoData();
        void request_more_videoDraw();

        void initAll();

    private:
        // UI
        QWidget *widget_filter_contents;

        FlowLayout *flowLayout_video;

        // Data
        tidal::ItemVideo *listVideo_all[999999];

        QList<tidal::VideoItemData> *list_video;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                               ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        QString api_subPath;

        int video_widget_cnt = 0;

        int next_offset = 0;
        int video_total_cnt = 0;
        int video_draw_cnt = 0;

        bool flagReqMore_video = false;
        bool flag_lastPage_video = false;

        bool flag_flow_draw = false;
        bool flag_video_draw = false;
    };
}

#endif // TIDALMYVIDEOALL_H
