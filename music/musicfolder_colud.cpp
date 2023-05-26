#include "music/musicfolder_colud.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

//#include "data/datapopup.h"

#include "login/dialogselect_cloud.h"
#include "login/dialogadd_cloud.h"

#include "widget/toastmsg.h"
#include "widget/dialogconfirm.h"
#include "widget/folderwidgetgroup.h"

#include <QScrollArea>
#include <QDebug>


const int SECTION_FOR_MORE_POPUP___MUSIC = 0;
const int SECTION_FOR_MORE_POPUP___VIDEO = 1;

const int HTTP_NETWORK_PLAY = 99;
const int HTTP_NETWORK_CLOUD_LIST = 98;
const int HTTP_NETWORK_CLOUD_DELETE = 97;

const QString ROW_TYPE_ADD = "ADD_CLOUD";
const QString ROW_TYPE_LIST = "LIST_CLOUD";


/**
 * @brief MusicFolder_Colud::MusicFolder_Colud:음악-폴더-클라우드
 * @details 클라우드 목록 구성 및 클라우드 추가
 * @param parent
 */
MusicFolder_Colud::MusicFolder_Colud(QWidget *parent, bool p_flagIsMusic)
    : QWidget(parent)
    , flagIsMusic(p_flagIsMusic)
{

    this->linker = Linker::getInstance();

    this->setInit();
    this->setUIControl();
}


MusicFolder_Colud::~MusicFolder_Colud(){

    qDeleteAll(this->listCloud->begin(), listCloud->end());
    this->listCloud->clear();

    this->deleteLater();
}


/**
 * @brief MusicFolder_Colud::setInit : 초기 세팅
 */
void MusicFolder_Colud::setInit(){

    this->listCloud = new QList<DataCloud*>();

    this->jsonArr_folder_step = QJsonArray();
    this->jsonArr_tracks_toPlay = QJsonArray();

    this->cloudPath_buf = "";
    this->cloudName_buf = "";
    this->flagDirChange = false;

    this->delegate_cloud = new cloudListDelegate(this);
    if(!this->flagIsMusic){
        this->delegate_cloud->setVideoFileMode();
    }

    connect(this->delegate_cloud, SIGNAL(signal_clickedBtn(bool,int,int)), this, SLOT(slot_delegateClicked(bool,int,int)));
}


/**
 * @brief MusicFolder_Colud::setUIControl : UI 생성 및 세팅
 */
void MusicFolder_Colud::setUIControl(){
\
    this->folder_addCloud = new Folder_ClickableRow(":/images/icon_net_add_fol.png", tr("Add cloud"));
    this->folder_addCloud->setProperty("type", ROW_TYPE_ADD);
    this->folder_addCloud->hide();

    connect(this->folder_addCloud, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));


    QPixmap pixmap_play(":/images/icon_list_allplay.png");
    this->btn_play = new QPushButton;
    this->btn_play->setIcon(pixmap_play);
    this->btn_play->setIconSize(pixmap_play.rect().size());
    this->btn_play->setText(tr("Play All"));
    this->btn_play->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    this->btn_play->setCursor(Qt::PointingHandCursor);

    QPixmap pixmap_shuffle(":/images/icon_list_shuffle.png");
    this->btn_shuffle = new QPushButton;
    this->btn_shuffle->setIcon(pixmap_shuffle);
    this->btn_shuffle->setIconSize(pixmap_play.rect().size());
    this->btn_shuffle->setText(tr("Shuffle Play"));
    this->btn_shuffle->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    this->btn_shuffle->setCursor(Qt::PointingHandCursor);

    connect(this->btn_play, SIGNAL(clicked()), this, SLOT(slot_clickedPlayAll()));
    connect(this->btn_shuffle, SIGNAL(clicked()), this, SLOT(slot_clickedPlayShuffle()));

    QHBoxLayout *box_option = new QHBoxLayout;
    box_option->setAlignment(Qt::AlignLeft);
    box_option->setSpacing(30);
    box_option->setContentsMargins(30, 10, 0, 10);
    box_option->addWidget(btn_play);
    box_option->addWidget(btn_shuffle);

    this->widget_cloud = new QWidget();
    this->widget_cloud->setStyleSheet("background-color:transparent;");
    this->widget_cloud->setLayout(box_option);
    this->widget_cloud->hide();


    this->box_cloudList = new QVBoxLayout;
    this->box_cloudList->setSpacing(0);
    this->box_cloudList->setContentsMargins(0,0,0,0);
    this->box_cloudList->setAlignment(Qt::AlignTop);


    QVBoxLayout *box_all = new QVBoxLayout;
    box_all->setSpacing(0);
    box_all->setContentsMargins(0,0,0,0);
    box_all->setAlignment(Qt::AlignTop);
    box_all->addWidget(this->folder_addCloud);
    box_all->addWidget(this->widget_cloud);
    box_all->addLayout(this->box_cloudList);

    QWidget *widget_scroll = new QWidget;
    widget_scroll->setContentsMargins(0,0,0,0);
    widget_scroll->setLayout(box_all);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(widget_scroll);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("#scrollArea { border:0px; }");
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QVBoxLayout *vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);

    this->setLayout(vl_scroll);
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
        this->requestCloudlist(true);
    }

    delete dlg_add;
}


