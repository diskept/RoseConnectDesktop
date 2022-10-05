#ifndef BUGSVIDEOLIST_WITHTRACK_H
#define BUGSVIDEOLIST_WITHTRACK_H

#include "bugs/AbstractBugsSubWidget.h"
#include <QWidget>

namespace bugs {

    /**
     * @brief Track에 관련된 영상 목록 클래스
     */
    class BugsVideoList_withTrack : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsVideoList_withTrack(QWidget *parent = nullptr);

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void applyLoginSuccessState() override;

    protected:
        void setUIControl_logined() override;

    protected slots:
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_applyResult_videoInfo(bugs::VideoItemData data_video, const QJsonObject& jsonObj_dataToPlay);

    private:
        void setUIControl_body_Video();
        void request_more_videoData();              ///< function - get more data : video

        QLabel* label_mainTitle;

        // data track
        bugs::TrackItemData data_track;

        // list Video
        QList<bugs::VideoItemData> *list_video;
        FlowLayout *flowLayout_video;
        QJsonArray jsonArr_videos_toPlay;

    };

};



#endif // BUGSVIDEOLIST_WITHTRACK_H
