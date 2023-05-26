#include "video/videohome.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"//c220729
#include "common/ProcJsonEasy.h"

#include "widget/toastmsg.h"//c220729

#include <QScrollBar>


namespace video {

    const int SECTION_FOR_MORE_POPUP___videos = 0;

    const int HTTP_VIDEO_GET_TOTAL = 10;
    const int HTTP_VIDEO_HOME_LIST = 11;
    const int HTTP_NETWORK_PLAY_ALL = 12;

    VideoHome::VideoHome(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_roseviewAll, parent)
    {
        global.isDrawingMainContent = false;

        linker = Linker::getInstance();

        // 기본 UI 세팅
        this->setUIControl_video();
    }


    VideoHome::~VideoHome(){

        this->deleteLater();
    }


    void VideoHome::setDataFromRose(){

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
        if(this->flag_flow_draw == false){
            NetworkHttp *network = new NetworkHttp(this);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QJsonObject json;
            json.insert("roseToken", global.device.getDeviceRoseToken());
            json.insert("totalCnt", "-1");

            QString url = QString("http://%1:").arg(global.device.getDeviceIP()) + QString("%1/video_data").arg(global.port);
            network->request(HTTP_VIDEO_GET_TOTAL, url, json, true);
        }
        print_debug();ContentLoadingwaitingMsgHide();//c230322_3
    }


    void VideoHome::setUIControl_video(){

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
        if(this->flag_flow_draw == false){
            // init
            this->curr_page = 0;
            this->video_total_cnt = 0;
            this->video_total_page = 0;
            this->video_draw_cnt = 0;

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            video::ItemWidget_video *listVideo = new video::ItemWidget_video(0, 0, tidal::AbstractItem::ImageSizeMode::Ractangle_200x281, true);

            this->video_widget_width = listVideo->get_fixedWidth();
            this->video_widget_margin = listVideo->get_rightMargin();

            //qDebug() << "[Debug] RoseHomeAlbumListAll::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

            this->flowLayout_videos = this->get_addUIControl_flowLayout(0, 20);

            GSCommon::clearLayout(this->flowLayout_videos);
        }
        print_debug();ContentLoadingwaitingMsgHide();//c230322_3
    }


    /**
     * @brief VideoHome::slot_responseHttp : 로즈 장치 정보요청 결과 처리
     * @param p_jsonObject QJsonObject&
     */
    void VideoHome::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

        int f_width = this->flowLayout_videos->geometry().width();

        if(this->flowLayout_videos->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->video_widget_width + this->video_widget_margin);
        int f_mod = this->video_draw_cnt % f_wg_cnt;