/**
 * @brief MusicFolder_Colud::appendCloudUI : 사용자 클라우드 UI 세팅
 * @param p_index int index
 */
void MusicFolder_Colud::appendCloudUI(const int &p_index){

    QString tmp_name = this->listCloud->at(p_index)->getName();

    FolderNetwork_ClickableRow *clickableRow = new FolderNetwork_ClickableRow(":/images/icon_net_cloud.png", tmp_name);
    clickableRow->setProperty("index", p_index);
    clickableRow->setProperty("type", ROW_TYPE_LIST);

    this->box_cloudList->setContentsMargins(0, 0, 0, 0);
    this->box_cloudList->addWidget(clickableRow);

    connect(clickableRow, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));
    connect(clickableRow, SIGNAL(signal_clickedRemove()), this, SLOT(slot_clickedRemove()));
}


/**
 * @brief MusicFolder_Colud::appendCloudUIFolder : 클라우드 폴더 UI 세팅
 * @param p_index int index
 */
void MusicFolder_Colud::appendCloudUIFolder(const int &p_index){

    QString tmp_name = this->listCloud->at(p_index)->getName();
    //QString tmp_path = this->listCloud->at(p_index)->getPath();

    QJsonObject sndObj;
    for(int i = 0; i < this->jsonArr_folder_step.count(); i++){
        QJsonObject stepObj = this->jsonArr_folder_step.at(i).toObject();

        if(stepObj.contains(tmp_name)){
            QJsonObject tmpObj = ProcJsonEasy::getJsonObject(stepObj, tmp_name);

            QString tmpModTime = "";
            if(stepObj.contains("humanReadableSize")){
                tmpModTime = ProcJsonEasy::getString(tmpObj, "humanReadableModTime") + " · ";
            }
            else{
                tmpModTime = ProcJsonEasy::getString(tmpObj, "humanReadableModTime");
            }

            sndObj.insert("index", p_index);
            sndObj.insert("fileName", ProcJsonEasy::getString(tmpObj, "name"));
            sndObj.insert("humanReadableModTime", tmpModTime);
            sndObj.insert("size", ProcJsonEasy::getString(tmpObj, "humanReadableSize"));
            sndObj.insert("isFile", !ProcJsonEasy::getBool(tmpObj, "isDir"));

            break;
        }
    }

    QListWidgetItem *item = new QListWidgetItem;
    item->setData(Qt::UserRole, sndObj);
    item->setData(Qt::DecorationRole, QPixmap(":/images/icon_folder_bic.png"));

    this->listWidget->addItem(item);

    int list_height = this->listWidget->count() * 70;
    this->listWidget->setFixedHeight(list_height);

    /*QString tmp_name = this->listCloud->at(p_index)->getName();
    QString tmp_path = this->listCloud->at(p_index)->getPath();

    FolderCloud_ClickableRow *clickableRow = new FolderCloud_ClickableRow(":/images/icon_folder_bic.png", tmp_name, tmp_path);
    clickableRow->setProperty("index", p_index);
    clickableRow->setProperty("type", ROW_TYPE_LIST);

    this->box_cloudList->setContentsMargins(0, 0, 0, 0);
    this->box_cloudList->addWidget(clickableRow);

    connect(clickableRow, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));
    connect(clickableRow, SIGNAL(signal_clickedRemove()), this, SLOT(slot_clickedRemove()));*/
}


/**
 * @brief MusicFolder_Colud::appendCloudUIFile : 클라우드 파잉 UI 세팅
 * @param p_index int index
 */
