#include "musicfolder_colud.h"
#include "widget/folderwidgetgroup.h"
#include "common/gscommon.h"
#include "common/global.h"
#include "widget/toastmsg.h"
#include "common/networkhttp.h"
#include "login/dialogselect_cloud.h"
#include "login/dialogadd_cloud.h"
#include "widget/dialogconfirm.h"
#include <QScrollArea>
#include <QDebug>

const int HTTP_NETWORK_CLOUD_LIST = 99;
const int HTTP_NETWORK_CLOUD_DELETE = 98;
const QString ROW_TYPE_ADD = "ADD_CLOUD";
const QString ROW_TYPE_LIST = "LIST_CLOUD";

/**
 * @brief MusicFolder_Colud::MusicFolder_Colud:음악-폴더-클라우드
 * @details 클라우드 목록 구성 및 클라우드 추가
 * @param parent
 */
MusicFolder_Colud::MusicFolder_Colud(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();
}

MusicFolder_Colud::~MusicFolder_Colud(){
    qDeleteAll(listCloud->begin(), listCloud->end());
    listCloud->clear();
}

/**
 * @brief MusicFolder_Colud::setInit : 초기 세팅
 */
void MusicFolder_Colud::setInit(){
    listCloud = new QList<DataCloud*>();
}

/**
 * @brief MusicFolder_Colud::setUIControl : UI 생성 및 세팅
 */
void MusicFolder_Colud::setUIControl(){

    Folder_ClickableRow *folder_addCloud = new Folder_ClickableRow(":images/icon_net_add_fol.png",tr("Add cloud"));
    //Folder_ClickableRow *folder_addCloud = new Folder_ClickableRow(":images/icon_net_add_fol.png",tr("클라우드 추가"));
    folder_addCloud->setProperty("type", ROW_TYPE_ADD);


    box_cloudList = new QVBoxLayout;
    box_cloudList->setSpacing(0);
    box_cloudList->setContentsMargins(0,0,0,0);
    box_cloudList->setAlignment(Qt::AlignTop);

    QVBoxLayout *box_all = new QVBoxLayout;
    box_all->setSpacing(0);
    box_all->setContentsMargins(0,0,0,0);
    box_all->setAlignment(Qt::AlignTop);
    box_all->addWidget(folder_addCloud);
    box_all->addLayout(box_cloudList);

    QWidget *widget_scroll = new QWidget;
    widget_scroll->setContentsMargins(0,0,0,0);
    widget_scroll->setLayout(box_all);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(widget_scroll);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("#scrollArea { border:0px; }");
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout *vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);

    this->setLayout(vl_scroll);

    // 커텍트
    connect(folder_addCloud, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));
}

/**
 * @brief MusicFolder_Colud::showDlgOfSelectCloudType : 클라우드 타입 선택 다이얼로그 show
 */
void MusicFolder_Colud::showDlgOfSelectCloudType(){
    Dialog::DialogSelect_Cloud *dlg_cloud = new Dialog::DialogSelect_Cloud(this);
    int result = dlg_cloud->exec();

    if(result == QDialog::Accepted){
        QString tmp_selectType = dlg_cloud->getSelectedCloudName();

        if(tmp_selectType=="ftp"){
            showDlgOfAddCloud();
        }
        else if(tmp_selectType=="webdav"){
            showDlgOfAddCloud(false);
        }
    }

    delete  dlg_cloud;
}

/**
 * @brief MusicFolder_Colud::showDlgOfAddCloud : 클라우드 추가 다이얼로그 show
 * @param p_flagIsFTP bool if true FTP 추가, flase Webdav 추가
 */
void MusicFolder_Colud::showDlgOfAddCloud(bool p_flagIsFTP){

    Dialog::DialogAdd_Cloud *dlg_add;

    if(p_flagIsFTP){
        dlg_add = new Dialog::DialogAdd_Cloud(Dialog::DialogType::ADD_FTP,this);
    }else{
        dlg_add = new Dialog::DialogAdd_Cloud(Dialog::DialogType::ADD_WEBDAV,this);
    }

    int result = dlg_add->exec();
    if(result == QDialog::Accepted){
        requestCloudlist(true);
    }

    delete dlg_add;
}


/**
 * @brief MusicFolder_Colud::appendCloudUI : 사용자 클라우드 UI 세팅
 * @param p_index int index
 */
