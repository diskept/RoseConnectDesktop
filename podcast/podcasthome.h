#ifndef PODCASTHOME_H
#define PODCASTHOME_H


#include <QWidget>
#include "widget/flowlayout.h"

class PodcastHome : public QWidget
{
    Q_OBJECT
public:
    explicit PodcastHome(QWidget *parent = nullptr);

    void requestData();

signals:

private:
    void setInit();
    void setUIControl();
    void setResultOfChannelList(const QJsonObject &p_jsonObject);
    void appendChannel(const QJsonObject &jsonData);

    FlowLayout *flowLayout;


private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);



};

#endif // PODCASTHOME_H