void MusicFolder_Colud::appendCloudUIFile(const int &p_index){

    QListWidgetItem *item = new QListWidgetItem;
    item->setData(Qt::UserRole, this->jsonArr_tracks_toPlay.at(p_index).toObject());
    item->setData(Qt::DecorationRole, QPixmap(":/images/def_mus_60.png"));

    this->listWidget->addItem(item);

    int list_height = this->listWidget->count() * 70;
    this->listWidget->setFixedHeight(list_height);

    /*FileDownloader *fileDownloader = new FileDownloader(this);
    fileDownloader->setProperty("index", listWidget->count()-1);
    fileDownloader->setImageURL( QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_albumImg));
    connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));*/
}


/**
 * @brief MusicFolder_Colud::clearCloudList : 클라우드 목록관련 데이터 UI 초기화
 */
void MusicFolder_Colud::clearCloudList(){

    qDeleteAll(this->listCloud->begin(), this->listCloud->end());
    this->listCloud->clear();
    GSCommon::clearLayout(this->box_cloudList);
}


/**
 * @brief MusicFolder_Colud::requestRemoveCloud : 클라우드 삭제 요청
 * @details 비디오-폴더와 동일 API : IP:PORT/music_rclone_delete
 * @param p_index
 */
void MusicFolder_Colud::requestRemoveCloud(const int &p_index){

    QString url = QString("http://%1:").arg(global.device.getDeviceIP()) + QString("%1/music_rclone_delete").arg(global.port);

    QJsonObject tmp_json;
    tmp_json.insert("data", this->listCloud->at(p_index)->getData());

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_NETWORK_CLOUD_DELETE,
                     url,
                     tmp_json,
                     true);
}


/**
 * @brief MusicFolder_Colud::requestCloudlist:클라우드 리스트 요청
 * @details 비디오-폴더와 동일 API : IP:PORT/get_rclone_info
 * @param flagRefresh
 */
void MusicFolder_Colud::requestCloudlist(bool flagRefresh){

    if(this->listCloud->size() == 0 || flagRefresh){
        this->clearCloudList();

        this->jsonArr_folder_step = QJsonArray();
        this->jsonArr_tracks_toPlay = QJsonArray();

        this->cloudPath_buf = "";
        this->cloudName_buf = "";
        this->flagDirChange = false;

        QString url = QString("http://%1:").arg(global.device.getDeviceIP()) + QString("%1/get_rclone_info").arg(global.port);

        QJsonObject tmp_json;

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_NETWORK_CLOUD_LIST,
                         url,
                         tmp_json,
                         true);
    }
}


void MusicFolder_Colud::reqeustCloudlist_changePath(QString cloudName, QString cloudPath){

    if(cloudPath == this->cloudPath_buf){

        emit this->signal_clickedDirName(this->cloudName_buf, this->cloudPath_buf);
    }
    else{
        for(int i = 0; i < this->jsonArr_folder_step.count(); i++){
            QJsonObject stepObj = this->jsonArr_folder_step.at(i).toObject();

            if(stepObj.contains(cloudName)){
                QJsonObject sndObj = ProcJsonEasy::getJsonObject(stepObj, cloudName);

                this->cloudPath_buf = ProcJsonEasy::getString(sndObj, "path");
                this->cloudName_buf = ProcJsonEasy::getString(sndObj, "name");

                if(this->cloudPath_buf.isEmpty()){
                    emit this->signal_clickedHost(this->cloudPath_buf, this->cloudName_buf);
                }
                else if(!this->cloudPath_buf.isEmpty()){
                    emit this->signal_clickedDirName(this->cloudName_buf, this->cloudPath_buf);
                }
                this->flagDirChange = true;

                this->clearCloudList();
                this->folder_addCloud->hide();
                this->widget_cloud->hide();

                QString url = QString("http://%1:").arg(global.device.getDeviceIP()) + QString("%1/get_rclone_list").arg(global.port);

                QJsonObject tmpData;
                if(sndObj.contains("remote")){
                    tmpData = ProcJsonEasy::getJsonObject(sndObj, "remote");
                }
                else{
                    tmpData = sndObj;
                }

                QJsonObject tmp_json;
                tmp_json.insert("cloud", ProcJsonEasy::getBool(sndObj, "isInstall"));
                tmp_json.insert("path", ProcJsonEasy::getString(sndObj, "path"));
                tmp_json.insert("data", tmpData);
                tmp_json.insert("position", 0);
                tmp_json.insert("type", this->flagIsMusic ? 0 : 1);

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(HTTP_NETWORK_CLOUD_LIST,
                                 url,
                                 tmp_json,
                                 true);

                for(int j = (this->jsonArr_folder_step.count() - 1); j > i; j--){
                    this->jsonArr_folder_step.removeAt(j);
                }

                break;
            }
        }
    }
}


