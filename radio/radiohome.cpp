#include "radio/radiohome.h"

#include "radio/dialogeditradio.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "data/datapopup.h"

#include "widget/framewidget.h"
#include "widget/imagelabelradio.h"
#include "widget/optionpopup.h"
#include "widget/toastmsg.h"
#include "widget/VerticalScrollArea.h"

#include <QJsonArray>


namespace radio {

const int HTTP_CHANNEL_LIST = 99;
const int HTTP_REQ_PLAY = 98;


    /**
     * @brief RadioHome::RadioHome : 라디오-즐겨찾기
     * @details 사용자가 즐겨찾기 등록한 라디오 채널 목록
     * 특이사항 : 라디오-국가, 라디오-사용자 채널과 다르게 FlowLayout h,v spacing 사용하지 않음.
     * 사유 : 즐겨찾기 해제로 라디오채널 아이템 삭제시 간격 조정 필요
     * @param parent
     * @note 사용자채널에만 존재하던 새로고침, 방송채널 추가 전 메뉴 추가요청<br>
     * 공통 Widget화 하지 않고 Copy하여 처리단만 다르게 함.
     */
    RadioHome::RadioHome(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_roseviewAll, parent)
    {
        global.isDrawingMainContent = false;

        setInit();
        setUIControl();
        setTopUIControl(parent);
    }


    RadioHome::~RadioHome(){

        dataList.clear();
        GSCommon::clearLayout(this->flowLayout_radio);

        this->deleteLater();
    }


    /**
     * @brief RadioUser::show_topBtnControl : public show, 새로고침, 방송채널 추가 UI
     */
    void RadioHome::show_topBtnControl(){

        this->widget_topContrl->show();
    }


    /**
     * @brief RadioUser::hide_topBtnFavorite : public hide, 새로고침, 방송채널 추가 UI
     */
    void RadioHome::hide_topBtnControl(){

        this->widget_topContrl->hide();
    }


