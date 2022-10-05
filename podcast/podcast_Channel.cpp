#include "podcast/podcast_Channel.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/networkhttp.h"

#include "widget/toastmsg.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRegExp>


namespace podcast {

    const int HTTP_NETWORK_SUBSCRIBE_GET = 99;
    const int HTTP_NETWOKR_SUBSCRIBE_LIST = 98;
    const int HTTP_NETWORK_CONTENTS_LIST = 97;
    const int HTTP_NETWORK_PLAY_PODCAST = 96;

    const QString ICON_PATH___subscriptionOff = ":/images/tube_sub_off.png";
    const QString ICON_PATH___subscriptionOn = ":/images/tube_sub_on.png";


    /**
     * @brief Podcast_Channel::Podcast_Channel : 팟캐스트-채널정보
     * @details 방송목록 확인 및 구독 On/Off 가능
     * @param parent
     * @note 디자인상에는 플레이리스트 담기, 좋아요(하트), 더보기 기능이 있으나 API 확인 안됨<br>
     * 구독중 확인 여부 문제!!!<br>
     * 채널 정보에 isSubScribe 키가 있으나 항상 false 리턴<br>
     * 따라서 구독 채널 리스트 API를 이용해 구독 채널을 모두 가져와 비교하는 것으로 처리함<br>
     */
    Podcast_Channel::Podcast_Channel(QWidget *parent)
        : QWidget(parent)
    {
        setInit();
        setUIControl();
    }

    /**
     * @brief Podcast_Channel::setInit : 초기 세팅
     */
    void Podcast_Channel::setInit(){

        linker = Linker::getInstance();
        data = new Data_PC_Channel(this);
        delegate = new PodcastContentsDelegate(this);

        /*pixmap_subscribe_on.load(":images/tube_sub_on.png");
        pixmap_subscribe_off.load(":images/tube_sub_off.png");

        btnStyle_subscribe_on = "QPushButton{ ";
        btnStyle_subscribe_on += "padding-left:15px; padding-right:15px; padding-top:0px; padding-bottom:0px;";
        btnStyle_subscribe_on += "background:#BE905F;color:#FFFFFF;font-size:18px;";
        btnStyle_subscribe_on += "border-radius:20px;}";

        btnStyle_subscribe_off = "QPushButton{ ";
        btnStyle_subscribe_off += "padding-left:15px; padding-right:15px; padding-top:0px; padding-bottom:0px;";
        btnStyle_subscribe_off += "background:transparent;color:#FFFFFF;font-size:18px;";
        btnStyle_subscribe_off += "border:2px solid #666666;border-radius:20px;}";*/

        connect(delegate, SIGNAL(signal_clickedBtn(int,int)), this, SLOT(slot_delegateClicked(int,int)));
    }

    /**
     * @brief Podcast_Channel::setUIControl : UI 세팅
     */
    void Podcast_Channel::setUIControl(){

        // 팟캐스트 이미지
        lb_img = new ImageLabel;
        lb_img->setType(ImageLabel::Type::None300);
        lb_img->setImageResource(":images/def_mus_300.png");

        // 타이틀 및 제작자
        lb_title = new QLabel;
        lb_title->setStyleSheet("color:#FFFFFF;font-size:30px;");
        lb_title->setContentsMargins(0,0,0,0);
        lb_title->setFixedHeight(148);
        lb_title->setAlignment(Qt::AlignBottom);
        lb_title->setWordWrap(true);
        lb_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        lb_artist = new QLabel;
        lb_artist->setStyleSheet("color:#E6E6E6;font-size:20px;");
        lb_artist->setContentsMargins(0,0,0,0);
        lb_artist->setFixedHeight(102);
        lb_artist->setAlignment(Qt::AlignTop);
        lb_artist->setWordWrap(true);
        lb_artist->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);


        // 구독 버튼
        /*btn_subscribe = new QPushButton;
        btn_subscribe->setIcon(pixmap_subscribe_off);
        btn_subscribe->setIconSize(pixmap_subscribe_off.rect().size());
        btn_subscribe->setText(tr("Subscribe"));
        //btn_subscribe->setText(tr("구독하기"));
        btn_subscribe->setFixedSize(140,40);
        btn_subscribe->setStyleSheet(btnStyle_subscribe_off);
        btn_subscribe->setCursor(Qt::PointingHandCursor);
        btn_subscribe->hide();*/