/**
 * @brief MusicFolder_Colud::setResultOfCloudList:클라우드 목록 요청 결과 처리
 * @param p_jsonData QJsonObject response
 */
void MusicFolder_Colud::setResultOfCloudList(const QJsonObject &p_jsonData){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_remoteItem = "remoteItem";
    const QString jsonKey_fileItem = "fileItem";

    if(p_jsonData.contains(jsonKey_flagOk) && ProcJsonEasy::get_flagOk(p_jsonData)){
        if(p_jsonData.contains(jsonKey_remoteItem)){
            this->folder_addCloud->show();
            this->jsonArr_folder_step = QJsonArray();
            QJsonArray jsonArr_remoteItem = ProcJsonEasy::getJsonArray(p_jsonData, jsonKey_remoteItem);

            QJsonObject tmpStep;
            for(int i = 0; i < jsonArr_remoteItem.size(); i++){
                QJsonObject tmp_remoteItem = jsonArr_remoteItem.at(i).toObject();

                if(p_jsonData.contains("isInstall")){
                    tmp_remoteItem.insert("isInstall", ProcJsonEasy::getBool(p_jsonData, "isInstall"));
                }
                else{
                    tmp_remoteItem.insert("isInstall", true);
                }

                if(!jsonArr_remoteItem.at(i).toObject().contains("path")){
                   tmp_remoteItem.insert("path", "");
                }

                tmpStep.insert(ProcJsonEasy::getString(tmp_remoteItem, "name"), tmp_remoteItem);

                DataCloud *data = new DataCloud;
                data->setData(tmp_remoteItem);
                this->listCloud->append(data);

                this->appendCloudUI(i);
            }
            this->jsonArr_folder_step.append(tmpStep);
        }
        else if(p_jsonData.contains(jsonKey_fileItem)){
            QJsonArray jsonArr_fileItem = ProcJsonEasy::getJsonArray(p_jsonData, jsonKey_fileItem);

            bool isDir = false;
            QJsonObject tmpStep;
            for(int i = 0; i < jsonArr_fileItem.size(); i++){
                QJsonObject tmp_fileItem = jsonArr_fileItem.at(i).toObject();

                isDir = ProcJsonEasy::getBool(tmp_fileItem, "isDir");

                if(p_jsonData.contains("isInstall")){
                    tmp_fileItem.insert("isInstall", ProcJsonEasy::getBool(p_jsonData, "isInstall"));
                }
                else{
                    tmp_fileItem.insert("isInstall", false);
                }

                tmpStep.insert(ProcJsonEasy::getString(tmp_fileItem, "name"), tmp_fileItem);

                if(isDir){
                    if(i == 0){
                        this->listWidget = new QListWidget();
                        this->listWidget->setItemDelegate(this->delegate_cloud);
                        this->listWidget->setCursor(Qt::PointingHandCursor);
                        this->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        this->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        this->listWidget->setStyleSheet("border:none;");
                        this->listWidget->clear();

                        this->box_cloudList->setContentsMargins(30, 0, 30, 0);
                        this->box_cloudList->addWidget(this->listWidget);
                    }

                    DataCloud *data = new DataCloud;
                    data->setData(tmp_fileItem);
                    listCloud->append(data);

                    this->jsonArr_folder_step.append(tmpStep);

                    this->appendCloudUIFolder(i);
                }
                else{
                    if(i == 0){
                        this->jsonArr_tracks_toPlay = QJsonArray();

                        this->listWidget = new QListWidget();
                        this->listWidget->setItemDelegate(this->delegate_cloud);
                        this->listWidget->setCursor(Qt::PointingHandCursor);
                        this->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        this->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        this->listWidget->setStyleSheet("border:none;");
                        this->listWidget->clear();

                        this->box_cloudList->setContentsMargins(30, 0, 30, 0);
                        this->box_cloudList->addWidget(this->listWidget);

                        if(this->flagIsMusic){
                            this->widget_cloud->show();
                        }
                    }

                    QJsonObject tmpRemote;
                    if(tmp_fileItem.contains("remote")){
                        tmpRemote = ProcJsonEasy::getJsonObject(tmp_fileItem, "remote");
                    }
                    else{
                        tmpRemote = tmp_fileItem;
                    }

                    QString tmpModTime = "";
                    if(tmp_fileItem.contains("humanReadableSize")){
                        tmpModTime = ProcJsonEasy::getString(tmp_fileItem, "humanReadableModTime") + " · ";
                    }
                    else{
                        tmpModTime = ProcJsonEasy::getString(tmp_fileItem, "humanReadableModTime");
                    }

                    QJsonObject json;
                    json.insert("index", i);
                    json.insert("filePath", ProcJsonEasy::getString(tmp_fileItem, "path"));
                    json.insert("fileName", ProcJsonEasy::getString(tmp_fileItem, "name"));
                    json.insert("title", ProcJsonEasy::getString(tmp_fileItem, "name"));
                    json.insert("rcloneServe", ProcJsonEasy::getString(tmpRemote, "name"));
                    json.insert("size", ProcJsonEasy::getString(tmp_fileItem, "humanReadableSize"));
                    json.insert("humanReadableModTime", tmpModTime);
                    json.insert("isCloud", true);
                    json.insert("isDir", isDir);
                    json.insert("isFile", true);

                    this->jsonArr_tracks_toPlay.append(json);

                    this->appendCloudUIFile(i);
                }

                /*if(isDir == true){
                    DataCloud *data = new DataCloud;
                    data->setData(tmp_fileItem);
                    listCloud->append(data);

                    this->appendCloudUIFolder(i);
                }
                else{
                    if(i == 0){
                        this->jsonArr_tracks_toPlay = QJsonArray();

                        this->listWidget = new QListWidget();
                        this->listWidget->setItemDelegate(this->delegate_cloud);
                        this->listWidget->setCursor(Qt::PointingHandCursor);
                        this->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        this->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        this->listWidget->setStyleSheet("border:none;");
                        this->listWidget->clear();

                        this->box_cloudList->setContentsMargins(30, 0, 30, 0);
                        this->box_cloudList->addWidget(this->listWidget);

                        if(this->flagIsMusic){
                            this->widget_cloud->show();
                        }
                    }

                    QJsonObject tmpRemote = ProcJsonEasy::getJsonObject(tmp_fileItem, "remote");

                    QString tmpModTime = "";
                    if(tmp_fileItem.contains("humanReadableSize")){
                        tmpModTime = ProcJsonEasy::getString(tmp_fileItem, "humanReadableModTime") + " · ";
                    }
                    else{
                        tmpModTime = ProcJsonEasy::getString(tmp_fileItem, "humanReadableModTime");
                    }

                    QJsonObject json;
                    json.insert("index", i);
                    json.insert("filePath", ProcJsonEasy::getString(tmp_fileItem, "path"));
                    json.insert("fileName", ProcJsonEasy::getString(tmp_fileItem, "name"));
                    json.insert("title", ProcJsonEasy::getString(tmp_fileItem, "name"));
                    json.insert("rcloneServe", ProcJsonEasy::getString(tmpRemote, "name"));
                    json.insert("size", ProcJsonEasy::getString(tmp_fileItem, "humanReadableSize"));
                    json.insert("humanReadableModTime", tmpModTime);
                    json.insert("isCloud", true);
                    json.insert("isDir", isDir);
                    json.insert("isFile", true);

                    this->jsonArr_tracks_toPlay.append(json);

                    this->appendCloudUIFile(i);
                }*/
            }

            /*if(isDir){
                this->jsonArr_folder_step.append(tmpStep);
            }*/
        }
    }
}


