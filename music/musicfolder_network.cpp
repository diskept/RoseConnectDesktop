#include "musicfolder_network.h"
#include "common/findip.h"
#include "common/gscommon.h"

#include "login/dialoglogin_network.h"
#include "widget/clickablewidget.h"
#include "widget/dialogconfirm_macshare.h"////c220913_1
#include "common/global.h"//c220908_2
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFileInfo>
#include <QDateTime>
#include <QProcess>

#if defined(Q_OS_WIN)//cheon210831-network
//#define print_debug() qDebug() << "\n" << "file_name: " << FILE << "function_name: " << FUNCTION << "line: " << LINE << "\n";
#elif defined(Q_OS_MAC)//cheon210831-network
#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";
#endif
/**
 * @brief MusicFolder_Network::MusicFolder_Network:음악-폴더-네트워크
 * @details 네트워크 목록만 구성한다.
 * @param parent
 */
MusicFolder_Network::MusicFolder_Network(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIcontrol();
}

/**
 * @brief MusicFolder_Network::~MusicFolder_Network : 소멸자
 */
MusicFolder_Network::~MusicFolder_Network(){
    delete findMyIP;
    thread.quit();
    thread.wait();
}

/**
 * @brief MusicFolder_Network::setInit : 초기 세팅
 */
void MusicFolder_Network::setInit(){
    findMyIP= new FindIP;
    this->linker = Linker::getInstance();//c220909_1
}

/**
 * @brief MusicFolder_Network::setUIcontrol : UI 세팅
 */
void MusicFolder_Network::setUIcontrol(){

    boxHost = new QVBoxLayout;
    boxHost->setSpacing(0);
    boxHost->setContentsMargins(0,0,0,0);
    boxHost->setAlignment(Qt::AlignTop);

    wg_contents = new QWidget;
    wg_contents->setContentsMargins(33,10,33,0);
    wg_contents->setLayout(boxHost);


    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(wg_contents);

    setLayout(vl_total);
}

/**
 * @brief MusicFolder_Network::appendHostUI : HOST UI 추가
 * @param p_ip QString 호스트 IP
 * @param p_hostName QString 호스트명
 * @param p_flagOn bool 해당 호스트 접속 가능 여부
 */
void MusicFolder_Network::appendHostUI(const QString &p_ip, const QString &p_hostName, const bool &p_flagOn){//c220906_1

    QString tmp_imgPath = ":/images/icon_network_on.png";

    if(!p_flagOn){
        tmp_imgPath = ":/images/icon_network_off.png";
    }


    QLabel *lb_img = GSCommon::getUILabelImg(tmp_imgPath);
    lb_img->setStyleSheet("background-color:transparent;");

    QLabel *lb_hostName = new QLabel;
    lb_hostName->setText("hostname: "+p_hostName);//c220908_2
    lb_hostName->setFixedWidth(300);
    lb_hostName->setStyleSheet("background-color:transparent;font-size:15px;color:#FFFFFF;");
    lb_hostName->setAlignment(Qt::AlignVCenter);

    QLabel *lb_ip = new QLabel;
    lb_ip->setText(p_ip);
    lb_ip->setStyleSheet("background-color:transparent;font-size:15px;color:#FFFFFF;");
    lb_ip->setAlignment(Qt::AlignVCenter);

    QHBoxLayout *box_item = new QHBoxLayout;
    box_item->setSpacing(15);
    box_item->setContentsMargins(0,0,0,0);
    box_item->addWidget(lb_img);
    box_item->addWidget(lb_hostName);
    box_item->addSpacing(150);
    box_item->addWidget(lb_ip);
    box_item->addStretch(1);

    ClickableWidget *wg_item = new ClickableWidget;
    wg_item->setObjectName("wg_item");
    wg_item->setContentsMargins(0,0,0,0);
    wg_item->setStyleSheet("#wg_item {border-bottom:1px solid #2A2A2A;} #wg_item:hover {border-bottom:1px solid #2A2A2A; background-color:#4D4D4D;}");
    wg_item->setProperty("ip", p_ip);
    wg_item->setProperty("hostName", p_hostName);
    wg_item->setProperty("flagOn", p_flagOn);
    wg_item->setLayout(box_item);
    wg_item->setCursor(Qt::PointingHandCursor);

    boxHost->addWidget(wg_item);
    //wg_contents->adjustSize();

    connect(wg_item, SIGNAL(clickedWidget()), SLOT(slot_clickedHost()));

}

/**
 * @brief MusicFolder_Network::slot_clickedHost:[슬롯] 호스트 클릭
 */
