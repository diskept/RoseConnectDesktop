#include "home/homemain.h"

#include "mainwindow.h"

#include "home/ReplyTimeout.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"
#include "common/rosesettings.h"
#include "common/sqlitehelper.h"
#include "common/sqliteviewtotable.h"

#include "data/datauser_forRoseRadio.h"

#include "music/musichome.h"
//#include "tidal/tidalmain_test.h"
#include "tidal/ProcCommon.h"
#include "tidal/ProcRosePlay_withTidal.h"
#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"
#include "qobuz/ProcCommon_forQobuz.h"
#include "qobuz/ProcRosePlay_withQobuz.h"
#include "apple/ProcCommon_forApple.h"
#include "apple/ProcRosePlay_withApple.h"

#include "roseHome/ProcCommon_forRosehome.h"
#include "roseRadio/roseradio_struct.h"

#include "cdplay/CDplay_struct.h"//c220721

#include "widget/downloadprogress.h"
#include "widget/toastmsg.h"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <QSettings>
#include <QDesktopWidget>
#include <QPixmapCache>
#include <QMainWindow>//cheon211008

const int FOOTER_H = 95;//c220526
const int REMOTE_W = 953;
const int REMOTE_H = 806;   //767;
const int TOP_NAVIBAR_H = 70;
const int QUEUEPLAY_W = 550;
const int QUEUEPLAY_H = 900;
const int VOLUMN_W = 90;
const int VOLUMN_H = 556;

const int HTTP_DEVICE_CONNECTED = 100;
const int HTTP_DEVICE_USER_INFO = 99;
const int HTTP_DEVICE_GET_INFO = 98;
const int HTTP_DEVICE_GET_VOLUMN = 97;
const int HTTP_AUTO_LOGIN = 97;
const int HTTP_AUTO_SESSION = 95;
const int HTTP_SET_ROSE_USERINFO = 94;
const int HTTP_GET_MYINFO = 93;
const int HTTP_ROSEFM_COUNTRY_GET = 92;
const int HTTP_ROSEFM_CHANNEL_GET = 91;
const int HTTP_SHAZAM_INFO = 90;

const int HTTP_DEVICE_HDMI_GET = 101;
const int HTTP_DEVICE_TIMER_GET = 104;
const int HTTP_NETWORK_PLAY = 105;//cheon210817-iso

const QString tmp_btnStyle      = "padding:10px;border:1px solid #707070;color:#CCCCCC;font-size:18px;";//cheon211008
const QString tmp_btnStyleHover = "background-color:#B18658;color:#FFFFFF;";//cheon211114-01


/**
 * @brief homeMain::homeMain : Application home
 * @param parent QWidget
 */
HomeMain::HomeMain(QWidget *parent)
    : QWidget(parent)
{

    //global.dialog_delay = new QDialog(parent);//c230317


    //c220408  start (to 77 line)
    print_debug();
    //global.dialog_delay = new QDialog(this);//c230331
    QString debugCHK = rosesettings.APPLICATION_VERSION;
    qDebug() << "debugCHK = " << debugCHK;
    QStringList debuglist = debugCHK.split(".");
    global.debugCHK = debuglist.count();

    global.legacy_v1 = (global.debugCHK > 3) ? "https://dev.api.roseaudio.kr/v1" : "https://api.roseaudio.kr/v1";
    global.newpipe_v1 = (global.debugCHK > 3) ? "https://dev.api.roseaudio.kr/newpipe/v1" : "https://api.roseaudio.kr/newpipe/v1";
    global.legacy_radio = (global.debugCHK > 3) ? "https://dev.api.roseaudio.kr/radio/v2" : "https://api.roseaudio.kr/radio/v2";
    global.rosehome_userpick = (global.debugCHK > 3) ? "https://dev.api.roseaudio.kr/v1/member/playlist/userpick/all" : "https://api.roseaudio.kr/v1/member/playlist/userpick/all";

    global.dialog_delay = nullptr;//c230305

    this->flagRoseRadio = false;

    //setUIControl();
    if(global.device.getDeviceIP().isEmpty() == true){
        //slot_showDlgOfDisconnect();
        print_debug();
        //QTimer::singleShot(10000, this, SLOT(slot_showDlgOfDisconnect()));//c221004_3
    }
    else{//c230302_3
        this->requestGetRoseUserInfo();    //c230302_3     // Rose Login check
        this->requestGetShazamInfo();    //c230302_3        // Rose External Token check
/*//c230304
        this->checkTidalLogin();      //          // Tidal login check -> added by sunnyfish -> fixed by jeon
        this->checkBugsLogin();       //          // Bugs Login check -> added by jeon
        this->checkQobuzLogin();       //         // Qobuz Login check -> added by jeon
        this->checkAppleLogin();       //         // Apple Musci Login check -> added by jeon
*/
    }

    global.media.clear();//c230516
    global.mediaTypeList.clear();//c230516
    global.media << "" << "UNITED" << "MUSIC" << "YOUTUBE" << "TIDAL" << "BUGS" << "QOBUZ" << "APPLE_MUSIC";//c230516
    global.mediaTypeList << tr("All Music") << tr("Mix") << tr("Music") << tr("RoseTube") << tr("Tidal") << tr("Bugs") << tr("Qobuz") << tr("Apple Music"); //c230516     // << "VIDEO" << "SPOTIFY";

    setInit();//
    setUIControl();//

    if(global.device.getDeviceIP().isEmpty() == true){//c230304
        print_debug();
    }
    else{//c230424
        print_debug();

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//

        global.user_forTidal.setLogout();
        global.user_forTidal.set_logoutState();

        print_debug();
        tidal::ProcCommon *proc_dbClear1 = new tidal::ProcCommon(this);
        proc_dbClear1->clearTablesInfo_tidalDB();
        print_debug();

        print_debug();this->checkTidalLogin();
        global.user_forBugs.set_logoutState();

        bugs::ProcBugsAPI *proc_dbClear2 = new bugs::ProcBugsAPI(this);
        proc_dbClear2->clearTablesInfo_bugsDB();

        this->checkBugsLogin();
        global.user_forQobuz.setLogout();

        qobuz::ProcCommon *proc_dbClear3 = new qobuz::ProcCommon(this);
        proc_dbClear3->clearTablesInfo_qobuzDB();

        this->checkQobuzLogin();


        /*
        print_debug();this->checkTidalLogin();      //          // Tidal login check -> added by sunnyfish -> fixed by jeon
        this->checkBugsLogin();       //          // Bugs Login check -> added by jeon
        this->checkQobuzLogin();       //         // Qobuz Login check -> added by jeon
        this->checkAppleLogin();       //         // Apple Musci Login check -> added by jeon
        */

    }
    this->queueplaylist = new QueuePlayList(this);//

    global.enable_section_left = true;//c230308_3


    this->sectionLeft->clickedMenu(QString(GSCommon::MainMenuCode::RoseHome));//


}


/**
 * @brief homeMain::~homeMain : 소멸자
 */
HomeMain::~HomeMain(){//c230302_1

    print_debug();
    global.music_player->stop();
    global.abs_ani_dialog_wait->close();//c230215
    delete findIp;

    this->fileDownLoader->deleteLater();

    this->deleteLater();
    qApp->exit();//
}


/**
 * @brief HomeMain::setInit : 초기 세팅
 */
void HomeMain::setInit(){

    findIp = new FindIP;
    linker = Linker::getInstance();

    connect(linker, SIGNAL(signal_httpservermsg(QJsonObject)), this, SLOT(slot_httpservermsg(QJsonObject)));//cheon211206
    connect(linker, SIGNAL(signal_queuelist_mouse_trigger_menu_flag()), this, SLOT(slot_queue_hide()));//cheon211206//c220418

    connect(linker, SIGNAL(signal_requestDownloadDB()), this, SLOT(slot_startDownloadDB()));
    connect(linker, SIGNAL(signal_logined()), SLOT(slot_changedLoginInfo()));
    connect(linker, SIGNAL(signal_devicePowerChanged()), SLOT(slot_changedDevicePower()));
    connect(linker, SIGNAL(signal_search(QString)), this, SLOT(slot_search5555(QString)));//c220904_1
    connect(linker, SIGNAL(signal_searchSuggestionsChanged(QJsonObject)), this, SLOT(slot_searchSuggestionsChanged(QJsonObject)));
    connect(linker, SIGNAL(signal_searchBarFocusChanged(bool)), SLOT(slot_searchBarFocusChanged(bool)));
    connect(linker, SIGNAL(signal_RoseHome_movePage(QString)), this, SLOT(slot_MovePageRoseHomeTo(QString)));

    //connect(linker, SIGNAL(signal_completeDownloadDB()), SLOT(slot_changedPageSetting_InputOutput()));//c220521

    connect(linker, SIGNAL(signal_device_connect_check()), SLOT(slot_showDlgOfDisconnect_hide()));//c221004_2
    connect(linker, SIGNAL(signal_device_connect_check_error()), SLOT(slot_showDlgOfDisconnect_call()));//c220722

    connect(linker, SIGNAL(signal_clickedRemoteEXTE()), this, SLOT(slot_changedPageSetting_InputOutput()));//c220521

    connect(linker, SIGNAL(signal_clickedMovePageMusicSearch()), this, SLOT(slot_clickedMovePageMusicSearch()));
    connect(linker, SIGNAL(signal_clickedMovePageVideoSearch()), this, SLOT(slot_clickedMovePageVideoSearch()));
    connect(linker, SIGNAL(signal_clickedMovePageRadioSearch()), this, SLOT(slot_clickedMovePageRadioSearch()));
    connect(linker, SIGNAL(signal_clickedMovePageRoseRadioSearch()), this, SLOT(slot_clickedMovePageRoseRadioSearch()));
    connect(linker, SIGNAL(signal_clickedMovePageRoseTubeSearch()), this, SLOT(slot_clickedMoveRoseTubeSearch()));
    connect(linker, SIGNAL(signal_clickedMovePagePodCastSearch()), this, SLOT(slot_clickedMovePagePodCastSearch()));

    connect(linker, SIGNAL(signal_clickedMovePageTidalSearch()), this, SLOT(slot_clickedMovePageTidalSearch()));
    connect(linker, SIGNAL(signal_clickedMovePageBugsSearch()), this, SLOT(slot_clickedMovePageBugsSearch()));
    connect(linker, SIGNAL(signal_clickedMovePageQobuzSearch()), this, SLOT(slot_clickedMovePageQobuzSearch()));

    connect(linker, SIGNAL(signal_checknetwork()), this, SLOT(slot_rebooting()));//c220624_1
    connect(linker, SIGNAL(signal_musicDbProcess()), this, SLOT(slot_process_notice()));
    connect(linker, SIGNAL(signal_connect_reset_notice()), this, SLOT(slot_connect_reset_notice()));//c230326

    connect(linker, SIGNAL(signal_successLoginQobuz()), this, SLOT(slot_completeReq_getqLoginInfoQobuz()));//c230422


    connect(linker, SIGNAL(signal_clickedMovePageSearch_share(QString)), this, SLOT(slot_clickedMovePageSearch_share(QString)));//c220818

    //global.abs_ani_dialog_wait = new QDialog(this) ;//c230323
    //c221006_1
    //  print_debug();
    //  QJsonObject tmp_json;
    //  NetworkHttp *network = new NetworkHttp;
    //  connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_deviceIOState(int, QJsonObject)));
    //  network->request(3333, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("in.out.mode.get"), tmp_json, true);
    //c221006_1



    /*  //c220705
    Dt_thread = new DesktopMediaPlayerThread("", this);
    connect(Dt_thread, &DesktopMediaPlayerThread::finished, Dt_thread, &QObject::deleteLater);
    connect(Dt_thread, SIGNAL(signal_stopThread()), this, SLOT(slot_player_valuestop()));
#if 0
    //DESKTOP_MODE  //c220705
    Dt_thread->start();
#endif
*/

}

void HomeMain::slot_process_notice(){//c230326
    print_debug();
    QTimer *timer = new QTimer();//c230423
    timer->setInterval(1000); // 1초 1000 -> 500
    connect(timer, SIGNAL(timeout()), SLOT(slot_musicDbdownloadingValue()));
    timer->start();
    //this->progress_pop->show();
    //this->progress_pop->hideText();
    this->progress->hide();//c230409
    dlgConfirmMusicDbDownloadingNotice = new DialogConfirm(this);
    dlgConfirmMusicDbDownloadingNotice->setAlignment(Qt::AlignLeft);
    dlgConfirmMusicDbDownloadingNotice->setTitle(tr("Music DB file Notice!"));
    dlgConfirmMusicDbDownloadingNotice->setText(QString(tr("DB is downloading from Rose-device. Please wait. \nDuring downloading,\n use other menus besides the music and video menus.\n     - Current progress :     %1 %")).arg(global.musicDB_DownloadingState));

    dlgConfirmMusicDbDownloadingNotice->setTextHeight(200);
  /*  QVBoxLayout *vl_progress = new QVBoxLayout();
    vl_progress->setSpacing(30);
    vl_progress->setContentsMargins(0, 0, 0, 0);
    vl_progress->addWidget(this->progress_pop);
*/
    int left = global.left_mainwindow + ((global.width_mainwindow - 350) / 2);
    int top = global.top_mainwindow + ((global.height_mainwindow - 400) / 2);

    dlgConfirmMusicDbDownloadingNotice->setGeometry(left, top, 350, 600);
    dlgConfirmMusicDbDownloadingNotice->setAlertMode();

    //dlgConfirmMusicDbDownloadingNotice->setLayoutMy_withText(vl_progress);
    //this->progress_pop->raise();
    if(dlgConfirmMusicDbDownloadingNotice->property("flagShown").toBool()==false){
        dlgConfirmMusicDbDownloadingNotice->setProperty("flagShown",true);

        int result = dlgConfirmMusicDbDownloadingNotice->exec();
        timer->stop();//c230423
        timer->deleteLater();//c230423
        dlgConfirmMusicDbDownloadingNotice->setProperty("flagShown",false);
        if(result == QDialog::Accepted){
            dlgConfirmMusicDbDownloadingNotice->hide();
            this->activateWindow();
            this->raise();
            print_debug();
        }
    }
}

void HomeMain::slot_musicDbdownloadingValue(){
    dlgConfirmMusicDbDownloadingNotice->setText(QString(tr("DB is downloading from Rose-device. Please wait. \nDuring downloading,\n use other menus besides the music and video menus.\n     - Current progress :     %1 %")).arg(global.musicDB_DownloadingState));

}

void HomeMain::slot_responseHttp_deviceIOState(const int &p_id, const QJsonObject &p_jsonObject){

    QString deviceType = global.device.getDeviceType();
    print_debug();
    qDebug() << "requestInputOutputMode()start-deviceType : " << deviceType  << ", global.device.getDeviceIP() : " << global.device.getDeviceIP();
    QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "slot_responseHttp =: " << strJson;//cheon12_io
    if(p_jsonObject.contains("outputMode")){
        global.device_outputMode = p_jsonObject["outputMode"].toInt(); //cheon09_io
        qDebug() << "global.device_outputMode : " << global.device_outputMode;
    }

}

void HomeMain::slot_rebooting(){//c220624

    print_debug();
    qApp->exit( MainWindow::EXIT_CODE_REBOOT );
}


void HomeMain::slot_connectDevice(){//c220525

    //print_debug();

    //emit linker->signal_checkQueue(28, "");//c220525

    QNetworkAccessManager * networkAccessManager = new QNetworkAccessManager(this);

    connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotReadyRead_connect(QNetworkReply*)));
    QNetworkRequest * networkRequest = new QNetworkRequest(QUrl(QString("http://%1:%2").arg(global.device.getDeviceIP()).arg(global.port)));
    //Getting the URL
    QNetworkReply * networkReply = networkAccessManager->get(*networkRequest);
    //ReplyTimeout::set(networkReply, 3000);
    // qDebug() << "networkAccessManager-timeout =" << networkReply->set;
    connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(printError(QNetworkReply::NetworkError)));
}


void HomeMain::slot_showDlgOfDisconnect_hide(){//c221004_2

    //print_debug();
    if(dlgConfirm != nullptr){
        //print_debug();
        if(dlgConfirm->isHidden() == false){
            //print_debug();

            dlgConfirm->hide();

        }else{
            //print_debug()
            if(global.user.getAccess_token().isEmpty()){
                //print_debug();
                //if(!this->reboot_flag){
                // this->reboot_flag = true;
                global.user.setDataUser(QJsonObject());
                // qApp->exit( MainWindow::EXIT_CODE_REBOOT );
                //}

            }
        }
    }

}


void HomeMain::slot_showDlgOfDisconnect_call(){//c221006_1

    print_debug();
    if(global.queuelist_mouse_trigger_menu_flag){
        print_debug();
        connectDevice(global.device.getDeviceIP());
    }

    QTimer::singleShot(3000, this, SLOT(slot_showDlgOfDisconnect()));//c221006_1
}


/**
 * @brief HomeMain::slot_showDlgOfDisconnect
 */
void HomeMain::slot_showDlgOfDisconnect(){//c220624

    print_debug();
    dlgConfirm->setText(QString(tr("Plase activate the network connection of the purchased ROSE device.\n\n- Rose Device Name : %1\n- Rose Device IP : %2\n- My Desktop IP : %3")).arg(global.device.getDeviceName()).arg(global.device.getDeviceIP()).arg(findIp->getMyIP()));//c220609

    if(this->remoteWidget->isHidden() != true){
        print_debug();
        showDlgOfDisconnect_flag = true;
    }
    else{
        print_debug();
        showDlgOfDisconnect_flag = false;
    }
    qDebug() << "showDlgOfDisconnect_flag= " << showDlgOfDisconnect_flag;
    //qDebug() << "dlgConfirm->isHidden()= " << dlgConfirm->isHidden();
    if(dlgConfirm->isHidden() == true && showDlgOfDisconnect_flag == false){
        //dlgConfirm->setProperty("flagShown", true);
        //print_debug();

        int result = dlgConfirm->exec();
        dlgConfirm->raise();
        if(result == QDialog::Accepted){
            //print_debug();
            showRemote(true);
            if(this->hlayout_content->count() >0){
                // print_debug();
                QWidget *w_currMainMenu = this->hlayout_content->itemAt(0)->widget();
                dynamic_cast<AbstractMainContent*>(w_currMainMenu)->showhideRemoteWidget(true);
                showDlgOfDisconnect_flag = true;
                //print_debug();
            }
        }else if(result == QDialog::Rejected){
            print_debug();
        }
    }
    else{

    }
}


void HomeMain::printError(QNetworkReply::NetworkError code){

    print_debug();
    //dlgConfirm->setProperty("flagShown", false);
    //slot_showDlgOfDisconnect();
    qDebug() << "dlgConfirm->property(flagShown).toBool() = " << dlgConfirm->property("flagShown").toBool();
    QTimer::singleShot(1000, this, SLOT(slot_showDlgOfDisconnect()));//c220601
    qDebug() << "Network Error Code: " << code;
    //qDebug() << networkReply->errorString();
    sender()->deleteLater();//c220526
}


/**
 * @brief HomeMain::connectDevice : 로즈 연결 API
 */
void HomeMain::connectDevice(QString p_ip){//c220521

    //print_debug();
    if(!p_ip.isEmpty()){
        // 로즈 연결 : 기기정보 가져오기
        //print_debug();qDebug() << "p_ip =" << p_ip;
        //dlgConfirm->setProperty("flagShown", false);



        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        QJsonObject tmp_json;
        tmp_json.insert("connectIP", findIp->getMyIP());
        network->request(HTTP_DEVICE_CONNECTED, QString("http://%1:%2/device_connected")
                         .arg(p_ip).arg(global.port), tmp_json, true);
    }
    else{
        print_debug();
        //dlgConfirm->setProperty("flagShown", false);
        //slot_showDlgOfDisconnect();
        QTimer::singleShot(1000, this, SLOT(slot_showDlgOfDisconnect()));//c220601
    }
}


void HomeMain::slotReadyRead_connect(QNetworkReply *reply){

    print_debug();
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();
    qDebug() << "status = " << status;
    qDebug() << "dlgConfirm->property(flagShown).toBool() = " << dlgConfirm->property("flagShown").toBool();
    if(status != 200){

        global.signal_device_connect_flag =  false;
    }else{
        global.signal_device_connect_flag = true;
        //if(global.device.getDeviceIP().isEmpty()
        //qApp->exit( MainWindow::EXIT_CODE_REBOOT );//c220624
        connectDevice(global.device.getDeviceIP());//c220624
    }

    sender()->deleteLater();//c220526
}


/**
 * @brief HomeMain::mousePressEvent 마우스 클릭시
 * @param event
 */
void HomeMain::mousePressEvent(QMouseEvent *event){

    Q_UNUSED(event);
    // 띄워진 객체 모두 hide
    print_debug();

    this->unRaiseAllWidget();
    emit linker->signal_clickListBtn();//c230331
}


/**
 * @brief HomeMain::unRaiseAllWidget 현재 띄워진 모든 위젯 내리기.. (리모콘 위젯과 큐 위젯 hide 하기)
 * @param p_unRaiseRemote 리모콘위젯 내릴지유무 (디폴트:true)
 * @param p_unRaiseQueue 큐 재생목록 내릴지유무 (디폴트:true)
 * @param p_unRaiseVolumn 볼륨조절컨트롤 내릴지유무 (디폴트:true)
 */
void HomeMain::unRaiseAllWidget(bool p_unRaiseRemote, bool p_unRaiseQueue, bool p_unRaiseVolumn){

    // ---------------------------------
    //  검색 제안어 hide
    // ---------------------------------
    if(this->suggestionBar!=nullptr){
        this->suggestionBar->hide();
    }

    if(p_unRaiseRemote){
        if(this->remoteWidget!=nullptr){
            this->remoteWidget->hide();

            // 입출력 페이지 변경
            if(this->hlayout_content->count() > 0){
                QWidget *w_currMainMenu = this->hlayout_content->itemAt(0)->widget();
                dynamic_cast<AbstractMainContent*>(w_currMainMenu)->showhideRemoteWidget(false);
            }
        }
    }
    if(p_unRaiseQueue){
        if(this->queueplaylist!=nullptr){
            // 하단 PlayBar 관련 초기화
            global.queueTimerFlag = false;//c211213
            this->queueplaylist->hide();
            this->sectionBottom->setInitUIQueueIcon();
        }
    }
    if(p_unRaiseVolumn){
        if(this->volumnControl!=nullptr){
            this->volumnControl->hide();
            this->sectionBottom->setInitUIVolumnIcon();
        }
    }
}


/**
 * @brief HomeMain::setUIControl : UI 세팅
 */
void HomeMain::setUIControl(){

    ToastMsg::delay(this,"", tr("delay"), 500);//c230430
    // 메인 레이아웃 구성
    this->sectionLeft = new SectionLeft(this);

    this->sectionBottom = new SectionBottom(this);
    this->sectionBottom->setFixedHeight(FOOTER_H);

    this->hlayout_content = new QHBoxLayout();
    hlayout_content->setContentsMargins(0,0,0,0);
    hlayout_content->setSpacing(0);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0,0,0,0);
    hlayout->setSpacing(0);
    hlayout->addWidget(this->sectionLeft);
    hlayout->addLayout(this->hlayout_content);

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(this->sectionBottom);

    this->setLayout(vlayout);
    //global.dialog_delay = new QDialog();//c230430

    delay_ani_init();//c230302_1