/**
 * @brief MediaFileTree::showOptionPopup 옵션팝업 띄우기
 */
void MusicFolder_Colud::showOptionPopup(int p_index){

    // OptMorePopup 띄우기 필요
    QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(p_index).toObject();

    OptMorePopup::HeaderData data_header;
    data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
    data_header.sub_title = "";
    data_header.imageUrl = "";
    data_header.data_pk = "";
    data_header.type = "CLOUD";
    data_header.flagProcStar = false;
    data_header.isShare = false;

    OptMorePopup *optPopup = new OptMorePopup(this);
    connect(optPopup, &OptMorePopup::signal_menuClicked, this, &MusicFolder_Colud::slot_optMorePopup_menuClicked);
    if(this->flagIsMusic){
        optPopup->setMenuMode(OptMorePopup::Rosehome_Track, data_header, p_index, SECTION_FOR_MORE_POPUP___MUSIC, false);
    }
    else{
        optPopup->setMenuMode(OptMorePopup::Video_Track, data_header, p_index, SECTION_FOR_MORE_POPUP___VIDEO, false);
    }
    optPopup->showPopup();
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
        if(p_jsonObject.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObject)){
            this->clearCloudList();
            this->folder_addCloud->hide();
            this->widget_cloud->hide();

            if(this->jsonArr_folder_step.count() > 0){
                if(this->cloudPath_buf.isEmpty() && this->flagDirChange == false){
                    emit this->signal_clickedHost(this->cloudPath_buf, this->cloudName_buf);
                }
                else if(!this->cloudPath_buf.isEmpty() && this->flagDirChange == false){
                    emit this->signal_clickedDirName(this->cloudName_buf, this->cloudPath_buf);
                }
            }
            this->flagDirChange = false;

            this->setResultOfCloudList(p_jsonObject);
        }
    }
    else if(p_id == HTTP_NETWORK_CLOUD_DELETE){
        this->clearCloudList();
        this->folder_addCloud->hide();
        this->widget_cloud->hide();

        this->setResultOfCloudList(p_jsonObject);
    }
    else if(p_id == HTTP_NETWORK_PLAY){

    }

    sender()->deleteLater();
}