        if(f_mod == 0){
            this->video_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->video_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        switch(p_id){
            case HTTP_VIDEO_GET_TOTAL:
                if(p_jsonObject.contains("flagOk") && p_jsonObject["flagOk"].toBool() == true){
                    if(p_jsonObject.contains("data")){
                        QJsonObject tmp_jsonData = p_jsonObject["data"].toObject();
                        if(tmp_jsonData.contains("mArr")){
                            QJsonArray tmp_arr = tmp_jsonData["mArr"].toArray();

                            this->curr_page = 1;

                            this->video_total_cnt = tmp_arr.count();
                            this->video_total_page = (this->video_total_cnt / 200) + 1;

                            NetworkHttp *network = new NetworkHttp(this);
                            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                            QJsonObject json;
                            json.insert("paging", QString("%1").arg(this->curr_page));
                            json.insert("pagingCnt", QString("%1").arg(200));
                            json.insert("roseToken", global.device.getDeviceRoseToken());
                            json.insert("totalCnt", QString("%1").arg(this->video_total_cnt));

                            QString url = QString("http://%1:").arg(global.device.getDeviceIP()) + QString("%1/video_data").arg(global.port);
                            network->request(HTTP_VIDEO_HOME_LIST, url, json, true);

                            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                        }
                    }
                }
                break;

            case HTTP_VIDEO_HOME_LIST:
                if(p_jsonObject.contains("flagOk") && p_jsonObject["flagOk"].toBool() == true){
                    if(p_jsonObject.contains("data")){
                        if(this->video_draw_cnt <= 0){
                            QJsonObject tmp_jsonData = ProcJsonEasy::getJsonObject(p_jsonObject, "data");
                            if(tmp_jsonData.contains("mArr")){
                                QJsonArray tmp_arr = ProcJsonEasy::getJsonArray(tmp_jsonData, "mArr");

                                this->jsonArr_tracks_toPlay = QJsonArray();
                                this->jsonArr_tracks_toPlay = tmp_arr;

                                int start_index = this->video_draw_cnt;
                                int max_cnt = ((this->video_total_cnt - this->video_draw_cnt) > this->video_widget_cnt ) ? this->video_widget_cnt : (this->video_total_cnt - this->video_draw_cnt);
                                this->video_draw_cnt += max_cnt;

                                for(int i = start_index; i < this->video_draw_cnt; i++){
                                    this->list_video[i] = new video::ItemWidget_video(i, SECTION_FOR_MORE_POPUP___videos, tidal::AbstractItem::ImageSizeMode::Ractangle_200x281, true);
                                    QCoreApplication::processEvents();
                                }

                                for(int i = start_index; i < this->video_draw_cnt; i++){
                                    this->list_video[i]->setData(this->jsonArr_tracks_toPlay.at(i).toObject());
                                }


                                for(int i = start_index; i < this->video_draw_cnt; i++){
                                    connect(this->list_video[i], &video::ItemWidget_video::signal_clicked, this, &VideoHome::slot_clickedItemVideo);
                                    flowLayout_videos->addWidget(this->list_video[i]);
                                }
                                this->setFlowLayoutResize(this, this->flowLayout_videos, this->video_widget_width, this->video_widget_margin);
                            }

                            this->flag_flow_draw = true;
                            global.isDrawingMainContent = true;
                            ContentLoadingwaitingMsgHide();
                        }
                        else{
                            QJsonObject tmp_jsonData = ProcJsonEasy::getJsonObject(p_jsonObject, "data");
                            if(tmp_jsonData.contains("mArr")){
                                QJsonArray tmp_arr = ProcJsonEasy::getJsonArray(tmp_jsonData, "mArr");

                                ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, tmp_arr);
                            }

                            this->flag_flow_draw = true;
                            ContentLoadingwaitingMsgHide();
                        }

                        if(this->video_total_cnt > this->video_draw_cnt){
                            this->curr_page++;
                            this->video_total_page = (this->video_total_cnt / 200) + 1;

                            NetworkHttp *network = new NetworkHttp(this);
                            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                            QJsonObject json;
                            json.insert("paging", QString("%1").arg(this->curr_page));
                            json.insert("pagingCnt", QString("%1").arg(200));
                            json.insert("roseToken", global.device.getDeviceRoseToken());
                            json.insert("totalCnt", QString("%1").arg(this->video_total_cnt));

                            QString url = QString("http://%1:").arg(global.device.getDeviceIP()) + QString("%1/video_data").arg(global.port);
                            network->request(HTTP_VIDEO_HOME_LIST, url, json, true);
                        }
                    }
                }
                break;

            case HTTP_NETWORK_PLAY_ALL:
                if(p_jsonObject.contains("flagOk") && p_jsonObject["flagOk"].toBool() == true){

                }
                break;
        }

