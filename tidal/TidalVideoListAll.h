#ifndef TIDALVIDEOLISTALL_H
#define TIDALVIDEOLISTALL_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemVideo.h"

#include "common/common_struct.h"

#include <QCoreApplication>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal의 Video 전체보기 화면 클래스
     */
    class TidalVideoListAll : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalVideoListAll(QWidget *parent = nullptr);
        ~TidalVideoListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_applyResult_videos(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage) override;
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private:
        // 초기 UI화면 구성
        void setUIControl_video();

        void request_more_videoData();
        void request_more_videoDraw();

    private:
        QString api_subPath;            ///< 타이달 API 요청을 위한 subPath

        QLabel *label_mainTitle;
        FlowLayout *flowLayout_video;

        // Data
        tidal::ItemVideo *listVideo_all[999999];

        QList<tidal::VideoItemData> *list_video;

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

#endif // TIDALVIDEOLISTALL_H
