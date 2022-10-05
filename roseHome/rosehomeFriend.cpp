#include "friendprofileitemRoseHome.h"
#include "rosehomeFriend.h"
#include "widget/toastmsg.h"
#include "common/global.h"

#include <common/gscommon.h>
#include <common/networkhttp.h>

#include <QKeyEvent>
#include <QScrollBar>

namespace roseHome {


const QString SF_TAB_STEP_FRIEND = "friend";        ///< 친구목록 TAB
const QString SF_TAB_STEP_RECEIVE = "receive";      ///< 받은요청 TAB
const QString SF_TAB_STEP_SEND = "send";            ///< 보낸요청 TAB
const QString SF_TAB_STEP_FIND = "find";            ///< 친구찾기 TAB

int const  HTTP_GET_FREIND_LIST = 99;
int const  HTTP_GET_RECEIVED_REQUEST = 98;
int const  HTTP_GET_SEND_FRIEND = 97;
int const  HTTP_GET_FIND_FRIEND = 96;
int const HTTP_SEARCH_FRIEND = 95;
/**
 * @brief RoseHomeFriend::RoseHomeFriend 생성자
 * @param parent
 */
RoseHomeFriend::RoseHomeFriend(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll, parent){

    this->setUIControl();
}


/**
 * @brief 소멸자.
 */
RoseHomeFriend::~RoseHomeFriend(){

    //print_debug();
    //GSCommon::clearLayout(this->hbox_UserPickPlaylist);
    //print_debug();
    //this->list_UserPickPlaylist->clear();
    //print_debug();

}

/**
 * @brief 타이달 > 탐색 > "장르" 1개 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
 * HTTP API 호출한다.
 */
void RoseHomeFriend::setActivePage(){//cheon211018

    // 항상 부모클래스의 함수 먼저 호출
    AbstractRoseHomeSubWidget::setActivePage();

    if(this->flagInitDraw){
        this->flagInitDraw = false;

/*
        QJsonObject json;
        QUrlQuery params;
        NetworkHttp *network_friend = new NetworkHttp(this);
        connect(network_friend, SIGNAL(response(int,QJsonObject)), SLOT(this->slot_responseHttp_friend(int, QJsonObject)));
        network_friend->request(HTTP_GET_FREIND_LIST, QString("%1/member/friend").arg(global.legacy_v1), params, false, true);
*/

    }
    qDebug() << "this->contentStep" << this->contentStep;
    if(this->contentStep==""){
        this->contentStep = SF_TAB_STEP_FRIEND;
        //this->contentStep = SF_TAB_STEP_SEND;//SF_TAB_STEP_SEND
    }

    // step 있는 경우
    this->changedOnlyTabUI_notSendSignal(this->contentStep);
}

void RoseHomeFriend::setUIControl()
{
    this->flagInitDraw = true;
    // tab
    QJsonArray *p_jsonArray_titlSub = new QJsonArray();
    QJsonObject sub1 { {"name", "Friend List"}, {"code", SF_TAB_STEP_FRIEND} };
    QJsonObject sub2 { {"name", "Recieved Request"}, {"code", SF_TAB_STEP_RECEIVE} };
    QJsonObject sub3 { {"name", "Sended Request"}, {"code", SF_TAB_STEP_SEND} };
    QJsonObject sub4 { {"name", "Friend Search"}, {"code", SF_TAB_STEP_FIND} };
    /*
     *     QJsonObject sub1 { {"name", "친구목록"}, {"code", SF_TAB_STEP_FRIEND} };
    QJsonObject sub2 { {"name", "받은요청"}, {"code", SF_TAB_STEP_RECEIVE} };
    QJsonObject sub3 { {"name", "보낸요청"}, {"code", SF_TAB_STEP_SEND} };
    QJsonObject sub4 { {"name", "친구찾기"}, {"code", SF_TAB_STEP_FIND} };
    */
    p_jsonArray_titlSub->push_back(sub1);
    p_jsonArray_titlSub->push_back(sub2);
    p_jsonArray_titlSub->push_back(sub3);
    p_jsonArray_titlSub->push_back(sub4);
    this->menubar = new TopMenuBar(TopMenuBar::ShowMenuStyle::FriendSubmenubar);//cheon211018
    this->menubar->setDataTopMenuBar(QJsonObject(), p_jsonArray_titlSub);
    this->menubar->setStyleSheet("background-color:transparent;border-bottom:5px solid #333333;");//B18658


    this->vl_friend = new QVBoxLayout();
    this->vl_friend->setContentsMargins(0,0,0,0);
    this->vl_friend->setSpacing(0);
    this->vl_friend->setAlignment(Qt::AlignTop);
    this->vl_receive = new QVBoxLayout();
    this->vl_receive->setContentsMargins(0,0,0,0);
    this->vl_receive->setSpacing(0);
    this->vl_receive->setAlignment(Qt::AlignTop);
    this->vl_send = new QVBoxLayout();
    this->vl_send->setContentsMargins(0,0,0,0);
    this->vl_send->setSpacing(0);
    this->vl_send->setAlignment(Qt::AlignTop);
    this->vl_find = new QVBoxLayout();
    this->vl_find->setContentsMargins(0,0,0,0);
    this->vl_find->setSpacing(0);
    this->vl_find->setAlignment(Qt::AlignTop);
    this->le_search = new QLineEdit();
    this->le_search->setContentsMargins(16,0,0,0);
    this->le_search->setTextMargins(40,12,12,12);
    this->le_search->setFixedHeight(40);
    this->le_search->setMaxLength(20);
    this->le_search->setStyleSheet("font-size:15px;color:#333333;background-color:#FFFFFF;");
    this->le_search->setPlaceholderText(tr("Search for friends by nickname."));
    //this->le_search->setPlaceholderText(tr("닉네임으로 친구를 검색하세요."));
    QLabel *lb_search  = GSCommon::getUILabelImg(":/images/icon_ser.png");
    lb_search->setStyleSheet("background-color:rgba(100,0,0,0);padding-left:20px;");

    QPushButton *btn_search = new QPushButton(tr("Search"));
    //QPushButton *btn_search = new QPushButton(tr("검색"));
    btn_search->setStyleSheet("color:#B18658;font-size:18px;padding-right:20px;");
    btn_search->setCursor(Qt::PointingHandCursor);
    QGridLayout *gl_search = new QGridLayout();
    gl_search->setContentsMargins(0,0,0,0);
    gl_search->setSpacing(0);
    gl_search->addWidget(le_search, 0, 0);
    gl_search->addWidget(lb_search, 0, 0, Qt::AlignLeft);
    gl_search->addWidget(btn_search, 0, 0, Qt::AlignRight);

    QVBoxLayout *vl_find_total = new QVBoxLayout();
    vl_find_total->setContentsMargins(0,0,0,0);
    vl_find_total->setSpacing(0);
    vl_find_total->setAlignment(Qt::AlignTop);
    vl_find_total->addSpacing(10);
    vl_find_total->addLayout(gl_search);
    vl_find_total->addSpacing(15);
    vl_find_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("Recommended Friends")));
    //vl_find_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("추천 친구")));
    vl_find_total->addSpacing(15);
    vl_find_total->addLayout(this->vl_find);

    QWidget *widget_friend = new QWidget();
    QWidget *widget_receive = new QWidget();
    QWidget *widget_send = new QWidget();
    QWidget *widget_find = new QWidget();
    widget_friend->setLayout(this->vl_friend);
    widget_receive->setLayout(this->vl_receive);
    widget_send->setLayout(this->vl_send);
    widget_find->setLayout(vl_find_total);
    this->stackedwidget = new QStackedWidget();
    this->stackedwidget->addWidget(getUIScrollViewWidget(widget_friend));
    this->stackedwidget->addWidget(getUIScrollViewWidget(widget_receive));
    this->stackedwidget->addWidget(getUIScrollViewWidget(widget_send));
    this->stackedwidget->addWidget(getUIScrollViewWidget(widget_find));

    QLabel *line_bottom_1 = new QLabel();//cheon211020
    line_bottom_1->setFixedHeight(10);
    line_bottom_1->setStyleSheet("background-color:transparent;");


    // 전체
    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,80);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    //vl_total->addWidget(this->menubar);


    vl_total->addWidget(this->stackedwidget);

    // 스크롤
    QWidget *widget_scrollArea = new QWidget();
    widget_scrollArea->setContentsMargins(0,0,0,0);
    widget_scrollArea->setObjectName("widget_scrollArea");
    widget_scrollArea->setLayout(vl_total);
    widget_scrollArea->setStyleSheet("#widget_scrollArea { border-left:1px solid #303030; }");

    scrollArea = new QScrollArea();
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(widget_scrollArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("#scrollArea { border:0px; }");



    QVBoxLayout *vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(40,0,300,0);//friend list submenu title width//cheon211114-01
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(this->menubar);
    vl_scroll->addWidget(line_bottom_1);//cheon211020

    vl_scroll->addWidget(scrollArea);

    //this->setLayout(vl_scroll);
    this->box_contents->addLayout(vl_scroll);
print_debug();
    // 커넥션
    connect(this->menubar, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(slot_changedSubTabUI(QJsonObject)));
    connect(this->le_search, &QLineEdit::returnPressed, this, &RoseHomeFriend::slot_searchFriend);
    connect(btn_search, &QPushButton::clicked, this, &RoseHomeFriend::slot_searchFriend);
}