        sender()->deleteLater();
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void VideoHome::proc_wheelEvent_to_getMoreData(){

        if((this->video_total_cnt > this->video_draw_cnt) && (this->flag_video_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_video_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_videoDraw();
        }
    }


    void VideoHome::request_more_videoDraw(){

        int f_width = this->flowLayout_videos->geometry().width();

        if(this->flowLayout_videos->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->video_widget_width + this->video_widget_margin);
        int f_mod = this->video_draw_cnt % f_wg_cnt;

        if(f_mod == 0){
            this->video_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->video_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        //qDebug() << "[Debug] RoseHomeAlbumListAll::request_more_albumDraw " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

        int start_index = this->video_draw_cnt;
        int max_cnt = ((this->video_total_cnt - this->video_draw_cnt) > this->video_widget_cnt ) ? this->video_widget_cnt : (this->video_total_cnt - this->video_draw_cnt);
        this->video_draw_cnt += max_cnt;

        for(int i = start_index; i < this->video_draw_cnt; i++){
            this->list_video[i] = new video::ItemWidget_video(i, SECTION_FOR_MORE_POPUP___videos, tidal::AbstractItem::ImageSizeMode::Ractangle_200x281, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->video_draw_cnt; i++){
            this->list_video[i]->setData(this->jsonArr_tracks_toPlay.at(i).toObject());
        }

        for(int i = start_index; i < this->video_draw_cnt; i++){
            connect(this->list_video[i], &video::ItemWidget_video::signal_clicked, this, &VideoHome::slot_clickedItemVideo);
            flowLayout_videos->addWidget(this->list_video[i]);
        }

        ContentLoadingwaitingMsgHide();

        this->flag_video_draw = false;
    }


    void VideoHome::slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        QJsonObject tmpObj = this->list_video[index]->get_videoInfo();

        if(section == SECTION_FOR_MORE_POPUP___videos){
            // ClickMode 별로 처리
            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){

                QJsonArray tmpArr = ProcJsonEasy::getJsonArray(tmpObj, "arr_video");

                QJsonArray jsonArrTracks = QJsonArray();

                for(int i = 0; i < tmpArr.size(); i++){
                    QJsonObject tmpTrack = tmpArr.at(i).toObject();

                    QString video_title = ProcJsonEasy::getString(tmpTrack, "videoTitle");
                    QString video_filePath = ProcJsonEasy::getString(tmpTrack, "videoFilePath");
                    QString video_duration = ProcJsonEasy::getString(tmpTrack, "videoDuration");

                    int video_id = ProcJsonEasy::getInt(tmpTrack, "id");

                    QJsonObject jsonTrack;
                    jsonTrack.insert("albumId", "");
                    jsonTrack.insert("albumKey", "");
                    jsonTrack.insert("artist", "");
                    jsonTrack.insert("duration", video_duration);
                    jsonTrack.insert("ext", "");
                    jsonTrack.insert("id", QString("%1").arg(video_id));
                    jsonTrack.insert("ip", "");
                    jsonTrack.insert("isCloud", false);
                    jsonTrack.insert("isDir", false);
                    jsonTrack.insert("isFile", true);
                    jsonTrack.insert("isNetworkAdd", false);
                    jsonTrack.insert("isServer", false);
                    jsonTrack.insert("isShare", false);
                    jsonTrack.insert("isThumbNailGetting", false);
                    jsonTrack.insert("isUp", false);
                    jsonTrack.insert("name", video_title);
                    jsonTrack.insert("path", video_filePath);
                    jsonTrack.insert("pw", "");
                    jsonTrack.insert("rcloneServe", "");
                    jsonTrack.insert("smbParamPath", "");
                    jsonTrack.insert("thumbnail", "");
                    jsonTrack.insert("title", "");

                    jsonArrTracks.append(jsonTrack);
                }

                QJsonObject jsonParam;
                jsonParam.insert("musicPlayType", 15);
                jsonParam.insert("music", jsonArrTracks);
                jsonParam.insert("playType", 21);
                jsonParam.insert("currentPosition", 0);
                jsonParam.insert("musicType", 11);
                jsonParam.insert("roseToken", global.device.getDeviceRoseToken());


                QString url = QString("http://%1:").arg(global.device.getDeviceIP()) + QString("%1/external_usb").arg(global.port);

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(HTTP_NETWORK_PLAY_ALL, url, jsonParam, true, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                QJsonObject jsonVideo;
                jsonVideo.insert("coverImg", ProcJsonEasy::getString(tmpObj, "coverImg"));
                jsonVideo.insert("folderName", ProcJsonEasy::getString(tmpObj, "folderName"));
                jsonVideo.insert("arr_video", ProcJsonEasy::getJsonArray(tmpObj, "arr_video"));

                QJsonObject jsonData;
                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_VIDEOINFO);
                jsonData.insert("video_info", jsonVideo);

                emit linker->signal_clickedMovePage(jsonData);
            }
        }
    }
    /**
     * @brief RoseHomeAlbumListAll::resizeEvent
     * @param event
     */
    void VideoHome::resizeEvent(QResizeEvent *event){//c230223

        Q_UNUSED(event);

        this->setFlowLayoutResize(this, this->flowLayout_videos, this->video_widget_width, this->video_widget_margin);
    }
}