        this->btn_subscribe = new QPushButton();
        this->btn_subscribe->setStyleSheet("background-color:transparent; border:2px solid #494949; border-radius:20px;");
        this->btn_subscribe->setCursor(Qt::PointingHandCursor);
        this->btn_subscribe->setFixedSize(150,40);
        this->btn_subscribe->hide();

        this->lb_subscribe_img = GSCommon::getUILabelImg(ICON_PATH___subscriptionOff, this->btn_subscribe);
        this->lb_subscribe_img->setFixedSize(40, 40);
        this->lb_subscribe_img->setGeometry(15, 0, 0, 0);
        this->lb_subscribe_img->setStyleSheet("background-color:transparent; border:0px");

        this->lb_subscribe = new QLabel(this->btn_subscribe);
        this->lb_subscribe->setFixedSize(80, 24);
        this->lb_subscribe->setGeometry(55, 7, 0, 0);
        this->lb_subscribe->setStyleSheet("background-color:transparent; font-size:16px; font-weight:300; color:#FFFFFF; border:0px");
        this->lb_subscribe->setAlignment(Qt::AlignCenter);
        this->lb_subscribe->setText(tr("Subscribe"));

        QVBoxLayout *box_channelInfo = new QVBoxLayout;
        box_channelInfo->setSpacing(0);
        box_channelInfo->setContentsMargins(0,0,0,0);
        box_channelInfo->setAlignment(Qt::AlignTop);
        box_channelInfo->addWidget(lb_title);
        box_channelInfo->addSpacing(10);
        box_channelInfo->addWidget(lb_artist);
        box_channelInfo->addWidget(btn_subscribe);

        QHBoxLayout *box_top = new QHBoxLayout;
        box_top->setSpacing(30);
        box_top->setContentsMargins(0,0,0,0);
        box_top->setAlignment(Qt::AlignLeft);
        box_top->addWidget(lb_img);
        box_top->addLayout(box_channelInfo);

        listWidget = new QListWidget;
        listWidget->setItemDelegate(delegate);
        listWidget->setCursor(Qt::PointingHandCursor);
        listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setStyleSheet("border:none;");

        QVBoxLayout *box_total = new QVBoxLayout;
        box_total->setSpacing(41);
        box_total->setContentsMargins(33,5,28,0);
        box_total->addLayout(box_top);
        box_total->addWidget(listWidget);

        setLayout(box_total);