QWidget* RoseHomeFriend::getUIScrollViewWidget(QWidget *p_widget){

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName("scrollArea_in");
    scrollArea->setWidget(p_widget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("#scrollArea_in { border:0px; }");

    QVBoxLayout *vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);
    QWidget *widget_scrollArea = new QWidget();
    widget_scrollArea->setLayout(vl_scroll);

    return widget_scrollArea;
}

/**
 * @brief RoseHomeFriend::setDataABSPage [override] 외부에서 데이터 세팅
 * @param p_data
 */
void RoseHomeFriend::setDataABSPage(QJsonObject p_data){

    // 공통 데이터 세팅
    AbstractRoseHomeSubWidget::setDataABSPage(p_data);
    print_debug();
    qDebug() << "this->contentStep" << this->contentStep;
    if(this->contentStep==""){
        this->contentStep = SF_TAB_STEP_FRIEND;
        //this->contentStep = SF_TAB_STEP_SEND;//SF_TAB_STEP_SEND
    }

    // step 있는 경우
    this->changedOnlyTabUI_notSendSignal(this->contentStep);
}

/**
 * @brief RoseHomeFriend::changedSubTab [SLOT][override]
 * @param p_data
 */
void RoseHomeFriend::slot_changedSubTabUI(const QJsonObject &p_data){

    QString tmp_step = p_data[KEY_PAGE_CODE].toString();

    // 페이지 변경 및 데이터 세팅
    this->changedOnlyTabUI_notSendSignal(tmp_step);

    // 시그널 발생 : 페이지가 변경되었다고 부모에게 알려준다.
    QJsonObject tmp_data = this->getDataJson();
    tmp_data[KEY_CONTENT_STEP] = tmp_step;
    emit clickedSubTab(tmp_data);
}

