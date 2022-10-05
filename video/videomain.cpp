#include "video/videomain.h"

#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "widget/toastmsg.h"


namespace video {

    VideoMain::VideoMain(QWidget *parent) : AbstractMainContent(parent)
    {
        this->setInit();
        this->setUIControl();
    }

    /**
     * @brief VideoMain::setInit : 초기 세팅
     */
    void VideoMain::setInit(){
        currMainMenuCode = GSCommon::MainMenuCode::Video;

        connect(linker, SIGNAL(signal_search(QString)), SLOT(slot_search(QString)));
        connect(linker, SIGNAL(signal_dragEnterEvent_hide_show(bool)), SLOT(slot_dragEnterEvent_hide_show(bool)));//c220730
        connect(linker, SIGNAL(signal_dropEvent_hide_show(bool)), SLOT(slot_dropEvent_hide_show(bool)));//c220730

        make_CustomLineEdit();//c220730
    }

    void VideoMain::slot_overrideSigalSearch(bool b){//c220728
        print_debug();
        if(b){
            emit linker->signal_clickedMovePageRoseTubeSearch();
        }else{
            slot_dragEnterEvent_hide_show(false);
            emit linker->signal_checkQueue(30, "");//c220729
        }

    }

    void VideoMain::slot_dragEnterEvent_hide_show(bool show){//c220826_1
        print_debug();
        qDebug()<<"##########################################################################################################";
        if(show){
            if(global.width_mainwindow==0){
                this->le_search_back->setFixedSize(800,500);
            }else{
                this->le_search_back->setFixedSize(global.width_mainwindow-200,global.height_mainwindow);
            }
            this->curr_widget = this->stackedWidget_content->currentWidget();
            this->curr_widget->hide();
            this->le_search_back->show();
            QTimer::singleShot(3000, this, SLOT(slot_dragEnterEvent_restore()));

        }else{
            print_debug();
            this->curr_widget = this->stackedWidget_content->currentWidget();//c220824_4
            this->le_search_back->clear();
            emit linker->signal_searchBarFocusChanged(false);
            this->le_search_back->clearFocus();
            this->curr_widget->show();
            this->le_search_back->hide();
        }

    }


    void VideoMain::slot_dragEnterEvent_restore(){//c220826_1
        print_debug();
        this->curr_widget = this->stackedWidget_content->currentWidget();//c220824_4
        this->le_search_back->clear();
        emit linker->signal_searchBarFocusChanged(false);
        this->le_search_back->clearFocus();
        this->curr_widget->show();
        this->le_search_back->hide();
    }

    /**
     * @brief VideoMain::setUIControl : UI 세팅
     */
    void VideoMain::setUIControl(){

        // 메뉴 페이지
        this->videoHome = new VideoHome();


        // 스택에 추가
        this->stackedWidget_content->addWidget(this->videoHome);
        this->stackedWidget_content->addWidget(le_search_back);//c220730

        // default : 첫번째 서브메뉴
        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = PAGECODE_V_HOME;
        this->goToMoveNewOrderPage(tmp_data);
    }

    /**
     * @brief VideoMain::slot_search : [슬롯] 검색 요청
     * @param p_text
     */
    void VideoMain::slot_search(const QString &p_text){

        if(p_text.size() >= 2){
            QJsonObject tmp_data;
            tmp_data[KEY_PAGE_CODE] = PAGECODE_V_SEARCH;
            tmp_data[KEY_DATA] = p_text;
            this->goToMoveNewOrderPage(tmp_data);
        }else{
            ToastMsg::show(this, "", tr("Please enter at least two letters for the search word."));
            //ToastMsg::show(this, "", tr("검색어는 2글자 이상 입력해주세요."));
        }
    }


