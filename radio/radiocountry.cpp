#include "radio/radiocountry.h"

#include "radio/dialogeditradio.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/rosesettings.h"

#include "widget/VerticalScrollArea.h"
#include "widget/toastmsg.h"
#include "widget/imagelabelradio.h"
#include "widget/framewidget.h"
#include "widget/clickablelabel.h"
#include "widget/optionpopup.h"

#include <QComboBox>
#include <QJsonArray>


namespace radio {

    const int HTTP_CHANNEL_LIST = 99;
    const int HTTP_COUNTRY_LIST = 98;
    const int HTTP_FAVORITE_LIST = 96;


    /**
     * @brief RadioCountry::RadioCountry : 라디오-국가
     * @details 사용자가 선택한 국가의 모든 라디오 채널 리스트
     * @param parent
     * @note 사용자채널에만 존재하던 새로고침, 방송채널 추가 전 메뉴 추가요청<br>
     * 공통 Widget화 하지 않고 Copy하여 처리단만 다르게 함.
     */
    RadioCountry::RadioCountry(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_rosetube, parent)
    {

        this->setInit();
        this->setUIControl();
        this->setTopUIControl(parent);

        this->widget_country = new QWidget;
        this->widget_country->setContentsMargins(0,0,0,0);

        this->widget_combobox = new QWidget(parent);
        this->widget_combobox->setObjectName("widget_combobox");
        this->widget_combobox->setStyleSheet("#widget_combobox { background-color:#333333; border:1px solid #4D4D4D; border-radius:3px; } ");

        //widget_combobox->setGeometry(250, 128, this->widget_combobox->sizeHint().width(), this->widget_combobox->sizeHint().height());
        this->flagShowedComboBox = false;
        this->widget_combobox->hide();
    }


    RadioCountry::~RadioCountry(){

        delete settings;
        this->listConntry->clear();
        this->favorUrlList.clear();

        this->deleteLater();
    }


    /**
     * @brief RadioUser::show_topBtnControl : public show, 새로고침, 방송채널 추가 UI
     */
    void RadioCountry::show_topBtnControl(){

        this->widget_topContrl->show();
    }


    /**
     * @brief RadioUser::hide_topBtnFavorite : public hide, 새로고침, 방송채널 추가 UI
     */
    void RadioCountry::hide_topBtnControl(){

        this->widget_topContrl->hide();
    }


    /**
     * @brief RadioCountry::hide_topBtnFavorite
     */
    void RadioCountry::hide_comboBoxWidget(bool flagCallSiangal){

        // 부모의 현재 서브메뉴명 변경 : 현재 선택된 콤보박스의 값으로...
        if(flagCallSiangal){
            emit signal_changedCurrSubMenuName(QString("%1").arg(settings->value(rosesettings.SETTINGS_RADIO_NAME).toString()));
        }

        this->flagShowedComboBox = false;
        this->widget_combobox->hide();
    }


    /**
     * @brief RadioCountry::setInit : 초기 세팅
     */
    void RadioCountry::setInit(){

        this->linker = Linker::getInstance();
        //connect(linker, SIGNAL(signal_changedRadioFavorChannel(bool)), SLOT(slot_changedFavorChannel(bool)));

        this->listConntry = new QList<QWidget*>();

        settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);

