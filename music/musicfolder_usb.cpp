#include "musicfolder_usb.h"

#include "common/global.h"
#include "common/networkhttp.h"
#include "widget/toastmsg.h"
#include <common/gscommon.h>
#include "widget/dialogconfirm.h"
#include <QDebug>
#include <QFileSystemModel>
#include <QScrollArea>
#include <login/dialogladd_network.h>
#include <login/dialogladd_network_local.h>
#include <QFileInfo>
#include <QFileDialog>//cheon210617-file
#include <QMediaPlayer>//cheon210617-file
#include <QCoreApplication>//c220913_2
#include "login/dialoglogin_mac.h"//c220913_2



const int HTTP_NETWORK_FOLDER_LIST = 99;
const int HTTP_NETWORK_FOLDER_DELETE = 98;

const int HTTP_LOCAL_NETWORK_FOLDER_LIST = 97;
const int HTTP_LOCAL_NETWORK_FOLDER_DELETE = 96;

const QString TYPE_FOLDER_LOCAL = "local";
const QString TYPE_FOLDER_USB = "folder_bic";
const QString TYPE_FOLDER_NETWORK = "network";
const QString TYPE_FOLDER_CLOUD = "net_cloud";
const QString TYPE_FOLDER_ADD_NETWORK_FOLDER = "net_add_fol";
const QString TYPE_FOLDER_NETWORK_FOLDER = "added_network";
const QString TYPE_FOLDER_LOCAL_NETWORK_FOLDER = "added_network";

const int HTTP_REMOTE_VOLUME_GET = 12;
const int HTTP_REMOTE_VOLUME_ADD = 13;
const int HTTP_REMOTE_VOLUME_SUB = 14;

/**
 * @brief MusicFolder_USB::MusicFolder_USB 음악 > 폴더 > USB 관련 위젯
 * @param parent
 */
MusicFolder_USB::MusicFolder_USB(QWidget *parent, bool p_flagIsMusic)
    : QWidget(parent)
    , flagIsMusic(p_flagIsMusic)
{
    setInit();
    setUIControl();

    if(global.device.getDeviceIP().isEmpty()==false){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_volume(int, QJsonObject)));
        QJsonObject json;
        json.insert("volumeType","volume_get");
        network->request(HTTP_REMOTE_VOLUME_GET, QString("http://%1:%2/volume").arg(global.device.getDeviceIP()).arg(global.port), json, true);
    }
}

/**
 * @brief MusicFolder_USB::~MusicFolder : 소멸자
 */
MusicFolder_USB::~MusicFolder_USB(){

    if(listNetworkFolder->count() > 0){
        qDeleteAll(listNetworkFolder->begin(), listNetworkFolder->end());
        listNetworkFolder->clear();
    }

    if(listLocalNetworkFolder->count() > 0){
        //qDeleteAll(listLocalNetworkFolder->begin(), listLocalNetworkFolder->end());
        listLocalNetworkFolder->clear();
    }

    if(listUsb->count() > 0){
        qDeleteAll(listUsb->begin(), listUsb->end());
        listUsb->clear();
    }
}

/**
 * @brief MusicFolder_USB::setInit::초기세팅
 */
void MusicFolder_USB::setInit(){
    linker = Linker::getInstance();
    listNetworkFolder = new QList<DataNetworkFolder*>();
    listLocalNetworkFolder = new QList<DataNetworkFolder*>();
    listUsb = new QList<DataNetworkFolder*>();



    connect(linker, SIGNAL(signal_networkFolderAdded()), this, SLOT(slot_requestNetworkFolderList()));
}

/**
 * @brief MusicFolder_USB::setUIControl : UI 세팅
 */
void MusicFolder_USB::setUIControl(){

    this->topDirPath = new Folder_TopDirPath();

    // 목록
    Folder_ClickableRow *clickableRow_local = new Folder_ClickableRow(":/images/icon_local.png", tr("Saved sound source"));//c220609
    //Folder_ClickableRow *clickableRow_local = new Folder_ClickableRow(":/images/icon_local.png", tr("Music stored on the desktop"));//c220609
    //Folder_ClickableRow *clickableRow_local = new Folder_ClickableRow(":/images/icon_local.png", tr("저장 된 음원"));
    Folder_ClickableRow *clickableRow_folder_bic = new Folder_ClickableRow(":/images/icon_folder_bic.png", tr("SanDisk USB drive"));//cheon210831-network
    //Folder_ClickableRow *clickableRow_folder_bic = new Folder_ClickableRow(":/images/icon_folder_bic.png", tr("SanDisk USB 드라이브"));
    Folder_ClickableRow *clickableRow_network = new Folder_ClickableRow(":/images/icon_network.png", tr("network"));//cheon210831-network
    //Folder_ClickableRow *clickableRow_network = new Folder_ClickableRow(":/images/icon_network.png", tr("네트워크"));
    Folder_ClickableRow *clickableRow_net_cloud = new Folder_ClickableRow(":/images/icon_net_cloud.png", tr("cloud"));
    //Folder_ClickableRow *clickableRow_net_cloud = new Folder_ClickableRow(":/images/icon_net_cloud.png", tr("클라우드"));
    Folder_ClickableRow *clickableRow_net_add_fol = new Folder_ClickableRow(":/images/icon_net_add_fol.png", tr("Add network folder"));
    //Folder_ClickableRow *clickableRow_net_add_fol = new Folder_ClickableRow(":/images/icon_net_add_fol.png", tr("네트워크 폴더 추가"));

    clickableRow_local->setProperty("type", TYPE_FOLDER_LOCAL);//c220505
    clickableRow_folder_bic->setProperty("type", TYPE_FOLDER_USB);
    clickableRow_network->setProperty("type", TYPE_FOLDER_NETWORK);
    clickableRow_net_cloud->setProperty("type", TYPE_FOLDER_CLOUD);
    clickableRow_net_add_fol->setProperty("type", TYPE_FOLDER_ADD_NETWORK_FOLDER);

    vl_networkList = new QVBoxLayout;
    vl_networkList->setContentsMargins(0,0,0,0);
    vl_networkList->setSpacing(0);
    vl_networkList->setAlignment(Qt::AlignTop);

    vl_usb = new QVBoxLayout();
    vl_usb->setContentsMargins(0,0,0,0);
    vl_usb->setSpacing(0);
    vl_usb->setAlignment(Qt::AlignTop);

    vl_localnetworkList = new QVBoxLayout();//cheon210617-file
    vl_localnetworkList->setContentsMargins(0,0,0,0);//cheon210617-file
    vl_localnetworkList->setSpacing(0);//cheon210617-file
    vl_localnetworkList->setAlignment(Qt::AlignTop);//cheon210617-file

    this->vl_list = new QVBoxLayout();
    this->vl_list->setContentsMargins(0,0,0,0);
    this->vl_list->setSpacing(0);
    this->vl_list->setAlignment(Qt::AlignTop);

#if defined(Q_OS_WIN)//cheon210831-network
    //this->vl_list->addWidget(clickableRow_local);//c220609
    //this->vl_list->addLayout(vl_localnetworkList);
    //this->vl_list->addWidget(clickableRow_folder_bic);//c220505
    this->vl_list->addLayout(vl_usb);
    this->vl_list->addWidget(clickableRow_network);
    this->vl_list->addWidget(clickableRow_net_cloud);
    this->vl_list->addWidget(clickableRow_net_add_fol);
    this->vl_list->addLayout(vl_networkList);
#elif defined(Q_OS_MAC)//cheon210831-network
    //this->vl_list->addWidget(clickableRow_local);//c220609
    //this->vl_list->addLayout(vl_localnetworkList);
    //this->vl_list->addWidget(clickableRow_folder_bic);
    this->vl_list->addLayout(vl_usb);
    this->vl_list->addWidget(clickableRow_network);//c220908_2
    //this->vl_list->addWidget(clickableRow_net_cloud);//c220908_2
    this->vl_list->addWidget(clickableRow_net_add_fol);
    this->vl_list->addLayout(vl_networkList);
    //ToastMsg::show(this, "", tr("MusicFolder_USB::setUIControl- The current version is not supported by MAC Desktop.\n It will be supported in a future version.."));//cheon210831-network
#endif

    // Page 생성
    QWidget *wg_init = new QWidget();
    wg_init->setLayout(vl_list);


    // 스크롤
    page_init = new QScrollArea();
    page_init->setObjectName("scrollArea");
    page_init->setWidget(wg_init);
    page_init->setWidgetResizable(true);
    page_init->setStyleSheet("#scrollArea { border:0px; }");
    page_init->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    this->page_fileTree = new MediaFileTree(this, flagIsMusic);
    this->page_network = new MusicFolder_Network(this);
    this->page_cloud = new MusicFolder_Colud(this);
    this->page_dirFileList = new DirFileListPage(DirFileListPage::ValiedFileType::Audio);//c220609

    // 스택 위젯
    this->stackedWidget_page = new QStackedWidget();
    this->stackedWidget_page->addWidget(page_init);    // 탐색 첫 화면
    this->stackedWidget_page->addWidget(page_fileTree);
    this->stackedWidget_page->addWidget(page_network);                 // 네트워크 클릭시 화면
    this->stackedWidget_page->addWidget(page_cloud);
    this->stackedWidget_page->addWidget(page_dirFileList);
    //this->stackedWidget->addWidget();                 // 클라우드 클릭시 화면

    this->vl_total= new QVBoxLayout();
    this->vl_total->setContentsMargins(0,0,0,0);
    this->vl_total->setSpacing(0);
    this->vl_total->setAlignment(Qt::AlignTop);
    this->vl_total->addWidget(this->topDirPath);
    this->vl_total->addWidget(this->stackedWidget_page);


    this->setLayout(vl_total);

    // 커넥션
    connect(linker, SIGNAL(signal_searchFolder_clicked()), this, SLOT(slot_searchNetworkFolder()));//c220906_1
    connect(page_fileTree, SIGNAL(signal_clickedDirName(QString, QString)), this, SLOT(slot_changeTopDirName(QString, QString)));
#if defined(Q_OS_WIN)//cheon210831-network
    //connect(clickableRow_local, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));//c220609
    //connect(clickableRow_folder_bic, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));
    connect(clickableRow_network, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));
    connect(clickableRow_net_cloud, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));
    connect(clickableRow_net_add_fol, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));
