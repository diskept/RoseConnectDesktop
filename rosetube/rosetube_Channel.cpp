#include "rosetube/rosetube_Channel.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"
#include "widget/toastmsg.h"

#include <QScrollBar>
#include <QPainterPath>


namespace rosetube {

    const int HTTP_NETWORK_PLAY = 99;
    const int HTTP_NETWORK_STREAM_GET = 98;
    const int HTTP_NETWORK_SUBSCRIBE_GET = 97;
    const int HTTP_NETWORK_SUBSCRIBE_SET = 96;

    const int HTTP_NETWORK_CACHE = 90;

    const int SECTION_FOR_MORE_POPUP___SUBSCRIBE = 0;

    const QString ICON_PATH___playAll = ":/images/icon_list_play.png";
    const QString ICON_PATH___playShuffle = ":/images/icon_list_shuffle.png";

    const QString ICON_PATH___subscriptionOff = ":/images/tube_sub_off.png";
    const QString ICON_PATH___subscriptionOn = ":/images/tube_sub_on.png";


    /**
     * @brief RoseTube_Channel::RoseTube_Channel : 로즈튜브-구독-채널
     * @details 로즈튜브-구독-채널, Youtube DATA API 사용
     * @param parent
     */
    RoseTube_Channel::RoseTube_Channel(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_rosetube, parent) {

        this->btnStyle_play = "QPushButton{background:#B18658; border:1px solid #B18658; border-radius:4px;}";
        this->btnStyle_play += "QPushButton:hover{background:#7D6144; border:1px solid #7D6144; border-radius:4px;}";

        this->btnStyle_shuffle = "QPushButton{background:#777777; border:1px solid #777777; border-radius:4px;}";
        this->btnStyle_shuffle += "QPushButton:hover{background:#4D4D4D; border:1px solid #4D4D4D; border-radius:4px;}";

        this->linker = Linker::getInstance();

        this->bannerDownloader = new FileDownloader;
        connect(this->bannerDownloader, SIGNAL(downloaded()), this, SLOT(slot_fileDownload_bannerImage()));

        this->avatarDownloader = new FileDownloader;
        connect(this->avatarDownloader, SIGNAL(downloaded()), this, SLOT(slot_fileDownload_avatarImage()));

        ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

        // YoutubeChannelVideo 객체 생성
        //this->ytbChannelVideo = new YoutubeChannelVideo(this);
        //connect(this->ytbChannelVideo, &YoutubeChannelVideo::signal_getDone_avatarData, this, &RoseTube_Channel::slot_ytbChannelVideo_getDone_avatarData);
        //connect(this->ytbChannelVideo, &YoutubeChannelVideo::signal_getDone_bannerData, this, &RoseTube_Channel::slot_ytbChannelVideo_getDone_bannerData);
        //connect(this->ytbChannelVideo, &YoutubeChannelVideo::signal_getDone_list_videoData, this, &RoseTube_Channel::slot_ytbChannelVideo_getDone_list_videoData);
        //connect(this->ytbChannelVideo, &YoutubeChannelVideo::signal_noMoreData_list_video, this, &RoseTube_Channel::slot_ytbChannelVideo_noMoreData_list_video);
        //connect(this->ytbChannelVideo, &YoutubeChannelVideo::signal_occurError, this, &RoseTube_Channel::slot_ytbChannelVideo_occurError);
    }


    RoseTube_Channel::~RoseTube_Channel(){

        this->deleteLater();
    }


    void RoseTube_Channel::setJsonObject_forData(const QJsonObject& jsonObj){

        const QString jsonKey_channel_id = "channel_id";

        QString tmp_channelId = ProcJsonEasy::getString(jsonObj, jsonKey_channel_id);
        this->flagNeedReload = false;

        if(this->channelId != tmp_channelId){
            this->flagNeedReload = true;

            this->channelId = tmp_channelId;

            this->jsonArr_tracks_toPlay = QJsonArray();
            this->jsonArr_tracks_tmp = QJsonArray();

            this->flagReqMore_video = true;
            //QString url_youtubeChannelVideo = "https://www.youtube.com/channel/" + channelId + "/videos";
            //this->ytbChannelVideo->getDataFirst_youtubeChannel_video(url_youtubeChannelVideo, 30);

            this->setUIControl_requestGetSubscription();

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QString preheader = global.newpipe_v1 + "/channel/";
            QString url = preheader + QString("%1").arg(this->channelId);
            QJsonObject json;

            network->request(HTTP_NETWORK_STREAM_GET
                             , url
                             , json
                             , false
                             , true);
        }
    }