/**
 * @brief RoseHomeFriend::getDataJson [override]
 * @return
 */
QJsonObject RoseHomeFriend::getDataJson(){

    // 공통 데이터 반환
    QJsonObject tmp_data = AbstractRoseHomeSubWidget::getDataJson();

    // 데이터 반환
    /*
    tmp_data[KEY_OP_TYPE] = this->type;
    tmp_data[KEY_OP_albumImg] = this->albumImg;
    tmp_data[KEY_OP_cntStar] = this->cntStar;
    QJsonArray tmp_array;
    for(int i=0; i<this->list_audioInfo.count(); i++){
        tmp_array.append(this->list_audioInfo.at(i)->getJsonData());
    }
    tmp_data[KEY_OP_DATA] = tmp_array;
    */
    return tmp_data;
}

/**
 * @brief RoseHomeFriend::changedOnlyTabUI_notSendSignal 페이지 변경 및 데이터 세팅 (부모에게 페이지 변경되엇다는 시그널 없음)
 * @param p_step
 */
void RoseHomeFriend::changedOnlyTabUI_notSendSignal(QString p_step){

    this->contentStep = p_step;

    // Tab 타이틀 변경
    this->menubar->setSelectedSubMenuNoSignal(this->contentStep);

    // DB 가져와서 데이터 세팅
    if(this->contentStep==SF_TAB_STEP_FRIEND){
        print_debug();
        this->requestFriendList();
        this->stackedwidget->setCurrentIndex(0);
    }else if(this->contentStep==SF_TAB_STEP_RECEIVE){
        this->requestReceivedRequest();
        this->stackedwidget->setCurrentIndex(1);
    }else if(this->contentStep==SF_TAB_STEP_SEND){
        this->requestReceivedSend();
        this->stackedwidget->setCurrentIndex(2);
    }else if(this->contentStep==SF_TAB_STEP_FIND){
        this->le_search->setText("");
        this->requestReceivedFind();
        this->stackedwidget->setCurrentIndex(3);
    }
    this->scrollArea->verticalScrollBar()->setValue(0);
}

