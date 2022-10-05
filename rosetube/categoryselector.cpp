#include "categoryselector.h"
#include "common/networkhttp.h"
#include "common/global.h"
#include "common/gscommon.h"

#include <QJsonArray>
#include <QScrollArea>
#include <QPushButton>

const int HTTP_CATEGORY = 99;

//const QString PAGECODE_VA_CATEGORY_DETAILS = "viewAll_category_detials";

CategorySelector::CategorySelector(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief CategorySelector::setInit : 초기 세팅
 */
void CategorySelector::setInit(){
}

/**
 * @brief CategorySelector::setUIControl : UI 세팅
 */
void CategorySelector::setUIControl(){
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

void CategorySelector::setData(const QJsonObject &p_jsonObject){
    const QString jsonKey_index = "index";
    const QString jsonKey_type = "type";

    QString tmp_type = "";
    int tmp_index= -1;
    if(p_jsonObject.contains(jsonKey_index)){
        tmp_index = p_jsonObject[jsonKey_index].toInt();
    }
    if(p_jsonObject.contains(jsonKey_type)){
        tmp_type = p_jsonObject[jsonKey_type].toString();
    }

    if(type!=tmp_type || (index != tmp_index && tmp_index >=0)){
        type = tmp_type;
        index = tmp_index;
        setDataFromAPI();
    }
}

/**
 * @brief CategorySelector::setDataFromAPI : 카테고리 데이터 세팅
 */
void CategorySelector::setDataFromAPI(){

    GSCommon::clearLayout(flowLayout);

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));


    QUrlQuery params;
    params.addQueryItem("name","rosetube_explore");

    network->request(HTTP_CATEGORY, QString("%1/menu/get")
                     .arg(global.legacy_mod), params, true,true);
}

/**
 * @brief CategorySelector::appendCategoryUI : 카테고리 UI 세팅
 * @param p_jsonArray
 */
void CategorySelector::appendCategoryUI(const QJsonArray &p_jsonArray){
    const QString jsonKey_id = "id";
    const QString jsonKey_nm = "nm";

    for(int i = 0; i < p_jsonArray.size(); i++){
        QJsonObject jsonData = p_jsonArray.at(i).toObject();

        int tmp_id = 0;
        QString tmp_nm = "";
        if(jsonData.contains(jsonKey_id)){
            tmp_id = jsonData[jsonKey_id].toInt();
        }
        if(jsonData.contains(jsonKey_nm)){
            tmp_nm = jsonData[jsonKey_nm].toString();
        }

        QString btn_style = "QPushButton{background-color:transparent;";
        btn_style += "text-align:left;";
        btn_style += "padding-left:5px;padding-top:29px;padding-bottom:21px;";
        btn_style += "color:#FFFFFF;font-size:20px;";
        btn_style += "border:0px;";
        btn_style += "border-bottom:5px solid #303030;}";
        btn_style += "QPushButton:hover{ background-color:#333333;}";

        QPushButton *btn = new QPushButton;
        btn->setText(tmp_nm);
        btn->setStyleSheet(btn_style);
        btn->setProperty("menu_id", tmp_id);
        btn->setProperty("nm", tmp_nm);
        btn->setProperty("title", tmp_nm);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedWidth(522);
        btn->setFixedHeight(70);
        btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);


        connect(btn, SIGNAL(clicked()), SLOT(slot_clickedItem()));

        flowLayout->addWidget(btn);
    }
}

/**
 * @brief CategorySelector::setResultOfCategory : 카테고리 요청결과 처리
 * @param p_jsonObject
 */
void CategorySelector::setResultOfCategory(const QJsonObject &p_jsonObject){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_submenu = "submenu";
    const QString jsonKey_type = "type";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_submenu)){
            QJsonArray jsonArrSub = p_jsonObject[jsonKey_submenu].toArray();

            for(int i = 0 ; i < jsonArrSub.size(); i++){
                QJsonObject jsonCategory = jsonArrSub.at(i).toObject();

                if(jsonCategory.contains(jsonKey_type) && jsonCategory[jsonKey_type].toString().toLower()=="keyword"){
                      if(index==i) {
                          appendCategoryUI(jsonCategory[jsonKey_submenu].toArray());
                      }
                }
            }
        }
    }
}

/**
 * @brief CategorySelector::slot_responseHttp : [슬롯] Http 요청결과 처리
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void CategorySelector::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id == HTTP_CATEGORY){
        setResultOfCategory(p_jsonObject);
    }

    sender()->deleteLater();
}

/**
 * @brief CategorySelector::slot_clickedItem : [슬롯] 아이템 클릭
 */
void CategorySelector::slot_clickedItem(){


    QJsonObject tmp_data;
    tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_CATEGORY_DETAILS);
    tmp_data.insert("menu_id", sender()->property("menu_id").toInt());
    tmp_data.insert("nm", sender()->property("nm").toString());
    tmp_data.insert("title", sender()->property("title").toString());

    // 부모에게 페이지 변경하라고 시그널 보냄
    emit this->signal_clickedViewAll(tmp_data);
}


