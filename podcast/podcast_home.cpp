#include "podcast/podcast_home.h"

#include "common/global.h"
#include "common/networkhttp.h"
#include "common/rosesettings.h"

#include "widget/framepodcast.h"

#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>


namespace podcast {

    const int HTTP_NETWORK_CHANNEL_LIST = 99;


    /**
     * @brief Podcast_Home::PodcastHome : 팟캐스트-인기
     * @details : 팟캐스트-인기 채널 목록<br>
     * 인기 채널 목록 GET API : https://www.hifimp.com:18081/v2/rest/rose/IF_ROSEPODCAST_007
     * @param parent
     */
    Podcast_Home::Podcast_Home(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_viewAll, parent)
    {

        setInit();
        setUIControl();

        if(global.enable_section_left == true){
            global.enable_section_left = false;
        }
    }


    Podcast_Home::~Podcast_Home(){

        this->deleteLater();
    }


    /**
     * @brief Podcast_Home::setInit : 초기 세팅
     */
    void Podcast_Home::setInit(){

    }


    /**
     * @brief Podcast_Home::setUIControl : UI 세팅
     */
    void Podcast_Home::setUIControl(){

        /*flowLayout = new FlowLayout(0,20,20);
        flowLayout->setContentsMargins(0,0,0,10);

        QWidget *wg_contents = new QWidget;
        wg_contents->setLayout(flowLayout);
        wg_contents->setContentsMargins(0,0,0,0);

        QScrollArea *scrollArea = new QScrollArea;
        scrollArea->setWidget(wg_contents);
        scrollArea->setWidgetResizable(true);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setStyleSheet("border:0px;");

        QVBoxLayout *boxMain = new QVBoxLayout;
        boxMain->setContentsMargins(33,42,28,0);
        boxMain->addWidget(scrollArea);

        setLayout(boxMain);*/

        // 항상 부모클래스의 함수 먼저 호출
        AbstractRoseHomeSubWidget::setActivePage();

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // layout for items
        this->flowLayout_podcast = new FlowLayout(0, 20, 20);
        this->flowLayout_podcast->setSizeConstraint(QLayout::SetMinimumSize);

        GSCommon::clearLayout(this->flowLayout_podcast);

        this->widget_podcast = new QWidget();
        this->widget_podcast->setLayout(this->flowLayout_podcast);
        this->widget_podcast->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->widget_podcast->setStyleSheet("background-color:transparent;");

        this->box_contents->addWidget(this->widget_podcast);
    }


    /**
     * @brief Podcast_Home::appendChannel : 채널 UI 추가
     * @param jsonData QJsonObject
     */
    void Podcast_Home::appendChannel(const QJsonObject &jsonData){

        FramePodcast *tmp_widget = new FramePodcast;
        tmp_widget->setData(jsonData);
        tmp_widget->setHover();
        tmp_widget->setCursor(Qt::PointingHandCursor);

        //flowLayout->addWidget(tmp_widget);
        this->flowLayout_podcast->addWidget(tmp_widget);
    }


    /**
     * @brief Podcast_Home::setResultOfChannelList : 인기채널 목록 요청결과 처리
     * @param p_jsonObject
     */
    void Podcast_Home::setResultOfChannelList(const QJsonObject &p_jsonObject){


        const QString jsonKey_flagOk = "flagOk";
        const QString jsonKey_status = "status";
        const QString jsonKey_resultCd = "resultCd";
        const QString jsonKey_data = "data";

        if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
            if(p_jsonObject.contains(jsonKey_status)){
                QJsonObject jsonStatus = p_jsonObject[jsonKey_status].toObject();
                if(jsonStatus.contains(jsonKey_resultCd) && jsonStatus[jsonKey_resultCd].toString().toLower()=="success"){
                    if(p_jsonObject.contains(jsonKey_data)){
                        QJsonArray jsonArrData = p_jsonObject[jsonKey_data].toArray();

                        for(int i = 0; i < jsonArrData.size(); i++){
                            appendChannel(jsonArrData.at(i).toObject());
                        }
                    }
                }
            }
        }

        if(global.enable_section_left == true){
            global.enable_section_left = false;
        }
    }


    /**
     * @brief Podcast_Home::requestData : 인기 채널 목록 데이터 요청
     */
    void Podcast_Home::requestData(){

        if(this->flowLayout_podcast->count()==0){
            GSCommon::clearLayout(this->flowLayout_podcast);

            QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
            int tmp_countryID = settings->value(rosesettings.SETTINGS_PODCAST_COUNTRY,2).toInt();
            delete settings;

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QJsonObject json;
            json.insert("countryID", tmp_countryID);
            json.insert("roseToken", global.device.getDeviceRoseToken());

            network->request(HTTP_NETWORK_CHANNEL_LIST, QString("%1/IF_ROSEPODCAST_007")
                             //.arg(global.hifimp)
                             .arg(global.release_roseaudio)
                             , json, true);
        }
    }


    /**
     * @brief Podcast_Home::slot_responseHttp : [슬롯] Http 요청 결과
     * @param p_id int 요청 아이디
     * @param p_jsonObject QJsonObject response
     */
    void Podcast_Home::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

        if(p_id==HTTP_NETWORK_CHANNEL_LIST){
            setResultOfChannelList(p_jsonObject);
        }

       sender()->deleteLater();
    }
}