print_debug();
    // 리모콘 UI
    this->remoteWidget = new RemoteWidget(this);
    this->remoteWidget->setStyleSheet("background-color:#333333; border-top-left-radius:20px; border-bottom-left-radius:20px;");

    // 검색 suggestion bar
    this->suggestionBar = new SuggestionsBar(this);
    this->suggestionBar->hide();

    //settingTrackOption = new SettingTrackOption();
    //this->settingTrackOption->getSettingInfo();
    // 재생목록 큐
    //this->queueplaylist = new QueuePlayList(this);//c230303
    //this->queueplaylist_copy = new QueuePlayList(this);
    this->volumnControl = new VolumnControl(this);

    // DB 다운로드 프로그레스
    this->progress = new DownloadProgress(100, this);
    global.pr = this->progress;//c230415
    //this->progress->setGeometry(400,7,500,55);
    this->progress->hide();

    //this->progress_pop = new DownloadProgress(100, this);
    //this->progress->setGeometry(400,7,500,55);
    //this->progress_pop->hide();


    // 커넥트 관련
    connect(this->sectionLeft, SIGNAL(changedMenu(QString)), this, SLOT(changedLeftMainMenu(QString)));
    connect(this->sectionLeft, SIGNAL(signal_goPagePlayListDetail(QJsonObject)), this, SLOT(slot_changePagePlayList(QJsonObject)));
    connect(this->sectionBottom, SIGNAL(clickedQueuePlayList(bool)), this, SLOT(clickedBottomQueueIcon(bool)));
    connect(this->sectionBottom, SIGNAL(clickedVolumn(bool)), this, SLOT(slot_clickedBottomVolumn(bool)));
    connect(this->sectionBottom, SIGNAL(signal_showPlayFullScreen(QJsonObject)), this, SLOT(slot_showPlayFullScreen(QJsonObject)));
    connect(this->sectionBottom, SIGNAL(signal_changedCurrPlayTrack(QJsonObject)), this, SLOT(slot_changedCurrPlayTrack(QJsonObject)));
    connect(this->sectionBottom, SIGNAL(signal_changedCurrHDMI(bool)), this, SLOT(slot_changedCurrHDMI(bool)));
    connect(this->remoteWidget, SIGNAL(clickedRemoteVU()), this, SLOT(slot_changedPageSetting_VU()));
    connect(this->remoteWidget, SIGNAL(clickedRemoteEXTE()), this, SLOT(slot_changedPageSetting_InputOutput()));
    connect(this->remoteWidget, SIGNAL(clickedRemoteEXTH()), this, SLOT(slot_changedPageSetting_HDMI()));   //cheon01
    connect(this->remoteWidget, SIGNAL(clickedRemoteEXTT()), this, SLOT(slot_changedPageSetting_Timer()));  //cheon01
    connect(this->remoteWidget, SIGNAL(connectNewDevice(QString)), this, SLOT(connectDevice(QString)));
    connect(this->progress, SIGNAL(signal_clickedRefresh()), this, SLOT(slot_clickedDBRefreshBtn()));
    //connect(this->progress_pop, SIGNAL(signal_clickedRefresh()), this, SLOT(slot_clickedDBRefreshBtn()));

    connect(linker, SIGNAL(signal_clickListBtn()), this, SLOT(slot_searchBarClear()));//c220628
    connect(linker, SIGNAL(clickedQueuePlayListHide(bool)), this, SLOT(clickedBottomQueueIconHide(bool)));//c230428


    // -------------------------------------
    //  기기연결 실패 임시 Dialog :: 추후 화면단 작업
    // -------------------------------------
    dlgConfirm = new DialogConfirm(this);
    dlgConfirm->setAlertMode();
    dlgConfirm->setProperty("flagShown", false);
    dlgConfirm->setTitle(tr("ROSE equipment connection failure"));
    dlgConfirm->setText(QString(tr("Plase activate the network connection of the purchased ROSE device.\n\n- Rose Device Name : %1\n- Rose Device IP : %2\n- My Desktop IP : %3")).arg(global.device.getDeviceName()).arg(global.device.getDeviceIP()).arg(findIp->getMyIP()));//c220609
    dlgConfirm->setTextHeight(250);//c230409

    //Dt_thread = new DesktopMediaPlayerThread(p_fileInfo_filePath, this);

    global.music_player = new QMediaPlayer;//c220609
    audioOutput = new QAudioOutput;
    //global.music_player->setAudioOutput(audioOutput);
    global.music_playlist = new QMediaPlaylist;//c220609
    global.music_playlist->setPlaybackMode(QMediaPlaylist::Loop);//c220609
    global.music_player->setPlaylist(global.music_playlist);//c220609
    //c220609  start

    /*
    connect(global.music_player, &QMediaPlayer::stateChanged, [](QMediaPlayer::State state) {
            print_debug();
            qDebug()<<"state---="<< state;
            qDebug()<<"global.repeatMode---="<< global.repeatMode;
            if (global.repeatMode == 2 && state == QMediaPlayer::State::StoppedState)
            {
                //global.music_player->play();
               // global.music_playlist->setPlaybackMode(QMediaPlaylist::Loop);//c220609
                global.music_playlist->next();
                global.music_player->play();
            }else if (global.repeatMode == 0 && state == QMediaPlayer::State::StoppedState){
                //global.music_playlist->setPlaybackMode(QMediaPlaylist::Sequential);//c220609
                //global.music_playlist->next();
                global.music_player->play();
            }else if (global.repeatMode == 1 && state == QMediaPlayer::State::StoppedState){
                //global.music_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);//c220609
                //global.music_playlist->next();
                global.music_player->play();
            }

            });
*/

    //global.abs_ani_dialog_wait = new QDialog(this) ;//c220616

    //c220609   end
    //dlgConfirmReadmeNotice = new DialogConfirm(this);
    //dlgConfirmReadmeNotice->setFixedWidth(800);
    //dlgConfirmReadmeNotice->setFixedHeight(1100);
}


void HomeMain::delay_ani_init(){//c230302_1
print_debug();
    int left = 0;
    int top = 0;
    global.abs_ani_dialog_wait = new QDialog(); //c230409
    //----------------------------------------------------
    //QMovie *mov = new QMovie("C:/Users/doulz/Rose_Connect-21-12-21-20/images/Spinner-4.2s-200px.gif");
    //abs_ani_mov = new QMovie(":/images/loading.gif");//":/images/Spinner-4.2s-200px.gif"

    QMovie *abs_ani_mov = new QMovie(":/images/Spinner-4.2s-200px.gif");//":/images/Spinner-4.2s-200px.gif"//c220616
    abs_ani_mov->setScaledSize(QSize(120, 120));
    abs_ani_mov->setBackgroundColor("transparent");

    QLabel *lb_Movie = new QLabel();
    lb_Movie->setStyleSheet("background-color:transparent;");
    lb_Movie->setMovie(abs_ani_mov);
    //lb_Movie->setGeometry(20, 20, 392, 225);
print_debug();
    QHBoxLayout *hl_msgBox = new QHBoxLayout();
    hl_msgBox->setContentsMargins(0, 0, 0, 0); //hl_msgBox->setContentsMargins(35,20,35,15);
    hl_msgBox->setSpacing(0);
    hl_msgBox->addWidget(lb_Movie);

    global.abs_ani_dialog_wait->setLayout(hl_msgBox);
    global.abs_ani_dialog_wait->setModal(true);
    global.abs_ani_dialog_wait->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    global.abs_ani_dialog_wait->setAttribute(Qt::WA_TranslucentBackground);
print_debug();
    /*QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
    int latestWidth = settings->value(rosesettings.SETTINGS_W, 0).toInt();
    int latestHeight = settings->value(rosesettings.SETTINGS_H, 800).toInt();*/

    left = global.left_mainwindow + ((global.width_mainwindow - 120) / 2);
    top = global.top_mainwindow + ((global.height_mainwindow - 120) / 2);

    global.abs_ani_dialog_wait->move(left, top);//c220804
    //global.abs_ani_dialog_wait->hide();//c230426

    abs_ani_mov->start();
    global.abs_ani_dialog_wait->hide();//c230426
print_debug();
    /*print_debug();
    //if(global.abs_ani_dialog_wait->layout()==0){//c220616
        print_debug();
        global.abs_ani_dialog_wait->setLayout(hl_msgBox);
        print_debug();
        global.abs_ani_dialog_wait->setModal(true);
        global.abs_ani_dialog_wait->setWindowFlags( Qt::Tool | Qt::FramelessWindowHint );
        global.abs_ani_dialog_wait->setAttribute(Qt::WA_TranslucentBackground);
print_debug();

        left = global.left_mainwindow + ((latestWidth - 120) / 2);
        top = global.top_mainwindow + ((latestHeight - 120) / 2);

        global.abs_ani_dialog_wait->move(left, top);//c220804

        //global.abs_ani_dialog_wait->setGeometry(left, top, 0, 0);
        //return;
    //}else{
     //   global.abs_ani_dialog_wait->move(left, top);
        global.abs_ani_dialog_wait->hide();
    //}
print_debug();


    abs_ani_mov->start();
    print_debug();*/
}


void HomeMain::slot_searchBarClear(){//c220628

    this->suggestionBar->hide();
}



/*
void HomeMain::nextSongPlay(){//c220705

    print_debug();
}
*/


void HomeMain::slot_queue_hide(){
    print_debug();
    //ToastMsg::delay(this,"", tr("delay"), 2000);//c230326
    emit linker->signal_checkQueue(14, "");//c221115
    if(!this->queueplaylist->isHidden()){
        print_debug();
        global.queueTimerFlag = false;//c211213
        this->queueplaylist->hide();
        this->unRaiseAllWidget(true, false, true);

    }else{
        print_debug();
    }
}


void HomeMain::slot_httpservermsg_menu_flag(){//c220418

    //this->queueplaylist->hide();
}

void HomeMain::slot_connect_reset_notice(){//c230326
    print_debug();

    if( dlgConfirmConnect_resetNotice ==  nullptr){
        dlgConfirmConnect_resetNotice = new DialogConfirm(this);//c230326
    }
    if( dlgConfirmConnect_resetNotice !=  nullptr){
        dlgConfirmConnect_resetNotice->setAlignment(Qt::AlignLeft);
        dlgConfirmConnect_resetNotice->setTitle(tr("Music Storage divice Notice!"));
        dlgConfirmConnect_resetNotice->setText(QString(tr("The current song cannot be played.\n The music storage device's network connection have been reset.\n Connect the network folder in the music menu.")));

        dlgConfirmConnect_resetNotice->setTextHeight(200);

        int left = global.left_mainwindow + ((global.width_mainwindow - 350) / 2);
        int top = global.top_mainwindow + ((global.height_mainwindow - 400) / 2);

        dlgConfirmConnect_resetNotice->setGeometry(left, top, 350, 400);
        dlgConfirmConnect_resetNotice->setAlertMode();
        //dlgConfirmMusicDbDownloadingNotice->setLayoutMy_withText(vl_progress);
        //this->progress_pop->raise();
        if(dlgConfirmConnect_resetNotice->property("flagShown").toBool()==false){
            dlgConfirmConnect_resetNotice->setProperty("flagShown",true);

            int result = dlgConfirmConnect_resetNotice->exec();

            dlgConfirmConnect_resetNotice->setProperty("flagShown",false);
            if(result == QDialog::Accepted){
                dlgConfirmConnect_resetNotice->hide();
                print_debug();
            }

        }
    }
}

void HomeMain::slot_httpservermsg(const QJsonObject &p_jsonObject){         //c220402

    print_debug();
    QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"MediaFileTree::playMusic---" << strJson;

    if(p_jsonObject.contains("code")){//cheon210812-iso
        QString tmp_code = p_jsonObject["code"].toString();
        bool tmp_codeOk = p_jsonObject["flagOk"].toBool();
        if(tmp_code =="CDPLAY" && tmp_codeOk){
            //qDebug() << "tmp_code-CDPLAY---";

            global.isoflag = true;
            return;
        }else{
            global.isoflag = false;
            //qDebug() << "tmp_code--" << tmp_code;
        }

    }else{
        global.isoflag = false;
    }

    if(p_jsonObject.contains("dataObj")){//c211213

        qDebug() << "global.device.CurrPlayType=" << global.device.CurrPlayType;
        QJsonObject tmp_Json = p_jsonObject["dataObj"].toObject();
        if(global.device.CurrPlayType !="CD"){
            print_debug();
            QString temp = tmp_Json["message"].toString();//cheon211102
            qDebug() << "Rose_Connect : " << temp;//cheon211102
            if(temp == "AVERROR_HTTP_NOT_FOUND" || temp == "AVERROR_HTTP_REQUEST_TIMEOUT" || temp == "AVERROR_HTTP_SERVICE_UNAVAILABLE")
                ;
            else
                ;
        }else{
            QJsonObject tmp_json;
            qDebug() << "*cdplaying.....................stop cmd executeed" ;
            //tmp_json.insert("roseToken",global.device.getDeviceRoseToken());

            //QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"MediaFileTree::playMusic---" << strJson;
            //QJsonObject tmp_json;

            //this->dialog_comfirmCD();//c220711
        }
    }
    else {//cheon211203

        QString messageType = p_jsonObject["messageType"].toString();
        QString playerType = p_jsonObject["playerType"].toString();

        if(messageType == "login_state_change"){
            global.user.setDeviceChange(true);

            if(playerType == "TIDAL"){
                global.user_forTidal.setLogout();
                global.user_forTidal.set_logoutState();

                tidal::ProcCommon *proc_dbClear = new tidal::ProcCommon(this);
                proc_dbClear->clearTablesInfo_tidalDB();

                this->checkTidalLogin();
            }
            else if(playerType == "BUGS"){
                global.user_forBugs.set_logoutState();

                bugs::ProcBugsAPI *proc_dbClear = new bugs::ProcBugsAPI(this);
                proc_dbClear->clearTablesInfo_bugsDB();

                this->checkBugsLogin();
            }
            else if(playerType == "QOBUZ"){
                global.user_forQobuz.setLogout();

                qobuz::ProcCommon *proc_dbClear = new qobuz::ProcCommon(this);
                proc_dbClear->clearTablesInfo_qobuzDB();

                this->checkQobuzLogin();
            }
            else if(playerType == "APPLE_MUSIC"){
                global.user_forApple.setLogout();

                this->checkAppleLogin();
            }

            QTimer::singleShot(4000,this,[=](){emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::RoseHome));});//c230426
        }
        else if(messageType == "user_login_noti" || messageType == "user_logout_noti"){
            global.user.setDeviceChange(true);

            if(messageType == "user_logout_noti"){
                global.user.setDataUser(QJsonObject());

                SqliteHelper *sqliteHelper = new SqliteHelper(this);
                QSqlError err = sqliteHelper->addConnectionLocal();
                if(err.type() == QSqlError::NoError){

                    QString strQuery_delete = "DELETE FROM User";
                    sqliteHelper->exec(strQuery_delete);
                }
            }

            this->checkUserLogin();

            QTimer::singleShot(4000,this,[=](){emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::RoseHome));});//c230426
        }
        else if(messageType == "music_play_type_change"){

            QJsonObject json = QJsonObject();


            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsoSnObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_DEVICE_GET_INFO, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("get.music.play.type"), json, true, true);

        }
        else if(messageType == "show_toast_noti"){

        }
        else if(messageType == "state_check"){//c220521
            print_debug();
            qDebug() << "global.device.getDeviceIP() = " << global.device.getDeviceIP();
            connectDevice(global.device.getDeviceIP());
        }
    }
}


void HomeMain::dialog_comfirmCD(){      //c220711
    print_debug();
    DialogConfirm *dlgConfirmCD = new DialogConfirm(this);//c220714
    dlgConfirmCD->setTitle(tr("CD PLAY Notice!"));
    dlgConfirmCD->setTextHeight(150);
    dlgConfirmCD->setText(tr("Stops the currently playing CDPLAY immediately. \nPlease select(click) a song again."));
    dlgConfirmCD->setAlertMode();
    //dlgConfirmCD->setGeometry(this->width()/3,this->height()/2, 350,400);//c221007_1
    int left = global.left_mainwindow + ((global.width_mainwindow - 350) / 2);  //global.width_mainwindow/4;//c221007_1
    int top = global.top_mainwindow + ((global.height_mainwindow - 400) / 2);   //global.height_mainwindow/4;//c221007_1
    dlgConfirmCD->setGeometry(left, top, 350, 400);//c221007_1

    //if(dlgConfirmCD->property("flagShown").toBool()==false){
    dlgConfirmCD->setProperty("flagShown", true);

    int result = dlgConfirmCD->exec();

    if(result == QDialog::Accepted){
        print_debug();
        QJsonObject tmp_json;
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("cd.play.stop"), tmp_json, true, true);
        print_debug();
    }
    dlgConfirmCD->setProperty("flagShown", false);
    print_debug();

    //}

    delete dlgConfirmCD;
}


// USER ----------------------------------------- START

void HomeMain::checkUserInfo(){

    const QString key_count = "count(*)";

    int count = 0;

    SqliteHelper *sqliteHelper = new SqliteHelper(this);
    QSqlError err = sqliteHelper->addConnectionLocal();

    QString strQuery_table = "SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name = 'User'";
    QVariantList data;
    sqliteHelper->exec(strQuery_table, data);

    if(data.size() > 0){
        QMap<QString, QVariant> map = data.at(0).toMap();

        if(map.contains(key_count)){
            count = map[key_count].toInt();

            if(count > 0){
                sqliteHelper->exec("DROP TABLE User");
            }
        }
    }

    QString strQuery_create = "CREATE TABLE IF NOT EXISTS User (";
    strQuery_create += "email VARCHAR(100), pw VARCHAR(100), user_name VARCHAR(100), token_type VARCHAR(255), access_token VARCHAR(1024), refresh_token VARCHAR(1024))";
    sqliteHelper->exec(strQuery_create);

    QString strQuery_delete = "DELETE FROM User";
    sqliteHelper->exec(strQuery_delete);

    QString strQuery_insert = "INSERT INTO User (email, pw, user_name, token_type, access_token, refresh_token) ";
    strQuery_insert += " VALUES ";
    strQuery_insert += " ('" + global.user.getEmail() + "','" + "" + "','" + global.user.getUsername() + "','"
            + global.user.getToken_type() + "','" + global.user.getAccess_token() + "','" + global.user.getRefresh_token() + "'";
    strQuery_insert += ") ";
    sqliteHelper->exec(strQuery_insert);
}


void HomeMain::checkUserLogin(){

    // Access Token 확인
    const QString key_email = "email";
    const QString key_pw = "pw";
    const QString key_type = "token_type";
    const QString key_token = "access_token";
    const QString key_refresh = "refresh_token";
    const QString key_user = "user_name";
    const QString key_count = "count(*)";

    bool flag_autoLogin = false;

    QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
    flag_autoLogin = settings->value(rosesettings.SETTINGS_AUTO_LOGIN, false).toBool();

    QString email;
    QString pw;
    int count = 0;

    if(flag_autoLogin == true){

        SqliteHelper *sqliteHelper = new SqliteHelper(this);
        QSqlError err = sqliteHelper->addConnectionLocal();

        if(err.type() == QSqlError::NoError){

            QVariantList data;
            sqliteHelper->exec("SELECT * FROM User", data);

            if(data.size() > 0){
                QMap<QString, QVariant> map = data.at(0).toMap();

                if(map.contains(key_email)){
                    email = map[key_email].toString();
                }
                if(map.contains(key_pw)){
                    pw = map[key_pw].toString();
                }
                if(map.contains(key_token)){
                    QString tmp_token = map[key_token].toString();

                    QString tmp_type = "";
                    QString tmp_refresh = "";
                    QString tmp_user = "";

                    if(!tmp_token.isEmpty()){
                        if(map.contains(key_type))
                        {
                            tmp_type = map[key_type].toString();
                        }
                        if(map.contains(key_refresh))
                        {
                            tmp_refresh = map[key_refresh].toString();
                        }
                        if(map.contains(key_user))
                        {
                            tmp_user = map[key_user].toString();
                        }

                        this->isToken = true;
                    }

                    QJsonObject user_login;
                    user_login.insert("email", email);
                    user_login.insert("accessToken", tmp_token);
                    user_login.insert("tokenType", tmp_type);
                    user_login.insert("refreshToken", tmp_refresh);
                    user_login.insert("userName", tmp_user);
                    user_login.insert("flagOk", true);

                    global.user.setDataUser(user_login);
                }
                else{
                    QString strQuery_table = "SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name = 'User'";
                    QVariantList data;
                    sqliteHelper->exec(strQuery_table, data);

                    if(data.size() > 0){
                        QMap<QString, QVariant> map = data.at(0).toMap();

                        if(map.contains(key_count)){
                            count = map[key_count].toInt();

                            if(count > 0){
                                sqliteHelper->exec("DROP TABLE User");
                            }
                        }
                    }

                    QString tmp_id = email;
                    QString tmp_pw = pw;
                    QString tmp_txt = "";

                    QString strQuery_create = "CREATE TABLE IF NOT EXISTS User (";
                    strQuery_create += "email VARCHAR(100), pw VARCHAR(100), user_name VARCHAR(100), token_type VARCHAR(255), access_token VARCHAR(1024), refresh_token VARCHAR(1024))";
                    sqliteHelper->exec(strQuery_create);

                    QString strQuery_delete = "DELETE FROM User";
                    sqliteHelper->exec(strQuery_delete);

                    QString strQuery_insert = "INSERT INTO User (email, pw, user_name, token_type, access_token, refresh_token) ";
                    strQuery_insert += " VALUES ";
                    strQuery_insert += " ('" + tmp_id + "','" + tmp_pw + "','" + tmp_txt + "','" + tmp_txt + "','" + tmp_txt + "','" + tmp_txt + "'";
                    strQuery_insert += ") ";
                    sqliteHelper->exec(strQuery_insert);
                }
            }
            else{
                QString strQuery_table = "SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name = 'User'";
                QVariantList data;
                sqliteHelper->exec(strQuery_table, data);

                if(data.size() > 0){
                    QMap<QString, QVariant> map = data.at(0).toMap();

                    if(map.contains(key_count)){
                        count = map[key_count].toInt();

                        if(count > 0){
                            sqliteHelper->exec("DROP TABLE User");
                        }
                    }
                }

                QString tmp_id = email;
                QString tmp_pw = pw;
                QString tmp_txt = "";

                QString strQuery_create = "CREATE TABLE IF NOT EXISTS User (";
                strQuery_create += "email VARCHAR(100), pw VARCHAR(100), user_name VARCHAR(100), token_type VARCHAR(255), access_token VARCHAR(1024), refresh_token VARCHAR(1024))";
                sqliteHelper->exec(strQuery_create);

                QString strQuery_delete = "DELETE FROM User";
                sqliteHelper->exec(strQuery_delete);

                QString strQuery_insert = "INSERT INTO User (email, pw, user_name, token_type, access_token, refresh_token) ";
                strQuery_insert += " VALUES ";
                strQuery_insert += " ('" + tmp_id + "','" + tmp_pw + "','" + tmp_txt + "','" + tmp_txt + "','" + tmp_txt + "','" + tmp_txt + "'";
                strQuery_insert += ") ";
                sqliteHelper->exec(strQuery_insert);
            }
        }

        sqliteHelper->close();
        delete sqliteHelper;
    }
    if(!global.user.isValid()){//c221116
        print_debug();
        this->requestGetRoseUserInfo();
    }

}


// TIDAL ----------------------------------------- START
/**
 * @brief 사용자가 타이달 자동로그인 체크 여부를 확인해서 로그인 처리를 해둔다.
 */
void HomeMain::checkTidalLogin(){

    tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);                                 ///< 타이달 관련 처리 클래스
    tidal::UserLoginInfo tidal_userLoginInfo = procTidal->getLoginInfo_tidalDB();

    if(!tidal_userLoginInfo.email.isEmpty() && !global.user_forTidal.getUsername().isEmpty()){
        global.user_forTidal.setLogin(tidal_userLoginInfo.access_token, "", 0, tidal_userLoginInfo.email, true);
        global.user_forTidal.set_loginState(tidal_userLoginInfo.access_token, tidal_userLoginInfo.refresh_token, "", 0, 0, tidal_userLoginInfo.email);
    }else{//
        // return;
    }

    global.user_forTidal.flag_login_wait = true;

    // Rose에게 정보를 요청함
    tidal::ProcRosePlay_withTidal *procRose = new tidal::ProcRosePlay_withTidal(this);
    connect(procRose, &tidal::ProcRosePlay_withTidal::signal_completeReq_get_session_info, this, &HomeMain::slot_completeReq_get_tsession_info);
    print_debug();
    procRose->request_get_session_info();
}


