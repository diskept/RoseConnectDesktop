#include "podcast/podcast_Subscription.h"

#include "common/global.h"
#include "common/networkhttp.h"

#include "widget/clickablewidget.h"
#include "widget/dialogconfirm.h"
#include "widget/imagelabel.h"

#include "widget/toastmsg.h"

#include <QScrollArea>
#include <QJsonObject>


namespace podcast {

    const int HTTP_NETWOKR_SUBSCRIBE_LIST = 99;
    const int HTTP_NETWORK_CANCEL_SUBSCRIBE = 98;

    /**
     * @brief Podcast_Subscription::Podcast_Subscription : 팟캐스트-구독
     */
    Podcast_Subscription::Podcast_Subscription(QWidget *parent)
        : QWidget(parent)
    {
        setInit();
        setUIControl();
    }


    /**
     * @brief Podcast_Subscription::~Podcast_Subscription : 소멸자
     */
    Podcast_Subscription::~Podcast_Subscription(){

        qDeleteAll(listData->begin(), listData->end());
        listData->clear();

        this->deleteLater();
    }


    /**
     * @brief Podcast_Subscription::setInit : 초기 세팅
     */
    void Podcast_Subscription::setInit(){

        linker = Linker::getInstance();
        listData = new QList<Data_PC_Channel*>;
    }


    /**
     * @brief Podcast_Subscription::setUIControl : UI 세팅
     */
    void Podcast_Subscription::setUIControl(){

        box_contents = new QVBoxLayout;
        box_contents->setSpacing(0);
        box_contents->setContentsMargins(0,0,0,0);
        box_contents->setAlignment(Qt::AlignTop);

        wg_contents = new QWidget;
        wg_contents->setLayout(box_contents);
        wg_contents->setContentsMargins(33,10,28,0);

        QScrollArea *scrollArea = new QScrollArea;
        scrollArea->setWidget(wg_contents);
        scrollArea->setWidgetResizable(true);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setStyleSheet("border:0px;");

        QVBoxLayout *boxMain = new QVBoxLayout;
        boxMain->setContentsMargins(0,0,0,0);
        boxMain->setSpacing(0);
        boxMain->addWidget(scrollArea);


        wg_boxMain = new QWidget;
        wg_boxMain->setContentsMargins(0,0,0,0);
        wg_boxMain->setLayout(boxMain);

        lb_empty = new QLabel;
        lb_empty->setAlignment(Qt::AlignCenter);
        lb_empty->setText(tr("There is no subscription list."));
        //lb_empty->setText(tr("구독 리스트가 없습니다."));
        lb_empty->setStyleSheet("color:#FFFFFF;font-size:18px;");

        stackedWidget = new QStackedWidget;
        stackedWidget->addWidget(wg_boxMain);
        stackedWidget->addWidget(lb_empty);

        QVBoxLayout *boxTotal = new QVBoxLayout;
        boxTotal->setContentsMargins(0,0,0,0);
        boxTotal->addWidget(stackedWidget);

        setLayout(boxTotal);
    }