    void RoseTube_Channel::setActivePage(){

        if(this->flagNeedReload){

            this->box_contents->removeWidget(this->widget_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->lb_topBanner = new QLabel();
            this->lb_topBanner->setFixedSize(1680, 278);
            this->lb_topBanner->setStyleSheet("background-color:#212121; border:2px solid #4D4D4D; border-radius:4px;");

            this->hBox_topBanner = new QHBoxLayout();
            this->hBox_topBanner->setContentsMargins(0, 0, 0, 0);
            this->hBox_topBanner->addWidget(this->lb_topBanner, 0, Qt::AlignCenter);

            this->widget_topBanner = new QWidget();
            this->widget_topBanner->setStyleSheet("background-color:#212121;");
            this->widget_topBanner->setFixedSize(1680, 278);
            this->widget_topBanner->setLayout(this->hBox_topBanner);

            this->widget_subscription_info = new QWidget();
            this->widget_subscription_info->setStyleSheet("background-color:#212121;");
            this->widget_subscription_info->setFixedSize(1500, 240);

            QWidget *widget_line = new QWidget(this->widget_subscription_info);
            widget_line->setFixedSize(1500, 1);
            widget_line->setStyleSheet("background-color:#4D4D4D; border:1px solid #4D4D4D;");
            widget_line->setGeometry(0, 239, 0, 0);

            QPixmap pixmap(":/images/rosehome/artist_def.png");
            pixmap = pixmap.scaled(180, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            QPixmap pixmap_painter = QPixmap(QSize(180, 180));
            pixmap_painter.fill(Qt::transparent);

            QPainter painter (&pixmap_painter);
            painter.setRenderHint(QPainter::Antialiasing, true);
            QBrush brush = QBrush(pixmap);
            painter.setBrush(brush);
            painter.drawRoundedRect(0, 0, 180, 180, 90, 90);

            this->lb_Avatar = new QLabel(this->widget_subscription_info);
            this->lb_Avatar->setStyleSheet("background-color:#212121; border:0px;");
            this->lb_Avatar->setFixedSize(180, 180);
            this->lb_Avatar->setGeometry(0, 0, 0, 0);
            this->lb_Avatar->setPixmap(pixmap_painter);

            this->lb_ChannelName = new QLabel(this->widget_subscription_info);
            this->lb_ChannelName->setStyleSheet("background-color:#212121; font-size:50px; color:#FFFFFF;");
            this->lb_ChannelName->setGeometry(225, 0, 1100, 75);
            this->lb_ChannelName->setAlignment(Qt::AlignVCenter);

            this->lb_Subscriber = new QLabel(this->widget_subscription_info);
            this->lb_Subscriber->setStyleSheet("background-color:#212121; font-size:20px; color:#999999; font-weight:300;");
            this->lb_Subscriber->setGeometry(225, 75, 1100, 30);
            this->lb_Subscriber->setAlignment(Qt::AlignVCenter);

            QPushButton *btnPlay = new QPushButton(this->widget_subscription_info);
            btnPlay->setFixedSize(150, 50);
            btnPlay->setGeometry(225, 130, 0, 0);
            btnPlay->setStyleSheet(this->btnStyle_play);
            btnPlay->setCursor(Qt::PointingHandCursor);
            btnPlay->setProperty("mode", 0);
            connect(btnPlay, &QPushButton::clicked, this, &RoseTube_Channel::slot_btnClicked_Play);

            QLabel *label_play_img = GSCommon::getUILabelImg(ICON_PATH___playAll, btnPlay);
            label_play_img->setFixedSize(30, 30);
            label_play_img->setGeometry(32, 10, 0, 0);
            label_play_img->setStyleSheet("background-color:transparent;");

            QLabel *label_play = new QLabel(btnPlay);
            label_play->setFixedSize(41, 29);
            label_play->setGeometry(67, 11, 0, 0);
            label_play->setStyleSheet("background-color:transparent; font-size:20px; font-weight:500; color:#FFFFFF;");
            label_play->setText("Play");

            QPushButton *btnSuffle = new QPushButton(this->widget_subscription_info);
            btnSuffle->setFixedSize(150, 50);
            btnSuffle->setGeometry(395, 130, 0, 0);
            btnSuffle->setStyleSheet(this->btnStyle_shuffle);
            btnSuffle->setCursor(Qt::PointingHandCursor);
            btnSuffle->setProperty("mode", 1);
            connect(btnSuffle, &QPushButton::clicked, this, &RoseTube_Channel::slot_btnClicked_Play);

            QLabel *label_shuffle_img = GSCommon::getUILabelImg(ICON_PATH___playShuffle, btnSuffle);
            label_shuffle_img->setFixedSize(30, 30);
            label_shuffle_img->setGeometry(23, 10, 0, 0);
            label_shuffle_img->setStyleSheet("background-color:transparent;");

            QLabel *label_shuffle = new QLabel(btnSuffle);
            label_shuffle->setFixedSize(68, 29);
            label_shuffle->setGeometry(58, 11, 0, 0);
            label_shuffle->setStyleSheet("background-color:transparent; font-size:20px; font-weight:500; color:#FFFFFF;");
            label_shuffle->setText("Shuffle");

            this->btnSubscription = new QPushButton(this->widget_subscription_info);
            this->btnSubscription->setStyleSheet("background-color:transparent; border:1px solid #494949; border-radius:20px;");
            this->btnSubscription->setCursor(Qt::PointingHandCursor);
            connect(this->btnSubscription, &QPushButton::clicked, this, &RoseTube_Channel::slot_btnClicked_Subscribe);

            this->lb_subscription_img = GSCommon::getUILabelImg(ICON_PATH___subscriptionOff, this->btnSubscription);
            this->lb_subscription_img->setFixedSize(40, 40);
            this->lb_subscription_img->setGeometry(15, 0, 0, 0);
            this->lb_subscription_img->setStyleSheet("background-color:transparent; border:0px");

            this->lb_subscription = new QLabel(this->btnSubscription);
            this->lb_subscription->setFixedSize(80, 24);
            this->lb_subscription->setGeometry(55, 7, 0, 0);
            this->lb_subscription->setStyleSheet("background-color:transparent; font-size:16px; font-weight:300; color:#FFFFFF; border:0px");
            this->lb_subscription->setAlignment(Qt::AlignCenter);
            this->lb_subscription->setText(tr("Subscribe"));

            this->btnSubscription->setFixedSize(150, 40);
            this->btnSubscription->setGeometry(1350, 80, 0, 0);

            this->vBox_subcription_info = new QVBoxLayout();
            this->vBox_subcription_info->setSpacing(0);
            this->vBox_subcription_info->setContentsMargins(75, 0, 75, 0);
            this->vBox_subcription_info->addWidget(this->widget_subscription_info);

            this->flow_subscription_track = new FlowLayout(0, 0, 20);
            this->flow_subscription_track->setSizeConstraint(QLayout::SetMinimumSize);
            this->flow_subscription_track->setContentsMargins(75, 0, 75, 0);

            this->widget_subscription_content = new QWidget();
            this->widget_subscription_content->setLayout(this->flow_subscription_track);
            this->widget_subscription_content->setContentsMargins(0, 0, 0, 0);
            this->widget_subscription_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            this->widget_subscription_content->setStyleSheet("background-color:transparent;");

            this->vbox_contents = new QVBoxLayout();
            this->vbox_contents->setSpacing(0);
            this->vbox_contents->setContentsMargins(0, 0, 0, 0);
            this->vbox_contents->addWidget(this->widget_topBanner);
            this->vbox_contents->addSpacing(20);
            this->vbox_contents->addLayout(this->vBox_subcription_info);
            this->vbox_contents->addSpacing(20);
            this->vbox_contents->addWidget(this->widget_subscription_content);

            this->widget_contents = new QWidget();
            this->widget_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_contents->setLayout(this->vbox_contents);

            this->box_contents->addWidget(this->widget_contents);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
    }


    void RoseTube_Channel::proc_wheelEvent_to_getMoreData(){

        if(this->flagReqMore_video == false && (this->flow_subscription_track->count() == this->jsonArr_tracks_toPlay.size())
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_video = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // 더 가져오기 요청
            //this->ytbChannelVideo->getMoreData_youtubeChannel_video(30);
            this->setUIControl_requestGetSubscriptionTrack();
        }
    }


    void RoseTube_Channel::setUIControl_requestGetSubscriptionTrack(){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString preheader = (global.debugCHK > 3) ? "https://dev.api.roseaudio.kr/newpipe/v1/channel/" : "https://api.roseaudio.kr/newpipe/v1/channel/";
        QString url = preheader + QString("%1/more").arg(this->channelId);
        QJsonObject json = this->channelNext;

        network->request(HTTP_NETWORK_STREAM_GET
                         , url
                         , json
                         , true
                         , true);
    }


    void RoseTube_Channel::setUIControl_requestGetSubscription(){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + QString("/member/youtube/channel/%1").arg(this->channelId);
        QJsonObject json;

        network->request(HTTP_NETWORK_SUBSCRIBE_GET
                         , url
                         , json
                         , false
                         , true);
    }


    void RoseTube_Channel::setUIControl_requestSetSubscription(){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        if(this->flagSubscribe == true){

            QString url = global.legacy_v1 + QString("/member/youtube/channel");
            QJsonObject json;
            json.insert("channelId", this->channelId);

            network->request(HTTP_NETWORK_SUBSCRIBE_SET
                             , url
                             , json
                             , true
                             , true);
        }
        else if(this->flagSubscribe == false){

            QString url = global.legacy_v1 + QString("/member/youtube/channel?channelId=%1").arg(this->channelId);
            QJsonObject json;

            network->requestDelete(HTTP_NETWORK_SUBSCRIBE_SET
                                     , url
                                     , json
                                     , true);
        }
    }


    void RoseTube_Channel::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << "p_jsonObj = " << strJson;*/

        const QString jsonKey_flagOk = "flagOk";
        const QString jsonKey_subscribe = "subscribe";

        if(p_id == HTTP_NETWORK_STREAM_GET){

            if(this->jsonArr_tracks_toPlay.size() == 0){
                this->channelNext = ProcJsonEasy::getJsonObject(p_jsonObj, "nextPage");
                QString banner = ProcJsonEasy::getString(p_jsonObj, "bannerUrl");
                banner.replace("=w1060","=w1680");
                QString avatar = ProcJsonEasy::getString(p_jsonObj, "avatarUrl");
                avatar.replace("=s48","=s180");
                this->channelName = ProcJsonEasy::getString(p_jsonObj, "name");
                this->channelSubscriber = ProcJsonEasy::getInt(p_jsonObj, "subscriberCount");
                QString subscriber = QString(tr("subscribers") +" %1").arg(QLocale(QLocale::English).toString(this->channelSubscriber));

                this->bannerDownloader->setImageURL(banner);
                this->avatarDownloader->setImageURL(avatar);
                this->lb_ChannelName->setText(this->channelName);
                this->lb_Subscriber->setText(subscriber);

                QJsonArray tmpArr = ProcJsonEasy::getJsonArray(p_jsonObj, "relatedItems");
                this->slot_streamData(tmpArr);
            }
            else{
                this->channelNext = ProcJsonEasy::getJsonObject(p_jsonObj, "nextPage");
                QJsonArray tmpArr = ProcJsonEasy::getJsonArray(p_jsonObj, "relatedItems");

                this->slot_streamData(tmpArr);
            }

        }
        else if(p_id == HTTP_NETWORK_SUBSCRIBE_GET){
            if(p_jsonObj.contains(jsonKey_flagOk) && p_jsonObj[jsonKey_flagOk].toBool()){
                if(p_jsonObj.contains(jsonKey_subscribe)){
                    flagSubscribe = ProcJsonEasy::getBool(p_jsonObj, jsonKey_subscribe);

                    this->setUIControl_setSubscribeBtnStyle();
                }
            }
        }
        else if(p_id == HTTP_NETWORK_SUBSCRIBE_SET){
            //qDebug() << "response";
            this->setUIControl_setSubscribeBtnStyle();
        }
        else if(p_id == HTTP_NETWORK_CACHE){
            if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
            }
        }

        sender()->deleteLater();
    }


    void RoseTube_Channel::setUIControl_setSubscribeBtnStyle(){

        if(this->flagSubscribe == true){

            this->lb_subscription_img->setPixmap(ICON_PATH___subscriptionOn);

            this->lb_subscription->setText(tr("Subscribed"));

            this->btnSubscription->setGeometry(1350, 80, 0, 0);
            this->btnSubscription->setStyleSheet("background-color:#B18658; border:1px solid #B18658; border-radius:20px;");
        }
        else{

            this->lb_subscription_img->setPixmap(ICON_PATH___subscriptionOff);

            this->lb_subscription->setText(tr("Subscribe"));

            this->btnSubscription->setStyleSheet("background-color:#212121; border:1px solid #494949; border-radius:20px;");
        }

    }


    void RoseTube_Channel::slot_fileDownload_bannerImage(){

        QImage image;
        bool flagLoad = image.loadFromData(this->bannerDownloader->downloadedData());

        if(flagLoad){
            QPixmap pixmapIMG = QPixmap(QSize(1680, 278));
            pixmapIMG.fill(Qt::transparent);

            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(1680, 278, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, 1680, 278, 0, 0);

            painter.setClipPath(path);
            painter.drawPixmap(0, 0, tmp_pixmap);
            painter.end();

            this->lb_topBanner->setStyleSheet("background-color:#212121; border:0px;");
            this->lb_topBanner->setFixedSize(1680, 278);
            this->lb_topBanner->setPixmap(pixmapIMG);
            this->lb_topBanner->setGeometry(0, 0, 0, 0);
        }
    }


    void RoseTube_Channel::slot_fileDownload_avatarImage(){

        QImage image;
        bool flagLoad = image.loadFromData(this->avatarDownloader->downloadedData());

        if(flagLoad){
            QPixmap pixmapIMG = QPixmap(QSize(180, 180));
            pixmapIMG.fill(Qt::transparent);

            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(180, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, 180, 180, 90, 90);

            painter.setClipPath(path);
            painter.drawPixmap(0, 0, tmp_pixmap);
            painter.end();

            this->lb_Avatar->setPixmap(pixmapIMG);
            this->lb_Avatar->setGeometry(0, 0, 0, 0);
        }
    }


    void RoseTube_Channel::slot_btnClicked_Subscribe(){

        if(global.user.isValid() == true){
            this->dlgSubscribe = new DialogConfirm(this);
            this->dlgSubscribe->setTitle(tr("SUBSCRIBE"));

            if(this->flagSubscribe == true){
                this->flagSubscribe = false;

                this->dlgSubscribe->setText(tr("Are you sure you want to unsubscribe from this channel?"));
            }
            else{
                this->flagSubscribe = true;

                this->dlgSubscribe->setText(tr("Would you like to subscribe to this channel?"));
            }

            int result = this->dlgSubscribe->exec();

            if(result == 1){
                this->setUIControl_requestSetSubscription();
            }
        }
        else{
            ToastMsg::show(this, "", tr("You can use it after Login"), 3000);
        }
    }


    void RoseTube_Channel::slot_btnClicked_Play(){

        int shuffle = sender()->property("mode").toInt();

        QJsonObject jsonData;
        jsonData.insert("youtubePlayType", 15);
        jsonData.insert("shuffle", shuffle ? 1 : 0);
        jsonData.insert("roseToken", global.device.getDeviceRoseToken());
        jsonData.insert("youtube", this->jsonArr_tracks_toPlay);

        if(global.queueTimerFlag && !global.queuelist_mouse_trigger_menu_flag && global.Queue_track_count != 0) {
            emit this->linker->signal_checkQueue(27, "");

            return;
        }
        emit this->linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += this->jsonArr_tracks_toPlay.count();     // 220419 queue count

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3")
                         .arg(global.device.getDeviceIP())
                         .arg(global.port).arg("youtubePlay"), jsonData, true);
    }