void HomeMain::slot_incompleteReq_get_tsession_info(){

    if(global.user_forTidal.get_access_token().isEmpty() && this->flag_tget_session == false){
        global.user_forTidal.flag_login_wait = false;
        global.user_forTidal.setLogout();
    }
    else if(!global.user_forTidal.get_access_token().isEmpty() && this->flag_tget_session == false && global.user_forTidal.flag_login_wait == false){
        global.user_forTidal.flag_login_wait = true;

        // TIDAL Session 정보를 요청함
        tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
        connect(procTidal, &tidal::ProcCommon::signal_completeReq_getSession, this, &HomeMain::slot_completeReq_getSessionInfo);
        procTidal->request_tidal_getSession();
    }
    else if(this->flag_tget_session == false)
    {
        global.user_forTidal.flag_login_wait = false;
        global.user_forTidal.setLogout();
    }
}


void HomeMain::slot_completeReq_get_tsession_info(const tidal::RoseSessionInfo_forTidal& sessionInfo){

    this->flag_tget_session = true;

    if(sessionInfo.TIDAL_UserName.isEmpty() && global.user_forTidal.getUsername().isEmpty()){
        global.user_forTidal.flag_login_wait = false;
        global.user_forTidal.setLogout();
        global.user_forTidal.set_logoutState();

        if(global.user.isChanged() && (global.user_forTidal.isLogined() == false)){
            emit linker->signal_change_device_state(SIGNAL_CATEGORY_TIDAL);
        }
    }
    else if(!sessionInfo.TIDAL_UserName.isEmpty() && global.user_forTidal.getUsername().isEmpty()){
        global.user_forTidal.flag_login_wait = true;

        // Rose 정보로 로그인 정보를 업데이트함
        global.user_forTidal.setLogin(sessionInfo.TIDAL_AccessToken, sessionInfo.TIDAL_CountryCode, sessionInfo.TIDAL_UserID, sessionInfo.TIDAL_UserName, true);
        global.user_forTidal.set_loginState(sessionInfo.TIDAL_AccessToken, sessionInfo.TIDAL_RefreshToken, sessionInfo.TIDAL_CountryCode, sessionInfo.TIDAL_SessionId, sessionInfo.TIDAL_UserID, sessionInfo.TIDAL_UserName);
        global.user_forTidal.set_soundState(sessionInfo.TIDAL_HighestSoundQuality, sessionInfo.TIDAL_SoundQuality);

        // Genre 종류 요청 (Tidal 공통사용)
        tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
        connect(procTidal, &tidal::ProcCommon::signal_completeReq_getListGenre, this, &HomeMain::slot_completeReq_getTidalGenre);
        procTidal->request_tidal_getListGenres();
    }
    else if((!sessionInfo.TIDAL_UserName.isEmpty() && !global.user_forTidal.getUsername().isEmpty()) && (sessionInfo.TIDAL_UserName != global.user_forTidal.getUsername())){
        global.user_forTidal.flag_login_wait = true;

        // Rose 정보로 로그인 정보를 업데이트함
        global.user_forTidal.setLogin(sessionInfo.TIDAL_AccessToken, sessionInfo.TIDAL_CountryCode, sessionInfo.TIDAL_UserID, sessionInfo.TIDAL_UserName, true);
        global.user_forTidal.set_loginState(sessionInfo.TIDAL_AccessToken, sessionInfo.TIDAL_RefreshToken, sessionInfo.TIDAL_CountryCode, sessionInfo.TIDAL_SessionId, sessionInfo.TIDAL_UserID, sessionInfo.TIDAL_UserName);
        global.user_forTidal.set_soundState(sessionInfo.TIDAL_HighestSoundQuality, sessionInfo.TIDAL_SoundQuality);

        // Genre 종류 요청 (Tidal 공통사용)
        tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
        connect(procTidal, &tidal::ProcCommon::signal_completeReq_getListGenre, this, &HomeMain::slot_completeReq_getTidalGenre);
        procTidal->request_tidal_getListGenres();
    }
    else if((!sessionInfo.TIDAL_UserName.isEmpty() && !global.user_forTidal.getUsername().isEmpty()) && (sessionInfo.TIDAL_UserName == global.user_forTidal.getUsername())){
        global.user_forTidal.flag_login_wait = true;

        global.user_forTidal.set_loginState(sessionInfo.TIDAL_AccessToken, sessionInfo.TIDAL_RefreshToken, sessionInfo.TIDAL_CountryCode, sessionInfo.TIDAL_SessionId, sessionInfo.TIDAL_UserID, sessionInfo.TIDAL_UserName);
        global.user_forTidal.set_soundState(sessionInfo.TIDAL_HighestSoundQuality, sessionInfo.TIDAL_SoundQuality);

        int data_length = tidal::ProcCommon::getCount_listGenreInfo();
        if(data_length == 0){
            // Genre 종류 요청 (Tidal 공통사용)
            tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
            connect(procTidal, &tidal::ProcCommon::signal_completeReq_getListGenre, this, &HomeMain::slot_completeReq_getTidalGenre);
            procTidal->request_tidal_getListGenres();
        }
        else if(data_length < 0){
            global.user_forTidal.flag_login_wait = false;
            global.user_forTidal.setLogout();
            global.user_forTidal.set_logoutState();
        }
    }
}


void HomeMain::slot_completeReq_getTidalGenre(const bool flag, const QString errorMsg){

    Q_UNUSED(errorMsg);

    tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
    if(flag == true){
        global.user_forTidal.flag_login_wait = true;

        // Mood 종류 요청 (Tidal 공통사용)
        connect(procTidal, &tidal::ProcCommon::signal_completeReq_getListMoods, this, &HomeMain::slot_completeReq_getTidalMoods);
        procTidal->request_tidal_getListMoods();
    }
    else if(flag == false){
        global.user_forTidal.flag_login_wait = true;

        connect(procTidal, &tidal::ProcCommon::signal_completeReq_getRefreshToken, this, &HomeMain::slot_completeReq_getRefreshToken);
        procTidal->request_tidal_getRefreshTokenAndSave();
    }
}


void HomeMain::slot_completeReq_getTidalMoods(const bool flag, const QString errorMsg){

    Q_UNUSED(errorMsg);

    if(flag == true){
        global.user_forTidal.flag_login_wait = false;
    }
    else{
        global.user_forTidal.flag_login_wait = true;

        tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
        connect(procTidal, &tidal::ProcCommon::signal_completeReq_getRefreshToken, this, &HomeMain::slot_completeReq_getRefreshToken);
        procTidal->request_tidal_getRefreshTokenAndSave();
    }
}


void HomeMain::slot_completeReq_getRefreshToken(const bool flagSuccess, const QString errorMsg){

    Q_UNUSED(errorMsg);

    if(flagSuccess == true)
    {
        global.user_forTidal.flag_login_wait = true;

        // Subscription 정보를 요청함
        tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
        connect(procTidal, &tidal::ProcCommon::signal_completeReq_getSubscription, this, &HomeMain::slot_completeReq_getSubscription);
        procTidal->request_tidal_getSubscription(global.user_forTidal.getUserId());
    }
    else if(flagSuccess == false)
    {
        global.user_forTidal.flag_login_wait = false;
        global.user_forTidal.setLogout();
        global.user_forTidal.set_logoutState();
    }
}


void HomeMain::slot_completeReq_getSessionInfo(const QString errorMsg, const QJsonObject& jsonObj_session){

    if(!errorMsg.isEmpty()){
        if(!global.user_forTidal.get_access_token().isEmpty() && !global.user_forTidal.get_refresh_token().isEmpty())
        {
            global.user_forTidal.flag_login_wait = true;

            tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
            connect(procTidal, &tidal::ProcCommon::signal_completeReq_getRefreshToken, this, &HomeMain::slot_completeReq_getRefreshToken);
            procTidal->request_tidal_getRefreshTokenAndSave();
        }
        else
        {
            global.user_forTidal.flag_login_wait = false;
            global.user_forTidal.setLogout();
            global.user_forTidal.set_logoutState();
        }
    }
    else{
        global.user_forTidal.flag_login_wait = true;

        tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);

        tidal::RoseSessionInfo_forTidal sessionInfo;
        sessionInfo.TIDAL_CountryCode = ProcJsonEasy::getString(jsonObj_session, "countryCode");
        sessionInfo.TIDAL_SessionId = ProcJsonEasy::getString(jsonObj_session, "sessionId");
        sessionInfo.TIDAL_UserID = ProcJsonEasy::getInt(jsonObj_session, "userId");

        tidal::TidalTokenInfo ptokenInfo = procTidal->getTokenInfo_tidalDB();
        sessionInfo.TIDAL_AccessToken = ptokenInfo.access_token;
        sessionInfo.TIDAL_RefreshToken = ptokenInfo.refresh_token;
        sessionInfo.TIDAL_UserName = ptokenInfo.email;

        // 토큰 정보를 global에 저장함
        global.user_forTidal.set_loginState(sessionInfo.TIDAL_AccessToken, sessionInfo.TIDAL_RefreshToken, sessionInfo.TIDAL_CountryCode, sessionInfo.TIDAL_SessionId, sessionInfo.TIDAL_UserID, sessionInfo.TIDAL_UserName);

        // Subscription 정보를 요청함
        connect(procTidal, &tidal::ProcCommon::signal_completeReq_getSubscription, this, &HomeMain::slot_completeReq_getSubscription);
        procTidal->request_tidal_getSubscription(sessionInfo.TIDAL_UserID);
    }
}


void HomeMain::slot_completeReq_getSubscription(const QString errorMsg, const QJsonObject& jsonObj_subscription){

    if(!errorMsg.isEmpty()){
        global.user_forTidal.flag_login_wait = false;
        global.user_forTidal.setLogout();
        global.user_forTidal.set_logoutState();
    }
    else{
        global.user_forTidal.flag_login_wait = true;

        // Rose 에게 tidal session + subscription 정보를 전달함
        int highest = 0;
        int type = 0;
        QString tmp_highest = ProcJsonEasy::getString(jsonObj_subscription, "highestSoundQuality");
        QJsonObject tmp_json = ProcJsonEasy::getJsonObject(jsonObj_subscription, "subscription");
        QString tmp_type = ProcJsonEasy::getString(tmp_json, "type");

        if(tmp_highest.compare("LOW") == 0)
        {
            highest = 0;
        }
        else if(tmp_highest.compare("HIGH") == 0)
        {
            highest = 1;
        }
        else if(tmp_highest.compare("LOSSLESS") == 0)
        {
            highest = 2;
        }
        else if(tmp_highest.compare("HI_RES") == 0)
        {
            highest = 3;
        }

        if(tmp_type.compare("NORMAL") == 0)
        {
            type = 0;
        }
        else if(tmp_type.compare("HIGH") == 0)
        {
            type = 1;
        }
        else if(tmp_type.compare("HIFI") == 0)
        {
            type = 2;
        }
        else if(tmp_type.compare("MASTER") == 0)
        {
            type = 3;
        }
        global.user_forTidal.set_soundState(highest, type);

        tidal::RoseSessionInfo_forTidal sessionInfo;
        sessionInfo.TIDAL_AccessToken = global.user_forTidal.get_access_token();
        sessionInfo.TIDAL_RefreshToken = global.user_forTidal.get_refresh_token();
        sessionInfo.TIDAL_CountryCode = global.user_forTidal.getCountryCode();
        sessionInfo.TIDAL_SessionId = global.user_forTidal.getSessionId();
        sessionInfo.TIDAL_UserID = global.user_forTidal.getUserId();
        sessionInfo.TIDAL_UserName = global.user_forTidal.getUsername();
        sessionInfo.TIDAL_HighestSoundQuality = global.user_forTidal.getHeighestSound();
        sessionInfo.TIDAL_SoundQuality = global.user_forTidal.getSoundQuality();

        tidal::ProcRosePlay_withTidal *procRose = new tidal::ProcRosePlay_withTidal(this);
        procRose->request_set_session_info(sessionInfo);

        int data_length = tidal::ProcCommon::getCount_listGenreInfo();
        if(data_length == 0){
            // Genre 종류 요청 (Tidal 공통사용)
            tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
            connect(procTidal, &tidal::ProcCommon::signal_completeReq_getListGenre, this, &HomeMain::slot_completeReq_getTidalGenre);
            procTidal->request_tidal_getListGenres();
        }
        else{
            tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
            connect(procTidal, &tidal::ProcCommon::signal_completeReq_getListMoods, this, &HomeMain::slot_completeReq_getTidalMoods);
            procTidal->request_tidal_getListMoods();
        }
    }
}
// TIDAL ----------------------------------------- END


// BUGS ----------------------------------------- START
void HomeMain::checkBugsLogin(){


    bugs::ProcBugsAPI *procBugs = new bugs::ProcBugsAPI(this);
    bugs::BugsTokenInfo bugs_TokenInfo = procBugs->getTokenInfo_bugsDB();

    if(!bugs_TokenInfo.access_token.isEmpty() && !global.user_forBugs.get_access_token().isEmpty()){//c220618
        print_debug();
        global.user_forBugs.set_loginState(bugs_TokenInfo.access_token, bugs_TokenInfo.refresh_token);
    }else{//
        print_debug();
        // return;
    }

    global.user_forBugs.flag_login_wait = true;

    // Rose 장비로부터 Session 정보를 요청
    bugs::ProcRoseAPI_withBugs *procRose = new bugs::ProcRoseAPI_withBugs(this);
    connect(procRose, &bugs::ProcRoseAPI_withBugs::signal_completeReq_get_session_info, this, &HomeMain::slot_completeReq_get_bsession_info);
    print_debug();
    procRose->request_get_session_info();


}





void HomeMain::slot_completeReq_get_bsession_info(const bugs::RoseSessionInfo_forBugs& sessionInfo){

    this->flag_bget_session = true;

    if(sessionInfo.BUGS_AccessToken.isEmpty() && sessionInfo.BUGS_Nickname.isEmpty()){
        print_debug();
        global.user_forBugs.flag_login_wait = false;
        global.user_forBugs.set_logoutState();

        if(global.user.isChanged() && (global.user_forBugs.isLogined() == false)){
            print_debug();
            emit linker->signal_change_device_state(SIGNAL_CATEGORY_BUGS);
        }
    }
    else if(sessionInfo.BUGS_AccessToken.isEmpty() && !global.user_forBugs.get_access_token().isEmpty()){
        print_debug();
        global.user_forBugs.flag_login_wait = true;

        // BUGS Session 정보를 요청함
        bugs::ProcRoseAPI_withBugs *procBugs = new bugs::ProcRoseAPI_withBugs(this);
        connect(procBugs, &bugs::ProcRoseAPI_withBugs::signal_completeReq_get_session_info, this, &HomeMain::slot_completeReq_get_bsession_info);
        procBugs->request_get_session_info();
    }
    else if(!sessionInfo.BUGS_AccessToken.isEmpty() && global.user_forBugs.get_access_token().isEmpty()){
        print_debug();
        global.user_forBugs.flag_login_wait = true;

        // Rose 정보로 로그인 정보를 업데이트함
        global.user_forBugs.set_loginState(sessionInfo.BUGS_AccessToken, sessionInfo.BUGS_RefreshToken);
        global.user_forBugs.setNickName(sessionInfo.BUGS_Nickname);
        global.user_forBugs.setProductName(sessionInfo.BUGS_ProductName);

        this->slot_completeReq_getbLoginInfo();
    }
    else if((!sessionInfo.BUGS_AccessToken.isEmpty() && !global.user_forBugs.get_access_token().isEmpty()) && (sessionInfo.BUGS_AccessToken != global.user_forBugs.get_access_token())){
        print_debug();
        global.user_forBugs.flag_login_wait = false;

        // Rose 정보로 로그인 정보를 업데이트함
        global.user_forBugs.set_loginState(sessionInfo.BUGS_AccessToken, sessionInfo.BUGS_RefreshToken);
        global.user_forBugs.setNickName(sessionInfo.BUGS_Nickname);
        global.user_forBugs.setProductName(sessionInfo.BUGS_ProductName);

        this->slot_completeReq_getbLoginInfo();
    }
    else if((!sessionInfo.BUGS_AccessToken.isEmpty() && !global.user_forBugs.get_access_token().isEmpty()) && (sessionInfo.BUGS_AccessToken == global.user_forBugs.get_access_token())){
        print_debug();
        global.user_forBugs.flag_login_wait = false;

        global.user_forBugs.set_loginState(sessionInfo.BUGS_AccessToken, sessionInfo.BUGS_RefreshToken);
        global.user_forBugs.setNickName(sessionInfo.BUGS_Nickname);
        global.user_forBugs.setProductName(sessionInfo.BUGS_ProductName);

        this->slot_completeReq_getbLoginInfo();
    }
}

void HomeMain::slot_incompleteReq_get_bsession_info(){

    if(global.user_forBugs.get_access_token().isEmpty() && this->flag_bget_session == false){
        global.user_forBugs.flag_login_wait = false;
        global.user_forBugs.set_logoutState();
    }
    else if(!global.user_forBugs.get_access_token().isEmpty() && this->flag_bget_session == false && global.user_forBugs.flag_login_wait == false){
        global.user_forBugs.flag_login_wait = true;

        // BUGS Session 정보를 요청함
        bugs::ProcRoseAPI_withBugs *procBugs = new bugs::ProcRoseAPI_withBugs(this);
        connect(procBugs, &bugs::ProcRoseAPI_withBugs::signal_completeReq_get_session_info, this, &HomeMain::slot_completeReq_get_bsession_info);
        print_debug();
        procBugs->request_get_session_info();
    }
    else if(this->flag_bget_session == false)
    {
        global.user_forBugs.flag_login_wait = false;
        global.user_forBugs.set_logoutState();
    }
}
void HomeMain::slot_completeReq_getbLoginInfo(){

    global.user_forBugs.flag_login_wait = false;

    // 전체 장르 정보를 요청함 - 공통으로 사용 (세팅 안된 경우)
    bugs::ProcBugsAPI *procBugs = new bugs::ProcBugsAPI(this);
    procBugs->request_bugs_getListGenres();

    if(global.user.isChanged()){
        emit linker->signal_change_device_state(SIGNAL_CATEGORY_BUGS);
    }
}
// BUGS ----------------------------------------- END


// QOBUZ ----------------------------------------- START
/**
 * @brief 사용자가 코부즈 자동로그인 체크 여부를 확인해서 로그인 처리를 해둔다.
 */
void HomeMain::checkQobuzLogin(){

    qobuz::ProcCommon *procQobuz = new qobuz::ProcCommon(this);                      ///< 코부즈 관련 처리 클래스
    qobuz::UserLoginInfo qobuz_userLoginInfo = procQobuz->getLoginInfo_qobuzDB();

    if(!qobuz_userLoginInfo.username.isEmpty() && !global.user_forQobuz.getUsername().isEmpty()){//c220618
        global.user_forQobuz.setLogin(qobuz_userLoginInfo.auth_token, qobuz_userLoginInfo.username, qobuz_userLoginInfo.user_id, true);
    }else{//
        //return;
    }

    global.user_forQobuz.flag_login_wait = true;

    // Rose에게 정보를 요청함
    qobuz::ProcRosePlay_withQobuz *procRose = new qobuz::ProcRosePlay_withQobuz(this);
    connect(procRose, &qobuz::ProcRosePlay_withQobuz::signal_completeReq_get_session_info, this, &HomeMain::slot_completeReq_get_qsession_info);
    print_debug();
    procRose->request_get_session_info();
}


void HomeMain::slot_completeReq_get_qsession_info(const qobuz::RoseSessionInfo_forQobuz& sessionInfo){

    //qobuz::UserLoginInfo qobuz_userLoginInfo = procQobuz->getLoginInfo_qobuzDB();

    this->flag_qget_session = true;

    if(sessionInfo.email.isEmpty() && global.user_forQobuz.getUsername().isEmpty()){
        global.user_forQobuz.flag_login_wait = false;
        global.user_forQobuz.setLogout();

        if(global.user.isChanged() && (global.user_forQobuz.isLogined() == false)){
            emit linker->signal_change_device_state(SIGNAL_CATEGORY_QOBUZ);
        }
    }
    else if(sessionInfo.email.isEmpty() && !global.user_forQobuz.getUsername().isEmpty()){
        global.user_forQobuz.flag_login_wait = true;

        // 현재 어플에서 코부즈 로그인 안된 상태임 && 자동으로 로그인 처리를 시도함.
        qobuz::ProcCommon *procQobuz = new qobuz::ProcCommon(this);
        qobuz::UserLoginInfo qobuz_userLoginInfo = procQobuz->getLoginInfo_qobuzDB();
        connect(procQobuz, &qobuz::ProcCommon::successLogin, this, &HomeMain::slot_completeReq_getqLoginInfo);
        procQobuz->request_qobuzLogin(qobuz_userLoginInfo);
    }
    else if(!sessionInfo.email.isEmpty() && global.user_forQobuz.getUsername().isEmpty()){
        global.user_forQobuz.flag_login_wait = true;

        // Rose 정보로 로그인 정보를 업데이트함
        global.user_forQobuz.setLogin(sessionInfo.user_auth_token, sessionInfo.email, sessionInfo.user_id, true);
        global.user_forQobuz.setStream_state(sessionInfo.hires_streaming, sessionInfo.lossless_streaming);

        slot_completeReq_getqLoginInfo();
    }
    else if((!sessionInfo.email.isEmpty() && !global.user_forQobuz.getUsername().isEmpty()) && (sessionInfo.email != global.user_forQobuz.getUsername())){
        global.user_forQobuz.flag_login_wait = true;

        // Rose 정보로 로그인 정보를 업데이트함
        global.user_forQobuz.setLogin(sessionInfo.user_auth_token, sessionInfo.email, sessionInfo.user_id, true);
        global.user_forQobuz.setStream_state(sessionInfo.hires_streaming, sessionInfo.lossless_streaming);

        slot_completeReq_getqLoginInfo();
    }
    else if((!sessionInfo.email.isEmpty() && !global.user_forQobuz.getUsername().isEmpty()) && (sessionInfo.email == global.user_forQobuz.getUsername())){
        global.user_forQobuz.flag_login_wait = true;
        global.user_forQobuz.setStream_state(sessionInfo.hires_streaming, sessionInfo.lossless_streaming);

        if(sessionInfo.user_auth_token != global.user_forQobuz.getUserAuthToken()){
            global.user_forQobuz.setLogin(sessionInfo.user_auth_token, sessionInfo.email, sessionInfo.user_id, true);
        }

        slot_completeReq_getqLoginInfo();
    }
}


void HomeMain::slot_incompleteReq_get_qsession_info(){

    qobuz::ProcCommon *procQobuz = new qobuz::ProcCommon(this);                      ///< 코부즈 관련 처리 클래스
    qobuz::UserLoginInfo qobuz_userLoginInfo = procQobuz->getLoginInfo_qobuzDB();

    //print_home_func();
    if(qobuz_userLoginInfo.password.isEmpty() && this->flag_qget_session == false){
        global.user_forQobuz.flag_login_wait = false;
    }
    else if(!qobuz_userLoginInfo.password.isEmpty() && this->flag_qget_session == false){
        global.user_forQobuz.flag_login_wait = true;

        global.user_forQobuz.setLogin(qobuz_userLoginInfo.auth_token, qobuz_userLoginInfo.username, qobuz_userLoginInfo.user_id, true);

        slot_completeReq_getqLoginInfo();
    }
    else if(this->flag_qget_session == false)
    {
        global.user_forQobuz.flag_login_wait = false;

        global.user_forQobuz.setLogout();
    }
}