#elif defined(Q_OS_MAC)//cheon210831-network
    //connect(clickableRow_local, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));//c220609
    //connect(clickableRow_folder_bic, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));
    connect(clickableRow_network, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));//c220908_2
    //connect(clickableRow_net_cloud, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));//c220908_2
    connect(clickableRow_net_add_fol, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));
#endif//cheon210831-network
    connect(topDirPath, SIGNAL(signal_changedDirPath(QString, QString)), this, SLOT(slot_changedRootPath(QString, QString)));
    connect(page_network, SIGNAL(signal_clickedHost(QString,QString)), this, SLOT(slot_clickedNetworkHost(QString, QString)));

    connect(this->page_dirFileList, SIGNAL(signal_fileInfo_filePath_play(QString)), this, SLOT(slot_fileInfo_filePath_play(QString)));//c220609
    connect(linker, SIGNAL(signal_fileInfo_filePath_stop()), this, SLOT(slot_fileInfo_filePath_stop()));//c220609
    connect(this->page_dirFileList, SIGNAL(signal_clickedDirName(QString, QString)), this, SLOT(slot_changeTopDirName(QString, QString)));//c220609

    connect(linker, SIGNAL(signal_setMountShareFolder(QString, QString, QString, QString)), this, SLOT(slot_process_smb_ok(QString,QString,QString,QString)));//c220913_1

    this->timer = new QTimer(this);
    this->timer->setInterval(1000); // 1초
    //connect(this->timer, SIGNAL(timeout()), SLOT(slot_timePlayValue()));
   // QThread::usleep(2000);
    this->timer->start();



}

void MusicFolder_USB::slot_process_smb_ok(QString ip, QString id, QString pw, QString folderName){//c220913_1
    print_debug();
    qDebug() << "folderName=" << folderName;
#if defined(Q_OS_MAC)
    //emit linker->signal_goFolder("folder");//c220913_1
    print_debug();
    global.shareFolderInfoSel.clear();
    global.shareFolderInfoSel.append(ip);
    global.shareFolderInfoSel.append(id);
    global.shareFolderInfoSel.append(pw);
    global.shareFolderInfoSel.append(folderName);

    // show 네트워크 폴더 Add Dialog
    Dialog::DialogAdd_Network *dlg_network = new Dialog::DialogAdd_Network(this);
    int result = dlg_network->exec();

    if(result == QDialog::Accepted){
        // nothing
    }

    //global.shareFolderInfoSel.clear();
    delete dlg_network;
#endif

    /*
#if defined(Q_OS_MAC)
    // 맥 관련 SMB 기술
print_debug();
    QProcess* process = new QProcess();
    QStringList sArgs;
    process->start(QString("sh -c \"sudo rmdir /Volumes/%1\"").arg(folderName));//c220913_1
    process->waitForFinished(-1);
    process->start(QString("sh -c \"sudo mkdir /Volumes/%1\"").arg(folderName));//c220913_1
    process->waitForFinished(-1);
    process->start(QString("sh -c \"sudo mount_smbfs //%1:%2@%3/%4 /Volumes/%4\"").arg(id).arg(pw).arg(ip).arg(folderName));//c220913_1
    process->waitForFinished(-1);
    QString standardOutput = process->readAllStandardOutput();
    qDebug() << standardOutput;
    qDebug() << process->readAllStandardError();

    requestAddNetworkFolder(ip, folderName.replace("\\","/"));//cheon210831-network
#endif
*/
}

void MusicFolder_USB::slot_fileInfo_filePath_play(QString index ){//c220609

    print_debug();
    qDebug()<<"index = "<< index;
    qDebug()<<"global.setTextVolumn = "<< global.setTextVolumn;

    //global.musicPlaylistMediaContent
    global.desktopPlayerMode = true;
    global.music_playlist->setCurrentIndex(index.toInt());
    //global.music_player->setMedia(QUrl::fromLocalFile(p_fileInfo_filePath));

    global.music_player->setVolume(global.setTextVolumn);

    //global.music_player->play();//아래의 커넥트 선언으로 한곡 반복 플레이

    emit linker->siganl_desktopModeStart();  //DESKTOP_MODE   //c220705


    global.music_player->play();//아래의 커넥트 선언으로 한곡 반복 플레이
}


/**
 * @brief VolumnControl::responseHttp : 로즈 장치 정보요청 결과 처리
 * @param p_jsonObject QJsonObject&
 */
void MusicFolder_USB::slot_responseHttp_volume(const int &p_id, const QJsonObject &p_jsonObject){

    Q_UNUSED(p_jsonObject);

    switch(p_id){
    case HTTP_REMOTE_VOLUME_ADD :
        //ToastMsg::show(this, "", tr("You have selected the volume."));
        //ToastMsg::show(this, "", tr("음량을 선택하였습니다."));
        break;
    case HTTP_REMOTE_VOLUME_SUB :
        break;
    case HTTP_REMOTE_VOLUME_GET :
        if(p_jsonObject.contains("volumeValue")){
            global.setTextVolumn = p_jsonObject["volumeValue"].toInt();
        }
        break;
    }
   sender()->deleteLater();
}

/**
 * @brief SectionBottom::slot_timePlayValue
 */
void MusicFolder_USB::slot_timePlayValue(){//c220525


}

/**
 * @brief MusicFolder_USB::appendNetworkFolderUI::네트워크 폴더 UI 추가
 * @param p_index int 네트워크 폴더 index
 */