        this->currCountryId = settings->value(rosesettings.SETTINGS_RADIO_ID, 2).toInt();
        this->currCountryName = settings->value(rosesettings.SETTINGS_RADIO_NAME,"KOREA").toString();
    }


    /**
     * @brief RadioCountry::setUIControl : UI 세팅
     */
    void RadioCountry::setUIControl(){

        /*
        flowLayout = new FlowLayout(0, 20, 20);

        QWidget *wg_contents = new QWidget;
        wg_contents->setLayout(this->flowLayout_country);
        wg_contents->setContentsMargins(0,0,0,0);

        VerticalScrollArea *scrollArea = new VerticalScrollArea;
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
        this->flowLayout_radio = new FlowLayout(0, 20, 20);
        this->flowLayout_radio->setSizeConstraint(QLayout::SetMinimumSize);
        this->flowLayout_radio->setContentsMargins(70, 50, 70, 30);

        GSCommon::clearLayout(this->flowLayout_radio);

        this->widget_radio = new QWidget();
        this->widget_radio->setLayout(this->flowLayout_radio);
        this->widget_radio->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->widget_radio->setStyleSheet("background-color:transparent;");

        this->box_contents->addWidget(this->widget_radio);
    }


    /**
     * @brief RadioCountry::setTopUIControl : 새로고침, 방송채널 TOP UI 세팅
     */
    void RadioCountry::setTopUIControl(QWidget *parent){

        // 상단 방송 채널 추가 UI
        QPixmap pixmap_refresh(":/images/icon_refresh_gray_onbd.png");
        QPushButton *btn_refresh = new QPushButton;
        btn_refresh->setIcon(pixmap_refresh);
        btn_refresh->setIconSize(pixmap_refresh.rect().size());
        btn_refresh->setText(tr("Refresh"));
        //btn_refresh->setText(tr("새로고침"));
        btn_refresh->setStyleSheet("font-size:18px;color:#CCCCCC;background-color:transparent;");
        btn_refresh->setCursor(Qt::PointingHandCursor);

        QPixmap pixmap_add(":/images/icon_add_ch.png");
        QPushButton *btn_add = new QPushButton;
        btn_add->setIcon(pixmap_add);
        btn_add->setIconSize(pixmap_refresh.rect().size());
        btn_add->setText(tr("Add Broadcast Channel"));
        //btn_add->setText(tr("방송 채널 추가"));
        btn_add->setStyleSheet("font-size:18px;color:#CCCCCC;background-color:transparent;");
        btn_add->setCursor(Qt::PointingHandCursor);

        QHBoxLayout *hl_topControl = new QHBoxLayout();
        hl_topControl->setContentsMargins(0,0,0,0);
        hl_topControl->setSpacing(0);
        hl_topControl->setAlignment(Qt::AlignVCenter);
        hl_topControl->addWidget(btn_refresh);
        hl_topControl->addSpacing(50);
        hl_topControl->addWidget(btn_add);

        widget_topContrl = new QWidget(parent);
        widget_topContrl->setContentsMargins(0,15,77,0);
        widget_topContrl->setObjectName("widget_topContrl");
        widget_topContrl->setLayout(hl_topControl);
        widget_topContrl->setStyleSheet("#widget_topContrl { background-color:#171717; } ");

        connect(btn_refresh, SIGNAL(clicked()), SLOT(slot_clickedRefresh()));
        connect(btn_add, SIGNAL(clicked()), SLOT(slot_clickedAddChannel()));
    }


    /**
     * @brief RadioCountry::clearChannelList : 채널 목록 클리어
     */
    void RadioCountry::clearChannelList(){

        this->dataList.clear();
        GSCommon::clearLayout(this->flowLayout_radio);
    }


    /**
     * @brief RadioCountry::requestData : 채널 목록 데이터 요청
     */
    void RadioCountry::requestData(){

        this->requestCountryList();
        this->requestFavoriteList();
    }


    /**
     * @brief RadioCountry::requestChannelList : 채널 목록 요청
     */
    void RadioCountry::requestChannelList(){

        int tmp_radioID = settings->value(rosesettings.SETTINGS_RADIO_ID,2).toInt();
        QString tmp_radioName = settings->value(rosesettings.SETTINGS_RADIO_NAME,"KOREA").toString();

        // 부모의 현재 서브메뉴명 변경 : 현재 선택된 콤보박스의 값으로...
        emit signal_changedCurrSubMenuName(QString("%1▼").arg(tmp_radioName));

        // 임시 :: 데이터 수가 0인경우에만 요청
        //if(dataList.size() == 0 || tmp_radioID!=currCountryId){
        if(true){

            this->clearChannelList();

            this->currCountryId = tmp_radioID;
            this->currCountryName = tmp_radioName;

            if(!global.device.getDeviceRoseToken().isEmpty()){
                NetworkHttp *network = new NetworkHttp(this);
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QJsonArray jsonCountryID;
                jsonCountryID.append(currCountryId);

                QJsonObject json;
                json.insert("roseToken", global.device.getDeviceRoseToken());
                json.insert("countryID", jsonCountryID);

                network->request(HTTP_CHANNEL_LIST, QString("%1/IF_ROSERADIO_001")
                                 //.arg(global.hifimp)
                                 .arg(global.release_roseaudio)
                                 , json, true);
            }
        }
    }


    /**
     * @brief RadioCountry::requestCountryList : 국가 리스트 요청
     */
    void RadioCountry::requestCountryList(){

        if(this->listConntry->count() == 0){

            NetworkHttp *network = new NetworkHttp(this);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QJsonArray jsonCountryID;
            jsonCountryID.append(2);

            QJsonObject json;
            json.insert("roseToken", global.device.getDeviceRoseToken());

            network->request(HTTP_COUNTRY_LIST, QString("%1/IF_ROSERADIO_002")
                             //.arg(global.hifimp)
                             .arg(global.release_roseaudio)
                             , json, true);
        }
    }


    /**
     * @brief RadioCountry::requestFavoriteList : 즐겨찾기 채널 리스트 요청
     */
    void RadioCountry::requestFavoriteList(){

        if(global.flagConnected){

            this->favorUrlList.clear();

            NetworkHttp *network = new NetworkHttp(this);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));


            QJsonObject json;
            json.insert("filter", "fav");

            network->request(HTTP_FAVORITE_LIST, QString("http://%1:%2/get.radio.user.channels.item")
                             .arg(global.device.getDeviceIP())
                             .arg(global.port)
                             , json, true);
        }
    }


    /**
     * @brief RadioCountry::appendRadioChannel : 채널 UI 추가
     * @details 썸네일 이미지 path가 없는 경우 radio_nm 이용
     * radio_nm 이용시 QT wrap은 문자열에 white space가 없는 경우 동작하지 않음.
     * '_' 문자열을 whitespace로 변경하는 것으로 처리함.
     * @param p_jsonObject QJsonObject 채널 UI 추가에 필요한 Json
     */
    void RadioCountry::appendRadioChannel(const QJsonObject &p_jsonObject){

        DataRadio *data = new DataRadio;
        data->setData(p_jsonObject);

        bool flagIsFavor = false;
        for(int i = 0 ; i < favorUrlList.size(); i++){
            if(favorUrlList.at(i).startsWith(data->getRadio_url().split("?").at(0))){
                flagIsFavor = true;
                break;
            }
        }
        data->setFavorite(flagIsFavor?1:0);

        this->dataList.append(data);

        ImageLabelRadio *tmp_widget = new ImageLabelRadio();
        tmp_widget->setBackgroundImg(dataList.size() % 10 + 1);

        // 라디오 채널 이미지 => 채널명 TEXT 사용 :: 20200705 미팅
        //if(data->getRadio_thumbnail_url().isEmpty()){
        if(true){

            // QT wrap 문제로 인한 문자열 교체
            QString radio_name = data->getRadio_nm();
            radio_name = radio_name.replace("_"," ");
            tmp_widget->setTitle(radio_name);
        }else{
            tmp_widget->setImageTitle(data->getRadio_thumbnail_url());
        }
        tmp_widget->setHover();
        tmp_widget->setFavorStatus(flagIsFavor);
        tmp_widget->setProperty("index", this->flowLayout_radio->count());

        this->flowLayout_radio->addWidget(tmp_widget);

        // Connect
        connect(tmp_widget, SIGNAL(clickedHoverItem(QString)), SLOT(slot_clickedHoverItem(QString)));
    }


    /**
     * @brief RadioCountry::slot_responseHttp : [슬롯] Http 요청 결과
     * @param p_id int 요청 아이디
     * @param p_jsonObject QJsonObject response
     */
    void RadioCountry::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

        if(p_id==HTTP_CHANNEL_LIST){
            this->setResultOfChannelList(p_jsonObject);
        }
        else if(p_id==HTTP_COUNTRY_LIST){
            this->setResultOfCountryList(p_jsonObject);
        }
        else if(p_id==HTTP_FAVORITE_LIST){
            this->setResultOfFavoriteList(p_jsonObject);

            this->requestChannelList();
        }
    }


    /**
     * @brief RadioCountry::setResultOfChannelList : 채널리스트 요청 결과 처리
     * @param p_jsonObject QJsonObject response
     */
    void RadioCountry::setResultOfChannelList(const QJsonObject &p_jsonObject){

        const QString jsonKey_flagOk = "flagOk";
        const QString jsonKey_status = "status";
        const QString jsonKey_resultCd = "resultCd";
        const QString jsonKey_resultMsg = "resultMsg";
        const QString jsonKey_data = "data";
        const QString jsonKey_country_ch_set = "country_ch_set";
        const QString jsonKey_ch_info = "ch_info";

        if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
            if(p_jsonObject.contains(jsonKey_status)){
                QJsonObject jsonStatus = p_jsonObject[jsonKey_status].toObject();
                if(jsonStatus.contains(jsonKey_resultCd)){
                    QString tmp_resultCd = jsonStatus[jsonKey_resultCd].toString("");
                    if(tmp_resultCd.toLower()=="success"){
                        if(p_jsonObject.contains(jsonKey_data)){
                            QJsonObject jsonData = p_jsonObject[jsonKey_data].toObject();
                            if(jsonData.contains(jsonKey_country_ch_set)){
                                QJsonArray jsonCHList = jsonData[jsonKey_country_ch_set].toArray();

                                for(int i = 0 ; i < jsonCHList.size(); i++){
                                    QJsonObject jsonCountry = jsonCHList.at(i).toObject();
                                    if(jsonCountry.contains(jsonKey_ch_info)){
                                        QJsonArray jsonCH = jsonCountry[jsonKey_ch_info].toArray();

                                        for(int j=0 ; j < jsonCH.size();j++){
                                            this->appendRadioChannel(jsonCH.at(j).toObject());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    /**
     * @brief RadioCountry::setResultOfCountryList : 국가 리스트 요청 결과 처리
     * @param p_jsonObject QJsonObject response
     */
    void RadioCountry::setResultOfCountryList(const QJsonObject &p_jsonData){

        const QString jsonKey_flagOk = "flagOk";
        const QString jsonKey_status = "status";
        const QString jsonKey_resultCd = "resultCd";
        const QString jsonKey_data = "data";
        const QString jsonKey_countryID = "countryID";
        const QString jsonKey_countryName = "countryName";

        if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
            if(p_jsonData.contains(jsonKey_status)){
                QJsonObject jsonStatus = p_jsonData[jsonKey_status].toObject();
                if(jsonStatus.contains(jsonKey_resultCd)){
                    QString tmp_resultCd = jsonStatus[jsonKey_resultCd].toString("");
                    if(tmp_resultCd.toLower()=="success"){
                        if(p_jsonData.contains(jsonKey_data)){
                            QJsonArray jsonChannelArray = p_jsonData[jsonKey_data].toArray();

                            QVBoxLayout *vl_country = new QVBoxLayout();
                            vl_country->setContentsMargins(0,0,0,0);
                            vl_country->setSpacing(0);

                            for(int i = 0 ; i < jsonChannelArray.size(); i++){
                                QJsonObject jsonChannel = jsonChannelArray.at(i).toObject();

                                int countryID=0;
                                QString countryName;

                                if(jsonChannel.contains(jsonKey_countryID)){
                                    countryID = jsonChannel[jsonKey_countryID].toInt();
                                }
                                if(jsonChannel.contains(jsonKey_countryName)){
                                    countryName = jsonChannel[jsonKey_countryName].toString();
                                }

                                QString tmp_background = "transparent";
                                if(currCountryId==countryID){
                                    tmp_background = "#B18658";
                                    currCountryIndex = i;
                                }

                                QString tmp_styleSheet = QString("#lb_comboItem { font-size:18px;color:#FFFFFF;background-color:%1; }").arg(tmp_background);
                                tmp_styleSheet += "#lb_comboItem:hover { background-color:#4D4D4D; }";

                                ClickableLabel *lb_comboItem = new ClickableLabel();
                                lb_comboItem->setProperty("countryId", countryID);
                                lb_comboItem->setProperty("countryName", countryName);
                                lb_comboItem->setProperty("countryIndex",i);
                                lb_comboItem->setObjectName("lb_comboItem");
                                lb_comboItem->setText(countryName);
                                lb_comboItem->setContentsMargins(25,0,0,0);
                                lb_comboItem->setStyleSheet(tmp_styleSheet);
                                lb_comboItem->setFixedSize(200,40);
                                lb_comboItem->setCursor(Qt::PointingHandCursor);

                                listConntry->append(lb_comboItem);
                                vl_country->addWidget(lb_comboItem);

                                connect(lb_comboItem, SIGNAL(signal_clicked()), SLOT(slot_clickedCountryItem()));
                            }

                            this->widget_country->setLayout(vl_country);

                            QScrollArea *scrollArea = new QScrollArea;
                            scrollArea->setWidget(widget_country);
                            scrollArea->setContentsMargins(0,0,0,0);
                            scrollArea->setWidgetResizable(true);
                            scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                            scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                            scrollArea->setStyleSheet("border:0px;background-color:transparent;");

                            QVBoxLayout *box_total = new QVBoxLayout;
                            box_total->setContentsMargins(0,0,0,0);
                            box_total->setSpacing(0);
                            box_total->setAlignment(Qt::AlignTop);
                            box_total->addWidget(scrollArea);

                            widget_combobox->setLayout(box_total);
                            widget_combobox->setGeometry(250, 128, this->widget_combobox->sizeHint().width(), 600);
                        }
                    }
                }
            }
        }
    }


    /**
     * @brief RadioCountry::setResultOfFavoriteList : 즐겨찾기 채널 목록 처리
     * @details 라디오 url만 따로 보관한다.
     * @param p_jsonObject
     */
    void RadioCountry::setResultOfFavoriteList(const QJsonObject &p_jsonObject){

        const QString jsonKey_flagOk = "flagOk";
        const QString jsonKey_status = "status";
        const QString jsonKey_outs = "outs";
        const QString jsonKey_resultMsg = "resultMsg";
        const QString jsonKey_arr = "arr";
        const QString jsonKey_data = "data";

        if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
            if(p_jsonObject.contains(jsonKey_status)){
                QJsonObject jsonStatus = p_jsonObject[jsonKey_status].toObject();
                // TODO:: Search Devices 16/02/2021 by disket
                //if(jsonStatus.contains(jsonKey_outs)){
                    QString tmp_outs = jsonStatus[jsonKey_outs].toString("");
                    if(tmp_outs.toLower()=="ok"){
                        if(p_jsonObject.contains(jsonKey_arr)){
                            QJsonArray jsonArr = p_jsonObject[jsonKey_arr].toArray();

                            for(int i = 0 ; i < jsonArr.size(); i++){
                                DataRadio *data = new DataRadio;
                                data->setDataForFavor(jsonArr.at(i).toObject());
                                favorUrlList.append(data->getRadio_url());
                                delete data;
                            }
                        }
                    }
                //}
            }
        }
    }


    /**
     * @brief RadioCountry::slot_clickedHoverItem : [슬롯]라디오 호버
     * @param p_code
     */
    void RadioCountry::slot_clickedHoverItem(const QString &p_code){

        QJsonObject jsonData;
        int tmp_index = sender()->property("index").toInt();
        if(tmp_index >= 0 && tmp_index < dataList.size()){

            DataRadio *tmp_data = dataList.at(tmp_index);

            QJsonArray jsonArrAlbum;
            jsonArrAlbum.append(tmp_data->getDataJsonForOptionPopup());

            jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Radio);
            jsonData.insert("flagFavorChannel",tmp_data->getFavorite());
            jsonData.insert(KEY_OP_albumImg, "");    // 팝업 없으므로 empty
            jsonData.insert(KEY_OP_DATA,jsonArrAlbum);

            if(p_code==HOVER_CODE_FAV){
                jsonData.insert("data_fav", tmp_data->getDataJsonForUpdateFavor());
                tmp_data->setFavorite(tmp_data->getFavorite() >0 ? 0 : 1);
            }
        }
        // 공용으로 쓰기 때문에 Abstract 부모가 하게 맡긴다.
        emit linker->signal_clickedHoverItem(p_code, jsonData);
    }


    /**
     * @brief RadioCountry::slot_showComboBoxPopup 국가 콤보박스 show
     */
    void RadioCountry::showComboBoxPopup(){

        if(this->flagShowedComboBox==false){
            this->flagShowedComboBox = true;
            this->widget_combobox->show();
        }else{
            this->flagShowedComboBox = false;
            this->widget_combobox->hide();
        }
    }


    /**
     * @brief RadioCountry::setCountryItemFocusStyle : 국가 선택 콤보박스 라벨 포커스 스타일 적용
     * @param p_indexFocus
     */
    void RadioCountry::setCountryItemFocusStyle(const int &p_indexFocus){

        if(p_indexFocus < listConntry->count()){
            QString tmp_focusStyleSheet = QString("#lb_comboItem { font-size:18px;color:#FFFFFF;background-color:%1; }").arg("#B18658");
            tmp_focusStyleSheet += "#lb_comboItem:hover { background-color:#4D4D4D; }";
            this->listConntry->at(p_indexFocus)->setStyleSheet(tmp_focusStyleSheet);

            if(currCountryIndex != p_indexFocus && currCountryIndex >=0 && currCountryIndex < listConntry->size()){
                QString tmp_focusStyleSheet = QString("#lb_comboItem { font-size:18px;color:#FFFFFF;background-color:%1; }").arg("transparent");
                tmp_focusStyleSheet += "#lb_comboItem:hover { background-color:#4D4D4D; }";

                this->listConntry->at(currCountryIndex)->setStyleSheet(tmp_focusStyleSheet);

                this->currCountryIndex = p_indexFocus;
            }
        }
    }


    /**
     * @brief RadioCountry::slot_clickedCountryItem : [슬롯] 국가 선택
     * @details 서브메뉴명 변경(선택한 국가), 선택한 국가의 라디오 채널 목록 요청
     */
    void RadioCountry::slot_clickedCountryItem(){

        int tmp_index = sender()->property("countryIndex").toInt();
        int tmp_countryId = sender()->property("countryId").toInt();
        QString tmp_countryName = sender()->property("countryName").toString();

        this->setCountryItemFocusStyle(tmp_index);

        this->settings->setValue(rosesettings.SETTINGS_RADIO_ID, tmp_countryId);
        this->settings->setValue(rosesettings.SETTINGS_RADIO_NAME, tmp_countryName);

        this->hide_comboBoxWidget();
        this->requestData();
    }


    /**
     * @brief RadioCountry::slot_changedFavorChannel:[슬롯]즐겨찾기 채널 변경
     * @details 즐겨찾기 화면에서 즐겨찾기 설정/해제가 이루어진 경우에만 목록 갱신
     */
    void RadioCountry::slot_changedFavorChannel(const bool &flagChangedInFavor){


        if(flagChangedInFavor){
            // 데이터, UI 삭제 : 화면진입시 채널 목록 갱신 위함
            //clearChannelList();
        }

        // 즐겨찾기 목록 갱신 : 채널 목록 GET이후 즐겨찾기 체크 위함
        this->requestFavoriteList();
    }


    void RadioCountry::resizeEvent(QResizeEvent *event){

        QWidget::resizeEvent(event);

        int tmp_comoboxHeigth = this->widget_country->sizeHint().height();
        if(tmp_comoboxHeigth > this->height()){
            tmp_comoboxHeigth = height();
        }

        this->widget_combobox->setGeometry(250, 128, widget_country->sizeHint().width(), tmp_comoboxHeigth);

        this->widget_topContrl->setGeometry((this->width()-this->widget_topContrl->sizeHint().width()), 71,  this->widget_topContrl->sizeHint().width(), this->widget_topContrl->sizeHint().height());
    }


    /**
     * @brief RadioCountry::slot_clickedRefresh : [슬롯] 새로고침 클릭
     */
    void RadioCountry::slot_clickedRefresh(){

        this->clearChannelList();
        this->requestData();
    }


    /**
     * @brief RadioCountry::slot_clickedAddChannel : [슬롯] 방송채널 추가 클릭
     */
    void RadioCountry::slot_clickedAddChannel(){

        Dialog::DialogEditRadio *dlg = new Dialog::DialogEditRadio(this);
        int result = dlg->exec();

        if(result == QDialog::Accepted){
            // nothing
            //clearChannelList();
            //requestData();
        }
    }
}
