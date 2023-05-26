#include "rosetube/rosetube_Search.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "login/dialoglogin.h"
#include "login/dialog_playlist_onRose.h"

#include "widget/NoData_Widget.h"
#include "widget/toastmsg.h"//c220727

#include "roseHome/ProcCommon_forRosehome.h"//c220818

#include <QScrollBar>


namespace rosetube {

    const QString tmp_btnStyle      = "padding:8px;border:1px solid #707070;color:#CCCCCC;font-size:16px;";//cheon211008
    //const QString tmp_btnStyleHover = "background-color:#B18658;color:#FFFFFF;";//cheon211115-01
    const QString tmp_btnStyleHover = "background-color:#CCCCCC;color:#FFFFFF;";//cheon211115-01
    const QString tmp_btnStyleHover_selected = "background-color:#B18658;color:#FFFFFF;";//cheon211115-01


    const QString SORT_STEP_MUSIC = "music";//cheon211008
    const QString SORT_STEP_RoseTube = "rosetube";//cheon211008
    const QString SORT_STEP_RoseRadio = "roseradio";//cheon211008
    const QString SORT_STEP_TIDAL = "tidal";//cheon211008
    const QString SORT_STEP_BUGS = "bugs";//cheon211008
    const QString SORT_STEP_QOBUZ = "qobuz";//cheon211008

    const int HTTP_NETWORK_PLAY = 99;
    const int HTTP_NETWORK_CHANNEL_LIST = 98;
    const int HTTP_NETWORK_CHANNEL_LIST_MORE = 97;
    const int HTTP_NETWORK_CACHE = 96;

    const int SECTION_FOR_MORE_POPUP___SEARCH = 0;

    /**
     * @brief RoseTube_Search::RoseTube_Search : 로즈튜브 검색 결과 화면
     * @param parent
     */
    RoseTube_Search::RoseTube_Search(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_viewAll, parent) {

        this->linker = Linker::getInstance();//c220727

        // 기본 UI 세팅
        this->setUIControl_basic();
    }


    RoseTube_Search::~RoseTube_Search(){

        this->deleteLater();
    }


    /**
     * @brief RoseTube_Search::search : 로즈튜브 검색
     * @param p_text
     */
    void RoseTube_Search::setJsonObject_forData(const QJsonObject& jsonObj){//c220923_1

        common::SearchData tmp_data = common::ConvertData_common::convertData_searchData(jsonObj);
        this->flagNeedReload = false;

        if(tmp_data.search_word != this->data_search.search_word){
            this->flagNeedReload = true;

            this->data_search = common::ConvertData_common::convertData_searchData(jsonObj);

            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + tr("Search result"));

            this->jsonArr_tracks_toPlay = QJsonArray();
            this->jsonArr_tracks_toTemp = QJsonArray();

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QString preheader = global.newpipe_v1 + "/search?query=";
            QString url = preheader + QString("%1").arg(this->data_search.search_word);
            QJsonObject json;
            network->request(HTTP_NETWORK_CHANNEL_LIST
                             , url
                             , json
                             , false
                             , true);
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    void RoseTube_Search::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // j230407 Removing ghosting effect start
            if(this->widget_contents != nullptr){
                this->widget_contents->hide();
                this->box_contents->removeWidget(this->widget_contents);

                delete this->widget_contents;
            }
            // j230407 Removing ghosting effect finish

            this->box_contents->removeWidget(this->widget_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->flowLayout_search_track = new FlowLayout(0, 0, 20);
            this->flowLayout_search_track->setSizeConstraint(QLayout::SetMinimumSize);
            this->flowLayout_search_track->setContentsMargins(0, 20, 0, 0);

            this->widget_search_content = new QWidget();
            this->widget_search_content->setLayout(this->flowLayout_search_track);
            this->widget_search_content->setContentsMargins(0, 0, 0, 0);
            this->widget_search_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            this->widget_search_content->setStyleSheet("background-color:transparent;");

            this->vbox_contents = new QVBoxLayout();
            this->vbox_contents->setSpacing(0);
            this->vbox_contents->setContentsMargins(0, 0, 0, 0);
            this->vbox_contents->addWidget(this->widget_search_content);


            this->widget_contents = new QWidget();
            this->widget_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_contents->setLayout(this->vbox_contents);


            this->box_contents->addWidget(this->widget_contents, 0, Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            rosetube::ItemTrack_rosetube *listAlbum = new rosetube::ItemTrack_rosetube(0, 0, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, false);

            this->rosetube_widget_width = listAlbum->get_fixedWidth();
            this->rosetube_widget_margin = listAlbum->get_rightMargin();

            delete listAlbum;

            // layout for items
            this->flowLayout_search_track = this->get_addUIControl_flowLayout(0, 20);
        }
    }


    /**
     * @brief 기본화면 구성
     */
    void RoseTube_Search::setUIControl_basic(){//cheon211008-search-----------------------------

        //start 상단 step 버튼//cheon211008-search-----------------------------
        global.btn_Music= new QPushButton(tr("Music"));
        global.btn_Music->setObjectName("btn_Music");
        global.btn_Music->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_MUSIC);

        global.btn_RoseTube = new QPushButton(tr("RoseTube"));
        global.btn_RoseTube->setObjectName("btn_RoseTube");
        global.btn_RoseTube->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_RoseTube);