//Media1, Video 등 smb 연결
void MusicFolder_USB::appendNetworkFolderUI(const int &p_index){//cheon210831-network
    if(p_index < listNetworkFolder->count()){
        QString tmp_ip = listNetworkFolder->at(p_index)->getIp();
        QString tmp_path = listNetworkFolder->at(p_index)->getPath();
        QString tmp_folderName = QString("smb://%1/%2").arg(tmp_ip).arg(tmp_path=="/" ? "" :tmp_path);
        QString tmp_imgPath = ":images/icon_net_fol_off.png";


        FolderNetwork_ClickableRow *clickableRow = new FolderNetwork_ClickableRow(tmp_imgPath, tmp_folderName);
        clickableRow->setProperty("type", TYPE_FOLDER_NETWORK_FOLDER);
        clickableRow->setProperty("index", p_index);
        clickableRow->setProperty("flagOn", true);
        clickableRow->setProperty("flagAccess", false);
        clickableRow->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), "netfolder_"+QString::number(p_index));
        this->vl_networkList->addWidget(clickableRow);

        connect(clickableRow, SIGNAL(signal_clicked()), this, SLOT(slot_clickedNetworkFolder()));
        connect(clickableRow, SIGNAL(signal_clickedRemove()), this, SLOT(slot_clickedRemoveNetwork()));


        // TCP 연결로 네트워크 IP 연결 가능 확인
        QTcpSocket *socket = new QTcpSocket(this);
        socket->setProperty("index", p_index);
        socket->connectToHost(tmp_ip,445, QTcpSocket::ReadOnly);

        QTimer timer;
        timer.setProperty("index", p_index);
        timer.setProperty("isTimer", true);
        timer.setSingleShot(true);

        QEventLoop loop;
        connect(socket, SIGNAL(connected()), SLOT(slot_networkIPConnected()));
        connect(socket, SIGNAL(connected()), &loop, SLOT(quit()));
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(slot_errorSocket()));
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), &loop, SLOT(quit()));
        connect(&timer, SIGNAL(timeout()), SLOT(slot_errorSocket()));
        connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        connect(&timer, SIGNAL(timeout()), socket, SLOT(deleteLater()));
        timer.start(1000);
        loop.exec();

        if(!timer.isActive()){
            disconnect(socket, SIGNAL(connected()), this, SLOT(slot_networkIPConnected()));
            disconnect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slot_errorSocket()));
        }
    }
}

void MusicFolder_USB::appendLocalNetworkFolderUI(const int &p_index){
    if(p_index < listLocalNetworkFolder->count()){
        QString tmp_ip = listLocalNetworkFolder->at(p_index)->getIp();
        QString tmp_path = listLocalNetworkFolder->at(p_index)->getPath();
        QString tmp_folderName = QString("smb://%1/%2").arg(tmp_ip).arg(tmp_path=="/" ? "" :tmp_path);
        QString tmp_imgPath = ":images/icon_net_fol_off.png";
print_debug();

        FolderNetwork_ClickableRow *clickableRow = new FolderNetwork_ClickableRow(tmp_imgPath, tmp_folderName);
        clickableRow->setProperty("type", TYPE_FOLDER_LOCAL_NETWORK_FOLDER);
        clickableRow->setProperty("index", p_index);
        clickableRow->setProperty("flagOn", true);
        clickableRow->setProperty("flagAccess", false);
        clickableRow->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), "netfolder_"+QString::number(p_index));
        this->vl_localnetworkList->addWidget(clickableRow);

        connect(clickableRow, SIGNAL(signal_clicked()), this, SLOT(slot_clickedLocalNetworkFolder()));
        connect(clickableRow, SIGNAL(signal_clickedRemove()), this, SLOT(slot_clickedRemoveLocalNetwork()));


        // TCP 연결로 네트워크 IP 연결 가능 확인
        QTcpSocket *socket = new QTcpSocket(this);
        socket->setProperty("index", p_index);
        socket->connectToHost(tmp_ip,445, QTcpSocket::ReadOnly);

        QTimer timer;
        timer.setProperty("index", p_index);
        timer.setProperty("isTimer", true);
        timer.setSingleShot(true);

        QEventLoop loop;
        connect(socket, SIGNAL(connected()), SLOT(slot_localnetworkIPConnected()));
        connect(socket, SIGNAL(connected()), &loop, SLOT(quit()));
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(slot_localerrorSocket()));
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), &loop, SLOT(quit()));
        connect(&timer, SIGNAL(timeout()), SLOT(slot_localerrorSocket()));
        connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        connect(&timer, SIGNAL(timeout()), socket, SLOT(deleteLater()));
        timer.start(1000);
        loop.exec();

        if(!timer.isActive()){
            disconnect(socket, SIGNAL(connected()), this, SLOT(slot_localnetworkIPConnected()));
            disconnect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slot_localerrorSocket()));
        }
    }
}



void MusicFolder_USB::slot_networkIPConnected(){

    int tmp_index = sender()->property("index").toInt();

    if(tmp_index < listNetworkFolder->count()){
        QString tmp_ip = listNetworkFolder->at(tmp_index)->getIp();
        QString tmp_path = listNetworkFolder->at(tmp_index)->getPath();
    print_debug();
        #if defined(Q_OS_WINDOWS)
            openNetworkDriveForWindow(tmp_index);
        #elif defined(Q_OS_MAC)
        #endif
qDebug() << "tmp_path=" << tmp_path;
        if(checkAccessAbleFile(tmp_ip, tmp_path)){
            FolderNetwork_ClickableRow *folderRow = qobject_cast<FolderNetwork_ClickableRow*>(vl_networkList->itemAt(tmp_index)->widget());
            folderRow->setImage(":images/icon_net_fol_on.png");
            folderRow->setProperty("flagOn", true);
            folderRow->setProperty("flagAccess", true);
        }else{
            QTimer::singleShot(500,this,[=](){reCheckNetworkDriveOpened(tmp_index, tmp_ip, tmp_path);});
        }
    }

    sender()->deleteLater();

}


void MusicFolder_USB::slot_localnetworkIPConnected(){

    int tmp_index = sender()->property("index").toInt();

    if(tmp_index < listLocalNetworkFolder->count()){
        QString tmp_ip = listLocalNetworkFolder->at(tmp_index)->getIp();
        QString tmp_path = listLocalNetworkFolder->at(tmp_index)->getPath();

        print_debug();
        #if defined(Q_OS_WINDOWS)
            openLocalNetworkDriveForWindow(tmp_index);
        #elif defined(Q_OS_MAC)
        #endif

        if(checkAccessAbleFile(tmp_ip, tmp_path)){
            FolderNetwork_ClickableRow *folderRow = qobject_cast<FolderNetwork_ClickableRow*>(vl_localnetworkList->itemAt(tmp_index)->widget());
            folderRow->setImage(":images/icon_net_fol_on.png");
            folderRow->setProperty("flagOn", true);
            folderRow->setProperty("flagAccess", true);
        }else{
            QTimer::singleShot(500,this,[=](){reCheckNetworkDriveOpened(tmp_index, tmp_ip, tmp_path);});
        }
    }

    sender()->deleteLater();

}

void MusicFolder_USB::slot_errorSocket(){

    int tmp_index = sender()->property("index").toInt();

    FolderNetwork_ClickableRow *folderRow = qobject_cast<FolderNetwork_ClickableRow*>(vl_networkList->itemAt(tmp_index)->widget());
    folderRow->setImage(":images/icon_net_fol_off.png");
    folderRow->setProperty("flagOn", false);
    folderRow->setProperty("flagAccess", false);

}
void MusicFolder_USB::slot_localerrorSocket(){

    int tmp_index = sender()->property("index").toInt();

    FolderNetwork_ClickableRow *folderRow = qobject_cast<FolderNetwork_ClickableRow*>(vl_localnetworkList->itemAt(tmp_index)->widget());
    folderRow->setImage(":images/icon_net_fol_off.png");
    folderRow->setProperty("flagOn", false);
    folderRow->setProperty("flagAccess", false);

}

void MusicFolder_USB::setData(){//c220616

    this->goInitPage();

    slot_requestNetworkFolderList();//하단의 Media1, Video 등 리스트 불러와 화면 출력후 smb 연결후 작업 수행
}

/**
 * @brief MusicFolder_USB::checkAccessAbleFile : 파일 접속 가능 여부 확인
 * @param p_ip QString smb ip
 * @param p_path QString path
 * @return bool if true, 접속 가능
 */