void HomeMain::slot_completeReq_getqLoginInfo(){

    global.user_forQobuz.flag_login_wait = false;

    //print_home_func();
    int data_length = qobuz::ProcCommon::getCount_listGenreInfo();
    if((data_length == 0) && (global.user_forQobuz.isLogined() == true)){
        // Genre 종류 요청 (Qobuz 공통사용)
        qobuz::ProcCommon *procQobuz = new qobuz::ProcCommon(this);
        connect(procQobuz, &qobuz::ProcCommon::successLogin, this, &HomeMain::slot_completeReq_getQobuzGenre);
        procQobuz->request_qobuz_getListGenres();
    }
    else{
        if(global.user.isChanged()){
            emit linker->signal_change_device_state(SIGNAL_CATEGORY_QOBUZ);
        }
    }
}


void HomeMain::slot_completeReq_getqLoginInfoQobuz(){//c230422

    global.user_forQobuz.flag_login_wait = false;

    //print_home_func();
    int data_length = qobuz::ProcCommon::getCount_listGenreInfo();
    if((data_length == 0) && (global.user_forQobuz.isLogined() == true)){
        //emit linker->signal_leftmenuLogined(10);//
        // Genre 종류 요청 (Qobuz 공통사용)
        qobuz::ProcCommon *procQobuz = new qobuz::ProcCommon(this);
        connect(procQobuz, &qobuz::ProcCommon::successLogin, this, &HomeMain::slot_completeReq_getQobuzGenre);
        procQobuz->request_qobuz_getListGenres();
    }
    else{
        if(global.user.isChanged()){
            emit linker->signal_change_device_state(SIGNAL_CATEGORY_QOBUZ);
        }
    }
}


void HomeMain::slot_completeReq_getQobuzGenre(){

    int data_length = qobuz::ProcCommon::getCount_listGenreInfo();
    if(data_length > 0){
        if(global.user.isChanged()){
            emit linker->signal_change_device_state(SIGNAL_CATEGORY_QOBUZ);
        }
    }
}
// QOBUZ ----------------------------------------- END


// APPLE MUSIC ----------------------------------------- START
void HomeMain::checkAppleLogin(){

    /*tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);                                 ///< 타이달 관련 처리 클래스
    tidal::UserLoginInfo tidal_userLoginInfo = procTidal->getLoginInfo_tidalDB();

    if(!tidal_userLoginInfo.email.isEmpty() && !global.user_forTidal.getUsername().isEmpty()){//c220618

        global.user_forTidal.setLogin(tidal_userLoginInfo.access_token, "", 0, tidal_userLoginInfo.email, true);
        global.user_forTidal.set_loginState(tidal_userLoginInfo.access_token, tidal_userLoginInfo.refresh_token, "", 0, 0, tidal_userLoginInfo.email);
    }

    //print_home_func();
    global.user_forTidal.flag_login_wait = true;*/

    // Rose에게 정보를 요청함
    apple::ProcRosePlay_withApple *procRose = new apple::ProcRosePlay_withApple(this);
    connect(procRose, &apple::ProcRosePlay_withApple::signal_completeReq_get_session_info, this, &HomeMain::slot_completeReq_get_asession_info);
    print_debug();
    procRose->request_get_session_info();
}

void HomeMain::slot_completeReq_get_asession_info(const apple::RoseSessionInfo_forApple& sessionInfo){

    if(!sessionInfo.APPLE_AccessToken.isEmpty() && !sessionInfo.APPLE_StoreFront.isEmpty()){

        global.user_forApple.setLogin(sessionInfo.APPLE_AccessToken, sessionInfo.APPLE_StoreFront, true);

        // Genre 종류 요청 (Tidal 공통사용)
        apple::ProcCommon *procApple = new apple::ProcCommon(this);
        connect(procApple, &apple::ProcCommon::signal_completeReq_getListGenre, this, &HomeMain::slot_completeReq_getAppleGenre);
        procApple->request_apple_getListGenres();
    }
}


void HomeMain::slot_completeReq_getAppleGenre(const bool flag, const QString errorMsg){

    if(flag == true && errorMsg.isEmpty()){
        int data_length = apple::ProcCommon::getCount_listGenreInfo();

        if(data_length > 0){
            if(global.user.isChanged()){
                emit linker->signal_change_device_state(SIGNAL_CATEGORY_APPLE);
            }
        }
    }
    else{

    }
}
// APPLE MUSIC ----------------------------------------- END


// RoseFM ----------------------------------------- START
void HomeMain::requestGetRoseFMCountryInfo(){
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    //==country list==//
    //https://api.roseaudio.kr/v1/radio/member/country?page=0&pageSize=10
    QUrlQuery params;
    params.addQueryItem("page", "0");
    params.addQueryItem("pageSize", "10");
    network->request(HTTP_ROSEFM_COUNTRY_GET, QString("%1/country").arg(global.rosefm_member), params, false, false);


}


void HomeMain::requestGetRoseFMChannelInfo(){
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    //==channel info==//
    //https://api.roseaudio.kr/v1/radio/member/channel/classic?countryId=1&page=0&pageSize=100
    QUrlQuery params;
    params.addQueryItem("countryId", "1");
    params.addQueryItem("page", "0");
    params.addQueryItem("pageSize", "100");
    network->request(HTTP_ROSEFM_CHANNEL_GET, QString("%1/channel/classic").arg(global.rosefm_member), params, false, false);
}


void HomeMain::setResultOfRoseFMCountryInfo(const QJsonObject &p_json){
    global.user_forRoseFM.setCountryInfo(p_json);

    //requestGetRoseFMChannelInfo();
}


void HomeMain::setResultOfRoseFMChannelInfo(const QJsonObject &p_json){
    global.user_forRoseFM.setChannelInfo(p_json);
}
// RoseFM ----------------------------------------- END


// RoseRadio ----------------------------------------- START
void HomeMain::requestGetRoseRadioGenreInfo(){

    // next_offset
    if(this->genre_total_count == 0){
        this->genre_offset = 0;
        this->jsonArr_genre = QJsonArray();
    }
    else{
        this->genre_offset++;
    }

    // request HTTP API
    roseHome::ProcCommon *proc_Genre = new roseHome::ProcCommon(this);
    connect(proc_Genre, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &HomeMain::setResultOfRoseRadioGenreInfo);
    proc_Genre->request_rose_getList_radioGenre(this->genre_offset, 30);

}


void HomeMain::requestGetRoseRadioRegionInfo(){

    // next_offset
    if(this->region_total_count == 0){
        this->region_offset = 0;
        this->jsonArr_region = QJsonArray();
    }
    else{
        this->region_offset++;
    }

    // request HTTP API
    roseHome::ProcCommon *proc_country = new roseHome::ProcCommon(this);
    connect(proc_country, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &HomeMain::setResultOfRoseRadioRegionInfo);
    proc_country->request_rose_getList_radioCountry(this->region_offset, 30);
}


void HomeMain::requestGetRoseRadioLanguageInfo(){

    // next_offset
    if(this->language_total_count == 0){
        this->language_offset = 0;
        this->jsonArr_language = QJsonArray();
    }
    else{
        this->language_offset++;
    }

    // request HTTP API
    roseHome::ProcCommon *proc_language = new roseHome::ProcCommon(this);
    connect(proc_language, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &HomeMain::setResultOfRoseRadioLanguageInfo);
    proc_language->request_rose_getList_radioLanguage(this->language_offset, 30);
}


void HomeMain::setResultOfRoseRadioGenreInfo(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage){

    if(jsonArr.size() > 0){
        if(this->genre_total_count == 0){
            this->genre_total_count = totalCount;

            this->jsonArr_genre = jsonArr;

            if(this->jsonArr_genre.size() < this->genre_total_count && flag_lastPage == false){
                this->requestGetRoseRadioGenreInfo();
            }
        }
        else{
            ProcJsonEasy::mergeJsonArray(this->jsonArr_genre, jsonArr);

            if(this->jsonArr_genre.size() < this->genre_total_count && flag_lastPage == false){
                this->requestGetRoseRadioGenreInfo();
            }
        }

        if(this->jsonArr_genre.size() == this->genre_total_count){
            global.user_forRoseRadio.setGenreInfo(this->jsonArr_genre);

            if((this->jsonArr_genre.size() == this->genre_total_count) && (this->jsonArr_region.size() == this->region_total_count) && (this->jsonArr_language.size() == this->language_total_count)){
                if(this->genre_total_count != 0 && this->region_total_count != 0 && this->language_total_count != 0){
                    this->flagRoseRadio = false;
                }
            }
        }
    }
}

void HomeMain::setResultOfRoseRadioRegionInfo(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage){

    if(jsonArr.size() > 0){
        if(this->region_total_count == 0){
            this->region_total_count = totalCount;

            this->jsonArr_region = jsonArr;

            if(this->jsonArr_region.size() < this->region_total_count && flag_lastPage == false){
                this->requestGetRoseRadioRegionInfo();
            }
        }
        else{
            ProcJsonEasy::mergeJsonArray(this->jsonArr_region, jsonArr);

            if(this->jsonArr_region.size() < this->region_total_count && flag_lastPage == false){
                this->requestGetRoseRadioRegionInfo();
            }
        }

        if(this->jsonArr_region.size() == this->region_total_count){
            global.user_forRoseRadio.setRegionInfo(this->jsonArr_region);

            if((this->jsonArr_genre.size() == this->genre_total_count) && (this->jsonArr_region.size() == this->region_total_count) && (this->jsonArr_language.size() == this->language_total_count)){
                if(this->genre_total_count != 0 && this->region_total_count != 0 && this->language_total_count != 0){
                    this->flagRoseRadio = false;
                }
            }
        }
    }
}

void HomeMain::setResultOfRoseRadioLanguageInfo(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage){

    Q_UNUSED(flag_lastPage);

    if(jsonArr.size() > 0){
        if(this->language_total_count == 0){
            this->language_total_count = totalCount;

            this->jsonArr_language = jsonArr;

            if(this->jsonArr_language.size() < this->language_total_count && flag_lastPage == false){
                this->requestGetRoseRadioLanguageInfo();
            }
        }
        else{
            ProcJsonEasy::mergeJsonArray(this->jsonArr_language, jsonArr);

            if(this->jsonArr_language.size() < this->language_total_count && flag_lastPage == false){
                this->requestGetRoseRadioLanguageInfo();
            }
        }

        if(this->jsonArr_language.size() == this->language_total_count){
            global.user_forRoseRadio.setLanguageInfo(this->jsonArr_language);

            if((this->jsonArr_genre.size() == this->genre_total_count) && (this->jsonArr_region.size() == this->region_total_count) && (this->jsonArr_language.size() == this->language_total_count)){
                if(this->genre_total_count != 0 && this->region_total_count != 0 && this->language_total_count != 0){
                    this->flagRoseRadio = false;
                }
            }
        }
    }
}
// RoseRadio ----------------------------------------- END


void HomeMain::slot_sectionleft_active(){

    if(global.user_forTidal.flag_login_wait == true){
        global.user_forTidal.flag_login_wait = false;
        global.user_forTidal.setLogout();
        global.user_forTidal.set_logoutState();
    }

    if(global.user_forQobuz.flag_login_wait == true){
        global.user_forQobuz.flag_login_wait = false;
        global.user_forQobuz.setLogout();
    }
}


/**
 * @brief MainWindow::changedLeftMainMenu [SLOT] 왼쪽 메뉴 클릭
 * @param p_menuCode
 */
void HomeMain::changedLeftMainMenu(QString p_menuCode){

    print_debug();
    qDebug() << "p_menuCode=" << p_menuCode;
    qDebug() << "this->currMainMenuCode=" << this->currMainMenuCode;
    if(global.abs_ani_dialog_wait->isHidden() != true ) return;//c230308_3

    if(this->currMainMenuCode != p_menuCode){
        print_debug();
        global.curMenuCode = p_menuCode;//c230423
        //ToastMsg::delay(this,"", tr("delay"), 2000);//c230314_1
        this->currMainMenuCode = p_menuCode;
        if(global.enable_section_left == true){
            print_debug();
            global.enable_section_left = false;//c230308_3
        }
        print_debug();


        //this->sectionBottom->setInitUIQueueIcon();//c230309
        // 메인메뉴 컨텐츠를 교체한다.
        AbstractMainContent *tmp_ABMainContent = nullptr;
        if(p_menuCode == QString(GSCommon::MainMenuCode::RoseHome)){


            tmp_ABMainContent = new roseHome::roseHomeMain(this);
            ToastMsg::delay(this,"", tr("delay"), 500);//c230316

            read_noticeDBTable();
            if(global.notice == 0){//cheon211008   start
                QMainWindow *full = new QMainWindow();
                print_debug();
                full->showFullScreen();
                print_debug();

                int fwidth = full->geometry().width();
                //int fheigth = full->geometry().height();

                full->close();
                if(fwidth >= 1330){
                    showDlgOfNotice_Readme();
                }
            }
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Music)){
            tmp_ABMainContent = new music::MusicMain(this);
            //}else if(p_menuCode == QString(GSCommon::MainMenuCode::Music_2)){
            //    tmp_ABMainContent = new music::MusicMain_2(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Video)){
            tmp_ABMainContent = new video::VideoMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Radio)){
            tmp_ABMainContent = new radio::RadioMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::RoseRadio)){
            tmp_ABMainContent = new roseRadio::roseRadioMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::RoseFM)){
            tmp_ABMainContent = new RoseFmMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::RoseTube)){
            tmp_ABMainContent = new rosetube::RoseTubeMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::PodCast)){
            tmp_ABMainContent = new podcast::PodCastMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::CDplay)){
            tmp_ABMainContent = new CDplay::CDplayMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Tidal)){
            tmp_ABMainContent = new tidal::TidalMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Bugs)){
            tmp_ABMainContent = new bugs::BugsMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Qobuz)){
            tmp_ABMainContent = new qobuz::QobuzMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::AppleMusic)){
            tmp_ABMainContent = new apple::AppleMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Spotify)){
            tmp_ABMainContent = new SpotifyMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Setting)){
            tmp_ABMainContent = new SettingMain(this);
        }
        print_debug();
        if(curr_absMainContent!=nullptr){
            this->curr_absMainContent->disconnect();
        }
        GSCommon::clearLayout_new(this->hlayout_content);


        this->curr_absMainContent = nullptr;
        this->hlayout_content->addWidget(tmp_ABMainContent);
        print_debug();
        //tmp_ABMainContent->disconnect();
        connect(tmp_ABMainContent, SIGNAL(showRemoteWidget(bool)), this, SLOT(showRemote(bool)));
        connect(tmp_ABMainContent, SIGNAL(signal_changeMyInfo()), this, SLOT(slot_changedLoginInfo()));
        connect(tmp_ABMainContent, &AbstractMainContent::changedSubMenu, this, &HomeMain::slot_changedSubMenu);
        connect(tmp_ABMainContent, &AbstractMainContent::signal_mustChangeMainMenuAndMovePage, this, &HomeMain::slot_changedMainMenuAndMovePage);
        connect(tmp_ABMainContent, &AbstractMainContent::signal_getCurrPlayData, this, &HomeMain::slot_getCurrPlayData);

        // 현재 메인메뉴의 pointer를 기억 by sunnyfish (검색 시그널 발생 시, 검색어 전달을 위함)
        this->curr_absMainContent = tmp_ABMainContent;

        // 띄워진 객체 모두 hide
        this->unRaiseAllWidget();
        print_debug();
        // 하단 PlayBar 관련 초기화
        this->sectionBottom->setInitUIQueueIcon();

        // DB 다운로드 프로그레스 raise
        //if((p_menuCode == QString(GSCommon::MainMenuCode::Music)) || (p_menuCode == QString(GSCommon::MainMenuCode::Setting))){
        if( (p_menuCode == QString(GSCommon::MainMenuCode::Setting))){//c230413
            this->progress->raise();
        }

        //c220818
        if(global.user.flag_rosehome == true){
            global.user.flag_rosehome = false;
            this->curr_absMainContent->goToMoveNewOrderPage(global.user.rosehome_obj);
        }
        else if(global.user.flag_music == true){
            global.user.flag_music = false;
            this->curr_absMainContent->goToMoveNewOrderPage(global.user.music_obj);
        }
        else if(global.user.flag_roseradio == true){
            global.user.flag_roseradio = false;
            this->curr_absMainContent->goToMoveNewOrderPage(global.user.roseradio_obj);
        }
        else if(global.user.flag_rosetube == true){
            global.user.flag_rosetube = false;
            this->curr_absMainContent->goToMoveNewOrderPage(global.user.rosetube_obj);
        }
        else if(global.user_forTidal.flag_rosehome == true){
            global.user_forTidal.flag_rosehome = false;
            this->curr_absMainContent->goToMoveNewOrderPage(global.user_forTidal.rosehome_obj);
        }
        else if(global.user_forBugs.flag_rosehome == true){
            global.user_forBugs.flag_rosehome = false;
            this->curr_absMainContent->goToMoveNewOrderPage(global.user_forBugs.rosehome_obj);
        }
        else if(global.user_forQobuz.flag_rosehome == true){
            global.user_forQobuz.flag_rosehome = false;
            this->curr_absMainContent->goToMoveNewOrderPage(global.user_forQobuz.rosehome_obj);
        }
        else if(global.user_forApple.flag_rosehome == true){
            global.user_forApple.flag_rosehome = false;
            this->curr_absMainContent->goToMoveNewOrderPage(global.user_forApple.rosehome_obj);
        }

        if(p_menuCode != QString(GSCommon::MainMenuCode::RoseHome)){

            global.user.flag_rosehome = false;
            global.user.flag_music = false;
            global.user.flag_roseradio = false;
            global.user.flag_rosetube = false;
            global.user_forTidal.flag_rosehome = false;
            global.user_forBugs.flag_rosehome = false;
            global.user_forQobuz.flag_rosehome = false;
            global.user_forApple.flag_rosehome = false;
        }
    }
    else{

        if(global.enable_section_left == true){
            global.enable_section_left = false;
        }

        //c220818
        if(global.user.flag_rosehome == true || global.user.flag_music == true || global.user.flag_roseradio == true || global.user.flag_rosetube == true ||
                global.user_forTidal.flag_rosehome == true || global.user_forBugs.flag_rosehome == true || global.user_forQobuz.flag_rosehome == true || global.user_forApple.flag_rosehome == true){

            emit linker->signal_dragEnterEvent_hide_show(false);

            if(global.user.flag_rosehome == true){
                global.user.flag_rosehome = false;
                this->curr_absMainContent->goToMoveNewOrderPage(global.user.rosehome_obj);
            }
            else if(global.user.flag_music == true){
                global.user.flag_music = false;
                this->curr_absMainContent->goToMoveNewOrderPage(global.user.music_obj);
            }
            else if(global.user.flag_roseradio == true){
                global.user.flag_roseradio = false;
                this->curr_absMainContent->goToMoveNewOrderPage(global.user.roseradio_obj);
            }
            else if(global.user.flag_rosetube == true){
                global.user.flag_rosetube = false;
                this->curr_absMainContent->goToMoveNewOrderPage(global.user.rosetube_obj);
            }
            else if(global.user_forTidal.flag_rosehome == true){
                global.user_forTidal.flag_rosehome = false;
                this->curr_absMainContent->goToMoveNewOrderPage(global.user_forTidal.rosehome_obj);
            }
            else if(global.user_forBugs.flag_rosehome == true){
                global.user_forBugs.flag_rosehome = false;
                this->curr_absMainContent->goToMoveNewOrderPage(global.user_forBugs.rosehome_obj);
            }
            else if(global.user_forQobuz.flag_rosehome == true){
                global.user_forQobuz.flag_rosehome = false;
                this->curr_absMainContent->goToMoveNewOrderPage(global.user_forQobuz.rosehome_obj);
            }
            else if(global.user_forApple.flag_rosehome == true){
                global.user_forApple.flag_rosehome = false;
                this->curr_absMainContent->goToMoveNewOrderPage(global.user_forApple.rosehome_obj);
            }
        }
    }
}


/**
 * @brief MainWindow::changedLeftMainMenu [SLOT] 왼쪽 메뉴 클릭
 * @param p_menuCode
 */
void HomeMain::changedLeftMainMenu_search(QString p_menuCode){//cheon211008

    print_debug();
    qDebug() << "HomeMain::changedLeftMainMenu_search------p_menuCode=" << p_menuCode;
    if(this->currMainMenuCode != p_menuCode){
        ToastMsg::delay(this,"", tr("delay"), 400);//c230313
        this->currMainMenuCode = p_menuCode;

        // 메인메뉴 컨텐츠를 교체한다.
        AbstractMainContent *tmp_ABMainContent = nullptr;

        if(p_menuCode == QString(GSCommon::MainMenuCode::RoseHome)){
            tmp_ABMainContent = new roseHome::roseHomeMain(this);
            read_noticeDBTable();
            if(global.notice == 0){//cheon211008   start
                QMainWindow *full = new QMainWindow();

                full->showFullScreen();

                int fwidth = full->geometry().width();
                //int fheigth = full->geometry().height();

                full->close();
                if(fwidth >= 1330){
                    showDlgOfNotice_Readme();
                }
            }
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Music)){
            tmp_ABMainContent = new music::MusicMain(this);
            this->progress->show();//c220826_2
            //this->progress_pop->show();
            //}else if(p_menuCode == QString(GSCommon::MainMenuCode::Music_2)){
            //    tmp_ABMainContent = new music::MusicMain_2(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Video)){
            tmp_ABMainContent = new video::VideoMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Radio)){
            tmp_ABMainContent = new radio::RadioMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::RoseRadio)){
            tmp_ABMainContent = new roseRadio::roseRadioMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::RoseFM)){
            tmp_ABMainContent = new RoseFmMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::RoseTube)){
            tmp_ABMainContent = new rosetube::RoseTubeMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::PodCast)){
            tmp_ABMainContent = new podcast::PodCastMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::CDplay)){
            tmp_ABMainContent = new CDplay::CDplayMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Tidal)){
            tmp_ABMainContent = new tidal::TidalMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Bugs)){
            tmp_ABMainContent = new bugs::BugsMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Qobuz)){
            tmp_ABMainContent = new qobuz::QobuzMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::AppleMusic)){
            tmp_ABMainContent = new apple::AppleMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Spotify)){
            tmp_ABMainContent = new SpotifyMain(this);
        }else if(p_menuCode == QString(GSCommon::MainMenuCode::Setting)){
            tmp_ABMainContent = new SettingMain(this);
        }


        if(curr_absMainContent!=nullptr){
            this->curr_absMainContent->disconnect();
        }
        GSCommon::clearLayout_new(this->hlayout_content);


        this->curr_absMainContent = nullptr;
        this->hlayout_content->addWidget(tmp_ABMainContent);

        //tmp_ABMainContent->disconnect();
        connect(tmp_ABMainContent, SIGNAL(showRemoteWidget(bool)), this, SLOT(showRemote(bool)));
        connect(tmp_ABMainContent, SIGNAL(signal_changeMyInfo()), this, SLOT(slot_changedLoginInfo()));
        connect(tmp_ABMainContent, &AbstractMainContent::changedSubMenu, this, &HomeMain::slot_changedSubMenu);
        connect(tmp_ABMainContent, &AbstractMainContent::signal_mustChangeMainMenuAndMovePage, this, &HomeMain::slot_changedMainMenuAndMovePage);
        connect(tmp_ABMainContent, &AbstractMainContent::signal_getCurrPlayData, this, &HomeMain::slot_getCurrPlayData);

        // 현재 메인메뉴의 pointer를 기억 by sunnyfish (검색 시그널 발생 시, 검색어 전달을 위함)
        this->curr_absMainContent = tmp_ABMainContent;

        // 띄워진 객체 모두 hide
        this->unRaiseAllWidget();

        // 하단 PlayBar 관련 초기화
        this->sectionBottom->setInitUIQueueIcon();

        // DB 다운로드 프로그레스 raise
        //if((p_menuCode == QString(GSCommon::MainMenuCode::Music)) || (p_menuCode == QString(GSCommon::MainMenuCode::Setting))){
        if( (p_menuCode == QString(GSCommon::MainMenuCode::Setting))){//c230413
            this->progress->raise();
        }

        //c220818
        if(global.user.flag_rosehome == true){
            global.user.flag_rosehome = false;
            this->curr_absMainContent->goToMoveNewOrderPage(global.user.rosehome_obj);
        }
        else if(global.user.flag_music == true){
            global.user.flag_music = false;
            this->curr_absMainContent->goToMoveNewOrderPage(global.user.music_obj);
        }
        else if(global.user.flag_roseradio == true){
            global.user.flag_roseradio = false;
            this->curr_absMainContent->goToMoveNewOrderPage(global.user.roseradio_obj);
        }
        else if(global.user.flag_rosetube == true){
            global.user.flag_rosetube = false;
            this->curr_absMainContent->goToMoveNewOrderPage(global.user.rosetube_obj);
        }
        else if(global.user_forTidal.flag_rosehome == true){
            global.user_forTidal.flag_rosehome = false;
            this->curr_absMainContent->goToMoveNewOrderPage(global.user_forTidal.rosehome_obj);
        }
        else if(global.user_forBugs.flag_rosehome == true){
            global.user_forBugs.flag_rosehome = false;
            this->curr_absMainContent->goToMoveNewOrderPage(global.user_forBugs.rosehome_obj);
        }
        else if(global.user_forQobuz.flag_rosehome == true){
            global.user_forQobuz.flag_rosehome = false;
            this->curr_absMainContent->goToMoveNewOrderPage(global.user_forQobuz.rosehome_obj);
        }

        if(p_menuCode != QString(GSCommon::MainMenuCode::RoseHome)){

            global.user.flag_rosehome = false;
            global.user.flag_music = false;
            global.user.flag_roseradio = false;
            global.user.flag_rosetube = false;
            global.user_forTidal.flag_rosehome = false;
            global.user_forBugs.flag_rosehome = false;
            global.user_forQobuz.flag_rosehome = false;
        }

        /*// 현재 메인메뉴의 pointer를 기억 by sunnyfish (검색 시그널 발생 시, 검색어 전달을 위함)
        this->curr_absMainContent = tmp_ABMainContent;

        // 띄워진 객체 모두 hide
        this->unRaiseAllWidget();

        // 하단 PlayBar 관련 초기화
        this->sectionBottom->setInitUIQueueIcon();

        // DB 다운로드 프로그레스 raise
        if((p_menuCode == QString(GSCommon::MainMenuCode::Music)) || (p_menuCode == QString(GSCommon::MainMenuCode::Setting))){
            this->progress->raise();
        }

        print_debug();
        //connect(linker, SIGNAL(signal_menumoveMusic()), this, SLOT(slot_MovePageSearchMusic()));
        connect(linker, SIGNAL(signal_menumoveBugs()), this, SLOT(slot_MovePageSearch()));
        connect(linker, SIGNAL(signal_menumoveTidal()), this, SLOT(slot_MovePageSearch()));
        connect(linker, SIGNAL(signal_menumoveQobuz()), this, SLOT(slot_MovePageSearch()));

        emit linker->signal_leftmenuSearch(p_menuCode);*/
    }
}