/**
 * @brief RoseHomeFriend::requestFriendList GET 친구 목록
 */
void RoseHomeFriend::requestFriendList(){//cheon211018
/*
    NetworkHttp *network = new NetworkHttp();
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("username",global.user.getUsername());
    network->request(HTTP_GET_FREIND_LIST, QString("%1/user/friend/fetch")
                     .arg(global.legacy_mod_api), params, true,true);
*/
    this->tmp_totalCount = 0, this->next = 0, totalCount = 0;
    print_debug();
    qDebug() << QString("%1/member/friend?page=0&size=20").arg(global.legacy_v1);


    //NetworkHttp *network_friend = new NetworkHttp();
    //connect(network_friend, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    //network_friend->request(HTTP_GET_FREIND_LIST, QString("%1/member/friend?page=0&size=20").arg(global.legacy_v1), json, false, true);

    QUrlQuery params;
    QJsonObject json;

    //json.insert("page", 0);
    //json.insert("size", 20);
    params.addQueryItem("page", "0");
    params.addQueryItem("size", "20");

        NetworkHttp *network_friend = new NetworkHttp();

        //while(this->totalCount == 0 ||this->tmp_totalCount < this->totalCount){?page=0&size=20
        qDebug() << "totalCount = " << totalCount;
        qDebug() << "tmp_totalCount = " << tmp_totalCount;
        connect(network_friend, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_friend(int, QJsonObject)));
        network_friend->request(HTTP_GET_FREIND_LIST, QString("%1/member/friend").arg(global.legacy_v1), params, false, true);

        GSCommon::clearLayout(this->vl_friend);


}
/**
 * @brief RoseHomeFriend::requestReceivedRequest GET 받은 요청
 */
void RoseHomeFriend::requestReceivedRequest(){
/*
    NetworkHttp *network = new NetworkHttp();
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("username",global.user.getUsername());
    network->request(HTTP_GET_RECEIVED_REQUEST, QString("%1/user/friend/get_received_requests")
                     .arg(global.legacy_mod_api), params, true,true);
*/
    this->tmp_totalCount = 0, this->next = 0, totalCount = 0;
    QUrlQuery params;
    QJsonObject json;

    //json.insert("page", 0);
    //json.insert("size", 20);
    params.addQueryItem("page", "0");
    params.addQueryItem("size", "20");

        NetworkHttp *network_friend = new NetworkHttp();

        //while(this->totalCount == 0 ||this->tmp_totalCount < this->totalCount){?page=0&size=20
        qDebug() << "totalCount = " << totalCount;
        qDebug() << "tmp_totalCount = " << tmp_totalCount;
        connect(network_friend, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_friend(int, QJsonObject)));
        network_friend->request(HTTP_GET_RECEIVED_REQUEST, QString("%1/member/friend/recieve").arg(global.legacy_v1), params, false, true);
        GSCommon::clearLayout(this->vl_receive);

}

/**
 * @brief RoseHomeFriend::requestReceivedSend GET 보낸요청
 */
void RoseHomeFriend::requestReceivedSend(){
/*
    NetworkHttp *network = new NetworkHttp();
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("username",global.user.getUsername());
    network->request(HTTP_GET_SEND_FRIEND, QString("%1/user/friend/get_sent_requests")
                     .arg(global.legacy_mod_api), params, true,true);
*/
    this->tmp_totalCount = 0, this->next = 0, totalCount = 0;
    QUrlQuery params;
    QJsonObject json;

    //json.insert("page", 0);
    //json.insert("size", 20);
    params.addQueryItem("page", "0");
    params.addQueryItem("size", "20");

        NetworkHttp *network_friend = new NetworkHttp();

        //while(this->totalCount == 0 ||this->tmp_totalCount < this->totalCount){?page=0&size=20
        qDebug() << "totalCount = " << totalCount;
        qDebug() << "tmp_totalCount = " << tmp_totalCount;
        connect(network_friend, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_friend(int, QJsonObject)));
        network_friend->request(HTTP_GET_SEND_FRIEND, QString("%1/member/friend/request").arg(global.legacy_v1), params, false, true);

    GSCommon::clearLayout(this->vl_send);

}