bool MusicFolder_USB::checkAccessAbleFile(const QString &p_ip, const QString &p_path){//c220913_1

    bool tmp_flagAccessAlbe = true;
print_debug();
    #if defined(Q_OS_WIN)
        QFileInfo check_file("//"+p_ip+"/"+p_path);
        if(!check_file.exists()){

            tmp_flagAccessAlbe = false;
            qDebug() << "checkAccessAbleFile-tmp_flagAccessAlbe" << tmp_flagAccessAlbe;
        }else
            qDebug() << "checkAccessAbleFile-tmp_flagAccessAlbe" << tmp_flagAccessAlbe;
    #elif defined(Q_OS_MAC)
qDebug() << "p_path=" << p_path;
        QFileInfo check_file("/Volumes/"+p_ip+"-"+p_path);
        //QFileInfo check_file("//"+p_ip+"/"+p_path);
        if(!check_file.exists()){

            tmp_flagAccessAlbe = false;
            qDebug() << "tmp_flagAccessAlbe" << tmp_flagAccessAlbe;//c220909_1
        }else
            qDebug() << "tmp_flagAccessAlbe" << tmp_flagAccessAlbe;//c220909_1

    #endif

    return tmp_flagAccessAlbe;
}



/**
 * @brief MusicFolder_USB::openNetworkDriveForWindow : 윈도우 네트워크 드라이브 연결
 * @param p_index int 등록된 네트워크 index
 */
void MusicFolder_USB::openNetworkDriveForWindow(const int &p_index){
    print_debug();
    QString tmp_ip = listNetworkFolder->at(p_index)->getIp();
    QString tmp_path = listNetworkFolder->at(p_index)->getPath();
    QString tmp_id = listNetworkFolder->at(p_index)->getId();
    QString tmp_pw = listNetworkFolder->at(p_index)->getPw();
    qDebug() << "tmp_path=" << tmp_path;
    if(tmp_path.right(1)=="/"){
        tmp_path.chop(1);
    }

    QString tmp_win_sep = "";
    if(!tmp_path.isEmpty()){
        tmp_win_sep = "\\";
    }
    print_debug();
    qDebug() << "listNetworkFolder->at(p_index)->getName()=" << listNetworkFolder->at(p_index)->getName();
qDebug() << "999999999999999999999999999999999----001";
    // net.exe 이용 smb 연결 삭제
    QProcess *process = new QProcess(this);
    process->setProgram("net.exe");
    process->setArguments(QStringList()<<"use" <<"/delete"<<"\\\\"+tmp_ip+tmp_win_sep+tmp_path.replace("/","\\"));
    process->startDetached();
    process->waitForFinished(-1);

    // net.exe 이용 smb 연결 추가
    QStringList strListProcess;
    strListProcess.append("use");
    strListProcess.append("\\\\"+tmp_ip+tmp_win_sep+tmp_path.replace("/","\\"));
    if(!tmp_pw.isEmpty()){
        strListProcess.append(tmp_pw);
    }
    if(!tmp_id.isEmpty()){
        strListProcess.append("/user:"+tmp_id);
    }
    strListProcess.append("/persistent:no");

    process->setProgram("net.exe");
    process->setArguments(strListProcess);

qDebug() << "999999999999999999999999999999999----002";
    process->startDetached();
    process->waitForFinished(-1);
    process->close();
    delete process;
}

void MusicFolder_USB::openLocalNetworkDriveForWindow(const int &p_index){
    print_debug();

    QString tmp_ip = listLocalNetworkFolder->at(p_index)->getIp();
    QString tmp_path = listLocalNetworkFolder->at(p_index)->getPath();
    QString tmp_id = listLocalNetworkFolder->at(p_index)->getId();
    QString tmp_pw = listLocalNetworkFolder->at(p_index)->getPw();
    qDebug() << "tmp_path=" << tmp_path;
    if(tmp_path.right(1)=="/"){
        tmp_path.chop(1);
    }

    QString tmp_win_sep = "";
    if(!tmp_path.isEmpty()){
        tmp_win_sep = "\\";
    }

    // net.exe 이용 smb 연결 삭제
    QProcess *process = new QProcess(this);
    process->setProgram("net.exe");
    process->setArguments(QStringList()<<"use" <<"/delete"<<"\\\\"+tmp_ip+tmp_win_sep+tmp_path.replace("/","\\"));
    process->startDetached();
    process->waitForFinished(-1);

    // net.exe 이용 smb 연결 추가
    QStringList strListProcess;
    strListProcess.append("use");
    strListProcess.append("\\\\"+tmp_ip+tmp_win_sep+tmp_path.replace("/","\\"));
    if(!tmp_pw.isEmpty()){
        strListProcess.append(tmp_pw);
    }
    if(!tmp_id.isEmpty()){
        strListProcess.append("/user:"+tmp_id);
    }
    strListProcess.append("/persistent:no");

    process->setProgram("net.exe");
    process->setArguments(strListProcess);


    process->startDetached();
    process->waitForFinished(-1);
    process->close();
    delete process;
}

/**
 * @brief MusicFolder_USB::reCheckNetworkDriveOpened : 네트워크 파일 접속 가능 여부 reCheck
 * @param p_index int 등록된 네트워크 폴더 index
 * @param p_ip QString 등록된 네트워크 폴더 ip
 * @param p_path QString 등록된 네트워크 폴더 path
 */
void MusicFolder_USB::reCheckNetworkDriveOpened( const int &p_index, const QString &p_ip,  const QString &p_path){

    if(checkAccessAbleFile(p_ip, p_path)){
        FolderNetwork_ClickableRow *folderRow = qobject_cast<FolderNetwork_ClickableRow*>(vl_networkList->itemAt(p_index)->widget());
        folderRow->setImage(":images/icon_net_fol_on.png");
        folderRow->setProperty("flagOn", true);
        folderRow->setProperty("flagAccess", true);
    }
}

void MusicFolder_USB::reCheckLocalNetworkDriveOpened( const int &p_index, const QString &p_ip,  const QString &p_path){

    if(checkAccessAbleFile(p_ip, p_path)){
        FolderNetwork_ClickableRow *folderRow = qobject_cast<FolderNetwork_ClickableRow*>(vl_localnetworkList->itemAt(p_index)->widget());
        folderRow->setImage(":images/icon_net_fol_on.png");
        folderRow->setProperty("flagOn", true);
        folderRow->setProperty("flagAccess", true);
    }
}

/**
 * @brief MusicFolder_USB::requestNetworkFolderList : 네트워크 폴더 리스트 요청
 * @details 비디오-폴더와 동일 API : IP:PORT/external_usb
 */
void MusicFolder_USB::slot_requestNetworkFolderList(){
print_debug();
    if(global.flagConnected){
        qDeleteAll(listNetworkFolder->begin(), listNetworkFolder->end());
        listNetworkFolder->clear();
        qDeleteAll(listUsb->begin(), listUsb->end());
        listUsb->clear();
        GSCommon::clearLayout(vl_networkList);
        GSCommon::clearLayout(vl_usb);

        QJsonObject tmp_json;
        tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
        tmp_json.insert("musicType","10");
        tmp_json.insert("music",QJsonArray());

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_NETWORK_FOLDER_LIST, QString("http://%1:%2/external_usb")
                         .arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true);
    }

}
void MusicFolder_USB::slot_requestLocalNetworkFolderList(){
print_debug();
    if(global.flagConnected){
        qDeleteAll(listLocalNetworkFolder->begin(), listLocalNetworkFolder->end());
        listLocalNetworkFolder->clear();
        qDeleteAll(listUsb->begin(), listUsb->end());
        listUsb->clear();
        GSCommon::clearLayout(vl_localnetworkList);
        GSCommon::clearLayout(vl_usb);

        QJsonObject tmp_json;
        tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
        tmp_json.insert("musicType","10");
        tmp_json.insert("music",QJsonArray());

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_NETWORK_FOLDER_LIST, QString("http://%1:%2/external_usb")
                         .arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true);
    }

}
/**
 * @brief MusicFolder_USB::setResultOfNetworkFolderList : 네트워크 폴더 목록 요청 결과 처리
 * @param p_jsonObject QJsonObject response
 */