    /**
     * @brief RadioHome::setInit : 초기 세팅
     */
    void RadioHome::setInit(){

        linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_changedRadioFavorChannel(bool)), SLOT(slot_changedFavorChannel(bool)));
    }


    /**
     * @brief RadioHome::setUIControl : UI 세팅
     */
    void RadioHome::setUIControl(){

        /*flowLayout = new FlowLayout(0,0,0);

        QWidget *wg_contents = new QWidget;
        wg_contents->setLayout(flowLayout);
        wg_contents->setContentsMargins(0,0,0,0);

        VerticalScrollArea *scrollArea = new VerticalScrollArea;
        scrollArea->setWidget(wg_contents);
        scrollArea->setWidgetResizable(true);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setStyleSheet("border:0px;");

        QVBoxLayout *boxMain = new QVBoxLayout;
        boxMain->setContentsMargins(23,32,18,0);
        boxMain->addWidget(scrollArea);

        setLayout(boxMain);*/

        // 항상 부모클래스의 함수 먼저 호출
        AbstractRoseHomeSubWidget::setActivePage();

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        this->radio_widget_width = 200;
        this->radio_widget_margin = 20;

        this->flowLayout_radio = this->get_addUIControl_flowLayout(0, 20);
    }


    /**
     * @brief RadioHome::setTopUIControl : 새로고침, 방송채널 TOP UI 세팅
     */
    void RadioHome::setTopUIControl(QWidget *parent){

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

        this->widget_topContrl = new QWidget(parent);
        this->widget_topContrl->setContentsMargins(0, 15, 77, 0);
        this->widget_topContrl->setObjectName("widget_topContrl");
        this->widget_topContrl->setLayout(hl_topControl);
        this->widget_topContrl->setStyleSheet("#widget_topContrl { background-color:#171717; } ");

        connect(btn_refresh, SIGNAL(clicked()), SLOT(slot_clickedRefresh()));
        connect(btn_add, SIGNAL(clicked()), SLOT(slot_clickedAddChannel()));
    }


    /**
     * @brief RadioHome::clearChannelList : 채널 목록 클리어
     */
    void RadioHome::clearChannelList(){

        dataList.clear();
        GSCommon::clearLayout(this->flowLayout_radio);
    }


    /**
     * @brief RadioCountry::requestData : 채널 목록 데이터 요청
     */
    void RadioHome::requestData(){

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//c230322_3

        if(this->width()-(this->radio_widget_width + this->radio_widget_margin) >= 0){
            this->setFlowLayoutResize(this, this->flowLayout_radio, this->radio_widget_width, this->radio_widget_margin);
        }

        // 임시 :: 데이터 수가 0인경우에만 요청
        if(dataList.size() == 0){
            clearChannelList();

            if(!global.device.getDeviceRoseToken().isEmpty()){
                NetworkHttp *network = new NetworkHttp(this);
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                QJsonObject json;
                json.insert("filter", "fav");
                network->request(HTTP_CHANNEL_LIST, QString("http://%1:%2/get.radio.user.channels.item")
                                 .arg(global.device.getDeviceIP())
                                 .arg(global.port)
                                 , json, true);
            }
        }

        print_debug();ContentLoadingwaitingMsgHide();//c230322_3
    }

    /**
     * @brief RadioHome::appendRadioChannel : 채널 UI 추가
     * @details 썸네일 이미지 path가 없는 경우 radio_nm 이용
     * radio_nm 이용시 QT wrap은 문자열에 white space가 없는 경우 동작하지 않음.
     * '_' 문자열을 whitespace로 변경하는 것으로 처리함.
     * @param p_jsonObject QJsonObject 채널 UI 추가에 필요한 Json
     */
    void RadioHome::appendRadioChannel(const QJsonObject &p_jsonObject){

        DataRadio *data = new DataRadio;
        data->setDataForFavor(p_jsonObject);

        dataList.append(data);

        ImageLabelRadio *tmp_widget = new ImageLabelRadio();
        tmp_widget->setBackgroundImg(dataList.size() % 10 + 1);

        // 라디오 채널 이미지 => 채널명 TEXT 사용 :: 20200705 미팅
        //if(data->getRadio_thumbnail_url().isEmpty()){
        if(true){

            // QT wrap 문제로 인한 문자열 교체
            QString radio_name = data->getRadio_nm();
            radio_name = radio_name.replace("_"," ");
            tmp_widget->setTitle(radio_name);
        }
        else{
            tmp_widget->setImageTitle(data->getRadio_thumbnail_url());
        }
        tmp_widget->setHover();
        tmp_widget->setProperty("index", this->flowLayout_radio->count());

        QVBoxLayout *box_wrap = new QVBoxLayout;
        box_wrap->setContentsMargins(10, 10, 10, 10);
        box_wrap->addWidget(tmp_widget);

        QWidget *wg_wrap = new QWidget;
        wg_wrap->setLayout(box_wrap);

        this->flowLayout_radio->addWidget(wg_wrap);

        // Connect
        connect(tmp_widget, SIGNAL(clickedHoverItem(QString)), SLOT(slot_clickedHoverItem(QString)));

    }

    /**
     * @brief RadioHome::slot_responseHttp : [슬롯] Http 요청 결과
     * @param p_id int 요청 아이디
     * @param p_jsonObject QJsonObject response
     */
    void RadioHome::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

        if(p_id==HTTP_CHANNEL_LIST){
            setResultOfChannelList(p_jsonObject);
        }
        else if(p_id==HTTP_REQ_PLAY){
            // nothing
        }

        sender()->deleteLater();
    }

    /**
     * @brief RadioHome::setResultOfChannelList : 채널리스트 요청 결과 처리
     * @param p_jsonObject QJsonObject response
     */
    void RadioHome::setResultOfChannelList(const QJsonObject &p_jsonObject){

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//c230322_3

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
                                appendRadioChannel(jsonArr.at(i).toObject());
                            }
                        }
                    }
                //}
            }
        }

        global.isDrawingMainContent = true;
        print_debug();ContentLoadingwaitingMsgHide();//c230322_3
    }

    /**
     * @brief RadioHome::slot_clickedHoverItem : [슬롯]라디오 호버
     * @param p_code
     */
    void RadioHome::slot_clickedHoverItem(const QString &p_code){

        QJsonObject jsonData;
        int tmp_index = sender()->property("index").toInt();
        if(tmp_index >= 0 && tmp_index < dataList.size()){
            DataRadio *tmp_data = dataList.at(tmp_index);

            QJsonArray jsonArrAlbum;
            jsonArrAlbum.append(tmp_data->getDataJsonForOptionPopup());

            jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Radio);
            jsonData.insert(KEY_OP_albumImg, "");    // 팝업 없으므로 empty
            jsonData.insert("flagFavorChannel",true);
            jsonData.insert(KEY_OP_cntStar, 0);
            jsonData.insert(KEY_OP_DATA,jsonArrAlbum);

            if(p_code==HOVER_CODE_FAV){
                jsonData.insert("data_fav", tmp_data->getDataJsonForUpdateFavor());
                tmp_data->setFavorite(0);   // 즐겨찾기 메뉴에서는 해제만 가능

                this->flowLayout_radio->itemAt(tmp_index)->widget()->hide();
            }
        }
        // 공용으로 쓰기 때문에 Abstract 부모가 하게 맡긴다.
        emit linker->signal_clickedHoverItem(p_code, jsonData);
    }

    /**
     * @brief RadioHome::slot_changedFavorChannel :[슬롯]즐겨찾기 채널 변경
     */
    void RadioHome::slot_changedFavorChannel(const bool &p_flagChangedInFavor){
        if(!p_flagChangedInFavor){
            clearChannelList();
        }
    }

    /**
     * @brief RadioHome::slot_clickedRefresh : [슬롯] 새로고침 클릭
     */
    void RadioHome::slot_clickedRefresh(){
        clearChannelList();
        requestData();
    }

    /**
     * @brief RadioHome::slot_clickedAddChannel : [슬롯] 방송채널 추가 클릭
     */
    void RadioHome::slot_clickedAddChannel(){

        Dialog::DialogEditRadio *dlg = new Dialog::DialogEditRadio(this);
        int result = dlg->exec();

        if(result == QDialog::Accepted){
            // nothing
            //clearChannelList();
            //requestData();
        }
    }
    /**
     * @brief RadioHome::resizeEvent
     * @param event
     */
    void RadioHome::resizeEvent(QResizeEvent *event){//c230223

        Q_UNUSED(event);
        widget_topContrl->setGeometry((this->width()-this->widget_topContrl->sizeHint().width()), 0,  this->widget_topContrl->sizeHint().width(), this->widget_topContrl->sizeHint().height());
        this->setFlowLayoutResize(this, this->flowLayout_radio, this->radio_widget_width, this->radio_widget_margin);
    }
}
