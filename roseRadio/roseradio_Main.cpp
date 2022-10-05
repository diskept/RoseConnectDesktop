#include "roseRadio/roseradio_Main.h"

#include "common/common_struct.h"
#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "widget/toastmsg.h"

#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QMetaObject>
#include <QResizeEvent>


namespace roseRadio {

    /**
     * @brief RoseRadio 메인 생성자
     * @param parent
     */
    roseRadioMain::roseRadioMain(QWidget *parent) : AbstractMainContent(parent){

        linker = Linker::getInstance();
        connect(linker, &Linker::signal_clicked_movePage, this, &roseRadioMain::goToMoveNewOrderPage);
        connect(linker, SIGNAL(signal_search(QString)), SLOT(slot_search(QString)));
        connect(linker, SIGNAL(signal_dragEnterEvent_hide_show(bool)), SLOT(slot_dragEnterEvent_hide_show(bool)));//c220730
        connect(linker, SIGNAL(signal_dropEvent_hide_show(bool)), SLOT(slot_dropEvent_hide_show(bool)));//c220730

        make_CustomLineEdit();//c220730

        this->setUIControl();
    }


    roseRadioMain::~roseRadioMain(){

        this->deleteLater();
    }


    void roseRadioMain::slot_overrideSigalSearch(bool b){//c220728
        print_debug();
        if(b){
            emit linker->signal_clickedMovePageRoseTubeSearch();
        }else{
            slot_dragEnterEvent_hide_show(false);
            emit linker->signal_checkQueue(30, "");//c220729
        }

    }