void MusicFolder_Colud::appendCloudUI(const int &p_index){
    QString tmp_name = listCloud->at(p_index)->getName();

    FolderNetwork_ClickableRow *clickableRow = new FolderNetwork_ClickableRow(":images/icon_net_cloud.png", tmp_name);
    clickableRow->setProperty("index", p_index);
    clickableRow->setProperty("type", ROW_TYPE_LIST);

    this->box_cloudList->addWidget(clickableRow);




    connect(clickableRow, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));
    connect(clickableRow, SIGNAL(signal_clickedRemove()), this, SLOT(slot_clickedRemove()));
}

/**
 * @brief MusicFolder_Colud::clearCloudList : 클라우드 목록관련 데이터 UI 초기화
 */
void MusicFolder_Colud::clearCloudList(){
    qDeleteAll(listCloud->begin(), listCloud->end());
    listCloud->clear();
    GSCommon::clearLayout(box_cloudList);
}

/**
 * @brief MusicFolder_Colud::requestRemoveCloud : 클라우드 삭제 요청
 * @details 비디오-폴더와 동일 API : IP:PORT/music_rclone_delete
 * @param p_index
 */
void MusicFolder_Colud::requestRemoveCloud(const int &p_index){
    QJsonObject tmp_json;
    tmp_json.insert("data", listCloud->at(p_index)->getData());


    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_NETWORK_CLOUD_DELETE, QString("http://%1:%2/music_rclone_delete")
                     .arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true);
}

/**
 * @brief MusicFolder_Colud::requestCloudlist:클라우드 리스트 요청
 * @details 비디오-폴더와 동일 API : IP:PORT/get_rclone_info
 * @param flagRefresh
 */
void MusicFolder_Colud::requestCloudlist(bool flagRefresh){

    if(listCloud->size()==0 || flagRefresh){
        clearCloudList();

        QJsonObject tmp_json;

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_NETWORK_CLOUD_LIST, QString("http://%1:%2/get_rclone_info")
                         .arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true);
    }
}

/**
 * @brief MusicFolder_Colud::setResultOfCloudList:클라우드 목록 요청 결과 처리
 * @param p_jsonData QJsonObject response
 */
void MusicFolder_Colud::setResultOfCloudList(const QJsonObject &p_jsonData){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_remoteItem = "remoteItem";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_remoteItem)){
            QJsonArray jsonArr_remoteItem = p_jsonData[jsonKey_remoteItem].toArray();

            for(int i=0; i < jsonArr_remoteItem.size(); i++){
                DataCloud *data = new DataCloud;
                data->setData(jsonArr_remoteItem.at(i).toObject());
                listCloud->append(data);

                appendCloudUI(i);

            }
        }
    }
}

/**
 * @brief MusicFolder_Colud::slot_responseHttp : [슬롯]Http 요청 결과 처리 함수
 * @note 쿨라우드 삭제 처리 반환값이 클라우드 목록 요청 반환값과 같다.<br>
 *
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void MusicFolder_Colud::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id == HTTP_NETWORK_CLOUD_LIST){
        setResultOfCloudList(p_jsonObject);
    }
    else if(p_id == HTTP_NETWORK_CLOUD_DELETE){
        clearCloudList();
        setResultOfCloudList(p_jsonObject);
    }
    sender()->deleteLater();
}

/**
 * @brief MusicFolder_Colud::slot_clickedRow : [슬롯] Row 클릭
 */
void MusicFolder_Colud::slot_clickedRow(){
    QString tmp_type = sender()->property("type").toString();

    if(tmp_type==ROW_TYPE_ADD){
        showDlgOfSelectCloudType();

    }
    else if(tmp_type==ROW_TYPE_LIST){
        ToastMsg::show(this,"", tr("API verification required"));
       // ToastMsg::show(this,"", tr("API 확인 필요"));
    }
}

/**
 * @brief MusicFolder_Colud::slot_clickedRemove : [슬롯] 삭제 클릭
 */
void MusicFolder_Colud::slot_clickedRemove(){
    int tmp_index = sender()->property("index").toInt();

    if(tmp_index >=0 && tmp_index < listCloud->size()){

        DialogConfirm *dlg = new DialogConfirm(this);
        dlg->setTitle(tr("Cloud deletion"));
        //dlg->setTitle(tr("클라우드 삭제"));
        dlg->setText(tr("Are you sure you want to delete?"));
        //dlg->setText(tr("정말 삭제하시겠습니까?"));

        int result = dlg->exec();

        if(result==QDialog::Accepted){
            requestRemoveCloud(tmp_index);
        }

        delete dlg;
    }
}