/**
 * @brief RoseHomeFriend::requestReceivedFind GET 친구찾기
 */
void RoseHomeFriend::requestReceivedFind(){
/*
    NetworkHttp *network = new NetworkHttp();
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("username",global.user.getUsername());
    network->request(HTTP_GET_FIND_FRIEND, QString("%1/user/friend/suggest")
                     .arg(global.legacy_mod_api), params, true,true);
*/
    this->tmp_totalCount = 0, this->next = 0, totalCount = 0;
    QUrlQuery params;
    QJsonObject json;

    //json.insert("page", 0);
    //json.insert("size", 20);
    params.addQueryItem("page", "0");
    params.addQueryItem("size", "20");

        NetworkHttp *network_friend = new NetworkHttp();

        //while(this->totalCount == 0 ||this->tmp_totalCount < this->totalCount){?page=0&size=20
        qDebug() << "totalCount = " << totalCount;
        qDebug() << "tmp_totalCount = " << tmp_totalCount;
        connect(network_friend, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_friend(int, QJsonObject)));
        network_friend->request(HTTP_GET_FIND_FRIEND, QString("%1/member/friend/recommend").arg(global.legacy_v1), params, false, true);
    GSCommon::clearLayout(this->vl_find);
}

/**
 * @brief RoseHomeFriend::slot_searchFriend [SLOT] GET 친구검색
 */
void RoseHomeFriend::slot_searchFriend(){

    if(this->le_search->text().trimmed()==""){
        print_debug();
        this->requestReceivedFind();
        //ToastMsg::show(this, "", tr("Please enter a friend nickname to search for."));
        //ToastMsg::show(this, "", tr("검색할 친구 닉네임을 입력해주세요."));
        //this->le_search->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
    }else{
        print_debug();
        GSCommon::clearLayout(this->vl_find);
        /*
        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("username",global.user.getUsername());
        params.addQueryItem("keyword", this->le_search->text().trimmed());
        network->request(HTTP_SEARCH_FRIEND, QString("%1/user/friend/search")
                         .arg(global.legacy_mod_api), params, true,true);
                         */

        QUrlQuery params;
        QJsonObject json;

        json.insert("page", "0");
        json.insert("size", "20");
        params.addQueryItem("page", "0");
        params.addQueryItem("size", "20");

        //params.addQueryItem("name",global.user.getUsername());
        qDebug() << "this->le_search->text().trimmed()--" << this->le_search->text().trimmed();
        json.insert("nickname", this->le_search->text().trimmed());
        params.addQueryItem("nickname", this->le_search->text().trimmed());

            NetworkHttp *network_friend = new NetworkHttp();

            //while(this->totalCount == 0 ||this->tmp_totalCount < this->totalCount){?page=0&size=20
            qDebug() << "totalCount = " << totalCount;
            qDebug() << "tmp_totalCount = " << tmp_totalCount;
            connect(network_friend, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_friend(int, QJsonObject)));
            network_friend->request(HTTP_SEARCH_FRIEND, QString("%1/member/member/find/nickname").arg(global.legacy_v1), params, false, true);

    }
}