void MusicFolder_USB::setResultOfNetworkFolderList(const QJsonObject &p_jsonData){////c220907_1

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_externalArr = "externalArr";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";
print_debug();
qDebug()<< p_jsonData;
    if(p_jsonData.contains(jsonKey_flagOk)&&p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonData[jsonKey_status].toObject();
            // TODO:: Search Devices 16/02/2021 by disket
            //if(jsonStatus.contains(jsonKey_outs)&&jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
                if(p_jsonData.contains(jsonKey_externalArr)){
                    QJsonArray jsonArrExternal = p_jsonData[jsonKey_externalArr].toArray();
                    this->listNetworkFolder_cnt = jsonArrExternal.size();//c220908_1
                    for(int i = 0; i < jsonArrExternal.size(); i++){
                        DataNetworkFolder *data = new DataNetworkFolder;
                        data->setData(jsonArrExternal.at(i).toObject());

                        if(data->getIsServer()){

                            // smb 드라이브 추가
                            listNetworkFolder->append(data);
                            appendNetworkFolderUI(listNetworkFolder->size()-1);
                        }else{
                            listUsb->append(data);
                            // usb 드라이브 추가
                            Folder_ClickableRow *clickableRow_usb = new Folder_ClickableRow(":/images/icon_folder_bic.png", data->getName());
                            clickableRow_usb->setProperty("type", TYPE_FOLDER_USB);
                            clickableRow_usb->setProperty("index", listUsb->size()-1);
                            clickableRow_usb->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), "usb_"+QString::number(listUsb->size()-1));
                            vl_usb->insertWidget(1, clickableRow_usb);

                            connect(clickableRow_usb, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));
                        }
                    }
                }
            //}
        }
    }
}
void MusicFolder_USB::setResultOfLocalNetworkFolderList(const QJsonObject &p_jsonData){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_externalArr = "externalArr";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";
print_debug();
    if(p_jsonData.contains(jsonKey_flagOk)&&p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonData[jsonKey_status].toObject();
            // TODO:: Search Devices 16/02/2021 by disket
            //if(jsonStatus.contains(jsonKey_outs)&&jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
                if(p_jsonData.contains(jsonKey_externalArr)){
                    QJsonArray jsonArrExternal = p_jsonData[jsonKey_externalArr].toArray();

                    for(int i = 0; i < jsonArrExternal.size(); i++){
                        DataNetworkFolder *data = new DataNetworkFolder;
                        data->setData(jsonArrExternal.at(i).toObject());

                        if(data->getIsServer()){
                            // smb 드라이브 추가
                            listLocalNetworkFolder->append(data);
                            appendLocalNetworkFolderUI(listLocalNetworkFolder->size()-1);//c220505
                        }else{
                            listUsb->append(data);
                            // usb 드라이브 추가
                            Folder_ClickableRow *clickableRow_usb = new Folder_ClickableRow(":/images/icon_folder_bic.png", data->getName());
                            clickableRow_usb->setProperty("type", TYPE_FOLDER_USB);
                            clickableRow_usb->setProperty("index", listUsb->size()-1);
                            clickableRow_usb->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), "usb_"+QString::number(listUsb->size()-1));
                            vl_usb->insertWidget(1, clickableRow_usb);

                            connect(clickableRow_usb, SIGNAL(signal_clicked()), this, SLOT(slot_clickedRow()));
                        }
                    }
                }
            //}
        }
    }
}
/**
 * @brief MusicFolder_USB::requestRemoveNetworkFolder : 네트워크 폴더 삭제 요청
 * @param p_index
 */
void MusicFolder_USB::requestRemoveNetworkFolder(const int &p_index){//c220913_2
    print_debug();
    qDebug() << "listNetworkFolder->size()=" << listNetworkFolder->size();
    qDebug() << "p_index=" << p_index;

    //QJsonObject fileInfo = listLocalNetworkFolder->at(p_index)->getData();

    //qDebug() << "listNetworkFolder->at(p_index)->getData()=" << listLocalNetworkFolder->at(p_index)->getData();
    QJsonObject tmp_json;
    tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
    tmp_json.insert("orderstate","DELETE");
    tmp_json.insert("fileInfo",listNetworkFolder->at(p_index)->getData());
    print_debug();
    qDebug() << tmp_json;
#if defined(Q_OS_WIN)
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_NETWORK_FOLDER_DELETE, QString("http://%1:%2/remote_network_share")
                     .arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true);
#elif defined(Q_OS_MAC)
    QJsonObject fileInfo = ProcJsonEasy::getJsonObject(tmp_json, "fileInfo");
    QString dialog_msg = QString(tr("Authenticate deletion of network folder /%1 in %2.")).arg(ProcJsonEasy::getString(fileInfo, "path").split("/").first()).arg(ProcJsonEasy::getString(fileInfo, "ip"));
    Dialog::DialogLogin_mac *dlg_network = new Dialog::DialogLogin_mac(ProcJsonEasy::getString(fileInfo, "ip"), dialog_msg,this);
    int result = dlg_network->exec();

    if(result == QDialog::Accepted){
        qDebug() << "ip=" << ProcJsonEasy::getString(fileInfo, "ip");
        QString cmd_argu = QString("/Volumes/%1-%2").arg(ProcJsonEasy::getString(fileInfo, "ip")).arg(ProcJsonEasy::getString(fileInfo, "path"));
        qDebug() << "cmd_argu=" << cmd_argu;
        QProcess *process = new QProcess();
        process->start(QString("sh -c \"echo %2 | sudo -S umount %1\"").arg(cmd_argu).arg(global.userPassword));//c220913_2
        process->waitForFinished();
        //process->start("kdialog --password \"Please enter your password to continue\"");//
        //process->waitForFinished();
        QString standardOutput = process->readAllStandardOutput();
        qDebug() << "umount-standardOutput=" << standardOutput;
        global.userPassword = "";

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_NETWORK_FOLDER_DELETE, QString("http://%1:%2/remote_network_share")
                         .arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true);
    }


#endif



}
void MusicFolder_USB::requestRemoveLocalNetworkFolder(const int &p_index){//c220909_1
    print_debug();
    QJsonObject tmp_json;
    tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
    tmp_json.insert("orderstate","DELETE");
    tmp_json.insert("fileInfo",listLocalNetworkFolder->at(p_index)->getData());
    /*
        //---------------------
        QProcess *process = new QProcess();
        process->setProgram("umount");

        QStringList strListProcess;
        QString cmd_argu = QString("/Volumes/%1").arg(listLocalNetworkFolder->at(p_index)->getPath());
        strListProcess.append(cmd_argu);
        // net.exe 이용 smb 연결 추가
        process->setArguments(strListProcess);
        bool bb = process->startDetached();
        qDebug() << "bb=" << bb;
        process->waitForFinished(-1);
        //----------------------
*/
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_LOCAL_NETWORK_FOLDER_DELETE, QString("http://%1:%2/remote_network_share")
                     .arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true);

}
/**
 * @brief MusicFolder_USB::setResultOfNetworkFolderRemove : 네트워크
 * @param p_jsonObject
 */
void MusicFolder_USB::setResultOfNetworkFolderRemove(const QJsonObject &p_jsonObject){
    print_debug();
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";
    const QString jsonKey_message = "message";

    if(p_jsonObject.contains(jsonKey_flagOk)&&p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonObject[jsonKey_status].toObject();
            //if(jsonStatus.contains(jsonKey_outs)&&jsonStatus[jsonKey_outs].toString().toLower()=="ok"){

            if(p_jsonObject.contains(jsonKey_message)&&p_jsonObject[jsonKey_message].toString().toLower()!="success"){
                ToastMsg::show(this,"",p_jsonObject[jsonKey_message].toString());
            }else{

                slot_requestNetworkFolderList();
            }
            /*}else if(p_jsonObject.contains(jsonKey_message)){
                ToastMsg::show(this,"",p_jsonObject[jsonKey_message].toString());
            }*/
        }
    }
    else{
        ToastMsg::show(this,"",tr("Please check the network status."));
        //ToastMsg::show(this,"",tr("네트워크 상태를 확인해주세요."));
    }

}
void MusicFolder_USB::setResultOfLocalNetworkFolderRemove(const QJsonObject &p_jsonObject){
print_debug();
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";
    const QString jsonKey_message = "message";

    if(p_jsonObject.contains(jsonKey_flagOk)&&p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonObject[jsonKey_status].toObject();
            //if(jsonStatus.contains(jsonKey_outs)&&jsonStatus[jsonKey_outs].toString().toLower()=="ok"){

                if(p_jsonObject.contains(jsonKey_message)&&p_jsonObject[jsonKey_message].toString().toLower()!="success"){
                    ToastMsg::show(this,"",p_jsonObject[jsonKey_message].toString());
                }else{
                    slot_requestLocalNetworkFolderList();
                }
            /*}else if(p_jsonObject.contains(jsonKey_message)){
                ToastMsg::show(this,"",p_jsonObject[jsonKey_message].toString());
            }*/
        }
    }
    else{
        ToastMsg::show(this,"",tr("Please check the network status."));
        //ToastMsg::show(this,"",tr("네트워크 상태를 확인해주세요."));
    }

}


