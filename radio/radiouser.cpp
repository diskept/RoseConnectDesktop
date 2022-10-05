#include "radiouser.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "widget/VerticalScrollArea.h"
#include "widget/imagelabelradio.h"
#include "dialogeditradio.h"
#include <widget/framewidget.h>
#include <QJsonArray>
#include <QHBoxLayout>
#include <QCheckBox>
#include "widget/optionpopup.h"


/**
 * @brief RadioUser::RadioUser : 라디오-사용자채널
 * @details 사용자가 추가한 채널 목록
 * @param parent
 */
RadioUser::RadioUser(QWidget *parent)
    : QWidget(parent)
{

    setInit();
    setUIControl();
    setTopUIControl(parent);
    //requestFavoriteList();
}


RadioUser::~RadioUser(){

    favorUrlList.clear();

    this->deleteLater();
}


/**
 * @brief RadioUser::show_topBtnControl : public show, 새로고침, 방송채널 추가 UI
 */
void RadioUser::show_topBtnControl(){
    widget_topContrl->setGeometry((this->width()-this->widget_topContrl->sizeHint().width()), 0,  this->widget_topContrl->sizeHint().width(), this->widget_topContrl->sizeHint().height());
    widget_topContrl->show();
}


/**
 * @brief RadioUser::hide_topBtnFavorite : public hide, 새로고침, 방송채널 추가 UI
 */
void RadioUser::hide_topBtnControl(){

    this->widget_topContrl->hide();
}


/**
 * @brief RadioUser::setInit : 초기 세팅
 */
void RadioUser::setInit(){

    linker = Linker::getInstance();
    connect(linker, SIGNAL(signal_changedRadioUserChannel()), this, SLOT(slot_changedUserChannelList()));
    connect(linker, SIGNAL(signal_changedRadioFavorChannel(bool)), SLOT(slot_changedFavorChannel(bool)));
}


/**
 * @brief RadioUser::setTopUIControl : 새로고침, 방송채널 TOP UI 세팅
 */
