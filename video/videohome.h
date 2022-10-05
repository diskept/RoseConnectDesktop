#ifndef VIDEOHOME_H
#define VIDEOHOME_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "video/ItemWidget_video.h"

#include "widget/myqwidget.h"//c220729
#include "widget/customlineedit.h"//c220727

#include <QCoreApplication>


namespace video {

    /**
     * @brief 비디오-홈 @n
     *      비디오 폴더로 구성
     */

    class VideoHome : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit VideoHome(QWidget *parent = nullptr);
        ~VideoHome();

        void setDataFromRose();

    protected slots:
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode);

    private:
        void setUIControl_video();

        void request_more_videoDraw();

    private:
        Linker *linker;

        FlowLayout *flowLayout_videos;

        video::ItemWidget_video *list_video[999999];
        QJsonArray jsonArr_tracks_toPlay;

        int video_widget_cnt = 0;

        int curr_page = 0;
        int video_total_cnt = 0;
        int video_total_page = 0;
        int video_draw_cnt = 0;

        bool flagReqMore_video = false;
        bool flag_lastPage_video = false;

        bool flag_flow_draw = false;
        bool flag_video_draw = false;
    };
};


#endif // VIDEOHOME_H
