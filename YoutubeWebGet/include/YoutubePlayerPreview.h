#ifndef YOUTUBEPLAYERPREVIEW_H
#define YOUTUBEPLAYERPREVIEW_H

#include "YoutubeWebGet_global.h"

#include <QWidget>

#include <QWidget>
#include <QWebEngineView>
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>


class YOUTUBEWEBGET_LIB_EXPORT YoutubePlayerPreview : public QWidget
{
    Q_OBJECT
public:
    YoutubePlayerPreview(QWidget *parent = nullptr);
    ~YoutubePlayerPreview();

    void getData_relatedVideoList(QString url_youtubeChannelVideo, int numVideo);
    QJsonArray getOutput_allList();
    QJsonArray getOutput_subList(int index_from, int countMax);

    enum ErrorType{
        FailedLoadingURL
    };
    Q_ENUM(ErrorType);


signals:
    void startGetAllData();
    void endedGetDescriptionInfo(QString descHtml, QString descText);
    void triedGetMoreRelatedVideoList();
    void endedGetMoreRelatedVideoList(int index_start, int count);
    void endedGetAllData(int count);
    void occurError(YoutubePlayerPreview::ErrorType errType);

private slots:
    void finishLoadingUrl(bool);
    void getMoreRelatedVideoList();

private:
    QWebEngineView *webEngineView;
    QTimer *timer;
    QString jQuery;
    bool flagGetDesc;
    int num_maxVideoList;

    int num_reqVideoList_caseEmpty;

    QString descHtml;
    QString descText;
    QJsonArray jsonArr_listVideo;

    void forceScrollBottom();

};



#endif // YOUTUBEPLAYERPREVIEW_H
