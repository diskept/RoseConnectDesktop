#include "radioedit.h"
#include "widget/imageradiocheckable.h"
#include "widget/dialogconfirm.h"
#include "common/networkhttp.h"
#include "common/global.h"
#include "widget/toastmsg.h"

#include <QPushButton>
#include <QCheckBox>
#include <QScrollArea>
#include <QJsonArray>



RadioEdit::RadioEdit(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();
}

RadioEdit::~RadioEdit(){

}

/**
 * @brief RadioEdit::setInit : 초기 세팅
 */
void RadioEdit::setInit(){
    linker = Linker::getInstance();
    listItem = new QList<ImageRadioCheckable*>();
    listPath = new QList<QString>();
    listCode = new QList<QString>();

    listPath->append("");
    listPath->append(":/images/fol_del_ico.png");   // 휴지통
    listPath->append("");

    listCode->append("");
    listCode->append("del");
    listCode->append("");
}

/**
 * @brief RadioEdit::setUIControl : UI 세팅
 */
void RadioEdit::setUIControl(){
    // 전체선택 & 삭제 UI
    checkBox = new QCheckBox;
    checkBox->setObjectName("checkBox");
    checkBox->setText(tr("Select All"));
    //checkBox->setText(tr("전체선택"));
    checkBox->setStyleSheet(
                "#checkBox {background-color:transparent;font-size:18px;color:#E6E6E6;spacing:0px;}"
                "QCheckBox::indicator:unchecked { image: url(:images/list_check_off.png);}"
                "QCheckBox::indicator:checked { image: url(:images/list_check_on.png);}"
                );
    checkBox->setCursor(Qt::PointingHandCursor);
    checkBox->setFixedHeight(50);

    QPushButton *btn_del = new QPushButton;
    btn_del->setText(tr("Delete"));
    //btn_del->setText(tr("삭제"));
    btn_del->setStyleSheet("background-color:transparent;font-size:18px;color:#FFFFFF;");
    btn_del->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *box_check = new QHBoxLayout;
    box_check->setContentsMargins(21,10,21,10);
    box_check->setSpacing(0);
    box_check->addWidget(checkBox);
    box_check->addStretch(1);
    box_check->addWidget(btn_del,0, Qt::AlignVCenter);

    QWidget *wg_check = new QWidget;
    wg_check->setObjectName("wg_check");
    wg_check->setContentsMargins(0,0,0,0);
    wg_check->setStyleSheet("#wg_check {background-color:#171717;}");
    wg_check->setLayout(box_check);

    /// 라디오 채널 UI
    flowLayout = new FlowLayout(0,20,20);

    QWidget *wg_contents = new QWidget;
    wg_contents->setLayout(flowLayout);
    wg_contents->setContentsMargins(33,20,28,0);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(wg_contents);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("border:0px;");

    QVBoxLayout *boxMain = new QVBoxLayout;
    boxMain->setContentsMargins(0,0,0,0);
    boxMain->addWidget(wg_check);
    boxMain->addWidget(scrollArea);

    setLayout(boxMain);
    connect(checkBox, SIGNAL(stateChanged(int)), SLOT(slot_checkedChanged(int)));
    connect(btn_del, SIGNAL(clicked()), SLOT(slot_clickedDel()));
}

/**
 * @brief RadioUser::requestData : 채널 목록 데이터 요청
 */
void RadioEdit::requestData(bool flagRefresh){

    // 임시 :: 데이터 수가 0인경우에만 요청
    if(dataList.size() == 0 || flagRefresh){
        dataList.clear();
        listItem->clear();
        GSCommon::clearLayout(flowLayout);

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
 * @brief RadioEdit::setResultFoChannelList : 데이터 세팅
 * @param p_jsonObject QJsonObject 사용자 채널 정보
 */
void RadioEdit::setResultFoChannelList(const QJsonObject &p_jsonObject){

    bool flagHasChannel = false;
    Q_UNUSED(flagHasChannel);

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
                            flagHasChannel = true;
                            appendRadioChannel(jsonArr.at(i).toObject());
                        }
                    }
                }
            //}
        }
    }
}

/**
 * @brief RadioEdit::appendRadioChannel : 채널 UI 추가
 * @details 썸네일 이미지 path가 없는 경우 radio_nm 이용
 * radio_nm 이용시 QT wrap은 문자열에 white space가 없는 경우 동작하지 않음.
 * '_' 문자열을 whitespace로 변경하는 것으로 처리함.
 * @param p_jsonObject QJsonObject 채널 UI 추가에 필요한 Json
 */
void RadioEdit::appendRadioChannel(const QJsonObject &p_jsonObject){


    DataRadio *data = new DataRadio;
    data->setDataForFavor(p_jsonObject);
    dataList.append(data);

    ImageRadioCheckable *tmp_widget = new ImageRadioCheckable();
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
    tmp_widget->setProperty("index", flowLayout->count());


    flowLayout->addWidget(tmp_widget);
    listItem->append(tmp_widget);

    // Connect
    connect(tmp_widget, SIGNAL(clickedHoverItem(QString)), SLOT(slot_clickedHoverItem(QString)));
}

