#include "podcast_search.h"
#include "common/rosesettings.h"
#include "common/global.h"
#include <QScrollArea>
#include <QSettings>
#include <common/networkhttp.h>
#include <widget/framepodcast.h>

const int HTTP_NETWORK_CHANNEL_LIST = 99;

/**
 * @brief Podcast_Search::Podcast_Search : 팟캐스트 검색 결과 화면
 * @param parent
 */
Podcast_Search::Podcast_Search(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief Podcast_Search::setInit : 초기 세팅
 */
void Podcast_Search::setInit(){
    linker = Linker::getInstance();
}

/**
 * @brief Podcast_Search::setUIControl : UI 세팅
 */
void Podcast_Search::setUIControl(){

    QLabel *lb_empty = new QLabel(tr("No results were found for your search."));
    //QLabel *lb_empty = new QLabel(tr("검색결과가 없습니다."));
    lb_empty->setStyleSheet("font-size:18px; color:#FFFFFF;");
    lb_empty->setContentsMargins(0,0,0,0);
    lb_empty->setAlignment(Qt::AlignCenter);

    QVBoxLayout *box_empty = new QVBoxLayout();
    box_empty->setContentsMargins(0,0,0,0);
    box_empty->setSpacing(0);
    box_empty->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    box_empty->addWidget(lb_empty,0, Qt::AlignCenter);

    widgetEmpty = new QWidget();
    widgetEmpty->setLayout(box_empty);
    widgetEmpty->hide();



    flowLayout = new FlowLayout(0,20,20);
    flowLayout->setContentsMargins(0,0,0,10);


    QWidget *wg_contents = new QWidget;
    wg_contents->setLayout(flowLayout);
    wg_contents->setContentsMargins(0,0,0,0);

    scrollArea_content = new QScrollArea();
    scrollArea_content->setWidget(wg_contents);
    scrollArea_content->setWidgetResizable(true);
    scrollArea_content->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_content->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_content->setStyleSheet("border:0px;");

    QVBoxLayout *boxMain = new QVBoxLayout;
    boxMain->setContentsMargins(33,42,28,0);
    boxMain->addWidget(scrollArea_content);
    boxMain->addWidget(widgetEmpty);

    setLayout(boxMain);
}

/**
 * @brief Podcast_Search::showEmptyLabel : empty 라벨 Show
 * @param bool if true show
 */
void Podcast_Search::showEmptyLabel(bool p_flagShow){
    if(p_flagShow){
        widgetEmpty->setVisible(true);
        scrollArea_content->setVisible(false);
    }else{
        widgetEmpty->setVisible(false);
        scrollArea_content->setVisible(true);
    }
}

/**
 * @brief Podcast_Search::search : 팟 캐스트 검색
 * @param p_text
 */
void Podcast_Search::search(const QString &p_text){

    strSearchWord = p_text;

    emit signal_changedMenuName("'"+p_text+"' "+tr("Search Results"));
    //emit signal_changedMenuName("'"+p_text+"' "+tr("검색결과"));

    requestChannel();
}

/**
 * @brief Podcast_Search::requestChannel : 팟캐스트 채널 요청
 */
void Podcast_Search::requestChannel(){
    GSCommon::clearLayout(flowLayout);
    showEmptyLabel(false);


    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;

    network->request(HTTP_NETWORK_CHANNEL_LIST, QString("%1?term=%2&limit=20&entry=podcasts&media=podcast&country=%3")
                     .arg(global.itunes_search)
                     .arg(strSearchWord)
                     .arg("kr")
                     , params, true);
}

/**
 * @brief Podcast_Search::setResultOfChannelList : 팟캐스트 검색요청결과 처리
 * @param p_jsonObject
 */
void Podcast_Search::setResultOfChannelList(const QJsonObject &p_jsonObject){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_results = "results";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_results)){
            QJsonArray jsonArr = p_jsonObject[jsonKey_results].toArray();

            for(int i = 0 ; i < jsonArr.size(); i++){
                appendChannel(jsonArr.at(i).toObject());
            }
        }
    }

    if(flowLayout->count() > 0){
        showEmptyLabel(false);
    }else{
        showEmptyLabel(true);
    }
}

/**
 * @brief Podcast_Search::appendChannel : 채널 추가
 * @param jsonData QjsonObject 채널 정보
 */
void Podcast_Search::appendChannel(const QJsonObject &jsonData){

    FramePodcast *tmp_widget = new FramePodcast;
    tmp_widget->setData(jsonData);
    tmp_widget->setHover();
    tmp_widget->setCursor(Qt::PointingHandCursor);

    flowLayout->addWidget(tmp_widget);

}

/**
 * @brief Podcast_Search::slot_responseHttp : [슬롯] Http 요청 결과
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void Podcast_Search::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id==HTTP_NETWORK_CHANNEL_LIST){
        setResultOfChannelList(p_jsonObject);
    }

    sender()->deleteLater();
}