    void roseRadioMain::slot_dragEnterEvent_hide_show(bool show){//c220826_1
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


    void roseRadioMain::slot_dragEnterEvent_restore(){//c220826_1
        print_debug();
        this->curr_widget = this->stackedWidget_content->currentWidget();//c220824_4
        this->le_search_back->clear();
        emit linker->signal_searchBarFocusChanged(false);
        this->le_search_back->clearFocus();
        this->curr_widget->show();
        this->le_search_back->hide();
    }

    /**
     * @brief 검색어에 대하여 Qobuz 검색 메인 페이지를 보이도록 한다.
     */
    void roseRadioMain::process_searchWord(const QString &search_word){

        if(search_word.size() >= 2){
            global.search_text = search_word;//searchtextcopy

            common::SearchData data_search;
            data_search.search_word = search_word;

            QJsonObject jsonObj_search;
            jsonObj_search[KEY_DATA] = common::ConvertData_common::getObjectJson_searchData(data_search);
            jsonObj_search[KEY_PAGE_CODE] = PAGECODE_RR_SEARCHMAIN;

            this->goToMoveNewOrderPage(jsonObj_search);
        }
        else{
            ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
        }
    }



    void roseRadioMain::slot_search(const QString &search_word){

        if(search_word.size() >= 2){
            global.search_text = search_word;//searchtextcopy

            common::SearchData data_search;
            data_search.search_word = search_word;

            QJsonObject jsonObj_search;
            jsonObj_search[KEY_DATA] = common::ConvertData_common::getObjectJson_searchData(data_search);
            jsonObj_search[KEY_PAGE_CODE] = PAGECODE_RR_SEARCHMAIN;

            this->goToMoveNewOrderPage(jsonObj_search);
        }
        else{
            ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
        }
    }


    void roseRadioMain::setUIControl(){

        QString pageCode_firstPage = PAGECODE_RR_HOME;
        QJsonObject jsonObj_first;
        jsonObj_first[KEY_PAGE_CODE] = pageCode_firstPage;

        this->goToMoveNewOrderPage(jsonObj_first);
    }


    /**
     * @brief RoseRadio 각 하위 페이지 클래스 마다, 발생하는 signal_clickedMovePage 시그널을 공통으로 처리한다.
     * @param p_jsonData : 요청한 페이지에 대한 정보가 담겨있다. JsonObject 형식. @n
     *    @li KEY_PAGE_CODE : 해당 페이지 클래스에 대응되는 Unique한 값이다. @see gscommon.h 파일에 정의.
     *    @li KEY_DATA : 옵션 JsonObject. 해당 페이지 클래스에 그대로 넘겨서 보내줄 데이터이다. {} 일 수 있음.
     */
    void roseRadioMain::slot_clickedMoveSubPage(const QJsonObject &p_jsonData){

        // 페이지 히스토리 관리 관련된 Abstract 함수 호출
        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = p_jsonData[KEY_PAGE_CODE].toString();
        tmp_data[KEY_DATA] = p_jsonData;

        this->goToMoveNewOrderPage(tmp_data);
    }


    /**
     * @brief TopMenuBar 객체에게 전달하기 위한 Main 타이틀 정보를 JsonObject 로 생성하여 반환
     * @return QJsonObject
     */
    QJsonObject roseRadioMain::getJsonObject_forMainTitle(){

        QJsonObject jsonObj;
        jsonObj["name"] = tr("RoseRadio");
        jsonObj[KEY_MAIN_CODE] = GSCommon::MainMenuCode::RoseRadio;

        return jsonObj;
    }


    /**
     * @brief TopMenuBar 객체에게 전달하기 위한 Sub 타이틀 정보 여러개를 JsonArray 로 생성하여 반환
     * @return QJsonArray* :
     */
    QJsonArray roseRadioMain::getJsonArray_forSubTitles(){

        QJsonArray p_jsonArray_titlSub ;
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Home")}, {"code", PAGECODE_RR_HOME} });
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Explore")}, {"code", PAGECODE_RR_EXPLORE} });
        //p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("my radio")}, {"code", PAGECODE_RR_MYRADIO} });
        //p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Reservation")}, {"code", PAGECODE_RR_RESERVATION} });
        //p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Setting")}, {"code", PAGECODE_RR_SETTINGS} });

        return p_jsonArray_titlSub;
    }


    /**
     * @brief RoseRadioMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
     * @param p_pageCode
     */
    void roseRadioMain::movePageOnly(QString p_pageCode, const QJsonObject &p_data){

        QString pageCode = "";
        QJsonObject data;

        if(global.user.isValid() == false && (p_pageCode != PAGECODE_RR_HOME)){

            if(p_pageCode == PAGECODE_RR_SEARCHMAIN){
                pageCode = p_pageCode;
                data = p_data;
            }
            else{
                pageCode = PAGECODE_RR_HOME;
                data[KEY_PAGE_CODE] = PAGECODE_RR_HOME;
            }
        }
        else{
            pageCode = p_pageCode;
            data = p_data;
        }

        QJsonObject tmp_jsonObj_titleMain = this->getJsonObject_forMainTitle();
        QJsonArray tmp_jsonArr_titlSub = this->getJsonArray_forSubTitles();

        // 부모 virtual 함수 호출
        AbstractMainContent::movePageOnly(pageCode, data);

        if(p_pageCode == "playfullscreen"){
            tmp_jsonObj_titleMain = QJsonObject();
            tmp_jsonArr_titlSub = QJsonArray();

            this->topMenuBar->setDataTopMenuBar(tmp_jsonObj_titleMain, &tmp_jsonArr_titlSub);
        }
        else{
            // 자식 override 함수 서술
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(tmp_jsonObj_titleMain, &tmp_jsonArr_titlSub);
        }
        this->topMenuBar->setVisible(true);


        // 서브 메뉴페이지에 대한 처리
        if(pageCode == PAGECODE_RR_HOME){
            this->sub_home = this->procCommon_showSubPage<roseRadio::RoseRadioHome>(true, this->sub_home, data);
        }
        else if(pageCode == PAGECODE_RR_EXPLORE){
            this->sub_explore = this->procCommon_showSubPage<roseRadio::RoseRadioExplore>(true, this->sub_explore, data);
        }

        else if(pageCode == PAGECODE_RR_LISTVIEWALL){
            this->sub_ListViewAll = this->procCommon_showSubPage<roseRadio::RoseRadioListViewAll>(false, this->sub_ListViewAll, data);
        }
        else if(pageCode == PAGECODE_RR_GENREVIEWALL){
            this->sub_GenreViewAll = this->procCommon_showSubPage<roseRadio::RoseRadioGenreViewAll>(false, this->sub_GenreViewAll, data);
        }
        else if(pageCode == PAGECODE_RR_COUNTRYVIEWALL){
            this->sub_CountryViewAll = this->procCommon_showSubPage<roseRadio::RoseRadioCountryViewAll>(false, this->sub_CountryViewAll, data);
        }
        else if(pageCode == PAGECODE_RR_REGIONVIEWALL){
            this->sub_RegionViewAll = this->procCommon_showSubPage<roseRadio::RoseRadioRegionViewAll>(false, this->sub_RegionViewAll, data);
        }
        else if(pageCode == PAGECODE_RR_LANGUAGEVIEWALL){
            this->sub_LanguageViewAll = this->procCommon_showSubPage<roseRadio::RoseRadioLanguageViewAll>(false, this->sub_LanguageViewAll, data);
        }

        else if(pageCode == PAGECODE_RR_COUNTRYDETAIL){
            this->sub_CountryDetail = this->procCommon_showSubPage<roseRadio::RoseRadioCountryDetail>(false, this->sub_CountryDetail, data);
        }

        else if(pageCode == PAGECODE_RR_SEARCHMAIN){
            this->sub_SearchMain = this->procCommon_showSubPage<roseRadio::roseRadioSearchMain>(false, this->sub_SearchMain, data);
        }
    }

    /**
     * @brief RoseRadioMain::procCommon_showSubPage
     * @param flagShow_topBar
     * @param sub_widget
     */
    template<typename T_roseRadioSub> T_roseRadioSub* roseRadioMain::procCommon_showSubPage(const bool flagShow_topBar, T_roseRadioSub* sub_widget, const QJsonObject &jsonObj_data){

        this->topMenuBar->setVisible(flagShow_topBar);

        if(sub_widget == nullptr){
            sub_widget = new T_roseRadioSub();
            this->stackedWidget_content->addWidget(sub_widget);
            this->stackedWidget_content->addWidget(le_search_back);//c220730
            sub_widget->setConnectSiganl_toMovePage(this, SLOT(slot_clickedMoveSubPage(const QJsonObject&)));
        }
        stackedWidget_content->setCurrentWidget(sub_widget);
        sub_widget->setJsonObject_forData(ProcJsonEasy::getJsonObject(jsonObj_data, KEY_DATA));
        sub_widget->setActivePage();

        return sub_widget;
    }


    void roseRadioMain::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

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