/**
 * @brief HomeMain::clickedBottomQueueIcon [SLOT] 하단 재생바의 큐 버튼 클릭
 * @param p_flagShow
 */
void HomeMain::clickedBottomQueueIcon(bool p_flagShow){//cheon211120-3
    print_debug();
    qDebug() << "p_flagShow = " << p_flagShow;

    if(p_flagShow){
        global.queueTimerFlag = true;//c211213
        //this->queueplaylist_copy->hide();
        //this->queueplaylist_copy = this->queueplaylist;

    }else{
        //this->queueplaylist = this->queueplaylist_copy;
        global.queueTimerFlag = false;//c211213
        this->queueplaylist->hide();
        this->unRaiseAllWidget(true, false, true);

    }

    if(p_flagShow){
        this->unRaiseAllWidget(true, false, true);
        //this->queueplaylist->hide();
        //this->queueplaylist = this->queueplaylist_copy;
        this->queueplaylist->islistOpen = true;
        global.queulistLeftPosion = (this->width()-QUEUEPLAY_W);//cheon211203
        this->queueplaylist->setGeometry((this->width()-QUEUEPLAY_W), TOP_NAVIBAR_H,  QUEUEPLAY_W, QUEUEPLAY_H);
        this->queueplaylist->show();
        //this->queueplaylist->hide();
        this->queueplaylist->raise();
        this->sectionBottom->raise();   // 큐보다 하단이 더 먼저 올라오게

    }

    /*
     * this->queueplaylist->hide();
    this->unRaiseAllWidget(true, false, true);

    if(p_flagShow){
        this->queueplaylist->islistOpen = true;
        this->queueplaylist->setGeometry((this->width()-QUEUEPLAY_W), TOP_NAVIBAR_H,  QUEUEPLAY_W, QUEUEPLAY_H);
        this->queueplaylist->show();
        this->queueplaylist->raise();
        this->sectionBottom->raise();   // 큐보다 하단이 더 먼저 올라오게
    }
    */
}
void HomeMain::clickedBottomQueueIconHide(bool p_flagShow){//c230428
    print_debug();
    qDebug() << "p_flagShow = " << p_flagShow;

    if(p_flagShow){
        global.queueTimerFlag = true;//c211213
        //this->queueplaylist_copy->hide();
        //this->queueplaylist_copy = this->queueplaylist;

    }else{
        //this->queueplaylist = this->queueplaylist_copy;
        global.queueTimerFlag = false;//c211213
        this->queueplaylist->hide();
        this->unRaiseAllWidget(true, false, true);

    }

    if(p_flagShow){
        this->unRaiseAllWidget(true, false, true);
        //this->queueplaylist->hide();
        //this->queueplaylist = this->queueplaylist_copy;
        this->queueplaylist->islistOpen = true;
        global.queulistLeftPosion = (this->width()-QUEUEPLAY_W);//cheon211203
        this->queueplaylist->setGeometry((this->width()-QUEUEPLAY_W), TOP_NAVIBAR_H,  QUEUEPLAY_W, QUEUEPLAY_H);
        this->queueplaylist->show();
        this->queueplaylist->hide();
        this->queueplaylist->raise();
        this->sectionBottom->raise();   // 큐보다 하단이 더 먼저 올라오게

    }

    /*
     * this->queueplaylist->hide();
    this->unRaiseAllWidget(true, false, true);

    if(p_flagShow){
        this->queueplaylist->islistOpen = true;
        this->queueplaylist->setGeometry((this->width()-QUEUEPLAY_W), TOP_NAVIBAR_H,  QUEUEPLAY_W, QUEUEPLAY_H);
        this->queueplaylist->show();
        this->queueplaylist->raise();
        this->sectionBottom->raise();   // 큐보다 하단이 더 먼저 올라오게
    }
    */
}


/**
 * @brief HomeMain::slot_clickedBottomVolumn [SLOT] 하단 재생바의 볼륨 버튼 클릭
 * @param p_flagShow
 */
void HomeMain::slot_clickedBottomVolumn(bool p_flagShow){

    this->volumnControl->hide();
    this->unRaiseAllWidget(true, true, false);
    if(p_flagShow){
        this->volumnControl->setGeometry((this->width()-VOLUMN_W), (this->height()-VOLUMN_H-110), VOLUMN_W, VOLUMN_H);
        this->volumnControl->show();
        this->volumnControl->raise();
        this->sectionBottom->raise();   // 하단이 더 먼저 올라오게
    }
}


/**
 * @brief HomeMain::slot_showPlayFullScreen [SLOT] 전체재생화면 페이지로 이동해라
 * @param p_jsonData
 */
void HomeMain::slot_showPlayFullScreen(const QJsonObject &p_jsonData){

    QJsonDocument doc(p_jsonData);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug() << "HomeMain::slot_showPlayFullScreen---p_jsonObj : " << strJson << "\n";                             //cheon210717-debugprint

    if(this->curr_absMainContent!=nullptr){
        this->curr_absMainContent->goToMoveNewOrderPage(p_jsonData);
    }
}


/**
 * @brief HomeMain::slot_changedCurrPlayTrack [SLOT] 현재 재생중인 음원이 변경되었다.
 * @param p_jsonData
 */
void HomeMain::slot_changedCurrPlayTrack(const QJsonObject &p_jsonData){

    if(this->curr_absMainContent!=nullptr){
        this->curr_absMainContent->resetCurrPlayData(p_jsonData);
    }
}


void HomeMain::slot_changedCurrHDMI(const bool flag){

    if(this->curr_absMainContent!=nullptr){
        this->curr_absMainContent->changeCurrHdmiData(flag);
    }
}


/**
 * @brief HomeMain::slot_searchBarFocusChanged [SLOT] 검색바 포커스 변경
 * @param p_flagHasFocus
 */
void HomeMain::slot_searchBarFocusChanged(bool p_flagHasFocus){

    if(p_flagHasFocus){
        print_debug();
        suggestionBar->setSuggestions_old();//c220620
        suggestionBar->setGeometry(this->width()-577, TOP_NAVIBAR_H, 500, this->height()-TOP_NAVIBAR_H-115);
        suggestionBar->show();
        suggestionBar->raise();
    }else{
        print_debug();//c220624
        suggestionBar->hide();
    }
}


/**
 * @brief HomeMain::showRemote [SLOT] 상단 리모콘 버튼 클릭시
 */
void HomeMain::showRemote(bool p_flagShow){

    /*//c230409
    if(!global.db_downloadComplete_flag) {//c220707
        print_debug();
        qDebug() << "global.db_downloadComplete_flag=" << global.db_downloadComplete_flag;
        ToastMsg::show(this,"", tr("DB is downloading from Rose-device. Please wait."), 3000, 0);

        return;
    }
    */

    if(!global.flagConnected){//c230417
        emit linker->signal_connected();//c230417
        ToastMsg::show(this,"", tr("Currently waiting for a response from the connected Rose Device."), 2000, 0);
    }

    print_debug();
    qDebug() << "global.db_downloadComplete_flag=" << global.db_downloadComplete_flag;
    this->unRaiseAllWidget(false, true, true);
    this->remoteWidget->hide();

    if(p_flagShow){
        if(this->size().height() < 1000){
            this->remoteWidget->setGeometry((this->width()-REMOTE_W), TOP_NAVIBAR_H,  REMOTE_W, this->size().height() - this->sectionBottom->height() - 80);
        }
        else{
            this->remoteWidget->setGeometry((this->width()-REMOTE_W), TOP_NAVIBAR_H,  REMOTE_W, REMOTE_H);
        }
        //this->remoteWidget->setGeometry((this->width()-REMOTE_W), TOP_NAVIBAR_H,  REMOTE_W, REMOTE_H*0.82);//cheon210812-remote
        this->remoteWidget->setInitData();
        this->remoteWidget->show();
        this->remoteWidget->raise();
    }
}


/**
 * @brief HomeMain::slot_changedPageSetting_VU [SLOT] 리모콘의 VU 버튼 클릭
 */
void HomeMain::slot_changedPageSetting_VU(){

    // 설정 메인메뉴 이동
    if(this->currMainMenuCode!=QString(GSCommon::MainMenuCode::Setting)){
        print_debug();
        this->sectionLeft->clickedMenu(QString(GSCommon::MainMenuCode::Setting));
    }
    // 입출력 페이지 변경
    if(this->hlayout_content->count() >0){
        QWidget *w_currMainMenu = this->hlayout_content->itemAt(0)->widget();
        dynamic_cast<SettingMain*>(w_currMainMenu)->changedPageVU();
    }
}


/**
 * @brief HomeMain::slot_changedPageSetting_InputOutput [SLOT] 리모콘의 외부입력 버튼 클릭
 */
void HomeMain::slot_changedPageSetting_InputOutput(){

    // 설정 메인메뉴 이동
    if(this->currMainMenuCode!=QString(GSCommon::MainMenuCode::Setting)){
        print_debug();
        this->sectionLeft->clickedMenu(QString(GSCommon::MainMenuCode::Setting));
    }
    // 입출력 페이지 변경
    if(this->hlayout_content->count() >0){
        QWidget *w_currMainMenu = this->hlayout_content->itemAt(0)->widget();
        dynamic_cast<SettingMain*>(w_currMainMenu)->changedPageInputOutput();
    }
}


/**
 * @brief HomeMain::slot_changedPageSetting_InputOutput [SLOT] 리모콘의 외부입력 버튼 클릭
 */
void HomeMain::slot_changedPageSetting_HDMI(){//cheon29_src

    // 설정 메인메뉴 이동
    if(this->currMainMenuCode!=QString(GSCommon::MainMenuCode::Setting)){
        print_debug();
        this->sectionLeft->clickedMenu(QString(GSCommon::MainMenuCode::Setting));
    }
    // 입출력 페이지 변경
    if(this->hlayout_content->count() >0){
        QWidget *w_currMainMenu = this->hlayout_content->itemAt(0)->widget();
        dynamic_cast<SettingMain*>(w_currMainMenu)->changedPageHDMI();//cheon29_src
    }
}


/**
 * @brief HomeMain::slot_changedPageSetting_InputOutput [SLOT] 리모콘의 외부입력 버튼 클릭
 */
void HomeMain::slot_changedPageSetting_Timer(){//cheon01_src

    // 설정 메인메뉴 이동
    if(this->currMainMenuCode!=QString(GSCommon::MainMenuCode::Setting)){
        print_debug();
        this->sectionLeft->clickedMenu(QString(GSCommon::MainMenuCode::Setting));
    }
    // 입출력 페이지 변경
    if(this->hlayout_content->count() >0){
        QWidget *w_currMainMenu = this->hlayout_content->itemAt(0)->widget();
        dynamic_cast<SettingMain*>(w_currMainMenu)->changedPageTimer();//cheon01_src
    }
}


/**
 * @brief HomeMain::reSize
 * @param p_width
 */
/*void HomeMain::reSize(const int &p_width){

    global.LmtCnt = p_width;

    if(p_width<=MAINWINDOW_SIZE_BOUNDARY){
        this->sectionLeft->setSmallSize();
        //this->sectionLeft->setFixedWidth(MENU_W_MIN);
    }else{
        this->sectionLeft->setBigSize();
        //this->sectionLeft->setFixedWidth(MENU_W_MAX);
    }

    this->suggestionBar->setGeometry(this->width()-577, TOP_NAVIBAR_H, 500, this->height()-TOP_NAVIBAR_H-115);
    //this->remoteWidget->setGeometry((this->width()-REMOTE_W), TOP_NAVIBAR_H,  REMOTE_W, REMOTE_H);//cheon210812-remote
    this->remoteWidget->setGeometry((this->width()-REMOTE_W), TOP_NAVIBAR_H,  REMOTE_W, REMOTE_H*0.82);//cheon210812-remote
    this->queueplaylist->setGeometry((this->width()-QUEUEPLAY_W), TOP_NAVIBAR_H, QUEUEPLAY_W, QUEUEPLAY_H);
    this->volumnControl->setGeometry((this->width()-VOLUMN_W), (this->height()-VOLUMN_H-110), VOLUMN_W, VOLUMN_H);
}*/


void HomeMain::reSize(const int &p_width){

    global.LmtCnt_H = this->height();

    if(p_width<=MAINWINDOW_SIZE_BOUNDARY){
        global.LmtCnt = p_width - 80;
        this->sectionLeft->setSmallSize();
        //this->sectionLeft->setFixedWidth(MENU_W_MIN);
    }
    else{
        global.LmtCnt = p_width - 240;
        this->sectionLeft->setBigSize();
        //this->sectionLeft->setFixedWidth(MENU_W_MAX);
    }

    if(this->size().height() < 1024){
        this->remoteWidget->setGeometry((this->width()-REMOTE_W), TOP_NAVIBAR_H,  REMOTE_W, this->size().height() - this->sectionBottom->height() - 80);
    }
    else{
        this->remoteWidget->setGeometry((this->width()-REMOTE_W), TOP_NAVIBAR_H,  REMOTE_W, REMOTE_H);
    }
    //this->remoteWidget->setGeometry((this->width()-REMOTE_W), TOP_NAVIBAR_H,  REMOTE_W, REMOTE_H*0.82);//cheon210812-remote

    this->suggestionBar->setGeometry(this->width()-577, TOP_NAVIBAR_H, 500, this->height()-TOP_NAVIBAR_H-115);
    this->queueplaylist->setGeometry((this->width()-QUEUEPLAY_W), TOP_NAVIBAR_H, QUEUEPLAY_W, QUEUEPLAY_H);
    this->volumnControl->setGeometry((this->width()-VOLUMN_W), (this->height()-VOLUMN_H-110), VOLUMN_W, VOLUMN_H);

    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}


/**
 * @brief HomeMain::getOldDbFileSize : OLD DB사이즈 반환
 * @return
 */
long HomeMain::getOldDbFileSize(){

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionLocal();

    if(err.type() == QSqlError::NoError){

        QString strQuery = "SELECT dbFileSize FROM Device";

        QVariantList data;
        sqlite->exec(strQuery, data);

        if(data.size() > 0){
            QMap<QString, QVariant> map = data.at(0).toMap();
            if(map.contains("dbFileSize")){
                return map["dbFileSize"].toInt();
            }

        }
    }

    sqlite->close();
    delete sqlite;

    return 0;
}


void HomeMain::saveCurrentDeviceData(DataDevice &p_data){

    SqliteHelper *sqliteHelper = new SqliteHelper(this);
    QSqlError err = sqliteHelper->addConnectionLocal();

    if(err.type() == QSqlError::NoError){

        QString strQuery_create = "CREATE TABLE IF NOT EXISTS Device (";
        strQuery_create += "deviceID VARCHAR(20), deviceIP VARCHAR(20), deviceName VARCHAR(20), deviceRoseToken VARCHAR(30), deviceType VARCHAR(20),";
        strQuery_create += "deviceVersion VARCHAR(20), isDbScanning integer, musicPlaytype integer, dbFileSize integer)";

        sqliteHelper->exec(strQuery_create);

        QString strQuery_delete = "DELETE FROM Device";
        sqliteHelper->exec(strQuery_delete);

        QString strQuery_insert = "INSERT INTO Device (deviceID, deviceIP, deviceName, deviceRoseToken, deviceType, deviceVersion, isDBScanning, musicPlaytype, dbFileSize) ";
        strQuery_insert += " VALUES ";
        strQuery_insert += " ('"+p_data.getDeviceID()+"','"+p_data.getDeviceIP()+"','"+p_data.getDeviceName()+"',";
        strQuery_insert += "  '"+p_data.getDeviceRoseToken()+"','"+p_data.getDeviceType()+"','"+p_data.getDeviceVersion()+"',";
        strQuery_insert += "  "+QString::number(p_data.getIsDbScanning())+","+QString::number(p_data.getMusicPlayType())+",";
        strQuery_insert += "  "+QString::number(p_data.getDbFileSize());
        strQuery_insert += "  )";

        sqliteHelper->exec(strQuery_insert);

    }

    sqliteHelper->close();
    delete sqliteHelper;
}


/**
 * @brief IntroRegister::setResultOfCurrDevice : 연결 Device 정보 요청 결과 처리
 * 미작업 :: DB 사이즈 확인용
 * @param p_jsonObject QJsonObject API device_connected 반환
 *
 */
void HomeMain::setResultOfCurrDevice(const QJsonObject &p_jsonObject){

    print_debug();
    //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"HomeMain::setResultOfCurrDevice---" << strJson;

    bool flagConnected = false;
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";
    const QString jsonKey_data = "data";
    //qDebug() << "p_jsonObject[jsonKey_flagOk].toBool(false)=" << p_jsonObject[jsonKey_flagOk].toBool(false);
    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        //print_debug();
        //dlgConfirm->hide();//c220525
        if(p_jsonObject.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonObject[jsonKey_status].toObject();
            // TODO:: Search Devices 16/02/2021 by disket
            //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
            if(p_jsonObject.contains(jsonKey_data)){

                this->unRaiseAllWidget(true, true, true);
                //dlgConfirm->setProperty("flagShown", true);
                if(dlgConfirm->isVisible()){
                    print_debug();
                    dlgConfirm->hide();
                }
                if(!dlgConfirm->isVisible()){
                    //emit linker->signal_checkQueue(26, "");//c220414
                }
                QJsonObject jsonData = p_jsonObject[jsonKey_data].toObject();

                flagConnected = true;

                QString old_deviceId = global.device.getDeviceID();

                global.device.setData(jsonData);
                global.flagConnected = true;

                long tmp_oldDbSize = getOldDbFileSize();

                //if(!global.user.isValid()){
                this->requestGetRoseUserInfo();//c230228
                this->requestGetShazamInfo();
                //}
                saveCurrentDeviceData(global.device);//c230423
                QString tmp_dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
                QDir dir(tmp_dir);
                //QFile file(QString("%1%2").arg(dir.path()).arg("/rose.db"));
                QString tmp_path = dir.path() + "/rose.db";
                QFile file(tmp_path);
                // print_debug();
                qDebug() << "file.exists()= " << file.exists();//c220826_2
                qDebug() << "file.size()= " << file.size();//c220826_2
                qDebug() << "tmp_oldDbSize= " << tmp_oldDbSize;//c220826_2
                qDebug() << "global.device.getDbFileSize()= " << global.device.getDbFileSize();//c220826_2
                if(((tmp_oldDbSize != global.device.getDbFileSize())) ||!file.exists() || ( ( file.size()==0) && global.device.getDbFileSize()!=0)){//c220426_2
                    print_debug();

                    qDebug() << "***************************************************************************************************************************************************************************************";
#if defined(Q_OS_WINDOWS)
                    QTimer::singleShot(2000, this, SLOT(slot_startDownloadDB()));
#endif
#if defined(Q_OS_MAC)
                    slot_startDownloadDB();
#endif

                }else{
                    print_debug();

                }
                if(old_deviceId != global.device.getDeviceID()){
                    print_debug();
                    global.signal_selectedDevice_flag = true;
                    QTimer::singleShot(2000, this, [=](){//
                        print_debug();
                        ToastMsg::show(this, "", QString(tr("RoseDevice has been changed.")));
                        });

                }else{
                    print_debug();
                    global.signal_selectedDevice_flag = false;
                }
                print_debug();//c230416
                emit linker->signal_connected();


            }else{//
                print_debug();
            }
            //}
        }

    }

    if(!flagConnected){
        print_debug();
        global.signal_device_connect_flag = false;
        //
        QTimer::singleShot(1000, this, SLOT(slot_showDlgOfDisconnect()));//c220601
        //slot_showDlgOfDisconnect();
        //ToastMsg::show(this, "", QString(tr("can't connect to the device.")));
    }


}


/**
 * @brief DialogLogin::setResultOfLogin : 로그인 요청결과 처리 함수
 * @param p_jsonData QJsonObject API response
 */