    /**
     * @brief VideoMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
     * @param p_pageCode 서브메뉴 코드
     */
    void VideoMain::movePageOnly(QString p_pageCode, const QJsonObject &p_data){

        if(this->videoFolder!=nullptr){
            this->videoFolder->hide_topBtnFavorite();
        }

        const QString jsonKey_video_info = "video_info";

        QJsonObject p_jsonObject_titleMain;
        QJsonArray *p_jsonArray_titlSub = new QJsonArray();

        p_jsonObject_titleMain["name"] = tr("Video");
        p_jsonObject_titleMain[KEY_MAIN_CODE] = GSCommon::MainMenuCode::Video;
        QJsonObject sub1 { {"name", tr("Home")}, {"code", PAGECODE_V_HOME} };
        QJsonObject sub2 { {"name", tr("Folder")}, {"code", PAGECODE_V_FOLDER} };
        /*
         *     p_jsonObject_titleMain["name"] = "비디오";
        p_jsonObject_titleMain[KEY_MAIN_CODE] = GSCommon::MainMenuCode::Video;
        QJsonObject sub1 { {"name", "홈"}, {"code", PAGECODE_V_HOME} };
        QJsonObject sub2 { {"name", "폴더"}, {"code", PAGECODE_V_FOLDER} };
        */

        p_jsonArray_titlSub->push_back(sub1);
        p_jsonArray_titlSub->push_back(sub2);

        this->topMenuBar->setVisible(true);


        // 부모 virtual 함수 호출
        AbstractMainContent::movePageOnly(p_pageCode, p_data);

        // 자식 override 함수 서술
        this->topMenuBar->setSelectedSubMenu(p_pageCode);
        this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, p_jsonArray_titlSub);