/**
 * @brief SettingFriendPage::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void RoseHomeFriend::slot_responseHttp_friend(const int &p_id, const QJsonObject &p_jsonObject){//cheon211018
    //int totalCount = 0;
    //QJsonArray jsonArruserpick = ProcJsonEasy::getJsonArray(p_jsonObject, "userPicks");
    //int userpick_cnt = jsonArruserpick.count();
    //print_debug();
    //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "SettingFriendPage::slot_responseHttp--p_jsonObject= " << strJson;
    QJsonArray jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObject, "members");
    //print_debug();
    qDebug()<<"\nRoseHomeUserPick::setResultOfUserPickPlaylist---jsonArrSub.count()---"<<jsonArrSub.count();
    if(p_jsonObject.contains("totalCount")){
        this->totalCount = p_jsonObject["totalCount"].toInt();
    }

    switch(p_id){
    case HTTP_GET_FREIND_LIST :
        /*
        GSCommon::clearLayout(this->vl_friend);
        if(p_jsonObject.contains("array")){
            QJsonArray tmp_array = p_jsonObject["array"].toArray();
            for(int i=0; i<tmp_array.count(); i++){
                FriendProfileItem *frendprofileItem = new FriendProfileItem(FriendProfileItem::ProfileBtnType::FriendList);
                connect(frendprofileItem, &FriendProfileItem::signal_removeItem, this, &SettingFriendPage::slot_removeRow);
                frendprofileItem->setDataProfileItem(tmp_array[i].toObject());
                this->vl_friend->addWidget(frendprofileItem);
            }
        }
        */


        if(jsonArrSub.size() > 0){

            for(int i = 0; i < jsonArrSub.count(); i++){
                QJsonObject tmp_json = jsonArrSub.at(i).toObject();
                //print_debug();
                //QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nSettingFriendPage::slot_responseHttp--tmp_json= " << strJson;

                FriendProfileItemRoseHome *frendprofileItem = new FriendProfileItemRoseHome(FriendProfileItemRoseHome::ProfileBtnType::FriendList);
                connect(frendprofileItem, &FriendProfileItemRoseHome::signal_removeItem, this, &RoseHomeFriend::slot_removeRow);
                frendprofileItem->setDataProfileItem(tmp_json);
                this->vl_friend->addWidget(frendprofileItem);
            }
            this->tmp_totalCount += jsonArrSub.size();
        }
        qDebug() << "totalCount = " << this->totalCount;
        qDebug() << "tmp_totalCount = " << this->tmp_totalCount;

        if(this->totalCount > 0 && this->tmp_totalCount < this->totalCount){
            QJsonObject json;
            QUrlQuery params;
            //json.insert("page", ++this->next);
            //json.insert("size", 20);
            params.addQueryItem("page", QString("%1").arg(++this->next));
            params.addQueryItem("size", "20");

            qDebug() << "*totalCount = " << totalCount;
            qDebug() << "*tmp_totalCount = " << this->tmp_totalCount;
            NetworkHttp *network_friend = new NetworkHttp();
            connect(network_friend, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_friend(int, QJsonObject)));
            network_friend->request(HTTP_GET_FREIND_LIST, QString("%1/member/friend").arg(global.legacy_v1), params, false, true);

        }

        break;
    case HTTP_GET_RECEIVED_REQUEST :


        /*
        GSCommon::clearLayout(this->vl_receive);
        if(p_jsonObject.contains("array")){
            QJsonArray tmp_array = p_jsonObject["array"].toArray();
            for(int i=0; i<tmp_array.count(); i++){
                FriendProfileItem *frendprofileItem = new FriendProfileItem(FriendProfileItem::ProfileBtnType::Receive);
                connect(frendprofileItem, &FriendProfileItem::signal_removeItem, this, &SettingFriendPage::slot_removeRow);
                frendprofileItem->setDataProfileItem(tmp_array[i].toObject());
                this->vl_receive->addWidget(frendprofileItem);
            }
        }
        */
        GSCommon::clearLayout(this->vl_receive);
        print_debug();
        if(jsonArrSub.size() > 0){

            for(int i = 0; i < jsonArrSub.count(); i++){
                QJsonObject tmp_json = jsonArrSub.at(i).toObject();
                //print_debug();
                //QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nSettingFriendPage::slot_responseHttp--tmp_json= " << strJson;

                FriendProfileItemRoseHome *frendprofileItem = new FriendProfileItemRoseHome(FriendProfileItemRoseHome::ProfileBtnType::Receive);
                connect(frendprofileItem, &FriendProfileItemRoseHome::signal_removeItem, this, &RoseHomeFriend::slot_removeRow);
                frendprofileItem->setDataProfileItem(tmp_json);
                this->vl_receive->addWidget(frendprofileItem);
            }
            this->tmp_totalCount += jsonArrSub.size();
        }
        qDebug() << "totalCount = " << this->totalCount;
        qDebug() << "tmp_totalCount = " << this->tmp_totalCount;

        if(this->totalCount > 0 && this->tmp_totalCount < this->totalCount){
            QJsonObject json;
            QUrlQuery params;
            //json.insert("page", ++this->next);
            //json.insert("size", 20);
            params.addQueryItem("page", QString("%1").arg(++this->next));
            params.addQueryItem("size", "20");

            qDebug() << "*totalCount = " << totalCount;
            qDebug() << "*tmp_totalCount = " << this->tmp_totalCount;
            NetworkHttp *network_friend = new NetworkHttp();
            connect(network_friend, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_friend(int, QJsonObject)));
            network_friend->request(HTTP_GET_RECEIVED_REQUEST, QString("%1/member/friend/recieve").arg(global.legacy_v1), params, false, true);

        }
        break;
    case HTTP_GET_SEND_FRIEND:

        /*
        GSCommon::clearLayout(this->vl_send);
        if(p_jsonObject.contains("array")){
            QJsonArray tmp_array = p_jsonObject["array"].toArray();
            for(int i=0; i<tmp_array.count(); i++){
                FriendProfileItem *frendprofileItem = new FriendProfileItem(FriendProfileItem::ProfileBtnType::Send);
                connect(frendprofileItem, &FriendProfileItem::signal_removeItem, this, &SettingFriendPage::slot_removeRow);
                frendprofileItem->setDataProfileItem(tmp_array[i].toObject());
                this->vl_send->addWidget(frendprofileItem);
            }
        }
        */

        //print_debug();
        if(jsonArrSub.size() > 0){

            for(int i = 0; i < jsonArrSub.count(); i++){
                QJsonObject tmp_json = jsonArrSub.at(i).toObject();
                //print_debug();
                //QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nSettingFriendPage::slot_responseHttp--tmp_json= " << strJson;

                FriendProfileItemRoseHome *frendprofileItem = new FriendProfileItemRoseHome(FriendProfileItemRoseHome::ProfileBtnType::Send);
                connect(frendprofileItem, &FriendProfileItemRoseHome::signal_removeItem, this, &RoseHomeFriend::slot_removeRow);
                frendprofileItem->setDataProfileItem(tmp_json);
                this->vl_send->addWidget(frendprofileItem);
            }
            this->tmp_totalCount += jsonArrSub.size();
        }
        qDebug() << "totalCount = " << this->totalCount;
        qDebug() << "tmp_totalCount = " << this->tmp_totalCount;

        if(this->totalCount > 0 && this->tmp_totalCount < this->totalCount){
            QJsonObject json;
            QUrlQuery params;
            //json.insert("page", ++this->next);
            //json.insert("size", 20);
            params.addQueryItem("page", QString("%1").arg(++this->next));
            params.addQueryItem("size", "20");

            qDebug() << "*totalCount = " << totalCount;
            qDebug() << "*tmp_totalCount = " << this->tmp_totalCount;
            NetworkHttp *network_friend = new NetworkHttp();
            connect(network_friend, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_friend(int, QJsonObject)));
            network_friend->request(HTTP_GET_SEND_FRIEND, QString("%1/member/friend/request").arg(global.legacy_v1), params, false, true);

        }

        break;
    case HTTP_GET_FIND_FRIEND:
        /*
        GSCommon::clearLayout(this->vl_find);
        if(p_jsonObject.contains("array")){
            QJsonArray tmp_array = p_jsonObject["array"].toArray();
            for(int i=0; i<tmp_array.count(); i++){
                FriendProfileItem *frendprofileItem = new FriendProfileItem(FriendProfileItem::ProfileBtnType::Find);
                frendprofileItem->setDataProfileItem(tmp_array[i].toObject());
                this->vl_find->addWidget(frendprofileItem);
            }
        }
        */

        print_debug();
        if(jsonArrSub.size() > 0){

            for(int i = 0; i < jsonArrSub.count(); i++){
                QJsonObject tmp_json = jsonArrSub.at(i).toObject();
                //print_debug();
                //QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nSettingFriendPage::slot_responseHttp--tmp_json= " << strJson;

                FriendProfileItemRoseHome *frendprofileItem = new FriendProfileItemRoseHome(FriendProfileItemRoseHome::ProfileBtnType::Find);
                //connect(frendprofileItem, &FriendProfileItem::signal_removeItem, this, &SettingFriendPage::slot_removeRow);
                frendprofileItem->setDataProfileItem(tmp_json);
                this->vl_find->addWidget(frendprofileItem);
            }
            this->tmp_totalCount += jsonArrSub.size();
        }
        qDebug() << "totalCount = " << this->totalCount;
        qDebug() << "tmp_totalCount = " << this->tmp_totalCount;

        if(this->totalCount > 0 && this->tmp_totalCount < this->totalCount){
            QJsonObject json;
            QUrlQuery params;
            //json.insert("page", ++this->next);
            //json.insert("size", 20);
            params.addQueryItem("page", QString("%1").arg(++this->next));
            params.addQueryItem("size", "20");

            qDebug() << "*totalCount = " << totalCount;
            qDebug() << "*tmp_totalCount = " << this->tmp_totalCount;
            NetworkHttp *network_friend = new NetworkHttp();
            connect(network_friend, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_friend(int, QJsonObject)));
            network_friend->request(HTTP_GET_FIND_FRIEND, QString("%1/member/friend/recommend").arg(global.legacy_v1), params, true, true);

        }
        break;
    case HTTP_SEARCH_FRIEND:
        /*
        GSCommon::clearLayout(this->vl_find);
        if(p_jsonObject.contains("array")){
            QJsonArray tmp_array = p_jsonObject["array"].toArray();
            for(int i=0; i<tmp_array.count(); i++){
                FriendProfileItem *frendprofileItem = new FriendProfileItem(FriendProfileItem::ProfileBtnType::Find);
                frendprofileItem->setDataProfileItem(tmp_array[i].toObject());
                this->vl_find->addWidget(frendprofileItem);
            }
        }
        */
        //GSCommon::clearLayout(this->vl_find);
        print_debug();
        if(jsonArrSub.size() > 0){

            for(int i = 0; i < jsonArrSub.count(); i++){
                QJsonObject tmp_json = jsonArrSub.at(i).toObject();
                //print_debug();
                //QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nSettingFriendPage::slot_responseHttp--tmp_json= " << strJson;

                FriendProfileItemRoseHome *frendprofileItem = new FriendProfileItemRoseHome(FriendProfileItemRoseHome::ProfileBtnType::Find);
                //connect(frendprofileItem, &FriendProfileItem::signal_removeItem, this, &SettingFriendPage::slot_removeRow);
                frendprofileItem->setDataProfileItem(tmp_json);
                this->vl_find->addWidget(frendprofileItem);
            }
            this->tmp_totalCount += jsonArrSub.size();
        }
        qDebug() << "totalCount = " << this->totalCount;
        qDebug() << "tmp_totalCount = " << this->tmp_totalCount;

        if(this->totalCount > 0 && this->tmp_totalCount < this->totalCount){
            QJsonObject json;
            QUrlQuery params;
            //json.insert("page", ++this->next);
            //json.insert("size", 20);
            params.addQueryItem("page", QString("%1").arg(++this->next));
            params.addQueryItem("size", "20");

            qDebug() << "*totalCount = " << totalCount;
            qDebug() << "*tmp_totalCount = " << this->tmp_totalCount;
            NetworkHttp *network_friend = new NetworkHttp();
            connect(network_friend, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_friend(int, QJsonObject)));
            network_friend->request(HTTP_SEARCH_FRIEND, QString("%1/member/friend/request").arg(global.legacy_v1), params, true, true);

        }


        break;
    }

    sender()->deleteLater();
}

/**
 * @brief RoseHomeFriend::slot_removeRow [SLOT] 해당 Row Item을 제거한다.
 */
void RoseHomeFriend::slot_removeRow(){
    print_debug();
    FriendProfileItemRoseHome *tmp_frendprofileItem = dynamic_cast<FriendProfileItemRoseHome*>(sender());
    if(this->contentStep==SF_TAB_STEP_FRIEND){
        this->vl_friend->removeWidget(tmp_frendprofileItem);
    }else if(this->contentStep==SF_TAB_STEP_RECEIVE){
        this->vl_receive->removeWidget(tmp_frendprofileItem);
    }else if(this->contentStep==SF_TAB_STEP_SEND){
        print_debug();
        this->vl_send->removeWidget(tmp_frendprofileItem);
    }else if(this->contentStep==SF_TAB_STEP_FIND){
        this->vl_friend->removeWidget(tmp_frendprofileItem);
    }
    tmp_frendprofileItem->hide();
}

}