/**
 * @brief MusicFolder_Colud::slot_clickedRow : [슬롯] Row 클릭
 */
void MusicFolder_Colud::slot_clickedRow(){

    QString tmp_type = sender()->property("type").toString();
    int tmp_idx = sender()->property("index").toInt();

    if(tmp_type == ROW_TYPE_ADD){
        showDlgOfSelectCloudType();
    }
    else if(tmp_type == ROW_TYPE_LIST){

        this->cloudPath_buf = this->listCloud->at(tmp_idx)->getPath();
        this->cloudName_buf = this->listCloud->at(tmp_idx)->getName();

        QString url = QString("http://%1:").arg(global.device.getDeviceIP()) + QString("%1/get_rclone_list").arg(global.port);

        QJsonObject tmp_json;
        tmp_json.insert("cloud", this->listCloud->at(tmp_idx)->getisisInstall());
        tmp_json.insert("path", this->listCloud->at(tmp_idx)->getPath());
        tmp_json.insert("data", this->listCloud->at(tmp_idx)->getData());
        tmp_json.insert("position", 0);
        tmp_json.insert("type", this->flagIsMusic ? 0 : 1);

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_NETWORK_CLOUD_LIST,
                         url,
                         tmp_json,
                         true);
    }
}


/**
 * @brief MusicFolder_Colud::slot_clickedRemove : [슬롯] 삭제 클릭
 */