    void RoseTube_Channel::slot_streamData(const QJsonArray &jsonArr){

        if(jsonArr.size() > 0){

            int start_index = this->jsonArr_tracks_tmp.size();

            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_tmp, jsonArr);

            for(int i = start_index; i < this->jsonArr_tracks_tmp.size(); i++){
                this->subscription_track[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___SUBSCRIBE, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
                connect(this->subscription_track[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTube_Channel::slot_clickedItemPlaylist);
            }

            for(int i = start_index; i < this->jsonArr_tracks_tmp.size(); i++){
                QJsonObject tmpTrack = this->jsonArr_tracks_tmp.at(i).toObject();

                QString title = ProcJsonEasy::getString(tmpTrack, "name");
                QString thumbnail = ProcJsonEasy::getString(tmpTrack, "thumbnailUrl");
                QString playUrl = ProcJsonEasy::getString(tmpTrack, "url");
                int duration = ProcJsonEasy::getInt(tmpTrack, "duration");
                int viewCount = ProcJsonEasy::getInt(tmpTrack, "viewCount");

                QStringList strListPlayUrl = playUrl.split("?v=");
                QString id = "";
                if(strListPlayUrl.size() == 2){
                    id = strListPlayUrl.at(1);
                }

                QJsonObject jsonPlayTrack;
                jsonPlayTrack.insert("channelId", this->channelId);
                jsonPlayTrack.insert("channelName", this->channelName);
                jsonPlayTrack.insert("duration", duration);
                jsonPlayTrack.insert("id", id);
                jsonPlayTrack.insert("isFavorite", false);
                jsonPlayTrack.insert("isLiveStream", false);
                jsonPlayTrack.insert("thumbnailUrl", thumbnail);
                jsonPlayTrack.insert("thumbsUpPercentage", 0);
                jsonPlayTrack.insert("title", title);
                jsonPlayTrack.insert("viewCount", viewCount);

                this->jsonArr_tracks_toPlay.append(jsonPlayTrack);

                QJsonObject jsonVideo;
                jsonVideo.insert("channelId", this->channelId);
                jsonVideo.insert("channelTitle", this->channelName);
                jsonVideo.insert("title", title);
                jsonVideo.insert("viewCount", viewCount);
                jsonVideo.insert("thumbnailUrl", thumbnail);
                jsonVideo.insert("duration", duration);

                this->subscription_track[i]->setData(jsonVideo);
                this->flow_subscription_track->addWidget(this->subscription_track[i]);

                QCoreApplication::processEvents();
            }

            if(this->channelNext.isEmpty()){
                this->flagReqMore_video = true;
                this->vbox_contents->addSpacing(30);
            }
            else{
                this->flagReqMore_video = false;
            }

            ContentLoadingwaitingMsgHide();
        }
        else{

            this->flagReqMore_video = true;
            ContentLoadingwaitingMsgHide();

            if(this->flow_subscription_track->count() == 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                noData_widget->setFixedSize(1500, 500);

                this->flow_subscription_track->addWidget(noData_widget);
            }
        }
    }


    void RoseTube_Channel::slot_applyResult_getShareLink(const QString &link){//c220818 share link

        this->shareLink = link;
    }


    void RoseTube_Channel::slot_ytbChannelVideo_getDone_avatarData(const QJsonObject &p_jsonData){

        const QString jsonKey_url = "url";
        const QString jsonkey_title = "title";
        const QString jsonKey_cntSubscriber = "cntSubscriber";

        if(p_jsonData.contains(jsonKey_url)){

            QString tmpUrl = p_jsonData[jsonKey_url].toString();

            this->avatarDownloader->setImageURL(tmpUrl);
        }
        if(p_jsonData.contains(jsonkey_title)){
            this->channelName = p_jsonData[jsonkey_title].toString();
            this->lb_ChannelName->setText(this->channelName);
        }
        if(p_jsonData.contains(jsonKey_cntSubscriber)){
            QString tmpSub = p_jsonData[jsonKey_cntSubscriber].toString();
            this->lb_Subscriber->setText(tmpSub);
        }
    }


    void RoseTube_Channel::slot_ytbChannelVideo_getDone_bannerData(const QJsonObject &p_jsonData){

        const QString jsonKey_url = "url";

        if(p_jsonData.contains(jsonKey_url)){

            QString tmp_url = p_jsonData[jsonKey_url].toString();
            if(tmp_url.startsWith("//")){
                tmp_url = "https:" + tmp_url;
            }

            this->bannerDownloader->setImageURL(tmp_url);
        }
    }


    void RoseTube_Channel::slot_ytbChannelVideo_getDone_list_videoData(const int start_index, const QJsonArray& jsonArr_video){

        if(jsonArr_video.size() > 0){

            /*QJsonDocument doc(jsonArr_video);
            QString strJson(doc.toJson(QJsonDocument::Compact));
            qDebug() << "jsonArr_video = " << strJson;*/

            if(jsonArr_video.at(0).toObject() == this->jsonArr_tracks_tmp.at(start_index).toObject()){
                this->flagReqMore_video = false;
                ContentLoadingwaitingMsgHide();

                return;
            }

            if(start_index == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
                this->jsonArr_tracks_tmp = QJsonArray();
            }
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_tmp, jsonArr_video);

            for(int i = start_index; i < this->jsonArr_tracks_tmp.size(); i++){
                this->subscription_track[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___SUBSCRIBE, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
                connect(this->subscription_track[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTube_Channel::slot_clickedItemPlaylist);
            }

            for(int i = start_index; i < this->jsonArr_tracks_tmp.size(); i++){
                QJsonObject tmpTrack = this->jsonArr_tracks_tmp.at(i).toObject();

                QString title = ProcJsonEasy::getString(tmpTrack, "title");
                QString thumbnail = ProcJsonEasy::getString(tmpTrack, "img");
                QString duration = ProcJsonEasy::getString(tmpTrack, "time");
                QString viewCount = ProcJsonEasy::getString(tmpTrack, "hit");
                QString playUrl = ProcJsonEasy::getString(tmpTrack, "url");

                QStringList strListPlayUrl = playUrl.split("?v=");
                QString id="";
                if(strListPlayUrl.size() == 2){
                    id = strListPlayUrl.at(1);
                }

                QJsonObject jsonVideo;
                jsonVideo.insert("channelId", this->channelId);
                jsonVideo.insert("channelTitle", this->channelName);
                jsonVideo.insert("title", title);
                jsonVideo.insert("viewCount", viewCount);
                jsonVideo.insert("thumbnailUrl", thumbnail);
                jsonVideo.insert("duration", duration);
                jsonVideo.insert("subs_type", "subscribe");

                this->subscription_track[i]->setData(jsonVideo);
                this->flow_subscription_track->addWidget(this->subscription_track[i]);

                QJsonObject jsonTracks = QJsonObject();
                jsonTracks.insert("channelId", this->channelId);
                jsonTracks.insert("channelName", this->channelName);
                jsonTracks.insert("duration", duration);
                jsonTracks.insert("id", id);
                jsonTracks.insert("isFavorite", false);
                jsonTracks.insert("isLiveStream", false);
                jsonTracks.insert("thumbnailUrl", thumbnail);
                jsonTracks.insert("thumbsUpPercentage", 0);
                jsonTracks.insert("title", title);
                jsonTracks.insert("viewCount", title);

                this->jsonArr_tracks_toPlay.append(jsonTracks);

                QCoreApplication::processEvents();
            }

            this->flagReqMore_video = false;
            ContentLoadingwaitingMsgHide();
        }
        else{

            this->flagReqMore_video = true;
            ContentLoadingwaitingMsgHide();

            if(this->flow_subscription_track->count() == 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
                noData_widget->setFixedSize(1500, 500);

                this->flow_subscription_track->addWidget(noData_widget);
            }
        }
    }


    void RoseTube_Channel::slot_ytbChannelVideo_noMoreData_list_video(){

        this->flagReqMore_video = true;

        if(this->flow_subscription_track->count() == 0){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Rosetube_NoData);
            noData_widget->setFixedSize(1500, 500);

            this->flow_subscription_track->addWidget(noData_widget);
        }
        else{
            this->vbox_contents->addSpacing(30);
        }

        ContentLoadingwaitingMsgHide();
    }


    void RoseTube_Channel::slot_ytbChannelVideo_occurError(YoutubeChannelVideo::ErrorType errType){

        Q_UNUSED(errType);
        ContentLoadingwaitingMsgHide();
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RoseTube_Channel::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___SUBSCRIBE){
            if(clickMode == tidal::AbstractItem::ClickMode::AllBox || clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Track 바로 재생 - clicked 항목만 재생
                int curr_clickMode = global.device.getMusicPlayType();

                OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                switch(curr_clickMode)
                {
                case 13:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                    break;
                case 12:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                    break;
                case 15:
                    playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    break;
                case 16:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                    break;
                case 17:
                    playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                    break;
                case 18:
                    playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                    break;
                case 19:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                    break;
                }

                QJsonArray jsonArr_toPlayReal = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_tracks_toPlay, index, playType);

                QJsonObject jsonData;
                jsonData.insert("youtubePlayType", curr_clickMode);
                jsonData.insert("shuffle", 0);
                jsonData.insert("roseToken", global.device.getDeviceRoseToken());
                jsonData.insert("youtube", jsonArr_toPlayReal);

                if(global.queueTimerFlag && !global.queuelist_mouse_trigger_menu_flag && global.Queue_track_count != 0) {
                    emit this->linker->signal_checkQueue(27, "");

                    return;
                }
                emit this->linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += this->jsonArr_tracks_toPlay.count();     // 220419 queue count

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3")
                                 .arg(global.device.getDeviceIP())
                                 .arg(global.port).arg("youtubePlay"), jsonData, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(index).toObject();

                OptMorePopup::HeaderData data_header;

                if(tmpObj.contains("data") && tmpObj.contains("snippet")){
                    QJsonObject data = ProcJsonEasy::getJsonObject(tmpObj, "data");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(data, "snippet");


                    data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                    data_header.sub_title = ProcJsonEasy::getString(snippet, "channelTitle");
                    data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                    data_header.data_pk = "https://youtu.be/" + QString("%1").arg(ProcJsonEasy::getString(tmpObj, "id"));  //j220906 share link
                    data_header.type = "YOUTUBE";      //j220906 share link
                    data_header.flagProcStar = false;
                    data_header.isShare = true;      //j220906 share link
                }
                else{
                    data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                    data_header.sub_title = ProcJsonEasy::getString(tmpObj, "channelName");
                    data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                    data_header.data_pk = "https://youtu.be/" + QString("%1").arg(ProcJsonEasy::getString(tmpObj, "id"));  //j220906 share link
                    data_header.type = "YOUTUBE";      //j220906 share link
                    data_header.flagProcStar = false;
                    data_header.isShare = true;      //j220906 share link
                }


                // OptMorePopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosetube_Video, data_header, index, section);
            }
        }
    }


    void RoseTube_Channel::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___SUBSCRIBE){

            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last

                    )
            {
                int curr_clickMode = this->get_rose_playType(clickMode);

                QJsonArray jsonArr_toPlayReal = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_tracks_toPlay, index, clickMode);

                QJsonObject jsonData;
                jsonData.insert("youtubePlayType", curr_clickMode);
                jsonData.insert("shuffle", 0);
                jsonData.insert("roseToken", global.device.getDeviceRoseToken());
                jsonData.insert("youtube", jsonArr_toPlayReal);

                if(global.queueTimerFlag && !global.queuelist_mouse_trigger_menu_flag && global.Queue_track_count != 0) {
                    emit this->linker->signal_checkQueue(27, "");

                    return;
                }
                emit this->linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArr_toPlayReal.count();     // 220419 queue count

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3")
                                 .arg(global.device.getDeviceIP())
                                 .arg(global.port)
                                 .arg("youtubePlay"), jsonData, true);
            }
            else if(clickMode == OptMorePopup::ClickMode::Share){//c220818
                print_debug();//c220818-3dot
                setUIShare();//c220818
                print_debug();
                qDebug() << "this->shareLink="<<this->shareLink;//c220818

            }
            else if(clickMode == OptMorePopup::ClickMode::Caching_ahead){
                QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(index).toObject();

                QString channelId = "";
                QString channelName = "";
                QString duration = "";
                QString id = "";
                QString thumbnailUrl = "";
                QString title = "";

                if(trackInfo.contains("data")){
                    QJsonObject trackData = ProcJsonEasy::getJsonObject(trackInfo, "data");
                    QJsonObject detailContents = ProcJsonEasy::getJsonObject(trackData, "detailContents");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(trackData, "snippet");
                    QJsonObject trackStatistic = ProcJsonEasy::getJsonObject(trackData, "detailContents");

                    QString tmpDuration = ProcJsonEasy::getString(detailContents, "duration");

                    QString time = tmpDuration;
                    QString hour;
                    QString minute;
                    QString second;

                    if(tmpDuration.indexOf("PT") >= 0){
                        time = tmpDuration.right(tmpDuration.size() - 2);
                    }
                    if(time.indexOf("H") >= 0){
                        hour = time.section("H", 0, -2);
                        time = time.mid(time.indexOf("H") + 1);
                    }
                    if(time.indexOf("M") >= 0){
                        minute = time.section("M", 0, -2);
                        time = time.mid(time.indexOf("M") + 1);
                    }
                    if(time.indexOf("S") >= 0){
                        second = time.section("S", 0, -2);
                    }

                    if(hour.isEmpty()){
                        if(minute.isEmpty()){
                            duration = "0:" + second.rightJustified(2, '0');
                        }
                        else{
                            duration = minute + ":" + second.rightJustified(2, '0');
                        }
                    }
                    else{
                        duration = hour + ":" + minute.rightJustified(2, '0') + ":" + second.rightJustified(2, '0');
                    }

                    channelId = ProcJsonEasy::getString(snippet, "channelId");
                    channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                    id = ProcJsonEasy::getString(trackInfo, "clientKey");
                    thumbnailUrl = ProcJsonEasy::getString(trackInfo, "thumbnailUrl");
                    title = ProcJsonEasy::getString(trackInfo, "title");
                }
                else{
                    if(trackInfo.contains("channelTitle")){
                        channelName = ProcJsonEasy::getString(trackInfo, "channelTitle");
                    }
                    else if(trackInfo.contains("channelName")){
                        channelName = ProcJsonEasy::getString(trackInfo, "channelName");
                    }

                    int time = ProcJsonEasy::getInt(trackInfo, "duration");
                    if(time == 0){
                        QString tmpDur = ProcJsonEasy::getString(trackInfo, "duration");

                        if(tmpDur.isEmpty()){
                            duration = "";
                        }
                        else{
                            duration = tmpDur;
                        }
                    }
                    else if(time >= 3600){
                         duration = QDateTime::fromTime_t(time).toUTC().toString("hh:mm:ss");
                    }
                    else{
                        duration = QDateTime::fromTime_t(time).toUTC().toString("mm:ss");
                    }

                    channelId = ProcJsonEasy::getString(trackInfo, "channelId");
                    id = ProcJsonEasy::getString(trackInfo, "id");
                    thumbnailUrl = ProcJsonEasy::getString(trackInfo, "thumbnailUrl");
                    title = ProcJsonEasy::getString(trackInfo, "title");
                }

                QJsonObject data;
                data.insert("channelId", channelId);
                data.insert("channelName", channelName);
                data.insert("duration", duration);
                data.insert("id", id);
                data.insert("isFavorite", false);
                data.insert("isLiveStream", false);
                data.insert("thumbnailUrl", thumbnailUrl);
                data.insert("thumbsUpPercentage", 0);
                data.insert("title", title);
                data.insert("viewsCount", "-1");

                QJsonObject json;
                json.insert("data", data);
                json.insert("isDelete", false);

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_download_item";
                network->request(HTTP_NETWORK_CACHE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
        }
    }


    int RoseTube_Channel::get_rose_playType(OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::Play_RightNow_withReorder
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
                ){
            // 바로 재생을 요청하는 타입
            return 15;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
                ){
            // 현재 Queue 끝에 추가
            return 13;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
                ){
            // 큐를 비우고 추가(재생)
            return 12;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
                ){
            return 16;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere){
            // 여기서부터 재생
            return 17;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty){
            // 큐 비우고 여기서부터 재생
            return 18;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last){
            // 여기서부터 큐 끝에 추가
            return 19;
        }
        else{
            return 0;           // unknown or not supported
        }
    }


    QJsonArray RoseTube_Channel::get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Play_RightNow_withReorder
                || clickMode == OptMorePopup::ClickMode::Play_RightNow
        ){
            if(curr_index > 0){
                // 전체이지만 re-ordering
                QJsonArray jsonArr_output = this->reorderJsonArray(jsonArr_toPlayAll, curr_index);
                return jsonArr_output;
            }
            else{
                return jsonArr_toPlayAll;
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
        ){
            // 1곡만
            QJsonArray jsonArr_output;
            jsonArr_output.append(jsonArr_toPlayAll.at(curr_index).toObject());
            return jsonArr_output;
        }
        else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last
        ){
            // index ~ 마지막까지 구성
            if(curr_index > 0){
                QJsonArray jsonArr_output = this->subRange_JsonArray(jsonArr_toPlayAll, curr_index);
                return jsonArr_output;
            }
            else{
                return jsonArr_toPlayAll;
            }
        }
        else{
            // 그 이외의 경우
            return jsonArr_toPlayAll;
        }

    }


    /**
     * @brief AbstractProcRosePlay::subRange_JsonArray
     * @param p_jsonArr
     * @param startIndex
     * @return
     */
    QJsonArray RoseTube_Channel::subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex){

        const int len_data = p_jsonArr.count();

        if(startIndex > 0 && startIndex < len_data){
            QJsonArray jsonArr_subRange;
            for(int i = startIndex; i < len_data; i++){
                jsonArr_subRange.append(p_jsonArr.at(i));
            }
            return jsonArr_subRange;
        }
        else{
            // 유효하지 않는 경우, 그대로 반환
            return p_jsonArr;
        }
    }


    /**
     * @brief AbstractProcRosePlay::reorderJsonArray
     * @param p_jsonArr
     * @param startIndex
     * @return
     */
    QJsonArray RoseTube_Channel::reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex){

        const int len_data = p_jsonArr.count();

        if(startIndex > 0 && startIndex < len_data){
            QJsonArray jsonArr_reorder;
            for(int i = startIndex; i < len_data; i++){
                jsonArr_reorder.append(p_jsonArr.at(i));
            }
            for(int i = 0; i < startIndex; i++){
                jsonArr_reorder.append(p_jsonArr.at(i));
            }
            return jsonArr_reorder;
        }
        else{
            // 유효하지 않는 경우, 그대로 반환
            return p_jsonArr;
        }
    }
}