        global.btn_RoseRadio= new QPushButton(tr("RoseRadio"));
        global.btn_RoseRadio->setObjectName("btn_RoseRadio");
        global.btn_RoseRadio->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_RoseRadio);

        global.btn_Tidal = new QPushButton(tr("Tidal"));
        global.btn_Tidal->setObjectName("btn_Tidal");
        global.btn_Tidal->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_TIDAL);

        global.btn_Bugs= new QPushButton(tr("Bugs"));
        global.btn_Bugs->setObjectName("btn_Bugs");
        global.btn_Bugs->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_BUGS);

        global.btn_Qobuz = new QPushButton(tr("Qobuz"));
        global.btn_Qobuz->setObjectName("btn_Qobuz");
        global.btn_Qobuz->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_QOBUZ);

        global.btn_Music->setFixedSize(125,40);
        global.btn_RoseTube->setFixedSize(125,40);
        global.btn_RoseRadio->setFixedSize(125,40);
        global.btn_Tidal->setFixedSize(125,40);
        global.btn_Bugs->setFixedSize(125,40);
        global.btn_Qobuz->setFixedSize(125,40);

        global.btn_Music->setCursor(Qt::PointingHandCursor);
        global.btn_RoseTube->setCursor(Qt::PointingHandCursor);
        global.btn_RoseRadio->setCursor(Qt::PointingHandCursor);
        global.btn_Tidal->setCursor(Qt::PointingHandCursor);
        global.btn_Bugs->setCursor(Qt::PointingHandCursor);
        global.btn_Qobuz->setCursor(Qt::PointingHandCursor);

        //end  상단 step 버튼 //--------------------------------------------------------------
        global.btn_Music->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Music").arg(tmp_btnStyle).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover_selected));
        global.btn_RoseRadio->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_RoseRadio").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_RoseTube->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_RoseTube").arg(tmp_btnStyle + tmp_btnStyleHover_selected).arg(tmp_btnStyleHover));//cheon211115-01
        global.btn_Tidal->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_Tidal").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_Bugs->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_Bugs").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_Qobuz->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Qobuz").arg(tmp_btnStyle).arg("border-top-right-radius:20px;border-bottom-right-radius:20px;").arg(tmp_btnStyleHover));

        // Main Title
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ", global.btn_Music, global.btn_RoseRadio, global.btn_RoseTube, global.btn_Tidal,  global.btn_Bugs, global.btn_Qobuz );//cheon211008-search
    }


    void RoseTube_Search::proc_wheelEvent_to_getMoreData(){

        if(this->flagReqMore_search == false && (this->flowLayout_search_track->count() == this->jsonArr_tracks_toPlay.size())
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_search = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // 더 가져오기 요청
            this->setUIControl_requestGetSearchTrack();
        }
    }


    void RoseTube_Search::setUIControl_requestGetSearchTrack(){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        ///newpipe/v1/more?query=IU&nextPageUrl=https://www.youtube.com/youtubei/v1/search?key%3DAIzaSyAO_FJ2SlqU8Q4STEHLGCilw_Y9_11qcW8
        ///                        &nextPageId=EpMDEgJJVRqMA0VnSVFBVWdVZ2dFTFowdGpXRTVZUVhGWFNqaUNBUXR6Y1dkNFkwTnFSREEwYzRJQkMzcGhVMUJsVldVek0xUm5nZ0VMZDBSbWNWaFNYelY1ZVZHQ0FRdFpiRmt5UTBwaFJYSmtSWUlCQ3pBdGNURkxZV1pHUTB4VmdnRUxiazB3ZUVSSk5WSTFNRVdDQVF0Mk4ySnVUM2hXTkdwQlk0SUJDMFF4VUhaSlYyUktPSGh2Z2dFTlVrUnpjV2Q0WTBOcVJEQTBjNElCQzBKNldXNU9aRXBvV2xGM2dnRUxWR2RQZFRBd1RXWXphMG1DQVFzemFVMWZNRFpSWlZwcE9JSUJDMk5JWWs1aFJrNXZTRU5aZ2dFTFpsOXBVVkpQTlVKa1EwMkNBUXRQWTFadFlVbHNTRm94YjRJQkMycGxjV1JaY1hOeWMwRXdnZ0VMWkRsSmVHUjNSVVpyTVdPQ0FRczBNa2QwYlRRdFFYZ3lWWUlCQzBwd1ZIRlRlbTAwU2s5cnNnRUdDZ1FJRXhBQxiB4OgYIgtzZWFyY2gtZmVlZA%253D%253D
        QString preheader = global.newpipe_v1 + "/more?query=";
        QString url = preheader + QString("%1").arg(this->data_search.search_word) + QString("&nextPageUrl=%1").arg(this->searchNextUrl) + QString("&nextPageId=%1").arg(this->searchNextId);
        QJsonObject json;

        network->request(HTTP_NETWORK_CHANNEL_LIST_MORE
                         , url
                         , json
                         , false
                         , true);
    }





    void RoseTube_Search::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << "p_jsonObj = " << strJson;*/

        if(p_id == HTTP_NETWORK_CHANNEL_LIST){

            this->searchNextId = ProcJsonEasy::getString(p_jsonObj, "nextPageId");
            this->searchNextUrl = ProcJsonEasy::getString(p_jsonObj, "nextPageUrl");

            QJsonArray tmpArr = ProcJsonEasy::getJsonArray(p_jsonObj, "info");
            if(tmpArr.size() > 0){
                this->slot_searchData(tmpArr);
            }
            else{
                this->flagReqMore_search = true;
                ContentLoadingwaitingMsgHide();

                if(this->flowLayout_search_track->count() == 0){
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::SearchResult_NoData);
                    noData_widget->setFixedSize(1500, 700);

                    this->flowLayout_search_track->addWidget(noData_widget);
                }
            }
        }
        else if(p_id == HTTP_NETWORK_CHANNEL_LIST_MORE){

            this->searchNextId = ProcJsonEasy::getString(p_jsonObj, "nextPageId");
            this->searchNextUrl = ProcJsonEasy::getString(p_jsonObj, "nextPageUrl");

            QJsonArray tmpArr = ProcJsonEasy::getJsonArray(p_jsonObj, "info");
            if(tmpArr.size() > 0){
                this->slot_searchData(tmpArr);
            }
            else{
                this->flagReqMore_search = true;
                ContentLoadingwaitingMsgHide();

                this->vbox_contents->addSpacing(30);
            }
        }
        else if(p_id == HTTP_NETWORK_CACHE){
            if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
            }
        }

        sender()->deleteLater();
    }


    void RoseTube_Search::slot_searchData(const QJsonArray &jsonArr){

        int start_index = this->jsonArr_tracks_toTemp.size();

        ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toTemp, jsonArr);

        for(int i = start_index; i < this->jsonArr_tracks_toTemp.size(); i++){
            this->search_track[i] = new rosetube::ItemTrack_rosetube(i, SECTION_FOR_MORE_POPUP___SEARCH, tidal::AbstractItem::ImageSizeMode::Ractangle_284x157, true);
        }

        for(int i = start_index; i < this->jsonArr_tracks_toTemp.size(); i++){
            QJsonObject tmpTrack = this->jsonArr_tracks_toTemp.at(i).toObject();

            QString tmpchannelId = ProcJsonEasy::getString(tmpTrack, "uploaderUrl");
            QString channelId = "";
            QStringList strListchannelId = tmpchannelId.split("channel/");
            if(strListchannelId.size() == 2){
                channelId = strListchannelId.at(1);
            }

            QString tmpId = ProcJsonEasy::getString(tmpTrack, "url");
            QString id = "";
            QStringList strListId = tmpId.split("?v=");
            if(strListId.size() == 2){
                id = strListId.at(1);
            }

            QString channelName = ProcJsonEasy::getString(tmpTrack, "uploaderName");
            QString title = ProcJsonEasy::getString(tmpTrack, "name");
            QString thumbnailUrl = ProcJsonEasy::getString(tmpTrack, "thumbnailUrl");

            int duration = ProcJsonEasy::getInt(tmpTrack, "duration");
            int viewCount = ProcJsonEasy::getInt(tmpTrack, "viewCount");

            QJsonObject jsonPlayTrack;
            jsonPlayTrack.insert("channelId", channelId);
            jsonPlayTrack.insert("channelName", channelName);
            jsonPlayTrack.insert("duration", duration);
            jsonPlayTrack.insert("id", id);
            jsonPlayTrack.insert("isFavorite", false);
            jsonPlayTrack.insert("isLiveStream", false);
            jsonPlayTrack.insert("thumbnailUrl", thumbnailUrl);
            jsonPlayTrack.insert("thumbsUpPercentage", 0);
            jsonPlayTrack.insert("title", title);
            jsonPlayTrack.insert("viewCount", viewCount);

            this->jsonArr_tracks_toPlay.append(jsonPlayTrack);

            QJsonObject jsonTrack;
            jsonTrack.insert("duration", duration);
            jsonTrack.insert("title", title);
            jsonTrack.insert("channelTitle", channelName);
            jsonTrack.insert("thumbnailUrl", thumbnailUrl);
            jsonTrack.insert("viewCount", ProcJsonEasy::getInt(tmpTrack, "viewCount"));
            jsonTrack.insert("favorite_view", false);

            this->search_track[i]->setData(jsonTrack);

            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->jsonArr_tracks_toTemp.size(); i++){
            this->flowLayout_search_track->addWidget(this->search_track[i]);
            connect(this->search_track[i], &rosetube::ItemTrack_rosetube::signal_clicked, this, &RoseTube_Search::slot_clickedItemPlaylist);
        }

        if(this->searchNextUrl.isEmpty()){
            this->flagReqMore_search = true;
            this->vbox_contents->addSpacing(30);
        }
        else{
            this->flagReqMore_search = false;
        }

        ContentLoadingwaitingMsgHide();
    }


    void RoseTube_Search::slot_add_rosePlaylist_withRosetube(const int &idx, const QJsonObject &dataObj){

        QString view_type = "";
        if(idx < 0){
            view_type = "create";
        }
        else{
            view_type = "add";
        }

        QJsonObject jsonObj_move = dataObj;
        jsonObj_move.insert("view_type", view_type);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RT_ADDPLAYLIST);

        emit this->signal_clickedPagemove(jsonObj_move);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RoseTube_Search::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___SEARCH){
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
                default:
                    playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    curr_clickMode = 15;
                    break;
                }

                QJsonArray jsonArr_toPlayReal = this->get_rearrangeJsonArray_toPlayData(this->jsonArr_tracks_toPlay, index, playType);

                qDebug() << "index=" << index;
                qDebug() << "section=" << section;
                qDebug() << "jsonArr_tracks_toPlay=" << jsonArr_tracks_toPlay;
                QJsonObject jsonData;
                jsonData.insert("youtube", jsonArr_toPlayReal);
                jsonData.insert("youtubePlayType", curr_clickMode);
                jsonData.insert("roseToken", global.device.getDeviceRoseToken());

                if(global.queueTimerFlag && !global.queuelist_mouse_trigger_menu_flag && global.Queue_track_count != 0) {
                    emit this->linker->signal_checkQueue(27, "");

                    return;
                }
                emit this->linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArr_toPlayReal.count();     // 220419 queue count

                QString playlistType = "";
                if(curr_clickMode == 12 || curr_clickMode == 15 || curr_clickMode == 17 || curr_clickMode == 18){
                    playlistType = "youtubePlay";
                }
                else{
                    playlistType = "youtubePlay.playlist.add";
                }

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3")
                                 .arg(global.device.getDeviceIP())
                                 .arg(global.port).arg(playlistType), jsonData, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){//c220818
                QJsonObject tmpObj = this->jsonArr_tracks_toTemp.at(index).toObject();
                //print_debug();
                //qDebug() << "tmpObj==" << tmpObj;

                QString id = ProcJsonEasy::getString(tmpObj, "url");
                QStringList tmpID = id.split("?v=");

                OptMorePopup::HeaderData data_header;
                data_header.main_title = ProcJsonEasy::getString(tmpObj, "name");
                data_header.sub_title = ProcJsonEasy::getString(tmpObj, "uploaderName");//c220818
                data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                data_header.data_pk = "https://youtu.be/" + tmpID.at(1);  //j220906 share link
                data_header.type = "YOUTUBE";      //j220906 share link
                data_header.flagProcStar = false;
                data_header.isShare = true;      //j220906 share link

                // OptMorePopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosetube_Video, data_header, index, section);
            }
        }
    }

    void RoseTube_Search::slot_applyResult_getShareLink(const QString &link){//c220818 share link

        this->shareLink = link;
    }

    void RoseTube_Search::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___SEARCH){
            /*if(clickMode == OptMorePopup::ClickMode::Share){////c220826_2
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }*/
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
                jsonData.insert("youtube", jsonArr_toPlayReal);
                jsonData.insert("youtubePlayType", curr_clickMode);
                jsonData.insert("roseToken", global.device.getDeviceRoseToken());

                if(global.queueTimerFlag && !global.queuelist_mouse_trigger_menu_flag && global.Queue_track_count != 0) {
                    emit this->linker->signal_checkQueue(27, "");

                    return;
                }
                emit this->linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArr_toPlayReal.count();     // 220419 queue count

                QString playlistType = "";
                if(curr_clickMode == 12 || curr_clickMode == 15 || curr_clickMode == 17 || curr_clickMode == 18){
                    playlistType = "youtubePlay";
                }
                else{
                    playlistType = "youtubePlay.playlist.add";
                }

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3")
                                 .arg(global.device.getDeviceIP())
                                 .arg(global.port).arg(playlistType), jsonData, true);
            }
            else if(clickMode == OptMorePopup::ClickMode::Add_MyCollection){
                QJsonArray tracks = QJsonArray();
                tracks.append(this->jsonArr_tracks_toPlay.at(index).toObject());

                QJsonObject json = QJsonObject();
                json.insert("tracks", tracks);

                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::ROSETUBE, json, this);
                dialog_playlist->request_playlist_fetch();
                connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &RoseTube_Search::slot_add_rosePlaylist_withRosetube);
                int result = dialog_playlist->exec();

                if(result == QDialog::Accepted){
                    delete dialog_playlist;
                }
            }
            else if(clickMode == OptMorePopup::ClickMode::SubscribInfo){
                QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(index).toObject();

                QString channelName = "";
                QString channelId = "";

                if(tmpObj.contains("data")){
                    QJsonObject dataObj = ProcJsonEasy::getJsonObject(tmpObj, "data");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(dataObj, "snippet");

                    channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                    channelId = ProcJsonEasy::getString(snippet, "channelId");
                }
                else{
                    if(tmpObj.contains("channelTitle")){
                        channelName = ProcJsonEasy::getString(tmpObj, "channelTitle");
                    }
                    else if(tmpObj.contains("channelName")){
                        channelName = ProcJsonEasy::getString(tmpObj, "channelName");
                    }

                    channelId = ProcJsonEasy::getString(tmpObj, "channelId");
                }

                QJsonObject tmp_data;
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert("title", channelName);
                tmp_data.insert("channel_id", channelId);

                emit this->signal_clickedViewAll(tmp_data);
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


    int RoseTube_Search::get_rose_playType(OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
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


    QJsonArray RoseTube_Search::get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
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
    QJsonArray RoseTube_Search::subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex){

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
    QJsonArray RoseTube_Search::reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex){

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


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void RoseTube_Search::resizeEvent(QResizeEvent *event){

        Q_UNUSED(event);

        this->setFlowLayoutResize(this, this->flowLayout_search_track, this->rosetube_widget_width, this->rosetube_widget_margin);
    }
}