void HomeMain::setResultOfLogin(const QJsonObject &p_jsonData){//c220618

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";
    const QString jsonKey_message = "message";
    //const QString jsonKey_msg = "msg";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool(false)){
        if(p_jsonData.contains(jsonKey_result) && p_jsonData[jsonKey_result].toString().toLower()=="success"){

            global.user.setDataUser(p_jsonData);
            //c220618  start
            QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);

            QString pw;
            SqliteHelper *sqliteHelper = new SqliteHelper(this);
            QSqlError err = sqliteHelper->addConnectionLocal();

            settings->setValue(rosesettings.SETTINGS_AUTO_LOGIN, true);

            if(err.type() == QSqlError::NoError){
                QVariantList data;
                sqliteHelper->exec("SELECT * FROM User", data);

                if(data.size() > 0){
                    QMap<QString, QVariant> map = data.at(0).toMap();
                    if(map.contains("pw")){
                        pw = map["pw"].toString();
                    }
                }

                QString strQuery_delete = "DELETE FROM User";
                sqliteHelper->exec(strQuery_delete);

                QString strQuery_insert = "INSERT INTO User (email, pw, user_name, token_type, access_token, refresh_token) ";
                strQuery_insert += " VALUES ";
                strQuery_insert += " ('" + global.user.getEmail() + "','" + pw + "','" + global.user.getUsername() + "','" + global.user.getToken_type() + "','" + global.user.getAccess_token() + "','" + global.user.getRefresh_token() + "'";
                strQuery_insert += ") ";
                sqliteHelper->exec(strQuery_insert);
            }
            //c220618  end
            print_debug();qDebug() << "----------------------signal_logined";
            emit linker->signal_logined();

            requestSetRoseUserInfo();
        }
        else if(p_jsonData[jsonKey_message].toString().toLower() == "정상"){
            global.user.setDataUser(p_jsonData);

            QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);

            QString pw;
            SqliteHelper *sqliteHelper = new SqliteHelper(this);
            QSqlError err = sqliteHelper->addConnectionLocal();

            settings->setValue(rosesettings.SETTINGS_AUTO_LOGIN, true);

            if(err.type() == QSqlError::NoError){
                QVariantList data;
                sqliteHelper->exec("SELECT * FROM User", data);

                if(data.size() > 0){
                    QMap<QString, QVariant> map = data.at(0).toMap();
                    if(map.contains("pw")){
                        pw = map["pw"].toString();
                    }
                }

                QString strQuery_delete = "DELETE FROM User";
                sqliteHelper->exec(strQuery_delete);

                QString strQuery_insert = "INSERT INTO User (email, pw, user_name, token_type, access_token, refresh_token) ";
                strQuery_insert += " VALUES ";
                strQuery_insert += " ('" + global.user.getEmail() + "','" + pw + "','" + global.user.getUsername() + "','" + global.user.getToken_type() + "','" + global.user.getAccess_token() + "','" + global.user.getRefresh_token() + "'";
                strQuery_insert += ") ";
                sqliteHelper->exec(strQuery_insert);
            }

            sqliteHelper->close();
            delete sqliteHelper;
            print_debug();qDebug() << "----------------------signal_logined";
            emit linker->signal_logined();
            requestSetRoseUserInfo();//c220618
        }
    }
    else{
        emit linker->signal_login_fail();
    }
}


/**
 * @brief HomeMain::setResultOfGetRoseUserInfo : 로즈기기의 사용자 정보 요청 결과 처리
 * @param p_jsonData
 * @note 자동 로그인전 호출<br>
 * 로즈 기기에 사용자 정보가 있는 경우 따로 로그인 호출하지 않음 :: 로그인 호출시 다른 단말기 로그인 끊김.
 */
void HomeMain::setResultOfGetRoseUserInfo(const QJsonObject &p_jsonData){

    print_debug();
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";
    //const QString jsonKey_message = "message";
    //const QString jsonKey_msg = "msg";
    //const QString jsonKey_email = "email";

    bool flagOk = false;

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){

        flagOk = true;

        if(p_jsonData.contains(jsonKey_status)){

            QJsonObject jsonStatus = p_jsonData[jsonKey_status].toObject();

            if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString() == "OK"){

                // -----------------------------------------------------------
                //  연결된 로즈기기 & App 사용자 정보 비교
                // -----------------------------------------------------------
                QString tmp_email = ProcJsonEasy::getString(p_jsonData, "email");
                QString tmp_userName = ProcJsonEasy::getString(p_jsonData, "userName");

                if(tmp_email.isEmpty() && tmp_userName.isEmpty()){

                    if(global.user.isChanged()){

                        // Sending user information to ROSE
                        global.user.setDataUser(p_jsonData);

                        if(global.user.isValid() == false){
                            SqliteHelper *sqliteHelper = new SqliteHelper(this);
                            QSqlError err = sqliteHelper->addConnectionLocal();
                            if(err.type() == QSqlError::NoError){

                                QString strQuery_delete = "DELETE FROM User";
                                sqliteHelper->exec(strQuery_delete);
                            }

                            this->slot_changedLoginInfo();
                        }


                        emit linker->signal_change_device_state(SIGNAL_CATEGORY_ROSE);
                    }
                    else{
                        this->requestSetRoseUserInfo();
                    }
                }
                else if(!tmp_email.isEmpty() && global.user.getEmail().isEmpty()){

                    // Get Rose user information
                    global.user.setDataUser(p_jsonData);
                    this->checkUserInfo();

                    if(global.user.isChanged()){

                        emit linker->signal_change_device_state(SIGNAL_CATEGORY_ROSE);
                    }
                }
                else if((!tmp_email.isEmpty() && !global.user.getEmail().isEmpty()) && (tmp_email != global.user.getEmail())){

                    // Get Rose user information
                    global.user.setDataUser(p_jsonData);
                    this->checkUserInfo();

                    if(global.user.isChanged()){

                        emit linker->signal_change_device_state(SIGNAL_CATEGORY_ROSE);
                    }

                    ToastMsg::show(this, "", tr("Your account information has been changed."), 3000);
                }


                //roseFM & roseRadio get country & channel info
                if(this->flagRoseRadio == false){

                    this->flagRoseRadio = true;

                    this->genre_offset = 0;
                    this->region_offset = 0;
                    this->language_offset = 0;

                    this->genre_total_count = 0;
                    this->region_total_count = 0;
                    this->language_total_count = 0;

                    this->jsonArr_genre = QJsonArray();
                    this->jsonArr_region = QJsonArray();
                    this->jsonArr_language = QJsonArray();

                    this->requestGetRoseRadioGenreInfo();
                    this->requestGetRoseRadioRegionInfo();
                    this->requestGetRoseRadioLanguageInfo();
                }
            }
        }
    }

    if((flagOk == true) && (global.user.isValid() == true)){

        //if(!dlgConfirm->isVisible()){
        //emit linker->signal_checkQueue(26, "");//c220414
        //}
        // 내 정보 가져오기
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString url = global.legacy_v1 + "/member/member/" + global.user.getSub();
        QJsonObject json;

        network->request(HTTP_GET_MYINFO
                         , url
                         , json
                         , false
                         , true);

        /*network->request(HTTP_GET_MYINFO, QString("%1/member/member/%2")
                         .arg(global.legacy_v1).arg(global.user.getSub()), json, false, true);*/
    }
}


void HomeMain::setResultOfGetShazamInfo(const QJsonObject &p_json){

    global.user.setShazamInfo(p_json);
}


/**
 * @brief HomeMain::setResultOfUserProfile : 사용자 정보 요청
 * @param p_json
 * @note 로즈 기기로부터 받아온 사용자 정보 유효성 확인 용도
 */
void HomeMain::setResultOfUserProfile(const QJsonObject &p_json){//c220625

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_member = "member";
    const QString jsonKey_name = "name";

    bool tmp_flagOk = false;

    if(p_json.contains(jsonKey_flagOk) && p_json[jsonKey_flagOk].toBool()){
        if(p_json.contains(jsonKey_member)){
            QJsonObject jsonUser = p_json[jsonKey_member].toObject();

            if(jsonUser.contains(jsonKey_name)){
                QString tmp_userName = jsonUser[jsonKey_name].toString();

                if(tmp_userName == global.user.getUsername()){
                    tmp_flagOk = true;
                    this->isToken = true;
                    print_debug();qDebug() << "----------------------signal_logined";
                    emit linker->signal_logined();
                }
            }
        }
    }
    else{
        ToastMsg::show(this, "", tr("Your account information has been changed.\nPlease log-in again."));
    }

    if(!tmp_flagOk){
        //global.user.setDataUser(QJsonObject());
        //requestAutoLogin();

        if(ProcJsonEasy::get_flagOk(p_json) == true){
            global.user.setUserInfo(p_json);
            print_debug();qDebug() << "----------------------signal_logined";
            emit linker->signal_logined();
        }
        else{
            global.user.setDataUser(QJsonObject());
            requestAutoLogin();

        }
    }

}


/**
 * @brief HomeMain::autoLogin : 자동 로그인 실행
 * @details 사용자 설정값, 정보 확인 후 자동 로그인을 실행한다.
 */
void HomeMain::requestAutoLogin(){

    if(this->isToken == false){
        const QString key_email = "email";
        const QString key_pw = "pw";

        QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
        bool flagAutoLogin = settings->value(rosesettings.SETTINGS_AUTO_LOGIN, false).toBool();

        if(flagAutoLogin){

            QString email;
            QString pw;

            SqliteHelper *sqlite = new SqliteHelper(this);
            QSqlError err = sqlite->addConnectionLocal();

            if(err.type() == QSqlError::NoError){
                QVariantList data;
                sqlite->exec("SELECT * FROM User", data);

                if(data.size() > 0){
                    QMap<QString, QVariant> map = data.at(0).toMap();
                    if(map.contains(key_email)){
                        email = map[key_email].toString();
                    }
                    if(map.contains(key_pw)){
                        pw = map[key_pw].toString();
                    }
                }
            }

            sqlite->close();
            delete sqlite;

            if(!email.isNull() && !pw.isNull()){
                qDebug() << "email=" << email;
                qDebug() << "pw=" << pw;
                NetworkHttp *network = new NetworkHttp(this);
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QJsonObject jsonData;

                // roseaudio api change      22/02/2021    fixed jeon
                jsonData.insert("email", email);
                jsonData.insert("password", pw);

                network->request(HTTP_AUTO_LOGIN
                                 , QString("%1/member/member/login").arg(global.legacy_v1)
                                 , jsonData
                                 , true);

                /*jsonData.insert("user_name", email);
                jsonData.insert("password", pw);

                network->request(HTTP_AUTO_LOGIN, QString("%1%2")
                                 .arg(global.legacy_v3)
                                 .arg("/auth/login"), jsonData, true);;*/
            }else{
                print_debug();
                if(global.abs_ani_dialog_wait == nullptr) return;//c221004_3
                qDebug() << "abs_ani_dialog_wait->isHidden() = " << global.abs_ani_dialog_wait->isHidden();
                if(global.abs_ani_dialog_wait->isHidden() != true){//c220926_1
                    print_debug();
                    //abs_ani_mov->stop();
                    global.abs_ani_dialog_wait->hide(); //cheontidal
                }
            }
        }else{
            print_debug();
            if(global.abs_ani_dialog_wait == nullptr) return;//c221004_3
            qDebug() << "abs_ani_dialog_wait->isHidden() = " << global.abs_ani_dialog_wait->isHidden();
            if(global.abs_ani_dialog_wait->isHidden() != true){//c220926_1
                print_debug();
                //abs_ani_mov->stop();
                global.abs_ani_dialog_wait->hide(); //cheontidal
            }
        }
    }
}


/**
 * @brief HomeMain::requestSetRoseUserInfo : 로즈기기 사용자 정보 SET 요청
 * @note 로즈기기에 로그인한 사용자 정보 SET<br>
 */
void HomeMain::requestSetRoseUserInfo(){

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject jsonData;
    jsonData.insert("userName", global.user.getUsername());
    jsonData.insert("email", global.user.getEmail());
    jsonData.insert("accesskey", global.user.getAccess_token());

    network->request(HTTP_SET_ROSE_USERINFO, QString("http://%1:%2/set_rose_user_info")
                     .arg(global.device.getDeviceIP()).arg(global.port), jsonData, true);
}


/**
 * @brief HomeMain::requestGetRoseUserInfo : 로즈기기 사용자 정보 GET 요청
 */
void HomeMain::requestGetRoseUserInfo(){

    print_debug();
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject tmp_json;

    //tmp_json.insert("connectIP", findIp->getMyIP());
    network->request(HTTP_DEVICE_USER_INFO, QString("http://%1:%2/get_rose_user_info")
                     .arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true);

}


void HomeMain::requestGetShazamInfo(){

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject tmp_json;

    QString url = global.legacy_v1 + "/member/external/token?type=SHAZAM";
    QJsonObject json;

    network->request(HTTP_SHAZAM_INFO
                     , url
                     , json
                     , false
                     , false);
}


void HomeMain::requestGetHDMI(){  //cheon01_src

    if(global.device.getDeviceIP()!=""){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        // API HDMI RESOLUTION 조절
        QJsonObject json;
        json.insert("roseToken",global.device.getDeviceRoseToken());
        network->request(HTTP_DEVICE_HDMI_GET, QString("http://%1:%2/get_control_info").arg(global.device.getDeviceIP()).arg(global.port), json, true);
    }else {
        ToastMsg::show(this, "", tr("Failed to get ROSE device IP"));//cheon29_src

    }
}


void HomeMain::requestGetTimer(){//cheon01_src

    print_debug(); //cheon01
    if(global.device.getDeviceIP()!=""){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        // API Timer RESOLUTION 조절
        QJsonObject json;
        //    json.insert("roseToken",global.device.getDeviceRoseToken());
        //     qDebug() << "QString("http://%1:%2/sleep.time.get").arg(global.device.getDeviceIP()).arg(global.port)" << QString("http://%1:%2/sleep.time.get").arg(global.device.getDeviceIP()).arg(global.port);
        //    QJsonDocument doc(json);
        //   QString strJson(doc.toJson(QJsonDocument::Compact));
        //    qDebug() << "GET----Timer_p_jsonObject : " << strJson;//cheon01.
        network->request(HTTP_DEVICE_TIMER_GET, QString("http://%1:%2/sleep.time.get").arg(global.device.getDeviceIP()).arg(global.port), json, true);
    }else {
        ToastMsg::show(this, "", tr("Failed to get ROSE device IP"));//cheon01

    }
}


/**
 * @brief HomeMain::responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void HomeMain::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    //print_debug();
    //QJsonDocument doc(p_jsonObject);
    //QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() <<"HomeMain::slot_responseHttp---" << strJson;


    if(!p_jsonObject.contains("flagOk")){//c220526
        print_debug();
        print_debug();
        if(global.abs_ani_dialog_wait == nullptr) return;//c221004_3
        qDebug() << "abs_ani_dialog_wait->isHidden() = " << global.abs_ani_dialog_wait->isHidden();
        if(global.abs_ani_dialog_wait->isHidden() != true){//c220926_1
            print_debug();
            //abs_ani_mov->stop();
            global.abs_ani_dialog_wait->hide(); //cheontidal
        }
    }
    else if(p_jsonObject["flagOk"].toBool()){//c220715


        if(p_id == HTTP_DEVICE_CONNECTED){
            //print_debug();
            setResultOfCurrDevice(p_jsonObject);
        }else if(p_id==4433){
            print_debug();

        }
        else if(p_id==HTTP_DEVICE_GET_VOLUMN){
            print_debug();
            // nothing
        }
        else if(p_id==HTTP_NETWORK_PLAY){//c220711
            print_debug();
            if(p_jsonObject.contains("code")){//cheon210812-iso
                QString tmp_code = p_jsonObject["code"].toString();
                bool tmp_codeOk = p_jsonObject["flagOk"].toBool();
                if(tmp_code =="CDPLAY" && tmp_codeOk){
                    //qDebug() << "tmp_code-CDPLAYING---";
                    //emit signal_isoplay(true);
                    //global.isoflag = true;
                }
                else if(tmp_code =="G0000"&& tmp_codeOk){
                    print_debug();

                    //emit signal_isoplay(false);
                    //QTimer::singleShot(2000, this, SLOT(slot_iso_play()));
                    //global.isoflag = false;
                    ToastMsg::show(this, "", tr("CD PLAY has stoped. \nPlaese Select a song again to play CD PLAY."),3000);//cheon210812-iso

                }


            }else {//cheon210812-iso

            }
        }
        else if(p_id==HTTP_AUTO_LOGIN){
            print_debug();
            setResultOfLogin(p_jsonObject);
        }
        else if(p_id==HTTP_AUTO_SESSION){
            print_debug();

        }
        else if(p_id==HTTP_SET_ROSE_USERINFO){
            print_debug();
            // nothing
        }
        else if(p_id==HTTP_DEVICE_USER_INFO){
            print_debug();
            //c221006_1
            //  print_debug();
            /*//c220127
                QJsonObject tmp_json;
                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_deviceIOState(int, QJsonObject)));
                network->request(3333, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("in.out.mode.get"), tmp_json, true);
                */
            //c221006_1
            setResultOfGetRoseUserInfo(p_jsonObject);
            this->checkTidalLogin();
            this->checkBugsLogin();
            this->checkQobuzLogin();
        }
        else if(p_id==HTTP_DEVICE_GET_INFO){
            print_debug();
            const QString jsonKey_flagOk = "flagOk";
            const QString jsonKey_data = "data";

            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
                if(p_jsonObject.contains(jsonKey_data)){

                    QJsonObject data = ProcJsonEasy::getJsonObject(p_jsonObject, "data");
                    int playType = ProcJsonEasy::getInt(data, "musicPlaytype");

                    global.device.setMusicPlayType(playType);

                    emit linker->signal_httpservermsg_music_play_type_change();
                }
            }
        }
        else if(p_id==HTTP_GET_MYINFO){
            print_debug();
            setResultOfUserProfile(p_jsonObject);
        }
        else if(p_id==HTTP_SHAZAM_INFO){
            setResultOfGetShazamInfo(p_jsonObject);
        }
        else if(p_id==HTTP_ROSEFM_COUNTRY_GET){
            print_debug();
            setResultOfRoseFMCountryInfo(p_jsonObject);
        }
        else if(p_id==HTTP_ROSEFM_CHANNEL_GET){
            print_debug();
            setResultOfRoseFMChannelInfo(p_jsonObject);
        }
        else if(p_id==HTTP_DEVICE_TIMER_GET){//cheon01_src
            print_debug();
            // nothing
            //print_debug();//cheon01_src
            //QJsonDocument doc(p_jsonObject);
            //QString strJson(doc.toJson(QJsonDocument::Compact));
            //qDebug() << "slot_responseHttp()-get-TIMER_pre-setResultOfCurrDevice--p_jsonObject : " << strJson;//cheon01_src.
            //setResultOfCurrDevice(p_jsonObject); //cheon01_src
        }else if(p_id==HTTP_DEVICE_HDMI_GET){//cheon01_src
            print_debug();
            // nothing
            //print_debug();//cheon01_src
            //QJsonDocument doc(p_jsonObject);
            //QString strJson(doc.toJson(QJsonDocument::Compact));
            //qDebug() << "slot_responseHttp()-get-HDMI_pre-setResultOfCurrDevice--p_jsonObject : " << strJson;//cheon01_src.
            //setResultOfCurrDevice(p_jsonObject); //cheon01_src
        }else{//c220521
            print_debug();
            //slot_showDlgOfDisconnect();
        }
    }
    else{
        print_debug();
        //c221006_1
        if(global.abs_ani_dialog_wait == nullptr) return;//c221004_3
        qDebug() << "abs_ani_dialog_wait->isHidden() = " << global.abs_ani_dialog_wait->isHidden();
        if(global.abs_ani_dialog_wait->isHidden() != true){//c220926_1
            print_debug();
            //abs_ani_mov->stop();
            global.abs_ani_dialog_wait->hide(); //cheontidal
        }
        print_debug();

        // QTimer::singleShot(1000, this, SLOT(slot_showDlgOfDisconnect()));//c221006_1
    }

    sender()->deleteLater();
}



/**
 * @brief MainWindow::downloadDB : DB Download
 */
void HomeMain::slot_startDownloadDB(){//c230409
    this->p_preByte = 0;
    global.db_downloadComplete_flag = false;
    //this->unRaiseAllWidget();//c230228
    //emit linker->signal_searchBar_clearfocus();
    ToastMsg::show(this, "", tr("Start downloading the music information file. \nIf you want to check, click on the music menu."),3000, 0,2);
    print_debug();
    qDebug()<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";

    QString tmp_dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(tmp_dir);
    //QFile file(QString("%1%2").arg(dir.path()).arg("/rose.db"));
    QString tmp_path = dir.path() + "/rose.db";
    QFile file(tmp_path);
    file.remove();

    //emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::RoseHome));//c220826_2
    //emit linker->signal_clickedRemoteEXTE();//c220715
    //c220826_2
    //if(db_downloadCompleting_flag||(this->currMainMenuCode == QString(GSCommon::MainMenuCode::Music))  (this->currMainMenuCode == QString(GSCommon::MainMenuCode::Setting))){
    //if((this->currMainMenuCode == QString(GSCommon::MainMenuCode::Music))  (this->currMainMenuCode == QString(GSCommon::MainMenuCode::Setting))){
    if( (this->currMainMenuCode == QString(GSCommon::MainMenuCode::Setting))){//c230409
        print_debug();
        this->progress->show();//
        //this->progress_pop->show();
    }

    // 대용량 테스트용
    //FileDownloader *fileDownLoader = new FileDownloader(QString("http://inkeldev2.seye.co.kr/admin/soskim/external.db"), this);
    if(this->fileDownLoader == nullptr){
        print_debug();
        this->fileDownLoader = new FileDownloader(QString("http://%1:9286/media.provider.get").arg(global.device.getDeviceIP()), this);//c230228
        print_debug();

    }else{
        print_debug();
        disconnect(this->fileDownLoader, SIGNAL(signal_downloading(qint64, qint64)), this, SLOT(downloadingDB(qint64,qint64)));
        disconnect(this->fileDownLoader, SIGNAL(downloaded()), this, SLOT(downloadedDB()));
        delete this->fileDownLoader;
        this->fileDownLoader = nullptr;
        this->fileDownLoader = new FileDownloader(QString("http://%1:9286/media.provider.get").arg(global.device.getDeviceIP()), this);

        print_debug();
    }

    connect(this->fileDownLoader, SIGNAL(signal_downloading(qint64, qint64)), this, SLOT(downloadingDB(qint64,qint64)));
    connect(this->fileDownLoader, SIGNAL(downloaded()), this, SLOT(downloadedDB()));
    global.device.setIsDbScanning(true);
    db_downloadCompleting_flag = true;//c220826_2
    global.db_downloadComplete_flag = false;;//c220826_2
    print_debug();
    qDebug() << "++++++++++++++++++++++++++++++++++++++++++++++++++++global.db_downloadComplete_flag=" << global.db_downloadComplete_flag;
}


/**
 * @brief HomeMain::downloadingDB : DB 다운로드 프로그레스바 업데이트
 * @param p_currByte
 * @param p_totalByte
 */
void HomeMain::downloadingDB(qint64 p_currByte, qint64 p_totalByte){//c220826_2


    global.db_downloadComplete_flag = false;
    //qDebug() << "p_currByte=" << p_currByte;
    //qDebug() << "p_totalByte=" << p_totalByte;
    global.musicDB_DownloadingState = ((float)p_currByte/(float)p_totalByte) *100.0;
    if( this->currMainMenuCode == QString(GSCommon::MainMenuCode::Music) || (this->currMainMenuCode == QString(GSCommon::MainMenuCode::Setting))){
        if(p_currByte > this->p_preByte){
            this->progress->setCurrentValue(p_currByte, p_totalByte);
            //this->progress_pop->setCurrentValue(p_currByte, p_totalByte);

            this->p_preByte = p_currByte;
            if(this->progress->isHidden()){
                this->progress->show();
            }
            //if(this->progress_pop->isHidden()){
            //this->progress_pop->show();
            //this->progress_pop->raise();
            //}
        }else{
            // global.db_downloadComplete_flag = true;;//c220826_1
        }
    }
}


/**
 * @brief HomeMain::downloadedDB : [슬롯] DB 다운로드 완료
 * @details : 대용량 파일의 경우 파일 저장 Delay 있음
 * Thread 작업 필요[추가작업]
 * DB 파일 다운로드후 View를 Table로 저장함.
 * (메모리 DB 미사용으로 ROSE 가 전달한 DB 파일의 가상테이블 쿼리속도 낮음)
 */