void MusicFolder_Network::slot_clickedHost(){//c220913_1

#if defined(Q_OS_WIN)
    QString tmp_ip = sender()->property("ip").toString();
    QString tmp_hostName = sender()->property("hostName").toString();
    bool tmp_flagOn = sender()->property("flagOn").toBool();

    if(tmp_flagOn){
        emit signal_clickedHost(tmp_ip, tmp_hostName);
    }else{
        // 네트워크 폴더 로그인창
        Dialog::DialogLogin_network *dlg_network = new Dialog::DialogLogin_network(tmp_ip, tmp_hostName,this);
        int result = dlg_network->exec();

        if(result == QDialog::Accepted){
            requestFindIP(true);
        }
    }
#elif defined(Q_OS_MAC)//cheon210831-network
    QString tmp_ip = sender()->property("ip").toString();
    QString tmp_hostName = sender()->property("hostName").toString();
    bool tmp_flagOn = sender()->property("flagOn").toBool();
//print_debug();
    if(tmp_flagOn){
       // print_debug();
        emit signal_clickedHost(tmp_ip, tmp_hostName);
    }else{
        print_debug();
        //dialog_comfirmNetworkfolder_forFinder_add();//c220908_2

        // 네트워크 폴더 로그인창
        Dialog::DialogLogin_network *dlg_network = new Dialog::DialogLogin_network(tmp_ip, tmp_hostName,this);
        int result = dlg_network->exec();

        if(result == QDialog::Accepted){
            qDebug() << "--------009903";
            //requestFindIP(true);

            QProcess* process = new QProcess();
            QStringList sArgs;
            QString cmd = "sh -c \"ls /Volumes\"";
            qDebug() << "cmd=" << cmd;
            process->start(cmd);
            process->waitForFinished(-1);
            QString out = process->readAllStandardOutput();
            QStringList outList = out.split("\n");
            qDebug() << "process->readAllStandardOutput()=" << out;
            qDebug() << "outList=" << outList;
            qDebug() << "process->readAllStandardError()=" <<  process->readAllStandardError();


            dialog_comfirmNetworkfolder_forFinder_add(outList);

        }




    }
#endif
}

void MusicFolder_Network::dialog_comfirmNetworkfolder_forFinder_add(QStringList outList)//c220913_1
{

    DialogConfirm_MacShare *dlgConfirmFinder = new DialogConfirm_MacShare(this);

    dlgConfirmFinder->setTitle(tr("Network Share Folder List"));
    QStringList temp;
    for(int i = 3 ; i < global.standardOutput.size() ; i++){
        temp << QString("%1").arg(global.standardOutput.at(i));
    }
    QString strShare = temp.join(", ");
    print_debug();
    qDebug() << strShare;
    dlgConfirmFinder->setText(QString(tr("If you select the folder list, you can add a network shared folder.\nShare Folder : %1\n")).arg(strShare));//c220913_1

    QJsonArray jsonOldPathArray;
    for(int i = 0 ; i < outList.size() ; i++){
        jsonOldPathArray.append(outList.at(i));
    }

    QJsonObject jsonShare;
    QJsonArray jsonShareArray;
    for(int i = 3 ; i < global.standardOutput.size() ; i++){
        jsonShareArray.append(global.standardOutput.at(i));
    }
    jsonShare.insert("ip", global.standardOutput.at(0));
    jsonShare.insert("id", global.standardOutput.at(1));
    jsonShare.insert("pw", global.standardOutput.at(2));
    jsonShare.insert("shareNameArr", jsonShareArray);
    jsonShare.insert("oldPath", jsonOldPathArray);

    dlgConfirmFinder->setSuggestions(jsonShare);//c220913_3

    //dlgConfirmFinder->setGeometry(this->width()/3,this->height()/5, 350,600);
    int left = global.left_mainwindow+global.width_mainwindow/2- (dlgConfirmFinder->sizeHint().width() / 2);//c220804
    int top = global.top_mainwindow+global.height_mainwindow/2- (dlgConfirmFinder->sizeHint().height() / 2);//c220804
    dlgConfirmFinder->move(left, top);//c220804
    //dlgConfirmFinder->setAlertMode();//c220907_1
    dlgConfirmFinder->setProperty("flagShown",false);
    if(dlgConfirmFinder->property("flagShown").toBool()==false){
        dlgConfirmFinder->setProperty("flagShown",true);

        int result = dlgConfirmFinder->exec();

        if(result == QDialog::Accepted){

            print_debug();
            //emit linker->signal_goFolder("folder");//c220913_1
        }
    }
    delete dlgConfirmFinder;
}

/**
 * @brief MusicFolder_Network::findedIP : [슬롯] SMBIP FIND
 * @details 네트워크 가능 IP, HOST를 UI로 그린다.
 * @note 로컬 아이피는 제외한다.
 * @param p_uuid qint64, SMB FIND 요청 아이디
 * @param p_ip QString finded ip
 * @param p_hostName QString finded hostname
 */
void MusicFolder_Network::findedIP(const qint64 &p_uuid,const QString &p_ip, const QString &p_hostName, const bool &flagOn){

    if(p_uuid == reqUUID && p_ip != findMyIP->getMyIP()){
        appendHostUI(p_ip, p_hostName, flagOn);
    }
}


/**
 * @brief MusicFolder_Network::requestFindIP
 * @details 로컬 아이피를 이용해 연결가능한 네트워크 아이피를 찾는다.
 * @note 연속으로 요청한 경우 기 요청을 해제하지 않음.
 * @see @link FindSMBIP @endlink
 */
void MusicFolder_Network::requestFindIP(bool flagRefresh){

    if(boxHost->count() ==0 || flagRefresh){
        reqUUID = QDateTime::currentSecsSinceEpoch();
        GSCommon::clearLayout(boxHost);

        FindSMBIP *findSMB = new FindSMBIP;
        findSMB->setIP(reqUUID, findMyIP->getMyIP());
        findSMB->moveToThread(&thread);
        thread.start();

        connect(findSMB, SIGNAL(signal_findedSMBIP(qint64,QString, QString, bool)), SLOT(findedIP(qint64,QString, QString, bool)));//c220909_3
        QObject::connect(findSMB, &FindSMBIP::signal_startFind, findSMB, &FindSMBIP::findNetworkHost);
        QObject::connect(&thread, &QThread::finished, findSMB, &FindSMBIP::deleteLater);
        QObject::connect(findSMB, &FindSMBIP::signal_endedFind, [&](){
            thread.quit();
        });

        emit findSMB->signal_startFind();
    }
}