void MusicFolder_Colud::slot_clickedRemove(){

    int tmp_index = sender()->property("index").toInt();

    if(tmp_index >= 0 && tmp_index < this->listCloud->size()){

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


/**
 * @brief MusicFolder_Colud::slot_delegateClicked : [슬롯] delegateClick
 * @details delegate item click 처리<br>
 * @param p_index
 * @param p_btnType
 */
void MusicFolder_Colud::slot_delegateClicked(const bool &isFile, const int &p_index, const int &p_btnType){

    if(p_index >= 0 && p_index < this->listWidget->count()){

        if(isFile){
            //if(p_btnType == FileDelegate::BtnType::etc){
            if(p_btnType == cloudListDelegate::BtnType::etc){

                OptMorePopup::ClickMode clickMode = OptMorePopup::ClickMode::Play_RightNow_withReorder;
                int playType = global.device.getMusicPlayType();

                if(playType == 15){
                    clickMode = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                }
                else if(playType == 13){
                    clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                }
                else if(playType == 12){
                    clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                }
                else if(playType == 16){
                    clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                }
                else if(playType == 17){
                    clickMode = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                }
                else if(playType == 18){
                    clickMode = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                }
                else if(playType == 19){
                    clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                }

                // 음원 재생
                this->playMusic(this->getPlayMusicInfoArray(p_index, clickMode), playType);
            }
            //else if(p_btnType == FileDelegate::BtnType::more){
            else if(p_btnType == cloudListDelegate::BtnType::more){
                // 옵션팝업 show
                this->showOptionPopup(p_index);
            }
        }
        else{
            this->cloudPath_buf = this->listCloud->at(p_index)->getPath();
            this->cloudName_buf = this->listCloud->at(p_index)->getName();

            QString url = QString("http://%1:").arg(global.device.getDeviceIP()) + QString("%1/get_rclone_list").arg(global.port);

            QJsonObject tmp_json;
            tmp_json.insert("cloud", this->listCloud->at(p_index)->getisisInstall());
            tmp_json.insert("path", this->listCloud->at(p_index)->getPath());
            tmp_json.insert("data", this->listCloud->at(p_index)->getData());
            tmp_json.insert("position", 0);
            tmp_json.insert("type", this->flagIsMusic ? 0 : 1);

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_NETWORK_CLOUD_LIST,
                             url,
                             tmp_json,
                             true);
        }
    }
}


/**
 * @brief MusicFolder_Colud::slot_clickedPlayAll : [슬롯] 전체 재생
 */
void MusicFolder_Colud::slot_clickedPlayAll(){

    this->playMusic(this->getPlayMusicInfoArray(-1), 15);
}


/**
 * @brief MusicFolder_Colud::slot_clickedPlayShuffle : [슬롯]
 */
void MusicFolder_Colud::slot_clickedPlayShuffle(){

    this->playMusic(this->getPlayMusicInfoArray(-1), 15, true);
}


void MusicFolder_Colud::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

    if(section == SECTION_FOR_MORE_POPUP___MUSIC || section == SECTION_FOR_MORE_POPUP___VIDEO){
        if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
            || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
            || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
            || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last
        )
        {
            int playType = 0;

            if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne){
                playType = 13;
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne){
                playType = 12;
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne){
                playType = 15;
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne){
                playType = 16;
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere){
                playType = 17;
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty){
                playType = 18;
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last){
                playType = 19;
            }

            this->playMusic(this->getPlayMusicInfoArray(index, clickMode), playType);
        }
    }
}


/**
 * @brief MusicFolder_Colud::getPlayMusicInfo : 재생음원 정보 JsonArray 형태 반환
 * @param p_index if p_index < 0, 전체 재생
 * @return QJsonArray
 */
