#ifndef ROSETUBE_CHANNEL_H
#define ROSETUBE_CHANNEL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "rosetube/ItemTrack_rosetube.h"

#include "widget/dialogconfirm.h"

#include <YoutubeWebGet_global.h>
#include <YoutubeChannelVideo.h>

#include <QCoreApplication>


namespace rosetube {

    /**
     * @brief 濡쒖쫰쒕툕 梨꾨꼸 곸꽭 @n
     *      濡쒖쫰쒕툕 梨꾨꼸 뺣낫 諛곸긽 뺣낫濡援ъ꽦 @n
     *
     * @note QWebengined댁슜щ·留     */
    class RoseTube_Channel : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT

    public:
        explicit RoseTube_Channel(QWidget *parent = nullptr);
        ~RoseTube_Channel();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_responseHttp(const int&, const QJsonObject&) override;

        // about Item
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_fileDownload_bannerImage();
        void slot_fileDownload_avatarImage();

        void slot_streamData(const QJsonArray&);
        void slot_applyResult_getShareLink(const QString &link);//c220818 share link

        void slot_btnClicked_Subscribe();
        void slot_btnClicked_Play();

        // for YoutubeChannelVideo - processing & handling
        void slot_ytbChannelVideo_getDone_avatarData(const QJsonObject&);
        void slot_ytbChannelVideo_getDone_bannerData(const QJsonObject&);
        void slot_ytbChannelVideo_getDone_list_videoData(const int start_index, const QJsonArray& jsonArr_video);
        void slot_ytbChannelVideo_noMoreData_list_video();
        void slot_ytbChannelVideo_occurError(YoutubeChannelVideo::ErrorType errType);

    private:
        void setUIControl_requestGetSubscriptionTrack();

        void setUIControl_requestGetSubscription();
        void setUIControl_requestSetSubscription();

        void setUIControl_setSubscribeBtnStyle();

        int get_rose_playType(OptMorePopup::ClickMode clickMode);
        QJsonArray get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode);
        QJsonArray subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex);
        QJsonArray reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex);

    private:
        Linker *linker;
        FileDownloader *bannerDownloader;
        FileDownloader *avatarDownloader;

        DialogConfirm *dlgSubscribe;

        QString btnStyle_play;
        QString btnStyle_shuffle;

        QString channelId = "";
        QString channelName = "";
        QString channelDescription = "";
        int channelSubscriber = 0;
        QJsonObject channelNext;

        YoutubeChannelVideo *ytbChannelVideo;

        QVBoxLayout *vbox_contents;
        QWidget *widget_contents;

        QHBoxLayout *hBox_topBanner;
        QWidget *widget_topBanner;
        QLabel *lb_topBanner;

        QVBoxLayout *vBox_subcription_info;
        QWidget *widget_subscription_info;
        QLabel *lb_Avatar;
        QLabel *lb_ChannelName;
        QLabel *lb_Subscriber;
        QPushButton *btnSubscription;
        QLabel *lb_subscription_img;
        QLabel *lb_subscription;

        FlowLayout *flow_subscription_track;
        QWidget *widget_subscription_content;
        rosetube::ItemTrack_rosetube *subscription_track[999999];

        QJsonArray jsonArr_tracks_toPlay;
        QJsonArray jsonArr_tracks_tmp;

        bool flagReqMore_video = false;

        bool flagSubscribe = false;
    };
};


#endif // ROSETUBE_CHANNEL_H