    /**
     * @brief Podcast_Subscription::appendSubscriptionUI : 구독정보 UI 추가
     * @param p_jsonObject
     */
    void Podcast_Subscription::appendSubscriptionUI(const QJsonObject &p_jsonObject){

        const QString jsonKey_artworkUrl100 = "artworkUrl100";
        const QString jsonKey_collectionId = "collectionId";
        const QString jsonKey_collectionName = "collectionName";


        QString tmp_collectionName = "";
        QString tmp_collectionID = "";
        QString tmp_artworkUrl100 = "";

        if(p_jsonObject.contains(jsonKey_collectionName)){
            tmp_collectionName = p_jsonObject[jsonKey_collectionName].toString();
        }
        if(p_jsonObject.contains(jsonKey_collectionId)){
            tmp_collectionID = p_jsonObject[jsonKey_collectionId].toInt();
        }
        if(p_jsonObject.contains(jsonKey_artworkUrl100)){
            tmp_artworkUrl100 = p_jsonObject[jsonKey_artworkUrl100].toString();
        }

        ImageLabel *img = new ImageLabel;
        img->setType(ImageLabel::Type::IconSmallRound);
        img->setImageUrl(tmp_artworkUrl100);
        img->setStyleSheet("background-color:transparent;");

        QLabel *lb_title = new QLabel;
        lb_title->setText(tmp_collectionName);
        lb_title->setStyleSheet("color:#FFFFFF;font-size:16;background-color:transparent;");

        QPushButton *btn_subscribe = GSCommon::getUIBtnImg("btn_subscribe", ":images/tube_sub_ico.png");
        btn_subscribe->setFixedSize(60,70);
        btn_subscribe->setProperty("index", box_contents->count());


        QHBoxLayout *box_total = new QHBoxLayout;
        box_total->setSpacing(20);
        box_total->setContentsMargins(0,0,0,0);
        box_total->addWidget(img,0, Qt::AlignVCenter);
        box_total->addWidget(lb_title,0, Qt::AlignVCenter);
        box_total->addStretch(1);
        box_total->addWidget(btn_subscribe,0, Qt::AlignVCenter);

        QPushButton *wg_total = new QPushButton;
        wg_total->setContentsMargins(5,0,0,0);
        wg_total->setObjectName("wg_total");
        wg_total->setFixedHeight(70);
        wg_total->setLayout(box_total);
        wg_total->setCursor(Qt::PointingHandCursor);
        wg_total->setStyleSheet("#wg_total {border-bottom:1px solid #303030;} #wg_total:hover{background-color:#333333;}");
        wg_total->setProperty("index", box_contents->count());

        connect(btn_subscribe, SIGNAL(clicked()), SLOT(slot_clickedCancelSubscribe()));
        connect(wg_total, SIGNAL(clicked()), SLOT(slot_clickedChannel()));

        box_contents->addWidget(wg_total);
    }


    /**
     * @brief RoseTube_Subscription::requestData : 사용자 구독 정보 요청
     * @param flagNeedRefresh if true, 강제 새로고침
     */
    void Podcast_Subscription::requestData(bool flagNeedRefresh){


        if(box_contents->count() == 0 || flagNeedRefresh){

            qDeleteAll(listData->begin(), listData->end());
            listData->clear();
            GSCommon::clearLayout(box_contents);

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QJsonObject jsonData;

            network->request(HTTP_NETWOKR_SUBSCRIBE_LIST, QString("http://%1:%2/get.pod.subscribe.list")
                             .arg(global.device.getDeviceIP()).arg(global.port), jsonData, true);
        }
    }


    /**
     * @brief Podcast_Subscription::setResultOfSubscription : 구독정보 요청 결과 처리
     * @param p_jsonObject QJsonObject response
     */
    void Podcast_Subscription::setResultOfSubscription(const QJsonObject &p_jsonObject){


        const QString jsonKey_flagOk = "flagOk";
        const QString jsonKey_status = "status";
        const QString jsonKey_outs = "outs";
        const QString jsonKey_subScribeArr = "subScribeArr";

        bool flagOk = false;

        if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
            if(p_jsonObject.contains(jsonKey_status)){
                QJsonObject jsonStatus = p_jsonObject[jsonKey_status].toObject();
                // TODO:: Search Devices 16/02/2021 by disket
                //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
                    if(p_jsonObject.contains(jsonKey_subScribeArr)){

                        flagOk = true;

                        QJsonArray jsonArrSub = p_jsonObject[jsonKey_subScribeArr].toArray();
                        if(jsonArrSub.size() > 0){
                            for(int i = 0 ; i < jsonArrSub.size(); i++){
                                listData->append(new Data_PC_Channel(jsonArrSub.at(i).toObject()));
                                appendSubscriptionUI(jsonArrSub.at(i).toObject());
                            }
                            stackedWidget->setCurrentWidget(wg_boxMain);
                        }else{
                            lb_empty->setText(tr("There is no subscription list."));
                            //lb_empty->setText(tr("구독 리스트가 없습니다."));
                            stackedWidget->setCurrentWidget(lb_empty);
                        }
                    }
                //}
            }
        }