/**
 * @brief MusicFolder_USB::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void MusicFolder_USB::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
print_debug();
    if(p_id == HTTP_NETWORK_FOLDER_LIST){
        setResultOfNetworkFolderList(p_jsonObject);
    }
    else if(p_id == HTTP_NETWORK_FOLDER_DELETE){
        setResultOfNetworkFolderRemove(p_jsonObject);
    }
    else if(p_id == HTTP_LOCAL_NETWORK_FOLDER_LIST){
        setResultOfLocalNetworkFolderList(p_jsonObject);
    }
    else if(p_id == HTTP_LOCAL_NETWORK_FOLDER_DELETE){
        setResultOfLocalNetworkFolderRemove(p_jsonObject);
    }

    sender()->deleteLater();
}

void MusicFolder_USB::slot_searchNetworkFolder(){//c220908_2
    print_debug();
#if defined(Q_OS_WIN)
    global.window_activate_flag = true;
    ToastMsg::show(this, "", tr("Searching for local network... Please wait."), 3000);//c220906_1
    this->topDirPath->addDirName(tr("Network"), FOLDER_USB_DIR_CODE_NETWORK);
    //this->topDirPath->addDirName(tr("네트워크"), FOLDER_USB_DIR_CODE_NETWORK);
    this->stackedWidget_page->setCurrentWidget(this->page_network);
    page_network->requestFindIP(true);
#elif defined(Q_OS_MAC)
    global.window_activate_flag = true;
    ToastMsg::show(this, "", tr("Searching for local network... Please wait."), 3000);//c220906_1
    this->topDirPath->addDirName(tr("Network"), FOLDER_USB_DIR_CODE_NETWORK);
    //this->topDirPath->addDirName(tr("네트워크"), FOLDER_USB_DIR_CODE_NETWORK);
    this->stackedWidget_page->setCurrentWidget(this->page_network);
    page_network->requestFindIP(true);

#endif

}

/**
 * @brief MusicFolder_USB::slot_clickedRow ROW 객체 클릭시
 */
void MusicFolder_USB::slot_clickedRow(){//c220908_2
    print_debug();
    QString tmp_type = dynamic_cast<Folder_ClickableRow*>(sender())->property("type").toString();//cheon210704-list
    qDebug() << "tmp_type=" << tmp_type;
#if defined(Q_OS_WIN)

    print_debug();
    qDebug() << "tmp_type = " << tmp_type;
    global.music_folder_topdir = tmp_type;
    if(tmp_type==TYPE_FOLDER_LOCAL){//c220609
        print_debug();

#if 1   //DEVICE_MODE  //c220705
        Dialog::DialogAdd_Network_local *dlg_network = new Dialog::DialogAdd_Network_local(this);//cheon210617-file
        int result = dlg_network->exec();

        if(result == QDialog::Accepted){
            print_debug();
            // nothing
        }

        delete dlg_network;
#endif

#if 0    //DESKTOP_MODE   //c220705
        //this->topDirPath->clearDirName();
        this->topDirPath->addDirName(tr("Desktop"), FOLDER_USB_DIR_CODE_DRIVER);
        this->stackedWidget_page->setCurrentWidget(this->page_dirFileList);
        this->page_dirFileList->setInitDriver();
        //connect(this->page_dirFileList, SIGNAL(signal_fileInfo_filePath(QString)), this, SLOT(slot_fileInfo_filePath(QString)));//c220609

        //connect(this->page_dirFileList, SIGNAL(signal_clickedDirName(QString, QString)), this, SLOT(slot_changeTopDirName(QString, QString)));//c220609
       // connect(this->topDirPath, SIGNAL(signal_clickedDirName(QString, QString)), this, SLOT(slot_changeTopDirName(QString, QString)));//c220609
#endif

    }else if(tmp_type==TYPE_FOLDER_USB){//cheon210617-file
        print_debug();
        int tmp_index = sender()->property("index").toInt();
        QString tmp_contentStep = sender()->property(KEY_CONTENT_STEP.toStdString().c_str()).toString();
        qDebug() << "MusicFolder_USB::slot_clickedRow-tmp_contentStep----- : " << tmp_contentStep;
        qDebug() << "MusicFolder_USB::slot_clickedRow-tmp_index----- : " << tmp_index;
        if(tmp_contentStep.size()> 0){//cheon210617-file
            emit clickedUSB(tmp_contentStep, listUsb->at(tmp_index)->getData());//cheon210617-file
        }else{
            ToastMsg::show(this, "", tr("There is no device in the usb port."));//cheon210617-file
        }


    }else if(tmp_type==TYPE_FOLDER_NETWORK){

        print_debug();
        ToastMsg::show(this, "", tr("Searching for local network... Please wait."), 3000);//c220906_1
        this->topDirPath->addDirName(tr("Network"), FOLDER_USB_DIR_CODE_NETWORK);
        //this->topDirPath->addDirName(tr("네트워크"), FOLDER_USB_DIR_CODE_NETWORK);
        this->stackedWidget_page->setCurrentWidget(this->page_network);
        page_network->requestFindIP(true);

    }else if(tmp_type==TYPE_FOLDER_CLOUD){

        this->topDirPath->addDirName(tr("Cloud"), FOLDER_USB_DIR_CODE_CLOUD);
        //this->topDirPath->addDirName(tr("클라우드"), FOLDER_USB_DIR_CODE_CLOUD);
        this->stackedWidget_page->setCurrentWidget(page_cloud);
        page_cloud->requestCloudlist(true);

    }else if(tmp_type==TYPE_FOLDER_ADD_NETWORK_FOLDER){

        print_debug();
        // show 네트워크 폴더 Add Dialog
        Dialog::DialogAdd_Network *dlg_network = new Dialog::DialogAdd_Network(this);
        int result = dlg_network->exec();

        if(result == QDialog::Accepted){
            // nothing
        }

        delete dlg_network;
    }
    else if(tmp_type==TYPE_FOLDER_NETWORK_FOLDER){
        print_debug();
    }
#elif defined(Q_OS_MAC)//cheon210831-network
    if(tmp_type==TYPE_FOLDER_LOCAL){
        print_debug();


        Dialog::DialogAdd_Network_local *dlg_network = new Dialog::DialogAdd_Network_local(this);//cheon210617-file
        int result = dlg_network->exec();

        if(result == QDialog::Accepted){
            print_debug();
            // nothing
        }

        delete dlg_network;
    }else if(tmp_type==TYPE_FOLDER_USB){//cheon210617-file
            int tmp_index = sender()->property("index").toInt();
            QString tmp_contentStep = sender()->property(KEY_CONTENT_STEP.toStdString().c_str()).toString();
            if(tmp_contentStep.size()> 0){//cheon220906_1
                emit clickedUSB(tmp_contentStep, listUsb->at(tmp_index)->getData());//cheon210617-file
            }else{
                ToastMsg::show(this, "", tr("There is no device in the usb port."));//cheon210617-file
            }
    }else if(tmp_type==TYPE_FOLDER_NETWORK){

        print_debug();
        ToastMsg::show(this, "", tr("Searching for local network... Please wait."), 3000);//c220906_1
        this->topDirPath->addDirName(tr("Network"), FOLDER_USB_DIR_CODE_NETWORK);
        this->stackedWidget_page->setCurrentWidget(this->page_network);
        page_network->requestFindIP(true);

    }
    else if(tmp_type==TYPE_FOLDER_ADD_NETWORK_FOLDER){//cheon210825-network
        //dialog_comfirmNetworkfolder_forFinder_add();//c220907_1
/*
        QProcess *process = new QProcess();

        //process->start(QString("sh -c \"echo 12345678 | sudo -S -l\""));//
        process->start(QString("sh -c \"echo 12ghhgtgh78 | sudo -S ls /\""));//
        process->waitForFinished();
        //process->start("echo dfdfd");//
        //process->waitForFinished();
        QString standardOutput = process->readAllStandardOutput();
        qDebug() << "umount-standardOutput=" << standardOutput;
*/
        qDebug() << "this->listNetworkFolder_cnt = " << this->listNetworkFolder_cnt;

        if(this->listNetworkFolder_cnt <= 0){//c220908_1
            dialog_comfirmNetworkfolder_forFinder();//c220907_1
        }

        // show 네트워크 폴더 Add Dialog
        Dialog::DialogAdd_Network *dlg_network = new Dialog::DialogAdd_Network(this);
        int result = dlg_network->exec();

        if(result == QDialog::Accepted){
            // nothing
        }

        delete dlg_network;
    }
    else if(tmp_type==TYPE_FOLDER_NETWORK_FOLDER){



    }else if(tmp_type==TYPE_FOLDER_CLOUD){

        this->topDirPath->addDirName(tr("Cloud"), FOLDER_USB_DIR_CODE_CLOUD);
        //this->topDirPath->addDirName(tr("클라우드"), FOLDER_USB_DIR_CODE_CLOUD);
        this->stackedWidget_page->setCurrentWidget(page_cloud);
        page_cloud->requestCloudlist(true);

    }
    //ToastMsg::show(this, "", tr("MusicFolder_USB::slot_clickedRow- The current version is not supported by MAC Desktop.\n It will be supported in a future version.."));//cheon210825-network
#endif
}