void HomeMain::downloadedDB(){

    this->p_preByte = 0;
    //FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());//c230228
    QByteArray byteArray = this->fileDownLoader->downloadedData();//c230228

    // Dir 확인 및 생성
    /*
    QDir dir(qApp->applicationDirPath()+"/config");
    if(!dir.exists()){
        dir.mkpath(qApp->applicationDirPath()+"/config");
    }
    */
    //saveCurrentDeviceData(global.device);//c230423
    oldDeviceID = global.device.getDeviceID();
    QString tmp_dirPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(tmp_dirPath);
    //qDebug() << dir.path() << "\n";

    //c230414_start -----------------------------------------------
    QDialog *dialog = new QDialog();
    if(global.window_activate_flag) {//c221001_1
        if(global.powerDialogShowFlag) return;
        //print_debug();
        QLabel *lb_msg = new QLabel();
        lb_msg->setText("The current Music DB file is being changed.\n Pleasure wait a minute.");
        lb_msg->setAlignment(Qt::AlignCenter);

        QHBoxLayout *hl_msgBox = new QHBoxLayout();
        hl_msgBox->setContentsMargins(0,0,0,0);
        hl_msgBox->setSpacing(0);
        hl_msgBox->setContentsMargins(35,20,35,15);
        hl_msgBox->addStretch(1);
        hl_msgBox->addWidget(lb_msg);
        hl_msgBox->addStretch(1);
        QWidget *widget_msgBox = new QWidget();
        widget_msgBox->setStyleSheet("background-color:#B18658;color:#FFFFFF;font-size:20px;border-radius:5px;");
        widget_msgBox->setLayout(hl_msgBox);

        QVBoxLayout *vl_total = new QVBoxLayout();
        vl_total->setContentsMargins(0,0,0,0);
        vl_total->setSpacing(0);
        vl_total->addWidget(widget_msgBox, 0, Qt::AlignCenter);



        dialog->setModal(true);
        dialog->setStyleSheet("background-color:#55FF0000;border-radius:20px;");
        dialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);//c221001_1
        dialog->setAttribute(Qt::WA_TranslucentBackground);
        dialog->setLayout(vl_total);
        dialog->show();
        //dialog->raise();
        int left = global.left_mainwindow+global.width_mainwindow/2- (dialog->sizeHint().width() / 2);//c220804
        int top = global.top_mainwindow+global.height_mainwindow/2 ;//- (dialog->sizeHint().height() / 2);//c220804
        dialog->move(left, top);//c220804
    }
    //c230414_end -----------------------------------------------------------

    // 파일 저장
    QFile file(QString("%1%2").arg(dir.path()).arg("/tmp_rose.db"));
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(byteArray);
        file.close();
    }
    else{
        print_debug();
        qDebug() << "Error opening file:" << file.errorString();
        ToastMsg::show(this, "", QString(tr("Error opening file : tmp_rose.db.")));//c221028
    }

//    QFile file(QString("%1%2").arg(dir.path()).arg("/tmp_rose.db"));
//    file.open(QIODevice::WriteOnly);
//    file.write(byteArray);
//    print_debug();
//    file.close();
//    print_debug();

    dialog->hide();//c230414

   // ToastMsg::show(this, "", QString(tr("DB download complete.")));//c221028

    // View TO Table
    SqliteViewToTable *viewToTable = new SqliteViewToTable();
    connect(viewToTable, SIGNAL(signal_finished()), SLOT(slot_completedViewToTable()));
    emit viewToTable->signal_viewToTable();

    db_downloadCompleting_flag = false;//c220826_2
    global.signal_selectedDevice_flag = false;//c220826_2
    global.db_downloadComplete_flag = true;//c220826_2
    print_debug();
    qDebug() << "global.db_downloadComplete_flag=" << global.db_downloadComplete_flag;
    qDebug() << "3-db_downloadCompleting_flag=" << db_downloadCompleting_flag;

    if(dlgConfirmMusicDbDownloadingNotice != nullptr){//7777
        if(!dlgConfirmMusicDbDownloadingNotice->isHidden()){
            dlgConfirmMusicDbDownloadingNotice->accept();
        }
    }

    //saveCurrentDeviceData(global.device);//
    print_debug();
    //this->fileDownLoader->deleteLater();//c230228
    print_debug();
}


/**
 * @brief HomeMain::slot_completedViewToTable:[슬롯]DB View To Table
 */
void HomeMain::slot_completedViewToTable(){//c230118

    sender()->deleteLater();
    print_debug();
    QString tmp_dirPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(tmp_dirPath);
    QFile file(QString("%1%2").arg(dir.path()).arg("/rose.db"));

    if(!file.exists() || file.size() == 0){
        print_debug();
        file.close();
        this->progress->hide();
        //this->progress_pop->hide();
        slot_clickedDBRefreshBtn();

        // 자동로그인 확인 및 실행
        if(!global.user.isValid()){//c221116
            //   this->requestGetRoseUserInfo();//c221116
        }

        if(this->currMainMenuCode == QString(GSCommon::MainMenuCode::Music)){
            this->currMainMenuCode = nullptr;
            changedLeftMainMenu(QString(GSCommon::MainMenuCode::Music));
        }
        emit linker->signal_finishedViewToTable();
    }
    else{
        //c230118
        print_debug();
        this->progress->showRefreshBtn();
        //this->progress_pop->showRefreshBtn();
    }
}


void HomeMain::resizeEvent(QResizeEvent *event){

    Q_UNUSED(event);

    this->queueplaylist->setFixedHeight(this->size().height() - this->sectionBottom->height() - 80);

    // DB 프로그레스 geometry 조정
    /*int tmp_progressWidth = 500;
    if((this->width()-sectionLeft->width()-570) < 660){
        tmp_progressWidth = this->width()-sectionLeft->width()-750;
    }
    this->progress->setGeometry(sectionLeft->width()+160,7,tmp_progressWidth,55);*/

    // DB 프로그레스 geometry 조정
    int tmp_progressWidth = 300;
    int tmp_progressLeft = this->width() - tmp_progressWidth  - 80;

    if(this->width() > 1319){
        if(this->progress->width() < 490 && this->flag_db_prograssbar == true){
            tmp_progressWidth = tmp_progressWidth + ((this->width() - 1024) * 0.3);
            tmp_progressLeft = this->width() - tmp_progressWidth  - 80;
        }
        else{
            tmp_progressWidth = 490;
            tmp_progressLeft = this->width() - tmp_progressWidth  - 80;

            if(this->flag_db_prograssbar == false){
                this->flag_db_prograssbar = true;
            }
        }
    }
    else if(this->width() < 1300){
        tmp_progressWidth = 300;
        tmp_progressLeft = this->width() - tmp_progressWidth  - 80;
    }

    this->progress->setGeometry(tmp_progressLeft, TOP_NAVIBAR_H - 10, tmp_progressWidth, 55);
}


/**
 * @brief HomeMain::slot_searchSuggestionsChanged : [슬롯] 검색 제안어 변경
 * @param p_jsonArray
 */
void HomeMain::slot_searchSuggestionsChanged(const QJsonObject &p_jsonObject){



    if(p_jsonObject.isEmpty()){
        this->suggestionBar->hide();
    }else{
        if(p_jsonObject.contains("array")){
            QJsonArray jsonArrTotal = p_jsonObject["array"].toArray();

            if(jsonArrTotal.size() > 1){
                QJsonArray jsonSuggestions = jsonArrTotal.at(1).toArray();

                if(jsonSuggestions.size() > 0){
                    suggestionBar->setSuggestions(jsonSuggestions);
                    suggestionBar->setGeometry(this->width()-577, TOP_NAVIBAR_H, 500, this->height()-TOP_NAVIBAR_H-115);
                    suggestionBar->show();
                    suggestionBar->raise();
                }else{
                    suggestionBar->hide();
                }
            }
        }
    }
}


/**
 * @brief HomeMain::slot_search : [슬롯] 검색 요청
 * @param p_text
 * @note 검색 페이지 이동은 각 메뉴 main에서 처리
 */
void HomeMain::slot_search5555(const QString &p_text){//c220904_1

    print_debug();
    //c220619-search
    SqliteHelper *sqliteHelperSel = new SqliteHelper();
    QSqlError err = sqliteHelperSel->addConnectionLocal();

    if(err.type() == QSqlError::NoError){
        print_debug();
        QString strQuery_select = "SELECT  *";
        strQuery_select += " FROM ";
        strQuery_select += " searchPreList";
        QVariantList data = QVariantList();
        sqliteHelperSel->exec(strQuery_select, data);
        if(data.size() > 0){
            qDebug() << "data.size()=" << data.size();
        }else{

        }
        //c220619-search
        print_debug();

        //QString strQuery_delete = "SELECT  *";//c220818
        QString strQuery_delete = "DELETE";//c220818
        strQuery_delete += " FROM ";
        strQuery_delete += " searchPreList";
        strQuery_delete += QString(" WHERE (search_text == '%1'").arg(p_text);
        strQuery_delete += ")";
        //qDebug() << "strQuery_delete=" << strQuery_delete;

        SqliteHelper *sqliteHelperDel = new SqliteHelper();
        QSqlError err = sqliteHelperDel->addConnectionLocal();
        QVariantList datad = QVariantList();
        if(err.type() == QSqlError::NoError){
            datad = QVariantList();
            sqliteHelperDel->exec(strQuery_delete, datad);
            // print_debug();
            //qDebug() << datad;
        }
        sqliteHelperDel->close();
        delete sqliteHelperDel;
        if(datad.size() > 0){
            qDebug() << "datad.size()=" << datad.size();
        }else{
            print_debug();
            if(p_text.size() > 0 && !p_text.contains("https://link.roseaudio.kr/connect/")){//c220901_1
                // SqliteHelper *sqliteHelper = new SqliteHelper();
                QString strQuery_insert = "INSERT INTO searchPreList (search_text) ";
                strQuery_insert += " VALUES ";
                strQuery_insert += " ('" + p_text + "'";
                strQuery_insert += ") ";
                sqliteHelperSel->exec(strQuery_insert);
                //c220619-search
            }

        }

    }
    sqliteHelperSel->close();
    delete sqliteHelperSel;

    emit linker->signal_clickListBtn();//c220726
    emit linker->signal_text_copy();//c220726

    qDebug() << "HomeMain::slot_search---p_text=" << p_text;
    qDebug() << "HomeMain::slot_search---this->currMainMenuCode=" << this->currMainMenuCode;
    if(this->curr_absMainContent!=nullptr && this->currMainMenuCode != QString(GSCommon::MainMenuCode::RoseHome)
            && this->currMainMenuCode != QString(GSCommon::MainMenuCode::CDplay)){//c220721
        print_debug();
        qDebug() << "p_text=" << p_text;
        global.search_text = p_text;//searchtextcopy
        //this->curr_absMainContent->process_searchWord(p_text);
    }else{//c220721
        global.search_text = p_text;//searchtextcopy
        slot_clickedMovePageMusicSearch();
    }

    //connect(linker, SIGNAL(signal_clickedHoverItem(QString, QJsonObject)), this, SLOT(slot_clickedHoverItem(QString, QJsonObject)));
    //connect(linker, SIGNAL(signal_clickedMovePageMusicSearch()), this, SLOT(slot_clickedMovePageMusicSearch(QString)));

    /*connect(linker, SIGNAL(signal_clickedMovePageMusicSearch()), this, SLOT(slot_clickedMovePageMusicSearch()));
    connect(linker, SIGNAL(signal_clickedMovePageVideoSearch()), this, SLOT(slot_clickedMovePageVideoSearch()));
    connect(linker, SIGNAL(signal_clickedMovePageRadioSearch()), this, SLOT(slot_clickedMovePageRadioSearch()));
    connect(linker, SIGNAL(signal_clickedMovePageRoseTubeSearch()), this, SLOT(slot_clickedMoveRoseTubeSearch()));
    connect(linker, SIGNAL(signal_clickedMovePagePodCastSearch()), this, SLOT(slot_clickedMovePagePodCastSearch()));

    connect(linker, SIGNAL(signal_clickedMovePageTidalSearch()), this, SLOT(slot_clickedMovePageTidalSearch()));
    connect(linker, SIGNAL(signal_clickedMovePageBugsSearch()), this, SLOT(slot_clickedMovePageBugsSearch()));
    connect(linker, SIGNAL(signal_clickedMovePageQobuzSearch()), this, SLOT(slot_clickedMovePageQobuzSearch()));*/
}


void HomeMain::slot_clickedMovePageMusicSearch(){//cheon211008-search-----------------------------

    //print_debug();
    //global.searchMenuFlag = true;
    //changedLeftMainMenu_search(QString(GSCommon::MainMenuCode::Music));//

    if(global.search_text.size() >= 2){

        global.user.flag_music = true;

        global.user.music_obj = QJsonObject();
        global.user.music_obj.insert(KEY_PAGE_CODE, PAGECODE_M_SEARCH);

        common::SearchData data_search;
        data_search.search_word = global.search_text;
        QJsonObject tmp_data = common::ConvertData_common::getObjectJson_searchData(data_search);

        global.user.music_obj.insert(KEY_DATA, tmp_data);

        emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Music));
    }
    else{
        ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
    }
}


void HomeMain::slot_clickedMovePageVideoSearch(){//cheon211008-search-----------------------------

    print_debug();
    changedLeftMainMenu_search(QString(GSCommon::MainMenuCode::Video));//
    //changedLeftMainMenu(QString(GSCommon::MainMenuCode::Bugs));//
}


void HomeMain::slot_clickedMovePageRadioSearch(){//cheon211008-search-----------------------------

    print_debug();
    changedLeftMainMenu_search(QString(GSCommon::MainMenuCode::Radio));//
    //changedLeftMainMenu(QString(GSCommon::MainMenuCode::Bugs));//
}


void HomeMain::slot_clickedMovePageRoseRadioSearch(){

    //print_debug();
    //changedLeftMainMenu_search(QString(GSCommon::MainMenuCode::RoseTube));

    if(global.search_text.size() >= 2){

        global.user.flag_roseradio = true;

        global.user.roseradio_obj = QJsonObject();
        global.user.roseradio_obj.insert(KEY_PAGE_CODE, PAGECODE_RR_SEARCHMAIN);

        common::SearchData data_search;
        data_search.search_word = global.search_text;
        QJsonObject tmp_data = common::ConvertData_common::getObjectJson_searchData(data_search);

        global.user.roseradio_obj.insert(KEY_DATA, tmp_data);

        emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::RoseRadio));
    }
    else{
        ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
    }
}


void HomeMain::slot_clickedMoveRoseTubeSearch(){//cheon211008-search-----------------------------
    //print_debug();
    //changedLeftMainMenu_search(QString(GSCommon::MainMenuCode::RoseTube));

    if(global.search_text.size() >= 2){

        global.user.flag_rosetube = true;

        global.user.rosetube_obj = QJsonObject();
        global.user.rosetube_obj.insert(KEY_PAGE_CODE, PAGECODE_RT_SEARCH);

        common::SearchData data_search;
        data_search.search_word = global.search_text;
        QJsonObject tmp_data = common::ConvertData_common::getObjectJson_searchData(data_search);

        global.user.rosetube_obj.insert(KEY_DATA, tmp_data);

        emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::RoseTube));
    }
    else{
        ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
    }
}


void HomeMain::slot_clickedMovePagePodCastSearch(){//cheon211008-search-----------------------------

    print_debug();
    changedLeftMainMenu_search(QString(GSCommon::MainMenuCode::PodCast));//
    //changedLeftMainMenu(QString(GSCommon::MainMenuCode::Bugs));//
}


void HomeMain::slot_clickedMovePageTidalSearch(){//cheon211008-search-----------------------------

    //print_debug();
    //changedLeftMainMenu_search(QString(GSCommon::MainMenuCode::Tidal));//

    if(global.search_text.size() >= 2){

        global.user_forTidal.flag_rosehome = true;

        global.user_forTidal.rosehome_obj = QJsonObject();
        global.user_forTidal.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_T_SEARCH_MAIN);

        common::SearchData data_search;
        data_search.search_word = global.search_text;
        QJsonObject tmp_data = common::ConvertData_common::getObjectJson_searchData(data_search);

        global.user_forTidal.rosehome_obj.insert(KEY_DATA, tmp_data);

        emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Tidal));
    }
    else{
        ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
    }
}



void HomeMain::slot_clickedMovePageBugsSearch(){//cheon211008-search-----------------------------

    //print_debug();
    //changedLeftMainMenu_search(QString(GSCommon::MainMenuCode::Bugs));//

    if(global.search_text.size() >= 2){

        global.user_forBugs.flag_rosehome = true;

        global.user_forBugs.rosehome_obj = QJsonObject();
        global.user_forBugs.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_BUGS___SEARCH_MAIN);

        common::SearchData data_search;
        data_search.search_word = global.search_text;
        QJsonObject tmp_data = common::ConvertData_common::getObjectJson_searchData(data_search);

        global.user_forBugs.rosehome_obj.insert(KEY_DATA, tmp_data);

        emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Bugs));
    }
    else{
        ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
    }
}


void HomeMain::slot_clickedMovePageQobuzSearch(){//cheon211008-search-----------------------------

    //print_debug();
    //changedLeftMainMenu_search(QString(GSCommon::MainMenuCode::Qobuz));//

    if(global.search_text.size() >= 2){

        global.user_forQobuz.flag_rosehome = true;

        global.user_forQobuz.rosehome_obj = QJsonObject();
        global.user_forQobuz.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_SEARCH_MAIN);

        common::SearchData data_search;
        data_search.search_word = global.search_text;
        QJsonObject tmp_data = common::ConvertData_common::getObjectJson_searchData(data_search);

        global.user_forQobuz.rosehome_obj.insert(KEY_DATA, tmp_data);

        emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Qobuz));
    }
    else{
        ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
    }
}

void HomeMain::slot_msg(){

    if(!global.window_activate_flag){//c230127
        global.window_activate_flag = true;
        //ToastMsg::show(this, "", tr("Drag and drop the title at the bottom of the link and drop it on the screen."),3000);//c220824_4
        ToastMsg::show(this, "", tr("This function is being prepared for service."),3000);//c220824_4
        global.window_activate_flag = false;
    }else{
        ToastMsg::show(this, "", tr("This function is being prepared for service."),3000);//c220824_4

    }


}