        if(!flagOk){
            lb_empty->setText(tr("Could not get subscription list."));
            //lb_empty->setText(tr("구독 리스트를 가져올 수 없습니다."));
            stackedWidget->setCurrentWidget(lb_empty);
        }
    }


    /**
     * @brief Podcast_Subscription::requestCancelSubscribe : 구독 취소 요청
     * @param p_index int 구독채널 index
     * @note 구독채널 목록 API 값 확인 필요<br>
     * 구독신청,구독취소 모두 같은 API를 사용한다. APTI : http://IP:PORT/set.pod.subscribe.item<br>
     * param 중 isInit, isSubScribe 값의 의미를 알 수 없다.<br>
     * 구독중인 채널 목록에서 해당 값이 모두 false, false로 넘어온다.<br>
     * Android App 디버깅으로 확인해보니 구독취소를 하기 위해서는 해당 값을 모두 true로 넘겨야한다.<br>
     * 반대로 구독신청을 하기 위해서는 isInit은 true, isSubScribe는 false로 넘겨야한다.
     */
    void Podcast_Subscription::requestCancelSubscribe(const int &p_index){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //
        QJsonObject json = listData->at(p_index)->getData();
        json.insert("isInit", true);
        json.insert("isSubScribe", true);

        QString tmp_url = QString("http://%1:%2/set.pod.subscribe.item").arg(global.device.getDeviceIP()).arg(global.port);
        network->request(HTTP_NETWORK_CANCEL_SUBSCRIBE, tmp_url, json, true);
    }


    /**
     * @brief Podcast_Subscription::setResultOfCancelSubscribe : 구독 취소 요청결과 처리
     * @param p_jsonObject QJsonObject response
     */
    void Podcast_Subscription::setResultOfCancelSubscribe(const QJsonObject &p_jsonObject){


        const QString jsonKey_flagOk = "flagOk";
        const QString jsonKey_status = "status";
        const QString jsonKey_outs = "outs";

        bool flagOk = false;

        if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
            if(p_jsonObject.contains(jsonKey_status)){
                QJsonObject jsonStatus = p_jsonObject[jsonKey_status].toObject();
                // TODO:: Search Devices 16/02/2021 by disket
                //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
                    flagOk = true;

                    requestData(true);
                //}
            }
        }

        if(!flagOk){
            ToastMsg::show(this,"",tr("You cannot cancel your subscription."));
            //ToastMsg::show(this,"",tr("구독취소를 할 수 없습니다."));
        }
    }


    /**
     * @brief Podcast_Subscription::slot_responseHttp : [슬롯] Http 요청 결과 처리
     * @param p_id int 요청 아이디
     * @param p_jsonObject QJsonObject response
     */
    void Podcast_Subscription::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){


        if(p_id==HTTP_NETWOKR_SUBSCRIBE_LIST){
            setResultOfSubscription(p_jsonObject);
        }
        else if(p_id==HTTP_NETWORK_CANCEL_SUBSCRIBE){
            setResultOfCancelSubscribe(p_jsonObject);
        }

       sender()->deleteLater();
    }


    /**
     * @brief Podcast_Subscription::slot_clickedChannel : [슬롯] 구독채널 클릭
     */
    void Podcast_Subscription::slot_clickedChannel(){


        int tmp_index = sender()->property("index").toInt();

        if(tmp_index >=0 && tmp_index < listData->size()){
            QJsonObject jsonData;
            jsonData.insert(KEY_PAGE_CODE, PAGECODE_PC_SUB_CHANNEL);
            jsonData.insert(KEY_DATA, listData->at(tmp_index)->getData());

            emit linker->signal_clickedMovePage(jsonData);
        }
    }


    /**
     * @brief Podcast_Subscription::slot_clickedCancelSubscribe : [슬롯] 구독취소 클릭
     */
    void Podcast_Subscription::slot_clickedCancelSubscribe(){

        DialogConfirm *dlg = new DialogConfirm(this);
        dlg->setTitle(tr("subscribe"));
        //dlg->setTitle(tr("구독"));
        dlg->setText(tr("Are you sure you want to cancel your subscription?"));
        //dlg->setText(tr("구독을 취소하시겠습니까?"));

        int result = dlg->exec();

        if(result==QDialog::Accepted){
            requestCancelSubscribe(sender()->property("index").toInt());
        }

        delete dlg;

    }
}
