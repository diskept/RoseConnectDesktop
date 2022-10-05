#ifndef VIDEODETAIL_H
#define VIDEODETAIL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QCoreApplication>


namespace video {

    /**
     * @brief The VideoDetail class 비디오 상세 @n
     *      비디오 폴더의 영상 목록으로 구성
     */
    class VideoDetail : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit VideoDetail(QWidget *parent = nullptr);
        ~VideoDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    signals:
        void signal_changedMenuName(const QString &p_menuName);

    protected slots:
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData) override;

    private slots:
        void slot_applyResult_videoAlbum();
        void slot_applyResult_videoTrack();

        // videoImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        //void proc_wheelEvent_to_getMoreData() override;

    private:
        QJsonArray get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode);
        QJsonArray subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex);
        QJsonArray reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex);

    private:
        AbstractImageDetailContents_RHV *video_detail_info;
        PlaylistTrackDetailInfo_RHV *video_track_info[99999];

        // UI
        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QString title = "";
        QString imagePath = "";

        int duration = 0;

        QJsonArray jsonArr_tracks_toPlay;

        bool flag_track_ok = false;
    };
};

#endif // VIDEODETAIL_H