void RadioUser::setTopUIControl(QWidget *parent){
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
 * @brief RadioUser::setUIControl : UI 세팅
 */
void RadioUser::setUIControl(){

    /// 채널편집 UI
    QPixmap pixmap(":images/icon_setting.png");
    QIcon ButtonIcon(pixmap);
    QPushButton *btn_mod = new QPushButton;
    btn_mod->setIcon(ButtonIcon);
    btn_mod->setIconSize(pixmap.rect().size());
    btn_mod->setText(tr("Edit Channel"));
    //btn_mod->setText(tr("채널편집"));
    btn_mod->setStyleSheet("background-color:transparent;font-size:18px;color:#FFFFFF;");
    btn_mod->setCursor(Qt::PointingHandCursor);
    btn_mod->setFixedHeight(50);

    QHBoxLayout *box_top = new QHBoxLayout;
    box_top->setContentsMargins(21,10,21,10);
    box_top->setSpacing(0);
    box_top->addWidget(btn_mod);
    box_top->addStretch(1);

    wg_top = new QWidget;
    wg_top->setObjectName("wg_top");
    wg_top->setContentsMargins(0,0,0,0);
    wg_top->setStyleSheet("#wg_top {background-color:#171717;}");
    wg_top->setLayout(box_top);


    /// 라디오 채널 UI
    flowLayout = new FlowLayout(0,20,20);

    QWidget *wg_contents = new QWidget;
    wg_contents->setLayout(flowLayout);
    wg_contents->setContentsMargins(33,20,28,0);

    VerticalScrollArea *scrollArea = new VerticalScrollArea;
    scrollArea->setWidget(wg_contents);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet("border:0px;");

    QVBoxLayout *boxMain = new QVBoxLayout;
    boxMain->setContentsMargins(0,0,0,0);
    boxMain->addWidget(wg_top);
    boxMain->addWidget(scrollArea);

    setLayout(boxMain);

    // connection
    connect(btn_mod, SIGNAL(clicked()), SLOT(slot_clickedModify()));
}


/**
 * @brief RadioUser::requestFavoriteList : 즐겨찾기 채널 리스트 요청
 * @details : 미사용 => 사용자채널 즐겨찾기는 API가 지원하지 않음.
 *
 */
void RadioUser::requestFavoriteList(){

    if(global.flagConnected){

        favorUrlList.clear();

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
 * @brief RadioUser::clearChannelList : 채널 목록 클리어
 */
void RadioUser::clearChannelList(){
    dataList.clear();
    GSCommon::clearLayout(flowLayout);
}


/**
 * @brief RadioUser::requestData : 채널 목록 데이터 요청
 */
void RadioUser::requestData(bool flagRefresh){

    // 임시 :: 데이터 수가 0인경우에만 요청
    if(dataList.size() == 0 || flagRefresh){
        clearChannelList();

        if(!global.device.getDeviceRoseToken().isEmpty()){
            NetworkHttp *network = new NetworkHttp(this);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));


            QJsonObject json;
            json.insert("filter", "etc");

            network->request(HTTP_CHANNEL_LIST, QString("http://%1:%2/get.radio.user.channels.item")
                             .arg(global.device.getDeviceIP())
                             .arg(global.port)
                             , json, true);
        }
    }
}


/**
 * @brief RadioUser::appendRadioChannel : 채널 UI 추가
 * @details 썸네일 이미지 path가 없는 경우 radio_nm 이용
 * radio_nm 이용시 QT wrap은 문자열에 white space가 없는 경우 동작하지 않음.
 * '_' 문자열을 whitespace로 변경하는 것으로 처리함.
 * @param p_jsonObject QJsonObject 채널 UI 추가에 필요한 Json
 */
void RadioUser::appendRadioChannel(const QJsonObject &p_jsonObject){

    DataRadio *data = new DataRadio;
    data->setDataForFavor(p_jsonObject);

    /*
     * 사용자 채널은 즐겨찾기 API 미지원
    bool flagIsFavor;
    if(favorUrlList.contains(data->getRadio_url())){
        flagIsFavor = true;
        data->setFavorite(1);
    }else{
        flagIsFavor = false;
        data->setFavorite(0);
    }
    */

    dataList.append(data);

    ImageLabelRadio *tmp_widget = new ImageLabelRadio(this, ImageLabelRadio::Mode::User);
    tmp_widget->setBackgroundImg(dataList.size()%10+1);

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
    //tmp_widget->setFavorStatus(flagIsFavor);
    tmp_widget->setProperty("index", flowLayout->count());

    flowLayout->addWidget(tmp_widget);
    // Connect
    connect(tmp_widget, SIGNAL(clickedHoverItem(QString)), SLOT(slot_clickedHoverItem(QString)));
}


/**
 * @brief RadioUser::slot_responseHttp : [슬롯] 채널 요청 결과
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void RadioUser::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    if(p_id==HTTP_CHANNEL_LIST){
        setResultOfChannelList(p_jsonObject);
    }
    else if(p_id==HTTP_FAVORITE_LIST){

    }
}


/**
 * @brief RadioUser::setResultOfChannelList : 채널리스트 요청 결과 처리
 * @param p_jsonObject QJsonObject response
 */
void RadioUser::setResultOfChannelList(const QJsonObject &p_jsonObject){

    jsonData = p_jsonObject;

    bool flagHasChannel = false;

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
                //if(tmp_outs.toLower()=="ok"){
                    if(p_jsonObject.contains(jsonKey_arr)){
                        QJsonArray jsonArr = p_jsonObject[jsonKey_arr].toArray();

                        for(int i = 0 ; i < jsonArr.size(); i++){
                            flagHasChannel = true;
                            appendRadioChannel(jsonArr.at(i).toObject());
                        }
                    }
                //}
            //}
        }
    }
    wg_top->setVisible(flagHasChannel);
}


/**
 * @brief RadioCountry::setResultOfFavoriteList : 즐겨찾기 채널 목록 처리
 * @details 라디오 url만 따로 보관한다.
 * @param p_jsonObject
 */
