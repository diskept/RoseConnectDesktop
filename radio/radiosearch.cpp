#include "radiosearch.h"
#include "common/global.h"
#include <QLabel>
#include <widget/optionpopup.h>
#include <common/gscommon.h>
#include <common/networkhttp.h>

#include <data/dataradio.h>

#include <widget/imagelabelradio.h>

const int HTTP_CHANNEL_LIST = 99;
const int HTTP_FAVORITE_LIST = 98;

/**
 * @brief RadioSearch::RadioSearch : 라디오 검색 결과 화면
 * @param parent
 */
RadioSearch::RadioSearch(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief RadioSearch::setInit : 초기 세팅
 */
void RadioSearch::setInit(){
    linker = Linker::getInstance();
}

/**
 * @brief RadioSearch::setUIControl : UI 세팅
 */
void RadioSearch::setUIControl(){
    // -------------------------------------------------------
    //  empty 라벨 박스
    // -------------------------------------------------------
    QLabel *lb_empty = new QLabel(tr("No results found."));
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
    scrollArea_content->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea_content->setStyleSheet("border:0px;");

    QVBoxLayout *boxMain = new QVBoxLayout;
    boxMain->setContentsMargins(33,42,28,0);
    boxMain->addWidget(scrollArea_content);
    boxMain->addWidget(widgetEmpty);

    setLayout(boxMain);
}

/**
 * @brief RadioSearch::showEmptyLabel : empty 라벨 Show
 * @param bool if true show
 */
void RadioSearch::showEmptyLabel(bool p_flagShow){
    if(p_flagShow){
        widgetEmpty->setVisible(true);
        scrollArea_content->setVisible(false);
    }else{
        widgetEmpty->setVisible(false);
        scrollArea_content->setVisible(true);
    }
}

/**
 * @brief RadioSearch::search : 라디오 채널 검색
 * @param p_text
 */
void RadioSearch::search(const QString &p_text){

    strSearchWord = p_text;

    // 부모의 메뉴명 변경 -> 앨범명 (서브메뉴는 hide)
    emit signal_changedMenuName("'"+p_text+"' "+tr("Search result"));
    //emit signal_changedMenuName("'"+p_text+"' "+tr("검색결과"));

    // 라디오 Favorite 채널 요쳥 선진행후 검색
    requestFavoriteList();
}

/**
 * @brief RadioCountry::requestFavoriteList : 즐겨찾기 채널 리스트 요청
 */
void RadioSearch::requestFavoriteList(){

    if(global.flagConnected){

        favorUrlList.clear();
        dataList.clear();
        GSCommon::clearLayout(flowLayout);
        showEmptyLabel(false);

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
 * @brief RadioSearch::requestChannel : 라디오 채널 요청
 */
void RadioSearch::requestChannel(){


    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));


    QJsonObject json;
    json.insert("searchText", strSearchWord);

    network->request(HTTP_CHANNEL_LIST, QString("%1/IF_ROSERADIO_003")
                     //.arg(global.hifimp)
                     .arg(global.release_roseaudio)
                     , json, true);
}

/**
 * @brief RadioSearch::setResultOfChannelList : 라디오 검색요청결과 처리
 * @param p_jsonObject
 */
void RadioSearch::setResultOfChannelList(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_resultCd = "resultCd";
     const QString jsonKey_data = "data";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonObject[jsonKey_status].toObject();
            if(jsonStatus.contains(jsonKey_resultCd)){
                QString tmp_resultCd = jsonStatus[jsonKey_resultCd].toString("");
                if(tmp_resultCd.toLower()=="success"){
                    if(p_jsonObject.contains(jsonKey_data)){
                        QJsonArray jsonData = p_jsonObject[jsonKey_data].toArray();

                        for(int i = 0 ; i < jsonData.size(); i++){
                            appendChannel(jsonData.at(i).toObject());
                        }
                    }
                }
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
 * @brief RadioSearch::setResultOfFavoriteList : 즐겨찾기 채널 목록 처리
 * @details 라디오 url만 따로 보관한다.
 * @param p_jsonObject
 */
void RadioSearch::setResultOfFavoriteList(const QJsonObject &p_jsonObject){
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
 * @brief RadioSearch::appendChannel : 채널 추가
 * @param jsonData QjsonObject 채널 정보
 */
void RadioSearch::appendChannel(const QJsonObject &p_jsonObject){
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

    dataList.append(data);


    ImageLabelRadio *tmp_widget = new ImageLabelRadio();
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
    tmp_widget->setFavorStatus(flagIsFavor);
    tmp_widget->setProperty("index", flowLayout->count());

    flowLayout->addWidget(tmp_widget);

    // Connect
    connect(tmp_widget, SIGNAL(clickedHoverItem(QString)), SLOT(slot_clickedHoverItem(QString)));
}

/**
 * @brief RadioSearch::slot_responseHttp : [슬롯] Http 요청 결과
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void RadioSearch::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    if(p_id==HTTP_CHANNEL_LIST){
        setResultOfChannelList(p_jsonObject);
    }
    else if(p_id==HTTP_FAVORITE_LIST){
        setResultOfFavoriteList(p_jsonObject);
        requestChannel();
    }

    sender()->deleteLater();
}

/**
 * @brief RadioSearch::slot_clickedHoverItem : [슬롯]라디오 호버
 * @param p_code
 */
void RadioSearch::slot_clickedHoverItem(const QString &p_code){

    QJsonObject jsonData;
    int tmp_index = sender()->property("index").toInt();
    if(tmp_index >= 0 && tmp_index < dataList.size()){

        DataRadio *tmp_data = dataList.at(tmp_index);

        QJsonArray jsonArrAlbum;
        jsonArrAlbum.append(tmp_data->getDataJsonForOptionPopup());
        jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Radio);      // 강제로 넣어주는 경우 : AbstractMainContain 페이지가 아닌 다른 페이지에서 옵션팝업을 띄우는 경우
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