        // connect
        connect(btn_subscribe, SIGNAL(clicked()), SLOT(slot_clickedSubscribe()));
        connect(lb_img, SIGNAL(setPixmapCompleted(QPixmap)), SLOT(slot_loadedThumb(QPixmap)));
    }

    /**
     * @brief Podcast_Channel::setSubscribeBtnStyle : 구독 버튼 스타일 세팅
     * @param flagIsSubscribe bool if true, 구독중 스타일 적용
     */
    void Podcast_Channel::setSubscribeBtnStyle(bool flagIsSubscribe){

        this->flagSubscribe = flagIsSubscribe;

        if(this->flagSubscribe == true){

            this->lb_subscribe_img->setPixmap(ICON_PATH___subscriptionOn);

            this->lb_subscribe->setText(tr("Subscribed"));

            this->btn_subscribe->setStyleSheet("background-color:#B18658; border:2px solid #B18658; border-radius:20px;");
        }
        else{

            this->lb_subscribe_img->setPixmap(ICON_PATH___subscriptionOff);

            this->lb_subscribe->setText(tr("Subscribe"));

            this->btn_subscribe->setStyleSheet("background-color:transparent; border:2px solid #666666; border-radius:20px;");
        }

        /*if(flagIsSubscribe){
            btn_subscribe->setStyleSheet(btnStyle_subscribe_on);
            btn_subscribe->setIcon(pixmap_subscribe_on);
            btn_subscribe->setText(tr("Subscribing"));
            //btn_subscribe->setText(tr("구독중"));
        }else{
            btn_subscribe->setStyleSheet(btnStyle_subscribe_off);
            btn_subscribe->setIcon(pixmap_subscribe_off);
            btn_subscribe->setText(tr("Subscribe"));
            //btn_subscribe->setText(tr("구독하기"));
        }

        btn_subscribe->setProperty("flagIsSubscribe", flagIsSubscribe);*/
    }

    /**
     * @brief Podcast_Channel::setData : 팟캐스트 채널 Basic 데이터 세팅
     * @param p_jsonData QJsonObject 채널 데이터
     */
    void Podcast_Channel::setData(const QJsonObject &p_jsonData){

        data->setData(p_jsonData);

        lb_img->setImageResource(":images/def_mus_300.png");
        lb_img->setImageUrl(data->getArtworkUrl600());
        lb_title->setText(data->getCollectionName());
        lb_artist->setText(data->getArtistName());


        btn_subscribe->hide();
        requestSubScribeList();

        requestContents();
    }


    /**
     * @brief Podcast_Channel::appendContentsUI : 컨텐츠 UI추가
     * @param p_jsonData QJsonObject response
     * @note 날짜 타입 일정하지 않음.<br>
     * 확인된 타입 1. 2015.02.03 / 타입 2. Wed, 21 Dec 2018 08:02:13 +0900<br>
     * duration 타입 일정하지 않음.<br>
     * 확인된 타입 1. 03:05, 2. 3105(초), 3. empty
     *
     */
    void Podcast_Channel::appendContentsUI(const int &p_index, const QJsonObject &p_jsonData){

        const QString jsonKey_collection_dt1_id = "collection_dt1_id";
        const QString jsonKey_duration = "duration";
        const QString jsonKey_preview_title = "preview_title";
        const QString jsonKey_preview_url = "preview_url";
        const QString jsonKey_release_date = "release_date";


        QJsonObject json;
        json.insert("index", p_index);
        json.insert("collectionID", p_jsonData[jsonKey_collection_dt1_id].toInt());
        json.insert("duration", getDuration(p_jsonData[jsonKey_duration].toString()));
        json.insert("title", p_jsonData[jsonKey_preview_title].toString());
        json.insert("url", p_jsonData[jsonKey_preview_url].toString());
        json.insert("date", getReleaseDate(p_jsonData[jsonKey_release_date].toString()));


        QListWidgetItem *item1 = new QListWidgetItem;
        item1->setData(Qt::UserRole, json);
        listWidget->addItem(item1);

    }

    /**
     * @brief Podcast_Channel::getReleaseDate : 컨텐츠 등록 날짜및 요일 반환
     * @param p_strDate QString 날짜 문자열
     * @return
     * @note 타입1. 2019.05.03, 타입 2. Wed, 21 Dec 2018 08:02:13 +0900<br>
     * 타입 3. Wed, 21 Dec 2018 08:02 +0900
     */
    QString Podcast_Channel::getReleaseDate(const QString &p_strDate){

        QStringList tmp_strListDate = p_strDate.split(" ");
        QString tmp_convertDate;

        QLocale locale(QLocale::English);
        if(tmp_strListDate.size()==6){
            // 시간값 제거
            while(tmp_strListDate.size() > 4){
                tmp_strListDate.removeAt(tmp_strListDate.size()-1);
            }

            QString string = tmp_strListDate.join(" ");
            QString format = "ddd, d MMM yyyy";
            QDate tmp_date = locale.toDate(string, format);


            tmp_convertDate += tmp_date.toString("yyyy.MM.dd");
            tmp_convertDate += " "+getDayOfWeekStr(tmp_date.dayOfWeek());
        }else{
            QString format = "yyyy.MM.dd";
            QDateTime tmp_date = locale.toDateTime(p_strDate, format).toLocalTime();

            tmp_convertDate += tmp_date.date().toString("yyyy.MM.dd");
            tmp_convertDate += " "+getDayOfWeekStr(tmp_date.date().dayOfWeek());
        }

        return tmp_convertDate;
    }

    /**
     * @brief Podcast_Channel::getDuration : 컨텐츠 재생시간 문자열로 반환
     * @param p_duration QString
     * @return QString
     * @note duration 타입 일정하지 않음.<br>
     * 확인된 타입 1. 03:05, 2. 3105(초), 3. empty
     */
    QString Podcast_Channel::getDuration(QString p_duration){

        QRegExp regex("\\d*");

        if(p_duration.contains(":")){

            if(p_duration.size()==8 && p_duration.startsWith("00:")){
                return p_duration.remove(0,3);
            }
            return p_duration;
        }else if(!p_duration.isEmpty() && regex.exactMatch(p_duration)){
            int tmp_totalSecond = p_duration.toInt();
            int tmp_minute = tmp_totalSecond / 60;
            int tmp_second = tmp_totalSecond % 60;
            int tmp_hour = 0;

            if(tmp_minute >= 60){
                tmp_hour = tmp_minute /60;
                tmp_minute = tmp_minute % 60;
            }

            QString tmp_convertDuration;

            if(tmp_hour > 0 || tmp_minute > 0 || tmp_second > 0){
                if(tmp_hour > 0){
                    tmp_convertDuration += QString::number(tmp_hour).rightJustified(2,'0');
                    tmp_convertDuration += ":";
                }

                tmp_convertDuration += QString::number(tmp_minute).rightJustified(2,'0');
                tmp_convertDuration += ":";
                tmp_convertDuration += QString::number(tmp_second).rightJustified(2,'0');
            }

            return tmp_convertDuration;
        }

        return  p_duration;
    }

    /**
     * @brief Podcast_Channel::setResultOfSubscribe : 구독 신청 / 취소 요청 결과 처리
     * @param p_jsonData QJsonObject response;
     */
    void Podcast_Channel::setResultOfSubscribe(const QJsonObject &p_jsonObject){
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

                    if(p_jsonObject.contains("item")){

                        QJsonObject item = ProcJsonEasy::getJsonObject(p_jsonObject, "item");
                        bool isSubScribe = ProcJsonEasy::getBool(item, "isSubScribe");

                        this->btn_subscribe->hide();
                        setSubscribeBtnStyle(isSubScribe);
                        this->btn_subscribe->show();
                    }
                //}
            }
        }

        if(!flagOk){
            //this->btn_subscribe->hide();
            //setSubscribeBtnStyle(!btn_subscribe->property("flagIsSubscribe").toBool());
            //this->btn_subscribe->show();
            ToastMsg::show(this,"",tr("Subscription information cannot be changed."));
            //ToastMsg::show(this,"",tr("구독정보를 변경할 수 없습니다."));
        }
    }

    /**
     * @brief Podcast_Channel::setResultOfSubscribeList : 구독중인 채널 목록 요청결과 처리
     * @details 현재 채널의 구독 상태 확인 후 버튼 스타일 변경
     * @param p_jsonData
     */
    void Podcast_Channel::setResultOfSubscribeList(const QJsonObject &p_jsonObject){


        const QString jsonKey_flagOk = "flagOk";
        const QString jsonKey_status = "status";
        const QString jsonKey_outs = "outs";
        const QString jsonKey_subScribeArr = "subScribeArr";
        const QString jsonKey_collectionId = "collectionId";

        bool flagOk = false;
        bool flagIsSubscribe = false;

        if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
            if(p_jsonObject.contains(jsonKey_status)){
                QJsonObject jsonStatus = p_jsonObject[jsonKey_status].toObject();
                // TODO:: Search Devices 16/02/2021 by disket
                //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
                    if(p_jsonObject.contains(jsonKey_subScribeArr)){

                        flagOk = true;

                        QJsonArray jsonArrSub = p_jsonObject[jsonKey_subScribeArr].toArray();
                        for(int i = 0 ; i < jsonArrSub.size(); i++){
                            QJsonObject jsonSub = jsonArrSub.at(i).toObject();

                            if(jsonSub.contains(jsonKey_collectionId)){
                                if(jsonSub[jsonKey_collectionId] == data->getCollectionId()){
                                    flagIsSubscribe = true;
                                    break;
                                }
                            }
                        }
                    }
                //}
            }
        }

        if(flagOk){
            setSubscribeBtnStyle(flagIsSubscribe);
            this->btn_subscribe->show();
        }
        else{
            ToastMsg::show(this,"",tr("Subscription information cannot be verified."));
            //ToastMsg::show(this,"",tr("구독정보를 확인할 수 없습니다."));
        }
    }


    /**
     * @brief Podcast_Channel::setResultOfContentsList : 컨텐츠 목록 요청결과 처리
     * @param p_jsonData QJsonObject response
     */
    void Podcast_Channel::setResultOfContentsList(const QJsonObject &p_jsonData){

        const QString jsonKey_flagOk = "flagOk";
        const QString jsonKey_status = "status";
        const QString jsonKey_outs = "outs";
        const QString jsonKey_data = "data";

        if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
            if(p_jsonData.contains(jsonKey_status)){
                QJsonObject jsonStatus = p_jsonData[jsonKey_status].toObject();
                // TODO:: Search Devices 16/02/2021 by disket
                //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
                    if(p_jsonData.contains(jsonKey_data)){
                        QJsonArray jsonArrContents = p_jsonData[jsonKey_data].toArray();

                        for(int i=0; i < jsonArrContents.size(); i++){
                            appendContentsUI(i,jsonArrContents.at(i).toObject());
                        }
                    }
                //}
            }
        }
    }

    /**
     * @brief Podcast_Channel::setResultOfPodcastPlay : 컨텐츠 재생 요청결과 처리
     * @param p_jsonData QJsonObject response
     */
    void Podcast_Channel::setResultOfPodcastPlay(const QJsonObject &p_jsonData){
        const QString jsonKey_flagOk = "flagOk";
        const QString jsonKey_status = "status";
        const QString jsonKey_outs = "outs";

        bool flagOk = false;

        if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
            if(p_jsonData.contains(jsonKey_status)){
                QJsonObject jsonStatus = p_jsonData[jsonKey_status].toObject();
                // TODO:: Search Devices 16/02/2021 by disket
                //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
                    flagOk = true;
                //}
            }
        }

        if(!flagOk){
            ToastMsg::show(this,"",tr("Content cannot be played."));
            //ToastMsg::show(this,"",tr("컨텐츠를 재생할 수 없습니다."));
        }
    }

    /**
     * @brief Podcast_Channel::slot_responseHttp : [슬롯] Http 요청결과 처리
     * @param p_id int 요청 아이디
     * @param p_jsonObject QJsonObject response
     */
    void Podcast_Channel::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
        if(p_id == HTTP_NETWORK_SUBSCRIBE_GET){
            setResultOfSubscribe(p_jsonObject);
        }
        else if(p_id == HTTP_NETWOKR_SUBSCRIBE_LIST){
            setResultOfSubscribeList(p_jsonObject);
        }
        else if(p_id == HTTP_NETWORK_CONTENTS_LIST){
            setResultOfContentsList(p_jsonObject);
        }
        else if(p_id == HTTP_NETWORK_PLAY_PODCAST){
            setResultOfPodcastPlay(p_jsonObject);
        }

       sender()->deleteLater();
    }

    /**
     * @brief Podcast_Channel::slot_clickedSubscribe : [슬롯] 구독하기, 구독중 버튼 클릭
     */
    void Podcast_Channel::slot_clickedSubscribe(){

        this->dlgSubscribe = new DialogConfirm(this);
        this->dlgSubscribe->setTitle(tr("SUBSCRIBE"));

        if(this->flagSubscribe == true){
            this->dlgSubscribe->setText(tr("Are you sure you want to unsubscribe from this channel?"));

            //this->flagSubscribe = false;
        }
        else{
            this->dlgSubscribe->setText(tr("Would you like to subscribe to this channel?"));

            //this->flagSubscribe = true;
        }

        int result = this->dlgSubscribe->exec();

        if(result == 1){

            QJsonObject json = data->getData();
            if(this->flagSubscribe == true){
                json.insert("isInit", true);
                json.insert("isSubScribe", true);
            }
            else{
                json.insert("isInit", false);
                json.insert("isSubScribe", false);
            }

            NetworkHttp *network = new NetworkHttp(this);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QString tmp_url = QString("http://%1:%2/set.pod.subscribe.item").arg(global.device.getDeviceIP()).arg(global.port);
            network->request(HTTP_NETWORK_SUBSCRIBE_GET, tmp_url, json, true);
        }

        //bool flagIsSubscribe = sender()->property("flagIsSubscribe").toBool();
        //setSubscribeBtnStyle(!flagIsSubscribe);
    }


    /**
     * @brief Podcast_Channel::requestSubScribeList : 현재 구독중인 채널 목록 요청
     */
    void Podcast_Channel::requestSubScribeList(){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject jsonData;

        network->request(HTTP_NETWOKR_SUBSCRIBE_LIST, QString("http://%1:%2/get.pod.subscribe.list")
                             .arg(global.device.getDeviceIP()).arg(global.port), jsonData, true);
    }


    /**
     * @brief Podcast_Channel::requestContents : 팟캐스트 방송 컨텐츠 목록 요청
     */
    void Podcast_Channel::requestContents(){

        listWidget->clear();

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));


        QJsonObject json;
        json.insert("collectID", data->getCollectionId());
        json.insert("roseToken", global.device.getDeviceRoseToken());


        network->request(HTTP_NETWORK_CONTENTS_LIST, QString("%1/IF_ROSEPODCAST_004")
                         //.arg(global.hifimp)
                         .arg(global.release_roseaudio)
                         , json, true);
    }

    /**
     * @brief Podcast_Channel::slot_delegateClicked : [슬롯] delegateClick
     * @details QlistWidget item click 처리<br>
     * @param p_index
     * @param p_btnType
     */
    void Podcast_Channel::slot_delegateClicked(const int &p_index, const int &p_btnType){

        QListWidgetItem *item = listWidget->item(p_index);
        QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
        int tmp_index = map["index"].toInt();
        QString tmp_collectionID = map["collectionID"].toString();
        QString tmp_duration = map["duration"].toString();
        QString tmp_title = map["title"].toString();
        QString tmp_url = map["url"].toString();
        QString tmp_date = map["date"].toString();

        if(p_btnType == PodcastContentsDelegate::BtnType::etc){

            QJsonObject tmp_json;
            tmp_json.insert("channel", data->getData());
            tmp_json.insert("currentPosition", QString::number(tmp_index));
            tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_NETWORK_PLAY_PODCAST, QString("http://%1:%2/%3")
                             .arg(global.device.getDeviceIP())
                             .arg(global.port)
                             .arg("podcastPlay"), tmp_json, true);
        }
        else if(p_btnType == PodcastContentsDelegate::BtnType::trackHeart){
            // 하트 클릭
            ToastMsg::show(this,"","팟캐스트 트랙 즐겨찾기 API 확인 필요");
        }
        else if(p_btnType == PodcastContentsDelegate::BtnType::playListAdd){
            // 플레이리스트 담기 클릭
            ToastMsg::show(this,"","팟캐스트 플레이리스트 담기 API 확인 필요");
        }
        else if(p_btnType == PodcastContentsDelegate::BtnType::more){
            // 더보기 클릭
        }
    }

    /**
     * @brief Podcast_Channel::slot_loadedThumb : [슬롯] 팟캐스트 썸네일 로딩 완료
     * @details 팟캐스트 채널 페이지 배경 세팅
     * @param pixmap
     */
    void Podcast_Channel::slot_loadedThumb(QPixmap pixmap){
        emit linker->signal_showPodcastBG(pixmap);
    }

    /**
     * @brief Podcast_Channel::getDayOfWeekStr : 요일 반환
     * @param p_dayOfWeek int QDate::dayOfWeek() value
     * @return
     */
    QString Podcast_Channel::getDayOfWeekStr(const int &p_dayOfWeek){
        QString tmp_str;

        switch (p_dayOfWeek) {
            case 1:
                tmp_str = tr("Monday");
                break;
            case 2:
                tmp_str = tr("Tuesday");
                break;
            case 3:
                tmp_str = tr("Wednesday");
                break;
            case 4:
                tmp_str = tr("Thursday");
                break;
            case 5:
                tmp_str = tr("Friday");
                break;
            case 6:
                tmp_str = tr("Saturday");
                break;
            case 7:
                tmp_str = tr("Sunday");
                break;
                /*
                 *         case 1:
                tmp_str = tr("월");
                break;
            case 2:
                tmp_str = tr("화");
                break;
            case 3:
                tmp_str = tr("수");
                break;
            case 4:
                tmp_str = tr("목");
                break;
            case 5:
                tmp_str = tr("금");
                break;
            case 6:
                tmp_str = tr("토");
                break;
            case 7:
                tmp_str = tr("일");
                break;
                */
            default:
                tmp_str = "";
                break;
        }

        return tmp_str;
    }
}
