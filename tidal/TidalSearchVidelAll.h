#ifndef TIDALSEARCHVIDELALL_H
#define TIDALSEARCHVIDELALL_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemVideo.h"

#include "common/common_struct.h"

#include <QCoreApplication>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief 검색결과 - Video 전체 목록 클래스
     */
    class TidalSearchVidelAll : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalSearchVidelAll(QWidget *parent = nullptr);
        ~TidalSearchVidelAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

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
        common::SearchData data_search;

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


#endif // TIDALSEARCHVIDELALL_H