/**
 * @brief RadioEdit::slot_checkedChanged : [슬롯] 체크상태 변경
 * @param p_state
 */
void RadioEdit::slot_checkedChanged(int p_state){

    bool flagChecked = p_state > 0 ? true : false;

    for(int i = 0; i < listItem->size(); i++){
        listItem->at(i)->checked(flagChecked);
    }
}

/**
 * @brief RadioEdit::showRemoveConfirmDlg : 삭제 확인 다이얼로그 Show
 * @param p_index int 삭제대상 채널 index
 */
void RadioEdit::showRemoveConfirmDlg(int p_index){
    DialogConfirm *dlg = new DialogConfirm(this);
    dlg->setTitle(tr("Delete Channel"));
    //dlg->setTitle(tr("채널 삭제"));
    if(p_index >=0 && p_index < dataList.size()){
        dlg->setText(dataList.at(p_index)->getRadio_nm()+" "+tr("Are you sure you want to delete the channel?"));
        //dlg->setText(dataList.at(p_index)->getRadio_nm()+" "+tr("채널을 삭제하시겠습니까?"));
    }else{
        dlg->setText(tr("Are you sure you want to delete the selected channel?"));
        //dlg->setText(tr("선택한 채널을 삭제하시겠습니까?"));
    }
    int result = dlg->exec();

    if(result==QDialog::Accepted){
        requestDelChannel(p_index);
    }

    delete dlg;
}

/**
 * @brief RadioEdit::requestDelChannel : 채널삭제 요청
 * @param p_index, 채널 index
 */
void RadioEdit::requestDelChannel(int p_index){
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));


    QJsonArray jsonArr;

    if(p_index >=0 && p_index < dataList.size()){
        QJsonObject json;
        json.insert("_id", dataList.at(p_index)->getId());
        json.insert("country", dataList.at(p_index)->getCountry());
        json.insert("favorite", dataList.at(p_index)->getFavorite());
        json.insert("isSelect", dataList.at(p_index)->getIsSelect());
        json.insert("radio_nm", dataList.at(p_index)->getRadio_nm());
        json.insert("radio_thumbnail_url", dataList.at(p_index)->getRadio_thumbnail_url());
        json.insert("radio_url", dataList.at(p_index)->getRadio_url());
        jsonArr.append(json);
    }else{
        for(int i = 0; i < listItem->size(); i++){
            if(listItem->at(i)->getCheckedValue()){
                QJsonObject json;
                json.insert("_id", dataList.at(i)->getId());
                json.insert("country", dataList.at(i)->getCountry());
                json.insert("favorite", dataList.at(i)->getFavorite());
                json.insert("isSelect", dataList.at(i)->getIsSelect());
                json.insert("radio_nm", dataList.at(i)->getRadio_nm());
                json.insert("radio_thumbnail_url", dataList.at(i)->getRadio_thumbnail_url());
                json.insert("radio_url", dataList.at(i)->getRadio_url());
                jsonArr.append(json);
            }
        }
    }


    QJsonObject jsonParam;
    jsonParam.insert("arr",jsonArr);


    network->request(HTTL_DEL_CHANNEL, QString("http://%1:%2/set.radio.channel.item.delete")
                     .arg(global.device.getDeviceIP())
                     .arg(global.port)
                     , jsonParam, true);

}


/**
 * @brief RadioEdit::setResultOfDelChannel : 채널삭제 요청 결과 처리
 * @param p_jsonObject QJsonObject response
 */
void RadioEdit::setResultOfDelChannel(const QJsonObject &p_jsonData){
    bool flagOk = false;
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonData[jsonKey_status].toObject();
            // TODO:: Search Devices 16/02/2021 by disket
            //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
               flagOk = true;
               emit linker->signal_changedRadioUserChannel();

               checkBox->setChecked(false);
               requestData(true);
            //}
        }
    }

    if(!flagOk){
        ToastMsg::show(this,"",tr("Unable to delete the channel."));
        //ToastMsg::show(this,"",tr("채널을 삭제할 수 없습니다."));
    }
}

/**
 * @brief RadioEdit::slot_clickedDel : [슬롯] 삭제 버튼 클릭
 * @details :: 서브 메뉴바의 삭제 버튼
 */
void RadioEdit::slot_clickedDel(){
    showRemoveConfirmDlg();
}

/**
 * @brief RadioEdit::slot_clickedHoverItem : [슬롯] 호버 아이템 클릭
 * @param p_code QString item code , write/del/list
 */
void RadioEdit::slot_clickedHoverItem(QString p_code){

    if(p_code=="write"){

    }else if(p_code=="del"){
        showRemoveConfirmDlg(sender()->property("index").toInt());
    }else if(p_code=="list"){

    }
}


/**
 * @brief RadioEdit::slot_responseHttp : [슬롯] http 요청 결과
 * @param p_id int 요청 아이디
 * @param jsonObject QJsonObject response
 */
void RadioEdit::slot_responseHttp(const int &p_id, const QJsonObject &jsonObject){
    if(p_id==HTTP_CHANNEL_LIST){
        setResultFoChannelList(jsonObject);
    }
    else if(p_id==HTTL_DEL_CHANNEL){
        setResultOfDelChannel(jsonObject);
    }
}


