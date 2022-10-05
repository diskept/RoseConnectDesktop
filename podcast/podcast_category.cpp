#include "podcast_category.h"
#include "common/global.h"
#include "common/rosesettings.h"
#include "common/networkhttp.h"
#include "common/gscommon.h"
#include <QSettings>
#include <QScrollArea>
#include <QPushButton>

const int HTTP_NETWORK_CATEGORY_LIST = 99;

/**
 * @brief Podcast_Category::PodcastCategory : 팟캐스트-카테고리
 * @details 카테고리 목록 GET API https://www.hifimp.com:18081/v2/rest/rose/IF_ROSEPODCAST_001<br>
 * @param parent
 */
Podcast_Category::Podcast_Category(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief Podcast_Category::setInit : 초기 세팅
 */
void Podcast_Category::setInit(){

}

/**
 * @brief Podcast_Category::setUIControl : UI 세팅
 */
void Podcast_Category::setUIControl(){
    flowLayout = new FlowLayout(0,21,0);
    flowLayout->setSizeConstraint(QLayout::SetNoConstraint);

    QWidget *wg_flow = new QWidget;
    wg_flow->setContentsMargins(0,0,0,0);
    wg_flow->setLayout(flowLayout);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(wg_flow);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("#scrollArea { border:0px; }");

    QVBoxLayout *box_total = new QVBoxLayout;
    box_total->setContentsMargins(33,11,33,0);
    box_total->setSpacing(0);
    box_total->addWidget(scrollArea);

    setLayout(box_total);
}

/**
 * @brief Podcast_Category::setResultOfCategory : 카테고리 목록 요청결과 처리
 * @param p_jsonData QJsonObject response
 */
void Podcast_Category::setResultOfCategory(const QJsonObject &p_jsonData){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_resultCd = "resultCd";
    const QString jsonKey_data = "data";
    const QString jsonKey_genreID = "genreID";
    const QString jsonKey_genreName = "genreName";
   // print_debug();
   // QJsonDocument doc(p_jsonData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"Podcast_Category::setResultOfCategory---" << strJson;

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonData[jsonKey_status].toObject();
            if(jsonStatus.contains(jsonKey_resultCd) && jsonStatus[jsonKey_resultCd].toString().toLower()=="success"){
                if(p_jsonData.contains(jsonKey_data)){
                    QJsonArray jsonArrData = p_jsonData[jsonKey_data].toArray();

                    for(int i = 0; i < jsonArrData.size(); i++){
                        QJsonObject jsonCategory = jsonArrData.at(i).toObject();

                        if(jsonCategory.contains(jsonKey_genreID) && jsonCategory.contains(jsonKey_genreName)){

                            QString tmp_str = jsonCategory[jsonKey_genreName].toString();
                            //qDebug() << tmp_str;
                            appendCategoryUI(jsonCategory[jsonKey_genreID].toInt(), tmp_str);
                        }
                    }
                }
            }
        }
    }
}

/**
 * @brief Podcast_Category::appendCategoryUI : 카테고리 UI 추가
 * @param p_id int 카테고리 아이디
 * @param p_name QString 카테고리명
 */
void Podcast_Category::appendCategoryUI(const int &p_id, const QString &p_name){
    QString btn_style = "ClickableLabel{background-color:transparent;";
    btn_style += "text-align:left top;";
    btn_style += "padding-left:5px;padding-top:29px;";
    btn_style += "color:#FFFFFF;font-size:20px;";
    btn_style += "border:0px;";
    btn_style += "border-bottom:2px solid #303030;}";
    btn_style += "QPushButton:hover{ background-color:#333333;}";
    qDebug() << "p_name = " << p_name;

    ClickableLabel *btn = new ClickableLabel();
    btn->setText(p_name.toUtf8());
    btn->setStyleSheet(btn_style);
    btn->setProperty("genreID", p_id);
    btn->setProperty("collectionName", p_name);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFixedWidth(522);
    btn->setFixedHeight(70);
    connect(btn, SIGNAL(signal_clicked()), SLOT(slot_clickedItem()));

    flowLayout->addWidget(btn);
}

/**
 * @brief Podcast_Category::requestData : 카테고리 데이터 요청
 */
void Podcast_Category::requestData(bool flagNeedRefresh){

    if(flowLayout->count() ==0 || flagNeedRefresh){
        GSCommon::clearLayout(flowLayout);

        QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
        int tmp_countryID = settings->value(rosesettings.SETTINGS_PODCAST_COUNTRY,2).toInt();
        delete settings;

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));


        QJsonObject json;
        json.insert("countryID", tmp_countryID);
        json.insert("roseToken", global.device.getDeviceRoseToken());


        network->request(HTTP_NETWORK_CATEGORY_LIST, QString("%1/IF_ROSEPODCAST_001")
                         //.arg(global.hifimp)
                         .arg(global.release_roseaudio)
                         , json, true);
    }
}


/**
 * @brief Podcast_Category::slot_responseHttp : [슬롯] Http 요청 결과
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void Podcast_Category::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    if(p_id==HTTP_NETWORK_CATEGORY_LIST){
        setResultOfCategory(p_jsonObject);
    }

   sender()->deleteLater();
}

/**
 * @brief Podcast_Category::slot_clickedItem : [슬롯] 아이템 클릭
 */
void Podcast_Category::slot_clickedItem(){

    QJsonObject tmp_data;
    tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_PC_CATEGORY_DETAIL);
    tmp_data.insert("genreID", sender()->property("genreID").toInt());
    tmp_data.insert("collectionName", sender()->property("collectionName").toString());

    emit this->signal_reqSubPage(tmp_data);
}