void HomeMain::slot_clickedMovePageSearch_share(QString link){//c220823

    print_debug();
    qDebug() << "link=" << link;
    QString tmpStr = link;
    QStringList tmpLink = tmpStr.split("/");

    QString shareLink = tmpLink.at(2);
    global.search_text = shareLink;
    qDebug() << "global.search_text=" << global.search_text;
    //changedLeftMainMenu_search(QString(GSCommon::MainMenuCode::Tidal));//

    if(link.contains("ROSE_TOTAL")){

        //c220818
        if(link.contains("ALBUM")){
            if(global.search_text.size() >= 2){

                global.user.flag_rosehome = true;

                global.user.rosehome_obj = QJsonObject();
                global.user.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_RH_ALBUM_DETAIL);

                QJsonObject tmp_data;
                tmp_data.insert("id", global.search_text.toInt());

                global.user.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::RoseHome));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("PLAYLIST")){
            print_debug();
            if(global.search_text.size() >= 2){

                global.user.flag_rosehome = true;

                global.user.rosehome_obj = QJsonObject();
                global.user.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_RH_PLAYLIST_ROSE_DETAIL);

                QJsonObject tmp_data;
                tmp_data.insert("id", global.search_text.toInt());

                global.user.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::RoseHome));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("TRACK")){
            print_debug();
            qDebug() << "global.search_text.toInt()=" << global.search_text.toInt();
            if(global.search_text.size() >= 2){

                global.user.flag_rosehome = true;

                global.user.rosehome_obj = QJsonObject();
                global.user.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_RH_TRACK_SHARE_LIST_ALL_VIEW);

                QJsonObject tmp_data;
                tmp_data.insert("type", tmpLink.at(0));
                tmp_data.insert("id", global.search_text.toInt());

                global.user.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::RoseHome));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }else{
            emit linker->signal_checkQueue(34, "");//c220824-1

        }
    }
    else if(link.contains("ROSE_TUBE")){
        if(link.contains("ALBUM")){
            print_debug();
            if(global.search_text.size() >= 2){
                /*
                global.user.flag_rosetube = true;

                global.user.rosetube_obj = QJsonObject();
                global.user.rosetube_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_ALBUM_DETAIL);

                QJsonObject tmp_data;
                tmp_data.insert("id", global.search_text);

                global.user.rosetube_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::RoseTube));
                */
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("PLAYLIST")){
            print_debug();
            if(global.search_text.size() >= 2){
                global.user.flag_rosetube = true;

                global.user.rosetube_obj = QJsonObject();
                global.user.rosetube_obj.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYDETAIL);//PAGECODE_RH_PLAYLIST_ROSE_DETAIL);

                QJsonObject tmp_data;
                tmp_data.insert("id", global.search_text.toInt());

                global.user.rosetube_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::RoseTube));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("TRACK")){
            print_debug();
            qDebug() << "global.search_text.toInt()=" << global.search_text.toInt();
            if(global.search_text.size() >= 2){

                global.user.flag_rosetube = true;

                global.user.rosetube_obj = QJsonObject();
                global.user.rosetube_obj.insert(KEY_PAGE_CODE, PAGECODE_VA_TRACK_SHARE_LIST);

                if(tmpStr.contains("https://")){
                    global.search_text = tmpStr;
                    global.search_text.replace("ROSE_TUBE/TRACK/", "");
                }

                QJsonObject tmp_data;
                tmp_data.insert("type", tmpLink.at(0));
                tmp_data.insert("id", global.search_text);

                global.user.rosetube_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::RoseTube));

                /*global.user.flag_rosetube = true;

                global.user.rosetube_obj = QJsonObject();
                global.user.rosetube_obj.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYDETAIL);//PAGECODE_RH_PLAYLIST_ROSE_DETAIL);

                QJsonObject tmp_data;
                tmp_data.insert("id", global.search_text.toInt());

                global.user.rosetube_obj.insert(KEY_DATA, tmp_data);

                //emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::RoseTube));*/
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }else{
            emit linker->signal_checkQueue(34, "");//c220824-1
        }
    }
    else if(link.contains("TIDAL")){
        if(link.contains("ALBUM")){
            print_debug();
            if(global.search_text.size() >= 2){

                global.user_forTidal.flag_rosehome = true;

                global.user_forTidal.rosehome_obj = QJsonObject();
                global.user_forTidal.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_T_ALBUM_DETAIL);

                QJsonObject tmp_data;
                tmp_data.insert("id", global.search_text.toInt());

                global.user_forTidal.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Tidal));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("PLAYLIST")){
            print_debug();
            if(global.search_text.size() >= 2){

                global.user_forTidal.flag_rosehome = true;

                global.user_forTidal.rosehome_obj = QJsonObject();
                global.user_forTidal.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_T_PLAYLIST_DETAIL);

                QJsonObject tmp_data;
                if(global.search_text.toInt()>0){//c220826_2
                    tmp_data.insert("id", global.search_text.toInt());
                }else{
                    tmp_data.insert("uuid", global.search_text);
                }

                global.user_forTidal.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Tidal));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("TRACK")){
            print_debug();
            qDebug() << "global.search_text.toInt()=" << global.search_text.toInt();
            if(global.search_text.size() >= 2){
                global.user_forTidal.flag_rosehome = true;

                global.user_forTidal.rosehome_obj = QJsonObject();
                global.user_forTidal.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_T_TRACK_SHARE_ALL_LIST_VIEW);

                QJsonObject tmp_data;
                tmp_data.insert("type", tmpLink.at(0));
                tmp_data.insert("subType", tmpLink.at(1));
                tmp_data.insert("id", global.search_text.toInt());

                global.user_forTidal.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Tidal));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("VIDEO")){
            print_debug();
            qDebug() << "global.search_text.toInt()=" << global.search_text.toInt();
            if(global.search_text.size() >= 2){
                global.user_forTidal.flag_rosehome = true;

                global.user_forTidal.rosehome_obj = QJsonObject();
                global.user_forTidal.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_T_TRACK_SHARE_ALL_LIST_VIEW);

                QJsonObject tmp_data;
                tmp_data.insert("type", tmpLink.at(0));
                tmp_data.insert("subType", tmpLink.at(1));
                tmp_data.insert("id", global.search_text.toInt());

                global.user_forTidal.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Tidal));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("ARTIST")){
            print_debug();
            qDebug() << "global.search_text.toInt()=" << global.search_text.toInt();
            if(global.search_text.size() >= 2){
                global.user_forTidal.flag_rosehome = true;

                global.user_forTidal.rosehome_obj = QJsonObject();
                global.user_forTidal.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_T_ARTIST_DETAIL);

                QJsonObject tmp_data;
                tmp_data.insert("id", global.search_text.toInt());

                global.user_forTidal.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Tidal));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else{
            emit linker->signal_checkQueue(34, "");//c220824-1

        }
    }
    else if(link.contains("BUGS")){
        if(link.contains("ALBUM")){
            print_debug();
            if(global.search_text.size() >= 2){

                global.user_forBugs.flag_rosehome = true;

                global.user_forBugs.rosehome_obj = QJsonObject();
                global.user_forBugs.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ALBUM_DETAIL);

                QJsonObject tmp_data;
                tmp_data.insert("album_id", global.search_text.toInt());
                int tt = global.search_text.toInt();
                qDebug() << "global.search_text.toInt()=" << tt;

                global.user_forBugs.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Bugs));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("PLAYLIST")){
            if(global.search_text.size() >= 2){

                global.user_forBugs.flag_rosehome = true;

                global.user_forBugs.rosehome_obj = QJsonObject();
                global.user_forBugs.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_BUGS__MY_ROSE_PLAYLIST_DETAIL);

                QJsonObject tmp_data;
                tmp_data.insert("id", global.search_text.toInt());

                global.user_forBugs.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Bugs));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("TRACK")){
            print_debug();
            qDebug() << "global.search_text.toInt()=" << global.search_text.toInt();
            if(global.search_text.size() >= 2){

                global.user_forBugs.flag_rosehome = true;

                global.user_forBugs.rosehome_obj = QJsonObject();
                global.user_forBugs.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_BUGS___TRACK_SHARE_ALL_VIEW);

                QJsonObject tmp_data;
                tmp_data.insert("type", tmpLink.at(0));
                tmp_data.insert("subType", tmpLink.at(1));
                tmp_data.insert("id", global.search_text.toInt());

                global.user_forBugs.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Bugs));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("mv")){
            print_debug();
            qDebug() << "global.search_text.toInt()=" << global.search_text.toInt();
            if(global.search_text.size() >= 2){

                global.user_forBugs.flag_rosehome = true;

                global.user_forBugs.rosehome_obj = QJsonObject();
                global.user_forBugs.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_BUGS___TRACK_SHARE_ALL_VIEW);

                QJsonObject tmp_data;
                tmp_data.insert("type", tmpLink.at(0));
                tmp_data.insert("subType", tmpLink.at(1));
                tmp_data.insert("id", global.search_text.toInt());

                global.user_forBugs.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Bugs));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("ARTIST")){
            print_debug();
            qDebug() << "global.search_text.toInt()=" << global.search_text.toInt();
            if(global.search_text.size() >= 2){
                global.user_forBugs.flag_rosehome = true;

                global.user_forBugs.rosehome_obj = QJsonObject();
                global.user_forBugs.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ARTIST_DETAIL);

                QJsonObject tmp_data;
                tmp_data.insert("artist_id", global.search_text.toInt());

                global.user_forBugs.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Bugs));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else{
            emit linker->signal_checkQueue(34, "");//c220824-1

        }
    }
    else if(link.contains("QOBUZ")){
        if(link.contains("ALBUM")){
            print_debug();
            if(global.search_text.size() >= 2){

                global.user_forQobuz.flag_rosehome = true;

                global.user_forQobuz.rosehome_obj = QJsonObject();
                global.user_forQobuz.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_ALBUM_DETAIL);

                QJsonObject tmp_data;
                tmp_data.insert("id", global.search_text);

                global.user_forQobuz.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Qobuz));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("PLAYLIST")){
            print_debug();
            if(global.search_text.size() >= 2){

                global.user_forQobuz.flag_rosehome = true;

                global.user_forQobuz.rosehome_obj = QJsonObject();
                global.user_forQobuz.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_PLAYLIST_DETAIL);//PAGECODE_RH_PLAYLIST_ROSE_DETAIL);

                QJsonObject tmp_data;
                tmp_data.insert("id", global.search_text);//.toInt());

                global.user_forQobuz.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Qobuz));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("TRACK")){
            print_debug();
            qDebug() << "global.search_text.toInt()=" << global.search_text.toInt();
            if(global.search_text.size() >= 2){
                global.user_forQobuz.flag_rosehome = true;

                global.user_forQobuz.rosehome_obj = QJsonObject();
                global.user_forQobuz.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_TRACK_SHARE_ALL_LIST_VIEW);

                QJsonObject tmp_data;
                tmp_data.insert("type", tmpLink.at(0));
                tmp_data.insert("id", global.search_text.toInt());

                global.user_forQobuz.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Qobuz));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("ARTIST")){
            print_debug();
            qDebug() << "global.search_text.toInt()=" << global.search_text.toInt();
            if(global.search_text.size() >= 2){
                global.user_forQobuz.flag_rosehome = true;

                global.user_forQobuz.rosehome_obj = QJsonObject();
                global.user_forQobuz.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_ARTIST_DETAIL);

                QJsonObject tmp_data;
                tmp_data.insert("id", global.search_text.toInt());

                global.user_forQobuz.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Qobuz));
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else{
            emit linker->signal_checkQueue(34, "");//c220824-1

        }
    }
    else if(link.contains("APPLE_MUSIC")){
        if(link.contains("ALBUM")){
            print_debug();
            if(global.search_text.size() >= 2){

                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
                /*global.user_forApple.flag_rosehome = true;

                global.user_forApple.rosehome_obj = QJsonObject();
                global.user_forApple.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_APPLE_ALBUM_DETAIL);

                QJsonObject tmp_data;
                tmp_data.insert("id", global.search_text);

                global.user_forApple.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::AppleMusic));*/
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("PLAYLIST")){
            print_debug();
            if(global.search_text.size() >= 2){

                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));

                /*global.user_forQobuz.flag_rosehome = true;

                global.user_forQobuz.rosehome_obj = QJsonObject();
                global.user_forQobuz.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_PLAYLIST_DETAIL);//PAGECODE_RH_PLAYLIST_ROSE_DETAIL);

                QJsonObject tmp_data;
                tmp_data.insert("id", global.search_text);//.toInt());

                global.user_forQobuz.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Qobuz));*/
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }
        else if(link.contains("TRACK")){
            print_debug();
            qDebug() << "global.search_text.toInt()=" << global.search_text.toInt();
            if(global.search_text.size() >= 2){

                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));

                /*global.user_forQobuz.flag_rosehome = true;

                global.user_forQobuz.rosehome_obj = QJsonObject();
                global.user_forQobuz.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_TRACK_SHARE_ALL_LIST_VIEW);

                QJsonObject tmp_data;
                tmp_data.insert("type", tmpLink.at(0));
                tmp_data.insert("id", global.search_text.toInt());

                global.user_forQobuz.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Qobuz));*/
            }
            else{
                ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            }
        }else{
            emit linker->signal_checkQueue(34, "");//c220824-1

        }
    }
    else{
        emit linker->signal_checkQueue(34, "");//c220824-4
    }
}




void HomeMain::slot_MovePageSearch(){//cheon211008-search-----------------------------

    print_debug();

    if(this->curr_absMainContent!=nullptr){
        print_debug();
        //global.search_text = p_text;//searchtextcopy
        this->curr_absMainContent->process_searchWord(global.search_text);
    }
    qDebug() << "HomeMain::slot_MovePageSearch---global.search_text" << global.search_text;
}


void HomeMain::slot_MovePageSearchMusic(){//cheon211008-search-----------------------------

    print_debug();

    if(this->curr_absMainContent!=nullptr){
        print_debug();
        this->musicSearch = new MusicSearch();
        //global.search_text = p_text;//searchtextcopy
        //this->curr_absMainContent->process_searchWord(global.search_text);
        this->musicSearch->musicSearch(global.search_text);
        //emit linker->signal_clickListBtn();//c220726
    }
    qDebug() << "HomeMain::slot_MovePageSearchMusic---global.search_text" << global.search_text;
}


void HomeMain::slot_MovePageSearchPodCast(){//cheon211008-search-----------------------------

    print_debug();
}


void HomeMain::slot_MovePageSearchVideo(){//cheon211008-search-----------------------------

    print_debug();
}


void HomeMain::slot_MovePageSearchRadio(){//cheon211008-search-----------------------------

    print_debug();
}


void HomeMain::slot_MovePageRoseHomeTo(const QString leftmenu){

    print_debug();
    //this->unRaiseAllWidget();//c230228
    this->sectionLeft->clickedMenu(leftmenu);
}


/**
 * @brief HomeMain::slot_changedLoginInfo [SLOT] 로그인 정보가 변경된 경우 처리 (로그인 혹은 로그아웃)
 */
void HomeMain::slot_changedLoginInfo(){

    this->setDataLoginAfter();
}


/**
 * @brief HomeMain::slot_changedDevicePower [슬롯] 장치 전원상태 변경
 */
void HomeMain::slot_changedDevicePower(){//c221115

    //print_debug();
    //emit linker->signal_checknetwork();
    slot_rebooting();

    /*emit linker->signal_searchBar_clearfocus();//c230220
    requestGetRoseUserInfo();

    this->checkTidalLogin();
    this->checkBugsLogin();
    this->checkQobuzLogin();
    this->checkAppleLogin();       //c221116         // Apple Musci Login check -> added by jeon
    */
}


/**
 * @brief HomeMain::setDataLoginAfter 로그인정보 세팅 후에,, 홈에서 처리해야하는 것들..
 */
void HomeMain::setDataLoginAfter(){

    this->sectionLeft->requestPlayList();
}


/**
 * @brief HomeMain::slot_changedSubMenu 각 메인메뉴의 서브메뉴가 변경된 경우..
 */
void HomeMain::slot_changedSubMenu(){

    // 띄워진 객체 모두 hide
    this->unRaiseAllWidget();
}


/**
 * @brief HomeMain::slot_changePagePlayList [SLOT] 각 메인메뉴에서의 플레이리스트 페이지 이동
 * @param p_jsonObject
 */
void HomeMain::slot_changePagePlayList(const QJsonObject &p_jsonObject){

    QString changedMainMenuCode = "";
    if(p_jsonObject.contains(KEY_MAIN_CODE)){
        changedMainMenuCode = QString(p_jsonObject[KEY_MAIN_CODE].toInt());
    }

    // 1) 메인메뉴 선택..
    if(this->currMainMenuCode!=changedMainMenuCode){
        print_debug();
        this->sectionLeft->clickedMenu(changedMainMenuCode);
    }

    // 2) 플레이리스트 페이지 이동
    if(this->hlayout_content->count() > 0){
        QWidget *w_currMainMenu = this->hlayout_content->itemAt(0)->widget();
        dynamic_cast<AbstractMainContent*>(w_currMainMenu)->goToMoveNewOrderPage(p_jsonObject);
    }
}


/**
 * @brief HomeMain::slot_changedMainMenuAndMovePage [SLOT] 해당 메인메뉴로 이동한 다음.. 해당 서브페이지 이동하라
 * @param p_mainMenuCode
 * @param p_jsonObject
 */
void HomeMain::slot_changedMainMenuAndMovePage(int p_mainMenuCode, const QJsonObject &p_jsonObject){

    // 해당 메인메뉴 이동
    if(this->currMainMenuCode!=QString(p_mainMenuCode)){
        print_debug();
        this->sectionLeft->clickedMenu(QString(p_mainMenuCode));
    }

    // 페이지 변경
    if(this->hlayout_content->count() > 0){
        QWidget *w_currMainMenu = this->hlayout_content->itemAt(0)->widget();
        dynamic_cast<AbstractMainContent*>(w_currMainMenu)->goToMoveNewOrderPage(p_jsonObject);
    }
}


/**
 * @brief HomeMain::slot_getCurrPlayData [SLOT] 현재 재생정보가 뭔지 알려줘.
 */
void HomeMain::slot_getCurrPlayData(){
    this->sectionBottom->emitSinalCurrPlayDataInfo();   // 현재 재생정보를 알려주는 시그널 발생
}


/**
 * @brief HomeMain::slot_clickedDBRefreshBtn : DB 새로고침 버튼 클릭
 */
void HomeMain::slot_clickedDBRefreshBtn(){

    // Dir 확인 및 생성
    /*
    QDir dir(qApp->applicationDirPath()+"/config");
    if(!dir.exists()){
        dir.mkpath(qApp->applicationDirPath()+"/config");
    }

    */
    //c230414_start -----------------------------------------------
    QDialog *dialog = new QDialog();
    if(global.window_activate_flag) {//c221001_1
        if(global.powerDialogShowFlag) return;
        //print_debug();
        QLabel *lb_msg = new QLabel();
        lb_msg->setText("The current Music DB file is being changed.\n Pleasure wait a minute.");
        lb_msg->setAlignment(Qt::AlignCenter);

        QHBoxLayout *hl_msgBox = new QHBoxLayout();
        hl_msgBox->setContentsMargins(0,0,0,0);
        hl_msgBox->setSpacing(0);
        hl_msgBox->setContentsMargins(35,20,35,15);
        hl_msgBox->addStretch(1);
        hl_msgBox->addWidget(lb_msg);
        hl_msgBox->addStretch(1);
        QWidget *widget_msgBox = new QWidget();
        widget_msgBox->setStyleSheet("background-color:#B18658;color:#FFFFFF;font-size:20px;border-radius:5px;");
        widget_msgBox->setLayout(hl_msgBox);

        QVBoxLayout *vl_total = new QVBoxLayout();
        vl_total->setContentsMargins(0,0,0,0);
        vl_total->setSpacing(0);
        vl_total->addWidget(widget_msgBox, 0, Qt::AlignCenter);



        dialog->setModal(true);
        dialog->setStyleSheet("background-color:#55FF0000;border-radius:20px;");
        dialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);//c221001_1
        dialog->setAttribute(Qt::WA_TranslucentBackground);
        dialog->setLayout(vl_total);
        dialog->show();
        //dialog->raise();
        int left = global.left_mainwindow+global.width_mainwindow/2- (dialog->sizeHint().width() / 2);//c220804
        int top = global.top_mainwindow+global.height_mainwindow/2 ;//- (dialog->sizeHint().height() / 2);//c220804
        dialog->move(left, top);//c220804
    }
    //c230414_end -----------------------------------------------------------

    QString tmp_dirPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(tmp_dirPath);
    QFile file_old(QString("%1%2").arg(dir.path()).arg("/rose.db"));

    if(file_old.exists()){
        file_old.remove();
    }

    file_old.close();//c230413
    print_debug();
    QFile file_new(QString("%1%2").arg(dir.path()).arg("/tmp_rose.db"));
    file_new.open(QIODevice::ReadWrite);
    if(file_new.exists()){
        file_new.copy(dir.path()+"/rose.db");
        //QCoreApplication::processEvents();//c230428_1
        //file_new.remove();//c230415
    }
    print_debug();
    //file_old.close();//c230415
    file_new.remove();//c230415
    file_new.close();
    dialog->hide();//c230414

//print_debug();
//    QFile file_new(QString("%1%2").arg(dir.path()).arg("/tmp_rose.db"));
//    file_new.open(QIODevice::ReadWrite);
//    if(file_new.exists()){
//        file_new.copy(dir.path()+"/rose.db");
//        file_new.remove();
//    }
//print_debug();

//    //file_old.close();//c230413
//    file_new.close();
//    dialog->hide();//c230414
    this->progress->hide();
    //this->progress_pop->hide();

    //QJsonObject tmp_data;
    //tmp_data[KEY_PAGE_CODE] = PAGECODE_M_HOME;
    //this->goToMoveNewOrderPage(tmp_data);
    //slot_changedMainMenuAndMovePage()

    // 장치연결 시그널 중계
    ToastMsg::show(this, "", QString(tr("DB download complete.")));//c221028
    if(global.signal_selectedDevice_flag){//c220826_2
        global.db_downloadComplete_flag = false;//c220826_2
        db_downloadCompleting_flag = false;//c220826_2
        print_debug();

    }
    //emit linker->signal_connected();//c230428_1
    emit linker->signal_completeDownloadDB();
    global.device.setIsDbScanning(false);
}


//void HomeMain::slot_clickedOpt_coaxpassGain(){
void HomeMain::showDlgOfNotice_Readme(){

    QMainWindow *full = new QMainWindow();

    full->showFullScreen();

    int left = full->geometry().width();
    int top = full->geometry().height();

    full->close();
    if(left >= 1330){
        left = (left - 900) / 2;
        top = (top - 800) / 2;

        this->dlgConfirmReadmeNotice = new DialogConfirm(this);
        this->dlgConfirmReadmeNotice->setFixedSize(900, 800);
        this->dlgNotice_Readme();
        this->dlgConfirmReadmeNotice->setGeometry(left, top, 900, 800);
        this->dlgConfirmReadmeNotice->setAlertMode();
        this->dlgConfirmReadmeNotice->setProperty("flagShown", false);

        QTimer::singleShot(500, this, SLOT(showDlgOfNotice_Readme_show()));
    }
}


void HomeMain::showDlgOfNotice_Readme_show(){

    if(this->dlgConfirmReadmeNotice->property("flagShown").toBool() == false){
        this->dlgConfirmReadmeNotice->setProperty("flagShown", true);

        int result = this->dlgConfirmReadmeNotice->exec();

        if(result == QDialog::Accepted){
            changedIndexNotice(1);
        }
        else{
            changedIndexNotice(0);
        }
    }

    //changedIndexNotice(0);
    /*QDesktopWidget desk;//cheon210728-test
    int px_width = desk.width();//cheon210728-test
    int px_height = desk.height();//cheon210728-test
    qDebug() << "HomeMain::showDlgOfNotice_Readme_show--px_width" << px_width<< " --px_height: " << px_height;

    this->dlgConfirmReadmeNotice->setGeometry(px_width-800,px_height-1100, 350, 100);//cheon210728-test
    this->dlgConfirmReadmeNotice->setAlertMode();
    this->dlgConfirmReadmeNotice->setProperty("flagShown",false);
    if(this->dlgConfirmReadmeNotice->property("flagShown").toBool()==false){
        this->dlgConfirmReadmeNotice->setProperty("flagShown",true);

        this->dlgConfirmReadmeNotice->exec();

        int result = this->dlgConfirmReadmeNotice->exec();

        if(result == QDialog::Accepted){
            changedIndexNotice(1);
        }else{
            changedIndexNotice(0);
        }
    }*/
}


void HomeMain::dlgNotice_Readme(){


    this->dlgConfirmReadmeNotice->setTitle(tr("ReadMe Notice!!!"));
    this->dlgConfirmReadmeNotice->setText(tr("Nice to meet you!\n"
                                             "\n"
                                             "Beyong Audio, HiFi 'Rose_Connect' If you have any problems\n"
                                             "or errors while using the PC/Mac App,\n"
                                             "please send the problem to the email below.\n"
                                             "\n"
                                             "- When installing for the first time, the basic English version is operated.\n"
                                             "- To use Korean, you can set up/language options/English-Korean in the menu. \n"
                                             "  After setting once, the program runs again and operates with the changed language version.\n"
                                             "\n"
                                             "(- 처음 인스톨 설치시에는 기본 영문 버변으로 동작합니다.\n"
                                             "- 한글 사용을 위해서는 메뉴에서 설정/언어옵션/영어-한글 설정을 할 수 있으며, \n"
                                             "  한번의 설정후 프로그램이 다시 실행되며 바뀐 언어 버젼으로 동작합니다.)\n"
                                             "\n"
                                             "\n"
                                             "This file is in the Rose_Connect/readme folder on your PC.\n"
                                             "\n"
                                             "    2021.02.10\n"
                                             "\n"
                                             "\n"
                                             "kevin@citech.kr(made in South Korea)"));
}


const QString QUERY_CREATE_TABLE = "CREATE TABLE IF NOT EXISTS notice (idx int, lang int)";
const QString QUERY_DROP_TABLE = "DROP TABLE notice";
const QString QUERY_DEL = "DELETE FROM notice";
const QString QUERY_SEL = "SELECT * FROM notice";

void HomeMain::read_noticeDBTable(){

    SqliteHelper *sqliteHelper = new SqliteHelper();
    //QSqlError err = sqlite->addConnectionRose();      // Rose에서 받는 DB 파일 수정 변경 불가
    QSqlError err = sqliteHelper->addConnectionLocal();

    // SELECT 처리
    int idx = 0;
    int sel = 0;

    QVariantList *list = new QVariantList();
    sqliteHelper->exec(QUERY_SEL, *list);
    if(err.type() == QSqlError::NoError){
        foreach(QVariant val, *list){
            QMap<QString, QVariant> map = val.toMap();
            idx = map["idx"].toInt();
            sel = map["lang"].toInt();
            break;
        }
    }

    global.notice = sel;
    sqliteHelper->close();
    delete sqliteHelper;
}


void HomeMain::changedIndexNotice(int index){

    int idx = 0;
    int sel = index;

    SqliteHelper *sqliteHelper = new SqliteHelper(this);
    //QSqlError err = sqlite->addConnectionRose();      // Rose에서 받는 DB 파일 수정 변경 불가
    QSqlError err = sqliteHelper->addConnectionLocal();

    QString strQuery_insert = "INSERT INTO notice (idx, lang) VALUES";
    strQuery_insert.append( QString("(%1, %2)").arg(idx).arg(sel));

    //qDebug() << strQuery_insert;

    // 항상 테이블 생성을 체크하고, 삭제처리 한다. for 초기화 (항상 레코드 최대 1개 유지)
    sqliteHelper->exec(QUERY_CREATE_TABLE);
    sqliteHelper->exec(QUERY_DEL);
    sqliteHelper->exec(strQuery_insert);

    // SELECT 처리
    QVariantList *list = new QVariantList();
    sqliteHelper->exec(QUERY_SEL, *list);
    if(err.type() == QSqlError::NoError){
        foreach(QVariant val, *list){
            QMap<QString, QVariant> map = val.toMap();
            idx = map["idx"].toInt();
            sel = map["lang"].toInt();
            global.notice = sel;
            qDebug() << "sel: " << sel;
            break;
        }
    }

    sqliteHelper->close();
    delete sqliteHelper;

    qDebug() << "select data -> idx : " << idx << ", sel : " << sel;
}


void HomeMain::ContentLoadingwaitingMsgShow(QString msg){    //c230409

    Q_UNUSED(msg);

    print_debug();
    if(global.abs_ani_dialog_wait->isHidden() != true){//c220616
        return;
    }

    print_debug();
    if(!global.window_activate_flag) return;//c220804

    if(global.powerDialogShowFlag) return;//c220804

    print_debug();

    //----------------------------------------------------

    int left = 0;
    int top = 0;

    left = global.left_mainwindow + ((global.width_mainwindow - 120) / 2);
    top = global.top_mainwindow + ((global.height_mainwindow - 120) / 2);

    print_debug();
    global.abs_ani_dialog_wait->move(left, top);//c220804

    print_debug();
    if(global.enable_section_left == true){
        global.enable_section_left = false;
    }

    print_debug();
    global.abs_ani_dialog_wait->show();//c230311
    global.abs_ani_dialog_wait->raise();//c230311
    print_debug();
}


void HomeMain::ContentLoadingwaitingMsgHide(){   //c230409

    print_debug();
    //qDebug() << "abs_ani_dialog_wait->isHidden() = " << global.abs_ani_dialog_wait->isHidden();
    if(global.abs_ani_dialog_wait->isHidden() != true){
        print_debug();
        //abs_ani_mov->stop();

        global.abs_ani_dialog_wait->hide(); //cheontidal

    }

    if(global.enable_section_left == false){
        global.enable_section_left = true;
    }

    //global.abs_ani_dialog_wait->show();//c230311
    //QTimer::singleShot(300, global.abs_ani_dialog_wait, SLOT(hide()));//c230311
    ToastMsg::delay(this,"", tr("delay"), 400);//c230323
}