        if(p_pageCode==PAGECODE_V_HOME){

            if(this->videoHome==nullptr){
                this->videoHome = new VideoHome();
                this->stackedWidget_content->addWidget(videoHome);
            }

            if(ProcJsonEasy::getString(p_data, "pageCode2") != "videoInfo"){
                this->videoHome->setDataFromRose();
            }
            stackedWidget_content->setCurrentWidget(this->videoHome);

        }else if(p_pageCode==PAGECODE_V_FOLDER){

            if(this->videoFolder==nullptr){
                this->videoFolder = new MusicFolder(topMenuBar,false);
                this->stackedWidget_content->addWidget(videoFolder);
                connect(videoFolder, SIGNAL(clickedSubTab(QJsonObject)), this, SLOT(goToMoveNewOrderPage_subStep(QJsonObject)));
            }

            videoFolder->setDataABSPage(p_data);
            stackedWidget_content->setCurrentWidget(videoFolder);

        }else if(p_pageCode==PAGECODE_OP_VIDEOINFO){

            if(this->videoDetail==nullptr){
                this->videoDetail = new VideoDetail();
                this->stackedWidget_content->addWidget(videoDetail);
                connect(this->videoDetail, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
            }

            QJsonObject jsonVideoInfo;
            if(p_data.contains(jsonKey_video_info)){
                jsonVideoInfo = p_data[jsonKey_video_info].toObject();
            }

            this->topMenuBar->setVisible(false);

            stackedWidget_content->setCurrentWidget(this->videoDetail);
            this->videoDetail->setJsonObject_forData(jsonVideoInfo);
            this->videoDetail->setActivePage();
        }
        else if(p_pageCode==PAGECODE_V_SEARCH){

            if(this->videoSearch==nullptr){
                this->videoSearch = new VideoSearch();
                this->stackedWidget_content->addWidget(videoSearch);
                connect(this->videoSearch, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
            }

            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            videoSearch->search(p_data[KEY_DATA].toString());
            stackedWidget_content->setCurrentWidget(this->videoSearch);
        }
    }

    void VideoMain::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

        print_debug();
        Q_UNUSED(p_jsonObj);
        if(p_id == 3344){//c220727
            print_debug();
            if(ProcJsonEasy::get_flagOk(p_jsonObj)){
                print_debug();
                ToastMsg::show(this,"", tr("Play the selected YouTube content."), 2000, 0);

            }else{
                print_debug();
                ToastMsg::show(this,"", tr("Failed to perform playback of the selected YouTube content."), 2000, 0);

            }
            slot_overrideSigalSearch(false);

        }
        else if(p_id == 5678){//c220803
            print_debug();
            if(ProcJsonEasy::get_flagOk(p_jsonObj)){
                qDebug() << "test";
                print_debug();

                bool p_flagShuffle = false;
                QJsonArray jsonArrYoutube;

                QJsonArray tmp_jsonArrYoutube = p_jsonObj["track"].toArray();
                //QJsonObject tmp_json;
                QJsonObject jsonTrackData;
                jsonTrackData.insert("playurl",global.dragDrop_pathStr);
                qDebug()<<"global.dragDrop_pathStr="<<global.dragDrop_pathStr;
                //---//c220802
                QString videoID;
                QString tmp_Path4 = global.dragDrop_pathStr.split("&").at(0);
                if((global.dragDrop_pathStr.contains("&")&& global.dragDrop_pathStr.split("&").at(1).size()>0)){
                    videoID = tmp_Path4.split("?v=").last();
                }else{
                    videoID = jsonTrackData["playurl"].toString().split("?v=").last();
                }
                //---------------------
                if(tmp_jsonArrYoutube.count()>0){
                    for(int i = 0 ; i < tmp_jsonArrYoutube.count(); i++){

                        jsonTrackData = tmp_jsonArrYoutube.at(i).toObject();
                        QString videoID = jsonTrackData["url"].toString().split("?v=").last();

                        QJsonObject jsonYoutube;
                        jsonYoutube.insert("channelId", p_jsonObj["channelId"].toString());
                        jsonYoutube.insert("channelName", p_jsonObj["uploaderName"].toString());
                        jsonYoutube.insert("duration", QString("%1").arg(p_jsonObj["duration"].toInt()));
                        jsonYoutube.insert("id", videoID);
                        jsonYoutube.insert("isFavorite", p_jsonObj["favorites"].toString()=="1" ? true : false);
                        jsonYoutube.insert("isLiveStream",p_jsonObj["streamType"].toString()=="LIVE_STREAM" ? true : false);
                        jsonYoutube.insert("thumbnailUrl", p_jsonObj["thumbnailUrl"].toString());
                        jsonYoutube.insert("thumbsUpPercentage",0);
                        jsonYoutube.insert("title", p_jsonObj["name"].toString());
                        jsonYoutube.insert("viewsCount", QString("%1").arg(p_jsonObj["viewCount"].toInt()));

                        // jsonArrYoutube.append(jsonYoutube);
                    }
                }else{
                    qDebug()<<"videoID="<<videoID;
                    QJsonObject jsonYoutube;
                    jsonYoutube.insert("channelId", p_jsonObj["channelId"].toString());
                    jsonYoutube.insert("channelName", p_jsonObj["uploaderName"].toString());
                    jsonYoutube.insert("duration", QString("%1").arg(p_jsonObj["duration"].toInt()));
                    jsonYoutube.insert("id", videoID);
                    jsonYoutube.insert("isFavorite", p_jsonObj["favorites"].toString()=="1" ? true : false);
                    jsonYoutube.insert("isLiveStream",p_jsonObj["streamType"].toString()=="LIVE_STREAM" ? true : false);
                    jsonYoutube.insert("thumbnailUrl", p_jsonObj["thumbnailUrl"].toString());
                    jsonYoutube.insert("thumbsUpPercentage",0);
                    jsonYoutube.insert("title", p_jsonObj["name"].toString());
                    jsonYoutube.insert("viewsCount", QString("%1").arg(p_jsonObj["viewCount"].toInt()));

                    jsonArrYoutube.append(jsonYoutube);
                }

                QJsonObject jsonParam;
                jsonParam.insert("roseToken", global.device.getDeviceRoseToken());
                jsonParam.insert("shuffle", p_flagShuffle ? 1 : 0);
                jsonParam.insert("youtubePlayType", 15);
                jsonParam.insert("youtube", jsonArrYoutube);

                if(global.Queue_track_count != 0) {
                    print_debug();emit linker->signal_checkQueue(27, "");

                    return;
                }
                print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArrYoutube.count();     // 220419 queue count
                emit linker->signal_checkQueue(32, QString(tr(" Play %1 : %2")).arg(p_jsonObj["uploaderName"].toString()).arg(p_jsonObj["name"].toString().left(20)));//c220802


                NetworkHttp *network = new NetworkHttp(this);
                connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));

                network->request(3344, QString("http://%1:%2/youtubePlay.playlist.add")
                                 .arg(global.device.getDeviceIP()).arg(global.port)
                                 , jsonParam, true);
            }else{

                print_debug();
                qDebug() << "ProcJsonEasy::get_flagOk(p_jsonObj) = "<< ProcJsonEasy::get_flagOk(p_jsonObj);

                emit linker->signal_checkQueue(33, "");//c220803

            }


            slot_overrideSigalSearch(false);
        }


        sender()->deleteLater();
    }
}
