#ifndef YOUTUBECHANNELVIDEO_H
#define YOUTUBECHANNELVIDEO_H

#include "YoutubeWebGet_global.h"

#include <QWidget>
#include <QWebEngineView>
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>


/**
 * @brief Youtube Channel
 * @details by sunnyfish
 * @version 2.0
 */
class YOUTUBEWEBGET_LIB_EXPORT YoutubeChannelVideo : public QWidget
{
    Q_OBJECT

public:

    enum class CrawlingStatus {
        Idle
        , Processing
        , End
    };
    Q_ENUM(CrawlingStatus);


    enum ErrorType{
        FailedLoadingURL
        , CrawlerProcessing
        , Needed_Call_GetFirstData
    };
    Q_ENUM(ErrorType);

    struct YtbChannel_CrawlingInfo {
        QString urlPath;
        QString timestamp_id;
        CrawlingStatus status = CrawlingStatus::Idle;
        int cnt_video = 0;
    };


    YoutubeChannelVideo(QWidget *parent = nullptr);
    ~YoutubeChannelVideo();

    // function about requesting
    void getDataFirst_youtubeChannel_video(QString url_youtubeChannelVideo, int req_max_cnt_video);
    void getMoreData_youtubeChannel_video(int req_max_cnt_video);

    // functions about return result data
    QJsonObject getOutputAll_first();
    QJsonObject getOutput_avatar();
    QJsonObject getOutput_banner();
    QJsonArray getOutput_videoList(int start_index, int max_cnt);




signals:
    // signals for debugging
    void signal_forDebug_start_getDataFirst();
    void signal_forDebug_start_getListVideoData(const int start_index, int max_cnt);
    void signal_forDebug_crawling_partial_vidioData(const int num_crwaling);
    void signal_forDebug_processing_list_videoData(const int num_toTry);

    // signals for processing and handling
    void signal_getDone_avatarData(const QJsonObject&);
    void signal_getDone_bannerData(const QJsonObject&);
    void signal_getDone_list_videoData(const int start_index, const QJsonArray&);
    void signal_noMoreData_list_video();
    void signal_occurError(YoutubeChannelVideo::ErrorType errType);



private slots:
    void slot_finishLoadingUrl(bool);
    void slot_timerChecked();

private:

    QWebEngineView *webEngineView;
    QTimer *timer_getMore_afterScrollBtm;

    QString jQuery;

    QList<YtbChannel_CrawlingInfo> list_crawlingInfo;



    int req_start_index;
    int req_max_cnt_video;

    int num_reqVideoList_caseEmpty;

    // to return result data
    QJsonObject jsonObj_banner;
    QJsonObject jsonObj_avatar;
    QJsonArray jsonArr_listVideo;

    // function
    void forceScrollBottom();
    void getMoreVideoList();

};


#endif // YOUTUBECHANNELVIDEO_H