/**
 * @brief MusicFolder_USB::goInitPage  폴더 메뉴 보고 있는 상태에서 또 폴더 메뉴 클릭시..
 * @note [예외상황임] 상단 TopNaviBar 클래스에서 동일 메뉴 클릭시 아무것도 처리 안하기 때문에 폴더메뉴는 별도로 처리 필요하다. ( 내부적으로 StackWidget 있지만 TopNaviBar에서 Page 관리를 안하기 때문이다)
 */
void MusicFolder_USB::goInitPage(){
    this->topDirPath->clearDirName();
    this->topDirPath->addDirName(tr("Explore"), FOLDER_USB_DIR_CODE_INIT);
    //this->topDirPath->addDirName(tr("탐색"), FOLDER_USB_DIR_CODE_INIT);
    this->stackedWidget_page->setCurrentIndex(0);
}

void MusicFolder_USB::slot_fileInfo_filePath_stop(){//c220609

    print_debug();

   // global.music_player->pause();


}




/**
 * @brief MusicFolder_USB::slot_changeTopDirName [SLOT] 상단 디렉토리명 변경
 * @param p_dirName
 * @param p_dirPath
 */
void MusicFolder_USB::slot_changeTopDirName(QString p_dirName, QString p_dirPath){
    print_debug();

    this->topDirPath->addDirName(p_dirName, p_dirPath);
    qDebug() << "this->topDirPath = " << p_dirName<< " "<< p_dirPath;
}

/**
 * @brief MusicFolder_USB::slot_changedRootPath [SLOT] 해당 경로로 Dir 루트 변경
 * @param p_dirName
 * @param p_dirPath
 */
void MusicFolder_USB::slot_changedRootPath(QString p_dirName, QString p_dirPath){
print_debug();
qDebug() << "p_dirPath=" << p_dirPath;
qDebug() << "global.music_folder_topdir=" << global.music_folder_topdir;
qDebug() << "FOLDER_USB_DIR_CODE_DRIVER=" << FOLDER_USB_DIR_CODE_DRIVER;
    if(p_dirPath==FOLDER_USB_DIR_CODE_INIT){

        this->goInitPage();

    }else if(p_dirPath==FOLDER_USB_DIR_CODE_DRIVER || global.music_folder_topdir == FOLDER_USB_DIR_CODE_DRIVER){
        print_debug();

        this->topDirPath->addDirName(tr("Driver"), FOLDER_USB_DIR_CODE_DRIVER);
        this->stackedWidget_page->setCurrentWidget(this->page_dirFileList);
        page_dirFileList->setInitDriver();


    }else if(p_dirPath==FOLDER_USB_DIR_CODE_NETWORK){

        print_debug();
        global.music_folder_topdir = p_dirPath;
        qDebug()<<"global.music_folder_topdir =" << global.music_folder_topdir ;

        this->topDirPath->addDirName(tr("network"), FOLDER_USB_DIR_CODE_NETWORK);
        //this->topDirPath->addDirName(tr("네트워크"), FOLDER_USB_DIR_CODE_NETWORK);
        this->stackedWidget_page->setCurrentWidget(page_network);
        page_network->requestFindIP();
    }else if(p_dirPath==FOLDER_USB_DIR_CODE_CLOUD){
        print_debug();
        global.music_folder_topdir = p_dirPath;
        qDebug()<<"global.music_folder_topdir =" << global.music_folder_topdir ;

        this->topDirPath->addDirName(tr("cloud"), FOLDER_USB_DIR_CODE_CLOUD);
        //this->topDirPath->addDirName(tr("클라우드"), FOLDER_USB_DIR_CODE_CLOUD);
        this->stackedWidget_page->setCurrentWidget(page_cloud);

    }else{
        print_debug();
        if(global.music_folder_topdir == TYPE_FOLDER_LOCAL){
            print_debug();
            // 상단 타이틀 디렉토리명 변경 요청
            this->topDirPath->addDirName(p_dirName, p_dirPath);
            page_dirFileList->setInitNetworkDriver(p_dirName,p_dirPath);
        }else{
            this->page_fileTree->slot_changedRootPath(p_dirName, p_dirPath);
        }


    }
}

void MusicFolder_USB::slot_clickedNetworkHost(const QString &p_ip, const QString &p_hostName){
    this->topDirPath->clearDirName();
    this->page_fileTree->setData(p_hostName, p_ip);
    this->stackedWidget_page->setCurrentWidget(this->page_fileTree);

}


/**
 * @brief MusicFolder_USB::slot_clickedRemoveNetwork::[슬롯]네트워크삭제 클릭
 */
void MusicFolder_USB::slot_clickedRemoveNetwork(){
    DialogConfirm *dlg = new DialogConfirm(this);
    dlg->setTitle(tr("Delete network folder"));
    //dlg->setTitle(tr("네트워크 폴더 삭제"));
    dlg->setText(tr("Are you sure you want to delete?"));
    //dlg->setText(tr("정말 삭제하시겠습니까?"));

    int result = dlg->exec();

    if(result==QDialog::Accepted){
        requestRemoveNetworkFolder(sender()->property("index").toInt());
    }

    delete dlg;
}
void MusicFolder_USB::slot_clickedRemoveLocalNetwork(){
    DialogConfirm *dlg = new DialogConfirm(this);
    dlg->setTitle(tr("Delete network folder"));
    //dlg->setTitle(tr("네트워크 폴더 삭제"));
    dlg->setText(tr("Are you sure you want to delete?"));
    //dlg->setText(tr("정말 삭제하시겠습니까?"));

    int result = dlg->exec();

    if(result==QDialog::Accepted){
        requestRemoveLocalNetworkFolder(sender()->property("index").toInt());
    }

    delete dlg;
}
/**
 * @brief MusicFolder_USB::slot_clickedNetworkFolder::[슬롯]네트워크폴더 클릭
 */