QJsonArray MusicFolder_Colud::getPlayMusicInfoArray(const int &p_index, const OptMorePopup::ClickMode clickMode){

    QJsonArray jsonArrPlay;
    int maxCnt = 0;
    if(p_index < 0){
        if(this->jsonArr_tracks_toPlay.count() > 3000){
            maxCnt = 3000;
        }
        else{
            maxCnt = this->jsonArr_tracks_toPlay.count();
        }

        for(int i = 0; i < maxCnt; i++){

            QJsonObject tmpData = this->jsonArr_tracks_toPlay.at(i).toObject();

            QString path = "http://127.0.0.1:9067/" + ProcJsonEasy::getString(tmpData, "filePath");

            QJsonObject json;
            json.insert("albumId", "");
            json.insert("albumKey", "");
            json.insert("artist", "");
            json.insert("duration", "");
            json.insert("ext", "");
            json.insert("id", "");
            json.insert("ip", "");
            json.insert("isCloud", true);
            json.insert("isDir", ProcJsonEasy::getBool(tmpData, "isDir"));
            json.insert("isFile", true);
            json.insert("isNetworkAdd", false);
            json.insert("isServer", true);
            json.insert("isShare", false);
            json.insert("isThumbNailGetting", false);
            json.insert("isUp", false);
            json.insert("name", "");
            json.insert("path", path);
            json.insert("pw", "");
            json.insert("rcloneServe", ProcJsonEasy::getString(tmpData, "rcloneServe"));
            json.insert("smbParamPath", "");
            json.insert("thumbnail", "");
            json.insert("title", "");

            jsonArrPlay.append(json);
        }
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
        || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
        || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
        || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
    )
    {
        QJsonObject tmpData = this->jsonArr_tracks_toPlay.at(p_index).toObject();

        QString path = "http://127.0.0.1:9067/" + ProcJsonEasy::getString(tmpData, "filePath");

        QJsonObject json;
        json.insert("albumId", "");
        json.insert("albumKey", "");
        json.insert("artist", "");
        json.insert("duration", "");
        json.insert("ext", "");
        json.insert("id", "");
        json.insert("ip", "");
        json.insert("isCloud", true);
        json.insert("isDir", ProcJsonEasy::getBool(tmpData, "isDir"));
        json.insert("isFile", true);
        json.insert("isNetworkAdd", false);
        json.insert("isServer", true);
        json.insert("isShare", false);
        json.insert("isThumbNailGetting", false);
        json.insert("isUp", false);
        json.insert("name", "");
        json.insert("path", path);
        json.insert("pw", "");
        json.insert("rcloneServe", ProcJsonEasy::getString(tmpData, "rcloneServe"));
        json.insert("smbParamPath", "");
        json.insert("thumbnail", "");
        json.insert("title", "");

        jsonArrPlay.append(json);
    }
    else{
        int startIDX = p_index;
        if(this->jsonArr_tracks_toPlay.count() > (3000 + startIDX)){
            maxCnt = 3000;
        }
        else{
            maxCnt = this->jsonArr_tracks_toPlay.count() - startIDX;
        }

        for(int i = startIDX; i < maxCnt; i++){

            QJsonObject tmpData = this->jsonArr_tracks_toPlay.at(i).toObject();

            QString path = "http://127.0.0.1:9067/" + ProcJsonEasy::getString(tmpData, "filePath");

            QJsonObject json;
            json.insert("albumId", "");
            json.insert("albumKey", "");
            json.insert("artist", "");
            json.insert("duration", "");
            json.insert("ext", "");
            json.insert("id", "");
            json.insert("ip", "");
            json.insert("isCloud", true);
            json.insert("isDir", ProcJsonEasy::getBool(tmpData, "isDir"));
            json.insert("isFile", true);
            json.insert("isNetworkAdd", false);
            json.insert("isServer", true);
            json.insert("isShare", false);
            json.insert("isThumbNailGetting", false);
            json.insert("isUp", false);
            json.insert("name", "");
            json.insert("path", path);
            json.insert("pw", "");
            json.insert("rcloneServe", ProcJsonEasy::getString(tmpData, "rcloneServe"));
            json.insert("smbParamPath", "");
            json.insert("thumbnail", "");
            json.insert("title", "");

            jsonArrPlay.append(json);
        }
    }

    return jsonArrPlay;
}


/**
 * @brief MusicFolder_Colud::playMusic : 음원 재생 if(jsonObj.contains(key)) {
       return jsonObj[key].toDouble(defValue);
   }
 * @param jsonArrMusic QJsonArray 재생음원 정보
 */
void MusicFolder_Colud::playMusic(QJsonArray jsonArrMusic, int playType, bool flagPlayShuffle){

    QJsonObject tmp_json;
    QString url = "";

    if(this->flagIsMusic){
        global.device.CurrExtType = "NONE";
        ToastMsg::show(this, "", tr("player has started."));

        tmp_json.insert("musicPlayType", playType);
        tmp_json.insert("music", jsonArrMusic);
        tmp_json.insert("playType", this->flagIsMusic ? 20 : 21);
        if(flagPlayShuffle){
            tmp_json.insert("shuffle", flagPlayShuffle ? 1 : 0);
        }
        tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

        if(global.Queue_track_count != 0) {
            print_debug();
            emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += jsonArrMusic.count();
        //QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"MediaFileTree::playMusic---" << strJson;

        url = QString("http://%1:").arg(global.device.getDeviceIP()) + QString("%1/music_network_play").arg(global.port);

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
        network->request(HTTP_NETWORK_PLAY,
                         url,
                         tmp_json,
                         true,
                         false);
    }
    else{
        int playTypeSND = 0;
        if(playType == 17){
            playTypeSND = 15;
        }
        else if(playType == 18){
            playTypeSND = 12;
        }
        else if(playType == 19){
            playTypeSND = 13;
        }
        else{
            playTypeSND = playType;
        }

        tmp_json.insert("musicPlayType", playTypeSND);
        tmp_json.insert("music", jsonArrMusic);
        tmp_json.insert("playType", this->flagIsMusic ? 20 : 21);
        //tmp_json.insert("currentPosition", 0);
        //tmp_json.insert("musicType", 11);
        tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

        url = QString("http://%1:").arg(global.device.getDeviceIP()) + QString("%1/music_network_play").arg(global.port);

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
        network->request(HTTP_NETWORK_PLAY,
                         url,
                         tmp_json,
                         true,
                         false);
    }
}
