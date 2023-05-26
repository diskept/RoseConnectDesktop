#include "video/videodetail.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"
#include "widget/framewidget.h"
#include "widget/toastmsg.h"

#include <QScrollBar>


namespace video {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___tracks = 2;

    const int HTTP_NETWORK_PLAY_ALL = 99;


    /**
     * @brief VideoDetail::VideoDetail : 비디오-상세
     * @details 비디오 폴더 영상 정보 화면<br>
     * 비디오 전체 재생 및 각 비디오 영상 재생 가능
     * @param parent
     */
    VideoDetail::VideoDetail(QWidget *parent): roseHome::AbstractRoseHomeSubWidget(VerticalScroll_viewAll, parent)
    {

    }


    /**
     * @brief VideoDetail::~VideoDetail : 소멸자
     */
    VideoDetail::~VideoDetail(){

        this->deleteLater();
    }


    /**
     * @brief VideoDetail::setJsonObject_forData
     * @param jsonObj
     */
    void VideoDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        QString tmpTitle = ProcJsonEasy::getString(jsonObj, "folderName");
        QString tmpImage = ProcJsonEasy::getString(jsonObj, "coverImg");
        QJsonArray tmpTrackArr = ProcJsonEasy::getJsonArray(jsonObj, "arr_video");
        this->flagNeedReload = false;

        if((tmpTitle != this->title) && (tmpTrackArr.size() != this->jsonArr_tracks_toPlay.size())){
            this->flagNeedReload = true;

            this->title = tmpTitle;
            this->imagePath = tmpImage;

            this->jsonArr_tracks_toPlay = tmpTrackArr;
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief VideoDetail 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void VideoDetail::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            if(this->flag_track_ok == true){
                this->widget_main_contents->hide();

                GSCommon::clearLayout(this->box_main_contents);
                this->box_contents->removeWidget(this->widget_main_contents);

                this->flag_track_ok = false;
            }

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // Album 상세정보
            this->video_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::Video);
            connect(this->video_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &VideoDetail::slot_detailInfo_btnClicked);

            this->video_detail_info->initView();

            this->box_main_contents->addWidget(this->video_detail_info);
            this->box_main_contents->addSpacing(50);

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->slot_applyResult_videoAlbum();
            this->slot_applyResult_videoTrack();
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : [Slot]
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void VideoDetail::slot_applyResult_videoAlbum(){

        for(int i = 0; i < this->jsonArr_tracks_toPlay.size(); i++){

            QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(i).toObject();

            QString strDuration = ProcJsonEasy::getString(tmpObj, "videoDuration");

            QRegExp regex("\\d*");
            if(!strDuration.isEmpty() && regex.exactMatch(strDuration)){
                this->duration += (strDuration.toInt() / 1000);
            }else{
                this->duration = 0;
            }
        }

        QJsonObject videoObj;
        videoObj.insert("image", this->imagePath);
        videoObj.insert("title", this->title);
        videoObj.insert("track_count", this->jsonArr_tracks_toPlay.size());
        videoObj.insert("duration", this->duration);

        this->video_detail_info->setData_fromVideoData(videoObj);
    }


    void VideoDetail::slot_applyResult_videoTrack(){

        int maxCnt = this->jsonArr_tracks_toPlay.size();

        for(int i = 0; i < maxCnt; i++){
            this->video_track_info[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->video_track_info[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &VideoDetail::slot_clickedItemTrack_inList);
            this->video_track_info[i]->setProperty("index", i);

            this->box_main_contents->addWidget(this->video_track_info[i]);
        }

        for(int i = 0; i < maxCnt; i++){
            QJsonObject tmpTrack = this->jsonArr_tracks_toPlay.at(i).toObject();

            QString strDuration = ProcJsonEasy::getString(tmpTrack, "videoDuration");
            int sndDuration = 0;

            QRegExp regex("\\d*");
            if(!strDuration.isEmpty() && regex.exactMatch(strDuration)){
                sndDuration = (strDuration.toInt() / 1000);
            }

            double sndVideoSize = ((ProcJsonEasy::getDouble(tmpTrack, "videoSize") / 1024) / 1024);

            QJsonObject tmpData;
            tmpData.insert("thumbnail", this->imagePath);
            tmpData.insert("title", ProcJsonEasy::getString(tmpTrack, "videoTitle"));
            tmpData.insert("duration", sndDuration);
            tmpData.insert("fileSize", sndVideoSize);

            this->video_track_info[i]->setDataTrackInfo_Video(tmpData);

            QCoreApplication::processEvents();
        }

        this->box_main_contents->addSpacing(50);

        ContentLoadingwaitingMsgHide();
        this->flag_track_ok = true;
    }


    /**
     * @brief VideoHome::slot_responseHttp : 로즈 장치 정보요청 결과 처리
     * @param p_jsonObject QJsonObject&
     */
    void VideoDetail::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

        switch(p_id){
            case HTTP_NETWORK_PLAY_ALL:
                if(p_jsonObject.contains("flagOk") && p_jsonObject["flagOk"].toBool() == true){

                }
                break;
        }

        sender()->deleteLater();
    }