void MusicFolder_USB::slot_clickedNetworkFolder(){

    int tmp_index = sender()->property("index").toInt();
    bool tmp_flagOn = sender()->property("flagOn").toBool();
    bool tmp_flagAccess = sender()->property("flagAccess").toBool();

    if(tmp_flagOn && tmp_flagAccess){
        QString tmp_conentsStep = sender()->property(KEY_CONTENT_STEP.toStdString().c_str()).toString();

        if(tmp_index >=0 && tmp_index < listNetworkFolder->size()){
            QString tmp_smbPath = listNetworkFolder->at(tmp_index)->getConvertSmbPath();
            QStringList tmp_pathList = listNetworkFolder->at(tmp_index)->getPath().split("/");
            QString tmp_rootName;
            for(int i = tmp_pathList.size()-1; i >=0; i--){

                if(!tmp_pathList.at(i).isEmpty()){
                    tmp_rootName = tmp_pathList.at(i);
                    break;
                }
            }

            if(tmp_rootName.isEmpty()){
                tmp_rootName = listNetworkFolder->at(tmp_index)->getIp();
            }
            print_debug();
            qDebug() << "tmp_rootName = " << tmp_rootName;
            qDebug() << "tmp_smbPath = " << tmp_smbPath;
            emit clickedNetworkFolder(tmp_conentsStep, tmp_rootName, tmp_smbPath);
        }
    }
    else{
        if(tmp_flagOn && !tmp_flagAccess){//cheon210831-network
            print_debug();
      #if defined(Q_OS_WIN)
                ToastMsg::show(this,"",tr("Could not connect to network folder."));
                //ToastMsg::show(this,"",tr("네트워크 폴더에 연결할 수 없습니다."));
      #elif defined(Q_OS_MAC)//c220918_2
            // show 네트워크 폴더 Add Dialog
            Dialog::DialogAdd_Network *dlg_network = new Dialog::DialogAdd_Network(this);

            dlg_network->setFolderInfo("//"+listNetworkFolder->at(tmp_index)->getIp()+"/"+listNetworkFolder->at(tmp_index)->getPath(), listNetworkFolder->at(tmp_index)->getId(), listNetworkFolder->at(tmp_index)->getPw());
            int result = dlg_network->exec();

            if(result == QDialog::Accepted){
                //this->hide();
                emit linker->signal_goFolder("folder");//
                // nothing
            }

            delete dlg_network;

                //dialog_comfirmNetworkfolder_forFinder();
                //ToastMsg::show(this,"",tr("Could not connect to network folder.\nIn the Finder App, click Go -> Connect to Server.. \nEnter smb://[address] in the server address field.\n\nFor example, if the server address is google.com/sharefolder, \nyou can enter as follows.\nAddress example: smb://google.com/sharefolder"),5000);//cheon210825-network
       #endif
        }

    }

}
//dialog_comfirmNetworkfolder_forFinder_add
void MusicFolder_USB::dialog_comfirmNetworkfolder_forFinder()//c220913_1
{
    /*
     * Finder에서 이동(Go) -> 서버의 연결(Connect to Server..)을 눌러 창을 띄우고 (단축키 : Command + K)
서버주소 란에 smb://[주소]를 입력한다.
예를들어 서버 주소가 google.com/smb 라면 다음과 같이 입력하면 된다.
주소 예 : smb://google.com/smb
출처: https://reysion.tistory.com/17 [미로의 노트:티스토리]
*/
    dlgConfirmFinder = new DialogConfirm_MacShare(this);
    dlgConfirmFinder->setTitle(tr("Network Folder Notice!!"));
    dlgConfirmFinder->setText(tr("\nCould not connect to network folder.\nin Finder App, click Go ->Connect to Server.. \nEnter smb://[address] in the server address field.\n\nFor example, if the server address is 192.168.1.34/sharefolder, \nyou can enter as follows.\nAddress example: smb://192.168.1.34/sharefolder.\n"));

    //dlgConfirmFinder->setAlertMode();//c220907_1
    dlgConfirmFinder->setProperty("flagShown",false);
    //--------------
//    dlgConfirmFinder->setTextHeight(150);
print_debug();
    QPixmap pixmapPI_off, pixmapPI_on;
    pixmapPI_off.load(":images/samba_finder01.png");
    pixmapPI_off = pixmapPI_off.scaled(300,300, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    pixmapPI_on.load(":images/samba_finder02.png");
    pixmapPI_on = pixmapPI_on.scaled(300,300, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QLabel *lb_PIImg_off = new QLabel;
    lb_PIImg_off->setFixedSize(300,300);
    lb_PIImg_off->setPixmap(pixmapPI_off);

    QLabel *lb_PIImg_on = new QLabel;
    lb_PIImg_on->setFixedSize(300,300);
    lb_PIImg_on->setPixmap(pixmapPI_on);


    QHBoxLayout *lh_PI_Info = new QHBoxLayout;
    lh_PI_Info->setSpacing(30);
    lh_PI_Info->setContentsMargins(0,0,0,20);
    lh_PI_Info->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    lh_PI_Info->addWidget(lb_PIImg_off);
    lh_PI_Info->addWidget(lb_PIImg_on);

    dlgConfirmFinder->setLayoutMy_withText(lh_PI_Info);
    dlgConfirmFinder->setFixedWidth(700);
    //dlgConfirmFinder->setGeometry(this->width()/3,this->height()/5, 800,800);
    int left = global.left_mainwindow+global.width_mainwindow/2- (dlgConfirmFinder->sizeHint().width() / 2);//c220804
    int top = global.top_mainwindow+global.height_mainwindow/2- (dlgConfirmFinder->sizeHint().height() / 2);//c220804
    dlgConfirmFinder->move(left, top);//c220804
    //--------------
    if(dlgConfirmFinder->property("flagShown").toBool()==false){
        dlgConfirmFinder->setProperty("flagShown",true);

        int result = dlgConfirmFinder->exec();

        if(result == QDialog::Accepted){


        }
    }
    delete dlgConfirmFinder;
}

void MusicFolder_USB::dialog_comfirmNetworkfolder_forFinder_add()//c220913_1
{
    dlgConfirmFinder = new DialogConfirm_MacShare(this);
    dlgConfirmFinder->setTitle(tr("Network Folder Notice!!"));
    dlgConfirmFinder->setText(tr("\nIn the Finder App, click Go -> Connect to Server.. \nEnter smb://[address] in the server address field.\n\nFor example, if the server address is 192.168.1.34/sharefolder, \nyou can enter as follows.\nAddress example: smb://192.168.1.34/sharefolder.\n"));
    dlgConfirmFinder->setGeometry(this->width()/3,this->height()/5, 350,600);
    //dlgConfirmFinder->setAlertMode();//c220907_1
    dlgConfirmFinder->setProperty("flagShown",false);
    if(dlgConfirmFinder->property("flagShown").toBool()==false){
        dlgConfirmFinder->setProperty("flagShown",true);

        int result = dlgConfirmFinder->exec();

        if(result == QDialog::Accepted){


        }
    }
    delete dlgConfirmFinder;
}

void MusicFolder_USB::slot_clickedLocalNetworkFolder(){

    int tmp_index = sender()->property("index").toInt();
    bool tmp_flagOn = sender()->property("flagOn").toBool();
    bool tmp_flagAccess = sender()->property("flagAccess").toBool();

    if(tmp_flagOn && tmp_flagAccess){
        QString tmp_conentsStep = sender()->property(KEY_CONTENT_STEP.toStdString().c_str()).toString();

        if(tmp_index >=0 && tmp_index < listLocalNetworkFolder->size()){
            QString tmp_smbPath = listLocalNetworkFolder->at(tmp_index)->getConvertSmbPath();
            QStringList tmp_pathList = listLocalNetworkFolder->at(tmp_index)->getPath().split("/");
            QString tmp_rootName;
            for(int i = tmp_pathList.size()-1; i >=0; i--){

                if(!tmp_pathList.at(i).isEmpty()){
                    tmp_rootName = tmp_pathList.at(i);
                    break;
                }
            }

            if(tmp_rootName.isEmpty()){
                tmp_rootName = listLocalNetworkFolder->at(tmp_index)->getIp();
            }

            emit clickedNetworkFolder(tmp_conentsStep, tmp_rootName, tmp_smbPath);
        }
    }
    else{
        ToastMsg::show(this,"",tr("Could not connect to local network folder."));
        //ToastMsg::show(this,"",tr("네트워크 폴더에 연결할 수 없습니다."));
    }

}