void RadioUser::setResultOfFavoriteList(const QJsonObject &p_jsonObject){

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
                //if(tmp_outs.toLower()=="ok"){
                    if(p_jsonObject.contains(jsonKey_arr)){
                        QJsonArray jsonArr = p_jsonObject[jsonKey_arr].toArray();

                        for(int i = 0 ; i < jsonArr.size(); i++){
                            DataRadio *data = new DataRadio;
                            data->setDataForFavor(jsonArr.at(i).toObject());
                            favorUrlList.append(data->getRadio_url());
                            delete data;
                        }
                    }
                //}
            //}
        }
    }
}


/**
 * @brief RadioUser::slot_clickedModify : [슬롯] 채널편집 클릭
 */
void RadioUser::slot_clickedModify(){

    jsonData.insert(KEY_PAGE_CODE, PAGECODE_R_EDIT);
    jsonData.insert("title", tr("Edit Channel"));
    //jsonData.insert("title", tr("채널편집"));

    emit this->signal_clickedEdit(jsonData);
}


/**
 * @brief RadioUser::slot_clickedHoverItem : [슬롯]라디오 호버
 * @param p_code
 */
void RadioUser::slot_clickedHoverItem(const QString &p_code){

    QJsonObject jsonData;
    int tmp_index = sender()->property("index").toInt();
    if(tmp_index >= 0 && tmp_index < dataList.size()){
        DataRadio *tmp_data = dataList.at(tmp_index);

        QJsonArray jsonArrAlbum;
        jsonArrAlbum.append(tmp_data->getDataJsonForOptionPopup());

        jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Radio);
        jsonData.insert("flagFavorChannel", 0);
        jsonData.insert(KEY_OP_albumImg, "");    // 팝업 없으므로 empty
        jsonData.insert(KEY_OP_DATA,jsonArrAlbum);

        if(p_code=="fav"){
            jsonData.insert("data_fav", tmp_data->getDataJsonForUpdateFavor());
            tmp_data->setFavorite(tmp_data->getFavorite() >0 ? 0 : 1);
        }

    }

    // 공용으로 쓰기 때문에 Abstract 부모가 하게 맡긴다.
    emit linker->signal_clickedHoverItem(p_code, jsonData);
}


/**
 * @brief RadioUser::slot_clickedRefresh : [슬롯] 새로고침 클릭
 */
void RadioUser::slot_clickedRefresh(){

    requestData(true);
}


/**
 * @brief RadioUser::slot_clickedAddChannel : [슬롯] 방송채널 추가 클릭
 */
void RadioUser::slot_clickedAddChannel(){

    Dialog::DialogEditRadio *dlg = new Dialog::DialogEditRadio(this);
    int result = dlg->exec();

    if(result == QDialog::Accepted){
        requestData(true);
    }
}


/**
 * @brief RadioUser::slot_changedFavorChannel:[슬롯]즐겨찾기 채널 변경
 * @details 즐겨찾기 화면에서 즐겨찾기 설정/해제가 이루어진 경우에만 목록 갱신
 * do nothing :: 즐겨찾기 설정/해제 API가 사용자채널은 지원하지 않음.
 */
void RadioUser::slot_changedFavorChannel(const bool &flagChangedInFavor){

    Q_UNUSED(flagChangedInFavor);
    /*
    if(flagChangedInFavor){

        // 데이터, UI 삭제 : 화면진입시 채널 목록 갱신 위함
        dataList.clear();
        GSCommon::clearLayout(flowLayout);

        // 즐겨찾기 목록 갱신 : 채널 목록 GET이후 즐겨찾기 체크 위함
        requestFavoriteList();
    }
    */
}


/**
 * @brief RadioUser::slot_changedUserChannelList : [슬롯] 사용자 채널 목록 변경
 * @details 채널목록갱신
 */
void RadioUser::slot_changedUserChannelList(){

    dataList.clear();
    GSCommon::clearLayout(flowLayout);
}