    /**
     * @brief [Slot] PlaylistImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void VideoDetail::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

        if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayAll){
            // 전체재생
            QJsonArray jsonArrTracks = QJsonArray();

            for(int i = 0; i < this->jsonArr_tracks_toPlay.size(); i++){
                QJsonObject tmpTrack = this->jsonArr_tracks_toPlay.at(i).toObject();

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
        else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayShuffle){

            int maxNumber = this->jsonArr_tracks_toPlay.count();

            int tmp = 0;
            int save[99999] = {};
            int count = 0;
            int isSame = 0;
            srand(time(0));

            while (count < maxNumber) {
                isSame = 0;
                tmp = rand() % maxNumber;
                for (int i = 0; i < count; i++) {
                    if (tmp == save[i]) {
                        isSame = 1;
                        break;
                    }
                }
                if (isSame == 0) {
                    save[count] = tmp;
                    count++;
                }
            }

            QJsonArray jsonArr_randomTracks_toPlay;
            for(int i = 0; i < this->jsonArr_tracks_toPlay.count(); i++){
                int idx = save[i];
                QJsonObject tmp_json = this->jsonArr_tracks_toPlay.at(idx).toObject();

                jsonArr_randomTracks_toPlay.append(tmp_json);
            }

            // 셔플재생
            QJsonArray jsonArrTracks = QJsonArray();

            for(int i = 0; i < jsonArr_randomTracks_toPlay.size(); i++){
                QJsonObject tmpTrack = jsonArr_randomTracks_toPlay.at(i).toObject();

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
    }


    /**
     * @brief [Slot] AlbumImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void VideoDetail::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        // ClickMode 별로 처리
        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

        }
        else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AllBox){

            // Track 바로 재생
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

            QJsonArray jsonArr_toPlayReal = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_tracks_toPlay, idx, playType);

            QJsonArray jsonArrTracks = QJsonArray();

            for(int i = 0; i < jsonArr_toPlayReal.size(); i++){
                QJsonObject tmpTrack = jsonArr_toPlayReal.at(i).toObject();

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
            jsonParam.insert("musicPlayType", global.device.getMusicPlayType());
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
        else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
            // 플레이리스트 담기 - Track
            //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
        }
        else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){

            QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

            OptMorePopup::HeaderData data_header;
            data_header.main_title = ProcJsonEasy::getString(tmpObj, "videoTitle");
            data_header.sub_title = "";
            data_header.imageUrl = this->imagePath;
            data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(tmpObj, "id"));
            data_header.type = "VIDEO";      //j220906 share link
            data_header.flagProcStar = false;
            data_header.isShare = false;      //j220906 share link

            // OptMorePopup 띄우기 필요
            this->makeObj_optMorePopup(OptMorePopup::Video_Track, data_header, idx, SECTION_FOR_MORE_POPUP___tracks);
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void VideoDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___tracks){
            //this->proc_clicked_optMorePopup_fromPlaylist(this->data_playlist, index, section, clickMode);

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
                // Rose Play 요청
                QJsonArray jsonArr_toPlayReal = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_tracks_toPlay, index, clickMode);

                QJsonArray jsonArrTracks = QJsonArray();

                for(int i = 0; i < jsonArr_toPlayReal.size(); i++){
                    QJsonObject tmpTrack = jsonArr_toPlayReal.at(i).toObject();

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
                jsonParam.insert("musicPlayType", global.device.getMusicPlayType());
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
            else if(clickMode == OptMorePopup::ClickMode::Share){//c220903_2
                setUIShare();
            }
        }
    }


    QJsonArray VideoDetail::get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode){

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
    QJsonArray VideoDetail::subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex){

        const int len_data = p_jsonArr.count();
        if(startIndex>0 && startIndex<len_data){
            QJsonArray jsonArr_subRange;
            for(int i=startIndex ; i<len_data ; i++){
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
    QJsonArray VideoDetail::reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex){

        const int len_data = p_jsonArr.count();

        if(startIndex>0 && startIndex<len_data){
            QJsonArray jsonArr_reorder;
            for(int i=startIndex ; i<len_data ; i++){
                jsonArr_reorder.append(p_jsonArr.at(i));
            }
            for(int i=0 ; i<startIndex ; i++){
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
